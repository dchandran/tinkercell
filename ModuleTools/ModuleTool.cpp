/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool handles module connections that merge items from two modules

****************************************************************************/

#include <math.h>
#include <QDir>
#include "ItemHandle.h"
#include "GraphicsScene.h"
#include "ConsoleWindow.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "LoadSaveTool.h"
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
        makeLink->setIcon(QIcon(tr(":/images/module.png")));
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
            connect(mainWindow,SIGNAL(modelSaved(NetworkWindow*)),this,SLOT(modelSaved(NetworkWindow*)));
			
			connect(mainWindow,SIGNAL(parentHandleChanged(NetworkWindow*, const QList<ItemHandle*>&, const QList<ItemHandle*>&)),
					this,SLOT(parentHandleChanged(NetworkWindow*, const QList<ItemHandle*>&, const QList<ItemHandle*>&)));
			
			connect(mainWindow,SIGNAL(copyItems(GraphicsScene *, QList<QGraphicsItem*>& , QList<ItemHandle*>& )),
					this,SLOT(copyItems(GraphicsScene *, QList<QGraphicsItem*>& , QList<ItemHandle*>& )));
			
			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),
					this,SLOT(escapeSignal(const QWidget*)));

            connect(mainWindow,SIGNAL(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&)),
					this, SLOT(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&)));
			
			connect(this,SIGNAL(itemsInsertedSignal(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)),
                    mainWindow,SIGNAL(itemsInsertedSignal(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)),
                    this, SLOT(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)));
					
            connect(mainWindow,SIGNAL(itemsAboutToBeRemoved(GraphicsScene*, QList<QGraphicsItem *>&, QList<ItemHandle*>&)),
                    this, SLOT(itemsAboutToBeRemoved(GraphicsScene*, QList<QGraphicsItem *>&, QList<ItemHandle*>&)));

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
                        (node->className == ModuleLinkerItem::CLASSNAME))
                {
                    alreadyLinked = true;
                    break;
                }

                if (alreadyLinked) continue;

                NodeGraphicsItem * module = VisualTool::parentModule(items[i]);

                if (!module) continue;
                ModuleLinkerItem * linker = new ModuleLinkerItem(module,0,0);
                setHandle(linker,handle);
                
                toInsert += (QGraphicsItem*)linker;
            }
        }

        scene->insert("module interface created",toInsert);
		
		for (int i=0; i < toInsert.size(); ++i)
			if (toInsert[i] && toInsert[i]->scene() != scene)
				delete toInsert[i];
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
                        (node->className == ModuleLinkerItem::CLASSNAME) &&
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
	
	void ModuleTool::itemsAboutToBeInserted(GraphicsScene* scene, QList<QGraphicsItem *>& items, QList<ItemHandle*>& handles)
	{
		for (int i=0; i < handles.size(); ++i)
		{
			QList<ItemHandle*> nodes;
			
			if (handles[i] 
				&& handles[i]->isA(tr("Module"))
				&& handles[i]->allGraphicsItems().size() < handles[i]->allChildren().size())
			{
				QList<ItemHandle*> children = handles[i]->visibleChildren();
				for (int j=0; j < children.size(); ++j)
					if (children[j] && NodeHandle::asNode(children[j]) && !nodes.contains(children[j]))
					{
						nodes << children[j];
					}
			}
			
			QPointF p;
			double w = 10.0;
			NodeGraphicsItem * module = 0;
			
			for (int j=0; j < items.size(); ++j)
			{
				if ((getHandle(items[j]) == handles[i])
					&& (module = NodeGraphicsItem::cast(items[j])))
				{
					p = items[j]->pos();
					w = items[j]->boundingRect().width();
					break;
				}
			}
			
			if (!module) continue;
			
			double theta = 0.0, dtheta = 2 * 3.14159 / nodes.size();
			
			for (int j=0; j < nodes.size(); ++j)
				if (nodes[j] && nodes[j]->family() 
					&& nodes[j]->family()->graphicsItems.size() > 0
					&& NodeGraphicsItem::cast(nodes[j]->family()->graphicsItems[0]))
				{
					NodeGraphicsItem * nodeItem = NodeGraphicsItem::cast(nodes[j]->family()->graphicsItems[0])->clone();
					nodeItem->scale(nodeItem->defaultSize.width()/nodeItem->boundingRect().width(),
									nodeItem->defaultSize.height()/nodeItem->boundingRect().height());
					nodeItem->setPos(module->pos());
					nodeItem->setHandle(nodes[j]);
					
					ModuleLinkerItem * linker = new ModuleLinkerItem(module);
					linker->setHandle(nodes[j]);
					
					linker->setPos( p + w * QPointF(cos(theta),sin(theta)) );
					linker->setPosOnEdge();
					theta += dtheta;
					
					items += nodeItem;
					items += linker;
				}
		}
	}

    void ModuleTool::itemsInserted(GraphicsScene* scene, const QList<QGraphicsItem *>& items, const QList<ItemHandle*>& handles)
    {	
		for (int i=0; i < handles.size(); ++i)
        {
            ItemHandle * handle = handles[i];
			
            if (NodeHandle::asNode(handle) && handle->family() && !handle->tools.contains(this))
            {
				handle->tools += this;
            }
        }
	
		if (!scene) return;
		QList<ConnectionGraphicsItem*> linkerConnections;
		
		NodeGraphicsItem * node = 0;
        ConnectionGraphicsItem * connection = 0;
		
        bool nothingToDo = true;
        for (int i=0; i < items.size(); ++i)
        {
            if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(items[i])) && 
			      node->className == ModuleLinkerItem::CLASSNAME)
            {
                nothingToDo = false;
                break;
            }
        }
		
        if (nothingToDo) return;

        NodeGraphicsItem * module = 0;

        QList<QGraphicsItem*> newLinkers;
		QList<ItemHandle*> newHandles;
		
		QList<QUndoCommand*> allCommands;

        for (int i=0; i < items.size(); ++i)
        {
            if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(items[i])) 
			    && node->className == ModuleLinkerItem::CLASSNAME)
            {
				if (!node->handle())
				{
					continue;
				}
				
				QList<QGraphicsItem*> itemsAt = scene->items(node->sceneBoundingRect());
				for (int j=0; j < itemsAt.size(); ++j)
					if ((module = NodeGraphicsItem::topLevelNodeItem(itemsAt[j])) &&
						module->handle() && module->handle()->isA(tr("module")))
						break;
					else
						module = 0;
				
				if (module)
                {
					ModuleLinkerItem * linker = 0;
					
					QList<ConnectionGraphicsItem*> connections = node->connections();
					
					if (!ModuleLinkerItem::isModuleLinker(node))
					{
						linker = new ModuleLinkerItem(module);
						(* ((NodeGraphicsItem*)linker) ) = (*node);
					}
					else
					{
						linker = static_cast<ModuleLinkerItem*>(node);
						linker->module = module;
					}
					
					ItemHandle * handle = getHandle(node);					
					setHandle(linker,handle);
					linker->setPosOnEdge();
					
					for (int j=0; j < connections.size(); ++j)
						if (connections[j] && !connections[j]->handle())
						{
							//ConsoleWindow::message("class name");
							connections[j]->className = tr("module connection");
						}
					
					if (linker != node)
					{
						for (int j=0; j < connections.size(); ++j)
							if (connections[j])
							{
								connections[j]->replaceNode(node,linker);
							}
							
						newLinkers << linker;
						newHandles << handle;

						allCommands << (new RemoveGraphicsCommand(tr("remove old linker"),scene,(QGraphicsItem*)node))
									<< (new InsertGraphicsCommand(tr("add new linker"),scene,(QGraphicsItem*)linker));
					}
                }
            }
        }
		
		if (allCommands.size() > 0)
		{
			QUndoCommand * command = new CompositeCommand(tr("reconnect modules"),allCommands);
			
			if (scene->historyStack)
			{
				scene->historyStack->push(command);
			}
			else
			{
				command->redo();
				delete command;
			}
			
			emit itemsInsertedSignal(scene,newLinkers,newHandles);
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
                && (node->className == ModuleLinkerItem::CLASSNAME))
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
                        && (node->className == ModuleLinkerItem::CLASSNAME)
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

    void ModuleTool::itemsMoved(GraphicsScene* scene, const QList<QGraphicsItem*>& items, const QList<QPointF>&, Qt::KeyboardModifiers)
    {
		if (!scene) return;
		
        NodeGraphicsItem::ControlPoint * cp;
        NodeGraphicsItem* node;

        QList<ItemHandle*> modules;

        ItemHandle * handle, * child;

        for (int i=0; i < items.size(); ++i)
        {
			handle = 0;
            if ((cp = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(items[i])) && cp->nodeItem)
                handle = cp->nodeItem->handle();
			else
			if (node = qgraphicsitem_cast<NodeGraphicsItem*>(items[i]))
				handle = node->handle();
			
			if (handle && 
				(handle->isA(tr("Module")) || handle->parentOfFamily(tr("Module"))) &&
				!modules.contains(handle))
                modules += handle;
			else
			if (handle && 
				(handle = handle->parentOfFamily(tr("Module"))) &&
				!modules.contains(handle))
				modules += handle;
        }

		bool inside;
        for (int i=0; i < modules.size(); ++i)
        {
            if ( (handle = modules[i]) && handle->isA(tr("Module")) )
            {
                for (int j=0; j < handle->children.size(); ++j)
                    if ((child = NodeHandle::asNode(handle->children[j])) && child->graphicsItems.size() > 0)
					{
						inside = false;
						for (int k=0; k < child->graphicsItems.size(); ++k)
						{
							if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(child->graphicsItems[k])) &&
								!(node->className == ModuleLinkerItem::CLASSNAME))
							{
								for (int l=0; l < handle->graphicsItems.size(); ++l)
								{
									if (handle->graphicsItems[l] && 
										handle->graphicsItems[l]->sceneBoundingRect().contains(node->sceneBoundingRect()))
									{
										inside = true;
										break;
									}
								}
								if (inside)
									break;
							}
						}
						
						if (inside)
						{
							for (int k=0; k < child->graphicsItems.size(); ++k)
								if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(child->graphicsItems[k])) &&
									(node->className == ModuleLinkerItem::CLASSNAME))
								{
									(static_cast<ModuleLinkerItem*>(node))->setPosOnEdge();
								}
						}
						else
						{
							scene->setParentHandle(child,0);
						}	
					}
            }
        }
    }

    void ModuleTool::mouseMoved(GraphicsScene* scene, QGraphicsItem*, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>& items)
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
		
		NodeGraphicsItem::ControlPoint * cp;
        NodeGraphicsItem* node;

        QList<ItemHandle*> modules;

        ItemHandle * handle, * child;

		for (int i=0; i < items.size(); ++i)
        {
			handle = 0;
            if ((cp = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(items[i])) && cp->nodeItem)
                handle = cp->nodeItem->handle();
			else
			if (node = qgraphicsitem_cast<NodeGraphicsItem*>(items[i]))
				handle = node->handle();
			
			if (handle && 
				(handle->isA(tr("Module")) || handle->parentOfFamily(tr("Module"))) &&
				!modules.contains(handle))
                modules += handle;
			else
			if (handle && 
				(handle = handle->parentOfFamily(tr("Module"))) &&
				!modules.contains(handle))
				modules += handle;
        }

        for (int i=0; i < modules.size(); ++i)
        {
            if ( (handle = modules[i]) && handle->isA(tr("Module")) )
            {
                for (int j=0; j < handle->children.size(); ++j)
                    if (child = handle->children[j])
					{
						for (int k=0; k < child->graphicsItems.size(); ++k)
						{
							if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(child->graphicsItems[k])) &&
								(node->className == ModuleLinkerItem::CLASSNAME))
							{
								(static_cast<ModuleLinkerItem*>(node))->setPosOnEdge();
							}
						}
					}
            }
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
                if (node1)// && node1->className == ModuleLinkerItem::CLASSNAME)
                    break;
                else
                    node1 = 0;
            }

            items = scene->items(QRectF(to.rx()-5.0,to.ry()-5.0,10.0,10.0));

            for (int i=0; i < items.size(); ++i)
            {
                node2 = NodeGraphicsItem::topLevelNodeItem(items[i]);
                if (node2)// && node2->className == ModuleLinkerItem::CLASSNAME)
                    break;
                else
                    node2 = 0;
            }

            if (node1 && node2 && node1 != node2 && node1->handle() && node2->handle() && node1->handle() != node2->handle()
                && node1->handle()->family() && node2->handle()->family())
                //&& node1->className == ModuleLinkerItem::CLASSNAME && node2->className == ModuleLinkerItem::CLASSNAME)
            {
		       if (
                        (node2->handle()->data && node2->handle()->data->numericalData.contains(tr("Fixed")) &&
                         node2->handle()->data->numericalData[tr("Fixed")].value(0,0) > 0)

                        ||

                        ( !(node1->handle()->data && node1->handle()->data->numericalData.contains(tr("Fixed")) &&
                            node1->handle()->data->numericalData[tr("Fixed")].value(0,0) > 0) &&
                          !(node2->handle()->family()->isA(node1->handle()->family())) &&
                          node1->handle()->family()->isA(node2->handle()->family())))
                {
                    NodeGraphicsItem * node3 = node2;
                    node2 = node1;
                    node1 = node3;  //swap
                }

                if (node2->handle()->family()->isA(node1->handle()->family()) ||
                    (node1->handle()->data && node1->handle()->data->numericalData.contains(tr("Fixed")) &&
                     node1->handle()->data->numericalData[tr("Fixed")].value(0,0) > 0))
					{
						MakeModuleConnection(node1,node2,scene);
					}
                }
                else
                {
                    ConsoleWindow::error(tr("These two items cannot be merged. Items must belong to the same family."));
                }
            }        

        lineItem.setVisible(false);
        if (lineItem.scene())
            lineItem.scene()->removeItem(&lineItem);
    }

    void ModuleTool::itemsAboutToBeRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& items, QList<ItemHandle*>&)
    {
		if (!scene) return;
		
		QList<QUndoCommand*> commands;		
        ConnectionGraphicsItem * connection = 0;
        for (int i=0; i < items.size(); ++i)
        {	
			if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i])) &&
				connection->className == tr("module connection") && 
                !connection->handle())
			{
				QList<NodeGraphicsItem*> nodes = connection->nodes();
				
				if (nodes.size() != 2 || !nodes[0] || !nodes[1]) return;
				
				ItemHandle * handle1 = nodes[0]->handle(),
						   * handle2 = nodes[1]->handle();
				
				if (!handle1 || handle1 != handle2 || handle1->children.size() != 1) return;
				
				ItemHandle * oldHandle = handle1;
				ItemHandle * newHandle = handle1->children[0];
				
				if (!newHandle) return;
				
				QList<QGraphicsItem*> items;
				
				ItemHandle * module = handle1->parentOfFamily(tr("Module"));
				
				ItemHandle * module2 = 0;
				
				for (int j=0; j < handle1->graphicsItems.size(); ++j)
					if (handle1->graphicsItems[j])
					{
						bool hits = false;
						if (module)
							for (int k=0; k < module->graphicsItems.size(); ++k)
								if (qgraphicsitem_cast<NodeGraphicsItem*>(module->graphicsItems[k]) 
									&&
									module->graphicsItems[k]->sceneBoundingRect().intersects(
										handle1->graphicsItems[j]->sceneBoundingRect()))
									{
										hits = true;
										break;
									}
						if (!hits && !items.contains(handle1->graphicsItems[j]))
						{
							items << handle1->graphicsItems[j];
							ItemHandle * h;
							QList<QGraphicsItem*> intersectingItems = scene->items(
													handle1->graphicsItems[j]->sceneBoundingRect());
							for (int k=0; k < intersectingItems.size(); ++k)
								if ((h = getHandle(NodeGraphicsItem::topLevelNodeItem(intersectingItems[k]))) &&
									h != module && h->isA(tr("Module")))
								{
									if (!module2 || h->isChildOf(module2))
										module2 = h;
								}
						}
						
					}
				
				if (items.isEmpty() || !module2) return;
				
				QList<ItemHandle*> affectedHandles;
				affectedHandles << module2 << module2->visibleChildren();
				
				commands 	<< new AssignHandleCommand(tr("assign handle"),items,newHandle)
							<< new RenameCommand(tr("name changed"),affectedHandles,oldHandle->fullName(),module2->fullName()+tr(".")+newHandle->name)
							<< new SetHandleVisibilityCommand(tr("set visible"), newHandle, true)
							<< new SetParentHandleCommand(tr("set parent"),0, newHandle, module2)
				;
			}
        }
		
		if (!commands.isEmpty())
		{
			QUndoCommand * compositeCommand = new CompositeCommand(tr("modules disconnected"),commands);
					
			if (scene && scene->historyStack)
				scene->historyStack->push(compositeCommand);
			else
			{
				compositeCommand->redo();
				delete compositeCommand;
			}
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

    void ModuleTool::MakeModuleConnection(NodeGraphicsItem * link1, NodeGraphicsItem * link2,GraphicsScene * scene)
    {
        if (!link1 || !link2 || !scene) return;

        ItemHandle * handle1 = getHandle(link1);
        ItemHandle * handle2 = getHandle(link2);

        if (!handle1 || !handle2 || !handle1->family() || !handle2->family()) return;

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
            return;

        ConnectionGraphicsItem * connection = new ConnectionGraphicsItem;
		connection->className = tr("module connection");
		connection->setPen(connection->defaultPen = QPen(QColor(255,100,0,255),3.0));
		
		//ModuleConnectionGraphicsItem * connection = new ModuleConnectionGraphicsItem;
		
        QList<ItemHandle*> handles;
        handles << handle1 << handle2;

        MergeHandlesCommand * mergeCommand = new MergeHandlesCommand(tr("items merged"),scene->networkWindow,handles);

		QList<QUndoCommand*> commands;
		commands 	<< new InsertGraphicsCommand(tr("module connection"),scene,connection)
					<< mergeCommand;
		
		for (int j=0; j < handles.size(); ++j)
			if (handles[j] != mergeCommand->newHandle)
			{
				commands << new SetParentHandleCommand(tr("set parent"), 0 , handles[j], mergeCommand->newHandle);
				commands << new SetHandleVisibilityCommand(tr("set invisible"), handles[j], false);
			}

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

		QUndoCommand * compositeCommand = new CompositeCommand(tr("modules connected"),commands);
		
		if (scene->historyStack)
			scene->historyStack->push(compositeCommand);
		else
		{
			compositeCommand->redo();
			delete compositeCommand;
		}
		
		emit itemsInsertedSignal(scene, QList<QGraphicsItem*>() << connection, QList<ItemHandle*>());
    }
	
	void ModuleTool::parentHandleChanged(NetworkWindow * window, const QList<ItemHandle*>& children, const QList<ItemHandle*>& oldParents)
	{
		if (!window->scene) return;
		NodeGraphicsItem * node;
		for (int i=0; i < children.size() && i < oldParents.size(); ++i)
			if (children[i] && oldParents[i] && oldParents[i]->isA(tr("Module")) && children[i]->parent != oldParents[i])
			{
				for (int j=0; j < children[i]->graphicsItems.size(); ++j)
				{
					if (children[i]->graphicsItems[j] && 
						(node = NodeGraphicsItem::cast(children[i]->graphicsItems[j])) &&
						ModuleLinkerItem::isModuleLinker(node) &&
						(node->scene() == window->scene))
					{
						window->scene->remove(tr("linker removed"),node); 
					}
				}
			}
	}
	
	void ModuleTool::copyItems(GraphicsScene * scene, QList<QGraphicsItem*>& items0, QList<ItemHandle*>& handles)
	{
		if (!scene) return;
		
		QGraphicsItem * item2;
		NodeGraphicsItem * node;
		ItemHandle * handle, * handle2, *moduleCopy;
		
		QList<QGraphicsItem*> items = scene->selected();
		QList<QGraphicsItem*> done;
		
		for (int i=0; i < items.size(); ++i)
		{
			if (NodeGraphicsItem::cast(items[i]) && (handle = getHandle(items[i])) && handle->isA(tr("Module")))
			{
				moduleCopy = 0;
				for (int j=0; j < handles.size(); ++j)
					if (handles[j] && handles[j]->isA(tr("Module")) && handles[j]->fullName() == handle->fullName())
					{
						moduleCopy = handles[j];
						break;
					}
				if (moduleCopy)
				{
					QList<QGraphicsItem*> list = scene->items(items[i]->sceneBoundingRect());
					for (int j=0; j < list.size(); ++j)
					{
						node = NodeGraphicsItem::topLevelNodeItem(list[j]);
						if (!done.contains(node) &&
							ModuleLinkerItem::isModuleLinker(node) &&
							(handle = node->handle()) &&
							(handle->children.size() > 0) &&
							(handle->children[0]))
						{
							done << node;
							handle2 = handle->children[0]->clone();
							handle2->setParent(moduleCopy);
							handles << handle2;
							for (int k=0; k < handle->graphicsItems.size(); ++k)
								if (handle->graphicsItems[k] && list.contains(handle->graphicsItems[k]))
								{
									item2 = cloneGraphicsItem(handle->graphicsItems[k]);
									setHandle(item2,handle2);									
									items0 << item2;
								}
						}
					}
				}
			}
		}
	}
}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
    if (!main) return;

    Tinkercell::ModuleTool * tool = new Tinkercell::ModuleTool;
    main->addTool(tool);

}
