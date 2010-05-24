/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This tool handles module connections that merge items from two modules

****************************************************************************/

#include <math.h>
#include <QDir>
#include <QToolBar>
#include <QMessageBox>
#include <QButtonGroup>
#include <QDockWidget>
#include <QScrollArea>
#include "ItemFamily.h"
#include "NetworkHandle.h"
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
	static QString interfaceFileName("/OtherItems/moduleInterface.xml");
	static QString linkerClassName("module linker item");
	static QString interfaceClassName("module interface item");
	static QString connectionClassName("module connection item");

    ModuleTool::ModuleTool() : Tool(tr("Module Connection Tool"),tr("Module tools"))
    {
        setPalette(QPalette(QColor(255,255,255,255)));
        setAutoFillBackground(true);
        mode = none;        
        lineItem.setPen(QPen(QColor(255,10,10,255),2.0,Qt::DotLine));
        
        QString appDir = QCoreApplication::applicationDirPath();
        NodeGraphicsReader reader;
        NodeGraphicsItem * image = new NodeGraphicsItem;
        reader.readXml(image, appDir + interfaceFileName);
        image->normalize();
        image->scale(40.0/image.sceneBoundingRect().width(),20.0/image.sceneBoundingRect().height());
        ToolGraphicsItem * toolGraphicsItem = new ToolGraphicsItem(this);
        toolGraphicsItem->addToGroup(image);
        toolGraphicsItem->setToolTip(tr("Module input/output"));
        addGraphicsItem(toolGraphicsItem);
        addAction(QIcon(":/images/moduleInput.png"),tr("Module input/output"),tr("Set selected nodes as interfaces for this module"));
    }
    
    void ModuleTool::select(int)
    {
    	GraphicsScene * scene = currentScene();
    	if (!scene) return;
    	
    	NodeGraphicsReader reader;
        NodeGraphicsItem * image = new NodeGraphicsItem;
        reader.readXml(image, appDir + interfaceFileName);
        image->normalize();
        image->scale(image->defaultSize.width()/image.sceneBoundingRect().width(),image->defaultSize.height()/image.sceneBoundingRect().height());
        
    }
    
    void ModuleTool::createInterface(const QList<NodeGraphicsItem*>& nodes, const QList<NodeGraphicsItem*>& modules)
    {
    	ItemHandle * h1, * h2;
    	NodeGraphicsItem * node1, * node2;
    	for (int i=0; i < nodes.size() && i < modules.size(); ++i)
    		if ((h1 = getHandle(nodes[i])) && (h2 = modules[i]) && h1->isChildOf(h2))
    		{
    			node1 = new NodeGraphicsItem;
    			
    		}
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

			connect(mainWindow,SIGNAL(mouseMoved(GraphicsScene* , QGraphicsItem*, QPointF , Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)),
                    this,SLOT(mouseMoved(GraphicsScene* , QGraphicsItem*, QPointF , Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)));

			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),
					this,SLOT(escapeSignal(const QWidget*)));

            connect(mainWindow,SIGNAL(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)),
					this, SLOT(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)));

			connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)),
                    this, SLOT(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)));

            connect(mainWindow,SIGNAL(itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)),
                    this, SLOT(itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)));

			connect(mainWindow, SIGNAL(itemsAboutToBeRemoved(GraphicsScene *, QList<QGraphicsItem*>& , QList<ItemHandle*>&, QList<QUndoCommand*>& )),
					this, SLOT(itemsAboutToBeRemoved(GraphicsScene *, QList<QGraphicsItem*>& , QList<ItemHandle*>&, QList<QUndoCommand*>& )));

			connect(mainWindow, SIGNAL(parentHandleChanged(NetworkHandle *, const QList<ItemHandle*>&, const QList<ItemHandle*>&)),
					this, SLOT(parentHandleChanged(NetworkHandle *, const QList<ItemHandle*>&, const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			connect(mainWindow,SIGNAL(historyChanged(int)),this,SLOT(historyChanged(int)));

			toolLoaded(mainWindow->tool(tr("Nodes Tree")));

			toolLoaded(mainWindow->tool(tr("Parts and Connections Catalog")));
			toolLoaded(mainWindow->tool(tr("Save and Load")));
        }

        return true;
    }

	void ModuleTool::toolLoaded(Tool * tool)
	{
		if (!tool) return;
		static connected1 = false, connected2 = false, connected3 = false;

		if (tool->name == tr("Nodes Tree") && !connected1)
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

				moduleFamily->graphicsItems += image;
				moduleFamily->pixmap = QPixmap(tr(":/images/module.png"));
				moduleFamily->color = QColor(0,0,255);
				moduleFamily->description = tr("Self-contained subsystem that can be used to build larger systems");
				moduleFamily->textAttributes[tr("Functional description")] = tr("");
				tree->nodeFamilies[moduleFamily->name] = moduleFamily;
				connected1 = true;
			}
		}

		if (tool->name == tr("Parts and Connections Catalog") && !connected2)
		{
			CatalogWidget * catalog = static_cast<CatalogWidget*>(tool);

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
			
			connected2 = true;
		}
		
		if (tool->name == tr("Save and Load") && !connected3)
		{
			LoadSaveTool * loadSaveTool = static_cast<LoadSaveTool*>(tool);
			connect(this,SIGNAL(loadItems(QList<QGraphicsItem*>&, const QString&)),loadSaveTool,SLOT(loadItems(QList<QGraphicsItem*>&, const QString&)));
			connected3 = true;
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

    QPointF ModuleTool::getPoint(QGraphicsItem* module, QPointF scenePos, QGraphicsItem * item)
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

    void ModuleTool::escapeSignal(const QWidget* )
    {
		if (mode == inserting && currentScene())
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
			while (scene->network->symbolsTable.uniqueItems.contains(handle->name) || 
				   scene->network->symbolsTable.uniqueData.contains(handle->name))
				handle->name = tr("mod") + QString::number(++n);

			QString appDir = QApplication::applicationDirPath();
			NodeGraphicsItem * image;
			if (moduleFamily && moduleFamily->graphicsItems.size() > 0 && NodeGraphicsItem::cast(moduleFamily->graphicsItems[0]))
			{
				image = NodeGraphicsItem::cast(moduleFamily->graphicsItems[0])->clone();
			}
			else
			{
				image = new NodeGraphicsItem;
				image->normalize();
				NodeGraphicsReader reader;
				reader.readXml(image, appDir + tr("/NodeItems/Module.xml"));
			}

			image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),
						 image->defaultSize.height()/image->sceneBoundingRect().height());
			image->setPos(point);
			image->adjustBoundaryControlPoints();
			image->setHandle(handle);

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

	void ModuleTool::itemsAboutToBeInserted(GraphicsScene* scene, QList<QGraphicsItem *>& items, QList<ItemHandle*>& handles, QList<QUndoCommand*> commands)
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
					if (h1 && h2)
					{
						oldNames << h1->fullName();
						newNames << h2->fullName();
					}
				}
			}

		RenameCommand * rename = new RenameCommand(scene->network,oldNames,newNames);
		commands << rename;
	}

	void ModuleTool::itemsAboutToBeRemoved(GraphicsScene* scene, QList<QGraphicsItem *>& items, QList<ItemHandle*>& handles, QList<QUndoCommand*> commands)
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
					if (h1 && h2 && )
						commands << new RenameCommand(tr("module connection removed"),scene->network,handles,h2->fullName(),h1->fullName());
				}
			}
		
		QList<QGraphicsItem*> interfaces;
		NodeGraphicsItem * node;
		ItemHandle * handle;
		for (int i=0; i < items.size(); ++i)
			if ((node = NodeGraphicsItem::cast(items[i])) 
				&& (handle = node->handle())
				&& (node->className == linkerClassName || node->className == interfaceClassName))
			{
				for (int j=0; j < handle->graphicsItems.size(); ++j)
					if ((node = NodeGraphicsItem::cast(handle->graphicsItems[j]))
						&& !interfaces.contains(node)
						&& (node->className == linkerClassName || node->className == interfaceClassName))
					{
						interfaces << node;
					}
			}

		if (interfaces.size() > 0)
			commands << new RemoveGraphicsCommand(tr("module interfaces removed"),scene,interfaces);
	}

    void ModuleTool::itemsInserted(NetworkHandle * network, const QList<ItemHandle*>& handles)
	{
    	NetworkWindow * window = network->currentWindow();    	
    	if (!window || !window->handle) return;

    	QStringList oldNames, newNames;
    	for (int i=0; i < handles.size(); ++i)
	    	if (handles[i] && !handles[i]->parent)
    		{
    			oldNames << handles[i]->fullName();
    			handles[i]->setParent(window->handle,false);
    			newNames << handles[i]->fullName();
    			if (!handles[i]->tools.contains(this))
	    			handles[i]->tools += this;
    		}
    	RenameCommand rename(tr(""),network,oldNames,newNames);
    	rename.redo();
    }

    void ModuleTool::itemsMoved(GraphicsScene* scene, const QList<QGraphicsItem*>& items, const QList<QPointF>&, Qt::KeyboardModifiers)
    {
		if (!scene) return;

        NodeGraphicsItem* node;
        ItemHandle * handle;

        for (int i=0; i < items.size(); ++i)
        {
            if ((node = NodeGraphicsItem::cast(items[i])) 
            	&& node->className == linkerClassName 
            	&& (handle = node->handle())
            	&& handle->parent
            	&& handle->parent->isA(tr("Module")))
            {
            	handle = handle->parent;
            	for (int j=0; j < handle->graphicsItems.size(); ++j)
            		if (NodeGraphicsItem::cast(handle->graphicsItems[j]))
            		{
    	        		node->setPos(getPoint(handle->graphicsItems[j],node->scenePos(),node));	
    	        		break;
    	        	}
			}
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

        NodeGraphicsItem * module1 = parentModule(link1);
        NodeGraphicsItem * module2 = parentModule(link2);

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
				emit loadItems(items, filename);
								
				NetworkWindow * window = currentNetwork()->currentWindow();
				if (window && !items.isEmpty())
				{
					GraphicsScene * scene = window->newScene();
					scene->insert(tr("new model"),items);
					scene->fitAll();
				}
			}
		}
	}
	
	QDockWidget * ModuleTool::makeDockWidget(const QString & family)
	{
		QString s = family.toLower();
		s.replace(tr(" "),tr("_"))
		QString dirname = MainWindow::userHome() + tr("/") + s;
		QDir dir(dirname);
		
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
		    QToolButton * button = new QToolButton(toolBar);
		    button->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
		    QString base = fileInfo.baseName();
		    button->setText(base);
		    button->setToolTip(fileInfo.absoluteFilePath();
		    
		    if (QFile::exists(dirname + base + tr(".png")))
			    button->setIcon(QIcon(dirname + base + tr(".png")));
		    else
		    	button->setIcon(QIcon(tr(":/images/module.png")));
		    group->addButton(button,i);
		    layout->addWidget(button);
		}
		
		widget->setLayout(layout);
		widget->setPalette(QPalette(QColor(255,255,255)));
		widget->setAutoFillBackground (true);

		QScrollArea * scrollArea = new QScrollArea;
		scrollArea->setWidget(widget);
		scrollArea->setPalette(QPalette(QColor(255,255,255)));
		scrollArea->setAutoFillBackground (true);
		
		dock->setWidget(scrollArea);
		return dock;
	}

	void ModuleTool::mouseDoubleClicked (GraphicsScene * scene, QPointF , QGraphicsItem * item, Qt::MouseButton, Qt::KeyboardModifiers modifier)
    {
		if (!scene || !scene->network || !item || !mainWindow) return;
		
		ItemHandle * handle = getHandle(item);
		if (handle && handle->family())
		{
			NetworkHandle * network = scene->network;
			GraphicsScene * newScene = network->createScene();
			NetworkWindow * window = newScene->networkWindow;
			if (window)
			{				
				QDockWidget * dock = makeDockWidget(handle->family()->name);
				if (dock)
					window->addDockWidget(Qt::TopDockWidgetArea,dock);
			}
		}
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
									(text->scene() == node->scene()) &&
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
