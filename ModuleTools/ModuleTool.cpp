/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This tool handles module connections that merge items from two modules

****************************************************************************/

#include <math.h>
#include <QDir>
#include <QMessageBox>
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
	static QString linkerFileName("/OtherItems/moduleLinker.xml");
    ModuleTool::VisualTool::VisualTool(ModuleTool * tool) : Tool::GraphicsItem(tool)
    {
        moduleTool = tool;
        QString appDir = QCoreApplication::applicationDirPath();
        NodeGraphicsReader reader;
        reader.readXml(&image, appDir + tr("/OtherItems/moduleTool.xml"));
        image.normalize();
        image.scale(40.0/image.sceneBoundingRect().width(),20.0/image.sceneBoundingRect().height());
        image.setToolTip(tr("Module input/output"));
        setToolTip(tr("Module input/output"));
        addToGroup(&image);
    }

    ModuleTool::ModuleTool() : Tool(tr("Module Connection Tool"),tr("Module tools"))
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
            //connect(mainWindow,SIGNAL(modelSaved(NetworkWindow*)),this,SLOT(modelSaved(NetworkWindow*)));
			
			connect(mainWindow,SIGNAL(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)),
                    this,SLOT(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)));

			//connect(mainWindow,SIGNAL(parentHandleChanged(NetworkWindow*, const QList<ItemHandle*>&, const QList<ItemHandle*>&)),
				//	this,SLOT(parentHandleChanged(NetworkWindow*, const QList<ItemHandle*>&, const QList<ItemHandle*>&)));

			//connect(mainWindow,SIGNAL(copyItems(GraphicsScene *, QList<QGraphicsItem*>& , QList<ItemHandle*>& )),
				//	this,SLOT(copyItems(GraphicsScene *, QList<QGraphicsItem*>& , QList<ItemHandle*>& )));

			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),
					this,SLOT(escapeSignal(const QWidget*)));

            connect(mainWindow,SIGNAL(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&)),
					this, SLOT(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&)));

			connect(this,SIGNAL(itemsInsertedSignal(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)),
                    mainWindow,SIGNAL(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)),
                    this, SLOT(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)));

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
                        (node->fileName.contains(linkerFileName)))
                {
                    alreadyLinked = true;
                    break;
                }

                if (alreadyLinked) continue;

                NodeGraphicsItem * module = VisualTool::parentModule(items[i]);

                if (!module) continue;
                NodeGraphicsItem * linker = new NodeGraphicsItem;
				QString appDir = QCoreApplication::applicationDirPath();
				NodeGraphicsReader reader;
				reader.readXml(linker,appDir + linkerFileName);
				linker->normalize();
				setToolTip(QString("Module interface"));

				linker->scale(150.0/linker->sceneBoundingRect().width(),150.0/linker->sceneBoundingRect().width());
				if (linker->boundaryControlPoints.size() > 0)
				{
					for (int j=0; j < linker->boundaryControlPoints.size(); ++j)
						if (linker->boundaryControlPoints[j])
						{
							if (linker->boundaryControlPoints[j]->scene())
								linker->boundaryControlPoints[j]->scene()->removeItem(linker->boundaryControlPoints[j]);
							delete linker->boundaryControlPoints[j];
						}
					linker->boundaryControlPoints.clear();
				}
				
				TextGraphicsItem * linkerText = new TextGraphicsItem(handle);
				linkerText->setPos(linker->pos());
				linkerText->scale(1.5,1.5);
				
                setHandle(linker,handle);

                toInsert += (QGraphicsItem*)linker;
				toInsert += (QGraphicsItem*)linkerText;
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
                        (node->fileName.contains(linkerFileName)) &&
                        (node->scene() == items[i]->scene()) &&
                        scene->isVisible(node))
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
		
	}

    void ModuleTool::itemsInserted(GraphicsScene* , const QList<QGraphicsItem *>& items, const QList<ItemHandle*>& handles)
    {
		for (int i=0; i < handles.size(); ++i)
        {
            ItemHandle * handle = handles[i];

            if (NodeHandle::cast(handle) && handle->family() && !handle->tools.contains(this))
            {
				handle->tools += this;
            }
        }
		
		NodeGraphicsItem * node = 0;
		ItemHandle * handle = 0;
		for (int i=0; i < handles.size(); ++i)
		{
			handle = 0;
			if (handles[i] && handles[i]->isA(tr("Module")))
				handle = handles[i];
			else
			if (handles[i] && handles[i]->parent && handles[i]->parent->isA(tr("Module")))
				handle = handles[i]->parent;
			
			if (handle)
				for (int j=0; j < handle->graphicsItems.size(); ++j)
					if (node = NodeGraphicsItem::cast(handle->graphicsItems[j]))
						adjustLinkerPositions(node);
		}
    }

    void ModuleTool::itemsSelected(GraphicsScene* scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers)
    {
		if (!scene) return;
        QList<QGraphicsItem*> & moving = scene->moving();

        NodeGraphicsItem * node = 0, * module = 0, * linker = 0;
        ItemHandle * handle = 0;

        for (int i=moving.size()-1; i >= 0; --i)
        {
            if ((node = NodeGraphicsItem::topLevelNodeItem(moving[i]))
                && (node->fileName.contains(linkerFileName)))
                {
					if (items.contains(node)) 
						linker = node;
					//moving.removeAt(i);
				}
        }
		
        if (linker && linker->handle() && 
			(handle = linker->handle()->parent) && 
			handle->isA(tr("Module")) &&
			items.size() == 1)
        {
			for (int i=0; i < handle->graphicsItems.size(); ++i)
				if (module = NodeGraphicsItem::cast(handle->graphicsItems[i]))
					break;
		}
		
		if (linker && module)
		{
			QRectF moduleRect = module->sceneBoundingRect();
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
                    if ((child = NodeHandle::cast(handle->children[j])) && child->graphicsItems.size() > 0)
					{
						inside = false;
						for (int k=0; k < child->graphicsItems.size(); ++k)
						{
							if (child->graphicsItems[k] && !scene->isVisible(child->graphicsItems[k]))
								inside = true;
							else
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
							/*for (int k=0; k < child->graphicsItems.size(); ++k)
								if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(child->graphicsItems[k])) &&
									(node->className == ModuleLinkerItem::CLASSNAME))
								{
									(static_cast<ModuleLinkerItem*>(node))->setPosOnEdge();
								}*/
						}
						else
						{
							scene->setParentHandle(child,0);
						}
					}
				
				 for (int j=0; j < handle->graphicsItems.size(); ++j)
                    if (node = NodeGraphicsItem::cast(handle->graphicsItems[j]))
						adjustLinkerPositions(node);
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

                if ( node2->handle()->children.isEmpty() && node1->handle()->children.isEmpty() &&
					(node2->handle()->family()->isA(node1->handle()->family()) ||
                    (node1->handle()->data && node1->handle()->data->numericalData.contains(tr("Fixed")) &&
                     node1->handle()->data->numericalData[tr("Fixed")].value(0,0) > 0))
					)
					{
						makeModuleConnection(node1,node2,scene);
					}
					else
					{
						if (console())
							console()->error(tr("These two items cannot be merged. Items must be of similar family and cannot have any subcomponents."));
					}
            }
		}

        lineItem.setVisible(false);
        if (lineItem.scene())
            lineItem.scene()->removeItem(&lineItem);
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

        QList<QUndoCommand*> commands;
		commands << new InsertGraphicsCommand(tr("module connection"),scene,connection);

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

	void ModuleTool::connectedItems(const QList<ItemHandle*>& items, QList<ItemHandle*>& from, QList<ItemHandle*>& to)
	{
	    QList<QGraphicsItem*> graphicsItems;
	    for (int i=0; i < items.size(); ++i)
            if (items[i])
                for (int j=0; j < items[i]->graphicsItems.size(); ++j)
                    if (!graphicsItems.contains( items[i]->graphicsItems[j] ))
                        graphicsItems << items[i]->graphicsItems[j];
						
		connectedItems(graphicsItems,from,to);
	}

	void ModuleTool::connectedItems(const QList<QGraphicsItem*>& items, QList<ItemHandle*>& from, QList<ItemHandle*>& to)
	{
        ItemHandle * handle, *h1, *h2;
	    NodeGraphicsItem * node, * node1, * node2;
	    QList<ConnectionGraphicsItem*> connections;
	    QList<NodeGraphicsItem*> connectedNodes;

	    for (int i=0; i < items.size(); ++i)
            if ((node = NodeGraphicsItem::cast(items[i])) &&
				ModuleLinkerItem::isModuleLinker(node) &&
                (handle = node->handle()))
            {
                connections = node->connections();
                for (int j=0; j < connections.size(); ++j)
				{
					connectedNodes = connections[j]->nodes();
                    if (connections[j] &&
                        connections[j]->className == tr("module connection") &&
                        connectedNodes.size() == 2)
                    {
                        node1 = connectedNodes[0];
                        node2 = connectedNodes[1];
                        if (node1 &&
                            node2 &&
                            (h1 = node1->handle()) &&
                            (h2 = node2->handle()) &&
                            (h1 != h2))
                            {
                                int x = from.indexOf(h2);

                                while (x > -1)
                                {
                                    h2 = to[x];
                                    x = from.indexOf(h2);
                                }

                                from << h1;
                                to << h2;

                                x = to.indexOf(h1);

                                while (x > -1)
                                {
                                    to[x] = h2;
                                    x = to.indexOf(h1);
                                }
                            }
                    }
				}
            }
	}
	
	void  ModuleTool::mouseDoubleClicked (GraphicsScene * scene, QPointF , QGraphicsItem * item, Qt::MouseButton, Qt::KeyboardModifiers modifier)
    {
		if (!scene || !item || modifier || !mainWindow) return;
		ItemHandle * handle = getHandle(item);
		NodeGraphicsItem * moduleItem, *node;
		if (handle && handle->isA(tr("Module")) && (moduleItem = NodeGraphicsItem::cast(item)))
		{
			QList<QGraphicsItem*> collidingItems = scene->items(moduleItem->sceneBoundingRect()),
								  childItems = handle->allGraphicsItems(),
								  hideItems,
								  allItems = scene->items();
			
			ConnectionGraphicsItem * connection;
			
			for (int i=0; i < collidingItems.size(); ++i)
				if (
					(connection = ConnectionGraphicsItem::cast(collidingItems[i]))
					&&
					!(connection->handle() && connection->handle()->isChildOf(handle))
					)
			{
				if (connection->handle())
					QMessageBox::information(this,tr("Cannot compress module"), 
						tr("This module cannot be compressed due to ") + connection->handle()->fullName());
				else
					QMessageBox::information(this,tr("Cannot compress module"), 
						tr("Please adjust any connections that are intersecting the module before compressing it."));
				return;
			}
			
			for (int i=0; i < childItems.size(); ++i)
			{
				if (moduleItem != childItems[i] &&
					getHandle(childItems[i]) != handle &&
					!((node = NodeGraphicsItem::cast(childItems[i])) && node->className == ModuleLinkerItem::CLASSNAME))
				{
					hideItems << childItems[i];
					//allItems.removeAll(childItems[i]);
				}
			}
			
			if (scene->networkWindow && scene->networkWindow->currentView())
			{
				scene->networkWindow->currentView()->hideItems(hideItems);
				GraphicsView * view = scene->networkWindow->createView(allItems);
				view->showItems(hideItems);
			}
		}
    }
	
	void ModuleTool::adjustLinkerPositions(NodeGraphicsItem * module)
	{
		ItemHandle * handle = getHandle(module);
		
		if (!handle) return;
	
		QList<NodeGraphicsItem*> linkItems;
		QList<TextGraphicsItem*> textItems;
		
		TextGraphicsItem * text;
		NodeGraphicsItem * node;
		
		for (int i=0; i < handle->children.size(); ++i)
			if (handle->children[i])
			{
				for (int j=0; j < handle->children[i]->graphicsItems.size(); ++j)
					if ((node = NodeGraphicsItem::cast(handle->children[i]->graphicsItems[j])) &&
						(node->fileName.contains(linkerFileName)))
						{
							linkItems << node;
							text = 0;
							for (int k=0; k < handle->children[i]->graphicsItems.size(); ++k)
								if ((text = TextGraphicsItem::cast(handle->children[i]->graphicsItems[k])) &&
									text->sceneBoundingRect().intersects(node->sceneBoundingRect()))
									break;
							textItems << text;
						}
			}
		
		if (linkItems.isEmpty()) return;
		
		QRectF rect = module->sceneBoundingRect();
		QPointF pos;
		
		qreal w = 120.0;
		
		for (int i=0; i < linkItems.size(); ++i)
		{
			linkItems[i]->resetTransform();
			linkItems[i]->scale(w/linkItems[i]->sceneBoundingRect().width(),w/linkItems[i]->sceneBoundingRect().width());
		}
		
		if (linkItems.size() > 4)
		{
			int n = (int)((linkItems.size() + 0.5)/ 4.0);
			
			for (int i=0,j=0,k=0; i < linkItems.size(); ++i, ++j)
			{
				if (j > n)
				{
					j = 0;
					++k;
				}
	
				if (k==0)
				{
					linkItems[i]->setPos(rect.right(),rect.top() + (j+1)*rect.height()/(2+n));
					linkItems[i]->rotate(0);
					if (textItems[i])
						textItems[i]->setPos(linkItems[i]->sceneBoundingRect().right(),rect.top() + (j+1)*rect.height()/(2+n));
				}

				if (k==1)
				{
					linkItems[i]->setPos(rect.left() + (j+1)*rect.width()/(2+n),rect.top());
					linkItems[i]->rotate(-90);
					if (textItems[i])
						textItems[i]->setPos(rect.left() + (j+1)*rect.width()/(2+n),linkItems[i]->sceneBoundingRect().top());
				}

				if (k==2)
				{
					linkItems[i]->setPos(rect.left(),rect.top() + (j+1)*rect.height()/(2+n));
					linkItems[i]->rotate(180);
					if (textItems[i])
						textItems[i]->setPos(linkItems[i]->sceneBoundingRect().left()-textItems[i]->sceneBoundingRect().width(),rect.top() + (j+1)*rect.height()/(2+n));
				}

				if (k==3)
				{
					linkItems[i]->setPos(rect.left() + (j+1)*rect.width()/(2+n),rect.bottom());
					linkItems[i]->rotate(90);
					if (textItems[i])
						textItems[i]->setPos(rect.left() + (j+1)*rect.width()/(2+n),linkItems[i]->sceneBoundingRect().top());
				}
			}
		}
		else
		{
			int dt = -90;
			if (linkItems.size() == 2) dt = -180;
			for (int i=0; i < linkItems.size(); ++i)
			{
				if (i==0)
				{
					linkItems[i]->setPos(rect.right(),rect.center().y());
					if (textItems[i])
						textItems[i]->setPos(linkItems[i]->sceneBoundingRect().right(),rect.center().y());
				}	
				if (i==1)
					if (linkItems.size() == 2)
					{
						linkItems[i]->setPos(rect.left(),rect.center().y());
						if (textItems[i])
							textItems[i]->setPos(linkItems[i]->sceneBoundingRect().left()-textItems[i]->sceneBoundingRect().width(),rect.center().y());
					}
					else
					{
						linkItems[i]->setPos(rect.center().x(),rect.top());
						if (textItems[i])
							textItems[i]->setPos(rect.center().x(),linkItems[i]->sceneBoundingRect().top());
					}
				if (i==2)
				{
					linkItems[i]->setPos(rect.left(),rect.center().y());
					if (textItems[i])
						textItems[i]->setPos(linkItems[i]->sceneBoundingRect().left()-textItems[i]->sceneBoundingRect().width(),rect.center().y());
				}
				
				if (i==3)
				{
					linkItems[i]->setPos(rect.center().x(),rect.bottom());
					if (textItems[i])
						textItems[i]->setPos(rect.center().x(),linkItems[i]->sceneBoundingRect().top());
				}
				linkItems[i]->rotate(i*dt);
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
