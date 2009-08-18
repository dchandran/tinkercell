/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file contains a collection of commands that perform simple operations that can be redone and undone.

****************************************************************************/

#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "Tool.h"
#include "GraphicsScene.h"
#include "NetworkWindow.h"
#include "UndoCommands.h"
#include "ConsoleWindow.h"
#include <QRegExp>
#include <QStringList>

namespace Tinkercell
{

	MoveCommand::MoveCommand(GraphicsScene * scene, const QList<QGraphicsItem *>& items, const QList<QPointF>& amounts)
		: QUndoCommand(QObject::tr("items moved by ..."))
	{
		graphicsScene = scene;
		graphicsItems.clear();

		change.clear();		
		graphicsItems.clear();

		ConnectionGraphicsItem * connection = 0;
		NodeGraphicsItem * node = 0;
		for (int i=0; i < items.size(); ++i)
		{
			if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i])))
			{
				QList<QGraphicsItem*> cps = connection->controlPointsAsGraphicsItems();
				for (int j=0; j < cps.size(); ++j)
				{
					if (!graphicsItems.contains(cps[j]))
					{
						graphicsItems += cps[j];
						change += amounts[i];
					}
				}
			}
			else
			{
				if (!graphicsItems.contains(items[i]))
				{
					graphicsItems += items[i];
					change += amounts[i];
				}

				if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(items[i])))
				{
					QVector<NodeGraphicsItem::ControlPoint*> cps = node->boundaryControlPoints;
					for (int j=0; j < cps.size(); ++j)
					{
						if (!graphicsItems.contains(cps[j]))
						{
							graphicsItems += cps[j];
							change += amounts[i];
						}
					}
				}
			}
		}
	}

	MoveCommand::MoveCommand(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QPointF& amount)
		: QUndoCommand(QObject::tr("items moved by (") + QString::number(amount.x()) + QObject::tr(",") + QString::number(amount.y()) + QObject::tr(")"))
	{
		graphicsScene = scene;
		change.clear();
		ConnectionGraphicsItem * connection = 0;
		NodeGraphicsItem * node = 0;
		for (int i=0; i < items.size(); ++i)
		{
			if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i])))
			{
				QList<QGraphicsItem*> cps = connection->controlPointsAsGraphicsItems();
				for (int j=0; j < cps.size(); ++j)
				{
					if (!graphicsItems.contains(cps[j]))
					{
						graphicsItems += cps[j];
						change += amount;
					}
				}
			}
			else
			{
				if (!graphicsItems.contains(items[i]))
				{
					graphicsItems += items[i];
					change += amount;
				}
				if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(items[i])))
				{
					QVector<NodeGraphicsItem::ControlPoint*> cps = node->boundaryControlPoints;
					for (int j=0; j < cps.size(); ++j)
					{
						if (!graphicsItems.contains(cps[j]))
						{
							graphicsItems += cps[j];
							change += amount;
						}
					}
				}
			}
		}
	}

	MoveCommand::MoveCommand(GraphicsScene * scene, QGraphicsItem * item, const QPointF& amount)
		: QUndoCommand(QObject::tr("items moved by (") + QString::number(amount.x()) + QObject::tr(",") + QString::number(amount.y()) + QObject::tr(")"))
	{
		graphicsScene = scene;
		graphicsItems.clear();
		change.clear();
		change += amount;
		graphicsItems.append(item);
	}

	void MoveCommand::refreshAllConnectionIn(const QList<QGraphicsItem*>& moving)
	{
		QList<ConnectionGraphicsItem*> connections;
		ConnectionGraphicsItem::ControlPoint* cgp = 0;
		for (int i=0; i < moving.size(); ++i)
			if ((cgp = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(moving[i]))
				&& cgp->connectionItem 
				&& !connections.contains(cgp->connectionItem))
				connections += cgp->connectionItem;
			else
			{
				QList<QGraphicsItem*> children = moving[i]->childItems();
				for (int i=0; i < children.size(); ++i)
					if ((cgp = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(children[i]))
						&& cgp->connectionItem
						&& !connections.contains(cgp->connectionItem))
						connections += cgp->connectionItem;					
			}

			for (int i=0; i < connections.size(); ++i)
				connections[i]->refresh();
	}

	void MoveCommand::redo()
	{
		QList<ControlPoint*> controlPoints;
		ConnectionGraphicsItem::ControlPoint * ccp = 0;
		NodeGraphicsItem::ControlPoint * pcp = 0;
		QGraphicsItem * parent = 0;
		for (int i=0; i<graphicsItems.size() && i<change.size(); ++i)
			if (graphicsItems[i])
			{
				parent = graphicsItems[i]->parentItem();
				graphicsItems[i]->setParentItem(0);

				graphicsItems[i]->moveBy(change[i].x(),change[i].y());
				if ((ccp = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(graphicsItems[i])))
					controlPoints += ccp;
				else
					if ((pcp = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(graphicsItems[i])) && 
						!graphicsItems.contains(pcp->nodeItem))
						controlPoints += pcp;

				graphicsItems[i]->setParentItem(parent);
			}
			for (int i=0; i < controlPoints.size(); ++i)
			{
				controlPoints[i]->sideEffect();
			}
			refreshAllConnectionIn(graphicsItems);
	}

	void MoveCommand::undo()
	{
		QList<ControlPoint*> controlPoints;
		ConnectionGraphicsItem::ControlPoint * ccp = 0;
		NodeGraphicsItem::ControlPoint * pcp = 0;
		QGraphicsItem * parent = 0;
		for (int i=0; i<graphicsItems.size() && i<change.size(); ++i)
			if (graphicsItems[i])
			{
				parent = graphicsItems[i]->parentItem();
				graphicsItems[i]->setParentItem(0);

				graphicsItems[i]->moveBy(-change[i].x(), -change[i].y());
				if ((ccp = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(graphicsItems[i])))
					controlPoints += ccp;
				else
					if ((pcp = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(graphicsItems[i])) && 
						!graphicsItems.contains(pcp->nodeItem))
						controlPoints += pcp;

				graphicsItems[i]->setParentItem(parent);
			}
			for (int i=0; i < controlPoints.size(); ++i)
			{
				controlPoints[i]->sideEffect();
			}
			refreshAllConnectionIn(graphicsItems);
	}

	InsertItemsCommand::InsertItemsCommand(TextEditor * editor, const QList<TextItem*> & list)
	{
		QStringList s;
		ItemHandle * h = 0;
		for (int i=0; i < list.size(); ++i)
			if (h = getHandle(list[i]))
				s << h->name;
		setText(s.join(QObject::tr(",")) + QObject::tr(" added"));
		textEditor = editor;
		items = list;
	}

	InsertItemsCommand::InsertItemsCommand(TextEditor * editor, TextItem * item)
	{
		ItemHandle * h = getHandle(item);
		if (h)
			setText(h->name + QObject::tr(" added"));
		else
			setText(QObject::tr("items added"));
		textEditor = editor;
		items << item;
	}

	void InsertItemsCommand::redo()
	{
		if (textEditor)
		{
			QList<TextItem*>& list = textEditor->items();
			for (int i=0; i < items.size(); ++i)
				if (items[i] && !list.contains(items[i]))
					list << items[i];
		}
	}

	void InsertItemsCommand::undo()
	{
		if (textEditor)
		{
			QList<TextItem*>& list = textEditor->items();
			for (int i=0; i < items.size(); ++i)
				if (items[i] && list.contains(items[i]))
					list.removeAll(items[i]);
		}
	}

	RemoveItemsCommand::RemoveItemsCommand(TextEditor * editor, const QList<TextItem*> & list)
	{
		QStringList s;
		ItemHandle * h = 0;
		for (int i=0; i < list.size(); ++i)
			if (h = getHandle(list[i]))
				s << h->name;
		setText(s.join(QObject::tr(",")) + QObject::tr(" removed"));
		textEditor = editor;
		items = list;
	}

	RemoveItemsCommand::RemoveItemsCommand(TextEditor * editor, TextItem * item)
	{
		ItemHandle * h = getHandle(item);
		if (h)
			setText(h->name + QObject::tr(" removed"));
		else
			setText(QObject::tr("items removed"));
		textEditor = editor;
		items << item;
	}

	void RemoveItemsCommand::undo()
	{
		if (textEditor)
		{
			QList<TextItem*>& list = textEditor->items();
			for (int i=0; i < items.size(); ++i)
				if (items[i] && !list.contains(items[i]))
					list << items[i];
		}
	}

	void RemoveItemsCommand::redo()
	{
		if (textEditor)
		{
			QList<TextItem*>& list = textEditor->items();
			for (int i=0; i < items.size(); ++i)
				if (items[i] && list.contains(items[i]))
					list.removeAll(items[i]);
		}
	}

	InsertGraphicsCommand::InsertGraphicsCommand(const QString& name, GraphicsScene * scene, QGraphicsItem * item)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems.clear();
		graphicsItems.append(item);
		handles.clear();
		handles += getHandle(item);
	}

	InsertGraphicsCommand::InsertGraphicsCommand(const QString& name, GraphicsScene * scene, const QList<QGraphicsItem*>& items)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems = items;
		handles.clear();
		for (int i=0; i < items.size(); ++i)
			handles += getHandle(items[i]);
	}

	void InsertGraphicsCommand::redo()
	{
		ConnectionGraphicsItem * connection;
		if (graphicsScene)
			for (int i=0; i<graphicsItems.size(); ++i)

				if (graphicsItems[i] && graphicsItems[i]->scene() != graphicsScene)
				{
					graphicsScene->addItem(graphicsItems[i]);
					setHandle(graphicsItems[i],handles[i]);
					if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(graphicsItems[i])))
						connection->refresh();
				}
	}

	void InsertGraphicsCommand::undo()
	{
		ConnectionGraphicsItem * connection = 0;
		if (graphicsScene)
			for (int i=0; i<graphicsItems.size(); ++i)

				if (graphicsItems[i] && graphicsItems[i]->scene() == graphicsScene)
				{
					graphicsScene->removeItem(graphicsItems[i]);
					if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(graphicsItems[i])))
					{
						QList<QGraphicsItem*> arrows = connection->arrowHeadsAsGraphicsItems();
						for (int j=0; j < arrows.size(); ++j)
							if (arrows[j] && arrows[j]->scene())
								arrows[j]->scene()->removeItem(arrows[j]);
					}
					setHandle(graphicsItems[i],0);
				}
	}

	InsertGraphicsCommand::~InsertGraphicsCommand()
	{
		/*		if (graphicsScene)
		for (int i=0; i < graphicsItems.size(); ++i)
		if (!graphicsScene->items().contains(graphicsItems[i]))
		//graphicsScene->addItem(graphicsItems[i]);
		delete graphicsItems[i];*/
	}

	RemoveGraphicsCommand::~RemoveGraphicsCommand()
	{
		/*
		if (graphicsScene)
		for (int i=0; i < graphicsItems.size(); ++i)
		if (!graphicsScene->items().contains(graphicsItems[i]))
		graphicsScene->addItem(graphicsItems[i]);
		*/
	}

	RemoveGraphicsCommand::RemoveGraphicsCommand(const QString& name, GraphicsScene * scene, QGraphicsItem * item)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems.clear();
		graphicsItems.append(item);

		itemHandles.append(getHandle(item));
		if (item)
			itemParents.append(item->parentItem());
		else
			itemParents.append(0);

	}

	RemoveGraphicsCommand::RemoveGraphicsCommand(const QString& name, GraphicsScene * scene, const QList<QGraphicsItem*>& items)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems.clear();
		itemParents.clear();
		for (int i=0; i < items.size(); ++i)
			if (items[i] != 0)
			{
				graphicsItems.append(items[i]);
				itemHandles.append(getHandle(items[i]));
				if (items[i])
					itemParents.append(items[i]->parentItem());
				else
					itemParents.append(0);
			}	
	}

	void RemoveGraphicsCommand::redo()
	{
		if (!graphicsScene) return;
		for (int i=0; i<graphicsItems.size(); ++i)
		{
			if (graphicsItems[i] && graphicsItems[i]->scene() == graphicsScene)
			{
				graphicsScene->removeItem(graphicsItems[i]);
				NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(graphicsItems[i]);
				if (node)
				{
					node->setBoundingBoxVisible(false);
				}
				else
				{
					ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(graphicsItems[i]);
					if (connection)
					{
						connection->setControlPointsVisible(false);
						for (int j=0; j < connection->pathVectors.size(); ++j)
						{
							if (connection->pathVectors[j].arrowStart && connection->pathVectors[j].arrowStart->scene() == graphicsScene)	
								graphicsScene->removeItem(connection->pathVectors[j].arrowStart);
							if (connection->pathVectors[j].arrowEnd && connection->pathVectors[j].arrowEnd->scene() == graphicsScene)	
								graphicsScene->removeItem(connection->pathVectors[j].arrowEnd);
						}
						if (connection->centerRegionItem)	
							graphicsScene->removeItem(connection->centerRegionItem);
					}
				}
			}

			if (itemHandles.size() > i && itemHandles[i] != 0)
			{
				setHandle(graphicsItems[i],0);
				if (itemHandles[i]->graphicsItems.isEmpty())
				{
					if (itemHandles[i]->parent)
						itemHandles[i]->parent->children.removeAll(itemHandles[i]);

					for (int j=0; j < itemHandles[i]->children.size(); ++j)
						if (itemHandles[i]->children[j])
							itemHandles[i]->children[j]->parent = 0;
				}
			}

		}

		bool firstTime = (oldData.isEmpty());
		if (firstTime)
		{
			QStringList namesToKill;
			for (int i=0; i < itemHandles.size(); ++i)
				if (itemHandles[i] && itemHandles[i]->graphicsItems.isEmpty())
					namesToKill << itemHandles[i]->fullName();

			ItemHandle * handle;
			QList<QGraphicsItem*> items = graphicsScene->items();
			for (int i=0; i < items.size(); ++i)
			{
				handle = getHandle(items[i]);
				if (handle && handle->data && !itemHandles.contains(handle) && !affectedHandles.contains(handle))
					affectedHandles += handle;
			}

			for (int i=0; i < affectedHandles.size(); ++i)
				oldData += ItemData(*affectedHandles[i]->data);  //SAVE old data

			DataTable<qreal> * nDat = 0;
			DataTable<QString> * sDat = 0;

			for (int i=0; i < affectedHandles.size(); ++i) //change all the handle data
			{
				bool affected = false;
				for (int i2=0; i2 < namesToKill.size(); ++i2)
				{
					QString oldname(namesToKill[i2]);

					QRegExp regexp1(QString("^") + oldname + QString("$")),  //just old name
						regexp2(QString("^") + oldname + QString("([^A-Za-z0-9_])")),  //oldname+(!letter/num)
						regexp3(QString("([^A-Za-z0-9_.])") + oldname + QString("$")), //(!letter/num)+oldname
						regexp4(QString("([^A-Za-z0-9_.])") + oldname + QString("([^A-Za-z0-9_])")); //(!letter/num)+oldname+(!letter/num)

					QList< QString > keys = affectedHandles[i]->data->numericalData.keys();
					for (int j=0; j < keys.size(); ++j)  //go through each numeric data
					{
						affected = false;
						nDat = &(affectedHandles[i]->data->numericalData[ keys[j] ]);
						for (int k=0; k < nDat->rows(); ++k)
						{
							if (nDat->rowName(k).contains(regexp1) || nDat->rowName(k).contains(regexp2) ||
								nDat->rowName(k).contains(regexp3) || nDat->rowName(k).contains(regexp4))
							{
								nDat->removeRow(k);
								--k;
								affected = true;
							}
						}
						for (int k=0; k < nDat->cols(); ++k)
						{
							if (nDat->colName(k).contains(regexp1) || nDat->colName(k).contains(regexp2) ||
								nDat->colName(k).contains(regexp3) || nDat->colName(k).contains(regexp4))
							{
								nDat->removeCol(k);
								--k;
								affected = true;
							}
						}
						if (affected)
							ConsoleWindow::message(QObject::tr("data changed : ") + keys[j] + QObject::tr(" in ") + affectedHandles[i]->fullName());
					}

					keys = affectedHandles[i]->data->textData.keys();

					for (int j=0; j < keys.size(); ++j)  //go through each text data
					{
						affected = false;
						sDat = &(affectedHandles[i]->data->textData[ keys[j] ]);
						for (int k=0; k < sDat->rows(); ++k)
						{
							if (sDat->rowName(k).contains(regexp1) || sDat->rowName(k).contains(regexp2) ||
								sDat->rowName(k).contains(regexp3) || sDat->rowName(k).contains(regexp4))
							{
								sDat->removeRow(k);
								--k;
								affected = true;
							}
						}
						for (int k=0; k < sDat->cols(); ++k)
						{
							if (sDat->colName(k).contains(regexp1) || sDat->colName(k).contains(regexp2) ||
								sDat->colName(k).contains(regexp3) || sDat->colName(k).contains(regexp4))
							{
								sDat->removeCol(k);
								--k;
								affected = true;
							}
						}

						QString newname("1.0");
						for (int k=0; k < sDat->rows(); ++k) //substitute each value in the table
							for (int l=0; l < sDat->cols(); ++l)
							{
								QString & target = sDat->value(k,l);// = QString("0.0");

								int n = regexp1.indexIn(target);
								while (n != -1)
								{
									target = newname;
									//target.replace(oldname,newname);
									n = regexp1.indexIn(target);
									affected = true;
								}
								n = regexp2.indexIn(target);
								while (n != -1)
								{
									target = newname;
									//target.replace(regexp2,newname+QString("\\1"));
									n = regexp2.indexIn(target);
									affected = true;
								}
								n = regexp3.indexIn(target);
								while (n != -1)
								{
									target = newname;
									//target.replace(regexp3,QString("\\1")+newname);
									n = regexp3.indexIn(target);
									affected = true;
								}
								n = regexp4.indexIn(target);
								while (n != -1)
								{
									target = newname;
									n = regexp4.indexIn(target);
									affected = true;
								}
							}
							if (affected)
								ConsoleWindow::message(QObject::tr("data changed : ") + keys[j] + QObject::tr(" in ") + affectedHandles[i]->fullName());
					}
				}
			}
		}

		if (newData.isEmpty()) //SAVE new data
		{
			for (int i=0; i < affectedHandles.size(); ++i)
				if (affectedHandles[i]->data)
				{
					newData += ItemData(*affectedHandles[i]->data);
				}
		}
		else
		{
			for (int i=0; i < affectedHandles.size() && i < newData.size() && i < oldData.size(); ++i)
			{
				if (affectedHandles[i] && affectedHandles[i]->data)
					(*affectedHandles[i]->data) = newData[i];
			}
		}
	}

	void RemoveGraphicsCommand::undo()
	{
		if (!graphicsScene) return;

		for (int i=0; i < affectedHandles.size() && i < newData.size() && i < oldData.size(); ++i)
		{
			if (affectedHandles[i] && affectedHandles[i]->data)
				(*affectedHandles[i]->data) = oldData[i];
		}

		for (int i=0; i<graphicsItems.size(); ++i)
		{
			if (graphicsItems[i] && graphicsItems[i]->scene() != graphicsScene)
				graphicsScene->addItem(graphicsItems[i]);

			NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(graphicsItems[i]);
			if (node)
			{						
				node->setBoundingBoxVisible(false);
			}
			else
			{
				ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(graphicsItems[i]);
				if (connection)
				{
					connection->setControlPointsVisible(false);
					connection->refresh();
				}
			}

			if (itemHandles.size() > i && itemHandles[i] != 0)
			{
				setHandle(graphicsItems[i],itemHandles[i]);

				if (itemHandles[i]->parent)
					itemHandles[i]->setParent(itemHandles[i]->parent);

				for (int j=0; j < itemHandles[i]->children.size(); ++j)
					if (itemHandles[i]->children[j])
						itemHandles[i]->children[j]->parent = itemHandles[i];

			}
			if (itemParents.size() > i && itemParents[i] != 0)
			{
				graphicsItems[i]->setParentItem(itemParents[i]);
			}
		}
	}


	ChangeBrushCommand::ChangeBrushCommand(const QString& name, QGraphicsItem * item, const QBrush& to)
		: QUndoCommand(name)
	{
		graphicsItems.clear();
		oldBrush.clear();
		newBrush.clear();

		QAbstractGraphicsShapeItem * aitem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(item);
		NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(item);
		ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(item);
		ControlPoint * controlPoint = qgraphicsitem_cast<ControlPoint*>(item);
		if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(item);
		if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(item);
		if (shape != 0)
		{
			graphicsItems.append(shape);
			oldBrush.append(shape->defaultBrush);
		}
		else
			if (connection != 0)
			{
				graphicsItems.append(connection);
				oldBrush.append(connection->defaultBrush);
			}
			else
				if (controlPoint != 0)
				{
					graphicsItems.append(controlPoint);
					oldBrush.append(controlPoint->defaultBrush);
				}
				else
					if (aitem != 0)
					{
						graphicsItems.append(aitem);
						oldBrush.append(aitem->brush());
					}

					newBrush.append(to);
	}

	ChangeBrushCommand::ChangeBrushCommand(const QString& name, const QList<QGraphicsItem*>& items, const QList<QBrush>& to)
		: QUndoCommand(name)
	{
		newBrush.clear();
		for (int i=0; i < items.size(); ++i)
		{
			QAbstractGraphicsShapeItem * aitem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(items[i]);
			NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(items[i]);
			ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i]);
			ControlPoint * controlPoint = qgraphicsitem_cast<ControlPoint*>(items[i]);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(items[i]);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(items[i]);
			if (shape != 0)
			{
				graphicsItems.append(shape);
				oldBrush.append(shape->defaultBrush);
			}
			else
				if (connection != 0)
				{
					graphicsItems.append(connection);
					oldBrush.append(connection->defaultBrush);
				}
				else
					if (controlPoint != 0)
					{
						graphicsItems.append(controlPoint);
						oldBrush.append(controlPoint->defaultBrush);
					}
					else
						if (aitem != 0)
						{
							graphicsItems.append(aitem);
							oldBrush.append(aitem->brush());
						}
		}
		newBrush = to;
	}
	void ChangeBrushCommand::redo()
	{
		for (int i=0; i < graphicsItems.size() && i < newBrush.size() && i < oldBrush.size(); ++i)
		{
			QAbstractGraphicsShapeItem * aitem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(graphicsItems[i]);
			NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(graphicsItems[i]);
			ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(graphicsItems[i]);
			ControlPoint * controlPoint = qgraphicsitem_cast<ControlPoint*>(graphicsItems[i]);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(graphicsItems[i]);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(graphicsItems[i]);
			if (shape != 0)
				shape->setBrush( shape->defaultBrush = newBrush[i] );	
			else
				if (connection != 0)
					connection->setBrush( connection->defaultBrush = newBrush[i] );
				else
					if (controlPoint != 0)
						controlPoint->setBrush( controlPoint->defaultBrush = newBrush[i] );
					else
						if (aitem != 0)
							aitem->setBrush(newBrush[i]);
		}
	}
	void ChangeBrushCommand::undo()
	{
		for (int i=0; i < graphicsItems.size() && i < oldBrush.size() && i < newBrush.size(); ++i)
		{
			QAbstractGraphicsShapeItem * aitem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(graphicsItems[i]);
			NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(graphicsItems[i]);
			ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(graphicsItems[i]);
			ControlPoint * controlPoint = qgraphicsitem_cast<ControlPoint*>(graphicsItems[i]);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(graphicsItems[i]);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(graphicsItems[i]);
			if (shape != 0)
				shape->setBrush( shape->defaultBrush = oldBrush[i] );
			else
				if (connection != 0)
					connection->setBrush( connection->defaultBrush = oldBrush[i] );
				else
					if (controlPoint != 0)
						controlPoint->setBrush( controlPoint->defaultBrush = oldBrush[i] );
					else
						if (aitem != 0)
							aitem->setBrush(oldBrush[i]);
		}
	}

	ChangePenCommand::ChangePenCommand(const QString& name, QGraphicsItem * item, const QPen& to)
		: QUndoCommand(name)
	{
		graphicsItems.clear();
		oldPen.clear();
		newPen.clear();
		QAbstractGraphicsShapeItem * aitem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(item);
		NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(item);
		ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(item);
		ControlPoint * controlPoint = qgraphicsitem_cast<ControlPoint*>(item);
		if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(item);
		if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(item);
		TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(item);
		if (shape != 0)
		{
			graphicsItems.append(shape);
			oldPen.append(shape->defaultPen);
		}
		else
			if (connection != 0)
			{
				graphicsItems.append(connection);
				oldPen.append(connection->defaultPen);
			}
			else
				if (controlPoint != 0)
				{
					graphicsItems.append(controlPoint);
					oldPen.append(controlPoint->defaultPen);
				}
				else
					if (textItem != 0)
					{
						graphicsItems.append(textItem);
						oldPen.append(QPen(textItem->defaultTextColor()));
					}
					else
						if (aitem != 0)
						{
							graphicsItems.append(aitem);
							oldPen.append(aitem->pen());
						}

						newPen.append(to);
	}

	ChangePenCommand::ChangePenCommand(const QString& name, const QList<QGraphicsItem*>& items, const QList<QPen>& to)
		: QUndoCommand(name)
	{
		newPen.clear();
		for (int i=0; i < items.size(); ++i)
		{
			QAbstractGraphicsShapeItem * aitem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(items[i]);
			NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(items[i]);
			ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i]);
			ControlPoint * controlPoint = qgraphicsitem_cast<ControlPoint*>(items[i]);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(items[i]);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(items[i]);
			TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(items[i]);
			if (shape != 0)
			{
				graphicsItems.append(shape);
				oldPen.append(shape->defaultPen);
			}
			else
				if (connection != 0)
				{
					graphicsItems.append(connection);
					oldPen.append(connection->defaultPen);
				}
				else
					if (controlPoint != 0)
					{
						graphicsItems.append(controlPoint);
						oldPen.append(controlPoint->defaultPen);
					}
					else
						if (textItem != 0)
						{
							graphicsItems.append(textItem);
							oldPen.append(QPen(textItem->defaultTextColor()));
						}
						else
							if (aitem != 0)
							{
								graphicsItems.append(aitem);
								oldPen.append(aitem->pen());
							}
		}
		newPen = to;
	}
	void ChangePenCommand::redo()
	{
		for (int i=0; i < graphicsItems.size() && i < oldPen.size() && i < newPen.size(); ++i)
		{
			QAbstractGraphicsShapeItem * aitem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(graphicsItems[i]);
			NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(graphicsItems[i]);
			ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(graphicsItems[i]);
			ControlPoint * controlPoint = qgraphicsitem_cast<ControlPoint*>(graphicsItems[i]);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(graphicsItems[i]);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(graphicsItems[i]);
			TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(graphicsItems[i]);
			if (shape != 0)
				shape->setPen( shape->defaultPen = newPen[i] );
			else
				if (connection != 0)
				{
					connection->setPen( connection->defaultPen = newPen[i] );
					connection->refresh();
				}
				else
					if (controlPoint != 0)
						controlPoint->setPen( controlPoint->defaultPen = newPen[i] );
					else
						if (textItem != 0)
							textItem->setDefaultTextColor( newPen[i].color() );
						else
							if (aitem != 0)
								aitem->setPen(newPen[i]);
		}
	}

	void ChangePenCommand::undo()
	{
		for (int i=0; i < graphicsItems.size() && i < oldPen.size() && i < newPen.size(); ++i)
		{
			QAbstractGraphicsShapeItem * aitem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(graphicsItems[i]);
			NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(graphicsItems[i]);
			ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(graphicsItems[i]);
			ControlPoint * controlPoint = qgraphicsitem_cast<ControlPoint*>(graphicsItems[i]);
			TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(graphicsItems[i]);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(graphicsItems[i]);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(graphicsItems[i]);
			if (shape != 0)
				shape->setPen( shape->defaultPen = oldPen[i] );
			else
				if (connection != 0)
				{
					connection->setPen( connection->defaultPen = oldPen[i] );
					connection->refresh();
				}
				else
					if (controlPoint != 0)
						controlPoint->setPen( controlPoint->defaultPen = oldPen[i] );
					else
						if (textItem != 0)
							textItem->setDefaultTextColor( oldPen[i].color() );
						else
							if (aitem != 0)
								aitem->setPen(oldPen[i]);
		}
	}

	ChangeBrushAndPenCommand::ChangeBrushAndPenCommand(const QString& name, QGraphicsItem * item, const QBrush& brush, const QPen& pen)
		: QUndoCommand(name)
	{
		graphicsItems.clear();
		oldPen.clear();
		oldBrush.clear();
		newPen.clear();
		newBrush.clear();

		QAbstractGraphicsShapeItem * aitem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(item);
		NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(item);
		ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(item);
		ControlPoint * controlPoint = qgraphicsitem_cast<ControlPoint*>(item);
		if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(item);
		if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(item);
		TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(item);
		if (aitem != 0 && shape == 0 && controlPoint == 0)
		{
			graphicsItems.append(aitem);
			oldBrush.append(aitem->brush());
			oldPen.append(aitem->pen());
		}
		else
		{
			if (shape != 0)
			{
				graphicsItems.append(shape);
				oldBrush.append(shape->defaultBrush);
				oldPen.append(shape->defaultPen);
			}
			else
				if (connection != 0)
				{
					graphicsItems.append(connection);
					oldBrush.append(connection->defaultBrush);
					oldPen.append(connection->defaultPen);
				}
				else
					if (controlPoint != 0)
					{
						graphicsItems.append(controlPoint);
						oldBrush.append(controlPoint->defaultBrush);
						oldPen.append(controlPoint->defaultPen);
					}
					else
						if (textItem != 0)
						{
							graphicsItems.append(textItem);
							oldBrush.append(QBrush(textItem->defaultTextColor()));
							oldPen.append(QPen(textItem->defaultTextColor()));
						}
		}

		newPen.append(pen);
		newBrush.append(brush);	
	}

	ChangeBrushAndPenCommand::ChangeBrushAndPenCommand(const QString& name, const QList<QGraphicsItem*>& items, const QList<QBrush>& brushes, const QList<QPen>& pens)
		: QUndoCommand(name)
	{
		newPen.clear();
		newBrush.clear();
		for (int i=0; i < items.size(); ++i)
		{
			QAbstractGraphicsShapeItem * aitem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(items[i]);
			NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(items[i]);
			ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(graphicsItems[i]);
			ControlPoint * controlPoint = qgraphicsitem_cast<ControlPoint*>(items[i]);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(items[i]);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(items[i]);
			TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(items[i]);
			if (aitem != 0 && shape == 0 && connection == 0 && controlPoint == 0)
			{
				graphicsItems.append(aitem);
				oldBrush.append(aitem->brush());
				oldPen.append(aitem->pen());
			}
			else
			{
				if (shape != 0)
				{
					graphicsItems.append(shape);
					oldBrush.append(shape->defaultBrush);
					oldPen.append(shape->defaultPen);
				}
				else
					if (connection != 0)
					{
						graphicsItems.append(connection);
						oldBrush.append(connection->defaultBrush);
						oldPen.append(connection->defaultPen);
					}
					else
						if (controlPoint != 0)
						{
							graphicsItems.append(controlPoint);
							oldBrush.append(controlPoint->defaultBrush);
							oldPen.append(controlPoint->defaultPen);
						}
						else
							if (textItem != 0)
							{
								graphicsItems.append(textItem);
								oldBrush.append(QBrush(textItem->defaultTextColor()));
								oldPen.append(QPen(textItem->defaultTextColor()));
							}
			}
		}
		newBrush = brushes;
		newPen = pens;
	}

	void ChangeBrushAndPenCommand::redo()
	{
		for (int i=0; i < graphicsItems.size(); ++i)
		{
			if (i < newPen.size())
			{
				QAbstractGraphicsShapeItem * aitem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(graphicsItems[i]);
				NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(graphicsItems[i]);
				ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(graphicsItems[i]);
				ControlPoint * controlPoint = qgraphicsitem_cast<ControlPoint*>(graphicsItems[i]);
				if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(graphicsItems[i]);
				if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(graphicsItems[i]);
				TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(graphicsItems[i]);
				if (shape != 0)
					shape->setPen( shape->defaultPen = newPen[i] );
				else
					if (connection != 0)
					{
						connection->setPen( connection->defaultPen = newPen[i] );
						connection->refresh();
					}
					else
						if (controlPoint != 0)
							controlPoint->setPen( controlPoint->defaultPen = newPen[i] );
						else
							if (textItem != 0)
								textItem->setDefaultTextColor( newPen[i].color() );
							else
								if (aitem != 0)
									aitem->setPen(newPen[i]);
			}
			if (i < newBrush.size())
			{
				QAbstractGraphicsShapeItem * aitem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(graphicsItems[i]);
				NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(graphicsItems[i]);
				ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(graphicsItems[i]);
				ControlPoint * controlPoint = qgraphicsitem_cast<ControlPoint*>(graphicsItems[i]);
				if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(graphicsItems[i]);
				if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(graphicsItems[i]);
				if (shape != 0)
					shape->setBrush( shape->defaultBrush = newBrush[i] );
				else
					if (connection != 0)
					{
						connection->setBrush( connection->defaultBrush = newBrush[i] );
						connection->refresh();
					}
					else
						if (controlPoint != 0)
							controlPoint->setBrush( controlPoint->defaultBrush = newBrush[i] );
						else
							if (aitem != 0)
								aitem->setBrush(newBrush[i]);
			}
		}
	}

	void ChangeBrushAndPenCommand::undo()
	{
		for (int i=0; i < graphicsItems.size(); ++i)
		{
			if (i < oldPen.size() && i < newPen.size())
			{
				QAbstractGraphicsShapeItem * aitem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(graphicsItems[i]);
				NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(graphicsItems[i]);
				ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(graphicsItems[i]);
				ControlPoint * controlPoint = qgraphicsitem_cast<ControlPoint*>(graphicsItems[i]);
				if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(graphicsItems[i]);
				if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(graphicsItems[i]);
				TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(graphicsItems[i]);
				if (shape != 0)
					shape->setPen( shape->defaultPen = oldPen[i] );
				else
					if (connection != 0)
					{
						connection->setPen( connection->defaultPen = oldPen[i] );
						connection->refresh();
					}
					else
						if (controlPoint != 0)
							controlPoint->setPen( controlPoint->defaultPen = oldPen[i] );
						else
							if (textItem != 0)
								textItem->setDefaultTextColor( oldPen[i].color() );
							else
								if (aitem != 0)
									aitem->setPen(oldPen[i]);
			}
			if (i < oldBrush.size() && i < newBrush.size())
			{
				QAbstractGraphicsShapeItem * aitem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(graphicsItems[i]);
				NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(graphicsItems[i]);
				ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(graphicsItems[i]);
				ControlPoint * controlPoint = qgraphicsitem_cast<ControlPoint*>(graphicsItems[i]);
				if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(graphicsItems[i]);
				if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(graphicsItems[i]);
				if (shape != 0)
					shape->setBrush( shape->defaultBrush = oldBrush[i] );
				else
					if (controlPoint != 0)
						controlPoint->setBrush( controlPoint->defaultBrush = oldBrush[i] );
					else
						if (connection != 0)
						{
							connection->setBrush(connection->defaultBrush = oldBrush[i]);
							connection->refresh();
						}
						else
							if (aitem != 0)
								aitem->setBrush(oldBrush[i]);
			}
		}
	}

	TransformCommand::TransformCommand(const QString& name, QGraphicsScene * scene, QGraphicsItem * item, 
		const QPointF& sizeChange, qreal changeInAngle,
		bool VFlip, bool HFlip)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems.append(item);
		sizeFactor.clear();
		angleChange.clear();

		sizeFactor.append(sizeChange);
		angleChange.append(changeInAngle);
		vFlip = VFlip;
		hFlip = HFlip;
	}

	TransformCommand::TransformCommand(const QString& name, QGraphicsScene * scene, const QList<QGraphicsItem *>& items, 
		const QList<QPointF>& sizeChange, const QList<qreal>& changeInAngle,
		bool VFlip, bool HFlip)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems = items;

		sizeFactor = sizeChange;
		angleChange = changeInAngle;
		vFlip = VFlip;
		hFlip = HFlip;
	}

	void TransformCommand::redo()
	{
		for (int i=0; i < graphicsItems.size(); ++i)

			if (graphicsItems[i])
			{
				if (sizeFactor.size() > i && !sizeFactor[i].isNull())
					graphicsItems[i]->scale(sizeFactor[i].x(),sizeFactor[i].y());

				if (angleChange.size() > i)
					graphicsItems[i]->rotate(angleChange[i]);

				if (hFlip)
					graphicsItems[i]->scale(-1,1);

				if (vFlip)
					graphicsItems[i]->scale(1,-1);

				NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(graphicsItems[i]);
				if (node)
				{
					node->adjustBoundaryControlPoints();
				}
			}
	}

	void TransformCommand::undo()
	{
		for (int i=0; i < graphicsItems.size(); ++i)

			if (graphicsItems[i])
			{
				if (sizeFactor.size() > i && !sizeFactor[i].isNull())
					graphicsItems[i]->scale(1.0/sizeFactor[i].x(),1.0/sizeFactor[i].y());

				if (angleChange.size() > i)
					graphicsItems[i]->rotate(-angleChange[i]);

				if (hFlip)
					graphicsItems[i]->scale(-1,1);

				if (vFlip)
					graphicsItems[i]->scale(1,-1);

				NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(graphicsItems[i]);
				if (node)
				{
					node->adjustBoundaryControlPoints();
				}
			}
	}

	ChangeZCommand::ChangeZCommand(const QString& name, QGraphicsScene * scene, QGraphicsItem * item, double to)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems.clear();
		oldZ.clear();
		newZ.clear();
		if (item != 0)
		{
			graphicsItems.append(item->topLevelItem());
			oldZ.append(item->zValue());
			newZ.append(to);
		}
	}

	ChangeZCommand::ChangeZCommand(const QString& name, QGraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<double>& to)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		newZ.clear();
		for (int i=0; i < items.size(); ++i)
		{
			QGraphicsItem * aitem = (items[i]);
			if (aitem != 0)
			{
				graphicsItems.append(aitem->topLevelItem());
				oldZ.append(aitem->zValue());
			}
		}
		newZ = to;
	}

	void ChangeZCommand::redo()
	{
		for (int i=0; i < graphicsItems.size() && i < newZ.size(); ++i)
		{
			if (graphicsItems[i] != 0)
			{
				graphicsItems[i]->setZValue(newZ[i]);
			}
		}
	}

	void ChangeZCommand::undo()
	{
		for (int i=0; i < graphicsItems.size() && i < oldZ.size(); ++i)
		{
			if (graphicsItems[i] != 0)
			{
				graphicsItems[i]->setZValue(oldZ[i]);
			}
		}
	}

	ChangeParentCommand::ChangeParentCommand(const QString& name, QGraphicsScene * scene, QGraphicsItem * item, QGraphicsItem * newParent)
		: QUndoCommand(name)
	{
		this->scene = scene;
		graphicsItems.clear();
		oldParents.clear();
		newParents.clear();
		if (item != 0)
		{
			graphicsItems.append(item);
			oldParents.append(item->parentItem());
			newParents.append(newParent);
		}
	}

	ChangeParentCommand::ChangeParentCommand(const QString& name, QGraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<QGraphicsItem*>& to)
		: QUndoCommand(name)
	{
		this->scene = scene;
		oldParents.clear();
		newParents.clear();
		for (int i=0; i < items.size(); ++i)
		{
			QGraphicsItem * aitem = (items[i]);
			if (aitem != 0)
			{
				graphicsItems.append(aitem);
				oldParents.append(aitem->parentItem());
			}
		}
		newParents = to;
	}

	void ChangeParentCommand::redo()
	{
		if (scene == 0) return;

		for (int i=0; i < graphicsItems.size() && i < newParents.size(); ++i)
		{
			if (graphicsItems[i] != 0)
			{
				QPointF pos = graphicsItems[i]->scenePos();
				if (newParents[i] == 0)
				{
					graphicsItems[i]->setParentItem(0);
					if (graphicsItems[i]->scene() != scene)
						scene->addItem(graphicsItems[i]);
					graphicsItems[i]->setPos(pos);
				}
				else
				{
					graphicsItems[i]->setParentItem(newParents[i]);
					graphicsItems[i]->setPos( newParents[i]->mapFromScene(pos) );
				}
			}
		}
	}

	void ChangeParentCommand::undo()
	{
		if (scene == 0) return;

		for (int i=0; i < graphicsItems.size() && i < oldParents.size(); ++i)
		{
			if (graphicsItems[i] != 0)
			{
				QPointF pos = graphicsItems[i]->scenePos();
				if (oldParents[i] == 0)
				{
					graphicsItems[i]->setParentItem(0);
					if (graphicsItems[i]->scene() != scene)
						scene->addItem(graphicsItems[i]);
					graphicsItems[i]->setPos(pos);
				}
				else
				{
					graphicsItems[i]->setParentItem(oldParents[i]);
					graphicsItems[i]->setPos( oldParents[i]->mapFromScene(pos) );
				}
			}
		}
	}

	RenameCommand::RenameCommand(const QString& name, NetworkWindow * win, ItemHandle * handle, const QString& newname)
		: QUndoCommand(name), changeDataCommand(0)
	{
		if (win)
			allhandles = win->allHandles();

		handles.clear();
		oldNames.clear();
		newNames.clear();

		ItemHandle * handle1;
		QStringList allNames;
		for (int i=0; i < allhandles.size(); ++i)
			if ((handle1 = (allhandles[i])) && (handle != handle1))
				allNames << handle1->fullName();

		if (handle)
		{
			handles += handle;
			oldNames += handle->fullName();
			newNames += assignUniqueName(newname,allNames);
		}		
	}

	RenameCommand::RenameCommand(const QString& name, const QList<ItemHandle*>& allItems, const QString& oldname, const QString& newname)
		: QUndoCommand(name), changeDataCommand(0)
	{
		this->allhandles = allItems;
		handles.clear();
		oldNames.clear();
		newNames.clear();

		oldNames += oldname;
		newNames += newname;
	}

	RenameCommand::RenameCommand(const QString& name, const QList<ItemHandle*>& allItems, const QList<QString>& oldname, const QList<QString>& newname)
		: QUndoCommand(name), changeDataCommand(0)
	{
		this->allhandles = allItems;
		handles.clear();
		oldNames.clear();
		newNames.clear();

		oldNames << oldname;
		newNames << newname;
	}

	RenameCommand::RenameCommand(const QString& name, const QList<ItemHandle*>& allItems, ItemHandle * handle, const QString& newname)
		: QUndoCommand(name), changeDataCommand(0)
	{
		this->allhandles = allItems;
		handles.clear();
		oldNames.clear();
		newNames.clear();

		ItemHandle * handle1;
		QStringList allNames;
		for (int i=0; i < allItems.size(); ++i)
			if ((handle1 = (allItems[i])) && (handle != handle1))
				allNames << handle1->fullName();

		if (handle)
		{
			handles += handle;
			oldNames += handle->fullName();
			newNames += assignUniqueName(newname,allNames);
		}		
	}

	RenameCommand::RenameCommand(const QString& name, NetworkWindow * win, const QList<ItemHandle*>& items, const QList<QString>& newnames)
		: QUndoCommand(name), changeDataCommand(0)
	{
		if (win)
			this->allhandles = win->allHandles();
		handles.clear();
		oldNames.clear();
		newNames.clear();

		ItemHandle * handle;
		QStringList allNames;
		for (int i=0; i < allhandles.size(); ++i)
			if ((handle = (allhandles[i])) && !items.contains(handle))
				allNames << handle->fullName();

		for (int i=0; i < items.size() && i < newnames.size() ; ++i)
		{
			handle = items[i];
			if (handle)
			{
				handles += handle;
				oldNames += handle->fullName();
				newNames += assignUniqueName(newnames[i],allNames);
			}
		}
	}

	RenameCommand::RenameCommand(const QString& name, const QList<ItemHandle*>& allItems, const QList<ItemHandle*>& items, const QList<QString>& newnames)
		: QUndoCommand(name), changeDataCommand(0)
	{
		this->allhandles = allItems;
		handles.clear();
		oldNames.clear();
		newNames.clear();
		ItemHandle * handle;
		QStringList allNames;
		for (int i=0; i < allItems.size(); ++i)
			if ((handle = (allItems[i])) && !items.contains(handle))
				allNames << handle->fullName();

		for (int i=0; i < items.size() && i < newnames.size() ; ++i)
		{
			handle = (items[i]);
			if (handle)
			{
				handles += handle;
				oldNames += handle->fullName();
				newNames += assignUniqueName(newnames[i],allNames);
			}
		}
	}

	void RenameCommand::substituteString(QString& target, const QString& oldname,const QString& newname0)
	{
		if (oldname == newname0 || target.size() > 1000) return;
		QString newname = newname0;
		newname.replace(QRegExp("[^A-Za-z0-9_]"),QString("_x_x_x_"));

		//qDebug() << "substitute " << oldname << " with " << newname0 << " in " << target;
		QRegExp regexp1(QString("^") + oldname + QString("$")),  //just old name
			regexp2(QString("^") + oldname + QString("([^A-Za-z0-9_])")),  //oldname+(!letter/num)
			regexp3(QString("([^A-Za-z0-9_.])") + oldname + QString("$")), //(!letter/num)+oldname
			regexp4(QString("([^A-Za-z0-9_.])") + oldname + QString("([^A-Za-z0-9_])")); //(!letter/num)+oldname+(!letter/num)
		int n = regexp1.indexIn(target);
		while (n != -1)
		{
			target.replace(oldname,newname);
			n = regexp1.indexIn(target);
		}
		n = regexp2.indexIn(target);
		while (n != -1)
		{
			target.replace(regexp2,newname+QString("\\1"));
			n = regexp2.indexIn(target);
		}
		n = regexp3.indexIn(target);
		while (n != -1)
		{
			target.replace(regexp3,QString("\\1")+newname);
			n = regexp3.indexIn(target);
		}
		n = regexp4.indexIn(target);
		while (n != -1)
		{
			target.replace(regexp4,QString("\\1")+newname+QString("\\2"));
			n = regexp4.indexIn(target);
		}
		target.replace(newname,newname0);
	}

	void RenameCommand::findReplaceAllHandleData(const QList<ItemHandle*>& handles,const QString& oldname,const QString& newname)
	{
		DataTable<qreal> * nDat = 0;
		DataTable<QString> * sDat = 0;

		for (int i=0; i < handles.size(); ++i)
		{
			if (handles[i] && handles[i]->data)  //go through each handles num data and text data
			{
				QList< QString > keys = handles[i]->data->numericalData.keys();
				for (int j=0; j < keys.size(); ++j)  //go through each num data
				{
					nDat = &(handles[i]->data->numericalData[ keys[j] ]);
					for (int k=0; k < nDat->rows(); ++k)
					{
						if (nDat->rowName(k).contains(oldname))
							substituteString(nDat->rowName(k),oldname,newname);
					}
					for (int k=0; k < nDat->cols(); ++k)
					{
						if (nDat->colName(k).contains(oldname))
							substituteString(nDat->colName(k),oldname,newname);
					}
				}
				keys = handles[i]->data->textData.keys();
				for (int j=0; j < keys.size(); ++j)  //go through each text data
				{
					sDat = &(handles[i]->data->textData[ keys[j] ]);
					for (int k=0; k < sDat->rows(); ++k)
					{
						if (sDat->rowName(k).contains(oldname))
							substituteString(sDat->rowName(k),oldname,newname);
					}
					for (int k=0; k < sDat->cols(); ++k)
					{
						if (sDat->colName(k).contains(oldname))
							substituteString(sDat->colName(k),oldname,newname);
					}
					for (int k=0; k < sDat->rows(); ++k) //substitute each value in the table
						for (int l=0; l < sDat->cols(); ++l)
						{
							if (sDat->value(k,l).contains(oldname))
								substituteString(sDat->value(k,l),oldname,newname);
						}
				}
			}
		}
	}

	QString RenameCommand::assignUniqueName(const QString& str,const QStringList& notAvailable)
	{
		QString name = str;
		bool taken = true;
		int c = 0;
		while (taken)
		{
			taken = (notAvailable.contains(name));
			if (taken)
			{
				name = name + QString::number(c);
				++c;
			}
		}
		return name;
	}

	RenameCommand::~RenameCommand()
	{
		//if (changeDataCommand)
		//delete changeDataCommand;
		//changeDataCommand = 0;
	}

	void RenameCommand::redo()
	{
		bool firstTime = (changeDataCommand == 0);
		QList< DataTable<qreal>* > oldData1, newData1;
		QList< DataTable<QString>* > oldData2, newData2;

		if (firstTime)
		{
			for (int i=0; i < allhandles.size(); ++i)
				if (allhandles[i]->data)
				{
					QList<QString> keys1 = allhandles[i]->data->numericalData.keys();
					QList<QString> keys2 = allhandles[i]->data->textData.keys();

					for (int j=0; j < keys1.size(); ++j)
						oldData1 += new DataTable<qreal>(allhandles[i]->data->numericalData[ keys1[j] ]);

					for (int j=0; j < keys2.size(); ++j)
						oldData2 += new DataTable<QString>(allhandles[i]->data->textData[ keys2[j] ]);
				}
		}

		QRegExp regexp("\\.([^\\.]+)$");
		for (int i=0; i < oldNames.size() && i < newNames.size(); ++i)
		{
			if (firstTime)
				findReplaceAllHandleData(allhandles,oldNames[i],newNames[i]);
			if (handles.size() == 0)
				for (int j=0; j < allhandles.size(); ++j)				
					if (allhandles[j] && allhandles[j]->fullName() == oldNames[i])
					{
						regexp.indexIn(newNames[i]);
						for (int k=0; k < allhandles[j]->graphicsItems.size(); ++k)
						{
							TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(allhandles[j]->graphicsItems[k]);
							if (textItem)
							{
								if (textItem->toPlainText() == allhandles[j]->name)
								{
									if (regexp.numCaptures() > 0 && !regexp.cap(1).isEmpty())
										textItem->setPlainText(regexp.cap(1));
									else
										textItem->setPlainText(newNames[i]);
								}
							}
							else
							if (allhandles[j]->graphicsItems[k])
							{
								allhandles[j]->graphicsItems[k]->update();
							}
						}
						if (regexp.numCaptures() > 0 && !regexp.cap(1).isEmpty())
							allhandles[j]->name = regexp.cap(1);
						else
							allhandles[j]->name = newNames[i];
					}
		}

		if (firstTime)
		{
			for (int i=0; i < allhandles.size(); ++i)
				if (allhandles[i]->data)
				{
					QList<QString> keys1 = allhandles[i]->data->numericalData.keys();
					QList<QString> keys2 = allhandles[i]->data->textData.keys();

					for (int j=0; j < keys1.size(); ++j)
						newData1 += &(allhandles[i]->data->numericalData[ keys1[j] ]);

					for (int j=0; j < keys2.size(); ++j)
						newData2 += &(allhandles[i]->data->textData[ keys2[j] ]);
				}
				changeDataCommand = new Change2DataCommand<qreal,QString>(QString(""), newData1, oldData1, newData2, oldData2);
				for (int i=0; i < oldData1.size(); ++i)
					if (oldData1[i])
						delete oldData1[i];
				for (int i=0; i < oldData2.size(); ++i)
					if (oldData2[i])
						delete oldData2[i];
		}
		else
		{
			if (changeDataCommand)
				changeDataCommand->undo();
		}

		for (int i=0; i < handles.size() && i < newNames.size(); ++i)
		{
			ItemHandle * handle = handles[i];
			if (handle)
			{
				regexp.indexIn(newNames[i]);
				for (int j=0; j < handle->graphicsItems.size(); ++j)
				{
					TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(handle->graphicsItems[j]);
					if (textItem)
					{
						if (textItem->toPlainText() == handle->name)
						{
							if (regexp.numCaptures() > 0 && !regexp.cap(1).isEmpty())
								textItem->setPlainText(regexp.cap(1));
							else
								textItem->setPlainText(newNames[i]);
						}
					}
				}
				if (regexp.numCaptures() > 0 && !regexp.cap(1).isEmpty())
					handle->name = regexp.cap(1);
				else
					handle->name = newNames[i];
			}
		}
	}

	void RenameCommand::undo()
	{		
		QRegExp regexp("\\.([^\\.]+)$");
		for (int i=0; i < oldNames.size() && i < newNames.size(); ++i)
		{
			if (handles.size() == 0)
				for (int j=0; j < allhandles.size(); ++j)				
					if (allhandles[j] && allhandles[j]->fullName() == newNames[i])
					{
						regexp.indexIn(oldNames[i]);
						for (int k=0; k < allhandles[j]->graphicsItems.size(); ++k)
						{
							TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(allhandles[j]->graphicsItems[k]);
							if (textItem)
							{
								if (textItem->toPlainText() == allhandles[j]->name)
								{
									if (regexp.numCaptures() > 0 && !regexp.cap(1).isEmpty())
										textItem->setPlainText(regexp.cap(1));
									else
										textItem->setPlainText(oldNames[i]);
								}
							}
							else
							if (allhandles[j]->graphicsItems[k])
							{
								allhandles[j]->graphicsItems[k]->update();
							}
						}
						if (regexp.numCaptures() > 0 && !regexp.cap(1).isEmpty())
							allhandles[j]->name = regexp.cap(1);
						else
							allhandles[j]->name = oldNames[i];
					}
		}

		if (changeDataCommand)
			changeDataCommand->redo();

		for (int i=0; i < handles.size() && i < oldNames.size(); ++i)
		{
			ItemHandle * handle = (handles[i]);
			if (handle)
			{
				regexp.indexIn(oldNames[i]);
				for (int j=0; j < handle->graphicsItems.size(); ++j)
				{
					TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(handle->graphicsItems[j]);
					if (textItem)
					{
						if (textItem->toPlainText() == handle->name)
						{
							if (regexp.numCaptures() > 0 && !regexp.cap(1).isEmpty())
								textItem->setPlainText(regexp.cap(1));
							else
								textItem->setPlainText(oldNames[i]);
						}
					}
				}
				if (regexp.numCaptures() > 0 && !regexp.cap(1).isEmpty())
					handle->name = regexp.cap(1);
				else
					handle->name = oldNames[i];
			}
		}
	}

	CompositeCommand::CompositeCommand(const QString& name, const QList<QUndoCommand*>& list, const QList<QUndoCommand*>& noDelete)
		: QUndoCommand(name)
	{
		commands = list;
		doNotDelete = noDelete;

		for (int i=0; i < noDelete.size(); ++i)
			if (!list.contains(noDelete[i]))
				commands += noDelete[i];
	}	
	CompositeCommand::CompositeCommand(const QString& name, QUndoCommand* cmd1, QUndoCommand* cmd2, bool deleteCommand)
		: QUndoCommand(name)
	{
		commands += cmd1;
		commands += cmd2;
		if (!deleteCommand)
		{
			doNotDelete += cmd1;
			doNotDelete += cmd2;
		}
	}	
	CompositeCommand::~CompositeCommand()
	{
		for (int i=0; i < commands.size(); ++i)
			if (commands[i] && !doNotDelete.contains(commands[i]))
				delete commands[i];
	}
	void CompositeCommand::redo()
	{
		for (int i=0; i < commands.size(); ++i)
		{
			if (commands[i])
				commands[i]->redo();
		}
	}
	void CompositeCommand::undo()
	{
		for (int i=commands.size()-1; i >= 0; --i)
		{
			if (commands[i])
				commands[i]->undo();
		}
	}

	ReverseUndoCommand::ReverseUndoCommand(const QString& name, QUndoCommand* cmd, bool deleteCmd)
		: QUndoCommand(name), command(cmd), deleteCommand(deleteCmd)
	{
	}
	ReverseUndoCommand::~ReverseUndoCommand()
	{
		if (command && deleteCommand) delete command;
	}
	void ReverseUndoCommand::redo()
	{
		if (command) command->undo();
	}
	void ReverseUndoCommand::undo()
	{
		if (command) command->redo();
	}

	ReplaceNodeGraphicsCommand::ReplaceNodeGraphicsCommand(const QString& text,NodeGraphicsItem* node,const QString& filename)
		: QUndoCommand(text)
	{
		if (node && !qgraphicsitem_cast<Tool::GraphicsItem*>(node->topLevelItem()))
		{
			targetNodes += node;
			NodeGraphicsItem copy1(*node);
			oldNodes += copy1;

			NodeGraphicsItem copy2(*node);
			loadFromFile(&copy2,filename);
			newNodes += copy2;
		}
	}

	ReplaceNodeGraphicsCommand::ReplaceNodeGraphicsCommand(const QString& text,const QList<NodeGraphicsItem*>& nodes,const QList<QString>& filenames)
		: QUndoCommand(text)
	{
		for (int i=0; i < nodes.size() && i < filenames.size(); ++i)
		{
			NodeGraphicsItem * node = nodes[i];
			QString filename = filenames[i];
			if (node && !qgraphicsitem_cast<Tool::GraphicsItem*>(node->topLevelItem()))
			{
				targetNodes += node;
				NodeGraphicsItem copy1(*node);
				setHandle(&copy1,0);
				oldNodes += copy1;

				NodeGraphicsItem copy2(*node);
				setHandle(&copy2,0);
				loadFromFile(&copy2,filename);
				newNodes += copy2;
			}
		}
	}

	ReplaceNodeGraphicsCommand::~ReplaceNodeGraphicsCommand()
	{
		for (int i=0; i < itemsToDelete.size(); ++i)
			if (itemsToDelete[i])
			{
				if (itemsToDelete[i]->scene())
					itemsToDelete[i]->scene()->removeItem(itemsToDelete[i]);
				delete itemsToDelete[i];
			}
	}

	void ReplaceNodeGraphicsCommand::redo()
	{
		for (int i=0; i < targetNodes.size(); ++i)
		{
			if (targetNodes[i])
			{
				for (int j=0; j < targetNodes[i]->shapes.size(); ++j)
					if (!itemsToDelete.contains(targetNodes[i]->shapes[j]) && targetNodes[i]->shapes[j])
					{
						itemsToDelete << targetNodes[i]->shapes[j];
					}

					for (int j=0; j < targetNodes[i]->controlPoints.size(); ++j)
						if (!itemsToDelete.contains(targetNodes[i]->controlPoints[j]) && targetNodes[i]->controlPoints[j])
						{
							itemsToDelete << targetNodes[i]->controlPoints[j];
						}

						for (int j=0; j < targetNodes[i]->boundaryControlPoints.size(); ++j)
							if (!itemsToDelete.contains(targetNodes[i]->boundaryControlPoints[j]) && targetNodes[i]->boundaryControlPoints[j])
							{
								itemsToDelete << targetNodes[i]->boundaryControlPoints[j];
							}

							targetNodes[i]->shapes.clear();
							targetNodes[i]->controlPoints.clear();
							targetNodes[i]->boundaryControlPoints.clear();

							(*targetNodes[i]) = newNodes[i];

			}
		}
		for (int i=0; i < itemsToDelete.size(); ++i)
			if (itemsToDelete[i] && itemsToDelete[i]->scene())
				itemsToDelete[i]->scene()->removeItem(itemsToDelete[i]);
	}
	void ReplaceNodeGraphicsCommand::undo()
	{
		for (int i=0; i < targetNodes.size(); ++i)
		{
			if (targetNodes[i])
			{
				for (int j=0; j < targetNodes[i]->shapes.size(); ++j)
					if (!itemsToDelete.contains(targetNodes[i]->shapes[j]) && targetNodes[i]->shapes[j])
					{
						itemsToDelete << targetNodes[i]->shapes[j];
					}

					for (int j=0; j < targetNodes[i]->controlPoints.size(); ++j)
						if (!itemsToDelete.contains(targetNodes[i]->controlPoints[j]) && targetNodes[i]->controlPoints[j])
						{
							itemsToDelete << targetNodes[i]->controlPoints[j];
						}

						for (int j=0; j < targetNodes[i]->boundaryControlPoints.size(); ++j)
							if (!itemsToDelete.contains(targetNodes[i]->boundaryControlPoints[j]) && targetNodes[i]->boundaryControlPoints[j])
							{
								itemsToDelete << targetNodes[i]->boundaryControlPoints[j];
							}

							targetNodes[i]->shapes.clear();
							targetNodes[i]->controlPoints.clear();
							targetNodes[i]->boundaryControlPoints.clear();

							(*targetNodes[i]) = oldNodes[i];

			}
		}
		for (int i=0; i < itemsToDelete.size(); ++i)
			if (itemsToDelete[i] && itemsToDelete[i]->scene())
				itemsToDelete[i]->scene()->removeItem(itemsToDelete[i]);
	}
	void ReplaceNodeGraphicsCommand::loadFromFile(NodeGraphicsItem* node,const QString& fileName)
	{
		if (!node) return;

		QFile file (fileName);

		if (!file.open(QFile::ReadOnly | QFile::Text)) 
		{
			return;
		}

		node->setParentItem(0);

		QPointF p = node->scenePos();			
		QTransform t0 = node->sceneTransform();
		QTransform t1(t0.m11(),t0.m12(),0,t0.m21(),t0.m22(),0,0,0,1);

		node->setBoundingBoxVisible(false);
		node->clear();
		node->setPos(QPointF());
		node->resetTransform();

		NodeGraphicsReader reader;
		reader.readNodeGraphics(node,&file);				    
		node->normalize();

		node->setPos(p);
		node->setTransform(t1);

		//node->setParentItem(parent);
	}

	AssignHandleCommand::AssignHandleCommand(const QString& text, const QList<QGraphicsItem*>& items, QList<ItemHandle*>& handles)
		: QUndoCommand(text)
	{
		graphicsItems.clear();
		oldHandles.clear();
		newHandles.clear();

		for (int i=0; i < items.size() && i < handles.size(); ++i)		
		{
			graphicsItems += items[i];
			oldHandles += getHandle(items[i]);
			newHandles += handles[i];
		}
	}

	AssignHandleCommand::AssignHandleCommand(const QString& text, const QList<QGraphicsItem*>& items, ItemHandle* handle)
		: QUndoCommand(text)
	{
		graphicsItems.clear();
		oldHandles.clear();
		newHandles.clear();

		for (int i=0; i < items.size(); ++i)		
		{
			graphicsItems += items[i];
			oldHandles += getHandle(items[i]);
			newHandles += handle;
		}

	}

	AssignHandleCommand::AssignHandleCommand(const QString& text, QGraphicsItem* item, ItemHandle* handle)
		: QUndoCommand(text)
	{
		graphicsItems.clear();
		oldHandles.clear();
		newHandles.clear();

		graphicsItems += item;
		oldHandles += getHandle(item);
		newHandles += handle;	

	}

	void AssignHandleCommand::redo()
	{
		TextGraphicsItem* textItem = 0;
		ItemHandle * itemHandle = 0;
		for (int i=0; i < graphicsItems.size() && i < newHandles.size(); ++i)
		{
			if (graphicsItems[i])
			{
				if (newHandles[i] &&
					(textItem = qgraphicsitem_cast<TextGraphicsItem*>(graphicsItems[i])) &&
					(itemHandle = getHandle(graphicsItems[i])) &&
					textItem->toPlainText() == itemHandle->name)
					textItem->setPlainText(newHandles[i]->name);
				setHandle(graphicsItems[i],newHandles[i]);
			}
		}
		for (int i=0; i < newHandles.size(); ++i)
		{	

			if (newHandles[i] && newHandles[i]->parent && !newHandles[i]->parent->children.contains(newHandles[i]))
				newHandles[i]->parent->children.append(newHandles[i]);
		}
		for (int i=0; i < oldHandles.size(); ++i)
		{	

			if (oldHandles[i] && oldHandles[i]->parent)
				oldHandles[i]->parent->children.removeAll(oldHandles[i]);
		}
	}

	void AssignHandleCommand::undo()
	{
		TextGraphicsItem* textItem = 0;
		ItemHandle * itemHandle = 0;
		for (int i=0; i < graphicsItems.size() && i < oldHandles.size(); ++i)
		{
			if (graphicsItems[i])
			{
				if (oldHandles[i] &&
					(textItem = qgraphicsitem_cast<TextGraphicsItem*>(graphicsItems[i])) &&
					(itemHandle = getHandle(graphicsItems[i])) &&
					textItem->toPlainText() == itemHandle->name)
					textItem->setPlainText(oldHandles[i]->name);

				setHandle(graphicsItems[i],oldHandles[i]);
			}
		}
		for (int i=0; i < oldHandles.size(); ++i)
		{	

			if (oldHandles[i] && oldHandles[i]->parent && !oldHandles[i]->parent->children.contains(oldHandles[i]))
				oldHandles[i]->parent->children.append(oldHandles[i]);
		}
		for (int i=0; i < newHandles.size(); ++i)
		{	

			if (newHandles[i] && newHandles[i]->parent)
				newHandles[i]->parent->children.removeAll(newHandles[i]);
		}
	}

	AssignHandleCommand::~AssignHandleCommand()
	{
		for (int i=0; i < oldHandles.size(); ++i)
			if (oldHandles[i])
			{
				bool pointedTo = false;
				for (int j=0; j < oldHandles[i]->graphicsItems.size(); ++j)
					if (getHandle(oldHandles[i]->graphicsItems[j]) == oldHandles[i])
					{
						pointedTo = true;
						break;
					}
					if (!pointedTo)
					{
						for (int j=0; j < oldHandles.size(); ++j)
							if (i != j && oldHandles[j] == oldHandles[i])
								oldHandles[j] = 0;

						oldHandles[i]->graphicsItems.clear();
						//oldHandles[i]->children.clear();
						//delete oldHandles[i];
						oldHandles[i] = 0;
					}
			}
			/*for (int i=0; i < newHandles.size(); ++i)
			if (newHandles[i])
			{
			bool pointedTo = false;
			for (int j=0; j < newHandles[i]->graphicsItems.size(); ++j)
			if (getHandle(newHandles[i]->graphicsItems[j]) == newHandles[i])
			{
			pointedTo = true;
			break;
			}
			if (!pointedTo)
			{
			for (int j=0; j < newHandles.size(); ++j)
			if (i != j && newHandles[j] == newHandles[i])
			newHandles[j] = 0;

			newHandles[i]->graphicsItems.clear();
			newHandles[i]->children.clear();
			delete newHandles[i];
			newHandles[i] = 0;
			}
			}*/

	}

	MergeHandlersCommand::MergeHandlersCommand(const QString& text, const QList<ItemHandle*>& handles) :
	QUndoCommand(text)
	{
		newHandle = 0;
		oldHandles = handles;
		if (handles.size() > 0)
		{
			for (int i=1; i < handles.size(); ++i)
				if (handles[i])
				{
					if (newHandle == 0)
					{
						newHandle = handles[i]->clone();
					}

					for (int j=0; j < handles[i]->tools.size(); ++j)
						if (!newHandle->tools.contains(handles[i]->tools[j]))
							newHandle->tools += handles[i]->tools[j];
				}
		}
	}

	MergeHandlersCommand::~MergeHandlersCommand()
	{
		if (newHandle)
		{
			bool pointedTo = false;
			for (int j=0; j < newHandle->graphicsItems.size(); ++j)
				if (getHandle(newHandle->graphicsItems[j]) == newHandle)
				{
					pointedTo = true;
					break;
				}
				if (!pointedTo)
				{
					int k = -1;
					while ((k = oldHandles.indexOf(newHandle) > -1) )
						oldHandles[k] = 0;
					//newHandle->children.clear();
					//delete newHandle;
					newHandle = 0;
				}
		}

		for (int i=0; i < oldHandles.size(); ++i)
		{
			if (oldHandles[i] && newHandle != oldHandles[i])
			{
				bool pointedTo = false;
				for (int j=0; j < oldHandles[i]->graphicsItems.size(); ++j)
					if (getHandle(oldHandles[i]->graphicsItems[j]) == oldHandles[i])
					{
						pointedTo = true;
						break;
					}
					if (!pointedTo)
					{
						for (int j=0; j < oldHandles.size(); ++j)
							if (i != j && oldHandles[j] == oldHandles[i])
								oldHandles[j] = 0;

						oldHandles[i]->graphicsItems.clear();
						oldHandles[i]->children.clear();
						delete oldHandles[i];
						oldHandles[i] = 0;
					}
			}
		}
	}

	void MergeHandlersCommand::redo()
	{
		if (newHandle == 0)
			if (oldHandles.size() > 0 && oldHandles[0])
				newHandle = oldHandles[0]->clone();
			else
				return;

		if (newHandle && oldHandles.size() > 0)
		{
			//move the children from the old handles to new handle
			for (int i=0; i < oldHandles.size(); ++i)
				if (oldHandles[i] && !oldHandles[i]->children.isEmpty())
					for (int j=0; j < oldHandles[i]->children.size(); ++j)
						if (!newHandle->children.contains(oldHandles[i]->children[j]))
						{
							newHandle->children += oldHandles[i]->children[j];
							oldHandles[i]->children[j]->parent = newHandle;
						}
		}

		if (newHandle && newHandle->parent && !newHandle->parent->children.contains(newHandle))
			newHandle->parent->children.append(newHandle);

		for (int i=0; i < oldHandles.size(); ++i)
		{	

			if (oldHandles[i] && oldHandles[i]->parent)
				oldHandles[i]->parent->children.removeAll(oldHandles[i]);
		}

		//for each graphics item in old handles, change its handle to new
		TextGraphicsItem * textItem = 0;
		//ItemHandle * handle = 0;
		for (int i=0; i < oldHandles.size(); ++i)
			if (oldHandles[i])
			{
				QList<QGraphicsItem*> list = oldHandles[i]->graphicsItems;
				for (int j=0; j < list.size(); ++j)
					if (list[j])
					{
						setHandle(list[j],newHandle);
						if ((textItem = qgraphicsitem_cast<TextGraphicsItem*>(list[j])) && (textItem->toPlainText() == oldHandles[i]->name))
						{
							textItem->setPlainText(newHandle->name);
						}
					}
					oldHandles[i]->graphicsItems = list;	//do not remove -- otherwise cannot undo			
			}

	}

	void MergeHandlersCommand::undo()
	{
		for (int i=0; i < oldHandles.size(); ++i)
			if (oldHandles[i])
			{
				TextGraphicsItem * textItem = 0;
				QList<QGraphicsItem*> list = oldHandles[i]->graphicsItems;
				oldHandles[i]->graphicsItems.clear();
				for (int j=0; j < list.size(); ++j)
				{				
					setHandle(list[j],oldHandles[i]);
					if ((textItem = qgraphicsitem_cast<TextGraphicsItem*>(list[j])) && (textItem->toPlainText() == newHandle->name))
					{
						textItem->setPlainText(oldHandles[i]->name);
					}
				}
				//oldHandles[i]->graphicsItems = list;			
			}
			for (int i=0; i < oldHandles.size(); ++i)
				if (oldHandles[i] && !oldHandles[i]->children.isEmpty())
					for (int j=0; j < oldHandles[i]->children.size(); ++j)					
						oldHandles[i]->children[j]->parent = oldHandles[i];

			newHandle->children.clear();

			QList<QGraphicsItem*> list = newHandle->graphicsItems;
			for (int i=0; i < list.size(); ++i)
				if (list[i])
				{
					setHandle(list[i],0);
				}

				newHandle->graphicsItems.clear();

				if (newHandle && newHandle->parent)
					newHandle->parent->children.removeAll(newHandle);

				for (int i=0; i < oldHandles.size(); ++i)
				{			
					if (oldHandles[i] && oldHandles[i]->parent && !oldHandles[i]->parent->children.contains(oldHandles[i]))
						oldHandles[i]->parent->children.append(oldHandles[i]);
				}
	}

	SetParentHandleCommand::SetParentHandleCommand(const QString& name, NetworkWindow * net, ItemHandle * child, ItemHandle * parent)
		: QUndoCommand(name)
	{
		this->net = net;
		children += child;
		newParents += parent;
		oldParents += child->parent;
	}
	SetParentHandleCommand::SetParentHandleCommand(const QString& name, NetworkWindow * net, const QList<ItemHandle*>& childlist, const QList<ItemHandle*>& parents)
		: QUndoCommand(name)
	{
		this->net = net;
		children += childlist;
		newParents += parents;
		for (int i=0; i < childlist.size(); ++i)
			if (childlist[i])
				oldParents += childlist[i]->parent;
			else
				oldParents += 0;
	}
	SetParentHandleCommand::SetParentHandleCommand(const QString& name, NetworkWindow * net, const QList<ItemHandle*>& childlist, ItemHandle * parent)
		: QUndoCommand(name)
	{
		this->net = net;
		children += childlist;
		for (int i=0; i < childlist.size(); ++i)
		{
			newParents += parent;
			if (childlist[i])
				oldParents += childlist[i]->parent;
			else
				oldParents += 0;
		}
	}
	void SetParentHandleCommand::redo()
	{
		QList<QString> newNames, oldNames;
		for (int i=0; i < children.size() && i < newParents.size() && i < oldParents.size(); ++i)
			if (children[i] && newParents[i] != oldParents[i])
			{
				if (children[i] != newParents[i] && !children[i]->isChildOf(newParents[i]))
				{
					oldNames += children[i]->fullName();
					children[i]->setParent(newParents[i]);
					newNames += children[i]->fullName();
				}
			}

			if (!net) return;

			QList<ItemHandle*> handles = net->allHandles();		

			for (int i=0; i < oldNames.size() && i < newNames.size(); ++i)
			{
				RenameCommand::findReplaceAllHandleData(handles,oldNames[i],newNames[i]);
			}
	}
	void SetParentHandleCommand::undo()
	{	
		QList<QString> newNames, oldNames;
		for (int i=0; i < children.size() && i < newParents.size() && i < oldParents.size(); ++i)
			if (children[i] && newParents[i] != oldParents[i])
			{
				if (children[i] != oldParents[i] && !children[i]->isChildOf(oldParents[i]))
				{
					oldNames += children[i]->fullName();
					children[i]->setParent(oldParents[i]);
					newNames += children[i]->fullName();
				}
			}

			if (!net) return;

			QList<ItemHandle*> handles = net->allHandles();		

			for (int i=0; i < oldNames.size() && i < newNames.size(); ++i)
			{
				RenameCommand::findReplaceAllHandleData(handles,oldNames[i],newNames[i]);
			}
	}
}
