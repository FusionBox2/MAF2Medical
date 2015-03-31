/*=========================================================================

 Program: MAF2Medical
 Module: medOpImporterLandmark
 Authors: Daniele Giunchi, Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpImporterLandmark_H__
#define __medOpImporterLandmark_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "medOperationsDefines.h"
#include "mafOp.h"
#include <vector>
#include <map>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMELandmarkCloud;
class mafEvent;
//----------------------------------------------------------------------------
// medOpImporterLandmark :
//----------------------------------------------------------------------------
/** */
class MED_OPERATION_EXPORT medOpImporterLandmark : public mafOp
{
public:
  mafTypeMacro(medOpImporterLandmark, mafOp)
  medOpImporterLandmark(const mafString& label = "");
  ~medOpImporterLandmark(); 
  mafOp* Copy();
  virtual void OnEvent(mafEventBase *maf_event);

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* node) {return true;};

  /** Builds operation's interface. */
  void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop	(int result);


  /** Set the filename for the file to import */
  void SetFileName(const char *file_name){m_Files.resize(1);m_Files[0] = file_name;};

  /** Import data. */
  bool Read();


  /** Makes the undo for the operation. */
  void OpUndo();                       // gia' implementata in mafOp

protected:
  /** Read the file.
  the format of the file admits some speficics.
  1) a line with initial # is a comment
  2) Before a sequence of landmark it can be a line with "Time XXX" where XXX is a number current of timestep
     After the list of landmark is finished for that time, a new line with Time XXY or similar will follow.
     If there's not time, the cloud is considered time-INVARIANT
  3) the line with landmark data are:
     nameOfLandmark x y z
  */
  mafVME *ReadFile(mafString& fname);

  /** Read the file.
  the format of the file admits some speficics.
  The line with landmark pose can be with any type of char separation (provided it has been selected by the user)
  */
  mafVME *ReadFileWithoutTag(mafString& fname);
  /** Create the dialog interface for the importer. */
  virtual void CreateGui();
  bool LoadDictionary();
  void DestroyDictionary();
  void DictionaryUpdate();

  void LMRenameUpdate();
  bool LoadLMRename();
  void DestroyLMRename();
  
  enum ID_LANDMARK_IMPORTER
  {
    ID_TYPE_FILE = MINID,
    ID_TYPE_SEPARATION,
    ID_ENABLE_STRING,
    ID_STRING_SEPARATION,
    ID_LOAD_DICT,
    ID_CLEAR_DICT,
    ID_LOAD_LMREN,
    ID_CLEAR_LMREN,
    MINID,
  };

  int m_TypeSeparation;
  int m_EnableString;
  mafString m_StringSeparation;
  std::vector<mafString>            m_Files;
  mafString                         m_FileDir;
  std::vector<mafVME*>              m_Results;
  int                               m_TagFileFlag;
  mafString                         m_DictionaryFileName;
  std::map<mafString, mafString>    m_dictionaryStruct;
  mafString                         m_LMRenameFileName;
  std::map<mafString, mafString>    m_LMRenameStruct;

  double m_DefaultRadius;
};
#endif
