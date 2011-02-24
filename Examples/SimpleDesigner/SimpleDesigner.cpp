/****************************************************************************

 Copyright (C) 2009 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYWRITE.TXT

This is an example application that uses the TinkerCell Core library
****************************************************************************/

#include <vector>
#include <iostream>
#include "sbml/SBMLReader.h"
#include "sbml/SBMLWriter.h"
#include "sbml/SBMLDocument.h"
#include "sbml/Species.h"
#include "sbml/SpeciesReference.h"
#include "sbml/ListOf.h"
#include "sbml/Model.h"
#include "sbml/Rule.h"
#include "sbml_sim.h"
#include "BasicGraphicsToolbar.h"
#include "ConsoleWindow.h"
#include "OctaveInterpreterThread.h"
#include "PythonInterpreterThread.h"
#include "LoadSaveTool.h"
#include "SimpleDesigner.h"

using namespace Tinkercell;
using namespace std;

SimpleDesigner::SimpleDesigner(): Tool(tr("Simple Designer"))
{
	actionGroup = new QActionGroup(this);
	mode = 0;
	plotTool = 0;
	
	toolBar = new QToolBar(this);
	toolBar->setObjectName("Simple Designer Toolbar");
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
	connect(listWidget,SIGNAL(itemActivated ( QListWidgetItem * )), this, SLOT(parameterItemActivated ( QListWidgetItem * )));
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
		handle->rename(name1->text());
	else
	if (name2->isVisible())
		handle->rename(name2->text());
		
	for (int i=0; i < handle->graphicsItems.size(); ++i)
		handle->graphicsItems[i]->update();
}

void SimpleDesigner::addParameters(QStringList& newVars)
{
	NetworkHandle * network = currentNetwork();
	if (!network || !network->globalHandle()) return;
	
	listWidget->clear();
	
	ItemHandle * globalHandle = network->globalHandle(); //handle for the entire model	
	QStringList vars;
	DataTable<qreal> params;
	
	params = globalHandle->numericalDataTable("parameters"); //get existing set of parameters
	vars = params.rowNames();
	
	for (int i=0; i < newVars.size(); ++i)
		if (!vars.contains(newVars[i]))
		{
			vars << newVars[i];
			params.value(newVars[i],0) = 1.0;   //add new parameters to existing set
		}
	
	if (vars.isEmpty()) return;

	globalHandle->changeData("parameters", &params); //update with new set of parameters
	vars.clear();
	
	for (int i=0; i < params.rows(); ++i)
		vars << params.rowName(i) + tr(" = ") + QString::number(params.value(i,0));  //"param = value"
	
	listWidget->addItems(vars);   //update list widget for viewing parameters
}

void SimpleDesigner::parameterItemActivated ( QListWidgetItem * item )
{
	NetworkHandle * network = currentNetwork();
	if (!network) return;  //no window open
	
	QString s = item->text();  //get string "name = value"
	QStringList parts = s.split("=");
	QString param = parts[0].trimmed(); //parse to get parameter name
	
	ItemHandle * globalHandle = network->globalHandle(); //handle for the model
	NumericalDataTable & oldTable = globalHandle->numericalDataTable("parameters");
	
	double d = QInputDialog::getDouble(this,"Change parameter", param, 	oldTable.value(param,0)); //get value from user
	
	//update parameter value using to the history window
	NumericalDataTable newTable(oldTable); //new parameter table
	newTable.value(param,0) = d;	
	QString message = tr("parameter ") + param + tr(" changed"); //message for the history stack
	network->changeData(message, globalHandle, "parameters", &newTable); //adds undo command
	
	item->setText(param + tr(" = ") + QString::number(d));
}

void SimpleDesigner::rateChanged() 
{
	GraphicsScene * scene = currentScene();
	if (!scene || scene->selected().size() != 1) return;
	
	NetworkHandle * net = scene->network;	
	QGraphicsItem * selectedItem = scene->selected()[0];
	ItemHandle * handle = getHandle(selectedItem);
	
	if (!handle || !handle->hasTextData("rate")) return;	
	
	QString formula = rate->text();
	
	//find all the new variables in this equation
	QStringList newVars;
	bool ok = net->parseMath(formula,newVars);
	
	if (ok && handle->textData("rate") != formula)
	{
		DataTable<QString> table;
		table.value(0,0) = formula;
		handle->changeData("rate",&table);
		addParameters(newVars); //insert new variables into the globalHandle
		setToolTip(handle);
	}
	else
		if (!ok)
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
	
	if (ok && handle->numericalData("concentration") != table.value(0,0))
		handle->changeData("concentration",&table);
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
		QWidget * tool = mainWindow->tool("Default Plot Tool");
		plotTool = static_cast<PlotTool*>(tool);
		
		if (plotTool && plotTool->parentWidget())
		{
			QDockWidget * dockWidget = static_cast<QDockWidget*>(plotTool->parentWidget());
			dockWidget->setFloating(false);
		}

		mainWindow->addToolBar(Qt::LeftToolBarArea,toolBar);

		setWindowTitle(tr("Information Box"));
		setWindowIcon(QIcon(tr(":/images/about.png")));
		QDockWidget * dockWidget = mainWindow->addToolWindow(this, MainWindow::DockWidget, Qt::BottomDockWidgetArea, Qt::NoDockWidgetArea);
		if (dockWidget)
			dockWidget->setFloating(true);

		mainWindow->toolBarForTools->addAction(QIcon(tr(":/images/play.png")), tr("Simulate"), this, SLOT(ode()));
		mainWindow->toolBarForTools->addAction(QIcon(tr(":/images/graph.png")), tr("Stochastic"), this, SLOT(ssa()));
		
		connect(mainWindow,SIGNAL(itemsInserted(NetworkHandle*, const QList<ItemHandle*>&)),
				this, SLOT(itemsInserted(NetworkHandle*,const QList<ItemHandle*>&)));
				
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
	if (!scene || !scene->network || mode != 1) return;
	
	QGraphicsItem * item = new SimpleNode;
	NodeHandle * handle = new NodeHandle;
	
	handle->name = scene->network->makeUnique(tr("s1"));
	setHandle(item,handle);
	item->setPos(point);
	
	scene->insert(handle->name + tr(" inserted"),item);
	
}

void SimpleDesigner::setToolTip(ItemHandle* item)
{
	if (!item) return;
	
	if (NodeHandle::cast(item) && item->hasNumericalData("concentration")) //is a node
	{
		for (int i=0; i < item->graphicsItems.size(); ++i)
			item->graphicsItems[i]->setToolTip
			(
				item->name + tr(" : ") + QString::number(item->numericalData("concentration"))
			);
	}
	else
	if (ConnectionHandle::cast(item) && item->hasTextData("rate")) //is a connection
	{
		for (int i=0; i < item->graphicsItems.size(); ++i)
			item->graphicsItems[i]->setToolTip
			(
				item->name + tr(" : ") + item->textData("rate")
			);
	}
}

void SimpleDesigner::itemsInserted(NetworkHandle * net,const QList<ItemHandle*>& items)
{
	for (int i=0; i < items.size(); ++i)
	{
		if (NodeHandle::cast(items[i])) //is node?
		{
			items[i]->numericalData("concentration") = 10.0;
		}
		if (ConnectionHandle::cast(items[i])) //is reaction?
		{
			ConnectionHandle * connection = ConnectionHandle::cast(items[i]);
			QString rate;
			
			if (connection->hasTextData("rate"))  //rate already exists
			{
				QStringList newVars;
				rate = connection->textData("rate");
				bool ok = net->parseMath(rate,newVars);
					
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
	if (NodeGraphicsItem::cast(item))
	{
		NodeGraphicsItem * node = NodeGraphicsItem::cast(item);
		node->setBoundingBoxVisible(select);
		if (select)
			for (int j=0; j < node->boundaryControlPoints.size(); ++j)
				scene->moving() += node->boundaryControlPoints[j];
	}
	else
	if (ConnectionGraphicsItem::cast(item))
	{
		ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(item);
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
			node = NodeGraphicsItem::cast(selectedItems[i]);
			if (node && !nodeItems.contains(node))
				nodeItems << node;
		}
	}
	
	selectedItems.clear();
	
	if (items.size() != 1 || !items[0])
	{
		groupBox1->hide();
		groupBox2->hide();
		listWidget->clear();
		
		NetworkHandle * network = scene->network;
		NumericalDataTable & params = network->globalHandle()->numericalDataTable("parameters");
		QStringList vars;
	
		for (int i=0; i < params.rows(); ++i)
			vars << params.rowName(i) + tr(" = ") + QString::number(params.value(i,0));  //"param = value"
	
		listWidget->addItems(vars);   //update list widget for viewing parameters
	}
	else
	{
		ItemHandle * handle = getHandle(items[0]);
		if (NodeHandle::cast(handle) && handle->hasNumericalData("concentration"))
		{
			name1->setText(  handle->name  );
			conc->setText(  QString::number( handle->numericalData("concentration")  ));
			groupBox1->show();
			groupBox2->hide();
		}
		else
		if (ConnectionHandle::cast(handle) && handle->hasTextData("rate"))
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
			node = NodeGraphicsItem::cast(selectedItems[i]);
			if (node && !nodeItems.contains(node))
				nodeItems << node;
		}
	}
	
	if (mode == 2 && nodeItems.size() == 2)
	{	
		//scene->remove("remove temp",nodeItems);
		QList<NodeGraphicsItem*> list1, list2;
		list1 << NodeGraphicsItem::cast(nodeItems[0]);
		list2 << NodeGraphicsItem::cast(nodeItems[1]);
		ConnectionGraphicsItem * item = new ConnectionGraphicsItem(list1,list2);
		ConnectionHandle * handle = new ConnectionHandle;

		handle->name = scene->network->makeUnique(tr("J1"));
		setHandle(item,handle);
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

void SimpleDesigner::ode()
{
	simulate(false);
}

void SimpleDesigner::ssa()
{
	simulate(true);
}

void SimpleDesigner::simulate(bool stochastic)
{
	NetworkHandle * network = currentNetwork();
	if (!network) return;
	
	SBMLDocument_t * doc = SBMLDocument_create();
	Model_t * model = SBMLDocument_createModel(doc);
	
	QList<ItemHandle*> handles = network->handles();
	NumericalDataTable params = network->globalHandle()->numericalDataTable("parameters");	
	
	//create compartment
	Compartment_t * comp = Model_createCompartment (model);
	Compartment_setId(comp, "DefaultCompartment");
	Compartment_setName(comp, "DefaultCompartment");
	Compartment_setVolume(comp, 1.0);
	Compartment_setUnits(comp, "uL");

	//create list of species
	for (int i=0; i < handles.size(); ++i)
		if (NodeHandle::cast(handles[i]))  //if node
		{
			double d = handles[i]->numericalData("concentration");
			QString name = handles[i]->name;

			Species_t * s = Model_createSpecies(model);
			Species_setId(s,ConvertValue(name));
			Species_setName(s,ConvertValue(name));
			Species_setConstant(s,0);
			Species_setInitialConcentration(s, d);
			Species_setInitialAmount(s, d);
			Species_setCompartment(s, "DefaultCompartment");
		}
	
	//create list of reactions
	for (int i=0; i < handles.size(); ++i)
		if (ConnectionHandle::cast(handles[i]))  //if reaction
		{

			ConnectionHandle * reactionHandle = ConnectionHandle::cast(handles[i]);
			
			QString name = reactionHandle->name;
			QString rate = reactionHandle->textData("rate");
			
			QList<NodeHandle*> reactants = reactionHandle->nodesIn();
			QList<NodeHandle*> products = reactionHandle->nodesOut();
			
			Reaction_t * reac = Model_createReaction(model);
			Reaction_setId(reac, ConvertValue(name));
			Reaction_setName(reac, ConvertValue(name));
			Reaction_setId(reac, ConvertValue(name));
			KineticLaw_t  * kinetic = Reaction_createKineticLaw(reac);
			KineticLaw_setFormula( kinetic, ConvertValue( rate ));

			for (int j=0; j < reactants.size(); ++j)
			{ 
				SpeciesReference_t * sref = Reaction_createReactant(reac);
				SpeciesReference_setId(sref, ConvertValue(reactants[j]->name));
				SpeciesReference_setName(sref, ConvertValue(reactants[j]->name));
				SpeciesReference_setSpecies(sref, ConvertValue(reactants[j]->name));
				//SpeciesReference_setStoichiometry( sref, -stoictc_matrix.value(j,i) );
			}
			for (int j=0; j < products.size(); ++j)
			{ 
				SpeciesReference_t * sref = Reaction_createProduct(reac);
				SpeciesReference_setId(sref, ConvertValue(products[j]->name));
				SpeciesReference_setName(sref, ConvertValue(products[j]->name));
				SpeciesReference_setSpecies(sref, ConvertValue(products[j]->name));
			}
		}
	
	//create list of parameters
	for (int i=0; i < params.rows(); ++i)
	{
		Parameter_t * p = Model_createParameter(model);
		Parameter_setId(p, ConvertValue(params.rowName(i)));
		Parameter_setName(p, ConvertValue(params.rowName(i)));
		Parameter_setValue(p, params.value(i,0));
	}	
	
	SBML_sim sim(doc);	

	vector<string> names = sim.getVariableNames();	
	vector< vector<double> > output;	
	NumericalDataTable results;

	try
	{
		if (stochastic)
			output = sim.ssa(100.0);
		else
			output = sim.simulate(100.0,0.1);
	}
	catch(...)
	{
		
	}

	if (output.size() > 0)
	{
		int sz = output[0].size();

		results.resize(sz,output.size());
		for (int i=0; i < names.size(); ++i)
			results.setColumnName(i+1, QString(names[i].c_str()));
		results.setColumnName(0, tr("time"));

		for (int i=0; i < output.size(); ++i)
			for (int j=0; j < sz; ++j)
				results.value(j,i) = output[i][j];
	}
	
	if (plotTool)
		plotTool->plot(results,"Simulation");
	
	delete doc;
}

/************ MAIN ******************/

int main(int argc, char *argv[])
{
	//setup project name
    MainWindow::PROJECTWEBSITE = "www.tinkercell.com";
    MainWindow::ORGANIZATIONNAME = "Simple Designer";
    MainWindow::PROJECTNAME = "Simple Designer";
	
	//MUST DO
	QApplication app(argc, argv);
	QString appDir = QCoreApplication::applicationDirPath();    
	MainWindow mainWindow(true,true,true,true);  //@args: enable scene, text, console, history
	mainWindow.readSettings();   //load settings such as window positions
	
	//optional
	mainWindow.setWindowTitle("Simple Designer"); 
    mainWindow.statusBar()->showMessage("Welcome to Simple Designer");  
    
	/*  install optional tools */
	mainWindow.addTool(new PlotTool);
	mainWindow.addTool(new BasicGraphicsToolbar);
	mainWindow.addTool(new LoadSaveTool);
	
	//This is our main tool
	mainWindow.addTool(new SimpleDesigner);
	
	/*  setup an interpreter for the console (optional)  */
	OctaveInterpreterThread::OCTAVE_FOLDER = "octave"; //this is where the libraries will be located
	ConsoleWindow * console = mainWindow.console();
	std::cout << "loading in simple de...\n";
	InterpreterThread * interpreter = new OctaveInterpreterThread("octave/tinkercell.oct", "octave/libtcoct", &mainWindow);
	//InterpreterThread * interpreter = new PythonInterpreterThread("python/_tinkercell", &mainWindow);
	interpreter->initialize();
	console->setInterpreter(interpreter);
	
	/*  optional  GUI configurations */
	GraphicsScene::SelectionRectangleBrush = QBrush(QColor(5,5,5,40));
	ConnectionGraphicsItem::DefaultMiddleItemFile = "";
	ConnectionGraphicsItem::DefaultArrowHeadFile = ":/images/arrow.xml";
	/*                                           */
	
    mainWindow.newScene();
    mainWindow.show();

    int output = app.exec();

    return output;
}

