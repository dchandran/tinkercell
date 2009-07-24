/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool provides functions for the C API, allowing temporary labels to be placed on top
 of items.
 
****************************************************************************/

#include "CLabelsTool.h"

namespace Tinkercell
{	
	
	CLabelsTool::CLabelsTool() : Tool(tr("C Labels Tool"))
	{
		bgColor = QColor(0,0,0);
		textColor = QColor(10,255,10);
	}
	
	bool CLabelsTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow)
		{
            connect(mainWindow,SIGNAL(windowClosing(NetworkWindow * , bool *)),this,SLOT(sceneClosing(NetworkWindow * , bool *)));
		
			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSignal(const QWidget*)));
			//connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),this,SLOT(itemsSelected(GraphicsScene *,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
			//connect(mainWindow,SIGNAL(keyReleased(GraphicsScene *, QKeyEvent *)),this,SLOT(keyPressed(GraphicsScene*, QKeyEvent *)));
			connect(mainWindow,SIGNAL(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)),this,SLOT(sceneDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)));
			
			
			connect(&fToS,SIGNAL(displayText(ItemHandle*,const QString&)),this,SLOT(displayText(ItemHandle*,const QString&)));
			connect(&fToS,SIGNAL(setLabelColor(int, int, int, int, int, int)),this,SLOT(setDisplayLabelColor(int, int, int, int, int, int)));
			connect(&fToS,SIGNAL(highlightItem(ItemHandle*,QColor)),this,SLOT(highlightItem(ItemHandle*,QColor)));
		}
		return (mainWindow != 0);
	}
	
	typedef void (*tc_CLabelsTool_api)(
		 void (*displayText)(OBJ item,const char*),
		 void (*displayNumber)(OBJ item,double),
		 void (*setDisplayLabelColor)(int r1, int g1, int b1, int r2, int g2, int b2),
		 void (*highlightItem)(OBJ item, int r, int g, int b)
	);
	
	void CLabelsTool::setupFunctionPointers( QLibrary * library )
	{
		tc_CLabelsTool_api f = (tc_CLabelsTool_api)library->resolve("tc_CLabelsTool_api");
		if (f)
		{
			f(
				&(_displayText),
				&(_displayNumber),
				&(_setDisplayLabelColor),
				&(_highlightItem)
			);
		}
	}
	
	void CLabelsTool::sceneDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers modifiers)
	{
		if (!textItems.isEmpty() || !rectItems.isEmpty() || !ellipseItems.isEmpty())
			clearLabels();
	}

	void CLabelsTool::keyPressed(GraphicsScene * scene, QKeyEvent *)
	{
		if (!textItems.isEmpty() || !rectItems.isEmpty() || !ellipseItems.isEmpty())
			clearLabels();
	}
	
	void CLabelsTool::itemsSelected(GraphicsScene *scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers)
	{
		if (!textItems.isEmpty() || !rectItems.isEmpty() || !ellipseItems.isEmpty())
			clearLabels();
	}
	
	void CLabelsTool::escapeSignal(const QWidget*)
	{
		if (!textItems.isEmpty() || !rectItems.isEmpty() || !ellipseItems.isEmpty())
			clearLabels();
	}
	
	void CLabelsTool::sceneClosing(NetworkWindow * , bool *)
	{
		if (!textItems.isEmpty() || !rectItems.isEmpty() || !ellipseItems.isEmpty())
			clearLabels();
	}
	
	void CLabelsTool::clearLabels()
	{
		if (textItems.isEmpty() && rectItems.isEmpty() && ellipseItems.isEmpty()) return;
		
		for (int i=0; i < textItems.size(); ++i)
		{
			if (textItems[i])
			{
				if (textItems[i]->scene())
					textItems[i]->scene()->removeItem(textItems[i]);
				delete textItems[i];
			}
		}
		textItems.clear();
		
		for (int i=0; i < rectItems.size(); ++i)
		{
			if (rectItems[i])
			{
				if (rectItems[i]->scene())
					rectItems[i]->scene()->removeItem(rectItems[i]);
				delete rectItems[i];
			}
		}
		rectItems.clear();
		
		for (int i=0; i < ellipseItems.size(); ++i)
		{
			if (ellipseItems[i])
			{
				if (ellipseItems[i]->scene())
					ellipseItems[i]->scene()->removeItem(ellipseItems[i]);
				delete ellipseItems[i];
			}
		}
		ellipseItems.clear();
	}
	
	void CLabelsTool::displayText(ItemHandle* handle, const QString& text)
	{
		GraphicsScene * scene = currentScene();
		if (!handle || !scene) return;
		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;
		QGraphicsRectItem * rectItem = 0;
		QGraphicsSimpleTextItem * textItem = 0;
		for (int i=0; i < handle->graphicsItems.size(); ++i)
		{
			if (handle->graphicsItems[i])
			{	
				QPointF p;
				if ((node= qgraphicsitem_cast<NodeGraphicsItem*>(handle->graphicsItems[i])))
					p = node->scenePos();				
				else
				{
					if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(handle->graphicsItems[i])))
						p = connection->centerLocation();					
				}
				if (!p.isNull())
				{
					textItem = new QGraphicsSimpleTextItem(text);
					textItem->setBrush(QBrush(textColor));
					textItem->setPos(p);
					QFont font = textItem->font();
					font.setPointSize(24);
					textItem->setFont(font);
					
					QRectF rect = textItem->boundingRect();
					rectItem = new QGraphicsRectItem(p.rx()-1.0,p.ry()-1.0,rect.width()+3.0,rect.height()+3.0);
					rectItem->setBrush(QBrush(bgColor));
					
					scene->addItem(rectItem);
					scene->addItem(textItem);
					rectItem->setZValue(scene->ZValue() + 10.0);
					textItem->setZValue(scene->ZValue() + 20.0);
					
					rectItems << rectItem;
					textItems << textItem;
				}
			}
		}
	}
	
	void CLabelsTool::highlightItem(ItemHandle* handle, QColor color)
	{
		GraphicsScene * scene = currentScene();
		if (!handle || !scene) return;
		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;
		QGraphicsEllipseItem * ellipseItem = 0;
		bool circle = false;
		for (int i=0; i < handle->graphicsItems.size(); ++i)
		{
			if (handle->graphicsItems[i])
			{	
				circle = false;
				QRectF rect;
				if ((node= qgraphicsitem_cast<NodeGraphicsItem*>(handle->graphicsItems[i])))
				{
					rect = node->sceneBoundingRect();
					rect.adjust(-50.0,-50.0,50.0,50.0);
					circle = true;
				}
				else
				{
					if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(handle->graphicsItems[i])))
					{
						rect = connection->sceneBoundingRect();
						circle = true;
					}
				}
				if (circle)
				{
					ellipseItem = new QGraphicsEllipseItem(rect);
					ellipseItem->setPen(QPen(QBrush(color),4,Qt::DashDotLine));
					scene->addItem(ellipseItem);
					ellipseItems << ellipseItem;
				}
			}
		}
	}
	
	void CLabelsTool::setDisplayLabelColor(int r1, int g1, int b1, int r2, int g2, int b2)
	{
		textColor = QColor(r1,g1,b1);
		bgColor = QColor(r2,g2,b2);
		
		for (int i=0; i < textItems.size(); ++i)
		{
			if (textItems[i])
			{
				textItems[i]->setBrush(QBrush(textColor));
			}
		}
		textItems.clear();
		
		for (int i=0; i < rectItems.size(); ++i)
		{
			if (rectItems[i])
			{
				rectItems[i]->setBrush(QBrush(bgColor));
			}
		}
	}
	
	CLabelsTool_FToS CLabelsTool::fToS;
	
	void CLabelsTool::_displayText(OBJ o,const char* c)
	{
		fToS.displayText(o,c);
	}
	
	void CLabelsTool::_displayNumber(OBJ o,double d)
	{
		fToS.displayNumber(o,d);
	}
	
	void CLabelsTool::_setDisplayLabelColor(int r1, int g1, int b1, int r2, int g2, int b2)
	{
		fToS.setDisplayLabelColor(r1,g1,b1,r2,g2,b2);
	}
	
	void CLabelsTool::_highlightItem(OBJ o, int r, int g, int b)
	{
		fToS.highlightItem(o,r,g,b);
	}
	
	void CLabelsTool_FToS::displayText(OBJ o,const char* c)
	{
		emit displayText(ConvertValue(o),ConvertValue(c));
	}
	
	void CLabelsTool_FToS::displayNumber(OBJ o,double d)
	{
		emit displayText(ConvertValue(o),QString::number(d));
	}
	
	void CLabelsTool_FToS::setDisplayLabelColor(int r1, int g1, int b1, int r2, int g2, int b2)
	{
		emit setLabelColor(r1,g1,b1,r2,b2,g2);
	}
	
	void CLabelsTool_FToS::highlightItem(OBJ o, int r, int g, int b)
	{
		emit highlightItem(ConvertValue(o),QColor(r,g,b));
	}
}



