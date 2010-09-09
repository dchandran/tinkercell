/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.

****************************************************************************/

#ifndef TINKERCELL_PLOTTOOL_H
#define TINKERCELL_PLOTTOOL_H

#include <QtGui>
#include <QList>
#include <QTableWidget>
#include <QPainterPath>
#include <QMenu>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSplitter>
#include <QCheckBox>
#include <QSemaphore>
#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QButtonGroup>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QDockWidget>
#include "TCstructs.h"
#include "Tool.h"
#include "DataTable.h"
#include "qwt_plot.h"
#include "qwt_color_map.h"
#include "qwt_plot_marker.h"
#include "qwt_plot_curve.h"
#include "qwt_legend.h"
#include "qwt_data.h"
#include "qwt_text.h"
#include "qwt_plot_layout.h"
#include "qwt_plot_zoomer.h"
#include "qwt_legend_item.h"


namespace Tinkercell
{

	class PlotWidget;
	class PlotTool;

	class PlotTool_FtoS : public QObject
	{
		Q_OBJECT

		signals:

			void plotDataTable(QSemaphore*, DataTable<qreal>& m, int x, const QString& title, int all);
			void plotDataTable3D(QSemaphore*,DataTable<qreal>& m, const QString& title);
			void plotHist(QSemaphore*,DataTable<qreal>& m, double bins, const QString& title);
			void plotErrorbars(QSemaphore*,DataTable<qreal>& m, int x, const QString& title);
			void plotMultiplot(QSemaphore*,int x, int y);
			void getDataTable(QSemaphore*,DataTable<qreal>*, int index);
			void plotScatter(QSemaphore*,DataTable<qreal>&,const QString& title);
			void gnuplot(QSemaphore*,const QString& script);
			void savePlotImage(QSemaphore*,const QString& filename);

		private slots:

			void plotMatrix(tc_matrix m, int x, const char* title, int all);
			void plotMatrix3D(tc_matrix m, const char * title);
			void plotHistC(tc_matrix m, double bins, const char * title);
			void plotErrorbarsC(tc_matrix m, int x, const char* title);
			void plotMultiplotC(int x, int y);
			void plotScatterC(tc_matrix m, const char* title);
			tc_matrix getDataMatrix(int index);
			void gnuplot(const char * script);
			void savePlotImage(const char * filename);
			
			friend class PlotTool;
	};

	/*!
	\brief A docking widget that can contains one or more PlotWidget instances. 
		Each PlotWidget can either be a text output, 2D graph, or 3D graph. 
		Alternatively, the PlotTool can use an separate Gnuplot window to generate plots.
	\ingroup plugins
	*/
	class TINKERCELLEXPORT PlotTool : public Tool
	{
		Q_OBJECT
		
	signals:
	
		/*!	\brief plot  a 2D graph
		\param NumericalDataTable data
		\param int column for the x-axis
		\param QString title
		\param int(bool) whether or not to graph all the columns or just the handles that exist in the network
		*/
		void plotDataTable(DataTable<qreal>& m, int x, const QString& title, int all);
		
		/*!	\brief plot  a 3D graph
		\param NumericalDataTable data with 3 columns
		\param QString title
		*/
		void plotDataTable3D(DataTable<qreal>& m, const QString& title);
		
		/*!	\brief plot  a histogram
		\param NumericalDataTable data
		\param int number of bins
		\param QString title
		*/
		void plotHist(DataTable<qreal>& m, double bins, const QString& title);
		
		/*!	\brief plot  a 2D graph with error bars, where every alternating column are the errors
		\param NumericalDataTable data
		\param int index of x-axis
		\param QString title
		*/
		void plotErrorbars(DataTable<qreal>& m, int x, const QString& title);
		
		/*!	\brief enable multiple plots (grid)
		\param int number of rows of plots
		\param int number of columns of plots
		*/
		void plotMultiplot(int rows, int columns);
		
		/*!	\brief make a scatterplot
		\param NumericalDataTable data
		\param QString title
		*/
		void plotScatterplot(DataTable<qreal>& m, const QString& title);
		
		/*!\brief send a script to gnuplot
		\param QString gnuplot script
		*/
		void gnuplot(const QString& script);

	public:
	
		/*! \brief available plot types*/
		enum PlotType { Plot2D, SurfacePlot, HistogramPlot, ScatterPlot, Text };
	
		/*! \brief default constructor*/
		PlotTool();
		/*! \brief default size of this widget*/
		virtual QSize sizeHint() const;
		/*! \brief set Tinkercell main window*/
		virtual bool setMainWindow(MainWindow *);
		/*! \brief make this widget visible and on top*/
		virtual void setVisible ( bool visible );
		/*! \brief add a new plot to the window*/
		virtual void addWidget(PlotWidget*);
		/*! \brief show message at the bottom*/
		virtual void setStatusBarMessage(const QString&);
		/*! \brief add a dock widget to the plot area*/
		virtual QDockWidget * addDockWidget(const QString& title, QWidget * widget, Qt::DockWidgetArea area = Qt::BottomDockWidgetArea);

	private:
		QCheckBox * keepOldPlots, *holdCurrentPlot;

	public slots:
	
		/*! \brief hold current plot (don't close it)*/
		void hold(bool b = true);
		
		/*! \brief plot on top of current plot (if the feature is available for current plot)*/
		void overplot(bool b= true);

		/*! \brief graph the given data with headers
			\param DataTable<qreal> table
			\param QString title
			\param QString column in the table that will be used as x-axis
			\param int 0 or 1, indicating whether to plot only those items that are visible on the screen
		*/
		void plot(const DataTable<qreal>&,const QString& title,int xaxis=0,int all = 0, PlotType type = Plot2D);

		/*! \brief surface plot of the given data
			\param DataTable<qreal> table where value(x,y) is the z value
			\param QString title
			\param int 0 or 1, indicating whether to plot only those items that are visible on the screen
		*/
		void surfacePlot(const DataTable<qreal>& matrix, const QString& title);		

		/*! \brief add export option. This will add a new button to the set of export options.
			When user selects this option, the exportData method in the current PlotWidget
			will be invoked
			\param QIcon icon for the export opion
			\param QString name of the export option
		*/
		void addExportOption(const QIcon&,const QString&,const QString& toolTip=QString());

		/*! \brief export data in the given format
			\param QString format
		*/
		void exportData(const QString&);
    public:

        /*! \brief remove all items in the data table that are not visible in any scene
		*/
        static void pruneDataTable(DataTable<qreal>& table, int& xaxis, MainWindow* main);

	private:

		QDockWidget* dockWidget;
		QMainWindow * window;
		QMdiArea * multiplePlotsArea;
		QPlainTextEdit functionsTextEdit;
		QDoubleSpinBox spinBox1, spinBox2;
		QSpinBox spinBox3;
		QLineEdit xaxisLine;
		
		static PlotTool_FtoS fToS;

		 /*! \brief launch gnuplot and plot the given matrix*/
        static void plotMatrix(tc_matrix m, const char* title);

        /*! \brief launch gnuplot and plot the given surface matrix*/
        static void plotMatrix3D(tc_matrix m, const char * title);

        /*! \brief launch gnuplot and plot histogram of each column in the given matrix*/
        static void plotHistC(tc_matrix m, const char * title);

        /*! \brief launch gnuplot and plot each column with errors listed in the next 2 columns. So every 3rd column is the data.*/
        static void plotErrorbarsC(tc_matrix m, const char* title);
		
		/*! \brief rows and columns for multiple  plots*/
		static void plotMultiplotC(int x, int y);
		
        /*! \brief get plotted data*/
        static tc_matrix getDataMatrix(int index);
		
		/*! \brief scatterplot*/
		static void plotScatterC(tc_matrix data,const char* title);
		
		/*! \brief gnuplot*/
		static void _gnuplot(const char* script);
		
		/*! \brief save plotl*/
		static void _savePlotImage(const char* filename);

		friend class PlotWidget;
		QStringList exportOptions;
		QActionGroup actionGroup;
		QToolBar toolBar;
		QMenu * exportMenu;
		QToolBar * otherToolBar;
		int numMultiplots;

	private slots:
        //void toolAboutToBeLoaded( Tool * , bool * );
		void actionTriggered(QAction*);
		void subWindowActivated(QMdiSubWindow *);
		void setupFunctionPointers( QLibrary * );
		void plotData(QSemaphore*, DataTable<qreal>&,int,const QString&,int);
		void plotScatter(QSemaphore*, DataTable<qreal>&,const QString&);
		void surface(QSemaphore*, DataTable<qreal>&,const QString&);
		void getData(QSemaphore*, DataTable<qreal>*,int i);		
		void plotHist(QSemaphore*,DataTable<qreal>& m, double bins, const QString& title);
		void plotErrorbars(QSemaphore*,DataTable<qreal>& m, int x, const QString& title);
		void plotMultiplot(QSemaphore*,int x, int y);
		void gnuplot(QSemaphore * , const QString& script);
		void savePlotImage(QSemaphore*, const QString& filename);
	protected:
		virtual void keyPressEvent ( QKeyEvent * event );
		virtual void mouseMoveEvent ( QMouseEvent * event );
	};
}
#endif
