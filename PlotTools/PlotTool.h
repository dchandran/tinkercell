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
#include "PlotWidget.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	class PlotWidget;

	class MY_EXPORT PlotSignals : public QObject
	{
		Q_OBJECT

		signals:

			void gnuplotDataTable(QSemaphore*, DataTable<qreal>& m, int x, const QString& title, int all);
			void plotDataTable3D(QSemaphore*,DataTable<qreal>& m, const QString& title);
			void plotHist(QSemaphore*,DataTable<qreal>& m, double bins, const QString& title);
			void plotErrorbars(QSemaphore*,DataTable<qreal>& m, int x, const QString& title);
			void plotMultiplot(QSemaphore*,int x, int y);
			void getDataTable(QSemaphore*,DataTable<qreal>&, int index);

		private slots:

			void plotMatrix(Matrix m, int x, const char* title, int all);
			void plotMatrix3D(Matrix m, const char * title);
			void plotHistC(Matrix m, double bins, const char * title);
			void plotErrorbarsC(Matrix m, int x, const char* title);
			void plotMultiplot(int x, int y);
			Matrix getDataMatrix(int index);

	};

	/*!
	\brief A docking widget that contains many PlotWidget instances
	*/
	class PlotTool : public Tool
	{
		Q_OBJECT

	public:
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

		QCheckBox * keepOldPlots, *holdCurrentPlot;
		
	signals:
	
		void plotDataTable(QSemaphore*, DataTable<qreal>& m, int x, const QString& title, int all);
		void plotDataTable3D(QSemaphore*,DataTable<qreal>& m, const QString& title);
		void plotHist(QSemaphore*,DataTable<qreal>& m, double bins, const QString& title);
		void plotErrorbars(QSemaphore*,DataTable<qreal>& m, int x, const QString& title);
		void plotMultiplot(QSemaphore*,int x, int y);

	public slots:

		/*! \brief graph the given data with headers
			\param DataTable<qreal> table
			\param QString title
			\param QString column in the table that will be used as x-axis
			\param int 0 or 1, indicating whether to plot only those items that are visible on the screen
		*/
		void plot2D(const DataTable<qreal>&,const QString& = QString(),int xaxis=0,int all = 0);

		/*! \brief surface plot of the given data
			\param DataTable<qreal> table where value(x,y) is the z value
			\param double min x
			\param double max x
			\param double min y
			\param double max y
			\param QString title
			\param QString column in the table that will be used as x-axis
			\param int 0 or 1, indicating whether to plot only those items that are visible on the screen
		*/
		void plot3DSurface(const DataTable<qreal>& matrix, const QString& title);

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

		/*! \brief open the window for entering functions for plotting*/
		void plotFormula();

		/*! \brief plot the given set of formulas wih the given x variable
			\param QStringList list of functions
			\param QString x variable
			\param double start value for x
			\param double end value for x
		*/
		void plotFormula(const QStringList&,const QString& x = QString(),double start=0,double end=0, int points=100, const QString& title=QString());
    public:

        /*! \brief remove all items in the data table that are not visible in any scene
		*/
        static void pruneDataTable(DataTable<qreal>& table, int& xaxis, MainWindow* main);

	private:

		QDockWidget* dockWidget, *functionsWidgetDock;
		QMainWindow * window;
		QMdiArea * multiplePlotsArea;
		QPlainTextEdit functionsTextEdit;
		QDoubleSpinBox spinBox1, spinBox2;
		QSpinBox spinBox3;
		QLineEdit xaxisLine;

		void connectTCFunctions();
		static void _plot(Matrix a, int b, const char*, int);
		static void _surface(Matrix a, const char*);
		static Matrix _plotData(int);
		static PlotTool_FToS fToS;

		friend class PlotWidget;
		QStringList exportOptions;
		QActionGroup actionGroup;
		QToolBar toolBar;
		QMenu * exportMenu;
		QToolBar * otherToolBar;

	private slots:
        //void toolAboutToBeLoaded( Tool * , bool * );
		void actionTriggered(QAction*);
		void subWindowActivated(QMdiSubWindow *);
		void setupFunctionPointers( QLibrary * );
		void plotData(QSemaphore*, DataTable<qreal>&,int,const QString&,int);
		void surface(QSemaphore*, DataTable<qreal>&,const QString&);
		void getData(QSemaphore*, DataTable<qreal>*,int i = -1);

	protected:
		virtual void keyPressEvent ( QKeyEvent * event );
		virtual void mouseMoveEvent ( QMouseEvent * event );
	};
}
#endif
