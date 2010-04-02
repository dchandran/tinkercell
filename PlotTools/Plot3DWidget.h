/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.

****************************************************************************/

#ifndef TINKERCELL_PLOT3DWIDGET_H
#define TINKERCELL_PLOT3DWIDGET_H

#include "PlotWidget.h"
#include "qwt3d_surfaceplot.h"
#include "qwt3d_function.h"
#include "qwt3d_color.h"
#include "qwt3d_color.h"

using namespace Qwt3D;

namespace Tinkercell
{

	class PlotTool;
	class PlotWidget;

	/*!
	\brief A widget containing a data plot, legend and options
	*/
	class Plot3DWidget : public PlotWidget
	{
		Q_OBJECT

	public:
		static QColor DEFAULT_LOW_COLOR;
		static QColor DEFAULT_HIGH_COLOR;
		
		Plot3DWidget(PlotTool * parent = 0);
		DataTable<qreal>* data();
		void updateData(const DataTable<qreal>&);
		void surface(const DataTable<qreal>& matrix,const QString& title=QString());

	public slots:

		void exportData(const QString&);

	private slots:
		void saveVector();
		void setTitle();
		void setXLabel();
		void setYLabel();
		void setZLabel();
		void setLowColor();
		void setHighColor();
		virtual void setTitle(const QString&);
		virtual void setXLabel(const QString&);
		virtual void setYLabel(const QString&);
		virtual void setZLabel(const QString&);

	protected:

		DataTable<qreal> dataTable;

		class DataFunction : public Function
		{
			public:
				DataFunction(SurfacePlot&);
				DataTable<qreal> * dataTable;
				double operator()(double x, double y);
				double minX, minY, maxX, maxY;
		};

		class StandardColor : public Qwt3D::Color
		{
			public:
				StandardColor(double, const QColor&, double, const QColor&);
				Qwt3D::RGBA operator()(double x, double y, double z) const;
				Qwt3D::RGBA operator()(Qwt3D::Triple const &t) const;
				Qwt3D::ColorVector& createVector(Qwt3D::ColorVector& vec) { return vec; }
				QColor start, end;
				double minZ, maxZ;
		};

		class Plot : public SurfacePlot
		{
			public:
				Plot();
				void setColor();
				QString title;
				double minZ, maxZ;
				QColor minColor, maxColor;
		};

		static double ** tableToArray(const DataTable<qreal>&);
		Plot * surfacePlot;
		DataFunction * function;

	};

}
#endif
