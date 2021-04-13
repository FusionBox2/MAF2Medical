/*=========================================================================

 Program: MAF2Medical
 Module: medOpRegisterClusters
 Authors: Paolo Quadrani      - porting Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpRegisterClusters_H__
#define __medOpRegisterClusters_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "medOperationsDefines.h"
#include "mafOp.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEGroup.h"
#include "mafVMEInfoText.h"
#include "mafVMESurface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;
class mafEvent;
class vtkPoints;
class mafGUIDialog;

//----------------------------------------------------------------------------
// medOpRegisterClusters :
//----------------------------------------------------------------------------
/** */
class MED_OPERATION_EXPORT medOpRegisterClusters: public mafOp
{
public:
  medOpRegisterClusters(const mafString& label = _L("Register Landmark Cloud"));
 ~medOpRegisterClusters(); 
  virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(medOpRegisterClusters, mafOp);
  
  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* node);   

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

	static bool ClosedCloudAccept(mafNode* node) {if(node != NULL && node->IsA("mafVMELandmarkCloud") && !((mafVMELandmarkCloud*)node)->IsOpen())return true;return false;}

	static bool SurfaceAccept(mafNode* node) {if(node != NULL && node->IsMAFType(mafVMESurface)) return true;if(node != NULL && node->IsA("mafVMELandmarkCloud") && !((mafVMELandmarkCloud*)node)->IsOpen() && !((mafVMELandmarkCloud*)node)->IsAnimated())return true; return false;}
  // added by Losi on 31/01/2011 to allow test OpDo method
  void SetTarget(mafVMELandmarkCloud *target);
  void SetFollower(mafVMESurface *follower);
  inline mafVMEGroup *GetResult(){return m_Result;};

protected:
  /** Method called to extract matching point between source and target.*/
	int ExtractMatchingPoints(double time = -1);
  
	/** Register the source  on the target  according 
	to the registration method selected: rigid, similar or affine. */
	double RegisterPoints(double currTime = -1);

	/** Check the correctness of the vme's type. */
	void OnChooseSurfaceVme(mafNode *vme);

  /** Check the correctness of the vme's type. */
  void OnChooseTargetVme(mafNode *vme);

  /** Open and Close source and Target Clouds */
  void CloseClouds();
  void OpenClouds();

	mafVMELandmarkCloud*    m_Source;
	mafVMELandmarkCloud*    m_Target;
	mafVMELandmarkCloud*    m_Registered;
  mafVMEGroup        *    m_Result;
  mafVMEInfoText     *    m_Info;

	mafVMELandmarkCloud*    m_CommonPoints;
	double*					m_Weight;			 

	mafVME *m_Follower;
  mafGUI *m_GuiSetWeights;
  mafGUIDialog *m_Dialog;
  
	mafString   m_SourceName;
	mafString   m_TargetName;
	mafString   m_FollowerName;

  mafMatrix   m_FollowerMatrix;
  
	vtkPoints *m_PointsSource;
	vtkPoints *m_PointsTarget;
  
	int m_RegistrationMode; 
	int m_MultiTime;           
	int m_Apply;
  int m_SettingsGuiFlag;

 };
#endif
