/*=========================================================================

 Program: MAF2Medical
 Module: medVMEOutputWrappedMeter
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

#include "medVMEOutputWrappedMeter.h"
#include "medVMEWrappedMeter.h"
#include "mafGUI.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(medVMEOutputWrappedMeter)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
medVMEOutputWrappedMeter::medVMEOutputWrappedMeter()
//-------------------------------------------------------------------------
{
	m_MiddlePoints.push_back(mafString()); //first middlepoint
	m_MiddlePoints.push_back(mafString()); //last middlepoint
}

//-------------------------------------------------------------------------
medVMEOutputWrappedMeter::~medVMEOutputWrappedMeter()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
mafGUI *medVMEOutputWrappedMeter::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();

  medVMEWrappedMeter *wrappedMeter = (medVMEWrappedMeter *)m_VME;
  m_Distance = mafToString(wrappedMeter->GetDistance());
  m_Gui->Label(_L("distance: "), &m_Distance, true);

	double *coordinateFIRST = NULL;
	double *coordinateLAST = NULL;

  if(wrappedMeter->GetWrappedMode() == medVMEWrappedMeter::MANUAL_WRAP)
  {
    coordinateFIRST = wrappedMeter->GetMiddlePointCoordinate(0);
    coordinateLAST = wrappedMeter->GetMiddlePointCoordinate(wrappedMeter->GetNumberMiddlePoints()-1);
  }
  else /*if(wrappedMeter->GetWrappedMode() == medVMEWrappedMeter::AUTOMATED_WRAP)*/
  {
    coordinateFIRST = wrappedMeter->GetWrappedGeometryTangent1();
    coordinateLAST =  wrappedMeter->GetWrappedGeometryTangent2();
  }

  if(coordinateFIRST != NULL)
    m_MiddlePoints[0] = mafString::Format(_R("%.2f %.2f %.2f"), coordinateFIRST[0], coordinateFIRST[1], coordinateFIRST[2]);
  if(coordinateLAST != NULL)
    m_MiddlePoints[m_MiddlePoints.size()-1] = mafString::Format(_R("%.2f %.2f %.2f"), coordinateLAST[0], coordinateLAST[1], coordinateLAST[2]);

  
	m_Gui->Label(_L("first mp:"), &m_MiddlePoints[0], true);
	m_Gui->Label(_L("last mp:"), &m_MiddlePoints[m_MiddlePoints.size()-1], true);

  m_Angle = mafToString(wrappedMeter->GetAngle());
  m_Gui->Label(_L("angle: "), &m_Angle, true);
	m_Gui->Divider();

  return m_Gui;
}
//-------------------------------------------------------------------------
void medVMEOutputWrappedMeter::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->Update();

  medVMEWrappedMeter *wrappedMeter = (medVMEWrappedMeter *)m_VME;

  if(wrappedMeter->GetMeterMode() == medVMEWrappedMeter::POINT_DISTANCE)
  {
		m_Distance = mafToString(((medVMEWrappedMeter *)m_VME)->GetDistance());

    double *coordinateFIRST = NULL;
    double *coordinateLAST = NULL; 
    
    if(wrappedMeter->GetNumberMiddlePoints() == 0) return;
    if(wrappedMeter->GetWrappedMode() == medVMEWrappedMeter::MANUAL_WRAP)
    {
      coordinateFIRST = wrappedMeter->GetMiddlePointCoordinate(0);
      coordinateLAST = wrappedMeter->GetMiddlePointCoordinate(wrappedMeter->GetNumberMiddlePoints()-1);
    }
    else /*if(wrappedMeter->GetWrappedMode() == medVMEWrappedMeter::AUTOMATED_WRAP)*/
    {
      coordinateFIRST = wrappedMeter->GetWrappedGeometryTangent1();
      coordinateLAST =  wrappedMeter->GetWrappedGeometryTangent2();
    }

		
		if(coordinateFIRST != NULL)
			m_MiddlePoints[0] = mafString::Format(_R("%.2f %.2f %.2f"), coordinateFIRST[0], coordinateFIRST[1], coordinateFIRST[2]);
		if(coordinateLAST != NULL)
			m_MiddlePoints[m_MiddlePoints.size()-1] = mafString::Format(_R("%.2f %.2f %.2f"), coordinateLAST[0], coordinateLAST[1], coordinateLAST[2]);

		m_Angle =_R("");
  }
/*	else if(wrappedMeter->GetMeterMode() == medVMEWrappedMeter::LINE_DISTANCE)
	{
		m_Distance = wrappedMeter->GetDistance();
		m_Angle ="";
	}
  else if(wrappedMeter->GetMeterMode() == medVMEWrappedMeter::LINE_ANGLE)
  {
    m_Distance ="";
    m_Angle= wrappedMeter->GetAngle();
  }*/
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
