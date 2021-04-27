/*=========================================================================

 Program: MAF2Medical
 Module: medOpExporterLandmark
 Authors: Stefania Paperini , Daniele Giunchi, Simone Brazzale
 
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

#include "medOpExporterLandmark.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafOpExplodeCollapse.h"

#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"

#include <fstream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpExporterLandmark);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpExporterLandmark::medOpExporterLandmark(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File = _R("");
  m_FileDir = _R("");
  m_Input   = NULL;
  m_GlobalPos = true;
}
//----------------------------------------------------------------------------
medOpExporterLandmark::~medOpExporterLandmark()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool medOpExporterLandmark::Accept(mafNode *node)   
//----------------------------------------------------------------------------
{ 
  if(node == NULL)
    return false;

  return true;
//  return node && node->IsMAFType(mafVMELandmarkCloud);
}
enum STL_EXPORTER_ID
{
  ID_ABS_POSITION = MINID,
};
//----------------------------------------------------------------------------
void medOpExporterLandmark::OpRun()   
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->Label(_R("absolute matrix"),true);
  m_Gui->Bool(ID_ABS_POSITION,_R("apply"),&m_GlobalPos,0);
  m_Gui->OkCancel();
  m_Gui->Divider();
  ShowGui();
}

//----------------------------------------------------------------------------
void medOpExporterLandmark::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case wxOK:
      /*{
        wxString proposed = mafGetApplicationDirectory().c_str();
        proposed += "/Data/External/";
        proposed += m_Input->GetName();
        proposed += ".txt";
        wxString wildc = "ascii file (*.txt)|*.txt";
        wxString f = mafGetSaveFile(proposed,wildc).c_str(); 
        int result = OP_RUN_CANCEL;
        if(!f.IsEmpty())
        {
          m_File = f;
          ExportLandmark();
          result = OP_RUN_OK;
        }
      }*/
      
      {
        int result = OP_RUN_CANCEL;
        m_Gui->Enable(wxOK, false);
        m_Gui->Enable(wxCANCEL, false);

        assert(m_Input);
        mafString proposed = mafGetApplicationDirectory() + _R("/Data/External/");

        if(m_Input->IsMAFType(mafVMELandmarkCloud))
        {
          proposed += m_Input->GetName();
          proposed += _R(".txt");
          mafString wildc = _R("ascii file (*.txt)|*.txt");

          mafString f = mafGetSaveFile(proposed,wildc); 

          if(!f.IsEmpty()) 
          {
            m_File = f;
            ExportLandmark();
            result = OP_RUN_OK;
          }
        }
        else
        {
          /*wxMessageDialog dialog(mafGetFrame(), _("Do you want to create separate files?"),
            _("Options"), wxYES_NO|wxYES_DEFAULT);
          if(dialog.ShowModal() == wxID_NO)
          {

            proposed += m_Input->GetName();
            proposed += ".txt";
            wxString wildc = "ascii file (*.txt)|*.txt";
            wxString f = mafGetSaveFile(proposed,wildc).c_str(); 

            if(f != "") 
            {
              m_File = f;
              ExportLandmark();
              result = OP_RUN_OK;
            }
          }
          else*/
          {
            mafString f = mafGetDirName(proposed);

            if(!f.IsEmpty()) 
            {
              m_FileDir = f;
              ExportLandmark();
              result = OP_RUN_OK;
            }
          }
        }
      }
      OpStop(OP_RUN_OK);
      break;
    case wxCANCEL:
      OpStop(OP_RUN_CANCEL);
      break;
    default:
      Superclass::OnEvent(maf_event);
      break;
    }
  }
}


void medOpExporterLandmark::ExportOneCloud(std::ostream &out, mafVMELandmarkCloud* cloud)
{
  std::vector<mafTimeStamp> timeStamps;
  if(m_GlobalPos)
    cloud->GetAbsTimeStamps(timeStamps);
  else
    cloud->GetLocalTimeStamps(timeStamps);
  int numberLandmark = cloud->GetNumberOfLandmarks();

  // if cloud is closed , open it
  bool initState = cloud->IsOpen();
  if(!initState)
    cloud->Open();

  mafString lmName;
  double pos[3] = {0.0,0.0,0.0};
  double ori[3] = {0.0,0.0,0.0};
  double t;
  int nmLength = 4;//time
  for(int j=0; j < numberLandmark; j++)
  {
    int lc;
    lmName = cloud->GetLandmarkName(j);
    lc = lmName.Length();
    if(nmLength < lc)
      nmLength = lc;
  }
  // pick up the values and write them into the file
  if(timeStamps.size() != 0)
  {
    for (int index = 0; index < timeStamps.size(); index++)
    {
      char numbs[1000];
      t = timeStamps[index];
      out << "Time";
      int curL = 4;
      for (int i = 0; i < nmLength - curL; i++)
      {
        out << " ";
      }
      sprintf(numbs, " %16lf\n", t);
      out << numbs;
      for(int j=0; j < numberLandmark; j++)
      {
        lmName = cloud->GetLandmarkName(j);

        //if(!cloud->GetLandmarkVisibility(j, timeStamps[index]))
        //  continue;

        mafMatrix cloudAbs;
        double invec[4];
        double outvec[4];
        cloud->GetOutput()->GetAbsMatrix(cloudAbs, timeStamps[index]);
        cloud->GetLandmark(j, invec, timeStamps[index]);
        invec[3] = 1.0;
        if(m_GlobalPos)
        {
          cloudAbs.MultiplyPoint(invec, outvec);
          for(unsigned indx = 0; indx < 3; indx++)
            invec[indx] = outvec[indx];
        }
        /*if(m_GlobalPos)
          cloud->GetLandmark(j)->GetOutput()->GetAbsPose(pos,ori,t);
        else
          cloud->GetLandmarkPosition(j, pos, t);*/
        out << lmName.GetCStr();
        int curL = lmName.Length();
        for (int i = 0; i < nmLength - curL; i++)
        {
          out << " ";
        }
        sprintf(numbs, " %16lf %16lf %16lf \n", invec[0], invec[1], invec[2]);
        out << numbs;
        //out << lmName << "\t" << invec[0] << "\t" << invec[1] << "\t" << invec[2] <<"\n";
        //sprintf(strng, "%s\t%.6f\t%.6f\t%.6f\n", j + 1, invec[0], invec[1], invec[2], 0.0, 0.0, 0.0);
        //out << strng;

        //if(indx != -1)
        //{
        //double xLandmark, yLandmark, zLandmark;
        //cloud->GetLandmark(j, xLandmark,yLandmark,zLandmark,timeStamps[index]);// landmark->GetPoint(xLandmark,yLandmark,zLandmark,timeStamps[index]);
        //landmark->GetPoint(xLandmark,yLandmark,zLandmark,timeStamps[index]);
        //}
      }
    }
  }
  else
  {
    char numbs[1000];
    t = 0.0;;
    out << "Time";
    int curL = 4;
    for (int i = 0; i < nmLength - curL; i++)
    {
      out << " ";
    }
    sprintf(numbs, " %16lf\n", t);
    out << numbs;
    for(int j=0; j < numberLandmark; j++)
    {
      lmName = cloud->GetLandmarkName(j);

      //if(!cloud->GetLandmarkVisibility(j, timeStamps[index]))
      //  continue;

      mafMatrix cloudAbs;
      double invec[4];
      double outvec[4];
      cloud->GetOutput()->GetAbsMatrix(cloudAbs);
      cloud->GetLandmark(j, invec);
      invec[3] = 1.0;
      if(m_GlobalPos)
      {
        cloudAbs.MultiplyPoint(invec, outvec);
        for(unsigned indx = 0; indx < 3; indx++)
          invec[indx] = outvec[indx];
      }
      /*if(m_GlobalPos)
      cloud->GetLandmark(j)->GetOutput()->GetAbsPose(pos,ori,t);
      else
      cloud->GetLandmarkPosition(j, pos, t);*/
      out << lmName.GetCStr();
      int curL = lmName.Length();
      for (int i = 0; i < nmLength - curL; i++)
      {
        out << " ";
      }
      sprintf(numbs, "% 16lf %16lf %16lf \n", invec[0], invec[1], invec[2]);
      out << numbs;
      //out << "   "<< invec[0] << "\t" << invec[1] << "\t" << invec[2] <<"\n";
      //sprintf(strng, "%s\t%.6f\t%.6f\t%.6f\n", j + 1, invec[0], invec[1], invec[2], 0.0, 0.0, 0.0);
      //out << strng;

      //if(indx != -1)
      //{
      //double xLandmark, yLandmark, zLandmark;
      //cloud->GetLandmark(j, xLandmark,yLandmark,zLandmark,timeStamps[index]);// landmark->GetPoint(xLandmark,yLandmark,zLandmark,timeStamps[index]);
      //landmark->GetPoint(xLandmark,yLandmark,zLandmark,timeStamps[index]);
      //}
    }

    /*for(int j=0; j < numberLandmark; j++)
    {
      char strng[256];

      mafMatrix cloudAbs;
      double invec[4];
      double outvec[4];
      cloud->GetOutput()->GetAbsMatrix(cloudAbs);
      cloud->GetLandmark(j, invec);
      invec[3] = 1.0;
      if(m_ABSPos)
      {
        cloudAbs.MultiplyPoint(invec, outvec);
        for(unsigned indx = 0; indx < 3; indx++)
          invec[indx] = outvec[indx];
      }
      sprintf(strng, "%d      %.6f      %.6f      %.6f      %.6f      %.6f      %.6f\n", j + 1, invec[0], invec[1], invec[2], 0.0, 0.0, 0.0);
      out << strng;
    }*/
  }

  // and now, close the cloud
  if(!initState)
    cloud->Close();
}

void medOpExporterLandmark::ExportingTraverse(std::ostream &out, const char *dirName, mafNode* node)
{
  if(node->IsMAFType(mafVMELandmarkCloud))
  {
    /*if(dirName == NULL)
      ExportOneCloud(out, mafVMELandmarkCloud::SafeDownCast(node));
    else*/
    {
      mafString intpath;
      mafString fn = _R(dirName);
      intpath = node->GetName();
      mafNode *nd = node;
      do 
      {
        nd = nd->GetParent();
        if(nd != NULL)
        {
          mafString tmp;
          tmp     = nd->GetName();
          tmp    += _R("_");
          tmp    += intpath;
          intpath = tmp;
        }
      }
      while(nd != NULL && nd!= m_Input);
      fn += _R("\\");
      fn += intpath;//node->GetName();
      /*if(node->GetParent() != NULL)
      {
      fn += "_";
      fn += node->GetParent()->GetName();
      }*/
      fn += _R(".txt");
      std::ofstream outF;
      outF.open(fn.GetCStr());
      ExportOneCloud(outF, mafVMELandmarkCloud::SafeDownCast(node));
      outF.close();
    }
  }
  int numberChildren = node->GetNumberOfChildren();
  for (int i= 0; i< numberChildren; i++)
  {
    mafNode *child = node->GetChild(i);
    ExportingTraverse(out, dirName, child);
  }
}
//----------------------------------------------------------------------------
void medOpExporterLandmark::ExportLandmark()
  //----------------------------------------------------------------------------
{
  wxBusyInfo *wait;
  if(!m_TestMode)
  {
    wait = new wxBusyInfo("Please wait, exporting...");
  }
  //file creation
  std::ofstream f_Out;

  if(m_Input->IsMAFType(mafVMELandmarkCloud))
  {
    f_Out.open(m_File.GetCStr());
    ExportOneCloud(f_Out, mafVMELandmarkCloud::SafeDownCast(m_Input));
    f_Out.close();
  }
  else
  {
    /*if(m_FileDir == "")
    {
      f_Out.open(fileName);
      ExportingTraverse(f_Out, NULL, m_Input);
      f_Out.close();
    }
    else*/
    {
      ExportingTraverse(f_Out, m_FileDir.GetCStr(), m_Input);
    }
  }
  if(!m_TestMode)
  {
    delete wait;
  }
}

//----------------------------------------------------------------------------
mafOp* medOpExporterLandmark::Copy()   
//----------------------------------------------------------------------------
{
  medOpExporterLandmark *cp = new medOpExporterLandmark(GetLabel());
  cp->m_Canundo      = m_Canundo;
  cp->m_OpType       = m_OpType;
  cp->SetListener(GetListener());
  cp->m_Next         = NULL;
  cp->m_File         = m_File;
  cp->m_Input        = m_Input;
  cp->m_FileDir      = m_FileDir;
  return cp;
}
