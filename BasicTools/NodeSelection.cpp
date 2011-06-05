/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool performs several tasks:
1) highlight selected items
2) moves text items along with selected items
3) displays GraphicsTools for selected items
4) signals the GraphicsTools when they are selected

****************************************************************************/

#include <math.h>
#include "UndoCommands.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "NodeSelection.h"
#include "ConsoleWindow.h"

namespace Tinkercell
{

	NodeSelection::NodeSelection() : Tool(tr("Node Selection"),tr("Basic GUI"))
	{
		selectionColor2 = QColor(tr("#FF2400"));
		selectionColor3 = QColor(tr("#FFCC00"));
		selectionColor1 = QColor(255,255,255,150);
		mainWindow = 0;
	}

	void NodeSelection::historyChanged(int i)
	{
		if (i < 0)
			escapeSignal(0);
	}

	bool NodeSelection::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSignal(const QWidget*)));
			connect(mainWindow,SIGNAL(mousePressed(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),this,SLOT(sceneClicked(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));
			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),this,SLOT(itemsSelected(GraphicsScene *,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
			connect(mainWindow,SIGNAL(itemsAboutToBeRemoved(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)),this ,SLOT(itemsRemoved(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)));
			connect(mainWindow,SIGNAL(networkClosing(NetworkHandle*,bool*)),this,SLOT(networkClosing(NetworkHandle*,bool*)));

			return true;
		}
		return false;
	}

	void NodeSelection::setSelectColor()
	{
		ItemHandle * handle = 0;
		ConnectionGraphicsItem * connection;
		for (int i=0; i < selectedConnections.size(); ++i)
		{
			if (selectedConnections[i] != 0)
			{
				selectedConnections[i]->setControlPointsVisible(true);
				selectionColor2.setAlphaF(selectedConnections[i]->defaultPen.color().alphaF());
				selectionColor3.setAlphaF(selectedConnections[i]->defaultPen.color().alphaF());
				selectedConnections[i]->setPen(QPen(selectionColor2,selectedConnections[i]->pen().width(),Qt::DashLine));
				handle = selectedConnections[i]->handle();
				if (handle != 0)
					for (int j=0; j < handle->graphicsItems.size(); ++j)
					{
						if (ConnectionGraphicsItem::topLevelConnectionItem(handle->graphicsItems[j]) &&
							handle->graphicsItems[j] != selectedConnections[i])
						{
							connection = ConnectionGraphicsItem::topLevelConnectionItem(handle->graphicsItems[j]);
							connection->setPen(QPen(selectionColor3,
								ConnectionGraphicsItem::topLevelConnectionItem(handle->graphicsItems[j])->pen().width(),
								Qt::DotLine));
							connection->refresh();
						}
					}
					selectedConnections[i]->refresh();
			}
		}

		for (int i=0; i < selectedControlPoints.size(); ++i)
		{
			if (selectedControlPoints[i] != 0)
			{
				selectionColor2.setAlphaF(selectedControlPoints[i]->defaultBrush.color().alphaF());
				selectedControlPoints[i]->setBrush(QBrush(selectionColor2));
				selectionColor2.setAlphaF(selectedControlPoints[i]->defaultPen.color().alphaF());
				selectedControlPoints[i]->setPen(QPen(selectionColor2));
				NodeGraphicsItem::ControlPoint * cp =
					qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(selectedControlPoints[i]);
				if (cp && cp->nodeItem)
					cp->nodeItem->setBoundingBoxVisible(true);
				else
				{
					ConnectionGraphicsItem::ControlPoint * cp2 =
						qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(selectedControlPoints[i]);
					if (cp2 && cp2->connectionItem)
						cp2->connectionItem->setControlPointsVisible(true);
				}

			}
		}

		QRectF boundingRect;

		for (int i=0; i < selectedNodes.size(); ++i)
			if (selectedNodes[i] != 0)
			{
				selectedNodes[i]->setBoundingBoxVisible(true,true);

				for (int j=0; j < selectedNodes[i]->shapes.size(); ++j)
				{
					NodeGraphicsItem::Shape * shape = selectedNodes[i]->shapes[j];
					boundingRect = shape->sceneBoundingRect();
					if (shape != 0)// && boundingRect.width() < 200 && boundingRect.height() < 200)
					{
						selectionColor2.setAlphaF(shape->defaultPen.color().alphaF());
						shape->setPen(QPen(selectionColor2,shape->pen().width()));//,Qt::DotLine));

						if (shape->defaultBrush.gradient())
						{
							QGradient * gradient = new QGradient(*shape->defaultBrush.gradient());
							QGradientStops stops = gradient->stops();

							if (stops.size() > 1)
							{
								selectionColor1.setAlphaF(stops[0].second.alphaF());
								selectionColor2.setAlphaF(stops[1].second.alphaF()/2.0);
								gradient->setColorAt(0,selectionColor1);
								gradient->setColorAt(1,selectionColor2);
								shape->setBrush(*gradient);
							}

							delete gradient;
						}
						else
						{
							QColor c = shape->defaultBrush.color();
							if (c.alphaF() > 0.5)
								selectionColor2.setAlphaF((255.0 - (c.red() + c.blue() + c.green())/3.0)/255.0);
							else
								selectionColor2.setAlphaF(c.alphaF());
							shape->setBrush(QBrush(selectionColor2));
						}
					}
				}
			}

			for (int i=0; i < selectedHandleNodes.size(); ++i)
			{
				if (selectedHandleNodes[i])
				{
					selectedHandleNodes[i]->setBoundingBoxVisible(true,false);

					for (int k=0; k < selectedHandleNodes[i]->shapes.size(); ++k)
					{
						NodeGraphicsItem::Shape * shape =  selectedHandleNodes[i]->shapes[k];
						if (shape != 0)
						{
							selectionColor3.setAlphaF(shape->defaultPen.color().alphaF());
							shape->setPen(QPen(selectionColor3,shape->pen().width()));//,Qt::DotLine));

							if (shape->defaultBrush.gradient())
							{
								QGradient * gradient = new QGradient(*shape->defaultBrush.gradient());
								QGradientStops stops = gradient->stops();

								if (stops.size() > 1)
								{
									selectionColor1.setAlphaF(stops[0].second.alphaF());
									selectionColor3.setAlphaF(stops[1].second.alphaF()/2.0);
									gradient->setColorAt(0,selectionColor1);
									gradient->setColorAt(1,selectionColor3);
									shape->setBrush(*gradient);
								}
								delete gradient;
							}
							else
							{
								QColor c = shape->defaultBrush.color();
								if (c.alphaF() > 0.5)
									selectionColor3.setAlphaF((255.0 - (c.red() + c.blue() + c.green())/3.0)/255.0);
								else
									selectionColor3.setAlphaF(c.alphaF());
								shape->setBrush(QBrush(selectionColor3));
							}
						}
					}
				}
			}

			/*if (selectedNodes.size() == 1 && selectedNodes[0] && !selectedNodes[0]->sceneTransform().isRotating())
			{
			selectedNodes[0]->setBoundingBoxVisible(true,true);
			}*/

			for (int i=0; i < selectedTexts.size(); ++i)
			{
				if (selectedTexts[i])
					selectedTexts[i]->showBorder(true);
			}

			//changeColorCommand = new ChangeBrushAndPenCommand(tr("selection color"),graphicsItemList,brushList,penList);
			//changeColorCommand->redo();
	}

	void NodeSelection::revertColor()
	{
		ItemHandle * handle = 0;
		for (int i=0; i < selectedConnections.size(); ++i)
		{
			ConnectionGraphicsItem * connection = 0;
			if (selectedConnections[i] != 0)
			{
				selectedConnections[i]->setControlPointsVisible(false);
				selectedConnections[i]->setPen(selectedConnections[i]->defaultPen);
				handle = selectedConnections[i]->handle();
				if (handle != 0)
					for (int j=0; j < handle->graphicsItems.size(); ++j)
					{
						if ((connection = ConnectionGraphicsItem::topLevelConnectionItem(handle->graphicsItems[j])))
						{
							connection->setPen(connection->defaultPen);
							connection->refresh();
						}
					}
			}
		}

		for (int i=0; i < selectedControlPoints.size(); ++i)
		{
			if (selectedControlPoints[i] != 0)
			{
				selectedControlPoints[i]->setBrush(selectedControlPoints[i]->defaultBrush);
				selectedControlPoints[i]->setPen(selectedControlPoints[i]->defaultPen);
				NodeGraphicsItem::ControlPoint * cp =
					qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(selectedControlPoints[i]);
				if (cp && cp->nodeItem)
					cp->nodeItem->setBoundingBoxVisible(false);
			}
		}

		for (int i=0; i < selectedNodes.size(); ++i)
			if (selectedNodes[i] != 0)
			{
				selectedNodes[i]->setBoundingBoxVisible(false);

				for (int j=0; j < selectedNodes[i]->shapes.size(); ++j)
				{

					NodeGraphicsItem::Shape * shape = selectedNodes[i]->shapes[j];
					if (shape != 0)
					{
						shape->setBrush(shape->defaultBrush);
						shape->setPen(shape->defaultPen);
					}
				}
			}

			for (int i=0; i < selectedHandleNodes.size(); ++i)
			{
				if (selectedHandleNodes[i])
				{
					selectedHandleNodes[i]->setBoundingBoxVisible(false);

					for (int k=0; k < selectedHandleNodes[i]->shapes.size(); ++k)
					{
						NodeGraphicsItem::Shape * shape = selectedHandleNodes[i]->shapes[k];
						if (shape != 0)
						{
							shape->setBrush(shape->defaultBrush);
							shape->setPen(shape->defaultPen);
						}
					}
				}
			}

			for (int i=0; i < selectedTexts.size(); ++i)
			{
				if (selectedTexts[i])
				{
					selectedTexts[i]->showBorder(false);
					QColor color = selectedTexts[i]->plainTextColor();
					color.setAlpha(255);
					selectedTexts[i]->setPlainTextColor(color);
				}
			}
	}

	/*void NodeSelection::toolSelected(GraphicsScene* , GraphicalTool* tool, QPointF, Qt::KeyboardModifiers)
	{
	if (tool)
	{
	if (!visibleTools.contains(tool))
	visibleTools += tool;
	tool->selected(allItems);
	}

	if (selectedNodes.size() == 1 && selectedNodes[0])
	selectedNodes[0]->setBoundingBoxVisible(false);
	}*/

	void NodeSelection::sceneClicked(GraphicsScene *scene, QPointF , Qt::MouseButton , Qt::KeyboardModifiers modifiers)
	{
		if (mainWindow && scene && scene->useDefaultBehavior())
		{
			if (modifiers != Qt::ShiftModifier && modifiers != Qt::ControlModifier)
			{
				deselect();
			}
		}
	}

	void NodeSelection::select()
	{
		setSelectColor();
	}

	void NodeSelection::deselect()
	{
		revertColor();
		selectedNodes.clear();
		selectedHandleNodes.clear();
		selectedConnections.clear();
		selectedControlPoints.clear();
		selectedTexts.clear();
		allItems.clear();
	}

	void NodeSelection::sceneDoubleClicked (GraphicsScene * scene, QPointF , QGraphicsItem * item, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
	{
		if (!scene || !item || button != Qt::LeftButton || modifiers != 0) return;
		scene->clearSelection();
		item = item->topLevelItem();
		ItemHandle * targetHandle = getHandle(item);
		if (targetHandle && (NodeGraphicsItem::cast(item) || ConnectionGraphicsItem::cast(item)))
		{
			QGraphicsItem* tinkercellItem;
			QList<QGraphicsItem*> list = scene->items();
			ItemHandle * handle;
			for (int i=0; i < list.size(); ++i)
			{
				tinkercellItem = (QGraphicsItem*)(NodeGraphicsItem::cast(list[i]));
				if (tinkercellItem == 0)
					tinkercellItem = (QGraphicsItem*)(ConnectionGraphicsItem::cast(list[i]));

				if (tinkercellItem
					&& (handle = getHandle(tinkercellItem))
					&& handle->family()
					&& handle->family()->isA(targetHandle->family()))
				{
					scene->selected() += tinkercellItem;
				}
			}
		}
		scene->select(0);
	}

	void NodeSelection::selectNearByItems(GraphicsScene * scene, ItemHandle * handle, QGraphicsItem* item, qreal dx)
	{
		if (!scene || !handle || !item) return;

		QList<QGraphicsItem*> list;
		list << item;
		QRectF rect = item->sceneBoundingRect();
		
		rect.adjust( -dx, -dx, dx, dx );
		
		QList<QGraphicsItem*> items = handle->allGraphicsItems();
		
		for (int i=0; i < items.size(); ++i)
		{
			if (items[i] && (items[i]->scene() == scene) && !list.contains(items[i]) && rect.intersects(items[i]->sceneBoundingRect()))
			{
				list << items[i];
				rect = rect.united(items[i]->sceneBoundingRect());
				rect.adjust( -dx, -dx, dx, dx );
				i = 0;
				items[i] = 0;
			}
		}
		
		/*NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * conn = 0;
		TextGraphicsItem* text = 0;
		
		QList<QGraphicsItem*> items = scene->items(rect);

		for (int i=0; i < items.size(); ++i)
		{
			node = 0;
			conn = 0;
			text = 0;
			
			node = NodeGraphicsItem::cast(items[i]);
			if (node)
			{
				if (!node->handle() || !children.contains(node->handle()) || node->parentItem()) 
					continue;
			}
			else			
				conn = ConnectionGraphicsItem::cast(items[i]);
			
			if (conn && (conn->parentItem() || (conn->handle() && !children.contains(conn->handle())))) 
				continue;
			else			
				text = TextGraphicsItem::cast(items[i]);
				
			if (text && (text->parentItem() || (text->handle() && !children.contains(text->handle()))))
				continue;
			
			if (node && !list.contains(node))
				list += node;
			else
				if (conn && !list.contains(conn))
					list += conn;
				else
					if (text && !list.contains(text))
						list += text;
		}*/
		
		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * conn = 0;

		for (int i=0; i < list.size(); ++i)
			if (!scene->moving().contains(list[i]))
			{
				scene->moving() += list[i];
				if ((node = NodeGraphicsItem::cast(list[i])))
				{
					for (int j=0; j < node->boundaryControlPoints.size(); ++j)
						scene->moving() += node->boundaryControlPoints[j];
				}
				else
					if ((conn = ConnectionGraphicsItem::cast(list[i])))
					{
						scene->moving() += conn->controlPointsAsGraphicsItems();
					}
			}
	}

	void NodeSelection::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF , Qt::KeyboardModifiers modifiers)
	{
		deselect();

		if (mainWindow && scene && scene->useDefaultBehavior())
		{
			QList<ItemHandle*> itemHandles;
			NodeGraphicsItem * ptr;
			NodeGraphicsItem * subPtr;
			ConnectionGraphicsItem * ptr2;

			for (int i=0; i < items.size(); ++i)
			{
				if (qgraphicsitem_cast<ToolGraphicsItem*>(items[i]->topLevelItem()))
				{
					continue;
				}

				ptr = NodeGraphicsItem::cast(items[i]);
				if (ptr)
				{
					allItems += ptr;
					itemHandles += ptr->handle();

					if (!selectedNodes.contains(ptr))
					{
						selectedNodes += ptr;
						for (int j=0; j < ptr->boundaryControlPoints.size(); ++j)
							scene->moving() += ptr->boundaryControlPoints[j];
					}

					//light-highlight on items of the same handle
					if (ptr->handle())
					{
						ItemHandle * ptrh = ptr->handle();
						subPtr = 0;
						for (int j=0; j < ptrh ->graphicsItems.size(); ++j)
							if ((subPtr = NodeGraphicsItem::cast(ptrh ->graphicsItems[j]))
								&& (subPtr != ptr)
								&& !selectedNodes.contains(subPtr)
								&& !selectedHandleNodes.contains(subPtr))
								selectedHandleNodes += subPtr;

						if (selectedHandleNodes.contains(ptr))
							selectedHandleNodes.removeAll(ptr);

						if (!(modifiers & Qt::ControlModifier))
							selectNearByItems(scene,ptrh,ptr,100.0);
					}
					else
					{
					}
				}
				else
				{
					ptr2 = ConnectionGraphicsItem::cast(items[i]);
					if (ptr2 != 0)
					{
						allItems += ptr2;
						if (!selectedConnections.contains(ptr2))
							selectedConnections += ptr2;

						itemHandles += ptr2->handle();
					}
					else
					{
						ConnectionGraphicsItem::ControlPoint * ptr3 = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(items[i]);
						if (ptr3 !=0 && ptr3->connectionItem != 0)
						{
							selectedControlPoints += ptr3;
							selectedConnections += ptr3->connectionItem;
						}
						else
						{
							NodeGraphicsItem::ControlPoint * ptr4 = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(items[i]);
							if (ptr4 !=0 && ptr4->nodeItem != 0)
							{
								selectedControlPoints += ptr4;
								//selectedNodes += ptr4->nodeItem;
							}
							else
							{
								TextGraphicsItem * ptr5 = TextGraphicsItem::cast(items[i]);
								if (ptr5 != 0)
								{
									selectedTexts += ptr5;
									allItems += items[i];
								}
								/*
								ItemHandle * ipt = getHandle(items[i]);
								if (ipt)
								{
								itemHandles += ipt;
								allItems += items[i];
								}
								*/
							}
						}
					}
				}
			}

			if (selectedNodes.size() > 0 || selectedConnections.size() > 0 || selectedControlPoints.size() > 0 || selectedTexts.size() > 0)
			{
				//for (int i=0; i < selectedNodes.size(); ++i)
				//	selectedHandleNodes.removeAll(seletedNodes[i]);
				select();
			}
		}
	}

	void NodeSelection::networkClosing(NetworkHandle * net, bool *)
	{
		deselect();
		if (net && net->currentScene())
			net->currentScene()->clearSelection();
	}

	void NodeSelection::escapeSignal(const QWidget * )
	{
		deselect();
		if (mainWindow && mainWindow->currentScene())
			mainWindow->currentScene()->clearSelection();
	}

	void NodeSelection::itemsRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& list, QList<ItemHandle*>& handles, QList<QUndoCommand*>&)
	{
		if (scene && !handles.isEmpty())
		{
			for (int i=0; i < handles.size(); ++i)
				if (handles[i] && !handles[i]->graphicsItems.isEmpty())
				{
					int j = 0;
					for (j=0; j < handles[i]->graphicsItems.size(); ++j)
						if (NodeGraphicsItem::cast(handles[i]->graphicsItems[j]) ||
							ConnectionGraphicsItem::cast(handles[i]->graphicsItems[j]))
							break;
					if (j > 0 && j == handles[i]->graphicsItems.size()) //only text or control points left in this handle
					{
						list += handles[i]->graphicsItems;
					}
				}

				deselect();
				selectedNodes.clear();
				selectedHandleNodes.clear();
				selectedConnections.clear();
				selectedControlPoints.clear();
				selectedTexts.clear();
				allItems.clear();
		}
	}

}

