/*=========================================================================

 Program: MAF2Medical
 Module: medGUIWizardPage
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __medGUIWizardPage_H__
#define __medGUIWizardPage_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medGuiDefines.h"
#include "wx/wizard.h"
#include "mafEvent.h"
#include "mafEventSender.h"
#include "mafBaseEventHandler.h"
#include "mafRWI.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medGUIWizard;
class mafGUI;

//----------------------------------------------------------------------------
// Const:
//----------------------------------------------------------------------------
enum DIALOG_EXSTYLES
{
	medUSEGUI = 1,
	medUSERWI = 2,
};

/**
  Class Name: medGUIWizardPage.
  This class is the simplest possible medGUIWizard implementation.
  Add gui in panel positions:
  - lower left
  - lower right
  - lower center
*/
class MED_GUI_EXPORT medGUIWizardPage : public wxWizardPageSimple, public mafBaseEventHandler, public mafEventSender
{
public:
  /** constructor. */
	medGUIWizardPage (medGUIWizard *wizardParent,long style = medUSEGUI | medUSERWI,wxString label="",bool plugCentralGui = true,bool plugLeftGui = true,bool plugRightGui = true);
  /** destructor. */
	virtual ~medGUIWizardPage ();
  /** Precess events coming from other objects. */
	void OnEvent(mafEventBase *maf_event);

  /** Add in wizard gui, another gui in lower left position. */
	void AddGuiLowerLeft(mafGUI *gui);
  /** Add in wizard gui, another gui in lower right position. */
  void AddGuiLowerRight(mafGUI *gui);
  /** Add in wizard gui, another gui in lower center position. */
  void AddGuiLowerCenter(mafGUI *gui);

  /** Remove in wizard gui, another gui in lower left position. */
  void RemoveGuiLowerLeft(mafGUI *gui);

  /** Remove in wizard gui, another gui in lower right position. */
  void RemoveGuiLowerRight(mafGUI *gui);

  /** Remove in wizard gui, another gui in lower center position. */
  void RemoveGuiLowerCenter(mafGUI *gui);

	/** Create a chain between this page ad nextPage. */
	void SetNextPage(medGUIWizardPage *nextPage);

  /** Retrieve the current Render Window. */
	mafRWI* GetRWI(){return m_Rwi;};

protected:

	wxWizardPageSimple *m_FirstPage;

	wxBoxSizer *m_GUISizer; ///< Sizer used for the Lower GUI
  wxBoxSizer *m_RwiSizer; ///< Sizer used for the vtk render window and if you want to plug any gui on bottom of the RWI
  wxBoxSizer *m_SizerAll; ///< Vertical sizer used to include all other sizer

	mafRWI     *m_Rwi; ///< Render window

  mafGUI     *m_GuiLowerLeft; ///< Gui variable used to plug custom widgets localized in LOWER LEFT
  mafGUI     *m_GuiLowerRight; ///< Gui variable used to plug custom widgets localized in LOWER RIGHT
  mafGUI     *m_GuiLowerCenter; ///< Gui variable used to plug custom widgets localized in LOWER CENTER

  long m_Style;

	//DECLARE_EVENT_TABLE()

  friend class medGUIWizardPageTest;
};
#endif
