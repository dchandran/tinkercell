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
#include "CloneItems.h"
#include "GraphicsScene.h"

namespace Tinkercell
{
    bool GraphicsView::checkVisibility(QGraphicsItem * item) const
	{
		return ( (item != 0) && (item->isVisible()) && !(hiddenItems.contains(item)) );
	}

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
			if ((hiddenItems.contains(items[i]) ||
				(scene && networkWindow && networkWindow->currentGraphicsView != this &&
					(&(scene->selectionRect) == items[i] || scene->movingItemsGroup == items[i])))
				&& !Tool::GraphicsItem::cast(items[i]->topLevelItem()))
			{
				items[i] = items[numItems-1];
				items[numItems-1] = 0;
				--i;
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
		: QGraphicsView (network->scene,parent), scene(network->scene), networkWindow(network)
	{
		if (network && !network->graphicsViews.contains(this))
			network->graphicsViews << this;
#if QT_VERSION > 0x046000
		setOptimizationFlag(QGraphicsView::IndirectPainting);
#endif
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
		if (!networkWindow) return;
		QUndoCommand * command = new SetGraphicsViewVisibilityCommand(this,item,true);
		if (networkWindow->graphicsViews.size() > 0 && networkWindow->graphicsViews[0] == this)
			networkWindow->history.push(command);
		else
		{
			command->redo();
			delete command;
		}
	}

	void GraphicsView::hideItem(QGraphicsItem* item)
	{
		if (!networkWindow) return;
		QUndoCommand * command = new SetGraphicsViewVisibilityCommand(this,item,false);

		if (networkWindow->graphicsViews.size() > 0 && networkWindow->graphicsViews[0] == this)
			networkWindow->history.push(command);
		else
		{
			command->redo();
			delete command;
		}
	}

	void GraphicsView::showItems(const QList<QGraphicsItem*>& items)
	{
		if (!networkWindow) return;
		QUndoCommand * command = new SetGraphicsViewVisibilityCommand(this,items,true);
		if (networkWindow->graphicsViews.size() > 0 && networkWindow->graphicsViews[0] == this)
			networkWindow->history.push(command);
		else
		{
			command->redo();
			delete command;
		}
	}

	void GraphicsView::hideItems(const QList<QGraphicsItem*>& items)
	{
		if (!networkWindow) return;
		QUndoCommand * command = new SetGraphicsViewVisibilityCommand(this,items,false);
		if (networkWindow->graphicsViews.size() > 0 && networkWindow->graphicsViews[0] == this)
			networkWindow->history.push(command);
		else
		{
			command->redo();
			delete command;
		}
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

	SetGraphicsViewVisibilityCommand::SetGraphicsViewVisibilityCommand(GraphicsView * view, QGraphicsItem * item, bool show)
	: QUndoCommand(QString("items hidden from view")), view(view), show(show)
	{
		if (show)
			setText(QString("items displayed in view"));
		ConnectionGraphicsItem * connection = 0;
		NodeGraphicsItem * node = 0;

		if (!Tool::GraphicsItem::cast(item->topLevelItem()))
		{
			if (connection = ConnectionGraphicsItem::cast(item))
			{
				items << connection
					  << connection->controlPointsAsGraphicsItems()
					  << connection->arrowHeadsAsGraphicsItems();
			}
			else
			if (node = NodeGraphicsItem::cast(item))
			{
				items << node;

				QList<ControlPoint*> controls = node->allControlPoints();
				for (int j=0; j < controls.size(); ++j)
					items << controls[j];
			}
			else
			if (ControlPoint::cast(item) || TextGraphicsItem::cast(item))
			{
				items << item;
			}
			for (int j=0; j < items.size(); ++j)
				if (items[j])
					items << items[j]->childItems();
		}
	}

	SetGraphicsViewVisibilityCommand::SetGraphicsViewVisibilityCommand(GraphicsView * view, const QList<QGraphicsItem*> & list, bool show)
	: QUndoCommand(QString("items hidden from view")), view(view), show(show)
	{
		if (show)
			setText(QString("items displayed in view"));

		for (int i=0; i < list.size(); ++i)
		{
			if (Tool::GraphicsItem::cast(list[i]->topLevelItem())) continue;

			ConnectionGraphicsItem * connection = 0;
			NodeGraphicsItem * node = 0;

			if (connection = ConnectionGraphicsItem::cast(list[i]))
			{
				items << connection
					  << connection->controlPointsAsGraphicsItems()
					  << connection->arrowHeadsAsGraphicsItems();
			}
			else
			if (node = NodeGraphicsItem::cast(list[i]))
			{
				items << node;

				QList<ControlPoint*> controls = node->allControlPoints();
				for (int j=0; j < controls.size(); ++j)
					items << controls[j];
			}
			else
			if (ControlPoint::cast(list[i]) || TextGraphicsItem::cast(list[i]))
			{
				items << list[i];
			}

			for (int j=0; j < items.size(); ++j)
				if (items[j])
					items << items[j]->childItems();
		}
	}

	void SetGraphicsViewVisibilityCommand::redo()
	{
		if (view)
		{
			for (int i=0; i < items.size(); ++i)
				if (show)
					view->hiddenItems.remove(items[i]);
				else
					view->hiddenItems[ items[i] ] = true;
		}
	}

	void SetGraphicsViewVisibilityCommand::undo()
	{
		if (view)
		{
			for (int i=0; i < items.size(); ++i)
				if (!show)
					view->hiddenItems.remove(items[i]);
				else
					view->hiddenItems[ items[i] ] = true;
		}
	}

}
