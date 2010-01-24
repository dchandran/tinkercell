#ifndef TINKERCELL_TC_PLOTTOOL_API_H
#define TINKERCELL_TC_PLOTTOOL_API_H

#include "../TCstructs.h"

void (*_tc_surface)(Matrix z, const char* title) = 0;
/*!
 \brief plot 3D data. Input matrix has x,y on the first two columns and z on the third column
 \ingroup Plotting
*/
void tc_surface(Matrix z, const char* title)
{
	if (_tc_surface)
		_tc_surface(z,title);
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

void (*_tc_hist)(Matrix data,double binSize,const char* title) = 0;
/*!
 \brief plot histogram for each column of the given matrix with the given bin size.
 \ingroup Plotting
*/
void tc_hist(Matrix data,double binSize,const char* title)
{
	if (_tc_hist)
		_tc_hist(data,binSize,title);
}

void (*_tc_multiplot)(int r, int c) = 0;
/*!
 \brief enable multi-plot, i.e. multiple plots on one screen. specify the number of rows and columns for the layout.
 \ingroup Plotting
*/
void tc_multiplot(int rows, int cols)
{
	if (_tc_multiplot)
		_tc_multiplot(rows,cols);
}

Matrix (*_tc_getPlotData)(int whichPlot) = 0;
/*!
 \brief get the data that is currently in the plot window
 \ingroup Plotting
*/
Matrix tc_getPlotData(int whichPlot)
{
	Matrix M;
	if (_tc_getPlotData)
		return _tc_getPlotData(whichPlot);
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
	void (*surface)(Matrix M, const char*),
	void (*hist)(Matrix data,double bins,const char* title),
	void (*errorBars)(Matrix data,int xaxis,const char* title, int),
	void (*multiplot)(int r, int c),
	Matrix (*plotData)(int))
{
	_tc_plot = plot;
	_tc_getPlotData = plotData;
	_tc_surface = surface;
	_tc_hist = hist;
	_tc_errorBars = errorBars;
	_tc_multiplot = multiplot;
}

#endif
