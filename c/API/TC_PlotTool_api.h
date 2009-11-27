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

void (*_tc_errorBars)(Matrix data,int xaxis,const char* title, int includeAll) = 0;
/*! 
 \brief plot the data in the matrix (with headers) with the given x-axis and title. For each column i, the i+1 and i+2 columns should contain the upper and lower bounds (errors).
 \ingroup Plotting
*/
void tc_errorBars(Matrix data,int xaxis,const char* title, int includeAll)
{
	if (_tc_errorBars)
		_tc_errorBars(data,xaxis,title,includeAll);
}

void (*_tc_hist)(Matrix data,int bins,const char* title) = 0;
/*! 
 \brief plot histogram for each column of the given matrix with the given number of bins.
 \ingroup Plotting
*/
void tc_hist(Matrix data,int bins,const char* title)
{
	if (_tc_hist)
		_tc_hist(data,bins,title);
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
	void (*hist)(Matrix data,int bins,const char* title),
	void (*errorBars)(Matrix data,int xaxis,const char* title, int),
	Matrix (*plotData)(int))
{
	_tc_plot = plot;
	_tc_plotData = plotData;
	_tc_surface = surface;
	_tc_hist = hist;
	_tc_errorBars = errorBars;
}

#endif
