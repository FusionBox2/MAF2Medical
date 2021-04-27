/*=========================================================================

 Program: MAF2Medical
 Module: mafViewImageCompound
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafViewImageCompound.h"
#include "mafViewImage.h"
#include "mafViewVTK.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafPipeImage3D.h"
#include "mafGUIViewWin.h"
#include "mafGUI.h"
#include "mafGUILutSlider.h"
#include "mafGUILutSwatch.h"
#include "mafNode.h"
#include "mafNodeIterator.h"
#include "mafVMEImage.h"
#include "mafGUIFloatSlider.h"
#include "mafVMEOutputImage.h"
#include "vtkImageData.h"
#include "vtkTexture.h"

#include "vtkLookupTable.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkDataSet.h"

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

enum SUBVIEW_ID
{
  ID_VIEW_IMAGE = 0,
};

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewImageCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewImageCompound::mafViewImageCompound(const mafString& label, int num_row, int num_col)
: mafViewCompound(label,num_row,num_col)
//----------------------------------------------------------------------------
{
	m_LutWidget = NULL;
	m_LutSlider = NULL;
	m_ColorLUT = NULL;
}
//----------------------------------------------------------------------------
mafViewImageCompound::~mafViewImageCompound()
//----------------------------------------------------------------------------
{
	m_ColorLUT = NULL;
	cppDEL(m_LutWidget);
	cppDEL(m_LutSlider);
}
//----------------------------------------------------------------------------
mafView *mafViewImageCompound::Copy(mafBaseEventHandler *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewImageCompound *v = new mafViewImageCompound(GetLabel(), m_ViewRowNum, m_ViewColNum);
  v->SetListener(Listener);
  v->m_Id = m_Id;
  for (int i=0;i<m_PluggedChildViewList.size();i++)
  {
    v->m_PluggedChildViewList.push_back(m_PluggedChildViewList[i]->Copy(this));
  }
  v->m_NumOfPluggedChildren = m_NumOfPluggedChildren;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewImageCompound::CreateGuiView()
//----------------------------------------------------------------------------
{
	m_GuiView = new mafGUI(this);
  
  m_LutSlider = new mafGUILutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
  m_LutSlider->SetListener(this);
  m_LutSlider->SetSize(500,24);
  m_LutSlider->SetMinSize(wxSize(500,24));
  EnableWidgets(false);
  m_GuiView->Add(m_LutSlider);
  m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void mafViewImageCompound::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId()) 
  {
		case ID_RANGE_MODIFIED:
			{
				//Windowing
				if(mafVMEImage::SafeDownCast(GetSceneGraph()->GetSelectedVme()))
				{
					double low, hi;
					m_LutSlider->GetSubRange(&low,&hi);
					m_ColorLUT->SetTableRange(low,hi);
					mafEventMacro(mafEvent(this,CAMERA_UPDATE));
				}
			}
			break;
		case ID_LUT_CHOOSER:
			{
				double *sr;
				sr = m_ColorLUT->GetRange();
				m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
			}
			break;
    default:
      mafEventMacro(*maf_event);
  }
}
//-------------------------------------------------------------------------
mafGUI* mafViewImageCompound::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
	m_Gui->AddGui(((mafViewImage*)m_ChildViewList[ID_VIEW_IMAGE])->GetGui());
	m_LutWidget = m_Gui->Lut(ID_LUT_CHOOSER,_R("lut"),m_ColorLUT);
	m_LutWidget->Enable(false);
	m_Gui->Divider();
	m_Gui->FitGui();
	m_Gui->Update();
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafViewImageCompound::PackageView()
//-------------------------------------------------------------------------
{
	m_ViewImage = new mafViewImage(_R("View Image"),CAMERA_FRONT,false,false,false);
	m_ViewImage->PlugVisualPipe(_R("mafVMEVolumeGray"),_R("mafPipeBox"),NON_VISIBLE);
  m_ViewImage->PlugVisualPipe(_R("mafVMESurface"),_R("mafPipeSurface"),NON_VISIBLE);
	
	PlugChildView(m_ViewImage);
}
//----------------------------------------------------------------------------
void mafViewImageCompound::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
	for(int i=0; i<this->GetNumberOfSubView(); i++)
    m_ChildViewList[i]->VmeShow(node, show);

	if(GetSceneGraph()->GetSelectedVme()==node)
	{
    mafPipeImage3D *pipe = (mafPipeImage3D *)m_ChildViewList[ID_VIEW_IMAGE]->GetNodePipe(node);
		UpdateWindowing(show && pipe && pipe->IsGrayImage(),node);
	}

	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafViewImageCompound::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
	//if a volume is visualized enable the widgets
	if(m_Gui)
		m_Gui->Enable(ID_LUT_CHOOSER,enable);
  m_LutSlider->Enable(enable);
}
//----------------------------------------------------------------------------
void mafViewImageCompound::VmeSelect(mafNode *node, bool select)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->VmeSelect(node, select);

  mafPipeImage3D *pipe = (mafPipeImage3D *)m_ChildViewList[ID_VIEW_IMAGE]->GetNodePipe(node);
	UpdateWindowing(node->IsA("mafVMEImage") && select && pipe && pipe->IsGrayImage(),node);
}
//----------------------------------------------------------------------------
void mafViewImageCompound::UpdateWindowing(bool enable,mafNode *node)
//----------------------------------------------------------------------------
{
	if(enable)
	{
		EnableWidgets(enable);
		mafVMEImage *image=mafVMEImage::SafeDownCast(node);
		double sr[2];
		image->GetOutput()->GetVTKData()->GetScalarRange(sr);

    m_ColorLUT = ((mafPipeImage3D *)m_ChildViewList[ID_VIEW_IMAGE]->GetNodePipe(node))->GetLUT();;

    m_ColorLUT->SetRange(sr);
    m_ColorLUT->Build();
      
    m_LutWidget->SetLut(m_ColorLUT);
		m_LutWidget->Enable(true);
		m_LutSlider->SetRange((long)sr[0],(long)sr[1]);
		m_LutSlider->SetSubRange((long)sr[0],(long)sr[1]);
	}
	else
	{
		EnableWidgets(enable);
		m_LutSlider->SetRange(-100,100);
		m_LutSlider->SetSubRange(-100,100);
	}
}