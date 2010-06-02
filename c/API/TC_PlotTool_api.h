#ifndef TINKERCELL_TC_PLOTTOOL_API_H
#define TINKERCELL_TC_PLOTTOOL_API_H

#include "../TCstructs.h"

/*!
 \brief plot 3D data. Input matrix has x,y on the first two columns and z on the third column
 \ingroup Plotting
*/
TCAPIEXPORT void tc_surface(Matrix z, String title);
/*!
 \brief plot the data in the matrix (with headers) with the given x-axis and title
 \ingroup Plotting
*/
TCAPIEXPORT void tc_plot(Matrix data,int xaxis,String title, int includeAll);
/*!
 \brief plot the 2-columns in the matrix (with headers) as a scatter plot
 \ingroup Plotting
*/
TCAPIEXPORT void tc_scatterplot(Matrix data,String title);
/*!
 \brief plot the data in the matrix (with headers) with the given x-axis and title. For each column i, the i+1 and i+2 columns should contain the upper and lower bounds (errors).
 \ingroup Plotting
*/
TCAPIEXPORT void tc_errorBars(Matrix data,int xaxis,String title, int includeAll);
/*!
 \brief plot histogram for each column of the given matrix with the given bin size.
 \ingroup Plotting
*/
TCAPIEXPORT void tc_hist(Matrix data,double binSize,String title);
/*!
 \brief enable multi-plot, i.e. multiple plots on one screen. specify the number of rows and columns for the layout.
 \ingroup Plotting
*/
TCAPIEXPORT void tc_multiplot(int rows, int cols);
/*!
 \brief get the data that is currently in the plot window
 \ingroup Plotting
*/
TCAPIEXPORT Matrix tc_getPlotData(int whichPlot);
/*!
 \brief plot the specific script using gnuplot
 \ingroup Plotting
*/
TCAPIEXPORT Matrix tc_gnuplot(const char*);
/*!
 \brief initializing function
 \ingroup init
*/
TCAPIEXPORT void tc_PlotTool_api(
	void (*plot)(Matrix,int,String,int),
	void (*surface)(Matrix M, String),
	void (*hist)(Matrix data,double bins,String title),
	void (*errorBars)(Matrix data,int xaxis,String title, int),
	void (*scatterplot)(Matrix data,String title) ,
	void (*multiplot)(int r, int c),
	Matrix (*plotData)(int)),
	void (*gnuplot)(const char*);

#endif
