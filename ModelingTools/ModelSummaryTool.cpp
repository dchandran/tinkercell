/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This class adds the "attributes" data to each item in Tinkercell.
Two types of attributes are added -- "Parameters" and "Text Attributes".
Attributes are essentially a <name,value> pair that are used to characterize an item.

The ModelSummaryTool also comes with two GraphicalTools, one for text attributes and one
for numerical attributes. The buttons are drawn as NodeGraphicsItems using the datasheet.xml and
textsheet.xml files that define the NodeGraphicsItems.

****************************************************************************/

#include <QSettings>
#include "ConsoleWindow.h"
#include "GraphicsScene.h"
#include "NetworkHandle.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "BasicInformationTool.h"
#include "ModelSummaryTool.h"

namespace Tinkercell
{

	void ModelSummaryTool::select(int)
	{
		openedByUser = true;
		updateTables();
		if (isVisible())
			openedByUser = false;
		else
			show();
		raise();
		this->setFocus();
	}

	void ModelSummaryTool::deselect(int)
	{
		if (openedByUser)
		{
			openedByUser = false;

			if (tabWidget)
				tabWidget->clear();

			hide();
		}
	}

	void ModelSummaryTool::keyPressed(GraphicsScene* scene,QKeyEvent * keyEvent)
	{
		if (!keyEvent || keyEvent->modifiers() || !scene || !scene->useDefaultBehavior) return;

		QList<QGraphicsItem*> & selected = scene->selected();
		
		if (selected.size() == 1)
		{
			ItemHandle * h = getHandle(selected[0]);
			if (h && !h->children.isEmpty() && !h->isA(tr("Compartment")))
				return;
		}

		if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
		{
			updateTables();
			if (!tabWidget || tabWidget->count() < 1) return;

			openedByUser = true;
			if (isVisible())
				openedByUser = false;
			else
				show();
		}
	}
	
	void ModelSummaryTool::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF , Qt::KeyboardModifiers)
	{
		if (!mainWindow || !scene || !scene->useDefaultBehavior || items.isEmpty()) return;
		
		if (isVisible())
			updateTables();
		
		ItemHandle * handle;
		
		for (int i=0; i < items.size(); ++i)
		{
			handle = getHandle(items[i]);
			if (handle && handle->isA("Molecule"))
			{
				if (separator)
					mainWindow->contextItemsMenu.addAction(separator);
				else
					separator = mainWindow->contextItemsMenu.addSeparator();
				
				mainWindow->contextItemsMenu.addAction(toggleFixedAction);
				return;
			}
		}
		
		if (separator)
			mainWindow->contextItemsMenu.removeAction(separator);
		mainWindow->contextItemsMenu.removeAction(toggleFixedAction);
	}

	void ModelSummaryTool::mouseDoubleClicked(GraphicsScene* scene, QPointF, QGraphicsItem* item, Qt::MouseButton, Qt::KeyboardModifiers modifiers)
	{
		if (!scene || !scene->useDefaultBehavior || !item || modifiers) return;

		updateTables();
		if (!tabWidget || tabWidget->count() < 1) return;

		openedByUser = true;
		if (isVisible())
			openedByUser = false;
		else
			show();
		raise();
	}

	void ModelSummaryTool::updateToolTips(const QList<ItemHandle*>& handles)
	{
		for (int i=0; i < handles.size(); ++i)
		{
			if (NodeHandle::cast(handles[i]) && handles[i]->family() 
				&& handles[i]->hasNumericalData(tr("Initial Value")))
			{
				QString s = handles[i]->family()->name() + tr(": ") + handles[i]->fullName() + tr("\n")
							+ handles[i]->family()->measurementUnit.property + tr(" = ")
							+ QString::number(handles[i]->numericalData(tr("Initial Value")))
							+ tr(" ") + handles[i]->family()->measurementUnit.name;
				
				if (handles[i]->hasNumericalData(tr("Fixed")) && handles[i]->numericalData(tr("Fixed")) > 0)
					s = tr("[FIXED] ") + s;
				
				for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
					if (NodeGraphicsItem::cast(handles[i]->graphicsItems[j]))
					{
						handles[i]->graphicsItems[j]->setToolTip(s);
					}
			}
			else
			if (ConnectionHandle::cast(handles[i]) && handles[i]->family())
			{
				QString s;
				
				if (handles[i]->hasTextData(tr("Rate equations")) && 
					handles[i]->textDataTable(tr("Rate equations")).rows() > 0 && 
					handles[i]->textDataTable(tr("Rate equations")).columns() > 0)
					s = handles[i]->family()->name() + tr(": ") + handles[i]->fullName() + tr("\n") + tr("Rate = ") 
						+ handles[i]->textDataTable(tr("Rate equations")).at(0,0);
				else
					s = handles[i]->family()->name() + tr(": ") + handles[i]->fullName() + tr("\n") + tr("[No Rate]");				
				
				for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
					if (ConnectionGraphicsItem::cast(handles[i]->graphicsItems[j]))
					{
						handles[i]->graphicsItems[j]->setToolTip(s);
					}
			}
		}
	}

	bool ModelSummaryTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow)
		{
			connect(this,SIGNAL(dataChanged(const QList<ItemHandle*>&)), mainWindow, SIGNAL(dataChanged(const QList<ItemHandle*>&)));
			
			connect(mainWindow,SIGNAL(dataChanged(const QList<ItemHandle*>&)), this, SLOT(updateToolTips(const QList<ItemHandle*>&)));
		
			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				this,SLOT(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
			
			connect(mainWindow,SIGNAL(itemsInserted(NetworkHandle *, const QList<ItemHandle*>&)),
				this,SLOT(itemsInserted(NetworkHandle *, const QList<ItemHandle*>&)));
			
			connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>& , const QList<ItemHandle*>& )),
				this,SLOT(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>& , const QList<ItemHandle*>& )));

			connect(mainWindow,SIGNAL(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)),
				this,SLOT(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(keyPressed(GraphicsScene*,QKeyEvent *)),
				this ,SLOT(keyPressed(GraphicsScene*,QKeyEvent *)));

			setWindowTitle(name);
			setWindowIcon(QIcon(tr(":/images/monitor.png")));
			mainWindow->addToViewMenu(this);

			connect(closeButton,SIGNAL(pressed()),this,SLOT(hide()));
			move(mainWindow->geometry().center());
			setAttribute(Qt::WA_ContentsPropagated);
			setPalette(QPalette(QColor(255,255,255,255)));
			setAutoFillBackground(true);
		}
		return (mainWindow != 0);
	}
	
	void ModelSummaryTool::itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>& )
	{
		NodeGraphicsItem * node = 0;
		for (int i=0; i < items.size(); ++i)
		{
			if ((node = NodeGraphicsItem::cast(items[i])) 
				&& node->boundaryControlPoints.size() > 0
				&& (node->name.toLower() == tr("lock")))
			{
				for (int k=0; k < node->boundaryControlPoints.size(); ++k)
					if (node->boundaryControlPoints[k])
					{
						if (node->boundaryControlPoints[k]->scene())
							node->boundaryControlPoints[k]->scene()->removeItem(node->boundaryControlPoints[k]);
						delete node->boundaryControlPoints[k];
					}
						
				node->boundaryControlPoints.clear();
			}
		}
	}

	void ModelSummaryTool::itemsInserted(NetworkHandle* , const QList<ItemHandle*>& handles)
	{
		for (int i=0; i < handles.size(); ++i)
		{
			if (handles[i] && handles[i]->family() && !handles[i]->tools.contains(this))
				handles[i]->tools += this;
		}
		
		updateToolTips(handles);
	}

	ModelSummaryTool::ModelSummaryTool() : Tool(tr("Model Summary"),tr("Basic GUI")), delegate(QStringList() << "floating" << "fixed")
	{
		groupBox.setTitle(tr(" Initial values of selected items "));

		tableWidget.setEditTriggers ( QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed );
		connect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));

		QFont font = this->font();
		font.setPointSize(12);
		tableWidget.setFont(font);

		QHBoxLayout * boxLayout = new QHBoxLayout;
		boxLayout->addWidget(&tableWidget);

		groupBox.setLayout(boxLayout);

		QVBoxLayout * layout = new QVBoxLayout;
		tabWidget = new QTabWidget(this);
		layout->addWidget(tabWidget);
		closeButton = new QPushButton;
		closeButton->setText(tr("Close"));
		QHBoxLayout * buttonLayout = new QHBoxLayout;
		buttonLayout->addWidget(closeButton);
		buttonLayout->addStretch(1);
		layout->addLayout(buttonLayout,Qt::AlignBottom);
		//connect(tabWidget,SIGNAL(currentChanged (int)),this,SLOT(currentChanged ( int)));

		setLayout(layout);

		delegate.textColumn = 1;
		delegate.suffixColumn = 1;
		delegate.comboBoxColumn = 2;
		delegate.doubleColumn = 1;
		tableWidget.setItemDelegate(&delegate);

		//tableWidget.setFixedWidth(150);
		//tableWidget.setColumnWidth(0,100);
		groupBox.setMinimumWidth(300);
		currentWidget = 0;

		openedByUser = false;

		QString appDir = QCoreApplication::applicationDirPath();
		NodeGraphicsReader reader;		
		reader.readXml(&item,appDir + tr("/icons/monitor.xml"));
		
		item.normalize();
		item.scale(30.0/item.sceneBoundingRect().width(),35.0/item.sceneBoundingRect().height());
		
		ToolGraphicsItem * toolGraphicsItem = new ToolGraphicsItem(this);
		addGraphicsItem(toolGraphicsItem);		
		toolGraphicsItem->addToGroup(&item);
		toolGraphicsItem->setToolTip(tr("Model summary"));

		separator = 0;
		toggleFixedAction = new QAction(this);
		toggleFixedAction->setText(tr("Toggle fixed/floating"));
		toggleFixedAction->setIcon(QIcon(tr(":/images/lock.png")));
		toggleFixedAction->setToolTip(tr("toggle between fixed and floating species"));
		connect(toggleFixedAction,SIGNAL(triggered()),this,SLOT(fixedAction()));
		
		setWindowFlags(Qt::Dialog);
	}
	
	void ModelSummaryTool::fixedAction()
	{
		NetworkHandle * net = currentNetwork();
		
		if (!net) return;
		
		GraphicsScene * scene = net->currentScene();
		
		if (!scene) return;
		
		QList<DataTable<qreal>*> nDataTablesNew, nDataTablesOld;
		QList<ItemHandle*> handles = getHandle(scene->selected());
		QList<ItemHandle*> changedHandles;
		QList<QGraphicsItem*> insertItems, removeItems;
		
		QStringList names;
		
		for (int i=0; i < handles.size(); ++i)
		{
			if (NodeHandle::cast(handles[i]) && handles[i]->hasNumericalData(tr("Fixed")))
			{
				changedHandles << handles[i];
				DataTable<qreal> * dat = new DataTable<qreal>(handles[i]->numericalDataTable(tr("Fixed")));
				
				nDataTablesOld += &(handles[i]->numericalDataTable(tr("Fixed")));
				nDataTablesNew += dat;

				if (dat->value(0,0) > 0)
					dat->value(0,0) = 0;
				else
					dat->value(0,0) = 1;
				
				if (scene)
				{
					NodeGraphicsItem * lockNode,  *node;
					QList<NodeGraphicsItem*> nodesToSet;
					if (dat->value(0,0))
					{
						for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
						{
							if ((node = NodeGraphicsItem::cast(handles[i]->graphicsItems[j])) && node->boundaryControlPoints.size() > 0)
							{
								QPointF p = node->sceneBoundingRect().topRight() + QPointF(10.0,0.0);
								
								lockNode = new NodeGraphicsItem;
								QString appDir = QCoreApplication::applicationDirPath();
								NodeGraphicsReader reader;
								reader.readXml(lockNode,appDir + tr("/icons/lock.xml"));
								lockNode->normalize();
								for (int k=0; k < lockNode->boundaryControlPoints.size(); ++k)
									if (lockNode->boundaryControlPoints[k])
										delete lockNode->boundaryControlPoints[k];
								lockNode->boundaryControlPoints.clear();
								lockNode->scale(18.0/lockNode->sceneBoundingRect().width(),30.0/lockNode->sceneBoundingRect().height());
								lockNode->setPos(p);
								nodesToSet << lockNode;
								insertItems << lockNode;
							}
						}
						for (int j=0; j < nodesToSet.size(); ++j)
						{
							nodesToSet[j]->setHandle(handles[i]);
						}
					}
					else
					{
						for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
						{
							if ((lockNode = NodeGraphicsItem::cast(handles[i]->graphicsItems[j]))
								&& 
								(lockNode->name.toLower() == tr("lock")))
							{
								removeItems << lockNode;
							}
						}
					}
				}
				
				names << handles[i]->fullName();
			}
		}
		
		if (nDataTablesNew.size() > 0)
		{	
			QList<QUndoCommand*> commands;
			commands << new ChangeDataCommand<qreal>(tr("change fixed"),nDataTablesOld,nDataTablesNew);
				
			if (scene && insertItems.size() > 0)
				commands << new InsertGraphicsCommand(tr("fixed text"), scene, insertItems);
				
			if (scene && removeItems.size() > 0)
				commands << new RemoveGraphicsCommand(tr("fixed text"), removeItems);
				
			CompositeCommand * command = new CompositeCommand(tr("toggle fixed for ") + names.join(tr(",")).left(5),commands);
			net->history.push(command);
			
			emit dataChanged(changedHandles);
			
			for (int i=0; i < nDataTablesNew.size(); ++i)
			{
				delete nDataTablesNew[i];
			}
		}
	}
	
	QSize ModelSummaryTool::sizeHint() const
	{
		return QSize(600, 300);
	}

	void ModelSummaryTool::setValue(int,int)
	{
		NetworkHandle * win = currentNetwork();
		if (!win) return;
		
		disconnect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));
		
		GraphicsScene * scene = win->currentScene();
		
		if (!scene) return;

		DataTable<qreal> * nDataTable1 = 0, * nDataTable2 = 0;

		QList<DataTable<qreal>*> nDataTablesNew, nDataTablesOld;
		
		QList<QGraphicsItem*> insertItems, removeItems;

		QStringList values, fixed, names;
		QString s;

		for (int i=0; i < tableWidget.rowCount() && i < delegate.suffix.size(); ++i)
			if (tableWidget.columnCount() > 2 && tableWidget.item(i,0) && tableWidget.item(i,1)  && tableWidget.item(i,2))
			{
				names += tableWidget.item(i,0)->text();
				s = tableWidget.item(i,1)->text();
				values += s.trimmed().replace(delegate.suffix[i],tr(""));
				fixed += tableWidget.item(i,2)->text();
			}

			int n = 0;
			bool ok = false;
			qreal temp;
			QString stemp;
			QList<ItemHandle*> handles, changedHandles;

			QList<QGraphicsItem*> itemsToRename;
			QList<QString> newNames;

			for (int i=0; i < itemHandles.size(); ++i)
			{
				if (itemHandles[i] != 0)
				{
					if (itemHandles[i]->hasNumericalData(tr("Initial Value")))
					{
						nDataTable2 = 0;
						if (itemHandles[i]->hasNumericalData(tr("Fixed")))
							nDataTable2 = new DataTable<qreal>(itemHandles[i]->numericalDataTable(tr("Fixed")));

						nDataTable1 = new DataTable<qreal>(itemHandles[i]->numericalDataTable(tr("Initial Value")));

						for (int j=0; j < nDataTable1->rows() && n < values.size(); ++j, ++n)
						{
							temp = values[n].toDouble(&ok);
							if (ok)
							{
								nDataTable1->value(j,0) = temp;
								if (itemHandles[i]->family())
									BasicInformationTool::initialValues[ itemHandles[i]->family()->measurementUnit.property ] = temp;
							}
							if (nDataTable2 && j < nDataTable2->rows())
							{
								nDataTable2->value(j,0) = 1.0 * (int)(fixed[n] == tr("fixed"));
							}

							if (itemHandles[i] && itemHandles[i]->name != names[n] && itemHandles[i]->graphicsItems.size() > 0 && itemHandles[i]->graphicsItems[0]
								&& !itemsToRename.contains(itemHandles[i]->graphicsItems[0]))
							{
								itemsToRename += itemHandles[i]->graphicsItems[0];
								newNames += names[n];
							}
						}
						nDataTablesOld += &(itemHandles[i]->numericalDataTable(tr("Initial Value")));
						nDataTablesNew += nDataTable1;
						handles += itemHandles[i];

						if (nDataTable2 && itemHandles[i]->numericalDataTable(tr("Fixed")) != (*nDataTable2))
						{
							nDataTablesOld += &(itemHandles[i]->numericalDataTable(tr("Fixed")));
							nDataTablesNew += nDataTable2;
							handles += itemHandles[i];
							
							if (scene)
							{
								NodeGraphicsItem * lockNode,  *node;
								QList<NodeGraphicsItem*> nodesToSet;
								if (nDataTable2->value(0,0))
								{
									for (int j=0; j < itemHandles[i]->graphicsItems.size(); ++j)
									{
										if ((node = NodeGraphicsItem::cast(itemHandles[i]->graphicsItems[j])) && node->boundaryControlPoints.size() > 0)
										{
											QPointF p = node->sceneBoundingRect().topRight() + QPointF(10.0,0.0);
											
											lockNode = new NodeGraphicsItem;
											QString appDir = QCoreApplication::applicationDirPath();
											NodeGraphicsReader reader;
											reader.readXml(lockNode,appDir + tr("/icons/lock.xml"));
											lockNode->normalize();
											for (int k=0; k < lockNode->boundaryControlPoints.size(); ++k)
												if (lockNode->boundaryControlPoints[k])
													delete lockNode->boundaryControlPoints[k];
											lockNode->boundaryControlPoints.clear();
											lockNode->scale(18.0/lockNode->sceneBoundingRect().width(),30.0/lockNode->sceneBoundingRect().height());
											lockNode->setPos(p);
											nodesToSet << lockNode;
											insertItems << lockNode;
										}
									}
									for (int j=0; j < nodesToSet.size(); ++j)
									{
										nodesToSet[j]->setHandle(itemHandles[i]);
									}
								}
								else
								{
									for (int j=0; j < itemHandles[i]->graphicsItems.size(); ++j)
									{
										if ((lockNode = NodeGraphicsItem::cast(itemHandles[i]->graphicsItems[j]))
											&& 
											(lockNode->name.toLower() == tr("lock")))
										{
											removeItems << lockNode;
										}
									}
								}
							}
						}
						else
						{
							delete nDataTable2;
						}
					}
				}
			}

		QList<QUndoCommand*> commands;
		commands << new ChangeDataCommand<qreal>(tr("change fixed"),nDataTablesOld,nDataTablesNew);
			
		if (scene && insertItems.size() > 0)
			commands << new InsertGraphicsCommand(tr("fixed text"), scene, insertItems);
			
		if (scene && removeItems.size() > 0)
			commands << new RemoveGraphicsCommand(tr("fixed text"), removeItems);
			
		CompositeCommand * command = new CompositeCommand(tr("changed initial values or fixed"),commands);
		win->history.push(command);
		
		emit dataChanged(handles);

		for (int i=0; i < nDataTablesNew.size(); ++i)
			delete nDataTablesNew[i];

		connect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));
	}

	void ModelSummaryTool::updateTables()
	{
		//disconnect(tabWidget,SIGNAL(currentChanged (int)),this,SLOT(currentChanged ( int)));
		tableWidget.clearContents();
		tableWidget.setRowCount(0);

		GraphicsScene * scene = currentScene();
		if (!scene || !scene->network) return;
		
		NetworkHandle * network = scene->network;

		QHash<QString,qreal> constants;
		QHash<QString,QString> equations;
		QHash<QString,QWidget*> widgets;
		QList<QGraphicsItem*> & items = scene->selected();

		NodeGraphicsItem * node;
		ItemHandle * handle;
		itemHandles.clear();

		for (int i=0; i < items.size(); ++i)
			if (!TextGraphicsItem::cast(items[i]))
			{
				handle = getHandle(items[i]);
				
				if (!handle)
				{
					if (qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(items[i]))
						handle = getHandle(qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(items[i])->connectionItem);
					else
					if ((node = NodeGraphicsItem::cast(items[i])) && ArrowHeadItem::cast(node))
					{
						ArrowHeadItem * arrow = static_cast<ArrowHeadItem*>(node);
						if (arrow->connectionItem && arrow->connectionItem->centerRegionItem == arrow)
							handle = getHandle(arrow->connectionItem);
					}
				}
				
				if ( handle &&
					(handle->children.isEmpty() || NodeHandle::cast(handle)))
					itemHandles += handle;
			}

		if (itemHandles.isEmpty()) return;

		disconnect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));

		QStringList parents, values, fixed, names, suffixes;
		DataTable<qreal> * nDataTable = 0;

		for (int i=0; i < itemHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (itemHandles[i] && itemHandles[i]->family() && !(itemHandles[i]->family()->measurementUnit.name.isEmpty())
				&& itemHandles[i]->hasNumericalData(tr("Initial Value")))
			{
				nDataTable = &(itemHandles[i]->numericalDataTable(tr("Initial Value")));
				for (int j=0; j < nDataTable->rows(); ++j)
				{
					if (itemHandles[i]->parent)
						parents += itemHandles[i]->parent->fullName() + tr(".");
					else
						parents += tr("");
					names += itemHandles[i]->name;
					suffixes += itemHandles[i]->family()->measurementUnit.name;
					values += QString::number(nDataTable->value(j,0)) + tr(" ") + itemHandles[i]->family()->measurementUnit.name;
					constants.insert(itemHandles[i]->fullName(),nDataTable->value(j,0));
				}
			}

			if (itemHandles[i] && itemHandles[i]->hasNumericalData(tr("Fixed")))
			{
				nDataTable = &(itemHandles[i]->numericalDataTable(tr("Fixed")));
				for (int j=0; j < nDataTable->rows(); ++j)
				{
					if (nDataTable->value(j,0) > 0.0)
						fixed << "fixed";
					else
						fixed << "floating";
				}
			}
		}

		delegate.suffix = suffixes;
		tableWidget.setRowCount(names.size());
		tableWidget.setColumnCount(3);
		tableWidget.setHorizontalHeaderLabels(QStringList() << "name" << "value" << "fixed?");
		tableWidget.setVerticalHeaderLabels(parents);
		while (fixed.size() < names.size())
			fixed << "floating";

		for (int i=0; i < tableWidget.rowCount(); ++i)
		{
			tableWidget.setItem(i,0,new QTableWidgetItem(names[i]));
			tableWidget.setItem(i,1,new QTableWidgetItem(values[i]));
			tableWidget.setItem(i,2,new QTableWidgetItem(fixed[i]));
		}

		emit aboutToDisplayModel(itemHandles, constants, equations);
		
		QStringList vars = constants.keys();
		for (int i=0; i < vars.size(); ++i)
			if (network->symbolsTable.uniqueDataWithDot.contains(vars[i]))
				itemHandles << network->symbolsTable.uniqueDataWithDot[ vars[i] ].first;
		
		if (tabWidget)
		{
			emit displayModel(*tabWidget, itemHandles, constants, equations);
			if (names.size() > 0 && (tabWidget->count() < 1 || tabWidget->tabText(0) != tr("Formulas")))
				tabWidget->insertTab(0,&groupBox,tr("Initial Values"));
			else
				tabWidget->insertTab(1,&groupBox,tr("Initial Values"));
			tabWidget->setCurrentIndex(0);
		}
		
		connect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));
	}

	void ModelSummaryTool::currentChanged ( int index )
	{
		//if (tabWidget)
		//currentWidget = tabWidget->widget ( index );
	}


}

