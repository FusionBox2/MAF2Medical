/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGUITransformSliders.h,v $
Language:  C++
Date:      $Date: 2009-09-14 07:03:53 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medGUITransformSliders_H__
#define __medGUITransformSliders_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafObserver.h"
#include "mafGUITransformInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;
class mafGUIButton;
class mmiGenericMouse;
class mmiCompositorMouse;
class mafInteractor;
class mafVME;
class mafMatrix;
class mafGUIFloatSlider;

//----------------------------------------------------------------------------
/** Class name: medGUITransformSliders.

A GUI component with sliders widgets for transform operations.
*/
class medGUITransformSliders : public mafGUITransformInterface
{
public:

  /** Constructor. */
  medGUITransformSliders(mafVME *input, double translationRange[6], mafObserver *listener = NULL, bool enableScaling = true);

  /** Destructor */
  ~medGUITransformSliders(); 

  /** Override superclass */
  void OnEvent(mafEventBase *maf_event);

  /** Enable-Disable the GUI's widgets */
  void EnableWidgets(bool enable);

  //----------------------------------------------------------------------------
  //gui constants: 
  //----------------------------------------------------------------------------

  // this constants must be visible from the owner object  
  enum TRANSFORMTEXTENTRIES_WIDGET_ID
  {
    ID_TRANSLATE_X = MINID,
    ID_TRANSLATE_Y,
    ID_TRANSLATE_Z,
    ID_ROTATE_X,
    ID_ROTATE_Y,
    ID_ROTATE_Z,
    ID_SCALE_X,
    ID_SCALE_Y,
    ID_SCALE_Z,
    ID_RESET,
  };

  /** Override superclass */
  void Reset();

  /** Set abs pose and update position, orientation and scale text entries according to current reference system. */
  void SetAbsPose(mafMatrix *absPose, mafTimeStamp timeStamp = -1);

protected:

  /** override superclass */
  void RefSysVmeChanged();  

  /** action to be performed when a slider has changed its position. */
  void SlidersValuesChanged();

  /** override superclass */ 
  void CreateGui();

  double m_Position[3];
  double m_Orientation[3];
  double m_Scaling[3];
  double m_TranslationRange[6];

  bool m_EnableScaling;

  mafMatrix *m_OldAbsMatrix;
};
#endif