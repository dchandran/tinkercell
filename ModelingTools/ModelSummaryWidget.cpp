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
#include "ModelSummaryWidget.h"
#include <QtDebug>

namespace Tinkercell
{
    ModelSummaryWidget::ModelSummaryWidget() : Tool(tr("Model Summary Window"),tr("Modeling"))
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

    ModelSummaryWidget::~ModelSummaryWidget()
    {
        if (treeDelegate)
            delete treeDelegate;

		if (treeView && treeView->model())
            delete treeView->model();
    }

    bool ModelSummaryWidget::setMainWindow(MainWindow * main)
    {
        Tool::setMainWindow(main);
        if (mainWindow)
        {
            connect(mainWindow,SIGNAL(historyChanged(int)),this,SLOT(updateTree(int)));

            connect(mainWindow,SIGNAL(windowChanged(NetworkWindow*, NetworkWindow*)),
                    this,SLOT(windowChanged(NetworkWindow*, NetworkWindow*)));

			connect(mainWindow,SIGNAL(networkClosed(NetworkHandle *)),this,SLOT(windowClosed(NetworkHandle *)));

            windowChanged(0,mainWindow->currentWindow());

            treeView->setEditTriggers ( QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed );

            treeView->setColumnWidth(0,30);
            treeView->setColumnWidth(1,50);

			setWindowTitle(tr("Model summary"));
			setWindowIcon(QIcon(tr(":/images/monitor.png")));
			
			mainWindow->addToolWindow(this, MainWindow::defaultToolWindowOption, Qt::RightDockWidgetArea);

            return true;
        }

        return false;
    }

	void ModelSummaryWidget::windowClosed(NetworkHandle *)
	{
		treeView->setUpdatesEnabled(false);

        if (treeView->model())
        {
            delete (treeView->model());
			treeView->setModel(0);
        }
	}

    void ModelSummaryWidget::windowChanged(NetworkWindow * , NetworkWindow * )
    {
        updateTree(0);
    }

    void ModelSummaryWidget::updateTree(int)
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

    void ModelSummaryWidget::indexSelected(const QModelIndex& index)
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

    QSize ModelSummaryWidget::sizeHint() const
    {
        return QSize(80, 300);
    }
    
    /**********************************************************************/

    void ModelSummaryWidget::adjustRates(GraphicsScene * scene, QList<ItemHandle*> childItems, QList<ItemHandle*> parentItems)
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
                if (connection = ConnectionGraphicsItem::cast(reactions[i]->graphicsItems[j]))
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

            /*for (int j=0; j < speciesOut.size(); ++j)
                if ((handle = speciesOut[j]->parentOfFamily(tr("Compartment"))) && !dilutionFactorOut.contains(handle->fullName())
                	&& !dilutionFactorIn.contains(handle->fullName()))
                {
					dilutionFactorOut << handle->fullName();
      		    }*/

            for (int j=0; j < rates->rows(); ++j)
                if (!rates->value(j,0).contains(reactions[i]->fullName() + tr(".DilutionFactor")))
                    rates->value(j,0) = rates->value(j,0) + tr(" * ") + reactions[i]->fullName() + tr(".DilutionFactor");

            QString in, out;
            if (dilutionFactorIn.isEmpty() /*|| dilutionFactorOut.isEmpty()*/)
			{
				if (data->hasRow(tr("DilutionFactor")))
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

               /*if (dilutionFactorOut.size() > 0)
                    out = tr("(") + dilutionFactorOut.join(" * ") + tr(")");
                else
                    out = tr("1.0");*/

                data->value(tr("DilutionFactor"),0) = tr("1.0/") + in;

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

