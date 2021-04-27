/*=========================================================================

 Program: MAF2Medical
 Module: medViewSliceBlendRX
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medViewSliceBlendRX_H__
#define __medViewSliceBlendRX_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medViewsDefines.h"
#include "mafViewCompound.h"
#include "mafSceneNode.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafViewRX;
class mafVMEVolume;
class vtkLookupTable;
class mafGUILutSwatch;
class mafGUILutSlider;
class medViewSliceBlend;
class mafGizmoSlice;

//----------------------------------------------------------------------------
// medViewSliceBlendRX :
//----------------------------------------------------------------------------
/** 
This view features one Rx views and one Slice Blend view.*/
class MED_VIEWS_EXPORT medViewSliceBlendRX: public mafViewCompound
{
public:
  
  /** constructor */
  medViewSliceBlendRX(const mafString&  label = _R("View Blend RX"));
  /** destructor */
  virtual ~medViewSliceBlendRX(); 
  /** RTTI macro */
  mafTypeMacro(medViewSliceBlendRX, mafViewCompound);

  /** clone the object*/
  /*virtual*/ mafView *Copy(mafBaseEventHandler *Listener, bool lightCopyEnabled = false);
  
  /** listen to other object events*/
  /*virtual*/ void OnEvent(mafEventBase *maf_event);
  
  /** Show/Hide VMEs into plugged sub-views*/
  /*virtual*/ void VmeShow(mafNode *node, bool show);

  /** Remove VME into plugged sub-views*/
  /*virtual*/ void VmeRemove(mafNode *node);

  /** Create visual pipe and initialize them to build an RXCT visualization */
  /*virtual*/ void PackageView();
  
  /** IDs for the GUI */
  enum VIEW_RXCT_WIDGET_ID
  {
    ID_LUT_WIDGET = Superclass::ID_LAST,
    ID_LAST
  };

  /** 
  Create the GUI on the bottom of the compounded view. */
  virtual void CreateGuiView();

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same panel GUI, each CreateGUI() function should first call the superclass' one.*/
  /*virtual*/ mafGUI  *CreateGui();

  /** 
  Redefine to arrange views to generate RXCT visualization.*/
  /*virtual*/ void LayoutSubViewCustom(int width, int height);

  /** 
  Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true);

  /** Create the gizmo to move the slices. */
  void GizmoCreate();

  /** Delete the gizmo. */
  void GizmoDelete();
  
  /** listen mouse events*/
  void OnEventMouseMove(mafEvent *e);

  /** check and correct gizmos positions using volume bounds  */
  void BoundsValidate(double *pos);

  mafVME *m_CurrentVolume; ///< Current visualized volume
  
  mafViewRX *m_ViewsRX;
  medViewSliceBlend *m_ViewSliceBlend;

  mafGizmoSlice *m_GizmoSlice[2];
  

  // this member variables are used by side panel gui view 
  std::vector<mafSceneNode*> m_CurrentSurface;

  mafGUI *m_BlendGui;
  mafGUI  *m_GuiViews;
  mafGUILutSlider *m_LutSliders;
  vtkLookupTable  *m_VtkLUT;  
  mafGUILutSwatch    *m_LutWidget;

  double m_BorderColor[2][3];
};
#endif
