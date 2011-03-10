#include "TC_PlotTool_api.h"

void (*_tc_surface)(tc_matrix z, const char* title) = 0;
/*!
 \brief plot 3D data. Input matrix has x,y on the first two columns and z on the third column
 \ingroup Plotting
*/ TCAPIEXPORT 
void tc_surface(tc_matrix z, const char* title)
{
	if (_tc_surface)
		_tc_surface(z,title);
}

void (*_tc_plot)(tc_matrix data, const char* title) = 0;
/*!
 \brief plot the data in the matrix (with headers) with the given x-axis and title
 \ingroup Plotting
*/ TCAPIEXPORT 
void tc_plot(tc_matrix data,const char* title)
{
	if (_tc_plot)
		_tc_plot(data,title);
}

void (*_tc_scatterplot)(tc_matrix data,const char* title) = 0;
/*!
 \brief plot the 2-columns in the matrix (with headers) as a scatter plot
 \ingroup Plotting
*/ TCAPIEXPORT 
void tc_scatterplot(tc_matrix data,const char* title)
{
	if (_tc_scatterplot)
		_tc_scatterplot(data,title);
}

void (*_tc_errorBars)(tc_matrix data,const char* title) = 0;
/*!
 \brief plot the data in the matrix (with headers) with the given x-axis and title. For each column i, the i+1 and i+2 columns should contain the upper and lower bounds (errors).
 \ingroup Plotting
*/ TCAPIEXPORT 
void tc_errorBars(tc_matrix data,const char* title)
{
	if (_tc_errorBars)
		_tc_errorBars(data,title);
}

void (*_tc_hist)(tc_matrix data,const char* title) = 0;
/*!
 \brief plot histogram for each column of the given matrix with the given bin size.
 \ingroup Plotting
*/ TCAPIEXPORT 
void tc_hist(tc_matrix data,const char* title)
{
	if (_tc_hist)
		_tc_hist(data,title);
}

void (*_tc_multiplot)(int r, int c) = 0;
/*!
 \brief enable multi-plot, i.e. multiple plots on one screen. specify the number of rows and columns for the layout.
 \ingroup Plotting
*/ TCAPIEXPORT 
void tc_multiplot(int rows, int cols)
{
	if (_tc_multiplot)
		_tc_multiplot(rows,cols);
}

void (*_tc_holdPlot)(int) = 0;
/*!
 \brief enable hold
 \ingroup Plotting
*/ TCAPIEXPORT 
void tc_holdPlot(int z)
{
	if (_tc_holdPlot)
		_tc_holdPlot(z);
}

void (*_tc_enableClustering)(int c) = 0;
/*!
 \brief enable clustering
 \ingroup Plotting
*/ TCAPIEXPORT 
void tc_enableClustering(int c)
{
	if (_tc_enableClustering)
		_tc_enableClustering(c);
}

tc_matrix (*_tc_getPlotData)(int whichPlot) = 0;
/*!
 \brief get the data that is currently in the plot window
 \ingroup Plotting
*/ TCAPIEXPORT 
tc_matrix tc_getPlotData(int whichPlot)
{
	tc_matrix M;
	if (_tc_getPlotData)
		return _tc_getPlotData(whichPlot);
	M.rows = M.cols = 0;
	M.colnames = tc_createStringsArray(0);
	M.rownames = tc_createStringsArray(0);
	M.values = 0;
	return M;
}


void (*_tc_gnuplot)(const char * ) = 0;
/*!
 \brief gnuplot
 \ingroup Plotting
*/ TCAPIEXPORT 
void tc_gnuplot(const char * s)
{
	if (_tc_gnuplot)
		_tc_gnuplot(s);
}

void (*_tc_savePlot)(const char * ) = 0;
/*!
 \brief save plot
 \ingroup Plotting
*/ TCAPIEXPORT 
void tc_savePlot(const char * filename)
{
	if (_tc_savePlot)
		_tc_savePlot(filename);
}

void (*_tc_setLogScale)(int) = 0;
/*!
 \brief save plot
 \ingroup Plotting
*/ TCAPIEXPORT 
void tc_setLogScale(int i)
{
	if (_tc_setLogScale)
		_tc_setLogScale(i);
}

/*!
 \brief initializing function
 \ingroup init
*/ TCAPIEXPORT 
void tc_PlotTool_api(
	void (*plot)(tc_matrix, const char*),
	void (*surface)(tc_matrix M, const char*),
	void (*hist)(tc_matrix data, const char* title),
	void (*errorBars)(tc_matrix data, const char* title),
	void (*scatterplot)(tc_matrix data, const char* title) ,
	void (*multiplot)(int r, int c),
	void (*hold)(int b),
	void (*enableClustering)(int c),
	tc_matrix (*plotData)(int),
	void (*gnuplot)(const char*),
	void (*savePlotImage)(const char *),
	void (*logscale)(int))
{
	_tc_plot = plot;
	_tc_getPlotData = plotData;
	_tc_surface = surface;
	_tc_hist = hist;
	_tc_errorBars = errorBars;
	_tc_scatterplot = scatterplot;
	_tc_multiplot = multiplot;
	_tc_holdPlot = hold;
	_tc_enableClustering = enableClustering;
	_tc_gnuplot = gnuplot;
	_tc_savePlot = savePlotImage;
	_tc_setLogScale = logscale;
}

