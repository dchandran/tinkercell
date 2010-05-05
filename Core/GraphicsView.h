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
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
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
	class MY_EXPORT GraphicsView : public QGraphicsView
	{
		Q_OBJECT
	
	public:
		/*! \brief background */
		QPixmap background;
		/*! \brief foreground */
		QPixmap foreground;
		/*! \brief the scene displayed by this view */
		GraphicsScene * scene;
		/*! \brief adjusts the view to fit all items*/
		virtual void fitAll();
		/*! \brief default size*/
		virtual QSize sizeHint() const;
		/*! \brief the network window that this view belongs to */
		NetworkWindow * networkWindow;
		/*! \brief default constructor
		*	\param NetworkWindow * window that this view belongs with
		*	\param QWidget * parent
		*/
		GraphicsView(NetworkWindow * networkWindow = 0);		

	protected:
		/*! \brief draw background*/
		virtual void drawBackground( QPainter * painter, const QRectF & rect );
		/*! \brief draw foreground*/
		virtual void drawForeground( QPainter * painter, const QRectF & rect );
		/*! \brief draw all items not in the hiddenItems list (obsolete)*/
		virtual void drawItems(QPainter *painter, int numItems,
                           QGraphicsItem *items[],
                           const QStyleOptionGraphicsItem options[]);
		/*! \brief drag on top event */
		virtual void dropEvent(QDropEvent *);
		/*! \brief drag and drop event*/
		virtual void dragEnterEvent(QDragEnterEvent *event);
		/*! \brief mouse wheel event*/
		virtual void wheelEvent(QWheelEvent * event);
		/*! \brief scroll event*/
		virtual void scrollContentsBy ( int dx, int dy );
		/*! \brief mouse event. sets the currentGraphicsView for NetworkWindow*/
		virtual void mousePressEvent ( QMouseEvent * event );
		/*! \brief mouse event. sets the currentGraphicsView for NetworkWindow*/
		virtual void keyPressEvent ( QKeyEvent * event );
		/*! \brief list of items to hide*/
		QHash<QGraphicsItem*,bool> hiddenItems;
		/*! \brief hide the items in the hide list before painting*/
		//virtual void paintEvent(QPaintEvent *event);

		friend class GraphicsScene;
		friend class NetworkHandle;
	};
}

#endif
