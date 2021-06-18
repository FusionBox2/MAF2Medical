/*=========================================================================

 Program: MAF2Medical
 Module: vtkMEDFixTopology
 Authors: Fuli Wu
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "vtkMEDFixTopology.h"
#include "vtkMEDPoissonSurfaceReconstruction.h"

#include "float.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkTriangleFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkUnstructuredGrid.h"
vtkCxxRevisionMacro(vtkMEDFixTopology, "$Revision: 1.1.2.1 $");
vtkStandardNewMacro(vtkMEDFixTopology);

//----------------------------------------------------------------------------
vtkMEDFixTopology::vtkMEDFixTopology()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
vtkMEDFixTopology::~vtkMEDFixTopology()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void vtkMEDFixTopology::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
int vtkMEDFixTopology::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkUnstructuredGrid *input = vtkUnstructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid *output = vtkUnstructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkTriangleFilter *triangle_mesh = vtkTriangleFilter::New();
  triangle_mesh->SetInputDataObject(this->GetInput());

  vtkPolyDataNormals *points_with_normal = vtkPolyDataNormals::New();
  points_with_normal->SetInputConnection(triangle_mesh->GetOutputPort());

  vtkMEDPoissonSurfaceReconstruction *psr_polydata =vtkMEDPoissonSurfaceReconstruction::New();
  psr_polydata->SetInputConnection(points_with_normal->GetOutputPort());

  psr_polydata->Update();
  this->GetOutput()->DeepCopy(psr_polydata->GetOutput());  

  //points_with_normal->GetOutput()->Update();
  //this->GetOutput()->DeepCopy(points_with_normal->GetOutput());  

  psr_polydata->Delete();
  points_with_normal->Delete();
  triangle_mesh->Delete();
  return 0;
}