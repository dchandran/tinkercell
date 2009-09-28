/****************************************************************************

 Copyright (C) 2009 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYWRITE.TXT

This is an example application that uses the TinkerCell Core library
****************************************************************************/

#include "SimpleDesigner.h"

using namespace Tinkercell;

SimpleDesigner::SimpleDesigner(): Tool(tr("Simple Designer"))
{
	actionGroup = new QActionGroup(this);
	mode = 0;
	
	toolBar = new QToolBar(this);
	actionGroup->setExclusive(true);
	
	arrowButton = new QAction(QIcon(":/images/arrow.png"),tr("arrow"),toolBar);
	QAction * nodeButton = new QAction(QIcon(":/images/blueRect.png"),tr("node"),toolBar);
	QAction * edgeButton = new QAction(QIcon(":/images/1to1.png"),tr("reaction"),toolBar);
	
	arrowButton->setCheckable(true);
	nodeButton->setCheckable(true);
	edgeButton->setCheckable(true);
	toolBar->addAction(arrowButton);
	toolBar->addAction(nodeButton);
	toolBar->addAction(edgeButton);

	connect(actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*)));
	actionGroup->addAction(arrowButton);
	actionGroup->addAction(nodeButton);
	actionGroup->addAction(edgeButton);
	
	QGridLayout * layout1 = new QGridLayout;
	QGridLayout * layout2 = new QGridLayout;
	QVBoxLayout * layout3 = new QVBoxLayout;
	
	layout1->addWidget(new QLabel(tr("name")),0,0);
	layout1->addWidget(new QLabel(tr("concentration")),1,0);
	layout1->addWidget(name1 = new QLineEdit,0,1);
	layout1->addWidget(conc  = new QLineEdit,1,1);
	
	layout2->addWidget(new QLabel(tr("name")),0,0);
	layout2->addWidget(new QLabel(tr("rate")),1,0);
	layout2->addWidget(name2 = new QLineEdit,0,1);
	layout2->addWidget(rate  = new QLineEdit,1,1);
	
	
	listWidget = new QListWidget;
	layout3->addWidget(listWidget);
	
	groupBox1 = new QGroupBox(tr(" Species "));
	groupBox1->setLayout(layout1);
	
	groupBox2 = new QGroupBox(tr(" Reaction "));
	groupBox2->setLayout(layout2);
	
	groupBox3 = new QGroupBox(tr(" Parameters "));
	groupBox3->setLayout(layout3);
	
	QVBoxLayout * layout4 = new QVBoxLayout;
	layout4->addWidget(groupBox1);
	layout4->addWidget(groupBox2);
	layout4->addWidget(groupBox3);
	
	setLayout(layout4);
	
	groupBox1->hide();
	groupBox2->hide();
	
	connect(name1,SIGNAL(editingFinished()),this,SLOT(nameChanged()));
	connect(name2,SIGNAL(editingFinished()),this,SLOT(nameChanged()));
	connect(conc,SIGNAL(editingFinished()),this,SLOT(concentrationChanged()));
	connect(rate,SIGNAL(editingFinished()),this,SLOT(rateChanged()));
}

void SimpleDesigner::nameChanged() 
{
	GraphicsScene * scene = currentScene();
	if (!scene || scene->selected().size() != 1) return;
	
	QGraphicsItem * selectedItem = scene->selected()[0];
	ItemHandle * handle = getHandle(selectedItem);
	
	if (name1->isVisible())
		scene->rename(handle,name1->text());
	else
	if (name2->isVisible())
		scene->rename(handle,name2->text());
		
	for (int i=0; i < handle->graphicsItems.size(); ++i)
		handle->graphicsItems[i]->update();
}

void SimpleDesigner::addParameters(QStringList& newVars)
{
	NetworkWindow * network = currentWindow();
	if (!network || !network->modelItem()) return;
	
	listWidget->clear();
	
	ItemHandle * modelItem = network->modelItem(); //handle for the entire model
	
	QStringList vars;
	DataTable<qreal> params;
	
	if (modelItem->hasNumericalData("parameters"))     
	{
		params = modelItem->data->numericalData["parameters"]; //get existing set of parameters
		vars = params.getRowNames();
	}
	
	for (int i=0; i < newVars.size(); ++i)
		if (!vars.contains(newVars[i]))
		{
			vars << newVars[i];
			params.value(newVars[i],0) = 1.0;   //add new parameters to existing set
		}
	
	modelItem->data->numericalData["parameters"] = params;   //update with new set of parameters
	
	vars.clear();
	
	for (int i=0; i < params.rows(); ++i)
		vars << params.rowName(i) + tr(" = ") + QString::number(params.value(i,0));  //"param = value"
	
	listWidget->addItems(vars);   //update list widget for viewing parameters
}

void SimpleDesigner::rateChanged() 
{
	NetworkWindow * win = currentWindow();
	if (!win) return;
	
	GraphicsScene * scene = win->scene;
	if (!scene || scene->selected().size() != 1) return;
	
	QGraphicsItem * selectedItem = scene->selected()[0];
	ItemHandle * handle = getHandle(selectedItem);
	
	if (!handle || !handle->hasTextData("rate")) return;
	
	
	QString formula = rate->text();
	
	//find all the new variables in this equation
	QStringList newVars;
	bool ok = win->parseMath(formula,newVars);
	
	if (ok)
	{
		DataTable<QString> table;
		table.value(0,0) = formula;
		scene->changeData(handle->name + tr("'s rate changed"), handle,"rate",&table);
		addParameters(newVars); //insert new variables into the modelItem
		setToolTip(handle);
	}
	else
		mainWindow->statusBar()->showMessage(tr("error in formula : ") + formula);
}

void SimpleDesigner::concentrationChanged()
{
	GraphicsScene * scene = currentScene();
	if (!scene || scene->selected().size() != 1) return;
	
	QGraphicsItem * selectedItem = scene->selected()[0];
	ItemHandle * handle = getHandle(selectedItem);
	
	if (!handle || !handle->hasNumericalData("concentration")) return;
	
	DataTable<qreal> table;
	
	bool ok;
	
	table.value(0,0) = conc->text().toDouble(&ok);
	
	scene->changeData(handle->name + tr("'s concentration changed"), handle,"concentration",&table);
}

void SimpleDesigner::actionTriggered(QAction* action)
{
	mode = 0;
	
	if (!action) return;
		
	if (action->text() == tr("node"))
		mode = 1;
	else
	if (action->text() == tr("reaction"))
		mode = 2;
}
	
bool SimpleDesigner::setMainWindow(MainWindow * main)
{
	Tool::setMainWindow(main);
	if (mainWindow)
	{
		mainWindow->addToolBar(Qt::LeftToolBarArea,toolBar);

		setWindowTitle(tr("Information Box"));
		setWindowIcon(QIcon(tr(":/images/about.png")));
		QDockWidget * dockWidget = mainWindow->addToolWindow(this, MainWindow::DockWidget, Qt::BottomDockWidgetArea, Qt::NoDockWidgetArea);
		if (dockWidget)
			dockWidget->setFloating(true);
		
		connect(mainWindow,SIGNAL(itemsInserted(NetworkWindow*, const QList<ItemHandle*>&)),
				this, SLOT(itemsInserted(NetworkWindow*,const QList<ItemHandle*>&)));
				
		connect(mainWindow,
				SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				this,
				SLOT(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers))
				);
				
		connect(mainWindow,
				SIGNAL(escapeSignal(const QWidget * )),
				this,
				SLOT(escapeSignal(const QWidget * ))
				);
		
		connect(mainWindow,
				SIGNAL(mousePressed(GraphicsScene * , QPointF , Qt::MouseButton, Qt::KeyboardModifiers)),
				this,
				SLOT(mousePressed(GraphicsScene * , QPointF , Qt::MouseButton, Qt::KeyboardModifiers))
				);
		return true;
	}
	return false;
}

void SimpleDesigner::mousePressed(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers)
{
	if (!scene || !scene->networkWindow || mode != 1) return;
	
	QGraphicsItem * item = new SimpleNode;
	NodeHandle * handle = new NodeHandle;
	
	QList<QString> names = scene->networkWindow->symbolsTable.handlesFirstName.keys();
	names += scene->networkWindow->symbolsTable.dataRowsAndCols.keys();
	
	int i = 1;
	handle->name = tr("s1");
	
	while (names.contains(handle->name))
	{
		++i;
		handle->name = tr("s") + QString::number(i);
	}
	
	setHandle(item,handle);
	item->setPos(point);
	
	scene->insert(handle->name + tr(" inserted"),item);
	
}

void SimpleDesigner::setToolTip(ItemHandle* item)
{
	if (!item) return;
	
	if (NodeHandle::asNode(item) && item->hasNumericalData("concentration")) //is a node
	{
		for (int i=0; i < item->graphicsItems.size(); ++i)
			item->graphicsItems[i]->setToolTip
			(
				item->name + tr(" : ") + QString::number(item->numericalData("concentration"))
			);
	}
	else
	if (ConnectionHandle::asConnection(item) && item->hasTextData("rate")) //is a connection
	{
		for (int i=0; i < item->graphicsItems.size(); ++i)
			item->graphicsItems[i]->setToolTip
			(
				item->name + tr(" : ") + item->textData("rate")
			);
	}
}

void SimpleDesigner::itemsInserted(NetworkWindow * win,const QList<ItemHandle*>& items)
{
	for (int i=0; i < items.size(); ++i)
	{
		if (NodeHandle::asNode(items[i])) //is node?
		{
			items[i]->numericalData("concentration") = 10.0;
		}
		if (ConnectionHandle::asConnection(items[i])) //is reaction?
		{
			ConnectionHandle * connection = ConnectionHandle::asConnection(items[i]);
			QString rate;
			
			if (connection->hasTextData("rate"))  //rate already exists
			{
				QStringList newVars;
				rate = connection->textData("rate");
				bool ok = win->parseMath(rate,newVars);
					
				if (ok)
				{
					addParameters(newVars);
				}
			}
			else
			{	
				QList<NodeHandle*> nodes = connection->nodesIn();
			
				rate = tr("1.0");
				
				for (int j=0; j < nodes.size(); ++j)
					rate += tr(" * ") + nodes[j]->name;   //default mass-action rate
			}
			
			connection->textData("rate") = rate;
		}
		
		setToolTip(items[i]);
	}
}

void SimpleDesigner::deselectItem(GraphicsScene * scene, QGraphicsItem * item)
{
	selectItem(scene, item, false);
}

void SimpleDesigner::selectItem(GraphicsScene * scene, QGraphicsItem * item, bool select)
{
	if (NodeGraphicsItem::topLevelNodeItem(item))
	{
		NodeGraphicsItem * node = NodeGraphicsItem::topLevelNodeItem(item);
		node->setBoundingBoxVisible(select);
		if (select)
			for (int j=0; j < node->boundaryControlPoints.size(); ++j)
				scene->moving() += node->boundaryControlPoints[j];
	}
	else
	if (ConnectionGraphicsItem::topLevelConnectionItem(item))
	{
		ConnectionGraphicsItem * connection = ConnectionGraphicsItem::topLevelConnectionItem(item);
		if (select)
		{
			connection->setPen(QPen(QColor(255,0,0,255)));
			scene->moving().removeAll(item);
			scene->moving() += connection->controlPointsAsGraphicsItems();
		}
		else
		{
			connection->setPen(connection->defaultPen);
			connection->update();
		}
		connection->setControlPointsVisible(select);     //show the red box
	}
	else
	if (qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(item))
		qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(item)->nodeItem->setBoundingBoxVisible(select);
	else
	if (qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(item))
		qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(item)->connectionItem->setControlPointsVisible(select);
}

void SimpleDesigner::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers)
{
	if (!scene) return;
	
	QList<QGraphicsItem*> nodeItems;
	NodeGraphicsItem * node = 0;
	
	for (int i=0; i < selectedItems.size(); ++i)
	{
		deselectItem(scene,selectedItems[i]);
		
		if (mode == 2)
		{
			node = NodeGraphicsItem::topLevelNodeItem(selectedItems[i]);
			if (node && !nodeItems.contains(node))
				nodeItems << node;
		}
	}
	
	selectedItems.clear();
	
	if (items.size() != 1 || !items[0])
	{
		groupBox1->hide();
		groupBox2->hide();
	}
	else
	{
		ItemHandle * handle = getHandle(items[0]);
		if (NodeHandle::asNode(handle) && handle->hasNumericalData("concentration"))
		{
			name1->setText(  handle->name  );
			conc->setText(  QString::number( handle->numericalData("concentration")  ));
			groupBox1->show();
			groupBox2->hide();
		}
		else
		if (ConnectionHandle::asConnection(handle) && handle->hasTextData("rate"))
		{
			name2->setText(  handle->name  );
			rate->setText(  handle->textData("rate") );
			groupBox1->hide();
			groupBox2->show();
		}
		else
		{
			groupBox1->hide();
			groupBox2->hide();
		}
	}
	
	selectedItems = items;
	
	for (int i=0; i < selectedItems.size(); ++i)
	{
		selectItem(scene,selectedItems[i]);
		if (mode == 2)
		{
			node = NodeGraphicsItem::topLevelNodeItem(selectedItems[i]);
			if (node && !nodeItems.contains(node))
				nodeItems << node;
		}
	}
	
	if (mode == 2 && nodeItems.size() == 2)
	{	
		ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
		ConnectionHandle * handle = new ConnectionHandle;
		QList<QString> names = scene->networkWindow->symbolsTable.handlesFirstName.keys();
		names += scene->networkWindow->symbolsTable.dataRowsAndCols.keys();

		int i = 1;
		handle->name = tr("J1");

		while (names.contains(handle->name))
		{
			++i;
			handle->name = tr("J") + QString::number(i);
		}
		
		setHandle(item,handle);
		
		QPointF midpt = (items[0]->scenePos() + items[1]->scenePos())/2.0;
		
		ConnectionGraphicsItem::PathVector path;
		path += new ConnectionGraphicsItem::ControlPoint(item,nodeItems[0]);
		path += new ConnectionGraphicsItem::ControlPoint(midpt,item);
		path += new ConnectionGraphicsItem::ControlPoint(midpt,item);
		path += new ConnectionGraphicsItem::ControlPoint(item,nodeItems[1]);
		
		ArrowHeadItem * arrow = 0;
		QString nodeImageFile;
		QString appDir = QCoreApplication::applicationDirPath();
		nodeImageFile = tr(":/images/arrow.xml");
		
		NodeGraphicsReader imageReader;
		arrow = new ArrowHeadItem(item);
		imageReader.readXml(arrow,nodeImageFile);
		arrow->normalize();
		arrow->scale(0.15,0.15);
		path.arrowEnd = arrow;
		
		item->pathVectors += path;
		
		scene->insert(tr("connection inserted"),item);
		
		scene->deselect();
	}
}

void SimpleDesigner::escapeSignal(const QWidget * sender)
{
	arrowButton->trigger();
	if (currentScene())
		currentScene()->deselect();
}

/************ MAIN ******************/

int main(int argc, char *argv[])
{
    PROJECTWEBSITE = QObject::tr("www.tinkercell.com");
    ORGANIZATIONNAME = QObject::tr("Simple Designer");
    PROJECTNAME = QObject::tr("Simple Designer");
	
    QApplication app(argc, argv);

	QString appDir = QCoreApplication::applicationDirPath();
    
    MainWindow mainWindow(true,false,false,false);
	
	mainWindow.setWindowTitle(QString("Simple Designer"));
    mainWindow.statusBar()->showMessage(QString("Welcome to Simple Designer"));

	Tool * tool = new SimpleDesigner;
	mainWindow.addTool(tool);
	
	GraphicsScene::SelectionRectangleBrush = QBrush(QColor(5,5,5,40));

    mainWindow.newGraphicsWindow();
	mainWindow.newGraphicsWindow();
	
    mainWindow.show();

    int output = app.exec();

    return output;
}
