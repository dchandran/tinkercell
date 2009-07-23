/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.
 
****************************************************************************/

#ifndef TINKERCELL_PLOTWIDGET_H
#define TINKERCELL_PLOTWIDGET_H

#include <QtGui>
#include <QList>
#include <QStringList>
#include <QString>
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
#include <QButtonGroup>
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

	class PlotTool;
	class PlotWidget;
	class DataPlot;

	class DataColumn : public QwtData
	{
	public:
		DataColumn(DataTable<qreal>* data, int,int,int dt=1);
		virtual QwtData * copy() const;
		virtual size_t size() const;
		virtual double x(size_t index) const;
		virtual double y(size_t index) const;
	private:
		DataTable<qreal> * dataTable;
		int column, xaxis, dt;
		
		friend class DataPlot;
		friend class PlotWidget;
	};
	
	class DataPlot : public QwtPlot
	{
		Q_OBJECT
	public:
		DataPlot(QWidget * parent = 0);
		void plot(const DataTable<qreal>&,int,const QString&,int dt=1);
		virtual QSize minimumSizeHint() const;
		virtual QSize sizeHint() const;
		virtual void setLogX(bool);
		virtual void setLogY(bool);
		void replotUsingHideList();
		DataTable<qreal>& data();
	protected:
		DataTable<qreal> dataTable;
		QwtPlotZoomer * zoomer;
		QStringList hideList;
	protected slots:
		void itemChecked(QwtPlotItem *,	bool);
		void setXAxis(int);
	};
	
	class PlotTool;

	/*!
	\brief A widget containing a data plot, legend and options
	*/
	class PlotWidget : public QWidget
	{
		Q_OBJECT
	public:
		PlotWidget(PlotTool * parent = 0);
		DataTable<qreal>* data();
		void plot(const DataTable<qreal>& matrix,const QString& title,int x);
	protected:
		DataPlot * dataPlot;
		QComboBox * axisNames;
		PlotTool * plotTool;
	protected slots:
		void copyData();
		void printToFile();
		void logX(bool);
		void logY(bool);
		void setTitle();
		void setXLabel();
		void setYLabel();
	};

}
#endif
