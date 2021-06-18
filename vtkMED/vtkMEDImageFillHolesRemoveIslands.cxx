/*=========================================================================

 Program: MAF2Medical
 Module: vtkMEDImageFillHolesRemoveIslands
 Authors: Alberto Losi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "vtkMEDImageFillHolesRemoveIslands.h"
#include "vtkMEDBinaryImageFloodFill.h"

#include "vtkMAFSmartPointer.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStructuredPoints.h"
#include "vtkUnsignedCharArray.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"

#include <cassert>


#define PENINSULA_CORNER_MAXIMUM_NUMBER_OF_PIXELS 1

vtkCxxRevisionMacro(vtkMEDImageFillHolesRemoveIslands, "$Revision: 1.1.2.2 $");
vtkStandardNewMacro(vtkMEDImageFillHolesRemoveIslands);

//----------------------------------------------------------------------------
vtkMEDImageFillHolesRemoveIslands::vtkMEDImageFillHolesRemoveIslands()
//----------------------------------------------------------------------------
{
  SetAlgorithmToFillHoles(); // Algorithm = FILL_HOLES; DiscriminationPixelValue = 255;
  EdgeSize = 1;
  RemovePeninsulaRegions = false;
}

//----------------------------------------------------------------------------
vtkMEDImageFillHolesRemoveIslands::~vtkMEDImageFillHolesRemoveIslands()
//----------------------------------------------------------------------------
{ 
}

//----------------------------------------------------------------------------
void vtkMEDImageFillHolesRemoveIslands::SetAlgorithm(int algorithm)
//----------------------------------------------------------------------------
{
  // set the filter algorithm 
  assert(algorithm < INVALID_ALGORITHM && algorithm >= 0);
  Algorithm = algorithm;
  switch (Algorithm)
  {
  case FILL_HOLES:
    {
      SetAlgorithmToFillHoles();
      break;
    }
  case REMOVE_ISLANDS:
    {
      SetAlgorithmToRemoveIslands();
      break;
    }
  }
}

//------------------------------------------------------------------------------
int vtkMEDImageFillHolesRemoveIslands::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and output
 // vtkDataSet *input = vtkDataSet::SafeDownCast(
 //   inInfo->Get(vtkDataObject::DATA_OBJECT()));
  //vtkDataSet *output = vtkDataSet::SafeDownCast(
  //  outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // get input
  vtkStructuredPoints *input = (vtkStructuredPoints*)this->GetInput();
  //input->Update();

  // prepare output
  vtkStructuredGrid *output = this->GetOutput();
  output->DeepCopy(input);
  //output->UpdateData();
  //output->Update();

  int recognitionSquareEdge = EdgeSize + 2; // Number of pixels of the recognition square

  int dims[3];
  input->GetDimensions(dims);
  
  // Flood fill external region of the shape to allow fill big holes inside the shape
  vtkMEDBinaryImageFloodFill *flood_fill = vtkMEDBinaryImageFloodFill::New();
  flood_fill->SetInputData(input);
  //flood_fill->Update();

  // get flood fill scalars
  vtkUnsignedCharArray* filled_scalars = (vtkUnsignedCharArray*)input->GetPointData()->GetScalars();
  // get output scalars
  vtkUnsignedCharArray* output_scalars = (vtkUnsignedCharArray*)output->GetPointData()->GetScalars();

  while(recognitionSquareEdge >= 3)
  {
    // (x0, y0) origin of the recognition square
    for(int y0 = 0; y0 < dims[1] - recognitionSquareEdge; y0++)
    {

      for(int x0 = 0; x0 < dims[0] - recognitionSquareEdge; x0++)
      {
        bool isolatedRegion = true;
        int peninsulaConerNumberOfPixels = 0;

        // check "y" sides
        for(int y = y0; y < y0 + recognitionSquareEdge; y = y + recognitionSquareEdge - 1)
        {
          for(int x = x0; x < x0 + recognitionSquareEdge; x++)
          {
            if((filled_scalars->GetTuple1(y * dims[0] + x) != DiscriminationPixelValue))
            {
              if(((x == x0 && y == y0) || (x == x0 + recognitionSquareEdge - 1 && y == y0) || (x == x0 && y == y0  + recognitionSquareEdge - 1) || (x == x0 + recognitionSquareEdge - 1 && y == y0  + recognitionSquareEdge - 1)) && RemovePeninsulaRegions) // Corner pixels
              {
                peninsulaConerNumberOfPixels++; // update the number of "corner pixel"
              }
              else
              {
                isolatedRegion = false;
                break;
              }
            }
          }
          if(!isolatedRegion)
          {
            break;
          }
        }
        if(peninsulaConerNumberOfPixels > PENINSULA_CORNER_MAXIMUM_NUMBER_OF_PIXELS && isolatedRegion == true)
        {
          isolatedRegion = false; // not isolated region is not a peninsula
        }

        // check x sides
        if(isolatedRegion)
        {
          peninsulaConerNumberOfPixels = 0;
          for(int x = x0; x < x0 + recognitionSquareEdge; x = x + recognitionSquareEdge - 1)
          {
            for(int y = y0; y < y0 + recognitionSquareEdge; y++)
            {
              if((filled_scalars->GetTuple1(y * dims[0] + x) != DiscriminationPixelValue))
              {
                if(!(((x == x0 && y == y0) || (x == x0 + recognitionSquareEdge - 1 && y == y0) || (x == x0 && y == y0  + recognitionSquareEdge - 1) || (x == x0 + recognitionSquareEdge - 1 && y == y0  + recognitionSquareEdge - 1)) && RemovePeninsulaRegions)) // Corner pixels
                {
                  isolatedRegion = false;
                  break;
                }
              }
            }
            if(!isolatedRegion)
            {
              break;
            }
          }
        }

        if(isolatedRegion)
        {
          // fill all the recognition square with discrimination value
          for(int y = y0 + 1; y < y0 + recognitionSquareEdge - 1; y++)
          {
            for(int x = x0 + 1; x < x0 + recognitionSquareEdge - 1; x++)
            {
              if(filled_scalars->GetTuple1(y * dims[0] + x) != DiscriminationPixelValue)
              {
                output_scalars->SetTuple1(y * dims[0] + x, DiscriminationPixelValue);
              }
            }
          }
        }
      }
    }
    recognitionSquareEdge --;
  }
  // set output's scalars and filter's output
  output->GetPointData()->SetScalars(output_scalars);
  output->GetPointData()->Update();
  output->GetPointData()->Modified();
  flood_fill->Delete();
 // output->UpdateData();
 // output->Update();
  this->SetOutput(output);


  return 0;
}