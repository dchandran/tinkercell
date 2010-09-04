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
	
	bool ConnectionInsertion::setRequirements()
	{
		numRequiredIn = numRequiredOut = 0;
		typeOut.clear();
		typeIn.clear();
		bool inOrder = false;

		if (selectedFamily != 0)
		{
			QList<NodeHandle*> nodes, visited;
			for (int i=0; i < selectedFamily->nodeFamilies.size() && i < selectedFamily->nodeFunctions.size(); ++i)
				if (isReactant(selectedFamily->nodeFunctions[i])) 
					//&& (numRequiredOut > 0 || i < (selectedFamily->nodeFunctions.size()-1)))
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
				
				if (isR && i >= numRequiredIn)	
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
			
			selectedNodes.clear();
			selectedNodes << reactants;
			for (int i=0; i < both.size(); ++i)
				if (selectedNodes.size() < numRequiredIn)
					selectedNodes.push_front(both[i]);
				else
					selectedNodes.push_back(both[i]);
			selectedNodes << products;
		}
		return inOrder;
	}

	ConnectionInsertion::ConnectionInsertion(ConnectionsTree * tree) : Tool(tr("Connection Insertion"),tr("Basic GUI")), selectedFamily(0)
	{
		QString appDir = QCoreApplication::applicationDirPath();

		ConnectionGraphicsItem::DefaultMiddleItemFile = appDir + QString("/DecoratorItems/simplecircle.xml");
		ConnectionGraphicsItem::DefaultArrowHeadFile = appDir + QString("/ArrowItems/Reaction.xml");

		nodeInsertionTool = 0;
		mainWindow = 0;
		nodesTree = 0;
		connectionsTree = tree;
		catalogWidget = 0;
		selectedFamily = 0;
		selectedFamilyOriginal = 0;
		connectTCFunctions();
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
		connect(&fToS,SIGNAL(insertConnection(QSemaphore*,ItemHandle**,const QList<ItemHandle*>&,const QString&, const QString&)),
			this,SLOT(insertConnection(QSemaphore*,ItemHandle**,const QList<ItemHandle*>&,const QString&, const QString&)));

		connect(&fToS,SIGNAL(getConnectedNodes(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)),
			this,SLOT(getConnectedNodes(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)));

		connect(&fToS,SIGNAL(getConnectedNodesWithRole(QSemaphore*,QList<ItemHandle*>*,ItemHandle*,const QString&)),
			this,SLOT(getConnectedNodesWithRole(QSemaphore*,QList<ItemHandle*>*,ItemHandle*,const QString&)));

		connect(&fToS,SIGNAL(getConnections(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)),
			this,SLOT(getConnections(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)));

		connect(&fToS,SIGNAL(getConnectionsWithRole(QSemaphore*,QList<ItemHandle*>*,ItemHandle*,const QString&)),
			this,SLOT(getConnectionsWithRole(QSemaphore*,QList<ItemHandle*>*,ItemHandle*,const QString&)));
	}

	typedef void (*tc_ConnectionInsertion_api)(
		long (*insertConnection)(ArrayOfItems, const char*, const char*),
		ArrayOfItems (*getConnectedNodes)(long),
		ArrayOfItems (*getConnectedNodesWithRole)(long,const char*),
		ArrayOfItems (*getConnections)(long),
		ArrayOfItems (*getConnectionsWithRole)(long,const char*));


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
		if (mainWindow->isValidHandlePointer(item) && list && item->type == ConnectionHandle::TYPE)
		{
			QList<NodeHandle*> nodes = (static_cast<ConnectionHandle*>(item))->nodes();
			for (int i=0; i < nodes.size(); ++i)
				(*list) += nodes[i];
		}

		if (sem)
			sem->release();
	}

	void ConnectionInsertion::getConnectedNodesWithRole(QSemaphore* sem,QList<ItemHandle*>* list,ItemHandle* item, const QString & role)
	{
		ConnectionHandle * connectionHandle = 0;
		if (mainWindow->isValidHandlePointer(item) && list && 
			(connectionHandle = ConnectionHandle::cast(item)) &&
			connectionHandle->hasTextData(tr("Participants")))
		{
			TextDataTable & table = connectionHandle->textDataTable(tr("Participants"));
			QList<NodeHandle*> nodes = connectionHandle->nodes();

			for (int i=0; i < nodes.size(); ++i)
				if (nodes[i] && table.at(nodes[i]->fullName(),0).contains(role))
					(*list) += nodes[i];
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
					if (table.at(item->fullName(),0).contains(role))
						(*list) << connections[i];
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
				QStringList nodeFunctions = family->nodeFunctions,
							nodeFamilies = family->nodeFamilies,
							oldRowNames = oldTable->getRowNames();

				for (int j=0; j < oldTable->rows(); ++j)
				{
					int k = nodeFunctions.indexOf(oldTable->value(j,0));
					if (k > -1)
						nodeFunctions[k] = tr("");
				}
				
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

		selectedFamilyOriginal = selectedFamily = connectionsTree->connectionFamilies.value(family);
		
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
		selectedFamilyOriginal = selectedFamily = 0;
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
		
		QList<QGraphicsItem*> insertList;
		ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
		insertList += item;
		
		if (selectedFamily->graphicsItems.size() > 1 && selectedFamily->graphicsItems.last())
		{
			NodeGraphicsItem * node = NodeGraphicsItem::cast(selectedFamily->graphicsItems.last());
			if (node && node->className == ArrowHeadItem::CLASSNAME)
			{
				item->centerRegionItem = new ArrowHeadItem(*static_cast<ArrowHeadItem*>(node));
				if (node->defaultSize.width() > 0 && node->defaultSize.height() > 0)
					node->scale(node->defaultSize.width()/node->sceneBoundingRect().width(),node->defaultSize.height()/node->sceneBoundingRect().height());
				insertList += item->centerRegionItem;
			}
		}

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
		insertList += nameItem;
		nameItem->setPos(center);
		QFont font = nameItem->font();
		font.setPointSize(22);
		nameItem->setFont(font);

		scene->insert(handle->name + tr(" inserted"), insertList);
		
		selectedNodes = saveSelectedNodes;
		selectedConnections = saveSelectedConnections;
		selectedFamily = 0;
		selectedFamilyOriginal = 0;

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
			selectedFamilyOriginal = connectionFamily;
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

	bool ConnectionInsertion::changeSelectedFamilyToMatchSelection(bool all, bool allowFlips)
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
		
		if (childFamilies.isEmpty() && selectedFamilyOriginal) //search all families under root
		{
			childFamilies = selectedFamilyOriginal->findValidChildFamilies(nodeHandles,all);
		}

		if (childFamilies.isEmpty()) return false; //no suitable connection family found
		QString s;
		if (!all)
		{
			selectedFamily = ConnectionFamily::cast(childFamilies.last());
		}
		else
		{
			if (allowFlips)
			{
				selectedFamily = ConnectionFamily::cast(childFamilies.last());
				setRequirements();
			}
			else
			{
				ConnectionFamily * finalSelectedFamily = 0;
				QList<NodeGraphicsItem*> originalNodesList = selectedNodes;
				for (int i=0; i < childFamilies.size(); ++i)
				{
					selectedFamily = ConnectionFamily::cast(childFamilies[i]);
					selectedNodes = originalNodesList;
					if (setRequirements())
					{
						finalSelectedFamily = selectedFamily;
					}
				}

				if (finalSelectedFamily && selectedFamily != finalSelectedFamily)
				{
					selectedFamily = finalSelectedFamily;
					setRequirements();
				}
			}
		}

		return true;
	}

	void ConnectionInsertion::itemsDropped(GraphicsScene * scene, const QString& family, const QPointF& point)
	{
		if (mainWindow && scene && scene->useDefaultBehavior && !selectedFamily && !family.isEmpty() && 
			connectionsTree && connectionsTree->connectionFamilies.contains(family))
		{
			selectedFamilyOriginal = selectedFamily = connectionsTree->connectionFamilies[family];
			setRequirements();
			sceneClicked(scene,point,Qt::LeftButton,Qt::NoModifier);
			selectedFamilyOriginal = selectedFamily = 0;
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
			QList<ItemHandle*> selectedHandles;
			for (int i=0; i < selectedNodes.size(); ++i)
				selectedHandles << selectedNodes[i]->handle();
			
			NodeFamily * nodeFamily;
			NodeFamily * moleculeFamily = 0;
			if (nodesTree->nodeFamilies.contains(tr("Molecule")))
				moleculeFamily = nodesTree->nodeFamilies[ tr("Molecule") ];

			QStringList usedNames;
			ItemHandle * handle;
			
			for (int i=0; i < alltypes.size(); ++i)
				if (nodesTree->nodeFamilies.contains(alltypes[i]))
				{
					nodeFamily = nodesTree->nodeFamilies[ alltypes[i] ];
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

				QList<QGraphicsItem*> insertList;
				bool allowFlips = false;

				if (!selected)
				{
					if (node || connection)
					{
						console()->error(tr("Please select one of each: ") + selectedFamily->nodeFamilies.join(tr(",")));
					}
					else
					{
						allowFlips = true;
						insertList = autoInsertNodes(scene,point);					
						for (int i=0; i < insertList.size(); ++i)
							if (node = NodeGraphicsItem::cast(insertList[i]))
								selectedNodes << node;
					}
				}

				QString appDir = QCoreApplication::applicationDirPath();
				
				bool valid = changeSelectedFamilyToMatchSelection(true,allowFlips);
				//check if enough items have been selected to make the connection
				if (selectedNodes.size() >= (numRequiredIn + numRequiredOut) && valid)
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
									if (node0 && node0->className == ArrowHeadItem::CLASSNAME)
										node = new ArrowHeadItem(*static_cast<ArrowHeadItem*>(node0));										
								}
								else
									node = new ArrowHeadItem(ConnectionGraphicsItem::DefaultMiddleItemFile, selectedConnections[j]);
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
						return;

					ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
		
					if (!createdCenterItem && selectedFamily->graphicsItems.size() > 1 && selectedFamily->graphicsItems.last())
					{
						NodeGraphicsItem * node0 = NodeGraphicsItem::cast(selectedFamily->graphicsItems.last());
						if (node0 && node0->className == ArrowHeadItem::CLASSNAME)
						{
							item->centerRegionItem = new ArrowHeadItem(*static_cast<ArrowHeadItem*>(node0));
							if (node0->defaultSize.width() > 0 && node0->defaultSize.height() > 0)
								node0->scale(node0->defaultSize.width()/node0->sceneBoundingRect().width(),node0->defaultSize.height()/node0->sceneBoundingRect().height());
							insertList += item->centerRegionItem;
						}
					}

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
						
						handle->name = selectedFamily->name.toLower() + tr("1");
						QStringList words = handle->name.split(tr(" "));
						if (words.size() > 1)
						{
							handle->name = tr("");
							for (int i=0; i < words.size(); ++i)
								handle->name += words[i].left(1);
							handle->name += tr("1");
						}
						
						if (handle->name.length() > 3)
							handle->name = handle->name.left( 3 ) + tr("1");
							
						if (!handle->name[0].isLetter())
							handle->name = tr("J") + handle->name;

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
						ArrowHeadItem * arrow = 0, * arrow0 = 0;
						if (!selectedFamily->graphicsItems.isEmpty() &&
							(arrow0 = qgraphicsitem_cast<ArrowHeadItem*>(handle->family()->graphicsItems.first())) &&
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
					
					if (handle->family()->name.contains(tr("Gene")) || handle->family()->name.contains(tr("Transcription")))
					{
						item->lineType = ConnectionGraphicsItem::line;					
						if (handle->family()->name.contains(tr("Repression")))
							item->defaultPen.setColor(QColor(tr("#C30000")));
						if (handle->family()->name.contains(tr("Activation")))
							item->defaultPen.setColor(QColor(tr("#049102")));
					}

					QList<QUndoCommand*> commands;
					QList<ItemFamily*> oldFamilies;
					
					if (handle->family() != selectedFamily)
					{
						oldFamilies << handle->family();
						commands << new SetHandleFamilyCommand(tr("new family"),handle,selectedFamily);
						handle->setFamily(selectedFamily,false);
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
						emit handleFamilyChanged(scene->network, QList<ItemHandle*>() << handle, oldFamilies);

					if (catalogWidget && selectedFamily->children().isEmpty())
						catalogWidget->showButtons(QStringList() << selectedFamily->name);

					selectedNodes.clear();
					selectedConnections.clear();
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
		selectedFamilyOriginal = 0;
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

	ConnectionInsertion_FToS ConnectionInsertion::fToS;

	long ConnectionInsertion::_insertConnection(ArrayOfItems A, const char* a0, const char* a1)
	{
		return fToS.insertConnection(A, a0, a1);
	}

	int ConnectionInsertion_FToS::insertConnection(ArrayOfItems A, const char* a0, const char* a1)
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

	ArrayOfItems ConnectionInsertion::_getConnectedNodes(long x)
	{
		return fToS.getConnectedNodes(x);
	}

	ArrayOfItems ConnectionInsertion_FToS::getConnectedNodes(long x)
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

	ArrayOfItems ConnectionInsertion::_getConnectedNodesWithRole(long x, const char * s)
	{
		return fToS.getConnectedNodesWithRole(x,s);
	}

	ArrayOfItems ConnectionInsertion_FToS::getConnectedNodesWithRole(long x, const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		QString qs = ConvertValue(c);
		emit getConnectedNodesWithRole(s,list,ConvertValue(x), qs);
		s->acquire();
		s->release();
		delete s;
		ArrayOfItems A = ConvertValue(*list);
		delete list;
		return A;
	}

	ArrayOfItems ConnectionInsertion::_getConnections(long x)
	{
		return fToS.getConnections(x);
	}

	ArrayOfItems ConnectionInsertion_FToS::getConnections(long x)
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

	ArrayOfItems ConnectionInsertion::_getConnectionsWithRole(long x, const char * c)
	{
		return fToS.getConnectionsWithRole(x,c);
	}

	ArrayOfItems ConnectionInsertion_FToS::getConnectionsWithRole(long x, const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		QString qs = ConvertValue(c);
		emit getConnectionsWithRole(s,list,ConvertValue(x),qs);
		s->acquire();
		s->release();
		delete s;
		ArrayOfItems A = ConvertValue(*list);
		delete list;
		return A;
	}

}

