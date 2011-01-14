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
#include "TextEditor.h"
#include "NetworkHandle.h"
#include "UndoCommands.h"
#include "ConsoleWindow.h"
#include <QRegExp>
#include <QStringList>
#include <QDebug>

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
		
		for (int i=0; i < items.size() && i < amounts.size(); ++i)
			if (items[i] && !amounts[i].isNull())
			{
				if ((connection = ConnectionGraphicsItem::cast(items[i])))
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

					if ((node = NodeGraphicsItem::cast(items[i])))
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
			if (items[i] && !amount.isNull())
			{
				if ((connection = ConnectionGraphicsItem::cast(items[i])))
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
					if ((node = NodeGraphicsItem::cast(items[i])))
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
		ConnectionGraphicsItem * connection = 0;
		NodeGraphicsItem * node = 0;
		
		if (item && !amount.isNull())
		{
			if ((connection = ConnectionGraphicsItem::cast(item)))
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
				if (!graphicsItems.contains(item))
				{
					graphicsItems += item;
					change += amount;
				}
				if ((node = NodeGraphicsItem::cast(item)))
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
	
	InsertHandlesCommand::~InsertHandlesCommand()
	{
		for (int i=0; i < items.size(); ++i)
			if (items[i] && !MainWindow::invalidPointers.contains((void*)items[i]))
			{
				MainWindow::invalidPointers[ (void*)items[i] ] = true;
				delete items[i];
				items[i] = 0;
			}
		if (renameCommand)
			delete renameCommand;
	}

	InsertHandlesCommand::InsertHandlesCommand(TextEditor * textEditor, const QList<ItemHandle*> & list, bool rename)
	{
		QStringList s;
		ItemHandle * h = 0;
		for (int i=0; i < list.size(); ++i)
			if (h = list[i])
				s << h->name;
		setText(s.join(QObject::tr(",")) + QObject::tr(" added"));
		this->textEditor = textEditor;
		checkNames = rename;
		network = 0;
		if (textEditor)
			network = textEditor->network;
		items = list;
		renameCommand = 0;
	}

	InsertHandlesCommand::InsertHandlesCommand(TextEditor * textEditor, ItemHandle * h, bool rename)
	{
		checkNames = rename;
		if (h)
			setText(h->name + QObject::tr(" added"));
		else
			setText(QObject::tr("items added"));
		this->textEditor = textEditor;
		network = 0;
		if (textEditor)
			network = textEditor->network;
		items << h;
		renameCommand = 0;
	}

	void InsertHandlesCommand::redo()
	{
		if (MainWindow::invalidPointers.contains(textEditor))
		{
			textEditor = 0;
		}

		if (textEditor && textEditor->network == network)
		{
			network->showTextEditor(textEditor);
			QStringList oldNames, newNames, usedNames;
			QList<ItemHandle*> nameChangeHandles = network->handles();
			QString s0,s1;
			bool isNum;
			
			QList<ItemHandle*>& list = textEditor->items();
			
			while (parentHandles.size() < items.size()) parentHandles += 0;
			
			for (int i=0; i < items.size(); ++i)
			{
				if (items[i] && !list.contains(items[i]))
				{
					if (parentHandles.size() > i && !MainWindow::invalidPointers.contains(parentHandles[i]))
						items[i]->setParent(parentHandles[i],false);					

					items[i]->network = textEditor->network;
					list << items[i];
					if (!renameCommand && !nameChangeHandles.contains(items[i]))
					{
						s0 = items[i]->fullName();
						s1 = textEditor->network->makeUnique(items[i],usedNames);
						usedNames << s1;
						nameChangeHandles << items[i];
						
						if (s0 != s1)
						{
							oldNames << s0;
							newNames << s1;
						}
					}
				}
			}
			
			if (!renameCommand && !newNames.isEmpty())
			{
				QList<ItemHandle*> allHandles;
				for (int i=0; i < items.size(); ++i)
					if (items[i])
						allHandles << items[i] << items[i]->allChildren();
					
				renameCommand = new RenameCommand(QString("rename"),textEditor->network,allHandles,oldNames,newNames);
			}
			
			if (renameCommand && checkNames)
				renameCommand->redo();
		}
	}

	void InsertHandlesCommand::undo()
	{
		if (MainWindow::invalidPointers.contains(textEditor) && network)
		{
			textEditor = 0;
		}
		
		if (textEditor)
		{
			//network->showTextEditor(textEditor);

			QList<ItemHandle*>& list = textEditor->items();
			for (int i=0; i < items.size(); ++i)
				if (items[i] && list.contains(items[i]))
				{
					items[i]->network = 0;
					list.removeAll(items[i]);
				}
			
			while (parentHandles.size() < items.size()) parentHandles += 0;
			
			for (int i=0; i < items.size(); ++i)
				if (items[i])
				{
					parentHandles[i] = items[i]->parent;
					items[i]->setParent(0,false);
				}
			
			if (renameCommand && checkNames)
				renameCommand->undo();
		}
	}

	RemoveHandlesCommand::RemoveHandlesCommand(TextEditor * editor, const QList<ItemHandle*> & list, bool update) : changeDataCommand(0)
	{
		QStringList s;
		ItemHandle * h = 0;
		updateData = update;
		for (int i=0; i < list.size(); ++i)
			if (h = list[i])
				s << h->name;
		setText(s.join(QObject::tr(",")) + QObject::tr(" removed"));
		textEditor = editor;
		network = 0;
		if (editor)
			network = editor->network;
		items = list;
	}

	RemoveHandlesCommand::RemoveHandlesCommand(TextEditor * editor, ItemHandle * h, bool update): changeDataCommand(0)
	{
		if (h)
			setText(h->name + QObject::tr(" removed"));
		else
			setText(QObject::tr("items removed"));
		textEditor = editor;
		network = 0;
		updateData = update;
		if (editor)
			network = editor->network;
		items << h;
	}

	void RemoveHandlesCommand::undo()
	{
		if (MainWindow::invalidPointers.contains(textEditor) && network)
		{
			textEditor = 0;
		}
		
		if (textEditor && textEditor->network == network)
		{
			network->showTextEditor(textEditor);

			QList<ItemHandle*>& list = textEditor->items();
			for (int i=0; i < items.size(); ++i)
				if (items[i] && !list.contains(items[i]))
				{
					if (parentHandles.size() > i && !MainWindow::invalidPointers.contains(parentHandles[i]))
					{
						items[i]->setParent(parentHandles[i],false);
					}
					items[i]->network = textEditor->network;
					list << items[i];
				}
			
			if (changeDataCommand)
				changeDataCommand->redo();
			
		}
	}

	void RemoveHandlesCommand::redo()
	{
		if (MainWindow::invalidPointers.contains(textEditor) && network)
		{
			textEditor = 0;
		}
		
		if (textEditor)
		{
			//network->showTextEditor(textEditor);				
			QList<ItemHandle*>& list = textEditor->items();
			
			while (parentHandles.size() < items.size()) parentHandles += 0;
			
			for (int i=0; i < items.size(); ++i)
				if (items[i] && list.contains(items[i]))
				{
					parentHandles[i] = items[i]->parent;
					items[i]->setParent(0,false);

					items[i]->network = 0;
					list.removeAll(items[i]);
				}
			
			bool firstTime = (changeDataCommand == 0);

			if (firstTime)
			{
				QList< DataTable<qreal>* > oldData1, newData1;
				QList< DataTable<QString>* > oldData2, newData2;

				bool emptyHandle;
				QStringList namesToKill;
				for (int i=0; i < items.size(); ++i)
					if (items[i])
					{
						emptyHandle = true;				
						for (int j=0; j < items[i]->graphicsItems.size(); ++j)
							if (items[i]->graphicsItems[j] && 
								items[i]->graphicsItems[j]->scene() &&
								static_cast<GraphicsScene*>(items[i]->graphicsItems[j]->scene())->networkWindow->isVisible())
							{
								emptyHandle = false;
								break;
							}
						if (emptyHandle)
							namesToKill << items[i]->fullName();
					}

				QList<ItemHandle*> affectedHandles = textEditor->items();

				for (int i=0; i < affectedHandles.size(); ++i)
				{
					QList<QString> keys1 = affectedHandles[i]->numericalDataNames();
					QList<QString> keys2 = affectedHandles[i]->textDataNames();

					for (int j=0; j < keys1.size(); ++j)
						oldData1 += new DataTable<qreal>(affectedHandles[i]->numericalDataTable( keys1[j] ));

					for (int j=0; j < keys2.size(); ++j)
						oldData2 += new DataTable<QString>(affectedHandles[i]->textDataTable( keys2[j] ));
				}

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

						QList< QString > keys = affectedHandles[i]->numericalDataNames();
						for (int j=0; j < keys.size(); ++j)  //go through each numeric data
						{
							affected = false;
							nDat = &(affectedHandles[i]->numericalDataTable( keys[j] ));
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
							for (int k=0; k < nDat->columns(); ++k)
							{
								if (nDat->columnName(k).contains(regexp1) || nDat->columnName(k).contains(regexp2) ||
									nDat->columnName(k).contains(regexp3) || nDat->columnName(k).contains(regexp4))
								{
									nDat->removeColumn(k);
									--k;
									affected = true;
								}
							}
						}

						keys = affectedHandles[i]->textDataNames();

						for (int j=0; j < keys.size(); ++j)  //go through each text data
						{
							affected = false;
							sDat = &(affectedHandles[i]->textDataTable( keys[j] ));
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
							for (int k=0; k < sDat->columns(); ++k)
							{
								if (sDat->columnName(k).contains(regexp1) || sDat->columnName(k).contains(regexp2) ||
									sDat->columnName(k).contains(regexp3) || sDat->columnName(k).contains(regexp4))
								{
									sDat->removeColumn(k);
									--k;
									affected = true;
								}
							}

							QString newname("1.0");
							for (int k=0; k < sDat->rows(); ++k) //substitute each value in the table
								for (int l=0; l < sDat->columns(); ++l)
								{
									QString & target = sDat->value(k,l);// = QString("0.0");

									int n = regexp1.indexIn(target);
									if (n != -1)
									{
										target = newname;
										//target.replace(oldname,newname);
										//n = regexp1.indexIn(target);
										affected = true;
									}
									n = regexp2.indexIn(target);
									if (n != -1)
									{
										target = newname;
										//target.replace(regexp2,newname+QString("\\1"));
										//n = regexp2.indexIn(target);
										affected = true;
									}
									n = regexp3.indexIn(target);
									if (n != -1)
									{
										target = newname;
										//target.replace(regexp3,QString("\\1")+newname);
										//n = regexp3.indexIn(target);
										affected = true;
									}
									n = regexp4.indexIn(target);
									if (n != -1)
									{
										target = newname;
										//n = regexp4.indexIn(target);
										affected = true;
									}
								}
						}
					}
				}
				for (int i=0; i < affectedHandles.size(); ++i)
				{
					QList<QString> keys1 = affectedHandles[i]->numericalDataNames();
					QList<QString> keys2 = affectedHandles[i]->textDataNames();

					for (int j=0; j < keys1.size(); ++j)
						newData1 += &(affectedHandles[i]->numericalDataTable( keys1[j] ));

					for (int j=0; j < keys2.size(); ++j)
						newData2 += &(affectedHandles[i]->textDataTable( keys2[j] ));
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
			
		}
	}

	InsertGraphicsCommand::InsertGraphicsCommand(const QString& name, GraphicsScene * scene, QGraphicsItem * item, bool checkNames)
		: QUndoCommand(name), checkNames(checkNames)
	{
		graphicsScene = scene;
		network = 0;
		if (scene)
			network = scene->network;
		graphicsItems.clear();
		
		item = getGraphicsItem(item);
		if (!NodeGraphicsItem::cast(item) &&
			!TextGraphicsItem::cast(item) &&
			!ConnectionGraphicsItem::cast(item))
			item = 0;

		ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(item);
		if (connection)
		{
			QList<NodeGraphicsItem*> nodes = connection->nodes();
			for (int i=0; i < nodes.size(); ++i)
				if (nodes[i] && !(nodes[i]->scene() == scene))
				{
					graphicsItems += nodes[i];
					parentGraphicsItems += NodeGraphicsItem::cast(nodes[i]->parentItem());
					handles += getHandle(nodes[i]);
				}
			
			QList<ArrowHeadItem*> arrows = connection->arrowHeads();
			for (int i=0; i < arrows.size(); ++i)
				if (arrows[i] && !(arrows[i]->scene() == scene))
				{
					graphicsItems += arrows[i];
					parentGraphicsItems += NodeGraphicsItem::cast(arrows[i]->parentItem());
					handles += getHandle(arrows[i]);
				}
		}
		
		if (item)
		{
			graphicsItems += item;
			handles.clear();
			handles += getHandle(item);
		}
		renameCommand = 0;
	}

	InsertGraphicsCommand::InsertGraphicsCommand(const QString& name, GraphicsScene * scene, const QList<QGraphicsItem*>& items, bool checkNames)
		: QUndoCommand(name), checkNames(checkNames)
	{
		graphicsScene = scene;
		network = 0;
		if (scene)
			network = scene->network;
		handles.clear();
		QGraphicsItem * item;
		ConnectionGraphicsItem * connection;
		for (int i=0; i < items.size(); ++i)
			if ((item = getGraphicsItem(items[i])) && !graphicsItems.contains(item) &&
					(NodeGraphicsItem::cast(items[i]) || TextGraphicsItem::cast(items[i]) || ConnectionGraphicsItem::cast(items[i])))
			{
				connection = ConnectionGraphicsItem::cast(item);
				if (connection)
				{
					QList<NodeGraphicsItem*> nodes = connection->nodes();
					for (int j=0; j < nodes.size(); ++j)
						if (nodes[j] && !(nodes[j]->scene() == scene || items.contains(nodes[j])))
						{
							graphicsItems += nodes[j];
							parentGraphicsItems += NodeGraphicsItem::cast(nodes[j]->parentItem());
							handles += getHandle(nodes[j]);
						}
						
					QList<ArrowHeadItem*> arrows = connection->arrowHeads();
					for (int j=0; j < arrows.size(); ++j)
						if (arrows[j] && !(items.contains(arrows[j]) || arrows[j]->scene() == scene))
						{
							graphicsItems += arrows[j];
							parentGraphicsItems += NodeGraphicsItem::cast(arrows[j]->parentItem());	
							handles += getHandle(arrows[j]);
						}
				}
				graphicsItems += item;
				parentGraphicsItems += NodeGraphicsItem::cast(item->parentItem());	
				handles += getHandle(item);
			}
		
		renameCommand = 0;
	}

	void InsertGraphicsCommand::redo()
	{
		QList<ConnectionGraphicsItem*> connections;
		ConnectionGraphicsItem * connection;
		bool isNum;
		
		if (MainWindow::invalidPointers.contains(graphicsScene))
		{
			graphicsScene = 0;
		}
		
		if (graphicsScene && graphicsScene->network == network)
		{
			network->showScene(graphicsScene);
			QStringList newNames, oldNames, usedNames;
			QList<ItemHandle*> nameChangeHandles = network->handles();
			QString s0,s1;
			
			for (int i=0; i<graphicsItems.size(); ++i)
			{
				if (graphicsItems[i] && graphicsItems[i]->scene() != graphicsScene)
				{
					if (graphicsItems[i]->scene())
						graphicsItems[i]->scene()->removeItem(graphicsItems[i]);
					graphicsScene->addItem(graphicsItems[i]);
					if ((connection = ConnectionGraphicsItem::cast(graphicsItems[i])))
					{
						connections << connection;
					}

					if (parentGraphicsItems.size() > i &&
						parentGraphicsItems[i] &&
						NodeGraphicsItem::cast(parentGraphicsItems[i]) &&
						parentGraphicsItems[i]->scene() == graphicsScene)
						graphicsItems[i]->setParentItem(parentGraphicsItems[i]);
						
					if (handles.size() > i)
					{
						setHandle(graphicsItems[i],handles[i]);
						if (handles[i])
						{
							if (parentHandles.size() > i && !MainWindow::invalidPointers.contains(parentHandles[i]))
								handles[i]->setParent(parentHandles[i],false);
							
							handles[i]->network = network;
							
							if (!renameCommand && !nameChangeHandles.contains(handles[i]))
							{
								s0 = handles[i]->fullName();
								s1 = graphicsScene->network->makeUnique(handles[i],usedNames);
								usedNames << s1;
								nameChangeHandles << handles[i];
								
								if (s0 != s1)
								{
									oldNames << s0;
									newNames << s1;
								}
							}
						}
					}
				}
			}
			
			if (!renameCommand && !newNames.isEmpty())
			{
				QList<ItemHandle*> allHandles;
				for (int i=0; i < handles.size(); ++i)
					if (handles[i])
						allHandles << handles[i] << handles[i]->allChildren();
					
				renameCommand = new RenameCommand(QString("rename"),graphicsScene->network,allHandles,oldNames,newNames,false);
			}
			
			if (renameCommand && checkNames)
				renameCommand->redo();
		}

		for (int i=0; i < connections.size(); ++i)
		{
			connection = connections[i];
			QList<QGraphicsItem*> arrows = connection->arrowHeadsAsGraphicsItems();
			for (int j=0; j < arrows.size(); ++j)
				if (arrows[j] && arrows[j]->scene() != graphicsScene)
				{
					if (arrows[j]->scene())
						arrows[j]->scene()->removeItem(arrows[j]);
					graphicsScene->addItem(arrows[j]);
				}
			connection->refresh();
			connection->setControlPointsVisible(false);
		}
	}

	void InsertGraphicsCommand::undo()
	{
		if (MainWindow::invalidPointers.contains(graphicsScene) && network)
		{
			graphicsScene = 0;
		}
		
		ConnectionGraphicsItem * connection = 0;
		if (graphicsScene && graphicsScene->network == network)
		{
			//network->showScene(graphicsScene);
			bool emptyHandle;
			for (int i=0; i<graphicsItems.size(); ++i)
			{
				if (graphicsItems[i] && graphicsItems[i]->scene() == graphicsScene)
				{
					while (parentGraphicsItems.size() <= i) parentGraphicsItems << 0;
					while (handles.size() <= i) handles << 0;
					
					parentGraphicsItems[i] = NodeGraphicsItem::cast(graphicsItems[i]->parentItem());

					if (handles[i] != getHandle(graphicsItems[i]))
					{
						if (handles[i])
							handles << handles[i];
						handles[i] = getHandle(graphicsItems[i]);
					}

					graphicsItems[i]->setParentItem(0);
					graphicsScene->removeItem(graphicsItems[i]);

					if ((connection = ConnectionGraphicsItem::cast(graphicsItems[i])))
					{
						QList<QGraphicsItem*> arrows = connection->arrowHeadsAsGraphicsItems();
						for (int j=0; j < arrows.size(); ++j)
							if (arrows[j] && arrows[j]->scene())
								arrows[j]->scene()->removeItem(arrows[j]);
					}
					
					if (handles[i] && !handles[i]->parent)
					{
						setHandle(graphicsItems[i],0);
						emptyHandle = true;				
						for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
							if (handles[i]->graphicsItems[j] && 
								handles[i]->graphicsItems[j]->scene() &&
								static_cast<GraphicsScene*>(handles[i]->graphicsItems[j]->scene())->networkWindow->isVisible())
							{
								emptyHandle = false;
								break;
							}
						if (emptyHandle)
							handles[i]->network = 0;
					}
				}
			}
			
			while (parentHandles.size() < handles.size()) parentHandles += 0;
			
			for (int i=0; i < handles.size(); ++i)
				if (handles[i])
				{
					parentHandles[i] = handles[i]->parent;
					emptyHandle = true;				
						for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
							if (handles[i]->graphicsItems[j] && 
								handles[i]->graphicsItems[j]->scene() &&
								static_cast<GraphicsScene*>(handles[i]->graphicsItems[j]->scene())->networkWindow->isVisible())
							{
								emptyHandle = false;
								break;
							}
					if (emptyHandle)
						handles[i]->setParent(0,false);
				}
			
			if (renameCommand && checkNames)
				renameCommand->undo();
		}
	}
	
	InsertGraphicsCommand::~InsertGraphicsCommand()
	{
		ItemHandle * handle = 0;
		
		QList<QGraphicsItem*> list;
		for (int i=0; i < graphicsItems.size(); ++i)
			if (!MainWindow::invalidPointers.contains((void*)graphicsItems[i]) &&
				(	NodeGraphicsItem::cast(graphicsItems[i]) || 
					TextGraphicsItem::cast(graphicsItems[i]) || 
					ConnectionGraphicsItem::cast(graphicsItems[i])))
				list << graphicsItems[i];

		graphicsItems.clear();

		for (int i=0; i < list.size(); ++i)
		{
			if (list[i] && !MainWindow::invalidPointers.contains((void*)list[i]))
			{
				if ((handle = getHandle(list[i])) &&
					!handles.contains(handle))
				{
					handles += handle;
					list[i] = 0;
				}
			}
		}
		
		for (int i=0; i < handles.size(); ++i)
		{
			if (!MainWindow::invalidPointers.contains( (void*)handles[i]) && 
				handles[i] &&
				!handles[i]->parent && 
				handles[i]->graphicsItems.isEmpty())
			{
			    MainWindow::invalidPointers[ (void*) handles[i] ] = true;
			    delete handles[i];
			}
		}
		handles.clear();

        ConnectionGraphicsItem * connection;
        NodeGraphicsItem * node;

		for (int i=0; i < list.size(); ++i)
		{
			if (list[i] && !MainWindow::invalidPointers.contains((void*)list[i]))
			{
			    if (list[i]->parentItem())
					list[i]->setParentItem(0);

				if (list[i]->scene())
					list[i]->scene()->removeItem(list[i]);
				
				MainWindow::invalidPointers[ (void*) list[i] ] = true;
				
				QList<QGraphicsItem *> childItems = list[i]->childItems();
				for (int j=0; j < childItems.size(); ++j)
					if (childItems[j])
						childItems[j]->setParentItem(0);
				delete list[i];
			}
		}
		
		if (renameCommand)
			delete renameCommand;
	}

	RemoveGraphicsCommand::RemoveGraphicsCommand(const QString& name, QGraphicsItem * item, bool update)
		: QUndoCommand(name), changeDataCommand(0), updateData(update)
	{
		item = getGraphicsItem(item);
		graphicsItems.append( item );
		
		if (item)
			itemParents.append(item->parentItem());
		else
			itemParents.append(0);
	}

	RemoveGraphicsCommand::RemoveGraphicsCommand(const QString& name, const QList<QGraphicsItem*>& items, bool update)
		: QUndoCommand(name), changeDataCommand(0), updateData(update)
	{
		QGraphicsItem * item;
		NodeGraphicsItem * node;
		
		for (int i=0; i < items.size(); ++i)
			if ( (item = getGraphicsItem(items[i]) ) )
			{
				graphicsItems.append(item);
				itemParents.append(item->parentItem());
			}
	}

	void RemoveGraphicsCommand::redo()
	{
		graphicsScenes.clear();
		itemHandles.clear();
		NodeGraphicsItem * node;
		GraphicsScene * scene;
		ItemHandle * h;
		
		ConnectionGraphicsItem * connection;
		for (int i=0; i < graphicsItems.size(); ++i)
			if (connection = ConnectionGraphicsItem::cast(graphicsItems[i]))
			{
				for (int j=0; j < connection->curveSegments.size(); ++j)
				{
					if (connection->curveSegments[j].arrowStart && !graphicsItems.contains(connection->curveSegments[j].arrowStart))
						graphicsItems += (connection->curveSegments[j].arrowStart);
					if (connection->curveSegments[j].arrowEnd && !graphicsItems.contains(connection->curveSegments[j].arrowStart))
						graphicsItems += (connection->curveSegments[j].arrowEnd);
				}
				if (connection->centerRegionItem && !graphicsItems.contains(connection->centerRegionItem))
					graphicsItems += connection->centerRegionItem;
			}
		
		for (int i=0; i<graphicsItems.size(); ++i)
		{
			scene = 0;
			if (graphicsItems[i])		
				scene = static_cast<GraphicsScene*>(graphicsItems[i]->scene());

			graphicsScenes += scene;
			
			h = getHandle(graphicsItems[i]);
			
			/*if (scene && scene->network && h)
			{
				QList<GraphicsScene*> otherScenes = scene->network->scenes();
				for (int j=0; j < otherScenes.size(); ++j)
				{
					if (otherScenes[j] != scene && otherScenes[j]->localHandle() == h)
					{
						QList<QGraphicsItem*> items = otherScenes[j]->items();
						QList<QGraphicsItem*> gitems;
						QGraphicsItem * item;
						for (int k=0; k < items.size(); ++k)
						{
							item = getGraphicsItem(items[k]);
							if (!gitems.contains(item))
								gitems << item;
						}
						graphicsItems << gitems;
					}
				}
			}*/

			itemHandles += h;
			node = NodeGraphicsItem::cast(graphicsItems[i]);
			if (node)
			{
				QList<ConnectionGraphicsItem*> connections = node->connections();
				for (int j=0; j < connections.size(); ++j)
					if (connections[j] && 
						connections[j]->scene() == node->scene() && 
						!graphicsItems.contains(connections[j]))
					{
						graphicsItems += connections[j];
						itemParents.append(connections[j]->parentItem());
					}
			}
		}

		bool emptyHandle;

		for (int i=0; i<graphicsItems.size(); ++i)
		{
			if (graphicsItems[i] && graphicsScenes[i])
			{
				if (graphicsItems[i]->scene() == graphicsScenes[i])
					graphicsScenes[i]->removeItem(graphicsItems[i]);

				NodeGraphicsItem * node = NodeGraphicsItem::cast(graphicsItems[i]);
				if (node)
				{
					node->setBoundingBoxVisible(false);
				}
				else
				{
					connection = ConnectionGraphicsItem::cast(graphicsItems[i]);
					if (connection)
					{
						connection->setControlPointsVisible(false);
						/*QList<ConnectionGraphicsItem::ControlPoint*> cps = connection->controlPoints(true);
						for (int j=0; j < cps.size(); ++j)
							if (cps[j])
								cps[j]->connectionItem = 0;*/
					}
				}
			}

			setHandle(graphicsItems[i],0);

			if (itemHandles.size() > i && itemHandles[i])
			{
				emptyHandle = true;				
				for (int j=0; j < itemHandles[i]->graphicsItems.size(); ++j)
					if (itemHandles[i]->graphicsItems[j] && 
						itemHandles[i]->graphicsItems[j]->scene() &&
						static_cast<GraphicsScene*>(itemHandles[i]->graphicsItems[j]->scene())->networkWindow->isVisible())
					{
						emptyHandle = false;
						break;
					}
				
				if (emptyHandle)
				{
					itemHandles[i]->network = 0;
					for (int j=0; j < itemHandles[i]->children.size(); ++j)
						if (itemHandles[i]->children[j])
							itemHandles[i]->children[j]->parent = 0;
				}
			}

		}

		bool firstTime = (changeDataCommand == 0);

		if (firstTime)
		{
			QList< DataTable<qreal>* > oldData1, newData1;
			QList< DataTable<QString>* > oldData2, newData2;
			QList<ItemHandle*> visited;

			bool exists = false;
			QStringList namesToKill;
			for (int i=0; i < itemHandles.size(); ++i)
				if (itemHandles[i] && !visited.contains(itemHandles[i]))
				{
					visited << itemHandles[i];
					exists = false;
					for (int j=0; j < itemHandles[i]->graphicsItems.size(); ++j)
						if (itemHandles[i]->graphicsItems[j]->scene())
						{
							h = (static_cast<GraphicsScene*>(itemHandles[i]->graphicsItems[j]->scene()))->localHandle();
							if (!h || !itemHandles.contains(h))
							{
								exists = true;
								break;
							}
						}
					if (!exists)
						namesToKill << itemHandles[i]->fullName();
				}

			QList<NetworkHandle*> networkHandles;
			for (int i=0; i < graphicsScenes.size(); ++i)
				if (graphicsScenes[i] && graphicsScenes[i]->network && !networkHandles.contains(graphicsScenes[i]->network))
				{
					networkHandles += graphicsScenes[i]->network;
					affectedHandles += graphicsScenes[i]->network->handles();
				}

			for (int i=0; i < affectedHandles.size(); ++i)		
			{
				QList<QString> keys1 = affectedHandles[i]->numericalDataNames();
				QList<QString> keys2 = affectedHandles[i]->textDataNames();

				for (int j=0; j < keys1.size(); ++j)
					oldData1 += new DataTable<qreal>(affectedHandles[i]->numericalDataTable( keys1[j] ));

				for (int j=0; j < keys2.size(); ++j)
					oldData2 += new DataTable<QString>(affectedHandles[i]->textDataTable( keys2[j] ));
			}

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

					QList< QString > keys = affectedHandles[i]->numericalDataNames();
					for (int j=0; j < keys.size(); ++j)  //go through each numeric data
					{
						affected = false;
						nDat = &(affectedHandles[i]->numericalDataTable( keys[j] ));
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
						for (int k=0; k < nDat->columns(); ++k)
						{
							if (nDat->columnName(k).contains(regexp1) || nDat->columnName(k).contains(regexp2) ||
								nDat->columnName(k).contains(regexp3) || nDat->columnName(k).contains(regexp4))
							{
								nDat->removeColumn(k);
								--k;
								affected = true;
							}
						}
					}

					keys = affectedHandles[i]->textDataNames();

					for (int j=0; j < keys.size(); ++j)  //go through each text data
					{
						affected = false;
						sDat = &(affectedHandles[i]->textDataTable( keys[j] ));
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
						for (int k=0; k < sDat->columns(); ++k)
						{
							if (sDat->columnName(k).contains(regexp1) || sDat->columnName(k).contains(regexp2) ||
								sDat->columnName(k).contains(regexp3) || sDat->columnName(k).contains(regexp4))
							{
								sDat->removeColumn(k);
								--k;
								affected = true;
							}
						}

						QString newname("1.0");
						for (int k=0; k < sDat->rows(); ++k) //substitute each value in the table
							for (int l=0; l < sDat->columns(); ++l)
							{
								QString & target = sDat->value(k,l);// = QString("0.0");

								int n = regexp1.indexIn(target);
								if (n != -1)
								{
									target = newname;
									//target.replace(oldname,newname);
									//n = regexp1.indexIn(target);
									affected = true;
								}
								n = regexp2.indexIn(target);
								if (n != -1)
								{
									target = newname;
									//target.replace(regexp2,newname+QString("\\1"));
									//n = regexp2.indexIn(target);
									affected = true;
								}
								n = regexp3.indexIn(target);
								if (n != -1)
								{
									target = newname;
									//target.replace(regexp3,QString("\\1")+newname);
									//n = regexp3.indexIn(target);
									affected = true;
								}
								n = regexp4.indexIn(target);
								if (n != -1)
								{
									target = newname;
									//n = regexp4.indexIn(target);
									affected = true;
								}
							}
					}
				}
			}

			for (int i=0; i < affectedHandles.size(); ++i)
			{
				QList<QString> keys1 = affectedHandles[i]->numericalDataNames();
				QList<QString> keys2 = affectedHandles[i]->textDataNames();

				for (int j=0; j < keys1.size(); ++j)
					newData1 += &(affectedHandles[i]->numericalDataTable( keys1[j] ));

				for (int j=0; j < keys2.size(); ++j)
					newData2 += &(affectedHandles[i]->textDataTable( keys2[j] ));
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

		while (parentHandles.size() < itemHandles.size()) parentHandles += 0;
		
		for (int i=0; i < itemHandles.size(); ++i)
			if (itemHandles[i] && (itemHandles.indexOf(itemHandles[i]) == i))
			{
				parentHandles[i] = itemHandles[i]->parent;
				emptyHandle = true;				
				for (int j=0; j < itemHandles[i]->graphicsItems.size(); ++j)
					if (itemHandles[i]->graphicsItems[j] && 
						itemHandles[i]->graphicsItems[j]->scene() &&
						static_cast<GraphicsScene*>(itemHandles[i]->graphicsItems[j]->scene())->networkWindow->isVisible())
					{
						emptyHandle = false;
						break;
					}
				if (emptyHandle)
					itemHandles[i]->setParent(0,false);
			}
	}

	void RemoveGraphicsCommand::undo()
	{
		QList<ConnectionGraphicsItem*> connections;
		ConnectionGraphicsItem * connection;

		for (int i=0; i<graphicsItems.size() && i<graphicsScenes.size(); ++i)
			if (graphicsItems[i] && graphicsScenes[i])
			{
				if (graphicsItems[i]->scene() != graphicsScenes[i])
					graphicsScenes[i]->addItem(graphicsItems[i]);

				NodeGraphicsItem * node = NodeGraphicsItem::cast(graphicsItems[i]);
				if (node)
				{
					node->setBoundingBoxVisible(false);
				}
				else
				{
					connection = ConnectionGraphicsItem::cast(graphicsItems[i]);
					if (connection)
					{
						connections << connection;
					}
				}

				if (itemHandles.size() > i && itemHandles[i])
				{
					if (parentHandles.size() > i && 
						(itemHandles.indexOf(itemHandles[i]) == i) &&
						!MainWindow::invalidPointers.contains(itemHandles[i]) && 
						!itemHandles[i]->parent)
						itemHandles[i]->setParent(parentHandles[i],false);
				
					itemHandles[i]->network = graphicsScenes[i]->network;
					setHandle(graphicsItems[i],itemHandles[i]);

					if ((itemHandles.indexOf(itemHandles[i]) == i))
						for (int j=0; j < itemHandles[i]->children.size(); ++j)
							if (itemHandles[i]->children[j])
								itemHandles[i]->children[j]->parent = itemHandles[i];

				}
				if (itemParents.size() > i && itemParents[i] != 0)
				{
					graphicsItems[i]->setParentItem(itemParents[i]);
				}
			}

		for (int i=0; i < connections.size(); ++i)
		{
			connections[i]->refresh();
			connections[i]->setControlPointsVisible(false);
		}

		if (changeDataCommand)
			changeDataCommand->redo();
	}


	ChangeBrushCommand::ChangeBrushCommand(const QString& name, QGraphicsItem * item, const QBrush& to)
		: QUndoCommand(name)
	{
		graphicsItems.clear();
		oldBrush.clear();
		newBrush.clear();

		QAbstractGraphicsShapeItem * aitem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(item);
		NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(item);
		ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(item);
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
			ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(items[i]);
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
			ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(graphicsItems[i]);
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
			ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(graphicsItems[i]);
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
		ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(item);
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
			ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(items[i]);
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
			ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(graphicsItems[i]);
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
			ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(graphicsItems[i]);
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
	
	ChangeBrushAndPenCommand::~ChangeBrushAndPenCommand()
	{
		if (changeBrushCommand)
			delete changeBrushCommand;

		if (changePenCommand)
			delete changePenCommand;
	}

	ChangeBrushAndPenCommand::ChangeBrushAndPenCommand(const QString& name, QGraphicsItem * item, const QBrush& brush, const QPen& pen)
		: QUndoCommand(name)
	{
		changeBrushCommand = new ChangeBrushCommand(name,item,brush);
		changePenCommand = new ChangePenCommand(name,item,pen);
	}

	ChangeBrushAndPenCommand::ChangeBrushAndPenCommand(const QString& name, const QList<QGraphicsItem*>& items, const QList<QBrush>& brushes, const QList<QPen>& pens)
		: QUndoCommand(name)
	{
		changeBrushCommand = new ChangeBrushCommand(name,items,brushes);
		changePenCommand = new ChangePenCommand(name,items,pens);
	}

	void ChangeBrushAndPenCommand::redo()
	{
		if (changeBrushCommand)
			changeBrushCommand->redo();
			
		if (changePenCommand)
			changePenCommand->redo();
	}

	void ChangeBrushAndPenCommand::undo()
	{
		if (changeBrushCommand)
			changeBrushCommand->undo();
			
		if (changePenCommand)
			changePenCommand->undo();
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
		vFlip.append(VFlip);
		hFlip.append(HFlip);
	}

	TransformCommand::TransformCommand(const QString& name, QGraphicsScene * scene, const QList<QGraphicsItem *>& items,
		const QList<QPointF>& sizeChange, const QList<qreal>& changeInAngle,
		const QList<bool>& VFlip, const QList<bool>& HFlip)
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
				QTransform t = graphicsItems[i]->transform();
				
				if (sizeFactor.size() > i && !sizeFactor[i].isNull())
				{
					QTransform scale(sizeFactor[i].x(), 0, 0, sizeFactor[i].y(), 0, 0);
					t = (t * scale);
				}

				if (hFlip.size() > i && hFlip[i])
				{
					QTransform scale(-1.0, 0, 0, 1.0, 0, 0);
					t = (t * scale);
				}

				if (vFlip.size() > i && vFlip[i])
				{
					QTransform scale(1.0, 0, 0, -1.0, 0, 0);
					t = (t * scale);
				}
				
				if (angleChange.size() > i && angleChange[i] != 0.0)
				{
					double sinx = sin(angleChange[i] * 3.14159/180.0),
						   cosx = cos(angleChange[i] * 3.14159/180.0);
					QTransform rotate(cosx, sinx, -sinx, cosx, 0, 0);
					t = (t * rotate);
				}
				
				graphicsItems[i]->setTransform(t);

				NodeGraphicsItem * node = NodeGraphicsItem::cast(graphicsItems[i]);
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
				QTransform t = graphicsItems[i]->transform();
				
				if (sizeFactor.size() > i && !sizeFactor[i].isNull())
				{
					QTransform scale(1.0/sizeFactor[i].x(), 0, 0, 1.0/sizeFactor[i].y(), 0, 0);
					t = (t * scale);
				}
				
				if (hFlip.size() > i && hFlip[i])
				{
					QTransform scale(-1.0, 0, 0, 1.0, 0, 0);
					t = (t * scale);
				}

				if (vFlip.size() > i && vFlip[i])
				{
					QTransform scale(1.0, 0, 0, -1.0, 0, 0);
					t = (t * scale);
				}

				if (angleChange.size() > i && angleChange[i] != 0.0)
				{
					double sinx = sin(-angleChange[i] * 3.14159/180.0),
						   cosx = cos(-angleChange[i] * 3.14159/180.0);
					QTransform rotate(cosx, sinx, -sinx, cosx, 0, 0);
					t = (t * rotate);
				}
				
				graphicsItems[i]->setTransform(t);

				NodeGraphicsItem * node = NodeGraphicsItem::cast(graphicsItems[i]);
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

	RenameCommand::RenameCommand(const QString& name, NetworkHandle * net, ItemHandle * handle, const QString& newname, bool forceUnique)
		: QUndoCommand(name), changeDataCommand(0), network(net), makeUnique(forceUnique)
	{
		if (net)
			allhandles = net->handles();

		handles.clear();
		oldNames.clear();
		newNames.clear();
		QString s;

		if (handle)
		{
			handles += handle;
			oldNames += handle->fullName();
			
			if (net && makeUnique)
			{
				s = handle->name;
				handle->name = newname;
				if (handle->parent)
					handle->name.remove(handle->parent->fullName() + QObject::tr("."));
				newNames += net->makeUnique(handle);
				handle->name = s;
			}
			else
				newNames += newname;
		}
	}

	RenameCommand::RenameCommand(const QString& name, NetworkHandle * net, const QList<ItemHandle*>& allItems, const QString& oldname, const QString& newname, bool forceUnique)
		: QUndoCommand(name), changeDataCommand(0), network(net), makeUnique(forceUnique)
	{
		allhandles = allItems;
		
		for (int i=0; i < allhandles.size(); ++i)
			if (allhandles[i])
			{
				QList<ItemHandle*> & children = allhandles[i]->children;
				for (int j=0; j < children.size(); ++j)
					if (children[j] && !allhandles.contains(children[j]))
						allhandles << children[j];
			}
		
		handles.clear();
		oldNames.clear();
		newNames.clear();

		oldNames += oldname;
		if (net && makeUnique)
			newNames += net->makeUnique(newname);
		else
			newNames += newname;
	}
	
	RenameCommand::RenameCommand(const QString& name, NetworkHandle * net, const QString& oldname, const QString& newname, bool forceUnique)
		: QUndoCommand(name), changeDataCommand(0), network(net), makeUnique(forceUnique)
	{
		if (net)
			this->allhandles = net->handles();

		handles.clear();
		oldNames.clear();
		newNames.clear();

		oldNames += oldname;
		if (net && makeUnique)
			newNames += net->makeUnique(newname);
		else
			newNames += newname;
	}
	
	RenameCommand::RenameCommand(const QString& name, NetworkHandle * net, const QList<ItemHandle*>& allItems, const QList<QString>& oldname, const QList<QString>& newname, bool forceUnique)
		: QUndoCommand(name), changeDataCommand(0), network(net), makeUnique(forceUnique)
	{
		allhandles = allItems;
		for (int i=0; i < allhandles.size(); ++i)
			if (allhandles[i])
			{
				QList<ItemHandle*> & children = allhandles[i]->children;
				for (int j=0; j < children.size(); ++j)
					if (children[j] && !allhandles.contains(children[j]))
						allhandles << children[j];
			}
		
		handles.clear();
		oldNames.clear();
		newNames.clear();

		oldNames << oldname;
		if (net && makeUnique)
			newNames << net->makeUnique(newname);
		else
			newNames << newname;
	}
	
	RenameCommand::RenameCommand(const QString& name, NetworkHandle * net, const QList<QString>& oldname, const QList<QString>& newname, bool forceUnique)
		: QUndoCommand(name), changeDataCommand(0), network(net), makeUnique(forceUnique)
	{
		if (net)
			this->allhandles = net->handles();
		handles.clear();
		oldNames.clear();
		newNames.clear();
		
		oldNames << oldname;
		if (net && makeUnique)
			newNames << net->makeUnique(newname);
		else
			newNames << newname;
	}

	RenameCommand::RenameCommand(const QString& name, NetworkHandle * net, const QList<ItemHandle*>& allItems, ItemHandle * handle, const QString& newname, bool forceUnique)
		: QUndoCommand(name), changeDataCommand(0), network(net), makeUnique(forceUnique)
	{
		allhandles = allItems;
		for (int i=0; i < allhandles.size(); ++i)
			if (allhandles[i])
			{
				QList<ItemHandle*> & children = allhandles[i]->children;
				for (int j=0; j < children.size(); ++j)
					if (children[j] && !allhandles.contains(children[j]))
						allhandles << children[j];
			}

		handles.clear();
		oldNames.clear();
		newNames.clear();

		ItemHandle * handle1;
		QStringList allNames;
		QString s;
		for (int i=0; i < allItems.size(); ++i)
			if ((handle1 = (allItems[i])) && (handle != handle1))
			{
				allNames << handle1->fullName();
				allNames << handle1->fullName(QObject::tr("_"));
			}

		if (handle)
		{
			handles += handle;
			oldNames += handle->fullName();
			if (net && makeUnique)
			{
				s = handle->name;
				handle->name = newname;
				if (handle->parent)
					handle->name.remove(handle->parent->fullName() + QObject::tr("."));
				newNames += net->makeUnique(handle);
				handle->name = s;
			}
			else
				newNames += newname;
		}
	}

	RenameCommand::RenameCommand(const QString& name, NetworkHandle * net, const QList<ItemHandle*>& items, const QList<QString>& newnames, bool forceUnique)
		: QUndoCommand(name), changeDataCommand(0), network(net), makeUnique(forceUnique)
	{
		if (net)
			this->allhandles = net->handles();
		handles.clear();
		oldNames.clear();
		newNames.clear();

		ItemHandle * handle;
		QStringList allNames;
		QString s;

		for (int i=0; i < allhandles.size(); ++i)
			if ((handle = (allhandles[i])) && !items.contains(handle))
			{
				allNames << handle->fullName();
				allNames << handle->fullName(QObject::tr("_"));
			}

		for (int i=0; i < items.size() && i < newnames.size() ; ++i)
		{
			handle = items[i];
			if (handle)
			{
				handles += handle;
				oldNames += handle->fullName();
				
				if (net && makeUnique)
				{
					s = handle->name;
					handle->name = newnames[i];
					if (handle->parent)
						handle->name.remove(handle->parent->fullName() + QObject::tr("."));
					newNames += net->makeUnique(handle,newNames);
					handle->name = s;
				}
				else
					newNames += newnames[i];
			}
		}
	}

	RenameCommand::RenameCommand(const QString& name, NetworkHandle * net, const QList<ItemHandle*>& allItems, const QList<ItemHandle*>& items, const QList<QString>& newnames, bool forceUnique)
		: QUndoCommand(name), changeDataCommand(0), network(net), makeUnique(forceUnique)
	{
		allhandles = allItems;
		for (int i=0; i < allhandles.size(); ++i)
			if (allhandles[i])
			{
				QList<ItemHandle*> & children = allhandles[i]->children;
				for (int j=0; j < children.size(); ++j)
					if (children[j] && !allhandles.contains(children[j]))
						allhandles << children[j];
			}

		handles.clear();
		oldNames.clear();
		newNames.clear();
		ItemHandle * handle;
		QString s;

		for (int i=0; i < items.size() && i < newnames.size() ; ++i)
		{
			handle = (items[i]);
			if (handle)
			{
				handles += handle;
				oldNames += handle->fullName();
				if (net && makeUnique)
				{
					s = handle->name;
					handle->name = newnames[i];
					if (handle->parent)
						handle->name.remove(handle->parent->fullName() + QObject::tr("."));
					newNames += net->makeUnique(handle,newNames);
					handle->name = s;
				}
				else
					newNames += newnames[i];
			}
		}
	}

	void RenameCommand::substituteString(QString& target, const QString& oldname,const QString& newname0)
	{
		if (oldname == newname0) return;
		QString newname = newname0;
		newname.replace(QRegExp("[^A-Za-z0-9_]"),QString("_@@@_"));

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
		if (oldname.isEmpty()) return; //impossible

		DataTable<qreal> * nDat = 0;
		DataTable<QString> * sDat = 0;

		for (int i=0; i < handles.size(); ++i)
		{
			if (handles[i])  //go through each handles num data and text data
			{
				QString fullname("");
				QString s = newname, s2;
				if (!handles[i]->name.isEmpty())
				{
					fullname = handles[i]->fullName() + QObject::tr(".");
					s.remove(fullname);
				}
				QList< QString > keys = handles[i]->numericalDataNames();
				for (int j=0; j < keys.size(); ++j)  //go through each num data
				{
					nDat = &(handles[i]->numericalDataTable( keys[j] ));
					for (int k=0; k < nDat->rows(); ++k)
					{
						if (nDat->rowName(k).contains(oldname))
						{
							s2 = nDat->rowName(k);
							substituteString(s2,oldname,newname);
							nDat->setRowName(k,s2);
						}

						if (fullname + nDat->rowName(k) == oldname)						
							nDat->setRowName(k,s);
					}
					for (int k=0; k < nDat->columns(); ++k)
					{
						if (nDat->columnName(k).contains(oldname))
						{
							s2 = nDat->columnName(k);
							substituteString(s2,oldname,newname);
							nDat->setColumnName(k,s2);
						}
						
						if (fullname + nDat->columnName(k) == oldname)
							nDat->setColumnName(k,s);
					}
				}
				keys = handles[i]->textDataNames();
				for (int j=0; j < keys.size(); ++j)  //go through each text data
				{
					sDat = &(handles[i]->textDataTable (keys[j]));
					for (int k=0; k < sDat->rows(); ++k)
					{
						if (sDat->rowName(k).contains(oldname))
						{
							s2 = sDat->rowName(k);
							substituteString(s2,oldname,newname);
							sDat->setRowName(k,s2);
						}

						if (fullname + sDat->rowName(k) == oldname)						
							sDat->setRowName(k,s);
					}
					for (int k=0; k < sDat->columns(); ++k)
					{
						if (sDat->columnName(k).contains(oldname))
						{
							s2 = sDat->columnName(k);
							substituteString(s2,oldname,newname);
							sDat->setColumnName(k,s2);
						}
						
						if (fullname + sDat->columnName(k) == oldname)
							sDat->setColumnName(k,s);
					}
					for (int k=0; k < sDat->rows(); ++k) //substitute each value in the table
						for (int l=0; l < sDat->columns(); ++l)
						{
							if (sDat->value(k,l).contains(oldname))
								substituteString(sDat->value(k,l),oldname,newname);
						}
				}
			}
		}
	}

	RenameCommand::~RenameCommand()
	{
		if (changeDataCommand)
            delete changeDataCommand;
		changeDataCommand = 0;
	}

	void RenameCommand::redo()
	{
		bool firstTime = (changeDataCommand == 0);
		QList< DataTable<qreal>* > oldData1, newData1;
		QList< DataTable<QString>* > oldData2, newData2;

		if (firstTime)
		{
			for (int i=0; i < allhandles.size(); ++i)
				if (allhandles[i])
				{
					QList<QString> keys1 = allhandles[i]->numericalDataNames();
					QList<QString> keys2 = allhandles[i]->textDataNames();

					for (int j=0; j < keys1.size(); ++j)
						oldData1 += new DataTable<qreal>(allhandles[i]->numericalDataTable( keys1[j] ));

					for (int j=0; j < keys2.size(); ++j)
						oldData2 += new DataTable<QString>(allhandles[i]->textDataTable( keys2[j] ));
				}
		}

		if (firstTime)
		{
			QRegExp regexp("\\.([^\\.]+)$");

			for (int i=0; i < oldNames.size() && i < newNames.size(); ++i)
			{
				if (firstTime)
					findReplaceAllHandleData(allhandles,oldNames[i],newNames[i]);

				if (handles.size() == 0)
					for (int j=0; j < allhandles.size(); ++j)
						if (allhandles[j] && allhandles[j]->fullName() == oldNames[i])
						{
							oldItemNames << QPair<ItemHandle*,QString>(allhandles[j],allhandles[j]->name);

							regexp.indexIn(newNames[i]);
							for (int k=0; k < allhandles[j]->graphicsItems.size(); ++k)
							{
								TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(allhandles[j]->graphicsItems[k]);
								if (textItem)
								{
									if (textItem->toPlainText() == allhandles[j]->name)
									{
										oldTextItemsNames << QPair<TextGraphicsItem*,QString>(textItem,allhandles[j]->name);

										if (regexp.numCaptures() > 0 && !regexp.cap(1).isEmpty())
										{
											textItem->setPlainText(regexp.cap(1));
											newTextItemsNames << QPair<TextGraphicsItem*,QString>(textItem,regexp.cap(1));
										}
										else
										{
											textItem->setPlainText(newNames[i]);
											newTextItemsNames << QPair<TextGraphicsItem*,QString>(textItem,newNames[i]);
										}
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

							newItemNames << QPair<ItemHandle*,QString>(allhandles[j],allhandles[j]->name);
						}
			}


			for (int i=0; i < allhandles.size(); ++i)
				if (allhandles[i])
				{
					QList<QString> keys1 = allhandles[i]->numericalDataNames();
					QList<QString> keys2 = allhandles[i]->textDataNames();

					for (int j=0; j < keys1.size(); ++j)
						newData1 += &(allhandles[i]->numericalDataTable( keys1[j] ));

					for (int j=0; j < keys2.size(); ++j)
						newData2 += &(allhandles[i]->textDataTable( keys2[j] ));
				}
				changeDataCommand = new Change2DataCommand<qreal,QString>(QString(""), newData1, oldData1, newData2, oldData2);
				for (int i=0; i < oldData1.size(); ++i)
					if (oldData1[i])
						delete oldData1[i];
				for (int i=0; i < oldData2.size(); ++i)
					if (oldData2[i])
						delete oldData2[i];

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
								oldTextItemsNames << QPair<TextGraphicsItem*,QString>(textItem,handle->name);

								if (regexp.numCaptures() > 0 && !regexp.cap(1).isEmpty())
								{
									textItem->setPlainText(regexp.cap(1));
									newTextItemsNames << QPair<TextGraphicsItem*,QString>(textItem,regexp.cap(1));
								}
								else
								{
									textItem->setPlainText(newNames[i]);
									newTextItemsNames << QPair<TextGraphicsItem*,QString>(textItem,newNames[i]);
								}

							}
						}
					}

					oldItemNames << QPair<ItemHandle*,QString>(handle,handle->name);

					if (regexp.numCaptures() > 0 && !regexp.cap(1).isEmpty())
						handle->name = regexp.cap(1);
					else
						handle->name = newNames[i];

					newItemNames << QPair<ItemHandle*,QString>(handle,handle->name);
				}
			}
		}
		else
		{
			for (int i=0; i < newItemNames.size(); ++i)
				if (newItemNames[i].first)
					newItemNames[i].first->name = newItemNames[i].second;

			for (int i=0; i < newTextItemsNames.size(); ++i)
				if (newTextItemsNames[i].first)
					newTextItemsNames[i].first->setPlainText(newTextItemsNames[i].second);

			if (changeDataCommand)
				changeDataCommand->undo();
		}
		
		/*if (network)
		{
			QList<TextEditor*> editors = network->editors();
			for (int i=0; i < editors.size(); ++i)
				if (editors[i])
					for (int j=0; j < oldNames.size() && j < newNames.size(); ++j)
						editors[i]->replace(oldNames[j],newNames[j]);
		}*/
	}

	void RenameCommand::undo()
	{
		for (int i=0; i < oldItemNames.size(); ++i)
				if (oldItemNames[i].first)
					oldItemNames[i].first->name = oldItemNames[i].second;

		for (int i=0; i < oldTextItemsNames.size(); ++i)
			if (oldTextItemsNames[i].first)
				oldTextItemsNames[i].first->setPlainText(oldTextItemsNames[i].second);

		if (changeDataCommand)
		{
		    changeDataCommand->redo();
		}
		
		/*if (network)
		{
			QList<TextEditor*> editors = network->editors();
			for (int i=0; i < editors.size(); ++i)
				if (editors[i])
					for (int j=0; j < oldNames.size() && j < newNames.size(); ++j)
						editors[i]->replace(newNames[j],oldNames[j]);
		}*/	
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
			if (commands[i] && !doNotDelete.contains(commands[i]) && !MainWindow::invalidPointers.contains(commands[i]))
			{
				MainWindow::invalidPointers[ (void*)commands[i] ] = true;
				delete commands[i];
			}
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
		if (command && deleteCommand && !MainWindow::invalidPointers.contains(command)) 
		{
			delete command;
			MainWindow::invalidPointers[ (void*)command ] = true;
		}
	}
	void ReverseUndoCommand::redo()
	{
		if (command) command->undo();
	}
	void ReverseUndoCommand::undo()
	{
		if (command) command->redo();
	}

	ReplaceNodeGraphicsCommand::ReplaceNodeGraphicsCommand(const QString& text,NodeGraphicsItem* node,const QString& filename,bool transform)
		: QUndoCommand(text), transform(transform)
	{
		if (node && !qgraphicsitem_cast<ToolGraphicsItem*>(node->topLevelItem()))
		{
			targetNodes += node;
			NodeGraphicsItem copy1(*node);
			oldNodes += copy1;

			NodeGraphicsItem copy2(*node);
			loadFromFile(&copy2,filename);
			newNodes += copy2;
		}
	}

	ReplaceNodeGraphicsCommand::ReplaceNodeGraphicsCommand(const QString& text,const QList<NodeGraphicsItem*>& nodes,const QList<QString>& filenames, bool transform)
		: QUndoCommand(text), transform(transform)
	{
		for (int i=0; i < nodes.size() && i < filenames.size(); ++i)
		{
			NodeGraphicsItem * node = nodes[i];
			QString filename = filenames[i];
			if (node && !qgraphicsitem_cast<ToolGraphicsItem*>(node->topLevelItem()))
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
			if (itemsToDelete[i] && !MainWindow::invalidPointers.contains( (void*)itemsToDelete[i]))
			{
				if (itemsToDelete[i]->scene())
					itemsToDelete[i]->scene()->removeItem(itemsToDelete[i]);
				
				MainWindow::invalidPointers[ (void*)itemsToDelete[i] ] = true;
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
				ArrowHeadItem * arrow = ArrowHeadItem::cast(targetNodes[i]);
				if (arrow)
				{
					if (arrow->connectionItem)
						arrow->connectionItem->refresh(false);
				}
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
				ArrowHeadItem * arrow = ArrowHeadItem::cast(targetNodes[i]);
				if (arrow)
				{
					if (arrow->connectionItem)
						arrow->connectionItem->refresh();
				}
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

		QString  home = MainWindow::homeDir(),
				temp = MainWindow::tempDir(),
				current = QDir::currentPath(),
				appDir = QCoreApplication::applicationDirPath();

		QString name[] = {  
			fileName,
			home + QObject::tr("/") + fileName,
			temp + QObject::tr("/") + fileName,
			current + QObject::tr("/") + fileName,
			appDir + QObject::tr("/") + fileName,
		};

		for (int i=0; i < 5; ++i)
		{
			file.setFileName(name[i]);
			if (file.exists())
				break;
		}

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
		
		if (transform)
		{
			node->setTransform(t1);
		}
		else
		{
			QRectF rect = node->boundingRect();
			QTransform t(node->defaultSize.width()/rect.width(), 0, 0, node->defaultSize.height()/rect.height(), 0, 0);
			node->setTransform(t);
		}

		//node->setParentItem(parent);
	}

	AssignHandleCommand::AssignHandleCommand(const QString& text, const QList<QGraphicsItem*>& items, QList<ItemHandle*>& handles)
		: QUndoCommand(text)
	{
		ItemHandle * handle = 0;

		for (int i=0; i < items.size() && i < handles.size(); ++i)
			if (items[i])
			{
				graphicsItems += items[i];

				handle = getHandle(items[i]);

				oldHandles += handle;
				newHandles += handles[i];

				oldItemHandles += QPair< QGraphicsItem*, ItemHandle* >(items[i], handle);
				newItemHandles += QPair< QGraphicsItem*, ItemHandle* >(items[i], handles[i]);
			}
	}

	AssignHandleCommand::AssignHandleCommand(const QString& text, const QList<QGraphicsItem*>& items, ItemHandle* newHandle)
		: QUndoCommand(text)
	{

		ItemHandle * handle = 0;
		for (int i=0; i < items.size(); ++i)
			if (items[i])
			{
				graphicsItems += items[i];
				handle = getHandle(items[i]);

				oldHandles += handle;
				newHandles += newHandle;

				oldItemHandles += QPair< QGraphicsItem*, ItemHandle* >( items[i], handle );
				newItemHandles += QPair< QGraphicsItem*, ItemHandle* >( items[i], newHandle );
			}

	}

	AssignHandleCommand::AssignHandleCommand(const QString& text, QGraphicsItem* item, ItemHandle* newHandle)
		: QUndoCommand(text)
	{
		graphicsItems += item;
		ItemHandle * handle = getHandle(item);

		oldHandles += handle;
		newHandles += newHandle;

		oldItemHandles += QPair< QGraphicsItem*, ItemHandle* >( item, handle );
		newItemHandles += QPair< QGraphicsItem*, ItemHandle* >( item, newHandle );
	}

	void AssignHandleCommand::redo()
	{
		TextGraphicsItem* textItem = 0;
		ItemHandle * handle = 0;
		for (int i=0; i < newItemHandles.size(); ++i)
		{
			if (newItemHandles[i].first)
			{
				handle = getHandle(newItemHandles[i].first);
				if ((textItem = qgraphicsitem_cast<TextGraphicsItem*>(newItemHandles[i].first)) &&
					handle && textItem->toPlainText() == handle->name &&
					newItemHandles[i].second)
					textItem->setPlainText(newItemHandles[i].second->name);
				setHandle(newItemHandles[i].first, newItemHandles[i].second);
			}
		}
	}

	void AssignHandleCommand::undo()
	{
		TextGraphicsItem* textItem = 0;
		ItemHandle * handle = 0;
		for (int i=0; i < oldItemHandles.size(); ++i)
		{
			if (oldItemHandles[i].first)
			{
				handle = getHandle(oldItemHandles[i].first);
				if ((textItem = qgraphicsitem_cast<TextGraphicsItem*>(oldItemHandles[i].first)) &&
					handle && textItem->toPlainText() == handle->name &&
					oldItemHandles[i].second)
					textItem->setPlainText(oldItemHandles[i].second->name);
				setHandle(oldItemHandles[i].first, oldItemHandles[i].second);
			}
		}
	}

	AssignHandleCommand::~AssignHandleCommand()
	{
		oldHandles << newHandles;
		for (int i=0; i < oldHandles.size(); ++i)
			if (oldHandles[i] && !MainWindow::invalidPointers.contains( (void*)oldHandles[i]))
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
						
						MainWindow::invalidPointers[ (void*)oldHandles[i] ] = true;
						delete oldHandles[i];
						oldHandles[i] = 0;
					}
			}
	}

	MergeHandlesCommand::MergeHandlesCommand(const QString& text, NetworkHandle * net, const QList<ItemHandle*>& handles) :
		QUndoCommand(text)
	{
		newHandle = 0;
		oldHandles = handles;
		
		QStringList oldNames, newNames;

		if (handles.size() > 0)
		{
			for (int i=0; i < handles.size(); ++i)
				if (handles[i])
				{
					newHandle = handles[i];
					oldHandles.removeAll(newHandle);
					break;
				}
		}

		for (int i =0; i < handles.size(); ++i)
			if (handles[i] && handles[i] != newHandle)
				oldNames << handles[i]->fullName();

		if (newHandle)
			for (int i=0; i < oldNames.size(); ++i)
				newNames << newHandle->fullName();

		QList<ItemHandle*> allHandles = net->handles();

		for (int i=0; i < handles.size(); ++i)
			allHandles.removeAll(handles[i]);

		allHandles << newHandle;

		renameCommand = 0;
		changeDataCommand = 0;

		if (newHandle)
		{
			QStringList list, keys;
			QList<NumericalDataTable*> newNumericTables, oldNumericTables;
			QList<TextDataTable*> newTextTables, oldTextTables;
			NumericalDataTable* nDat;
			TextDataTable* sDat;

			for (int i=0; i < handles.size(); ++i)
				if (handles[i])
				{
					list = handles[i]->numericalDataNames();
					for (int j=0; j < list.size(); ++j)
						if (!keys.contains(list[j]))
							keys << list[j];
				}
			
			for (int j=0; j < keys.size(); ++j)
			{
				oldNumericTables << &(newHandle->numericalDataTable(keys[j]));
				nDat = new NumericalDataTable(newHandle->numericalDataTable(keys[j]));
				newNumericTables << nDat;

				for (int i=0; i < handles.size(); ++i)
					if (handles[i] && handles[i]->hasNumericalData(keys[j]))
					{
						NumericalDataTable & dat = handles[i]->numericalDataTable(keys[j]);
						for (int r=0; r < dat.rows(); ++r)
							for (int c=0; c < dat.columns(); ++c)
								nDat->value(dat.rowName(r), dat.columnName(c)) = dat.value(r,c);
					}
			}
			
			keys.clear();
			for (int i=0; i < handles.size(); ++i)
				if (handles[i])
				{
					list = handles[i]->textDataNames();
					for (int j=0; j < list.size(); ++j)
						if (!keys.contains(list[j]))
							keys << list[j];
				}
			
			for (int j=0; j < keys.size(); ++j)
			{
				oldTextTables << &(newHandle->textDataTable(keys[j]));
				sDat = new TextDataTable(newHandle->textDataTable(keys[j]));
				newTextTables << sDat;

				for (int i=0; i < handles.size(); ++i)
					if (handles[i] && handles[i]->hasTextData(keys[j]))
					{
						TextDataTable & dat = handles[i]->textDataTable(keys[j]);
						for (int r=0; r < dat.rows(); ++r)
							for (int c=0; c < dat.columns(); ++c)
								sDat->value(dat.rowName(r), dat.columnName(c)) = dat.value(r,c);
					}
			}
			
			changeDataCommand = new Change2DataCommand<qreal,QString>(QString(""), oldNumericTables, newNumericTables, oldTextTables, newTextTables);

			for (int i=0; i < newNumericTables.size(); ++i)
				delete newNumericTables[i];
			
			for (int i=0; i < newTextTables.size(); ++i)
				delete newTextTables[i];
			
			renameCommand = new RenameCommand(QString("rename"),net,allHandles,oldNames,newNames,false);
		}
	}

	MergeHandlesCommand::~MergeHandlesCommand()
	{
		if (renameCommand)
			delete renameCommand;
		
		if (changeDataCommand)
			delete changeDataCommand;
	}

	void MergeHandlesCommand::redo()
	{
		if (newHandle == 0) return;

		allChildren.clear();
		allGraphicsItems.clear();

		for (int i=0; i < oldHandles.size(); ++i)
			if (oldHandles[i])
			{
				oldParents[ oldHandles[i] ] = oldHandles[i]->parent;
				oldHandles[i]->setParent(0,false);
				
				oldChildren[ oldHandles[i] ] = oldHandles[i]->children;
				allChildren << oldHandles[i]->children;

				oldGraphicsItems[ oldHandles[i] ] = oldHandles[i]->graphicsItems;
				allGraphicsItems << oldHandles[i]->graphicsItems;
			}

		for (int i=0; i < allChildren.size(); ++i)
			if (allChildren[i])
				allChildren[i]->setParent(newHandle,false);

		TextGraphicsItem * textItem = 0;
		ItemHandle * handle = 0;
			
		for (int i=0; i < allGraphicsItems.size(); ++i)
			if (allGraphicsItems[i])
			{
				handle = getHandle(allGraphicsItems[i]);

				if ( 	handle &&
						(textItem = qgraphicsitem_cast<TextGraphicsItem*>(allGraphicsItems[i])) &&
						(textItem->toPlainText() == handle->name || textItem->toPlainText() == handle->fullName()) )
						textItem->setPlainText(newHandle->name);

				setHandle(allGraphicsItems[i],newHandle);
			}

		if (changeDataCommand)
			changeDataCommand->redo();

		if (renameCommand)
			renameCommand->redo();
	}

	void MergeHandlesCommand::undo()
	{
		if (newHandle == 0) return;

		if (renameCommand)
			renameCommand->undo();

		if (changeDataCommand)
			changeDataCommand->redo();

		QList<ItemHandle*> keyHandles = oldChildren.keys();
		for (int i=0; i < keyHandles.size(); ++i)
			if (keyHandles[i])
			{
				QList<ItemHandle*> children = oldChildren[ keyHandles[i] ];
				for (int j=0; j < children.size(); ++j)
					if (children[j])
						children[j]->setParent(keyHandles[i],false);
			}
		
		keyHandles = oldParents.keys();
		for (int i=0; i < keyHandles.size(); ++i)
			if (keyHandles[i])
			{
				keyHandles[i]->setParent( oldParents[ keyHandles[i] ], false );
			}

		TextGraphicsItem * textItem = 0;
		ItemHandle * handle = 0;

		keyHandles = oldGraphicsItems.keys();

		for (int i=0; i < keyHandles.size(); ++i)
			if (keyHandles[i])
			{
				QList<QGraphicsItem*> items = oldGraphicsItems[ keyHandles[i] ];
				for (int j=0; j < items.size(); ++j)
					if (items[j])
					{
						handle = getHandle(items[j]);

						if ( 	handle &&
								(textItem = qgraphicsitem_cast<TextGraphicsItem*>(items[j])) &&
								(textItem->toPlainText() == handle->name || textItem->toPlainText() == handle->fullName()) )
								textItem->setPlainText(keyHandles[i]->name);

						setHandle(items[j],keyHandles[i]);
					}
			}
	}

	SetParentHandleCommand::SetParentHandleCommand(const QString& name, NetworkHandle * net, ItemHandle * child, ItemHandle * parent)
		: QUndoCommand(name)
	{
		this->net = net;
		children += child;
		newParents += parent;
		oldParents += child->parent;
		renameCommand = 0;
	}

	SetParentHandleCommand::SetParentHandleCommand(const QString& name, NetworkHandle * net, const QList<ItemHandle*>& childlist, const QList<ItemHandle*>& parents)
		: QUndoCommand(name)
	{
		this->net = net;
		
		for (int i=0; i < childlist.size() && i < parents.size(); ++i)
			if (childlist[i] && !children.contains(childlist[i]))
			{
				children += childlist[i];
				newParents += parents[i];
				if (childlist[i])
					oldParents += childlist[i]->parent;
				else
					oldParents += 0;
			}
		renameCommand = 0;
	}

	SetParentHandleCommand::SetParentHandleCommand(const QString& name, NetworkHandle * net, const QList<ItemHandle*>& childlist, ItemHandle * parent)
		: QUndoCommand(name)
	{
		this->net = net;
		
		for (int i=0; i < childlist.size(); ++i)
			if (childlist[i] && !children.contains(childlist[i]))
			{
				children += childlist[i];
				newParents += parent;
				if (childlist[i])
					oldParents += childlist[i]->parent;
				else
					oldParents += 0;
			}
		renameCommand = 0;
	}

	SetParentHandleCommand::~SetParentHandleCommand()
	{
		if (renameCommand)
			delete renameCommand;
		for (int i=0; i < children.size(); ++i)
			if (children[i] && !MainWindow::invalidPointers.contains((void*)children[i]))
			{
				delete children[i];
				MainWindow::invalidPointers[ (void*)children[i] ] = true;
				children[i] = 0;
			}
	}

	void SetParentHandleCommand::redo()
	{
		if (!renameCommand && net)
		{
			QList<QString> newNames, oldNames;
			QStringList allNames;
			QString s0, s1;
			
			for (int i=0; i < children.size() && i < newParents.size() && i < oldParents.size(); ++i)
				if (children[i] && newParents[i] != oldParents[i])
				{
					if (children[i] != newParents[i] && !children[i]->isChildOf(newParents[i]))
					{
						children[i]->setParent(newParents[i],false);
						s1 = net->makeUnique(children[i],allNames);						
						children[i]->setParent(oldParents[i],false);
						s0 = children[i]->fullName();
						if (s0 != s1)
						{
							oldNames += s0;
							newNames += s1;
						}
						allNames += s1;
					}
				}
			renameCommand = new RenameCommand(QString("rename"),net,oldNames,newNames);
		}

		if (renameCommand)
			renameCommand->redo();
		
		for (int i=0; i < children.size() && i < newParents.size() && i < oldParents.size(); ++i)
			if (children[i] && newParents[i] != oldParents[i])
			{
				if (children[i] != newParents[i] && !children[i]->isChildOf(newParents[i]))
				{
					children[i]->setParent(newParents[i],false);
				}
			}
	}

	void SetParentHandleCommand::undo()
	{
		if (renameCommand)
			renameCommand->undo();
			
		for (int i=0; i < children.size() && i < newParents.size() && i < oldParents.size(); ++i)
			if (children[i] && newParents[i] != oldParents[i])
			{
				if (children[i] != oldParents[i] && !children[i]->isChildOf(oldParents[i]))
				{
					children[i]->setParent(oldParents[i],false);
				}
			}
	}

	SetGraphicsSceneVisibilityCommand::SetGraphicsSceneVisibilityCommand(const QString& name, const QList<QGraphicsItem*>& list, const QList<bool>& values)
		: QUndoCommand(name)
	{
		ConnectionGraphicsItem * connection;
		NodeGraphicsItem * node;
		for (int i=0; i < list.size() && i < values.size(); ++i)
		{
			if (list[i] && list[i]->isVisible() != values[i])
			{
				items << list[i];
				before << list[i]->isVisible();
				if (connection = ConnectionGraphicsItem::cast(list[i]))
				{
					QList<QGraphicsItem*> list2 = connection->controlPointsAsGraphicsItems();
					list2 << connection->arrowHeadsAsGraphicsItems();
					if (connection->centerRegionItem)
						list2 << connection->centerRegionItem;
					for (int j=0; j < list2.size(); ++j)
						if (list2[j] && list[i]->isVisible() == list2[j]->isVisible())
						{
							items << list2[j];
							before << list[i]->isVisible();
						}
				}
				else
				if (node = NodeGraphicsItem::cast(list[i]))
				{
					QList<ControlPoint*> list2 = node->allControlPoints();
					for (int j=0; j < list2.size(); ++j)
						if (list2[j] && list[i]->isVisible() == list2[j]->isVisible())
						{
							items << list2[j];
							before << list[i]->isVisible();
						}
				}
			}
		}
	}

	SetGraphicsSceneVisibilityCommand::SetGraphicsSceneVisibilityCommand(const QString& name, QGraphicsItem* item, bool value)
		: QUndoCommand(name)
	{
		if (item && item->isVisible() != value)
		{
			ConnectionGraphicsItem * connection;
			NodeGraphicsItem * node;
			
			items << item;
			before << item->isVisible();
			
			if (connection = ConnectionGraphicsItem::cast(item))
			{
				QList<QGraphicsItem*> list2 = connection->controlPointsAsGraphicsItems();
				list2 << connection->arrowHeadsAsGraphicsItems();
				if (connection->centerRegionItem)
					list2 << connection->centerRegionItem;
				for (int j=0; j < list2.size(); ++j)
					if (list2[j] && item->isVisible() == list2[j]->isVisible())
					{
						items << list2[j];
						before << item->isVisible();
					}
			}
			else
			if (node = NodeGraphicsItem::cast(item))
			{
				QList<ControlPoint*> list2 = node->allControlPoints();
				for (int j=0; j < list2.size(); ++j)
					if (list2[j] && item->isVisible() == list2[j]->isVisible())
					{
						items << list2[j];
						before << item->isVisible();
					}
			}
		}
	}

	SetGraphicsSceneVisibilityCommand::SetGraphicsSceneVisibilityCommand(const QString& name, const QList<QGraphicsItem*>& list, bool value)
		: QUndoCommand(name)
	{
		ConnectionGraphicsItem * connection;
		NodeGraphicsItem * node;
		for (int i=0; i < list.size(); ++i)
		{
			if (list[i] && list[i]->isVisible() != value)
			{
				items << list[i];
				before << list[i]->isVisible();
				
				if (connection = ConnectionGraphicsItem::cast(list[i]))
				{
					QList<QGraphicsItem*> list2 = connection->controlPointsAsGraphicsItems();
					list2 << connection->arrowHeadsAsGraphicsItems();
					if (connection->centerRegionItem)
						list2 << connection->centerRegionItem;
					for (int j=0; j < list2.size(); ++j)
						if (list2[j] && list[i]->isVisible() == list2[j]->isVisible())
						{
							items << list2[j];
							before << list[i]->isVisible();
						}
				}
				else
				if (node = NodeGraphicsItem::cast(list[i]))
				{
					QList<ControlPoint*> list2 = node->allControlPoints();
					for (int j=0; j < list2.size(); ++j)
						if (list2[j] && list[i]->isVisible() == list2[j]->isVisible())
						{
							items << list2[j];
							before << list[i]->isVisible();
						}
				}
			}
		}
	}

	void SetGraphicsSceneVisibilityCommand::redo()
	{
		for (int i=0; i < items.size() && i < before.size(); ++i)
		{
			items[i]->setVisible(!before[i]);
		}
	}

	void SetGraphicsSceneVisibilityCommand::undo()
	{
		for (int i=0; i < items.size() && i < before.size(); ++i)
		{
			items[i]->setVisible(before[i]);
		}
	}
	
	SetHandleFamilyCommand::SetHandleFamilyCommand(const QString& name, const QList<ItemHandle*>& items, const QList<ItemFamily*>& families) 
		: QUndoCommand(name)
	{
		for (int i=0; i < items.size() && i < families.size(); ++i)
			if (items[i])
			{
				handles << items[i];
				oldFamily << items[i]->family();
				newFamily << families[i];
			}
	}
	
	SetHandleFamilyCommand::SetHandleFamilyCommand(const QString& name, ItemHandle* item, ItemFamily* family)
	{
		if (item)
		{
			handles << item;
			oldFamily << item->family();
			newFamily << family;
		}
	}
	
	void SetHandleFamilyCommand::redo()
	{
		for (int i=0; i < handles.size() && i < newFamily.size(); ++i)
			if (handles[i])			
				handles[i]->setFamily(newFamily[i],false);
	}
	
	void SetHandleFamilyCommand::undo()
	{
		for (int i=0; i < handles.size() && i < oldFamily.size(); ++i)
			if (handles[i])			
				handles[i]->setFamily(oldFamily[i],false);
	}
	
		RemoveCurveSegmentCommand::RemoveCurveSegmentCommand(
		const QString& name, GraphicsScene * scene,
		ConnectionGraphicsItem::ControlPoint* item)
		: QUndoCommand(name)
	{
		curveSegments.clear();
		graphicsScene = scene;
		connectionItem = item->connectionItem;
		if (connectionItem == 0 || connectionItem->curveSegments.size() < 2) return;
		
		QList<ArrowHeadItem*> arrowHeads = connectionItem->arrowHeads();
		int only_in_node = -1, only_out_node = -1;
		for (int i=0; i < arrowHeads.size(); ++i)
		{
			if (arrowHeads[i] == 0)
			{
				if (only_in_node == -1)
					only_in_node = i;
				else
				{
					only_in_node = -1;
					break;
				}
			}
		}
		for (int i=0; i < arrowHeads.size(); ++i)
		{
			if (arrowHeads[i] != 0)
			{
				if (only_out_node == -1)
					only_out_node = i;
				else
				{
					only_out_node = -1;
					break;
				}
			}
		}

		for (int i=0; i < connectionItem->curveSegments.size(); ++i)
		{
			for (int j=0; j < connectionItem->curveSegments[i].size(); ++j)
			{
				if (connectionItem->curveSegments[i][j] != 0 && connectionItem->curveSegments[i][j] == item)
				{
					if (i != only_in_node && i != only_out_node)
					{
						curveSegments.append(connectionItem->curveSegments[i]);
						if (connectionItem->curveSegments[i][0])
							parentsAtStart.append(connectionItem->curveSegments[i][0]->parentItem());
						else
							parentsAtStart.append(0);
						if (connectionItem->curveSegments[i].last())
							parentsAtEnd.append(connectionItem->curveSegments[i].last()->parentItem());
						else
							parentsAtEnd.append(0);
					}
					break;
				}
			}
		}
	}

	RemoveCurveSegmentCommand::RemoveCurveSegmentCommand(
		const QString& name, GraphicsScene * scene,
		ConnectionGraphicsItem* connection,
		QList<ConnectionGraphicsItem::ControlPoint*> items)
		: QUndoCommand(name)
	{
		curveSegments.clear();
		graphicsScene = scene;
		connectionItem = connection;
		if (connectionItem == 0 || connectionItem->curveSegments.size() < 2) return;

		bool done = false;
		for (int i=0; i < connectionItem->curveSegments.size(); ++i)
		{
			done = false;
			for (int j=0; j < connectionItem->curveSegments[i].size(); ++j)
			{
				for (int k = 0; k < items.size(); ++k)
				{
					if (connectionItem->curveSegments[i][j] != 0 && connectionItem->curveSegments[i][j] == items[k])
					{
						curveSegments.append(connectionItem->curveSegments[i]);
						if (connectionItem->curveSegments[i][0])
							parentsAtStart.append(connectionItem->curveSegments[i][0]->parentItem());
						else
							parentsAtStart.append(0);
						if (connectionItem->curveSegments[i].last())
							parentsAtEnd.append(connectionItem->curveSegments[i].last()->parentItem());
						else
							parentsAtEnd.append(0);
						done = true;
						break;
					}
				}
				if (done) break;
			}
		}
	}

	void RemoveCurveSegmentCommand::undo()
	{
		if (connectionItem == 0) return;

		if (connectionItem->curveSegments.size() == 1 && connectionItem->curveSegments.size() >= 4
			&& curveSegments.size() > 0 && curveSegments[0].at(3) != 0
			&& connectionItem->curveSegments[0][3]->scene() != 0)
		{
			connectionItem->curveSegments[0][3]->setParentItem(0);
			connectionItem->curveSegments[0][3]->scene()->removeItem(connectionItem->curveSegments[0][3]);
			connectionItem->curveSegments[0][3] = curveSegments[0].at(3);
		}
		for (int i=0; i < curveSegments.size(); ++i)
		{
			if (curveSegments[i][0] && parentsAtStart.size() > i && parentsAtStart[i] &&
				!MainWindow::invalidPointers.contains((void*)parentsAtStart[i]))
				curveSegments[i][0]->setParentItem(parentsAtStart[i]);
			if (curveSegments[i].last() && parentsAtEnd.size() > i && parentsAtEnd[i] &&
				!MainWindow::invalidPointers.contains((void*)parentsAtEnd[i]))
				curveSegments[i].last()->setParentItem(parentsAtEnd[i]);

			connectionItem->curveSegments.append(curveSegments[i]);
			for (int j=0; j < curveSegments[i].size(); ++j)
				if (curveSegments[i][j] != 0)
					curveSegments[i][j]->setVisible( connectionItem->controlPointsVisible );
			if (curveSegments[i].arrowStart)
				curveSegments[i].arrowStart->setVisible(true);
			if (curveSegments[i].arrowEnd)
				curveSegments[i].arrowEnd->setVisible(true);
		}
		connectionItem->refresh();
	}

	void RemoveCurveSegmentCommand::redo()
	{
		if (connectionItem == 0) return;
		int k;
		for (int i=0; i < curveSegments.size(); ++i)
		{
			k = connectionItem->curveSegments.indexOf(curveSegments[i]);
			if (k >=0 && k < connectionItem->curveSegments.size())
				connectionItem->curveSegments.removeAt(k);
			for (int j=0; j < curveSegments[i].size(); ++j)
				if (curveSegments[i][j] != 0)
				{
					curveSegments[i][j]->setVisible(false);
					if (curveSegments[i][j]->parentItem())
						curveSegments[i][j]->setParentItem(0);
				}
			if (curveSegments[i].arrowStart)
				curveSegments[i].arrowStart->setVisible(false);
			if (curveSegments[i].arrowEnd)
				curveSegments[i].arrowEnd->setVisible(false);
		}
		if (connectionItem->curveSegments.size() == 1 && connectionItem->curveSegments.size() >= 4
			&& curveSegments.size() > 0 && curveSegments[0].at(0) != 0)
		{
			connectionItem->curveSegments[0][3] = new ConnectionGraphicsItem::ControlPoint(*connectionItem->curveSegments[0][3]);
			connectionItem->curveSegments[0][3]->setParentItem(curveSegments[0].at(0)->parentItem());
		}
		connectionItem->refresh();
	}

	AddCurveSegmentCommand::AddCurveSegmentCommand(
		const QString& name, GraphicsScene * scene,
		ConnectionGraphicsItem* connection,
		ConnectionGraphicsItem::CurveSegment& item)
		: QUndoCommand(name)
	{
		undone = false;
		curveSegments.clear();
		graphicsScene = scene;
		connectionItem = connection;
		if (connectionItem == 0) return;
		curveSegments.append(item);
	}

	AddCurveSegmentCommand::AddCurveSegmentCommand(
		const QString& name, GraphicsScene * scene,
		ConnectionGraphicsItem* connection,
		QList<ConnectionGraphicsItem::CurveSegment> items)
		: QUndoCommand(name)
	{
		undone = false;
		curveSegments.clear();
		graphicsScene = scene;
		connectionItem = connection;
		if (connectionItem == 0) return;
		curveSegments << items;
	}

	void AddCurveSegmentCommand::redo()
	{
		if (connectionItem == 0) return;

		for (int i=0; i < curveSegments.size(); ++i)
		{
			connectionItem->curveSegments.append(curveSegments[i]);

			if (curveSegments[i].arrowStart)
				curveSegments[i].arrowStart->setVisible(true);
			if (curveSegments[i].arrowEnd)
				curveSegments[i].arrowEnd->setVisible(true);
		}
		connectionItem->refresh();
		
		undone = false;
	}

	void AddCurveSegmentCommand::undo()
	{
		if (connectionItem == 0) return;
		int k;
		for (int i=0; i < curveSegments.size(); ++i)
		{
			k = connectionItem->curveSegments.indexOf(curveSegments[i]);
			if (k >=0 && k < connectionItem->curveSegments.size())
				connectionItem->curveSegments.removeAt(k);

			if (curveSegments[i].arrowStart)
				curveSegments[i].arrowStart->setVisible(false);
			if (curveSegments[i].arrowEnd)
				curveSegments[i].arrowEnd->setVisible(false);
		}
		connectionItem->refresh();
		
		undone = true;
	}

	AddCurveSegmentCommand::~AddCurveSegmentCommand()
	{
		if (!undone) return;

		for (int i=0; i < curveSegments.size(); ++i)
		{
			for (int j=0; j < curveSegments[i].size(); ++j)
				if (curveSegments[i][j] && 
					!curveSegments[i][j]->connectionItem)
				{
					curveSegments[i][j]->setParentItem(0);
					delete curveSegments[i][j];
				}
				if (curveSegments[i].arrowStart && 
					!curveSegments[i].arrowStart->connectionItem &&
					!MainWindow::invalidPointers.contains((void*)curveSegments[i].arrowStart))
				{
					curveSegments[i].arrowStart->setParentItem(0);
					delete curveSegments[i].arrowStart;
					MainWindow::invalidPointers[ (void*)curveSegments[i].arrowStart ] = true;
					curveSegments[i].arrowStart = 0;
				}
				if (curveSegments[i].arrowEnd && 
					!curveSegments[i].arrowEnd->connectionItem &&
					!MainWindow::invalidPointers.contains((void*)curveSegments[i].arrowEnd))
				{
					curveSegments[i].arrowEnd->setParentItem(0);
					delete curveSegments[i].arrowEnd;
					MainWindow::invalidPointers[ (void*)curveSegments[i].arrowEnd ] = true;
					curveSegments[i].arrowEnd = 0;
				}
		}
	}
	
		AddControlPointCommand::AddControlPointCommand(
		const QString& name, GraphicsScene * scene,
		ConnectionGraphicsItem::ControlPoint* item)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems.clear();
		graphicsItems += item;
		undone = false;
	}

	AddControlPointCommand::AddControlPointCommand(
		const QString& name, GraphicsScene * scene,
		QList<ConnectionGraphicsItem::ControlPoint*> items)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems = items;
		undone = false;
	}

	void AddControlPointCommand::redo()
	{
		QList<ConnectionGraphicsItem::ControlPoint*> controlPointItems = graphicsItems;
		graphicsItems.clear();

		for (int k=0; k < controlPointItems.size(); k+=3)

			if (controlPointItems[k] && graphicsScene && controlPointItems[k]->connectionItem)
			{
				ConnectionGraphicsItem::ControlPoint* controlPoint = controlPointItems[k];
				ConnectionGraphicsItem * item = controlPoint->connectionItem;

				QPointF loc = controlPoint->scenePos();
				qreal dist = -1;
				int k1 = 0, k2 = 0;
				QPointF p0,p1;

				for (int i=0; i < item->curveSegments.size(); ++i)
					for (int j=0; j < item->curveSegments[i].size(); ++j)
					{
						if (item->curveSegments[i][j])
						{
							p0 = item->curveSegments[i][j]->scenePos();

							if (j > 0 && j < (item->curveSegments[i].size()-1))
								p0 =  (item->curveSegments[i][j+1]->scenePos() + item->curveSegments[i][j-1]->scenePos() + p0)/3.0;
							else
							{
								if (j > 0)
									p0 =  (item->curveSegments[i][j-1]->scenePos() + p0)/2.0;
								else
									if (j < (item->curveSegments[i].size()-1))
										p0 =  (item->curveSegments[i][j+1]->scenePos() + p0)/2.0;
							}

							qreal dist2 = (p0.x() - loc.x())*(p0.x() - loc.x()) +
								(p0.y() - loc.y())*(p0.y() - loc.y());
							if (dist < 0 || (dist > dist2))
							{
								dist = dist2;
								k1 = i;
								if ((j+1) < item->curveSegments[i].size())
									k2 = j;
								else
									k2 = j - 1;
							}
						}

						if (item->lineType == ConnectionGraphicsItem::line)
							j += 2;
					}

					//controlPoint->graphicsItem = item;
					while (item->curveSegments.size() <= k1) item->curveSegments.append(ConnectionGraphicsItem::CurveSegment());

					if (item->curveSegments[k1].size() < k2) k2 = item->curveSegments[k1].size();

					if (graphicsScene)
					{
						ConnectionGraphicsItem::ControlPoint * cp1, * cp2;

						if (controlPointItems.size() > k+2)
						{
							cp1 = controlPointItems[k+1];
							cp2 = controlPointItems[k+2];
						}
						else
						{
							cp1 = new ConnectionGraphicsItem::ControlPoint(item);
							cp2 = new ConnectionGraphicsItem::ControlPoint(item);
						}

						qreal dx1 = 50.0;
						//qreal dx2 = 25.0;
						int d1 = -1, d2 = 1;
						if (k2 % 3 == 0) { d1 = -2; d2 = -1; }
						if (k2 % 3 == 1) { d1 = -1; d2 = 1; }
						if (k2 % 3 == 2) { d1 = 1; d2 = 2; }

						//if (item->curveSegments[k1].size() <= k2)
						//{
						cp1->setPos( QPointF(controlPoint->pos().x()+d1*dx1, controlPoint->pos().y() ) );
						cp2->setPos( QPointF(controlPoint->pos().x()+d2*dx1, controlPoint->pos().y() ) );
						/*}
						else
						{
						qreal slopeAtCp = item->slopeAtPoint(controlPoint->pos());
						if (slopeAtCp == 0) slopeAtCp = 0.001;
						QPointF pos1 = QPointF(controlPoint->pos().x() + d1*dx2, controlPoint->pos().y() + d1*dx2*slopeAtCp),
						pos2 = QPointF(controlPoint->pos().x() + d2*dx2, controlPoint->pos().y() + d2*dx2*slopeAtCp);
						cp1->setPos( QPointF(0.4*pos1.x() + 0.6*controlPoint->pos().x(), 0.4*pos1.y() + 0.6*controlPoint->pos().y()) );
						cp2->setPos( QPointF(0.4*pos2.x() + 0.6*controlPoint->pos().x(), 0.4*pos2.y() + 0.6*controlPoint->pos().y()) );
						}*/

						graphicsScene->addItem(controlPoint);
						graphicsScene->addItem(cp1);
						graphicsScene->addItem(cp2);

						controlPoint->setZValue(item->zValue()+0.02);
						cp1->setZValue(controlPoint->zValue());
						cp2->setZValue(controlPoint->zValue());

						if (item->curveSegments[k1].size() < 4)
						{
							ConnectionGraphicsItem::ControlPoint * cp3 = new ConnectionGraphicsItem::ControlPoint(item);
							cp3->setPos( QPointF(controlPoint->pos().x(), controlPoint->pos().y()) );
							graphicsScene->addItem(cp3);
							cp3->setZValue(controlPoint->zValue());

							item->curveSegments[k1].insert(0,cp1);
							item->curveSegments[k1].insert(0,cp2);
							item->curveSegments[k1].insert(0,cp3);
							item->curveSegments[k1].insert(0,controlPoint);

							listK1 << k1 << k1 << k1;
							listK2 << 0 << 0 << 0 << 0;

							graphicsItems << cp1 << cp2 << cp3 << controlPoint;
						}
						else
						{
							listK1 << k1 << k1 << k1;
							listK2 << (k2+1) << (k2+1)<< (k2+1);

							if (k2 % 3 == 0)
							{
								item->curveSegments[k1].insert(k2+1,controlPoint);
								item->curveSegments[k1].insert(k2+1,cp2);
								item->curveSegments[k1].insert(k2+1,cp1);
								graphicsItems << controlPoint << cp2 << cp1;
							}
							else
								if (k2 % 3 == 1)
								{
									item->curveSegments[k1].insert(k2+1,cp2);
									item->curveSegments[k1].insert(k2+1,controlPoint);
									item->curveSegments[k1].insert(k2+1,cp1);
									graphicsItems << cp2 << controlPoint << cp1;
								}
								else
								{
									item->curveSegments[k1].insert(k2+1,cp2);
									item->curveSegments[k1].insert(k2+1,cp1);
									item->curveSegments[k1].insert(k2+1,controlPoint);
									graphicsItems << cp2 << cp1 << controlPoint;
								}
						}
					}
					item->setControlPointsVisible(true);
					item->refresh();
			}
			
			undone = false;
	}

	void AddControlPointCommand::undo()
	{
		for (int i=0; i < graphicsItems.size(); ++i)
		{
			if (graphicsItems[i] && graphicsItems[i]->connectionItem && graphicsScene)
			{
				ConnectionGraphicsItem * item = graphicsItems[i]->connectionItem;
				if (listK1[i] >= 0 && listK1[i] < item->curveSegments.size())
				{
					int k = item->curveSegments[ listK1[i] ].indexOf(graphicsItems[i]);
					item->curveSegments[ listK1[i] ].remove(k);
				}

				if (graphicsScene)
				{
					graphicsItems[i]->setParentItem(0);
					graphicsScene->removeItem(graphicsItems[i]);
				}

				if (graphicsItems[i]->scene())
				{
					graphicsItems[i]->setParentItem(0);
					graphicsItems[i]->scene()->removeItem(graphicsItems[i]);
				}

				item->refresh();
			}
		}
		
		undone = true;
	}

	AddControlPointCommand::~AddControlPointCommand()
	{
		if (!undone) return;

		for (int i=0; i < graphicsItems.size(); ++i)
		{
			if (graphicsItems[i])
			{
				graphicsItems[i]->setParentItem(0);
				delete graphicsItems[i];
			}
		}
	}

	RemoveControlPointCommand::RemoveControlPointCommand(
		const QString& name, GraphicsScene * scene,
		ConnectionGraphicsItem::ControlPoint* item)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems.clear();
		graphicsItems += item;
	}

	RemoveControlPointCommand::RemoveControlPointCommand(
		const QString& name, GraphicsScene * scene,
		QList<ConnectionGraphicsItem::ControlPoint*> items)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems = items;
	}

	void RemoveControlPointCommand::undo()
	{
		for (int i=0; i < graphicsItems.size(); ++i)

			if (graphicsItems[i] && graphicsItems[i]->connectionItem && graphicsScene)
			{
				ConnectionGraphicsItem * item = graphicsItems[i]->connectionItem;
				if (listK1[i] >= 0 && listK1[i] < item->curveSegments.size() && listK2[i]  >= 0 && listK2[i] < item->curveSegments[ listK1[i] ].size() )
				{
					item->curveSegments[ listK1[i] ].insert(listK2[i],graphicsItems[i]);
					graphicsScene->addItem(graphicsItems[i]);
				}
				item->refresh();
			}
	}

	void RemoveControlPointCommand::redo()
	{
		QList<ConnectionGraphicsItem::ControlPoint*> controlPointItems = graphicsItems;
		graphicsItems.clear();

		for (int i=0; i < controlPointItems.size(); ++i)

			if (controlPointItems[i] && controlPointItems[i]->connectionItem  && graphicsScene)
			{
				ConnectionGraphicsItem::ControlPoint * controlPoint = controlPointItems[i];
				ConnectionGraphicsItem * item = controlPointItems[i]->connectionItem;

				for (int i=0; i < item->curveSegments.size(); ++i)
				{
					int index = -1;
					for (int j=3; j < item->curveSegments[i].size()-3; j+=3)
					{
						if (item->curveSegments[i][j] == controlPoint ||
							item->curveSegments[i][j-1] == controlPoint ||
							item->curveSegments[i][j+1] == controlPoint)
						{

							index = j;
							break;
						}
					}
					if (index > -1)
					{
						graphicsItems << item->curveSegments[i][index-1]
						<< item->curveSegments[i][index]
						<< item->curveSegments[i][index+1];

						listK1 << i << i << i;
						listK2 << (index-1) << index << (index+1);

						item->curveSegments[i][index-1]->setParentItem(0);
						item->curveSegments[i][index]->setParentItem(0);
						item->curveSegments[i][index+1]->setParentItem(0);
						graphicsScene->removeItem(item->curveSegments[i][index-1]);
						graphicsScene->removeItem(item->curveSegments[i][index]);
						graphicsScene->removeItem(item->curveSegments[i][index+1]);

						item->curveSegments[i].remove(index-1,3);
						//item->refresh();

						break;
					}
				}
				item->refresh();
			}
	}
	
	ReplaceConnectedNodeCommand::ReplaceConnectedNodeCommand(const QString& name, ConnectionGraphicsItem * c, NodeGraphicsItem * a, NodeGraphicsItem * b)
		: QUndoCommand(name), connection(c), oldNode(a), newNode(b)
	{
	}
	void ReplaceConnectedNodeCommand::redo()
	{
		if (connection && oldNode && newNode && oldNode != newNode)
		{
			connection->replaceNode(oldNode,newNode);
			connection->refresh();
		}
	}
	
	void ReplaceConnectedNodeCommand::undo()
	{
		if (connection && oldNode && newNode && oldNode != newNode)
		{
			connection->replaceNode(newNode,oldNode);
			connection->refresh();
		}
	}
}

