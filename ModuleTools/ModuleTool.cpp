/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This tool handles module connections that merge items from two modules

****************************************************************************/

#include <math.h>
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
	static QString linkerFileName("/DecoratorItems/moduleLinker.xml");
	static QString interfaceFileName("/DecoratorItems/moduleInterface.xml");
	static QString moduleFileName("/DecoratorItems/Module.xml");
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
        
        QString appDir = QCoreApplication::applicationDirPath();
        NodeGraphicsReader reader;
        reader.readXml(&image, appDir + interfaceFileName);
        image.normalize();
        image.scale(40.0/image.sceneBoundingRect().width(),20.0/image.sceneBoundingRect().height());
        ToolGraphicsItem * toolGraphicsItem = new ToolGraphicsItem(this);
        toolGraphicsItem->addToGroup(&image);
        toolGraphicsItem->setToolTip(tr("Module input/output"));
        addGraphicsItem(toolGraphicsItem);
        addAction(QIcon(":/images/moduleInput.png"),tr("Module input/output"),tr("Set selected nodes as interfaces for this module"));
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

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			toolLoaded(mainWindow->tool(tr("Nodes Tree")));
			
			toolLoaded(mainWindow->tool(tr("Connections Tree")));

			toolLoaded(mainWindow->tool(tr("Parts and Connections Catalog")));

			toolLoaded(mainWindow->tool(tr("Save and Load")));
        }

        return true;
    }

	void ModuleTool::toolLoaded(Tool * tool)
	{
		if (!tool) return;
		static bool connected2 = false;
		
		if (tool->name == tr("Nodes Tree") && !nodesTree)
		{
			nodesTree = static_cast<NodesTree*>(tool);
		}
		
		if (tool->name == tr("Connections Tree") && !connectionsTree)
		{
			connectionsTree = static_cast<ConnectionsTree*>(tool);
			if (!connectionsTree->getFamily(tr("Module")))
			{
				QString appDir = QCoreApplication::applicationDirPath();
				
				ConnectionFamily * moduleFamily = new ConnectionFamily(tr("Module"));
				moduleFamily->pixmap = QPixmap(tr(":/images/module.png"));
				moduleFamily->description = tr("Self-contained subsystem that can be used to build larger systems");
				moduleFamily->textAttributes[tr("Functional description")] = tr("");
				moduleFamily->graphicsItems << new ArrowHeadItem(appDir + interfaceFileName)
											 << new ArrowHeadItem(appDir + moduleFileName);				
				connectionsTree->insertFamily(moduleFamily,0);
			}
		}

		if (tool->name == tr("Parts and Connections Catalog") && !connected2)
		{
			catalogWidget = static_cast<CatalogWidget*>(tool);

			connect(catalogWidget,SIGNAL(buttonPressed(const QString&)),
					this,SLOT(moduleButtonPressed(const QString&)));

			QList<QToolButton*> newButtons = catalogWidget->addNewButtons(
				tr("Modules"),
				QStringList() 	<< tr("New module") 
								//<< tr("Insert input/output")
								<< tr("Connect input/output"),
				QList<QIcon>() 	<< QIcon(QPixmap(tr(":/images/module.png"))) 
								//<< QIcon(QPixmap(tr(":/images/moduleInput.png")))
								<< QIcon(QPixmap(tr(":/images/connectmodules.png"))),
				QStringList() 	<< tr("A module is a self-contained subsystem that can be used to build larger systems")
								//<< tr("Use this to set an item inside a module as an input or ouput for that module")
								<< tr("Use this to connect inputs and ouputs of two modules")
				);

			connected2 = true;
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
			Tool * tool = mainWindow->tool(tr("Nodes Tree"));
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
				image = new NodeGraphicsItem(appDir + tr("/NodeItems/Module.xml"));
			}
			
			image->setPos(point);
			image->adjustBoundaryControlPoints();
			image->setHandle(handle);

			TextGraphicsItem * text = new TextGraphicsItem(handle);
			text->setPos(QPointF(point.x(),image->sceneBoundingRect().bottom() + 10.0));
			text->scale(2,2);

			scene->insert(handle->name + tr(" inserted"),QList<QGraphicsItem*>() << image << text);

			return;
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
						handle1->name + tr(" (") + handle1->family()->name + tr(") and ") +
						handle2->name + tr(" (") + handle2->family()->name + tr(") are different types of objects")	);
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
		NodeGraphicsItem * node;
		for (int i=0; i < items.size(); ++i)
			if (handle = getHandle(items[i]))
			{
				if (ConnectionGraphicsItem::cast(items[i]))
				{
					for (int j=0; j < handle->children.size(); ++j)
						if (handle->children[j])
						{
							QList<QGraphicsItem*> childItems = handle->children[j]->allGraphicsItems();
							for (int k=0; k < childItems.size(); ++k)
							{
								items.removeAll(childItems[k]);
								handles.removeAll( getHandle(childItems[k]) );
							}
						}
				}
			}
	}

	void ModuleTool::itemsAboutToBeInserted(GraphicsScene* scene, QList<QGraphicsItem *>& items, QList<ItemHandle*>& handles, QList<QUndoCommand*>& commands)
	{
		if (!scene || !scene->network) return;

		commands << moduleConnectionsInserted(items)
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
		}

		QStringList visitedFiles;

		for (int i=0; i < handles.size(); ++i)
			if (handles[i] && handles[i]->children.isEmpty() && ConnectionFamily::cast(handles[i]->family()))
			{
				QString s = handles[i]->family()->name;
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
						QString filename = list.first().absoluteFilePath();

						if (QFile::exists(filename) && !visitedFiles.contains(filename))
						{			
							visitedFiles << filename;

							QList<ItemHandle*> handles2 = mainWindow->getItemsFromFile(filename);
							
							QList<ItemHandle*> visitedHandles;
							
							for (int j=0; j < handles2.size(); ++j)
								if (handles2[j] && !visitedHandles.contains(handles2[j]))
								{
									visitedHandles << handles2[j];
									if (!handles2[j]->parent)
									{
										handles2[j]->setParent(handles[i],false);
										RenameCommand::findReplaceAllHandleData(handles2,handles2[j]->name,handles2[j]->fullName());
									}
								}
						}
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
		if (!node || !connection || !node || !connection->hasTextData(tr("Participants"))) return 0;
		QList<NodeHandle*> nodes = connection->nodes();
		
		if (nodes.contains(node)) return 0;
		
		TextDataTable & participants = connection->textDataTable(tr("Participants"));
		QStringList rownames = participants.getRowNames();
		QString s;
		
		for (int i=0; i < nodes.size(); ++i)
		{
			if (rownames.contains(nodes[i]->fullName()))
			{
				s = participants.value(nodes[i]->fullName(),0);
				if (node->name.compare(s,Qt::CaseInsensitive) == 0)
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
			NodeGraphicsItem * node, * module;
			QList<QUndoCommand*> commands;
			
			for (int i=0; i < handles.size(); ++i)
				if (handles[i])
				{
					//if (NodeHandle::cast(handles[i]) && !handles[i]->tools.contains(this))
						//handles[i]->tools += this;
										
					if (ConnectionHandle::cast(handles[i]) && !handles[i]->children.isEmpty())
					{
						ItemHandle * h;
						
						for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
							if (c = ConnectionGraphicsItem::cast(handles[i]->graphicsItems[j]))
							{
								scene->showToolTip(c->centerLocation(),handles[i]->name + tr(" contains a model inside"));
								break;
							}

						for (int j=0; j < handles[i]->children.size(); ++j)
						{
							h = findCorrespondingHandle(NodeHandle::cast(handles[i]->children[j]),ConnectionHandle::cast(handles[i]));
							if (h)
							{
								commands << new MergeHandlesCommand(
										tr("merge"), network, QList<ItemHandle*>() << h << handles[i]->children[j]);
								/*for (int k=0; k < handles[i]->children[j]->graphicsItems.size(); ++k)
									setHandle(handles[i]->children[j]->graphicsItems[k],h);
								for (int k=0; k < handles[i]->children[j]->children.size(); ++k)
									handles[i]->children[j]->children[k]->setParent(h);*/
							}
						}
					}
				}
			
			if (!commands.isEmpty())
				network->push( new CompositeCommand(tr("Merged models"),commands) );
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

		//ModuleConnectionGraphicsItem * connection = new ModuleConnectionGraphicsItem;

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
			QList<QGraphicsItem*> items;
			
			if (QFile::exists(filename) && currentNetwork())
			{
				NetworkWindow * window = currentNetwork()->currentWindow();
				QList<ItemHandle*> handles = mainWindow->getItemsFromFile(filename);
				
				if (!window || handles.isEmpty()) return;
				
				QList<QGraphicsItem*> items;
				for (int i=0; i < handles.size(); ++i)
				{
					if (!handles[i]->parent)
					{
						handles[i]->setParent(window->handle,false);
						if (!ConnectionHandle::cast(handles[i]))
							items << handles[i]->graphicsItems;
					}
				}
				
				if (window && window->scene)
				{
					if (!items.isEmpty())
					{
						GraphicsScene * scene = window->scene;
						scene->remove(tr("remove model"),scene->items());
						scene->insert(tr("new model"),items);
						scene->fitAll();
					}
					else
					{
						QString modelText;
						emit getTextVersion(handles, modelText);
						window->newTextEditor()->setText(modelText);
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
						emit getTextVersion(handles, modelText);
						window->newTextEditor()->setText(modelText);
					}
				}
			}
		}
	}
	
	QDockWidget * ModuleTool::makeDockWidget(const QString & family)
	{
		QString s = family;
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
			return 0;
			
		QDockWidget * dock = new QDockWidget;
		QWidget * widget = new QWidget;
		QHBoxLayout * layout = new QHBoxLayout;
		QScrollArea * scrollArea = new QScrollArea;
		
		QButtonGroup * group = new QButtonGroup(this);
		connect(group,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(modelButtonClicked(QAbstractButton*)));

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

	void ModuleTool::mouseDoubleClicked (GraphicsScene * scene, QPointF , QGraphicsItem * item, Qt::MouseButton, Qt::KeyboardModifiers)
    {
		if (!scene || !scene->network || !item || !mainWindow) return;
		
		ItemHandle * handle = getHandle(item);
		
		if (!handle)
		{
			NodeGraphicsItem * node;
			if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(item)) && 
				(node->className == ArrowHeadItem::CLASSNAME))
			{
				ArrowHeadItem * arrow = static_cast<ArrowHeadItem*>(node);
				if (arrow->connectionItem && arrow->connectionItem->centerRegionItem == arrow)
					handle = getHandle(arrow->connectionItem);
			}
		}

		if (handle && handle->family())
		{
			QList<TextEditor*> editors = scene->network->editors();
			QList<GraphicsScene*> scenes = scene->network->scenes();

			for (int i=0; i < editors.size(); ++i)
				if (editors[i]->localHandle() == handle)
				{
					editors[i]->popOut();
					return;
				}

			for (int i=0; i < scenes.size(); ++i)
				if (scenes[i]->localHandle() == handle)
				{
					scenes[i]->popOut();
					return;
				}
			
			QDockWidget * dock = makeDockWidget(handle->family()->name);
			if (dock)
			{
				NetworkHandle * network = scene->network;
				GraphicsScene * newScene = network->createScene();
				NetworkWindow * window = newScene->networkWindow;
				if (window)
				{
					window->addDockWidget(Qt::TopDockWidgetArea,dock);
					window->handle = handle;
					
					if (!handle->children.isEmpty())
					{
						QList<QGraphicsItem*> items, items2;
						for (int i=0; i < handle->children.size(); ++i)
							if (handle->children[i])
							{
								items2 = handle->children[i]->graphicsItems;
								for (int j=0; j < items2.size(); ++j)							
									if (items2[j] && !items2[j]->scene())
										items << items2[j];
							}

						if (!items.isEmpty())
						{
							newScene->insert(handle->name + tr(" expanded"),items);
						}
					}
				}
				else
					delete dock;
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

		if (name == tr("Connect input/output"))
			mode = connecting;

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
			
			newModuleFamily->nodeRoles += lineEdit->text();
			newModuleFamily->nodeFamilies += comboBox->currentText();
		}

		catalogWidget->showButtons(QStringList() << newModuleFamily->name);
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

