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
		int meshSizeX;
		int meshSizeY;
		Plot3DWidget(PlotTool * parent = 0);
		DataTable<qreal>* data();
		void surface(const DataTable<qreal>& matrix,double xmin, double xmax, double ymin, double ymax,const QString& title=QString());
	
	public slots:
		
		void exportData(const QString&);
		
	private slots:
		void saveVector();
		void setTitle();
		void setXLabel();
		void setYLabel();
		void setZLabel();
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
		
		class Plot : public SurfacePlot
		{
			public:
				Plot();
		};
		
		static double ** tableToArray(const DataTable<qreal>&);
		Plot * surfacePlot;
		DataFunction * function;
	};

}
#endif
