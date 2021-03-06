/*=========================================================================

 Program: MAF2Medical
 Module: medVMEOutputSurfaceEditor
 Authors: Matteo Giacomoni
 
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

#include "medVMEOutputSurfaceEditor.h"
#include "mafVME.h"
#include "mafGUI.h"

#include "vtkPolyData.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(medVMEOutputSurfaceEditor)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
medVMEOutputSurfaceEditor::medVMEOutputSurfaceEditor()
//-------------------------------------------------------------------------
{

}

//-------------------------------------------------------------------------
medVMEOutputSurfaceEditor::~medVMEOutputSurfaceEditor()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
vtkPolyData *medVMEOutputSurfaceEditor::GetSurfaceData()
//-------------------------------------------------------------------------
{
	return (vtkPolyData *)GetVTKData();
}
//-------------------------------------------------------------------------
mafGUI* medVMEOutputSurfaceEditor::CreateGui()
//-------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = mafVMEOutput::CreateGui();

	return m_Gui;
}
//-------------------------------------------------------------------------
void medVMEOutputSurfaceEditor::Update()
//-------------------------------------------------------------------------
{
	assert(m_VME);
	m_VME->Update();

	if (m_Gui)
	{
		m_Gui->Update();
	}
}
