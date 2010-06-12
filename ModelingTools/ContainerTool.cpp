/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 A tool for displaying all the handles (as a tree) and their attributes. This tool
 also handles move events where an item is moved into a module or Compartment

****************************************************************************/

#include "GraphicsScene.h"
#include "ConsoleWindow.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "CollisionDetection.h"
#include "ContainerTool.h"
#include <QtDebug>

namespace Tinkercell
{
    CompartmentTool::CompartmentTool() : Tool(tr("Compartment Tool"),tr("Modeling"))
    {
        treeView = new QTreeView(this);
        treeView->setAlternatingRowColors(true);

        treeDelegate = new ContainerTreeDelegate(treeView);
        treeView->setItemDelegate(treeDelegate);

        QFont font = treeView->font();
        font.setPointSizeF( font.pointSizeF() * 1.2 );
        treeView->setFont(font);
        connect(treeView,SIGNAL(doubleClicked(const QModelIndex&)),this,SLOT(indexSelected(const QModelIndex&)));

        QVBoxLayout * layout = new QVBoxLayout;
        layout->addWidget(treeView);
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
        setLayout(layout);
    }

    CompartmentTool::~CompartmentTool()
    {
        if (treeDelegate)
            delete treeDelegate;

		if (treeView && treeView->model())
            delete treeView->model();
    }

    bool CompartmentTool::setMainWindow(MainWindow * main)
    {
        Tool::setMainWindow(main);
        if (mainWindow)
        {
            connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
                    this,SLOT(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));

            connect(mainWindow,SIGNAL(itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<QPointF>&)),
                    this,SLOT(itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<QPointF>&)));

            connect(mainWindow,SIGNAL(historyChanged(int)),this,SLOT(updateTree(int)));

            connect(mainWindow,SIGNAL(windowChanged(NetworkWindow*, NetworkWindow*)),
                    this,SLOT(windowChanged(NetworkWindow*, NetworkWindow*)));

			connect(mainWindow,SIGNAL(networkClosed(NetworkHandle *)),this,SLOT(windowClosed(NetworkHandle *)));

//          connect(this,SIGNAL(parentHandleChanged(NetworkHandle*, const QList<ItemHandle*>&, const QList<ItemHandle*>&)),
   //                 mainWindow,SIGNAL(parentHandleChanged(NetworkHandle*, const QList<ItemHandle*>&, const QList<ItemHandle*>&)));

            windowChanged(0,mainWindow->currentWindow());

            treeView->setEditTriggers ( QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed );

            treeView->setColumnWidth(0,30);
            treeView->setColumnWidth(1,50);

			setWindowTitle(tr("Model summary"));
			setWindowIcon(QIcon(tr(":/images/monitor.png")));
			mainWindow->addToolWindow(this, MainWindow::defaultToolWindowOption, Qt::RightDockWidgetArea);

            connectCollisionDetector();
            connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

            return true;
        }
        return false;
    }

    void CompartmentTool::toolLoaded(Tool*)
    {
        connectCollisionDetector();
    }

    void CompartmentTool::connectCollisionDetector()
    {
        static bool alreadyConnected = false;
        if (alreadyConnected || !mainWindow) return;

        if (mainWindow->tool(tr("Collision Detection")))
        {
            QWidget * widget = mainWindow->tool(tr("Collision Detection"));
            CollisionDetection * collisionDetection = static_cast<CollisionDetection*>(widget);
            if (collisionDetection)
            {
                alreadyConnected = true;
                connect(collisionDetection,SIGNAL(nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& )),
                        this, SLOT( nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& )));
            }
        }
    }

	void CompartmentTool::windowClosed(NetworkHandle *)
	{
		treeView->setUpdatesEnabled(false);

        if (treeView->model())
        {
            delete (treeView->model());
			treeView->setModel(0);
        }
	}

    void CompartmentTool::windowChanged(NetworkWindow * , NetworkWindow * )
    {
        treeView->setUpdatesEnabled(false);

        if (treeView->model())
        {
            delete (treeView->model());
			treeView->setModel(0);
        }

		ContainerTreeModel * model = 0;

		NetworkHandle * net = currentNetwork();

		if (net)
			model = new ContainerTreeModel(net);

        treeView->setModel(model);

        treeView->setSortingEnabled(true);
        treeView->sortByColumn(0,Qt::DescendingOrder);

        treeView->setUpdatesEnabled(true);
    }

    void CompartmentTool::updateTree(int)
    {
        if (!mainWindow) return;
        NetworkHandle * net = mainWindow->currentNetwork();
        if (!net) return;

        treeView->setUpdatesEnabled(false);

        if (treeView->model())
        {
            ContainerTreeModel * model = static_cast<ContainerTreeModel*>(treeView->model());
            model->reload(net);            
        }
        else
        {
            ContainerTreeModel * model = new ContainerTreeModel(net);
            treeView->setModel(model);
        }

        treeView->setSortingEnabled(true);
        //treeView->sortByColumn(0,Qt::DescendingOrder);

        treeView->setUpdatesEnabled(true);
    }

    void CompartmentTool::indexSelected(const QModelIndex& index)
    {
        if (!mainWindow) return;
        /*GraphicsScene * scene = mainWindow->currentScene();
        if (!scene) return;

        if (index.isValid())
        {
            ContainerTreeItem *item = static_cast<ContainerTreeItem*>(index.internalPointer());
            if (item && item->handle())
            {
                scene->select(item->handle()->graphicsItems);
            }
        }*/
    }

    void CompartmentTool::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF , Qt::KeyboardModifiers )
    {
        if (!scene || items.isEmpty()) return;

        ItemHandle * handle = 0, *child = 0;
        ConnectionGraphicsItem * connection = 0;

        for (int i=0; i < items.size(); ++i)
        {
            if (qgraphicsitem_cast<NodeGraphicsItem*>(items[i])
                && (handle = getHandle(items[i]))
                && handle->isA(tr("Compartment")))
            {
				QRectF sceneBoundingRect = items[i]->sceneBoundingRect().adjusted(-10,-10,10,10);
                QList<ItemHandle*> list = handle->children;
                for (int j=0; j < list.size(); ++j)
                {
                    if (list[j])
                    {
                        child = list[j];
                        for (int k=0; k < child->graphicsItems.size(); ++k)
							if (child->graphicsItems[k] && child->graphicsItems[k]->scene() == scene)
							{
								if (child->graphicsItems[k] != items[i] &&
									!scene->moving().contains(child->graphicsItems[k]) &&
									sceneBoundingRect.intersects(child->graphicsItems[k]->sceneBoundingRect()))
								{
									if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(child->graphicsItems[k])))
									{
										QList<QGraphicsItem*> controlPoints = connection->controlPointsAsGraphicsItems();
										for (int l=0; l < controlPoints.size(); ++l)
											if (!scene->moving().contains(controlPoints[l]))
												scene->moving() += controlPoints[l];
									}
									else
									{
										scene->moving() += child->graphicsItems[k];
									}
								}
								list += child->children;
							}
                    }
                }
            }
        }
    }

    bool CompartmentTool::connectionInsideRect(ConnectionGraphicsItem* connection, const QRectF& rect, bool all)
    {
        if (!connection) return false;

        QList<QGraphicsItem*> nodes = connection->nodesAsGraphicsItems();

        for (int i=0; i < nodes.size(); ++i)
        {
            if (!all && rect.contains(nodes[i]->sceneBoundingRect()))
                return true;

            if (all && nodes[i] && !rect.contains(nodes[i]->sceneBoundingRect()))
                return false;
        }

        return all;
    }

    void CompartmentTool::itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>& handleList)
    {
        if (!mainWindow || !scene || !scene->network) return;
        ItemHandle * handle = 0, * parentHandle = 0;

        QList<ItemHandle*> handles, newParents, newChildren, specialCaseChildren, specialCaseParents;

        QList<QGraphicsItem*> itemsAt;

        ConnectionGraphicsItem * connection;

        for (int i=0; i < items.size(); ++i)
            if (connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i]))
            {
		        QList<NodeGraphicsItem*> nodes = connection->nodes();

		        for (int j=0; j < nodes.size(); ++j)
		            if (nodes[j])
		            {
				        handle = nodes[j]->handle();
				        if (handle)
				        {
				            parentHandle = handle->parentOfFamily(tr("Compartment"));
				            if (parentHandle && !handles.contains(parentHandle))
				            {
				                handles << parentHandle;
				            }
				        }
				    }
		    }

        for (int i=0; i < handleList.size(); ++i)
        {
            handle = handleList[i];

            if (handle && handle->family() && !handles.contains(handle) &&
                handle->family()->isA(tr("Compartment")))
                handles << handle;
        }

        ItemHandle * child = 0;

        QList<QGraphicsItem*> itemsToRename;
        QList<QString> newNames;

        QString temp;

        for (int k=0; k < handles.size(); ++k)
        {
            newChildren.clear();
            itemsToRename.clear();
            newNames.clear();

            handle = handles[k];
            if (!handle) continue;

            bool contained = true, contained0 = true;
            QRectF childRect;

            for (int i=0; i < items.size(); ++i)
            {
                child = getHandle(items[i]);

                if (child && child != handle && !handle->children.contains(child))
                {
                    contained = true;

                    for (int j=0; j < child->graphicsItems.size(); ++j)
                    {
                        if (qgraphicsitem_cast<TextGraphicsItem*>(child->graphicsItems[j])) continue;

                        connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(child->graphicsItems[j]);

                        childRect = child->graphicsItems[j]->sceneBoundingRect();
                        contained0 = false;
                        for (int l=0; l < handle->graphicsItems.size(); ++l)
							if (handle->graphicsItems[l])
							{
								if (connection)
								{
									if (connectionInsideRect(connection,handle->graphicsItems[l]->sceneBoundingRect()))
									{
										contained0 = true;
										break;
									}
									else
										if (connectionInsideRect(connection,handle->graphicsItems[l]->sceneBoundingRect(),false))
										{
											if (child && !specialCaseChildren.contains(child))
											{
												specialCaseChildren += child;
												specialCaseParents += handle;
											}
										}
								}
								else
									if (handle->graphicsItems[l]->sceneBoundingRect().contains(childRect))
									{
										contained0 = true;
										break;
									}
							}
                        if (!contained0)
                        {
                            contained = false;
                            break;
                        }
                    }

                    if (contained)
                    {
                        newChildren += child;
                        newParents += handle;
                        temp = child->name;
                        temp = scene->network->makeUnique(temp);
                        if (temp != child->name)
                        {
                            itemsToRename += items[i];
                            newNames += handle->fullName() + tr(".") + temp;
                        }
                    }
                }
            }
        }

        if (!newChildren.isEmpty())
            scene->network->setParentHandle(newChildren,newParents);
		
        newChildren << specialCaseChildren;
        newParents << specialCaseParents;

        if (!newChildren.isEmpty())
        {
            QList<ItemHandle*> parentItems;
            while (parentItems.size() < newChildren.size()) parentItems << handle;

            adjustRates(scene, newChildren, parentItems);
        }
    }

    void CompartmentTool::nodeCollided(const QList<QGraphicsItem*>& movingItems0, NodeGraphicsItem * nodeHit, const QList<QPointF>& )
    {
        if (!mainWindow || !nodeHit) return;

        GraphicsScene * scene = mainWindow->currentScene();

        if (!scene || !scene->network) return;

        ItemHandle * handle = getHandle(nodeHit);

        //if items are placed into a Compartment or module...
        if (!handle || !handle->isA(tr("Compartment")))
            return;

		ConnectionGraphicsItem::ControlPoint * cpt = 0;
        QList<ItemHandle*> newChildren;
        ItemHandle * child = 0;

        QList<QGraphicsItem*> itemsToRename;
        QList<QString> newNames;

        QString temp;

        bool stillWithParent;

        NodeGraphicsItem * node = 0;

        QList<QGraphicsItem*> movingItems;

		for (int i=0; i < movingItems0.size(); ++i)
            if (node = qgraphicsitem_cast<NodeGraphicsItem*>(movingItems0[i]))
				movingItems << node;

		if (movingItems.isEmpty())
		{
			QList<QGraphicsItem*> insersectingItems = scene->items(nodeHit->sceneBoundingRect());
			for (int i=0; i < insersectingItems.size(); ++i)
				if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(insersectingItems[i])) && !movingItems.contains(node))
					movingItems << node;
		}

        for (int i=0; i < movingItems.size(); ++i)
            if (node = qgraphicsitem_cast<NodeGraphicsItem*>(movingItems[i]))
            {
				movingItems << node->connectionsAsGraphicsItems();
			}

        ConnectionGraphicsItem* connection;
        QRectF hitRect = nodeHit->sceneBoundingRect();

        for (int i=0; i < movingItems.size(); ++i)
        {
            if (!movingItems[i]) continue;

            QRectF itemRect = movingItems[i]->sceneBoundingRect();
            if (itemRect.width() >= hitRect.width() || itemRect.height() >= hitRect.height()) continue;

            if (qgraphicsitem_cast<TextGraphicsItem*>(movingItems[i])) continue;
            connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(movingItems[i]);

            if (connection && !hitRect.contains(connection->sceneBoundingRect())) continue;

            if ((cpt = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(movingItems[i])) &&
                (cpt->connectionItem) &&
                (hitRect.contains(cpt->connectionItem->sceneBoundingRect())))
				{
					child = getHandle(cpt->connectionItem);
				}
				else
				{
					child = getHandle(movingItems[i]);
				}
            if (child && child != handle && !handle->children.contains(child) && !handle->isChildOf(child))
            {
                stillWithParent = false;
                
                if (child->parent && !stillWithParent)
                {
	                for (int j=0; j < child->graphicsItems.size(); ++j)
	                {
	                    if (child->graphicsItems[j])
						{
							for (int k=0; k < child->parent->graphicsItems.size(); ++k)
							{
								if (child->parent->graphicsItems[k] &&
									child->parent->graphicsItems[k]->sceneBoundingRect().contains(child->graphicsItems[j]->sceneBoundingRect()))
								{
									stillWithParent = true;
									break;
								}
							}
						}
	                    if (stillWithParent)
	                        break;
                	}
                }

                if (!stillWithParent)
                {
                    newChildren += child;
                    temp = child->name;
                    temp = scene->network->makeUnique(temp);
                    if (temp != child->name)
                    {
                        itemsToRename += movingItems[i];
                        newNames += handle->fullName() + tr(".") + temp;
                    }
                }
            }
        }

        if (handle)
        {
            if (!newChildren.isEmpty())
            {
                scene->network->setParentHandle(newChildren,handle);

                QList<ItemHandle*> parentItems;
                while (parentItems.size() < newChildren.size()) parentItems << handle;

                adjustRates(scene, newChildren, parentItems);
            }
            sendToBack(nodeHit,scene);
        }
    }

    void CompartmentTool::sendToBack(QGraphicsItem* item, GraphicsScene * scene)
    {
        if (item && scene)
        {
            double z = 0;

            QList<QGraphicsItem*> targetItems;
            QList<double> zvalues;

            QRectF rect = item->sceneBoundingRect();
            QList<QGraphicsItem*> items = scene->items(rect);
            items.removeAll(item);
            if (item->parentItem())
                items.removeAll(item->parentItem());

            for (int i=0; i < items.size(); ++i)
                if (items[i] != 0 && (z == 0 || items[i]->zValue() < z))
                    z = items[i]->topLevelItem()->zValue();

            if (item->zValue() > z)
            {
                z -= 0.2;
                targetItems += item;
                zvalues += z;
                scene->setZValue(tr("send back"),targetItems,zvalues);
            }
        }
    }

    void CompartmentTool::itemsRemoved(GraphicsScene * scene, const QList<QGraphicsItem*>&, const QList<ItemHandle*>& handles)
    {
        if (!mainWindow || !scene || !scene->network) return;

        QList<ItemHandle*> children;
        QList<ItemHandle*> newParents;
        ItemHandle * child = 0;

        QList<QGraphicsItem*> itemsToRename;
        QList<QString> newNames;

        QString temp;

        //if items are placed in or out of a Compartment or module...

        QList<QGraphicsItem*> items;

        for (int i=0; i < handles.size(); ++i)
        {
            if (handles[i])
            {
                for (int j=0; j < handles[i]->children.size(); ++j)
                    if (handles[i]->children[j])
                        items += handles[i]->children[j]->graphicsItems;
            }
        }

        for (int i=0; i < items.size(); ++i)
        {
            if ((child = getHandle(items[i])))
            {
                children += child;
                newParents += 0;
            }
        }

        if (!children.isEmpty() && !newParents.isEmpty())
        {
            scene->network->setParentHandle(children,newParents);

            adjustRates(scene, children, newParents);
        }
    }

    void CompartmentTool::moveChildItems(GraphicsScene * scene, const QList<QGraphicsItem*> & items0, const QList<QPointF> & dist)
    {
        QList<QGraphicsItem*> connections;
        ConnectionGraphicsItem::ControlPoint * cp;

        for (int i=0; i < items0.size(); ++i)
        {
            if ((cp = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(items0[i]))
                && cp->connectionItem && !connections.contains(cp->connectionItem))
                connections << cp->connectionItem;
        }

        QList<QGraphicsItem*> items;
        QList<QPointF> points;

        ItemHandle * handle, * child;
        QList<ItemHandle*> visited;
        
        for (int i=0; i < items0.size(); ++i)
        {
            if (qgraphicsitem_cast<NodeGraphicsItem*>(items0[i]) &&
                (handle = getHandle(items0[i])) && !visited.contains(handle) &&
                handle->isA(tr("Compartment")) &&
                handle->children.size() > 0)
            {
                visited += handle;

                QList<QGraphicsItem*> insideBox;
                QGraphicsItem * tcItem;

                QRectF rect = items0[i]->sceneBoundingRect();
                rect.adjust(-dist[i].x(),-dist[i].y(),-dist[i].x(),-dist[i].y());
                insideBox = scene->items(rect);

                for (int k=0; k < insideBox.size(); ++k)
                {
                    if ( (tcItem = getGraphicsItem(insideBox[k])) &&
                         tcItem->sceneBoundingRect().width() < rect.width() &&
                         tcItem->sceneBoundingRect().height() < rect.height() &&
                         (child = getHandle(tcItem)) &&
                         child != handle &&
                         !items0.contains(tcItem) && !items.contains(tcItem) && !connections.contains(tcItem))
                    {
                        items << tcItem;
                        points << dist[i];
                    }
                }

            }
        }
        if (items.size() > 0)
        {
            scene->move(items,points);
        }
    }

    void CompartmentTool::itemsMoved(GraphicsScene * scene, const QList<QGraphicsItem*>& items0, const QList<QPointF>& dist)
    {
        if (!mainWindow || !scene || !scene->network) return;

        QList<ItemHandle*> children;
        QList<ItemHandle*> newParents;

        ItemHandle * child = 0, * parent = 0, * handle = 0;
        bool outOfBox;

		QList<ItemHandle*> movedChildNodes, movedCompartmentNodes;

        QList<QGraphicsItem*> itemsToRename;
        QList<QString> newNames;

        QString temp;

        ConnectionGraphicsItem::ControlPoint * cp = 0;
        NodeGraphicsItem * node = 0;

        QList<QGraphicsItem*> items = items0;
        for (int i=0; i < items0.size(); ++i)
        {
            if (node = qgraphicsitem_cast<NodeGraphicsItem*>(items0[i]))
            {
                items << node->connectionsAsGraphicsItems();
            }
        }

        //if items are placed in or out of a Compartment or module...
        QList<ItemHandle*> visitedHandles;
        for (int i=0; i < items.size(); ++i)
        {
            if (qgraphicsitem_cast<TextGraphicsItem*>(items[i])) continue;

            handle = getHandle(items[i]);

			if (!handle || !handle->family() || visitedHandles.contains(handle)) continue;

			visitedHandles << handle;

            if (node = qgraphicsitem_cast<NodeGraphicsItem*>(items[i]))
            {
                items << node->connectionsAsGraphicsItems();
            }

			if (handle->parent && handle->parent->isA(tr("Compartment")))
				movedChildNodes << handle;
			else
				if (handle->isA(tr("Compartment")))
				{
					movedCompartmentNodes << handle;
					movedChildNodes << handle->children;
				}
		}

		for (int i=0; i < movedCompartmentNodes.size(); ++i)
			for (int j=0; j < movedCompartmentNodes[i]->graphicsItems.size(); ++j)
				if (node = qgraphicsitem_cast<NodeGraphicsItem*>(movedCompartmentNodes[i]->graphicsItems[j]))
					nodeCollided(QList<QGraphicsItem*>(),node,QList<QPointF>());


		for (int i=0; i < movedChildNodes.size(); ++i)
		{
			child = movedChildNodes[i];
			if (child->graphicsItems.isEmpty()) continue;

            outOfBox = true;

            if (outOfBox)
				for (int j=0; j < child->parent->graphicsItems.size(); ++j) //is the item still inside the Compartment/module?
					if (child->parent->graphicsItems[j])
					{
						//QPainterPath p1 = child->parent->graphicsItems[j]->mapToScene(child->parent->graphicsItems[j]->shape());
						QRectF p1 = child->parent->graphicsItems[j]->sceneBoundingRect().adjusted(-5,-5,5,5);
						for (int k=0; k < child->graphicsItems.size(); ++k)
							if (child->graphicsItems[k])
							{
								//QPainterPath p2 = child->graphicsItems[k]->mapToScene(child->graphicsItems[k]->shape());
								QRectF p2 = child->graphicsItems[k]->sceneBoundingRect();
								if (p1.intersects(p2) || p1.contains(p2))
								{
									outOfBox = false; //yes, still contained inside the module/Compartment
									break;
								}
							}
						if (!outOfBox) break;
					}
			if (outOfBox)
			{
				children += child;
				newParents += 0;
			}
        }

        if (!children.isEmpty() && !newParents.isEmpty())
        {
            scene->network->setParentHandle(children,newParents);
            adjustRates(scene,children,newParents);
        }
    }

    QSize CompartmentTool::sizeHint() const
    {
        return QSize(80, 300);
    }

    /********************************
            TREE DELEGATE
    *********************************/

    ContainerTreeDelegate::ContainerTreeDelegate(QTreeView *parent)
        : QItemDelegate(parent), treeView(parent)
    {
    }

    QWidget *ContainerTreeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */,	const QModelIndex & index) const
    {
    	if (index.isValid() && treeView)
			treeView->scrollTo(index.sibling(index.row(),0));
        return new QLineEdit(parent);
    }

    void ContainerTreeDelegate::setEditorData(QWidget *widget, const QModelIndex &index) const
    {
        if (index.isValid())
        {
            ContainerTreeItem * item = static_cast<ContainerTreeItem*>(index.internalPointer());
            ItemHandle * handle = item->handle();
            QString attributeName = item->text();
            QLineEdit * editor = static_cast<QLineEdit*>(widget);

            if (handle && handle->data)
            {
				if (index.column() == 0)
				{
					if (attributeName.isEmpty())
						editor->setText( handle->name );
					else
						editor->setText( attributeName );
				}

				if (index.column() == 1)
				{
					if (attributeName.isEmpty())
					{
						if (handle->hasNumericalData(QString("Initial Value")))
							editor->setText( QString::number(handle->numericalData(QString("Initial Value"))) );
						else
							if (handle->hasTextData(QString("Rate equations")))
								editor->setText(handle->textData(QString("Rate equations")));
					}
					else
						if (handle->hasNumericalData(QString("Parameters")))
							editor->setText( QString::number(handle->numericalData(QString("Parameters"),attributeName)) );
										
				}
			}
			
        }
    }

    void ContainerTreeDelegate::setModelData(QWidget *widget, QAbstractItemModel * model,
                                             const QModelIndex &index) const
    {
        if (index.isValid())
        {
            ContainerTreeItem * item = static_cast<ContainerTreeItem*>(index.internalPointer());
            ItemHandle * handle = item->handle();
            QString attributeName = item->text();
            QVariant value;

            if (handle && handle->data)// && !attributeName.isEmpty())
            {
				QLineEdit * editor = static_cast<QLineEdit*>(widget);
				value = QVariant(editor->text());
                model->setData(index, value, Qt::EditRole);
            }
        }
    }

    void ContainerTreeDelegate::updateEditorGeometry(QWidget *editor,
                                                     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
    {
        editor->setGeometry(option.rect);
    }
    
    /**********************************************************************/

    void CompartmentTool::adjustRates(GraphicsScene * scene, QList<ItemHandle*> childItems, QList<ItemHandle*> parentItems)
    {
        if (!scene || childItems.isEmpty() || childItems.size() != parentItems.size()) return;

        QList< TextDataTable* > newTables;
        QList<QString> toolNames;
        QList<ItemHandle*> targetHandles;
        QList<ItemHandle*> reactions;
        NodeHandle * nodeHandle = 0;

        for (int i=0; i < childItems.size() && i < parentItems.size(); ++i)
        {
            if (childItems[i]->type == ConnectionHandle::TYPE)
            {
                if (	childItems[i]	&&
                        !reactions.contains(childItems[i]) &&
                        childItems[i]->hasTextData(tr("Rate equations")) &&
                        childItems[i]->hasTextData(tr("Assignments")))

                    reactions << (childItems[i]);
            }
            else
                if (childItems[i] && childItems[i]->isA(tr("Molecule")) && (nodeHandle = NodeHandle::cast(childItems[i])))
                {
                QList<ConnectionHandle*> connections = nodeHandle->connections();
                for (int j=0; j < connections.size(); ++j)
                    if (connections[j] &&
                        !reactions.contains(connections[j]) &&
                        connections[j]->hasTextData(tr("Rate equations")) &&
                        connections[j]->hasTextData(tr("Assignments")))

                        reactions << (connections[j]);
            }
        }

        if (reactions.isEmpty()) return;

        ItemHandle * handle;
        ConnectionGraphicsItem * connection;

        for (int i=0; i < reactions.size(); ++i)
        {
            TextDataTable * rates = new TextDataTable(reactions[i]->textDataTable(tr("Rate equations")));
            TextDataTable * data = new TextDataTable(reactions[i]->textDataTable(tr("Assignments")));

            QList<NodeGraphicsItem*> nodesIn, nodesOut;
            QList<ItemHandle*> speciesIn, speciesOut;

            for (int j=0; j < reactions[i]->graphicsItems.size(); ++j)
            {
                if (connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(reactions[i]->graphicsItems[j]))
                {
                    nodesIn = connection->nodesWithoutArrows();
                    nodesOut = connection->nodesWithArrows();
                    for (int k=0; k < nodesIn.size(); ++k)
                        if ((handle = getHandle(nodesIn[k])) && !speciesIn.contains(handle))
                        {
                        speciesIn << handle;
                    }
                    for (int k=0; k < nodesOut.size(); ++k)
                        if ((handle = getHandle(nodesOut[k])) && !speciesOut.contains(handle))
                        {
                        speciesOut << handle;
                    }
                }
            }

            if (speciesIn.isEmpty() && speciesOut.isEmpty()) continue;

            QStringList dilutionFactorIn, dilutionFactorOut;
            for (int j=0; j < speciesIn.size(); ++j)
                if ((handle = speciesIn[j]->parentOfFamily(tr("Compartment"))) && !dilutionFactorIn.contains(handle->fullName()))
                {
					dilutionFactorIn << handle->fullName();
            	}

            for (int j=0; j < speciesOut.size(); ++j)
                if ((handle = speciesOut[j]->parentOfFamily(tr("Compartment"))) && !dilutionFactorOut.contains(handle->fullName()))
                {
					dilutionFactorOut << handle->fullName();
      		    }

            for (int j=0; j < rates->rows(); ++j)
                if (!rates->value(j,0).contains(reactions[i]->fullName() + tr(".DilutionFactor")))
                    rates->value(j,0) = rates->value(j,0) + tr(" * ") + reactions[i]->fullName() + tr(".DilutionFactor");

            QString in, out;
            if (dilutionFactorIn.size() == 0 && dilutionFactorOut.size() == 0)
			{
				if (data->getRowNames().contains(tr("DilutionFactor")))
				{
					data->value(tr("DilutionFactor"),0) = tr("1.0");
					targetHandles << reactions[i];
					newTables << data;
					toolNames << tr("Assignments");
				}
			}
			else
            {
                if (dilutionFactorIn.size() > 0)
                    in = tr("(") + dilutionFactorIn.join(" * ") + tr(")");
                else
                    in = tr("1.0");
                if (dilutionFactorOut.size() > 0)
                    out = tr("(") + dilutionFactorOut.join(" * ") + tr(")");
                else
                    out = tr("1.0");

                data->value(tr("DilutionFactor"),0) = out + tr("/") + in;

                targetHandles << reactions[i] << reactions[i];
                newTables << data << rates;
                toolNames << tr("Assignments") << tr("Rate equations");
            }
        }

        if (targetHandles.size() > 0)
        {
            scene->network->changeData(tr("volume added to rates"),targetHandles,toolNames,newTables);
            if (console())
                console()->message(tr("Rates have been updated to include volume of Compartment(s)"));
        }

		for (int i=0; i < newTables.size(); ++i)
			if (newTables[i])
				delete newTables[i];
    }

}

