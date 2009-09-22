/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.
 
****************************************************************************/

#include "ConsoleWindow.h"
#include "Plot3DWidget.h"

namespace Tinkercell
{
	using namespace Qwt3D;
	
	Plot3DWidget::Plot3DWidget(PlotTool * parent) : PlotWidget(parent), surfacePlot(0), function(0)
	{
		meshSizeX = meshSizeY = 100;
		
		QVBoxLayout * layout = new QVBoxLayout;
		
		surfacePlot = new Plot();
		layout->addWidget(surfacePlot,10);
		
		setPalette(QPalette(QColor(255,255,255,255)));
		setAutoFillBackground(true);
		
		QToolButton * copy = new QToolButton(this);
		copy->setIcon(QIcon(":/images/copy.png"));
		copy->setToolTip(tr("Copy tab-delimited data to clipboard"));
		copy->setText(tr("Copy data"));
		copy->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		
		QToolButton * setLabels = new QToolButton(this);
		setLabels->setIcon(QIcon(":/images/text.png"));
		setLabels->setText(tr("Labels"));
		setLabels->setToolTip(tr("Change title and axis labels"));
		QMenu * labelsMenu = new QMenu(tr("Set labels"),setLabels);
		labelsMenu->addAction(tr("Title"),this,SLOT(setTitle()));
		labelsMenu->addAction(tr("x label"),this,SLOT(setXLabel()));
		labelsMenu->addAction(tr("y label"),this,SLOT(setYLabel()));
		labelsMenu->addAction(tr("Z label"),this,SLOT(setZLabel()));
		setLabels->setMenu(labelsMenu);
		setLabels->setPopupMode ( QToolButton::MenuButtonPopup );
		setLabels->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		
		QToolButton * print = new QToolButton(this);
		print->setIcon(QIcon(":/images/print.png"));
		print->setText(tr("Print to file"));
		print->setToolTip(tr("Print graph to pixel or vector file"));
		print->setPopupMode ( QToolButton::MenuButtonPopup );
		print->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		
		QMenu * printMenu = new QMenu(tr("save to file"),print);
		print->setMenu(printMenu);
		
		printMenu->addAction(tr("portable network graphics (PNG)"),this,SLOT(savePixmap()));
		printMenu->addAction(tr("post script (PS)"),this,SLOT(saveVector()));
		connect(copy,SIGNAL(pressed()),this,SLOT(copyData()));
		
		QHBoxLayout * layout2 = new QHBoxLayout;
		layout2->addWidget(print);
		layout2->addWidget(copy);
		layout2->addWidget(setLabels);
		
		QGroupBox * groupBox2 = new QGroupBox(tr(" options "));
		groupBox2->setLayout(layout2);
		
		QHBoxLayout * layout3 = new QHBoxLayout;
		layout3->addStretch(2);
		layout3->addWidget(groupBox2,1,Qt::AlignRight);
		layout->addLayout(layout3,1);
		
		setLayout(layout);
		setMinimumHeight(200);
		
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
	
	void Plot3DWidget::surface(const DataTable<qreal>& data,const QString& title,int minX, int maxX, int minY, int maxY)
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
		
		if (minX >= maxX)
		{
			minX = 0;
			maxX = dataTable.rows();
		}
		
		if (minY >= maxY)
		{
			minY = 0;
			maxY = dataTable.cols();
		}
		
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
			ConsoleWindow::error(tr("No clipboard available."));
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
		
		ConsoleWindow::message(tr("Tab-delimited data copied to clipboard."));
	}
	
	void Plot3DWidget::printToFile(const QString& fileName)
	{
		if (surfacePlot)
			surfacePlot->savePixmap(fileName, tr("PNG"));
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
	
	void Plot3DWidget::savePixmap()
	{		
		QString fileName = 
			QFileDialog::getSaveFileName(this, tr("Print to File"),
                                          MainWindow::userHome(),
                                          tr("PNG Files (*.png)"));
		if (surfacePlot)
		{
			QPixmap pixmap = surfacePlot->renderPixmap();
			pixmap.save(fileName,"PNG");
		}
	}
	
	void Plot3DWidget::saveVector()
	{		
		QString fileName = 
			QFileDialog::getSaveFileName(this, tr("Print to File"),
                                          MainWindow::userHome(),
                                          tr("PS Files (*.ps)"));
		if (surfacePlot)
			surfacePlot->saveVector(fileName, tr("PS"),VectorWriter::PIXEL,VectorWriter::NOSORT);
	}
	
	void Plot3DWidget::setTitle()
	{	
		QString s = QInputDialog::getText(this,tr("Plot Title"),tr("title :"));
		
		setTitle(s);	
	}
	
	void Plot3DWidget::setXLabel()
	{	
		QString s = QInputDialog::getText(this,tr("Plot Label"),tr("x-axis label :"));
		
		setXLabel(s);	
	}
	
	void Plot3DWidget::setYLabel()
	{
		QString s = QInputDialog::getText(this,tr("Plot Label"),tr("y-axis label :"));
		
		setYLabel(s);
	}
	
	void Plot3DWidget::setZLabel()
	{
		QString s = QInputDialog::getText(this,tr("Plot Label"),tr("z-axis label :"));
		
		setZLabel(s);
	}

}
