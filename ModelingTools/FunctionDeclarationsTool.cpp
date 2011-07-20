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
#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "ConsoleWindow.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "LabelingTool.h"
#include "ModelSummaryTool.h"
#include "FunctionDeclarationsTool.h"
#include "EquationParser.h"
#include "BasicInformationTool.h"
#include "StoichiometryTool.h"
#include "ModuleTools/ModuleTool.h"
#include "muParserDef.h"
#include "muParser.h"
#include "muParserInt.h"

namespace Tinkercell
{
	#define WINDOW_WIDTH 200
	QString AssignmentFunctionsTool::Self("self");
	
	void AssignmentFunctionsTool::select(int)
	{
		NetworkHandle * net = currentNetwork();
		if (!net) return;

		itemHandles = net->handles();

		openedByUser = true;
		updateTable();
		if (isVisible())
			openedByUser = false;
		else
			show();
		raise();
		this->setFocus();
	}

	void AssignmentFunctionsTool::deselect(int)
	{
		if (openedByUser)
		{
			openedByUser = false;

			hide();
		}
	}

	bool AssignmentFunctionsTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow)
		{
			ModuleTool::textTablesToBeReplaced << "assignments";
			
			connect(mainWindow,SIGNAL(itemsInserted(NetworkHandle*,const QList<ItemHandle*>&)),
				this, SLOT(itemsInserted(NetworkHandle*,const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				this,SLOT(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
			
			connect(mainWindow,SIGNAL(mouseMoved(GraphicsScene* , QGraphicsItem*, QPointF , Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)),
                    this,SLOT(mouseMoved(GraphicsScene* , QGraphicsItem*, QPointF , Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)));

			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			connect(mainWindow,SIGNAL(historyChanged(int)),this,SLOT(historyUpdate(int)));

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			setWindowTitle(name);
			setWindowFlags(Qt::Dialog);
			setWindowIcon(QIcon(":/images/function.png"));
			mainWindow->addToViewMenu(this);
			move(mainWindow->geometry().bottomRight() - QPoint(sizeHint().width()*2,sizeHint().height()*3));
			setAttribute(Qt::WA_ContentsPropagated);
			setPalette(QPalette(QColor(255,255,255,255)));
			setAutoFillBackground(true);
			
			//module snapshot window
			snapshotToolTip = new QDialog(mainWindow);
			snapshotToolTip->setPalette(QPalette(Qt::black));
			snapshotToolTip->setFixedSize(WINDOW_WIDTH,WINDOW_WIDTH);
			QRect rect = mainWindow->geometry();
			snapshotToolTip->setGeometry (rect.right() - WINDOW_WIDTH, rect.bottom() - WINDOW_WIDTH, WINDOW_WIDTH, WINDOW_WIDTH );
			QHBoxLayout * layout = new QHBoxLayout;
			layout->setContentsMargins(1,1,1,1);
			snapshotIcon = new QToolButton;
			layout->addWidget(snapshotIcon);
			snapshotToolTip->setLayout(layout);

			hide();
			
			toolLoaded(0);
		}

		return (mainWindow != 0);
	}
	
	void AssignmentFunctionsTool::mouseMoved(GraphicsScene* scene, QGraphicsItem * hoverOverItem, QPointF , Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>& )
	{
		if (mainWindow && scene && scene->useDefaultBehavior() && hoverOverItem && !TextGraphicsItem::cast(hoverOverItem) && snapshotToolTip)
		{
			ItemHandle * h = getHandle(hoverOverItem);
			
			if (h && h->hasTextData(tr("Assignments")) && graphWidget)
			{
				TextDataTable & assignments = h->textDataTable(tr("Assignments"));
				if (assignments.columns() > 0 && (assignments.hasRow(Self) || assignments.hasRow(h->fullName())))
				{
					QString s;
					if (assignments.hasRow(Self))
						s = assignments.value(Self,0);
					else
						s = assignments.value(h->fullName(),0);
					
					if (s.isEmpty())
						if (assignments.hasRow(Self))
							s = assignments.value(Self, assignments.columns()-1);
						else
							s = assignments.value(h->fullName(),assignments.columns()-1);
					
					if (!s.isEmpty() && s.size() > 2 && !functionSnapshots.contains(s))
					{
						QPixmap printer(WINDOW_WIDTH, WINDOW_WIDTH);
						printer.fill();
						graphWidget->setFormula(s,scene->network);
						graphWidget->setYLabel(h->name);
						graphWidget->setTitle(tr("Function"));
						graphWidget->print(printer);
						functionSnapshots[s] = printer;
					}

					if (!s.isEmpty() && s.size() > 2 && functionSnapshots.contains(s) && !snapshotToolTip->isVisible())
					{
						QRect rect = scene->mapToWidget( hoverOverItem->sceneBoundingRect() );
						snapshotToolTip->setGeometry (rect.right() + 100, rect.top() - 100, WINDOW_WIDTH, WINDOW_WIDTH );
						snapshotIcon->setIcon(QIcon(functionSnapshots[s]));
						snapshotIcon->setIconSize(QSize(WINDOW_WIDTH,WINDOW_WIDTH));
						snapshotToolTip->show();
						snapshotToolTip->raise();
					}
					return;
				}
			}
		}
		
		if (snapshotToolTip && snapshotToolTip->isVisible())
		{
			snapshotToolTip->hide();
			emit clearLabels(0);
		}
	}

	void AssignmentFunctionsTool::toolLoaded(Tool*)
	{
		static bool connected1 = false;
		static bool connected2 = false;
		if (connected1 && connected2) return;

		if (!connected1 && mainWindow && mainWindow->tool(tr("Model Summary")))
		{
			QWidget * widget = mainWindow->tool(tr("Model Summary"));
			ModelSummaryTool * modelSummary = static_cast<ModelSummaryTool*>(widget);
			connect(modelSummary,SIGNAL(aboutToDisplayModel(const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)),
				this,SLOT(aboutToDisplayModel(const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)));
			connect(modelSummary,SIGNAL(displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)),
				this,SLOT(displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)));
			connected1 = true;
		}
		
		if (!connected2 && mainWindow && mainWindow->tool(tr("Labeling Tool")))
		{
			QWidget * widget = mainWindow->tool(tr("Labeling Tool"));
			LabelingTool * labelingTool = static_cast<LabelingTool*>(widget);
			connect(labelingTool,SIGNAL(highlightItem(ItemHandle*,QColor)),
				this,SLOT(highlightItem(ItemHandle*,QColor)));
			connect(labelingTool,SIGNAL(clearLabels(ItemHandle * h)),
				this,SLOT(clearLabels(ItemHandle * h)));
			connected2 = true;
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
		if (!updatedFunctions.isEmpty())
			widgets.insertTab(0,this,tr("Formulas"));
		else	
			widgets.addTab(this,tr("Formulas"));
	}

	void AssignmentFunctionsTool::historyUpdate(int i)
	{
		if (isVisible())
			updateTable();
	}

	void AssignmentFunctionsTool::itemsInserted(NetworkHandle*, const QList<ItemHandle*>& handles)
	{
		for (int i=0; i < handles.size(); ++i)
		{
			//if (handles[i] && handles[i]->isA("Node") && !handles[i]->tools.contains(this))
				//handles[i]->tools += this;

			if (handles[i] && handles[i]->family())
			{
				if (!(handles[i]->hasTextData(tr("Functions"))) ||
					!(handles[i]->hasTextData(tr("Assignments")))
					)
				{
					insertDataMatrix(handles[i]);
				}
				else
				{
					if (!StoichiometryTool::userModifiedRates.contains(handles[i]) && handles[i]->textDataTable(tr("Assignments")).hasRow(Self))
					{
						StoichiometryTool::userModifiedRates += handles[i];
					}
				}
			}
		}
	}

	void AssignmentFunctionsTool::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& list, QPointF , Qt::KeyboardModifiers )
	{
		if (scene && isVisible())
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
		NetworkHandle * win = currentNetwork();
		if (!win) return;

		
		GraphicsScene * scene = win->currentScene();
		if (!scene) return;

		if (col > 0 || !tableWidget.item(row,col)) return;

		QString text = tableWidget.item(row,col)->text().replace(tr(" "),tr(""));

		ItemHandle * handle = 0;

		if (row < tableItems.size())
			handle = tableItems[row];
		else
			if (scene->selected().size() > 0)
				handle = getHandle(scene->selected()).last();

		if (!handle) return;

		if (handle->hasTextData(tr("Assignment")) || handle->hasTextData(tr("Functions")))
		{
			if (text.isEmpty())   //delete function
			{
				if (row >= updatedFunctionNames.size()) return;

				QString f = updatedFunctionNames[row];

				if (handle->hasTextData(tr("Assignments")) && handle->textDataTable(tr("Assignments")).hasRow(f))
				{
					DataTable<QString> newData(handle->textDataTable(tr("Assignments")));
					newData.removeRow(f);

					win->changeData(handle->fullName() + tr(".") + f + tr(" removed"), handle,tr("Assignments"),&newData);
					BasicInformationTool::removeUnusedParametersInModel(win);
				}
				if (handle->hasTextData(tr("Functions")) && handle->textDataTable(tr("Functions")).hasRow(f))
				{
					DataTable<QString> newData(handle->textDataTable(tr("Functions")));
					newData.removeRow(f);

					win->changeData(handle->fullName() + tr(".") + f + tr(" removed"), handle,tr("Functions"),&newData);
					BasicInformationTool::removeUnusedParametersInModel(win);
				}
				
				StoichiometryTool::userModifiedRates.removeAll(handle);
			}

			QRegExp regex1(tr("^([A-Za-z0-9_\\.]+)\\s*=\\s*(.+)")),
				regex2(tr("^([A-Za-z0-9_\\.]+)\\s*\\(([A-Za-z0-9_,]+)\\)\\s*=\\s*(.+)"));

			if (regex1.indexIn(text) > -1 && regex1.numCaptures() > 1)
			{
				if (!handle->hasTextData(tr("Assignments")))
					insertDataMatrix(handle);

				QString var = regex1.cap(1),
						func = regex1.cap(2);

				if (!EquationParser::validate(currentNetwork(), handle, func, QStringList() << "time" << "Time" << "TIME"))
					return;

				if (handle->name == var || handle->fullName() == var) 
					var = Self;
				else
				{
					if (var.startsWith(handle->fullName() + tr(".")))
						var.remove(handle->fullName() + tr("."));
					var = handle->fullName() + tr(".") + var;
					var = win->makeUnique(var);
					var.remove(handle->fullName() + tr("."));
				}

				if (var == Self && !StoichiometryTool::userModifiedRates.contains(handle))
					StoichiometryTool::userModifiedRates << handle;

				DataTable<QString> newData(handle->textDataTable(tr("Assignments")));
				newData.value(var,0) = func;

				win->changeData(handle->fullName() + tr(".") + var + tr(" = ") + func, handle,tr("Assignments"),&newData);
				BasicInformationTool::removeUnusedParametersInModel(win);
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
					var = handle->fullName() + tr(".") + var;
					var = win->makeUnique(var);

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

					DataTable<QString> newData(handle->textDataTable(tr("Functions")));
					newData.value(var,0) = args.join(tr(","));
					newData.value(var,1) = s;

					win->changeData(handle->fullName() + tr(".") + var + tr("(") + newData.value(var,0) + tr("(") + tr(" = ") + s,handle,tr("Functions"),&newData);
					BasicInformationTool::removeUnusedParametersInModel(win);
				}
				else
				if (!text.isEmpty())
				{
					if (!handle->hasTextData(tr("Assignments")))
						insertDataMatrix(handle);

					QString var = Self,
								  func = text;

					if (!EquationParser::validate(currentNetwork(), handle, func, QStringList() << "time" << "Time" << "TIME"))
						return;
					
					if (var == Self && !StoichiometryTool::userModifiedRates.contains(handle))
						StoichiometryTool::userModifiedRates << handle;

					DataTable<QString> newData(handle->textDataTable(tr("Assignments")));
					newData.value(var,0) = func;

					win->changeData(handle->fullName() + tr(".") + var + tr(" = ") + func, handle,tr("Assignments"),&newData);
					BasicInformationTool::removeUnusedParametersInModel(win);
				}
				updateTable();
		}
	}

	AssignmentFunctionsTool::AssignmentFunctionsTool() : Tool(tr("Functions and Assignments"),tr("Modeling"))
	{
		AssignmentFunctionsTool::fToS = new AssignmentFunctionsTool_FToS;
		AssignmentFunctionsTool::fToS->setParent(this);
		
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

		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(groupBox);
		layout->addWidget(graphWidget = new EquationGraph);
		setLayout(layout);

		connectTCFunctions();
		tableWidget.setColumnCount(1);

		QString appDir = QCoreApplication::applicationDirPath();

		/*NodeGraphicsReader reader;
		reader.readXml(&item,tr(":/images/func.xml"));

		item.normalize();
		item.scale(35.0/item.sceneBoundingRect().width(),35.0/item.sceneBoundingRect().height());
		ToolGraphicsItem * toolGraphicsItem = new ToolGraphicsItem(this);
		addGraphicsItem(toolGraphicsItem);
		toolGraphicsItem->addToGroup(&item);
		toolGraphicsItem->setToolTip(name);

		QToolButton * toolButton = new QToolButton(this);
		toolButton->setIcon(QIcon(tr(":/images/func.xml")));
		toolButton->setToolTip(name);
		this->buttons.addButton(toolButton);*/

		addAction(QIcon(":/images/monitor.png"), "Forcing functions");	

		//module snapshot window
		snapshotToolTip = 0;
		snapshotIcon = 0;

		openedByUser = false;
	}

	QSize AssignmentFunctionsTool::sizeHint() const
	{
		return QSize(300, 200);
	}

	void AssignmentFunctionsTool::insertDataMatrix(ItemHandle * handle)
	{
		if (handle == 0 || handle->family() == 0 ) return;

		QStringList columnNames;

		columnNames << "value";

		if (!handle->hasTextData(tr("Assignments")))
		{
			DataTable<QString> table;
			table.resize(0,1);

			table.setColumnName(0,QString("rule"));
			table.description() = tr("Assignments: A set of forcing functions. Row names correspond to the function name, and first column will contain the function string.");

			handle->textDataTable(tr("Assignments")) = table;
		}

		if (!handle->hasTextData(tr("Functions")))
		{
			DataTable<QString> table;
			table.resize(0,2);

			table.setColumnName(0, QString("args"));
			table.setColumnName(1, QString("defn"));
			table.description() = tr("Functions: A set of function definitions. First column contains the list of arguments, and second column contains the function strings. Row names correspond to the function names.");

			handle->textDataTable(tr("Functions")) = table;
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
			graphWidget->hide();
			return;
		}

		QStringList functions;

		DataTable<QString> * sDataTable = 0;
		QString assignmentVar, assignmentFormula;

		for (int i=0; i < itemHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (itemHandles[i] != 0 && itemHandles[i]->hasTextData(tr("Assignments")))
			{
				sDataTable = &(itemHandles[i]->textDataTable(tr("Assignments")));
				if (sDataTable->columns() < 1) continue;

				for (int j=0; j < sDataTable->rows(); ++j)
				{
					tableItems << (itemHandles[i]);
					updatedFunctions << sDataTable->value(j,0);
					updatedFunctionNames << sDataTable->rowName(j);

					if (sDataTable->rowName(j) == itemHandles[i]->fullName() || 
						sDataTable->rowName(j).toLower() == Self ||
						sDataTable->rowName(j).isEmpty())
					{
						functions += itemHandles[i]->fullName() + tr(" = ") + (sDataTable->value(j,0));
						assignmentVar = itemHandles[i]->fullName();
						assignmentFormula = sDataTable->value(j,0);
					}
					else
						functions += itemHandles[i]->fullName() + tr(".") + sDataTable->rowName(j) + tr(" = ") + (sDataTable->value(j,0));
				}
			}
		}

		for (int i=0; i < itemHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (itemHandles[i] != 0 && itemHandles[i]->hasTextData(tr("Functions")))
			{
				sDataTable = &(itemHandles[i]->textDataTable(tr("Functions")));
				if (sDataTable->columns() < 2) continue;

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
		
		if (!assignmentVar.isEmpty() && !assignmentFormula.isEmpty())
		{
			graphWidget->show();
			graphWidget->setFormula(assignmentFormula,currentNetwork());
			graphWidget->setTitle(assignmentVar + tr(" formula"));
			graphWidget->setYLabel(assignmentVar);
		}
		else
		{
			graphWidget->hide();
		}

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

		if (lastItem == 0) return;

		disconnect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));

		int n = tableWidget.rowCount();
		tableWidget.insertRow(n);
		tableWidget.setItem(n,0,new QTableWidgetItem(tr("enter function, e.g. f(x) = x^2 + 2*x or g = 1+sin(time) or simply 1+sin(time)")));
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

			if (!handle) continue;

			if (row >= updatedFunctionNames.size()) continue;
			QString f = updatedFunctionNames[row];
			if (handle->hasTextData(tr("Assignments")) && handle->textDataTable(tr("Assignments")).hasRow(f))
			{
				DataTable<QString> * sDat = new DataTable<QString>(handle->textDataTable(tr("Assignments")));
				sDat->removeRow(f);
				sDats << sDat;
				handles << handle;
				toolNames << tr("Assignments");
			}
			if (handle->hasTextData(tr("Functions")) && handle->textDataTable(tr("Functions")).hasRow(f))
			{
				DataTable<QString> * sDat = new DataTable<QString>(handle->textDataTable(tr("Functions")));
				sDat->removeRow(f);
				sDats << sDat;
				handles << handle;
				toolNames << tr("Functions");
			}
		}

		if (sDats.size() > 0)
		{
			scene->network->changeData(tr("selected functions removed"),handles,toolNames,sDats);
			BasicInformationTool::removeUnusedParametersInModel(scene->network);
		}

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
	AssignmentFunctionsTool_FToS * AssignmentFunctionsTool::fToS;

	void AssignmentFunctionsTool::connectTCFunctions()
	{
		connect(fToS,SIGNAL(getForcingFunctionNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&)),this,SLOT(getForcingFunctionNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&)));
		connect(fToS,SIGNAL(getForcingFunctionAssignments(QSemaphore*,QStringList*,const QList<ItemHandle*>&)),this,SLOT(getForcingFunctionAssignments(QSemaphore*,QStringList*,const QList<ItemHandle*>&)));
		connect(fToS,SIGNAL(addForcingFunction(QSemaphore*,ItemHandle*,const QString&, const QString&)),this,SLOT(addForcingFunction(QSemaphore*,ItemHandle*,const QString&, const QString&)));
	}

	typedef void (*tc_AssignmentFunctionsTool_api)(
		tc_strings (*getForcingFunctionNames)(tc_items),
		tc_strings (*getForcingFunctionAssignments)(tc_items),
		void (*addForcingFunction)(long, const char*, const char*)
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

			if (currentNetwork() && currentNetwork()->globalHandle())
				if (!items.contains(currentNetwork()->globalHandle()))
					items << currentNetwork()->globalHandle();

			QList<ItemHandle*> visited;
			QRegExp regex(tr("\\.(?!\\d)"));
			for (int i=0; i < items.size(); ++i)
			{
				if (items[i] && !visited.contains(items[i])  &&  items[i]->hasTextData(tr("Assignments")))
				{
					QString s;
					QStringList lst = items[i]->textDataTable(tr("Assignments")).rowNames();


					for (int j=0; j < lst.size(); ++j)
					{
						s = lst[j];
						s.replace(regex,tr("_"));

						if (items[i]->name.isEmpty())
							(*list) << s;
						else
						if (s == Self)
							(*list) << items[i]->fullName(tr("_"));
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
			QList<ItemHandle*> items = handles;

			if (currentNetwork() && currentNetwork()->globalHandle())
				if (!items.contains(currentNetwork()->globalHandle()))
					items << currentNetwork()->globalHandle();

			QList<ItemHandle*> visited;
			QRegExp regex(tr("\\.(?!\\d)"));
			for (int i=0; i < items.size(); ++i)
			{
				if (items[i] && !visited.contains(items[i]) && items[i]->hasTextData(tr("Assignments"))
					&& items[i]->textDataTable(tr("Assignments")).columns() > 0)
				{
					DataTable<QString>& dat = items[i]->textDataTable(tr("Assignments"));
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

	void AssignmentFunctionsTool::addForcingFunction(QSemaphore* sem,ItemHandle* item,const QString& var, const QString& func)
	{
		NetworkHandle * win = currentNetwork();
		
		if (!win) return;
		
		if (!item)
			item = win->globalHandle();

		if (item && !func.isEmpty() && !var.isEmpty())
		{
			if (!item->hasTextData(tr("Assignments")))
				item->textDataTable(tr("Assignments")) = DataTable<QString>();

			DataTable<QString> dat = item->textDataTable(tr("Assignments"));

			QString f = func;
			QRegExp regex(QString("([A-Za-z0-9])_([A-Za-z])"));
			f.replace(regex,QString("\\1.\\2"));
			
			int k = 0;
			
			QString s;
			QString s0 = var;
			
			if (s0.startsWith(item->fullName() + tr("_")))
				s0.remove(item->fullName() + tr("_"));
			
			if (s0.startsWith(item->fullName() + tr(".")))
				s0.remove(item->fullName() + tr("."));
			
			if (s0 == item->fullName() || s0 == item->name || s0.toLower() == Self.toLower() || s0.isEmpty())
				s0 = Self;

			if (s0 == Self && !StoichiometryTool::userModifiedRates.contains(item))
				StoichiometryTool::userModifiedRates << item;
			
			if (dat.hasRow(s0))
			{
				s = s0;
			}
			else
			{
				if (!item->name.isEmpty())
					s0 = item->fullName() + tr(".") + s0;
			
				s = s0;

				while (win->symbolsTable.uniqueDataWithDot.contains(s))
					s = s0 + QString::number(++k);
			
				s.remove(item->fullName() + tr("."));
			}

			if (!dat.hasRow(s) || f != dat.value(s,0))
			{
				dat.value(s,0) = func;
				if (currentNetwork())
				{
					if (item->name.isEmpty())
						currentNetwork()->changeData(s + tr(" = ") + f,item,tr("Assignments"),&dat);
					else
						currentNetwork()->changeData(item->fullName() + tr(".") + s + tr(" = ") + f,item,tr("Assignments"),&dat);
					BasicInformationTool::removeUnusedParametersInModel(currentNetwork());
				}
				else
					item->textDataTable(tr("Assignments")) = dat;
			}
		}
		else
		if (item)
		{
			QString s = var;
			if (s == item->fullName() || s == item->name || s.toLower() == Self.toLower() || s.isEmpty())
				s = Self;
			StoichiometryTool::userModifiedRates.removeAll(item);
			DataTable<QString> dat = item->textDataTable(tr("Assignments"));
			if (dat.hasRow(s))
			{
				dat.removeRow(s);
				if (currentNetwork())
				{
					if (item->name.isEmpty())
						currentNetwork()->changeData(s + tr(" removed"),item,tr("Assignments"),&dat);
					else
						currentNetwork()->changeData(item->fullName() + tr(".") + s + tr(" removed"),item,tr("Assignments"),&dat);
					BasicInformationTool::removeUnusedParametersInModel(currentNetwork());
				}
				else
					item->textDataTable(tr("Assignments")) = dat;
			}
		}
		if (sem)
			sem->release();
	}

	tc_strings AssignmentFunctionsTool::_getForcingFunctionNames(tc_items a0)
	{
		return fToS->getForcingFunctionNames(a0);
	}

	tc_strings AssignmentFunctionsTool_FToS::getForcingFunctionNames(tc_items a0)
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
		return (tc_strings)ConvertValue(p);
	}

	tc_strings AssignmentFunctionsTool::_getForcingFunctionAssignments(tc_items a0)
	{
		return fToS->getForcingFunctionAssignments(a0);
	}

	tc_strings AssignmentFunctionsTool_FToS::getForcingFunctionAssignments(tc_items a0)
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
		return (tc_strings)ConvertValue(p);
	}

	void AssignmentFunctionsTool::_addForcingFunction(long o, const char* a, const char* b)
	{
		return fToS->addForcingFunction(o,a,b);
	}

	void AssignmentFunctionsTool_FToS::addForcingFunction(long o, const char* a, const char* b)
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
				double d = EquationParser::eval(currentNetwork(), s, &b);
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

