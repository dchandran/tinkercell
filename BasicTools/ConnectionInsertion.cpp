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
#include "NetworkWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "CThread.h"
#include "ConsoleWindow.h"
#include "ConnectionInsertion.h"

namespace Tinkercell
{

	void ConnectionInsertion::initializeHashes()
	{
		defaultInputs.insert(tr("Connection"),1); defaultOutputs.insert(tr("Connection"),1);
		defaultInputs.insert(tr("Biochemical"),1); defaultOutputs.insert(tr("Biochemical"),1);
	}

	void ConnectionInsertion::setRequirements()
	{
		numRequiredIn = numRequiredOut = 0;
		typeOut = typeIn = tr("");

		if (selectedFamily != 0)
		{
			/*if (defaultInputs.contains(selectedFamily->name) && defaultOutputs.contains(selectedFamily->name))
			{
				numRequiredIn = defaultInputs.value(selectedFamily->name);
				numRequiredOut = defaultOutputs.value(selectedFamily->name);
			}
			else
			{*/
			if (selectedFamily->numericalAttributes.contains(tr("numin")) && selectedFamily->numericalAttributes.contains(tr("numout")))
			{
				numRequiredIn = (int)selectedFamily->numericalAttributes.value("numin");
				numRequiredOut = (int)selectedFamily->numericalAttributes.value("numout");

				if (numRequiredIn < 1)
					numRequiredIn = 1;

				if (numRequiredOut < 1)
					numRequiredOut = 1;
			}


			if (selectedFamily->textAttributes.contains(tr("typein")) && selectedFamily->textAttributes.contains(tr("typeout")))
			{
				typeIn = selectedFamily->textAttributes[tr("typein")];
				typeOut = selectedFamily->textAttributes[tr("typeout")];
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
		initializeHashes();
		connectTCFunctions();
	}

	bool ConnectionInsertion::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSignal(const QWidget*)));

			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			//connect(mainWindow,SIGNAL(sceneRightClick(GraphicsScene *, QGraphicsItem*, QPointF, Qt::KeyboardModifiers)),
			//	this, SLOT(sceneRightClick(GraphicsScene *, QGraphicsItem*, QPointF, Qt::KeyboardModifiers)));

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
		connect(&fToS,SIGNAL(insertConnection(QSemaphore*,ItemHandle**,const QList<ItemHandle*>&,const QList<ItemHandle*>&,const QString&, const QString&)),
			this,SLOT(insertConnection(QSemaphore*,ItemHandle**,const QList<ItemHandle*>&,const QList<ItemHandle*>&,const QString&, const QString&)));

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
		OBJ (*insertConnection)(Array, Array, const char*, const char*),
		Array (*getConnectedNodes)(OBJ),
		Array (*getConnectedNodesIn)(OBJ),
		Array (*getConnectedNodesOut)(OBJ),
		Array (*getConnectedNodesOther)(OBJ),
		Array (*getConnections)(OBJ),
		Array (*getConnectionsIn)(OBJ),
		Array (*getConnectionsOut)(OBJ),
		Array (*getConnectionsOther)(OBJ));


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

	void ConnectionInsertion::insertConnection(QSemaphore* sem,ItemHandle** retitem,const QList<ItemHandle*>& in,const QList<ItemHandle*>& out,const QString& name, const QString& family)
	{
		if (!mainWindow || !connectionsTree)
		{
			if (retitem)
				(*retitem) = 0;
			if (sem)
				sem->release();
			return;
		}

		if (!connectionsTree->connectionFamilies.contains(family) || !connectionsTree->connectionFamilies.value(family))
		{
			if (retitem)
				(*retitem) = 0;
			if (sem)
				sem->release();
			return;
		}

		GraphicsScene * scene = mainWindow->currentScene();

		if (!scene)
		{
			if (retitem)
				(*retitem) = 0;
			if (sem)
				sem->release();
			return;
		}

		selectedFamily = connectionsTree->connectionFamilies.value(family);
		setRequirements();

		if (family != tr("Connection") && !(defaultInputs.contains(family) || defaultInputs.contains(family.toLower())) && (in.size() != numRequiredIn || out.size() != numRequiredOut))
		{
			if (retitem)
				(*retitem) = 0;
			if (sem)
				sem->release();
			return;
		}
		ItemHandle * handle;
		for (int i=0; i < in.size(); ++i)
		{
			handle = in[i];
			if (!handle || !handle->family() || !(handle->family()->isA(typeIn) || handle->family()->isA(tr("empty"))))
			{
				if (retitem)
					(*retitem) = 0;
				if (sem)
					sem->release();
				return;
			}
		}
		for (int i=0; i < out.size(); ++i)
		{
			handle = out[i];
			if (!handle ||!handle->family() || !(handle->family()->isA(typeOut) || handle->family()->isA(tr("empty"))))
			{
				if (retitem)
					(*retitem) = 0;
				if (sem)
					sem->release();
				return;
			}
		}
		ConnectionGraphicsItem * connection;
		NodeGraphicsItem * node;
		QList<ConnectionGraphicsItem*> selectedConnections;
		QList<NodeGraphicsItem*> selectedNodes;
		for (int i=0; i < in.size(); ++i)
		{
			if (!in[i]->graphicsItems.isEmpty())
			{
				for (int j=0; j < in[i]->graphicsItems.size(); ++j)
				{
					if ((connection = ConnectionGraphicsItem::topLevelConnectionItem(in[i]->graphicsItems[j])))
					{
						selectedConnections += connection;
						break;
					}
					if ((node = NodeGraphicsItem::topLevelNodeItem(in[i]->graphicsItems[j])))
					{
						selectedNodes += node;
						break;
					}
				}
			}
		}
		for (int i=0; i < out.size(); ++i)
		{
			if (!out[i]->graphicsItems.isEmpty() && out[i]->graphicsItems[0] != 0)
			{
				for (int j=0; j < in[i]->graphicsItems.size(); ++j)
				{
					if ((connection = ConnectionGraphicsItem::topLevelConnectionItem(out[i]->graphicsItems[j])))
					{
						selectedConnections += connection;
						break;
					}
					if ((node = NodeGraphicsItem::topLevelNodeItem(out[i]->graphicsItems[j])))
					{
						selectedNodes += node;
						break;
					}
				}
			}
		}

		QString appDir = QCoreApplication::applicationDirPath();

		if (selectedConnections.size() > 0)
		{
			if (retitem)
				(*retitem) = 0;
			if (sem)
				sem->release();
			return;
		}

		ConnectionGraphicsItem * item = familyToGraphicsItem(selectedFamily);
		if (item == 0)
		{
			if (sem)
				sem->release();
			return;
		}

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
		handle->name = findUniqueName(handle,scene->allHandles());

		TextGraphicsItem * nameItem = new TextGraphicsItem(handle,0);
		insertList += nameItem;
		nameItem->setPos(center);
		QFont font = nameItem->font();
		font.setPointSize(22);
		nameItem->setFont(font);

		scene->insert(handle->name + tr(" inserted"), insertList);
		
		selectedConnections.clear();
		selectedNodes.clear();
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
			/*
			if (connectionFamily != 0 && !connectionFamily->pixmap.isNull())
			{
				QList<NetworkWindow*> allWindows = mainWindow->allWindows();
				for (int i=0; i < allWindows.size(); ++i)
					if (allWindows[i]->scene)
						allWindows[i]->setCursor(QCursor(connectionFamily->pixmap.scaled(30,30)));
			}
			else
			{
				QList<NetworkWindow*> allWindows = mainWindow->allWindows();
				for (int i=0; i < allWindows.size(); ++i)
					if (allWindows[i]->scene)
						allWindows[i]->setCursor(Qt::ArrowCursor);
			}*/
			if (mainWindow->currentScene())
			{
				mainWindow->currentScene()->useDefaultBehavior = false;
				mainWindow->currentScene()->clearSelection();
			}
		}
	}

	QString ConnectionInsertion::findUniqueName(ItemHandle * handle1, const QList<ItemHandle*>& items)
	{
		if (!handle1) return tr("");
		int	c = 1;
		QString name = handle1->name;
		bool uniqueName = false;

		ItemHandle* handle2 = 0;

		while (!uniqueName)
		{
			uniqueName = true;

			for (int i=0; i < items.size(); ++i)
			{
				handle2 = (items[i]);
				if (handle2 && (handle1 != handle2) &&
					handle2->fullName(tr(".")) == name)
				{
					uniqueName = false;
					break;
				}
			}
			if (!uniqueName)
			{
				name =  tr("J") + QString::number(c);
				++c;
			}
		}

		return name;
	}

	bool ConnectionInsertion::changeSelectedFamilyToMatchSelection(NodeGraphicsItem * node)
	{
		if (!(selectedFamily && connectionsTree && node)) return false;

		ConnectionFamily * selectedFamily0 = selectedFamily;

		int numRequiredIn0 = numRequiredIn;
		int numRequiredOut0 = numRequiredOut;
		QString typeOut0 = typeOut;
		QString typeIn0 = typeIn;

		QList<QString> keys = connectionsTree->connectionFamilies.keys();

		QList<NodeGraphicsItem*> selectedNodes0 = selectedNodes;
		selectedNodes0 += node;

		ConnectionFamily * bestPick = 0;

		for (int i=0; i < keys.size(); ++i)
		{
			if ((selectedFamily = connectionsTree->connectionFamilies[ keys[i] ]) && selectedFamily->name.toLower() != tr("connection"))
			{
				setRequirements();
				if (numRequiredIn != numRequiredIn0 || numRequiredOut != numRequiredOut0) continue;
				for (int j=0; j < selectedNodes0.size() && selectedFamily != 0; ++j)
				{
					ItemHandle * handle = getHandle(selectedNodes0[j]);
					if (handle && handle->family())
					{
						if (!( ((j < numRequiredIn) && (handle->family()->isA(typeIn) || handle->family()->isA(tr("empty"))))
							||
							((j >= numRequiredIn) && (handle->family()->isA(typeOut) || handle->family()->isA(tr("empty"))))
							))
						{
							selectedFamily = 0;
						}
					}
				}
				if (selectedFamily != 0)
				{
					if (bestPick == 0 || (selectedFamily->children().size() > bestPick->children().size()))
						bestPick = selectedFamily;
				}
			}
		}

		selectedFamily = bestPick;

		if (!selectedFamily)
		{
			selectedFamily = selectedFamily0;
			numRequiredIn = numRequiredIn0;
			numRequiredOut = numRequiredOut0;
			typeOut = typeOut0;
			typeIn = typeIn0;
			return false;
		}
		/*
		if (!selectedFamily->pixmap.isNull())
		{
			QList<NetworkWindow*> allWindows = mainWindow->allWindows();
				for (int i=0; i < allWindows.size(); ++i)
					if (allWindows[i]->scene)
						allWindows[i]->setCursor(QCursor(selectedFamily->pixmap.scaled(30,30)));
			connectionsTree->setCursor(selectedFamily->pixmap.scaled(30,30));
		}
		else
		{
			QList<NetworkWindow*> allWindows = mainWindow->allWindows();
				for (int i=0; i < allWindows.size(); ++i)
					if (allWindows[i]->scene)
						allWindows[i]->setCursor(Qt::ArrowCursor);
			connectionsTree->setCursor(Qt::ArrowCursor);
		}*/
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
					if ((node = NodeGraphicsItem::topLevelNodeItem(items[i])) != 0)
						break;

				int totalSelected = selectedNodes.size() + selectedConnections.size();
				bool selected = false;

				if (node)
				{
					ItemHandle * handle = getHandle(node);
					if (handle && handle->family())
					{
						if ( ((totalSelected < numRequiredIn) && (handle->family()->isA(typeIn) || handle->family()->isA(tr("empty"))))
							||
							((totalSelected >= numRequiredIn) && (handle->family()->isA(typeOut) || handle->family()->isA(tr("empty"))))
							)
						{
							selectedNodes += node;
							scene->selected() += node;
							selected = true;
						}
						else
						{
							if (changeSelectedFamilyToMatchSelection(node))
							{
								selectedNodes += node;
								scene->selected() += node;
								selected = true;
							}
						}
					}
				}

				if (!selected)
				{
					for (int i=0; i < items.size(); ++i)
						if ((connection = ConnectionGraphicsItem::topLevelConnectionItem(items[i])) != 0)
							break;
					if (connection)
					{
						ItemHandle * handle2 = getHandle(connection);
						if (handle2 && handle2->family())
						{
							if ( ((totalSelected < numRequiredIn) && (handle2->family()->isA(typeIn) || handle2->family()->isA(tr("empty"))))
								||
								((totalSelected >= numRequiredOut) && (handle2->family()->isA(typeOut) || handle2->family()->isA(tr("empty"))))
								)
							{
								selectedConnections += connection;
								scene->selected() += connection;
								selected = true;
							}
						}
					}
				}

				if (!selected)
				{
					QString messageString = tr("Select ") +
						QString::number(numRequiredIn) + tr(" \"") + typeIn + tr("\" item and ") +
						QString::number(numRequiredOut) + tr(" \"") + typeOut + tr("\" item");
					mainWindow->statusBar()->showMessage(messageString);
					if (console())
                        console()->message(messageString);
				}

				QString appDir = QCoreApplication::applicationDirPath();
				//check if enough items have been selected to make the connection
				if ((selectedNodes.size() + selectedConnections.size()) >= (numRequiredIn + numRequiredOut))
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
								}
							}

							if (selectedConnections[j]->centerRegionItem && selectedConnections[j]->handle() &&
								selectedConnections[j]->handle()->type == ConnectionHandle::TYPE)
							{
								insertList += selectedConnections[j]->centerRegionItem;
								selectedNodes += selectedConnections[j]->centerRegionItem;
								if (!handle)
									handle = static_cast<ConnectionHandle*>(selectedConnections[j]->handle());
							}
						}
					ConnectionGraphicsItem * item = familyToGraphicsItem(selectedFamily);
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

					if (!handle)
					{
						handle = new ConnectionHandle(selectedFamily,item);

						handle->name = tr("J1");
						handle->name = findUniqueName(handle,scene->allHandles());

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
						item->defaultPen.setStyle(Qt::DashLine);
						item->setPen(item->defaultPen);
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

	ConnectionGraphicsItem * ConnectionInsertion::familyToGraphicsItem(ConnectionFamily * family)
	{
		return new ConnectionGraphicsItem;
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

	OBJ ConnectionInsertion::_insertConnection(Array A1, Array A2, const char* a0, const char* a1)
	{
		return fToS.insertConnection(A1, A2, a0, a1);
	}

	OBJ ConnectionInsertion_FToS::insertConnection(Array A1, Array A2, const char* a0, const char* a1)
	{
		QSemaphore * s = new QSemaphore(1);
		ItemHandle * item = 0;
		s->acquire();
		QList<ItemHandle*> * list1 = ConvertValue(A1);
		QList<ItemHandle*> * list2 = ConvertValue(A2);
		emit insertConnection(s,&item,*list1,*list2,ConvertValue(a0),ConvertValue(a1));
		s->acquire();
		s->release();
		delete s;
		delete list1;
		delete list2;
		return ConvertValue(item);
	}

	Array ConnectionInsertion::_getConnectedNodes(OBJ x)
	{
		return fToS.getConnectedNodes(x);
	}

	Array ConnectionInsertion_FToS::getConnectedNodes(OBJ x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnectedNodes(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		Array A = ConvertValue(*list);
		delete list;
		return A;
	}

	Array ConnectionInsertion::_getConnectedNodesIn(OBJ x)
	{
		return fToS.getConnectedNodesIn(x);
	}

	Array ConnectionInsertion_FToS::getConnectedNodesIn(OBJ x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnectedNodesIn(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		Array A = ConvertValue(*list);
		delete list;
		return A;
	}

	Array ConnectionInsertion::_getConnectedNodesOut(OBJ x)
	{
		return fToS.getConnectedNodesOut(x);
	}

	Array ConnectionInsertion_FToS::getConnectedNodesOut(OBJ x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnectedNodesOut(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		Array A = ConvertValue(*list);
		delete list;
		return A;
	}

	Array ConnectionInsertion::_getConnectedNodesOther(OBJ x)
	{
		return fToS.getConnectedNodesOther(x);
	}

	Array ConnectionInsertion_FToS::getConnectedNodesOther(OBJ x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnectedNodesOther(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		Array A = ConvertValue(*list);
		delete list;
		return A;
	}

	Array ConnectionInsertion::_getConnections(OBJ x)
	{
		return fToS.getConnections(x);
	}

	Array ConnectionInsertion_FToS::getConnections(OBJ x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnections(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		Array A = ConvertValue(*list);
		delete list;
		return A;
	}

	Array ConnectionInsertion::_getConnectionsIn(OBJ x)
	{
		return fToS.getConnectionsIn(x);
	}

	Array ConnectionInsertion_FToS::getConnectionsIn(OBJ x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnectionsIn(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		Array A = ConvertValue(*list);
		delete list;
		return A;
	}

	Array ConnectionInsertion::_getConnectionsOut(OBJ x)
	{
		return fToS.getConnectionsOut(x);
	}

	Array ConnectionInsertion_FToS::getConnectionsOut(OBJ x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnectionsOut(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		Array A = ConvertValue(*list);
		delete list;
		return A;
	}

	Array ConnectionInsertion::_getConnectionsOther(OBJ x)
	{
		return fToS.getConnectionsOther(x);
	}

	Array ConnectionInsertion_FToS::getConnectionsOther(OBJ x)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* list = new QList<ItemHandle*>;
		s->acquire();
		emit getConnectionsOther(s,list,ConvertValue(x));
		s->acquire();
		s->release();
		delete s;
		Array A = ConvertValue(*list);
		delete list;
		return A;
	}

}

