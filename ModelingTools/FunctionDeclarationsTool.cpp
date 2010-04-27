/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This class adds the "Functions" and "Assignments" data to each item in Tinkercell.
Functions are function declarations such as f(x) = x^2
Assignments are parameters that are defined as a function, eg. k1 = sin(time) + 1

****************************************************************************/

#include <QRegExp>
#include <QSettings>
#include <QMessageBox>
#include "NetworkWindow.h"
#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "ConsoleWindow.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ModelSummaryTool.h"
#include "FunctionDeclarationsTool.h"
#include "EquationParser.h"
#include "ModuleTool.h"
#include "muParserDef.h"
#include "muParser.h"
#include "muParserInt.h"

namespace Tinkercell
{
	void AssignmentFunctionsTool::select(int)
	{
		NetworkWindow * net = currentWindow();
		if (!net) return;

		if (dockWidget && dockWidget->widget() != this)
			dockWidget->setWidget(this);

		itemHandles = net->allHandles();

		openedByUser = true;
		updateTable();
		if (dockWidget != 0)
		{
			if (dockWidget->isVisible())
				openedByUser = false;
			else
				dockWidget->show();
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

	void AssignmentFunctionsTool::deselect(int)
	{
		if (openedByUser && (!dockWidget || dockWidget->isFloating()))
		{
			openedByUser = false;

			if (dockWidget != 0)
				dockWidget->hide();
			else
				hide();
		}
	}

	bool AssignmentFunctionsTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(itemsInserted(NetworkWindow*,const QList<ItemHandle*>&)),
				this, SLOT(itemsInserted(NetworkWindow*,const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				this,SLOT(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			connect(mainWindow,SIGNAL(historyChanged(int)),this,SLOT(historyUpdate(int)));

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			setWindowTitle(name);
			setWindowIcon(QIcon(":/images/function.png"));
			dockWidget = mainWindow->addToolWindow(this,MainWindow::DockWidget,Qt::BottomDockWidgetArea,Qt::NoDockWidgetArea);

			if (dockWidget)
			{
				dockWidget->move(mainWindow->geometry().bottomRight() - QPoint(sizeHint().width()*2,sizeHint().height()*3));

				dockWidget->setWindowFlags(Qt::Tool);
				dockWidget->setAttribute(Qt::WA_ContentsPropagated);
				dockWidget->setPalette(QPalette(QColor(255,255,255,255)));
				dockWidget->setAutoFillBackground(true);
				//dockWidget->setWindowOpacity(0.8);

				dockWidget->hide();
				dockWidget->setFloating(true);
			}

			toolLoaded(0);
		}

		return (mainWindow != 0);
	}

	void AssignmentFunctionsTool::toolLoaded(Tool*)
	{
		static bool connected = false;
		if (connected) return;

		if (!connected && mainWindow && mainWindow->tool(tr("Model Summary")))
		{
			QWidget * widget = mainWindow->tool(tr("Model Summary"));
			ModelSummaryTool * modelSummary = static_cast<ModelSummaryTool*>(widget);
			connect(modelSummary,SIGNAL(aboutToDisplayModel(const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)),
				this,SLOT(aboutToDisplayModel(const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)));
			connect(modelSummary,SIGNAL(displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)),
				this,SLOT(displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)));
			connected = true;
		}
	}

	void AssignmentFunctionsTool::aboutToDisplayModel(const QList<ItemHandle*>& items, QHash<QString,qreal>& , QHash<QString,QString>& equations)
	{
		itemHandles = items;
		updateTable();
		for (int i=0; i < updatedFunctions.size() && i < updatedFunctionNames.size(); ++i)
			equations[ updatedFunctionNames[i] ] = updatedFunctions[i];
	}

	void AssignmentFunctionsTool::displayModel(QTabWidget& widgets, const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equationsList)
	{
		//if (functionsListWidget.count() > 0)
		widgets.addTab(this,tr("Function declarations"));
		/*else
		if (dockWidget && dockWidget->widget() != this)
		dockWidget->setWidget(this);*/
	}

	void AssignmentFunctionsTool::historyUpdate(int i)
	{
		if (parentWidget() && parentWidget()->isVisible())
			updateTable();
	}

	void AssignmentFunctionsTool::itemsInserted(NetworkWindow*, const QList<ItemHandle*>& handles)
	{
		for (int i=0; i < handles.size(); ++i)
		{
			if (handles[i] && handles[i]->family() && !handles[i]->tools.contains(this))
				handles[i]->tools += this;

			if (handles[i] && handles[i]->family() && handles[i]->data && (
				!(handles[i]->hasTextData(tr("Functions"))) ||
				!(handles[i]->hasTextData(tr("Assignments")))
				))
			{
				insertDataMatrix(handles[i]);
			}
		}
	}

	void AssignmentFunctionsTool::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& list, QPointF , Qt::KeyboardModifiers )
	{
		if (scene && (parentWidget() && parentWidget() != mainWindow && parentWidget()->isVisible()))
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

	static double d = 1.0;
	static double* AddVariable(const char*, void*)
	{
		return &d;
	}

	void AssignmentFunctionsTool::setValue(int row,int col)
	{
		NetworkWindow * win = currentWindow();
		if (!win) return;

		if (col > 0 || !tableWidget.item(row,col)) return;

		QString text = tableWidget.item(row,col)->text().replace(tr(" "),tr(""));

		ItemHandle * handle = 0;

		if (row < tableItems.size())
			handle = tableItems[row];
		else
			if (win->selectedHandles().size() > 0)
				handle = win->selectedHandles().last();

		if (!handle || !handle->data) return;

		if (handle->hasTextData(tr("Assignment")) || handle->hasTextData(tr("Functions")))
		{
			if (text.isEmpty())   //delete function
			{
				if (row >= updatedFunctionNames.size()) return;

				QString f = updatedFunctionNames[row];

				if (handle->hasTextData(tr("Assignments")) && handle->data->textData[tr("Assignments")].getRowNames().contains(f))
				{
					DataTable<QString> newData(handle->data->textData[tr("Assignments")]);
					newData.removeRow(f);

					win->changeData(handle->fullName() + tr(".") + f + tr(" removed"), handle,tr("Assignments"),&newData);

				}
				if (handle->hasTextData(tr("Functions")) && handle->data->textData[tr("Functions")].getRowNames().contains(f))
				{
					DataTable<QString> newData(handle->data->textData[tr("Functions")]);
					newData.removeRow(f);

					win->changeData(handle->fullName() + tr(".") + f + tr(" removed"), handle,tr("Functions"),&newData);
				}
			}

			QRegExp regex1(tr("^([A-Za-z0-9_\\.]+)\\s*=\\s*(.+)")),
				regex2(tr("^([A-Za-z0-9_\\.]+)\\s*\\(([A-Za-z0-9_,]+)\\)\\s*=\\s*(.+)"));

			if (regex1.indexIn(text) > -1 && regex1.numCaptures() > 1)
			{
				if (!handle->hasTextData(tr("Assignments")))
					insertDataMatrix(handle);

				QString var = regex1.cap(1),
						func = regex1.cap(2);

				if (!EquationParser::validate(currentWindow(), handle, func, QStringList() << "time"))
					return;

				if (handle->name == var) var = handle->fullName();

				int k = 0;
				while (
					(win->symbolsTable.dataRowsAndCols.contains(handle->fullName() + tr(".") + var)) &&
					!(win->symbolsTable.dataRowsAndCols[handle->fullName() + tr(".") + var].second == tr("Assignments"))
					)
					var = regex1.cap(1) + QString::number(++k);

				if (var.startsWith(handle->fullName() + tr(".")))
					var.remove(handle->fullName() + tr("."));

				DataTable<QString> newData(handle->data->textData[tr("Assignments")]);

				if (!newData.rowNames().contains(var))
				{
					newData.insertRow(newData.rows(),var);
				}

				newData.value(var,0) = func;

				win->changeData(handle->fullName() + tr(".") + var + tr(" = ") + func, handle,tr("Assignments"),&newData);

			}
			else
				if (regex2.indexIn(text) > -1 && regex2.numCaptures() > 2)
				{

					if (!handle->hasTextData(tr("Functions")))
						insertDataMatrix(handle);

					QString var = regex2.cap(1),
						s = regex2.cap(3);

					if (var.startsWith(handle->fullName() + tr(".")))
						var.remove(handle->fullName() + tr("."));

					if (var.contains(tr(".")))
						return;

					//parse function
					mu::Parser parser;
					parser.SetExpr(s.toAscii().data());
					QString n;
					QStringList args;

					try
					{
						parser.SetVarFactory(AddVariable, 0);
						parser.Eval();
						mu::varmap_type variables = parser.GetVar();
						mu::varmap_type::const_iterator item = variables.begin();
						for (; item!=variables.end(); ++item)
							args << tr(item->first.data());
					}
					catch(mu::Parser::exception_type &)
					{
						if (console())
                            console()->error("cannot parse " + s);
						return;
					}

					DataTable<QString> newData(handle->data->textData[tr("Functions")]);

					if (!newData.rowNames().contains(var))
					{
						newData.insertRow(newData.rows(),var);
					}

					newData.value(var,0) = args.join(tr(","));
					newData.value(var,1) = s;

					win->changeData(handle->fullName() + tr(".") + var + tr("(") + newData.value(var,0) + tr("(") + tr(" = ") + s,handle,tr("Functions"),&newData);

				}
				updateTable();
		}

	}

	AssignmentFunctionsTool::AssignmentFunctionsTool() : Tool(tr("Functions and Assignments"),tr("Modeling"))
	{
		tableWidget.setEditTriggers ( QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed );
		connect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));

		QFont font = this->font();
		font.setPointSize(12);
		tableWidget.setFont(font);

		QHBoxLayout * actionsLayout = new QHBoxLayout;

		QToolButton * addAttribAction = new QToolButton(this);
		addAttribAction->setIcon(QIcon(":/images/plus.png"));

		QToolButton * removeAttribAction = new QToolButton(this);
		removeAttribAction->setIcon(QIcon(":/images/minus.png"));

		QString message;
		addAttribAction->setToolTip(tr("Add function"));
		removeAttribAction->setToolTip(tr("Remove function"));
		//tableWidget.setItemDelegate(&delegate);
		//tableWidget.setFixedWidth(160);

		QGroupBox * groupBox = new QGroupBox(name,this);

		groupBox->setMinimumWidth(100);
		message = tr("This table shows the functions and assignment rules belonging with the selected object(s). Assignment rules are definitions of variables, e.g. x = A + B, where x is an assignment rule that is always the sum of A and B. Functions contain arguments, e.g. f(a,b) = sin(a) + cos(b). Functions must be defined in terms of the arguments, i.e. f(x) = A + sin(x) is incorrect because A is not in the argument.");

		connect(addAttribAction,SIGNAL(pressed()),this,SLOT(addAttribute()));
		connect(removeAttribAction,SIGNAL(pressed()),this,SLOT(removeSelectedAttributes()));

		QToolButton * calc = new QToolButton(this);
		calc->setIcon(QIcon(":/images/calc.png"));
		calc->setToolTip(tr("Get function values"));

		QToolButton * question = new QToolButton(this);
		question->setIcon(QIcon(":/images/question.png"));

		QMessageBox * messageBox = new QMessageBox(QMessageBox::Information,tr("About Functions Table"),message,QMessageBox::StandardButtons(QMessageBox::Close), const_cast<QWidget*>((QWidget*)this), Qt::WindowFlags (Qt::Dialog));
		connect(question,SIGNAL(pressed()),messageBox,SLOT(exec()));
		connect(calc,SIGNAL(pressed()),this,SLOT(eval()));

		actionsLayout->addWidget(addAttribAction);
		actionsLayout->addWidget(removeAttribAction);
		actionsLayout->addWidget(calc);
		actionsLayout->addStretch(1);
		actionsLayout->addWidget(question);

		QVBoxLayout * boxLayout = new QVBoxLayout;
		boxLayout->addWidget(&tableWidget,1);

		boxLayout->addLayout(actionsLayout);
		groupBox->setLayout(boxLayout);

		dockWidget = 0;

		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(groupBox);
		setLayout(layout);

		connectTCFunctions();
		tableWidget.setColumnCount(1);

		QString appDir = QCoreApplication::applicationDirPath();

		NodeGraphicsReader reader;
		reader.readXml(&item,appDir + tr("/OtherItems/func.xml"));

		item.normalize();
		item.scale(35.0/item.sceneBoundingRect().width(),35.0/item.sceneBoundingRect().height());
		graphicsItems += new GraphicsItem(this);
		graphicsItems[0]->addToGroup(&item);
		graphicsItems[0]->setToolTip(name);

		/*QToolButton * toolButton = new QToolButton(this);
		toolButton->setIcon(QIcon(appDir + tr("/OtherItems/func.xml")));
		toolButton->setToolTip(name);
		this->buttons.addButton(toolButton);*/

		openedByUser = false;
	}

	QSize AssignmentFunctionsTool::sizeHint() const
	{
		return QSize(300, 200);
	}

	void AssignmentFunctionsTool::insertDataMatrix(ItemHandle * handle)
	{
		if (handle == 0 || handle->family() == 0 || !handle->data) return;

		QStringList colNames;
		colNames << "value";

		if (!handle->hasTextData(tr("Assignments")))
		{
			DataTable<QString> table;
			table.resize(0,1);

			table.colName(0) = QString("rule");
			table.description() = tr("Assignments: A set of forcing functions. Row names correspond to the function name, and first column will contain the function string.");

			handle->data->textData.insert(tr("Assignments"),table);
		}

		if (!handle->hasTextData(tr("Functions")))
		{
			DataTable<QString> table;
			table.resize(0,2);

			table.colName(0) = QString("args");
			table.colName(1) = QString("defn");
			table.description() = tr("Functions: A set of function definitions. First column contains the list of arguments, and second column contains the function strings. Row names correspond to the function names.");

			handle->data->textData.insert(tr("Functions"),table);
		}
	}

	void AssignmentFunctionsTool::updateTable()
	{
		updatedFunctions.clear();
		updatedFunctionNames.clear();

		tableItems.clear();
		tableWidget.clear();

		if (itemHandles.size() < 1)
		{
			tableWidget.clearContents();
			tableWidget.setRowCount(0);
			tableWidget.setColumnCount(1);
			tableWidget.setHorizontalHeaderLabels(QStringList() << "function");
			return;
		}

		QStringList functions;

		DataTable<QString> * sDataTable = 0;

		for (int i=0; i < itemHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (itemHandles[i] != 0 && itemHandles[i]->hasTextData(tr("Assignments")))
			{
				sDataTable = &(itemHandles[i]->data->textData[tr("Assignments")]);
				if (sDataTable->cols() < 1) continue;

				for (int j=0; j < sDataTable->rows(); ++j)
				{
					tableItems << (itemHandles[i]);
					updatedFunctions << sDataTable->value(j,0);
					updatedFunctionNames << sDataTable->rowName(j);

					if (sDataTable->rowName(j) == itemHandles[i]->fullName())
						functions += sDataTable->rowName(j) + tr(" = ") + (sDataTable->value(j,0));
					else
						functions += itemHandles[i]->fullName() + tr(".") + sDataTable->rowName(j) + tr(" = ") + (sDataTable->value(j,0));
				}
			}
		}

		for (int i=0; i < itemHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (itemHandles[i] != 0 && itemHandles[i]->hasTextData(tr("Functions")))
			{
				sDataTable = &(itemHandles[i]->data->textData[tr("Functions")]);
				if (sDataTable->cols() < 2) continue;

				for (int j=0; j < sDataTable->rows(); ++j)
				{
					tableItems << (itemHandles[i]);
					updatedFunctions << sDataTable->value(j,0);
					updatedFunctionNames << sDataTable->rowName(j);

					functions += itemHandles[i]->fullName() + tr(".") + sDataTable->rowName(j) +
						tr("(") + sDataTable->value(j,0) + tr(")") + tr(" = ") + sDataTable->value(j,1);
				}
			}
		}

		tableWidget.setRowCount(functions.size());
		tableWidget.setColumnCount(1);
		tableWidget.setHorizontalHeaderLabels(QStringList() << "function");
		//tableWidget.setHorizontalHeader(0);
		//tableWidget.setVerticalHeader(0);

		disconnect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));

		for (int i=0; i < tableWidget.rowCount(); ++i)
		{
			tableWidget.setItem(i,0,new QTableWidgetItem(functions[i]));
		}

		tableWidget.resizeColumnToContents(0);

		connect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));
	}

	void AssignmentFunctionsTool::addAttribute()
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		QString name;

		int i=0;
		ItemHandle * lastItem = 0;

		if (tableItems.size() < 1)
		{
			if (scene->selected().isEmpty()) return;
			lastItem = getHandle(scene->selected()[0]);
		}
		else
		{
			lastItem = tableItems[i];
			while (!lastItem && (i+1) < tableItems.size()) lastItem = tableItems[++i];
		}

		if (lastItem == 0 || lastItem->data == 0) return;

		disconnect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));

		int n = tableWidget.rowCount();
		tableWidget.insertRow(n);
		tableWidget.setItem(n,0,new QTableWidgetItem(tr("enter function, e.g. f(x) = x^2 + 2*x or g = 1+sin(time)")));
		tableItems << lastItem;

		connect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));
	}

	void AssignmentFunctionsTool::removeSelectedAttributes()
	{
		QList<QTableWidgetItem*> selectedItems = tableWidget.selectedItems();
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		QStringList toolNames;
		QList<ItemHandle*> handles;
		QList<DataTable<qreal>*> nDats;
		QList<DataTable<QString>*> sDats;

		for (int i=0; i < selectedItems.size(); ++i)
		{
			int row = selectedItems[i]->row();

			if (row >= tableItems.size()) continue;

			ItemHandle * handle = tableItems[row];

			if (!handle || !handle->data) continue;

			if (row >= updatedFunctionNames.size()) continue;
			QString f = updatedFunctionNames[row];
			if (handle->hasTextData(tr("Assignments")) && handle->data->textData[tr("Assignments")].getRowNames().contains(f))
			{
				DataTable<QString> * sDat = new DataTable<QString>(handle->data->textData[tr("Assignments")]);
				sDat->removeRow(f);
				sDats << sDat;
				handles << handle;
				toolNames << tr("Assignments");
			}
			if (handle->hasTextData(tr("Functions")) && handle->data->textData[tr("Functions")].getRowNames().contains(f))
			{
				DataTable<QString> * sDat = new DataTable<QString>(handle->data->textData[tr("Functions")]);
				sDat->removeRow(f);
				sDats << sDat;
				handles << handle;
				toolNames << tr("Functions");
			}
		}

		if (sDats.size() > 0)
			scene->changeData(tr("selected functions removed"),handles,toolNames,sDats);

		for (int i=0; i < sDats.size(); ++i)
			delete sDats[i];
	}

	void AssignmentFunctionsTool::keyPressEvent ( QKeyEvent * event )
	{
		if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
		{
			removeSelectedAttributes();
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
		ArrayOfStrings (*getForcingFunctionNames)(ArrayOfItems),
		ArrayOfStrings (*getForcingFunctionAssignments)(ArrayOfItems),
		void (*addForcingFunction)(void*,const char*, const char*)
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

	void AssignmentFunctionsTool::getForcingFunctionNames(QSemaphore* sem,QStringList* list,const QList<ItemHandle*>& handles)
	{
		if (list && !handles.isEmpty())
		{
			QList<ItemHandle*> items = handles;

			if (currentWindow() && currentWindow()->modelItem())
				if (!items.contains(currentWindow()->modelItem()))
					items << currentWindow()->modelItem();

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

						if (items[i]->name.isEmpty())
							(*list) << s;
						else
							(*list) << items[i]->fullName(tr("_")) + tr("_") + s;
					}
				}
			}
		}
		if (sem)
			sem->release();
	}

	void AssignmentFunctionsTool::getForcingFunctionAssignments(QSemaphore* sem,QStringList* list,const QList<ItemHandle*>& handles)
	{
		if (list && !handles.isEmpty())
		{
		    QList<ItemHandle*> from,to;
            ModuleTool::connectedItems(handles, from,to);

			QList<ItemHandle*> items = handles;

			if (currentWindow() && currentWindow()->modelItem())
				if (!items.contains(currentWindow()->modelItem()))
					items << currentWindow()->modelItem();

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
						for (int k=0; i < from.size() && k < to.size(); ++k)
                            s.replace(from[k]->fullName(),to[k]->fullName());
						(*list) << s;
					}
				}
			}
		}
		if (sem)
			sem->release();
	}

	void AssignmentFunctionsTool::addForcingFunction(QSemaphore* sem,ItemHandle* item,const QString& var, const QString& func)
	{
		NetworkWindow * win = currentWindow();
		
		if (!win) return;
		
		if (!item)
			item = win->modelItem();

		if (item && item->data && !func.isEmpty() && !var.isEmpty())
		{
			if (!item->hasTextData(tr("Assignments")))
				item->data->textData[tr("Assignments")] = DataTable<QString>();

			DataTable<QString> dat = item->data->textData[tr("Assignments")];

			QString f = func;
			QRegExp regex(QString("([A-Za-z0-9])_([A-Za-z])"));
			f.replace(regex,QString("\\1.\\2"));
			
			int k = 0;
			
			QString s;
			QString s0 = var;
			
			if (!item->name.isEmpty())
				s0 = item->fullName() + tr(".") + s0;
			
			s = s0;

			while (
				(win->symbolsTable.dataRowsAndCols.contains(s)) &&
				!(win->symbolsTable.dataRowsAndCols[s].second == tr("Assignments"))
				)
				s = s0 + QString::number(++k);
			
			if (!dat.getRowNames().contains(s) || f != dat.value(s,0))
			{
				dat.value(s,0) = func;
				if (currentWindow())
					if (item->name.isEmpty())
						currentWindow()->changeData(s + tr(" = ") + f,item,tr("Assignments"),&dat);
					else
						currentWindow()->changeData(item->fullName() + tr(".") + s + tr(" = ") + f,item,tr("Assignments"),&dat);
				else
					item->data->textData[tr("Assignments")] = dat;
			}
		}
		if (sem)
			sem->release();
	}

	ArrayOfStrings AssignmentFunctionsTool::_getForcingFunctionNames(ArrayOfItems a0)
	{
		return fToS.getForcingFunctionNames(a0);
	}

	ArrayOfStrings AssignmentFunctionsTool_FToS::getForcingFunctionNames(ArrayOfItems a0)
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
		return (ArrayOfStrings)ConvertValue(p);
	}

	ArrayOfStrings AssignmentFunctionsTool::_getForcingFunctionAssignments(ArrayOfItems a0)
	{
		return fToS.getForcingFunctionAssignments(a0);
	}

	ArrayOfStrings AssignmentFunctionsTool_FToS::getForcingFunctionAssignments(ArrayOfItems a0)
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
		return (ArrayOfStrings)ConvertValue(p);
	}

	void AssignmentFunctionsTool::_addForcingFunction(void* o, const char* a, const char* b)
	{
		return fToS.addForcingFunction(o,a,b);
	}

	void AssignmentFunctionsTool_FToS::addForcingFunction(void* o, const char* a, const char* b)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit addForcingFunction(s,ConvertValue(o),ConvertValue(a),ConvertValue(b));
		s->acquire();
		s->release();
		delete s;
	}

	void AssignmentFunctionsTool::eval()
	{
		bool b;
		QStringList values;
		for (int i=0; i < tableItems.size() && i < updatedFunctions.size() && i < updatedFunctionNames.size(); ++i)
			if (tableItems[i])
			{
				QString s = updatedFunctions[i];
				double d = EquationParser::eval(currentWindow(), s, &b);
				if (b)
				{
					if (tableItems[i]->fullName() != updatedFunctionNames[i])
						values += tableItems[i]->fullName() + tr(".") + updatedFunctionNames[i] + tr(" = ") + QString::number(d);
					else
						values += tableItems[i]->fullName() + tr(" = ") + QString::number(d);
				}
			}
		if (values.size() > 0)
			if (console())
                console()->message(values.join(tr("\n")));
	}
}
