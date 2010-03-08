/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This class adds the "attributes" data to each item in Tinkercell.
Two types of attributes are added -- "Numerical Attributes" and "Text Attributes".
Attributes are essentially a <name,value> pair that are used to characterize an item.

The SimulationEventsTool also comes with two GraphicalTools, one for text attributes and one
for numerical attributes. The buttons are drawn as NodeGraphicsItems using the datasheet.xml and
textsheet.xml files that define the NodeGraphicsItems.

****************************************************************************/
#include <QToolTip>
#include "NetworkWindow.h"
#include "SymbolsTable.h"
#include "GraphicsScene.h"
#include "EquationParser.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "SimulationEventTool.h"
#include "CatalogWidget.h"
#include "ModuleTool.h"

#include "muParserDef.h"
#include "muParser.h"
#include "muParserInt.h"
#include <QGroupBox>

namespace Tinkercell
{
	void SimulationEventsTool::select(int)
	{
		NetworkWindow * net = currentWindow();
		if (!net) return;

		if (dockWidget && dockWidget->widget() != this)
			dockWidget->setWidget(this);

		openedByUser = true;

		updateTable();
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

	void SimulationEventsTool::deselect(int)
	{
		if (openedByUser && (!dockWidget || dockWidget->isFloating()))
		{
			openedByUser = false;
			if (parentWidget() != 0)
				parentWidget()->hide();
			else
				hide();
		}
	}

	void SimulationEventsTool::toolLoaded(Tool * tool)
	{
		if (!tool) return;

		if (tool->name == tr("Parts and Connections Catalog"))
		{
			CatalogWidget * catalog = static_cast<CatalogWidget*>(tool);

			connect(this,SIGNAL(addNewButtons(const QList<QToolButton*>&,const QString&)),
					catalog,SLOT(addNewButtons(const QList<QToolButton*>&,const QString&)));
			
			connect(catalog,SIGNAL(buttonPressed(const QString&)),
					this,SLOT(insertButtonPressed(const QString&)));

			QToolButton * clockButton = new QToolButton;
			clockButton->setText(tr("New event"));
			clockButton->setIcon(QIcon(QPixmap(tr(":/images/clock.png"))));
			clockButton->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
			clockButton->setToolTip(tr("An event is an action that occurs as a reponse to a given condition"));

			QToolButton * stepButton = new QToolButton;
			stepButton->setText(tr("Step input"));
			stepButton->setIcon(QIcon(QPixmap(tr(":/images/stepFunc.png"))));
			stepButton->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
			stepButton->setToolTip(tr("Insert a step function as input for one of the variables in the model"));

			QToolButton * pulseButton = new QToolButton;
			pulseButton->setText(tr("Impulse"));
			pulseButton->setIcon(QIcon(QPixmap(tr(":/images/pulseFunc.png"))));
			pulseButton->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
			pulseButton->setToolTip(tr("Insert an delta function as as input for one of the variables in the model"));

			QToolButton * sinButton = new QToolButton;
			sinButton->setText(tr("Wave input"));
			sinButton->setIcon(QIcon(QPixmap(tr(":/images/sinFunc.png"))));
			sinButton->setToolButtonStyle (Qt::ToolButtonTextUnderIcon);
			sinButton->setToolTip(tr("Insert a sin function as as input for one of the variables in the model"));

			emit addNewButtons(
				QList<QToolButton*>() << clockButton << stepButton << pulseButton << sinButton,
				tr("Inputs"));
		}
	}


	bool SimulationEventsTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow)
		{
			eventDialog = new QDialog(mainWindow);
			setupDialogs();

			connect(mainWindow,SIGNAL(windowClosing(NetworkWindow * , bool *)),this,SLOT(sceneClosing(NetworkWindow * , bool *)));
			
			connect(this, SIGNAL(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
					mainWindow,SIGNAL(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),
					this,SLOT(escapeSignal(const QWidget*)));

			connect(mainWindow,SIGNAL(mousePressed(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
				this,SLOT(sceneClicked(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			connect(mainWindow,SIGNAL(itemsRemoved(GraphicsScene * , const QList<QGraphicsItem*>& , const QList<ItemHandle*>&)),
				this,SLOT(itemsRemoved(GraphicsScene * , const QList<QGraphicsItem*>& , const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(mouseDoubleClicked (GraphicsScene * , QPointF, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers)),
				this,SLOT(mouseDoubleClicked (GraphicsScene * , QPointF, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(keyPressed(GraphicsScene* ,QKeyEvent * )),
				this,SLOT(keyPressed(GraphicsScene* ,QKeyEvent *)));

			connect(mainWindow,SIGNAL(historyChanged(int)),this,SLOT(historyUpdate(int)));

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			toolLoaded(mainWindow->tool(tr("Parts and Connections Catalog")));

			setWindowTitle(name);

			dockWidget = mainWindow->addToolWindow(this,MainWindow::DockWidget, Qt::BottomDockWidgetArea,Qt::NoDockWidgetArea);

			if (dockWidget)
			{
				dockWidget->move(mainWindow->geometry().bottomRight() - QPoint(sizeHint().width()*2,sizeHint().height()*2));

				connect(dockWidget,SIGNAL(visibilityChanged(bool)),this,SLOT(visibilityChanged(bool)));

				dockWidget->setWindowFlags(Qt::Tool);
				dockWidget->setAttribute(Qt::WA_ContentsPropagated);
				dockWidget->setPalette(QPalette(QColor(255,255,255,255)));
				dockWidget->setAutoFillBackground(true);
				//dockWidget->setWindowOpacity(0.9);

				QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);

				settings.beginGroup("SimulationEventsTool");
				//dockWidget->resize(settings.value("size", sizeHint()).toSize());
				//dockWidget->move(settings.value("pos", dockWidget->pos()).toPoint());

				dockWidget->hide();
				if (settings.value("floating", true).toBool())
					dockWidget->setFloating(true);

				settings.endGroup();
			}
		}
		return (mainWindow != 0);
	}

	void SimulationEventsTool::sceneClosing(NetworkWindow * , bool *)
	{
		QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);

		if (dockWidget)
		{
			settings.beginGroup("SimulationEventsTool");
			//settings.setValue("floating", dockWidget && dockWidget->isFloating());
			settings.setValue("size", dockWidget->size());
			settings.setValue("pos", dockWidget->pos());
			settings.endGroup();
		}
	}

	void SimulationEventsTool::historyUpdate(int)
	{
		if (isVisible() || (parentWidget() && parentWidget() != mainWindow && parentWidget()->isVisible()))
			updateTable();
	}

	SimulationEventsTool::SimulationEventsTool() : Tool(tr("Events and Inputs"),tr("Modeling"))
	{
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
		dockWidget = 0;

		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(groupBox);
		setLayout(layout);

		connectTCFunctions();
	}

	void SimulationEventsTool::visibilityChanged(bool b)
	{
		if (b && dockWidget && groupBox)
		{
			dockWidget->setWidget(groupBox);
		}
	}

	QSize SimulationEventsTool::sizeHint() const
	{
		return QSize(400, 200);
	}

	void SimulationEventsTool::sceneClicked(GraphicsScene *scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
	{
		if (mode == none || button == Qt::RightButton || !scene || !scene->symbolsTable || scene->useDefaultBehavior)
		{
			if (dockWidget && dockWidget->widget() != this)
				dockWidget->setWidget(this);

			if (dockWidget && dockWidget->isVisible())
				dockWidget->hide();

			return;
		}

		if (mode == addingEvent)
		{
			QString appDir = QApplication::applicationDirPath();
			NodeGraphicsItem * image = new NodeGraphicsItem;
			NodeGraphicsReader reader;
			reader.readXml(image, appDir + tr("/OtherItems/clock.xml"));
			image->normalize();
			image->className = tr("Event function");
			image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),
				image->defaultSize.height()/image->sceneBoundingRect().height());

			image->setPos(point);
			image->setToolTip(tr("List of events in this model"));

			scene->insert(tr("Events box inserted"),image);

			return;
		}
		
		QList<QGraphicsItem*> items = scene->items(point);
		NodeGraphicsItem * node = 0;
		ItemHandle * handle = 0;
		for (int i=0; i < items.size(); ++i)
			if ((node = NodeGraphicsItem::cast(getGraphicsItem(items[i]))) && 
				(handle = node->handle()) &&
				(handle->hasNumericalData(tr("Initial Value"))) &&
				(handle->hasNumericalData(tr("Numerical Attributes"))) &&
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
		DataTable<qreal> parameters(handle->numericalDataTable(tr("Numerical Attributes")));
		
		ConnectionGraphicsItem * connection = new ConnectionGraphicsItem;
		connection->lineType = ConnectionGraphicsItem::line;
		connection->curveSegments += ConnectionGraphicsItem::CurveSegment(1,new ConnectionGraphicsItem::ControlPoint(connection,node));
		NodeGraphicsItem * image = new NodeGraphicsItem;
		QString appDir = QApplication::applicationDirPath();
		NodeGraphicsReader reader;
		QString command;
		
		if (mode == addingStep)
		{
			reader.readXml(image, appDir + tr("/OtherItems/stepFunc.xml"));
			image->setToolTip(tr("Step function"));	
			command = tr("Step function inserted");
			assignments.value( handle->fullName() , 0 ) = name + tr(".step_height/(1.0 + exp(pow(") + name + tr(".step_time,") + name + tr(".step_steepness) - pow(time,") + name + tr(".step_steepness)))");
			parameters.value( tr("step_height"), 0 ) = 1.0;
			parameters.value( tr("step_time"), 0 ) = 2.0;
			parameters.value( tr("step_steepness"), 0 ) = 4.0;
		}
		else		
		if (mode == addingPulse)
		{
			reader.readXml(image, appDir + tr("/OtherItems/pulseFunc.xml"));
			image->setToolTip(tr("Impulse function"));
			command = tr("Impulse function inserted");
			assignments.value( handle->fullName() , 0 ) = name + tr(".impulse_height*exp( - pow(") + name + tr(".impulse_width*(time - ") + name + tr(".impulse_time),2))");
			parameters.value( tr("impulse_height"), 0 ) = 1.0;
			parameters.value( tr("impulse_width"), 0 ) = 2.0;
			parameters.value( tr("impulse_time"), 0 ) = 4.0;
		}
		else
		if (mode == addingWave)
		{
			reader.readXml(image, appDir + tr("/OtherItems/sinFunc.xml"));
			image->setToolTip(tr("Sine function"));
			command = tr("Sine function inserted");
			assignments.value( handle->fullName() , 0 ) = name + tr(".sin_amplitude + ") + name + tr(".sin_amplitude*sin(time*") + name + tr(".sin_frequency)");
			parameters.value( tr("sin_amplitude"), 0 ) = 1.0;
			parameters.value( tr("sin_frequency"), 0 ) = 2.0;
		}
		
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
		
		list << new InsertGraphicsCommand(command, scene, newItems)
			 << new ChangeDataCommand<qreal>(command, &handle->numericalDataTable(tr("Numerical Attributes")), &parameters)
			 << new ChangeDataCommand<QString>(command, &handle->textDataTable(tr("Assignments")), &assignments);
		//scene->insert(command,newItems);
		scene->networkWindow->history.push(new CompositeCommand(command,list));
		emit itemsInserted(scene,newItems,QList<ItemHandle*>());
	}

	void SimulationEventsTool::escapeSignal(const QWidget* )
    {
		mode = none;
    }

	void SimulationEventsTool::mouseDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem * item, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
	{
		if (!scene || modifiers != 0 || button == Qt::RightButton || !scene->networkWindow) return;

		NodeGraphicsItem * node = NodeGraphicsItem::cast(item);
		ItemHandle * modelItem = scene->networkWindow->modelItem();

		if (!node || node->handle() || !modelItem || node->className != tr("Event function")) return;

		if (!modelItem->hasTextData(tr("Events")))
		{
			DataTable<QString> events;
			events.resize(0,1);
			events.colName(0) = tr("event");
			events.description() = tr("Events: set of triggers and events. The row names are the triggers, and the first column contains a string describing one or more events, usually an assignment.");
			modelItem->data->textData.insert(tr("Events"),events);
		}
		
		select(0);
	}

	void SimulationEventsTool::keyPressed(GraphicsScene* scene,QKeyEvent * keyEvent)
	{
		int key = keyEvent->key();
		if (!scene || !scene->useDefaultBehavior || scene->selected().size() != 1 ||
			!(key == Qt::Key_Enter || key == Qt::Key_Return)
			) 
			return;

		QGraphicsItem * item = scene->selected()[0];

		NodeGraphicsItem * node = NodeGraphicsItem::cast(item);
		ItemHandle * modelItem = scene->networkWindow->modelItem();

		if (!node || node->handle() || !modelItem || node->className != tr("Event function")) return;

		if (!modelItem->hasTextData(tr("Events")))
		{
			DataTable<QString> events;
			events.resize(0,1);
			events.colName(0) = tr("event");
			events.description() = tr("Events: set of triggers and events. The row names are the triggers, and the first column contains a string describing one or more events, usually an assignment.");
			modelItem->data->textData.insert(tr("Events"),events);
		}
		
		select(0);
	}

	void SimulationEventsTool::itemsRemoved(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>& )
	{
		if (!scene || !scene->networkWindow) return;

		ItemHandle * modelItem = scene->networkWindow->modelItem();

		if (!modelItem) return;

		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;
		
		for (int i=0; i < items.size(); ++i)
			if ( (node = NodeGraphicsItem::cast(items[i])) && 
				 (node->handle() == 0) && 
 			 	 (node->className == tr("Event function")) && 
 				 modelItem->hasTextData(tr("Events")))
				{
					DataTable<QString> emptyData;
					scene->changeData(tr("Events removed"),modelItem,tr("Events"),&emptyData);					
					break;
				}


		QList<DataTable<QString>*> newTextTables, oldTextTables;
		QList<DataTable<qreal>*> newNumericalTables, oldNumericalTables;
		QList<ItemHandle*> handles;
		
		for (int i=0; i < items.size(); ++i)
			if ( (connection = ConnectionGraphicsItem::cast(items[i])) && 
				 (connection->handle() == 0) && 
 			 	 (connection->className == tr("Forcing function")))
				{
					QList<NodeGraphicsItem*> nodes = connection->nodes();
					ItemHandle * nodeHandle = 0;
					for (int j=0; j < nodes.size(); ++j)
						if (nodes[j] && (nodeHandle = nodes[j]->handle()))
							break;
					
					if (nodeHandle && nodeHandle->hasTextData(tr("Assignments")) && nodeHandle->hasNumericalData(tr("Numerical Attributes")))
					{
						handles << nodeHandle;
						
						DataTable<QString> * newData1 = new DataTable<QString>(nodeHandle->textDataTable(tr("Assignments")));
						newData1->removeRow(nodeHandle->fullName());
						newData1->removeRow(nodeHandle->name);
						oldTextTables << &(nodeHandle->textDataTable(tr("Assignments")));
						newTextTables << newData1;
						
						DataTable<qreal> * newData2 = new DataTable<qreal>(nodeHandle->numericalDataTable(tr("Numerical Attributes")));
						newData2->removeRow(tr("step_height"));
						newData2->removeRow(tr("step_time"));
						newData2->removeRow(tr("step_steepness"));
						newData2->removeRow(tr("impulse_height"));
						newData2->removeRow(tr("impulse_width"));
						newData2->removeRow(tr("impulse_time"));
						newData2->removeRow(tr("sin_amplitude"));
						newData2->removeRow(tr("sin_frequency"));
						
						oldNumericalTables << &(nodeHandle->numericalDataTable(tr("Numerical Attributes")));
						newNumericalTables << newData2;
					}
				}

		scene->changeData(tr("Forcing function changed"),handles, oldNumericalTables,newNumericalTables, oldTextTables,newTextTables);

		for (int i=0; i < newNumericalTables.size(); ++i)
			delete newNumericalTables[i];
			
		for (int i=0; i < newTextTables.size(); ++i)
			delete newTextTables[i];
	}

	void SimulationEventsTool::updateTable()
	{
		if (!currentNetwork()) return;

		ItemHandle * modelItem = currentNetwork()->modelItem();

		if (!modelItem) return;

		eventsListWidget.clear();

		QStringList ifthens;

		DataTable<QString> * sDataTable = 0;


		if (modelItem->hasTextData(tr("Events")))
		{
			sDataTable = &(modelItem->data->textData[tr("Events")]);
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
		NetworkWindow * win = currentWindow();

		if (!win || eventIf == 0 || eventThen == 0) return;

		QString ifs = eventIf->text();
		QString thens = eventThen->text();

		if (ifs.isEmpty() || thens.isEmpty()) return;

		ItemHandle * lastItem = win->modelItem();

		if (lastItem == 0 || lastItem->data == 0) return;

		if (!lastItem->hasTextData(tr("Events")))
			lastItem->data->textData[tr("Events")] = DataTable<QString>();

		
		if (!parseRateString(win, lastItem, ifs))
			return;

		if (!parseRateString(win, lastItem, thens))
			return;

		DataTable<QString> newData(lastItem->data->textData[tr("Events")]);

		if (!oldEvent.isEmpty())
		{
			int k = newData.rowNames().indexOf(oldEvent);
			if (k >= 0)
				newData.rowName(k) = ifs;
		}

		if (!newData.rowNames().contains(ifs))
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
		NetworkWindow * win = currentWindow();

		if (eventsListWidget.currentItem() && win)
		{
			QRegExp regexp("WHEN\\s+(.+)\\s+DO\\s+(.+)");
			regexp.indexIn( eventsListWidget.currentItem()->text() );
			if (regexp.numCaptures() < 2) return;

			int n = eventsListWidget.currentRow();
			int j = 0;

			QList<ItemHandle*> itemHandles;

			itemHandles << win->modelItem();

			for (int i=0; i < itemHandles.size(); ++i)
			{
				if (itemHandles[i] && itemHandles[i]->data && itemHandles[i]->hasTextData(tr("Events")))
				{
					DataTable<QString> dat(itemHandles[i]->data->textData[tr("Events")]);
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
		
		if (name.toLower() == tr("step input"))
			mode = addingStep;
			
		if (name.toLower() == tr("impulse"))
			mode = addingPulse;
			
		if (name.toLower() == tr("wave input"))
			mode = addingWave;
		
		if (mode != none)
			scene->useDefaultBehavior = false;
	}


	/******************
	C API
	******************/
	SimulationEventsTool_FToS SimulationEventsTool::fToS;

	void SimulationEventsTool::connectTCFunctions()
	{
		connect(&fToS,SIGNAL(getEventTriggers(QSemaphore*,QStringList*)),this,SLOT(getEventTriggers(QSemaphore*,QStringList*)));
		connect(&fToS,SIGNAL(getEventResponses(QSemaphore*,QStringList*)),this,SLOT(getEventResponses(QSemaphore*,QStringList*)));
		connect(&fToS,SIGNAL(addEvent(QSemaphore*,const QString&, const QString&)),this,SLOT(addEvent(QSemaphore*,const QString&, const QString&)));
	}

	typedef void (*tc_SimulationEventsTool_api)(
		char** (*getEventTriggers)(),
		char** (*getEventResponses)(),
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
		NetworkWindow * win = currentWindow();
		if (list && win)
		{
			QList<ItemHandle*> items;

			QList<ItemHandle*> from, to;
			ModuleTool::connectedItems(win->allHandles(),from,to);

			QList<ItemHandle*> visited;
			QRegExp regex(tr("\\.(?!\\d)"));
			for (int i=0; i < items.size(); ++i)
			{
				if (items[i] && !visited.contains(items[i]) && items[i]->data && items[i]->hasTextData(tr("Events")))
				{
					QString s;
					QStringList lst = items[i]->data->textData[tr("Events")].getRowNames();

					for (int j=0; j < lst.size(); ++j)
					{
						s = lst[j];
						s.replace(regex,tr("_"));
						int k = from.indexOf(items[i]);
						if (k > -1)
						{
							s.replace(items[i]->fullName(tr("_")), to[k]->fullName(tr("_")));
							(*list) << to[k]->fullName(tr("_")) + tr("_") + s;
						}
						else
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
		NetworkWindow * win = currentWindow();
		if (list && win)
		{
			QList<ItemHandle*> items;
			items << win->modelItem();

			QList<ItemHandle*> from, to;
			ModuleTool::connectedItems(win->allHandles(),from,to);

			QList<ItemHandle*> visited;
			QRegExp regex(tr("\\.(?!\\d)"));
			for (int i=0; i < items.size(); ++i)
			{
				if (items[i] &&
					!visited.contains(items[i]) &&
					items[i]->data &&
					items[i]->hasTextData(tr("Events")) &&
					items[i]->data->textData[tr("Events")].cols() > 0)
				{
					DataTable<QString>& dat = items[i]->data->textData[tr("Events")];
					for (int j=0; j < dat.rows(); ++j)
					{
						QString s = dat.value(j,0);
						s.replace(regex,tr("_"));
						int k = from.indexOf(items[i]);
						if (k > -1)
							s.replace(items[i]->fullName(tr("_")), to[k]->fullName(tr("_")));

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
		
		if (currentWindow())
			item = currentWindow()->modelItem();

		if (item && item->data && !trigger.isEmpty() && !event.isEmpty())
		{
			if (!item->hasTextData(tr("Events")))
				item->data->textData[tr("Events")] = DataTable<QString>();

			DataTable<QString> dat = item->data->textData[tr("Events")];

			QRegExp regex(QString("([A-Za-z0-9])_([A-Za-z])"));

			QString s1 = trigger, s2 = event;
			s1.replace(regex,QString("\\1.\\2"));
			s2.replace(regex,QString("\\1.\\2"));

			dat.value(s1,0) = s2;
			if (currentScene())
				currentScene()->changeData(tr("new event: when ") + s1 + tr(" do ") + s2,item,tr("Events"),&dat);
			else
				item->data->textData[tr("Events")] = dat;
		}
		if (sem)
			sem->release();
	}

	char** SimulationEventsTool::_getEventTriggers()
	{
		return fToS.getEventTriggers();
	}

	char** SimulationEventsTool_FToS::getEventTriggers()
	{
		QSemaphore * s = new QSemaphore(1);
		QStringList p;
		s->acquire();
		emit getEventTriggers(s,&p);
		s->acquire();
		s->release();
		delete s;
		return (char**)ConvertValue(p);
	}

	char** SimulationEventsTool::_getEventResponses()
	{
		return fToS.getEventResponses();
	}

	char** SimulationEventsTool_FToS::getEventResponses()
	{
		QSemaphore * s = new QSemaphore(1);
		QStringList p;
		s->acquire();
		emit getEventResponses(s,&p);
		s->acquire();
		s->release();
		delete s;
		return (char**)ConvertValue(p);
	}

	void SimulationEventsTool::_addEvent( const char* a, const char* b)
	{
		return fToS.addEvent(a,b);
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

	bool SimulationEventsTool::parseRateString(NetworkWindow * win, ItemHandle * handle, QString& s)
	{
		return EquationParser::validate(win, handle, s, QStringList() << "time");
	}

}


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::SimulationEventsTool * simulationEventsTool = new Tinkercell::SimulationEventsTool;
	main->addTool(simulationEventsTool);

}

