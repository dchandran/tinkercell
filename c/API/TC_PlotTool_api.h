#ifndef TINKERCELL_TC_PLOTTOOL_API_H
#define TINKERCELL_TC_PLOTTOOL_API_H

#include "../TCstructs.h"

/*! 
 \brief plot the data in the matrix (with headers) with the given x-axis and title
 \ingroup Plotting
*/
void (*tc_plot)(Matrix data,int xaxis,const char* title, int includeAll) = 0;
/*! 
 \brief get the data that is currently in the plot window
 \ingroup Plotting
*/
Matrix (*tc_plotData)(int whichPlot) = 0;
/*! 
 \brief initializing function
 \ingroup init
*/
void tc_PlotTool_api(void (*plot)(Matrix,int,const char*,int), Matrix (*plotData)(int))
{
	tc_plot = plot;
	tc_plotData = plotData;
}

#endif
