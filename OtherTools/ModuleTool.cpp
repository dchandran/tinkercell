/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool handles module connections that merge items from two modules

****************************************************************************/

#include "ItemHandle.h"
#include "GraphicsScene.h"
#include "ConsoleWindow.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ModuleTool.h"

namespace Tinkercell
{
    ModuleTool::VisualTool::VisualTool(ModuleTool * tool) : Tool::GraphicsItem(tool)
    {
        moduleTool = tool;
        QString appDir = QCoreApplication::applicationDirPath();
        NodeGraphicsReader reader;
        reader.readXml(&image, appDir + tr("/OtherItems/moduleTool.xml"));
        image.normalize();
        image.scale(40.0/image.sceneBoundingRect().width(),40.0/image.sceneBoundingRect().height());
        image.setToolTip(tr("Module input/output"));
        setToolTip(tr("Module input/output"));
        addToGroup(&image);
    }

    ModuleTool::ModuleTool() : Tool(tr("Module Connection Tool"))
    {
        separator = 0;
        VisualTool * visualTool = new VisualTool(this);
        this->graphicsItems += visualTool;
        makeLink = new QAction(tr("Set input/output"),this);
        makeLink->setIcon(QIcon(tr(":/images/plug.png")));
        connect(makeLink,SIGNAL(triggered()),this,SLOT(makeLinks()));

        setPalette(QPalette(QColor(255,255,255,255)));
        setAutoFillBackground(true);
        mode = none;
        lineItem.setPen(QPen(QColor(255,10,10,255),2.0,Qt::DotLine));
    }

    bool ModuleTool::setMainWindow(MainWindow * main)
    {
		Tool::setMainWindow(main);
        if (mainWindow != 0)
        {
            connect(mainWindow,SIGNAL(prepareModelForSaving(NetworkWindow*)),this,SLOT(prepareModelForSaving(NetworkWindow*)));
            connect(mainWindow,SIGNAL(modelSaved(NetworkWindow*)),this,SLOT(modelSaved(NetworkWindow*)));

            connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSignal(const QWidget*)));
            connect(mainWindow,SIGNAL(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&)),
                    this, SLOT(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&)));
			connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)),
                    this, SLOT(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)));
            connect(mainWindow,SIGNAL(itemsAboutToBeRemoved(GraphicsScene*, QList<QGraphicsItem *>&, QList<ItemHandle*>&)),
                    this, SLOT(itemsRemoved(GraphicsScene*, QList<QGraphicsItem *>&, QList<ItemHandle*>&)));
            connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
                    this, SLOT(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
            connect(mainWindow,SIGNAL(itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)),
                    this, SLOT(itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)));

            connect(mainWindow,SIGNAL(mouseReleased(GraphicsScene*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
                    this,SLOT(mouseReleased(GraphicsScene*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));

            connect(mainWindow,SIGNAL(mouseDragged(GraphicsScene*,QPointF,QPointF,Qt::MouseButton,Qt::KeyboardModifiers)),
                    this,SLOT(mouseDragged(GraphicsScene*,QPointF,QPointF,Qt::MouseButton,Qt::KeyboardModifiers)));

            connect(mainWindow,SIGNAL(mouseMoved(GraphicsScene*, QGraphicsItem*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)),
                    this,SLOT(mouseMoved(GraphicsScene*, QGraphicsItem*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)));

            connect(mainWindow,SIGNAL(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)),
                    this,SLOT(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)));
        }
        return true;
    }

    NodeGraphicsItem* ModuleTool::VisualTool::parentModule(QGraphicsItem* item)
    {
        ItemHandle * handle = getHandle(item);
        if (!handle) return 0;

        ItemHandle * module = handle->parentOfFamily(tr("module"));
        if (!module) return 0;

        NodeGraphicsItem * node;
        for (int i=0; i < module->graphicsItems.size(); ++i)
        {
            node = qgraphicsitem_cast<NodeGraphicsItem*>(module->graphicsItems[i]);
            if (node)
                return node;
        }
        return 0;
    }

    void ModuleTool::select(int)
    {
        makeLinks();
    }

    void ModuleTool::makeLinks()
    {
        GraphicsScene * scene = currentScene();
        if (!scene) return;

        QList<QGraphicsItem*> & items = scene->selected();

        QList<QGraphicsItem*> toInsert;
        QList<ModuleLinkerItem*> linkers;

        bool alreadyLinked = false;
        NodeGraphicsItem * node;
        for (int i=0; i < items.size(); ++i)
        {
            ItemHandle * handle = getHandle(items[i]);
            if (handle && handle->family() && handle->family()->isA("node"))
            {
                alreadyLinked = false;
                for (int j=0; j < handle->graphicsItems.size(); ++j)
                    if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(handle->graphicsItems[j])) &&
                        (node->className == ModuleLinkerItem::class_name))
                {
                    alreadyLinked = true;
                    break;
                }

                if (alreadyLinked) continue;

                NodeGraphicsItem * module = VisualTool::parentModule(items[i]);

                if (!module) continue;
                ModuleLinkerItem * linker = new ModuleLinkerItem(module,0,0);
                setHandle(linker,handle);

                linkers += linker;
                toInsert += linker;
            }
        }

        scene->insert("module interface created",toInsert);

        for (int i=0; i < linkers.size(); ++i)
            linkers[i]->setPosOnEdge();
    }

    QPointF ModuleTool::VisualTool::getPoint(QGraphicsItem* module, QPointF scenePos, QGraphicsItem * linker)
    {
        if (!linker || !module) return scenePos;

        qreal width = linker->sceneBoundingRect().width();

        linker->resetTransform();

        linker->scale(width/linker->sceneBoundingRect().width(),width/linker->sceneBoundingRect().width());

        qreal w = 0;

        if (linker->sceneBoundingRect().width() > linker->sceneBoundingRect().height())
            w = linker->sceneBoundingRect().width();
        else
            w = linker->sceneBoundingRect().height();

        QPointF point = pointOnEdge(module->sceneBoundingRect(), scenePos);
        QPointF diff = point - scenePos;
        if (diff.rx()*diff.rx() > diff.ry()*diff.ry())
            point.ry() = scenePos.ry();
        else
            point.rx() = scenePos.rx();

        if (point.ry() != scenePos.ry())
        {
            if (diff.ry() > 0)
            {
                point.ry() += w/2.0;
                linker->rotate(90);
            }
            else
            {
                point.ry() -= w/2.0;
                linker->rotate(-90);
            }
        }
        else
            if (point.rx() != scenePos.rx())
            {
            if (diff.rx() < 0)
            {
                point.rx() -= w/2.0;
                linker->rotate(180);
            }
            else
            {
                point.rx() += w/2.0;
            }
        }
        return point;
    }

    void ModuleTool::VisualTool::visible(bool b)
    {
        if  (!b || !moduleTool)
        {
            Tool::GraphicsItem::visible(false);
            return;
        }

        MainWindow * mainWindow = moduleTool->mainWindow;

        if (!mainWindow)
        {
            Tool::GraphicsItem::visible(false);
            return;
        }
		
		mainWindow->contextItemsMenu.removeAction(moduleTool->makeLink);
        if (moduleTool->separator)
			mainWindow->contextItemsMenu.removeAction(moduleTool->separator);
        
        GraphicsScene * scene = mainWindow->currentScene();

		if (!scene)
        {
            Tool::GraphicsItem::visible(false);
            return;
        }

        QList<QGraphicsItem*> & items = scene->selected();
        NodeGraphicsItem * node = 0;
        bool alreadyLinked = false;

        for (int i=0; i < items.size(); ++i)
        {
            ItemHandle * handle = getHandle(items[i]);
            if (handle && handle->family() && handle->family()->isA("node"))
            {
                alreadyLinked = false;
                for (int j=0; j < handle->graphicsItems.size(); ++j)
                    if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(handle->graphicsItems[j])) &&
                        (node->className == ModuleLinkerItem::class_name) &&
                        (node->scene() == items[i]->scene()) &&
                        node->isVisible())
                {
                    alreadyLinked = true;
                    break;
                }
                if (alreadyLinked) continue;
                NodeGraphicsItem * module = parentModule(items[i]);
                if (module)
                {
                    Tool::GraphicsItem::visible(b);
                    if (mainWindow)
                    {
                        if (moduleTool->separator)
                            mainWindow->contextItemsMenu.addAction(moduleTool->separator);
                        else
                            moduleTool->separator = mainWindow->contextItemsMenu.addSeparator();

                        mainWindow->contextItemsMenu.addAction(moduleTool->makeLink);
						
                        return;
                    }
                }
            }
        }

        Tool::GraphicsItem::visible(false);
    }

    void ModuleTool::escapeSignal(const QWidget*)
    {
        mode = none;
    }

    void ModuleTool::prepareModelForSaving(NetworkWindow* win)
    {
        if (!win || !win->scene) return;
		
		GraphicsScene * scene = win->scene;

        QList<QGraphicsItem*> items = scene->items();
        ConnectionGraphicsItem * connection = 0;

        for (int i=0; i < items.size(); ++i)
        {
            connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i]);
            if (connection && ModuleConnectionGraphicsItem::isModuleConnectionItem(connection))
            {
                ModuleConnectionGraphicsItem * mc = static_cast<ModuleConnectionGraphicsItem*>(connection);
                if (mc->command)
                {
                    QUndoCommand * reverseCommand = new ReverseUndoCommand("disconnect modules (saving)",mc->command,false);
                    if (scene && scene->historyStack)
                        scene->historyStack->push(reverseCommand);
                    else
                    {
                        reverseCommand->redo();
                        delete reverseCommand;
                    }
                }
            }
        }
    }

    void ModuleTool::modelSaved(NetworkWindow* win)
    {
        if (!win || !win->scene) return;
		
		GraphicsScene* scene = win->scene;

        QList<QGraphicsItem*> items = scene->items();
        ConnectionGraphicsItem * connection = 0;

        for (int i=0; i < items.size(); ++i)
        {
            connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i]);
            if (connection && ModuleConnectionGraphicsItem::isModuleConnectionItem(connection))
            {
                ModuleConnectionGraphicsItem * mc = static_cast<ModuleConnectionGraphicsItem*>(connection);
                if (mc->command)
                {
                    QUndoCommand * command = new CompositeCommand(tr("reconnecting modules"),mc->command,false);
                    if (scene && scene->historyStack)
                        scene->historyStack->push(command);
                    else
                    {
                        mc->command->redo();
                    }
                }
            }
        }
    }

    void ModuleTool::itemsInserted(GraphicsScene*, const QList<QGraphicsItem *>&, const QList<ItemHandle*>& handles)
    {	
		for (int i=0; i < handles.size(); ++i)
        {
            ItemHandle * handle = handles[i];
			
            if (NodeHandle::asNode(handle) && !handle->tools.contains(this))
            {
				handle->tools += this;
            }
        }
	}
	
	void ModuleTool::itemsAboutToBeInserted(GraphicsScene* scene, QList<QGraphicsItem *>& items, QList<ItemHandle*>& handles)
	{
		if (!scene) return;
		
		NodeGraphicsItem * node = 0;
        ConnectionGraphicsItem * connection = 0;
		
        bool nothingToDo = true;
        for (int i=0; i < items.size(); ++i)
        {
            if ( ((node = qgraphicsitem_cast<NodeGraphicsItem*>(items[i])) && 
			      (node->className == ModuleLinkerItem::class_name))
                  ||
                 ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i])) && 
				  (connection->className == ModuleConnectionGraphicsItem::class_name))
                )
            {
                nothingToDo = false;
                break;
            }
        }

        if (nothingToDo) return;

        NodeGraphicsItem * module = 0;

        QList<QGraphicsItem*> moduleConnections;

        for (int i=0; i < items.size(); ++i)
        {
            if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(items[i])) &&
                (node->className == ModuleLinkerItem::class_name))
            {
				if (!node->handle())
				{
					node->className = NodeGraphicsItem::class_name;
					continue;
				}
				module = VisualTool::parentModule(node);
                QList<QGraphicsItem*> itemsAt = scene->items(node->sceneBoundingRect());

                QList<ConnectionGraphicsItem*> connections = node->connections();

                bool allLinks = true;
				
                for (int j=0; j < connections.size(); ++j)
                    if (connections[j] && !moduleConnections.contains(connections[j]))
                    {
                        moduleConnections += connections[j];
                    }
                
				if (module)
                {
					NodeGraphicsItem * linker = new ModuleLinkerItem(module);
					
					ItemHandle * handle = getHandle(node);
                    setHandle(node,0);
                    (*linker) = (*node);
					setHandle(linker,handle);
					
					ConsoleWindow::message(handle->name);

                    allLinks = true;
					
				    for (int j=0; j < connections.size(); ++j)
                        if (connections[j])
                        {
							connections[j]->replaceNode(node,linker);
							node->className = NodeGraphicsItem::class_name;
						}

                    /*CompositeCommand * command = new CompositeCommand( tr("add linker"),
                                                                       QList<QUndoCommand*>()
                                                                       << (new RemoveGraphicsCommand(tr("remove old linker"),scene,node))
                                                                       << (new InsertGraphicsCommand(tr("add new linker"),scene,linker)));
                    if (scene->historyStack)
                        scene->historyStack->push(command);
                    else
                    {
                        command->redo();
                        delete command;
                    }*/
					
					//delete node;
					
					items[i] = linker; //replace
					
                    (static_cast<ModuleLinkerItem*>(linker))->setPosOnEdge();
                }
            }
        }
		
        for (int i=0; i < moduleConnections.size(); ++i)
        {
            if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(moduleConnections[i])) &&
                (connection->className == ModuleConnectionGraphicsItem::class_name))
            {
                ModuleConnectionGraphicsItem * mc = new ModuleConnectionGraphicsItem;  //the module connection
                QList<QUndoCommand*> commands;   //all the commands that will be executed when modules are connected

                ConnectionGraphicsItem * c = mc;   //copy the existing connection's visual appearance
                (*c) = (*connection);
                //connection->clear();

                connection->className = ConnectionGraphicsItem::class_name;

                QList<NodeGraphicsItem*> nodes = mc->nodes();

                QList<ItemHandle*> mergeHandles;
                if (nodes.size() == 2 && nodes[0] && nodes[1] && nodes[0] != nodes[1])
                {
                    ItemHandle * handle1 = getHandle(nodes[0]),
                    * handle2 = getHandle(nodes[1]);
                    if (handle1 == handle2)
                    {
                        NodeGraphicsItem * parentModule = static_cast<ModuleLinkerItem*>(nodes[0])->module;
                        ItemHandle * parentHandle = getHandle(parentModule);

                        ItemHandle * clone = handle1->clone();
                        setHandle(nodes[0],clone);
                        QList<QGraphicsItem*> graphicsItems = handle1->graphicsItems;

                        clone->graphicsItems.clear();
                        clone->children.clear();
                        clone->setParent(parentHandle);

                        if (parentModule)
                        {
                            QList<QGraphicsItem*> itemsAt = scene->items(parentModule->sceneBoundingRect());
                            for (int j=0; j < graphicsItems.size(); ++j)
                                if (graphicsItems[j] && itemsAt.contains(graphicsItems[j]))
                                {
									setHandle(graphicsItems[j],clone);
								}
                        }

                        mergeHandles << handle1 << clone;
                    }
                    else
                    {
                        mergeHandles << handle1 << handle2;
                    }
                }

                MergeHandlersCommand * mergeCommand = new MergeHandlersCommand(tr("items merged"),mergeHandles);

                QList<QString> newNames;

                for (int j=0; j < mergeHandles.size(); ++j)
                    newNames << mergeCommand->newHandle->fullName();

                commands += mergeCommand;
                commands += new RenameCommand(tr("name changed"),scene->networkWindow,mergeHandles,newNames);
                mc->command = new CompositeCommand(tr("items merged"),commands);

                mc->refresh();

                /*CompositeCommand * command = new CompositeCommand( tr("modules connected"),
                                                                   QList<QUndoCommand*>()
                                                                   << (new InsertGraphicsCommand(tr("new modules connection"),scene,mc))
                                                                   << mc->command
                                                                   << (new RemoveGraphicsCommand(tr("remove old connection"),scene,connection)),
                                                                   QList<QUndoCommand*>() << mc->command);
                if (scene->historyStack)
                {
                    scene->historyStack->push(command);
                }
                else
                {
                    command->redo();
                    delete command;
                }*/
				
				if (mc->command)
					mc->command->redo();
				
				for (int j=0; j < items.size(); ++j)
					if (items[j] == moduleConnections[i])
					{
						delete items[j];
						items[j] = mc;
					}
            }
        }
    }

    void ModuleTool::itemsSelected(GraphicsScene* scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers)
    {
        if (!scene) return;
        QList<QGraphicsItem*> & moving = scene->moving();

        NodeGraphicsItem * node = 0;
        ModuleLinkerItem * linker = 0;
        ItemHandle * handle = 0;

        for (int i=moving.size()-1; i >= 0; --i)
        {
            if ((node = NodeGraphicsItem::topLevelNodeItem(moving[i]))
                && (node->className == ModuleLinkerItem::class_name))
                {
					if (items.contains(node)) linker = static_cast<ModuleLinkerItem*>(node);
					moving.removeAt(i);
				}
        }

        if (linker && linker->module && items.size() == 1 && !linker->module->contains(point))
        {
            QRectF moduleRect = linker->module->sceneBoundingRect();
            moving.clear();
            mode = connecting;
            scene->useDefaultBehavior = false;

            QRectF rect = linker->sceneBoundingRect();
            scene->lastPoint() = rect.center();

            if (rect.bottom() > moduleRect.bottom())
                scene->lastPoint().ry() = rect.bottom();
            else
                if (rect.top() < moduleRect.top())
                    scene->lastPoint().ry() = rect.top();
            else
                if (rect.right() > moduleRect.right())
                    scene->lastPoint().rx() = rect.right();
            else
                if (rect.left() < moduleRect.left())
                    scene->lastPoint().rx() = rect.left();

            return;
        }

        for (int i=0; i < moving.size(); ++i)
        {
            if (qgraphicsitem_cast<NodeGraphicsItem*>(moving[i]) &&
                (handle = getHandle(moving[i])) && handle->family() && handle->family()->isA("module"))
            {
                node = 0;
                QRectF rect = moving[i]->sceneBoundingRect();
                QList<QGraphicsItem*> itemsInside = scene->items(rect.adjusted(-10,-10,10,10));
                for (int j=0; j < itemsInside.size(); ++j)
                {
                    if ((node = NodeGraphicsItem::topLevelNodeItem(itemsInside[j]))
                        && (node->className == ModuleLinkerItem::class_name)
                        && ((static_cast<ModuleLinkerItem*>(itemsInside[j]))->module == moving[i]))
                        {
                        if (!moving.contains(node)) moving += node;
                        if ((handle = getHandle(node)))
                        {
                            for (int k=0; k < handle->graphicsItems.size(); ++k)
                            {
                                if (handle->graphicsItems[k] &&
                                    rect.contains(handle->graphicsItems[k]->sceneBoundingRect()) &&
                                    !moving.contains(handle->graphicsItems[k]))

                                    moving += handle->graphicsItems[k];
                            }
                        }
                    }
                }
            }
        }
    }

    void ModuleTool::itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>& items, const QList<QPointF>&, Qt::KeyboardModifiers)
    {
        NodeGraphicsItem::ControlPoint * cp;
        NodeGraphicsItem* node;

        QList<ItemHandle*> modules;

        ItemHandle * handle, * child;

        for (int i=0; i < items.size(); ++i)
        {
            if ( (((cp = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(items[i])) &&
                   cp->nodeItem &&
                   (handle = getHandle(cp->nodeItem))) ||

                  (handle = getHandle(items[i])) )
                && handle->isA(tr("Module")) && !modules.contains(handle))
                modules += handle;

            if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(items[i])) &&
                node->itemHandle &&
                (handle = node->itemHandle->parentOfFamily(tr("Module"))) &&
                !modules.contains(handle))
                modules += handle;
        }

        for (int i=0; i < modules.size(); ++i)
        {
            if ( (handle = modules[i]) && handle->isA(tr("Module")) )
            {
                //qDebug() << handle->name << " module moved";
                for (int j=0; j < handle->children.size(); ++j)
                    if (child = handle->children[j])
                    {
                    for (int k=0; k < child->graphicsItems.size(); ++k)
                    {
                        if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(child->graphicsItems[k])) &&
                            (node->className == ModuleLinkerItem::class_name))
                        {
                            (static_cast<ModuleLinkerItem*>(node))->setPosOnEdge();
                        }
                    }
                }
            }
        }
    }

    void ModuleTool::mouseMoved(GraphicsScene* scene, QGraphicsItem*, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)
    {
        if (mode == connecting && scene)
        {
            if (lineItem.scene() != scene)
                scene->addItem(&lineItem);

            if (!lineItem.isVisible())
                lineItem.setVisible(true);

            lineItem.setLine(QLineF(scene->lastPoint(),point));
            return;
        }
    }

    void ModuleTool::mouseReleased(GraphicsScene * scene, QPointF , Qt::MouseButton, Qt::KeyboardModifiers )
    {
        if (mode != none)
            scene->useDefaultBehavior = true;

        lineItem.setVisible(false);
        if (lineItem.scene())
            lineItem.scene()->removeItem(&lineItem);
        mode = none;
    }

    void ModuleTool::mouseDragged(GraphicsScene* scene,QPointF from,QPointF to,Qt::MouseButton,Qt::KeyboardModifiers)
    {
        if (mode == connecting && scene)
        {
            mode = none;
            scene->useDefaultBehavior = true;

            NodeGraphicsItem * node1=0, *node2=0;

            QList<QGraphicsItem*> items = scene->items(QRectF(from.rx()-5.0,from.ry()-5.0,10.0,10.0));

            for (int i=0; i < items.size(); ++i)
            {
                node1 = NodeGraphicsItem::topLevelNodeItem(items[i]);
                if (node1)// && node1->className == ModuleLinkerItem::class_name)
                    break;
                else
                    node1 = 0;
            }

            items = scene->items(QRectF(to.rx()-5.0,to.ry()-5.0,10.0,10.0));

            for (int i=0; i < items.size(); ++i)
            {
                node2 = NodeGraphicsItem::topLevelNodeItem(items[i]);
                if (node2)// && node2->className == ModuleLinkerItem::class_name)
                    break;
                else
                    node2 = 0;
            }

            if (node1 && node2 && node1 != node2 && node1->itemHandle && node2->itemHandle && node1->itemHandle != node2->itemHandle
                && node1->itemHandle->family() && node2->itemHandle->family())
                //&& node1->className == ModuleLinkerItem::class_name && node2->className == ModuleLinkerItem::class_name)
            {
		       if (
                        (node2->itemHandle->data && node2->itemHandle->data->numericalData.contains(tr("Fixed")) &&
                         node2->itemHandle->data->numericalData[tr("Fixed")].value(0,0) > 0)

                        ||

                        ( !(node1->itemHandle->data && node1->itemHandle->data->numericalData.contains(tr("Fixed")) &&
                            node1->itemHandle->data->numericalData[tr("Fixed")].value(0,0) > 0) &&
                          !(node2->itemHandle->family()->isA(node1->itemHandle->family())) &&
                          node1->itemHandle->family()->isA(node2->itemHandle->family())))
                {
                    NodeGraphicsItem * node3 = node2;
                    node2 = node1;
                    node1 = node3;  //swap
                }

                if (node2->itemHandle->family()->isA(node1->itemHandle->family()) ||
                    (node1->itemHandle->data && node1->itemHandle->data->numericalData.contains(tr("Fixed")) &&
                     node1->itemHandle->data->numericalData[tr("Fixed")].value(0,0) > 0))
                {
		
					ModuleConnectionGraphicsItem* mc =
                            MakeModuleConnection(node1,
                                                 node2,
                                                 scene);
                    if (mc)
                    {
		                mc->refresh();
                        CompositeCommand * command = new CompositeCommand( tr("modules connected"),
                                                                           QList<QUndoCommand*>()
                                                                           << (new InsertGraphicsCommand(tr("modules connected"),scene,mc))
                                                                           << mc->command, QList<QUndoCommand*>() << mc->command);
                        if (scene->historyStack)
                        {
                            scene->historyStack->push(command);
                        }
                        else
                        {
                            command->redo();
                            delete command;
                        }
                    }
                }
                else
                {
                    ConsoleWindow::error(tr("These two items cannot be merged. Items must belong to the same family."));
                }
            }
        }

        lineItem.setVisible(false);
        if (lineItem.scene())
            lineItem.scene()->removeItem(&lineItem);
    }

    void ModuleTool::itemsRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& items, QList<ItemHandle*>&)
    {
        ConnectionGraphicsItem * connection = 0;
        for (int i=0; i < items.size(); ++i)
        {
            if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i])) &&
                ModuleConnectionGraphicsItem::isModuleConnectionItem(connection))
            {
                ModuleConnectionGraphicsItem * mc = static_cast<ModuleConnectionGraphicsItem*>(connection);
                if (mc->command)
                {
                    QUndoCommand * reverseCommand = new ReverseUndoCommand("disconnect modules",mc->command,false);
                    if (scene && scene->historyStack)
                        scene->historyStack->push(reverseCommand);
                    else
                    {
                        reverseCommand->redo();
                        delete reverseCommand;
                    }
                }
            }
        }
    }

    void  ModuleTool::mouseDoubleClicked (GraphicsScene * , QPointF , QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers )
    {
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

    ModuleConnectionGraphicsItem * ModuleTool::MakeModuleConnection(NodeGraphicsItem * link1, NodeGraphicsItem * link2,GraphicsScene * scene)
    {
        if (!link1 || !link2) return 0;

        ItemHandle * handle1 = getHandle(link1);
        ItemHandle * handle2 = getHandle(link2);

        if (!handle1 || !handle2 || !handle1->family() || !handle2->family()) return 0;

        NodeGraphicsItem * module1 = VisualTool::parentModule(link1);
        NodeGraphicsItem * module2 = VisualTool::parentModule(link2);

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
            return 0;

        ModuleConnectionGraphicsItem * connection = new ModuleConnectionGraphicsItem;

        QList<ItemHandle*> handles;
        handles << handle1 << handle2;

        MergeHandlersCommand * mergeCommand = new MergeHandlersCommand(tr("items merged"),handles);

        QList<QString> newNames;
        for (int j=0; j < handles.size(); ++j)
            newNames << mergeCommand->newHandle->fullName();

        QList<QUndoCommand*> commands;
        commands += mergeCommand;
        commands += new RenameCommand(tr("name changed"),scene->networkWindow,handles,newNames);
        connection->command = new CompositeCommand(tr("modules connected"),commands);

        ConnectionGraphicsItem::PathVector controlPoints;

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

        connection->pathVectors += controlPoints;
        connection->lineType = ConnectionGraphicsItem::line;
        return connection;
    }
}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
    if (!main) return;

    Tinkercell::ModuleTool * tool = new Tinkercell::ModuleTool;
    main->addTool(tool);

}
