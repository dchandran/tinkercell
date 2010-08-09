/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This class received signals from the ConnectionsTree class and provides the interface
for connecting items using the connections in the ConnectionsTree

****************************************************************************/

#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NetworkHandle.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "CThread.h"
#include "ConsoleWindow.h"
#include "ConnectionInsertion.h"

namespace Tinkercell
{
	
	bool ConnectionInsertion::isReactant(const QString& s)
	{
		return !(s.toLower().contains(tr("target")) || s.toLower().contains(tr("product")));
	}
	
	bool ConnectionInsertion::isReactant(NodeHandle * node)
	{
		if (!node) return false;
		for (int i=0; i < typeIn.size(); ++i)
			if (node->isA(typeIn[i]))
				return true;
		return false;
	}
	
	bool ConnectionInsertion::isProduct(NodeHandle * node)
	{
		if (!node) return false;
		for (int i=0; i < typeOut.size(); ++i)
			if (node->isA(typeOut[i]))
				return true;
		return false;
	}
	
	void ConnectionInsertion::setRequirements()
	{
		numRequiredIn = numRequiredOut = 0;
		typeOut.clear();
		typeIn.clear();

		if (selectedFamily != 0)
		{
			QList<NodeHandle*> nodes, visited;
			for (int i=0; i < selectedFamily->nodeFamilies.size() && i < selectedFamily->nodeFunctions.size(); ++i)
				if (isReactant(selectedFamily->nodeFunctions[i]))
				{
					++numRequiredIn;
					typeIn << selectedFamily->nodeFamilies[i];
				}
				else
				{
					++numRequiredOut;
					typeOut << selectedFamily->nodeFamilies[i];
				}
			for (int i=0; i < selectedConnections.size(); ++i)
			{
				nodes = NodeHandle::cast( getHandle(selectedConnections[i]->nodesAsGraphicsItems()) );
				for (int j=0; j < nodes.size(); ++j)
					if (isReactant(nodes[j]))
						--numRequiredIn;
					else
					if (isProduct(nodes[j]))
						--numRequiredOut;
			}
		}
	}

	ConnectionInsertion::ConnectionInsertion(ConnectionsTree * tree) : Tool(tr("Connection Insertion"),tr("Basic GUI")), selectedFamily(0)
	{
		QString appDir = QCoreApplication::applicationDirPath();

		ConnectionGraphicsItem::DefaultMiddleItemFile = appDir + QString("/OtherItems/simplecircle.xml");
		ConnectionGraphicsItem::DefaultArrowHeadFile = appDir + QString("/ArrowItems/Reaction.xml");

		mainWindow = 0;
		connectionsTree = tree;
		selectedFamily = 0;
		connectTCFunctions();
	}

	bool ConnectionInsertion::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSignal(const QWidget*)));

			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			connect(mainWindow,SIGNAL(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)),
					this, SLOT(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)));

			connect(mainWindow, SIGNAL(itemsAboutToBeRemoved(GraphicsScene *, QList<QGraphicsItem*>& , QList<ItemHandle*>&, QList<QUndoCommand*>& )),
					this, SLOT(itemsAboutToBeRemoved(GraphicsScene *, QList<QGraphicsItem*>& , QList<ItemHandle*>&, QList<QUndoCommand*>& )));

			connectToConnectionsTree();

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			return (connectionsTree != 0);
		}
		else
			return false;
	}

	void ConnectionInsertion::toolLoaded(Tool*)
	{
		connectToConnectionsTree();
	}

	void ConnectionInsertion::connectTCFunctions()
	{
		connect(&fToS,SIGNAL(insertConnection(QSemaphore*,ItemHandle**,const QList<ItemHandle*>&,const QString&, const QString&)),
			this,SLOT(insertConnection(QSemaphore*,ItemHandle**,const QList<ItemHandle*>&,const QString&, const QString&)));

		connect(&fToS,SIGNAL(getConnectedNodes(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)),
			this,SLOT(getConnectedNodes(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)));

		connect(&fToS,SIGNAL(getConnectedNodesIn(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)),
			this,SLOT(getConnectedNodesIn(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)));

		connect(&fToS,SIGNAL(getConnectedNodesOut(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)),
			this,SLOT(getConnectedNodesOut(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)));

		connect(&fToS,SIGNAL(getConnectedNodesOther(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)),
			this,SLOT(getConnectedNodesOther(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)));

		connect(&fToS,SIGNAL(getConnections(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)),
			this,SLOT(getConnections(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)));

		connect(&fToS,SIGNAL(getConnectionsIn(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)),
			this,SLOT(getConnectionsIn(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)));

		connect(&fToS,SIGNAL(getConnectionsOut(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)),
			this,SLOT(getConnectionsOut(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)));

		connect(&fToS,SIGNAL(getConnectionsOther(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)),
			this,SLOT(getConnectionsOther(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)));
	}

	typedef void (*tc_ConnectionInsertion_api)(
		void* (*insertConnection)(ArrayOfItems, const char*, const char*),
		ArrayOfItems (*getConnectedNodes)(void*),
		ArrayOfItems (*getConnectedNodesIn)(void*),
		ArrayOfItems (*getConnectedNodesOut)(void*),
		ArrayOfItems (*getConnectedNodesOther)(void*),
		ArrayOfItems (*getConnections)(void*),
		ArrayOfItems (*getConnectionsIn)(void*),
		ArrayOfItems (*getConnectionsOut)(void*),
		ArrayOfItems (*getConnectionsOther)(void*));


	void ConnectionInsertion::setupFunctionPointers( QLibrary * library )
	{
		tc_ConnectionInsertion_api f = (tc_ConnectionInsertion_api)library->resolve("tc_ConnectionInsertion_api");
		if (f)
		{
			f(
				&(_insertConnection),
				&(_getConnectedNodes),
				&(_getConnectedNodesIn),
				&(_getConnectedNodesOut),
				&(_getConnectedNodesOther),
				&(_getConnections),
				&(_getConnectionsIn),
				&(_getConnectionsOut),
				&(_getConnectionsOther)
				);
		}
	}

	void ConnectionInsertion::getConnectedNodes(QSemaphore* sem,QList<ItemHandle*>* list,ItemHandle* item)
	{
		if (item && list && item->type == ConnectionHandle::TYPE)
		{
			QList<NodeHandle*> nodes = (static_cast<ConnectionHandle*>(item))->nodes();
			for (int i=0; i < nodes.size(); ++i)
				(*list) += nodes[i];
		}

		if (sem)
			sem->release();
	}

	void ConnectionInsertion::getConnectedNodesIn(QSemaphore* sem,QList<ItemHandle*>* list,ItemHandle* item)
	{
		if (item && list && item->type == ConnectionHandle::TYPE)
		{
			QList<NodeHandle*> nodes = (static_cast<ConnectionHandle*>(item))->nodesIn();
			for (int i=0; i < nodes.size(); ++i)
				(*list) += nodes[i];
		}

		if (sem)
			sem->release();
	}

	void ConnectionInsertion::getConnectedNodesOut(QSemaphore* sem,QList<ItemHandle*>* list,ItemHandle* item)
	{
		if (item && list && item->type == ConnectionHandle::TYPE)
		{
			QList<NodeHandle*> nodes = (static_cast<ConnectionHandle*>(item))->nodesOut();
			for (int i=0; i < nodes.size(); ++i)
				(*list) += nodes[i];
		}
		if (sem)
			sem->release();
	}

	void ConnectionInsertion::getConnectedNodesOther(QSemaphore* sem,QList<ItemHandle*>* list,ItemHandle* item)
	{
		if (item && list && item->type == ConnectionHandle::TYPE)
		{
			QList<NodeHandle*> nodes = (static_cast<ConnectionHandle*>(item))->nodes();
			QList<NodeHandle*> nodesIn = (static_cast<ConnectionHandle*>(item))->nodesIn();
			QList<NodeHandle*> nodesOut = (static_cast<ConnectionHandle*>(item))->nodesOut();
			for (int i=0; i < nodes.size(); ++i)
				if (nodes[i] && !nodesIn.contains(nodes[i]) && !nodesOut.contains(nodes[i]) && !((*list).contains(nodes[i])))
					(*list) << nodes[i];
		}
		if (sem)
			sem->release();
	}

	void ConnectionInsertion::getConnections(QSemaphore* sem,QList<ItemHandle*>* list,ItemHandle* item)
	{
		if (item && item->type == NodeHandle::TYPE && list)
		{
			QList<ConnectionHandle*> connections = (static_cast<NodeHandle*>(item))->connections();
			for (int i=0; i < connections.size(); ++i)
				(*list) += connections[i];
		}


		if (sem)
			sem->release();
	}

	void ConnectionInsertion::getConnectionsIn(QSemaphore* sem,QList<ItemHandle*>* list,ItemHandle* item)
	{
		if (item && item->type == NodeHandle::TYPE && list)
		{
			NodeHandle * node = static_cast<NodeHandle*>(item);
			QList<ConnectionHandle*> connections = node->connections();
			for (int i=0; i < connections.size(); ++i)
				if (connections[i] && connections[i]->nodesOut().contains(node) && !(*list).contains(connections[i]))
					(*list) << connections[i];
		}
		if (sem)
			sem->release();
	}

	void ConnectionInsertion::getConnectionsOut(QSemaphore* sem,QList<ItemHandle*>* list,ItemHandle* item)
	{
		if (item && item->type == NodeHandle::TYPE && list)
		{
			NodeHandle * node = static_cast<NodeHandle*>(item);
			QList<ConnectionHandle*> connections = node->connections();
			for (int i=0; i < connections.size(); ++i)
				if (connections[i] && connections[i]->nodesIn().contains(node) && !(*list).contains(connections[i]))
					(*list) << connections[i];
		}
		if (sem)
			sem->release();
	}

	void ConnectionInsertion::getConnectionsOther(QSemaphore* sem,QList<ItemHandle*>* list,ItemHandle* item)
	{
		if (item && item->type == NodeHandle::TYPE && list)
		{
			NodeHandle * node = static_cast<NodeHandle*>(item);
			QList<ConnectionHandle*> connections = node->connections();
			for (int i=0; i < connections.size(); ++i)
				if (connections[i] &&
					!connections[i]->nodesIn().contains(node) &&
					!connections[i]->nodesOut().contains(node) &&
					!(*list).contains(connections[i]))
					(*list) << connections[i];
		}

		if (sem)
			sem->release();
	}
	
	void ConnectionInsertion::itemsAboutToBeRemoved(GraphicsScene * , QList<QGraphicsItem*>& items, QList<ItemHandle*>& handles, QList<QUndoCommand*>& commands)
	{
		NodeGraphicsItem * nodeItem;
		QList<ConnectionGraphicsItem*> tempList1;
		QList<NodeGraphicsItem*> tempList2;
		QHash<ConnectionGraphicsItem*,int> toBeRemoved;
		
		ItemHandle * connectionHandle, * nodeHandle;
		ConnectionGraphicsItem * connectionItem;
		ConnectionGraphicsItem::ControlPoint * controlPoint;
		ConnectionFamily * family;
		ItemFamily * nodeFamily;

		TextDataTable * oldTable, * newTable;
		QList<TextDataTable*> oldTables, newTables;
		int k;
		
		for (int i=0; i < items.size(); ++i)
		{
			if (nodeItem = NodeGraphicsItem::cast(items[i]))
			{
				tempList1 = nodeItem->connections();
				for (int j=0; j < tempList1.size(); ++j)
					if (tempList1[j] && (k = tempList1[j]->indexOf(nodeItem)) > -1)
						toBeRemoved.insertMulti(tempList1[j],k);
			}
			
			if ((connectionItem = ConnectionGraphicsItem::cast(items[i])) &&
				(connectionHandle = connectionItem->handle()) &&
				(ConnectionGraphicsItem::cast(connectionHandle->graphicsItems)).size() > 1)
			{
				tempList2 = connectionItem->nodes();
				for (int j=0; j < tempList2.size(); ++j)
					if (tempList2[j] && (k = connectionItem->indexOf(tempList2[j])) > -1)
						toBeRemoved.insertMulti(connectionItem,k);
			}
			
			if ((controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(items[i])) &&
				controlPoint->connectionItem)
			{
				k = controlPoint->connectionItem->indexOf(controlPoint);
				if (controlPoint->connectionItem->curveSegments[k].size() < 5)
					toBeRemoved.insertMulti(controlPoint->connectionItem,k);
			}
		}

		QList<ConnectionGraphicsItem*> keys = toBeRemoved.keys();
		QList<int> indices;
		
		for (int i=0; i < keys.size(); ++i)
			if (connectionHandle = keys[i]->handle())
			{
				oldTable = &(connectionHandle->textDataTable(tr("Participants")));
				newTable = new TextDataTable(*oldTable);
				
				QStringList removeRowNames;
				indices = toBeRemoved.values(keys[i]);

				for (int j=0; j < indices.size(); ++j)
				{
					nodeHandle = getHandle(keys[i]->nodeAt(indices[j]));
					if (nodeHandle)
						removeRowNames << nodeHandle->fullName();
				}
				
				if (!removeRowNames.isEmpty())
				{
					for (int j=0; j < removeRowNames.size(); ++j)
						newTable->removeRow(removeRowNames[j]);
					oldTables << oldTable;
					newTables << newTable;
				}
			}

		if (newTables.size() > 0)
		{
			commands << new ChangeTextDataCommand(tr("Add node roles"),oldTables,newTables);
			for (int i=0; i < newTables.size(); ++i)
				delete newTables[i];
		}
	}
	
	//adds the Participants data table
	void ConnectionInsertion::itemsAboutToBeInserted (GraphicsScene* scene, QList<QGraphicsItem *>& , QList<ItemHandle*>& handles, QList<QUndoCommand*>& commands)
	{
		ConnectionHandle * connection;
		ConnectionFamily * family;
		ItemFamily * nodeFamily;

		TextDataTable * oldTable, * newTable;
		QList<TextDataTable*> oldTables, newTables;
		QList<NodeHandle*> nodesIn, nodes;
		
		for (int i=0; i < handles.size(); ++i)
			if ((connection = ConnectionHandle::cast(handles[i])) && 
				(family = ConnectionFamily::cast(connection->family())))
			{
				oldTable = &(connection->textDataTable(tr("Participants")));
				newTable = new TextDataTable(*oldTable);
				
				nodesIn = connection->nodesIn();
				nodes = connection->nodes();
				
				bool in;
				QStringList nodeFunctions = family->nodeFunctions,
							nodeFamilies = family->nodeFamilies,
							oldRowNames = oldTable->getRowNames();
				
				for (int j=0; j < nodes.size(); ++j) //for each node
					if (nodes[j] &&
						!oldRowNames.contains(nodes[j]->fullName()) &&
						(nodeFamily = nodes[j]->family()))
					{
						in = nodesIn.contains(nodes[j]);
						//look for suitable role for this node
						for (int k=0; k < nodeFunctions.size() && k < nodeFamilies.size(); ++k)
							if (!nodeFunctions[k].isEmpty() && 
								nodeFamily->isA(nodeFamilies[k]) &&
								(!in || (in && isReactant(nodeFunctions[k]))) //if in-node, then must be reactant
								)
							{
								newTable->value(nodes[j]->fullName(),0) = nodeFunctions[k];
								nodeFunctions[k] = tr("");
								break;
							}
					}

				oldTables << oldTable;
				newTables << newTable;
			}
		if (newTables.size() > 0)
		{
			commands << new ChangeTextDataCommand(tr("Add node roles"),oldTables,newTables);
			for (int i=0; i < newTables.size(); ++i)
				delete newTables[i];
		}
	}

	void ConnectionInsertion::insertConnection(QSemaphore* sem,ItemHandle** retitem,const QList<ItemHandle*>& items,const QString& name, const QString& family)
	{
		if (!mainWindow || !connectionsTree)
		{
			if (retitem)
				(*retitem) = 0;
			if (sem)
				sem->release();
			return;
		}
		
		GraphicsScene * scene = mainWindow->currentScene();

		if (!scene || !scene->network)
		{
			if (console())
				console()->error(tr("Cannot insert without a scene!"));
			if (retitem)
				(*retitem) = 0;
			if (sem)
				sem->release();
			return;
		}

		if (!connectionsTree->connectionFamilies.contains(family) || !connectionsTree->connectionFamilies.value(family))
		{
			if (console())
				console()->error(family + tr(" not recognized"));
			if (retitem)
				(*retitem) = 0;
			if (sem)
				sem->release();
			return;
		}

		selectedFamily = connectionsTree->connectionFamilies.value(family);
		
		QList<NodeHandle*> nodes;
		NodeHandle * h1;
		ConnectionHandle * h2;
		for (int i=0; i < items.size(); ++i)
			if (h1 = NodeHandle::cast(items[i]))
				nodes << h1;
			else
			if (h2 = ConnectionHandle::cast(items[i]))
				nodes << h2->nodes();
		
		QList<ItemFamily*> subFamilies = selectedFamily->findValidChildFamilies(nodes);
		selectedFamily = 0;
		if (!subFamilies.isEmpty())
			selectedFamily = ConnectionFamily::cast(subFamilies.last());
		
		if (!selectedFamily)
		{
			if (console())
				console()->error(tr("Given set of nodes are not appropriate for a ") + family);
			if (retitem)
				(*retitem) = 0;
			if (sem)
				sem->release();
			return;
		}
		
		QList<NodeGraphicsItem*> saveSelectedNodes = selectedNodes;
		QList<ConnectionGraphicsItem*> saveSelectedConnections = selectedConnections;
		selectedNodes.clear();
		selectedConnections.clear();
		
		ItemHandle * handle;

		ConnectionGraphicsItem * connection;
		NodeGraphicsItem * node;

		for (int i=0; i < items.size(); ++i)
			if (items[i])
			{
				for (int j=0; j < items[i]->graphicsItems.size(); ++j)
					if (items[i]->graphicsItems[j])
					{
						if ((connection = ConnectionGraphicsItem::topLevelConnectionItem(items[i]->graphicsItems[j])))
						{
							selectedConnections += connection;
							break;
						}
						if ((node = NodeGraphicsItem::topLevelNodeItem(items[i]->graphicsItems[j])))
						{
							selectedNodes += node;
							break;
						}
					}
			}
		
		setRequirements();
		
		QString appDir = QCoreApplication::applicationDirPath();

		ConnectionGraphicsItem * item = new ConnectionGraphicsItem;

		//making new connections
		handle = new ConnectionHandle(selectedFamily,item);
		if (retitem)
			(*retitem) = handle;

		QList<QGraphicsItem*> insertList;
		insertList += item;

		QPointF center;

		for (int i=0; i < selectedNodes.size(); ++i)
		{
			center += selectedNodes[i]->scenePos();
			item->curveSegments +=
				ConnectionGraphicsItem::CurveSegment(1,new ConnectionGraphicsItem::ControlPoint(item,selectedNodes[i]));

			if (i >= numRequiredIn)
			{
				ArrowHeadItem * arrow = 0;
				if (!selectedFamily->graphicsItems.isEmpty() &&
					(arrow = qgraphicsitem_cast<ArrowHeadItem*>(selectedFamily->graphicsItems.last())) &&
					arrow->isValid())
				{
					arrow = new ArrowHeadItem(*arrow);
					arrow->connectionItem = item;
					if (arrow->defaultSize.width() > 0 && arrow->defaultSize.height() > 0)
						arrow->scale(arrow->defaultSize.width()/arrow->sceneBoundingRect().width(),arrow->defaultSize.height()/arrow->sceneBoundingRect().height());
				}
				else
				{
					QString nodeImageFile = appDir + tr("/ArrowItems/Reaction.xml");
					NodeGraphicsReader imageReader;
					arrow = new ArrowHeadItem(item);
					imageReader.readXml(arrow,nodeImageFile);
					arrow->normalize();
					if (arrow->defaultSize.width() > 0 && arrow->defaultSize.height() > 0)
						arrow->scale(arrow->defaultSize.width()/arrow->sceneBoundingRect().width(),arrow->defaultSize.height()/arrow->sceneBoundingRect().height());
				}
				item->curveSegments.last().arrowStart = arrow;
				insertList += arrow;
			}
		}

		if (selectedNodes.size() > 0)
			center /= selectedNodes.size();

		handle->name = name;
		handle->name = scene->network->makeUnique(name);

		TextGraphicsItem * nameItem = new TextGraphicsItem(handle,0);
		insertList += nameItem;
		nameItem->setPos(center);
		QFont font = nameItem->font();
		font.setPointSize(22);
		nameItem->setFont(font);

		scene->insert(handle->name + tr(" inserted"), insertList);
		
		selectedNodes = saveSelectedNodes;
		selectedConnections = saveSelectedConnections;
		selectedFamily = 0;

		if (sem)
			sem->release();
	}

	void ConnectionInsertion::connectionSelected(ConnectionFamily * connectionFamily)
	{
		if ((selectedFamily ||
			(mainWindow && mainWindow->currentScene()->useDefaultBehavior))
			&& connectionFamily && connectionsTree)
		{
			selectedFamily = connectionFamily;
			setRequirements();

			while (connectionFamily != 0 && connectionFamily->pixmap.isNull())
				connectionFamily = static_cast<ConnectionFamily*>(connectionFamily->parent());

			if (mainWindow->currentScene())
			{
				mainWindow->currentScene()->useDefaultBehavior = false;
				mainWindow->currentScene()->clearSelection();
			}
		}
	}	

	bool ConnectionInsertion::changeSelectedFamilyToMatchSelection(bool all)
	{
		if (!(selectedFamily && connectionsTree)) return false;
		
		QList<NodeHandle*> nodeHandles;
		QList<NodeGraphicsItem*> nodeItems;
		NodeHandle * h;
		
		for (int i=0; i < selectedNodes.size(); ++i)
			if (h = NodeHandle::cast(selectedNodes[i]->handle()))
				nodeHandles << h;
		
		for (int i=0; i < selectedConnections.size(); ++i)
		{
			nodeItems = selectedConnections[i]->nodes();
			for (int j=0; j < nodeItems.size(); ++j)
				if (h = NodeHandle::cast(nodeItems[j]->handle()))
					nodeHandles << h;
		}

		QList<ItemFamily*> childFamilies = selectedFamily->findValidChildFamilies(nodeHandles,all);
		
		/*if (childFamilies.isEmpty()) //search all families under root
		{
			ConnectionFamily * root = ConnectionFamily::cast(selectedFamily->root());
			if (root) 
			{	
				childFamilies = root->findValidChildFamilies(nodeHandles,all);
			}
		}*/

		if (childFamilies.isEmpty()) return false; //no suitable connection family found
		selectedFamily = ConnectionFamily::cast(childFamilies[0]);
		
		setRequirements();
		return true;
	}

	void ConnectionInsertion::sceneClicked(GraphicsScene *scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers )
	{
		if (mainWindow && scene && selectedFamily)
		{
			if (button == Qt::LeftButton)
			{
				revertColors();

				NodeGraphicsItem* node = 0;
				ConnectionGraphicsItem* connection = 0;
				
				QList<QGraphicsItem*> items = scene->items(QRectF(point.rx()-10.0,point.ry()-10.0,20.0,20.0));
				for (int i=0; i < items.size(); ++i)
					if (connection = ConnectionGraphicsItem::cast(items[i]))
						break;
				
				if (!connection)
				{
					QGraphicsItem * item = scene->itemAt(point);
	
					if (item)
					{
						node = NodeGraphicsItem::cast(item);
						if (!node)
							connection = ConnectionGraphicsItem::cast(item);
					}

					if (!node && !connection)
					{
						items = scene->items(QRectF(point.rx()-10.0,point.ry()-10.0,20.0,20.0));
						for (int i=0; i < items.size(); ++i)
							if (node = NodeGraphicsItem::cast(items[i]))
								break;
					}
					else
					{
						items << item;
					}
				}

				bool selected = false;

				if (node)
				{
					ItemHandle * handle = getHandle(node);
					if (handle && handle->family())
					{
						selectedNodes.push_back(node);
						if (!changeSelectedFamilyToMatchSelection())
						{
							selectedNodes.pop_back();
						}
						else
						{
							scene->selected() += node;
							selected = true;
						}
					}
				}

				if (!selected && connection)
				{
					ItemHandle * handle2 = getHandle(connection);
					if (handle2 && handle2->family())
					{
						selectedConnections.push_back(connection);
						if (!changeSelectedFamilyToMatchSelection())
						{
							selectedConnections.pop_back();
						}
						else
						{
							scene->selected() += connection;
							selected = true;
						}
					}
				}

				if (!selected)
				{
					QString messageString = tr("Invalid selection: ") + 
						selectedFamily->name + tr(" consists of ") +
						selectedFamily->nodeFamilies.join(tr(", "));
					mainWindow->statusBar()->showMessage(messageString);
					if (console())
                        console()->message(messageString);
				}

				QString appDir = QCoreApplication::applicationDirPath();
				//check if enough items have been selected to make the connection
				if (selectedNodes.size() >= (numRequiredIn + numRequiredOut) && changeSelectedFamilyToMatchSelection(true))
				{
					scene->selected().clear();
					mainWindow->statusBar()->clearMessage();
					ConnectionHandle * handle = 0;
					QList<QGraphicsItem*> insertList;

					for (int j=0; j < selectedConnections.size(); ++j)
						if (selectedConnections[j])
						{
							if (!selectedConnections[j]->centerRegionItem)
							{
								NodeGraphicsReader imageReader;
								ArrowHeadItem * node = new ArrowHeadItem(selectedConnections[j]);
								imageReader.readXml(node,ConnectionGraphicsItem::DefaultMiddleItemFile);
								if (node->isValid())
								{
									node->normalize();
									
									if (node->defaultSize.width() > 0 && node->defaultSize.height() > 0)
										node->scale(node->defaultSize.width()/node->sceneBoundingRect().width(),node->defaultSize.height()/node->sceneBoundingRect().height());

									selectedConnections[j]->centerRegionItem = node;
									selectedConnections[j]->refresh();
								}
							}

							if (selectedConnections[j]->centerRegionItem && selectedConnections[j]->handle() &&
								selectedConnections[j]->handle()->type == ConnectionHandle::TYPE)
							{
								insertList += selectedConnections[j]->centerRegionItem;
								selectedNodes += selectedConnections[j]->centerRegionItem;
								if (!handle)
								{
									handle = static_cast<ConnectionHandle*>(selectedConnections[j]->handle());
									if (handle->family() != selectedFamily)
										scene->network->setHandleFamily(handle,selectedFamily);
								}
							}
						}
					ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
					if (item == 0 || selectedNodes.size() < 2)
						return;

					//making new connections

					insertList += item;

					QPointF center;

					for (int i=0; i < selectedNodes.size(); ++i)
					{
						center += selectedNodes[i]->scenePos();

						item->curveSegments +=
							ConnectionGraphicsItem::CurveSegment(1,new ConnectionGraphicsItem::ControlPoint(item,selectedNodes[i]));
					}

					if (selectedNodes.size() > 0)
						center /= selectedNodes.size();

					if (!handle)
					{
						handle = new ConnectionHandle(selectedFamily,item);

						handle->name = tr("J1");
						/*ItemHandle * h = 0;
						for (int j=0; j < selectedNodes.size(); ++j)
							if (h = getHandle(selectedNodes[j]))
								handle->name += h->name + tr("_");*/

						handle->name = scene->network->makeUnique(handle->name);

						TextGraphicsItem * nameItem = new TextGraphicsItem(handle,0);
						insertList += nameItem;
						nameItem->setPos(center);
						QFont font = nameItem->font();
						font.setPointSize(22);
						nameItem->setFont(font);
					}
					else
					{
						setHandle(item,handle);
						item->defaultPen.setStyle(Qt::DashLine); //assuming modifier
						item->setPen(item->defaultPen);
					}

					ArrowHeadItem temparrow;
					for (int i=numRequiredIn; i < item->curveSegments.size(); ++i)
					{
						item->curveSegments[i].arrowStart = &temparrow;
					}

					for (int i=numRequiredIn; i < item->curveSegments.size(); ++i)
					{
						ArrowHeadItem * arrow = 0;
						if (!selectedFamily->graphicsItems.isEmpty() &&
							(arrow = qgraphicsitem_cast<ArrowHeadItem*>(handle->family()->graphicsItems.last())) &&
							arrow->isValid())
						{
							arrow = new ArrowHeadItem(*arrow);
							arrow->connectionItem = item;
							if (arrow->defaultSize.width() > 0 && arrow->defaultSize.height() > 0)
								arrow->scale(arrow->defaultSize.width()/arrow->sceneBoundingRect().width(),arrow->defaultSize.height()/arrow->sceneBoundingRect().height());
						}
						else
						{
							QString nodeImageFile = appDir + tr("/ArrowItems/Reaction.xml");
							NodeGraphicsReader imageReader;
							arrow = new ArrowHeadItem(item);
							imageReader.readXml(arrow,nodeImageFile);
							arrow->normalize();
							if (arrow->defaultSize.width() > 0 && arrow->defaultSize.height() > 0)
								arrow->scale(arrow->defaultSize.width()/arrow->sceneBoundingRect().width(),arrow->defaultSize.height()/arrow->sceneBoundingRect().height());
						}
						item->curveSegments[i].arrowStart = arrow;
						insertList += arrow;
					}
					
					if (handle->family()->name.contains(tr("Gene")) || handle->family()->name.contains(tr("Transcription")))
					{
						item->lineType = ConnectionGraphicsItem::line;					
						if (handle->family()->name.contains(tr("Repression")))
							item->defaultPen.setColor(QColor(tr("#C30000")));
						if (handle->family()->name.contains(tr("Activation")))
							item->defaultPen.setColor(QColor(tr("#049102")));
					}
					
					scene->insert(handle->name + tr(" inserted"), insertList);

					selectedNodes.clear();
					selectedConnections.clear();
				}
				else
				{
					if (selectedNodes.size() > 0 || selectedConnections.size() > 0)
						setSelectColor();
				}
			}
		}
	}

	void ConnectionInsertion::clear(bool arrows)
	{
		selectedFamily = 0;
		revertColors();
		selectedConnections.clear();
		selectedNodes.clear();
		if (arrows)
		{
			if (mainWindow->currentScene())
				mainWindow->currentScene()->useDefaultBehavior = true;
		}
	}

	void ConnectionInsertion::sceneRightClick(GraphicsScene *, QGraphicsItem*, QPointF, Qt::KeyboardModifiers)
	{
		if (selectedFamily)
			clear();
	}

	void ConnectionInsertion::escapeSignal(const QWidget * )
	{
		if (selectedFamily)
			clear();
	}

	void ConnectionInsertion::setSelectColor()
	{
		QColor selectionColor1(100,10,10);
		QColor selectionColor2(255,0,0);
		for (int i=0; i < selectedNodes.size(); ++i)

			if (selectedNodes[i] != 0)
			{
				selectedNodes[i]->setBoundingBoxVisible(true,true);

				for (int j=0; j < selectedNodes[i]->shapes.size(); ++j)
				{
					NodeGraphicsItem::Shape * shape = selectedNodes[i]->shapes[j];
					if (shape != 0)
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
								selectionColor2.setAlphaF(stops[1].second.alphaF());
								gradient->setColorAt(0,selectionColor1);
								gradient->setColorAt(1,selectionColor2);
								shape->setBrush(*gradient);
							}

							delete gradient;
						}
						else
						{
							selectionColor2.setAlphaF(shape->defaultBrush.color().alphaF());
							shape->setBrush(QBrush(selectionColor2));
						}
					}
				}
			}
			for (int i=0; i < selectedConnections.size(); ++i)
				if (selectedConnections[i] != 0)
				{
					selectedConnections[i]->setPen( QPen( QColor(255,0,0,255),selectedConnections[i]->pen().width() ) );
				}

	}

	void ConnectionInsertion::revertColors()
	{
		for (int j=0; j < selectedNodes.size(); ++j)

			if (selectedNodes[j] != 0)
			{
				selectedNodes[j]->setBoundingBoxVisible(false);
				for (int i=0; i < selectedNodes[j]->shapes.size(); ++i)
				{
					NodeGraphicsItem::Shape * shape = selectedNodes[j]->shapes[i];
					if (shape != 0)
					{
						shape->setPen(shape->defaultPen);
						shape->setBrush(shape->defaultBrush);
					}
				}
			}

	}

	void ConnectionInsertion::connectToConnectionsTree()
	{
		if (connectionsTree || !mainWindow) return;

		if (mainWindow->tool(tr("Connections Tree")))
		{
			QWidget * treeWidget = mainWindow->tool(tr("Connections Tree"));
			connectionsTree = static_cast<ConnectionsTree *> (treeWidget);
			if (connectionsTree != 0)
			{
				connect(connectionsTree,SIGNAL(connectionSelected(ConnectionFamily*)),
					this,SLOT(connectionSelected(ConnectionFamily*)));

				connect(mainWindow,SIGNAL(mousePressed(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
					this,SLOT(sceneClicked(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));
			}
		}
	}

	/***************************************************/

	ConnectionInsertion_FToS ConnectionInsertion::fToS;

	void* ConnectionInsertion::_insertConnection(ArrayOfItems A, const char* a0, const char* a1)
	{
		return fToS.insertConnection(A, a0, a1);
	}

	void* ConnectionInsertion_FToS::insertConnection(ArrayOfItems A, const char* a0, const char* a1)
	{
		QSemaphore * s = new QSemaphore(1);
		ItemHandle * item = 0;
		s->acquire();
		QList<ItemHandle*> * list = ConvertValue(A);
		emit insertConnection(s,&item,*list,ConvertValue(a0),ConvertValue(a1));
		s->acquire();
		s->release();
		delete s;
		delete list;
		return ConvertValue(item);
	}

	ArrayOfItems ConnectionInsertion::_getConnectedNodes(void* x)
	{
		return fToS.getConnectedNodes(x);
	}

	ArrayOfItems ConnectionInsertion_FToS::getConnectedNodes(void* x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnectedNodes(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		ArrayOfItems A = ConvertValue(*list);
		delete list;
		return A;
	}

	ArrayOfItems ConnectionInsertion::_getConnectedNodesIn(void* x)
	{
		return fToS.getConnectedNodesIn(x);
	}

	ArrayOfItems ConnectionInsertion_FToS::getConnectedNodesIn(void* x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnectedNodesIn(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		ArrayOfItems A = ConvertValue(*list);
		delete list;
		return A;
	}

	ArrayOfItems ConnectionInsertion::_getConnectedNodesOut(void* x)
	{
		return fToS.getConnectedNodesOut(x);
	}

	ArrayOfItems ConnectionInsertion_FToS::getConnectedNodesOut(void* x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnectedNodesOut(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		ArrayOfItems A = ConvertValue(*list);
		delete list;
		return A;
	}

	ArrayOfItems ConnectionInsertion::_getConnectedNodesOther(void* x)
	{
		return fToS.getConnectedNodesOther(x);
	}

	ArrayOfItems ConnectionInsertion_FToS::getConnectedNodesOther(void* x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnectedNodesOther(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		ArrayOfItems A = ConvertValue(*list);
		delete list;
		return A;
	}

	ArrayOfItems ConnectionInsertion::_getConnections(void* x)
	{
		return fToS.getConnections(x);
	}

	ArrayOfItems ConnectionInsertion_FToS::getConnections(void* x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnections(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		ArrayOfItems A = ConvertValue(*list);
		delete list;
		return A;
	}

	ArrayOfItems ConnectionInsertion::_getConnectionsIn(void* x)
	{
		return fToS.getConnectionsIn(x);
	}

	ArrayOfItems ConnectionInsertion_FToS::getConnectionsIn(void* x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnectionsIn(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		ArrayOfItems A = ConvertValue(*list);
		delete list;
		return A;
	}

	ArrayOfItems ConnectionInsertion::_getConnectionsOut(void* x)
	{
		return fToS.getConnectionsOut(x);
	}

	ArrayOfItems ConnectionInsertion_FToS::getConnectionsOut(void* x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnectionsOut(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		ArrayOfItems A = ConvertValue(*list);
		delete list;
		return A;
	}

	ArrayOfItems ConnectionInsertion::_getConnectionsOther(void* x)
	{
		return fToS.getConnectionsOther(x);
	}

	ArrayOfItems ConnectionInsertion_FToS::getConnectionsOther(void* x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnectionsOther(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		ArrayOfItems A = ConvertValue(*list);
		delete list;
		return A;
	}

}

