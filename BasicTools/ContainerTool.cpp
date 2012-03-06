/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

This tool sets parent/child relationships for items belonging to the family "Container"

****************************************************************************/

#include "GraphicsScene.h"
#include "ConsoleWindow.h"
#include "MainWindow.h"
#include "NetworkHandle.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "CollisionDetection.h"
#include "ContainerTool.h"
#include "GlobalSettings.h"
#include <QtDebug>

namespace Tinkercell
{
    ContainerTool::ContainerTool() : Tool(tr("Container Tool"),tr("Basic GUI"))
    {
    }

    bool ContainerTool::setMainWindow(MainWindow * main)
    {
        Tool::setMainWindow(main);
        if (mainWindow)
        {
            connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
                    this,SLOT(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));

            connect(mainWindow,SIGNAL(itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<QPointF>&)),
                    this,SLOT(itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<QPointF>&)));

            //connect(mainWindow,SIGNAL(historyChanged(int)),this,SLOT(updateTree(int)));

            //connect(mainWindow,SIGNAL(windowChanged(NetworkWindow*, NetworkWindow*)),
                //    this,SLOT(windowChanged(NetworkWindow*, NetworkWindow*)));

			//connect(mainWindow,SIGNAL(networkClosed(NetworkHandle *)),this,SLOT(windowClosed(NetworkHandle *)));

			connectCollisionDetector();

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

            return true;
        }

        return false;
    }

    void ContainerTool::toolLoaded(Tool*)
    {
        connectCollisionDetector();
    }

    void ContainerTool::connectCollisionDetector()
    {
        static bool alreadyConnected = false;
        if (alreadyConnected || !mainWindow) return;

        if (mainWindow->tool(tr("Collision Detection")))
        {
            QWidget * widget = mainWindow->tool(tr("Collision Detection"));
            CollisionDetection * collisionDetection = static_cast<CollisionDetection*>(widget);
            if (collisionDetection)
            {
                alreadyConnected = true;
                connect(collisionDetection,SIGNAL(nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& )),
                        this, SLOT( nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& )));
            }
        }
    }

    void ContainerTool::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF , Qt::KeyboardModifiers )
    {
        if (!scene || items.isEmpty()) return;

        ItemHandle * handle = 0, *child = 0;
        ConnectionGraphicsItem * connection = 0;

        for (int i=0; i < items.size(); ++i)
        {
            if (NodeGraphicsItem::cast(items[i])
                && (handle = getHandle(items[i]))
                && handle->isA(tr("Container"))
				&& !handle->isA(tr("Empty")))
            {
				QRectF sceneBoundingRect = items[i]->sceneBoundingRect().adjusted(-10,-10,10,10);
                QList<ItemHandle*> list = handle->children;
                for (int j=0; j < list.size(); ++j)
                {
                    if (list[j])
                    {
                        child = list[j];
                        for (int k=0; k < child->graphicsItems.size(); ++k)
				if (child->graphicsItems[k] && child->graphicsItems[k]->scene() == scene)
				{
					if (child->graphicsItems[k] != items[i] &&
						!scene->moving().contains(child->graphicsItems[k]) &&
						sceneBoundingRect.intersects(child->graphicsItems[k]->sceneBoundingRect()))
					{
						if ((connection = ConnectionGraphicsItem::cast(child->graphicsItems[k])))
						{
							QList<QGraphicsItem*> controlPoints = connection->controlPointsAsGraphicsItems();
							for (int l=0; l < controlPoints.size(); ++l)
								if (!scene->moving().contains(controlPoints[l]))
									scene->moving() += controlPoints[l];
						}
						else
						{
							scene->moving() += child->graphicsItems[k];
						}
					}
					list += child->children;
				}
                    }
                }
            }
        }
    }

    bool ContainerTool::connectionInsideRect(ConnectionGraphicsItem* connection, const QRectF& rect, bool all)
    {
        if (!connection) return false;

        QList<QGraphicsItem*> nodes = connection->nodesAsGraphicsItems();

        for (int i=0; i < nodes.size(); ++i)
		if (nodes[i] && connection->scene() == nodes[i]->scene())
		{
		    if (!all && rect.contains(nodes[i]->sceneBoundingRect()))
		        return true;

		    if (all && nodes[i] && !rect.contains(nodes[i]->sceneBoundingRect()))
		        return false;
		}

        return all;
    }

    void ContainerTool::itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>& handleList)
    {
        if (!mainWindow || !scene || !scene->network) return;
        ItemHandle * handle = 0, * parentHandle = 0;

        QList<ItemHandle*> handles, newParents, newChildren, specialCaseChildren, specialCaseParents;

        QList<QGraphicsItem*> itemsAt;

        ConnectionGraphicsItem * connection;

        for (int i=0; i < items.size(); ++i)
            if (connection = ConnectionGraphicsItem::cast(items[i]))
            {
		        QList<NodeGraphicsItem*> nodes = connection->nodes();

		        for (int j=0; j < nodes.size(); ++j)
		            if (nodes[j])
		            {
				        handle = nodes[j]->handle();
				        if (handle)
				        {
				            parentHandle = handle->parentOfFamily(tr("Container"));
				            if (parentHandle && !handles.contains(parentHandle))
				            {
				                handles << parentHandle;
				            }
				        }
				    }
		    }

        for (int i=0; i < handleList.size(); ++i)
        {
            handle = handleList[i];

            if (handle && handle->family() && !handles.contains(handle) &&
                handle->family()->isA(tr("Container")) &&
				!handle->family()->isA(tr("Empty")))
                handles << handle;
        }

        ItemHandle * child = 0;

        QList<QGraphicsItem*> itemsToRename;
        QList<QString> newNames;

        QString temp;

        for (int k=0; k < handles.size(); ++k)
        {
            newChildren.clear();
            itemsToRename.clear();
            newNames.clear();

            handle = handles[k];
            if (!handle) continue;

            bool contained = true, contained0 = true;
            QRectF childRect;

            for (int i=0; i < items.size(); ++i)
            {
                child = getHandle(items[i]);

                if (child && child != handle && !handle->children.contains(child))
                {
                    contained = true;

                    for (int j=0; j < child->graphicsItems.size(); ++j)
                    {
                        if (TextGraphicsItem::cast(child->graphicsItems[j])) continue;

                        connection = ConnectionGraphicsItem::cast(child->graphicsItems[j]);
                        childRect = child->graphicsItems[j]->sceneBoundingRect();
                        contained0 = false;

                        for (int l=0; l < handle->graphicsItems.size(); ++l)
							if (handle->graphicsItems[l] && handle->graphicsItems[l]->scene() == child->graphicsItems[j]->scene())
							{
								if (connection)
								{
									if (connectionInsideRect(connection,handle->graphicsItems[l]->sceneBoundingRect()))
									{
										contained0 = true;
										break;
									}
									else
										if (connectionInsideRect(connection,handle->graphicsItems[l]->sceneBoundingRect(),false))
										{
											if (child && !specialCaseChildren.contains(child))
											{
												specialCaseChildren += child;
												specialCaseParents += handle;
											}
										}
								}
								else
									if (handle->graphicsItems[l]->sceneBoundingRect().contains(childRect))
									{
										contained0 = true;
										break;
									}
							}
                        if (!contained0)
                        {
                            contained = false;
                            break;
                        }
                    }

                    if (contained)
                    {
                        newChildren += child;
                        newParents += handle;
                        temp = child->name;
                        temp = scene->network->makeUnique(temp);
                        if (temp != child->name)
                        {
                            itemsToRename += items[i];
                            newNames += handle->fullName() + tr(".") + temp;
                        }
                    }
                }
            }
        }

        if (!newChildren.isEmpty())
            scene->network->setParentHandle(newChildren,newParents);
		
        newChildren << specialCaseChildren;
        newParents << specialCaseParents;

        /*if (!newChildren.isEmpty())
        {
            QList<ItemHandle*> parentItems;
            while (parentItems.size() < newChildren.size()) parentItems << handle;

            adjustRates(scene, newChildren, parentItems);
        }*/
    }

    void ContainerTool::nodeCollided(const QList<QGraphicsItem*>& movingItems0, NodeGraphicsItem * nodeHit, const QList<QPointF>& )
    {
        if (!mainWindow || !nodeHit) return;

        GraphicsScene * scene = mainWindow->currentScene();

        if (!scene || !scene->network) return;

        ItemHandle * handle = getHandle(nodeHit);

        //if items are placed into a Compartment or module...
        if (!handle || !handle->isA(tr("Container")) || handle->isA(tr("Empty")))
            return;

		ConnectionGraphicsItem::ControlPoint * cpt = 0;
        QList<ItemHandle*> newChildren;
        ItemHandle * child = 0;

        QList<QGraphicsItem*> itemsToRename;
        QList<QString> newNames;

        QString temp;

        bool stillWithParent;

        NodeGraphicsItem * node = 0;

        QList<QGraphicsItem*> movingItems;

		for (int i=0; i < movingItems0.size(); ++i)
            if (node = NodeGraphicsItem::cast(movingItems0[i]))
				movingItems << node;

		if (movingItems.isEmpty())
		{
			QList<QGraphicsItem*> insersectingItems = scene->items(nodeHit->sceneBoundingRect());
			for (int i=0; i < insersectingItems.size(); ++i)
				if ((node = NodeGraphicsItem::cast(insersectingItems[i])) && !movingItems.contains(node))
					movingItems << node;
		}

        for (int i=0; i < movingItems.size(); ++i)
            if (node = NodeGraphicsItem::cast(movingItems[i]))
            {
				movingItems << node->connectionsAsGraphicsItems();
			}

        ConnectionGraphicsItem* connection;
        QRectF hitRect = nodeHit->sceneBoundingRect();

        for (int i=0; i < movingItems.size(); ++i)
        {
            if (!movingItems[i]) continue;

            QRectF itemRect = movingItems[i]->sceneBoundingRect();
            if (itemRect.width() >= hitRect.width() || itemRect.height() >= hitRect.height() || !hitRect.contains(itemRect.center())) continue;

            if (TextGraphicsItem::cast(movingItems[i])) continue;

			connection = ConnectionGraphicsItem::cast(movingItems[i]);

            if (connection && !hitRect.contains(connection->sceneBoundingRect())) continue;

            if ((cpt = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(movingItems[i])) &&
                (cpt->connectionItem) &&
                (hitRect.contains(cpt->connectionItem->sceneBoundingRect())))
				{
					child = getHandle(cpt->connectionItem);
				}
				else
				{
					child = getHandle(movingItems[i]);
				}
            if (child && child != handle && !handle->isA(tr("Empty")) && !handle->children.contains(child) && !handle->isChildOf(child)
				/*&& ((child->isA("part") && handle->isA("part")) || !handle->isA("part"))*/) //special case for parts
            {
                stillWithParent = false;
                
                if (child->parent && !stillWithParent)
                {
	                for (int j=0; j < child->graphicsItems.size(); ++j)
	                {
	                    if (child->graphicsItems[j])
						{
							for (int k=0; k < child->parent->graphicsItems.size(); ++k)
							{
								if (child->parent->graphicsItems[k] &&
									child->parent->graphicsItems[k]->sceneBoundingRect().contains(child->graphicsItems[j]->sceneBoundingRect()))
								{
									stillWithParent = true;
									break;
								}
							}
						}
	                    if (stillWithParent)
	                        break;
                	}
                }

                if (!stillWithParent)
                {
                    newChildren += child;
                    temp = child->name;
                    temp = scene->network->makeUnique(temp);
                    if (temp != child->name)
                    {
                        itemsToRename += movingItems[i];
                        newNames += handle->fullName() + tr(".") + temp;
                    }
                }
            }
        }

        if (handle)
        {
            if (!newChildren.isEmpty())
            {
                scene->network->setParentHandle(newChildren,handle);

                /*QList<ItemHandle*> parentItems;
                while (parentItems.size() < newChildren.size()) parentItems << handle;

                adjustRates(scene, newChildren, parentItems);*/
            }
            sendToBack(nodeHit,scene);
        }
    }

    void ContainerTool::sendToBack(QGraphicsItem* item, GraphicsScene * scene)
    {
        if (item && scene)
        {
            double z = 0;

            QList<QGraphicsItem*> targetItems;
            QList<double> zvalues;

            QRectF rect = item->sceneBoundingRect();
            QList<QGraphicsItem*> items = scene->items(rect);
            items.removeAll(item);
            if (item->parentItem())
                items.removeAll(item->parentItem());

            for (int i=0; i < items.size(); ++i)
                if (items[i] != 0 && (z == 0 || items[i]->zValue() < z))
                    z = items[i]->topLevelItem()->zValue();

            if (item->zValue() > z)
            {
                z -= 0.2;
                targetItems += item;
                zvalues += z;
                scene->setZValue(tr("send back"),targetItems,zvalues);
            }
        }
    }
/*
    void ContainerTool::itemsRemoved(GraphicsScene * scene, const QList<QGraphicsItem*>&, const QList<ItemHandle*>& handles)
    {
        if (!mainWindow || !scene || !scene->network) return;

        QList<ItemHandle*> children;
        QList<ItemHandle*> newParents;
        ItemHandle * child = 0;

        QList<QGraphicsItem*> itemsToRename;
        QList<QString> newNames;

        QString temp;

        //if items are placed in or out of a Compartment or module...

        QList<QGraphicsItem*> items;

        for (int i=0; i < handles.size(); ++i)
        {
            if (handles[i])
            {
                for (int j=0; j < handles[i]->children.size(); ++j)
                    if (handles[i]->children[j])
                        items += handles[i]->children[j]->graphicsItems;
            }
        }

        for (int i=0; i < items.size(); ++i)
        {
            if ((child = getHandle(items[i])))
            {
                children += child;
                newParents += 0;
            }
        }

        if (!children.isEmpty() && !newParents.isEmpty())
        {
            scene->network->setParentHandle(children,newParents);		
			adjustRates(scene, children, newParents);
        }
        
    }
*/

    void ContainerTool::moveChildItems(GraphicsScene * scene, const QList<QGraphicsItem*> & items0, const QList<QPointF> & dist)
    {
        QList<QGraphicsItem*> connections;
        ConnectionGraphicsItem::ControlPoint * cp;

        for (int i=0; i < items0.size(); ++i)
        {
            if ((cp = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(items0[i]))
                && cp->connectionItem && !connections.contains(cp->connectionItem))
                connections << cp->connectionItem;
        }

        QList<QGraphicsItem*> items;
        QList<QPointF> points;

        ItemHandle * handle, * child;
        QList<ItemHandle*> visited;
        
        for (int i=0; i < items0.size(); ++i)
        {
            if (NodeGraphicsItem::cast(items0[i]) &&
                (handle = getHandle(items0[i])) && 
				!visited.contains(handle) &&
                handle->isA(tr("Container")) &&
				!handle->isA(tr("Empty")) &&
                handle->children.size() > 0)
            {
                visited += handle;

                QList<QGraphicsItem*> insideBox;
                QGraphicsItem * tcItem;

                QRectF rect = items0[i]->sceneBoundingRect();
                rect.adjust(-dist[i].x(),-dist[i].y(),-dist[i].x(),-dist[i].y());
                insideBox = scene->items(rect);

                for (int k=0; k < insideBox.size(); ++k)
                {
                    if ( (tcItem = getGraphicsItem(insideBox[k])) &&
                         tcItem->sceneBoundingRect().width() < rect.width() &&
                         tcItem->sceneBoundingRect().height() < rect.height() &&
                         (child = getHandle(tcItem)) &&
                         child != handle &&
                         !items0.contains(tcItem) && !items.contains(tcItem) && !connections.contains(tcItem))
                    {
                        items << tcItem;
                        points << dist[i];
                    }
                }

            }
        }
        if (items.size() > 0)
        {
            scene->move(items,points);
        }
    }

    void ContainerTool::itemsMoved(GraphicsScene * scene, const QList<QGraphicsItem*>& items0, const QList<QPointF>& dist)
    {
        if (!mainWindow || !scene || !scene->network) return;

        QList<ItemHandle*> children;
        QList<ItemHandle*> newParents;

        ItemHandle * child = 0, * parent = 0, * handle = 0;
         bool outOfBox;

	 	 QList<ItemHandle*> movedChildNodes, movedCompartmentNodes;

        QList<QGraphicsItem*> itemsToRename;
        QList<QString> newNames;

        QString temp;

        ConnectionGraphicsItem::ControlPoint * cp = 0;
        NodeGraphicsItem * node = 0;

        QList<QGraphicsItem*> items = items0;
        for (int i=0; i < items0.size(); ++i)
        {
            if (node = NodeGraphicsItem::cast(items0[i]))
            {
                items << node->connectionsAsGraphicsItems();
            }
        }

        //if items are placed in or out of a Compartment or module...
        QList<ItemHandle*> visitedHandles;
        for (int i=0; i < items.size(); ++i)
        {
            if (TextGraphicsItem::cast(items[i])) continue;

          handle = getHandle(items[i]);

			if (!handle || !handle->family() || visitedHandles.contains(handle)) continue;

			visitedHandles << handle;

            if (node = NodeGraphicsItem::cast(items[i]))
            {
                items << node->connectionsAsGraphicsItems();
            }

			if (handle->parent && handle->parent->isA(tr("Container")) && !handle->parent->isA(tr("Empty")))
				movedChildNodes << handle;
			else
				if (handle->isA(tr("Container")) && !handle->isA(tr("Empty")))
				{
					movedCompartmentNodes << handle;
					movedChildNodes << handle->children;
				}
		}

		for (int i=0; i < movedCompartmentNodes.size(); ++i)
			for (int j=0; j < movedCompartmentNodes[i]->graphicsItems.size(); ++j)
				if (node = NodeGraphicsItem::cast(movedCompartmentNodes[i]->graphicsItems[j]))
					nodeCollided(QList<QGraphicsItem*>(),node,QList<QPointF>());


		for (int i=0; i < movedChildNodes.size(); ++i)
		{
			child = movedChildNodes[i];
			if (child->graphicsItems.isEmpty()) continue;

            outOfBox = true;

            if (outOfBox)
				for (int j=0; j < child->parent->graphicsItems.size(); ++j) //is the item still inside the Compartment/module?
					if (child->parent->graphicsItems[j])
					{
						//QPainterPath p1 = child->parent->graphicsItems[j]->mapToScene(child->parent->graphicsItems[j]->shape());
						QRectF p1 = child->parent->graphicsItems[j]->sceneBoundingRect().adjusted(-5,-5,5,5);
						for (int k=0; k < child->graphicsItems.size(); ++k)
							if (child->graphicsItems[k])
							{
								//QPainterPath p2 = child->graphicsItems[k]->mapToScene(child->graphicsItems[k]->shape());
								QRectF p2 = child->graphicsItems[k]->sceneBoundingRect();
								if (p1.intersects(p2) || p1.contains(p2))
								{
									outOfBox = false; //yes, still contained inside the module/Compartment
									break;
								}
							}
						if (!outOfBox) break;
					}
			if (outOfBox)
			{
				children += child;
				newParents += 0;
			}
        }

        if (!children.isEmpty() && !newParents.isEmpty())
        {
            scene->network->setParentHandle(children,newParents);
        }
    }

}

