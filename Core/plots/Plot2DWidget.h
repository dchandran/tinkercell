/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.
 
****************************************************************************/

#ifndef TINKERCELL_PLOT2DWIDGET_H
#define TINKERCELL_PLOT2DWIDGET_H

#include <QButtonGroup>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QColorDialog>
#include <QPen>
#include <QList>
#include <QColor>
#include <QDialog>
#include "PlotWidget.h"
#include "qwt_plot.h"
#include "qwt_color_map.h"
#include "qwt_plot_marker.h"
#include "qwt_plot_curve.h"
#include "qwt_legend.h"
#include "qwt_data.h"
#include "qwt_text.h"
#include "qwt_symbol.h"
#include "qwt_plot_layout.h"
#include "qwt_plot_zoomer.h"
#include "qwt_legend_item.h"
#include "qwt_scale_draw.h"

namespace Tinkercell
{

	class PlotTool;
	class PlotWidget;
	class Plot2DWidget;
	class DataPlot;
	class GetPenInfoDialog;
	class ShowHideLegendItemsWidget;
	class PlotCurve;

	/*!
	\brief This class represents the data for one curve in a Plot2DWidget graph
	\ingroup plotting
	*/
	class DataColumn : public QwtData
	{
	public:
		DataColumn(const NumericalDataTable * data, int,int,int dt=1);
		virtual QwtData * copy() const;
		virtual size_t size() const;
		virtual double x(size_t index) const;
		virtual double y(size_t index) const;
	private:
		const NumericalDataTable * dataTable;
		int column, xaxis, dt;
		
		friend class DataPlot;
		friend class Plot2DWidget;
		friend class PlotCurve;
	};
	
	/*!
	\brief This class represents a set of curves in a Plot2DWidget graph. However, the entire set is represented as a single plot item (i.e. one legend entry) in the main plot
	         The set of curves are plotted by pointing to different DataColumn objects and calling drawCurve again.
	\ingroup plotting
	*/
	class PlotCurve: public QwtPlotCurve
	{
	public:
		PlotCurve(const QString& title, DataPlot * dataplot, int xaxis, int index, int dt);
	protected:
		void drawCurve (QPainter *p, int style, const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const;
		void 	drawSymbols (QPainter *p, const QwtSymbol &, const QwtScaleMap &xMap, const QwtScaleMap &yMap, int from, int to) const;
		DataColumn dataColumn;
		DataPlot * dataPlot;
		
		friend class DataPlot;
		friend class Plot2DWidget;
		friend class DataColumn;
	};
	
	/*!
	\brief This class is used to draw the axis labels when the plot has text as axis labels
	\ingroup plotting
	*/
	class DataAxisLabelDraw : public QwtScaleDraw
	{
		public:
			DataAxisLabelDraw(const QStringList&);
			virtual QwtText label(double v) const;
			Qt::Orientation orientation() const;
		protected:
			QStringList labels;
	};
	
	/*!
	\brief This is the main plottigng widget. It is contained inside the Plot2DWidget. It uses PlotCurve to render the curves. 
		The "type" variable determines what symbols to use when plotting (e.g. scatterplot uses dots instead of continuous curves)
	\ingroup plotting
	*/
	class DataPlot : public QwtPlot
	{
		Q_OBJECT
	public:
		DataPlot(QWidget * parent = 0);
		void plot(const NumericalDataTable&,int x, const QString& title,  bool append = false);
		virtual QSize minimumSizeHint() const;
		virtual QSize sizeHint() const;
		virtual void setLogX(bool);
		virtual void setLogY(bool);
		
	protected:
		QList< NumericalDataTable* > dataTables;
		QwtPlotZoomer * zoomer;
		static QStringList hideList;
		static QList<QPen> penList;
		int xcolumn, numBars;
		PlotTool::PlotType type;
		void processData(NumericalDataTable *);
		void replotUsingHideList();
		bool usesRowNames() const;
		
	protected slots:
		void itemChecked(QwtPlotItem *,	bool);
		void setXAxis(int);
		
		friend class PlotCurve;
		friend class Plot2DWidget;
		friend class GetPenInfoDialog;
		friend class ShowHideLegendItemsWidget;
	};
	
	/*!
	\brief A widget that is used to set the pen color and size
	\ingroup plotting
	*/
	class GetPenInfoDialog : public QDialog
	{	
		Q_OBJECT
	public:
		GetPenInfoDialog(QWidget * parent);
		void setPen(const QPen&, int);
		QPen getPen() const;
		int currentIndex() const;
	private slots:
		void currentColorChanged ( const QColor & color );
	private:
		int index;
		QColor color;
		QColorDialog colorDialog;
		QDoubleSpinBox spinBox;
		QComboBox comboBox;
	};
	
	/*!
	\brief A widget that is used to select the curves to show/hide in all Plot2DWidgets
	\ingroup plotting
	*/
	class ShowHideLegendItemsWidget : public QDialog
	{
		Q_OBJECT
	public:
		ShowHideLegendItemsWidget(Plot2DWidget * plot);
		
	private slots:
		void updatePlot();
		void checkAll();
		void checkNone();
		
	private:
		Plot2DWidget * plotWidget;
		DataPlot * plot;
		QStringList names;
		QList<QCheckBox*> checkBoxes;
	};

	/*!
	\brief A widget containing a data plot, legend and options. Can be used to plot
		line-plots, scatterplots, bar-plots, or histograms
	\ingroup plotting
	*/
	class TINKERCELLEXPORT Plot2DWidget : public PlotWidget
	{
		Q_OBJECT
	
	signals:
	
		/*!	\brief show fire on the scene. only enabled in LabelingTool is enabled*/
		void displayFire(ItemHandle*, double);
		/*!	\brief hide fire on the scene. only enabled in LabelingTool is enabled*/
		void hideFire();
		
	public:
		/*! \brief Default constructor*/
		Plot2DWidget(PlotTool * parent = 0);
		/*! \brief Get the data in the plot. If the plot contains multiple layers, then the last plot data is returned*/
		virtual NumericalDataTable* data();
		/*! \brief Returns true because Plot2DWidget can append data to existing plots*/
		virtual bool canAppendData() const;
		/*! \brief Append new data to existing plot.  The new plot will contain all plots with the same columns in same color.
			    The new data must have the same column names*/
		virtual void appendData(const NumericalDataTable&, const QString& title, int x = 0);
		/*! \brief plot data with the given title and x-axis*/
		virtual void plot(const NumericalDataTable& matrix,const QString& title,int x=0);
		/*! \brief update the current plot's data with the new data, title and x-axis*/
		virtual void updateData(const NumericalDataTable&, const QString& title, int x = 0);
		
	public slots:
		/*! \brief set log scale for an axis*/
		void setLogScale(int index, bool set=true);
		/*! \brief print the current graph to file or other device*/
		void print(QPaintDevice&);
		/*! \brief export data. see PlotTool*/
		void exportData(const QString&, const QString &);
		/*! \brief set log scale for x*/
		void logX(bool);
		/*! \brief set log scale for y*/
		void logY(bool);
		/*! \brief set log scale for x or y*/
		void logAxis(int,bool);
		/*! \brief set title with popup asking for text*/
		void setTitle();
		/*! \brief set label for x-axis with popup asking for text*/
		void setXLabel();
		/*! \brief set label for y-axis with popup asking for text*/
		void setYLabel();
		/*! \brief set plot title*/
		void setTitle(const QString&);
		/*! \brief set label for x-axis*/
		void setXLabel(const QString&);
		/*! \brief set label for y-axis*/
		void setYLabel(const QString&);
		/*! \brief replot all the other Plot2DWidget that are visible in the PlotTool window*/
		void replotAllOther2DWidgets();
	
	private slots:
		void buttonPressed(int);
		void penSet();
		void legendConfigure();
		void mouseMoved(const QPoint&);
	
	private:
		void displayFire();
		QWidget * dialogWidget();
		GetPenInfoDialog * dialog;
		QString latex();
		DataPlot * dataPlot;
		QComboBox * axisNames;
		QComboBox * lineTypes;
		QButtonGroup buttonsGroup;
		
		friend class ShowHideLegendItemsWidget;
	};

}
#endif
