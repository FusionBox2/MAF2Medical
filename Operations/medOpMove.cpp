/*=========================================================================

 Program: MAF2Medical
 Module: medOpMove
 Authors: Stefano Perticoni
 
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


#include "medOpMove.h"

#include <wx/busyinfo.h>

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafGizmoTranslate.h"
#include "mafGizmoRotate.h"
#include "mafGizmoScale.h"
#include "mafGUITransformMouse.h"
#include "mafGUISaveRestorePose.h"
#include "mafGUITransformTextEntries.h"
#include "mafVMELandmark.h"

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
  ID_CHOOSE_GIZMO_COMBO,
  ID_ROTATION_STEP,
  ID_TRANSLATION_STEP,
  ID_ENABLE_STEP,
  ID_ROT_SNAP,
  ID_RESET,
  ID_AUX_REF_SYS,
  ID_ENABLE_SCALING,
  ID_ROLLOUT_TEXT_ENTRIES,
  ID_ROLLOUT_GIZMO_TRANSLATE,
  ID_ROLLOUT_GIZMO_ROTATE,
  ID_ROLLOUT_GIZMO_SCALE,
  ID_ROLLOUT_SAVE_POS,
  ID_TRANSLATE_X,
  ID_TRANSLATE_Y,
  ID_TRANSLATE_Z,
  ID_ROTATE_X,
  ID_ROTATE_Y,
  ID_ROTATE_Z,
  ID_SCALE_X,
  ID_SCALE_Y,
  ID_SCALE_Z,
  ID_HELP,
};

namespace
{
  void DecomposeMatrix(const mafMatrix& delta, double *translation, double *rotation, double *scaling)
  {
    mafTransform::GetPosition(delta, translation);
    mafTransform::GetOrientation(delta, rotation);
    mafTransform::GetScale(delta, scaling);
  }
}


//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpMove);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpMove::medOpMove(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_EnableScaling = FALSE;
  m_OpType = OPTYPE_OP;
  m_Canundo = true;

  m_RotationStep    = 10.0;
  m_TranslationStep = 2.0;
  m_EnableStep      = 0;

  m_GizmoTranslate          = NULL;
  m_GizmoRotate             = NULL;
  m_GuiTransformMouse            = NULL;
  m_GuiSaveRestorePose      = NULL;
  m_GuiTransformTextEntries = NULL;
  m_TransfTranslation[0] = m_TransfTranslation[1] = m_TransfTranslation[2] = 0;
  m_TransfRotation[0] = m_TransfRotation[1] = m_TransfRotation[2] = 0;
  m_TransfScaling[0] = m_TransfScaling[1] = m_TransfScaling[2] = 1;
}
//----------------------------------------------------------------------------
medOpMove::~medOpMove()
//----------------------------------------------------------------------------
{
  cppDEL(m_GizmoTranslate);
  cppDEL(m_GizmoRotate);
  cppDEL(m_GuiTransformMouse);
  cppDEL(m_GuiSaveRestorePose);
  cppDEL(m_GuiTransformTextEntries);
}
//----------------------------------------------------------------------------
bool medOpMove::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
	/*mafEvent e(this,VIEW_SELECTED);
	mafEventMacro(e);*/
	return (vme!=NULL && vme->IsMAFType(mafVME) && !vme->IsA("mafVMERoot") 
    && !vme->IsA("mafVMEExternalData") && !vme->IsA("mafVMERefSysAbstract") /*&& e.GetBool()*/);
}
//----------------------------------------------------------------------------
mafOp* medOpMove::Copy()   
//----------------------------------------------------------------------------
{
  return new medOpMove(GetLabel());
}

//----------------------------------------------------------------------------
void medOpMove::OpRun()
//----------------------------------------------------------------------------
{
  // progress bar stuff
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
void medOpMove::GetDelta(mafMatrix& delta)
{
  mafMatrix newMatr;
  mafMatrix oldMatr;
  mafMatrix convMatrix;
  newMatr = m_NewAbsMatrix;
  oldMatr = m_OldAbsMatrix;
  oldMatr.Invert();
  mafMatrix::Multiply4x4(newMatr, oldMatr, delta);
}

//----------------------------------------------------------------------------
void medOpMove::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // perform different actions depending on sender
  // process events: 
  if (maf_event->GetSender() == this->m_Gui) // from this operation gui
  {
    OnEventThis(maf_event); 
    return;
  }
  else if (maf_event->GetSender() == m_GuiTransformMouse) // from gui transform
  {
    OnEventGuiTransformMouse(maf_event);
    mafMatrix delta;
    GetDelta(delta);
    DecomposeMatrix(delta, m_TransfTranslation, m_TransfRotation, m_TransfScaling);
    m_Gui->Update();
  }
  else if (maf_event->GetSender() == m_GizmoTranslate) // from translation gizmo
  {
    OnEventGizmoTranslate(maf_event);
    mafMatrix delta;
    GetDelta(delta);
    DecomposeMatrix(delta, m_TransfTranslation, m_TransfRotation, m_TransfScaling);
    m_Gui->Update();
  }
  else if (maf_event->GetSender() == m_GizmoRotate) // from rotation gizmo
  {
    OnEventGizmoRotate(maf_event);
    mafMatrix delta;
    GetDelta(delta);
    DecomposeMatrix(delta, m_TransfTranslation, m_TransfRotation, m_TransfScaling);
    m_Gui->Update();
  }
  else if (maf_event->GetSender() == this->m_GuiSaveRestorePose) // from save/restore gui
  {
    OnEventGuiSaveRestorePose(maf_event); 
		mafEventMacro(mafEvent(this,CAMERA_UPDATE));
    mafMatrix delta;
    GetDelta(delta);
    DecomposeMatrix(delta, m_TransfTranslation, m_TransfRotation, m_TransfScaling);
    m_Gui->Update();
  }
  else if (maf_event->GetSender() == this->m_GuiTransformTextEntries)
  {
    OnEventGuiTransformTextEntries(maf_event);
		mafEventMacro(mafEvent(this,CAMERA_UPDATE));
    mafMatrix delta;
    GetDelta(delta);
    DecomposeMatrix(delta, m_TransfTranslation, m_TransfRotation, m_TransfScaling);
    m_Gui->Update();
  }
  else
  {
    // if no one can handle this event send it to the operation listener
    mafEventMacro(*maf_event); 
  }

  if(mafVMELandmark *landmark = mafVMELandmark::SafeDownCast(m_Input)) 
  {
    landmark->Modified();
    landmark->Update();
  }
}
	  
//----------------------------------------------------------------------------
void medOpMove::OpDo()
//----------------------------------------------------------------------------
{
  mafOpTransformInterface::OpDo();
}
//----------------------------------------------------------------------------
void medOpMove::OpUndo()
//----------------------------------------------------------------------------
{  
	((mafVME *)m_Input)->SetAbsMatrix(m_OldAbsMatrix);
  mafEventMacro(mafEvent(this,CAMERA_UPDATE)); 
}
//----------------------------------------------------------------------------
void medOpMove::OpStop(int result)
//----------------------------------------------------------------------------
{  
  if (!m_TestMode)
  {
    // progress bar stuff
    wxBusyInfo wait("destroying gui...");
  
    m_GizmoTranslate->Show(false);
    cppDEL(m_GizmoTranslate);

    m_GizmoRotate->Show(false);
    cppDEL(m_GizmoRotate);

    m_GuiTransformMouse->DetachInteractorFromVme();

	  mafEventMacro(mafEvent(this,CAMERA_UPDATE)); 

    // HideGui seems not to work  with plugged guis :(; using it generate a SetFocusToChild
    // error when operation tab is selected after the operation has ended
    mafEventMacro(mafEvent(this,OP_HIDE_GUI,(wxWindow *)m_Gui->GetParent()));
  }
  mafEventMacro(mafEvent(this,result));  
}

//----------------------------------------------------------------------------
void medOpMove::OnEventThis(mafEventBase *maf_event)
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
    
    case ID_TRANSLATE_X:
    case ID_TRANSLATE_Y:
    case ID_TRANSLATE_Z:
    case ID_ROTATE_X:
    case ID_ROTATE_Y:
    case ID_ROTATE_Z:
    case ID_SCALE_X:
    case ID_SCALE_Y:
    case ID_SCALE_Z:
    {
      mafSmartPointer<mafTransform> tran;
      tran->Scale(m_TransfScaling[0], m_TransfScaling[1], m_TransfScaling[2],POST_MULTIPLY);
      tran->RotateY(m_TransfRotation[1], POST_MULTIPLY);
      tran->RotateX(m_TransfRotation[0], POST_MULTIPLY);
      tran->RotateZ(m_TransfRotation[2], POST_MULTIPLY);
      tran->SetPosition(m_TransfTranslation);
      mafMatrix::Multiply4x4(tran->GetMatrix(), m_OldAbsMatrix, m_NewAbsMatrix);
      m_NewAbsMatrix.SetTimeStamp(m_CurrentTime);
      ((mafVME*)m_Input)->SetAbsMatrix(m_NewAbsMatrix, m_CurrentTime);

      // update gizmos positions
      if (m_GizmoTranslate) m_GizmoTranslate->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
      if (m_GizmoRotate) m_GizmoRotate->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
      if (m_GuiTransformMouse) m_GuiTransformMouse->SetRefSys(m_RefSysVME);

      // update gui 
      m_GuiTransformTextEntries->SetAbsPose(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());

      m_Gui->Update();
      mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
    }

	case ID_SHOW_GIZMO:
    {
      // update gizmo choose gui
      m_Gui->Enable(ID_CHOOSE_GIZMO_COMBO, m_UseGizmo ? true : false);
      m_Gui->Enable(ID_ROTATION_STEP, m_UseGizmo?true:false);
      m_Gui->Enable(ID_TRANSLATION_STEP, m_UseGizmo?true:false);
      m_Gui->Enable(ID_ENABLE_STEP, m_UseGizmo?true:false);
      
      if (m_UseGizmo == 0)
      {
        m_GizmoRotate->Show(false);
        m_GizmoTranslate->Show(false);
        m_GuiTransformMouse->SetRefSys(m_RefSysVME);
        m_GuiTransformMouse->EnableWidgets(true);
      }
      else if (m_UseGizmo == 1)
      {
        m_GuiTransformMouse->EnableWidgets(false);

        mafEvent e(this,VIEW_SELECTED);
        mafEventMacro(e);

        if (m_ActiveGizmo == TR_GIZMO)
        {
          m_GizmoRotate->Show(false);
          m_GizmoTranslate->SetRefSys(m_RefSysVME);
          m_GizmoTranslate->Show(true && e.GetBool());
        }
        else if (m_ActiveGizmo == ROT_GIZMO)
        {
          m_GizmoTranslate->Show(false);
          m_GizmoRotate->SetRefSys(m_RefSysVME);
          m_GizmoRotate->Show(true && e.GetBool());
        }
      }
      mafEventMacro(mafEvent(this, CAMERA_UPDATE));
    }
    break;
    
    case ID_CHOOSE_GIZMO_COMBO:
    {
      mafEvent e(this,VIEW_SELECTED);
      mafEventMacro(e);

      if (m_ActiveGizmo == TR_GIZMO)
      {
        m_GizmoRotate->Show(false);
        m_GizmoTranslate->SetRefSys(m_RefSysVME);
        m_GizmoTranslate->Show(true && e.GetBool());
      }
      else if (m_ActiveGizmo == ROT_GIZMO)
      {
        m_GizmoTranslate->Show(false);
        m_GizmoRotate->SetRefSys(m_RefSysVME);
        m_GizmoRotate->Show(true && e.GetBool());
      }
      else if (m_ActiveGizmo == SCAL_GIZMO)
      {
        m_GizmoTranslate->Show(false);
        m_GizmoRotate->Show(false);
      }
      mafEventMacro(mafEvent(this, CAMERA_UPDATE));
    }
    break;

    case ID_RESET:
  	{ 
      Reset();
	  }
    break;
    case ID_ROTATION_STEP:
      m_GizmoRotate->GetInteractor(0)->GetRotationConstraint()->SetStep(0,m_RotationStep);
      m_GizmoRotate->GetInteractor(1)->GetRotationConstraint()->SetStep(1,m_RotationStep);
      m_GizmoRotate->GetInteractor(2)->GetRotationConstraint()->SetStep(2,m_RotationStep);
    break;
    case ID_TRANSLATION_STEP:
      m_GizmoTranslate->SetStep(0,m_TranslationStep);
      m_GizmoTranslate->SetStep(1,m_TranslationStep);
      m_GizmoTranslate->SetStep(2,m_TranslationStep);
    break;
    case ID_ENABLE_STEP:
      if (m_EnableStep != 0)
      {
        m_GizmoRotate->GetInteractor(0)->GetRotationConstraint()->SetConstraintModality(0,mafInteractorConstraint::SNAP_STEP);
        m_GizmoRotate->GetInteractor(1)->GetRotationConstraint()->SetConstraintModality(1,mafInteractorConstraint::SNAP_STEP);
        m_GizmoRotate->GetInteractor(2)->GetRotationConstraint()->SetConstraintModality(2,mafInteractorConstraint::SNAP_STEP);
        m_GizmoRotate->GetInteractor(0)->GetRotationConstraint()->SetStep(0,m_RotationStep);
        m_GizmoRotate->GetInteractor(1)->GetRotationConstraint()->SetStep(1,m_RotationStep);
        m_GizmoRotate->GetInteractor(2)->GetRotationConstraint()->SetStep(2,m_RotationStep);
        m_GizmoTranslate->SetConstraintModality(0,mafInteractorConstraint::SNAP_STEP);
        m_GizmoTranslate->SetConstraintModality(1,mafInteractorConstraint::SNAP_STEP);
        m_GizmoTranslate->SetConstraintModality(2,mafInteractorConstraint::SNAP_STEP);
        m_GizmoTranslate->SetStep(0,m_TranslationStep);
        m_GizmoTranslate->SetStep(1,m_TranslationStep);
        m_GizmoTranslate->SetStep(2,m_TranslationStep);
      }
      else
      {
        m_GizmoRotate->GetInteractor(0)->GetRotationConstraint()->SetConstraintModality(0,mafInteractorConstraint::FREE);
        m_GizmoRotate->GetInteractor(1)->GetRotationConstraint()->SetConstraintModality(1,mafInteractorConstraint::FREE);
        m_GizmoRotate->GetInteractor(2)->GetRotationConstraint()->SetConstraintModality(2,mafInteractorConstraint::FREE);
        m_GizmoTranslate->SetConstraintModality(0,mafInteractorConstraint::FREE);
        m_GizmoTranslate->SetConstraintModality(1,mafInteractorConstraint::FREE);
        m_GizmoTranslate->SetConstraintModality(2,mafInteractorConstraint::FREE);
      }
    break;
    // move this to opgui; both gizmos and gui should know ref sys
    case ID_AUX_REF_SYS:
    {
      mafString s = _R("Choose VME ref sys");
			mafEvent e(this,VME_CHOOSE, &s);
			mafEventMacro(e);
      SetRefSysVME(mafVME::SafeDownCast(e.GetVme()));
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

    default:
    {
      mafEventMacro(*maf_event);
    }
    break;
  }
}

//----------------------------------------------------------------------------
void medOpMove::OnEventGizmoTranslate(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
	{
    case ID_TRANSFORM:
  	{    
      // post multiplying matrixes coming from the gizmo to the vme
      // gizmo does not set vme pose  since they cannot scale
      PostMultiplyEventMatrix(maf_event);
      
      // update gui 
      m_GuiTransformTextEntries->SetAbsPose(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());
	  }
    break;
  
    default:
    {
      mafEventMacro(*maf_event);
    }
  }
}

//----------------------------------------------------------------------------
void medOpMove::OnEventGizmoRotate(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{ 
  switch(maf_event->GetId())
	{
    case ID_TRANSFORM:
  	{ 
      
      // post multiplying matrix coming from the gizmo to the vme
      // gizmo does not set vme pose  since they cannot scale
      PostMultiplyEventMatrix(maf_event);

      // update gui 
      m_GuiTransformTextEntries->SetAbsPose(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());
	  }
    break;

    default:
    {
      mafEventMacro(*maf_event);
    }
  }
}

//----------------------------------------------------------------------------
void medOpMove::OnEventGuiTransformMouse(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
	{
    case ID_TRANSFORM: // from mafGUITransformMouse
    {
      PostMultiplyEventMatrix(maf_event);

      // update gizmos positions
      if (m_GizmoTranslate) m_GizmoTranslate->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
      if (m_GizmoRotate) m_GizmoRotate->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());

      m_GuiTransformMouse->SetRefSys(m_RefSysVME);      
      m_GuiTransformTextEntries->SetAbsPose(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());
    }
    break;
    default:
    {
      mafEventMacro(*maf_event);
    }
  }
}             

//----------------------------------------------------------------------------
void medOpMove::OnEventGuiSaveRestorePose(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
	{
    case ID_TRANSFORM: // from m_GuiSaveRestorePose
    {
      // update gizmos positions
      m_GizmoTranslate->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
      m_GizmoRotate->SetAbsPose(m_RefSysVME->GetOutput()->GetAbsMatrix());
      m_GuiTransformMouse->SetRefSys(m_RefSysVME);

      // update gui 
      m_GuiTransformTextEntries->SetAbsPose(((mafVME *)m_Input)->GetOutput()->GetAbsMatrix());
      
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
void medOpMove::OnEventGuiTransformTextEntries(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_TRANSFORM: // from m_GuiTransformTextEntries
      {
        mafMatrix absPose;
        absPose = *(e->GetMatrix());
        absPose.SetTimeStamp(m_CurrentTime);

        // update gizmos positions if refsys is local
        if (m_RefSysVME == mafVME::SafeDownCast(m_Input))
        {      
          m_GizmoTranslate->SetAbsPose(&absPose);
          m_GizmoRotate->SetAbsPose(&absPose);
          m_GuiTransformMouse->SetRefSys(m_RefSysVME);
        }

        m_NewAbsMatrix = absPose;
      }
      break;
      default:
      {
        mafEventMacro(*e);
      }
    }
  }
}

//----------------------------------------------------------------------------
void medOpMove::CreateGui()
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

  // enable/disable gizmo interaction
  m_Gui->Label(_R("interaction modality"), true);
  mafString interactionModality[2] = {_R("mouse"), _R("gizmo")};
  m_Gui->Combo(ID_SHOW_GIZMO, _R(""),&m_UseGizmo,2,interactionModality);

  m_Gui->Divider(2);
  m_Gui->Label(_R("gizmo interaction"), true);
  m_Gui->Label(_R("left mouse: interact through gizmo"));

  // choose active gizmo
  mafString available_gizmos[3] = {_R("translate"), _R("rotate")};
  m_Gui->Combo(ID_CHOOSE_GIZMO_COMBO, _R(""), &m_ActiveGizmo, 2, available_gizmos);
  m_Gui->Divider(2);
  m_Gui->Label(_R("step parameters:"),true);
  m_Gui->Double(ID_TRANSLATION_STEP,_R("translation"),&m_TranslationStep,0.01);
  m_Gui->Double(ID_ROTATION_STEP,_R("rotation"),&m_RotationStep,0.01);
  m_Gui->Bool(ID_ENABLE_STEP,_R("on/off"),&m_EnableStep);
  m_Gui->Enable(ID_CHOOSE_GIZMO_COMBO, m_UseGizmo?true:false);
  m_Gui->Enable(ID_ROTATION_STEP, m_UseGizmo?true:false);
  m_Gui->Enable(ID_TRANSLATION_STEP, m_UseGizmo?true:false);
  m_Gui->Enable(ID_ENABLE_STEP, m_UseGizmo?true:false);

  //---------------------------------
  // Transform Gui
  //---------------------------------
  // create the transform Gui
  m_GuiTransformMouse = new mafGUITransformMouse(mafVME::SafeDownCast(m_Input), this);

  // add transform gui to operation
  m_Gui->AddGui(m_GuiTransformMouse->GetGui());

  m_Gui->Double(ID_TRANSLATE_X, _R("Translate X"), &m_TransfTranslation[0]);
  m_Gui->Double(ID_TRANSLATE_Y, _R("Translate Y"), &m_TransfTranslation[1]);
  m_Gui->Double(ID_TRANSLATE_Z, _R("Translate Z"), &m_TransfTranslation[2]);
  m_Gui->Double(ID_ROTATE_X, _R("Rotate X"), &m_TransfRotation[0]);
  m_Gui->Double(ID_ROTATE_Y, _R("Rotate Y"), &m_TransfRotation[1]);
  m_Gui->Double(ID_ROTATE_Z, _R("Rotate Z"), &m_TransfRotation[2]);

  m_Gui->Enable(ID_TRANSLATE_X, true);
  m_Gui->Enable(ID_TRANSLATE_Y, true);
  m_Gui->Enable(ID_TRANSLATE_Z, true);
  m_Gui->Enable(ID_ROTATE_X, true);
  m_Gui->Enable(ID_ROTATE_Y, true);
  m_Gui->Enable(ID_ROTATE_Z, true);
  if (false)
  {
    m_Gui->Double(ID_SCALE_X, _R("Scale X"), &m_TransfScaling[0], 0);
    m_Gui->Double(ID_SCALE_Y, _R("Scale Y"), &m_TransfScaling[1], 0);
    m_Gui->Double(ID_SCALE_Z, _R("Scale Z"), &m_TransfScaling[2], 0);
    m_Gui->Enable(ID_SCALE_X, true);
    m_Gui->Enable(ID_SCALE_Y, true);
    m_Gui->Enable(ID_SCALE_Z, true);
  }

  //---------------------------------
  // Text transform Gui
  //---------------------------------
  // create the transform Gui
  m_GuiTransformTextEntries = new mafGUITransformTextEntries(mafVME::SafeDownCast(m_Input), this,false);

  // add transform Gui to operation
  //m_Gui->AddGui(m_GuiTransformTextEntries->GetGui());
  m_Gui->RollOut(ID_ROLLOUT_TEXT_ENTRIES,_R(" Text entries"), m_GuiTransformTextEntries->GetGui(), false);

  //---------------------------------
  // Translation Gizmo Gui
  //---------------------------------
	
  // create the gizmos
  m_GizmoTranslate = new mafGizmoTranslate(mafVME::SafeDownCast(m_Input), this);
  m_GizmoTranslate->Show(false);

  // add translation gizmo Gui to operation
  //m_Gui->AddGui(m_GizmoTranslate->GetGui());
  m_Gui->RollOut(ID_ROLLOUT_GIZMO_TRANSLATE,_R(" Gizmo translate"), m_GizmoTranslate->GetGui(), false);
  
  //---------------------------------
  // Rotation Gizmo Gui
  //---------------------------------
  m_GizmoRotate = new mafGizmoRotate(mafVME::SafeDownCast(m_Input), this);
  m_GizmoRotate->Show(false);

  // add rotation gizmo Gui to operation
  //m_Gui->AddGui(m_GizmoRotate->GetGui());
  m_Gui->RollOut(ID_ROLLOUT_GIZMO_ROTATE,_R(" Gizmo rotate"), m_GizmoRotate->GetGui(), false);
  
  //---------------------------------
  // Store/Restore position Gui
  //---------------------------------
  m_GuiSaveRestorePose = new mafGUISaveRestorePose(mafVME::SafeDownCast(m_Input), this);
  
  // add Gui to operation
  //m_Gui->AddGui(m_GuiSaveRestorePose->GetGui());
  m_Gui->RollOut(ID_ROLLOUT_SAVE_POS,_R(" Save pose"), m_GuiSaveRestorePose->GetGui(), false);

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
}

//----------------------------------------------------------------------------
void medOpMove::Reset()
//----------------------------------------------------------------------------
{
  ((mafVME *)m_Input)->SetAbsMatrix(m_OldAbsMatrix);  
  if (!m_TestMode)
  {
    m_GuiTransformTextEntries->Reset();
    SetRefSysVME(mafVME::SafeDownCast(m_Input)); 
    mafEventMacro(mafEvent(this, CAMERA_UPDATE));
  }
}

//----------------------------------------------------------------------------
void medOpMove::RefSysVmeChanged()
//----------------------------------------------------------------------------
{
  // plugged components set their refsys;
  /*
  this should cycle on all plugged components => improve in order to use base class
  SetRefSys on mafGUITransformInterface pointer
  */

  // change isa refsys
  m_GuiTransformMouse->SetRefSys(m_RefSysVME);
  // change gtranslate refsys
  m_GizmoTranslate->SetRefSys(m_RefSysVME);
  // change grotate refsys
  m_GizmoRotate->SetRefSys(m_RefSysVME);
  // change grotate refsys
  m_GuiTransformTextEntries->SetRefSys(m_RefSysVME);

  m_RefSysVMEName = m_RefSysVME->GetName();
  assert(m_Gui);
  m_Gui->Update();
}
