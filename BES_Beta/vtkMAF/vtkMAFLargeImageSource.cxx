/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeImageSource.cxx,v $ 
  Language: C++ 
  Date: $Date: 2011-05-26 08:51:01 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#include "vtkObjectFactory.h"
#include "vtkMAFLargeImageSource.h"
#include "vtkMAFLargeImageData.h"
#include "vtkMAFLargeDataProvider.h"
#include "vtkExecutive.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

#include "vtkUnstructuredGrid.h"

vtkCxxRevisionMacro(vtkMAFLargeImageSource, "$Revision: 1.1.2.2 $");
vtkStandardNewMacro(vtkMAFLargeImageSource);

//----------------------------------------------------------------------------
vtkMAFLargeImageSource::vtkMAFLargeImageSource()
{


	


	this->GetExecutive()->SetOutputData(0,vtkMAFLargeImageData::New());
	// Releasing data for pipeline parallism.
	// Filters will know it is empty. 
	this->GetOutput(0)->ReleaseData();
	this->GetOutput(0)->Delete();
}

//----------------------------------------------------------------------------
// Specify the input data or filter.
void vtkMAFLargeImageSource::SetOutput(vtkMAFLargeImageData *output)
{
	this->GetExecutive()->SetOutputData(0, output);
}

//----------------------------------------------------------------------------
// Specify the input data or filter.
vtkMAFLargeImageData *vtkMAFLargeImageSource::GetOutput()
{
	if (this->GetNumberOfOutputPorts() < 1)//NumberOfOutputs
	{
		return NULL;
	}

	return (vtkMAFLargeImageData *)(this->GetOutput(0));
}


//----------------------------------------------------------------------------
// Convert to Imaging API
int vtkMAFLargeImageSource::RequestData(
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
  vtkMAFLargeImageData*output = vtkMAFLargeImageData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

	//vtkMAFLargeImageData *output = this->GetOutput();

	// If we have multiple Outputs, they need to be allocate
	// in a subclass.  We cannot be sure all outputs are images.
	//output->SetExtent(this->GetUpdateExtent());
	//output->AllocateScalars();

	this->Execute(this->GetOutput());

	return 0;
}

//----------------------------------------------------------------------------
// This function can be defined in a subclass to generate the data
// for a region.
/*int vtkMAFLargeImageSource::RequestData(
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

	vtkErrorMacro(<< "Execute(): Method not defined.");


	return 0;
}*/


//----------------------------------------------------------------------------
vtkMAFLargeImageData *vtkMAFLargeImageSource::AllocateOutputData(vtkDataObject *out)
{
	vtkMAFLargeImageData *res = vtkMAFLargeImageData::SafeDownCast(out);
	if (!res)
	{
		vtkWarningMacro("Call to AllocateOutputData with non vtkMAFLargeImageData output");
		return NULL;
	}

	// I would like to eliminate this method which requires extra "information"
	// That is not computed in the graphics pipeline.
	// Until I can eliminate the method, I will reexecute the ExecuteInformation
	// before the execute.
	//this->ExecuteInformation();

	vtkInformation* info;
	vtkInformationVector *infoV,**infoVect;
	this->RequestInformation(info,infoVect,infoV);


	res->SetExtent(this->GetUpdateExtent());
	res->AllocateScalars();
	return res;
}

//----------------------------------------------------------------------------
vtkMAFLargeImageData *vtkMAFLargeImageSource::GetOutput(int idx)
{
	return (vtkMAFLargeImageData *) this->vtkAlgorithm::GetOutputPort(idx);
}

//----------------------------------------------------------------------------
void vtkMAFLargeImageSource::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}
