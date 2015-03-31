/*=========================================================================

 Program: MAF2Medical
 Module: medWizardBlock
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medWizardBlock_H__
#define __medWizardBlock_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
#include "mafEvent.h"
#include "mafEventSender.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: medWizardBlock.
  Class for the base wizard block, each block on a wizard must extend this block
*/
class MED_COMMON_EXPORT medWizardBlock : public mafEventSender
{
public:

  /** Default constructor   
      Requires the name of the block 
      "START","END" are reserved. */
  medWizardBlock(const char *name);

  /** Default destructor */
  virtual ~medWizardBlock();

  
  /** Get the name of the block */
  wxString GetName();


  /** Set name of the Block called after operation. 
      There are some special blocks:
      WIZARD{<name>} Switch to the wizard <name> and execute it
      START Goes to the first block of the wizard
      END Exits from the wizard
      */
  virtual void SetNextBlock(const char *block);

  /** Return the name of the Block witch will be executed after this */
  virtual wxString GetNextBlock();

  /** Called to clean up memory*/
  virtual void Delete(){delete this;};

  /** Get a Label containing a description of the the current step*/
  mafString GetDescriptionLabel();

  /** Set a Label containing a description of the the current step*/
  void SetDescriptionLabel(const char *label);

  /** Set a Label containing a description of the the current step*/
  void SetNextBlockOnAbort(const char *label);

  /** Abort the execution of the block */
  virtual void Abort();

  /** Set the progress associated to this block */
  void SetBlockProgress(int progress){m_BlockProgress=progress;};

  /** Get the progress associated to this block */
  int GetBlockProgress(){return m_BlockProgress;};

protected:

  /** Starts the execution of the block */
  virtual void ExcutionBegin();

  /** Ends the execution of the block */
  virtual void ExcutionEnd();

  /** Set the selected VME, this function must be called before execution begin*/
  void SetSelectedVME(mafNode *node);

  /** Return true if the user has aborted the operation */
  int Success();

  /** Returns the name of the operation required by this block 
      Return an empty string if no operation is required */
  virtual wxString GetRequiredOperation();

  wxString m_Name;
  wxString m_BlockType;
  wxString m_AbortBlock;
  wxString m_NextBlock;
  mafNode	*m_SelectedVME; ///< Pointer to the current selected node.
  mafNode *m_InputVME; ///< The vme selected on operation start.
  int m_Success;
  int m_Running;
  mafString m_DescriptionLabel;
  long m_BlockProgress;

private:

  friend class medWizard; // class medWizard can now access data directly
};
#endif
