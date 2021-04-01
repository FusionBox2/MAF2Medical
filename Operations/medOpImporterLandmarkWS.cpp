/*=========================================================================

 Program: MAF2Medical
 Module: medOpImporterLandmarkWS
 Authors: Roberto Mucci
 
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

#include "medOpImporterLandmarkWS.h"
#include <wx/busyinfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafVME.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafTagArray.h"
#include "mafSmartPointer.h"

#include <iostream>

mafCxxTypeMacro(medOpImporterLandmarkWS)

//----------------------------------------------------------------------------
medOpImporterLandmarkWS::medOpImporterLandmarkWS(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= true;
	m_File		= _R("");
	m_FileDir = mafGetApplicationDirectory() + _R("/Data/External/");
	m_VmeCloud		= NULL;
}
//----------------------------------------------------------------------------
medOpImporterLandmarkWS::~medOpImporterLandmarkWS()
//----------------------------------------------------------------------------
{
  mafDEL(m_VmeCloud);
}
//----------------------------------------------------------------------------
mafOp* medOpImporterLandmarkWS::Copy()   
//----------------------------------------------------------------------------
{
	medOpImporterLandmarkWS *cp = new medOpImporterLandmarkWS(GetLabel());
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
  cp->SetListener(GetListener());
	cp->m_Next = NULL;

	cp->m_File = m_File;
	cp->m_VmeCloud = m_VmeCloud;
	return cp;
}
//----------------------------------------------------------------------------
void medOpImporterLandmarkWS::OpRun()   
//----------------------------------------------------------------------------
{
	int result = OP_RUN_CANCEL;
	m_File = _R("");
	mafString pgd_wildc	= _R("Landmark (*.*)|*.*");
  mafString f;
  if (!m_TestMode)
    {
      f = mafGetOpenFile(m_FileDir,pgd_wildc); 
    }
	if(!f.IsEmpty() && mafFileExists(f))
	 {
	   m_File = f;
     Read();
     result = OP_RUN_OK;
	 }
  mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void medOpImporterLandmarkWS::Read()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait("Please wait, working...");
  }
  mafNEW(m_VmeCloud);
  mafString path, name, ext;
  mafSplitPath(m_File,&path,&name,&ext);
  m_VmeCloud->SetName(name);

  mafTagItem tag_Nature;
  tag_Nature.SetName(_R("VME_NATURE"));
  tag_Nature.SetValue(_R("NATURAL"));

  m_VmeCloud->GetTagArray()->SetTag(tag_Nature);

  if (m_TestMode == true)
  {
	m_VmeCloud->TestModeOn();
  }

  m_VmeCloud->Open();
  m_VmeCloud->SetRadius(10);

  wxString skipc, line;
  mafString time, first_time, x, y, z;
  double xval, yval, zval, tval;
  std::vector<mafString> stringVec;
  
  std::vector<int> lm_idx;

  wxFileInputStream inputFile( m_File.toWx() );
  wxTextInputStream text( inputFile );

  //check if file starts with the string "ANALOG"
  line = text.ReadLine(); //Ignore 4 lines of textual information
  if (line.CompareTo("TRAJECTORIES")!= 0)
  {
    mafErrorMessage(_M("Invalid file format!"));
    return;
  }

  line = text.ReadLine();

  //Read frequency 
  int comma = line.Find(',');
  wxString freq = line.SubString(0,comma - 1);
  double freq_val;
  freq_val = atof(freq.c_str());

  //Put the signals names in a vector of string
  line = text.ReadLine();
  wxStringTokenizer tkzName(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  
  tkzName.GetNextToken(); //To skip ","
  while (tkzName.HasMoreTokens())
  {
    stringVec.push_back(mafWxToString(tkzName.GetNextToken())); 
    tkzName.GetNextToken(); //To skip ","
    tkzName.GetNextToken(); //To skip ","
  }
  
  line = text.ReadLine();
  line = text.ReadLine();

  wxStringTokenizer tkz_numAL(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  int numland = (tkz_numAL.CountTokens()-1)/3;

  mafString lm_name;
  int index;
  std::vector<int> indexSPlitOriginal;
  std::vector<int> indexSplitCopy;

  for (int i=0;i<numland;i++)
  {
    lm_name = stringVec.at(i);
    index = m_VmeCloud->FindLandmarkIndex(lm_name);
    if (index == -1)
    {
      lm_idx.push_back(m_VmeCloud->AppendLandmark(lm_name));
    }
    else
    {
      //To store index of LA split
     indexSPlitOriginal.push_back(index);
     indexSplitCopy.push_back(i);
    }
  }
  
  do 
  {
    wxStringTokenizer tkz(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
    time = mafWxToString(tkz.GetNextToken());
    long counter = 0;
    int counterAL = 0;
    int indexCounter = 0;
    tval = atof(time.GetCStr())/freq_val;

    while (tkz.HasMoreTokens())
    {
      x = mafWxToString(tkz.GetNextToken());
      y = mafWxToString(tkz.GetNextToken());
      z = mafWxToString(tkz.GetNextToken());
      xval = atof(x.GetCStr());
      yval = atof(y.GetCStr());
      zval = atof(z.GetCStr());


      if (!indexSplitCopy.empty()) //true if AL is split in columns
      {
        if ( counter == indexSplitCopy[indexCounter]) //If TRUE this AL already exists
        {
          if(!x.IsEmpty() && !y.IsEmpty() && !z.IsEmpty() )
          {
            //Insert the values in the AL with the same name (idx)
            m_VmeCloud->SetLandmark(lm_idx[indexSPlitOriginal[indexCounter]],xval,yval,zval,tval);
          }
          indexCounter++;
          counter++;
        }
        else
        {
          if(x.IsEmpty() && y.IsEmpty() && z.IsEmpty() )
          {
            m_VmeCloud->SetLandmark(lm_idx[counterAL],0,0,0,tval);
            m_VmeCloud->SetLandmarkVisibility(lm_idx[counterAL], 0,tval);
          }
          else
          {
            m_VmeCloud->SetLandmark(lm_idx[counterAL],xval,yval,zval,tval);
          }
          counter++;
          counterAL++;
        }
      }
      else //AL is not spit in columns
      {
        if(x.IsEmpty() && y.IsEmpty() && z.IsEmpty() )
        {
          m_VmeCloud->SetLandmark(lm_idx[counterAL],0,0,0,tval);
          m_VmeCloud->SetLandmarkVisibility(lm_idx[counterAL], 0,tval);
        }
        else
        {
          m_VmeCloud->SetLandmark(lm_idx[counterAL],xval,yval,zval,tval);
        }
        counter++;
        counterAL++;
      }
 
    }
    line = text.ReadLine();

  } while (!inputFile.Eof());

  m_VmeCloud->Modified();
  m_VmeCloud->ReparentTo(m_Input);  
  m_Output = m_VmeCloud;
}
