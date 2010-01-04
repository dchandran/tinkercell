/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This is one of the main classes in Tinkercell
This file defines the GraphicsView class that is used to view the contents
of a GraphicsScene. The class inherits from QGraphicsView. The main capability
this class provides is the ability to show/hide items in this view without
affecting other views.

****************************************************************************/

#include "NetworkWindow.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ItemHandle.h"
#include "UndoCommands.h"
#include "ConsoleWindow.h"
#include "GraphicsScene.h"

namespace Tinkercell
{
	void GraphicsView::drawBackground( QPainter * painter, const QRectF & rect )
	{
		if (!background.isNull() && painter)
			painter->drawPixmap(rect,background,QRectF(background.rect()));
	}
	
	void GraphicsView::drawForeground( QPainter * painter, const QRectF & rect )
	{
		if (!foreground.isNull() && painter)
			painter->drawPixmap(rect,foreground,QRectF(foreground.rect()));
	}
	
	void GraphicsView::drawItems(QPainter *painter, int numItems, QGraphicsItem *items[], const QStyleOptionGraphicsItem options[])
	{
		for (int i=0; i < numItems; ++i)
		{
			if (hiddenItems.contains(items[i]))
			{
				items[i] = items[numItems-1];
				--numItems;
			}
		}
		QGraphicsView::drawItems(painter,numItems,items,options);
	}
	
	void GraphicsView::wheelEvent(QWheelEvent * wheelEvent)
	{
		if (wheelEvent->modifiers() & Qt::ControlModifier)
		{
			double factor = 1.0 + 0.2 * qAbs(wheelEvent->delta()/120.0);
			if (wheelEvent->delta() > 0)
				scale(factor,factor);
			else
				scale(1.0/factor,1.0/factor);
		}
		else
		{
			QGraphicsView::wheelEvent(wheelEvent);
		}
	}
	
	void GraphicsView::closeEvent(QCloseEvent *event)
	{
		if (networkWindow)
			networkWindow->graphicsViews.removeAll(this);
	}

	void GraphicsView::scrollContentsBy ( int dx, int dy )
	{
		QGraphicsView::scrollContentsBy(dx,dy);
		if (viewport())
			viewport()->update();
	}

	void GraphicsView::dragEnterEvent(QDragEnterEvent * /*event*/)
	{
		//event->acceptProposedAction();
	}

	void GraphicsView::dropEvent(QDropEvent * /*event*/)
	{
		/*if (parentWidget())
		{
		QList<QUrl> urlList;
		QList<QFileInfo> files;
		QString fName;
		QFileInfo info;

		if (event->mimeData()->hasUrls())
		{
		urlList = event->mimeData()->urls(); // returns list of QUrls

		// if just text was dropped, urlList is empty (size == 0)
		if ( urlList.size() > 0) // if at least one QUrl is present in list
		{
		fName = urlList[0].toLocalFile(); // convert first QUrl to local path
		info.setFile( fName ); // information about file
		if ( info.isFile() )
		files += info;
		}
		}
		MainWindow * main = static_cast<MainWindow*>(parentWidget());
		main->dragAndDropFiles(files);
		}*/
	}
	/*! \brief Constructor: connects all the signals of the new window to that of the main window */
	GraphicsView::GraphicsView(NetworkWindow * network, QWidget * parent)
		: QGraphicsView (network->scene,parent), scene(scene), networkWindow(network)
	{
		if (network)
			network->graphicsViews << this;

		setCacheMode(QGraphicsView::CacheBackground);

		//setViewportUpdateMode (QGraphicsView::FullViewportUpdate);
		//setViewportUpdateMode (QGraphicsView::SmartViewportUpdate);
		setViewportUpdateMode (QGraphicsView::BoundingRectViewportUpdate);
		//setViewport(new QGLWidget);
		//setDragMode(QGraphicsView::RubberBandDrag);
		//setDragMode(QGraphicsView::ScrollHandDrag);
		//setOptimizationFlags(QGraphicsView::DontClipPainter | QGraphicsView::DontSavePainterState);

		setPalette(QPalette(QColor(255,255,255)));
		setAutoFillBackground(true);
		setAcceptDrops(true);

		setRenderHint(QPainter::Antialiasing);
		setCacheMode(QGraphicsView::CacheBackground);
		setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
		fitInView(QRectF(0,0,1000,800),Qt::KeepAspectRatio);
		
		setFocusPolicy(Qt::StrongFocus);
	}
	
	void GraphicsView::showItem(QGraphicsItem* item)
	{
		hiddenItems.remove(item);
	}
	
	void GraphicsView::hideItem(QGraphicsItem* item)
	{
		if (!hiddenItems.contains(item))
			hiddenItems.insert(item,true);
	}
	
	void GraphicsView::showItems(const QList<QGraphicsItem*>& items)
	{
		for (int i=0; i < items.size(); ++i)
			hiddenItems.remove(items[i]);
	}
	
	void GraphicsView::hideItems(const QList<QGraphicsItem*>& items)
	{
		for (int i=0; i < items.size(); ++i)
			hiddenItems.insert(items[i],true);
	}
	
	void GraphicsView::mousePressEvent ( QMouseEvent * event )
	{
		if (networkWindow)
			networkWindow->currentGraphicsView = this;
		QGraphicsView::mousePressEvent(event);
	}
	
	void GraphicsView::keyPressEvent ( QKeyEvent * event )
	{
		if (networkWindow)
			networkWindow->currentGraphicsView = this;
		QGraphicsView::keyPressEvent(event);
	}

}
