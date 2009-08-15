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
	
	Plot3DWidget::Plot3DWidget(PlotTool * parent) : PlotWidget(parent), surfacePlot(0), function(0)
	{
		meshSizeX = meshSizeY = 100;
		QHBoxLayout * layout = new QHBoxLayout;
		surfacePlot = new Plot();
		layout->addWidget(surfacePlot);
		setLayout(layout);
	}
	
	double ** Plot3DWidget::tableToArray(const DataTable<qreal>& table)
	{
		double ** array = new double*[table.rows()];
		for (int i=0; i < table.rows(); ++i)
		{
			array[i] = new double[table.cols()];
			for (int j=0; j < table.cols(); ++j)
				array[i][j] = table.at(i,j);
		}
		return array;
	}
	
	void Plot3DWidget::plot(const DataTable<qreal>& data,const QString& title,int)
	{
		if (!surfacePlot) return;
		
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
		
		
		/*double  minX = dataTable.value(1,0),
				minY = dataTable.value(0,1),
				maxX = dataTable.value(dataTable.rows()-1,0),
				maxY = dataTable.value(0,dataTable.cols()-1);*/
		
		double  minX = 0,
				minY = 0,
				maxX = dataTable.rows(),
				maxY = dataTable.cols();
		
		for (int i=0; i < surfacePlot->coordinates()->axes.size(); ++i)
		{
			//surfacePlot->coordinates()->axes[i].setAutoScale(true);
			surfacePlot->coordinates()->axes[i].setMajors(5);
			surfacePlot->coordinates()->axes[i].setMinors(2);
		}
		
		surfacePlot->loadFromData(tableToArray(dataTable),dataTable.cols(),dataTable.rows(),minX,maxX,minY,maxY);
		
		setTitle(title);
		
		double max = maxZ;
		if (maxX > max) max = maxX;
		if (maxY > max) max = maxY;
		
		surfacePlot->setScale(max/maxX,max/maxY,max/maxZ);
		setXLabel(dataTable.colName(0));
		setYLabel(dataTable.colName(1));
		setZLabel(dataTable.colName(2));
		
		surfacePlot->setCoordinateStyle(BOX);
		
		surfacePlot->updateData();
		surfacePlot->updateGL();
	}
	
	DataTable<qreal>* Plot3DWidget::data()
	{
		return &dataTable;
	}
	
    Plot3DWidget::DataFunction::DataFunction(SurfacePlot& pw) :Function(), dataTable(0)
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
		
		coordinates()->axes[X1].setLabelString("x");
		coordinates()->axes[Y1].setLabelString("y");
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
		if (surfacePlot)
			surfacePlot->save(fileName, tr("PDF"));
	}
	
	void Plot3DWidget::setTitle(const QString& s)
	{
		if (surfacePlot)
			surfacePlot->setTitle(s);
	}
	
	void Plot3DWidget::setXLabel(const QString& s)
	{
		if (surfacePlot)
			surfacePlot->coordinates()->axes[X1].setLabelString(s);
	}
	
	void Plot3DWidget::setYLabel(const QString& s)
	{
		if (surfacePlot)
			surfacePlot->coordinates()->axes[Y1].setLabelString(s);
	}
	
	void Plot3DWidget::setZLabel(const QString& s)
	{
		if (surfacePlot)
			surfacePlot->coordinates()->axes[Z1].setLabelString(s);
	}

}
