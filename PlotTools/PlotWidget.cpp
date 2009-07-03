/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.
 
****************************************************************************/

#include <math.h>
#include <QGroupBox>
#include "qwt_scale_engine.h"
#include "GraphicsScene.h"
#include "MainWindow.h"
//#include "PartGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "OutputWindow.h"
#include "PlotTool.h"
#include "PlotWidget.h"

namespace Tinkercell
{
	/**********************
	Data Column
	**********************/
	DataColumn::DataColumn(DataTable<qreal>* dataPtr, int xindex, int yindex, int delta)
	{
		dataTable = dataPtr;
		column = yindex;
		xaxis = xindex;
		dt = delta;
		if (dt < 1 || dt > dataTable->rows()/2) dt = 1;
	}
	
	QwtData * DataColumn::copy() const
	{
		return new DataColumn(dataTable,xaxis, column);
	}
	
	size_t DataColumn::size() const
	{
		if (!dataTable) return 0;
		return (int)(dataTable->rows()/dt);
	}
	
	double DataColumn::x(size_t index) const
	{
		if (!dataTable) return 0;
		if (xaxis < 0) return (int)index;
		return dataTable->at((int)index,xaxis);
	}
	
	double DataColumn::y(size_t index) const
	{
		if (!dataTable) return 0;
		return dataTable->at((int)index*dt,column);
	}

	/****************************
	Data Plot
	****************************/

	DataPlot::DataPlot(QWidget * parent) : QwtPlot(parent)
	{
		zoomer = new QwtPlotZoomer(xBottom,yLeft,QwtPicker::DragSelection,QwtPicker::AlwaysOff,canvas());
		zoomer->setRubberBandPen(QPen(Qt::black));
		setCanvasBackground(Qt::white);
		plotLayout()->setAlignCanvasToScales(true);
		plotLayout()->setCanvasMargin(0);
		//setAxisAutoScale(xBottom);
		//setAxisAutoScale(yLeft);
	}

	QSize DataPlot::minimumSizeHint() const
	{
		return QSize(100,50);
	}

	QSize DataPlot::sizeHint() const
	{
		return QSize(160,80);
	}
	
	void DataPlot::itemChecked(QwtPlotItem * plotItem,bool on)
	{
		if (plotItem)
		{
			on = !on;
			plotItem->setVisible(on);
			const QwtPlotItemList& list = itemList();
			for (int i=0; i < dataTable.cols() && i < list.size(); ++i)
				if (list.at(i) == plotItem)
				{
					if (on && hideList.contains(dataTable.colName(i)))
						hideList.removeAll(dataTable.colName(i));
					else
					if (!on && !hideList.contains(dataTable.colName(i)))
						hideList += (dataTable.colName(i));
				}
			this->replot();
		}
	}
	
	void DataPlot::setXAxis(int x)
	{
		if (x >= 0 && x < dataTable.cols())
		{
			int dt = 1;
			if (dataTable.rows() > 100)
			{
				dt = (int)(dataTable.rows() / 100) + 1;
			}
			plot(dataTable,x,title().text(),dt);
		}
	}
	
	void DataPlot::plot(const DataTable<qreal>& dat, int x, const QString& title, int dt)
	{
		if (!this->isVisible())
		{
			if (this->parentWidget() && !this->parentWidget()->isVisible())
				this->parentWidget()->show();
			else
				this->show();
		}
		setAutoReplot(false);
		this->dataTable = dat;
		
		QRegExp regex(tr("\\_(?!_)"));
		for (int i=0; i < dataTable.rows(); ++i)
		{
			this->dataTable.rowName(i).replace(regex,tr("."));
		}

		this->clear();
		insertLegend(new QwtLegend(this), QwtPlot::RightLegend,0.2);
		legend()->setItemMode(QwtLegend::CheckableItem);
		
		connect(this,SIGNAL(legendChecked(QwtPlotItem*,bool)),this,SLOT(itemChecked(QwtPlotItem*,bool)));
		//legend()->setStyleSheet("background: white");

		/*QwtDoubleInterval interval(0.0,(double)dataTable.cols());
		QwtLinearColorMap colorMap;
		colorMap.setMode(QwtLinearColorMap::FixedColors);
		colorMap.addColorStop(0.0,QColor(0,0,255));
		colorMap.addColorStop(0.5,QColor(255,0,0));
		colorMap.addColorStop(1.0,QColor(0,0,0));*/
		
		QList<QColor> colors;
		colors << QColor("#0005DF") << QColor("#F35600") << QColor("#E4DC00")
			   << QColor("#00C312") << QColor("#9600E4") << QColor("#00C1C3");
			   
		QList<Qt::PenStyle> styles;
		styles << Qt::SolidLine << Qt::DotLine << Qt::DashDotLine;
		
		QList<QwtPlotCurve*> curves;
		for (int i=0, c = 0, t = 0; i < dataTable.cols(); ++i)
		{
			if (i != x && dataTable.colName(i).toLower() != tr("time"))
			{
				QwtPlotCurve * curve = new QwtPlotCurve(dataTable.colName(i));
				curve->setRenderHint(QwtPlotItem::RenderAntialiased);
				
				//curve->setPen(QPen(colorMap.color(interval,(double)i),3));
				if (c >= colors.size())
				{
					c = 0;
					++t;
				}
				if (t > styles.size())
				{
					t = 0;
					c = 0;
				}
				curve->setPen(QPen(colors[c],2.3,styles[t]));
				curve->setData( DataColumn(&dataTable,x,i,dt) );
				curve->attach(this);
				curve->updateLegend(legend());
				
				++c;
			}
		}
		if (dataTable.cols() > x)
			setAxisTitle(xBottom, dataTable.colName(x));
		else
			if (x < 0)
				setAxisTitle(xBottom, "Index");
			else
				setAxisTitle(xBottom, "Time");
		
		QString ylabel = axisTitle(QwtPlot::yLeft).text();
		if (ylabel.isEmpty() || ylabel.isNull())
			ylabel = tr("Values");
		setAxisTitle(yLeft, ylabel);
		setTitle(title);

		setAxisAutoScale(xBottom);
		setAxisAutoScale(yLeft);
		setAutoReplot(true);
		replot();
		if (zoomer)
		{
			zoomer->setZoomBase();
		}
		
		replotUsingHideList();
	}
	
	void DataPlot::replotUsingHideList()
	{
		const QwtPlotItemList& list = itemList();
		QwtLegend * leg = legend();
		for (int i=0; i < dataTable.cols() && i < list.size(); ++i)
			if (hideList.contains(dataTable.colName(i)))
			{
				list[i]->setVisible(false);
				QWidget * w = leg->find(list[i]);
				if ( w && w->inherits( "QwtLegendItem" ) )
					((QwtLegendItem *)w)->setChecked( true );
			}
		replot();
	}
	
	void DataPlot::setLogX(bool b)
	{
		if (b)
		{
			setAxisMaxMajor(QwtPlot::xBottom, 6);
			setAxisMaxMinor(QwtPlot::xBottom, 10);
			QwtLog10ScaleEngine * engine = new QwtLog10ScaleEngine;
			double d1,d2,d3;
			engine->autoScale(1,d1,d2,d3);
			setAxisScaleEngine(QwtPlot::xBottom, engine);
		}
		else
		{
			setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
		}
		replot();
	}
	
	void DataPlot::setLogY(bool b)
	{
		if (b)
		{
			setAxisMaxMajor(QwtPlot::yLeft, 6);
			setAxisMaxMinor(QwtPlot::yLeft, 10);
			QwtLog10ScaleEngine * engine = new QwtLog10ScaleEngine;
			double d1,d2,d3;
			engine->autoScale(1,d1,d2,d3);
			setAxisScaleEngine(QwtPlot::yLeft, engine);
		}
		else
		{
			setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
		}
		replot();
	}
	
	DataTable<qreal>& DataPlot::data()
	{
		return dataTable;
	}
	
	/*********************************
		Plot Widget
	*********************************/
	
	PlotWidget::PlotWidget(PlotTool * parent) : QWidget(parent)
	{
		plotTool = parent;
		dataPlot = new DataPlot();
		axisNames = new QComboBox();
		connect(axisNames,SIGNAL(currentIndexChanged(int)),dataPlot,SLOT(setXAxis(int)));
		
		dataPlot->setTitle(tr("Plot"));
		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(dataPlot,10);
		
		setPalette(QPalette(QColor(255,255,255,255)));
		setAutoFillBackground(true);

		QToolButton * print = new QToolButton(this);
		print->setIcon(QIcon(":/images/print.png"));
		print->setText(tr("Print to file"));
		print->setToolTip(tr("Print graph to file"));
		print->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		
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
		setLabels->setMenu(labelsMenu);
		setLabels->setPopupMode ( QToolButton::MenuButtonPopup );
		setLabels->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		
		QToolButton * logScale = new QToolButton(this);
		logScale->setIcon(QIcon(":/images/log.png"));
		logScale->setText(tr("Log scale"));
		logScale->setToolTip(tr("Change to and from log scale"));
		QMenu * logMenu = new QMenu(tr("Set axis labels"),logScale);
		QAction * logx = logMenu->addAction(tr("x-axis"));
		QAction * logy = logMenu->addAction(tr("y-axis"));
		logx->setCheckable(true);
		logy->setCheckable(true);
		logScale->setMenu(logMenu);
		logScale->setPopupMode ( QToolButton::MenuButtonPopup );
		logScale->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		
		connect(logx,SIGNAL(toggled(bool)),this,SLOT(logX(bool)));
		connect(logy,SIGNAL(toggled(bool)),this,SLOT(logY(bool)));		
		
		connect(print,SIGNAL(pressed()),this,SLOT(printToFile()));
		connect(copy,SIGNAL(pressed()),this,SLOT(copyData()));
		
		QHBoxLayout * layout3 = new QHBoxLayout;
		
		QHBoxLayout * layout2 = new QHBoxLayout;
		layout2->addWidget(print);
		layout2->addWidget(copy);
		layout2->addWidget(setLabels);
		layout2->addWidget(logScale);
		
		QHBoxLayout * layout1 = new QHBoxLayout;
		layout1->addWidget(axisNames);
		
		QGroupBox * groupBox1 = new QGroupBox(tr(" x-axis "));
		groupBox1->setLayout(layout1);
		
		QGroupBox * groupBox2 = new QGroupBox(tr(" options "));
		groupBox2->setLayout(layout2);
		
		layout3->addStretch(2);
		layout3->addWidget(groupBox1,1,Qt::AlignRight);
		layout3->addWidget(groupBox2,1,Qt::AlignRight);
		layout->addLayout(layout3,1);
		
		/*QVBoxLayout * layout4 = new QVBoxLayout;
		
		QToolButton * xButton = new QToolButton(this);
		xButton->setIcon(QIcon(":/images/x.png"));
		xButton->setToolTip(tr("Close this plot"));
		connect(xButton,SIGNAL(pressed()),this,SLOT(removePlot()));
		
		QToolButton * upButton = new QToolButton(this);
		upButton->setIcon(QIcon(":/images/up.png"));
		upButton->setToolTip(tr("Move this plot down"));
		connect(upButton,SIGNAL(pressed()),this,SLOT(moveUp()));
		
		QToolButton * downButton = new QToolButton(this);
		downButton->setIcon(QIcon(":/images/down.png"));
		downButton->setToolTip(tr("Move this plot up"));
		connect(downButton,SIGNAL(pressed()),this,SLOT(moveDown()));
		
		layout4->addWidget(xButton);
		layout4->addWidget(upButton);
		layout4->addWidget(downButton);	
		layout4->addStretch(5);
		
		QHBoxLayout * layout5 = new QHBoxLayout;
		layout5->addLayout(layout);
		layout5->addLayout(layout4);
		*/
		setLayout(layout);
		setMinimumHeight(200);
	}
	
	void PlotWidget::plot(const DataTable<qreal>& matrix,const QString& title,int x)
	{
		if (!dataPlot) return;
		
		int dt = 1;
		if (matrix.rows() > 100)
		{
			dt = (int)(matrix.rows() / 100) + 1;
		}
		
		dataPlot->plot(matrix,x,title,dt);
		if (axisNames)
		{
			axisNames->clear();
			if (x >= 0)
				axisNames->addItems(matrix.getColNames());
		}
	}
	
	DataTable<qreal>* PlotWidget::data()
	{
		if (!dataPlot) return 0;
		
		return &(dataPlot->data());
	}
	
	void PlotWidget::printToFile()
	{
		if (!dataPlot)
		{
			OutputWindow::error(tr("Plot tool not initialized."));
		}
		//QPrinter printer(QPrinter::HighResolution);
		QPrinter printer(QPrinter::ScreenResolution);
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOrientation(QPrinter::Landscape);
		
		QString fileName = 
			QFileDialog::getSaveFileName(this, tr("Print to File"),
                                          MainWindow::userHome(),
                                          tr("PDF Files (*.pdf)"));
		if (fileName.isEmpty())
			return;

		printer.setOutputFileName(fileName);
		dataPlot->print(printer);	
	}
	
	void PlotWidget::copyData()
	{
		if (!dataPlot)
		{
			OutputWindow::error(tr("Plot tool not initialized."));
		}
		
		QClipboard * clipboard = QApplication::clipboard();
		
		if (!clipboard)
		{
			OutputWindow::error(tr("No clipboard available."));
		}
		
		QString outputs;
		
		DataTable<qreal> & table = dataPlot->data();
		
		QStringList colnames = table.getColNames(), rownames = table.getRowNames();
		
		for (int i=0; i < colnames.size(); ++i)
		{
			if (i > 0)
				outputs += tr("\t") + colnames.at(i);
			else
				outputs += colnames.at(i);
		}
		outputs += tr("\n");
		for (int i=0; i < table.rows(); ++i)
		{
			outputs += rownames.at(i);
			for (int j=0; j < table.cols(); ++j)
			{
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
	
	void PlotWidget::logX(bool b)
	{
		if (dataPlot) dataPlot->setLogX(b);
	}
	
	void PlotWidget::logY(bool b)
	{
		if (dataPlot) dataPlot->setLogY(b);
	}
	
	void PlotWidget::setTitle()
	{
		if (!dataPlot) return;
		
		QString s = QInputDialog::getText(this,tr("Plot Title"),tr("Plot title :"));
		
		if (s.isNull() || s.isEmpty()) return;
		
		dataPlot->setTitle(s);
	}
	
	void PlotWidget::setXLabel()
	{
		if (!dataPlot) return;
		
		QString s = QInputDialog::getText(this,tr("Plot Label"),tr("x-axis label :"));
		
		if (s.isNull() || s.isEmpty()) return;
		
		dataPlot->setAxisTitle(QwtPlot::xBottom, s);
		
	}
	
	void PlotWidget::setYLabel()
	{
		if (!dataPlot) return;
		
		QString s = QInputDialog::getText(this,tr("Plot Label"),tr("y-axis label :"));
		
		if (s.isNull() || s.isEmpty()) return;
		
		dataPlot->setAxisTitle(QwtPlot::yLeft, s);
	}
}
