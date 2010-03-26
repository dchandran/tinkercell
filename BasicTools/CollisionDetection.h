/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This class sends a signal whenever two items in the current scene intersect.

****************************************************************************/

#ifndef TINKERCELL_COLLISIONDETECTIONTOOL_H
#define TINKERCELL_COLLISIONDETECTIONTOOL_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
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
#include <QGraphicsWidget>
#include <QFrame>

#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "ItemHandle.h"
#include "Tool.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	/*! \brief This class monitors the movement of items on a GraphicsScene and
	emits a signals whenever one or more items are moved on top of another.
	It it designed to be used by other tools that would need to respond to
	such collision events.
	This plug-in also makes items on the screen glow when the mouse is on top.
	\ingroup plugins
	*/
	class MY_EXPORT CollisionDetection : public Tool
	{
		Q_OBJECT

	public:
		/*! \brief default constructor */
		CollisionDetection();
		/*! \brief set the TinkerCell main window
		\param MainWindow* main window*/
		bool setMainWindow(MainWindow * main);

		public slots:

			/*! \brief monitors mouse moved to check whether the mouse is on top of an item.
			The item is made to "glow" using a QTimeLine */
			void sceneMouseMoved(GraphicsScene *, QGraphicsItem *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&);
			/*! \brief checks whether a collision has occured when items have moved */
			void itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers);
			/*! \brief stops the "glowing" effect when items are selected*/
			void itemsSelected(GraphicsScene *scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
			/*! \brief stops the "glowing" effect when items are inserted*/
			void itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<ItemHandle*>& handles);

	signals:
			/*!
			\brief two or more items have been moved on top of a node item
			\param QList<QGraphicsItem*> the moving items
			\param NodeGraphicsItem* the target node that was hit by the moving items
			\param QList<QPointF> the moving items' locations
			\param Qt::KeyboardModifiers keyboard modifiers
			*/
			void nodeCollided(const QList<QGraphicsItem*>& movingNodes, NodeGraphicsItem * collidedNode, const QList<QPointF>& movingItemsPos, Qt::KeyboardModifiers );
			/*!
			\brief two or more items have been moved on top of a connections item
			\param QList<QGraphicsItem*> the moving items
			\param ConnectionGraphicsItem* the target connections that was hit by the moving items
			\param QList<QPointF> the moving items' locations
			\param Qt::KeyboardModifiers keyboard modifiers
			*/
			void connectionCollided(const QList<QGraphicsItem*>& , ConnectionGraphicsItem * , const QList<QPointF>& , Qt::KeyboardModifiers );

	private slots:
		/*!
		\brief used to make items "glow" using a QTimeLine
		\param int the time line status
		*/
		void makeNodeGlow(int alpha);
		/*!
		\brief stops the "glow" effect in response to QTimeLine
		\param int the time line status
		*/
		void stopGlow(QTimeLine::State);
	private:
		/*!\brief timer used to make objects glow (change transparency)*/
		QTimeLine glowTimer;
		/*!\brief the node that is under the mouse that is "glowing"*/
		NodeGraphicsItem * nodeBelowCursor;
		/*!\brief the connection that is under the mouse that is "glowing"*/
		ConnectionGraphicsItem * connectionBelowCursor;
	};

}

#endif
