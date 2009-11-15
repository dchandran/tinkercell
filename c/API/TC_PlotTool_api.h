#ifndef TINKERCELL_TC_PLOTTOOL_API_H
#define TINKERCELL_TC_PLOTTOOL_API_H

#include "../TCstructs.h"

void (*_tc_surface)(Matrix z, double xmin,double xmax,double ymin,double ymax, const char* title) = 0;
/*! 
 \brief plot 3D data. Input matrix will contain the z-value at position (x,y).
 \ingroup Plotting
*/
void tc_surface(Matrix z, double xmin,double xmax,double ymin,double ymax, const char* title)
{
	if (_tc_surface)
		_tc_surface(z,xmin,xmax,ymin,ymax,title);
}

void (*_tc_plot)(Matrix data,int xaxis,const char* title, int includeAll) = 0;
/*! 
 \brief plot the data in the matrix (with headers) with the given x-axis and title
 \ingroup Plotting
*/
void tc_plot(Matrix data,int xaxis,const char* title, int includeAll)
{
	if (_tc_plot)
		_tc_plot(data,xaxis,title,includeAll);
}

Matrix (*_tc_plotData)(int whichPlot) = 0;
/*! 
 \brief get the data that is currently in the plot window
 \ingroup Plotting
*/
Matrix tc_plotData(int whichPlot)
{
	if (_tc_plotData)
		return _tc_plotData(whichPlot);
	Matrix M;
	M.rows = M.cols = 0;
	M.colnames = M.rownames = 0;
	M.values = 0;
	return M;
}

/*! 
 \brief initializing function
 \ingroup init
*/
void tc_PlotTool_api(
	void (*plot)(Matrix,int,const char*,int), 
	void (*surface)(Matrix M, double,double,double,double, const char*),
	Matrix (*plotData)(int))
{
	_tc_plot = plot;
	_tc_plotData = plotData;
	_tc_surface = surface;
}

#endif
