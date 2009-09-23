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
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSplitter>
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

namespace Tinkercell
{

	class PlotTool_FToS : public QObject
	{
		Q_OBJECT

		signals:

			void plot(QSemaphore*, DataTable<qreal>&,int,const QString&,int all);
			void surface(QSemaphore*, DataTable<qreal>&,double, double, double, double, const QString&);
			void plotData(QSemaphore*, DataTable<qreal>*,int);

		public slots:

			void plot(Matrix a0,int a1,const char*, int);
			void surface(Matrix a0, double, double, double, double, const char*);
			Matrix plotData(int);
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
		void plot3DSurface(const DataTable<qreal>& matrix, double xmin, double xmax, double ymin, double ymax, const QString& title);
		
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
		static void _surface(Matrix a, double, double, double, double, const char*);
		static Matrix _plotData(int);
		static PlotTool_FToS fToS;
		static void pruneDataTable(DataTable<qreal>& table, int& xaxis, MainWindow* main);

		friend class PlotWidget;
		QStringList exportOptions;
		QActionGroup actionGroup;
		QToolBar toolBar;
		QToolBar * otherToolBar;
		
	private slots:
		void actionTriggered(QAction*);
		void subWindowActivated(QMdiSubWindow *);
		void setupFunctionPointers( QLibrary * );
		void plotData(QSemaphore*, DataTable<qreal>&,int,const QString&,int);
		void surface(QSemaphore*, DataTable<qreal>&,double,double,double,double,const QString&);
		void getData(QSemaphore*, DataTable<qreal>*,int i = -1);
	};
}
#endif
