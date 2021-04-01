/*=========================================================================

 Program: MAF2Medical
 Module: medOpImporterC3D
 Authors: Paolo Quadrani - porting Daniele Giunchi
 
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
#include "medOpImporterC3D.h"

#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVME.h"
//#include "mafC3DReader.h"

mafCxxTypeMacro(medOpImporterC3D)

//----------------------------------------------------------------------------
medOpImporterC3D::medOpImporterC3D(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo = true;
	m_File		= _R("");
	m_Vme			= NULL;
	this->m_DictionaryAvailable = 0;

	m_FileDir = mafGetApplicationDirectory(); 
  m_FileDir +=  _R("/Data/External/");
	m_DictDir = mafGetApplicationDirectory();
  m_DictDir += _R("/Config/Dictionary/");
}
//----------------------------------------------------------------------------
medOpImporterC3D::~medOpImporterC3D( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* medOpImporterC3D::Copy()   
/** restituisce una copia di se stesso, serve per metterlo nell'undo stack */
//----------------------------------------------------------------------------
{
	//non devo incrementare l'id counter --- vfc le operazioni sono gia inserite nei menu;
	medOpImporterC3D *cp = new medOpImporterC3D(GetLabel());
	cp->m_Canundo = m_Canundo;
	cp->m_OpType = m_OpType;
  cp->SetListener(GetListener());
	cp->m_Next = NULL;

	cp->m_File = m_File;
	cp->m_Vme = m_Vme;
	return cp;
}
//----------------------------------------------------------------------------
void medOpImporterC3D::OpRun()   
//----------------------------------------------------------------------------
{
	int result = OP_RUN_CANCEL;
	m_File = _R("");
	m_Dict = _R("");
	mafString c3d_wildc	= _R("C3D Data (*.c3d)|*.c3d");
	mafString dict_wildc = _R("Dictionary (*.txt)|*.txt");

	mafString f = mafGetOpenFile(m_FileDir,c3d_wildc); 
	if(!f.IsEmpty())
	{
		m_File = f;
		f = mafGetOpenFile(m_DictDir,dict_wildc,_R("Open Dictionary")); 
		if(!f.IsEmpty())
		{
			m_Dict = f;
			this->m_DictionaryAvailable = 1;
			result = OP_RUN_OK;
		}
		else
		{
			this->m_DictionaryAvailable = 0;
			result = OP_RUN_OK;
		}
	}
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void medOpImporterC3D::OpDo()   
/**  */
//----------------------------------------------------------------------------
{
  assert(!m_Vme);
	
	//modified by Stefano. 18-9-2003
	wxBusyInfo wait("Please wait, working...");
  
  //mafC3DReader *reader = mafC3DReader::New();
  //reader->SetFileName(m_File);
	//reader->SetDictionaryFileName(m_Dict);

	if (this->m_DictionaryAvailable)
		;//reader->DictionaryOn();
	else
		;//reader->DictionaryOff();

	//reader->Read();

  mafString path, name, ext;
  mafSplitPath(m_File,&path,&name,&ext);

	//m_vme = reader->GetOutput()->GetFirstChild();
	//m_Vme = reader->GetOutput();
  m_Vme->SetName(name);

	//mafEventMacro(mafEvent(this,VME_ADD,m_Vme));
	//reader->Delete();
}
//----------------------------------------------------------------------------
void medOpImporterC3D::OpUndo()   
/**  */
//----------------------------------------------------------------------------
{
	assert(m_Vme);
	mafEventMacro(mafEvent(this,VME_REMOVE,m_Vme));
	m_Vme->Delete();
	m_Vme = NULL;
}
