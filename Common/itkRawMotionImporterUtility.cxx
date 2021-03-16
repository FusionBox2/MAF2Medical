/*=========================================================================

 Program:   Visualization Toolkit
 Module:    $RCSfile: itkRawMotionImporterUtility.cxx,v $
 Language:  C++
 Date:      $Date: 2012-04-06 09:04:52 $
 Version:   $Revision: 1.1.2.2 $

=========================================================================*/
#include "itkRawMotionImporterUtility.h"

#include <fstream>

//----------------------------------------------------------------------------
int itkRawMotionImporterUtility::ReadMatrix(vnl_matrix<double> &M, const char *fname)
{
  //Read raw motion data
	std::ifstream v_raw_matrix(fname, std::ios::in);

	
	if(v_raw_matrix.is_open() != 0)
	{	
		M.read_ascii(v_raw_matrix);
    return 0;
	}

 	return 1;
}
  

