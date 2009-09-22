/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.
 
****************************************************************************/

#ifndef TINKERCELL_PLOT2DWIDGET_H
#define TINKERCELL_PLOT2DWIDGET_H

#include <QListWidget>
#include <QListWidgetItem>
#include <QPen>
#include "PlotWidget.h"
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
	class Plot2DWidget;
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
		friend class Plot2DWidget;
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
		QList<QPen>& pens();
		
	public slots:
		void setColors(const QList<QColor>&);
		
	protected:
		DataTable<qreal> dataTable;
		QwtPlotZoomer * zoomer;
		QStringList hideList;
		static QList<QColor> lineColors;
		int xcolumn, delta;
		
	protected slots:
		void itemChecked(QwtPlotItem *,	bool);
		void setXAxis(int);
		
		friend class Plot2DWidget;
	};

	/*!
	\brief A widget containing a data plot, legend and options
	*/
	class Plot2DWidget : public PlotWidget
	{
		Q_OBJECT
	public:
		Plot2DWidget(PlotTool * parent = 0);
		DataTable<qreal>* data();
		void plot(const DataTable<qreal>& matrix,const QString& title,int x=0);
		
	protected:
		DataPlot * dataPlot;
		QComboBox * axisNames;
		QListWidget * colorWidget;
		
	public slots:
		void copyData();
		void printToFile();
		void logX(bool);
		void logY(bool);
		void logAxis(int,bool);
		void setTitle();
		void setXLabel();
		void setYLabel();
		
		void printToFile(const QString&);
		void setTitle(const QString&);
		void setXLabel(const QString&);
		void setYLabel(const QString&);
	
	private slots:
		void selectColor(QListWidgetItem*);
	
	private:
		void setupColorWidget();
	};

}
#endif
