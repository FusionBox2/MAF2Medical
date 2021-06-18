/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkHoleConnectivity.cxx
Language:  C++
Version:   $Id: vtkHoleConnectivity.cxx,v 1.3.2.2 2011-05-26 08:33:31 ior02 Exp $

Copyright (c) 2003-2004 Goodwin Lawlor
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE. 

Some modifications by Matteo Giacomoni in order to make it work
under MAF (www.openmaf.org)

=========================================================================*/

#include "vtkHoleConnectivity.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkPolyData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkHoleConnectivity);

//----------------------------------------------------------------------------
vtkHoleConnectivity::vtkHoleConnectivity(vtkPolyData *input,vtkIdType ID)
//----------------------------------------------------------------------------
{
	this->SetInputData(input);
	PointID = ID;
}
//----------------------------------------------------------------------------
vtkHoleConnectivity::~vtkHoleConnectivity()
//----------------------------------------------------------------------------
{  
}
//----------------------------------------------------------------------------
int vtkHoleConnectivity::RequestData(
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

//	vtkPolyData *output = this->GetOutput();
	//vtkPolyData *input = this->GetPolyDataInput(0);

	vtkPolyDataConnectivityFilter *connectivityFilter = vtkPolyDataConnectivityFilter::New();
	connectivityFilter->SetInputConnection(0,this->GetOutputPort());
	connectivityFilter->SetExtractionModeToClosestPointRegion ();
	connectivityFilter->SetClosestPoint(Point);
	connectivityFilter->Modified();
	connectivityFilter->Update();

	output->DeepCopy(vtkPolyData::SafeDownCast(connectivityFilter->GetOutput()));

	connectivityFilter->Delete();


	return 0;
}
//----------------------------------------------------------------------------
void vtkHoleConnectivity::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
	vtkPolyDataAlgorithm::PrintSelf(os,indent);
}

