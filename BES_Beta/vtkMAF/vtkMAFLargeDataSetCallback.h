/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeDataSetCallback.h,v $ 
  Language: C++ 
  Date: $Date: 2012-04-06 10:17:54 $ 
  Version: $Revision: 1.1.2.4 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#ifndef __vtkMAFLargeDataSetCallback__
#define __vtkMAFLargeDataSetCallback__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
#include "vtkCommand.h"
#include "mafBaseEventHandler.h"
#include "mafEventSender.h"

//----------------------------------------------------------------------------
// Forward declarations:
//----------------------------------------------------------------------------

//this class handles events produced by LargeDataSet
class MED_COMMON_EXPORT vtkMAFLargeDataSetCallback : public vtkCommand, public mafEventSender
{
  //vtkTypeMacro(vtkMAFLargeDataSetCallback,vtkCommand);

public:
	inline static vtkMAFLargeDataSetCallback* New() {
		return new vtkMAFLargeDataSetCallback();
	}

	//callback routine called by VTK, translate events into MAF events
	virtual void Execute(vtkObject* caller, unsigned long eventId, void* callData);
  
  virtual const char *GetClassName() const {return "vtkMAFLargeDataSetCallback";};


protected:
	vtkMAFLargeDataSetCallback() {
	}

	~vtkMAFLargeDataSetCallback() {
		
	}
};

#endif //__vtkMAFLargeDataSetCallback__