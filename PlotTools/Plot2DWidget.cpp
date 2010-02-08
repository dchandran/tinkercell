/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.
 
****************************************************************************/

#include <math.h>
#include <QSettings>
#include <QRegExp>
#include <QColorDialog>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QPrinter>
#include <QListWidget>
#include <QTableWidget>
#include "qwt_scale_engine.h"
#include "qwt_symbol.h"
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "PlotTool.h"
#include "PlotTextWidget.h"
#include "Plot2DWidget.h"

namespace Tinkercell
{
	/**********************
	Scatterplot data Column
	**********************/
	ScatterPlotColumn::ScatterPlotColumn(DataTable<qreal> * data, int x, int y) : 
			QwtPlotMarker(), dataTable(data), column(y), xaxis(x) 
	{ 
	}
		
	void ScatterPlotColumn::draw(QPainter *painter,
			const QwtScaleMap &xMap, const QwtScaleMap &yMap,
			const QRect &canvasRect) const
	{
		int x,y;
		for (int i=0; i < dataTable->rows(); ++i)
		{
			x = xMap.transform(dataTable->at(i,xaxis));
			y = yMap.transform(dataTable->at(i,column));
			QwtPlotMarker::drawAt(painter, canvasRect, QPoint(x, y));
		}
	}
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

	QList<QPen> DataPlot::penList = QList<QPen>();

	DataPlot::DataPlot(QWidget * parent) : QwtPlot(parent)
	{
		xcolumn = 0;
		delta = 1;
		zoomer = new QwtPlotZoomer(xBottom,yLeft,QwtPicker::DragSelection,QwtPicker::AlwaysOff,canvas());
		zoomer->setRubberBandPen(QPen(Qt::black));
		setCanvasBackground(Qt::white);
		plotLayout()->setAlignCanvasToScales(true);
		plotLayout()->setCanvasMargin(0);
		//setAxisAutoScale(xBottom);
		//setAxisAutoScale(yLeft);
		QwtPlotCanvas * c = canvas();
		c->setFrameShadow ( QFrame::Plain );
		c->setFrameShape ( QFrame::NoFrame );
		c->setFrameStyle (QFrame::NoFrame );
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
					{
						hideList.removeAll(dataTable.colName(i));
					}
					else
					if (!on && !hideList.contains(dataTable.colName(i)))
					{
						hideList += (dataTable.colName(i));
					}
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
	
	void DataPlot::plot(const DataTable<qreal>& dat, int x, const QString& title, int dt, bool scatterplot)
	{
		delta = dt;
		xcolumn = x;
		if (!this->isVisible())
		{
			if (this->parentWidget() && !this->parentWidget()->isVisible())
				this->parentWidget()->show();
			else
				this->show();
		}
		setAutoReplot(false);
		this->dataTable = dat;
		//visibleDataTable = dat;
		
		QRegExp regex(tr("\\_(?!_)"));
		for (int i=0; i < dataTable.rows(); ++i)
		{
			this->dataTable.rowName(i).replace(regex,tr("."));
		}

		this->clear();
		insertLegend(new QwtLegend(this), QwtPlot::RightLegend,0.2);
		legend()->setItemMode(QwtLegend::CheckableItem);
		
		connect(this,SIGNAL(legendChecked(QwtPlotItem*,bool)),this,SLOT(itemChecked(QwtPlotItem*,bool)));
		
		QList<QwtPlotCurve*> curves;
		for (int i=0, c = 0, t = 0; i < dataTable.cols(); ++i)
		{
			if (c >= penList.size())
				c = 0;
			if (i != x && dataTable.colName(i).toLower() != tr("time"))
			{
				if (scatterplot)
				{
					QwtPlotMarker * marker = new ScatterPlotColumn(&dataTable,x,i);
					if (c < 10)
						marker->setSymbol(
								QwtSymbol(QwtSymbol::Style(c),
													Qt::NoBrush,
													penList[c],
													QSize(1,1)));
					else
						marker->setSymbol(
								QwtSymbol(QwtSymbol::Style(c-10),
													Qt::NoBrush,
													penList[c],
													QSize(1,1)));
					marker->setLinePen(penList[c]);
					marker->attach(this);
					marker->updateLegend(legend());
				}
				else
				{
					QwtPlotCurve * curve = new QwtPlotCurve(dataTable.colName(i));
					curve->setRenderHint(QwtPlotItem::RenderAntialiased);						
					curve->setPen(penList[c]);
					curve->setData( DataColumn(&dataTable,x,i,dt) );
					curve->attach(this);
					curve->updateLegend(legend());
				}
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
		
		//replotUsingHideList();
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
	
	/*void DataPlot::makeVisibleDataTable()
	{
		visibleDataTable = dataTable;
		for (int i=0; i < hideList.size(); ++i)
			visibleDataTable.removeCol(hideList[i]);
	}*/
	
	/*********************************
		Plot Widget
	*********************************/
	
	Plot2DWidget::Plot2DWidget(PlotTool * parent) : PlotWidget(parent), buttonsGroup(this)
	{
		type = PlotTool::Plot2D;
		if (DataPlot::penList.isEmpty())
		{
			QCoreApplication::setOrganizationName(Tinkercell::ORGANIZATIONNAME);
			QCoreApplication::setOrganizationDomain(Tinkercell::PROJECTWEBSITE);
			QCoreApplication::setApplicationName(Tinkercell::ORGANIZATIONNAME);

			QSettings settings(Tinkercell::ORGANIZATIONNAME, Tinkercell::ORGANIZATIONNAME);
			
			settings.beginGroup("Plot2DWidget");
			
			QStringList colors, penWidths, penStyles;
			
			colors = settings.value(tr("colors"),QStringList()).toStringList();
			penWidths = settings.value(tr("widths"),QStringList()).toStringList();
			penStyles = settings.value(tr("styles"),QStringList()).toStringList();
		
			settings.endGroup();
			
			for (int i=0; i < colors.size() && i < penWidths.size() && i < penStyles.size(); ++i)
			{
				bool ok;
				double w = penWidths[i].toDouble(&ok);
				if (!ok) w = 2.0;
				
				int k = penStyles[i].toInt(&ok);
				if (!ok) k = 0;
				
				DataPlot::penList << QPen(QColor(colors[i]),w,Qt::PenStyle(k));
			}
		}
		
		dataPlot = new DataPlot();
		
		QwtPlotPicker * d_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
																	QwtPicker::PointSelection | QwtPicker::DragSelection, 
																	QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn, 
																	dataPlot->canvas());
		d_picker->setRubberBandPen(QColor(Qt::green));
		d_picker->setRubberBand(QwtPicker::CrossRubberBand);
		d_picker->setTrackerPen(QColor(Qt::white));
		connect(d_picker, SIGNAL(moved(const QPoint &)),
						SLOT(mouseMoved(const QPoint &)));
		
		axisNames = new QComboBox();
		connect(axisNames,SIGNAL(currentIndexChanged(int)),dataPlot,SLOT(setXAxis(int)));
		
		dataPlot->setTitle(tr("Plot"));
		
		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(dataPlot);
		setPalette(QPalette(QColor(255,255,255,255)));
		setAutoFillBackground(true);
		setLayout(layout);
		
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
		
		QToolButton * changeColors = new QToolButton(this);
		changeColors->setIcon(QIcon(":/images/pencil.png"));
		changeColors->setText(tr("Colors"));
		changeColors->setToolTip(tr("Line type and color"));
		changeColors->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		
		this->dialog = new GetPenInfoDialog(this);
		this->dialog->setModal (true);
		this->dialog->setSizeGripEnabled (true);
		dialog->hide();
		connect(dialog,SIGNAL(accepted()),this,SLOT(penSet()));
		
		QDialog * dialog2 = new QDialog(this);
		dialog2->setSizeGripEnabled (true);
		QVBoxLayout * dialogLayout = new QVBoxLayout;
		dialogLayout->addWidget(dialogWidget());
		QPushButton * closeButton = new QPushButton;
		closeButton->setText(tr("Close"));
		dialogLayout->addWidget(closeButton);
		dialog2->setLayout(dialogLayout);
		dialog2->resize(100,200);
		
		connect(closeButton,SIGNAL(released()),dialog2,SLOT(accept()));
		connect(changeColors,SIGNAL(pressed()),dialog2,SLOT(exec()));
		
		toolBar.addWidget(new QLabel(tr("x-axis:")));
		toolBar.addWidget(axisNames);
		toolBar.addWidget(setLabels);
		toolBar.addWidget(logScale);
		toolBar.addWidget(changeColors);
		
		connect(&buttonsGroup,SIGNAL(buttonPressed(int)),this,SLOT(buttonPressed(int)));
		
		setMinimumHeight(200);
		setMouseTracking(true);
	}
	
	void Plot2DWidget::buttonPressed(int k)
	{
		if (dataPlot && dialog && DataPlot::penList.size() > k)
		{
			dialog->setPen(DataPlot::penList[k],k);
			dialog->exec();
		}
	}
	
	void Plot2DWidget::penSet()
	{
			int k = dialog->currentIndex();
			DataPlot::penList[k] = dialog->getPen();
			
			QAbstractButton * button = buttonsGroup.button(k);
			if (button)
				button->setStyleSheet(tr("background-color: ") + DataPlot::penList[k].color().name());
			
			QCoreApplication::setOrganizationName(Tinkercell::ORGANIZATIONNAME);
			QCoreApplication::setOrganizationDomain(Tinkercell::PROJECTWEBSITE);
			QCoreApplication::setApplicationName(Tinkercell::ORGANIZATIONNAME);

			QSettings settings(Tinkercell::ORGANIZATIONNAME, Tinkercell::ORGANIZATIONNAME);
			
			settings.beginGroup("Plot2DWidget");
			
			QStringList colors, penWidth, penStyles;
			
			QList<QPen>& penList = DataPlot::penList;
			
			for (int i=0; i < penList.size(); ++i)
			{
				colors << penList[i].color().name();
				penStyles << QString::number((int)(penList[i].style()));
				penWidth << QString::number(penList[i].widthF());
			}
			
			settings.setValue(tr("colors"),colors);
			settings.setValue(tr("widths"),penWidth);
			settings.setValue(tr("styles"),penStyles);
			
			settings.endGroup();
			
			//dataPlot->replot();
			dataPlot->plot(	
						dataPlot->dataTable,
						dataPlot->xcolumn,
						dataPlot->title().text(),
						dataPlot->delta,
						type == PlotTool::ScatterPlot);
	}
	
	void Plot2DWidget::plot(const DataTable<qreal>& matrix,const QString& title,int x)
	{
		if (!dataPlot) return;
		
		int dt = 1;
		if (matrix.rows() > 100)
		{
			dt = (int)(matrix.rows() / 100) + 1;
		}
		
		dataPlot->plot(matrix,x,title,dt,type == PlotTool::ScatterPlot);
		if (axisNames)
		{
			axisNames->clear();
			if (x >= 0)
				axisNames->addItems(matrix.getColNames());
		}
	}
	
	DataTable<qreal>* Plot2DWidget::data()
	{
		if (!dataPlot) return 0;
		
		//dataPlot->makeVisibleDataTable();
		
		return &(dataPlot->dataTable);
	}
	
	void Plot2DWidget::updateData(const DataTable<qreal> & newData)
	{
		if (!dataPlot) return;
		
		DataTable<qreal> & dataTable = dataPlot->dataTable;
		dataTable = newData;
		
		int removeSz = 0;
		for (int i=0; i < newData.cols(); ++i)
			if (dataPlot->hideList.contains(newData.colName(i)))
				++removeSz;
				
		dataTable.resize(newData.rows(), newData.cols() - removeSz);
		dataTable.setRowNames(newData.getRowNames());
		
		for (int i=0, k = 0; i < newData.cols(); ++i)
			if (!dataPlot->hideList.contains(dataTable.colName(i)))
			{
				dataTable.colName(k) = dataTable.colName(i);
				for (int j=0; j < dataTable.rows(); ++j)
					dataTable.value(j,k) = newData.at(j,i);
				++k;
			}
		if (dataPlot->hideList.isEmpty())
		{
			dataPlot->replot();
		}
		else
		{
			dataPlot->hideList.clear();
			dataPlot->plot(	dataTable,
						dataPlot->xcolumn,
						dataPlot->title().text(),
						dataPlot->delta,
						type == PlotTool::ScatterPlot);
		}
		
	}
	
	void Plot2DWidget::exportData(const QString& type)
	{
		if (!dataPlot) return;
		
		if (type.toLower() == tr("image"))
		{
			QString fileName = 
				QFileDialog::getSaveFileName(this, tr("Print to File"),
                                          MainWindow::previousFileName,
                                          tr("PDF Files (*.pdf)"));
			
			if (fileName.isEmpty() || fileName.isNull()) return;
			
			MainWindow::previousFileName = fileName.remove(QRegExp(tr("\\.*")));
			//QPrinter printer(QPrinter::HighResolution);
			QPrinter printer(QPrinter::ScreenResolution);
			printer.setOutputFormat(QPrinter::PdfFormat);
			printer.setOrientation(QPrinter::Landscape);
			
			printer.setOutputFileName(fileName);
			dataPlot->print(printer);
		}
		else
		if (type.toLower() == tr("snapshot"))
		{
			QClipboard * clipboard = QApplication::clipboard();
			if (clipboard)
			{
				QImage image(600,400,QImage::Format_ARGB32);
				dataPlot->print(image);
				clipboard->setImage(image);
			}
		}
		else
		if (type.toLower() == tr("latex"))
		{
			plotTool->addWidget(new PlotTextWidget(dataPlot->dataTable,plotTool, latex()));
		}
		else
		{
			PlotWidget::exportData(type);
		}
	}
	
	void Plot2DWidget::logX(bool b)
	{
		if (dataPlot) dataPlot->setLogX(b);
	}
	
	void Plot2DWidget::logY(bool b)
	{
		if (dataPlot) dataPlot->setLogY(b);
	}
	
	void Plot2DWidget::logAxis(int i,bool b)
	{
		if (i==0) 
			logX(b);
		else 
			logY(b);
	}
	
	void Plot2DWidget::setTitle()
	{	
		QString s = QInputDialog::getText(this,tr("Plot Title"),tr("Plot title :"));
		if (s.isNull()) return;
		setTitle(s);
	}
	
	void Plot2DWidget::setTitle(const QString& s)
	{
		if (!dataPlot) return;
		
		if (s.isNull()) return;
		
		dataPlot->setTitle(s);
	}
	
	void Plot2DWidget::setXLabel()
	{	
		QString s = QInputDialog::getText(this,tr("Plot Label"),tr("x-axis label :"));
		if (s.isNull()) return;
		setXLabel(s);	
	}
	
	void Plot2DWidget::setXLabel(const QString& s)
	{
		if (!dataPlot) return;
		
		if (s.isNull() || s.isEmpty()) return;
		
		dataPlot->setAxisTitle(QwtPlot::xBottom, s);		
	}
	
	void Plot2DWidget::setYLabel()
	{
		QString s = QInputDialog::getText(this,tr("Plot Label"),tr("y-axis label :"));
		if (s.isNull()) return;
		setYLabel(s);
	}
	
	void Plot2DWidget::setYLabel(const QString& s)
	{
		if (!dataPlot) return;
		
		if (s.isNull()) return;
		
		dataPlot->setAxisTitle(QwtPlot::yLeft, s);
	}
	
	void Plot2DWidget::mouseMoved ( const QPoint& p )
	{
		if (p.isNull() || !plotTool || !dataPlot) return;
		
		plotTool->setStatusBarMessage(
			tr("  x: ") 
			+ QString::number(p.x())
			+ tr("  y: ") 
			+ QString::number(p.y())
		);
	}
	
	QWidget * Plot2DWidget::dialogWidget()
	{
		if (DataPlot::penList.isEmpty())
		{
			DataPlot::penList 	<< QPen(QColor(tr("#232CE6")),2,Qt::SolidLine)
								<< QPen(QColor(tr("#CA420D")),2,Qt::SolidLine)
								<< QPen(QColor(tr("#11A306")),2,Qt::SolidLine)
								<< QPen(QColor(tr("#BF0CB0")),2,Qt::SolidLine)
								<< QPen(QColor(tr("#D9C11F")),2,Qt::SolidLine)
								<< QPen(QColor(tr("#0CBDBF")),2,Qt::SolidLine)
								<< QPen(QColor(tr("#232CE6")),2,Qt::DotLine)
								<< QPen(QColor(tr("#CA420D")),2,Qt::DotLine)
								<< QPen(QColor(tr("#11A306")),2,Qt::DotLine)
								<< QPen(QColor(tr("#BF0CB0")),2,Qt::DotLine)
								<< QPen(QColor(tr("#D9C11F")),2,Qt::DotLine)
								<< QPen(QColor(tr("#0CBDBF")),2,Qt::DotLine);
		}
		
		QTableWidget * tableWidget = new QTableWidget(DataPlot::penList.size(),1);
		tableWidget->horizontalHeader()->hide();
		tableWidget->verticalHeader()->hide();
		
		for (int i=0; i < DataPlot::penList.size(); ++i)
		{
			QPushButton * button = new QPushButton;
			button->setStyleSheet(tr("background-color: ") + DataPlot::penList[i].color().name());
			tableWidget->setCellWidget(i,0,button);
			buttonsGroup.addButton(button,i);
		}
		
		return tableWidget;
	}
	
	QString Plot2DWidget::latex()
	{
		if (!dataPlot) return QString();
		
		QString output;
		
		DataTable<qreal> & table = *(data());
		
		double xmin = table.at(0,dataPlot->xcolumn),
			   xmax = table.at(table.rows()-1,dataPlot->xcolumn),
			   ymin = table.at(0,1),
			   ymax = table.at(0,1);
			
		QStringList colnames(table.getColNames()),
					rownames(table.getRowNames());

		int c = 0;
		
		output += tr("\\documentclass{article}\n\n\\usepackage{tikz}\n\n\\usepackage{pgfplots}\n\n\n\\begin{document}\n\n");
		output += tr("\\begin{ticzpicture}\n");

		for (int i=0; i < colnames.size(); ++i)
		{
			for (int j=0; j < table.rows(); ++j)
				for (int k=0; k < table.cols(); ++k)
					if (k != dataPlot->xcolumn)
					{
						if (ymin > table.at(j,k))
							ymin = table.at(j,k);

						if (ymax < table.at(j,k))
							ymax = table.at(j,k);
					}
				
			if (c >= DataPlot::penList.size())
			{
				c = 0;
			}				
				
			QPen pen = DataPlot::penList[c];
		
			output += tr("\\begin{axis}[\ngrid=major,\nxlabel=")
						+ colnames.at(dataPlot->xcolumn) 
						+ tr(",\nylabel=Values")
						+ tr(",\nxmin=") + QString::number(xmin)
						+ tr(",\nxmax=") + QString::number(xmax)
						+ tr(",\nymin=") + QString::number(ymin)
						+ tr(",\nymax=") + QString::number(ymax)
						+ tr(",\nwidth=8cm,\nheight=6cm,]\n\\addplot[smooth,color=")
						+ pen.color().name()
						+ tr(",line width=1.5pt] coordinates {\n");
		
			for (int j=0; j < table.rows(); ++j)
			{
				output += 	tr("(") 
							+ QString::number(table.at(j,dataPlot->xcolumn))
							+ tr(", ")
							+ QString::number(table.at(j,i))
							+ tr(")\n");
			}
			output += tr("};\n\\addlegendentry{") + colnames.at(i) + tr("}\n");
			output += tr("\\end{axis}\n");
		}
		
		output += tr("\\end{tikzpicture}\n");
		
		return output;
	}
	
	GetPenInfoDialog::GetPenInfoDialog(QWidget * parent) : QDialog(parent)
	{
		connect(&colorDialog,SIGNAL(currentColorChanged(const QColor&)),this,SLOT(currentColorChanged(const QColor&)));
		
		QHBoxLayout * layout1 = new QHBoxLayout;
		layout1->addWidget(&colorDialog);
		QGroupBox * colorGrp = new QGroupBox(tr(" pen color "));
		colorGrp->setLayout(layout1);
		
		QHBoxLayout * layout2 = new QHBoxLayout;
		layout2->addWidget(&comboBox);
		comboBox.addItems(QStringList() << "Solid line" << "Dotted line");
		layout2->addWidget(&spinBox);
		spinBox.setRange(0,10);
		spinBox.setValue(2.0);
		QGroupBox * spinBoxGrp = new QGroupBox(tr(" line type and width "));
		spinBoxGrp->setLayout(layout2);
		
		setSizeGripEnabled(true);
		
		QHBoxLayout * layout3 = new QHBoxLayout;		
		QPushButton * okButton = new QPushButton;
		QPushButton * cancelButton = new QPushButton;
		okButton->setDefault ( true );
		okButton->setCheckable(false);
		okButton->setAutoExclusive(false);
		cancelButton->setCheckable(false);
		cancelButton->setAutoExclusive(false);
		okButton->setText(tr(" Set Pen "));
		cancelButton->setText(tr(" Cancel "));
		layout3->addWidget(okButton);
		layout3->addWidget(cancelButton);
		
		connect(okButton,SIGNAL(pressed()),this,SLOT(accept()));
		connect(cancelButton,SIGNAL(pressed()),this,SLOT(reject()));
		
		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(colorGrp,10);
		layout->addWidget(spinBoxGrp,0);
		layout->addLayout(layout3,0);
		
		setLayout(layout);
	}
	
	void GetPenInfoDialog::currentColorChanged ( const QColor & color )
	{
		this->color = color;
	}
	
	QPen GetPenInfoDialog::getPen() const
	{
		if (comboBox.currentIndex() == 0)
			return QPen(color,spinBox.value(),Qt::SolidLine);
		else
			return QPen(color,spinBox.value(),Qt::DotLine);
	}
	
	void GetPenInfoDialog::setPen(const QPen& pen, int k)
	{
		index = k;
		colorDialog.setCurrentColor(pen.color());
		color = pen.color();
		spinBox.setValue(pen.widthF());
		if (pen.style() == Qt::SolidLine)
			comboBox.setCurrentIndex(0);
		else
			comboBox.setCurrentIndex(1);
	}
	
	int GetPenInfoDialog::currentIndex() const
	{
		return index;
	}
	
	bool Plot2DWidget::canAppendData() const
	{
		return true;
	}
	
	void Plot2DWidget::appendData(const DataTable<qreal>& newData)
	{
		if (!dataPlot) return;
		
		DataTable<qreal> dataTable(*data());
		int xIndex = -1;
		
		QStringList colNames = newData.getColNames();
		
		if (dataPlot->xcolumn >= 0)
			xIndex = colNames.indexOf(dataTable.colName(dataPlot->xcolumn));
		
		for (int i=0; i < dataTable.cols(); ++i)
			if (colNames.contains(dataTable.colName(i)) && i != dataPlot->xcolumn)
				dataTable.colName(i) += tr("'"); 
		
		int m = newData.rows();
		if (m > dataTable.rows())
			m = dataTable.rows();
		
		int n = dataTable.cols();
		
		if (xIndex >= 0)
			dataTable.resize(m,dataTable.cols() + newData.cols() - 1);
		else
			dataTable.resize(m,dataTable.cols() + newData.cols());
		
		for (int i=0, k=0; i < newData.cols(); ++i)
			if (i != xIndex)
			{
				dataTable.colName(k+n) = newData.colName(i);
				for (int j=0; j < m; ++j)
					dataTable.value(j,k+n) = newData.at(j,i);
				++k;
			}
		
		dataPlot->plot(	dataTable,
						dataPlot->xcolumn,
						dataPlot->title().text(),
						dataPlot->delta,
						type == PlotTool::ScatterPlot);
	}
}
