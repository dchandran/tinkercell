/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool places a stoichiometry table and a table of rates inside all connection handles.
An associated GraphicsTool is also defined. This allow brings up a table for editting
the stoichiometry and rates tables.

****************************************************************************/

#include <QSettings>
#include <QToolBox>
#include <QMessageBox>
#include "GraphicsScene.h"
#include "NetworkWindow.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "ConnectionSelection.h"
#include "StoichiometryTool.h"
#include "ModelSummaryTool.h"
#include "BasicInformationTool.h"
#include "CThread.h"
#include "DefaultReactionRates.h"
#include "muParserDef.h"
#include "muParser.h"
#include "muParserInt.h"
#include <QtDebug>

namespace Tinkercell
{

	void StoichiometryTool::select(int)
	{
		NetworkWindow * win = currentWindow();
		if (!win) return;

		if (dockWidget && dockWidget->widget() != this)
			dockWidget->setWidget(this);

		connectionHandles.clear();
		ConnectionHandle * connectionHandle = 0;

		QList<ItemHandle*> list = win->selectedHandles();

		for (int i=0; i < list.size(); ++i)
		{
			connectionHandle = ConnectionHandle::asConnection(list[i]);
			if (connectionHandle  && !connectionHandles.contains(connectionHandle))
				connectionHandles += connectionHandle;
		}
		if (connectionHandles.size() < 1) return;

		updateTable();

		openedByUser = true;
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

		//showMatrix();
		//else
		//  editTool->hideMatrix();

		this->setFocus();

	}

	void StoichiometryTool::deselect(int)
	{
		if (openedByUser && (!dockWidget || dockWidget->isFloating()))
		{
			openedByUser = false;
			if (dockWidget != 0)
				dockWidget->hide();
			else
				this->hide();
		}
	}

	bool StoichiometryTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(windowClosing(NetworkWindow * , bool *)),this,SLOT(sceneClosing(NetworkWindow * , bool *)));

			connect(mainWindow,SIGNAL(itemsInserted(NetworkWindow*,const QList<ItemHandle*>&)),
				this, SLOT(itemsInserted(NetworkWindow*, const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				this,SLOT(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			connect(mainWindow,SIGNAL(historyChanged(int)),this,SLOT(historyUpdate(int)));

			dockWidget = mainWindow->addDockingWindow(name,this,Qt::BottomDockWidgetArea,Qt::NoDockWidgetArea);
			if (dockWidget)
			{
				dockWidget->setWindowFlags(Qt::Tool);
				dockWidget->setAttribute(Qt::WA_ContentsPropagated);
				dockWidget->setPalette(QPalette(QColor(255,255,255,255)));
				dockWidget->setAutoFillBackground(true);
				//dockWidget->setWindowOpacity(0.9);

				dockWidget->move(mainWindow->geometry().bottomRight() - QPoint((int)(sizeHint().width()*1.5),(int)(sizeHint().height()*1.5)));

				QCoreApplication::setOrganizationName("TinkerCell");
				QCoreApplication::setOrganizationDomain("www.tinkercell.com");
				QCoreApplication::setApplicationName("TinkerCell");

				QSettings settings("TinkerCell", "TinkerCell");

				settings.beginGroup("StoichiometryTool");
				//QSize sz = settings.value("size", sizeHint()).toSize();
				//dockWidget->move(settings.value("pos", dockWidget->pos()).toPoint());

				dockWidget->setFloating(true);
				settings.endGroup();
				dockWidget->hide();
			}
		}
		return (mainWindow != 0);
	}

	void StoichiometryTool::toolLoaded(Tool*)
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

		if (!connected2 && mainWindow->tool(tr("Connection Selection")))
		{
			QWidget * widget = mainWindow->tool(tr("Connection Selection"));
			ConnectionSelection * connectionSelection = static_cast<ConnectionSelection*>(widget);
			if (connectionSelection)
			{
				connected2 = true;
				connect(this,SIGNAL(setMiddleBox(int,const QString&)),connectionSelection, SLOT(showMiddleBox(int,const QString&)));
			}
		}
	}

	void StoichiometryTool::aboutToDisplayModel(const QList<ItemHandle*>& items, QHash<QString,qreal>& , QHash<QString,QString>& equations)
	{
		connectionHandles.clear();
		for (int i=0; i < items.size(); ++i)
			if (items[i] && items[i]->type == ConnectionHandle::Type)
				connectionHandles += static_cast<ConnectionHandle*>(items[i]);
		updateTable();
		for (int i=0; i < ratesTable.rowCount() && i < updatedRowNames.size(); ++i)
			if (ratesTable.item(i,0))
			{
				equations[ updatedRowNames[i] ] = ratesTable.item(i,0)->text();
			}
	}

	void StoichiometryTool::displayModel(QTabWidget& widgets, const QList<ItemHandle*>& , QHash<QString,qreal>& constants, QHash<QString,QString>& )
	{
		if (ratesTable.rowCount() > 0)
		{
			for (int i=0; i < updatedColumnNames.size(); ++i)
				if (!constants.contains(updatedColumnNames[i]))
				{
					constants[ updatedColumnNames[i] ] = 1.0;
				}

				if (dockWidget && dockWidget->isVisible())
					dockWidget->hide();

				widgets.addTab(this,tr("Rates"));
				//hideMatrix();
		}
		else
			if (dockWidget && dockWidget->widget() != this)
				dockWidget->setWidget(this);
	}

	void StoichiometryTool::historyUpdate(int )
	{
		if (isVisible() || (parentWidget() && parentWidget() != mainWindow && parentWidget()->isVisible()))// && dockWidget && dockWidget->isVisible())
			updateTable();
	}

	void StoichiometryTool::connectCFuntions()
	{
		connect(&fToS,SIGNAL(getStoichiometry(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&)),this,SLOT(getStoichiometrySlot(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&)));
		connect(&fToS,SIGNAL(setStoichiometry(QSemaphore*,QList<ItemHandle*>&,const DataTable<qreal>&)),this,SLOT(setStoichiometrySlot(QSemaphore*,QList<ItemHandle*>&,const DataTable<qreal>&)));
		connect(&fToS,SIGNAL(getRates(QSemaphore*,QStringList*,const QList<ItemHandle*>&)),this,SLOT(getRatesSlot(QSemaphore*,QStringList*,const QList<ItemHandle*>&)));
		connect(&fToS,SIGNAL(setRates(QSemaphore*,QList<ItemHandle*>&,const QStringList&)),this,SLOT(setRatesSlot(QSemaphore*,QList<ItemHandle*>&,const QStringList&)));
	}

	typedef void (*tc_StoichiometryTool_api)(
		Matrix (*getStoichiometry)(Array ),
		void (*setStoichiometry)(Array ,Matrix ),
		char** (*getRates)(Array ),
		void (*setRates)(Array ,char** )
		);

	void StoichiometryTool::setupFunctionPointers( QLibrary * library)
	{
		tc_StoichiometryTool_api f = (tc_StoichiometryTool_api)library->resolve("tc_StoichiometryTool_api");
		if (f)
		{
			//qDebug() << "tc_StoichiometryTool_api resolved";
			f(
				&(_getStoichiometry),
				&(_setStoichiometry),
				&(_getRates),
				&(_setRates)
				);
		}
	}

	void StoichiometryTool::getStoichiometrySlot(QSemaphore * s, DataTable<qreal>* p, const QList<ItemHandle*>& items)
	{
		if (p)
		{
			(*p) = (getStoichiometry(items));
		}
		if (s)
			s->release();
	}

	void StoichiometryTool::getRatesSlot(QSemaphore * s, QStringList* p, const QList<ItemHandle*>& items)
	{
		if (p)
		{
			(*p) = getRates(items);
		}
		if (s)
			s->release();
	}


	void StoichiometryTool::setStoichiometrySlot(QSemaphore * s, QList<ItemHandle*>& items, const DataTable<qreal>& table)
	{
		if (mainWindow && mainWindow->currentWindow())
		{
			setStoichiometry(mainWindow->currentWindow(),items,table);
		}
		if (s)
			s->release();
	}

	void StoichiometryTool::setRatesSlot(QSemaphore * s, QList<ItemHandle*>& items, const QStringList& rates)
	{
		if (mainWindow && mainWindow->currentWindow())
			setRates(mainWindow->currentWindow(),items,rates);
		if (s)
			s->release();
	}

	void StoichiometryTool::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& list, QPointF , Qt::KeyboardModifiers )
	{
		if (scene && (isVisible() || (parentWidget() && parentWidget() != mainWindow && parentWidget()->isVisible())))
		{
			connectionHandles.clear();
			ConnectionGraphicsItem * connection = 0;
			ConnectionHandle * connectionHandle = 0;
			for (int i=0; i < list.size(); ++i)
			{
				connection = ConnectionGraphicsItem::topLevelConnectionItem(list[i]);
				if (connection && connection->itemHandle && connection->itemHandle->type == ConnectionHandle::Type)
				{
					connectionHandle = static_cast<ConnectionHandle*>(connection->itemHandle);
					if (connectionHandle)
						connectionHandles += connectionHandle;
				}
			}

			updateTable();
		}

		bool reactions = false;
		ItemHandle * handle;

		for (int i=0; i < list.size(); ++i)
			if (qgraphicsitem_cast<ConnectionGraphicsItem*>(list[i]) && (handle = getHandle(list[i])) && handle->isA(tr("Biochemical")))
			{
				reactions = true;
				break;
			}

			if (reactions)
			{
				if (separator)
					mainWindow->contextItemsMenu.addAction(separator);
				else
					separator = mainWindow->contextItemsMenu.addSeparator();

				mainWindow->contextItemsMenu.addAction(&autoReverse);
			}
			else
			{
				if (separator)
					mainWindow->contextItemsMenu.removeAction(separator);
				mainWindow->contextItemsMenu.removeAction(&autoReverse);
			}
	}

	void StoichiometryTool::sceneClosing(NetworkWindow * , bool *)
	{
		QCoreApplication::setOrganizationName("TinkerCell");
		QCoreApplication::setOrganizationDomain("www.tinkercell.com");
		QCoreApplication::setApplicationName("TinkerCell");

		QSettings settings("TinkerCell", "TinkerCell");

		if (dockWidget)
		{
			settings.beginGroup("StoichiometryTool");
			//settings.setValue("floating", dockWidget && dockWidget->isFloating());
			settings.setValue("size", dockWidget->size());
			settings.setValue("pos", dockWidget->pos());
			settings.endGroup();
		}
	}

	void StoichiometryTool::itemsInserted(NetworkWindow* win, const QList<ItemHandle*>& handles)
	{
		for (int i=0; i < handles.size(); ++i)
		{
			if (handles[i] && handles[i]->type == ConnectionHandle::Type)
			{
				ConnectionHandle * connectionHandle = static_cast<ConnectionHandle*>(handles[i]);
				if (!connectionHandle->tools.contains(this))
					connectionHandle->tools += this;

				if (connectionHandle->data &&
					(
					!(connectionHandle->hasNumericalData(tr("Stoichiometry"))) ||
					!(connectionHandle->hasTextData(tr("Rates")))
					))
				{
					insertDataMatrix(connectionHandle);
				}
				else  //just for modifier arcs
					if (connectionHandle->data && 
						connectionHandle->hasNumericalData(tr("Stoichiometry")) &&
						connectionHandle->hasNumericalData(tr("Numerical Attributes")) &&
						connectionHandle->hasTextData(tr("Rates")) &&
						(connectionHandle->nodes().size() > (connectionHandle->nodesIn().size() +connectionHandle->nodesOut().size())) &&
						(connectionHandle->data->textData[tr("Rates")].rows() == 1))
					{

						QList<NodeHandle*> nodesIn = connectionHandle->nodesIn(),
							nodesOut = connectionHandle->nodesOut(),
							nodes = connectionHandle->nodes();
						QStringList s1,s2;
						for (int j=0; j < nodesIn.size(); ++j)
							if (nodesIn[j])
								s1 += nodesIn[j]->fullName();
						for (int j=0; j < nodes.size(); ++j)
							if (nodes[j] && !nodesIn.contains(nodes[j]) && !nodesOut.contains(nodes[j]))
								s2 += nodes[j]->fullName();
						QString name = connectionHandle->fullName();

						DataTable<qreal>* nDat = new DataTable<qreal>(connectionHandle->data->numericalData[tr("Numerical Attributes")]);
						DataTable<QString>* sDat = new DataTable<QString>(connectionHandle->data->textData[tr("Rates")]);
						QString oldRate = sDat->value(0,0);
						bool alreadyDone = true;
						for (int j=0; j < s2.size(); ++j)
							if (!oldRate.contains(s2[j]))
							{
								alreadyDone = false;
								break;
							}
							if (win && !alreadyDone)
							{
								nDat->value(tr("Vmax"),0) = 1.0;
								nDat->value(tr("Km"),0) = 1.0;
								sDat->value(0,0) = name + tr(".Vmax * ") + s2.join(tr("*")) + tr("*") + s1.join(tr("*")) + tr("/(") + name + tr(".Km + ") + s1.join(tr("*")) + tr(")");
								win->changeData(connectionHandle->fullName() + tr("'s kinetics changed"),
												QList<ItemHandle*>() << connectionHandle << connectionHandle, 
												QList<QString>() << tr("Numerical Attributes") << tr("Rates"),
												QList<DataTable<qreal>*>() << nDat,
												QList<DataTable<QString>*>() << sDat);
								ConsoleWindow::message(tr("Rate for ") + name + tr(" = ") + sDat->value(0,0));
							}
							delete sDat;
							delete nDat;
					}
			}
		}
	}

	void StoichiometryTool::hideMatrix()
	{
		if (matrixBox)
		{
			matrixBox->setVisible(false);
			if (dockWidget && ratesBox)
			{
				resize( ratesBox->width() + 10, ratesBox->height() + 10);
				dockWidget->resize( ratesBox->width() + 10, ratesBox->height() + 10);
			}
		}
	}

	void StoichiometryTool::showMatrix()
	{
		if (matrixBox)
		{
			matrixBox->setVisible(true);
			if (dockWidget && ratesBox)
			{
				resize( matrixBox->width() + ratesBox->width() + 10, ratesBox->height() + 10);
				dockWidget->resize( matrixBox->width() + ratesBox->width() + 10, ratesBox->height() + 10);
			}
		}
	}

	void StoichiometryTool::setStoichiometry(int , int )
	{
		bool ok = false;
		qreal temp = 0;
		DataTable<qreal> stoicMatrix;
		stoicMatrix.resize(matrixTable.rowCount(),matrixTable.columnCount());

		QTableWidgetItem * tableItem = 0;

		for (int i=0; i < matrixTable.rowCount(); ++i)
			if (matrixTable.verticalHeaderItem(i))
				stoicMatrix.rowName(i) = matrixTable.verticalHeaderItem(i)->text();

		for (int j=0; j < matrixTable.columnCount(); ++j)
			if (matrixTable.horizontalHeaderItem(j))
				stoicMatrix.colName(j) = matrixTable.horizontalHeaderItem(j)->text();

		for (int i=0; i < matrixTable.rowCount(); ++i)
			for (int j=0; j < matrixTable.columnCount(); ++j)
			{
				tableItem = matrixTable.item(i,j);
				if (tableItem != 0)
				{
					temp = tableItem->text().toDouble(&ok);
					if (!ok) temp = 0;
					stoicMatrix.value(i,j) = temp;
				}
			}

			DataTable<qreal> * nDataTable = 0;
			QList< DataTable<qreal> *> nDataTablesNew;
			QList<ItemHandle*> handles;

			int n2 = 0, j0 = 0;
			for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
			{
				if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
				{
					handles += connectionHandles[i];

					if (connectionHandles[i]->hasNumericalData(tr("Stoichiometry")))
					{
						nDataTable = new DataTable<qreal>(connectionHandles[i]->data->numericalData[ tr("Stoichiometry") ]);

						for (int k=0; k < nDataTable->rows(); ++k)
						{
							for (int j=0; j < nDataTable->cols(); ++j)     //get unique species
							{
								j0 = stoicMatrix.colNames().indexOf(nDataTable->colName(j));
								if (j0 >= 0)
									nDataTable->value(k,j) = stoicMatrix.value(n2,j0);
							}
							++n2;
						}
						nDataTablesNew += nDataTable;
					}
				}
			}

			if (mainWindow != 0 && mainWindow->currentWindow() != 0)
				mainWindow->currentWindow()->changeData(tr("stoichiometry changed"),handles,tr("Stoichiometry"),nDataTablesNew);

			for (int i=0; i < nDataTablesNew.size(); ++i)
				if (nDataTablesNew[i])
					delete nDataTablesNew[i];
	}

	static double d = 1.0;
	static double* AddVariable(const char*, void*)
	{
		return &d;
	}

	static double CallFunction(const double*, int)
	{
		return d;
	}

	bool StoichiometryTool::parseRateString(NetworkWindow * win, ItemHandle * handle, QString& s)
	{
		if (!win || !handle) return false;

		static QStringList reservedWords;
		if (reservedWords.isEmpty())
			reservedWords << "time";

		mu::Parser parser;

		s.replace(QRegExp(tr("\\.(?!\\d)")),tr("_qqq_"));
		parser.SetExpr(s.toAscii().data());
		s.replace(tr("_qqq_"),tr("."));
		parser.SetVarFactory(AddVariable, 0);

		SymbolsTable * symbolsTable = &win->symbolsTable;
		QList<ItemHandle*> allHandles = symbolsTable->handlesFullName.values();

		for (int i=0; i < allHandles.size(); ++i)
		{
			if (allHandles[i] && allHandles[i]->hasTextData(tr("Functions")))
			{
				DataTable<QString>& sDat = allHandles[i]->data->textData[tr("Functions")];
				for (int j=0; j < sDat.rows(); ++j)
					parser.DefineFun((allHandles[i]->fullName() + tr("_qqq_") + sDat.rowName(j)).toAscii().data(), &(CallFunction), true);
			}
		}

		QString str;

		try
		{
			parser.Eval();

			if (handle && handle->data && handle->hasNumericalData(tr("Numerical Attributes")))
			{
				// Get the map with the variables
				mu::varmap_type variables = parser.GetVar();

				// Get the number of variables
				mu::varmap_type::const_iterator item = variables.begin();

				// Query the variables
				for (; item!=variables.end(); ++item)
				{
					str = tr(item->first.data());
					str.replace(QRegExp(tr("[^A-Za-z0-9_]")),tr(""));
					str.replace(tr("_qqq_"),tr("."));
					QString str2 = str;
					str2.replace(tr("_"),tr("."));
					if (handle && !reservedWords.contains(str) &&
						!symbolsTable->handlesFullName.contains(str)) //maybe new symbol in the formula
					{
						if (symbolsTable->dataRowsAndCols.contains(str) && symbolsTable->dataRowsAndCols[str].first)
						{
							if (! str.contains(QRegExp(tr("^")+symbolsTable->dataRowsAndCols[str].first->fullName())) )
							{
								handle = symbolsTable->dataRowsAndCols[str].first;
								s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),handle->fullName() + tr(".") + str + tr("\\1"));
								s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + handle->fullName() + tr(".") + str + tr("\\2"));
								s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("$")),tr("\\1") + handle->fullName() + tr(".")  + str);
							}
						}
						else
							if (symbolsTable->dataRowsAndCols.contains(str2) && symbolsTable->dataRowsAndCols[str2].first)
							{
								if (! str2.contains(QRegExp(tr("^")+symbolsTable->dataRowsAndCols[str2].first->fullName())) )
								{
									handle = symbolsTable->dataRowsAndCols[str2].first;
									s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),handle->fullName() + tr(".") + str2 + tr("\\1"));
									s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + handle->fullName() + tr(".") + str2 + tr("\\2"));
									s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("$")),tr("\\1") + handle->fullName() + tr(".")  + str2);
								}
								else
								{
									s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),str2 + tr("\\1"));
									s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + str + tr("\\2"));
									s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("$")),tr("\\1") + str);
								}
							}
							else
							{
								if (symbolsTable->handlesFirstName.contains(str) && symbolsTable->handlesFirstName[str])
								{
									s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),symbolsTable->handlesFirstName[str]->fullName() + tr("\\1"));
									s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + symbolsTable->handlesFirstName[str]->fullName() + tr("\\2"));
									s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("$")),tr("\\1") + symbolsTable->handlesFirstName[str]->fullName());
								}
								else
									if (symbolsTable->handlesFirstName.contains(str2) && symbolsTable->handlesFirstName[str2])
									{
										s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),symbolsTable->handlesFirstName[str2]->fullName() + tr("\\1"));
										s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + symbolsTable->handlesFirstName[str2]->fullName() + tr("\\2"));
										s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("$")),tr("\\1") + symbolsTable->handlesFirstName[str2]->fullName());
									}
									else
									{
										//qDebug() << str << "not in symbol table";
										DataTable<qreal> dat(handle->data->numericalData[tr("Numerical Attributes")]);

										if (!str.contains(QRegExp(tr("^") + handle->fullName() + tr("\\."))))
										{
											str2 = handle->fullName() + tr(".") + str;
											s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),str2 + tr("\\1"));
											s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + str2 + tr("\\2"));
											s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("$")),tr("\\1") + str2);
										}
										else
										{
											str2 = str;
											str.replace(QRegExp(tr("^") + handle->fullName() + tr("\\.")),tr(""));
										}
										dat.value(str,0) = 1.0;
										win->changeData(handle->fullName() + tr(".") + str + tr(" = 1"),handle,tr("Numerical Attributes"),&dat);
										ConsoleWindow::message(tr("New parameter ") + str2 + tr(" = 1.0"));
									}
							}
					}
				}
			}
		}
		catch(mu::Parser::exception_type &e)
		{
			ConsoleWindow::error(tr(e.GetMsg().data()));
			return false;
		}
		return true;
	}

	void StoichiometryTool::setRate(int , int )
	{
		NetworkWindow * win = currentWindow();
		if (!win) return;

		QStringList rates;

		QTableWidgetItem * tableItem = 0;

		for (int i=0; i < ratesTable.rowCount(); ++i)
		{
			tableItem = ratesTable.item(i,0);
			if (tableItem != 0)
				rates.append(tableItem->text());

		}

		DataTable<QString> * sDataTable = 0;

		QList< DataTable<QString> *> sDataTablesNew;
		QList<ItemHandle*> handles;

		int n1 = 0;
		bool change = false;
		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				handles += connectionHandles[i];
				if (connectionHandles[i]->hasTextData(tr("Rates")))
				{
					sDataTable = new DataTable<QString>(connectionHandles[i]->data->textData[ tr("Rates") ]);
					change = false;

					for (int k=0; k < sDataTable->rows() && n1 < rates.size(); ++k)
					{
						QString s = rates.at(n1);
						if (parseRateString(win, connectionHandles[i],s))
						{
							sDataTable->value(k,0) = s;
							change = true;
						}
						++n1;
					}
					if (change)  //if anything changed
						sDataTablesNew += sDataTable;
				}
			}
		}

		if (mainWindow != 0 && mainWindow->currentWindow() != 0)
			mainWindow->currentWindow()->changeData(tr("selected kinetics changed"),handles,tr("Rates"),sDataTablesNew);

		for (int i=0; i < sDataTablesNew.size(); ++i)
			if (sDataTablesNew[i])
				delete sDataTablesNew[i];
	}


	StoichiometryTool::StoichiometryTool() : Tool(tr("Stoichiometry and Rates")),
		autoReverse("Make reversible",this),separator(0)
	{
		QString appDir = QCoreApplication::applicationDirPath();
		openedByUser = false;
		NodeGraphicsReader reader;
		reader.readXml(&graphics1,appDir + tr("/OtherItems/stoichiometry.xml"));
		graphics1.setToolTip(tr("Reaction rates"));
		graphics1.normalize();
		graphics1.scale(40.0/graphics1.sceneBoundingRect().width(),40.0/graphics1.sceneBoundingRect().height());

		reader.readXml(&graphics2,appDir + tr("/OtherItems/curve.xml"));
		graphics2.setToolTip(tr("Reaction stoichiometry"));
		graphics2.normalize();
		graphics2.scale(40.0/graphics2.sceneBoundingRect().width(),40.0/graphics2.sceneBoundingRect().height());

		graphicsItems += new GraphicsItem(this);
		graphicsItems[0]->addToGroup(&graphics1);
		graphicsItems[0]->setToolTip(tr("Reaction rates"));
		//graphicsItem->addToGroup(&graphics2);
		//graphics2.setPos(graphics1.x(),graphics1.y() + graphics1.boundingRect().height());

		/*QToolButton * toolButton = new QToolButton(this);
		toolButton->setIcon(QIcon(appDir + tr("/BasicTools/monitor.PNG")));
		toolButton->setToolTip(tr("Reaction rates"));
		this->buttons.addButton(toolButton);*/

		setPalette(QPalette(QColor(255,255,255,255)));
		setAutoFillBackground(true);

		ratesTable.setAlternatingRowColors(true);
		ratesTable.setEditTriggers ( QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed );
		connect(&ratesTable,SIGNAL(cellChanged(int,int)),this,SLOT(setRate(int,int)));

		matrixTable.setAlternatingRowColors(true);
		matrixTable.setEditTriggers ( QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed );
		connect(&matrixTable,SIGNAL(cellChanged(int,int)),this,SLOT(setStoichiometry(int,int)));

		QFont font = this->font();
		font.setPointSize(12);
		ratesTable.setFont(font);
		matrixTable.setFont(font);

		QHBoxLayout * rowButtonLayout = new QHBoxLayout;

		QToolButton * addRow = new QToolButton(this);
		addRow->setIcon(QIcon(":/images/plus.png"));

		QToolButton * removeRow = new QToolButton(this);
		removeRow->setIcon(QIcon(":/images/minus.png"));

		addRow->setToolTip(tr("Add row"));
		removeRow->setToolTip(tr("Remove row"));

		QToolButton * question1 = new QToolButton(this);
		question1->setIcon(QIcon(":/images/question.png"));

		QToolButton * question2 = new QToolButton(this);
		question2->setIcon(QIcon(":/images/question.png"));

		QString message = tr("The rates shows the reaction rates for the selected reactions.\nThe stoichiometry table shows the corresponding stoichiometries.\n\n For example, if the rates table shows J0.k0*A*B and the stoichiometry table shows -1 for A, -1 for B and +2 for C, then this means that that reaction consumes one A and one B and produces two C molecules, i.e. negative values indicate consumption and positive indicate production. \n\n The rate equation governs the speed at which this consumption and production is taking place. J0.k0 is a parameter for reaction J0. \n\n You may add intermediate molecules to the stoichiometry matrix by adding a column. For example, if there is an intermediate I when A is being converted to B, then you may add a new row and a new column and set the stoichiometries, i.e. A -1 and I +1 for the first reaction and I -1 and B +1 for ths second reaction. This is a way of adding intermediate steps without cluttering the drawing itself.");
		QMessageBox * messageBox = new QMessageBox(QMessageBox::Information,tr("About Stoichiometry and Rates Tables"),message,QMessageBox::StandardButtons(QMessageBox::Close), const_cast<QWidget*>((QWidget*)this), Qt::WindowFlags (Qt::Dialog));
		connect(question1,SIGNAL(pressed()),messageBox,SLOT(exec()));
		connect(question2,SIGNAL(pressed()),messageBox,SLOT(exec()));

		rowButtonLayout->addWidget(addRow);
		rowButtonLayout->addWidget(removeRow);
		rowButtonLayout->addStretch(1);
		rowButtonLayout->addWidget(question1);

		connect(addRow,SIGNAL(pressed()),this,SLOT(addRow()));
		connect(removeRow,SIGNAL(pressed()),this,SLOT(removeRow()));

		QVBoxLayout * colButtonLayout = new QVBoxLayout;

		QToolButton * addCol = new QToolButton(this);
		addCol->setIcon(QIcon(":/images/plus.png"));

		QToolButton * removeCol = new QToolButton(this);
		removeCol->setIcon(QIcon(":/images/minus.png"));

		addCol->setToolTip(tr("Add column"));
		removeCol->setToolTip(tr("Remove column"));

		colButtonLayout->addWidget(addCol);
		colButtonLayout->addWidget(removeCol);
		colButtonLayout->addStretch(1);
		colButtonLayout->addWidget(question2);

		connect(addCol,SIGNAL(pressed()),this,SLOT(addCol()));
		connect(removeCol,SIGNAL(pressed()),this,SLOT(removeCol()));

		ratesBox = new QGroupBox(tr(" Rates for selected items "),this);
		ratesBox->setMinimumWidth(100);

		matrixBox = new QGroupBox(tr(" Stoich "),this);
		matrixBox->setMinimumWidth(100);

		matrixBox->setTitle(" Stoichiometry matrix for selected items");
		QVBoxLayout * ratesLayout = new QVBoxLayout;
		ratesLayout->addWidget(&ratesTable);
		ratesLayout->addLayout(rowButtonLayout);

		QHBoxLayout * matrixLayout = new QHBoxLayout;
		matrixLayout->addWidget(&matrixTable);
		matrixLayout->addLayout(colButtonLayout);

		ratesBox->setLayout(ratesLayout);
		matrixBox->setLayout(matrixLayout);

		dockWidget = 0;

		QHBoxLayout * layout = new QHBoxLayout;

		QSplitter * splitter = new QSplitter;
		splitter->addWidget(ratesBox);
		splitter->addWidget(matrixBox);
		//layout->addWidget(ratesBox,1);
		//layout->addWidget(matrixBox,2);
		layout->addWidget(splitter);
		setLayout(layout);

		connectCFuntions();

		//matrixTable.setItemDelegate(&delegate);

		connect(&autoReverse,SIGNAL(triggered()),this,SLOT(addReverseReaction()));
		autoReverse.setIcon(QIcon(":/images/horizontalFlip.png"));
	}

	void StoichiometryTool::addReverseReaction()
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		QList<ItemHandle*> items;
		QList<DataTable<qreal>*> nDataNew, nDataOld;
		QList<DataTable<QString>*> sDataNew, sDataOld;
		DataTable<qreal>* nDat;
		DataTable<QString>* sDat;

		QList<QGraphicsItem*>& selected = scene->selected();
		ItemHandle* handle;
		QStringList cannots;

		for (int i=0; i < selected.size(); ++i)
		{
			if ((handle = getHandle(selected[i])) && handle->data
				&& handle->hasNumericalData(tr("Stoichiometry"))
				&& handle->hasTextData(tr("Rates")))
			{
				items << handle;

				if (handle->data->numericalData[tr("Stoichiometry")].rows() != 1 ||
					handle->data->textData[tr("Rates")].rows() != 1)
				{
					cannots << handle->fullName();
				}
				else
				{
					nDat = new DataTable<qreal>(handle->data->numericalData[tr("Stoichiometry")]);
					sDat = new DataTable<QString>(handle->data->textData[tr("Rates")]);

					nDat->rowName(0) = tr("forward");
					nDat->insertRow(1,tr("reverse"));

					sDat->rowName(0) = tr("forward");
					sDat->insertRow(1,tr("reverse"));

					QStringList reactants;
					for (int j=0; j < nDat->cols(); ++j)
					{
						nDat->value(1,j) = -nDat->value(0,j);
						if (nDat->value(1,j) < 0)
						{
							if (nDat->value(1,j) == -1)
								reactants << nDat->colName(j);
							else
								reactants << ( nDat->colName(j) + tr("^") + QString::number(nDat->value(1,j)) );
						}
					}

					if (reactants.isEmpty())
						sDat->value(1,0) = handle->fullName() + tr(".k0");
					else
						sDat->value(1,0) = handle->fullName() + tr(".k0*") + reactants.join(tr("*"));

					nDataNew << nDat;
					sDataNew << sDat;

					nDataOld << &(handle->data->numericalData[tr("Stoichiometry")]);
					sDataOld << &(handle->data->textData[tr("Rates")]);
				}
			}
		}

		if (nDataNew.size() > 0)
		{
			QUndoCommand * command = new Change2DataCommand<qreal,QString>(tr("reversible reactions added"),nDataOld,nDataNew,sDataOld,sDataNew);

			if (scene->historyStack)
				scene->historyStack->push(command);
			else
			{
				command->redo();
				delete command;
			}

			for (int i=0; i < nDataNew.size(); ++i)
				delete nDataNew[i];

			for (int i=0; i < sDataNew.size(); ++i)
				delete sDataNew[i];
		}

		if (cannots.size() > 0)
		{
			ConsoleWindow::message(tr("Automatic reversibility can only be added to connections with one reaction, so the following were ignored: ") +
				cannots.join(tr(",")));
		}
		else
		{
			QString appDir = QCoreApplication::applicationDirPath();
			QString filename = appDir + tr("/OtherItems/Reversible.xml");
			emit setMiddleBox(1,filename);
		}
	}

	QSize StoichiometryTool::sizeHint() const
	{
		return QSize(600, 200);
	}

	void StoichiometryTool::insertDataMatrix(ConnectionHandle * handle)
	{
		if (!handle || !handle->data) return;

		DefaultRateAndStoichiometry::setDefault(handle);
	}
	/*
	int StoichiometryTool::updateText()
	{
	if (!textView) return 20;

	QStringList colNames, rowNames, rates;
	DataTable<qreal> * nDataTable = 0;
	DataTable<QString> * sDataTable = 0;
	DataTable<qreal> combinedTable;

	for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
	{
	if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
	{
	if (connectionHandles[i]->hasNumericalData(this->name))
	{
	nDataTable = &(connectionHandles[i]->data->numericalData[this->name]);
	for (int j=0; j < nDataTable->cols(); ++j) //get unique species
	if (!colNames.contains(nDataTable->colName(j))
	&& !colNames.contains(QString(nDataTable->colName(j)).replace(".","_"))
	&& !colNames.contains(QString(nDataTable->colName(j)).replace("_",".")))
	colNames += nDataTable->colName(j);
	}
	if (connectionHandles[i]->hasTextData(this->name))
	{
	sDataTable = &(connectionHandles[i]->data->textData[this->name]);
	for (int j=0; j < sDataTable->rows(); ++j) //get rates and reaction names
	{
	QString row;
	if (sDataTable->rows() > 1)
	{
	row = sDataTable->rowName(j);
	if (row.length() == 0) row = tr("_J0");
	row = connectionHandles[i]->fullName() + tr(".") + row;
	}
	else
	row = connectionHandles[i]->fullName();

	int i = 0;
	while (rowNames.contains(row))
	row = tr("_J") + QString::number(i++); //avoid duplicate rowname
	rowNames += row;
	rates += sDataTable->value(j,0);
	}
	}
	}
	}

	combinedTable.resize(rowNames.size(),colNames.size());
	for (int i=0; i < colNames.size(); ++i)
	combinedTable.colName(i) = colNames[i];

	for (int i=0; i < rowNames.size(); ++i)
	combinedTable.rowName(i) = rowNames[i];

	int n = 0, j0;
	for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
	if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
	if (connectionHandles[i]->hasNumericalData(this->name))
	{
	nDataTable = &(connectionHandles[i]->data->numericalData[this->name]);
	for (int k=0; k < nDataTable->rows(); ++k)
	{
	for (int j=0; j < nDataTable->cols(); ++j)     //get unique species
	{
	j0 = combinedTable.colNames().indexOf(nDataTable->colName(j));
	if (j0 < 0)
	j0 = combinedTable.colNames().indexOf(QString(nDataTable->colName(j)).replace(".","_"));
	if (j0 < 0)
	j0 = combinedTable.colNames().indexOf(QString(nDataTable->colName(j)).replace("_","."));

	if (j0 >= 0)
	combinedTable.value(n,j0) = nDataTable->value(k,j);
	}
	++n;
	}
	}

	int nameSz = 4, rateSz = 4, reacSize = 9;

	for (int i=0; i < rowNames.size(); ++i)
	if (rowNames[i].length() > nameSz)
	nameSz = rowNames[i].size();

	for (int i=0; i < rates.size(); ++i)
	if (rates[i].length() > rateSz)
	rateSz = rates[i].size();

	QStringList reactions;
	for (int i=0; i < combinedTable.rows(); ++i)
	{
	QStringList lhs, rhs;
	for (int j=0; j < combinedTable.cols(); ++j)
	{
	if (combinedTable.value(i,j) < 0)
	if (combinedTable.value(i,j) != -1)
	lhs += QString::number(- combinedTable.value(i,j)) + colNames[j];
	else
	lhs += colNames[j];

	if (combinedTable.value(i,j) > 0)
	if (combinedTable.value(i,j) != 1)
	rhs += QString::number(combinedTable.value(i,j)) + colNames[j];
	else
	rhs += colNames[j];
	}

	QString s;
	if (lhs.isEmpty() && rhs.isEmpty())
	{
	s = tr("(no reaction)");
	}
	else
	{
	if (lhs.isEmpty())
	lhs += tr("(nothing)");
	if (rhs.isEmpty())
	rhs += tr("(nothing)");

	s = lhs.join(" + ") + tr(" --> ") + rhs.join(" + ");
	}
	if (s.length() > reacSize)
	reacSize= s.length();
	reactions += s;
	}

	nameSz += 5;
	rateSz += 5;
	reacSize += 5;

	QString text;

	text += tr("Name").leftJustified(nameSz) + tr("     ")
	+ tr("Rate").rightJustified(rateSz) + tr("     ") + tr("Reaction").rightJustified(reacSize) + tr("\n\n");

	for (int i=0; i < rowNames.size() &&  i < rates.size() && i < reactions.size(); ++i)
	{
	text += rowNames[i].leftJustified(nameSz)
	+ tr("     ") + rates[i].leftJustified(rateSz)
	+ tr("     ") + reactions[i].rightJustified(reacSize) + tr("\n\n");
	}

	textView->setPlainText(text);

	return (int)((rateSz-5) * 5 * (1 + textView->fontPointSize()));

	}
	*/
	void StoichiometryTool::updateTable()
	{
		updatedColumnNames.clear();
		updatedRowNames.clear();

		if (connectionHandles.size() < 1)
		{
			matrixTable.clearContents();
			ratesTable.clearContents();
			matrixTable.setRowCount(0);
			ratesTable.setRowCount(0);
			//if (textView) textView->setPlainText(tr(""));
			return;
		}

		//updateText();

		QStringList colNames, rowNames, rates;
		DataTable<qreal> * nDataTable = 0;
		DataTable<QString> * sDataTable = 0;
		DataTable<qreal> combinedTable;

		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				if (connectionHandles[i]->hasNumericalData(tr("Stoichiometry")))
				{
					nDataTable = &(connectionHandles[i]->data->numericalData[tr("Stoichiometry")]);
					for (int j=0; j < nDataTable->cols(); ++j) //get unique species
						if (!colNames.contains(nDataTable->colName(j))
							&& !colNames.contains(QString(nDataTable->colName(j)).replace(".","_"))
							&& !colNames.contains(QString(nDataTable->colName(j)).replace("_",".")))
							colNames += nDataTable->colName(j);
				}

				if (connectionHandles[i]->hasTextData(tr("Rates")))
				{
					sDataTable = &(connectionHandles[i]->data->textData[tr("Rates")]);
					for (int j=0; j < sDataTable->rows(); ++j) //get rates and reaction names
					{
						QString row;
						if (sDataTable->rows() > 1)
						{
							row = sDataTable->rowName(j);
							if (row.length() == 0) row = tr("_J0");
							row = connectionHandles[i]->fullName() + tr(".") + row;
						}
						else
							row = connectionHandles[i]->fullName();

						int i = 0;
						while (rowNames.contains(row))
							row = tr("_J") + QString::number(i++); //avoid duplicate rowname
						rowNames += row;
						rates += sDataTable->value(j,0);
					}
				}
			}
		}

		combinedTable.resize(rowNames.size(),colNames.size());
		for (int i=0; i < colNames.size(); ++i)
			combinedTable.colName(i) = colNames[i];

		for (int i=0; i < rowNames.size(); ++i)
			combinedTable.rowName(i) = rowNames[i];

		int n = 0, j0;
		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
				if (connectionHandles[i]->hasNumericalData(tr("Stoichiometry")))
				{
					nDataTable = &(connectionHandles[i]->data->numericalData[tr("Stoichiometry")]);
					for (int k=0; k < nDataTable->rows(); ++k)
					{
						for (int j=0; j < nDataTable->cols(); ++j)     //get unique species
						{
							j0 = combinedTable.colNames().indexOf(nDataTable->colName(j));
							if (j0 < 0)
								j0 = combinedTable.colNames().indexOf(QString(nDataTable->colName(j)).replace(".","_"));
							if (j0 < 0)
								j0 = combinedTable.colNames().indexOf(QString(nDataTable->colName(j)).replace("_","."));

							if (j0 >= 0)
								combinedTable.value(n,j0) = nDataTable->value(k,j);
						}
						++n;
					}
				}

				ratesTable.setRowCount(rowNames.size());
				ratesTable.setColumnCount(1);
				ratesTable.setHorizontalHeaderLabels(QStringList() << "Rates");
				ratesTable.setVerticalHeaderLabels(rowNames);

				matrixTable.setRowCount(rowNames.size());
				matrixTable.setColumnCount(colNames.size());
				matrixTable.setHorizontalHeaderLabels(colNames);
				matrixTable.setVerticalHeaderLabels(rowNames);

				disconnect(&ratesTable,SIGNAL(cellChanged(int,int)),this,SLOT(setRate(int,int)));
				disconnect(&matrixTable,SIGNAL(cellChanged(int,int)),this,SLOT(setStoichiometry(int,int)));

				for (int i=0; i < matrixTable.rowCount(); ++i)
				{
					ratesTable.setItem(i,0,new QTableWidgetItem(rates[i]));
					for (int j=0; j < matrixTable.columnCount(); ++j)
					{
						matrixTable.setItem(i,j,new QTableWidgetItem(QString::number(combinedTable.value(i,j))));
					}
				}
				ratesTable.resizeColumnToContents(0);
				connect(&ratesTable,SIGNAL(cellChanged(int,int)),this,SLOT(setRate(int,int)));
				connect(&matrixTable,SIGNAL(cellChanged(int,int)),this,SLOT(setStoichiometry(int,int)));

				updatedColumnNames = colNames;
				updatedRowNames = rowNames;
	}

	void StoichiometryTool::addRow()
	{
		if (connectionHandles.isEmpty()) return;

		ItemHandle * lastItem = connectionHandles.last();
		if (lastItem == 0 || lastItem->data == 0) return;

		disconnect(&ratesTable,SIGNAL(cellChanged(int,int)),this,SLOT(setRate(int,int)));
		disconnect(&matrixTable,SIGNAL(cellChanged(int,int)),this,SLOT(setStoichiometry(int,int)));

		//add a row in the rates data and stoichiometry data
		if (lastItem->hasNumericalData(tr("Stoichiometry")) &&
			lastItem->hasTextData(tr("Rates")))
		{
			QString rowName;

			DataTable<qreal> * nDat = &(lastItem->data->numericalData[tr("Stoichiometry")]);
			DataTable<qreal> * nDataTable = new DataTable<qreal>(*nDat);

			QString newName = QInputDialog::getText(this,tr("New Reaction"),tr("Row name (without prefix) :"));

			if (newName.isNull() || newName.isEmpty())
				return;
			else
				rowName = Tinkercell::RemoveDisallowedCharactersFromName(newName);

			if (!nDataTable->insertRow(nDataTable->rows(),rowName))
			{
				delete nDataTable;
				//QMessageBox::information(mainWindow,tr("Add Row"),tr("That row name is already being used"),QMessageBox::Ok,QMessageBox::Ok);
				ConsoleWindow::error(tr("That row name is already being used."));
				return;
			}

			DataTable<QString> * sDat = &(lastItem->data->textData[tr("Rates")]);
			DataTable<QString> * sDataTable = new DataTable<QString>(*sDat);
			if (!sDataTable->insertRow(sDataTable->rows(),rowName))
			{
				delete nDataTable;
				delete sDataTable;
				//QMessageBox::information(mainWindow,tr("Add Row"),tr("That row name is already being used"),QMessageBox::Ok,QMessageBox::Ok);
				ConsoleWindow::error(tr("That row name is already being used."));
				return;
			}

			for (int i=0; i < nDataTable->cols(); ++i)
				nDataTable->value(nDataTable->rows()-1,i) = 0;

			sDataTable->value(sDataTable->rows()-1,0) = tr("0");


			if (mainWindow && mainWindow->currentWindow())
			{
				mainWindow->currentWindow()->changeData(tr("rates row for ") + lastItem->fullName() + tr(" added"),QList<ItemHandle*>() << lastItem,nDat,nDataTable,sDat,sDataTable);
			}


			delete nDataTable;
			delete sDataTable;


			int n = matrixTable.rowCount();
			matrixTable.insertRow(n);
			ratesTable.insertRow(n);

			matrixTable.setVerticalHeaderItem(n, new QTableWidgetItem(rowName));
			ratesTable.setVerticalHeaderItem(n, new QTableWidgetItem(rowName));

			for (int i=0; i < matrixTable.columnCount(); ++i)
				matrixTable.setItem(n,i,new QTableWidgetItem("0"));

			ratesTable.setItem(n,0,new QTableWidgetItem("0"));
		}

		connect(&ratesTable,SIGNAL(cellChanged(int,int)),this,SLOT(setRate(int,int)));
		connect(&matrixTable,SIGNAL(cellChanged(int,int)),this,SLOT(setStoichiometry(int,int)));
	}
	void StoichiometryTool::removeRow()
	{
		if (connectionHandles.size() < 1)
		{
			return;
		}
		QList<QTableWidgetItem*> selectedItems = ratesTable.selectedItems();
		selectedItems += matrixTable.selectedItems();
		if (selectedItems.isEmpty()) return;

		DataTable<qreal> * nDataTable = 0;
		DataTable<QString> * sDataTable = 0;

		QList< DataTable<qreal>* > nDataTablesOld, nDataTablesNew;
		QList< DataTable<QString>* > sDataTablesOld, sDataTablesNew;
		QList<ItemHandle*> handles;

		int n = 0;

		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			handles += connectionHandles[i];
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				if (connectionHandles[i]->hasNumericalData(tr("Stoichiometry"))
					&& connectionHandles[i]->hasTextData(tr("Rates")))
				{
					nDataTable = &(connectionHandles[i]->data->numericalData[tr("Stoichiometry")]);
					sDataTable = &(connectionHandles[i]->data->textData[tr("Rates")]);

					QList<int> rowsToRemove;
					for (int j=0; j < nDataTable->rows() && j < sDataTable->rows(); ++j, ++n)
					{
						for (int k=0; k < selectedItems.size(); ++k)
							if (selectedItems[k] && selectedItems[k]->row() == n)
							{
								rowsToRemove += j;
								break;
							}
					}

					DataTable<qreal> * nDataTable2 = new DataTable<qreal>(*nDataTable);
					DataTable<QString> * sDataTable2 = new DataTable<QString>(*sDataTable);

					for (int j=0; j < rowsToRemove.size(); ++j)
					{
						nDataTable2->removeRow(rowsToRemove[j]);
						sDataTable2->removeRow(rowsToRemove[j]);

						for (int k=0; k < rowsToRemove.size(); ++k)
							if (rowsToRemove[k] > rowsToRemove[j])
								rowsToRemove[k] -= 1;
					}

					nDataTablesOld += nDataTable;
					sDataTablesOld += sDataTable;
					nDataTablesNew += nDataTable2;
					sDataTablesNew += sDataTable2;
				}
			}
		}

		if (nDataTablesNew.size() > 0)
		{
			if (mainWindow != 0 && mainWindow->currentWindow() != 0)
			{
				mainWindow->currentWindow()->changeData(tr("selected rates removed"), QList<ItemHandle*>() << handles,nDataTablesOld,nDataTablesNew,sDataTablesOld,sDataTablesNew);
			}

			for (int i=0; i < nDataTablesNew.size(); ++i)
				delete nDataTablesNew[i];

			for (int i=0; i < sDataTablesNew.size(); ++i)
				delete sDataTablesNew[i];
		}
	}
	void StoichiometryTool::addCol()
	{
		if (connectionHandles.size() < 1)
		{
			return;
		}

		DataTable<qreal> * nDataTable = 0;

		QList< DataTable<qreal>* > nDataTablesNew, nDataTablesOld;

		QList<ItemHandle*> handles;

		QString newName = QInputDialog::getText(this,tr("Add Intermediate Species"),tr("Column name (with prefix) :"));

		if (newName.isNull() || newName.isEmpty()) return;

		QString colName = newName;

		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				if (connectionHandles[i]->hasNumericalData(tr("Stoichiometry")))
				{
					nDataTable = &(connectionHandles[i]->data->numericalData[tr("Stoichiometry")]);

					DataTable<qreal> * nDataTable2 = new DataTable<qreal>(*nDataTable);

					nDataTable2->insertCol(nDataTable2->cols(), /*connectionHandles[i]->name + tr(".") +*/ colName);

					nDataTablesOld += nDataTable;
					nDataTablesNew += nDataTable2;
					handles += connectionHandles[i];
				}
			}
		}

		if (nDataTablesNew.size() > 0)
		{
			if (mainWindow != 0 && mainWindow->currentWindow() != 0)
			{
				mainWindow->currentWindow()->changeData(tr("intermediate steps added"), handles,tr("Stoichiometry"),nDataTablesNew);
			}

			for (int i=0; i < nDataTablesNew.size(); ++i)
				delete nDataTablesNew[i];
		}

		updateTable();
	}
	void StoichiometryTool::removeCol()
	{
		if (connectionHandles.size() < 1)
		{
			return;
		}
		QList<QTableWidgetItem*> selectedItems = matrixTable.selectedItems();

		if (selectedItems.isEmpty()) return;

		DataTable<qreal> * nDataTable = 0;
		//DataTable<QString> * sDataTable = 0;

		QList< DataTable<qreal>* > nDataTablesNew, nDataTablesOld;
		//QList< DataTable<QString>* > sDataTablesNew, sDataTablesOld;

		QList<ItemHandle*> handles;


		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				if (connectionHandles[i]->hasNumericalData(tr("Stoichiometry")))
				{
					nDataTable = &(connectionHandles[i]->data->numericalData[tr("Stoichiometry")]);
					//sDataTable = &(connectionHandles[i]->data->textData[tr("Rates")]);

					QList<int> colsToRemove;
					for (int j=0; j < nDataTable->cols(); ++j)
					{
						for (int k=0; k < selectedItems.size(); ++k)
							if (selectedItems[k] &&
								matrixTable.horizontalHeaderItem(selectedItems[k]->column())->text()
								== nDataTable->colName(j))
							{
								colsToRemove += j;
								break;
							}
					}

					DataTable<qreal> * nDataTable2 = new DataTable<qreal>(*nDataTable);
					//DataTable<QString> * sDataTable2 = new DataTable<QString>(*sDataTable);

					for (int j=0; j < colsToRemove.size(); ++j)
					{
						nDataTable2->removeCol(colsToRemove[j]);
						//sDataTable2->removeRow(rowsToRemove[j]);

						for (int k=0; k < colsToRemove.size(); ++k)
							if (colsToRemove[k] > colsToRemove[j])
								colsToRemove[k] -= 1;
					}

					nDataTablesOld += nDataTable;
					nDataTablesNew += nDataTable2;
					handles += connectionHandles[i];
				}
			}
		}

		if (nDataTablesNew.size() > 0)
		{
			if (mainWindow != 0 && mainWindow->currentWindow() != 0)
			{
				mainWindow->currentWindow()->changeData(tr("stoichiometry columns removed"), handles,tr("Stoichiometry"),nDataTablesNew);
			}

			for (int i=0; i < nDataTablesNew.size(); ++i)
				delete nDataTablesNew[i];
		}

		updateTable();
	}

	//get the stoiciometry of the items and return the matrix
	DataTable<qreal> StoichiometryTool::getStoichiometry(const QList<ItemHandle*>& connectionHandles, const QString& replaceDot)
	{
		//qDebug() << "get stoichiometry";

		DataTable<qreal> combinedTable;

		if (connectionHandles.size() < 1)
		{
			return combinedTable;
		}

		QStringList colNames, rowNames, rates;
		DataTable<qreal> * nDataTable = 0;
		DataTable<QString> * sDataTable = 0;

		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				if (connectionHandles[i]->hasNumericalData(QObject::tr("Stoichiometry")))
				{
					nDataTable = &(connectionHandles[i]->data->numericalData[QObject::tr("Stoichiometry")]);
					for (int j=0; j < nDataTable->cols(); ++j) //get unique species
						if (!colNames.contains(nDataTable->colName(j))
							&& !colNames.contains(QString(nDataTable->colName(j)).replace(".",replaceDot))
							&& !colNames.contains(QString(nDataTable->colName(j)).replace(replaceDot,".")))
						{
							colNames += nDataTable->colName(j);
							//qDebug() << nDataTable->colName(j);
						}
				}
				if (connectionHandles[i]->hasTextData(QObject::tr("Rates")))
				{
					sDataTable = &(connectionHandles[i]->data->textData[QObject::tr("Rates")]);
					for (int j=0; j < sDataTable->rows(); ++j) //get rates and reaction names
					{
						QString row;

						if (sDataTable->rows() > 1)
						{
							row = sDataTable->rowName(j);
							if (row.length() == 0) row = tr("_J0");
							row = connectionHandles[i]->fullName() + tr(".") + row;
						}
						else
							row = connectionHandles[i]->fullName();

						int i = 0;
						while (rowNames.contains(row))
							row = tr("_J") + QString::number(i++); //avoid duplicate rowname
						rowNames += row;
						rates += sDataTable->value(j,0);
					}
				}
			}
		}

		combinedTable.resize(rowNames.size(),colNames.size());
		for (int i=0; i < colNames.size(); ++i)
			combinedTable.colName(i) = colNames[i];

		for (int i=0; i < rowNames.size(); ++i)
			combinedTable.rowName(i) = rowNames[i];

		int n = 0, j0;
		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
				if (connectionHandles[i]->hasNumericalData(QObject::tr("Stoichiometry")))
				{
					nDataTable = &(connectionHandles[i]->data->numericalData[QObject::tr("Stoichiometry")]);
					for (int k=0; k < nDataTable->rows(); ++k)
					{
						for (int j=0; j < nDataTable->cols(); ++j)     //get unique species
						{
							j0 = colNames.indexOf(nDataTable->colName(j));
							if (j0 < 0)
								j0 = colNames.indexOf(QString(nDataTable->colName(j)).replace(".",replaceDot));
							if (j0 < 0)
								j0 = colNames.indexOf(QString(nDataTable->colName(j)).replace(replaceDot,"."));

							if (j0 >= 0)
								combinedTable.value(n,j0) = nDataTable->value(k,j);
						}
						++n;
					}
				}

				/*this tool's matrix is actually the transpose of traditional stoichiometry matrix*/

		for (int i=0; i < colNames.size(); ++i)
			combinedTable.colName(i).replace(QString("."),replaceDot);

		for (int i=0; i < rowNames.size(); ++i)
			combinedTable.rowName(i).replace(QString("."),replaceDot);

		return combinedTable.transpose();
	}

	//set the stoiciometry of the items from the matrix
	void StoichiometryTool::setStoichiometry(NetworkWindow * win, QList<ItemHandle*>& connectionHandles,const DataTable<qreal>& N, const QString& replaceDot)
	{
		DataTable<qreal> stoicMatrix = N.transpose();

		if (connectionHandles.size() < 1)
		{
			return;
		}

		DataTable<qreal> * nDataTable = 0;
		//DataTable<QString> * sDataTable = 0;

		QList< DataTable<qreal> *> nDataTablesNew;
		//QList< DataTable<QString> *>  sDataTablesNew;

		QList<ItemHandle*> handles;

		QRegExp regex(QString("([A-Za-z0-9])")+replaceDot+QString("([A-Za-z])"));

		for (int i=0; i < stoicMatrix.rows(); ++i)
		{
			stoicMatrix.rowName(i).replace(regex,QString("\\1.\\2"));
		}
		for (int i=0; i < stoicMatrix.cols(); ++i)
		{
			stoicMatrix.colName(i).replace(regex,QString("\\1.\\2"));
		}


		int n=0;
		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				if (connectionHandles[i]->hasNumericalData(QObject::tr("Stoichiometry")))
				{
					nDataTable = new DataTable<qreal>(connectionHandles[i]->data->numericalData[ QObject::tr("Stoichiometry") ]);
					//sDataTable = new DataTable<QString>(connectionHandles[i]->data->textData[ QObject::tr("Rates") ]);

					bool last = i == connectionHandles.size() - 1;
					QList<bool> pickCol;
					int cols = 0, rows = 0;
					for (int j=0; j < stoicMatrix.cols(); ++j)
						pickCol << false;
					int n0 = n;

					//determine which cols to pick
					for (int k=0; (last || (k < nDataTable->rows())) && n0 < stoicMatrix.rows(); ++k, ++n0, ++rows)
					{
						for (int j=0; j < stoicMatrix.cols(); ++j)
						{
							if (stoicMatrix.at(n0,j) != 0)
							{
								if (!pickCol[j]) ++cols;
								pickCol[j] = true;
							}
						}
					}

					if (nDataTable->rows() != rows || nDataTable->cols() !=  cols)
						nDataTable->resize(rows,cols);

					for (int k=0; k < nDataTable->rows() && n < stoicMatrix.rows(); ++k, ++n)
					{
						int j0 = 0;
						for (int j=0; j < stoicMatrix.cols(); ++j)
						nDataTable->colName(j0) = stoicMatrix.colName(j);
						nDataTable->value(k,j0) = stoicMatrix.at(n,j0);
						++j0;
					}

					nDataTablesNew += nDataTable;
					handles += connectionHandles[i];
				}
			}
		}

		if (win)
		{
			win->changeData(tr("stoichiometry changed"),handles,QObject::tr("Stoichiometry"),nDataTablesNew);
		}

		for (int i=0; i < nDataTablesNew.size(); ++i)
			if (nDataTablesNew[i])
				delete nDataTablesNew[i];
	}

	//get the rates of the given items
	QStringList StoichiometryTool::getRates(const QList<ItemHandle*>& connectionHandles, const QString& )
	{
		QStringList rates;
		DataTable<QString> * sDataTable = 0;

		if (connectionHandles.size() < 1)
		{
			return rates;
		}

		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				if (connectionHandles[i]->hasTextData(QObject::tr("Rates")))
				{
					sDataTable = &(connectionHandles[i]->data->textData[QObject::tr("Rates")]);
					for (int j=0; j < sDataTable->rows(); ++j) //get rates and reaction names
					{
						rates += sDataTable->value(j,0);
					}
				}
			}
		}

		QRegExp regex(tr("\\.(?!\\d)"));
		for (int i=0; i<rates.size(); ++i)
		{
			rates[i].replace(regex,QString("_"));
		}
		return rates;
	}

	//set the rates of the given items
	void StoichiometryTool::setRates(NetworkWindow * win, QList<ItemHandle*>& connectionHandles,const QStringList& list, const QString& replaceDot)
	{
		if (connectionHandles.size() < 1)
		{
			return;
		}

		DataTable<QString> * sDataTable = 0;

		QList< DataTable<QString> *> sDataTablesNew;

		QList<ItemHandle*> handles;

		QRegExp regex(QString("([A-Za-z0-9])") + replaceDot + QString("([A-Za-z])"));

		int n=0;
		bool change = false;
		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				if (connectionHandles[i]->hasTextData(QObject::tr("Rates")))
				{
					//nDataTable = new DataTable<qreal>(connectionHandles[i]->data->numericalData[ thisName ]);
					sDataTable = new DataTable<QString>(connectionHandles[i]->data->textData[ QObject::tr("Rates") ]);

					bool last = (i == (connectionHandles.size() - 1));
					int /*cols = 0,*/ rows = 0;

					int n0 = n;

					for (int k=0; (last || (k < sDataTable->rows() /*&& k < nDataTable->rows()*/)) && n0 < list.size(); ++k, ++n0, ++rows)
					{

					}

					if (sDataTable->rows() != rows)
						sDataTable->resize(rows,1);
					change = false;
					for (int k=0; k < sDataTable->rows() /*&& k < nDataTable->rows()*/ && n < list.size(); ++k, ++n)
					{
						sDataTable->value(k,0) = list.at(n);
						sDataTable->value(k,0).replace(regex,QString("\\1.\\2"));
						if (parseRateString(win,connectionHandles[i],sDataTable->value(k,0)))
						{
							change = true;
						}
					}
					if (change)
					{
						sDataTablesNew += sDataTable;
						handles += connectionHandles[i];
					}
				}
			}
		}
		if (win)
		{
			win->changeData(tr("kinetic rates changed"),handles,QObject::tr("Rates"),sDataTablesNew);
		}

		for (int i=0; i < sDataTablesNew.size(); ++i)
			if (sDataTablesNew[i])
				delete sDataTablesNew[i];
	}


	/************************************************************/

	StoichiometryTool_FToS StoichiometryTool::fToS;

	Matrix StoichiometryTool::_getStoichiometry(Array a0)
	{
		return fToS.getStoichiometry(a0);
	}

	Matrix StoichiometryTool_FToS::getStoichiometry(Array a0)
	{
		QList<ItemHandle*> * list = ConvertValue(a0);
		QSemaphore * s = new QSemaphore(1);
		DataTable<qreal> * p = new DataTable<qreal>;
		s->acquire();
		emit getStoichiometry(s,p,*list);
		s->acquire();
		s->release();
		delete s;
		delete list;
		if (p)
		{
			Matrix m = ConvertValue(*p);
			delete p;
			return m;
		}
		return emptyMatrix();
	}

	void StoichiometryTool::_setStoichiometry(Array a0,Matrix a1)
	{
		return fToS.setStoichiometry(a0,a1);
	}

	void StoichiometryTool_FToS::setStoichiometry(Array a0,Matrix a1)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*> * list = ConvertValue(a0);
		DataTable<qreal> * dat = ConvertValue(a1);
		s->acquire();
		emit setStoichiometry(s,*list,*dat);
		s->acquire();
		s->release();
		delete s;
		delete dat;
		delete list;
	}

	char** StoichiometryTool::_getRates(Array a0)
	{
		return fToS.getRates(a0);
	}

	char** StoichiometryTool_FToS::getRates(Array a0)
	{
		QSemaphore * s = new QSemaphore(1);
		QStringList p;
		s->acquire();
		QList<ItemHandle*> * list = ConvertValue(a0);
		emit getRates(s,&p,*list);
		s->acquire();
		s->release();
		delete s;
		delete list;
		return (char**)ConvertValue(p);
	}

	void StoichiometryTool::_setRates(Array a0,char** a1)
	{
		return fToS.setRates(a0,a1);
	}

	void StoichiometryTool_FToS::setRates(Array a0,char** a1)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*> * list = ConvertValue(a0);
		s->acquire();
		emit setRates(s,*list,ConvertValue(a1));
		s->acquire();
		s->release();
		delete s;
		delete list;
	}

	void StoichiometryTool::keyPressEvent ( QKeyEvent * event )
	{
		if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
		{
			QList<QTableWidgetItem*> selectedItems = ratesTable.selectedItems();
			selectedItems += matrixTable.selectedItems();

			int row = -1, col = -1;
			bool rowSelected = true, colSelected = true;

			for (int i=0; i < selectedItems.size(); ++i)
			{
				if (selectedItems[i])
				{
					if (rowSelected && row != -1 && selectedItems[i]->row() != row)
					{
						rowSelected = false;
					}
					if (colSelected && col != -1 && selectedItems[i]->column() != col)
					{
						colSelected = false;
					}
					row = selectedItems[i]->row();
					col = selectedItems[i]->column();
				}
			}

			if (rowSelected)
				removeRow();
			else
				if (colSelected)
					removeCol();
		}
	}
}

