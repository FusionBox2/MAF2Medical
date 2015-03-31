/*=========================================================================

 Program: MAF2Medical
 Module: medGUITransformSliders
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medGUITransformSliders_H__
#define __medGUITransformSliders_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medInteractionDefines.h"
#include "mafBaseEventHandler.h"
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
class MED_INTERACTION_EXPORT medGUITransformSliders : public mafGUITransformInterface
{
public:

  /** Constructor. */
  medGUITransformSliders(mafVME *input, double translationRange[6], mafBaseEventHandler *listener = NULL, bool enableScaling = true,bool testMode = false);

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

  /** Return the current position */
  void GetPosition(double pos[3]);

  /** Return the current orientation */
  void GetOrientation(double orientation[3]);

  /** Return the current scaling */
  void GetScaling(double scaling[3]);

  /** Set the current position */
  void SetPosition(double pos[3]);

  /** Set the current orientation */
  void SetOrientation(double orientation[3]);

  /** Set the current scaling */
  void SetScaling(double scaling[3]);


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
