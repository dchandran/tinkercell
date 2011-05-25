/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This class received signals from the ConnectionsTree class and provides the interface
for connecting items using the connections in the ConnectionsTree

****************************************************************************/
#include <math.h>
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
	
	QStringList ConnectionInsertion::excludeList;
	
	bool ConnectionInsertion::isReactant(const QString& s)
	{
		return !(s.toLower().contains(tr("target")) || s.toLower().contains(tr("product")) || s.toLower().contains(tr("output")));
	}
	
	bool ConnectionInsertion::isReactant(NodeHandle * node)
	{
		if (!node || !node->family()) return false;
		for (int i=0; i < typeIn.size(); ++i)
			if (node->family()->isA(typeIn[i]) || node->family()->isParentOf(typeIn[i]))
				return true;
		return false;
	}
	
	bool ConnectionInsertion::isProduct(NodeHandle * node)
	{
		if (!node || !node->family()) return false;
		for (int i=0; i < typeOut.size(); ++i)
			if (node->family()->isA(typeOut[i]) || node->family()->isParentOf(typeOut[i]))
				return true;
		return false;
	}
	
	bool ConnectionInsertion::setRequirements(bool adjustList)
	{
		numRequiredIn = numRequiredOut = 0;
		typeOut.clear();
		typeIn.clear();
		bool inOrder = false;

		if (selectedFamily != 0)
		{
			QList<NodeHandle*> nodes, visited;
			QStringList nodeRoles = selectedFamily->participantRoles(),
						nodeFamilies = selectedFamily->participantTypes();
			for (int i=0; i < nodeFamilies.size() && i < nodeRoles.size(); ++i)
				if (isReactant(nodeRoles[i]))
				{
					++numRequiredIn;
					typeIn << nodeFamilies[i];
				}
				else
				{
					++numRequiredOut;
					typeOut << nodeFamilies[i];
				}
			
			for (int i=0; i < selectedConnections.size(); ++i)
			{
				ConnectionHandle * h = ConnectionHandle::cast(selectedConnections[i]->handle());
				for (int j=0; j < h->graphicsItems.size(); ++j)
				{
					ConnectionGraphicsItem * c;
					if (c = ConnectionGraphicsItem::cast(h->graphicsItems[j]))
					{
						nodes += NodeHandle::cast( getHandle(c->nodesAsGraphicsItems()) );
					}
				}
				for (int j=0; j < nodes.size(); ++j)
					if (isReactant(nodes[j]) && numRequiredIn > 0)
						--numRequiredIn;
					else
					if (isProduct(nodes[j]) && numRequiredOut > 0)
						--numRequiredOut;
			}
			
			NodeHandle * h;
			QList<NodeGraphicsItem*> reactants, products, both;
			inOrder = true;
			
			bool isR, isP;
			
			for (int i=0; i < selectedNodes.size(); ++i)
			{
				h = NodeHandle::cast(selectedNodes[i]->handle());
				if (!h) continue;
				
				isR = isReactant(h);
				isP = isProduct(h);
				
				if (isR && !isP && i >= numRequiredIn)	
					inOrder = false;

				if (isR && isP)
					both << selectedNodes[i];
				else
				if (isR)
					reactants << selectedNodes[i];
				else
				if (isP)
					products << selectedNodes[i];
			}
			
			if (adjustList)
			{
				selectedNodes.clear();
				selectedNodes << reactants;
				for (int i=0; i < both.size(); ++i)
					if (selectedNodes.size() < numRequiredIn)
						selectedNodes.push_front(both[i]);
					else
						selectedNodes.push_back(both[i]);
				selectedNodes << products;
			}
		}
		return inOrder;
	}

	ConnectionInsertion::ConnectionInsertion(ConnectionsTree * tree) : Tool(tr("Connection Insertion"),tr("Basic GUI")), selectedFamily(0)
	{
		ConnectionInsertion::fToS = new ConnectionInsertion_FToS;
		ConnectionInsertion::fToS->setParent(this);
		
		QString appDir = QCoreApplication::applicationDirPath();

		ConnectionGraphicsItem::DefaultMiddleItemFile = QString(":/images/Rect.xml");
		ConnectionGraphicsItem::DefaultArrowHeadFile = appDir + QString("/Graphics/Bio1/Arrows/Biochemical.xml");

		nodeInsertionTool = 0;
		mainWindow = 0;
		nodesTree = 0;
		connectionsTree = tree;
		catalogWidget = 0;
		selectedFamily = 0;
		excludeList << tr("1 to 1")  << tr("1 to 2")  << tr("1 to 3")
						  << tr("2 to 1") << tr("2 to 2") << tr("2 to 3")
						  << tr("3 to 1") << tr("3 to 2") << tr("3 to 3")
						  << tr( "1 to 1") << tr("regulation") << tr("activation") << tr("repression");
		

		connectTCFunctions();
		pickFamilyDialog = new QDialog(this);
		pickFamilyDialogLayout = 0;
		pickFamilyDialog->setWindowTitle(tr("Select process"));
	}

	bool ConnectionInsertion::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			connect(this,SIGNAL(aboutToInsertItems( GraphicsScene* , QList<QGraphicsItem *>& , QList<ItemHandle*>& , QList<QUndoCommand*>&)),
						 mainWindow,SIGNAL(itemsAboutToBeInserted( GraphicsScene* , QList<QGraphicsItem *>& , QList<ItemHandle*>& , QList<QUndoCommand*>&)));
			
			connect(this,SIGNAL(insertedItems( GraphicsScene* ,  const QList<QGraphicsItem *>& , const QList<ItemHandle*>& )),
						 mainWindow,SIGNAL(itemsInserted( GraphicsScene* ,  const QList<QGraphicsItem *>& , const QList<ItemHandle*>& )));
						 
			connect(this,SIGNAL(handleFamilyChanged(NetworkHandle * , const QList<ItemHandle*>& , const QList<ItemFamily*>& )),
						 mainWindow,SIGNAL(handleFamilyChanged(NetworkHandle * , const QList<ItemHandle*>& , const QList<ItemFamily*>& )));
			
			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSignal(const QWidget*)));

			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			connect(mainWindow,SIGNAL(mousePressed(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
					this,SLOT(sceneClicked(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(itemsDropped(GraphicsScene *, const QString&, const QPointF&)),
				this, SLOT(itemsDropped(GraphicsScene *, const QString&, const QPointF&)));

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
		connect(fToS,SIGNAL(insertConnection(QSemaphore*,ItemHandle**,const QList<ItemHandle*>&,const QString&, const QString&)),
			this,SLOT(insertConnection(QSemaphore*,ItemHandle**,const QList<ItemHandle*>&,const QString&, const QString&)));

		connect(fToS,SIGNAL(getConnectedNodes(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)),
			this,SLOT(getConnectedNodes(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)));

		connect(fToS,SIGNAL(getConnectedNodesWithRole(QSemaphore*,QList<ItemHandle*>*,ItemHandle*,const QString&)),
			this,SLOT(getConnectedNodesWithRole(QSemaphore*,QList<ItemHandle*>*,ItemHandle*,const QString&)));

		connect(fToS,SIGNAL(getConnections(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)),
			this,SLOT(getConnections(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)));

		connect(fToS,SIGNAL(getConnectionsWithRole(QSemaphore*,QList<ItemHandle*>*,ItemHandle*,const QString&)),
			this,SLOT(getConnectionsWithRole(QSemaphore*,QList<ItemHandle*>*,ItemHandle*,const QString&)));
	}

	typedef void (*tc_ConnectionInsertion_api)(
		long (*insertConnection)(tc_items, const char*, const char*),
		tc_items (*getConnectedNodes)(long),
		tc_items (*getConnectedNodesWithRole)(long,const char*),
		tc_items (*getConnections)(long),
		tc_items (*getConnectionsWithRole)(long,const char*));


	void ConnectionInsertion::setupFunctionPointers( QLibrary * library )
	{
		tc_ConnectionInsertion_api f = (tc_ConnectionInsertion_api)library->resolve("tc_ConnectionInsertion_api");
		if (f)
		{
			f(
				&(_insertConnection),
				&(_getConnectedNodes),
				&(_getConnectedNodesWithRole),
				&(_getConnections),
				&(_getConnectionsWithRole)
				);
		}
	}

	void ConnectionInsertion::getConnectedNodes(QSemaphore* sem,QList<ItemHandle*>* list,ItemHandle* item)
	{
		if (mainWindow->isValidHandlePointer(item) && list)
		{
			if (item->type == ConnectionHandle::TYPE)
			{
				QList<NodeHandle*> nodes = (static_cast<ConnectionHandle*>(item))->nodes();
				for (int i=0; i < nodes.size(); ++i)
					(*list) += nodes[i];
			}
			else
			if (item->type == NodeHandle::TYPE)
			{
				QList<ConnectionHandle*> connections = (static_cast<NodeHandle*>(item))->connections();
				QList<ItemHandle*> & lst = (*list);
				for (int j=0; j < connections.size(); ++j)
				{
					QList<NodeHandle*> nodes = connections[j]->nodes();
					for (int i=0; i < nodes.size(); ++i)
						if (!lst.contains(nodes[i]) && nodes[i] != item)
							lst += nodes[i];
				}
			}
		}

		if (sem)
			sem->release();
	}

	void ConnectionInsertion::getConnectedNodesWithRole(QSemaphore* sem,QList<ItemHandle*>* list,ItemHandle* item, const QString & role)
	{
		ConnectionHandle * connectionHandle = 0;
		NodeHandle * nodeHandle = 0;
		if (mainWindow->isValidHandlePointer(item) && list)
		{
			if ((connectionHandle = ConnectionHandle::cast(item)) && connectionHandle->hasTextData(tr("Participants")))
			{
				TextDataTable & table = connectionHandle->textDataTable(tr("Participants"));
				QList<NodeHandle*> nodes = connectionHandle->nodes();

				for (int i=0; i < nodes.size(); ++i)
					if (nodes[i])
						for (int j=0; j < table.rows(); ++j)
							if (table.rowName(j).contains(role) && table.at(j,0) == nodes[i]->fullName())
							{
								(*list) += nodes[i];
								break;
							}
			}	
			else
			if (nodeHandle = NodeHandle::cast(item))
			{
				QList<ConnectionHandle*> connections = nodeHandle->connections();
				QList<ItemHandle*> & lst = (*list);
				for (int j=0; j < connections.size(); ++j)
					if (connections[j]->hasTextData(tr("Participants")))
					{
						QList<NodeHandle*> nodes = connections[j]->nodes();
						TextDataTable & table = connections[j]->textDataTable(tr("Participants"));

						for (int i=0; i < nodes.size(); ++i)
							if (nodes[i] != nodeHandle && nodes[i] && !lst.contains(nodes[i]))
								for (int j=0; j < table.rows(); ++j)
									if (table.rowName(j).contains(role) && table.at(j,0) == nodes[i]->fullName())
									{
										lst += nodes[i];
										break;
									}
					}
			}
		}
		if (sem)
			sem->release();
	}

	void ConnectionInsertion::getConnections(QSemaphore* sem,QList<ItemHandle*>* list,ItemHandle* item)
	{
		if (mainWindow->isValidHandlePointer(item) && item->type == NodeHandle::TYPE && list)
		{
			QList<ConnectionHandle*> connections = (static_cast<NodeHandle*>(item))->connections();
			for (int i=0; i < connections.size(); ++i)
				(*list) += connections[i];
		}

		if (sem)
			sem->release();
	}

	void ConnectionInsertion::getConnectionsWithRole(QSemaphore* sem,QList<ItemHandle*>* list,ItemHandle* item, const QString & role)
	{
		NodeHandle * node;
		if (mainWindow->isValidHandlePointer(item) && (node = NodeHandle::cast(item)) && list)
		{
			QList<ConnectionHandle*> connections = node->connections();
			for (int i=0; i < connections.size(); ++i)
				if (connections[i] && connections[i]->hasTextData(tr("Participants")) && !(*list).contains(connections[i]))
				{
					TextDataTable & table = connections[i]->textDataTable(tr("Participants"));
					for (int j=0; j < table.rows(); ++j)
						if (table.rowName(j).contains(role) && table.at(j,0) == node->fullName())
						{
							(*list) << connections[i];
							break;
						}
				}
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
					{
						for (int k=0; k < newTable->rows(); ++k)
							if (newTable->at(k,0) == nodeHandle->fullName())
								removeRowNames << newTable->rowName(k);
					}
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
				if (connection->hasTextData(tr("Participants")))
				{
					oldTable = &(connection->textDataTable(tr("Participants")));
					newTable = new TextDataTable(*oldTable);
				}
				else
				{
					oldTable = newTable = &(connection->textDataTable(tr("Participants")));
				}
				
				nodesIn = connection->nodesIn();
				nodes = connection->nodes();
				
				bool in;
				QStringList nodeRoles = family->participantRoles(),
							nodeFamilies = family->participantTypes(),
							oldRowNames;
				
				for (int j=0; j < oldTable->rows(); ++j)
					oldRowNames << oldTable->value(j,0);
							
				for (int j=0; j < nodes.size(); ++j) //for each node
					if (nodes[j] &&
						!oldRowNames.contains(nodes[j]->fullName()) &&
						(nodeFamily = nodes[j]->family()))
					{
						in = nodesIn.contains(nodes[j]) && (j < nodesIn.size());
						//look for suitable role for this node
						for (int k=0; k < nodeRoles.size() && k < nodeFamilies.size(); ++k)
							if (!nodeRoles[k].isEmpty() && 
								nodeFamily->isA(nodeFamilies[k]) &&
								(!in || (in && isReactant(nodeRoles[k]))) //if in-node, then must be reactant
								)
							{
								newTable->value(nodeRoles[k],0) = nodes[j]->fullName();
								nodeRoles[k] = tr("");
								break;
							}
					}
					
				oldRowNames.clear();

				for (int j=0; j < oldTable->rows(); ++j)
				{
					int k = nodeRoles.indexOf(oldTable->rowName(j));
					if (k > -1)
					{
						nodeRoles[k] = tr("");
						oldRowNames += oldTable->value(j,0);
					}
				}
				
				for (int j=0; j < nodes.size(); ++j) //for each node
					if (nodes[j] &&
						!oldRowNames.contains(nodes[j]->fullName()) &&
						(nodeFamily = nodes[j]->family()))
					{
						in = nodesIn.contains(nodes[j]) && (j < nodesIn.size());
						//look for suitable role for this node
						for (int k=0; k < nodeRoles.size() && k < nodeFamilies.size(); ++k)
							if (!nodeRoles[k].isEmpty() && 
								nodeFamily->isA(nodeFamilies[k]) &&
								(!in || (in && isReactant(nodeRoles[k]))) //if in-node, then must be reactant
								)
							{
								newTable->value(nodeRoles[k],0) = nodes[j]->fullName();
								nodeRoles[k] = tr("");
								break;
							}
					}
				
				if (oldTable && oldTable != newTable)
				{
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

		if (!connectionsTree->getFamily(family))
		{
			if (console())
				console()->error(family + tr(" not recognized"));
			if (retitem)
				(*retitem) = 0;
			if (sem)
				sem->release();
			return;
		}

		selectedFamily = connectionsTree->getFamily(family);
		
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
						if ((connection = ConnectionGraphicsItem::topLevelConnectionItem(items[i]->graphicsItems[j])) &&
							!selectedConnections.contains(connection))
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
		
		QList<QGraphicsItem*> insertList;
		ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
		insertList += item;
		
		//center decorator
		/*if (selectedFamily->graphicsItems.size() > 1 && selectedFamily->graphicsItems.last())
		{
			NodeGraphicsItem * node = NodeGraphicsItem::cast(selectedFamily->graphicsItems.last());
			if (node && ArrowHeadItem::cast(node))
			{
				item->centerRegionItem = new ArrowHeadItem(*ArrowHeadItem::cast(node));
				if (node->defaultSize.width() > 0 && node->defaultSize.height() > 0)
					item->centerRegionItem->scale(node->defaultSize.width()/node->sceneBoundingRect().width(),node->defaultSize.height()/node->sceneBoundingRect().height());
				insertList += item->centerRegionItem;
			}
		}*/

		//making new connections
		handle = new ConnectionHandle(selectedFamily,item);
		if (retitem)
			(*retitem) = handle;

		QPointF center;

		for (int i=0; i < selectedNodes.size(); ++i)
		{
			center += selectedNodes[i]->scenePos();
			item->curveSegments +=
				ConnectionGraphicsItem::CurveSegment(1,new ConnectionGraphicsItem::ControlPoint(item,selectedNodes[i]));

			if (i >= numRequiredIn)
			{
				ArrowHeadItem * arrow0 = 0, * arrow = 0;
				if (!selectedFamily->graphicsItems.isEmpty() &&
					(arrow0 = qgraphicsitem_cast<ArrowHeadItem*>(selectedFamily->graphicsItems.first())) &&
					arrow0->isValid())
				{
					arrow = new ArrowHeadItem(*arrow0);
					arrow->connectionItem = item;
					if (arrow->defaultSize.width() > 0 && arrow->defaultSize.height() > 0)
						arrow->scale(arrow->defaultSize.width()/arrow->sceneBoundingRect().width(),arrow->defaultSize.height()/arrow->sceneBoundingRect().height());
				}
				else
				{
					arrow = new ArrowHeadItem(ConnectionGraphicsItem::DefaultArrowHeadFile, item);
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
		if (!handle->name[0].isLetter())
			handle->name = tr("J") + handle->name;
		handle->name = scene->network->makeUnique(name);

		TextGraphicsItem * nameItem = new TextGraphicsItem(handle,0);
		if (item)
			nameItem->relativePosition = QPair<QGraphicsItem*,QPointF>(item,QPointF(0,0));
		insertList += nameItem;
		nameItem->setPos(center);
		QFont font = nameItem->font();
		font.setPointSize(22);
		nameItem->setFont(font);

		if (handle->family()->name().contains(tr("gene")) || handle->family()->name().contains(tr("transcription")))
			item->lineType = ConnectionGraphicsItem::line;

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
			(mainWindow && mainWindow->currentScene()->useDefaultBehavior()))
			&& connectionFamily && connectionsTree)
		{
			selectedFamily = connectionFamily;
			setRequirements(false);

			while (connectionFamily != 0 && connectionFamily->pixmap.isNull())
				connectionFamily = static_cast<ConnectionFamily*>(connectionFamily->parent());

			if (mainWindow->currentScene())
			{
				mainWindow->currentScene()->useDefaultBehavior(false);
				mainWindow->currentScene()->clearSelection();
			}
		}
	}

	void ConnectionInsertion::itemsDropped(GraphicsScene * scene, const QString& family, const QPointF& point)
	{
		if (mainWindow && scene && scene->useDefaultBehavior() && !selectedFamily && !family.isEmpty() && 
			connectionsTree && connectionsTree->getFamily(family))
		{
			selectedFamily = connectionsTree->getFamily(family);
			setRequirements();
			sceneClicked(scene,point,Qt::LeftButton,Qt::NoModifier);
			selectedFamily = 0;
		}
	}

	QList<QGraphicsItem*> ConnectionInsertion::autoInsertNodes(GraphicsScene * scene, const QPointF & point)
	{
		QList<QGraphicsItem*> newNodes;
		if (nodesTree)
		{
			setRequirements();

			QStringList alltypes;
			alltypes << typeIn << typeOut;
	
			double dtheta = 2*3.14159 / alltypes.size();
			QPointF p;
			bool alreadyPresent;
			ItemHandle * handle;
			QList<ItemHandle*> selectedHandles;
			QList<NodeGraphicsItem*> nodeItems;

			for (int i=0; i < selectedNodes.size(); ++i)
				selectedHandles << selectedNodes[i]->handle();
			
			for (int i=0; i < selectedConnections.size(); ++i)
			{
				nodeItems = selectedConnections[i]->nodes();
				for (int j=0; j < nodeItems.size(); ++j)
					if (handle = NodeHandle::cast(nodeItems[j]->handle()))
						selectedHandles << handle;
			}
			
			NodeFamily * nodeFamily;
			NodeFamily * moleculeFamily = 0;
			if (nodesTree->getFamily(tr("Molecule")))
				moleculeFamily = nodesTree->getFamily(tr("Molecule"));

			QStringList usedNames;
			
			for (int i=0; i < alltypes.size(); ++i)
				if (nodesTree->getFamily(alltypes[i]))
				{
					nodeFamily = nodesTree->getFamily( alltypes[i] );
					alreadyPresent = false;
					for (int j=0; j < selectedHandles.size(); ++j)
						if (selectedHandles[j] && selectedHandles[j]->isA(nodeFamily))
						{
							selectedHandles[j] = 0;
							alreadyPresent = true;
							break;
						}
					
					if (!alreadyPresent)
					{
						p = point + QPointF(200.0 * cos(i * dtheta), 200.0 * sin(i * dtheta));
						if (moleculeFamily && nodeFamily->isA(tr("Node")) && !nodeFamily->parent())
							nodeFamily = moleculeFamily;
						newNodes << nodeInsertionTool->createNewNode(scene, p,tr(""),nodeFamily, usedNames);
						for (int j=0; j < newNodes.size(); ++j)
							if (handle = getHandle(newNodes[j]))
								usedNames << handle->fullName();
					}
				}
		}
		return newNodes;
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
						if (!pickFamily(false,false))
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
						if (!pickFamily(false,false))
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

				QList<QGraphicsItem*> insertList;
				bool allowFlips = false;

				if (!selected)
				{
					if (node || connection)
					{
						console()->error(tr("For ") + selectedFamily->name() + tr(", please select one of each: ") + selectedFamily->participantTypes().join(tr(",")));
					}
					else
						if (pickFamily(false,true))
						{
							insertList = autoInsertNodes(scene,point);
							for (int i=0; i < insertList.size(); ++i)
								if (node = NodeGraphicsItem::cast(insertList[i]))
								{
									selectedNodes << node;
								}
						}
				}

				QString appDir = QCoreApplication::applicationDirPath();
				bool valid = pickFamily(true,true);
				//check if enough items have been selected to make the connection
				if (selectedNodes.size() > 0 && 
					selectedNodes.size() >= (numRequiredIn + numRequiredOut) && 
					valid)
				{
					scene->selected().clear();
					mainWindow->statusBar()->clearMessage();
					ConnectionHandle * handle = 0;
					bool createdCenterItem = false;

					for (int j=0; j < selectedConnections.size(); ++j)
						if (selectedConnections[j])
						{
							if (!selectedConnections[j]->centerRegionItem)
							{
								ArrowHeadItem * node;
								if (selectedFamily->graphicsItems.size() > 1 && selectedFamily->graphicsItems.last())
								{
									NodeGraphicsItem * node0 = NodeGraphicsItem::cast(selectedFamily->graphicsItems.last());
									if (node0 && ArrowHeadItem::cast(node0))
										node = new ArrowHeadItem(*ArrowHeadItem::cast(node0));										
								}
								else
								{
									node = new ArrowHeadItem(ConnectionGraphicsItem::DefaultMiddleItemFile, selectedConnections[j]);
								}
								
								if (node->isValid())
								{
									if (node->defaultSize.width() > 0 && node->defaultSize.height() > 0)
										node->scale(node->defaultSize.width()/node->sceneBoundingRect().width(),node->defaultSize.height()/node->sceneBoundingRect().height());

									selectedConnections[j]->centerRegionItem = node;
									selectedConnections[j]->refresh();
								}
							}

							if (selectedConnections[j]->centerRegionItem && selectedConnections[j]->handle() &&
								selectedConnections[j]->handle()->type == ConnectionHandle::TYPE)
							{
								createdCenterItem = true;
								insertList += selectedConnections[j]->centerRegionItem;
								selectedNodes += selectedConnections[j]->centerRegionItem;
								if (!handle)
								{
									handle = static_cast<ConnectionHandle*>(selectedConnections[j]->handle());
								}
							}
						}

					if (selectedNodes.size() < 2)
					{
						return;
					}

					ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
					
					//center region decorator
					/*
					if (!createdCenterItem && selectedFamily->graphicsItems.size() > 1 && selectedFamily->graphicsItems.last())
					{
						NodeGraphicsItem * node0 = NodeGraphicsItem::cast(selectedFamily->graphicsItems.last());
						if (node0 && ArrowHeadItem::cast(node0))
						{
							item->centerRegionItem = new ArrowHeadItem(*ArrowHeadItem::cast(node0));
							if (node0->defaultSize.width() > 0 && node0->defaultSize.height() > 0)
								node0->scale(node0->defaultSize.width()/node0->sceneBoundingRect().width(),node0->defaultSize.height()/node0->sceneBoundingRect().height());
							insertList += item->centerRegionItem;
						}
					}*/
					
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
						
						handle->name = selectedFamily->name().toLower() + tr("1");
						QStringList words = handle->name.split(tr(" "));
						if (words.size() > 1)
						{
							handle->name = tr("");
							if (words.first()[0].isLetter())
								handle->name += words.first().left(1);
							if (words.last()[0].isLetter())
								handle->name += words.last().left(1);
							handle->name += tr("1");
						}
						
						if (!handle->name[0].isLetter())
							handle->name = tr("J") + handle->name;

						if (handle->name.length() > 3)
							handle->name = handle->name.left( 3 ) + tr("1");
						else	
						if (handle->name.length() < 2)
							handle->name = tr("J1");

						handle->name = scene->network->makeUnique(handle->name);

						TextGraphicsItem * nameItem = new TextGraphicsItem(handle,0);
						if (item)
							nameItem->relativePosition = QPair<QGraphicsItem*,QPointF>(item,QPointF(0,0));
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

					for (int i=numRequiredIn; i < item->curveSegments.size(); ++i)
					{
						ArrowHeadItem * arrow = 0, * arrow0 = 0;
						if (!selectedFamily->graphicsItems.isEmpty() &&
							(arrow0 = qgraphicsitem_cast<ArrowHeadItem*>(selectedFamily->graphicsItems.first())) &&
							arrow0->isValid())
						{
							arrow = new ArrowHeadItem(*arrow0);
							arrow->connectionItem = item;
							if (arrow->defaultSize.width() > 0 && arrow->defaultSize.height() > 0)
								arrow->scale(arrow->defaultSize.width()/arrow->sceneBoundingRect().width(),arrow->defaultSize.height()/arrow->sceneBoundingRect().height());
						}
						else
						{
							arrow = new ArrowHeadItem(ConnectionGraphicsItem::DefaultArrowHeadFile, item);
							if (arrow->defaultSize.width() > 0 && arrow->defaultSize.height() > 0)
								arrow->scale(arrow->defaultSize.width()/arrow->sceneBoundingRect().width(),arrow->defaultSize.height()/arrow->sceneBoundingRect().height());
						}
						item->curveSegments[i].arrowStart = arrow;
						insertList += arrow;
					}
					
					if (handle->family()->name().contains(tr("gene")) || handle->family()->name().contains(tr("transcription")))
						item->lineType = ConnectionGraphicsItem::line;
					
					if (handle->isA(tr("Repression")))
						item->defaultPen.setColor(QColor(tr("#C30000")));

					if (handle->isA(tr("Activation")))
						item->defaultPen.setColor(QColor(tr("#049102")));

					item->setPen(item->defaultPen);

					QList<QUndoCommand*> commands;
					QList<ItemHandle*> handlesWithNewFamilies;
					QList<ItemFamily*> oldFamilies;
					
					if (handle->family() != selectedFamily)
					{
						handlesWithNewFamilies << handle;
						oldFamilies << handle->family();
						commands << new SetHandleFamilyCommand(tr("new family"),handle,selectedFamily);
						handle->setFamily(selectedFamily,false);
					}
					
					QStringList participantTypes = selectedFamily->participantTypes();
					for (int i=0; i < selectedNodes.size(); ++i)
					{
						bool b = false;
						ItemHandle * nodeHandle = selectedNodes[i]->handle();
						if (!nodeHandle) continue;
						
						for (int j=0; j < participantTypes.size(); ++j)
							if (nodeHandle->isA(participantTypes[j]))
							{
								b = true;
								break;
							}
						if (!b)
						{
							for (int j=0; j < participantTypes.size(); ++j)
								if (nodeHandle->family() && 
										nodeHandle->family()->isParentOf(participantTypes[j]) &&
										nodesTree->getFamily(participantTypes[j]))
								{
									handlesWithNewFamilies << nodeHandle;
									oldFamilies << nodeHandle->family();
									commands << new SetHandleFamilyCommand(tr("new family"),nodeHandle,nodesTree->getFamily(participantTypes[j]));
									nodeHandle->setFamily(nodesTree->getFamily(participantTypes[j]), false);
									break;
								}
						}
					}
					
					QList<ItemHandle*> handles = getHandle(insertList);
					
					emit aboutToInsertItems(scene, insertList, handles,commands);
					commands << new InsertGraphicsCommand(tr("new connection"), scene, insertList);
					
					if (selectedConnections.isEmpty())
						scene->network->push(new CompositeCommand(handle->name + tr(" inserted"),commands));
					else
						scene->network->push(new CompositeCommand(handle->name + tr(" modified"),commands));
					
					emit insertedItems( scene, insertList, handles);
					
					if (!oldFamilies.isEmpty())
						emit handleFamilyChanged(scene->network, handlesWithNewFamilies, oldFamilies);

					//if (catalogWidget && selectedFamily->children().isEmpty())
						//catalogWidget->showButtons(QStringList() << selectedFamily->name());
						
					selectedNodes.clear();
					selectedConnections.clear();
					mainWindow->sendEscapeSignal(this);
				}
				else
				{
					if (selectedNodes.size() >= (numRequiredIn + numRequiredOut))
						mainWindow->sendEscapeSignal(this);
					else
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
				mainWindow->currentScene()->useDefaultBehavior(true);
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
		if ((nodeInsertionTool && connectionsTree && nodesTree) || !mainWindow) return;

		if (!nodeInsertionTool && mainWindow->tool(tr("Node Insertion")))
		{
			QWidget * widget = mainWindow->tool(tr("Node Insertion"));
			nodeInsertionTool = static_cast<NodeInsertion *> (widget);
		}

		if (!connectionsTree && mainWindow->tool(tr("Connections Tree")))
		{
			QWidget * treeWidget = mainWindow->tool(tr("Connections Tree"));
			connectionsTree = static_cast<ConnectionsTree *> (treeWidget);
			if (connectionsTree != 0)
			{
				connect(connectionsTree,SIGNAL(connectionSelected(ConnectionFamily*)),
					this,SLOT(connectionSelected(ConnectionFamily*)));
			}
		}

		if (!nodesTree && mainWindow->tool(tr("Nodes Tree")))
		{
			QWidget * treeWidget = mainWindow->tool(tr("Nodes Tree"));
			nodesTree = static_cast<NodesTree*>(treeWidget);
		}
		
		if (!catalogWidget && mainWindow->tool(tr("Parts and Connections Catalog")))
		{
			QWidget * widget = mainWindow->tool(tr("Parts and Connections Catalog"));
			catalogWidget = static_cast<CatalogWidget*>(widget);
		}
	}

	/***************************************************/

	ConnectionInsertion_FToS * ConnectionInsertion::fToS = 0;

	long ConnectionInsertion::_insertConnection(tc_items A, const char* a0, const char* a1)
	{
		return fToS->insertConnection(A, a0, a1);
	}

	long ConnectionInsertion_FToS::insertConnection(tc_items A, const char* a0, const char* a1)
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

	tc_items ConnectionInsertion::_getConnectedNodes(long x)
	{
		return fToS->getConnectedNodes(x);
	}

	tc_items ConnectionInsertion_FToS::getConnectedNodes(long x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnectedNodes(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		tc_items A = ConvertValue(*list);
		delete list;
		return A;
	}

	tc_items ConnectionInsertion::_getConnectedNodesWithRole(long x, const char * s)
	{
		return fToS->getConnectedNodesWithRole(x,s);
	}

	tc_items ConnectionInsertion_FToS::getConnectedNodesWithRole(long x, const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		QString qs = ConvertValue(c);
		emit getConnectedNodesWithRole(s,list,ConvertValue(x), qs.toLower());
		s->acquire();
		s->release();
		delete s;
		tc_items A = ConvertValue(*list);
		delete list;
		return A;
	}

	tc_items ConnectionInsertion::_getConnections(long x)
	{
		return fToS->getConnections(x);
	}

	tc_items ConnectionInsertion_FToS::getConnections(long x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnections(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		tc_items A = ConvertValue(*list);
		delete list;
		return A;
	}

	tc_items ConnectionInsertion::_getConnectionsWithRole(long x, const char * c)
	{
		return fToS->getConnectionsWithRole(x,c);
	}

	tc_items ConnectionInsertion_FToS::getConnectionsWithRole(long x, const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		QString qs = ConvertValue(c);
		emit getConnectionsWithRole(s,list,ConvertValue(x),qs.toLower());
		s->acquire();
		s->release();
		delete s;
		tc_items A = ConvertValue(*list);
		delete list;
		return A;
	}

	 void ConnectionInsertion::setupPickFamilyDialog(const QList<QToolButton*>& buttons)
     {
     	if (pickFamilyDialogLayout)
     		delete pickFamilyDialogLayout;
     	
     	QWidget * widget = new QWidget;
		QHBoxLayout * layout = new QHBoxLayout;
		for (int i=0; i < buttons.size(); ++i)
			layout->addWidget(buttons[i],1,Qt::AlignCenter);
		
		widget->setLayout(layout);
		widget->setPalette(QColor(255,255,255));
		QScrollArea * scrollArea = new QScrollArea;
		scrollArea->setWidget(widget);
		
		pickFamilyDialogLayout = new QVBoxLayout;
		pickFamilyDialogLayout->setContentsMargins(0,0,0,0);
		pickFamilyDialogLayout->addWidget(new QLabel(tr("Which process did you mean?")),0);
		pickFamilyDialogLayout->addWidget(scrollArea,1);
		pickFamilyDialog->setPalette(QColor(255,255,255));
		pickFamilyDialog->setLayout(pickFamilyDialogLayout);
     }
	
	bool ConnectionInsertion::pickFamily(bool all, bool dialog)
	{
		if (!pickFamilyDialog || !selectedFamily) return false;
		
		QList<NodeHandle*> nodeHandles;
		QList<NodeGraphicsItem*> nodeItems;
		NodeHandle * h;
		
		QStringList lst;
		for (int i=0; i < selectedNodes.size(); ++i)
			if (h = NodeHandle::cast(selectedNodes[i]->handle()))
			{
				nodeHandles << h;
			}
		
		for (int i=0; i < selectedConnections.size(); ++i)
		{
			nodeItems = selectedConnections[i]->nodes();
			for (int j=0; j < nodeItems.size(); ++j)
				if (h = NodeHandle::cast(nodeItems[j]->handle()))
				{
					nodeHandles << h;
				}
		}
		
		QList<ItemFamily*> childFamilies = selectedFamily->findValidChildFamilies(nodeHandles,all);
		
		if (childFamilies.isEmpty())// || !ConnectionFamily::cast(childFamilies.first()))
		{
			return false;
		}
		
		if ((!all && !dialog) || (childFamilies.size() == 1))
		{
			ConnectionFamily * original = selectedFamily;
			selectedFamily = ConnectionFamily::cast(childFamilies.first());
			setRequirements(all);
			if (childFamilies.size() != 1)
				selectedFamily = original;
			return true;
		}

		QList<ConnectionFamily*> list, leaves;
		QList<QToolButton*> toolButtons;
		if (!childFamilies.isEmpty())
			if (nodeHandles.isEmpty())
			{
				for (int i=0; i < childFamilies.size();  ++i)
					list << ConnectionFamily::cast(childFamilies[i]);
			}
			else
			{
				ConnectionFamily * savedFamily = selectedFamily;
				for (int i=(childFamilies.size()-1); i >= 0; --i)
				{
					selectedFamily = ConnectionFamily::cast(childFamilies[i]);
					if (selectedFamily && setRequirements(false))
						list << selectedFamily;
				}
				selectedFamily = savedFamily;
			}
		
		if (list.size() > 1)
		{
			for (int i=0; i < list.size(); ++i)
				//if (list[i]->children().isEmpty())
				if (!excludeList.contains(list[i]->name()))
				{
					leaves << list[i];
					QList<ItemFamily*> parents = list[i]->parents();
					for (int j=0; j < parents.size(); ++j)
						leaves.removeAll(ConnectionFamily::cast(parents[j]));
				}
		}
		else
			if (list.size() > 0)
			{
				selectedFamily = list[0];
				setRequirements();
				return true;
			}
		
		//if (leaves.size() < 2)
			//leaves = list;

		QToolButton * button;
		for (int i=0; i < leaves.size(); ++i)
		{
			button = new QToolButton;
			button->setToolTip(leaves[i]->description);
			button->setCheckable(true);
			connect(button,SIGNAL(clicked()),pickFamilyDialog,SLOT(accept()));
			button->setIcon(QIcon(leaves[i]->pixmap));
			button->setText(leaves[i]->name());
			button->setIconSize(QSize(100,100));
			button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			toolButtons << button;
		}
		
		if (leaves.size() == 1)
		{
			selectedFamily = leaves[0];
			setRequirements();
			return true;
		}
		
		setupPickFamilyDialog(toolButtons);
		
		pickFamilyDialog->exec();
		for (int i=0; i < leaves.size() && i < toolButtons.size(); ++i)
		{
			button = toolButtons[i];
			if (button->isChecked())
			{
				 selectedFamily = leaves[i];
				 setRequirements();
				 return true;
			}
		}
		
		return false;
	}
}

