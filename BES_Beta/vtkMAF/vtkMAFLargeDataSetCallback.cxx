/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFLargeDataSetCallback.cxx,v $ 
  Language: C++ 
  Date: $Date: 2011-05-26 08:51:00 $ 
  Version: $Revision: 1.1.2.3 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "mafEvent.h"
#include "vtkMAFLargeDataSetCallback.h"

#include "vtkObject.h"

/*virtual*/ void vtkMAFLargeDataSetCallback
	::Execute(vtkObject* caller, unsigned long eventId, void* callData)
{	
	if (GetListener() != NULL)
	{
		if (eventId == vtkCommand::ProgressEvent)
		{
			mafEvent ev(this, PROGRESSBAR_SET_VALUE, (intptr_t)(*((double*)callData)*100));
			InvokeEvent(&ev);
		}
		else if (eventId == vtkCommand::StartEvent)
		{
			mafEvent ev(this, PROGRESSBAR_SHOW);
      InvokeEvent(&ev);

			ev.SetId(PROGRESSBAR_SET_VALUE);
			ev.SetArg(0);
      InvokeEvent(&ev);

      mafString szStr = _R(caller->GetClassName()); //"Processing ";
			ev.SetId(PROGRESSBAR_SET_TEXT);
			ev.SetString(&szStr);
		}
		else if (eventId == vtkCommand::EndEvent)
		{
			mafEvent ev(this, PROGRESSBAR_HIDE);
      InvokeEvent(&ev);
		}
	}
}
/*virtual void vtkMAFLargeDataSetCallback::SetListener(mafObserver* listener)
  {
    Listener = listener;
  }*/