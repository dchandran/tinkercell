/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.
 
****************************************************************************/

#include "OutputWindow.h"
#include "Plot3DWidget.h"

namespace Tinkercell
{
	using namespace Qwt3D;
	
	Plot3DWidget::Plot3DWidget(PlotTool * parent) : PlotWidget(parent), surfacePlot(), function(surfacePlot)
	{
		meshSizeX = meshSizeY = 100;
		QHBoxLayout * layout = new QHBoxLayout;
		layout->addWidget(&surfacePlot);
		setLayout(layout);
	}
	
	void Plot3DWidget::plot(const DataTable<qreal>& data,const QString& title,int)
	{
		OutputWindow::message("here 1");
		dataTable = data;
		
		double minZ = dataTable.value(0,0);
		double maxZ = dataTable.value(0,0);
		
		for (int i=0; i < dataTable.rows(); ++i)
			for (int j=0; j < dataTable.cols(); ++j)
			{
				if (dataTable.value(i,j) < minZ)
					minZ = dataTable.value(i,j);
				if (dataTable.value(i,j) > maxZ)
					maxZ = dataTable.value(i,j);
			}
		
		function.setMesh(meshSizeX, meshSizeY);
		
		double minX = dataTable.value(1,0),
				minY = dataTable.value(0,1),
				maxX = dataTable.value(dataTable.rows()-1,0),
				maxY = dataTable.value(0,dataTable.cols()-1);
		
		function.minX = minX;
		function.minY = minY;
		function.maxX = maxX;
		function.maxY = maxY;
		
		function.setDomain(minX,maxX,minY,maxY);
		function.setMinZ(minZ);
		function.setMaxZ(maxZ);
		function.create();
		
		for (int i=0; i < surfacePlot.coordinates()->axes.size(); ++i)
		{
		  surfacePlot.coordinates()->axes[i].setMajors(7);
		  surfacePlot.coordinates()->axes[i].setMinors(4);
		}
		
		function.dataTable = &dataTable;
		
		setTitle(title);
		setXLabel(dataTable.rowName(0));
		setYLabel(dataTable.rowName(1));
		setZLabel(dataTable.rowName(2));
		
		OutputWindow::message( tr("minX = ") + QString::number(minX) +
								tr("maxX = ") + QString::number(maxX) +
								tr("minY = ") + QString::number(minY) +
								tr("maxY = ") + QString::number(maxY) +
								tr("minZ = ") + QString::number(minZ) +
								tr("maxZ = ") + QString::number(maxZ) );
		
		surfacePlot.updateData();
		surfacePlot.updateGL();
	}
	
	DataTable<qreal>* Plot3DWidget::data()
	{
		return &dataTable;
	}
	
    Plot3DWidget::DataFunction::DataFunction(SurfacePlot& pw) :Function(pw), dataTable(0)
    {
    }

    double Plot3DWidget::DataFunction::operator()(double x, double y)
    {
		if (dataTable)
		{
			int r = dataTable->rows()-1, 
				c = dataTable->cols()-1;
			
			int i = (int)(r * (x-minX) / maxX) +1,
				j = (int)(c * (y-minY) / maxY) +1;
				
			OutputWindow::message(QString::number(x) + tr(" -> ") + QString::number(i) +
							QString::number(y) + tr(" -> ") + QString::number(j));
				
			if (i >= 0 && j >= 0 && i < dataTable->rows() && j < dataTable->cols())
				return dataTable->value(i,j);
		}
		return 0.0;
    }
  
	Plot3DWidget::Plot::Plot()
	{
		setTitle("Plot");
		
		setRotation(30,0,15);
		setScale(1,1,1);
		setShift(0.15,0,0);
		setZoom(0.9);
		
		coordinates()->axes[X1].setLabelString("x-axis");
		coordinates()->axes[Y1].setLabelString("y-axis");
		coordinates()->axes[Z1].setLabelString(QChar(0x38f)); // Omega - see http://www.unicode.org/charts/

		setCoordinateStyle(BOX);

		updateData();
		updateGL();
	}
	
	void Plot3DWidget::copyData()
	{
		QClipboard * clipboard = QApplication::clipboard();
		
		if (!clipboard)
		{
			OutputWindow::error(tr("No clipboard available."));
		}
		
		QString outputs;
		
		DataTable<qreal> & table = dataTable;
		
		QStringList colnames = table.getColNames(), rownames = table.getRowNames();
		
		for (int i=0; i < table.rows(); ++i)
		{
			for (int j=0; j < table.cols(); ++j)
			{
				if (i == 0 && j == 0) continue;
				
				if (j > 0)
					outputs += tr("\t") + QString::number(table.at(i,j));
				else
					outputs += QString::number(table.at(i,j));
			}
			outputs += tr("\n");
		}
		
		clipboard->setText(outputs);
		
		OutputWindow::message(tr("Tab-delimited data copied to clipboard."));
	}
	
	void Plot3DWidget::printToFile(const QString& fileName)
	{
		surfacePlot.save(fileName, tr("PDF"));
	}
	
	void Plot3DWidget::setTitle(const QString& s)
	{
		surfacePlot.setTitle(s);
	}
	
	void Plot3DWidget::setXLabel(const QString& s)
	{
		surfacePlot.coordinates()->axes[X1].setLabelString(s);
	}
	
	void Plot3DWidget::setYLabel(const QString& s)
	{
		surfacePlot.coordinates()->axes[Y1].setLabelString(s);
	}
	
	void Plot3DWidget::setZLabel(const QString& s)
	{
		surfacePlot.coordinates()->axes[Z1].setLabelString(s);
	}

}
