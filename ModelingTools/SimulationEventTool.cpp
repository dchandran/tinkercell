/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This class adds the "attributes" data to each item in Tinkercell.
Two types of attributes are added -- "Parameters" and "Text Attributes".
Attributes are essentially a <name,value> pair that are used to characterize an item.

The SimulationEventsTool also comes with two GraphicalTools, one for text attributes and one
for numerical attributes. The buttons are drawn as NodeGraphicsItems using the datasheet.xml and
textsheet.xml files that define the NodeGraphicsItems.

****************************************************************************/
#include <QToolTip>
#include "UndoCommands.h"
#include "NetworkHandle.h"
#include "SymbolsTable.h"
#include "GraphicsScene.h"
#include "EquationParser.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "SimulationEventTool.h"
#include "BasicInformationTool.h"
#include "FunctionDeclarationsTool.h"
#include "CatalogWidget.h"
#include "muParserDef.h"
#include "muParser.h"
#include "muParserInt.h"
#include <QGroupBox>

namespace Tinkercell
{
	void SimulationEventsTool::select(int)
	{
		NetworkHandle * net = currentNetwork();
		if (!net) return;

		openedByUser = true;

		updateTable();
		if (isVisible())
			openedByUser = false;
		else
			show();
		raise();
	}

	void SimulationEventsTool::deselect(int)
	{
		if (openedByUser)
		{
			openedByUser = false;
			hide();
		}
	}

	void SimulationEventsTool::toolLoaded(Tool * tool)
	{
		static bool connected1 = false, connected2 = false;
		if (!tool || (connected1 && connected2)) return;

		if (tool->name == tr("Parts and Connections Catalog") && !connected1)
		{
			CatalogWidget * catalog = static_cast<CatalogWidget*>(tool);

			connect(catalog,SIGNAL(buttonPressed(const QString&)),
					this,SLOT(insertButtonPressed(const QString&)));
			
			QList<QToolButton*> newButtons = catalog->addNewButtons(
				tr("Inputs"),
				QStringList() 	<< tr("New event") 
								<< tr("Global parameters")
								<< tr("Step input") 
								<< tr("Impulse") 
								<< tr("Wave input"),
				QList<QIcon>() 	<< QIcon(QPixmap(tr(":/images/clock.png")))
								<< QIcon(QPixmap(tr(":/images/scroll.png")))
								<< QIcon(QPixmap(tr(":/images/stepFunc.png")))
								<< QIcon(QPixmap(tr(":/images/pulseFunc.png")))
								<< QIcon(QPixmap(tr(":/images/sinFunc.png"))),
				QStringList() 	<< tr("Add an event that occurs as a reponse to a given condition")
								<< tr("Edit global parameters")
								<< tr("Insert a step function as input for one of the variables in the model")
								<< tr("Insert an delta function as as input for one of the variables in the model")
								<< tr("Insert a sin function as as input for one of the variables in the model")
				);
			
			connected1 = true;
		}
		if (tool->name == tr("Functions and Assignments") && !connected2)
		{
			AssignmentFunctionsTool * assignmentsTool = static_cast<AssignmentFunctionsTool*>(tool);
			connect(this,SIGNAL(showAssignments(int)),assignmentsTool,SLOT(select(int)));
			connected2 = false;
		}
	}


	bool SimulationEventsTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow)
		{
			eventDialog = new QDialog(mainWindow);
			setupDialogs();
			
			connect(this,SIGNAL(dataChanged(const QList<ItemHandle*>&)),
					mainWindow,SIGNAL(dataChanged(const QList<ItemHandle*>&)));
			
			connect(this, SIGNAL(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
					mainWindow,SIGNAL(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),
					this,SLOT(escapeSignal(const QWidget*)));

			connect(mainWindow,SIGNAL(mousePressed(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
				this,SLOT(sceneClicked(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));
				
			connect(mainWindow,SIGNAL(itemsDropped(GraphicsScene *, const QString&, const QPointF&)),
				this,SLOT(itemsDropped(GraphicsScene *, const QString&, const QPointF&)));

			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			connect(mainWindow,SIGNAL(itemsAboutToBeRemoved(GraphicsScene * , QList<QGraphicsItem*>& , QList<ItemHandle*>&, QList<QUndoCommand*>&)),
				this,SLOT(itemsRemoved(GraphicsScene * , QList<QGraphicsItem*>& , QList<ItemHandle*>&, QList<QUndoCommand*>&)));

			connect(mainWindow,SIGNAL(mouseDoubleClicked (GraphicsScene * , QPointF, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers)),
				this,SLOT(mouseDoubleClicked (GraphicsScene * , QPointF, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(keyPressed(GraphicsScene* ,QKeyEvent * )),
				this,SLOT(keyPressed(GraphicsScene* ,QKeyEvent *)));

			connect(mainWindow,SIGNAL(historyChanged(int)),this,SLOT(historyUpdate(int)));

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			toolLoaded(mainWindow->tool(tr("Parts and Connections Catalog")));
			
			toolLoaded(mainWindow->tool(tr("Functions and Assignments")));

			setWindowTitle(name);
			setWindowFlags(Qt::Dialog);

			mainWindow->addToViewMenu(this);

			move(mainWindow->geometry().bottomRight() - QPoint(sizeHint().width()*2,sizeHint().height()*2));
			setAttribute(Qt::WA_ContentsPropagated);
			setPalette(QPalette(QColor(255,255,255,255)));
			setAutoFillBackground(true);
			//setWindowOpacity(0.9);
		}
		return (mainWindow != 0);
	}

	void SimulationEventsTool::historyUpdate(int)
	{
		if (isVisible())
			updateTable();
	}

	SimulationEventsTool::SimulationEventsTool() : Tool(tr("Events and Inputs"),tr("Modeling"))
	{
		SimulationEventsTool::fToS = new SimulationEventsTool_FToS;
		SimulationEventsTool::fToS->setParent(this);
		
		mode = none;
		QString appDir = QCoreApplication::applicationDirPath();
		openedByUser = false;

		QFont font = this->font();
		font.setPointSize(12);
		eventsListWidget.setFont(font);

		QHBoxLayout * actionsLayout = new QHBoxLayout;

		QToolButton * addEventAction = new QToolButton(this);
		addEventAction->setIcon(QIcon(":/images/plus.png"));
		addEventAction->setToolTip(tr("Add new event"));
		connect(addEventAction,SIGNAL(pressed()),this,SLOT(addEvent()));

		QToolButton * removeEventAction = new QToolButton(this);
		removeEventAction->setIcon(QIcon(":/images/minus.png"));
		removeEventAction->setToolTip(tr("Remove selected event(s)"));
		connect(removeEventAction,SIGNAL(pressed()),this,SLOT(removeEvents()));

		QToolButton * question = new QToolButton(this);
		question->setIcon(QIcon(":/images/question.png"));

		QString message = tr("This table can be used to declare events. Events are a set of triggers and responses to those triggers. An example of an event is WHEN A < 0.1 DO A = 10. In this event, whenever A goes below 0.1, the value of A is set to 10. Another example is WHEN time > 50 DO A = 20; B = 10; where two values are set at time 50.");
		QMessageBox * messageBox = new QMessageBox(QMessageBox::Information,tr("About Forcing Functions"),message,QMessageBox::StandardButtons(QMessageBox::Close), const_cast<QWidget*>((QWidget*)this), Qt::WindowFlags (Qt::Dialog));
		connect(question,SIGNAL(pressed()),messageBox,SLOT(exec()));

		actionsLayout->addWidget(addEventAction);
		actionsLayout->addWidget(removeEventAction);
		actionsLayout->addStretch(1);
		actionsLayout->addWidget(question);

		groupBox = new QGroupBox(tr(" Events "),this);

		QVBoxLayout * eventBoxLayout = new QVBoxLayout;
		eventBoxLayout->addWidget(&eventsListWidget,1);

		//QWidget * widget = new QWidget(this);
		//widget->setLayout(actionsLayout);

		eventBoxLayout->addLayout(actionsLayout);
		groupBox->setLayout(eventBoxLayout);

		eventDialog = 0;

		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(groupBox);
		setLayout(layout);

		connectTCFunctions();
	}

	QSize SimulationEventsTool::sizeHint() const
	{
		return QSize(400, 200);
	}
	
	void SimulationEventsTool::itemsDropped(GraphicsScene * scene, const QString& name, const QPointF& point)
	{
		scene->useDefaultBehavior(false);
		if (name.toLower() == tr("new event"))
			mode = addingEvent;
		
		if (name.toLower() == tr("global parameters"))
			mode = globalParams;
		
		if (name.toLower() == tr("step input"))
			mode = addingStep;
			
		if (name.toLower() == tr("impulse"))
			mode = addingPulse;
			
		if (name.toLower() == tr("wave input"))
			mode = addingWave;

		sceneClicked(scene,point,Qt::LeftButton,Qt::NoModifier);
		
		scene->useDefaultBehavior(true);
		mode = none;
	}

	void SimulationEventsTool::sceneClicked(GraphicsScene * scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
	{
		if (mode == none || button == Qt::RightButton || !scene || scene->useDefaultBehavior())
		{
			return;
		}
		
		QList<QGraphicsItem*> items;
		
		if (mode == addingEvent)
		{
			items = scene->items();
			NodeGraphicsItem * image = 0;
			ItemHandle * globalHandle = 0; 
			globalHandle = scene->localHandle();
			if (!globalHandle)
				globalHandle = scene->globalHandle();
			
			if (globalHandle && !globalHandle->hasTextData(tr("Events")))
			{
				DataTable<QString> events;
				events.resize(0,1);
				events.setColumnName(0,tr("event"));
				events.description() = tr("Events: set of triggers and events. The row names are the triggers, and the first column contains a string describing one or more events, usually an assignment.");
				globalHandle->textDataTable(tr("Events")) = events;
			}
		
			for (int i=0; i < items.size(); ++i)
			{
				image = NodeGraphicsItem::cast(items[i]);
				if (image && image->className == tr("Event function"))
				{
					mainWindow->sendEscapeSignal(this);
					select(0);
					return;
				}
			}
		
			QString appDir = QApplication::applicationDirPath();
			image = new NodeGraphicsItem;
			NodeGraphicsReader reader;
			reader.readXml(image, tr(":/images/clock.xml"));
			image->normalize();
			image->className = tr("Event function");
			image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),
			image->defaultSize.height()/image->sceneBoundingRect().height());

			image->setPos(point);
			image->setToolTip(tr("List of events in this model"));

			scene->insert(tr("Events box inserted"),image);
			mainWindow->sendEscapeSignal(this);
			
			select(0);
			return;
		}
		
		if (mode == globalParams)
		{
			items = scene->items();
			NodeGraphicsItem * image = 0;
			ItemHandle * globalHandle = 0; 
			globalHandle = scene->localHandle();
			if (!globalHandle)
				globalHandle = scene->globalHandle();
			
			if (globalHandle && !globalHandle->hasNumericalData(tr("Parameters")))
			{
				DataTable<double> params;
				params.resize(0,3);
				params.setColumnName(0,tr("value"));
				params.setColumnName(1,tr("min"));
				params.setColumnName(2,tr("max"));
				params.description() =  tr("Parameters: an Nx3 table storing all the real attributes for this item. Row names are the attribute names. First column holds the values. Second and third columns hold the upper and lower bounds.");
				globalHandle->numericalDataTable(tr("Parameters")) = params;
			}
			
			QWidget * widget = mainWindow->tool(tr("Parameters"));
			if (!widget) return;
			
			BasicInformationTool * basicInfoTool = static_cast<BasicInformationTool*>(widget);
		
			for (int i=0; i < items.size(); ++i)
			{
				image = NodeGraphicsItem::cast(items[i]);
				if (image && image->className == tr("Global parameters"))
				{
					mainWindow->sendEscapeSignal(this);
					basicInfoTool->select(0);
					return;
				}
			}
			
			QString appDir = QApplication::applicationDirPath();
			image = new NodeGraphicsItem;
			NodeGraphicsReader reader;
			reader.readXml(image, tr(":/images/plainpaper.xml"));
			image->normalize();
			image->className = tr("Global parameters");
			image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),
			image->defaultSize.height()/image->sceneBoundingRect().height());

			image->setPos(point);if (name.toLower() == tr("global parameters"))
			mode = globalParams;
			image->setToolTip(tr("List of global parameters in this model"));

			scene->insert(tr("Global parameters icon added"),image);
			mainWindow->sendEscapeSignal(this);
			
			basicInfoTool->select(0);
			return;
		}
		
		items = scene->items(point);
		NodeGraphicsItem * node = 0;
		ItemHandle * handle = 0;
		for (int i=0; i < items.size(); ++i)
			if ((node = NodeGraphicsItem::cast(getGraphicsItem(items[i]))) && 
				(handle = node->handle()) &&
				(handle->hasNumericalData(tr("Initial Value"))) &&
				(handle->hasNumericalData(tr("Parameters"))) &&
				(handle->hasTextData(tr("Assignments"))))
			{
				break;
			}
			else
			{
				handle = 0;
				node = 0;
			}
		
		if (!node || !handle)
		{
			QMessageBox::information(this,tr("Cannot insert function"),tr("Select an item with a quantitative value (e.g. concentration)"));
			return;
		}
		
		QString name = handle->fullName();
		DataTable<QString> assignments(handle->textDataTable(tr("Assignments")));
		DataTable<qreal> parameters(handle->numericalDataTable(tr("Parameters")));
		
		ConnectionGraphicsItem * connection = new ConnectionGraphicsItem;
		connection->lineType = ConnectionGraphicsItem::line;
		connection->curveSegments += ConnectionGraphicsItem::CurveSegment(1,new ConnectionGraphicsItem::ControlPoint(connection,node));
		NodeGraphicsItem * image = new NodeGraphicsItem;
		QString appDir = QApplication::applicationDirPath();
		NodeGraphicsReader reader;
		QString command;
		
		if (mode == addingStep)
		{
			reader.readXml(image, tr(":/images/stepFunc.xml"));
			image->setToolTip(tr("Step function"));	
			command = tr("Step function inserted");
			assignments.value( "self" , 0 ) = name + tr(".step_height/(1.0 + exp((") + name + tr(".step_time^") + name + tr(".step_steepness) - (time^") + name + tr(".step_steepness)))");
			parameters.value( tr("step_height"), 0 ) = 1.0;
			parameters.value( tr("step_time"), 0 ) = 2.0;
			parameters.value( tr("step_steepness"), 0 ) = 4.0;
		}
		else		
		if (mode == addingPulse)
		{
			reader.readXml(image, tr(":/images/pulseFunc.xml"));
			image->setToolTip(tr("Impulse function"));
			command = tr("Impulse function inserted");
			assignments.value( "self" , 0 ) = name + tr(".impulse_height*exp( - ((") + name + tr(".impulse_width*(time - ") + name + tr(".impulse_time))^2))");
			parameters.value( tr("impulse_height"), 0 ) = 1.0;
			parameters.value( tr("impulse_width"), 0 ) = 2.0;
			parameters.value( tr("impulse_time"), 0 ) = 4.0;
		}
		else
		if (mode == addingWave)
		{
			reader.readXml(image, tr(":/images/sinFunc.xml"));
			image->setToolTip(tr("Sine function"));
			command = tr("Sine function inserted");
			assignments.value( "self" , 0 ) = name + tr(".sin_amplitude + ") + name + tr(".sin_amplitude*sin(time*") + name + tr(".sin_frequency)");
			parameters.value( tr("sin_amplitude"), 0 ) = 1.0;
			parameters.value( tr("sin_frequency"), 0 ) = 2.0;
		}
		
		image->className = tr("Forcing function");
		image->normalize();
		image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),
			image->defaultSize.height()/image->sceneBoundingRect().height());
		image->setPos(QPointF(node->sceneBoundingRect().left() - 100.0, node->scenePos().y()));
		connection->curveSegments += ConnectionGraphicsItem::CurveSegment(1,new ConnectionGraphicsItem::ControlPoint(connection,image));
		connection->className = tr("Forcing function");
		connection->defaultPen = QPen(QColor(10,155,10),3.0);
		connection->setPen(connection->defaultPen);
		connection->lineType = ConnectionGraphicsItem::line;

		QList<QUndoCommand*> list;
		QList<QGraphicsItem*> newItems;
		newItems << image << connection;

		list << (new InsertGraphicsCommand(command, scene, newItems))
			 << (new ChangeDataCommand<qreal>(command, &handle->numericalDataTable(tr("Parameters")), &parameters))
			 << (new ChangeDataCommand<QString>(command, &handle->textDataTable(tr("Assignments")), &assignments));
		
		scene->network->push(new CompositeCommand(command,list));
		emit dataChanged(QList<ItemHandle*>() << handle);
		emit itemsInserted(scene,newItems,QList<ItemHandle*>());
	}

	void SimulationEventsTool::escapeSignal(const QWidget* )
    {
		if (mode != none && currentScene())
			currentScene()->useDefaultBehavior(true);

		mode = none;
    }

	void SimulationEventsTool::mouseDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem * item, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
	{
		if (!scene || modifiers != 0 || button == Qt::RightButton || !scene->networkWindow) return;

		NodeGraphicsItem * node = NodeGraphicsItem::cast(item);
		if (!node) 
			return;

		ItemHandle * globalHandle = scene->localHandle();
		if (!globalHandle)
			globalHandle = scene->globalHandle();

		if (node && node->className == tr("Event function"))
		{
			if (!globalHandle->hasTextData(tr("Events")))
			{
				DataTable<QString> events;
				events.resize(0,1);
				events.setColumnName(0, tr("event"));
				events.description() = tr("Events: set of triggers and events. The row names are the triggers, and the first column contains a string describing one or more events, usually an assignment.");
				globalHandle->textDataTable(tr("Events")) = events;
			}
			select(0);
		}
		else
		if (node && node->className == tr("Global parameters"))
		{
			QWidget * widget = mainWindow->tool(tr("Parameters"));
			if (!widget) return;
			
			BasicInformationTool * basicInfoTool = static_cast<BasicInformationTool*>(widget);
			if (!globalHandle->hasNumericalData(tr("Parameters")))
			{
				DataTable<double> params;
				params.resize(0,3);
				params.setColumnName(0,tr("value"));
				params.setColumnName(1,tr("min"));
				params.setColumnName(2,tr("max"));
				params.description() =  tr("Parameters: an Nx3 table storing all the real attributes for this item. Row names are the attribute names. First column holds the values. Second and third columns hold the upper and lower bounds.");
				globalHandle->numericalDataTable(tr("Parameters")) = params;
			}
			basicInfoTool->select(0);
		}
		else
		if (node && node->className == tr("Forcing function"))
		{
			QList<NodeGraphicsItem*> nodes = node->connectedNodes();
			for (int i=0; i < nodes.size(); ++i)
				if (nodes[i])
					scene->selected() += nodes[i];
			scene->select(0);
			emit showAssignments(0);
		}
	}

	void SimulationEventsTool::keyPressed(GraphicsScene* scene,QKeyEvent * keyEvent)
	{
		int key = keyEvent->key();
		if (!scene || !scene->useDefaultBehavior() || scene->selected().size() != 1 ||
			!(key == Qt::Key_Enter || key == Qt::Key_Return)
			) 
			return;

		QGraphicsItem * item = scene->selected()[0];
		mouseDoubleClicked ( scene, QPointF(0,0), item, Qt::LeftButton, 0);
	}

	void SimulationEventsTool::itemsRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& items, QList<ItemHandle*>&, QList<QUndoCommand*>& commands)
	{
		if (!scene || !scene->network) return;

		ItemHandle * globalHandle = scene->network->globalHandle();

		if (!globalHandle) return;

		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;
		
		for (int i=0; i < items.size(); ++i)
			if ( (node = NodeGraphicsItem::cast(items[i])) && 
				 (node->handle() == 0) && 
 			 	 (node->className == tr("Event function")) && 
 				 globalHandle->hasTextData(tr("Events")))
				{
					DataTable<QString> emptyData;
					commands << new ChangeTextDataCommand(tr("Events removed"),&globalHandle->textDataTable(tr("Events")),&emptyData);
					break;
				}


		QList<DataTable<QString>*> newTextTables, oldTextTables;
		QList<DataTable<qreal>*> newNumericalTables, oldNumericalTables;
		QList<ItemHandle*> handles;
		
		for (int i=0; i < items.size(); ++i)
		{
			ItemHandle * nodeHandle = 0;
			
			if ( (connection = ConnectionGraphicsItem::cast(items[i])) && 
				 (connection->handle() == 0) && 
 			 	 (connection->className == tr("Forcing function")))
				{
					QList<NodeGraphicsItem*> nodes = connection->nodes();
					for (int j=0; j < nodes.size(); ++j)
						if (nodes[j] && (nodeHandle = nodes[j]->handle()))
							break;
				}
			else
			if ( (node = NodeGraphicsItem::cast(items[i])) && 
				 (node->handle() == 0) && 
 			 	 (node->className == tr("Forcing function")))
				{
					QList<NodeGraphicsItem*> nodes = node->connectedNodes();
					for (int j=0; j < nodes.size(); ++j)
						if (nodes[j] && (nodeHandle = nodes[j]->handle()))
							break;
				}
			
			if (nodeHandle && nodeHandle->hasTextData(tr("Assignments")) && nodeHandle->hasNumericalData(tr("Parameters")))
			{
				handles << nodeHandle;
			
				DataTable<QString> * newData1 = new DataTable<QString>(nodeHandle->textDataTable(tr("Assignments")));
				newData1->removeRow("self");
				oldTextTables << &(nodeHandle->textDataTable(tr("Assignments")));
				newTextTables << newData1;
			
				DataTable<qreal> * newData2 = new DataTable<qreal>(nodeHandle->numericalDataTable(tr("Parameters")));
				newData2->removeRow(tr("step_height"));
				newData2->removeRow(tr("step_time"));
				newData2->removeRow(tr("step_steepness"));
				newData2->removeRow(tr("impulse_height"));
				newData2->removeRow(tr("impulse_width"));
				newData2->removeRow(tr("impulse_time"));
				newData2->removeRow(tr("sin_amplitude"));
				newData2->removeRow(tr("sin_frequency"));
			
				oldNumericalTables << &(nodeHandle->numericalDataTable(tr("Parameters")));
				newNumericalTables << newData2;
			}
		}

		if (!newNumericalTables.isEmpty() || !newTextTables.isEmpty())
		{
			commands << new Change2DataCommand<qreal,QString>(tr("Forcing function changed"),oldNumericalTables,newNumericalTables, oldTextTables,newTextTables);

			for (int i=0; i < newNumericalTables.size(); ++i)
				delete newNumericalTables[i];
			
			for (int i=0; i < newTextTables.size(); ++i)
				delete newTextTables[i];
		}
	}

	void SimulationEventsTool::updateTable()
	{
		if (!currentNetwork()) return;

		ItemHandle * globalHandle = currentNetwork()->globalHandle();

		if (!globalHandle) return;

		eventsListWidget.clear();

		QStringList ifthens;

		DataTable<QString> * sDataTable = 0;


		if (globalHandle->hasTextData(tr("Events")))
		{
			sDataTable = &(globalHandle->textDataTable(tr("Events")));
			for (int j=0; j < sDataTable->rows(); ++j)
			{
				ifthens << (tr("WHEN  ") + sDataTable->rowName(j) + tr("   DO   ") + sDataTable->value(j,0));
			}
		}

		eventsListWidget.addItems(ifthens);

		eventsListWidget.setVisible((eventsListWidget.count() > 0));
	}

	void SimulationEventsTool::setupDialogs()
	{
		if (!eventDialog) return;

		//event dialog
		eventDialog->setSizeGripEnabled(true);

		QGridLayout * layout = new QGridLayout;
		QPushButton * okButton = new QPushButton("OK");
		connect(okButton,SIGNAL(released()),eventDialog,SLOT(accept()));
		QPushButton * cancelButton = new QPushButton("Cancel");
		connect(cancelButton,SIGNAL(released()),eventDialog,SLOT(reject()));
		QLabel * label1 = new QLabel(tr("Trigger : "));
		QLabel * label2 = new QLabel(tr("Respose : "));

		eventIf = new QLineEdit(tr(""));
		eventIf->setFixedHeight(20);
		eventThen = new QLineEdit(tr(""));
		eventThen->setFixedHeight(20);

		layout->addWidget(label1,0,0,Qt::AlignLeft);
		layout->addWidget(label2,1,0,Qt::AlignLeft);
		layout->addWidget(eventIf,0,1);
		layout->addWidget(eventThen,1,1);

		QHBoxLayout * okCancelLayout = new QHBoxLayout;
		okCancelLayout->addWidget(okButton);
		okCancelLayout->addWidget(cancelButton);
		layout->addLayout(okCancelLayout,2,1,Qt::AlignRight);

		eventDialog->setWindowTitle(tr("New Event"));
		layout->setColumnStretch(1,3);
		eventDialog->setLayout(layout);

		connect(eventDialog,SIGNAL(accepted()),this,SLOT(eventDialogFinished()));
		connect(eventThen,SIGNAL(returnPressed()),eventDialog,SIGNAL(accepted()));
		connect(&eventsListWidget,SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(editEventsList(QListWidgetItem*)));

		oldEvent = tr("");
	}

	void SimulationEventsTool::keyPressEvent(QKeyEvent* keyEvent)
	{
		if (keyEvent->key() == Qt::Key_Delete)
			removeEvents();

	}

	void SimulationEventsTool::editEventsList(QListWidgetItem* item)
	{
		if (!item) return;
		QString text = item->text();
		QRegExp regexp("WHEN\\s+(.+)\\s+DO\\s+(.+)");
		regexp.indexIn(text);
		if (eventIf && eventThen && eventDialog && regexp.numCaptures() > 0)
		{
			eventIf->setText(regexp.cap(1));
			eventThen->setText(regexp.cap(2));
			oldEvent = regexp.cap(1);
			eventDialog->resize(regexp.cap(2).length()*6,100);
			eventDialog->exec();
		}
	}

	void SimulationEventsTool::eventDialogFinished()
	{
		NetworkHandle * win = currentNetwork();

		if (!win || eventIf == 0 || eventThen == 0) return;

		QString ifs = eventIf->text();
		QString thens = eventThen->text();

		if (ifs.isEmpty() || thens.isEmpty()) return;

		ItemHandle * lastItem = win->globalHandle();

		if (lastItem == 0) return;

		if (!lastItem->hasTextData(tr("Events")))
			lastItem->textDataTable(tr("Events")) = DataTable<QString>();

		
		if (!parseRateString(win, lastItem, ifs))
			return;

		if (!parseRateString(win, lastItem, thens))
			return;

		DataTable<QString> newData(lastItem->textDataTable(tr("Events")));

		if (!oldEvent.isEmpty())
		{
			int k = newData.rowNames().indexOf(oldEvent);
			if (k >= 0)
				newData.setRowName(k,ifs);
		}

		if (!newData.hasRow(ifs))
		{
			newData.insertRow(newData.rows(),ifs);
		}

		newData.value(ifs,0) = thens;

		win->changeData(tr("when ") + ifs + tr(" do ") + thens,lastItem,tr("Events"),&newData);

		oldEvent = tr("");
	}

	void SimulationEventsTool::addEvent()
	{
		if (eventDialog)
			eventDialog->exec();
	}

	void SimulationEventsTool::removeEvents()
	{
		NetworkHandle * win = currentNetwork();

		if (eventsListWidget.currentItem() && win)
		{
			QRegExp regexp("WHEN\\s+(.+)\\s+DO\\s+(.+)");
			regexp.indexIn( eventsListWidget.currentItem()->text() );
			if (regexp.numCaptures() < 2) return;

			int n = eventsListWidget.currentRow();
			int j = 0;

			QList<ItemHandle*> itemHandles;

			itemHandles << win->globalHandle();

			for (int i=0; i < itemHandles.size(); ++i)
			{
				if (itemHandles[i] && itemHandles[i]->hasTextData(tr("Events")))
				{
					DataTable<QString> dat(itemHandles[i]->textDataTable(tr("Events")));
					if ( (j + dat.rows()) > n )
					{
						int k = n - j;
						if (k > -1)
						{
							QString name = dat.rowName(k);
							dat.removeRow(k);
							
							win->changeData(tr("event ") + name + tr(" removed"),itemHandles[i],tr("Events"),&dat);
							
							return;
						}
					}
				}
			}
		}
	}

	void SimulationEventsTool::insertButtonPressed(const QString& name)
	{
		GraphicsScene * scene = currentScene();
		if (!mainWindow || !scene) return;

		if (name.toLower() == tr("new event"))
			mode = addingEvent;
		
		if (name.toLower() == tr("global parameters"))
			mode = globalParams;
		
		if (name.toLower() == tr("step input"))
			mode = addingStep;
			
		if (name.toLower() == tr("impulse"))
			mode = addingPulse;
			
		if (name.toLower() == tr("wave input"))
			mode = addingWave;
		
		if (mode != none)
			scene->useDefaultBehavior(false);
	}


	/******************
	C API
	******************/
	SimulationEventsTool_FToS * SimulationEventsTool::fToS;

	void SimulationEventsTool::connectTCFunctions()
	{
		connect(fToS,SIGNAL(getEventTriggers(QSemaphore*,QStringList*)),this,SLOT(getEventTriggers(QSemaphore*,QStringList*)));
		connect(fToS,SIGNAL(getEventResponses(QSemaphore*,QStringList*)),this,SLOT(getEventResponses(QSemaphore*,QStringList*)));
		connect(fToS,SIGNAL(addEvent(QSemaphore*,const QString&, const QString&)),this,SLOT(addEvent(QSemaphore*,const QString&, const QString&)));
	}

	typedef void (*tc_SimulationEventsTool_api)(
		tc_strings (*getEventTriggers)(),
		tc_strings (*getEventResponses)(),
		void (*addEvent)(const char*, const char*)
		);

	void SimulationEventsTool::setupFunctionPointers( QLibrary * library )
	{
		tc_SimulationEventsTool_api f = (tc_SimulationEventsTool_api)library->resolve("tc_SimulationEventsTool_api");
		if (f)
		{
			f(
				&(_getEventTriggers),
				&(_getEventResponses),
				&(_addEvent)
				);
		}
	}

	void SimulationEventsTool::getEventTriggers(QSemaphore* sem,QStringList* list)
	{
		NetworkHandle * win = currentNetwork();
		if (list && win)
		{
			QList<ItemHandle*> items = win->handles();

			QList<ItemHandle*> visited;
			QRegExp regex(tr("\\.(?!\\d)"));
			for (int i=0; i < items.size(); ++i)
			{
				if (items[i] && !visited.contains(items[i]) && items[i]->hasTextData(tr("Events")))
				{
					QString s;
					QStringList lst = items[i]->textDataTable(tr("Events")).rowNames();

					for (int j=0; j < lst.size(); ++j)
					{
						s = lst[j];
						s.replace(regex,tr("_"));

						(*list) << items[i]->fullName(tr("_")) + tr("_") + s;
					}
				}
			}
		}
		if (sem)
			sem->release();
	}

	void SimulationEventsTool::getEventResponses(QSemaphore* sem,QStringList* list)
	{
		NetworkHandle * win = currentNetwork();
		if (list && win)
		{
			QList<ItemHandle*> items = win->handles();

			QList<ItemHandle*> visited;
			QRegExp regex(tr("\\.(?!\\d)"));
			for (int i=0; i < items.size(); ++i)
			{
				if (items[i] &&
					!visited.contains(items[i]) &&
					items[i]->hasTextData(tr("Events")) &&
					items[i]->textDataTable(tr("Events")).columns() > 0)
				{
					DataTable<QString>& dat = items[i]->textDataTable(tr("Events"));
					for (int j=0; j < dat.rows(); ++j)
					{
						QString s = dat.value(j,0);
						s.replace(regex,tr("_"));

						(*list) << s;
					}
				}
			}
		}
		if (sem)
			sem->release();
	}

	void SimulationEventsTool::addEvent(QSemaphore* sem,const QString& trigger, const QString& event)
	{
		ItemHandle * item = 0;
		NetworkHandle * network = currentNetwork();
		if (network)
		{
			if (sem)
				sem->release();
			return;
		}
		
		item = network->globalHandle();

		if (item && !trigger.isEmpty() && !event.isEmpty())
		{
			if (!item->hasTextData(tr("Events")))
				item->textDataTable(tr("Events")) = DataTable<QString>();

			DataTable<QString> dat = item->textDataTable(tr("Events"));

			QRegExp regex(QString("([A-Za-z0-9])_([A-Za-z])"));

			QString s1 = trigger, s2 = event;
			s1.replace(regex,QString("\\1.\\2"));
			s2.replace(regex,QString("\\1.\\2"));

			dat.value(s1,0) = s2;
			network->changeData(tr("new event: when ") + s1 + tr(" do ") + s2,item,tr("Events"),&dat);
		}
		if (sem)
			sem->release();
	}

	tc_strings SimulationEventsTool::_getEventTriggers()
	{
		return fToS->getEventTriggers();
	}

	tc_strings SimulationEventsTool_FToS::getEventTriggers()
	{
		QSemaphore * s = new QSemaphore(1);
		QStringList p;
		s->acquire();
		emit getEventTriggers(s,&p);
		s->acquire();
		s->release();
		delete s;
		return (tc_strings)ConvertValue(p);
	}

	tc_strings SimulationEventsTool::_getEventResponses()
	{
		return fToS->getEventResponses();
	}

	tc_strings SimulationEventsTool_FToS::getEventResponses()
	{
		QSemaphore * s = new QSemaphore(1);
		QStringList p;
		s->acquire();
		emit getEventResponses(s,&p);
		s->acquire();
		s->release();
		delete s;
		return (tc_strings)ConvertValue(p);
	}

	void SimulationEventsTool::_addEvent( const char* a, const char* b)
	{
		return fToS->addEvent(a,b);
	}

	void SimulationEventsTool_FToS::addEvent(const char* a, const char* b)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit addEvent(s,ConvertValue(a),ConvertValue(b));
		s->acquire();
		s->release();
		delete s;
	}

	bool SimulationEventsTool::parseRateString(NetworkHandle * win, ItemHandle * handle, QString& s)
	{
		return EquationParser::validate(win, handle, s, QStringList() << "time" << "Time" << "TIME");
	}

}

