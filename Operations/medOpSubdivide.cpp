/*=========================================================================

 Program: MAF2Medical
 Module: medOpSubdivide
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpSubdivide.h"
#include "medDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafVMESurface.h"

#include "vtkLinearSubdivisionFilter.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkTriangleFilter.h"
#include "vtkCell.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpSubdivide);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpSubdivide::medOpSubdivide(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_OP;
	m_Canundo	= true;

	m_InputPreserving = false;

  m_OriginalPolydata = NULL;
  m_ResultPolydata = NULL;
}
//----------------------------------------------------------------------------
medOpSubdivide::~medOpSubdivide()
//----------------------------------------------------------------------------
{
  vtkDEL(m_OriginalPolydata);
  vtkDEL(m_ResultPolydata);
}
//----------------------------------------------------------------------------
bool medOpSubdivide::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return ( node != NULL && node->IsA("mafVMESurface") );
}
//----------------------------------------------------------------------------
mafOp *medOpSubdivide::Copy()   
//----------------------------------------------------------------------------
{
  return (new medOpSubdivide(GetLabel()));
}
//----------------------------------------------------------------------------
// Constants:
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void medOpSubdivide::OpRun()   
//----------------------------------------------------------------------------
{
  mafVMESurface *surface = mafVMESurface::SafeDownCast(m_Input);
  surface->Update();

  vtkPolyData *polydataInput = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polydataInput->Update();

  vtkNEW(m_OriginalPolydata);
  m_OriginalPolydata->DeepCopy(polydataInput);
  m_OriginalPolydata->Update();

  vtkNEW(m_ResultPolydata);
  m_ResultPolydata->DeepCopy(polydataInput);
  m_ResultPolydata->Update();

  for(int i=0;i<polydataInput->GetNumberOfCells();i++)
  {
    if(polydataInput->GetCell(i)->GetNumberOfPoints()!=3)
    {
      if(m_TestMode == false)
        wxMessageBox(_("The mesh will be triangolarized to be subdivide"));

      vtkTriangleFilter *triangleFilter;
      vtkNEW(triangleFilter);

      triangleFilter->SetInput(polydataInput);
      triangleFilter->Update();

      surface->SetData(triangleFilter->GetOutput(),surface->GetTimeStamp());
      surface->Update();

      m_ResultPolydata->DeepCopy(triangleFilter->GetOutput());
      m_ResultPolydata->Update();

      vtkDEL(triangleFilter);

      break;
    }
  }

  if(m_TestMode == false)
    CreateGui();
}
//----------------------------------------------------------------------------
void medOpSubdivide::CreateGui()
//----------------------------------------------------------------------------
{
  // interface:
  m_Gui = new mafGUI(this);

  // ToDO: add your custom widgets...

  m_NumberOfSubdivision = 1;
  m_Gui->Label(_L("num. subdivision"));
  m_Gui->Integer(ID_NUMBER_OF_SUBDIVISION, _R(""),&m_NumberOfSubdivision,0);

  m_Gui->Button(ID_SUBDIVIDE,_L("subdivide"));
  m_Gui->Button(ID_PREVIEW,_L("preview"));

  m_Gui->OkCancel();
  m_Gui->Divider();

  ShowGui();
}
//----------------------------------------------------------------------------
void medOpSubdivide::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {	
      case ID_PREVIEW:
        {
          mafEventMacro(mafEvent(this,CAMERA_UPDATE));
        }
      break;
      case ID_NUMBER_OF_SUBDIVISION:
        {
          if (m_NumberOfSubdivision >5)
          {
            if(m_TestMode == false)
              wxMessageBox("Warning! the number of subdivision is hight, it could require too many memory!");
          }
        }
        break;
      case ID_SUBDIVIDE:
        {
          Algorithm();
        }
        break;
      case wxOK:
        OpStop(OP_RUN_OK);        
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);        
      break;
    }
  }
}
//----------------------------------------------------------------------------
void medOpSubdivide::OpStop(int result)
//----------------------------------------------------------------------------
{
  if (result == OP_RUN_CANCEL)
  {
    mafVMESurface *surface = mafVMESurface::SafeDownCast(m_Input);
    surface->Update();
    
    surface->SetData(m_OriginalPolydata,surface->GetTimeStamp());
    surface->Update();
  }

  HideGui();
  mafEventMacro(mafEvent(this,result));  
}
//----------------------------------------------------------------------------
void medOpSubdivide::OpUndo()
//----------------------------------------------------------------------------
{
  mafVMESurface *surface = mafVMESurface::SafeDownCast(m_Input);
  surface->Update();

  surface->SetData(m_OriginalPolydata,surface->GetTimeStamp());
  surface->Update();
}
//----------------------------------------------------------------------------
void medOpSubdivide::OpDo()
//----------------------------------------------------------------------------
{
  mafVMESurface *surface = mafVMESurface::SafeDownCast(m_Input);
  surface->Update();

  surface->SetData(m_ResultPolydata,surface->GetTimeStamp());
  surface->Update();
}
//----------------------------------------------------------------------------
void medOpSubdivide::Algorithm()
//----------------------------------------------------------------------------
{
  vtkLinearSubdivisionFilter *subdivisionFilter;
  vtkNEW(subdivisionFilter);

  mafVMESurface *surface = mafVMESurface::SafeDownCast(m_Input);
  surface->Update();

  vtkPolyData *polydataInput = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polydataInput->Update();

  subdivisionFilter->SetInput(polydataInput);
  subdivisionFilter->SetNumberOfSubdivisions(m_NumberOfSubdivision);
  subdivisionFilter->Update();

  m_ResultPolydata->DeepCopy(subdivisionFilter->GetOutput());
  m_ResultPolydata->Update();

  surface->SetData(m_ResultPolydata,surface->GetTimeStamp());
  surface->Update();

  vtkDEL(subdivisionFilter);
}
