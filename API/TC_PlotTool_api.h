#ifndef TINKERCELL_TC_PLOTTOOL_API_H
#define TINKERCELL_TC_PLOTTOOL_API_H

#include "TC_structs.h"
BEGIN_C_DECLS

/*!
 \brief plot 3D data. Input matrix has x,y on the first two columns and z on the third column
 \param tc_matrix tree column matrix
 \param string title of plot
 \ingroup Plotting
*/
TCAPIEXPORT void tc_surface(tc_matrix z, const char* title);
/*!
 \brief plot the data in the matrix (with headers) with the given x-axis and title
 \param tc_matrix data with first column being the x-axis
 \param string title of plot
 \ingroup Plotting
*/
TCAPIEXPORT void tc_plot(tc_matrix data,const char* title);
/*!
 \brief plot the data in the matrix (with headers) as a scatter plot
 \param tc_matrix data with first column as x-axis
 \param string title of plot
 \ingroup Plotting
*/
TCAPIEXPORT void tc_scatterplot(tc_matrix data,const char* title);
/*!
 \brief plot the data in the matrix (with headers) with the given x-axis and title. For each column i, the i+1 and i+2 columns should contain the upper and lower bounds (errors).
 \param tc_matrix data
 \param string title of plot
 \ingroup Plotting
*/
TCAPIEXPORT void tc_errorBars(tc_matrix data,const char* title);
/*!
 \brief plot histogram for each column of the given matrix with the given bin size.
 \param tc_matrix data
 \param string title of plot
 \ingroup Plotting
*/
TCAPIEXPORT void tc_hist(tc_matrix data,const char* title);
/*!
 \brief enable multi-plot, i.e. multiple plots on one screen. specify the number of rows and columns for the layout.
 \param int number of rows
 \param int number of columns
 \ingroup Plotting
*/
TCAPIEXPORT void tc_multiplot(int rows, int cols);
/*!
 \brief hold current plot and plot on top of it
 \param int on(1) or off (0)
 \ingroup Plotting
*/
TCAPIEXPORT void tc_holdPlot(int on);
/*!
 \brief enable clustering, i.e. as new plots are generated, they will be clustered and each cluster will be displayed separately. Use 0 or 1 as the argument to disable clustering.
 \param int number of clusters (max 10). Use 0 or 1 to disable.
 \ingroup Plotting
*/
TCAPIEXPORT void tc_enableClustering(int clusters);
/*!
 \brief get the data in the plot window
 \param int index of the plot (if multiple plots are being displayed)
 \return tc_matrix data
 \ingroup Plotting
*/
TCAPIEXPORT tc_matrix tc_getPlotData(int whichPlot);
/*!
 \brief plot the specific script using gnuplot
 \param string gnuplot commands
 \ingroup Plotting
*/
TCAPIEXPORT void tc_gnuplot(const char*);
/*!
 \brief save the current plot as a PDF file
 \param string filename (PDF suffix)
 \ingroup Plotting
*/
TCAPIEXPORT void tc_savePlot(const char * filename);
/*!
 \brief set log scale for current plot; argument: 0=x-axis, 1=y-axis, 2=both
 \param int 0=x-axis, 1=y-axis, 2=both
 \ingroup Plotting
*/
TCAPIEXPORT void tc_setLogScale(int);
/*!
 \brief initialize plot plug-in
 \ingroup init
*/
TCAPIEXPORT void tc_PlotTool_api(
	void (*plot)(tc_matrix,const char*),
	void (*surface)(tc_matrix, const char*),
	void (*hist)(tc_matrix,const char*),
	void (*errorBars)(tc_matrix,const char*),
	void (*scatterplot)(tc_matrix,const char*) ,
	void (*multiplot)(int, int),
	void (*hold)(int),
	void (*enableClustering)(int),
	tc_matrix (*plotData)(int),
	void (*gnuplot)(const char*),
	void (*savePlotImage)(const char *),
	void (*logscale)(int));

END_C_DECLS
#endif

