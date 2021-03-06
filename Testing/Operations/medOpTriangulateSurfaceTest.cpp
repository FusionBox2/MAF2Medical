/*=========================================================================

 Program: MAF2Medical
 Module: medOpTriangulateSurfaceTest
 Authors: Alessandro Chiarini , Matteo Giacomoni
 
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

#include "medOpTriangulateSurfaceTest.h"
#include "medOpTriangulateSurface.h"

#include "mafString.h"
#include "mafVMESurface.h"

#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkDataSet.h"
#include "vtkPolyData.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void medOpTriangulateSurfaceTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpTriangulateSurface *Triangulate=new medOpTriangulateSurface();
  mafDEL(Triangulate);
}
//-----------------------------------------------------------
void medOpTriangulateSurfaceTest::TestOpRun() 
//-----------------------------------------------------------
{
  vtkSphereSource *sphere=vtkSphereSource::New();
  sphere->Update();

  mafVMESurface *surfaceInput;
  mafNEW(surfaceInput);
  surfaceInput->SetData(sphere->GetOutput(),0.0);
  surfaceInput->GetOutput()->GetVTKData()->Update();
  surfaceInput->GetOutput()->Update();
  surfaceInput->Update();

  medOpTriangulateSurface *Triangulate=new medOpTriangulateSurface();
  Triangulate->TestModeOn();
  Triangulate->SetInput(surfaceInput);
  Triangulate->OpRun();
  Triangulate->OnTriangle();
  Triangulate->OpDo();

  mafDEL(surfaceInput);

  sphere->Delete();

  mafDEL(Triangulate);
}
