#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
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
		ItemHandle * handle, *cloneHandle;
		QList<ItemHandle*> oldHandles;
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
								cloneHandle->children.clear();
								cloneHandle->graphicsItems.clear();
								allNewHandles << cloneHandle;
								originalAndCloneHandles << QPair<ItemHandle*,ItemHandle*>(handle,cloneHandle);
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
}
