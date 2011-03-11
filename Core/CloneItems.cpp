#include <QHash>
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ItemHandle.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "Tool.h"
#include "CloneItems.h"

namespace Tinkercell
{
	/*
	* \param a pointer to a QGraphicsItem
	* \return a QGraphicsItem that is either a NodeGraphicsItem, ConnectionGraphicsItem, ControlPoint, or TextGraphicsItem
	*/
	QGraphicsItem * getGraphicsItem( QGraphicsItem * item )
	{
		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;
		ControlPoint * controlPoint = 0;
		TextGraphicsItem * text = 0;

		QGraphicsItem * p = item;

		while (p/* && !MainWindow::invalidPointers.contains((void*)p)*/)
		{
			text = TextGraphicsItem::cast(p);
			if (text)
			{
				if (!ToolGraphicsItem::cast(text->topLevelItem()))
					return (QGraphicsItem*)(text);
			}

			connection = ConnectionGraphicsItem::cast(p);
			if (connection)
			{
				if (!ToolGraphicsItem::cast(connection->topLevelItem()))
					return (QGraphicsItem*)connection;
			}

			node = NodeGraphicsItem::cast(p);
			if (node)
			{
				if (!ToolGraphicsItem::cast(node->topLevelItem()))
					return (QGraphicsItem*)node;
			}

			controlPoint = qgraphicsitem_cast<ControlPoint*>(p);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(p);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(p);
			if (controlPoint)
			{
				if (!ToolGraphicsItem::cast(controlPoint->topLevelItem()))
					return (QGraphicsItem*)controlPoint;
			}

			p = p->parentItem();
		}

		return (0);
	}

	/* Clone a graphics item
	* \param a pointer to a QGraphicsItem
	* \return a QGraphicsItem that is one of the Tinkercell Graphics Items
	*/
	QGraphicsItem * cloneGraphicsItem( QGraphicsItem * item )
	{
		ArrowHeadItem* arrow = 0;
		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;
		ControlPoint * controlPoint0 = 0;
		ConnectionGraphicsItem::ControlPoint * controlPoint1 = 0;
		NodeGraphicsItem::ControlPoint * controlPoint2 = 0;
		TextGraphicsItem * text = 0;

		QGraphicsItem * p = item;

		while (p && (ToolGraphicsItem::cast(p->topLevelItem()) == 0))
		{
			text = TextGraphicsItem::cast(p);
			if (text) return (QGraphicsItem*)(text->clone());

			connection = ConnectionGraphicsItem::cast(p);
			if (connection) return (QGraphicsItem*)(connection->clone());
			
			arrow = ArrowHeadItem::cast(p);
			if (arrow) return (QGraphicsItem*)(arrow->clone());

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

		//copy connected nodes
		QList<NodeGraphicsItem*> connectedNodes;
		QList<QGraphicsItem*> items, visited;
		
		//copy child items
		for (int i=0; i < items0.size(); ++i)
            if (items0[i] && !items.contains(items0[i]))
            {
            	items << items0[i];
            		
            	if (connection = ConnectionGraphicsItem::cast(items0[i]))
		        {
		        	connectedNodes = connection->nodes();
		        	for (int j=0; j < connectedNodes.size(); ++j)
		        		if (!items0.contains(connectedNodes[j]))
		        		{
		        			if (ArrowHeadItem::cast(connectedNodes[j]))
		        			{
		        				if (static_cast<ArrowHeadItem*>(connectedNodes[j])->connectionItem &&
		        					!items0.contains(static_cast<ArrowHeadItem*>(connectedNodes[j])->connectionItem))
		        					items0 += static_cast<ArrowHeadItem*>(connectedNodes[j])->connectionItem;
							}
							else
			        			items0 += connectedNodes[j];
		        		}
		        }

                if (handle = getHandle(items0[i]))
                {
                    QList<QGraphicsItem*> list = handle->allGraphicsItems();
                    for (int j=0; j < list.size(); ++j)
                        if (!items.contains(list[j]))
                        {
                            items << list[j];
                            if (list[j]->scene() && (list[j]->scene() == items0[i]->scene()))
                            	items0 << list[j];
                        }
                }
            }

        //top level handles
		for (int i=0; i < items.size(); ++i)
            if (!visited.contains(items[i]) && 
            	!ArrowHeadItem::cast(items[i]) &&
            	!ControlPoint::cast(items[i]))
            {
                visited << items[i];
                handle = getHandle(items[i]);
                
                QGraphicsItem * itemClone = cloneGraphicsItem(items[i]);

                if (itemClone)
                {
                    setHandle(itemClone,0);
                   if (items0.contains(items[i]))
                   {
	                    duplicateItems << itemClone;
	               }

                    if ((connection = ConnectionGraphicsItem::cast(itemClone)))
                    {
                        connectionItems += connection;
                        if (connection->centerRegionItem)
                        {
                        	node2 = connection->centerRegionItem;
                        	connection = ConnectionGraphicsItem::cast(items[i]);
                            if (connection && node2)
                            {
                                node1 = connection->centerRegionItem;
                                if (node1 && node2)
                                {
                                    originalsAndClones += QPair<NodeGraphicsItem*,NodeGraphicsItem*>(node1,node2);
                                    if (items0.contains(node1))
	                                    duplicateItems << node2;
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
								
								cloneHandle->setParent(0,false);
								cloneHandle->children.clear();
								cloneHandle->graphicsItems.clear();
								allNewHandles << cloneHandle;
								originalAndCloneHandles << QPair<ItemHandle*,ItemHandle*>(handle,cloneHandle);
								
								childHandles = handle->children;
								for (int j=0; j < childHandles.size(); ++j)
									if ((handle = childHandles[j]) && handle->graphicsItems.isEmpty())
									{
										cloneChildHandle = handle->clone();
										cloneChildHandle->setParent(0,false);
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
						originalAndCloneHandles[i].second->setParent(originalAndCloneHandles[j].second,false);
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
	
	QList<ItemHandle*> cloneHandles(const QList<ItemHandle*>& oldHandles)
	{
		QList<ItemHandle*> newHandles;
		QHash<ItemHandle*,ItemHandle*> hash;

		ItemHandle * handle = 0, * handle2 = 0;
		ConnectionHandle * connection = 0, * connection2 = 0;
		for (int i=0; i < oldHandles.size(); ++i)
			if ((handle = oldHandles[i]) && !hash.contains(handle))
			{
				handle2 = handle->clone();
				newHandles << handle2;
				handle2->setParent(0,false);
				handle2->children.clear();
				handle2->graphicsItems.clear();
				hash[handle] = handle2;
			}

		NodeHandle * node;
		for (int i=0; i < oldHandles.size(); ++i)
		{
			handle = oldHandles[i];
			if (handle && (handle2 = hash[handle]))
			{
				if (handle && handle->parent && hash.contains(handle->parent))
				{
					handle2->setParent( hash[ handle->parent ],false );
				}
			
				if (handle &&
					(connection = ConnectionHandle::cast(handle)) && 
					(connection2 = ConnectionHandle::cast(handle2)))
				{
					connection2->nodesWithRoles.clear();
					for (int j=0; j < connection->nodesWithRoles.size(); ++j)
						if (hash.contains(connection->nodesWithRoles[j].first) &&
							(node = NodeHandle::cast(hash[connection->nodesWithRoles[j].first])))
						{
							connection2->addNode(node,connection->nodesWithRoles[j].second);
						}
				}
			}
		}
		return newHandles;
	}
}
