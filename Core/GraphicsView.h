/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines the GraphicsView class that is used to view the contents
of a GraphicsScene. The class inherits from QGraphicsView. 

****************************************************************************/

#ifndef TINKERCELL_GRAPHICSVIEW_H
#define TINKERCELL_GRAPHICSVIEW_H

#include <stdlib.h>
#include <QtGui>
#include <QString>
#include <QPair>
#include <QFileDialog>
#include <QtDebug>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAction>
#include <QPixmap>
#include <QMenu>
#include <QFile>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QHash>
#include <QUndoCommand>
#include <QGraphicsItemAnimation>
#include <QPrinter>

#include "DataTable.h"
#include "HistoryWindow.h"
#include "SymbolsTable.h"
#include "CloneItems.h"

#ifdef Q_WS_WIN
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{
	class SetGraphicsViewVisibilityCommand;

	class NodeGraphicsItem;
	class ConnectionGraphicsItem;
	class ItemHandle;
	class ItemData;
	class NetworkWindow;
	class GraphicsScene;

	/*! \brief GraphicsView class that is used to view the contents 
				of a GraphicsScene. The class inherits from QGraphicsView. 
		\ingroup core
	*/
	class TINKERCELLEXPORT GraphicsView : public QGraphicsView
	{
		Q_OBJECT
	
	private:
		/*! \brief background */
		QPixmap background;
		/*! \brief foreground */
		QPixmap foreground;
		/*! \brief the scene displayed by this view */
		GraphicsScene * scene;
		/*! \brief default size*/
		virtual QSize sizeHint() const;
		/*! \brief default constructor
		*	\param NetworkWindow * window that this view belongs with
		*	\param QWidget * parent
		*/
		GraphicsView(NetworkWindow * networkWindow = 0);		

	signals:
		/*! \brief signal is emitted when some object OTHER than files are dropped on the canvas*/
		void itemsDropped(GraphicsScene*, const QString&, const QPointF&);
	protected:
		/*! \brief draw background*/
		virtual void drawBackground( QPainter * painter, const QRectF & rect );
		/*! \brief draw foreground*/
		virtual void drawForeground( QPainter * painter, const QRectF & rect );
		/*! \brief drag and drop */
		virtual void dropEvent(QDropEvent *);
		/*! \brief drag and drop*/
		virtual void dragEnterEvent(QDragEnterEvent *event);
		/*! \brief drag and drop*/
		virtual void dragMoveEvent(QDragMoveEvent *event);
		/*! \brief mouse wheel event*/
		virtual void wheelEvent(QWheelEvent * event);
		/*! \brief scroll event*/
		virtual void scrollContentsBy ( int dx, int dy );
		/*! \brief mouse event. sets the currentGraphicsView for NetworkWindow*/
		virtual void mousePressEvent ( QMouseEvent * event );
		/*! \brief mouse event. sets the currentGraphicsView for NetworkWindow*/
		virtual void keyPressEvent ( QKeyEvent * event );

		friend class GraphicsScene;
		friend class NetworkWindow;
		friend class NetworkHandle;
		friend class MainWindow;
	};
}

#endif
