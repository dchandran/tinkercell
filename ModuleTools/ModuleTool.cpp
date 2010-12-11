/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This tool handles module connections that merge items from two modules

****************************************************************************/

#include <math.h>
#include <iostream>
#include <QRegExp>
#include "ItemFamily.h"
#include "NetworkHandle.h"
#include "ItemHandle.h"
#include "GraphicsScene.h"
#include "TextEditor.h"
#include "ConsoleWindow.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "LoadSaveTool.h"
#include "TreeButton.h"
#include "ModuleTool.h"

namespace Tinkercell
{
	static QString linkerFileName("/icons/moduleLinker.xml");
	static QString interfaceFileName("/icons/moduleInterface.xml");
	static QString moduleFileName("/icons/Module.xml");
	static QString linkerClassName("module linker item");
	static QString interfaceClassName("module interface item");
	static QString connectionClassName("module connection item");

    ModuleTool::ModuleTool() : Tool(tr("Module Connection Tool"),tr("Module tools")), 
    	newModuleDialog(0), newModuleTable(0), newModuleName(0), connectionsTree(0), nodesTree(0)
    {
        setPalette(QPalette(QColor(255,255,255,255)));
        setAutoFillBackground(true);
        mode = none;
        lineItem.setPen(QPen(QColor(255,10,10,255),2.0,Qt::DotLine));
    }

	//insert interface node
    void ModuleTool::select(int)
    {
    	GraphicsScene * scene = currentScene();
    	if (!scene) return;
    	
    	QList<QGraphicsItem*> & selected = scene->selected();
    	QList<QGraphicsItem*> itemsToInsert;
		ItemHandle * h = 0;
		NodeGraphicsItem * node;
		
		NodeGraphicsReader reader;
		
		QString appDir = QCoreApplication::applicationDirPath();

		for (int i=0; i < selected.size(); ++i)
			if ((h = getHandle(selected[i])) && NodeHandle::cast(h))
			{
				for (int j=0; j < h->graphicsItems.size(); ++j)
					if ((node = NodeGraphicsItem::cast(h->graphicsItems[j])) && node->className == interfaceClassName)
						continue;

				node = new NodeGraphicsItem(appDir + interfaceFileName);
				node->setPos(
					QPointF(selected[i]->sceneBoundingRect().left() - node->sceneBoundingRect().width()/2.0,selected[i]->scenePos().y())
				);
				node->className = interfaceClassName;
				node->setHandle(h);
				itemsToInsert << node;
			}

		if (!itemsToInsert.isEmpty())
		{
			if (scene->localHandle())
				scene->insert(scene->localHandle()->name + tr(" interface created"),itemsToInsert);
			else
			if (itemsToInsert.size() == 1)
				scene->insert(getHandle(itemsToInsert[0])->name + tr(" interface created"),itemsToInsert);
			else
				scene->insert(tr("Interfaces created"),itemsToInsert);
		}
    }

	bool ModuleTool::setMainWindow(MainWindow * main)
    {
		Tool::setMainWindow(main);
        if (mainWindow != 0)
        {
			connect(mainWindow,SIGNAL(itemsDropped(GraphicsScene *, const QString&, const QPointF&)),
				this, SLOT(itemsDropped(GraphicsScene *, const QString&, const QPointF&)));
        	
			connect(mainWindow,SIGNAL(mousePressed(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
					this,SLOT(sceneClicked(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)),
                    this,SLOT(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)));
            
            connect(mainWindow,SIGNAL(keyPressed(GraphicsScene*,QKeyEvent *)),
				this ,SLOT(keyPressed(GraphicsScene*,QKeyEvent *)));

			connect(mainWindow,SIGNAL(mouseMoved(GraphicsScene* , QGraphicsItem*, QPointF , Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)),
                    this,SLOT(mouseMoved(GraphicsScene* , QGraphicsItem*, QPointF , Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)));

			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),
					this,SLOT(escapeSignal(const QWidget*)));

            connect(mainWindow,SIGNAL(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)),
					this, SLOT(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)));

			connect(mainWindow,SIGNAL(itemsInserted(NetworkHandle*, const QList<ItemHandle*>&)),
                    this, SLOT(itemsInserted(NetworkHandle*, const QList<ItemHandle*>&)));

			connect(mainWindow, SIGNAL(itemsAboutToBeRemoved(GraphicsScene *, QList<QGraphicsItem*>& , QList<ItemHandle*>&, QList<QUndoCommand*>& )),
					this, SLOT(itemsAboutToBeRemoved(GraphicsScene *, QList<QGraphicsItem*>& , QList<ItemHandle*>&, QList<QUndoCommand*>& )));
			
			connect(mainWindow, SIGNAL(itemsRenamed(NetworkHandle * , const QList<ItemHandle*>& , const QList<QString>& , const QList<QString>& )),
					this, SLOT(itemsRenamed(NetworkHandle * , const QList<ItemHandle*>& , const QList<QString>& , const QList<QString>& )));

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			toolLoaded(mainWindow->tool(tr("")));
        }

        return true;
    }

	void ModuleTool::toolLoaded(Tool*)
	{
		static bool connected1 = false;
		
		if (mainWindow->tool(tr("Nodes Tree")) && !nodesTree)
		{
			Tool * tool = static_cast<Tool*>(mainWindow->tool(tr("Nodes Tree")));
			nodesTree = static_cast<NodesTree*>(tool);
		}
		
		QStringList moduleFamilyNames;
		if (mainWindow->tool(tr("Connections Tree")) && !connectionsTree)
		{
			Tool * tool = static_cast<Tool*>(mainWindow->tool(tr("Connections Tree")));
			connectionsTree = static_cast<ConnectionsTree*>(tool);
		}

		QString appDir = QCoreApplication::applicationDirPath();
		QString home = homeDir();
		
		if (connectionsTree)
		{
			ConnectionFamily * moduleFamily = connectionsTree->getFamily(tr("Module"));
			if (!moduleFamily)
			{
				moduleFamily = new ConnectionFamily(tr("Module"));
				moduleFamily->pixmap = QPixmap(tr(":/images/module.png"));
				moduleFamily->description = tr("Self-contained subsystem that can be used to build larger systems");
				moduleFamily->textAttributes[tr("Functional description")] = tr("");
				moduleFamily->graphicsItems << new ArrowHeadItem(appDir + interfaceFileName)
											 << new ArrowHeadItem(appDir + moduleFileName);				
				connectionsTree->insertFamily(moduleFamily,0);
				if (QFile::exists(home + tr("/Modules/modules.xml")))
					connectionsTree->readTreeFile(home + tr("/Modules/modules.xml"));
				else
				if (QFile::exists(appDir + tr("/Modules/modules.xml")))				
					connectionsTree->readTreeFile(appDir + tr("/Modules/modules.xml"));
			}
			
			QList<ItemFamily*> childFamilies = moduleFamily->allChildren();
			
			for (int i=0; i < childFamilies.size(); ++i)
			{
				QString s = childFamilies[i]->name();
				s.replace(tr(" "),tr(""));
				QString dirname = home + tr("/Modules/") + s;
				QDir dir(dirname);
		
				if (!dir.exists())
					dir.setPath(home + tr("/Modules/") + s.toLower());

				if (dir.exists())
					moduleFamilyNames << childFamilies[i]->name();
			}
		}

		if (mainWindow->tool(tr("Parts and Connections Catalog")) && !connected1)
		{
			Tool * tool = static_cast<Tool*>(mainWindow->tool(tr("Parts and Connections Catalog")));
			catalogWidget = static_cast<CatalogWidget*>(tool);

			connect(catalogWidget,SIGNAL(buttonPressed(const QString&)),
					this,SLOT(moduleButtonPressed(const QString&)));

			QList<QToolButton*> newButtons = catalogWidget->addNewButtons(
				tr("Modules"),
				QStringList() 	<< tr("New module"),
								//<< tr("Connect input/output"),
				QList<QIcon>() 	<< QIcon(QPixmap(tr(":/images/module.png"))),
								//<< QIcon(QPixmap(tr(":/images/merge.png"))),
				QStringList() 	<< tr("A module is a self-contained subsystem that can be used to build larger systems")
								//<< tr("Use this to connect inputs and ouputs of two modules")
				);
			
			if (!moduleFamilyNames.isEmpty())
				catalogWidget->showButtons(moduleFamilyNames);

			connected1 = true;
		}
	}

    void ModuleTool::escapeSignal(const QWidget* )
    {
		if (mode != none && currentScene())
			currentScene()->useDefaultBehavior = true;

		mode = none;
		for (int i=0; i < selectedItems.size(); ++i)
			selectedItems[i]->resetPen();
		selectedItems.clear();
		
		lineItem.setVisible(false);
        if (lineItem.scene())
            lineItem.scene()->removeItem(&lineItem);
    }

	void ModuleTool::sceneClicked(GraphicsScene *scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
	{
		if (mode == none || button == Qt::RightButton || !scene || !scene->network || scene->useDefaultBehavior) return;

		QString appDir = QApplication::applicationDirPath();

		if (mode == inserting)
		{
			/*Tool * tool = mainWindow->tool(tr("Nodes Tree"));
			if (!tool) return;

			NodesTree * nodesTree = static_cast<NodesTree*>(tool);

			if (!nodesTree->getFamily(tr("Module"))) return;

			NodeHandle * handle = new NodeHandle;
			handle->name = scene->network->makeUnique(tr("mod1"));
			NodeFamily * moduleFamily = nodesTree->getFamily(tr("Module"));
			handle->setFamily(moduleFamily);

			NodeGraphicsItem * image;
			if (moduleFamily && moduleFamily->graphicsItems.size() > 0 && NodeGraphicsItem::cast(moduleFamily->graphicsItems[0]))
			{
				image = NodeGraphicsItem::cast(moduleFamily->graphicsItems[0])->clone();
				image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),
						 image->defaultSize.height()/image->sceneBoundingRect().height());
			}
			else
			{
				image = new NodeGraphicsItem(appDir + tr("/icons/Module.xml"));
			}
			
			image->setPos(point);
			image->adjustBoundaryControlPoints();
			image->setHandle(handle);

			TextGraphicsItem * text = new TextGraphicsItem(handle);
			text->setPos(QPointF(point.x(),image->sceneBoundingRect().bottom() + 10.0));
			text->scale(2,2);

			scene->insert(handle->name + tr(" inserted"),QList<QGraphicsItem*>() << image << text);

			return;*/
		}

		QList<QGraphicsItem*> items = scene->items(point);
		QList<QGraphicsItem*> nodeItems;

		for (int i=0; i < items.size(); ++i)
			if (NodeGraphicsItem::cast(items[i]))
				nodeItems << items[i];

		if (nodeItems.size() < 1) return;

		if (mode == linking)
		{
			ItemHandle * h;
			
			if (nodeItems.size() == 1 && (h = getHandle(nodeItems[0])))
			{
				scene->selected() = nodeItems;
				select(0);
			}
			return;
		}

		if (mode == connecting && nodeItems.size() == 1)
		{
			selectedItems << NodeGraphicsItem::cast(nodeItems[0]);
			//selectedItems.last()->setAlpha(100);

			if (selectedItems.size() == 2)
			{
				ItemHandle * handle1 = selectedItems[0]->handle(),
						   * handle2 = selectedItems[1]->handle();

				if (!handle1 || !handle2 || !handle1->family() || !handle2->family())
				{
					QMessageBox::information(this, tr("Cannot connect"), tr("Cannot connect because these objects do not belong in any family"));
					return;
				}

				if (!(handle1->family()->isA(handle2->family()) || handle2->family()->isA(handle1->family())))
				{
					QMessageBox::information(this,
						tr("Cannot connect"),
						 tr("Cannot connect because ") +
						handle1->name + tr(" (") + handle1->family()->name() + tr(") and ") +
						handle2->name + tr(" (") + handle2->family()->name() + tr(") are different types of objects")	);
					return;
				}

				makeModuleConnection(selectedItems[0],selectedItems[1],scene);
				for (int i=0; i < selectedItems.size(); ++i)
					selectedItems[i]->resetPen();
				selectedItems.clear();
				
				lineItem.setVisible(false);
				if (lineItem.scene())
				    lineItem.scene()->removeItem(&lineItem);
			}
		}
	}

	QUndoCommand * ModuleTool::moduleConnectionsInserted(QList<QGraphicsItem*>& items)
	{
		QStringList from, to;
		ConnectionGraphicsItem * c;
		for (int i=0; i < items.size(); ++i)
			if ((c = ConnectionGraphicsItem::cast(items[i])) && (c->className == connectionClassName))
			{
				QList<NodeGraphicsItem*> nodes = c->nodes();
				if (nodes.size() == 2 && nodes[0] && nodes[1])
				{
					ItemHandle * h1 = nodes[0]->handle(), * h2 = nodes[1]->handle();
					if (h1 && h2 && h1 != h2)
					{
						int k = substituteFrom.indexOf(h2->fullName());
						if (k >= 0)
						{
							substituteWith << substituteFrom[k];
							to << substituteFrom[k];
							
							substituteFrom << h1->fullName();
							from << h1->fullName();
						}
						else
						{
							substituteFrom << h1->fullName();
							from << h1->fullName();
							
							substituteWith << h2->fullName();
							to << h2->fullName();
						}
					}
				}
			}
		return new RenameCommand(tr("Substitute items"),currentNetwork(),from,to);
	}

	QUndoCommand * ModuleTool::substituteStrings(const QList<ItemHandle*> & items)
	{
		return new RenameCommand(tr("Substitute items"),currentNetwork(),items,substituteFrom,substituteWith);
	}

	void ModuleTool::removeSubnetworks(QList<QGraphicsItem*>& items, QList<ItemHandle*>& handles)
	{
		ItemHandle * handle, * h;
		QList<NodeGraphicsItem*> nodes;
		ConnectionGraphicsItem * connection;
		for (int i=0; i < items.size(); ++i)
			if ((handle = getHandle(items[i])) && ConnectionFamily::cast(handle->family()))
			{
				for (int j=0; j < handle->children.size(); ++j)
					if (handle->children[j])
					{
						QList<QGraphicsItem*> childItems = handle->children[j]->allGraphicsItems();
						for (int k=0; k < childItems.size(); ++k)
						{
							items.removeAll(childItems[k]);
							handles.removeAll( getHandle(childItems[k]) );
							if (connection = ConnectionGraphicsItem::cast(childItems[k]))
							{
								nodes = connection->nodes();
								for (int l=0; l < nodes.size(); ++l)
								{
									items.removeAll(nodes[l]);
									QRectF rect = nodes[l]->sceneBoundingRect();
									rect.adjust(-10,-10,10,10);
									if (h=nodes[l]->handle())
										for (int m=0; m < h->graphicsItems.size(); ++m)
										{
											if (rect.intersects(h->graphicsItems[m]->sceneBoundingRect()))
												items.removeAll(h->graphicsItems[m]);
										}
								}
							}
						}
					}
			}
	}
	
	void ModuleTool::itemsRenamed(NetworkHandle * network, const QList<ItemHandle*>& items, const QList<QString>& oldnames, const QList<QString>& newnames)
	{
		QList<GraphicsScene*> scenes = network->scenes();
		QRegExp regex("\\.([^\\.]+)$");
		ConnectionHandle * c;
		
		for (int i=0; i < items.size(); ++i)
			if (items[i] 
				&& (c = ConnectionHandle::cast(items[i]))
				&& !items[i]->children.isEmpty() 
				&& newnames.contains(items[i]->fullName()))
			{
				int k = newnames.indexOf(items[i]->fullName());
				if (k < 0 || k > oldnames.size()) continue;
				
				QString oldname = oldnames[i], newname = newnames[i];
				
				if (regex.indexIn(oldname) && !regex.cap(1).isEmpty())
					oldname = regex.cap(1);
				if (regex.indexIn(newname) && !regex.cap(1).isEmpty())
					newname = regex.cap(1);

				QList<ItemHandle*> children = items[i]->children;
				QList<NodeHandle*> nodes = c->nodes();
				for (int j=0; j < nodes.size(); ++j)
					children << nodes[j];

				QList<QGraphicsItem*> gitems;

				for (int j=0; j < children.size(); ++j)
					gitems << children[j]->graphicsItems;

				NodeGraphicsItem * node;
				ConnectionGraphicsItem * connection;
				TextGraphicsItem * text;
				for (int j=0; j < gitems.size(); ++j)
					if ((node = NodeGraphicsItem::cast(gitems[j])) && node->groupID == oldname)
						node->groupID = newname;
					else
					if ((connection = ConnectionGraphicsItem::cast(gitems[j])) && connection->groupID == oldname)
						connection->groupID = newname;
					else
					if ((text = TextGraphicsItem::cast(gitems[j])) && text->groupID == oldname)
						text->groupID = newname;
			}
	}

	void ModuleTool::itemsAboutToBeInserted(GraphicsScene* scene, QList<QGraphicsItem *>& items, QList<ItemHandle*>& handles, QList<QUndoCommand*>& commands)
	{
		if (!scene || !scene->network) return;
		
		aboutToBeRenamed.clear();

		/*commands << moduleConnectionsInserted(items)
				 << substituteStrings(handles);
		
		removeSubnetworks(items,handles);

		ItemHandle * h1 = 0, * h2 = 0;

		ConnectionGraphicsItem * connection = 0;
		NodeGraphicsItem * node = 0;
		QList<NodeGraphicsItem*> nodes;
		QStringList oldNames, newNames;
		QGraphicsItem* newLinker, * newLinkerText;
		ItemHandle * nodeHandle;

		for (int i=0; i < items.size(); ++i)
		{
			if ((connection = ConnectionGraphicsItem::cast(items[i])) && connection->className == connectionClassName)
			{
				nodes = connection->nodes();
				if (nodes.size() == 2)
				{
					h1 = getHandle(nodes[0]);
					h2 = getHandle(nodes[1]);
					if (h1 && h2)
					{
						oldNames << h1->fullName();
						newNames << h2->fullName();
					}
				}
			}
		}*/
		
		if (scene->localHandle())
		{
			ItemHandle * parentHandle = scene->localHandle();
			NodeGraphicsItem * node;
			ConnectionGraphicsItem * connection;
			TextGraphicsItem * text;
			QString groupName = parentHandle->name;
			for (int i=0; i < items.size(); ++i)
				if (node = NodeGraphicsItem::cast(items[i]))
					node->groupID = groupName;
				else
				if (connection = ConnectionGraphicsItem::cast(items[i]))
					connection->groupID = groupName;
				else
				if (text = TextGraphicsItem::cast(items[i]))
					text->groupID = groupName;
					
			QList<ItemHandle*> allChildren = handles;
			for (int i=0; i < handles.size(); ++i)
				if (handles[i])
					allChildren << handles[i]->allChildren();

			QList<ItemHandle*> existingHandles = scene->network->handles();

			for (int i=0; i < handles.size(); ++i)
				if (handles[i] && !existingHandles.contains(handles[i]) && !handles[i]->parent)
				{
					existingHandles << handles[i];
					commands << new SetParentHandleCommand(tr("set parent"),0,handles[i],parentHandle);
					commands << new RenameCommand(tr("rename"),0,allChildren,handles[i]->name,parentHandle->fullName() + tr(".") + handles[i]->name);
				}
		}
		
		QList<ItemHandle*> visited;

		for (int i=0; i < handles.size(); ++i)
			if (handles[i] && !visited.contains(handles[i]) && ConnectionFamily::cast(handles[i]->family()))
			{
				visited << handles[i];
				QString groupName = handles[i]->name;
				QList<QGraphicsItem*> items2;
				
				if (handles[i]->children.isEmpty())
				{
					QString s = handles[i]->family()->name();
					s.replace(tr(" "),tr(""));
					QString dirname = homeDir() + tr("/Modules/") + s;
					QDir dir(dirname);
		
					if (!dir.exists())
						dir.setPath(homeDir() + tr("/Modules/") + s.toLower());
		
					if (!dir.exists())
						dir.setPath(QCoreApplication::applicationDirPath() + tr("/Modules/") + s);
		
					if (!dir.exists())
						dir.setPath(QCoreApplication::applicationDirPath() + tr("/Modules/") + s.toLower());
				
					if (dir.exists())
					{
						dir.setFilter(QDir::Files);
						dir.setSorting(QDir::Time);
						QFileInfoList list = dir.entryInfoList();
				
						if (!list.isEmpty())
						{	
							bool loaded = false;
							while (!loaded)
							{
								QString filename = list.last().absoluteFilePath();

								if (QFile::exists(filename))
								{
									QPair< QList<ItemHandle*> , QList<QGraphicsItem*> > pair = mainWindow->getItemsFromFile(filename,0);

									items2 = pair.second;
									QList<ItemHandle*> handles2 = pair.first;
									QList<ItemHandle*> allChildren = handles2;
									for (int j=0; j < handles2.size(); ++j)
										if (handles2[j])
											allChildren << handles2[j]->allChildren();

									loaded = !handles2.isEmpty();

									QList<ItemHandle*> visitedHandles;

									for (int j=0; j < handles2.size(); ++j)
										if (handles2[j] && !visitedHandles.contains(handles2[j]))
										{
											visitedHandles << handles2[j];
											if (!handles2[j]->parent || handles2[j]->parent == handles[i])
											{
												handles2[j]->setParent(0,false);
												commands << new SetParentHandleCommand(tr("set parent"),0,handles2[j],handles[i]);
												if (scene->localHandle())
													commands << new RenameCommand(tr("rename"),0,allChildren,handles2[j]->name,scene->localHandle()->fullName() + tr(".") + handles[i]->fullName() + tr(".") + handles2[j]->name);
												else
													commands << new RenameCommand(tr("rename"),0,allChildren,handles2[j]->name,handles[i]->fullName() + tr(".") + handles2[j]->name);
											}
										}
									}
								}
							}
						}
					}
					
					aboutToBeRenamed << handles[i]->name;
					
					if (!items2.isEmpty())
					{
						NodeGraphicsItem * node;
						ConnectionGraphicsItem * connection;
						TextGraphicsItem * text;

						for (int j=0; j < items2.size(); ++j)
							if (node = NodeGraphicsItem::cast(items2[j]))
							{
								if (node->groupID.isEmpty())
									node->groupID = groupName;
							}
							else
							if (connection = ConnectionGraphicsItem::cast(items2[j]))
							{
								if (connection->groupID.isEmpty())
									connection->groupID = groupName;
							}
							else
							if (text = TextGraphicsItem::cast(items2[j]))
							{
								if (text->groupID.isEmpty())
									text->groupID = groupName;
							}
					}
				}
	}

	void ModuleTool::itemsAboutToBeRemoved(GraphicsScene* scene, QList<QGraphicsItem *>& items, QList<ItemHandle*>& handles, QList<QUndoCommand*>& commands)
	{
		if (!scene || !scene->network) return;
		
		ItemHandle * h1 = 0, * h2 = 0;
		
		ConnectionGraphicsItem * connection = 0;
		QList<NodeGraphicsItem*> nodes;
		QStringList oldNames, newNames;
		
		for (int i=0; i < items.size(); ++i)
			if ((connection = ConnectionGraphicsItem::cast(items[i])) && connection->className == connectionClassName)
			{
				nodes = connection->nodes();
				if (nodes.size() == 2)
				{
					h1 = getHandle(nodes[0]);
					h2 = getHandle(nodes[1]);
					if (h1 && h2 && h1->parent && h1 != h2)
					{
						QList<ItemHandle*> moduleHandles;
						moduleHandles << h1->parent;
						moduleHandles << h1->parent->allChildren();
						commands << new RenameCommand(tr("module connection removed"),scene->network,moduleHandles,h2->fullName(),h1->fullName());
					}
				}
			}
	}
	
	ItemHandle * ModuleTool::findCorrespondingHandle(NodeHandle * node, ConnectionHandle * connection)
	{
		if (!node || !connection || !connection->hasTextData(tr("Participants")))
			return 0;

		QList<NodeHandle*> nodes = connection->nodes();
		
		if (nodes.contains(node)) return 0;
		
		TextDataTable & participants = connection->textDataTable(tr("Participants"));
		QString s;
		
		for (int i=0; i < nodes.size(); ++i)
		{
			if (participants.hasRow(node->name.toLower()))
			{
				s = participants.value(node->name.toLower(),0);
				if (nodes[i]->fullName().compare(s,Qt::CaseInsensitive) == 0)
					return nodes[i];
			}
		}
		return 0;
	}

    void ModuleTool::itemsInserted(NetworkHandle * network, const QList<ItemHandle*>& handles)
	{
		GraphicsScene * scene = network->currentScene();
		if (scene)
		{
			ConnectionGraphicsItem * c;
			ConnectionHandle * ch;
			QList<QUndoCommand*> commands;
			QList<ItemHandle*> modules;
			ItemHandle * h;
			
			for (int i=0; i < handles.size(); ++i)
				if ((ch = ConnectionHandle::cast(handles[i])) && !handles[i]->children.isEmpty())
				{
					if (handles.size() == 1)
						for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
							if ((c = ConnectionGraphicsItem::cast(handles[i]->graphicsItems[j])) && (c->scene() == scene))
							{
								scene->showToolTip(c->centerLocation(),handles[i]->name + tr(" contains a model inside"));
								break;
							}
					modules << handles[i];

					QList<QGraphicsItem*> items2;	
					
					QList<ItemHandle*> children = handles[i]->allChildren();

					children = handles[i]->children;
					QList<NodeHandle*> nodes = ch->nodes();
					for (int j=0; j < nodes.size(); ++j)
						children << nodes[j];

					for (int j=0; j < children.size(); ++j)
						items2 << children[j]->graphicsItems;
						
					QString groupName = handles[i]->name;

					if (!items2.isEmpty() && !aboutToBeRenamed.contains(groupName))
					{
						NodeGraphicsItem * node;
						ConnectionGraphicsItem * connection;
						TextGraphicsItem * text;

						for (int j=0; j < items2.size(); ++j)
							if (node = NodeGraphicsItem::cast(items2[j]))
							{
								if (node->groupID != groupName && aboutToBeRenamed.contains(node->groupID))
									node->groupID = groupName;
							}
							else
							if (connection = ConnectionGraphicsItem::cast(items2[j]))
							{
								if (connection->groupID != groupName && aboutToBeRenamed.contains(connection->groupID))
									connection->groupID = groupName;
							}
							else
							if (text = TextGraphicsItem::cast(items2[j]))
							{
								if (text->groupID != groupName && aboutToBeRenamed.contains(text->groupID))
									text->groupID = groupName;
							}
					}
				}

			for (int i=0; i < modules.size(); ++i)
				for (int j=0; j < modules[i]->children.size(); ++j)
				{
					h = findCorrespondingHandle(NodeHandle::cast(modules[i]->children[j]),ConnectionHandle::cast(modules[i]));
					if (h)
					{
						commands << new MergeHandlesCommand(
								tr("merge"), network, QList<ItemHandle*>() << h << modules[i]->children[j]);
					}
				}

			if (!commands.isEmpty())
				network->push( new CompositeCommand(tr("merged models by roles"),commands) );
	    }
    }

    void ModuleTool::mouseMoved(GraphicsScene* scene, QGraphicsItem*, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>& items)
    {
        if (mode == connecting && scene && selectedItems.size() == 1 && selectedItems[0])
        {
            if (lineItem.scene() != scene)
                scene->addItem(&lineItem);

            if (!lineItem.isVisible())
                lineItem.setVisible(true);

            lineItem.setLine(QLineF(selectedItems[0]->scenePos(),point));
            return;
        }
    }

    QList<QPointF> ModuleTool::pathAroundRect(QRectF rect1, QRectF rect2, QPointF p1, QPointF p2)
    {
        QList<QPointF> list;
        qreal y;

        if (p1.ry() > rect1.bottom() || p2.ry() > rect2.bottom())
        {
            y = p1.ry();
            if (p2.ry() > p1.ry())
                y = p2.ry();
            y += 50.0;
        }
        else
        {
            y = p1.ry();
            if (p2.ry() < y) y = p2.ry();
            if (rect1.top() < y) y = rect1.top();
            if (rect2.top() < y) y = rect2.top();
            y -= 50.0;
        }

        list << QPointF(p1.rx(),y) << QPointF(p2.rx(),y);

        return list;
    }

    void ModuleTool::makeModuleConnection(NodeGraphicsItem * link1, NodeGraphicsItem * link2,GraphicsScene * scene)
    {
        if (!link1 || !link2 || !scene) return;

        ItemHandle * handle1 = getHandle(link1);
        ItemHandle * handle2 = getHandle(link2);

        if (!handle1 || !handle2 || !handle1->family() || !handle2->family()) return;

        NodeGraphicsItem * module1 = 0;
        NodeGraphicsItem * module2 = 0;
        
        if (handle1->parent)
	        for (int i=0; i < handle1->parent->graphicsItems.size(); ++i)
	        	if (module1 = NodeGraphicsItem::cast(handle1->parent->graphicsItems[i]))
	        		break;

        if (handle2->parent)
	        for (int i=0; i < handle2->parent->graphicsItems.size(); ++i)
	        	if (module2 = NodeGraphicsItem::cast(handle2->parent->graphicsItems[i]))
	        		break;

		QRectF rect1, rect2;
		QPointF point1 = link1->scenePos(),
				point2 = link2->scenePos();

		if (module1)
			rect1 = module1->sceneBoundingRect();
		else
			rect1 = link1->sceneBoundingRect();

		if (point1.ry() >= rect1.bottom())
			point1.ry() = link1->sceneBoundingRect().bottom();
		else
			if (point1.ry() <= rect1.top())
				point1.ry() = link1->sceneBoundingRect().top();
		else
			if (point1.rx() >= rect1.right())
				point1.rx() = link1->sceneBoundingRect().right();
		else
			if (point1.rx() <= rect1.left())
				point1.rx() = link1->sceneBoundingRect().left();

		if (module2)
			rect2 = module2->sceneBoundingRect();
		else
			rect2 = link2->sceneBoundingRect();


		if (point2.ry() >= rect2.bottom())
			point2.ry() = link2->sceneBoundingRect().bottom();
		else
			if (point2.ry() <= rect2.top())
				point2.ry() = link2->sceneBoundingRect().top();
		else
			if (point2.rx() >= rect2.right())
				point2.rx() = link2->sceneBoundingRect().right();
		else
			if (point2.rx() <= rect2.left())
				point2.rx() = link2->sceneBoundingRect().left();

        QPointF midpt1(point1.rx(),point2.ry()), midpt2(point2.rx(),point1.ry());

        QList<QPointF> path = pathAroundRect(rect1.adjusted(-20,-20,20,20),rect2.adjusted(-20,-20,20,20),point1,point2);

        if (path.isEmpty())
            return;

        ConnectionGraphicsItem * connection = new ConnectionGraphicsItem;
		connection->className = connectionClassName;
		connection->setPen(connection->defaultPen = QPen(QColor(255,100,0,255),3.0));

        QList<ItemHandle*> handles;
        handles << handle1 << handle2;

        ConnectionGraphicsItem::CurveSegment controlPoints;

        controlPoints += new ConnectionGraphicsItem::ControlPoint(point1,connection,link1);
        controlPoints += new ConnectionGraphicsItem::ControlPoint((point1+path.first())/2.0,connection);
        controlPoints += new ConnectionGraphicsItem::ControlPoint((point1+path.first())/2.0,connection);
        for (int i=0; i < path.size(); ++i)
        {
            if (i>0)
            {
                controlPoints += new ConnectionGraphicsItem::ControlPoint((path[i-1]+path[i])/2.0,connection);
                controlPoints += new ConnectionGraphicsItem::ControlPoint((path[i-1]+path[i])/2.0,connection);
            }
            controlPoints += new ConnectionGraphicsItem::ControlPoint(path[i],connection);
        }
        controlPoints += new ConnectionGraphicsItem::ControlPoint((point2+path.last())/2.0,connection);
        controlPoints += new ConnectionGraphicsItem::ControlPoint((point2+path.last())/2.0,connection);
        controlPoints += new ConnectionGraphicsItem::ControlPoint(point2,connection,link2);

        connection->curveSegments += controlPoints;
        connection->lineType = ConnectionGraphicsItem::line;

		scene->insert(tr("modules connected"),connection);
    }    
	
	void ModuleTool::modelButtonClicked ( QAbstractButton * button )
	{
		if (button)
		{
			QString filename = button->toolTip();
			NetworkHandle * network = currentNetwork();
			
			if (QFile::exists(filename) && network)
			{
				NetworkWindow * window = network->currentWindow();
				
				if (!window || !window->handle || !window->handle->family()) return;
				ItemHandle * parentHandle = window->handle;
				
				QPair< QList<ItemHandle*> , QList<QGraphicsItem*> > pair = mainWindow->getItemsFromFile(filename,parentHandle);
				
				QList<QGraphicsItem*> items = pair.second;
				QList<ItemHandle*> handles = pair.first;
				
				NodeGraphicsItem * node;
				ConnectionGraphicsItem * connection;
				TextGraphicsItem * text;
				QString groupName = parentHandle->name;
				for (int i=0; i < items.size(); ++i)
					if (node = NodeGraphicsItem::cast(items[i]))
						node->groupID = groupName;
					else
					if (connection = ConnectionGraphicsItem::cast(items[i]))
						connection->groupID = groupName;
					else
					if (text = TextGraphicsItem::cast(items[i]))
						text->groupID = groupName;
				
				if (handles.isEmpty()) return;
				
				QList<ItemHandle*> visitedHandles;

				if (items.isEmpty())
				{
					for (int i=0; i < handles.size(); ++i)
						if (handles[i] && !visitedHandles.contains(handles[i]))
						{
							visitedHandles << handles[i];
							if (!handles[i]->parent)							
								items += handles[i]->graphicsItems;
						}
				}

				if (window && window->scene)
				{
					if (!items.isEmpty())
					{
						GraphicsScene * scene = window->newScene();
						scene->insert(tr("new model"),items);
						scene->fitAll();
					}
					else
					{
						QString modelText;
						emit getTextVersion(handles, &modelText);
						TextEditor * newEditor = window->newTextEditor();
						newEditor->setText(modelText);
						newEditor->insert(handles);
					}
				}
				else
				if (window && window->editor)
				{
					if (!items.isEmpty())
					{
						GraphicsScene * scene = window->newScene();
						scene->insert(tr("new model"),items);
						scene->fitAll();
					}
					else
					{
						QString modelText;
						emit getTextVersion(handles, &modelText);
						TextEditor * newEditor = window->newTextEditor();
						newEditor->setText(modelText);
						newEditor->insert(handles);
					}
				}
				
				QList<QUndoCommand*> commands;
				ItemHandle * h;

				for (int i=0; i < parentHandle->children.size(); ++i)
				{
					h = findCorrespondingHandle(NodeHandle::cast(parentHandle->children[i]),ConnectionHandle::cast(parentHandle));
					if (h)
					{
						commands << new MergeHandlesCommand(
								tr("merge"), network, QList<ItemHandle*>() << h << parentHandle->children[i]);
					}
				}
			
				if (!commands.isEmpty())
					network->push( new CompositeCommand(tr("Merged models"),commands) );
			}
		}
	}
	
	QDockWidget * ModuleTool::makeDockWidget(const QStringList & families)
	{
		QDockWidget * dock = 0;
		QWidget * widget = 0;
		QHBoxLayout * layout = 0;
		QScrollArea * scrollArea = 0;
		QButtonGroup * group = 0;
		
		for (int i=0; i < families.size(); ++i)
		{
			QString s = families[i];
			s.replace(tr(" "),tr(""));

			QString dirname = homeDir() + tr("/Modules/") + s;
			QDir dir(dirname);

			if (!dir.exists())		
				dir.setPath(homeDir() + tr("/Modules/") + s.toLower());

			if (!dir.exists())
				dir.setPath(QCoreApplication::applicationDirPath() + tr("/Modules/") + s);

			if (!dir.exists())
				dir.setPath(QCoreApplication::applicationDirPath() + tr("/Modules/") + s.toLower());

			if (!dir.exists())
				continue;
				
			if (!dock)
			{
				dock = new QDockWidget;
				dock->setMaximumHeight(150);
				widget = new QWidget;
				layout = new QHBoxLayout;
				scrollArea = new QScrollArea;

				group = new QButtonGroup(this);
				connect(group,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(modelButtonClicked(QAbstractButton*)));
			}
			
			dir.setFilter(QDir::Files);
			dir.setSorting(QDir::Time);
			QFileInfoList list = dir.entryInfoList();

			for (int i = 0; i < list.size(); ++i)
			{
				QFileInfo fileInfo = list.at(i);
				QToolButton * button = new QToolButton;
				button->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
				QString base = fileInfo.baseName();
				button->setText(base);
				button->setToolTip(fileInfo.absoluteFilePath());

				if (QFile::exists(dirname + base + tr(".png")))
					button->setIcon(QIcon(dirname + base + tr(".png")));
				else
					button->setIcon(QIcon(tr(":/images/module.png")));
				group->addButton(button,i);
				layout->addWidget(button,0,Qt::AlignTop);
			}
		}

		layout->setContentsMargins(5,8,5,5);
		layout->setSpacing(12);
		widget->setLayout(layout);
		widget->setPalette(QPalette(QColor(255,255,255)));
		widget->setAutoFillBackground (true);

		scrollArea->setWidget(widget);
		scrollArea->setPalette(QPalette(QColor(255,255,255)));
		scrollArea->setAutoFillBackground (true);
		
		widget = new QWidget;
		QVBoxLayout * vlayout = new QVBoxLayout;
		vlayout->addWidget(scrollArea);
		vlayout->setContentsMargins(0,0,0,0);
		vlayout->setSpacing(0);
		widget->setLayout(vlayout);
		
		dock->setWidget(widget);
		dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
		dock->setMaximumHeight(120);
		
		return dock;
	}
	
	void ModuleTool::keyPressed(GraphicsScene* scene,QKeyEvent * keyEvent)
	{
		if (!keyEvent || keyEvent->modifiers() || !scene || !scene->useDefaultBehavior) return;

		QList<QGraphicsItem*> & selected = scene->selected();
		
		if (selected.size() == 1 && keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
		{
			mouseDoubleClicked(scene,QPointF(),selected[0],Qt::LeftButton,(Qt::KeyboardModifiers)0);
		}
	}

	void ModuleTool::mouseDoubleClicked (GraphicsScene * scene, QPointF , QGraphicsItem * item, Qt::MouseButton, Qt::KeyboardModifiers modifiers)
    {
		if (!scene || !scene->network || !item || !mainWindow || modifiers || !(ArrowHeadItem::cast(item) || ConnectionGraphicsItem::cast(item))) return;

		ItemHandle * handle = getHandle(item);

		if (!handle)
		{
			ArrowHeadItem * arrow = ArrowHeadItem::cast(item);
			if (arrow)
			{
				if (arrow->connectionItem && arrow->connectionItem->centerRegionItem == arrow)
					handle = getHandle(arrow->connectionItem);
			}
		}

		ConnectionHandle * chandle = ConnectionHandle::cast(handle);

		if (chandle && handle->family() && !handle->children.isEmpty())
		{
			QList<TextEditor*> editors = scene->network->editors();
			QList<GraphicsScene*> scenes = scene->network->scenes();

			for (int i=0; i < editors.size(); ++i)
				if (editors[i]->localHandle() == handle && !editors[i]->text().isEmpty())
				{
					editors[i]->popOut();
					return;
				}

			for (int i=0; i < scenes.size(); ++i)
				if (scenes[i]->localHandle() == handle && !scenes[i]->items().isEmpty())
				{
					scenes[i]->popOut();
					return;
				}

			ItemFamily * family = handle->family();
			QList<ItemFamily*> children = family->children();
			QStringList familynames;
			familynames << family->name();
			for (int i=0; i < children.size(); ++i)
				familynames << children[i]->name();
			
			QDockWidget * dock = makeDockWidget(familynames);
			if (dock)
			{
				if (!handle->children.isEmpty())
				{
					ConnectionGraphicsItem * connection;
					NodeGraphicsItem * node;
					TextGraphicsItem * text;
					QString groupID = handle->name;
					QList<QGraphicsItem*> items, items2;
					for (int i=0; i < handle->children.size(); ++i)
						if (handle->children[i])
							items2 << handle->children[i]->graphicsItems;
					
					QList<NodeHandle*> nodes = chandle->nodes();
					for (int i=0; i < nodes.size(); ++i)
						if (nodes[i])
							items2 << nodes[i]->graphicsItems;
					
					for (int j=0; j < items2.size(); ++j)
						if (!items2[j]->scene() &&
							(
								( (connection = ConnectionGraphicsItem::cast(items2[j])) &&
									(groupID == connection->groupID))
								||
								( (node = NodeGraphicsItem::cast(items2[j]))  &&
									(groupID  ==  node->groupID))
								||	
								( (text = TextGraphicsItem::cast(items2[j]))  &&
									(groupID == text->groupID))
							))
							{
								items << items2[j];
							}
				
					
					NetworkHandle * network = scene->network;
					NetworkWindow * window = 0;
					if (!items.isEmpty())
					{
						GraphicsScene * newScene = network->createScene();
						window = newScene->networkWindow;
						if (window)
						{
							window->addDockWidget(Qt::TopDockWidgetArea,dock);
							window->handle = handle;	
						}
						newScene->insert(handle->name + tr(" visible"),items);
						newScene->fitAll();
					}
					else
					{
						QString modelText;
						emit getTextVersion(handle->children, &modelText);
						TextEditor * newEditor = network->createTextEditor(modelText);	
						window = newEditor->networkWindow;
						if (window)
						{
							window->addDockWidget(Qt::TopDockWidgetArea,dock);
							window->handle = handle;	
						}
						newEditor->insert(handle->children);
					}
					
					if (!window)
						delete dock;
				}
			}
		}
    }

	void ModuleTool::moduleButtonPressed(const QString& name)
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		if (name == tr("New module"))
		{
			mainWindow->setCursor(Qt::ArrowCursor);
			showNewModuleDialog();
		}

		//if (name == tr("Connect input/output"))
		//	mode = connecting;

		if (mode != none)
			scene->useDefaultBehavior = false;
	}
	
	void ModuleTool::updateNumberForNewModule(int n)
	{
		if (!newModuleTable || !nodesTree) return;
		
		for (int i=0; i < newModuleTable->rowCount(); ++i)
		{
			QWidget * widget = newModuleTable->cellWidget(i,1);
			delete widget;
		}
		newModuleTable->setRowCount(n);
		QStringList names(nodesTree->getAllFamilyNames());
		
		int k = names.indexOf(tr("Molecule"));
		if (k < 0)
			k = 0;
		QLineEdit * lineEdit;
		QComboBox * comboBox;
		
		for (int i=0; i < newModuleTable->rowCount(); ++i)
		{
			lineEdit = new QLineEdit;
			comboBox = new QComboBox;
			
			comboBox->addItems(names);
			comboBox->setCurrentIndex(k);
			
			newModuleTable->setCellWidget(i,0,lineEdit);
			newModuleTable->setCellWidget(i,1,comboBox);
		}
	}

	void ModuleTool::showNewModuleDialog()
	{
		if (!nodesTree || !connectionsTree)
		{
			QMessageBox::information(mainWindow, tr("No catalog"), tr("Cannot create new modules because no catalog of components is available"));
			return;
		}
		
		if (!newModuleDialog)
		{
			newModuleDialog = new QDialog(mainWindow);
			QVBoxLayout * layout = new QVBoxLayout;
			
			QGroupBox * group1 = new QGroupBox(tr(""));
			QVBoxLayout * layout1 = new QVBoxLayout;
			QHBoxLayout * layout1a = new QHBoxLayout, * layout1b = new QHBoxLayout;
			newModuleName = new QLineEdit;
			layout1a->addStretch(1);
			layout1a->addWidget(new QLabel(tr(" Module name : ")),0);
			layout1a->addWidget(newModuleName,0);
			layout1a->addStretch(1);

			layout1b->addStretch(1);
			layout1b->addWidget(new QLabel(tr(" Number of inputs/outputs : ")),0);
			QSpinBox * spinBox = new QSpinBox;
			spinBox->setRange(2,20);
			connect(spinBox,SIGNAL(valueChanged(int)),this,SLOT(updateNumberForNewModule(int)));
			layout1b->addWidget(spinBox,0);
			layout1b->addStretch(1);

			layout1->addLayout(layout1a);
			layout1->addLayout(layout1b);

			group1->setLayout(layout1);
			layout->addWidget(group1);

			newModuleTable = new QTableWidget;
			newModuleTable->setColumnCount(2);
			newModuleTable->setHorizontalHeaderLabels(QStringList() << "Name" << "Family" );
			QGroupBox * group2 = new QGroupBox(tr(""));
			QHBoxLayout * layout2 = new QHBoxLayout;
			layout2->addWidget(newModuleTable,1,Qt::AlignCenter);
			group2->setLayout(layout2);
			layout->addWidget(group2);

			QPushButton * okButton = new QPushButton("&Make Module");
			QPushButton * cancelButton = new QPushButton("&Cancel");
			connect(okButton,SIGNAL(pressed()),newModuleDialog,SLOT(accept()));
			connect(cancelButton,SIGNAL(pressed()),newModuleDialog,SLOT(reject()));
			if (mainWindow)
				connect(cancelButton,SIGNAL(pressed()),mainWindow,SLOT(sendEscapeSignal()));
			QGroupBox * group3 = new QGroupBox(tr(""));
			QHBoxLayout * layout3 = new QHBoxLayout;
			layout3->addStretch(1);
			layout3->addWidget(okButton);
			layout3->addWidget(cancelButton);
			layout3->addStretch(1);
			group3->setLayout(layout3);
			layout->addWidget(group3);

			newModuleDialog->setLayout(layout);
			spinBox->setValue(3);
		}

		newModuleDialog->exec();
		
		if (newModuleDialog->result() == QDialog::Accepted)		
			makeNewModule();
	}
	
	void ModuleTool::makeNewModule()
	{
		if (!catalogWidget || !nodesTree || !connectionsTree || !newModuleName || !newModuleTable || 
			!connectionsTree->getFamily(tr("Module"))) 
			return;

		QString name = newModuleName->text();
		if (name.isNull() || name.isEmpty()) return;
		
		QString appDir = QCoreApplication::applicationDirPath();

		ConnectionFamily * moduleFamily = connectionsTree->getFamily(tr("Module"));
		ConnectionFamily * newModuleFamily = new ConnectionFamily(name);
		newModuleFamily->setParent(moduleFamily);
		newModuleFamily->pixmap = moduleFamily->pixmap;
		newModuleFamily->description = moduleFamily->description;
		newModuleFamily->graphicsItems << new ArrowHeadItem(appDir + interfaceFileName)
										<< new ArrowHeadItem(appDir + moduleFileName);

		
		FamilyTreeButton * button = new FamilyTreeButton(newModuleFamily);
		connectionsTree->insertFamily(newModuleFamily,button);
		connect(button,SIGNAL(connectionSelected(ConnectionFamily*)),connectionsTree,SLOT(buttonPressed(ConnectionFamily*)));
				
		QLineEdit * lineEdit;
		QComboBox * comboBox;

		for (int i=0; i < newModuleTable->rowCount(); ++i)
		{
			lineEdit = static_cast<QLineEdit*>(newModuleTable->cellWidget(i,0));
			comboBox = static_cast<QComboBox*>(newModuleTable->cellWidget(i,1));			
			newModuleFamily->addParticipant(lineEdit->text(), comboBox->currentText());
		}

		catalogWidget->showButtons(QStringList() << newModuleFamily->name());
	}

	void ModuleTool::itemsDropped(GraphicsScene * scene, const QString& family, const QPointF& point)
	{
		if (scene && scene->network && family == tr("New module") && mode == none)
		{
			mode = inserting;
			sceneClicked(scene, point, Qt::LeftButton, 0);
			mode = none;
		}
	}
}

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
    if (!main) return;

    Tinkercell::ModuleTool * tool = new Tinkercell::ModuleTool;
    main->addTool(tool);

}

