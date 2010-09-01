#include "TC_PlotTool_api.h"

void (*_tc_surface)(TableOfReals z, String title) = 0;
/*!
 \brief plot 3D data. Input matrix has x,y on the first two columns and z on the third column
 \ingroup Plotting
*/
void tc_surface(TableOfReals z, String title)
{
	if (_tc_surface)
		_tc_surface(z,title);
}

void (*_tc_plot)(TableOfReals data, String title) = 0;
/*!
 \brief plot the data in the matrix (with headers) with the given x-axis and title
 \ingroup Plotting
*/
void tc_plot(TableOfReals data,String title)
{
	if (_tc_plot)
		_tc_plot(data,title);
}

void (*_tc_scatterplot)(TableOfReals data,String title) = 0;
/*!
 \brief plot the 2-columns in the matrix (with headers) as a scatter plot
 \ingroup Plotting
*/
void tc_scatterplot(TableOfReals data,String title)
{
	if (_tc_scatterplot)
		_tc_scatterplot(data,title);
}

void (*_tc_errorBars)(TableOfReals data,String title) = 0;
/*!
 \brief plot the data in the matrix (with headers) with the given x-axis and title. For each column i, the i+1 and i+2 columns should contain the upper and lower bounds (errors).
 \ingroup Plotting
*/
void tc_errorBars(TableOfReals data,String title)
{
	if (_tc_errorBars)
		_tc_errorBars(data,title);
}

void (*_tc_hist)(TableOfReals data,String title) = 0;
/*!
 \brief plot histogram for each column of the given matrix with the given bin size.
 \ingroup Plotting
*/
void tc_hist(TableOfReals data,String title)
{
	if (_tc_hist)
		_tc_hist(data,title);
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

TableOfReals (*_tc_getPlotData)(int whichPlot) = 0;
/*!
 \brief get the data that is currently in the plot window
 \ingroup Plotting
*/
TableOfReals tc_getPlotData(int whichPlot)
{
	TableOfReals M;
	if (_tc_getPlotData)
		return _tc_getPlotData(whichPlot);
	M.rows = M.cols = 0;
	M.colnames = newArrayOfStrings(0);
	M.rownames = newArrayOfStrings(0);
	M.values = 0;
	return M;
}


void (*_tc_gnuplot)(const char * ) = 0;
/*!
 \brief gnuplot
 \ingroup Plotting
*/
void tc_gnuplot(const char * s)
{
	if (_tc_gnuplot)
		_tc_gnuplot(s);
}

/*!
 \brief initializing function
 \ingroup init
*/
void tc_PlotTool_api(
	void (*plot)(TableOfReals, String),
	void (*surface)(TableOfReals M, String),
	void (*hist)(TableOfReals data, String title),
	void (*errorBars)(TableOfReals data, String title),
	void (*scatterplot)(TableOfReals data, String title) ,
	void (*multiplot)(int r, int c),
	TableOfReals (*plotData)(int),
	void (*gnuplot)(const char*))
{
	_tc_plot = plot;
	_tc_getPlotData = plotData;
	_tc_surface = surface;
	_tc_hist = hist;
	_tc_errorBars = errorBars;
	_tc_scatterplot = scatterplot;
	_tc_multiplot = multiplot;
	_tc_gnuplot = gnuplot;
}

