#include <QHash>
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "TextItem.h"
#include "ItemHandle.h"
#include "UndoCommands.h"
#include "Tool.h"
#include "CloneItems.h"

namespace Tinkercell
{
	/*
	* \param a pointer to a QGraphicsItem
	* \return a QGraphicsItem that is one of the Tinkercell Graphics Items
	*/
	QGraphicsItem * getGraphicsItem( QGraphicsItem * item )
	{
		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;
		ControlPoint * controlPoint = 0;
		TextGraphicsItem * text = 0;

		QGraphicsItem * p = item;

		while (p && (Tool::GraphicsItem::cast(p->topLevelItem()) == 0))
		{
			text = TextGraphicsItem::cast(p);
			if (text) return (QGraphicsItem*)(text);

			connection = ConnectionGraphicsItem::cast(p);
			if (connection) return (QGraphicsItem*)connection;

			node = NodeGraphicsItem::cast(p);
			if (node) return (QGraphicsItem*)node;

			controlPoint = qgraphicsitem_cast<ControlPoint*>(p);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(p);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(p);
			if (controlPoint) return (QGraphicsItem*)controlPoint;

			if (p != p->parentItem())
				p = p->parentItem();
			else
				p = 0;
		}

		return (0);
	}

	/* Clone a graphics item
	* \param a pointer to a QGraphicsItem
	* \return a QGraphicsItem that is one of the Tinkercell Graphics Items
	*/
	QGraphicsItem * cloneGraphicsItem( QGraphicsItem * item )
	{
		//ArrowHeadItem* arrow = 0;
		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;
		ControlPoint * controlPoint0 = 0;
		ConnectionGraphicsItem::ControlPoint * controlPoint1 = 0;
		NodeGraphicsItem::ControlPoint * controlPoint2 = 0;
		TextGraphicsItem * text = 0;

		QGraphicsItem * p = item;

		while (p && (Tool::GraphicsItem::cast(p->topLevelItem()) == 0))
		{
			text = TextGraphicsItem::cast(p);
			if (text) return (QGraphicsItem*)(text->clone());

			connection = ConnectionGraphicsItem::cast(p);
			if (connection) return (QGraphicsItem*)(connection->clone());

			node = NodeGraphicsItem::cast(p);
			if (node) return (QGraphicsItem*)(node->clone());

			controlPoint0 = qgraphicsitem_cast<ControlPoint*>(p);
			if (controlPoint0)
				return (QGraphicsItem*)(controlPoint0->clone());

			controlPoint1 = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(p);
			if (controlPoint1)
				return (QGraphicsItem*)(controlPoint1->clone());

			controlPoint2 = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(p);
			if (controlPoint2)
				return (QGraphicsItem*)(controlPoint2->clone());

			if (p != p->parentItem())
				p = p->parentItem();
			else
				p = 0;
		}

		return (0);
	}

	QList<QGraphicsItem*> cloneGraphicsItems( QList<QGraphicsItem*>& items0, QList<ItemHandle*>& allNewHandles, bool deep)
	{
		QList<QGraphicsItem*> duplicateItems;
		QList< QPair<ItemHandle*,ItemHandle*> > originalAndCloneHandles;

		//for connections
		QList< QPair<NodeGraphicsItem*,NodeGraphicsItem*> > originalsAndClones;
		QList< ConnectionGraphicsItem* > connectionItems;
		NodeGraphicsItem *node1 = 0, *node2 = 0;
		ConnectionGraphicsItem* connection = 0;
		QList<ArrowHeadItem*> arrowHeads;

		//for copying handles
		ItemHandle * handle, *cloneHandle, *cloneChildHandle;
		QList<ItemHandle*> oldHandles;
		QList<ItemHandle*> childHandles;
		allNewHandles.clear();

		QRectF boundingRect;
		QList<QGraphicsItem*> items, visited;
		for (int i=0; i < items0.size(); ++i)
            if (items0[i] && !items.contains(items0[i]))
            {
                boundingRect = boundingRect.united(items0[i]->sceneBoundingRect());
                if (handle = getHandle(items0[i]))
                {
                    QList<QGraphicsItem*> list = handle->allGraphicsItems();
                    for (int j=0; j < list.size(); ++j)
                        if (!items.contains(list[j]))
                            items << list[j];
                }
                else
                    if (ConnectionGraphicsItem::cast(items0[i]) ||
                        TextGraphicsItem::cast(items0[i]))
                        items << items0[i];
            }

        //top level handles
		for (int i=0; i < items.size(); ++i)
            if (!visited.contains(items[i]))
            {
                visited << items[i];
                handle = getHandle(items[i]);
                QGraphicsItem * itemClone = cloneGraphicsItem(items[i]);

                if (itemClone)
                {
                    setHandle(itemClone,0);
                    duplicateItems << itemClone;
                    if ((connection = ConnectionGraphicsItem::cast(itemClone)))
                    {
                        connectionItems += connection;
                        if (connection->centerRegionItem)
                        {
                            node1 = connection->centerRegionItem;
                            connection = ConnectionGraphicsItem::cast(items[i]);
                            if (connection && connection->centerRegionItem && connection->centerRegionItem->scene())
                            {
                                node2 = connection->centerRegionItem;
                                if (node1 && node2)
                                {
                                    originalsAndClones += QPair<NodeGraphicsItem*,NodeGraphicsItem*>(node2,node1);
                                    duplicateItems << node1;
                                }
                            }
                        }
                    }
                    else
                        if ((node1 = NodeGraphicsItem::cast(items[i]))
                            && (node2 = NodeGraphicsItem::cast(itemClone)))
                        {
                            originalsAndClones += QPair<NodeGraphicsItem*,NodeGraphicsItem*>(node1,node2);
                        }

                    if (handle)
                    {
                        cloneHandle = 0;
						
						if (deep)
						{
							for (int j=0; j < originalAndCloneHandles.size(); ++j)
								if (originalAndCloneHandles[j].first == handle)
								{
									cloneHandle = originalAndCloneHandles[j].second;
									break;
								}

							if (!cloneHandle)
							{
								cloneHandle = handle->clone();
								cloneHandle->setParent(0);
								cloneHandle->textItems.clear();
								cloneHandle->children.clear();
								cloneHandle->graphicsItems.clear();
								allNewHandles << cloneHandle;
								originalAndCloneHandles << QPair<ItemHandle*,ItemHandle*>(handle,cloneHandle);
								
								childHandles = handle->children;
								for (int j=0; j < childHandles.size(); ++j)
									if ((handle = childHandles[j]) && handle->graphicsItems.isEmpty())
									{
										cloneChildHandle = handle->clone();
										cloneChildHandle->setParent(0);
										cloneChildHandle->textItems.clear();
										cloneChildHandle->children.clear();
										cloneChildHandle->graphicsItems.clear();
										
										for (int k=0; k < handle->children.size(); ++k)
											if (!childHandles.contains(handle->children[k]))
												childHandles << handle->children[k];
										
										allNewHandles << cloneChildHandle;
										originalAndCloneHandles << QPair<ItemHandle*,ItemHandle*>(handle,cloneChildHandle);
									}
							}
						}
						else
							cloneHandle = handle;
						
                        setHandle(itemClone,cloneHandle);
                    }
                }
            }

		for (int i=0; i < connectionItems.size(); ++i)
		{
			QList<NodeGraphicsItem*> nodes = connectionItems[i]->nodes();
			for (int k=0; k < originalsAndClones.size(); ++k)
			{
				if (nodes.contains(originalsAndClones[k].first) && originalsAndClones[k].first && originalsAndClones[k].second)
				{
					connectionItems[i]->replaceNode(originalsAndClones[k].first,originalsAndClones[k].second);
				}
			}

			QList<QGraphicsItem*> arrows = connectionItems[i]->arrowHeadsAsGraphicsItems();
			for (int k=0; k < arrows.size(); ++k)
                duplicateItems.removeAll(arrows[k]);
		}

		//replace parent handles
		bool parentCopied = false;
		for (int i=0; i < originalAndCloneHandles.size(); ++i)
		{
			if (originalAndCloneHandles[i].first->parent != 0)
			{
				parentCopied = false;
				for (int j=0; j < originalAndCloneHandles.size(); ++j)
				{
					if (originalAndCloneHandles[i].first->parent == originalAndCloneHandles[j].first)
					{
						originalAndCloneHandles[i].second->setParent(originalAndCloneHandles[j].second);
						parentCopied = true;
						break;
					}
				}
				if (!parentCopied)
				{
					QString oldName = originalAndCloneHandles[i].first->fullName();
					QString newName = originalAndCloneHandles[i].second->fullName();
					RenameCommand::findReplaceAllHandleData(allNewHandles,oldName,newName);
				}
			}
		}

		return duplicateItems;
	}
	
	QList<TextItem*> cloneTextItems(const QList<TextItem*>& items)
	{
		QList<TextItem*> newItems;
		QList<ItemHandle*> oldHandles, childHandles;
		QHash<ItemHandle*,ItemHandle*> hash;

		for (int i=0; i < items.size(); ++i)
		{
			oldHandles << getHandle(items[i]);
			if (items[i])
				newItems << items[i]->clone();
			else
				newItems << 0;
		}

		ItemHandle * handle = 0, * handle2 = 0, * cloneChildHandle = 0;
		for (int i=0; i< items.size(); ++i)
		{
			handle = getHandle(items[i]);
			if (newItems[i])
				if (handle)
				{
					if (hash.contains(handle))
					{
						setHandle(newItems[i],hash[handle]);
					}
					else
					{
						handle2 = handle->clone();
						handle2->setParent(0);
						handle2->textItems.clear();
						handle2->children.clear();
						handle2->graphicsItems.clear();
						hash[handle] = handle2;
						setHandle(newItems[i],handle2);
						
						childHandles = handle->children;
						for (int j=0; j < childHandles.size(); ++j)
							if ((handle = childHandles[j]) && handle->textItems.isEmpty() && handle->graphicsItems.isEmpty())
							{
								cloneChildHandle = handle->clone();
								cloneChildHandle->setParent(0);
								cloneChildHandle->textItems.clear();
								cloneChildHandle->children.clear();
								cloneChildHandle->graphicsItems.clear();
								
								for (int k=0; k < handle->children.size(); ++k)
									if (!childHandles.contains(handle->children[k]))
										childHandles << handle->children[k];
								
								hash[handle] = cloneChildHandle;
							}
					}
				}
				else
					setHandle(newItems[i],0);
		}

		for (int i=0; i< items.size() && i< newItems.size(); ++i)
		{
			handle = getHandle(items[i]);
			handle2 = getHandle(newItems[i]);
			if (handle && handle->parent && handle2)
			{
				if (hash.contains(handle->parent))
				{
					handle2->setParent( hash[handle->parent] );
				}
			}
		}

		for (int i=0; i < items.size(); ++i)
		{
			if (items[i] && items[i]->asNode() && newItems[i]->asNode())
			{
				NodeTextItem* newNode = newItems[i]->asNode();
				newNode->connections.clear();
				QList<ConnectionTextItem*>& connections = items[i]->asNode()->connections;
				for(int j=0; j < connections.size(); ++j)
					for (int k=0; k < items.size(); ++k)
						if (connections[j] == items[k] && newItems[k]->asConnection())
						{
							newNode->connections << newItems[k]->asConnection();
						}
			}
			else
			if (items[i] && items[i]->asConnection() && newItems[i]->asConnection())
			{
				ConnectionTextItem* newConnection = newItems[i]->asConnection();
				newConnection->nodesIn.clear();
				newConnection->nodesOut.clear();
				newConnection->nodesOther.clear();
				QList<NodeTextItem*>& nodesIn = items[i]->asConnection()->nodesIn;
				QList<NodeTextItem*>& nodesOut = items[i]->asConnection()->nodesOut;
				QList<NodeTextItem*>& nodesOther = items[i]->asConnection()->nodesOther;
				for(int j=0; j < nodesIn.size(); ++j)
					for (int k=0; k < items.size(); ++k)
						if (nodesIn[j] == items[k] && newItems[k]->asNode())
							newConnection->nodesIn << newItems[k]->asNode();
				for(int j=0; j < nodesOut.size(); ++j)
					for (int k=0; k < items.size(); ++k)
						if (nodesOut[j] == items[k] && newItems[k]->asNode())
							newConnection->nodesOut << newItems[k]->asNode();
				for(int j=0; j < nodesOther.size(); ++j)
					for (int k=0; k < items.size(); ++k)
						if (nodesOther[j] == items[k] && newItems[k]->asNode())
							newConnection->nodesOther << newItems[k]->asNode();

			}
		}
		return newItems;
	}
}
