/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This class adds the "attributes" data to each item in Tinkercell.
 Two types of attributes are added -- "Numerical Attributes" and "Text Attributes".
 Attributes are essentially a <name,value> pair that are used to characterize an item.
 
 The AssignmentFunctionsTool also comes with two GraphicalTools, one for text attributes and one
 for numerical attributes. The buttons are drawn as PartGraphicsItems using the datasheet.xml and
 textsheet.xml files that define the PartGraphicsItems.
 
****************************************************************************/


#include "Core/GraphicsScene.h"
#include "Core/MainWindow.h"
#include "Core/OutputWindow.h"
#include "Core/PartGraphicsItem.h"
#include "Core/PartGraphicsReader.h"
#include "Core/ConnectionGraphicsItem.h"
#include "Core/TextGraphicsItem.h"
#include "BasicTools/StoichiometryTool.h"
#include "OtherTools/AssignmentFunctionsTool.h"
#include "BasicTools/ModelSummaryTool.h"
#include "PlotTools/PlotTool.h"
#include "muparser/muParser.h"
#include "muparser/muParserInt.h"
#include <QGroupBox>

namespace Tinkercell
{
	/***************************************
	        VISUAL TOOL
	*****************************************/
	AssignmentFunctionsTool::VisualTool::VisualTool() : GraphicalTool(tr("Assignments and Functions"))
	{
		QString appDir = QCoreApplication::applicationDirPath();
		openedByUser = false;
		PartGraphicsReader reader;
		reader.readXml(&item,appDir + tr("/OtherItems/func.xml"));
		item.setToolTip(tr("Functions and assignments"));
		setToolTip(tr("Functions and assignments"));
		
		item.normalize();
		item.scale(35.0/item.sceneBoundingRect().width(),35.0/item.sceneBoundingRect().height());
		editTool = 0;
		addToGroup(&item);
	}

	void AssignmentFunctionsTool::VisualTool::selected(const QList<QGraphicsItem*>& list)
	{
		if (editTool != 0)
		{
			if (editTool->dockWidget && editTool->dockWidget->widget() != editTool)
				editTool->dockWidget->setWidget(editTool);
				
			editTool->itemHandles.clear();
			for (int i=0; i < list.size(); ++i)
			{
				ItemHandle * handle = getHandle(list[i]);
				if (handle && !editTool->itemHandles.contains(handle))
					editTool->itemHandles += handle;
			}
			if (editTool->itemHandles.size() < 1) return;
			openedByUser = true;

			editTool->updateTable();
			if (editTool->dockWidget != 0)
			{
				if (editTool->dockWidget->isVisible())
					openedByUser = false;
				else
					editTool->parentWidget()->show();
			}
			else
			{
				if (editTool->isVisible())
					openedByUser = false;
				else
					editTool->show();
			}		
		}
	}

	void AssignmentFunctionsTool::VisualTool::deselected()
	{
		if (editTool != 0 && openedByUser && (!editTool->dockWidget || editTool->dockWidget->isFloating()))
		{
			openedByUser = false;
			if (editTool->dockWidget != 0)
				editTool->dockWidget->hide();
			else
				editTool->hide();
		}
	}

	/******************************************
	        ASSIGNMENT FUNCTIONS TOOL
	******************************************/
	
	void AssignmentFunctionsTool::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& list, QPointF , Qt::KeyboardModifiers )
	{
		if (scene && (isVisible() || (parentWidget() && parentWidget() != mainWindow && parentWidget()->isVisible())))
		{
			itemHandles.clear();
			ItemHandle * handle = 0;
			for (int i=0; i < list.size(); ++i)
			{
				if ((handle = getHandle(list[i])))
					itemHandles += handle;
			}
			
			updateTable();
		}
	}
	
	bool AssignmentFunctionsTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		visualTool.editTool = this;
		visualTool.setMainWindow(main);

		if (mainWindow)
		{
			functionDialog = new QDialog(mainWindow);
			setupDialogs();

			connect(mainWindow,SIGNAL(sceneClosing(GraphicsScene * , bool *)),this,SLOT(sceneClosing(GraphicsScene * , bool *)));

			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				         this,SLOT(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
			
			connect(mainWindow,SIGNAL(itemsInserted(NetworkWindow*, const QList<ItemHandle*>&)),
						  this, SLOT(itemsInserted(NetworkWindow*, const QList<ItemHandle*>&)));
			
			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			connect(mainWindow,SIGNAL(historyChanged(int)),this,SLOT(historyUpdate(int)));
			
			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));
			
			dockWidget = mainWindow->addDockingWindow(name,this,Qt::BottomDockWidgetArea,Qt::NoDockWidgetArea);
			
			if (dockWidget)
			{
				dockWidget->setWindowTitle(tr("Forcing Functions"));
				//dockWidget->move(mainWindow->geometry().bottomRight() - QPoint(sizeHint().width()*2,sizeHint().height()*2));
				
				dockWidget->setWindowFlags(Qt::Tool);				
				dockWidget->setAttribute(Qt::WA_ContentsPropagated);
				dockWidget->setPalette(QPalette(QColor(255,255,255,255)));
				dockWidget->setAutoFillBackground(true);
				//dockWidget->setWindowOpacity(0.9);
				
				QCoreApplication::setOrganizationName("TinkerCell");
				QCoreApplication::setOrganizationDomain("www.tinkercell.com");
				QCoreApplication::setApplicationName("TinkerCell");
				
				QSettings settings("TinkerCell", "TinkerCell");
				
				settings.beginGroup("AssignmentFunctionsTool");
				//dockWidget->resize(settings.value("size", sizeHint()).toSize());
				//dockWidget->move(settings.value("pos", dockWidget->pos()).toPoint());
				
				if (settings.value("floating", true).toBool())
					dockWidget->setFloating(true);

				settings.endGroup();
				dockWidget->hide();
			}
			
			toolLoaded(0);
		}
		return (mainWindow != 0);
	}
	
	void AssignmentFunctionsTool::toolLoaded(Tool*)
	{
		static bool connected1 = false;
		static bool connected2 = false;
		if (connected1 && connected2) return;
		
		if (!connected1 && mainWindow && mainWindow->tool(tr("Model Summary")))
		{
			QWidget * widget = mainWindow->tools(tr("Model Summary"));
			ModelSummaryTool * modelSummary = static_cast<ModelSummaryTool*>(widget);
			connect(modelSummary,SIGNAL(aboutToDisplayModel(const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)),
					this,SLOT(aboutToDisplayModel(const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)));
			connect(modelSummary,SIGNAL(displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)),
					this,SLOT(displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)));
			connected1 = true;
		}
		if (!connected2 && mainWindow && mainWindow->tool(tr("Graph Tool")))
		{
			QWidget * widget = mainWindow->tool(tr("Graph Tool"));
			PlotTool * plotTool = static_cast<PlotTool*>(widget);
			connect(this,SIGNAL(plot(const DataTable<qreal>&,const QString&,int,int)),
					plotTool,SLOT(plot(const DataTable<qreal>&,const QString&,int,int)));
			connected2 = true;
		}
	}
	
	void AssignmentFunctionsTool::aboutToDisplayModel(const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations)
	{
		itemHandles = items;
		updateTable();
		for (int i=0; i < updatedFunctions.size() && i < updatedFunctionNames.size(); ++i)
			equations[ updatedFunctionNames[i] ] = updatedFunctions[i];
	}
	
	void AssignmentFunctionsTool::displayModel(QTabWidget& widgets, const QList<ItemHandle*>& items, QHash<QString,qreal>& constants,QHash<QString,QString>& equations)
	{
		if (functionsListWidget.count() > 0)
		{
			widgets.addTab(this,tr("Function declarations"));
		}
		else
			if (dockWidget && dockWidget->widget() != this)
				dockWidget->setWidget(this);
	}

	void AssignmentFunctionsTool::sceneClosing(GraphicsScene * , bool *)
	{
		QCoreApplication::setOrganizationName("TinkerCell");
		QCoreApplication::setOrganizationDomain("www.tinkercell.com");
		QCoreApplication::setApplicationName("TinkerCell");
		
		QSettings settings("TinkerCell", "TinkerCell");
		
		if (dockWidget)
		{
			settings.beginGroup("AssignmentFunctionsTool");
			//settings.setValue("floating", dockWidget && dockWidget->isFloating());
			//settings.setValue("size", dockWidget->size());
			//settings.setValue("pos", dockWidget->pos());
			settings.endGroup();
		}
	}

	void AssignmentFunctionsTool::historyUpdate(int)
	{
		if (isVisible() || (parentWidget() && parentWidget() != mainWindow && parentWidget()->isVisible()))// && dockWidget && dockWidget->isVisible())
			updateTable();
	}

	void AssignmentFunctionsTool::itemsInserted(NetworkWindow* , const QList<ItemHandle*>& handles)
	{
		for (int i=0; i < handles.size(); ++i)
		{
			if (handles[i] && !handles[i]->graphicalTools.contains(&visualTool))
				handles[i]->graphicalTools += &visualTool;
				
			if (handles[i] && handles[i]->data && 
				!(handles[i]->hasTextData(tr("Assignments"))))
			{
				insertData(handles[i]);
			}
		}
	}

	AssignmentFunctionsTool::AssignmentFunctionsTool() : Tool(tr("Assignments and Functions")), visualTool()
	{	
		QFont font = this->font();
		font.setPointSize(12);
		functionsListWidget.setFont(font);
		
		QHBoxLayout * actionsLayout = new QHBoxLayout;
		
		QToolButton * addFuncAction = new QToolButton(this);
		addFuncAction->setIcon(QIcon(":/images/plus.png"));
		addFuncAction->setToolTip(tr("Add new function"));
		connect(addFuncAction,SIGNAL(pressed()),this,SLOT(addFunction()));
		
		QToolButton * removeFuncAction = new QToolButton(this);
		removeFuncAction->setIcon(QIcon(":/images/minus.png"));
		removeFuncAction->setToolTip(tr("Remove selected functions(s)"));
		connect(removeFuncAction,SIGNAL(pressed()),this,SLOT(removeFunctions()));
		
		QToolButton * question = new QToolButton(this);
		question->setIcon(QIcon(":/images/question.png"));
		
		QString message = tr("This table can be used to declare functions that can then be used in other parts of the model. For example, you may declare that A.func = sin(time) and then use A.func inside a reaction rate.");
		QMessageBox * messageBox = new QMessageBox(QMessageBox::Information,tr("About Forcing Functions"),message,QMessageBox::StandardButtons(QMessageBox::Close), const_cast<QWidget*>((QWidget*)this), Qt::WindowFlags (Qt::Dialog));
		connect(question,SIGNAL(pressed()),messageBox,SLOT(exec()));
		
		actionsLayout->addWidget(addFuncAction);
		actionsLayout->addWidget(removeFuncAction);
		actionsLayout->addStretch(1);
		actionsLayout->addWidget(question);
		
		groupBox = new QGroupBox(tr(" Functions "),this);

		QVBoxLayout * eventBoxLayout = new QVBoxLayout;
		eventBoxLayout->addWidget(&functionsListWidget,1);
		
		eventBoxLayout->addLayout(actionsLayout);
		groupBox->setLayout(eventBoxLayout);

		functionDialog = 0;
		dockWidget = 0;
		
		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(groupBox);
		setLayout(layout);
		
		connectTCFunctions();
	}
	
	QSize AssignmentFunctionsTool::sizeHint() const
	{
		return QSize(400, 200);
	}

	void AssignmentFunctionsTool::insertData(ItemHandle * handle)
	{
		if (handle == 0) return;

		DataTable<QString> functions;
		functions.resize(0,1);
		functions.colName(0) = tr("function");
		
		handle->data->textData.insert(tr("Assignments"),functions);
	}

	void AssignmentFunctionsTool::updateTable()
	{
		functionsListWidget.clear();
		
		QStringList ifthens;
		QStringList functions;
		updatedFunctions.clear();
		updatedFunctionNames.clear();

		DataTable<QString> * sDataTable = 0;
		
		for (int i=0; i < itemHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (itemHandles[i] != 0 && itemHandles[i]->data != 0)
			{
				if (itemHandles[i]->hasTextData(tr("Assignments")))
				{
					sDataTable = &(itemHandles[i]->data->textData[tr("Assignments")]);
					for (int j=0; j < sDataTable->rows(); ++j)
					{
						//if (itemHandles[i]->family() && sDataTable->rowName(j) == itemHandles[i]->family()->defaultNumericalAttribute)
						if (itemHandles[i]->fullName(tr("_")) == sDataTable->rowName(j))
						{
							sDataTable->rowName(j) = itemHandles[i]->fullName();
						}
						
						if (itemHandles[i]->fullName() == sDataTable->rowName(j) || sDataTable->rowName(j).isEmpty())
						{
							int k = sDataTable->rowNames().indexOf(itemHandles[i]->fullName());
							while (k >= 0 && k < j)
							{
								sDataTable->removeRow(k);
								k = sDataTable->rowNames().indexOf(itemHandles[i]->fullName());
								--j;
							}
							
							functions << (
										itemHandles[i]->fullName() 
										+ tr(" = ") + sDataTable->value(j,0));
							updatedFunctionNames += itemHandles[i]->fullName();
						}
						else
						{
							functions << (
										itemHandles[i]->fullName() + tr(".") 
										+ sDataTable->rowName(j) 
										+ tr(" = ") + sDataTable->value(j,0));
							updatedFunctionNames += itemHandles[i]->fullName() + tr(".") + sDataTable->rowName(j) ;
						}
						updatedFunctions += sDataTable->value(j,0);
						
					}
				}
			}
		}

		functionsListWidget.addItems(functions);
		
		functionsListWidget.setVisible((functionsListWidget.count() > 0));
		
	}

	void AssignmentFunctionsTool::setupDialogs()
	{
		if (!functionDialog) return;
		
		//functions dialog
		functionDialog->setSizeGripEnabled(true);
		
		QGridLayout * layout = new QGridLayout;
		QPushButton * okButton = new QPushButton("OK");
		connect(okButton,SIGNAL(released()),functionDialog,SLOT(accept()));
		QPushButton * cancelButton = new QPushButton("Cancel");
		connect(cancelButton,SIGNAL(released()),functionDialog,SLOT(reject()));
		QLabel * label1 = new QLabel(tr("Variable :"));
		QLabel * label2 = new QLabel(tr("Function : "));
		
		functionVar = new QLineEdit(tr(""));
		functionVar->setFixedHeight(20);
		functionDef = new QLineEdit(tr(""));
		functionDef->setFixedHeight(20);
		
		layout->addWidget(label1,0,0,Qt::AlignLeft);
		layout->addWidget(label2,1,0,Qt::AlignLeft);
		layout->addWidget(functionVar,0,1);
		layout->addWidget(functionDef,1,1);
		
		QHBoxLayout * okCancelLayout = new QHBoxLayout;
		okCancelLayout->addWidget(okButton);
		okCancelLayout->addWidget(cancelButton);
		layout->addLayout(okCancelLayout,2,1,Qt::AlignRight);
		
		functionDialog->setWindowTitle(tr("New Function"));
		layout->setColumnStretch(1,3);
		functionDialog->setLayout(layout);
		
		connect(functionDialog,SIGNAL(accepted()),this,SLOT(functionDialogFinished()));
		connect(functionDef,SIGNAL(returnPressed()),functionDialog,SIGNAL(accepted()));
		connect(&functionsListWidget,SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(editFunctionsList(QListWidgetItem*)));
		
		oldVar = tr("");
	}
	
	/*void AssignmentFunctionsTool::keyPressEvent(QKeyEvent* keyEvent)
	{
		if (keyEvent->key() == Qt::Key_Delete)
		{
			if (functionsListWidget.hasFocus())
				removeFunctions();
		}
	}*/
	
	void AssignmentFunctionsTool::editFunctionsList(QListWidgetItem* item)
	{
		if (!item) return;
		QString text = item->text();
		QRegExp regexp("([^\\.]+) = (.+)");
		
		regexp.indexIn(text);
		if (functionVar && functionDef && functionDialog && regexp.numCaptures() > 0)
		{
			functionVar->setText(regexp.cap(1));
			functionDef->setText(regexp.cap(2));
			oldVar = regexp.cap(1);
			functionDialog->resize(regexp.cap(2).length()*6,100);
			functionDialog->exec();
		}
	}
	
	void AssignmentFunctionsTool::functionDialogFinished()
	{
		if (functionVar == 0 || functionDef == 0 || itemHandles.isEmpty()) return;
		QString var = functionVar->text();
		QString func = functionDef->text();

		if (var.isEmpty() || func.isEmpty()) return;
		
		ItemHandle * lastItem = itemHandles.last();
		
		for (int i=0; i < itemHandles.size(); ++i)
			if (itemHandles[i] && itemHandles[i]->fullName().contains(var))
			{
				lastItem = itemHandles[i];
				break;
			}
		
		if (lastItem == 0 || lastItem->data == 0) return;
		
		if (!StoichiometryTool::parseRateString(currentWindow(), lastItem, func))
			return;
		
		if (!lastItem->hasTextData(tr("Assignments")))
			insertData(lastItem);
		
		GraphicsScene * scene = currentScene();
		
		if (lastItem->name == var) var = lastItem->fullName();
		
		if (scene)
		{
			DataTable<QString> newData(lastItem->data->textData[tr("Assignments")]);
			
			if (!oldVar.isEmpty())
			{
				int k = newData.rowNames().indexOf(oldVar);
				if (k >= 0)
					newData.rowName(k) = var;
			}
			
			if (!newData.rowNames().contains(var))
			{
				newData.insertRow(newData.rows(),var);
			}
			
			newData.value(var,0) = func;
			
			scene->changeData(lastItem,tr("Assignments"),&newData);
		}
		else
		{
			lastItem->data->textData[tr("Assignments")].value(var,0) = func;
		}
		
		oldVar = tr("");
	}
	
	void AssignmentFunctionsTool::addFunction()
	{
		if (functionDialog)
			functionDialog->exec();
	}
	
	void AssignmentFunctionsTool::removeFunctions()
	{
		if (functionsListWidget.currentItem())
		{
			QRegExp regexp(".+\\.(.+) = (.+)");
			regexp.indexIn( functionsListWidget.currentItem()->text() );
			if (regexp.numCaptures() < 2) return;
			
			int n = functionsListWidget.currentRow();
			int j = 0;
			
			for (int i=0; i < itemHandles.size(); ++i)
			{
				if (itemHandles[i] && itemHandles[i]->data && itemHandles[i]->hasTextData(tr("Assignments")))
				{
					DataTable<QString> dat(itemHandles[i]->data->textData[tr("Assignments")]);
					if ( (j + dat.rows()) > n )
					{
						int k = n - j;
						if (k > -1)
						{
							dat.removeRow(k);
							GraphicsScene * scene = currentScene();
		
							if (scene)
							{
								scene->changeData(itemHandles[i],tr("Assignments"),&dat);
							}
							else
							{
								itemHandles[i]->data->textData[tr("Assignments")].removeRow(k);
							}
							return;
						}
					}
				}
			}
		}
	}
	
		
	/******************
	C API
	******************/
	AssignmentFunctionsTool_FToS AssignmentFunctionsTool::fToS;

	void AssignmentFunctionsTool::connectTCFunctions()
	{
		connect(&fToS,SIGNAL(getForcingFunctionNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&)),this,SLOT(getForcingFunctionNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&)));
		connect(&fToS,SIGNAL(getForcingFunctionAssignments(QSemaphore*,QStringList*,const QList<ItemHandle*>&)),this,SLOT(getForcingFunctionAssignments(QSemaphore*,QStringList*,const QList<ItemHandle*>&)));
		connect(&fToS,SIGNAL(addForcingFunction(QSemaphore*,ItemHandle*,const QString&, const QString&)),this,SLOT(addForcingFunction(QSemaphore*,ItemHandle*,const QString&, const QString&)));
	}

	typedef void (*tc_AssignmentFunctionsTool_api)(
		 char** (*getForcingFunctionNames)(Array),
		 char** (*getForcingFunctionAssignments)(Array),
		 void (*addForcingFunction)(OBJ,const char*, const char*)
	);
	
	void AssignmentFunctionsTool::setupFunctionPointers( QLibrary * library )
	{
		tc_AssignmentFunctionsTool_api f = (tc_AssignmentFunctionsTool_api)library->resolve("tc_AssignmentFunctionsTool_api");
		if (f)
		{
			f(
				&(_getForcingFunctionNames),
				&(_getForcingFunctionAssignments),
				&(_addForcingFunction)
			);
		}
	}
	
	void AssignmentFunctionsTool::getForcingFunctionNames(QSemaphore* sem,QStringList* list,const QList<ItemHandle*>& items)
	{
		if (list && !items.isEmpty())
		{
			QList<ItemHandle*> visited;
			QRegExp regex(tr("\\.(?!\\d)"));
			for (int i=0; i < items.size(); ++i)
			{
				if (items[i] && !visited.contains(items[i]) && items[i]->data && items[i]->hasTextData(tr("Assignments")))
				{
					QString s; 
					QStringList lst = items[i]->data->textData[tr("Assignments")].getRowNames();
					
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
	
	void AssignmentFunctionsTool::getForcingFunctionAssignments(QSemaphore* sem,QStringList* list,const QList<ItemHandle*>& items)
	{
		if (list && !items.isEmpty())
		{
			QList<ItemHandle*> visited;
			QRegExp regex(tr("\\.(?!\\d)"));
			for (int i=0; i < items.size(); ++i)
			{
				if (items[i] && !visited.contains(items[i]) && items[i]->data && items[i]->hasTextData(tr("Assignments"))
					&& items[i]->data->textData[tr("Assignments")].cols() > 0)
				{
					DataTable<QString>& dat = items[i]->data->textData[tr("Assignments")];
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
	
	void AssignmentFunctionsTool::addForcingFunction(QSemaphore* sem,ItemHandle* item,const QString& trigger, const QString& event)
	{
		if (item && item->data && !trigger.isEmpty() && !event.isEmpty())
		{
			if (!item->hasTextData(tr("Assignments")))
				item->data->textData[tr("Assignments")] = DataTable<QString>();
			
			DataTable<QString> dat = item->data->textData[tr("Assignments")];
			dat.value(trigger,0) = event;
			if (currentScene())
				currentScene()->changeData(item,tr("Assignments"),&dat);
			else
				item->data->textData[tr("Assignments")] = dat;
		}
		if (sem)
			sem->release();
	}
	
	char** AssignmentFunctionsTool::_getForcingFunctionNames(Array a0)
	{
		return fToS.getForcingFunctionNames(a0);
	}
	
	char** AssignmentFunctionsTool_FToS::getForcingFunctionNames(Array a0)
	{
		QSemaphore * s = new QSemaphore(1);
		QStringList p;
		s->acquire();
		QList<ItemHandle*> * list = ConvertValue(a0);
		emit getForcingFunctionNames(s,&p,*list);
		s->acquire();
		s->release();
		delete s;
		delete list;
		return (char**)ConvertValue(p);
	}
	
	char** AssignmentFunctionsTool::_getForcingFunctionAssignments(Array a0)
	{
		return fToS.getForcingFunctionAssignments(a0);
	}
	
	char** AssignmentFunctionsTool_FToS::getForcingFunctionAssignments(Array a0)
	{
		QSemaphore * s = new QSemaphore(1);
		QStringList p;
		s->acquire();
		QList<ItemHandle*> * list = ConvertValue(a0);
		emit getForcingFunctionAssignments(s,&p,*list);
		s->acquire();
		s->release();
		delete s;
		delete list;
		return (char**)ConvertValue(p);
	}
		
	void AssignmentFunctionsTool::_addForcingFunction(OBJ o, const char* a, const char* b)
	{
		return fToS.addForcingFunction(o,a,b);
	}
	
	void AssignmentFunctionsTool_FToS::addForcingFunction(OBJ o, const char* a, const char* b)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit addForcingFunction(s,ConvertValue(o),ConvertValue(a),ConvertValue(b));
		s->acquire();
		s->release();
		delete s;
	}
	
}


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::AssignmentFunctionsTool * AssignmentFunctionsTool = new Tinkercell::AssignmentFunctionsTool;
	main->addTool(AssignmentFunctionsTool);

}

