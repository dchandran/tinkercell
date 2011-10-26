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
#include <QInputDialog>
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "NetworkHandle.h"
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "Tool.h"
#include "ConsoleWindow.h"
#include "PlotTool.h"

using namespace Tinkercell;

class SimpleNode: public NodeGraphicsItem
{
public:

	QGraphicsSimpleTextItem * textItem;

	SimpleNode() : NodeGraphicsItem(":/images/node.xml")
	{
		textItem = new QGraphicsSimpleTextItem;
		textItem->scale(2.0,2.0);
		addToGroup(textItem);
		
		refresh();
		recomputeBoundingRect();
		
		textItem->setPos(-boundingRect().width()/4.0,-boundingRect().height()/2.0);
		textItem->setZValue(1.0);
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
	
	void itemsInserted(NetworkHandle* , const QList<ItemHandle*>& handles);
	
	void escapeSignal(const QWidget * sender);
	
	void mousePressed(GraphicsScene * , QPointF , Qt::MouseButton, Qt::KeyboardModifiers);

private slots:
	void actionTriggered(QAction*);
	void parameterItemActivated ( QListWidgetItem * item );
	void ode();
	void ssa();
	void simulate(bool stochastic);

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
	PlotTool * plotTool;
	
	QList<QGraphicsItem*> selectedItems;
	
	void selectItem(GraphicsScene * scene, QGraphicsItem * item, bool select = true);
	void deselectItem(GraphicsScene * scene, QGraphicsItem * item);
	void addParameters(QStringList&);
	void setToolTip(ItemHandle*);
};

