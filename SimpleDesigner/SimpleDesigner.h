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
#include <QListWidget>
#include <QGraphicsSimpleTextItem>
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "NetworkWindow.h"
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "Tool.h"
#include "NodeGraphicsReader.h"
#include "ConsoleWindow.h"

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
	RoundRectItem * rectItem;

	SimpleNode(QGraphicsItem * g = 0) : NodeGraphicsItem(g)
	{
		textItem = new QGraphicsSimpleTextItem;
		textItem->scale(2.0,2.0);
		
		rectItem = new RoundRectItem;
		rectItem->rect = QRectF(-50.0,-30.0,100,60);
		
		addShape(rectItem);
		addToGroup(textItem);
		
		refresh();
		recomputeBoundingRect();
		
		textItem->setPos(-0.5*textItem->boundingRect().width(),-0.5*textItem->boundingRect().height());
	}
	
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
	{
		NodeGraphicsItem::paint(painter, option, widget);
		if (textItem && itemHandle)
		{
			textItem->setText(itemHandle->name);
		}
	}
	
	NodeGraphicsItem* clone() const
	{
		SimpleNode * node = new SimpleNode;
		node->rectItem->rect = rectItem->rect;
		RoundRectItem * rect = new RoundRectItem;
		node->refresh();
		node->recomputeBoundingRect();
		node->setPos(scenePos());
		node->adjustBoundaryControlPoints();
		return node;
	}

};


class SimpleDesigner : public Tool
{
	Q_OBJECT
	
public:
	SimpleDesigner();

	bool setMainWindow(MainWindow*);

public slots:

	void nameChanged();
	
	void rateChanged();
	
	void concentrationChanged();
	
	void itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
	
	void itemsInserted(NetworkWindow* , const QList<ItemHandle*>& handles);
	
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
	QGroupBox * groupBox3;
	QListWidget * listWidget;
	QLineEdit * name1;
	QLineEdit * name2;
	QLineEdit * conc;
	QLineEdit * rate;
	QAction * arrowButton;
	
	QList<QGraphicsItem*> selectedItems;
	
	void selectItem(GraphicsScene * scene, QGraphicsItem * item, bool select = true);
	void deselectItem(GraphicsScene * scene, QGraphicsItem * item);
	void addParameters(QStringList&);
	void setToolTip(ItemHandle*);
};
