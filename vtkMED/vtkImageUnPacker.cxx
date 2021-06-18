/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkImageUnPacker.cxx
  Language:  C++
  Date:      03/1999
  Version:   
  Credits:   This class has been developed by Marco Petrone

=========================================================================*/

#include <stdio.h>
#include <ctype.h>
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkImageUnPacker.h"
#include "vtkImageData.h"
#include "vtkStructuredGrid.h"
vtkCxxRevisionMacro(vtkImageUnPacker, "$Revision: 1.1.1.1.8.1 $");
vtkStandardNewMacro(vtkImageUnPacker);

//----------------------------------------------------------------------------
vtkImageUnPacker::vtkImageUnPacker()
//----------------------------------------------------------------------------
{
	this->Input=NULL;
	this->FileName=NULL;
  this->DataExtent[0]=0; this->DataExtent[1]=0; this->DataExtent[2]=0;
  this->DataExtent[3]=0; this->DataExtent[4]=0; this->DataExtent[5]=0;
	SetDataScalarType(VTK_UNSIGNED_CHAR);
	SetNumberOfScalarComponents(1);
	UnPackFromFileOff();
}

//----------------------------------------------------------------------------
vtkImageUnPacker::~vtkImageUnPacker()
//----------------------------------------------------------------------------
{
	SetInput(NULL);
	SetFileName(NULL);
}

//----------------------------------------------------------------------------
void vtkImageUnPacker::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  vtkImageAlgorithm::PrintSelf(os,indent);

  os << indent << "FileName: " <<
    (this->FileName ? this->FileName : "(none)") << "\n";
}

//----------------------------------------------------------------------------
// This method returns the largest data that can be generated.
int vtkImageUnPacker::RequestInformation(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Here information on the image are read and set in the Output cache.
	
	if (ReadImageInformation(this->GetInput()))
	{
		vtkGenericWarningMacro("Problems extracting the image information. ");
	}

	this->SetDataExtent(GetDataExtent());
	this->SetUpdateExtent(GetDataExtent());


	GetOutput()->SetScalarType(GetDataScalarType(), request);
	GetOutput()->SetNumberOfScalarComponents(GetNumberOfScalarComponents(), request);

	return 0;
}

//----------------------------------------------------------------------------
// This function reads an image from a stream.
int vtkImageUnPacker::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector, vtkImageData* data)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkStructuredGrid *input = vtkStructuredGrid::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkStructuredGrid *output = vtkStructuredGrid::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

	if (VtkImageUnPackerUpdate(this->Input,data))
	{
		vtkErrorMacro("Cannot Unpack Image!");
	}

	return 0;
}
