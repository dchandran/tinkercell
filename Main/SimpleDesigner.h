/****************************************************************************

 Copyright (C) 2009 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYWRITE.TXT

This is an example application that uses the TinkerCell Core library
****************************************************************************/

#include <QButtonGroup>
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QToolButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QGraphicsSimpleTextItem>
#include "Core/NodeGraphicsItem.h"
#include "Core/ConnectionGraphicsItem.h"
#include "Core/NetworkWindow.h"
#include "Core/GraphicsScene.h"
#include "Core/MainWindow.h"
#include "Core/Tool.h"
#include "Core/NodeGraphicsReader.h"
#include "Core/OutputWindow.h"

using namespace Tinkercell;

class RoundRectItem : public NodeGraphicsItem::Shape
{
public:
	QRectF rect;
	
	QRectF boundingRect() const
	{
		return rect;
	}
	
	virtual QPainterPath shape() const
	{
		QPainterPath path;
		path.addRect(rect);
		return path;
	}
	
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
	{	
		if (painter)
		{
			painter->setPen(pen());
			painter->setBrush(brush());
			painter->drawRoundedRect(rect, 20.0, 20.0, Qt::AbsoluteSize);
		}
	}
};

class SimpleNode: public NodeGraphicsItem
{
public:

	QGraphicsSimpleTextItem * textItem;

	SimpleNode(QGraphicsItem * g = 0) : NodeGraphicsItem(g)
	{
		textItem = new QGraphicsSimpleTextItem;
		textItem->scale(2.0,2.0);
		RoundRectItem * rect = new RoundRectItem;
		rect->rect = QRectF(-50.0,-30.0,100,60);
		addShape(rect);
		addToGroup(textItem);
		textItem->setPos(-2.0*textItem->boundingRect().width(),-2.0*textItem->boundingRect().height());
		refresh();
		recomputeBoundingRect();
	}
	
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
	{
		NodeGraphicsItem::paint(painter, option, widget);
		if (textItem && itemHandle)
		{
			textItem->setText(itemHandle->name);
		}
	}

};


class SimpleDesigner : public Tool
{
	Q_OBJECT
	
public:
	SimpleDesigner();

	bool setMainWindow(MainWindow*);

public slots:
	
	void itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
	
	void escapeSignal(const QWidget * sender);
	
	void mousePressed(GraphicsScene * , QPointF , Qt::MouseButton, Qt::KeyboardModifiers);

private slots:
	void actionTriggered(QAction*);

private:
	int mode;
	QToolBar * toolBar;
	QActionGroup * actionGroup;
	QGroupBox * groupBox1;
	QGroupBox * groupBox2;
	QLineEdit * name1;
	QLineEdit * name2;
	QLineEdit * conc;
	QLineEdit * rate;
	QAction * arrowButton;
	QList<QGraphicsItem*> selectedItems;
};
