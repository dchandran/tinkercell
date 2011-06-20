/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This class performs some housekeeping operations for selecting and moving connection items.
Connection items cannot be moved by themselves since their location is defined by the control
points.

****************************************************************************/

#ifndef TINKERCELL_CONNECTIONSELECTIONTOOL_H
#define TINKERCELL_CONNECTIONSELECTIONTOOL_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
#include <QSemaphore>
#include <QPixmap>
#include <QString>
#include <QtDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAction>
#include <QFile>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QGraphicsRectItem>
#include <QTimeLine>

#include "ConnectionGraphicsItem.h"
#include "ItemHandle.h"
#include "NetworkHandle.h"
#include "Tool.h"
#include "MainWindow.h"

namespace Tinkercell
{

	/*! \brief This class listens to selection events from the scene and responds by changing colors
	of the selected objects and displaying tools and widgets that point to that object.
	\ingroup plugins
	*/
	class ConnectionSelection : public Tool
	{
		Q_OBJECT

	public:
		/*!\brief Constructor - does nothing
		*/
		ConnectionSelection();
		/*!\brief set main window. connects all the event functions.
		\param the main window
		\return success or failure
		*/
		bool setMainWindow(MainWindow * main);

	signals:
		/*! \brief signal sent to main window when a center box is inserted*/
		void itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<ItemHandle*>& handles);
		
		void substituteNodeGraphics();

	public slots:
		void select(int);
		/*! \brief signal sent to main window when a center box is inserted*/
		void itemsInsertedSlot(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<ItemHandle*>& handles);

		/*! \brief event that reponds to scene double-clicks. It an item is selected, then scene is centered on the item
		\param the scene that was clicked on
		\param point clicked on
		\param graphics item clicked on (can be null)
		\param the mouse button used for the click
		\param the keyboard modifiers used
		\return nothing
		*/
		void sceneDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		/*! \brief event that reponds to key pressed. If excape, items are deselected
		\param the scene where key was pressed
		\param the keyboard modifiers used
		\return nothing
		*/
		void sceneKeyPressed(GraphicsScene *scene, QKeyEvent*);
		
		void toolLoaded(Tool*);

		void sceneKeyReleased(GraphicsScene *scene, QKeyEvent*);
		/*! \brief event that reponds to items moved.
		\param the scene that was clicked on
		\param the list of items underneath mouse
		\param point moved from
		\param point moved to
		\param modifier keys used
		\return nothing
		*/
		void itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>&);

		void mouseMoved(GraphicsScene * scene, QGraphicsItem* item, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers, QList<QGraphicsItem*>&);

		void nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& );

		void connectionCollided(const QList<QGraphicsItem*>& , ConnectionGraphicsItem * , QPointF );

		void itemsRemoved(GraphicsScene * , QList<QGraphicsItem*>& , QList<ItemHandle*>& , QList<QUndoCommand*>&);

		void setLineType(int);
		void showMiddleBox(int, const QString& filename = QString());
		void showMiddleBox();
		void hideMiddleBox();
		void arrowHeadDistance(double value);
		void newControlPoint();
		void setLineTypeStraight();
		void setLineTypeCurved();

	private slots:
		void escapeSignal(const QWidget*);

	protected:
		/*!\brief change control points of moved connection items (may not be needed for most connectors)*/
		void adjustConnectorPoints(const QList<QGraphicsItem*>& movingItems);
		void connectCollisionDetector();
		bool controlHeld;
		qreal gridDist;

	private:
		QList<ConnectionGraphicsItem*> temporarilyChangedConnections;

	};

}

#endif
