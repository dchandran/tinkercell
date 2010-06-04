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
            connect(mainWindow,SIGNAL(windowClosing(NetworkHandle * , bool *)),this,SLOT(networkClosing(NetworkHandle * , bool *)));
		
			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
			
			//connect(mainWindow,SIGNAL(historyChanged(int)),this,SLOT(historyChanged(int)));
			
			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSignal(const QWidget*)));
			//connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),this,SLOT(itemsSelected(GraphicsScene *,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
			//connect(mainWindow,SIGNAL(keyReleased(GraphicsScene *, QKeyEvent *)),this,SLOT(keyPressed(GraphicsScene*, QKeyEvent *)));
			connect(mainWindow,SIGNAL(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)),this,SLOT(sceneDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)));
			
			
			connect(&fToS,SIGNAL(displayText(ItemHandle*,const QString&)),this,SLOT(displayText(ItemHandle*,const QString&)));
			connect(&fToS,SIGNAL(setLabelColor(QColor,QColor)),this,SLOT(setDisplayLabelColor(QColor,QColor)));
			connect(&fToS,SIGNAL(highlightItem(ItemHandle*,QColor)),this,SLOT(highlightItem(ItemHandle*,QColor)));
		}
		return (mainWindow != 0);
	}
	
	typedef void (*tc_CLabelsTool_api)(
		 void (*displayText)(void* item,String),
		void (*displayNumber)(void* item,double),
		void (*setDisplayLabelColor)(const char *, const char *),
		void (*highlight)(void*,const char*)
	);
	
	void CLabelsTool::historyChanged( int )
	{
		escapeSignal(0);
	}
	
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
		if (!currentScene() || !currentScene()->useDefaultBehavior) 
			return;
		if (!textItems.isEmpty() || !rectItems.isEmpty() || !ellipseItems.isEmpty())
			clearLabels();
	}
	
	void CLabelsTool::networkClosing(NetworkHandle * , bool *)
	{
		if (!textItems.isEmpty() || !rectItems.isEmpty() || !ellipseItems.isEmpty())
			clearLabels();
	}
	
	void CLabelsTool::clearLabels(ItemHandle * h)
	{
		if (textItems.isEmpty() && rectItems.isEmpty() && ellipseItems.isEmpty()) return;
		
		for (int i=0; i < textItems.size(); ++i)
		{
			if (textItems[i].second && (!h || textItems[i].first==h))
			{
				if (textItems[i].second->scene())
					textItems[i].second->scene()->removeItem(textItems[i].second);
				delete textItems[i].second;
				textItems[i].second = 0;
			}
		}
		
		for (int i=0; i < rectItems.size(); ++i)
		{
			if (rectItems[i].second && (!h || rectItems[i].first==h))
			{
				if (rectItems[i].second->scene())
					rectItems[i].second->scene()->removeItem(rectItems[i].second);
				delete rectItems[i].second;
				rectItems[i].second = 0;
			}
		}
		
		for (int i=0; i < ellipseItems.size(); ++i)
		{
			if (ellipseItems[i].second && (!h || ellipseItems[i].first==h))
			{
				if (ellipseItems[i].second->scene())
					ellipseItems[i].second->scene()->removeItem(ellipseItems[i].second);
				delete ellipseItems[i].second;
				ellipseItems[i].second = 0;
			}
		}
		
		if (!h)
		{
			textItems.clear();
			rectItems.clear();
			ellipseItems.clear();
		}
	}
	
	void CLabelsTool::displayText(ItemHandle* handle, const QString& text)
	{
		clearLabels(handle);
		
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
					
					rectItems << QPair<ItemHandle*,QGraphicsRectItem*>(handle,rectItem);
					textItems << QPair<ItemHandle*,QGraphicsSimpleTextItem*>(handle,textItem);
				}
			}
		}
	}
	
	void CLabelsTool::highlightItem(ItemHandle* handle, QColor color)
	{
		clearLabels(handle);
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
					ellipseItems << QPair<ItemHandle*,QGraphicsEllipseItem*>(handle,ellipseItem);
				}
			}
		}
	}
	
	void CLabelsTool::setDisplayLabelColor(QColor textColor,QColor bgColor)
	{	
		for (int i=0; i < textItems.size(); ++i)
		{
			if (textItems[i].second)
			{
				textItems[i].second->setBrush(QBrush(textColor));
			}
		}
		textItems.clear();
		
		for (int i=0; i < rectItems.size(); ++i)
		{
			if (rectItems[i].second)
			{
				rectItems[i].second->setBrush(QBrush(bgColor));
			}
		}
	}
	
	CLabelsTool_FToS CLabelsTool::fToS;
	
	void CLabelsTool::_displayText(void* o,const char* c)
	{
		fToS.displayText(o,c);
	}
	
	void CLabelsTool::_displayNumber(void* o,double d)
	{
		fToS.displayNumber(o,d);
	}
	
	void CLabelsTool::_setDisplayLabelColor(const char * a, const char * b)
	{
		fToS.setDisplayLabelColor(a,b);
	}
	
	void CLabelsTool::_highlightItem(void* o, const char * c)
	{
		fToS.highlightItem(o,c);
	}
	
	void CLabelsTool_FToS::displayText(void* o,const char* c)
	{
		emit displayText(ConvertValue(o),ConvertValue(c));
	}
	
	void CLabelsTool_FToS::displayNumber(void* o,double d)
	{
		emit displayText(ConvertValue(o),QString::number(d));
	}
	
	void CLabelsTool_FToS::setDisplayLabelColor(const char* c1, const char* c2)
	{
		emit setLabelColor(QColor(tr(c1)), QColor(tr(c2)));
	}
	
	void CLabelsTool_FToS::highlightItem(void* o, const char* c)
	{
		emit highlightItem(ConvertValue(o),QColor(tr(c)));
	}
}



