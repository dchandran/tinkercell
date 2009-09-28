/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This class adds the "attributes" data to each item in Tinkercell.
Two types of attributes are added -- "Numerical Attributes" and "Text Attributes".
Attributes are essentially a <name,value> pair that are used to characterize an item.

The ModelSummaryTool also comes with two GraphicalTools, one for text attributes and one
for numerical attributes. The buttons are drawn as NodeGraphicsItems using the datasheet.xml and
textsheet.xml files that define the NodeGraphicsItems.

****************************************************************************/

#include <QSettings>
#include "GraphicsScene.h"
#include "NetworkWindow.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ModelSummaryTool.h"

namespace Tinkercell
{

	void ModelSummaryTool::select(int)
	{
		openedByUser = true;
		updateTables();
		if (parentWidget() != 0)
		{
			if (parentWidget()->isVisible())
				openedByUser = false;
			else
				parentWidget()->show();
		}
		else
		{
			if (isVisible())
				openedByUser = false;
			else
				show();
		}
		this->setFocus();
	}

	void ModelSummaryTool::deselect(int)
	{
		if (openedByUser && (!dockWidget || dockWidget->isFloating()))
		{
			openedByUser = false;

			if (tabWidget)
				tabWidget->clear();

			if (parentWidget() != 0)
				parentWidget()->hide();
			else
				hide();
		}
	}

	void ModelSummaryTool::keyPressed(GraphicsScene* scene,QKeyEvent * keyEvent)
	{
		if (!scene || !scene->useDefaultBehavior) return;

		int key = keyEvent->key();

		if (key == Qt::Key_Enter || key == Qt::Key_Return)
		{
			updateTables();
			if (!tabWidget || tabWidget->count() < 1) return;

			openedByUser = true;
			if (parentWidget() != 0)
			{
				if (parentWidget()->isVisible())
					openedByUser = false;
				else
					parentWidget()->show();
			}
			else
			{
				if (isVisible())
					openedByUser = false;
				else
					show();
			}
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
			if (handle && handle->isA("Species"))
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
		if (parentWidget() != 0)
		{
			if (parentWidget()->isVisible())
				openedByUser = false;
			else
				parentWidget()->show();
		}
		else
		{
			if (isVisible())
				openedByUser = false;
			else
				show();
		}
	}

	void ModelSummaryTool::updateToolTips(const QList<ItemHandle*>& handles)
	{
		for (int i=0; i < handles.size(); ++i)
		{
			if (NodeHandle::asNode(handles[i]) && handles[i]->family() 
				&& handles[i]->hasNumericalData(tr("Initial Value")))
			{
				QString s = handles[i]->family()->name + tr(": ") + handles[i]->fullName() + tr("\n")
							+ handles[i]->family()->measurementUnit.first + tr(" = ")
							+ QString::number(handles[i]->numericalData(tr("Initial Value")));
				
				if (handles[i]->hasNumericalData(tr("Fixed")) && handles[i]->numericalData(tr("Fixed")) > 0)
					s = tr("[FIXED] ") + s;
				
				for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
					if (NodeGraphicsItem::cast(handles[i]->graphicsItems[j]))
					{
						handles[i]->graphicsItems[j]->setToolTip(s);
					}
			}
			else
			if (ConnectionHandle::asConnection(handles[i]) && handles[i]->family() 
				&& handles[i]->hasTextData(tr("Rates")))
			{
				QString s = handles[i]->family()->name + tr(": ") + handles[i]->fullName() + tr("\n")
							+ tr("Rate = ") + handles[i]->textData(tr("Rates"));
				
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
			
			connect(mainWindow,SIGNAL(windowClosing(NetworkWindow * , bool *)),this,SLOT(sceneClosing(NetworkWindow * , bool *)));

			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				this,SLOT(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
			
			connect(mainWindow,SIGNAL(itemsInserted(NetworkWindow *, const QList<ItemHandle*>&)),
				this,SLOT(itemsInserted(NetworkWindow *, const QList<ItemHandle*>&)));
			
			connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>& , const QList<ItemHandle*>& )),
				this,SLOT(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>& , const QList<ItemHandle*>& )));

			connect(mainWindow,SIGNAL(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)),
				this,SLOT(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(keyPressed(GraphicsScene*,QKeyEvent *)),
				this ,SLOT(keyPressed(GraphicsScene*,QKeyEvent *)));

			setWindowTitle(name);
			setWindowIcon(QIcon(tr(":/images/monitor.png")));
			dockWidget = mainWindow->addToolWindow(this,MainWindow::DockWidget,Qt::BottomDockWidgetArea,Qt::NoDockWidgetArea);

			if (dockWidget)
			{
				dockWidget->move(mainWindow->geometry().center());

				dockWidget->setWindowFlags(Qt::Tool);
				dockWidget->setAttribute(Qt::WA_ContentsPropagated);
				dockWidget->setPalette(QPalette(QColor(255,255,255,255)));
				dockWidget->setAutoFillBackground(true);
				//dockWidget->setWindowOpacity(0.9);

				QCoreApplication::setOrganizationName("TinkerCell");
				QCoreApplication::setOrganizationDomain("www.tinkercell.com");
				QCoreApplication::setApplicationName("TinkerCell");

				QSettings settings("TinkerCell", "TinkerCell");

				settings.beginGroup("ModelSummaryTool");
				//dockWidget->resize(settings.value("size", sizeHint()).toSize());
				//dockWidget->move(settings.value("pos", dockWidget->pos()).toPoint());

				dockWidget->setFloating(true);
				settings.endGroup();
				dockWidget->hide();
			}
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
				&& node->fileName.toLower().contains(tr("/lock.xml")))
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

	void ModelSummaryTool::itemsInserted(NetworkWindow* , const QList<ItemHandle*>& handles)
	{
		for (int i=0; i < handles.size(); ++i)
		{
			if (handles[i] && handles[i]->family() && !handles[i]->tools.contains(this))
				handles[i]->tools += this;
		}
		
		updateToolTips(handles);
	}

	void ModelSummaryTool::sceneClosing(NetworkWindow * , bool *)
	{
		QCoreApplication::setOrganizationName("TinkerCell");
		QCoreApplication::setOrganizationDomain("www.tinkercell.com");
		QCoreApplication::setApplicationName("TinkerCell");

		QSettings settings("TinkerCell", "TinkerCell");

		if (dockWidget)
		{
			settings.beginGroup("ModelSummaryTool");
			//settings.setValue("floating", dockWidget && dockWidget->isFloating());
			settings.setValue("size", dockWidget->size());
			settings.setValue("pos", dockWidget->pos());
			settings.endGroup();
		}
	}

	ModelSummaryTool::ModelSummaryTool() : Tool(tr("Model Summary")), delegate(QStringList() << "floating" << "fixed")
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

		dockWidget = 0;

		QVBoxLayout * layout = new QVBoxLayout;
		tabWidget = new QTabWidget(this);
		layout->addWidget(tabWidget);
		QToolButton * closeButton = new QToolButton;
		closeButton->setText(tr("Close"));
		connect(closeButton,SIGNAL(pressed()),this,SLOT(deselect()));
		layout->addWidget(closeButton,Qt::AlignBottom);
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
		reader.readXml(&item,appDir + tr("/OtherItems/monitor.xml"));

		item.normalize();
		item.scale(30.0/item.sceneBoundingRect().width(),35.0/item.sceneBoundingRect().height());

		graphicsItems += new GraphicsItem(this);
		graphicsItems[0]->addToGroup(&item);
		graphicsItems[0]->setToolTip(tr("Model summary"));

		separator = 0;
		toggleFixedAction = new QAction(this);
		toggleFixedAction->setText(tr("toggle fixed/floating"));
		toggleFixedAction->setIcon(QIcon(appDir + tr(":/images/lock.png")));
		toggleFixedAction->setToolTip(tr("toggle between fixed and floating species"));
		connect(toggleFixedAction,SIGNAL(triggered()),this,SLOT(fixedAction()));
	}
	
	void ModelSummaryTool::fixedAction()
	{
		NetworkWindow * net = currentWindow();
		
		if (!net) return;
		
		QList<ItemHandle*> handles = net->selectedHandles();
		
		QList< DataTable<qreal>* > newTables;
		QList<ItemHandle*> changedHandles;
		QStringList names;
		
		for (int i=0; i < handles.size(); ++i)
		{
			if (NodeHandle::asNode(handles[i]) && handles[i]->hasNumericalData(tr("Fixed")))
			{
				DataTable<qreal> * dat = new DataTable<qreal>(handles[i]->numericalDataTable(tr("Fixed")));
				if (dat->value(0,0) > 0)
					dat->value(0,0) = 0;
				else
					dat->value(0,0) = 1;

				newTables << dat;
				changedHandles << handles[i];
				names << handles[i]->fullName();
			}
		}
		
		if (newTables.size() > 0)
		{
			net->changeData(tr("toggle fixed for ") + names.join(tr(",")).left(5), changedHandles, tr("Fixed"), newTables);
			for (int i=0; i < newTables.size(); ++i)
			{
				delete newTables[i];
			}
		}
	}
	
	QSize ModelSummaryTool::sizeHint() const
	{
		return QSize(600, 300);
	}

	void ModelSummaryTool::setValue(int,int)
	{
		NetworkWindow * win = currentWindow();
		if (!win) return;
		
		disconnect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));
		
		GraphicsScene * scene = win->scene;

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
			QList<ItemHandle*> handles;

			QList<QGraphicsItem*> itemsToRename;
			QList<QString> newNames;

			for (int i=0; i < itemHandles.size(); ++i)
			{
				if (itemHandles[i] != 0 && itemHandles[i]->data != 0)
				{
					if (itemHandles[i]->hasNumericalData(tr("Initial Value")))
					{
						nDataTable2 = 0;
						if (itemHandles[i]->hasNumericalData(tr("Fixed")))
							nDataTable2 = new DataTable<qreal>(itemHandles[i]->data->numericalData[tr("Fixed")]);

						nDataTable1 = new DataTable<qreal>(itemHandles[i]->data->numericalData[tr("Initial Value")]);

						for (int j=0; j < nDataTable1->rows() && n < values.size(); ++j, ++n)
						{
							temp = values[n].toDouble(&ok);
							if (ok)
							{
								nDataTable1->value(j,0) = temp;
							}
							if (nDataTable2 && j < nDataTable2->rows())
							{
								nDataTable2->value(j,0) = 1.0 * (int)(fixed[n] == "fixed");
							}

							if (itemHandles[i] && itemHandles[i]->name != names[n] && itemHandles[i]->graphicsItems.size() > 0 && itemHandles[i]->graphicsItems[0]
							&& !itemsToRename.contains(itemHandles[i]->graphicsItems[0]))
							{
								itemsToRename += itemHandles[i]->graphicsItems[0];
								newNames += names[n];
							}
						}
						nDataTablesOld += &(itemHandles[i]->data->numericalData[tr("Initial Value")]);
						nDataTablesNew += nDataTable1;
						handles += itemHandles[i];

						if (nDataTable2 && itemHandles[i]->data->numericalData[tr("Fixed")] != (*nDataTable2))
						{
							nDataTablesOld += &(itemHandles[i]->data->numericalData[tr("Fixed")]);
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
											reader.readXml(lockNode,appDir + tr("/OtherItems/lock.xml"));
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
											lockNode->fileName.toLower().contains(tr("/lock.xml")))
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
			commands << new RemoveGraphicsCommand(tr("fixed text"), scene, removeItems);
			
		CompositeCommand * command = new CompositeCommand(tr("changed initial values or fixed"),commands);
		win->history.push(command);
		
		for (int i=0; i < insertItems.size(); ++i)
			handles << getHandle(insertItems[i]);
			
		for (int i=0; i < removeItems.size(); ++i)
			handles << getHandle(removeItems[i]);
		
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
		if (!scene) return;

		QHash<QString,qreal> constants;
		QHash<QString,QString> equations;
		QHash<QString,QWidget*> widgets;
		QList<QGraphicsItem*> items = scene->selected();

		ItemHandle * handle;
		itemHandles.clear();

		for (int i=0; i < items.size(); ++i)
			if (!TextGraphicsItem::cast(items[i]))
			{
				handle = getHandle(items[i]);
				if (handle && handle->family() && !itemHandles.contains(handle))
					itemHandles += handle;
				else
					if (qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(items[i]) &&
						(handle = getHandle(qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(items[i])->connectionItem)) &&
						handle->family())
						itemHandles += handle;

			}

		if (itemHandles.isEmpty()) return;

		disconnect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));

		QStringList parents, values, fixed, names, suffixes;
		DataTable<qreal> * nDataTable = 0;

		for (int i=0; i < itemHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (itemHandles[i] && itemHandles[i]->family() && !(itemHandles[i]->family()->measurementUnit.first.isEmpty())
				&& itemHandles[i]->data && itemHandles[i]->hasNumericalData(tr("Initial Value")))
			{
				nDataTable = &(itemHandles[i]->data->numericalData[tr("Initial Value")]);
				for (int j=0; j < nDataTable->rows(); ++j)
				{
					if (itemHandles[i]->parent)
						parents += itemHandles[i]->parent->fullName() + tr(".");
					else
						parents += tr("");
					names += itemHandles[i]->name;
					suffixes += itemHandles[i]->family()->measurementUnit.second;
					values += QString::number(nDataTable->value(j,0)) + tr(" ") + itemHandles[i]->family()->measurementUnit.second;
					constants.insert(itemHandles[i]->fullName(),nDataTable->value(j,0));
				}
			}

			if (itemHandles[i] && itemHandles[i]->data && itemHandles[i]->hasNumericalData(tr("Fixed")))
			{
				nDataTable = &(itemHandles[i]->data->numericalData[tr("Fixed")]);
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

		connect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));

		emit aboutToDisplayModel(itemHandles, constants, equations);

		//widgets.insert(tr("Initial Values"),groupBox);

		if (tabWidget)
		{
			emit displayModel(*tabWidget, itemHandles, constants, equations);
			if (names.size() > 0)
				tabWidget->insertTab(0,&groupBox,tr("Initial Values"));
			tabWidget->setCurrentIndex(0);
		}

		//if (currentWidget)
		//tabWidget->setCurrentWidget(currentWidget);

		//connect(tabWidget,SIGNAL(currentChanged (int)),this,SLOT(currentChanged ( int)));

		/*
		if (!constants.isEmpty() && !equations.isEmpty())
		{
		emit graph(equations, constants.keys(),constants.values());
		}

		delete layout();


		//QHBoxLayout * layout = new QHBoxLayout;
		//QHBoxLayout * checksLayout = new QHBoxLayout;



		for (int i=0; i < widgets.size(); i+=2)
		{
		//QCheckBox * check = new QCheckBox(this);
		//connect(check,SIGNAL(checked(bool)),widgets[i],SLOT(setVisible(bool)));
		//checksLayout->addWidget(check);

		QVBoxLayout * layout2 = new QVBoxLayout;
		if (widgets[i])
		layout2->addWidget(widgets[i]);
		if ((i+1) < widgets.size() && widgets[i+1])
		layout2->addWidget(widgets[i+1]);
		layout->addLayout(layout2);
		}
		//layout->insertStretch(0,20);

		///QVBoxLayout * layoutv = new QVBoxLayout;
		//layoutv->addLayout(checksLayout);
		//layoutv->addLayout(layout);

		setLayout(layout);*/

		/*
		QList<QString> keys = widgets.keys();
		QList<QWidget*> wids = widgets.values();

		for (int i=0; i < keys.size(); ++i)
		if (wids[i])
		{
		tabWidget.insertTab(tabWidget.count(),wids[i],keys[i]);
		}*/
	}

	void ModelSummaryTool::currentChanged ( int index )
	{
		//if (tabWidget)
		//currentWidget = tabWidget->widget ( index );
	}


}
