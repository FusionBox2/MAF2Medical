/*=========================================================================

 Program: MAF2Medical
 Module: medOpSurfaceMirror
 Authors: Paolo Quadrani - porting  Daniele Giunchi
 
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

#include "medOpSurfaceMirror.h"
#include <wx/busyinfo.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"


#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEGroup.h"
#include "vtkPolyData.h"
#include "vtkMEDPolyDataMirror.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpSurfaceMirror);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpSurfaceMirror::medOpSurfaceMirror(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType			 		= OPTYPE_OP;
	m_Canundo			 		= true;
	m_InputPreserving = false; //Natural_preserving
	
	m_OutputPolydata = NULL;
	m_InputPolydata		= NULL;
  m_MirrorFilter     = NULL;

  m_MirrorX      = 1;
  m_MirrorY      = 0;
  m_MirrorZ      = 0;
  m_FlipNormals = 0;
}
//----------------------------------------------------------------------------
medOpSurfaceMirror::~medOpSurfaceMirror( ) 
//----------------------------------------------------------------------------
{
	vtkDEL(m_InputPolydata);
	vtkDEL(m_OutputPolydata);
	vtkDEL(m_MirrorFilter);
}
//----------------------------------------------------------------------------
mafOp* medOpSurfaceMirror::Copy()   
//----------------------------------------------------------------------------
{
  medOpSurfaceMirror *cp = new medOpSurfaceMirror(GetLabel());
  cp->m_Canundo		= m_Canundo;
  cp->m_OpType		= m_OpType;
  cp->SetListener(GetListener());
  cp->m_Next			= NULL;
  return cp;
}
//----------------------------------------------------------------------------
bool medOpSurfaceMirror::Accept(mafNode* node)   
//----------------------------------------------------------------------------
{
	return  (node && (node->IsMAFType(mafVMESurface) || node->IsMAFType(mafVMEGroup)
							/*
							|| 
							( 
								 vme->IsA("mflVMELandmarkCloud") 
								 && 
								 !((mflVMELandmarkCloud*)vme)->IsOpen()
							) 
							*/
					)
					//&& node->GetNumberOfItems()==1
					//&& node->GetItem(0)
					//&& node->GetItem(0)->GetData() 	
					);
};   
//----------------------------------------------------------------------------
enum SURFACE_MIRROR_ID
//----------------------------------------------------------------------------
{
	ID_MIRRORX = MINID,
	ID_MIRRORY,
	ID_MIRRORZ,
	ID_FLIPNORMALS,
	ID_HELP,
};
//----------------------------------------------------------------------------
void medOpSurfaceMirror::OpRun()   
//----------------------------------------------------------------------------
{  
	if(!m_TestMode)
	{
		// interface:
		m_Gui = new mafGUI(this);
		m_Gui->SetListener(this);
		
		mafEvent buildHelpGui;
		buildHelpGui.SetSender(this);
		buildHelpGui.SetId(GET_BUILD_HELP_GUI);
		mafEventMacro(buildHelpGui);

		if (buildHelpGui.GetArg() == true)
		{
			m_Gui->Button(ID_HELP, _R("Help"), _R(""));
		}

		m_Gui->Label(_R("this doesn't work on animated vme"));
		m_Gui->Label(_R(""));
		
		m_Gui->Bool(ID_MIRRORX,_R("mirror x coords"), &m_MirrorX, 1);
		m_Gui->Bool(ID_MIRRORY,_R("mirror y coords"), &m_MirrorY, 1);
		m_Gui->Bool(ID_MIRRORZ,_R("mirror z coords"), &m_MirrorZ, 1);
		//m_Gui->Bool(ID_FLIPNORMALS,"flip normals",&m_FlipNormals,1);
		m_Gui->Label(_R(""));
		m_Gui->OkCancel();

		ShowGui();
	}


	if (m_Input->IsMAFType(mafVMESurface))
	{
		vtkNEW(m_InputPolydata);
		m_InputPolydata->DeepCopy((vtkPolyData*)((mafVMESurface *)m_Input)->GetOutput()->GetVTKData());

		vtkNEW(m_OutputPolydata);
		m_OutputPolydata->DeepCopy((vtkPolyData*)((mafVMESurface *)m_Input)->GetOutput()->GetVTKData());




		m_MirrorFilter = vtkMEDPolyDataMirror::New();
		m_MirrorFilter->SetInput(m_InputPolydata);

		Preview();
	}

	if (m_Input->IsMAFType(mafVMEGroup))

	{
		
		m_MirrorFilter = vtkMEDPolyDataMirror::New();
		PreviewGroup();
	}
}
//----------------------------------------------------------------------------
void medOpSurfaceMirror::OpDo()
//----------------------------------------------------------------------------
{
	wxBusyInfo wait("operation do ..");
	Sleep(2500);
 
	if (m_Input->IsMAFType(mafVMESurface))
	{
		assert(m_OutputPolydata);

		((mafVMESurface *)m_Input)->SetData(m_OutputPolydata, ((mafVME *)m_Input)->GetTimeStamp());

	}
	if (m_Input->IsMAFType(mafVMEGroup))
	{
		int nbr = ((mafVMEGroup*)m_Input)->GetNumberOfChildren();
		std::string str = "nbr children" + std::to_string(nbr);
		wxString mafs = str.c_str();
		wxBusyInfo wait(mafs);
		Sleep(2500);


		for (int i = 0; i < nbr; i++)

		{
			if (((mafVMEGroup*)m_Input)->GetChild(i)->IsMAFType(mafVMESurface))
			{
				mafString synthetic_name = _R("Copied ");
				mafAutoPointer<mafNode> node = (((mafVMEGroup*)m_Input)->GetChild(i))->MakeCopy();
				synthetic_name.Append(((mafVMEGroup*)m_Input)->GetChild(i)->GetName());
				node->SetName(synthetic_name);
				
				((mafVMEGroup*)m_Input)->AddChild(node);


				vtkNEW(m_InputPolydata);
				vtkNEW(m_OutputPolydata);
				m_InputPolydata->DeepCopy((vtkPolyData*)((mafVMESurface*)((mafVMEGroup*)m_Input)->GetChild(i))->GetOutput()->GetVTKData());
				m_OutputPolydata->DeepCopy((vtkPolyData*)((mafVMESurface*)((mafVMEGroup*)m_Input)->GetChild(i))->GetOutput()->GetVTKData());


				//m_OutputGroup->AddChild(((mafVMEGroup*)m_Input)->GetChild(i));


				m_MirrorFilter->SetInput(m_InputPolydata);
				m_MirrorFilter->Update();


				std::string str = "num child" + std::to_string(i) + " " + std::to_string(m_MirrorX) + " " + std::to_string(m_MirrorY) + " " + std::to_string(m_MirrorZ);
				wxString mafs = str.c_str();
				wxBusyInfo wait3(mafs);
				Sleep(1500);



				m_OutputPolydata->DeepCopy(m_MirrorFilter->GetOutput());
				m_OutputPolydata->Update();
				
				wxBusyInfo wait4("mirror output ok");
				Sleep(2500);

				((mafVMESurface*)((mafVMEGroup*)m_Input)->GetChild(i))->SetData(m_OutputPolydata, ((mafVME*)((mafVMEGroup*)m_Input)->GetChild(i))->GetTimeStamp());
				assert(m_OutputPolydata);
				((mafVMESurface*)((mafVMEGroup*)m_Input)->GetChild(i))->SetData(m_OutputPolydata, ((mafVME*)((mafVMEGroup*)m_Input)->GetChild(i))->GetTimeStamp());


				
				vtkDEL(m_OutputPolydata);
				vtkDEL(m_InputPolydata);
				m_MirrorFilter->RemoveAllInputs();

			}
			if (((mafVMEGroup*)m_Input)->GetChild(i)->IsMAFType(mafVMELandmarkCloud))
			{
				mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(((mafVMEGroup*)m_Input)->GetChild(i));

				if (cloud != NULL)
				{

					mafString synthetic_name = _R("Copied ");
					mafAutoPointer<mafNode> node = (((mafVMEGroup*)m_Input)->GetChild(i))->MakeCopy();
					synthetic_name.Append(((mafVMEGroup*)m_Input)->GetChild(i)->GetName());
					node->SetName(synthetic_name);

					((mafVMEGroup*)m_Input)->AddChild(node);
					std::vector<mafTimeStamp> stamps;
					cloud->GetLocalTimeStamps(stamps);
					for (unsigned i = 0; i < stamps.size(); i++)
					{
						for (unsigned j = 0; j < cloud->GetNumberOfLandmarks(); j++)
						{
							double xyz[3];
							cloud->GetLandmark(j, xyz, stamps[i]);
							if (m_MirrorX)
								xyz[0] = -xyz[0];
							if (m_MirrorY)
								xyz[1] = -xyz[1];
							if (m_MirrorZ)
								xyz[2] = -xyz[2];
							cloud->SetLandmark(j, xyz[0], xyz[1], xyz[2], stamps[i]);
						}
					}
				}

			}

		}
	}

	
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpSurfaceMirror::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_InputPolydata);

	((mafVMESurface *)m_Input)->SetData(m_InputPolydata,((mafVME *)m_Input)->GetTimeStamp());
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpSurfaceMirror::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
	  switch(e->GetId())
	  {
			case ID_HELP:
			{
				mafEvent helpEvent;
				helpEvent.SetSender(this);
				mafString operationLabel = GetLabel();
				helpEvent.SetString(&operationLabel);
				helpEvent.SetId(OPEN_HELP_PAGE);
				mafEventMacro(helpEvent);
			}
			break;

			case ID_MIRRORX:
			case ID_MIRRORY:
			case ID_MIRRORZ:
			case ID_FLIPNORMALS:
				if (m_Input->IsMAFType(mafVMESurface))
				{
					Preview();
				}
				if (m_Input->IsMAFType(mafVMEGroup))
				{
					PreviewGroup();
				}
			break;
			case wxOK:
				OpStop(OP_RUN_OK);        
			break;
			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);        
			break;
				default:
				mafEventMacro(*e);
			break; 
	  }
	}  
}
//----------------------------------------------------------------------------
void medOpSurfaceMirror::OpStop(int result)
//----------------------------------------------------------------------------
{
  if(result == OP_RUN_CANCEL) OpUndo();

	if(!m_TestMode)
	{
	  HideGui();
	  delete m_Gui;
	}
	mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void medOpSurfaceMirror::Preview()  
//----------------------------------------------------------------------------
{
	wxBusyCursor *wait=NULL;
	if(!m_TestMode)
	{
		wait=new wxBusyCursor();
	}

  m_MirrorFilter->SetMirrorXCoordinate(m_MirrorX);
  m_MirrorFilter->SetMirrorYCoordinate(m_MirrorY);
  m_MirrorFilter->SetMirrorZCoordinate(m_MirrorZ);
  //m_MirrorFilter->SetFlipNormals(m_FlipNormals);
  m_MirrorFilter->Update();
  

  m_OutputPolydata->DeepCopy(m_MirrorFilter->GetOutput());
  m_OutputPolydata->Update();
  ((mafVMESurface *)m_Input)->SetData(m_OutputPolydata,((mafVME *)m_Input)->GetTimeStamp());


  mafEventMacro(mafEvent(this, CAMERA_UPDATE));

	if (wait)
		delete wait;
}

void medOpSurfaceMirror::PreviewGroup()
//----------------------------------------------------------------------------
{
	wxBusyCursor *wait = NULL;
	if (!m_TestMode)
	{
		wait = new wxBusyCursor();
	}

	m_MirrorFilter->SetMirrorXCoordinate(m_MirrorX);
	m_MirrorFilter->SetMirrorYCoordinate(m_MirrorY);
	m_MirrorFilter->SetMirrorZCoordinate(m_MirrorZ);
	//m_MirrorFilter->SetFlipNormals(m_FlipNormals);
	m_MirrorFilter->Update();
	
	mafEventMacro(mafEvent(this, CAMERA_UPDATE));

	if (wait)
		delete wait;
}
