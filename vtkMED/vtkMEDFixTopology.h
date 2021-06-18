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

#ifndef __vtkMEDFixTopology_h
#define __vtkMEDFixTopology_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "vtkMEDConfigure.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkPolyDataAlgorithm.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// vtkMEDFixTopology class 
//----------------------------------------------------------------------------
/**
class name: vtkMEDFixTopology
This class is a filter which use vtkMEDPoissonSurfaceReconstruction class for fixing the topology.
*/
class VTK_vtkMED_EXPORT vtkMEDFixTopology : public vtkPolyDataAlgorithm
{
  public:
    /** create instance of the class*/
    static vtkMEDFixTopology *New();
    /** RTTI macro*/
    vtkTypeRevisionMacro(vtkMEDFixTopology,vtkPolyDataAlgorithm);
    /** print information*/
    void PrintSelf(ostream& os, vtkIndent indent);
  
  protected:
    /** constructor */
    vtkMEDFixTopology();
    /** destructor */
    ~vtkMEDFixTopology();

    /** execute the filter*/
    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  private:
    /** copy constructor not implemented*/
    vtkMEDFixTopology(const vtkMEDFixTopology&);
    /** operator= not implemented*/
    void operator=(const vtkMEDFixTopology&);
};

#endif