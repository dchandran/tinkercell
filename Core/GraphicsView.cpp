/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

The GraphicsView class provides a view for a GraphicsScene. It 

****************************************************************************/

#include <QMimeData>
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
#include "NetworkHandle.h"
#include "GraphicsScene.h"
#include "GraphicsView.h"

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

	void GraphicsView::wheelEvent(QWheelEvent * wheelEvent)
	{
		if (scene && wheelEvent->modifiers() & Qt::ControlModifier)
		{
			double factor = 1.0 + 0.2 * qAbs(wheelEvent->delta()/120.0);
			if (wheelEvent->delta() > 0)
				scale(factor,factor);
			else
				scale(1.0/factor,1.0/factor);
			
			scene->scaleGraphicalTools();
		}
		else
		{
			QGraphicsView::wheelEvent(wheelEvent);
		}
	}

	void GraphicsView::scrollContentsBy ( int dx, int dy )
	{
		QGraphicsView::scrollContentsBy(dx,dy);
		if (scene) 
			scene->update();
	}
	
	QSize GraphicsView::sizeHint() const
	{
		return QSize(500,500);
	}

	/*! \brief Constructor: connects all the signals of the new window to that of the main window */
	GraphicsView::GraphicsView(NetworkWindow * network)
		: QGraphicsView (network->scene,network), scene(network->scene)
	{		
		setCacheMode(QGraphicsView::CacheBackground);
		setViewportUpdateMode (QGraphicsView::BoundingRectViewportUpdate);

		//setViewportUpdateMode (QGraphicsView::FullViewportUpdate);
		//setViewportUpdateMode (QGraphicsView::SmartViewportUpdate);
		//setViewport(new QGLWidget);
		//setDragMode(QGraphicsView::RubberBandDrag);
		//setDragMode(QGraphicsView::ScrollHandDrag);
		//setOptimizationFlags(QGraphicsView::DontClipPainter | QGraphicsView::DontSavePainterState);

		setMouseTracking (true);
		setPalette(QPalette(QColor(255,255,255)));
		setAutoFillBackground(true);
		setAcceptDrops(true);

		setRenderHint(QPainter::Antialiasing);
		setCacheMode(QGraphicsView::CacheBackground);
		setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
		fitInView(QRectF(0,0,20,20),Qt::KeepAspectRatio);
		QPointF center(scene->sceneRect().width()/2, scene->sceneRect().height()/2);
		scene->lastPoint() = center;
		centerOn( center);

		setFocusPolicy(Qt::StrongFocus);
	}

	void GraphicsView::mousePressEvent ( QMouseEvent * event )
	{
		if (scene && scene->networkWindow)
		{
			scene->networkWindow->setAsCurrentWindow();
		}
		
		QGraphicsView::mousePressEvent(event);
	}

	void GraphicsView::keyPressEvent ( QKeyEvent * event )
	{
		if (scene && scene->networkWindow)
		{
			scene->networkWindow->setAsCurrentWindow();
		}
		QGraphicsView::keyPressEvent(event);
	}
	
	void GraphicsView::dragEnterEvent(QDragEnterEvent *event)
	{
		event->accept();
	}
	
	void GraphicsView::dragMoveEvent(QDragMoveEvent *event)
	{
	}

	void GraphicsView::dropEvent(QDropEvent * event)
	{
		QList<QUrl> urlList;
		QList<QFileInfo> files;
		QString fName;
		QFileInfo info;
		
		const QMimeData * mimeData = event->mimeData();
		
		if (mimeData->hasUrls())
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
		
		if (!files.isEmpty() && scene && scene->network && scene->network->mainWindow)
		{
			event->accept();
			scene->network->mainWindow->loadFiles(files);
		}
		else
		{
			QString text = mimeData->text();
			if (!text.isNull() && !text.isEmpty())
			{
				scene->lastPoint() = mapToScene(event->pos());
				emit itemsDropped(scene, text, scene->lastPoint());
			}
		}
	}

}
