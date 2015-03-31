/*=========================================================================

 Program: MAF2Medical
 Module: medPipeGraph
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medPipeGraph_H__
#define __medPipeGraph_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medVMEDefines.h"
#include "mafPipeVTK.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class medVMEAnalog;
class vtkActor;
class mafGUICheckListBox;
class vtkLegendBoxActor;
class vtkXYPlotActor;
class vtkDoubleArray;
class vtkRectilinearGrid;


/** 
class name medPipeGraph.
Visual pipe to visualize graphs of analog signals. 
*/
class MED_VME_EXPORT medPipeGraph : public mafPipeVTK
{
public:
  /** RTTI Macro */
  mafTypeMacro(medPipeGraph,mafPipeVTK);
  /** Constructor. */
  medPipeGraph();
  /** Destructor. */
  virtual     ~medPipeGraph ();

  /** process events that comes from  other objects*/
  void medPipeGraph::OnEvent(mafEventBase *maf_event); 

  /** visual pipe creation */
  /*virtual*/ void Create(mafNode *node, mafView *view);

  /** Create plots of scalar data*/
  void UpdateGraph();

  /**Change the name of the selected item in the legend box*/
  void ChangeItemName();

  /**Change the title of the axis*/
  void ChangeAxisTitle();

  /**Change signal color*/
  void ChangeSignalColor();

  /** Set if visualize or not a particular signal */
  void SetSignalToPlot(int index,bool plot);

  /** set title on x axis */
  void SetTitleX(mafString title);
  /** set title on Y axis */
  void SetTitleY(mafString title);
  /** set title  */
  void SetTitle(mafString title);
  
protected:
  /** creation of the gui */
  mafGUI* CreateGui();

  enum PIPE_GRAPH_GUI_WIDGETS
  {
    ID_X_MIN = Superclass::ID_LAST, 
    ID_DRAW,
    ID_CHECK_BOX,
    ID_LEGEND,
    ID_ITEM_NAME,
    ID_AXIS_NAME_X,
    ID_AXIS_NAME_Y,
    ID_RANGE_X,
    ID_RANGE_Y,
    ID_FIT_PLOT,
    ID_SIGNALS_COLOR,
    ID_LAST
  };

  mafGUICheckListBox* m_CheckBox;
  vtkLegendBoxActor *m_LegendBox_Actor;
  vtkLegendBoxActor *m_LegendBoxTimeLine_Actor;

private:
  /**create the legend*/
  void CreateLegend();

  double m_OldColour[3];
  double m_ColorRGB[3];
  wxColor m_SignalColor;

  double      m_DataMax;
  double      m_DataMin;
  double      m_TimeStampMax;
  double      m_TimesRange[2];
  double      m_DataManualRange[2];
  double      m_TimesManualRange[2];
  int				m_Xlabel;
  int				m_Ylabel;
  int       m_NumberOfSignals;
  int       m_TimeStamp;
  long      m_ItemId;

  int m_Legend;
  int m_FitPlot;

  mafString   m_ItemName;
  mafString		m_TitileX;
  mafString		m_TitileY;
  mafString    m_Title;

  std::vector<vtkRectilinearGrid*> m_VtkData;
  std::vector<vtkDoubleArray*> m_ScalarArray;
  std::vector<bool> m_CheckedVector;
 
  vtkDoubleArray      *m_TimeArray;
  std::vector<mafTimeStamp> m_TimeVector;  

  medVMEAnalog   *m_EmgPlot;
  vtkXYPlotActor *m_PlotActor;
  vtkXYPlotActor *m_PlotTimeLineActor;

  vtkRectilinearGrid *m_TimeLine;
};  
#endif // __medPipeGraph_H__
