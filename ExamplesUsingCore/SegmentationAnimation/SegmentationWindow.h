/****************************************************************************
 **
 ** Copyright (c) 2008 Deepak Chandran
 ** Contact: Deepak Chandran (dchandran1@gmail.com)
 **
 ** See COPYWRITE.TXT
 **
 ****************************************************************************/

#ifndef SEGMENTATION_MAINWINDOW_H
#define SEGMENTATION_MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QToolBox>
#include <QtDebug>
#include <QtGui>
#include <QSlider>
#include <QAction>
#include <QFileDialog>
#include <QTimeLine>
#include <QSlider>
#include <QGraphicsRectItem>
#include <QGraphicsPathItem>
#include "DataTable.h"

namespace SegmentationAnimation
{
typedef Tinkercell::DataTable<qreal> DataTable;

class RoundedRectItem : public QGraphicsRectItem
{
public:
	RoundedRectItem(const QRectF & rect, QGraphicsItem * parent = 0) : QGraphicsRectItem(rect,parent)
	{
	}
	RoundedRectItem(QGraphicsItem * parent = 0) : QGraphicsRectItem(parent)
	{
	}
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
	{
		painter->setBrush(brush());
		painter->setPen(pen());
		painter->drawRoundedRect(rect(), 5, 5);
	}
};

class WaveItem : public QGraphicsPathItem
{
public:
	WaveItem(QGraphicsItem * parent = 0) : QGraphicsPathItem(parent)
	{
	}
	void setPoints(const QPointF& start, const QPointF& middle, const QPointF& end, qreal w=20.0)
	{
		qreal h = end.y() - start.y();
		QPainterPath p;
		p.moveTo(start);
		if ((middle.x() - w/2.0) > start.x())
			p.lineTo(middle - QPointF(w/2.0,h/2.0));
		else
			p.lineTo(start.x(), middle.y() - h/2.0);
		
		if ((middle.x() + w/2.0) > end.x())
			p.cubicTo(middle - QPointF(0.0,h/2.0),middle + QPointF(0.0,h/2.0),QPointF(end.x(),middle.y() + h/2.0));
		else
			p.cubicTo(middle - QPointF(0.0,h/2.0),middle + QPointF(0.0,h/2.0),middle + QPointF(w/2.0,h/2.0));
		p.lineTo(end);
		p.lineTo(end.x(),start.y());
		p.closeSubpath();
		setPath(p);
	}
};

/*! \brief The main window that provides the interface for constructing a new PartGraphicsItem and saving it*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	MainWindow();
	~MainWindow();

public slots:
	
	void init();
	void frameChanged(int value);
	void getTimeSimFile();
	void getSSFile();
	void setTimeSimCol(int);
	void setSSCol(int);
	void speedChanged(int);
	
private:
	DataTable timeSimulationData;
	DataTable steadyStateData;
	QString timeSimulationFile;
	QString steadyStateFile;
	QGraphicsScene drawScene;
	QGraphicsView graphicsView;
	QTimeLine timer;
	QSlider speedSlider;
	QDockWidget* makeToolBox();
	int simCol, ssCol, dx;
	QList<QGraphicsRectItem*> cells;

	//QGraphicsRectItem stimulantRect;
	WaveItem waveItem;
	
	void readTable(DataTable&, QFile& file);
	void normalize(DataTable&,int);
};

}
#endif
