/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medPipeTensorFieldSurface.h,v $ 
  Language: C++ 
  Date: $Date: 2009-06-12 16:34:48 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef medPipeTensorFieldSurface_h__
#define medPipeTensorFieldSurface_h__

#include "medPipeTensorField.h"

class mafGUI;

class vtkPolyDataMapper;
class vtkDataSet;
class vtkLookupTable;
class vtkActor;
class vtkScalarBarActor;

/** Displays the surface of input VME (even, if it is volume),
using color mapping according to X,Y,Z or magnitude of associated
tensor field (selected in the gui). */
class medPipeTensorFieldSurface : public medPipeTensorField
{
public:
  mafTypeMacro(medPipeTensorFieldSurface, medPipeTensorField);

protected:
  /** IDs for the GUI */
  enum PIPE_VIEWFLOW_WIDGET_ID
  {
    ID_VECTORFIELD = Superclass::ID_LAST,     
    ID_COLOR_MAPPING_MODE,
    ID_COLOR_MAPPING_LUT,
    ID_SHOW_COLOR_MAPPING,
    ID_LAST,
  };    

  enum COLOR_MAPPING_MODES
  {
    CMM_MAGNITUDE,
    CMM_COMPONENT1,
    CMM_COMPONENT2,
    CMM_COMPONENT3,
    CMM_COMPONENT4,
    CMM_COMPONENT5,
    CMM_COMPONENT6,
    CMM_COMPONENT7,
    CMM_COMPONENT8,
    CMM_COMPONENT9,
  };
  
  int m_ColorMappingMode;             ///<color mapping mode    
  vtkLookupTable* m_ColorMappingLUT;  ///<lookup table used for the mapping    
  int m_ShowMapping;                  ///<non-zero, if the mapping should be displayed in the main view
  
  vtkScalarBarActor* m_MappingActor;  ///<actor that displays the mapping bar

  vtkPolyDataMapper* m_SurfaceMapper;  ///<mapper for glyphs
  vtkActor* m_SurfaceActor;            ///<actor for glyphs  

  wxComboBox* m_comboColorBy;           ///<combo box with list of components
public:	
  medPipeTensorFieldSurface();
  virtual ~medPipeTensorFieldSurface();

public:  
  /** Processes events coming from GUI */
  /*virtual*/ void OnEvent(mafEventBase *maf_event);

protected:
  /*virtual*/ mafGUI  *CreateGui();

  /** Constructs VTK pipeline. */
  virtual void CreateVTKPipe();

  /** Updates VTK pipeline (setting radius, etc.). */
  virtual void UpdateVTKPipe();  

  /** Updates the content of m_comboColorBy combobox.
  "magnitude" and 0..NumberOfComponents-1 will be listed.*/
  virtual void UpdateColorByCombo();
};
#endif // medPipeTensorFieldSurface_h__