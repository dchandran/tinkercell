/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This class performs some housekeeping operations for selecting and moving connection items.
Connection items cannot be moved by themselves since their location is defined by the control
points.

****************************************************************************/
#include <math.h>
#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "ConnectionSelection.h"
#include "CollisionDetection.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "NodeGraphicsWriter.h"
#include "GraphicsReplaceTool.h"

namespace Tinkercell
{

	ConnectionSelection::ConnectionSelection() : Tool(tr("Connection Selection"),tr("Basic GUI"))
	{
		mainWindow = 0;
		controlHeld = false;
		gridDist = 100.0;

		addAction(QIcon(":/images/blueDot.png"),tr("Add control point"),tr("Insert new control point"));
		addAction(QIcon(),tr("Use straight lines"),tr("Use straight lines to draw the selected connectors"));
		addAction(QIcon(),tr("Use Bezier lines"),tr("Use Bezier curves to draw the selected connectors"));
		addAction(QIcon(),tr("Show middle region"),tr("Show information box for connector(s)"));
		addAction(QIcon(),tr("Hide middle region"),tr("Hide information box for connector(s)"));
	}
	
	void ConnectionSelection::select(int i)
	{
		if (i==0)
			newControlPoint();
		else
		if (i==1)
			setLineTypeStraight();
		else
		if (i==2)
			setLineTypeCurved();
		else
		if (i==3)
			showMiddleBox();
		else
		if (i==4)
			hideMiddleBox();
	}

	bool ConnectionSelection::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSignal(const QWidget*)));
			
			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			connect(mainWindow,SIGNAL(keyPressed(GraphicsScene *, QKeyEvent*)),
				this,SLOT(sceneKeyPressed(GraphicsScene *, QKeyEvent*)));

			connect (mainWindow,SIGNAL(itemsAboutToBeRemoved(GraphicsScene*, QList<QGraphicsItem*>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)),
				this,SLOT(itemsRemoved(GraphicsScene*, QList<QGraphicsItem*>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)));

			connect(mainWindow,SIGNAL(keyReleased(GraphicsScene *, QKeyEvent*)),
				this,SLOT(sceneKeyReleased(GraphicsScene *, QKeyEvent*)));

			connect(mainWindow,SIGNAL(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)),
				this,SLOT(sceneDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(this, SIGNAL(itemsInserted(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
				mainWindow, SIGNAL(itemsInserted(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));
			
			connect(mainWindow, SIGNAL(itemsInserted(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
				this, SLOT(itemsInsertedSlot(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>&)),
				this ,SLOT(itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>&)));

			connect(mainWindow,SIGNAL(mouseMoved(GraphicsScene*, QGraphicsItem*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)),
				this,SLOT(mouseMoved(GraphicsScene*, QGraphicsItem*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)));

			connectCollisionDetector();

			return true;
		}

		return false;
	}
	
	void ConnectionSelection::itemsInsertedSlot(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>& handles)
	{
		for (int i=0; i < handles.size(); ++i)
			if (handles[i] && ConnectionHandle::cast(handles[i]) && !handles[i]->tools.contains(this))
				handles[i]->tools += this;
	}

	void ConnectionSelection::toolLoaded(Tool*)
	{
		connectCollisionDetector();
	}

	void ConnectionSelection::connectCollisionDetector()
	{
		static bool alreadyConnected = false;
		if (alreadyConnected || !mainWindow) return;

		/*if (mainWindow->tool(tr("Collision Detection")))
		{
			QWidget * widget = mainWindow->tool(tr("Collision Detection"));
			CollisionDetection * collisionDetection = static_cast<CollisionDetection*>(widget);
			if (collisionDetection)
			{
				alreadyConnected = true;
				connect(collisionDetection,SIGNAL(nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& , Qt::KeyboardModifiers )),
					this, SLOT( nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& , Qt::KeyboardModifiers )));
			}
		}*/
		
		if (mainWindow->tool(tr("Graphics Replace Tool")))
		{
			alreadyConnected = true;
			GraphicsReplaceTool * replaceTool = static_cast<GraphicsReplaceTool*>(mainWindow->tool(tr("Graphics Replace Tool")));
			connect(this,SIGNAL(substituteNodeGraphics()),replaceTool,SLOT(substituteNodeGraphics()));
		}
	}

	void ConnectionSelection::newControlPoint()
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !scene->network) return;

		QPointF point = scene->lastPoint();
		QList<QGraphicsItem*>& selected = scene->selected();

		for (int i=0; i < selected.size(); ++i)
		{
			ConnectionGraphicsItem * item = ConnectionGraphicsItem::cast(selected[i]);
			if (item)
			{
				ConnectionGraphicsItem::ControlPoint * cp = new ConnectionGraphicsItem::ControlPoint(item);
				cp->setPos(point);
				scene->addItem(cp);
				scene->network->push(new AddControlPointCommand("control point added",scene,cp));
			}
		}
	}

	void ConnectionSelection::sceneDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem * qitem, Qt::MouseButton , Qt::KeyboardModifiers modifiers)
	{
		if (scene)
		{
			ConnectionGraphicsItem * item = ConnectionGraphicsItem::cast(qitem);
			if (item && modifiers == Qt::ControlModifier)
			{
				ConnectionGraphicsItem::ControlPoint * cp = new ConnectionGraphicsItem::ControlPoint(item);
				cp->setPos(point);
				scene->addItem(cp);
				if (scene->network)
					scene->network->push(new AddControlPointCommand("control point added",scene,cp));
			}
			else
			{
				NodeGraphicsItem * node = NodeGraphicsItem::cast(qitem);
				if (ArrowHeadItem::cast(node))
				{
					ArrowHeadItem * arrow = static_cast<ArrowHeadItem*>(node);
					if (arrow->connectionItem && arrow->connectionItem->centerRegionItem != arrow)
					{
						scene->selected().clear();
						scene->selected() += node;
						emit substituteNodeGraphics();
					}
				}
			}
		}
	}

	void ConnectionSelection::adjustConnectorPoints(const QList<QGraphicsItem*>& movingItems)
	{
		QList<ConnectionGraphicsItem*> visited;
		for (int i=0; i < movingItems.size(); ++i)
		{
			ConnectionGraphicsItem::ControlPoint * cp = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(movingItems[i]);
			if (cp != 0 && cp->connectionItem != 0 && !visited.contains(cp->connectionItem))
			{
				if (cp->connectionItem->lineType == ConnectionGraphicsItem::line)
				{
					QPointF closest, p1,p2,p;
					p1 = cp->scenePos();
					for (int i1=0; i1 < cp->connectionItem->curveSegments.size(); ++i1)
					{
						for (int j=0; j < cp->connectionItem->curveSegments[i1].size(); ++j)
							if (cp->connectionItem->curveSegments[i1][j] && (cp->connectionItem->curveSegments[i1][j]->parentItem() == 0) &&
								cp->connectionItem->lineType == ConnectionGraphicsItem::line && (j%3) == 0)
							{
								p2 = cp->connectionItem->curveSegments[i1][j]->scenePos();

								if (p1 != p2)
								{
									if ((p1.rx() - p2.rx())*(p1.rx() - p2.rx()) <
										(p1.rx() - p.rx())*(p1.rx() - p.rx()))
										p.rx() = p2.rx();
									if ((p1.ry() - p2.ry())*(p1.ry() - p2.ry()) <
										(p1.ry() - p.ry())*(p1.ry() - p.ry()))
										p.ry() = p2.ry();
								}
							}
					}
					if (!p.isNull())
					{
						if ((p.rx() - p1.rx())*(p.rx() - p1.rx()) < gridDist*2.0)
							cp->topLevelItem()->moveBy( p.x() - p1.x(), 0 );
						if ((p.ry() - p1.ry())*(p.ry() - p1.ry()) < gridDist*2.0)
							cp->topLevelItem()->moveBy( 0 , p.y() - p1.y() );
					}
				}
				cp->connectionItem->refresh();
				visited.append(cp->connectionItem);
			}
		}
	}

	void ConnectionSelection::itemsMoved(GraphicsScene * ,const QList<QGraphicsItem*>& movingItems, const QList<QPointF>& )
	{
		adjustConnectorPoints(movingItems);
	}

	void ConnectionSelection::itemsRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& items, QList<ItemHandle*>& handles, QList<QUndoCommand*>& commands)
	{
		if (!scene || items.isEmpty()) return;

		ConnectionGraphicsItem::ControlPoint * p = 0;
		QList<ConnectionGraphicsItem::ControlPoint*> points;
		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;
		ItemHandle * h = 0;

		for (int i=0; i < items.size(); ++i)
		{
			p = 0;
			points.clear();
			connection = ConnectionGraphicsItem::cast(items[i]);

			if (connection)
			{
				connection->setControlPointsVisible(false);
				continue;
			}

			node = NodeGraphicsItem::cast(items[i]);

			if (node)
			{
				QList<QGraphicsItem*> children = node->childItems();
				for (int j=0; j < children.size(); ++j)
				{
					if ((p = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(children[j])) &&
						p->connectionItem && p->connectionItem->scene() == scene)
						points << p;
				}
			}
			else
			{
				p = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(items[i]);
				if (p && p->connectionItem)
					points << p;
			}
			
			int k;
			for (int j=0; j < points.size(); ++j)
			{
				if ((p = points[j]) && !items.contains(p->connectionItem))
				{
					k = p->connectionItem->indexOf(p);
					if (k > -1 && 
						p->connectionItem->curveSegments[k].size() > 4 &&
						!NodeGraphicsItem::cast(p->parentItem()))
					{
						RemoveControlPointCommand * cmmd1 = new RemoveControlPointCommand("control point removed",scene,p);
						commands << cmmd1;
					}
					else
					if (p->connectionItem->curveSegments.size() > 1)
					{
						RemoveCurveSegmentCommand * cmmd2 = new RemoveCurveSegmentCommand("path removed",scene,p);
						commands << cmmd2;
					}
					else
					{
						if (p->connectionItem && !items.contains(p->connectionItem))
						{
							QString s;
							if ((h = p->connectionItem->handle()) && !handles.contains(h))
							{
								handles += h;
								items += h->graphicsItems;
							}
						}
					}

					if (p)
						p->setVisible(false);
				}
				else
				{
					connection = ConnectionGraphicsItem::cast(items[i]);
					if (connection != 0)
						connection->setControlPointsVisible(false);
				}
			}
		}
	}

	void ConnectionSelection::escapeSignal(const QWidget*)
	{
		if (temporarilyChangedConnections.isEmpty()) return;

		for (int i=0; i < temporarilyChangedConnections.size(); ++i)
			if (temporarilyChangedConnections[i])
			{
				temporarilyChangedConnections[i]->setPen(temporarilyChangedConnections[i]->defaultPen);
				temporarilyChangedConnections[i]->update();
			}
			temporarilyChangedConnections.clear();
	}

	void ConnectionSelection::sceneKeyPressed(GraphicsScene *scene, QKeyEvent* keyEvent)
	{
		if (!scene || !keyEvent || !scene->useDefaultBehavior()) return;

		if (keyEvent->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
		{
			QList<QGraphicsItem*> items = scene->items( scene->visibleRegion() );
			NodeGraphicsItem * node = 0;
			ConnectionGraphicsItem * connection = 0;

			for (int i=0; i < items.size(); ++i)
			{
				node = NodeGraphicsItem::cast(items[i]);
				if (node && (ToolGraphicsItem::cast(node->topLevelItem()) == 0))
				{
					node->setBoundingBoxVisible(true);
				}
				else
				{
					connection = ConnectionGraphicsItem::cast(items[i]);
					if (connection)
					{
						connection->setControlPointsVisible(true);
					}
				}
			}
			controlHeld = true;
			return;
		}
		else
			if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
			{
				NodeGraphicsItem * node;
				QList<QGraphicsItem*> & list = scene->selected();
				for (int i=0; i < list.size(); ++i)
					if (!((node = NodeGraphicsItem::cast(list[i])) && ArrowHeadItem::cast(node)))
						return;
				
				emit substituteNodeGraphics();
			}
	}

	void ConnectionSelection::sceneKeyReleased(GraphicsScene * scene, QKeyEvent * )
	{
		if (controlHeld && scene)
		{
			QList<QGraphicsItem*> items = scene->items( scene->visibleRegion() );
			NodeGraphicsItem * node = 0;
			ConnectionGraphicsItem * connection = 0;

			for (int i=0; i < items.size(); ++i)
			{
				node = NodeGraphicsItem::cast(items[i]);
				if (node && !scene->selected().contains(node))
				{
					node->setBoundingBoxVisible(false);
				}
				else
				{
					connection = ConnectionGraphicsItem::cast(items[i]);
					if (connection && !scene->selected().contains(connection))
					{
						connection->setControlPointsVisible(false);
					}
				}
			}
			controlHeld = false;
		}
	}

	void ConnectionSelection::nodeCollided(const QList<QGraphicsItem*>& movingItems, NodeGraphicsItem * nodeBelow, const QList<QPointF>& )
	{
		if (!nodeBelow || !mainWindow) return;
		GraphicsScene * scene = mainWindow->currentScene();
		ItemHandle * nodeHandle = getHandle(nodeBelow);
		if (!nodeHandle || !nodeHandle->family() || !scene) return;

		for (int i=0; i < movingItems.size(); ++i)
		{
			NodeGraphicsItem * node = NodeGraphicsItem::cast(movingItems[i]);
			if (node && ArrowHeadItem::cast(node))
			{
				ArrowHeadItem * arrow = static_cast<ArrowHeadItem*>(node);
				if (arrow && arrow->connectionItem)
				{
					ItemHandle * handle = 0;
					int k = arrow->connectionItem->arrowHeads().indexOf(arrow);
					if (k > -1)
					{
						handle = getHandle( arrow->connectionItem->nodeAt(k) );
					}

					if (handle && handle->family() && nodeHandle->family()->isA(handle->family()))
					{
						QList<ArrowHeadItem*> arrows = arrow->connectionItem->arrowHeads();
						int k = arrows.indexOf(arrow);
						if (k > -1)
						{
							ConnectionGraphicsItem * copy = arrow->connectionItem->clone();
							copy->replaceNodeAt(k,nodeBelow);
							scene->remove(tr("remove old reaction"),arrow->connectionItem);
							scene->insert(tr("insert new reaction"),copy);
						}
					}
				}
			}
		}
	}

	void ConnectionSelection::mouseMoved(GraphicsScene * scene, QGraphicsItem* , QPointF , Qt::MouseButton, Qt::KeyboardModifiers modifiers, QList<QGraphicsItem*>& moving)
	{
		if (!scene || moving.size() != 1) return;

		if (!moving.at(0)) return;

		QGraphicsItem * cp = 0;
		for (int i=0; i < moving.size(); ++i)
		{
			if (ControlPoint::cast(moving.at(i)))
			{
				cp = moving.at(i);
				break;
			}
			else
				if (NodeGraphicsItem::cast(moving.at(i)))
				{
					cp = NodeGraphicsItem::cast(moving.at(i));
					break;
				}
		}

		if (!cp) return;

		bool avoidBoundary = false;
		controlHeld = (modifiers == Qt::ControlModifier);
		
		if (!controlHeld)
			return;

		QRectF viewport = scene->visibleRegion();
		QList<QGraphicsItem*> items = scene->items( viewport );
		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;

		QPointF p = cp->scenePos();
		QPointF closest;

		for (int i=0; i < items.size(); ++i)
		{
			node = NodeGraphicsItem::cast(items[i]);
			if (node && !scene->moving().contains(node) && (ToolGraphicsItem::cast(node->topLevelItem()) == 0))
			{
				if (!avoidBoundary)
					for (int j=0; j < node->boundaryControlPoints.size(); ++j)
					{
						if (node->boundaryControlPoints[j] && !scene->moving().contains(node->boundaryControlPoints[j]))
						{
							if ((closest.isNull() || ((p.x() - closest.x())*(p.x() - closest.x()) > (p.x() - node->boundaryControlPoints[j]->x())*(p.x() - node->boundaryControlPoints[j]->x())))
								&& (gridDist > (p.x() - node->boundaryControlPoints[j]->x())*(p.x() - node->boundaryControlPoints[j]->x()))
								)
								closest.rx() = node->boundaryControlPoints[j]->x();

							if ((closest.isNull() || ((p.y() - closest.y())*(p.y() - closest.y()) > (p.y() - node->boundaryControlPoints[j]->y())*(p.y() - node->boundaryControlPoints[j]->y())))
								&& (gridDist > (p.y() - node->boundaryControlPoints[j]->y())*(p.y() - node->boundaryControlPoints[j]->y()))
								)
								closest.ry() = node->boundaryControlPoints[j]->y();
						}
					}

				if ((closest.isNull() || ((p.x() - closest.x())*(p.x() - closest.x()) > (p.x() - node->scenePos().x())*(p.x() - node->scenePos().x())))
					&& (gridDist > (p.x() - node->scenePos().x())*(p.x() - node->scenePos().x()))
					)
					closest.rx() = node->scenePos().x();

				if ((closest.isNull() || ((p.y() - closest.y())*(p.y() - closest.y()) > (p.y() - node->scenePos().y())*(p.y() - node->scenePos().y())))
					&& (gridDist > (p.y() - node->scenePos().y())*(p.y() - node->scenePos().y()))
					)
					closest.ry() = node->scenePos().y();
			}
			else
			{
				connection = ConnectionGraphicsItem::cast(items[i]);
				if (connection && !scene->moving().contains(connection))
				{
					QList<ConnectionGraphicsItem::ControlPoint*> controlPoints = connection->controlPoints();
					for (int j=0; j < controlPoints.size(); ++j)
					{
						if (controlPoints[j] && controlPoints[j]->isVisible() && !scene->moving().contains(controlPoints[j]))
						{
							if ((closest.isNull() || ((p.x() - closest.x())*(p.x() - closest.x()) > (p.x() - controlPoints[j]->x())*(p.x() - controlPoints[j]->x())))
								&& (gridDist > (p.x() - controlPoints[j]->x())*(p.x() - controlPoints[j]->x()))
								)
								closest.rx() = controlPoints[j]->x();

							if ((closest.isNull() || ((p.y() - closest.y())*(p.y() - closest.y()) > (p.y() - controlPoints[j]->y())*(p.y() - controlPoints[j]->y())))
								&& (gridDist > (p.y() - controlPoints[j]->y())*(p.y() - controlPoints[j]->y()))
								)
								closest.ry() = controlPoints[j]->y();
						}
					}
				}
			}
		}

		if (closest.x() == 0)
			closest.rx() = p.x();

		if (closest.y() == 0)
			closest.ry() =p.y();

		cp->topLevelItem()->moveBy( closest.x() - p.x(), closest.y() - p.y() );
	}

	void ConnectionSelection::connectionCollided(const QList<QGraphicsItem*>& , ConnectionGraphicsItem * , QPointF )
	{
	}

	void ConnectionSelection::showMiddleBox()
	{
		showMiddleBox(1);
	}

	void ConnectionSelection::hideMiddleBox()
	{
		showMiddleBox(0);
	}

	void ConnectionSelection::showMiddleBox(int value, const QString& filename)
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		QList<QGraphicsItem*> centerBoxes;
		QList<QGraphicsItem*> &targetItems = scene->selected();

		QList<QGraphicsItem*> items;
		QList<NodeGraphicsItem*> nodeItems;

		for (int i=0; i < targetItems.size(); ++i)
		{
			ConnectionGraphicsItem * connectionPtr = 0;
			if (targetItems[i] &&
				(connectionPtr = ConnectionGraphicsItem::cast(targetItems[i])))
			{
				if (!connectionPtr->centerRegionItem)
				{
					ArrowHeadItem * node = new ArrowHeadItem(ConnectionGraphicsItem::DefaultMiddleItemFile,connectionPtr);
					if (node->isValid())
					{
						node->scale(25.0/node->sceneBoundingRect().height(),25.0/node->sceneBoundingRect().height());
						connectionPtr->centerRegionItem = node;
					}
				}
				if (connectionPtr->centerRegionItem)
				{
					if (connectionPtr->centerRegionItem->scene() != scene)
						items << connectionPtr->centerRegionItem;
					nodeItems << connectionPtr->centerRegionItem;
				}
			}
		}

		if (value)
		{
			QList<ItemHandle*> handles;
			QStringList filenames;


			for (int i=0; i < items.size(); ++i)
			{
				handles << getHandle(items[i]);
				filenames << filename;
			}

			QList<QUndoCommand*> commands;


			if (!filename.isNull() && !filename.isEmpty())
				commands << new ReplaceNodeGraphicsCommand(tr("center decorator changed"),nodeItems,filenames);

			commands << new InsertGraphicsCommand(tr("center decorator added"),scene,items);
			QUndoCommand * command = new CompositeCommand(tr("center decorator added"),commands);

			if (scene->network)
				scene->network->push(command);
			else
			{
				command->redo();
				delete command;
			}

			emit itemsInserted(scene, items, handles);

			//scene->insert(tr("center box added"),centerBoxes);
		}
		else
		{
			//scene->remove(tr("center decorator removed"),items);
		}
	}

	void ConnectionSelection::setLineTypeStraight()
	{
		setLineType(1);
	}

	void ConnectionSelection::setLineTypeCurved()
	{
		setLineType(0);
	}

	void ConnectionSelection::setLineType(int value)
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		QList<QGraphicsItem*> &targetItems = scene->selected();

		LineTypeChanged * command = new LineTypeChanged;
		command->straight = (value == 1);
		if (command->straight)
			command->setText("make straight");
		else
			command->setText("make curved");

		for (int i=0; i < targetItems.size(); ++i)
		{
			ConnectionGraphicsItem * connectionPtr = 0;
			if (targetItems[i] &&
				(connectionPtr = ConnectionGraphicsItem::cast(targetItems[i])))
			{
				command->list += connectionPtr;
			}
		}

		if (command->list.isEmpty())
		{
			delete command;
			return;
		}


		if (scene->network)
			scene->network->push(command);
		else
		{
			command->redo();
			delete command;
		}
	}

	void ConnectionSelection::arrowHeadDistance(double value)
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		QList<QGraphicsItem*> &targetItems = scene->selected();

		ChangeArrowHeadDistance * command = new ChangeArrowHeadDistance;
		command->setText("change arrow distance");

		for (int i=0; i < targetItems.size(); ++i)
		{
			ConnectionGraphicsItem * connectionPtr = 0;
			if (targetItems[i] &&
				(connectionPtr = ConnectionGraphicsItem::cast(targetItems[i])))
			{
				command->dists += value - connectionPtr->arrowHeadDistance;
				command->list += connectionPtr;
			}
		}

		if (command->list.isEmpty())
		{
			delete command;
			return;
		}

		if (scene->network)
			scene->network->push(command);
		else
		{
			command->redo();
			delete command;
		}
	}
}

