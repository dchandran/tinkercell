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
	
	layout1->addWidget(new QLabel(tr("name")),0,0);
	layout1->addWidget(new QLabel(tr("concentration")),1,0);
	layout1->addWidget(name1 = new QLineEdit,0,1);
	layout1->addWidget(conc  = new QLineEdit,1,1);
	
	layout2->addWidget(new QLabel(tr("name")),0,0);
	layout2->addWidget(new QLabel(tr("rate")),1,0);
	layout2->addWidget(name2 = new QLineEdit,0,1);
	layout2->addWidget(rate  = new QLineEdit,1,1);
	
	groupBox1 = new QGroupBox(tr(" Species "));
	groupBox1->setLayout(layout1);
	
	groupBox2 = new QGroupBox(tr(" Reaction "));
	groupBox2->setLayout(layout2);
	
	QHBoxLayout * layout3 = new QHBoxLayout;
	layout3->addWidget(groupBox1);
	layout3->addWidget(groupBox2);
	
	setLayout(layout3);
	
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
}
	
void SimpleDesigner::rateChanged() 
{
	GraphicsScene * scene = currentScene();
	if (!scene || scene->selected().size() != 1) return;
	
	QGraphicsItem * selectedItem = scene->selected()[0];
	ItemHandle * handle = getHandle(selectedItem);
	
	if (!handle || !handle->hasTextData("rate")) return;
	
	DataTable<QString> table;
	table.value(0,0) = rate->text();
	
	scene->changeData(handle,"rate",&table);	
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
	
	scene->changeData(handle,"concentration",&table);	
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
		
		QDockWidget * dockWidget = mainWindow->addDockingWindow(tr("Information Box"), this, Qt::BottomDockWidgetArea, Qt::NoDockWidgetArea);
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

void SimpleDesigner::itemsInserted(NetworkWindow * win,const QList<ItemHandle*>& items)
{
	for (int i=0; i < items.size(); ++i)
	{
		if (NodeHandle::asNode(items[i])) //is node?
		{
			items[i]->setNumericalData("concentration",0,0, 1.0);
		}
		if (ConnectionHandle::asConnection(items[i])) //is reaction?
		{
			ConnectionHandle * connection = ConnectionHandle::asConnection(items[i]);
			QList<NodeHandle*> nodes = connection->nodesIn();
			
			QString rate = tr("1.0");
			
			for (int j=0; j < nodes.size(); ++j)
				rate += tr(" * ") + nodes[j]->name;
			
			items[i]->setTextData("rate",0,0, rate);
		}
	}
}

void SimpleDesigner::selectItem(GraphicsScene * scene, QGraphicsItem * item)
{
	if (NodeGraphicsItem::topLevelNodeItem(item))
	{
		if (mode == 2 && !scene->selected().contains(item))  //in connecting mode
			scene->selected() += item;
		else
			NodeGraphicsItem::topLevelNodeItem(item)->setBoundingBoxVisible(false);
	}
	else
	if (ConnectionGraphicsItem::topLevelConnectionItem(item))
		ConnectionGraphicsItem::topLevelConnectionItem(item)->setControlPointsVisible(false);     //show the red box
	else
	if (qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(item))
		qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(item)->nodeItem->setBoundingBoxVisible(false);
	else
	if (qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(item))
		qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(item)->connectionItem->setControlPointsVisible(false);
}

void SimpleDesigner::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers)
{
	if (!scene) return;
	
	for (int i=0; i < selectedItems.size(); ++i)
	{
		selectItem(scene,selectedItems[i]);
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
			conc->setText(  QString::number( handle->getNumericalData("concentration",0,0)  ));
			groupBox1->show();
			groupBox2->hide();
		}
		else
		if (ConnectionHandle::asConnection(handle) && handle->hasTextData("rate"))
		{
			name2->setText(  handle->name  );
			rate->setText(  handle->getTextData("rate",0,0) );
			groupBox1->hide();
			groupBox2->show();
		}
	}
	
	selectedItems = items;
	NodeGraphicsItem * node = 0;
	for (int i=0; i < selectedItems.size(); ++i)
	{
		if (node = NodeGraphicsItem::topLevelNodeItem(selectedItems[i]))
		{
			node->setBoundingBoxVisible();
			for (int j=0; j < node->boundaryControlPoints.size(); ++j)
				scene->moving() += node->boundaryControlPoints[j];
		}
		else
		if (ConnectionGraphicsItem::topLevelConnectionItem(selectedItems[i]))
		{
			scene->moving().removeAll(selectedItems[i]);
			ConnectionGraphicsItem::topLevelConnectionItem(selectedItems[i])->setControlPointsVisible(true);
			scene->moving() += ConnectionGraphicsItem::topLevelConnectionItem(selectedItems[i])->controlPointsAsGraphicsItems();
		}
		else
		if (qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(selectedItems[i]))
			qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(selectedItems[i])->nodeItem->setBoundingBoxVisible();
		else
		if (qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(selectedItems[i]))
			qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(selectedItems[i])->connectionItem->setControlPointsVisible(true);
	}
	
	if (mode == 2 && items.size() == 2)
	{
	
		ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
		ConnectionHandle * connection = new ConnectionHandle;
		setHandle(item,connection);
		
		QPointF midpt = (items[0]->scenePos() + items[1]->scenePos())/2.0;
		
		ConnectionGraphicsItem::PathVector path;
		path += new ConnectionGraphicsItem::ControlPoint(item,items[1]);
		path += new ConnectionGraphicsItem::ControlPoint(midpt,item);
		path += new ConnectionGraphicsItem::ControlPoint(midpt,item);
		path += new ConnectionGraphicsItem::ControlPoint(item,items[0]);
		
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

    mainWindow.newGraphicsWindow();
	mainWindow.newGraphicsWindow();
	
    mainWindow.show();

    int output = app.exec();

    return output;
}
