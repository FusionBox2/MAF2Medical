/*=========================================================================

 Program: MAF2Medical
 Module: medOpCreateLabeledVolume
 Authors: Roberto Mucci
 
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


#include "medOpCreateLabeledVolume.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMEVolumeGray.h"

#include "medVMELabeledVolume.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpCreateLabeledVolume);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpCreateLabeledVolume::medOpCreateLabeledVolume(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_LabeledVolume   = NULL;
}
//----------------------------------------------------------------------------
medOpCreateLabeledVolume::~medOpCreateLabeledVolume( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_LabeledVolume);
}
//----------------------------------------------------------------------------
mafOp* medOpCreateLabeledVolume::Copy()   
//----------------------------------------------------------------------------
{
	return new medOpCreateLabeledVolume(GetLabel());
}
//----------------------------------------------------------------------------
bool medOpCreateLabeledVolume::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMEVolumeGray));
}
//----------------------------------------------------------------------------
void medOpCreateLabeledVolume::OpRun()   
//----------------------------------------------------------------------------
{
  mafNEW(m_LabeledVolume);
  m_LabeledVolume->SetName(_R("Labeled Volume"));

 
  
  m_Output = m_LabeledVolume;
  m_LabeledVolume->SetVolumeLink(m_Input);
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void medOpCreateLabeledVolume::OpDo()
//----------------------------------------------------------------------------
{
  m_LabeledVolume->ReparentTo(m_Input);
}
