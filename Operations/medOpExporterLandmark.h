/*=========================================================================

 Program: MAF2Medical
 Module: medOpExporterLandmark
 Authors: Paolo Quadrani, Daniele Giunchi, Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpExporterLandmark_H__
#define __medOpExporterLandmark_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "medOperationsDefines.h"
#include "mafOp.h"

class mafVMELandmarkCloud;
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// medOpExporterLandmark :
//----------------------------------------------------------------------------
/** Exporter for the landmark coordinates: the data are exported in ASCII format. 
Each raw represents a landmark and contains the (x,y,z) coordinate.*/
class MED_OPERATION_EXPORT medOpExporterLandmark: public mafOp
{
public:
  mafTypeMacro(medOpExporterLandmark, mafOp);
	medOpExporterLandmark(const wxString &label = "Landmark Exporter");
	~medOpExporterLandmark(); 
	mafOp* Copy();
  void OnEvent(mafEventBase *maf_event);

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Build the interface of the operation, i.e the dialog that let choose the name of the output file. */
	void OpRun();

  /** Export landmarks contained into a mafVMELandmarkCloud.*/
  void ExportLandmark();

  /** Set the filename for the .stl to export */
  void SetFileName(const char *file_name) {m_File = file_name;};

protected:
  void ExportingTraverse(std::ostream &out, const char *dirName, mafNode* node);
  void ExportOneCloud(std::ostream &out, mafVMELandmarkCloud* cloud);

	wxString	m_File;
	wxString	m_FileDir;
  int       m_GlobalPos;
};
#endif
