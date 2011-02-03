/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This class sends a signal whenever two items in the current scene intersect.

****************************************************************************/

#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "CollisionDetection.h"
#include "NodeSelection.h"

namespace Tinkercell
{

	CollisionDetection::CollisionDetection() : Tool(tr("Collision Detection"),tr("Basic GUI"))
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

			connect(mainWindow,SIGNAL(itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>&)),
				this ,SLOT(itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>&)));

			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				this,SLOT(itemsSelected(GraphicsScene *,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
		}

		return false;
	}

	void CollisionDetection::itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>&)
	{
		
		if (!(nodeBelowCursor != 0 || connectionBelowCursor != 0))
		{
			QPointF & p = scene->lastPoint();
			QRectF rect(p - QPointF(20,20), p + QPointF(20,20));
			QList<QGraphicsItem*> existingItems = scene->items(rect);
			
			for (int i=0; i < existingItems.size(); ++i)
				if ((nodeBelowCursor = NodeGraphicsItem::cast(existingItems[i])) && !items.contains(nodeBelowCursor))
					break;
				else
				{
					nodeBelowCursor = 0;
					if ((connectionBelowCursor = ConnectionGraphicsItem::cast(existingItems[i])) && !items.contains(connectionBelowCursor))
						break;
					else
						connectionBelowCursor = 0;
				}
		}

		if ((nodeBelowCursor && nodeBelowCursor->scene()==scene) || 
			 (connectionBelowCursor && connectionBelowCursor->scene()==scene))
		{
			if (nodeBelowCursor != 0 && !ToolGraphicsItem::cast(nodeBelowCursor->topLevelItem()))
				emit nodeCollided(items,nodeBelowCursor,QList<QPointF>());
			else
				if (connectionBelowCursor != 0 && !ToolGraphicsItem::cast(connectionBelowCursor->topLevelItem()))
					emit connectionCollided(items,connectionBelowCursor,QList<QPointF>());
		}
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
			{
				if (alpha < 150)
					nodeBelowCursor->setAlpha(alpha);
				else
					nodeBelowCursor->setAlpha(300 - alpha);
			}
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
			bool isControlPoint;

			if (item == 0 && selected.size() == 1 && selected[0])
			{
				QPainterPath path = selected[0]->mapToScene(selected[0]->shape());
				QList<QGraphicsItem*> itemsNearby = scene->items(path);
				NodeGraphicsItem* itemHit = 0;
				
				for (int i=0; i < itemsNearby.size(); ++i)
				{
					itemHit = NodeGraphicsItem::topLevelNodeItem(itemsNearby[i]);
					if (itemHit && 
						 itemHit->handle() && 
						!movingItems.contains(itemHit) && 
						!selected.contains(itemHit))
					{
						isControlPoint = false;
						for (int i=0; i < itemHit->boundaryControlPoints.size(); ++i)
							if (movingItems.contains(itemHit->boundaryControlPoints[i]))
							{
								isControlPoint = true;
								break;
							}
						if (!isControlPoint)
						{
							item = itemHit;
							break;
						}
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
				
				if (selected.contains(nodeBelowCursor2))
					nodeBelowCursor2 = 0;

				if (nodeBelowCursor2)
				{
					for (int i=0; i < nodeBelowCursor2->boundaryControlPoints.size(); ++i)
						if (movingItems.contains(nodeBelowCursor2->boundaryControlPoints[i]))
						{
							nodeBelowCursor2 = 0;
							break;
						}
				}

				if (nodeBelowCursor2 != 0)
				{
					if (nodeSelectionTool &&
						(nodeSelectionTool->selectedNodes.contains(nodeBelowCursor2) ||
						nodeSelectionTool->selectedHandleNodes.contains(nodeBelowCursor2)))
						nodeBelowCursor2 = 0;
					else
					{
						/*
						ItemHandle * handle = getHandle(nodeBelowCursor2);
						for (int i=0; i < nodeSelectionTool->selectedNodes.size(); ++i)
						{
							ItemHandle * handle2 = getHandle(nodeSelectionTool->selectedNodes[i]);
							if (handle2 && handle2->isChildOf(handle))
							{
								nodeBelowCursor2 = 0;
								break;
							}
						}*/
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
						/*
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
							*/
					}
				}

				if ((nodeBelowCursor2 != 0 || connectionBelowCursor2 != 0) &&
					(nodeBelowCursor2 != nodeBelowCursor || connectionBelowCursor2 != connectionBelowCursor))
				{
					if (glowTimer.state() != QTimeLine::NotRunning)
						glowTimer.stop();
					
					connectionBelowCursor = connectionBelowCursor2;
					nodeBelowCursor = nodeBelowCursor2;
					
					glowTimer.stop();
					glowTimer.setFrameRange(50,250);

					glowTimer.setDirection(QTimeLine::Backward);
					//glowTimer.setUpdateInterval(100);
					glowTimer.setDuration(2000);
					glowTimer.setLoopCount(0);
					
					if (connectionBelowCursor || nodeBelowCursor)
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

	void CollisionDetection::itemsMoved(GraphicsScene *,const QList<QGraphicsItem*>& movingItems, const QList<QPointF>& distance)
	{
		if (nodeBelowCursor != 0 || connectionBelowCursor != 0)
		{
			if (nodeBelowCursor != 0 && !ToolGraphicsItem::cast(nodeBelowCursor->topLevelItem()))
			{
				emit nodeCollided(movingItems,nodeBelowCursor,distance);
			}
			else
				if (connectionBelowCursor != 0 && !ToolGraphicsItem::cast(connectionBelowCursor->topLevelItem()))
					emit connectionCollided(movingItems,connectionBelowCursor,distance);
		}
	}

}

