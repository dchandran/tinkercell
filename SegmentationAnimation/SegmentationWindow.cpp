/****************************************************************************
 **
 ** Copyright (c) 2008 Deepak Chandran
 ** Contact: Deepak Chandran (dchandran1@gmail.com)
 **
 ** See COPYRIGHT.TXT
 **
 ****************************************************************************/

#include <QtDebug>
#include <QSpinBox>
#include <QDir>
#include "SegmentationWindow.h"

SegmentationAnimation::MainWindow::MainWindow()
{
	simCol = ssCol = 1;
	setPalette(QPalette(QColor(255, 255, 255)));
	drawScene.setBackgroundBrush(QBrush(QColor(0,0,0)));
	setAutoFillBackground(true);

	QDockWidget * toolBox = makeToolBox();
	addDockWidget(Qt::TopDockWidgetArea,toolBox);
	toolBox->setAllowedAreas(Qt::NoDockWidgetArea);
	toolBox->setFloating(true);
	graphicsView.setScene(&drawScene);
	graphicsView.setRenderHint(QPainter::Antialiasing);

	speedSlider.setRange(5,50);
	speedSlider.setValue(15);
	timer.setDuration(15000);
	connect(&timer,SIGNAL(frameChanged(int)),this,SLOT(frameChanged(int)));
	connect(&speedSlider,SIGNAL(valueChanged(int)),this,SLOT(speedChanged(int)));

	setCentralWidget(&graphicsView);
	setWindowTitle(tr("Segmentation Animation"));

	statusBar()->showMessage("Welcome to Segmentation Animation....Enjoy!");
	steadyStateFile = timeSimulationFile = QDir::homePath();
}

void SegmentationAnimation::MainWindow::speedChanged(int i)
{
	timer.setDuration(i * 1000);
}

void SegmentationAnimation::MainWindow::readTable(DataTable& data, QFile& file)
{
	data.resize(0,0);
	int i = 0;
	while (!file.atEnd())
	{
		QString line(file.readLine());
		QStringList words = line.split('\t');
		for (int j=0; j < words.size(); ++j)
		{
			data.value(i,j) = words[j].toDouble();
		}
		++i;
	}
}

void SegmentationAnimation::MainWindow::getTimeSimFile()
{
	QString fileName =
			QFileDialog::getOpenFileName(this, tr("Get Time Series Data"),
										  timeSimulationFile,
										  tr("Tab-Delimited Files (*.txt *.tab *.out)"));
	if (fileName.isEmpty())
		return;

	timeSimulationFile = fileName;

	QFile file (timeSimulationFile);

	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Simulation File"),
							  tr("Cannot read file %1:\n%2.")
							  .arg(fileName)
							  .arg(file.errorString()));
		return;
	}

	timer.stop();

	readTable(timeSimulationData,file);

	statusBar()->showMessage( QString::number(timeSimulationData.rows()) + tr(" rows and ")
				+ QString::number(timeSimulationData.cols()) + tr(" cols read."));

	file.close();
}

void SegmentationAnimation::MainWindow::getSSFile()
{
	QString fileName =
			QFileDialog::getOpenFileName(this, tr("Get Steady State Data"),
										  steadyStateFile,
										  tr("Tab-Delimited Files (*.txt *.tab *.out)"));
	if (fileName.isEmpty())
		return;

	steadyStateFile = fileName;

	QFile file (steadyStateFile);

	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("Steady State File"),
							  tr("Cannot read file %1:\n%2.")
							  .arg(fileName)
							  .arg(file.errorString()));
		return;
	}

	timer.stop();

	readTable(steadyStateData,file);

	statusBar()->showMessage( QString::number(steadyStateData.rows()) + tr(" rows and ")
				+ QString::number(timeSimulationData.cols()) + tr(" cols read."));

	file.close();
}
void SegmentationAnimation::MainWindow::setTimeSimCol(int i)
{
	timer.stop();
	simCol = i;
}
void SegmentationAnimation::MainWindow::setSSCol(int i)
{
	timer.stop();
	ssCol = i;
}

QDockWidget* SegmentationAnimation::MainWindow::makeToolBox()
{
	QDockWidget * dockWidget = new QDockWidget;

	QGroupBox * group1 = new QGroupBox("Simulation file");
	QGroupBox * group2 = new QGroupBox("Steady state file");
	QGroupBox * group3 = new QGroupBox("Animation speed (x1000ms)");

	QPushButton * button1 = new QPushButton(tr("Open Simulation File"),group1);
	connect(button1,SIGNAL(released()),this,SLOT(getTimeSimFile()));
	QSpinBox * spinBox1 = new QSpinBox(group1);
	spinBox1->setValue(1);

	connect(spinBox1,SIGNAL(valueChanged(int)),this,SLOT(setTimeSimCol(int)));
	spinBox1->setPrefix(tr("Column Number   "));

	QPushButton * button2 = new QPushButton(tr("Open Steady State File"),group2);
	connect(button2,SIGNAL(released()),this,SLOT(getSSFile()));
	QSpinBox * spinBox2 = new QSpinBox(group2);
	spinBox2->setValue(1);
	connect(spinBox2,SIGNAL(valueChanged(int)),this,SLOT(setSSCol(int)));
	spinBox2->setPrefix(tr("Column Number   "));

	QHBoxLayout * layout1 = new QHBoxLayout;
	layout1->addWidget(button1);
	layout1->addWidget(spinBox1);
	group1->setLayout(layout1);

	QHBoxLayout * layout2 = new QHBoxLayout;
	layout2->addWidget(button2);
	layout2->addWidget(spinBox2);
	group2->setLayout(layout2);

	QHBoxLayout * layout3 = new QHBoxLayout;
	layout3->addWidget(&speedSlider);
	speedSlider.setOrientation(Qt::Horizontal);
	group3->setLayout(layout3);

	QVBoxLayout * layout = new QVBoxLayout;
	layout->addWidget(group1);
	layout->addWidget(group2);
	layout->addWidget(group3);

	QPushButton * runButton = new QPushButton(tr("START"),dockWidget);
	connect(runButton,SIGNAL(released()),this,SLOT(init()));
	layout->addWidget(runButton);

	QWidget * widget = new QWidget(dockWidget);
	widget->setPalette(QPalette(QColor(222, 221, 238)));
	widget->setLayout(layout);
	dockWidget->setWidget(widget);
	dockWidget->setPalette(QPalette(QColor(222, 221, 238)));
	return dockWidget;
}

void SegmentationAnimation::MainWindow::normalize(DataTable& data,int col)
{
	qreal max = -10000.0;
	for (int i=0; i < data.rows(); ++i)
		if (data.value(i,col) > max)
			max = data.value(i,col);
	if (max != 0)
	{
		//max = 200.0/max;
		for (int i=0; i < data.rows(); ++i)
		{
			data.value(i,col) /= max;
			//data.value(i,col) *= max;
			//data.value(i,col) += 50.0;
		}
	}
}

void SegmentationAnimation::MainWindow::init()
{
	timer.stop();
	statusBar()->showMessage(QString("stopped"));

	normalize(steadyStateData,ssCol);
	normalize(timeSimulationData,simCol);

	statusBar()->showMessage(QString("loaded"));
	qDeleteAll(cells);
	statusBar()->showMessage(QString("refreshed"));
	cells.clear();

	statusBar()->showMessage(QString("cleared"));

	drawScene.addItem(&waveItem);

	int rows = steadyStateData.rows();

        //TODO: I guess this condition could be avoided 
        //by a previous missing condition.
        //arnaudgelas: 07/16/2009
        if( rows == 0 ) 
          return;

	dx = 1000 / rows;
	qreal w = dx - 2;

	drawScene.setSceneRect(0,0,dx*rows,400);

	qreal x = 10.0, y = 60.0;
	QRectF rect(-w/2.0,-50,w,100);

	QPen pen(QColor(0,255,0),2);

	QLinearGradient gradient(QPointF(-5,0),QPointF(5,0));
	gradient.setColorAt(0.1,QColor(192,255,0));
	gradient.setColorAt(1.0,QColor(79,196,5));

	QBrush brush(gradient);
	for (int i=0; i < rows; ++i)
	{
		RoundedRectItem * cell = new RoundedRectItem(rect);
		drawScene.addItem(cell);
		cell->setPos(x,y);
		cell->setPen(pen);
		cell->setBrush(brush);
		x += dx;
		cells += cell;
	}

	waveItem.setPen(QPen(QColor(255,250,0),2));
	waveItem.setBrush(QBrush(QColor(255,168,0)));
	waveItem.setPoints(QPointF(5.0,200.0),QPointF(10.0,175.0),QPointF(x-dx,150.0),50.0);

	rows = timeSimulationData.rows();
	timer.setFrameRange(0,rows-1);
	timer.setLoopCount(0);
	timer.start();
}
void SegmentationAnimation::MainWindow::frameChanged(int value)
{
	int rows = timeSimulationData.rows();
	if (value >= rows) return;

	qreal currentTime = timeSimulationData.value(value,0);

	qreal g = (timeSimulationData.value(value, simCol));
	QLinearGradient gradient(QPointF(-5,0),QPointF(5,0));
	gradient.setColorAt(0.0,QColor(192*g,255*g,0));
	gradient.setColorAt(1.0,QColor(79,196*g,5*g));
	QBrush brush(gradient);

	for (int i=0; i < cells.size(); ++i)
		if (cells[i])
		{
			if (steadyStateData.value(i,0) > currentTime)
				cells[i]->setBrush(brush);
			else
			{
				qreal g = steadyStateData.value(i,ssCol);
				QLinearGradient gradient(QPointF(-5,0),QPointF(5,0));
				gradient.setColorAt(0.0,QColor(192*g,255*g,0));
				gradient.setColorAt(1.0,QColor(79,196*g,5*g));
				QBrush brush(gradient);
				cells[i]->setBrush(brush);
			}
		}

	qreal maxTime = timeSimulationData.value(rows-1,0);
	if (value < rows)
	{
		qreal x = 10.0 + (currentTime/maxTime) * (cells.size()*dx);
		statusBar()->showMessage(QString("T0=") + QString::number(maxTime) + QString(" : t=") + QString::number(currentTime) + QString(" :: x = ") + QString::number(x));
		waveItem.setPoints(QPointF(5.0,200.0),QPointF(x,175.0),QPointF(10.0 + cells.size()*dx,150.0),50.0);
	}
}

SegmentationAnimation::MainWindow::~MainWindow()
{
	timer.stop();
	qDeleteAll(cells);
}


