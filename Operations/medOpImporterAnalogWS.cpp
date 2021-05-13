/*=========================================================================

 Program: MAF2Medical
 Module: medOpImporterAnalogWS
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
#include "medOpImporterAnalogWS.h"

#include <wx/busyinfo.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>
#include "mafGUI.h"

#include "mafTagArray.h"
#include "medVMEAnalog.h"

#include <iostream>

mafCxxTypeMacro(medOpImporterAnalogWS)

//----------------------------------------------------------------------------
medOpImporterAnalogWS::medOpImporterAnalogWS(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= true;
	m_File		= _R("");
	m_FileDir = mafGetApplicationDirectory() + _R("/Data/External/");

  m_EmgScalar = NULL;
}
//----------------------------------------------------------------------------
medOpImporterAnalogWS::~medOpImporterAnalogWS()
//----------------------------------------------------------------------------
{
  mafDEL(m_EmgScalar);
}
//----------------------------------------------------------------------------
mafOp* medOpImporterAnalogWS::Copy()   
//----------------------------------------------------------------------------
{
	medOpImporterAnalogWS *cp = new medOpImporterAnalogWS(GetLabel());
	cp->m_File = m_File;
  cp->m_FileDir = m_FileDir;
	return cp;
}
//----------------------------------------------------------------------------
void medOpImporterAnalogWS::OpRun()   
//----------------------------------------------------------------------------
{
	int result = OP_RUN_CANCEL;
	m_File = _R("");
	mafString pgd_wildc	= _R("EMG File (*.*)|*.*");
  mafString f;
  f = mafGetOpenFile(m_FileDir,pgd_wildc); 
	if(!f.IsEmpty() && mafFileExists(f))
	{
	  m_File = f;
    Read();
    result = OP_RUN_OK;
  }
  mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void medOpImporterAnalogWS::Read()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxSetCursor(wxCursor(wxCURSOR_WAIT));
	  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  }
  
  mafNEW(m_EmgScalar);
  mafString path, name, ext;
  mafSplitPath(m_File,&path,&name,&ext);
  m_EmgScalar->SetName(name);

  mafTagItem tag_Nature;
  tag_Nature.SetName(_R("VME_NATURE"));
  tag_Nature.SetValue(_R("NATURAL"));

  m_EmgScalar->GetTagArray()->SetTag(tag_Nature);

  mafString time, scalar;
  wxFileInputStream inputFile( m_File.toWx() );
  wxTextInputStream text( inputFile );

  double emg_time; 
  double val_scalar;
  wxString line;
  std::vector<mafString> stringVec;

  //check if file starts with the string "ANALOG"
  line = text.ReadLine(); 
  if (line.Cmp(wxString("ANALOG"))!= 0)
  {
    mafErrorMessage(_M("Invalid file format!"));
    return;
  }
 
  line = text.ReadLine();
  int comma = line.Find(',');
  wxString freq = line.SubString(0,comma - 1); //Read frequency 
  double freq_val;
  freq_val = atof(freq.c_str());

  //Put the signals names in a vector of string
  int num_tk;
  line = text.ReadLine();
  wxStringTokenizer tkzName(line,wxT(','),wxTOKEN_RET_EMPTY_ALL);
  num_tk = tkzName.CountTokens();

  tkzName.GetNextToken(); //To skip ","
  while (tkzName.HasMoreTokens())
  {
    stringVec.push_back(mafWxToString(tkzName.GetNextToken())); 
  }

  line = text.ReadLine();

  int space;
  unsigned i;
  unsigned c = 0;
  int rowNumber = 0;
  wxString frame;

  do  
  {
    line = text.ReadLine();
    rowNumber++;
  } while (!inputFile.Eof());

  vnl_matrix<double> emgMatrix;
  emgMatrix.set_size(rowNumber , num_tk);

  wxFileInputStream inputFile1( m_File.toWx() );
  wxTextInputStream text1( inputFile1 );
  
  line = text1.ReadLine();
  line = text1.ReadLine();
  line = text1.ReadLine();
  line = text1.ReadLine();
  line = text1.ReadLine();
  line.Replace(","," ");
    
  for (int n = 0; n < rowNumber; n++)
  {
    i = 0;
    space = line.Find(' ');
    frame = line.SubString(0,space - 1);
    emg_time = atof(frame)/freq_val; 
    emgMatrix.put(n,i, emg_time); //Add scalar value to the vnl_matrix

    wxStringTokenizer tkz(line,wxT(' '),wxTOKEN_RET_EMPTY_ALL);
    tkz.GetNextToken(); //To skip the time value

    while (tkz.HasMoreTokens())
    {
      i++;
      scalar = mafWxToString(tkz.GetNextToken());
      val_scalar = atof(scalar.GetCStr());
    
      emgMatrix.put(n,i,val_scalar); //Add scalar value to the vnl_matrix 
    }
    line = text1.ReadLine();
    line.Replace(","," ");
    if (!m_TestMode)
    {
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)(((double) n)/((double) rowNumber-1)*100.)));
    }
  } 

  vnl_matrix<double> emgMatrixTranspose = emgMatrix.transpose();

  m_EmgScalar->SetData(emgMatrixTranspose, 0);

  mafTagItem tag_Sig;
  tag_Sig.SetName(_R("SIGNALS_NAME"));
  tag_Sig.SetNumberOfComponents(num_tk - 1);
  m_EmgScalar->GetTagArray()->SetTag(tag_Sig);

  mafTagItem *tag_Signals = m_EmgScalar->GetTagArray()->GetTag(_R("SIGNALS_NAME"));
  for (int n = 0; n < stringVec.size(); n++)
  {
    tag_Signals->SetValue(stringVec[n], n);
  }
  
  if (!m_TestMode)
  {
    mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
    wxSetCursor(wxCursor(wxCURSOR_DEFAULT));
  }

  m_Output = m_EmgScalar;
  m_Output->ReparentTo(m_Input);
}
