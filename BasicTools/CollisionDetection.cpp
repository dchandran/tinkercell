/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This class sends a signal whenever two items in the current scene intersect.

****************************************************************************/

#include "Core/GraphicsScene.h"
#include "Core/UndoCommands.h"
#include "Core/MainWindow.h"
#include "Core/NodeGraphicsItem.h"
#include "Core/ConnectionGraphicsItem.h"
#include "Core/TextGraphicsItem.h"
#include "BasicTools/CollisionDetection.h"
#include "BasicTools/NodeSelection.h"

namespace Tinkercell
{

    CollisionDetection::CollisionDetection() : Tool(tr("Collision Detection"))
    {
        mainWindow = 0;
        connect(&glowTimer,SIGNAL(frameChanged(int)),this,SLOT(makeNodeGlow(int)));
        connect(&glowTimer,SIGNAL(stateChanged(QTimeLine::State)),this,SLOT(stopGlow(QTimeLine::State)));
        nodeBelowCursor = 0;
        connectionBelowCursor = 0;
    }

    bool CollisionDetection::setMainWindow(MainWindow * main)
    {
        Tool::setMainWindow(main);
        if (mainWindow)
        {
            connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
                    this,SLOT(itemsInserted(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));

            connect(mainWindow,SIGNAL(mouseMoved(GraphicsScene *, QGraphicsItem *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)),
                    this,SLOT(sceneMouseMoved(GraphicsScene *, QGraphicsItem *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)));

            connect(mainWindow,SIGNAL(itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)),
                    this ,SLOT(itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)));

            connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
                    this,SLOT(itemsSelected(GraphicsScene *,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));

        }

        return false;
    }

    void CollisionDetection::itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>&)
    {
        if (nodeBelowCursor != 0 || connectionBelowCursor != 0)
        {
            if (nodeBelowCursor != 0 && !qgraphicsitem_cast<Tool::GraphicsItem*>(nodeBelowCursor->topLevelItem()))
                emit nodeCollided(items,nodeBelowCursor,QList<QPointF>(),0);
            else
                if (connectionBelowCursor != 0 && !qgraphicsitem_cast<Tool::GraphicsItem*>(connectionBelowCursor->topLevelItem()))
                    emit connectionCollided(items,connectionBelowCursor,QList<QPointF>(),0);
        }
        /*
                if (scene)
                {
                        QList<QGraphicsItem*> itemsAt;
                        NodeGraphicsItem * node = 0;
                        ConnectionGraphicsItem * connection = 0;
                        for (int i=0; i < items.size(); ++i)
                        {
                                //QRectF rect;
                                QPainterPath path;
                                if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i])))
                                {
                                        QList<NodeGraphicsItem*> nodes = connection->nodes();
                                        for (int j=0; j < nodes.size(); ++j)
                                                if (nodes[j])
                                                        path = path.united(nodes[j]->mapToScene(nodes[j]->shape()));
                                }
                                //itemsAt = scene->items(rect);
                                itemsAt = scene->items(path);
                                QRectF rect = path.boundingRect();
                                for (int j=0; j < itemsAt.size(); ++j)
                                {
                                        if (itemsAt[j]->sceneBoundingRect().contains(rect))
                                        {
                                                if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(itemsAt[j])))
                                                        emit nodeCollided(QList<QGraphicsItem*>() << connection, node, QPointF(), 0);
                                        }
                                }
                        }
                }*/
    }

    void CollisionDetection::stopGlow(QTimeLine::State state)
    {
        if (state != QTimeLine::NotRunning) return;

        GraphicsScene * scene = currentScene();
        if (scene && scene->selected().contains(nodeBelowCursor))
            nodeBelowCursor = 0;

        if (nodeBelowCursor)
        {
            for (int i=0; i < nodeBelowCursor->shapes.size(); ++i)
            {
                NodeGraphicsItem::Shape * shape = nodeBelowCursor->shapes[i];
                if (shape != 0)
                {
                    shape->setPen(shape->defaultPen);
                    shape->setBrush(shape->defaultBrush);
                }
            }
        }

        if (scene && scene->selected().contains(connectionBelowCursor))
            connectionBelowCursor = 0;
        if (connectionBelowCursor)
        {
            connectionBelowCursor->setPen(connectionBelowCursor->defaultPen);
        }

        nodeBelowCursor = 0;
        connectionBelowCursor = 0;
    }

    void CollisionDetection::makeNodeGlow(int alpha)
    {
        if (connectionBelowCursor)
        {
            QPen pen = QPen(connectionBelowCursor->pen());
            QColor color = pen.color();
            if (alpha < 150)
                color.setAlpha(alpha);
            else
                color.setAlpha(300 - alpha);
            pen.setColor(color);
            connectionBelowCursor->setPen(pen);
        }
        else
            if (nodeBelowCursor)
                if (alpha < 150)
                    nodeBelowCursor->setAlpha(alpha);
        else
            nodeBelowCursor->setAlpha(300 - alpha);
    }

    void CollisionDetection::itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>& , QPointF , Qt::KeyboardModifiers )
    {
        glowTimer.stop();
        nodeBelowCursor = 0;
        connectionBelowCursor = 0;
    }


    void CollisionDetection::sceneMouseMoved(GraphicsScene * scene, QGraphicsItem *item, QPointF , Qt::MouseButton , Qt::KeyboardModifiers, QList<QGraphicsItem*>& movingItems)
    {
        static NodeSelection * nodeSelectionTool = 0;
        if (!nodeSelectionTool && mainWindow)
        {
            if (mainWindow->tool(tr("Node Selection")))
                nodeSelectionTool = static_cast<NodeSelection*>(mainWindow->tool(tr("Node Selection")));
        }
        if (scene && nodeSelectionTool)
        {
            QList<QGraphicsItem*>& selected = scene->selected();
            if (selected.contains(nodeBelowCursor))
                nodeBelowCursor = 0;

            if (selected.contains(connectionBelowCursor))
                connectionBelowCursor = 0;

            if (item == 0 && selected.size() == 1 && selected[0])
            {
                //QRectF rect = selected[0]->sceneBoundingRect();
                QPainterPath path = selected[0]->mapToScene(selected[0]->shape());
                //rect.adjust(-1,-1,1,1);
                QList<QGraphicsItem*> itemsNearby = scene->items(path);
                NodeGraphicsItem* itemHit = 0;
                for (int i=0; i < itemsNearby.size(); ++i)
                {
                    itemHit = NodeGraphicsItem::topLevelNodeItem(itemsNearby[i]);
                    if (itemHit && itemHit->itemHandle && !movingItems.contains(itemHit) && !selected.contains(itemHit))
                    {
                        item = itemHit;
                        break;
                    }
                }
            }

            if (item == 0)
            {
                if (glowTimer.state() != QTimeLine::NotRunning)
                    glowTimer.stop();
            }
            else
            {
                NodeGraphicsItem * nodeBelowCursor2 = NodeGraphicsItem::topLevelNodeItem(item);

                if (nodeBelowCursor2 != 0)
                {
                    if (nodeSelectionTool &&
                        (nodeSelectionTool->selectedNodes.contains(nodeBelowCursor2) ||
                         nodeSelectionTool->selectedHandleNodes.contains(nodeBelowCursor2)))
                        nodeBelowCursor2 = 0;
                    else
                    {
                        /*if (!movingItems.isEmpty() && nodeBelowCursor2->sceneBoundingRect().contains(scene->lastPoint()))
                                                        nodeBelowCursor2 = 0;*/
                        ItemHandle * handle = getHandle(nodeBelowCursor2);
                        for (int i=0; i < nodeSelectionTool->selectedNodes.size(); ++i)
                        {
                            ItemHandle * handle2 = getHandle(nodeSelectionTool->selectedNodes[i]);
                            if (handle2 && handle2->isChildOf(handle))
                            {
                                nodeBelowCursor2 = 0;
                                break;
                            }
                        }
                    }
                }

                ConnectionGraphicsItem * connectionBelowCursor2 = 0;
                if (nodeBelowCursor2 == 0)
                    connectionBelowCursor2 = ConnectionGraphicsItem::topLevelConnectionItem(item);
                else
                    connectionBelowCursor2 = 0;

                if (connectionBelowCursor2 != 0)
                {
                    if (nodeSelectionTool &&
                        (nodeSelectionTool->selectedConnections.contains(connectionBelowCursor2)))
                        connectionBelowCursor2 = 0;
                    else
                    {
                        /*if (!movingItems.isEmpty() && connectionBelowCursor2->pathShape.contains(scene->lastPoint()))
                                                        connectionBelowCursor2 = 0;*/
                        ItemHandle * handle = getHandle(connectionBelowCursor2);
                        for (int i=0; i < nodeSelectionTool->selectedNodes.size(); ++i)
                        {
                            ItemHandle * handle2 = getHandle(nodeSelectionTool->selectedNodes[i]);
                            if (handle2 && handle2->isChildOf(handle))
                            {
                                connectionBelowCursor2 = 0;
                                break;
                            }
                        }
                        if (connectionBelowCursor2)
                            for (int i=0; i < nodeSelectionTool->selectedConnections.size(); ++i)
                            {
                            ItemHandle * handle2 = getHandle(nodeSelectionTool->selectedConnections[i]);
                            if (handle2 && handle2->isChildOf(handle))
                            {
                                connectionBelowCursor2 = 0;
                                break;
                            }
                        }
                    }
                }

                if ((nodeBelowCursor2 != 0 || connectionBelowCursor2 != 0) &&
                    (nodeBelowCursor2 != nodeBelowCursor || connectionBelowCursor2 != connectionBelowCursor))
                {
                    if (glowTimer.state() != QTimeLine::NotRunning)
                    {
                        glowTimer.stop();
                    }

                    connectionBelowCursor = connectionBelowCursor2;
                    nodeBelowCursor = nodeBelowCursor2;
                    glowTimer.stop();
                    glowTimer.setFrameRange(50,250);
                    glowTimer.setDirection(QTimeLine::Backward);
                    //glowTimer.setUpdateInterval(100);
                    glowTimer.setDuration(2000);
                    glowTimer.setLoopCount(0);
                    glowTimer.start();
                }
                else
                {
                    if (nodeBelowCursor2 == 0 && connectionBelowCursor2 == 0)
                        glowTimer.stop();
                }
            }
        }
        else
        {
            if (glowTimer.state() != QTimeLine::NotRunning)
                glowTimer.stop();
        }
    }

    void CollisionDetection::itemsMoved(GraphicsScene *,const QList<QGraphicsItem*>& movingItems, const QList<QPointF>& distance, Qt::KeyboardModifiers modifiers)
    {
        if (nodeBelowCursor != 0 || connectionBelowCursor != 0)
        {
            if (nodeBelowCursor != 0 && !qgraphicsitem_cast<Tool::GraphicsItem*>(nodeBelowCursor->topLevelItem()))
            {
                emit nodeCollided(movingItems,nodeBelowCursor,distance,modifiers);
            }
            else
                if (connectionBelowCursor != 0 && !qgraphicsitem_cast<Tool::GraphicsItem*>(connectionBelowCursor->topLevelItem()))
                    emit connectionCollided(movingItems,connectionBelowCursor,distance,modifiers);
        }
    }

}

