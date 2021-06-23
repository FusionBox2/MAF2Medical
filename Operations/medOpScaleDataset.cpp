/*=========================================================================

 Program: MAF2Medical
 Module: medOpScaleDataset
 Authors: Daniele Giunchi , Stefano Perticoni
 
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


#include "medOpScaleDataset.h"

#include <wx/busyinfo.h>

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafGizmoScale.h"
#include "mafGUITransformMouse.h"
#include "mafGUISaveRestorePose.h"
#include "mafGUITransformTextEntries.h"

#include "mafInteractorGenericMouse.h"

#include "mafSmartPointer.h"
#include "mafTransform.h"
#include "mafMatrix.h"
#include "mafVME.h"
#include "mafVMEOutput.h"

#include "vtkTransform.h"
#include "vtkDataSet.h"

//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
enum MAF_TRANSFORM_ID
{
  ID_SHOW_GIZMO = MINID,
  ID_RESET,
  ID_ENABLE_SCALING,
  ID_AUX_REF_SYS,
  ID_HELP,
};

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpScaleDataset);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpScaleDataset::medOpScaleDataset(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_OP;
  m_Canundo = true;

  m_GizmoScale              = NULL;
  m_GuiSaveRestorePose      = NULL;
}
//----------------------------------------------------------------------------
medOpScaleDataset::~medOpScaleDataset()
//----------------------------------------------------------------------------
{
  cppDEL(m_GizmoScale);
  cppDEL(m_GuiSaveRestorePose);
}
//----------------------------------------------------------------------------
bool medOpScaleDataset::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
	return (vme!=NULL && vme->IsMAFType(mafVME) && !vme->IsA("mafVMERoot") && !vme->IsA("mafVMEExternalData"));
}
//----------------------------------------------------------------------------
mafOp* medOpScaleDataset::Copy()   
//----------------------------------------------------------------------------
{
  return new medOpScaleDataset(GetLabel());
}

//----------------------------------------------------------------------------
void medOpScaleDataset::OpRun()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait("creating gui...");
  }

  assert(m_Input);
  m_CurrentTime = ((mafVME *)m_Input)->GetTimeStamp();

  m_NewAbsMatrix = *((mafVME *)m_Input)->GetOutput()->GetAbsMatrix();
  m_OldAbsMatrix = *((mafVME *)m_Input)->GetOutput()->GetAbsMatrix();

  if (!m_TestMode)
  {
    CreateGui();
    ShowGui();
  }
}
//----------------------------------------------------------------------------
void medOpScaleDataset::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (maf_event->GetSender() == this->m_Gui) // from this operation gui
  {
    OnEventThis(maf_event); 
    return;
  }
  else if (maf_event->GetSender() == m_GizmoScale) // from scaling gizmo
  {
    OnEventGizmoScale(maf_event);
  }
  else if (maf_event->GetSender() == this->m_GuiSaveRestorePose) // from save/restore gui
  {
    OnEventGuiSaveRestorePose(maf_event); 
		mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
  else
  {
    mafEventMacro(*maf_event); 
  }	
}
	  
//----------------------------------------------------------------------------
void medOpScaleDataset::OpDo()
//----------------------------------------------------------------------------
{
	;// mafOpTransformInterface::OpDo();
}
//----------------------------------------------------------------------------
void medOpScaleDataset::OpUndo()
//----------------------------------------------------------------------------
{  
	((mafVME *)m_Input)->SetAbsMatrix(m_OldAbsMatrix);
  mafEventMacro(mafEvent(this,CAMERA_UPDATE)); 
}
//----------------------------------------------------------------------------
void medOpScaleDataset::OpStop(int result)
//----------------------------------------------------------------------------
{  
  wxBusyInfo wait("destroying gui...");

  m_GizmoScale->Show(false);
  cppDEL(m_GizmoScale);

  // HideGui seems not to work  with plugged guis :(; using it generate a SetFocusToChild
  // error when operation tab is selected after the operation has ended
  mafEventMacro(mafEvent(this,OP_HIDE_GUI,(wxWindow *)m_Gui->GetParent()));
  mafEventMacro(mafEvent(this,result));  
}

//----------------------------------------------------------------------------
void medOpScaleDataset::OnEventThis(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
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

    case ID_RESET:
  	{ 
      Reset();
	  }
    break;
 
    case wxOK:
    {
			this->OpStop(OP_RUN_OK);
      return;
    }
    break;

		case wxCANCEL:
    {
			Reset();
			this->OpStop(OP_RUN_CANCEL);
      return;
    }
    break;

	case ID_AUX_REF_SYS:
	{
		mafString s = _R("Choose VME ref sys");
		mafEvent e(this,VME_CHOOSE, &s);
		mafEventMacro(e);
		SetRefSysVME(mafVME::SafeDownCast(e.GetVme()));
	}
	break;

    default:
    {
      mafEventMacro(*maf_event);
    }
    break;
  }
}
//----------------------------------------------------------------------------
void medOpScaleDataset::OnEventGizmoScale(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{ 
  switch(maf_event->GetId())
	{
    case ID_TRANSFORM:
  	{ 
      m_NewAbsMatrix = *((mafVME *)m_Input)->GetOutput()->GetAbsMatrix();
      mafEventMacro(mafEvent(this, CAMERA_UPDATE));
	  }
    break;

    default:
    {
      mafEventMacro(*maf_event);
    }
  }
}


//----------------------------------------------------------------------------
void medOpScaleDataset::OnEventGuiSaveRestorePose(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
	{
    case ID_TRANSFORM: // from m_GuiSaveRestorePose
    {   
      m_NewAbsMatrix = *(m_RefSysVME->GetOutput()->GetAbsMatrix());
    }
    break;
    default:
    {
      mafEventMacro(*maf_event);
    }
  }
}

//----------------------------------------------------------------------------
void medOpScaleDataset::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);

  mafEvent buildHelpGui;
  buildHelpGui.SetSender(this);
  buildHelpGui.SetId(GET_BUILD_HELP_GUI);
  mafEventMacro(buildHelpGui);

  if (buildHelpGui.GetArg() == true)
  {
	  m_Gui->Button(ID_HELP, _R("Help"), _R(""));
  }

  m_Gui->Divider(2);
  m_Gui->Label(_R("gizmo interaction"), true);
  m_Gui->Label(_R("left mouse: interact through gizmo"));

  //---------------------------------
  // Scaling Gizmo Gui
  //---------------------------------
  m_GizmoScale = new mafGizmoScale(mafVME::SafeDownCast(m_Input), this);
  m_GizmoScale->Show(false);

  // add scaling gizmo gui to operation
  m_Gui->AddGui(m_GizmoScale->GetGui());

  //---------------------------------
  // Store/Restore position Gui
  //---------------------------------
  m_GuiSaveRestorePose = new mafGUISaveRestorePose(mafVME::SafeDownCast(m_Input), this);
  
  // add Gui to operation
  m_Gui->AddGui(m_GuiSaveRestorePose->GetGui());

  //--------------------------------- 
  m_Gui->Divider(2);
  m_Gui->Label(_R("auxiliary ref sys"), true);
  m_Gui->Button(ID_AUX_REF_SYS,_R("choose"));

  if(this->m_RefSysVME == NULL)
  {
    SetRefSysVME(mafVME::SafeDownCast(m_Input));
    m_RefSysVMEName = m_Input->GetName();
  }
  m_Gui->Label(_R("refsys name: "),&m_RefSysVMEName);

  m_Gui->Divider(2);
  m_Gui->Button(ID_RESET,_R("reset"), _R(""),_R("Cancel the transformation."));

	m_Gui->OkCancel(); 
  m_Gui->Label(_R(""));
  //--------------------------------- 

  m_Gui->Update();

	m_GizmoScale->SetRefSys(m_RefSysVME);
	m_GizmoScale->Show(true);


	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void medOpScaleDataset::Reset()
//----------------------------------------------------------------------------
{
  ((mafVME *)m_Input)->SetAbsMatrix(m_OldAbsMatrix);  
  SetRefSysVME(mafVME::SafeDownCast(m_Input)); 
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void medOpScaleDataset::RefSysVmeChanged()
//----------------------------------------------------------------------------
{
  // plugged components set their refsys;
  /*
  this should cycle on all plugged components => improve in order to use base class
  SetRefSys on mafGUITransformInterface pointer
  */
  
  if (!m_TestMode)
  {
	  // change gscale refsys
	  m_GizmoScale->SetRefSys(m_RefSysVME);  
  }

  m_RefSysVMEName = m_RefSysVME->GetName();
  if (!m_TestMode)
  {
	  assert(m_Gui);
	  m_Gui->Update();
  }
}
