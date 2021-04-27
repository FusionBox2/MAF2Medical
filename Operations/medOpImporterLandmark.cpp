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

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpImporterLandmark.h"
#include <wx/busyinfo.h>


#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafVME.h"
#include "mafVMEGroup.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafTagArray.h"
#include "mafSmartPointer.h"

#include <string>

#include <fstream>

const bool DEBUG_MODE = true;

mafCxxTypeMacro(medOpImporterLandmark)

//----------------------------------------------------------------------------
medOpImporterLandmark::medOpImporterLandmark(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= true;
	m_FileDir = mafGetApplicationDirectory() + _R("/Data/External/");
  m_TypeSeparation = 0;
  m_EnableString = 0;
  m_StringSeparation = _R("");
	
	m_TagFileFlag        = true;
	m_DictionaryFileName = _R("");
  m_LMRenameFileName = _R("");

  m_DefaultRadius = 10;
}
//----------------------------------------------------------------------------
medOpImporterLandmark::~medOpImporterLandmark( ) 
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Results.size(); i++)
    mafDEL(m_Results[i]);
}
//----------------------------------------------------------------------------
mafOp* medOpImporterLandmark::Copy()   
/** restituisce una copia di se stesso, serve per metterlo nell'undo stack */
//----------------------------------------------------------------------------
{
	//non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
	medOpImporterLandmark *cp = new medOpImporterLandmark(GetLabel());
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
  cp->SetListener(GetListener());
	cp->m_Next = NULL;

	cp->m_Files               = m_Files;
	cp->m_Results             = m_Results;
  cp->m_TypeSeparation = m_TypeSeparation;
  cp->m_EnableString = m_EnableString;
  cp->m_StringSeparation = m_StringSeparation;
	return cp;
}


/** Create the dialog interface for the importer. */
//----------------------------------------------------------------------------
void medOpImporterLandmark::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  mafString choices[2] =  {_L("Space"),_L("Comma")};
  m_Gui->Radio(ID_TYPE_SEPARATION,_R("Separation"),&m_TypeSeparation,2,choices,1, _R(""));
  m_Gui->Bool(ID_ENABLE_STRING,_R("Other chars"),&m_EnableString,1);
  m_Gui->Divider();
  m_Gui->String(ID_STRING_SEPARATION, _R(""),&m_StringSeparation,_R("Insert here the right char for separation"));
  m_Gui->Divider();
  m_Gui->Bool(ID_TYPE_FILE,_R("Tagged file"),&m_TagFileFlag,0,_R("Check if the format is NAME x y z"));
  m_Gui->Label(_R(""));
  m_Gui->Double(ID_RADIUS, _R("Radius"), &m_DefaultRadius, 0);
  m_Gui->Divider();
  m_Gui->FileOpen(ID_LOAD_LMREN, _R("Rename"),  &m_LMRenameFileName, _R("*.txt"));
  m_Gui->Button(ID_CLEAR_LMREN, _R("Clean"), _R(""), _R("Press to cancel using LM renamer") );
  m_Gui->Divider();
  m_Gui->FileOpen(ID_LOAD_DICT, _R("Segment"),  &m_DictionaryFileName, _R("*.txt"));
  m_Gui->Button(ID_CLEAR_DICT, _R("Clean"), _R(""), _R("Press to cancel using dictionary") );

  m_Gui->Enable(ID_CLEAR_DICT, (!m_DictionaryFileName.IsEmpty()));
  m_Gui->Enable(ID_CLEAR_LMREN, (!m_LMRenameFileName.IsEmpty()));

  m_Gui->OkCancel();
  m_Gui->Enable(ID_ENABLE_STRING,!m_TagFileFlag);
  m_Gui->Enable(ID_TYPE_SEPARATION,!m_TagFileFlag && !m_EnableString);
  m_Gui->Enable(ID_STRING_SEPARATION,!m_TagFileFlag && m_EnableString);
}

//----------------------------------------------------------------------------
void medOpImporterLandmark::OpRun()   
//----------------------------------------------------------------------------
{

  mafString vrml_wildc = _R("Landmark (*.*)|*.*");

  m_Files.clear();
  //if (m_File.IsEmpty())
  {
    mafGetOpenMultiFiles(m_FileDir,vrml_wildc, m_Files);
  }

  int result = OP_RUN_CANCEL;

  if(m_Files.size() == 0) 
  {
    mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
  }
  else if (!m_TestMode)
  {
    CreateGui();
    ShowGui();
  }
  else
  {
    if(Read())
    {
      mafEventMacro(mafEvent(this,OP_RUN_OK));
    }
    else
    {
      mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
    }
  }
}

//----------------------------------------------------------------------------
void medOpImporterLandmark::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case wxOK:
      {
        if(Read())
        {
          OpStop(OP_RUN_OK);
        }
        else
        {
          OpStop(OP_RUN_CANCEL);
        }
      }
      break;
    case wxCANCEL:
      {
        OpStop(OP_RUN_CANCEL);
      }
      break;
      case ID_RADIUS:
        break;
      case ID_TYPE_FILE:
        {  
          if (m_TagFileFlag)
          {
            m_TypeSeparation = 0;
            m_EnableString = 0;
          }
          if (!m_TestMode)
          {
            m_Gui->Enable(ID_ENABLE_STRING,!m_TagFileFlag);
            m_Gui->Enable(ID_TYPE_SEPARATION,!m_TagFileFlag && !m_EnableString);
            m_Gui->Enable(ID_STRING_SEPARATION,!m_TagFileFlag && m_EnableString);
	          m_Gui->Update();
          }
        }
      break;
      case ID_ENABLE_STRING:
        {
          if (!m_TestMode)
          {
            m_Gui->Enable(ID_STRING_SEPARATION,!m_TagFileFlag && m_EnableString);
            m_Gui->Enable(ID_TYPE_SEPARATION,!m_TagFileFlag && !m_EnableString);
	          m_Gui->Update();
          }
        }
      break;
    case ID_CLEAR_DICT:
      {
        m_DictionaryFileName = _R("");
      }//WARNING! NO break operator here, execution will continue in ID_LOAD_DICT
    case ID_LOAD_DICT:
      {
        DictionaryUpdate();
        break;
      }
      case ID_CLEAR_LMREN:
        {
          m_LMRenameFileName = _R("");
        }//WARNING! NO break operator here, execution will continue in ID_LOAD_DICT
      case ID_LOAD_LMREN:
        {
          LMRenameUpdate();
          break;
        }
    default:
      mafEventMacro(*e);
      break;
    }  
  }
}
//----------------------------------------------------------------------------
void medOpImporterLandmark::OpDo()   
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Results.size(); i++)
  {
    if (m_Results[i])
    {
      m_Results[i]->ReparentTo(m_Input);
      //mafEventMacro(mafEvent(this, VME_ADD, m_Clouds[i]));
    }
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void medOpImporterLandmark::OpUndo()   
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Results.size(); i++)
  {
    if (m_Results[i])
    {
      mafEventMacro(mafEvent(this, VME_REMOVE, m_Results[i]));
    }
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
bool medOpImporterLandmark::Read()   
//----------------------------------------------------------------------------
{
  //modified by Stefano. 18-9-2003
  wxBusyInfo wait("Please wait, working...");

  for(unsigned i = 0; i < m_Results.size(); i++)
    mafDEL(m_Results[i]);
  m_Results.clear();

  for(unsigned i = 0; i < m_Files.size(); i++)
  {
    if(m_Files[i].IsEmpty())
      continue;
    mafString path, name, ext;
    mafSplitPath(m_Files[i],&path,&name,&ext);
    mafVME *imported;
    imported = (m_TagFileFlag) ? ReadFile(m_Files[i]) : ReadFileWithoutTag(m_Files[i]);
    if(imported == NULL)
    {
      for(unsigned i = 0; i < m_Results.size(); i++)
        mafDEL(m_Results[i]);
      m_Results.clear();
      return false;
    }
    imported->SetName(name);
    m_Results.push_back(imported);
    mafTagItem tag_Nature;
    tag_Nature.SetName(_R("VME_NATURE"));
    tag_Nature.SetValue(_R("NATURAL"));
    imported->GetTagArray()->SetTag(tag_Nature);
  }
  if(m_Files.size() == 1)
  {
    m_Output = m_Results[0];
  }
  return true;
}
//----------------------------------------------------------------------------
void medOpImporterLandmark::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
mafVME *medOpImporterLandmark::ReadFile(mafString& fname)   
//----------------------------------------------------------------------------
{
  // need the number of landmarks for the progress bar
  std::ifstream  landmarkNumberPromptFileStream(fname.GetCStr());
  int numberOfLines = 0;
  char tmp[200];
  while(!landmarkNumberPromptFileStream.fail())
  {
    landmarkNumberPromptFileStream.getline(tmp,200);
    numberOfLines += 1;
  }
  landmarkNumberPromptFileStream.close();

  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Reading " << numberOfLines << " lines in landmark file" << std::endl;
    mafLogMessage(_M(stringStream.str().c_str()));
  }

  mafVME *result = NULL;
  bool usingDictionary = (!m_DictionaryFileName.IsEmpty());
  mafVMEGroup         *group     = NULL;
  mafVMELandmarkCloud *specCloud = NULL;//the only cloud if read without dictionary and NOT_IN_DICTIONARY with
  mafString specCloudName;//name of specCloud

  if(!usingDictionary)//without dictionary create cloud and set its name
  {
    mafNEW(specCloud);
    if(specCloud == NULL)
      return NULL;
	specCloud->SetRadius(m_DefaultRadius);
    result = specCloud;
  }
  else//with dictionary just prepare name, creation only if needed
  {
    mafNEW(group);
    if(group == NULL)
      return NULL;
    result = group;
    specCloudName.Append(_R("NOT_IN_DICTIONARY"));
  }


  /*mafVMELandmarkCloud *cloud;
  mafNEW(cloud);
  //cloud->Open();*/

  std::ifstream  landmarkFileStream(fname.GetCStr());
  char nameBuf[50];
  char time[30] = "0";
  char tx[30];
  char ty[30];
  char tz[30];
  std::map<mafString, std::pair<mafVMELandmarkCloud*, int> > lms;
  std::map<mafString, mafVMELandmarkCloud*>                  clouds;

  double x = 0;
  double y = 0;
  double z = 0;
  double t = 0;

  if (!m_TestMode)
  {
    wxBusyInfo wait("Reading landmark cloud");
  }

  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));

  long counter = 0;
  long progress = 0;

  while(!landmarkFileStream.fail())
  {
    landmarkFileStream >> nameBuf;
	mafString nameStr;
	nameStr = mafString(_R(nameBuf));
#pragma message ("Use StartsWith")
    if(nameBuf[0] == '#' || nameStr.IsEmpty())
    {
      //jump the comment or the blank line
      landmarkFileStream.getline(nameBuf,20);
      continue;
    }
    else if(nameStr == _R("Time"))
    {
      landmarkFileStream >> time;
      counter = 0;
      continue;
    }
    else
    {
      landmarkFileStream >> tx;
      landmarkFileStream >> ty;
      landmarkFileStream >> tz;
      x = atof(tx);
      y = atof(ty);
      z = atof(tz);
      t = atof(time);

	  std::map<mafString, mafString>::iterator renIt = m_LMRenameStruct.find(nameStr);
	  //trajectory name found
	  if(renIt != m_LMRenameStruct.end())
		  nameStr = renIt->second;


      std::map<mafString, std::pair<mafVMELandmarkCloud*, int> >::iterator it = lms.find(nameStr);
      if(it == lms.end())
      {
        mafVMELandmarkCloud *addTo = specCloud;//by default add to this specific cloud

        if(usingDictionary)
        {
          //find current trajectory name in dictionary
          std::map<mafString, mafString>::iterator nmIt = m_dictionaryStruct.find(nameStr);
          //trajectory name found
          if(nmIt != m_dictionaryStruct.end())
          {
            //find corresponding cloud if already exists
            std::map<mafString, mafVMELandmarkCloud*>::iterator clIt = clouds.find(nmIt->second);
            //not created yet
            if(clIt == clouds.end() || clIt->second == NULL)
            {
              mafVMELandmarkCloud *cld = NULL;
              mafString cldName;
              //create cloud
              mafNEW(cld);
              //if created successfully use it
              if(cld != NULL)
              {
                cldName.Append(nmIt->second);
                cld->SetName(cldName);
				cld->SetRadius(m_DefaultRadius);
                clouds[nmIt->second] = cld;
                addTo = cld;
              }
              //clear and exit in case of problems in cloud creation, as we have not correct one, we cannot create new
              if(addTo == NULL)
              {
                for(auto it = clouds.begin(); it != clouds.end(); ++it)
                {
                  mafDEL(it->second);
                }
                clouds.clear();
                mafDEL(group);
                return NULL;
              }
              addTo->ReparentTo(group);
              addTo->UnRegister(this);
            }
            //cloud is already created, just select it to use
            else
            {
              addTo = clIt->second;
            }
          }
          //trajectory name not in dictionary
          else
          {
            //if NOT_IN_DICTIONARY is not created yet, create it and use. in case of problems exit
            if(specCloud == NULL)
            {
              mafNEW(specCloud);
              //clear and exit in case of problems in cloud creation, as we have not correct one, we cannot create new
              if(specCloud == NULL)
              {
                for(std::map<mafString, mafVMELandmarkCloud*>::iterator it = clouds.begin(); it != clouds.end(); ++it)
                {
                  mafDEL(it->second);
                }
                clouds.clear();
                mafDEL(group);
                return NULL;
              }
              //select this cloud for using
              specCloud->SetName(specCloudName);
			  specCloud->SetRadius(m_DefaultRadius);
              addTo = specCloud;
              specCloud->ReparentTo(group);
              addTo->UnRegister(this);
            }
          }
        }
        int idx = addTo->AppendLandmark(nameStr, false);
        it = lms.insert(std::make_pair(nameStr, std::make_pair(addTo, idx))).first;
      }

      it->second.first->SetLandmark(it->second.second, x ,y ,z,t);
      //m_VmeCloud->GetLandmark(counter)->SetRadius(5);
      if(x == -9999 && y == -9999 && z == -9999 )
        it->second.first->SetLandmarkVisibility(it->second.second, 0, t);
      counter++;

      progress = counter * 100 / numberOfLines;
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE, progress));

    }
  }
  //cloud->Close();
  //cloud->Modified();

  landmarkFileStream.close();

  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
  return result;
}
//----------------------------------------------------------------------------
mafVME *medOpImporterLandmark::ReadFileWithoutTag(mafString& fname)   
//----------------------------------------------------------------------------
{
  mafVMELandmarkCloud *cloud;
   mafNEW(cloud);
   cloud->SetRadius(m_DefaultRadius);
  //cloud->Open();

  // need the number of landmarks for the progress bar
  std::ifstream  landmarkNumberPromptFileStream(fname.GetCStr());
  int numberOfLines = 0;
  char tmp[200];
  while(!landmarkNumberPromptFileStream.fail())
  {
    landmarkNumberPromptFileStream.getline(tmp,200);
    numberOfLines += 1;
  }
  landmarkNumberPromptFileStream.close();

  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Reading " << numberOfLines - 1  << " landmarks" << std::endl;
    mafLogMessage(_M(stringStream.str().c_str()));
  }

  // end  number of lm check

  std::ifstream  landmarkFileStream(fname.GetCStr());

  double x = 0;
  double y = 0;
  double z = 0;

  long counter = 0; 
  long progress = 0;

  bool exception = FALSE;
  
  // select right type of character separation
  wxChar separation_char;
  if (!m_EnableString)
  {
    switch (m_TypeSeparation)
    {
    case 0:
        separation_char = ' ';
        break;
    case 1:
        separation_char = ',';
        break;
    }
  }
  else
  {
    separation_char = m_StringSeparation[0];
  }

  if (!m_TestMode)
  {
    wxBusyInfo wait("Reading landmark cloud");
  }

  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));

  while(!landmarkFileStream.fail())
  {
    landmarkFileStream.getline(tmp,200);
#pragma message ("needs check")
    if(mafString(_R(tmp)).IsEmpty()) 
    {
      // jump the blank line
      landmarkFileStream.getline(tmp,200);
      continue;
    }
    wxString s = wxString(tmp);
    wxString t1 = s.BeforeFirst(separation_char);
    wxString t2 = s.AfterFirst(separation_char);
    wxString t3 = t2.BeforeFirst(separation_char);
    wxString t4 = t2.AfterFirst(separation_char);

    t1.ToDouble(&x);
    t3.ToDouble(&y);
    t4.ToDouble(&z);

    // todo: optimize this append
    cloud->AppendLandmark(mafToString(counter), false);
    cloud->SetLandmark(counter, x ,y ,z,0);
    //m_VmeCloud->GetLandmark(counter)->SetRadius(5);
    if(x == -9999 && y == -9999 && z == -9999 )
      cloud->SetLandmarkVisibility(counter, 0, 0);
    
    counter++;

    progress = counter * 100 / numberOfLines;
    mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE, progress));

  }

  //cloud->Close();
  cloud->Modified();

  landmarkFileStream.close();

  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
  return cloud;
}
//----------------------------------------------------------------------------
bool medOpImporterLandmark::LoadDictionary()
//----------------------------------------------------------------------------
{
  std::string landmarkName, segmentName;
  std::ifstream dictionaryInputStream(m_DictionaryFileName.GetCStr(), std::ios::in);

  if(dictionaryInputStream.is_open() == 0)
    return false;
  while(dictionaryInputStream >> landmarkName)
  {
    dictionaryInputStream >> segmentName;
    auto it = m_dictionaryStruct.find(_R(landmarkName.c_str()));
    if(it != m_dictionaryStruct.end())
    {
      m_dictionaryStruct.clear();
      return false;
    }
    m_dictionaryStruct[_R(landmarkName.c_str())] = _R(segmentName.c_str());
  }
  return true;
}
//----------------------------------------------------------------------------
void medOpImporterLandmark::DestroyDictionary()
//----------------------------------------------------------------------------
{
  m_dictionaryStruct.clear();
}
//----------------------------------------------------------------------------
void medOpImporterLandmark::DictionaryUpdate() 
//----------------------------------------------------------------------------
{
  bool emptyName = m_DictionaryFileName.IsEmpty();
  DestroyDictionary();
  if(!emptyName)
  {
    if(!LoadDictionary())
    {
      wxLogMessage("Error reading dictionary.");
      m_DictionaryFileName = _R("");
    }
  }
  if(m_Gui)
  {
    m_Gui->Enable(ID_CLEAR_DICT, !emptyName);
    m_Gui->Update();
  }
}

//----------------------------------------------------------------------------
bool medOpImporterLandmark::LoadLMRename()
//----------------------------------------------------------------------------
{
  std::string landmarkName, segmentName;
  std::ifstream LMRenameInputStream(m_LMRenameFileName.GetCStr(), std::ios::in);

  if(LMRenameInputStream.is_open() == 0)
    return false;
  while(LMRenameInputStream >> landmarkName)	
  {
    LMRenameInputStream >> segmentName;			
    auto it = m_LMRenameStruct.find(_R(landmarkName.c_str()));
    if(it != m_LMRenameStruct.end())
    {
      m_LMRenameStruct.clear();
      return false;
    }
    m_LMRenameStruct[_R(landmarkName.c_str())] = _R(segmentName.c_str());
  }
  return true;
}
//----------------------------------------------------------------------------
void medOpImporterLandmark::DestroyLMRename()
//----------------------------------------------------------------------------
{
  m_LMRenameStruct.clear();
}
//----------------------------------------------------------------------------
void medOpImporterLandmark::LMRenameUpdate() 
//----------------------------------------------------------------------------
{
  bool emptyName = m_LMRenameFileName.IsEmpty();
  DestroyLMRename();
  if(!emptyName)
  {
    if(!LoadLMRename())
    {
      wxLogMessage("Error reading LM renamer.");
      m_LMRenameFileName = _R("");
    }
  }
  if(m_Gui)
  {
    m_Gui->Enable(ID_CLEAR_LMREN, !emptyName);
    m_Gui->Update();
  }
}
