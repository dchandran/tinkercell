/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 Whenever a new connection item is created, this class adds control points to the item
 so that it looks appealing

****************************************************************************/

#ifndef TINKERCELL_CONNECTIONMAKERTOOL_H
#define TINKERCELL_CONNECTIONMAKERTOOL_H

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
#include <QUndoCommand>

#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "ItemHandle.h"
#include "CollisionDetection.h"
#include "Tool.h"

namespace Tinkercell
{

    /*!
\brief This class creates the default layout for connections. If this tool
is not included, it must be replaced with another layout tool, because
the connections insertions do not create a default layout.
*/
    class ConnectionMaker : public Tool
    {
        Q_OBJECT

    public:

        /*! \brief constructor */
        ConnectionMaker();
        /*! \brief adds this tool to TinkerCell */
        bool setMainWindow(MainWindow * main);

    public slots:

        /*! \brief does nothing currently */
        void nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>&  , Qt::KeyboardModifiers );
        /*! \brief does nothing currently */
        void connectionCollided(const QList<QGraphicsItem*>& , ConnectionGraphicsItem * itemCollided , const QList<QPointF>& , Qt::KeyboardModifiers );
        /*! \brief if connectors are inserted and they are not valid, then this function will provide a new layout for them */
        void itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>& handles);

    public:
        CollisionDetection * collisionDetection;
        /*! \brief makes all the individual segments leading to each node from the middle segment*/
        static void makeSegments(GraphicsScene*,ConnectionGraphicsItem * connection, const QList<NodeGraphicsItem*>& nodes, int inputs);
        /*! \brief makes the middle segment for a connector */
        static void setupMiddleSegment(ConnectionGraphicsItem * connection, const QList<NodeGraphicsItem*>& nodes, int inputs);
        /*! \brief mix problems occurring when there one node appears multiple times in the same connector */
        static void FixMultipleConnections(ConnectionGraphicsItem * connection, const QList<NodeGraphicsItem*>& nodes, int inputs);
    };

}

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif
