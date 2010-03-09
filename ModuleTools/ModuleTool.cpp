/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This tool handles module connections that merge items from two modules

****************************************************************************/

#include <math.h>
#include <QDir>
#include <QMessageBox>
#include "ItemFamily.h"
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
#include "NodesTree.h"
#include "CatalogWidget.h"
#include "ModuleTool.h"

namespace Tinkercell
{
	static QString linkerFileName("/OtherItems/moduleLinker.xml");
	static QString linkerClassName("module linker item");
	static QString connectionClassName("module connection item");

    ModuleTool::VisualTool::VisualTool(ModuleTool * tool) : Tool::GraphicsItem(tool)
    {
        moduleTool = tool;
        QString appDir = QCoreApplication::applicationDirPath();
        NodeGraphicsReader reader;
        reader.readXml(&image, appDir + linkerFileName);
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
        makeLink->setIcon(QIcon(tr(":/images/lollipop.png")));
        connect(makeLink,SIGNAL(triggered()),this,SLOT(makeLinks()));

        setPalette(QPalette(QColor(255,255,255,255)));
        setAutoFillBackground(true);
        mode = none;
        //lineItem.setPen(QPen(QColor(255,10,10,255),2.0,Qt::DotLine));
    }

	bool ModuleTool::setMainWindow(MainWindow * main)
    {
		Tool::setMainWindow(main);
        if (mainWindow != 0)
        {
			connect(mainWindow,SIGNAL(mousePressed(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
					this,SLOT(sceneClicked(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)),
                    this,SLOT(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)));

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

			connect(mainWindow, SIGNAL(itemsAboutToBeRemoved(GraphicsScene *, QList<QGraphicsItem*>& , QList<ItemHandle*>& )),
					this, SLOT(itemsAboutToBeRemoved(GraphicsScene *, QList<QGraphicsItem*>& , QList<ItemHandle*>& )));

			connect(mainWindow, SIGNAL(parentHandleChanged(NetworkWindow *, const QList<ItemHandle*>&, const QList<ItemHandle*>&)),
					this, SLOT(parentHandleChanged(NetworkWindow *, const QList<ItemHandle*>&, const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			connect(mainWindow,SIGNAL(historyChanged(int)),this,SLOT(historyChanged(int)));

			toolLoaded(mainWindow->tool(tr("Nodes Tree")));

			catalogTool = 0;
			toolLoaded(mainWindow->tool(tr("Parts and Connections Catalog")));
        }

        return true;
    }

	void ModuleTool::toolLoaded(Tool * tool)
	{
		if (!tool) return;

		if (tool->name == tr("Nodes Tree"))
		{
			NodesTree * tree = static_cast<NodesTree*>(tool);
			if (!tree->nodeFamilies.contains(tr("Module")))
			{
				NodeFamily * moduleFamily = new NodeFamily(tr("Module"));

				QString appDir = QCoreApplication::applicationDirPath();
				NodeGraphicsItem * image = new NodeGraphicsItem;
				NodeGraphicsReader reader;
				reader.readXml(image, appDir + tr("/NodeItems/Module.xml"));
				image->normalize();
				image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),
					image->defaultSize.height()/image->sceneBoundingRect().height());

				moduleFamily->graphicsItems += image;
				moduleFamily->pixmap = QPixmap(tr(":/images/module.png"));
				moduleFamily->color = QColor(0,0,255);
				moduleFamily->description = tr("Self-contained subsystem that can be used to build larger systems");
				moduleFamily->textAttributes[tr("Functional description")] = tr("");
				tree->nodeFamilies[moduleFamily->name] = moduleFamily;
			}
		}

		if (tool->name == tr("Parts and Connections Catalog"))
		{
			CatalogWidget * catalog = static_cast<CatalogWidget*>(tool);
			catalogTool = catalog;

			connect(this,SIGNAL(addNewButtons(const QList<QToolButton*>&,const QString&)),
					catalog,SLOT(addNewButtons(const QList<QToolButton*>&,const QString&)));

			connect(catalog,SIGNAL(buttonPressed(const QString&)),
					this,SLOT(moduleButtonPressed(const QString&)));

			QToolButton * moduleButton = new QToolButton;
			moduleButton->setText(tr("New module"));
			moduleButton->setIcon(QIcon(QPixmap(tr(":/images/module.png"))));
			moduleButton->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
			moduleButton->setToolTip(tr("A module is a self-contained subsystem that can be used to build larger systems"));

			QToolButton * linkButton = new QToolButton;
			linkButton->setText(tr("Insert input/output"));
			linkButton->setIcon(QIcon(QPixmap(tr(":/images/lollipop.png"))));
			linkButton->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
			linkButton->setToolTip(tr("Use this to set an item inside a module as an input or ouput for that module"));

			QToolButton * connectButton = new QToolButton;
			connectButton->setText(tr("Connect input/output"));
			connectButton->setIcon(QIcon(QPixmap(tr(":/images/connectmodules.png"))));
			connectButton->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
			connectButton->setToolTip(tr("Use this to connect inputs and ouputs of two modules"));

			emit addNewButtons(
				QList<QToolButton*>() << moduleButton << linkButton << connectButton,
				tr("Modules"));
		}
	}

	void ModuleTool::historyChanged(int)
	{
		TextEditor * textEditor = currentTextEditor();
		if (textEditor && moduleScripts.contains(textEditor))
		{
			GraphicsScene * scene = moduleScripts[textEditor].first;
			ItemHandle * moduleHandle = moduleScripts[textEditor].second;
			ItemHandle * handle;

			if (!scene || !moduleHandle) return;

			QList<TextItem*> newTextItems = cloneTextItems(textEditor->items());
			QList<ItemHandle*> children, parents, oldChildren;

			oldChildren = moduleHandle->children;
			for (int i=0; i < oldChildren.size(); ++i)
			{
				children << oldChildren[i];
				parents << 0;
			}

			for (int i=0; i < newTextItems.size(); ++i)
			{
				handle = getHandle(newTextItems[i]);
				if (handle && !handle->parent)
				{
					children << handle;
					parents << moduleHandle;
				}
			}

			scene->setParentHandle(children,parents);
		}
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

        makeLinks(scene,scene->selected());
	}

	void ModuleTool::makeLinks(GraphicsScene * scene,QList<QGraphicsItem*> & selectedItems)
	{
        QList<QGraphicsItem*> toInsert;

        bool alreadyLinked = false;
        NodeGraphicsItem * node;
        ItemHandle * handle, * moduleHandle;

        QList<QGraphicsItem*> items;
        QList<TextItem*> textItems;

        for (int i=0; i < selectedItems.size(); ++i)
        {
            moduleHandle = getHandle(selectedItems[i]);
            if (moduleHandle && NodeGraphicsItem::cast(selectedItems[i]) && moduleHandle->isA(tr("Module")))
            {
                for (int j=0; j < moduleHandle->children.size(); ++j)
                    if (handle = moduleHandle->children[j])
                    {
                    	alreadyLinked = false;
				        for (int k=0; k < handle->graphicsItems.size(); ++k)
				            if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(handle->graphicsItems[k])) &&
								(node->className == linkerClassName))
				        {
				            alreadyLinked = true;
				            break;
				        }

				        if (!alreadyLinked)
				        {
				            items << moduleHandle->children[j]->graphicsItems;
    	                    textItems << moduleHandle->children[j]->textItems;
    	                }
                    }
            }
        }

        if (!moduleHandle)
        {
        	items = selectedItems;
        	textItems.clear();
        }

        for (int i=0; i < items.size(); ++i)
        {
            handle = getHandle(items[i]);

            if (!NodeHandle::cast(handle)) continue;

            NodeGraphicsItem * module = VisualTool::parentModule(items[i]);

            if (!module) continue;
            NodeGraphicsItem * linker = new NodeGraphicsItem;
			QString appDir = QCoreApplication::applicationDirPath();
			NodeGraphicsReader reader;
			reader.readXml(linker,appDir + linkerFileName);
			linker->normalize();
			linker->className = linkerClassName;
			setToolTip(QString("Module interface"));

			linker->scale(linker->defaultSize.width()/linker->sceneBoundingRect().width(),linker->defaultSize.height()/linker->sceneBoundingRect().height());
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

			setHandle(linker,handle);
			linker->setPos(VisualTool::getPoint(module,items[i]->scenePos(),linker));
            toInsert += (QGraphicsItem*)linker;

			TextGraphicsItem * linkerText = new TextGraphicsItem(handle);
			linkerText->setPos(linker->pos());
			linkerText->scale(1.5,1.5);
			toInsert += (QGraphicsItem*)linkerText;
        }

        for (int i=0; i < textItems.size(); ++i)
        {
            handle = getHandle(textItems[i]);
            if (!NodeHandle::cast(handle)) continue;

            NodeGraphicsItem * module = 0;

            for (int j=0; j < moduleHandle->graphicsItems.size(); ++j)
            	if (module = NodeGraphicsItem::cast(moduleHandle->graphicsItems[j]))
            		break;

            if (!module) continue;

            NodeGraphicsItem * linker = new NodeGraphicsItem;
			QString appDir = QCoreApplication::applicationDirPath();
			NodeGraphicsReader reader;
			reader.readXml(linker,appDir + linkerFileName);
			linker->normalize();
			linker->className = linkerClassName;
			setToolTip(QString("Module interface"));

			linker->scale(linker->defaultSize.width()/linker->sceneBoundingRect().width(),linker->defaultSize.height()/linker->sceneBoundingRect().height());
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

			setHandle(linker,handle);
			linker->setPos(VisualTool::getPoint(module,module->scenePos(),linker));
            toInsert += (QGraphicsItem*)linker;

			TextGraphicsItem * linkerText = new TextGraphicsItem(handle);
			linkerText->setPos(linker->pos());
			linkerText->scale(1.5,1.5);
			toInsert += (QGraphicsItem*)linkerText;
        }

        scene->insert("module interface created",toInsert);

		for (int i=0; i < toInsert.size(); ++i)
			if (toInsert[i] && toInsert[i]->scene() != scene)
				delete toInsert[i];
    }

    QPointF ModuleTool::VisualTool::getPoint(QGraphicsItem* module, QPointF scenePos, QGraphicsItem * item)
    {
    	NodeGraphicsItem * linker = NodeGraphicsItem::cast(item);

        if (!linker || !module) return scenePos;

		linker->resetTransform();

        linker->scale(linker->defaultSize.width()/linker->sceneBoundingRect().width(),linker->defaultSize.height()/linker->sceneBoundingRect().height());

        qreal w = 0;

        if (linker->sceneBoundingRect().width() > linker->sceneBoundingRect().height())
            w = linker->sceneBoundingRect().width();
        else
            w = linker->sceneBoundingRect().height();

        QPointF point = pointOnEdge(module->sceneBoundingRect(), scenePos);
		QPointF diff = point - module->scenePos();
        if (diff.rx()*diff.rx() > diff.ry()*diff.ry())
            point.ry() = scenePos.ry();
        else
            point.rx() = scenePos.rx();

        double angle = 0.0;

        if (point.ry() != scenePos.ry())
        {
            if (diff.ry() > 0)
            {
                point.ry() += w/2.0;
                angle = 90.0;
            }
            else
            {
                point.ry() -= w/2.0;
                angle = -90.0;
            }
        }
        else
        if (point.rx() != scenePos.rx())
        {
            if (diff.rx() < 0)
            {
                point.rx() -= w/2.0;
                angle = 180.0;
            }
            else
            {
                point.rx() += w/2.0;
            }
        }

        QTransform t = linker->transform();
        double sinx = sin(angle * 3.14/180.0),
               cosx = cos(angle * 3.14/180.0);
        QTransform rotate(cosx, sinx, -sinx, cosx, 0, 0);
        linker->setTransform(t * rotate);

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

            if (!handle) continue;

            if (handle->isA(tr("Module")))
            {
                Tool::GraphicsItem::visible(b);
                return;
            }

            if (handle && handle->isA("node"))
            {
                alreadyLinked = false;
                for (int j=0; j < handle->graphicsItems.size(); ++j)
                    if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(handle->graphicsItems[j])) &&
                        (node->className == linkerClassName) &&
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

    void ModuleTool::escapeSignal(const QWidget* )
    {
		if (mode == inserting && currentScene())
			currentScene()->useDefaultBehavior = true;
		mode = none;
		for (int i=0; i < selectedItems.size(); ++i)
			selectedItems[i]->resetPen();
		selectedItems.clear();
    }

	void ModuleTool::sceneClicked(GraphicsScene *scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
	{
		if (mode == none || button == Qt::RightButton || !scene || !scene->symbolsTable || scene->useDefaultBehavior) return;

		if (mode == inserting)
		{
			Tool * tool = mainWindow->tool(tr("Nodes Tree"));
			if (!tool) return;

			NodesTree * nodesTree = static_cast<NodesTree*>(tool);

			if (!nodesTree->nodeFamilies.contains(tr("Module"))) return;

			NodeHandle * handle = new NodeHandle(tr("mod1"));
			NodeFamily * moduleFamily = nodesTree->nodeFamilies.value(tr("Module"));
			handle->setFamily(moduleFamily);

			int n = 1;
			while (scene->symbolsTable->handlesFullName.contains(handle->name))
				handle->name = tr("mod") + QString::number(++n);

			QString appDir = QApplication::applicationDirPath();
			NodeGraphicsItem * image;
			if (moduleFamily->graphicsItems.size() > 0)
			{
				image = new NodeGraphicsItem(moduleFamily->graphicsItems[0]);
			}
			else
			{
				image = new NodeGraphicsItem;
				NodeGraphicsReader reader;
				reader.readXml(image, appDir + tr("/NodeItems/Module.xml"));
				image->normalize();
				image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),
					image->defaultSize.height()/image->sceneBoundingRect().height());
			}
			image->setHandle(handle);
			image->setPos(point);

			scene->insert(handle->name + tr(" inserted"),image);

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
			makeLinks(scene,nodeItems);
			return;
		}

		if (mode == connecting && nodeItems.size() == 1)
		{
			selectedItems << NodeGraphicsItem::cast(nodeItems[0]);
			selectedItems.last()->setPen(QPen(QBrush(QColor(255,0,0)),10));

			if (selectedItems.size() == 2)
			{
				ItemHandle * handle1 = selectedItems[0]->handle(),
								* handle2 = selectedItems[1]->handle();

				if (!handle1 || !handle2 || !handle1->family() || !handle2->family())
				{
					QMessageBox::information(this, tr("Cannot connect"), tr("These objects do not belong in any family"));
					return;
				}

				if (!(handle1->family()->isA(handle2->family()) || handle2->family()->isA(handle1->family())))
				{
					QMessageBox::information(this,
						tr("Cannot connect"),
						handle1->name + tr(" (") + handle1->family()->name + tr(") and ") +
						handle2->name + tr(" (") + handle2->family()->name + tr(") are different types of objects")	);
					return;
				}

				makeModuleConnection(selectedItems[0],selectedItems[1],scene);
				for (int i=0; i < selectedItems.size(); ++i)
					selectedItems[i]->resetPen();
				selectedItems.clear();
			}
		}
	}

	void ModuleTool::itemsAboutToBeInserted(GraphicsScene* scene, QList<QGraphicsItem *>& items, QList<ItemHandle*>& handles)
	{

	}

    void ModuleTool::itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem *>& items, const QList<ItemHandle*>& handles)
    {
        if (!scene) return;

		for (int i=0; i < handles.size(); ++i)
        {
            ItemHandle * handle = handles[i];

            if (NodeHandle::cast(handle) && handle->family() && !handle->tools.contains(this))
            {
				handle->tools += this;
            }
        }

		NodeGraphicsItem * linker = 0;
		ConnectionGraphicsItem * connection = 0;

		for (int i=0; i < items.size(); ++i)
		{
			linker = NodeGraphicsItem::cast(items[i]);
			if (linker && linker->className == linkerClassName && linker->boundaryControlPoints.size() > 0)
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

			if (!linker)
			{
				connection = ConnectionGraphicsItem::cast(items[i]);
				if (connection && connection->className == connectionClassName)
				{
					QList<NodeGraphicsItem*> nodes = connection->nodes();
					if (nodes.size() == 2 && nodes[0] && nodes[1] && nodes[0]->handle() && nodes[1]->handle())
					{
						QString s = nodes[0]->handle()->fullName();
						s += tr(" is the same as ");
						s += nodes[1]->handle()->fullName();
						connection->setToolTip(s);
					}
				}
			}
		}

		//graphics view adjustments

		GraphicsView * currentView = scene->currentView();
        if (moduleViews.contains(currentView))
        {
            ItemHandle * moduleHandle = moduleViews[currentView];
            if (moduleHandle)
                scene->setParentHandle(handles,moduleHandle);
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
                && (node->className == linkerClassName))
                {
					if (items.contains(node))
						linker = node;
					//moving.removeAt(i);
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
				handle->isA(tr("Module")) &&
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
                    if (child = NodeHandle::cast(handle->children[j]))
					{
						inside = !child->textItems.isEmpty() || child->graphicsItems.isEmpty();
						if (!inside)
                            for (int k=0; k < child->graphicsItems.size(); ++k)
                            {
                                if (child->graphicsItems[k] && !scene->isVisible(child->graphicsItems[k]))
                                    inside = true;
                                else
                                    if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(child->graphicsItems[k]))
                                        && node->className != linkerClassName)
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
									(node->className == linkerClassName))
								{
									for (int l=0; l < modules[i]->graphicsItems.size(); ++l)
										if (NodeGraphicsItem::cast(modules[i]->graphicsItems[l]))
											node->setPos(VisualTool::getPoint(modules[i]->graphicsItems[l],node->scenePos(),node));
								}
						}
					}
            }
        }
    }
	void ModuleTool::itemsAboutToBeRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& items, QList<ItemHandle*>& handles)
	{
		if (!scene) return;

		NodeGraphicsItem * node = 0;
		for (int i =0; i < handles.size(); ++i)
			for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
			{
				if ( (node = NodeGraphicsItem::cast(handles[i]->graphicsItems[j])) &&
						(node->className == linkerClassName) &&
						!items.contains(node))
						items << node;
			}
	}

	void ModuleTool::parentHandleChanged(NetworkWindow * net, const QList<ItemHandle*> & handles, const QList<ItemHandle*> & parents)
	{
		if (!net->scene) return;

		NodeGraphicsItem * node = 0;
		QList<QGraphicsItem*> items, items2, linkers;

		for (int i =0; i < handles.size(); ++i)
			for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
			{
				if ( (node = NodeGraphicsItem::cast(handles[i]->graphicsItems[j])) &&
						(node->className == linkerClassName) &&
						!(parents[i] && parents[i]->isA(tr("Module"))) &&
						!items.contains(node))
						items << node;
			}
		if (!items.isEmpty())
			net->scene->remove(tr("Links removed"), items);

        items.clear();
        QList<GraphicsView*> views = net->views();
        ItemHandle * module = 0, * handle = 0;

        for (int i=0; i < handles.size() && i < parents.size(); ++i)
            if (handles[i] && handles[i]->parent != parents[i])
            {
                items2 = handles[i]->allGraphicsItems();
                linkers.clear();
                for (int j=0; j < items2.size(); ++j)
                    if ((node = NodeGraphicsItem::cast(items2[j])) && node->className == linkerClassName )
                        linkers << items2[j];
                    else
                        items << items2[j];
/*
                for (int j=0; j < linkers.size(); ++j)
                {
                    handle = getHandle(linkers[j]);
                    if (handle)
                    {
                        QRectF rect = linkers[j]->sceneBoundingRect().adjusted(-10,-10,10,10);
                        for (int k=0; k < handle->graphicsItems.size(); ++k)
                            if (TextGraphicsItem::cast(handle->graphicsItems[k]) &&
                                rect.intersects(handle->graphicsItems[k]->sceneBoundingRect()))
                                {
                                    items.removeAll(handle->graphicsItems[k]);
                                    linkers << handle->graphicsItems[k];
                                }
                    }
                }*/

                module = handles[i]->parent;
                if (module && module->isA(tr("Module")) && moduleHandles.contains(module)) //new parent is module
                {
                    GraphicsView * otherView = moduleHandles[module];
                    for (int j=0; j < views.size(); ++j)
                    {
                        if (views[j] && views[j] != otherView)
                            views[j]->hideItems(items);
                    }
                }

                module = parents[i];
                if (module && module->isA(tr("Module")) && moduleHandles.contains(module)) //new parent is module
                {
                    //items << linkers;
                    GraphicsView * otherView = moduleHandles[module];
                    for (int j=0; j < views.size(); ++j)
                        if (views[j])
                        {
                            if (views[j] == otherView)
                                views[j]->hideItems(items);
                            else
                                views[j]->showItems(items);
                        }
                }
            }
	}

/*
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
    }*/

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
		connection->className = connectionClassName;
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

	void ModuleTool::connectedItems(const QList<ItemHandle*>& items, QList<ItemHandle*>& to, QList<ItemHandle*>& from)
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
                (handle = node->handle()))
            {
                connections = node->connections();
                for (int j=0; j < connections.size(); ++j)
				{
					connectedNodes = connections[j]->nodes();
                    if (connections[j] &&
                        connections[j]->className == connectionClassName &&
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

	void ModuleTool::createView()
	{
		GraphicsScene * scene = currentScene();
		if (scene && scene->selected().size() == 1)
			createView(scene, scene->selected().at(0));
	}

	void ModuleTool::createView(GraphicsScene * scene, QGraphicsItem * item)
	{
		if (!scene || !mainWindow || !item) return;

		ItemHandle * handle = getHandle(item);
		NodeGraphicsItem * moduleItem, *node;
		if (handle && handle->isA(tr("Module")) && (moduleItem = NodeGraphicsItem::cast(item)))
		{
			QList<QGraphicsItem*> childItems = handle->allGraphicsItems();
			if (childItems.isEmpty())
			{
				if (!handle->textItems.isEmpty())
				{
					QList<ItemHandle*> handles;
					handles << handle->allChildren();

					TextEditor * newEditor = mainWindow->newTextWindow();
					newEditor->networkWindow->popOut();
					moduleScripts[ newEditor ] = QPair<GraphicsScene*,ItemHandle*>(scene,handle);
					emit createTextWindow(newEditor, handles);
				}
				return;
			}

			QList<QGraphicsItem*> collidingItems = scene->items(moduleItem->sceneBoundingRect().adjusted(-5,-5,5,5)),
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
					!((node = NodeGraphicsItem::cast(childItems[i])) && node->className == linkerClassName))
				{
					hideItems << childItems[i];
				}
			}

			if (scene->networkWindow && scene->networkWindow->currentView())
			{
				scene->networkWindow->currentView()->hideItems(hideItems);
				GraphicsView * view = scene->networkWindow->createView(allItems);
				view->showItems(hideItems);

				moduleViews[view] = handle;
				moduleHandles[handle] = view;
			}
		}
	}

	void  ModuleTool::mouseDoubleClicked (GraphicsScene * scene, QPointF , QGraphicsItem * item, Qt::MouseButton, Qt::KeyboardModifiers modifier)
    {
		if (!scene || !item || !modifier || !mainWindow) return;
		createView(scene,item);
    }


	void ModuleTool::moduleButtonPressed(const QString& name)
	{
		GraphicsScene * scene = currentScene();
		if (!mainWindow || !scene) return;

		if (name == tr("New module"))
			mode = inserting;

		if (name == tr("Insert input/output"))
			mode = linking;

		if (name == tr("Connect input/output"))
			mode = connecting;

		if (mode != none)
			scene->useDefaultBehavior = false;
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
						(node->className == linkerClassName))
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
