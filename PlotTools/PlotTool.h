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
#include <QButtonGroup>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPlainTextEdit>
#include <QLineEdit>
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
			void plotData(QSemaphore*, DataTable<qreal>*,int);
		public slots:
			void plot(Matrix a0,int a1,const char*,int);
			Matrix plotData(int);
	};

	/*!
	\brief A docking widget that contains many PlotWidget instances
	*/
	class PlotTool : public Tool
	{
		Q_OBJECT
		
	public:
		PlotTool();
		virtual QSize sizeHint() const;
		virtual bool setMainWindow(MainWindow *);
		virtual void setVisible ( bool visible );

	public slots:		
		
		void plot(const DataTable<qreal>&,const QString& = QString(),int xaxis=0,int all = 0);
		void plot(const QStringList& functions,const QString& xaxis,qreal start, qreal end, int points=100, const QString& title=QString());
		void plot3D(const DataTable<qreal>& matrix,const QString& title,int x=0,int y=1,int z=2, int meshX = 100, int meshY = 100);

	protected:
		QDockWidget* dockWidget;
		QMdiArea * multiplePlotsArea;
		
		QPlainTextEdit functionsTextEdit;
		QDoubleSpinBox spinBox1, spinBox2;
		QSpinBox spinBox3;
		QLineEdit xaxisLine;
		
		void connectTCFunctions();
		static void _plot(Matrix a, int b, const char*,int);
		static Matrix _plotData(int);
		static PlotTool_FToS fToS;
		static void pruneDataTable(DataTable<qreal>& table, int& xaxis, MainWindow* main);

	protected slots:
		void setupFunctionPointers( QLibrary * );
		void plotData(QSemaphore*, DataTable<qreal>&,int,const QString&,int);
		void getData(QSemaphore*, DataTable<qreal>*,int i = -1);
		void plotTexts();
		
		friend class PlotWidget;
	};
}
#endif
