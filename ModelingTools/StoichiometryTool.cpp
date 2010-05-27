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
#include "NetworkHandle.h"
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
#include "ConnectionsTree.h"
#include "EquationParser.h"
#include "ModuleTool.h"
#include "muParserDef.h"
#include "muParser.h"
#include "muParserInt.h"
#include <QtDebug>

namespace Tinkercell
{

	void StoichiometryTool::select(int)
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		if (dockWidget && dockWidget->widget() != this)
			dockWidget->setWidget(this);
		
		if (tabWidget->count() < 2)
		{
			tabWidget->addTab(ratesBox,tr("Rate equations"));
			tabWidget->addTab(ratePlotWidget,tr("Rate equation"));
			tabWidget->addTab(matrixBox,tr("Stoichiometry"));
		}

		connectionHandles.clear();
		ConnectionHandle * connectionHandle = 0;

		QList<ItemHandle*> list = getHandle(scene->selected());

		for (int i=0; i < list.size(); ++i)
		{
			connectionHandle = ConnectionHandle::cast(list[i]);
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
			connect(mainWindow,SIGNAL(networkClosing(NetworkHandle * , bool *)),this,SLOT(sceneClosing(NetworkHandle * , bool *)));

			connect(mainWindow,SIGNAL(itemsInserted(NetworkHandle*,const QList<ItemHandle*>&)),
				this, SLOT(itemsInserted(NetworkHandle*, const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				this,SLOT(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			connect(mainWindow,SIGNAL(historyChanged(int)),this,SLOT(historyUpdate(int)));

			setWindowTitle(name);
			dockWidget = mainWindow->addToolWindow(this,MainWindow::DockWidget,Qt::BottomDockWidgetArea,Qt::NoDockWidgetArea);
			if (dockWidget)
			{
				dockWidget->setWindowFlags(Qt::Tool);
				dockWidget->setAttribute(Qt::WA_ContentsPropagated);
				dockWidget->setPalette(QPalette(QColor(255,255,255,255)));
				dockWidget->setAutoFillBackground(true);
				//dockWidget->setWindowOpacity(0.9);

				dockWidget->move(mainWindow->geometry().bottomRight() - QPoint((int)(sizeHint().width()*1.5),(int)(sizeHint().height()*1.5)));
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
			if (items[i] && items[i]->type == ConnectionHandle::TYPE)
				connectionHandles += static_cast<ConnectionHandle*>(items[i]);
		updateTable();
		for (int i=0; i < ratesTable.rowCount() && i < updatedRowNames.size(); ++i)
			if (ratesTable.item(i,0) && !ratesTable.item(i,0)->text().isEmpty())
			{
				equations[ updatedRowNames[i] ] = ratesTable.item(i,0)->text();
			}
	}

	void StoichiometryTool::displayModel(QTabWidget& widgets, const QList<ItemHandle*>& , QHash<QString,qreal>& constants, QHash<QString,QString>& )
	{
		if (updatedRowNames.size() > 0)
		{
			for (int i=0; i < updatedColumnNames.size(); ++i)
				if (!constants.contains(updatedColumnNames[i]))
				{
					constants[ updatedColumnNames[i] ] = 1.0;
				}

			if (dockWidget && dockWidget->isVisible())
				dockWidget->hide();

			if (ratesBox->isVisible())
				widgets.insertTab(0,ratesBox,tr("Rate equations"));
			else
				widgets.insertTab(0,ratePlotWidget,tr("Rate equation"));
			widgets.insertTab(1,matrixBox,tr("Stoichiometry matrix"));
		}
		else
		{
			if (dockWidget && dockWidget->widget() != this)
				dockWidget->setWidget(this);

			if (tabWidget->count() < 2)
			{
				tabWidget->addTab(ratesBox,tr("Rate equations"));
				tabWidget->addTab(ratePlotWidget,tr("Rate equation"));
				tabWidget->addTab(matrixBox,tr("Stoichiometry"));
			}
		}
	}

	void StoichiometryTool::historyUpdate(int )
	{
		if (parentWidget() && parentWidget()->isVisible())
			updateTable();
	}

	void StoichiometryTool::connectCFuntions()
	{
		connect(&fToS,SIGNAL(getStoichiometry(QSemaphore*,NumericalDataTable*,const QList<ItemHandle*>&)),this,SLOT(getStoichiometrySlot(QSemaphore*,NumericalDataTable*,const QList<ItemHandle*>&)));
		connect(&fToS,SIGNAL(setStoichiometry(QSemaphore*,QList<ItemHandle*>&,const NumericalDataTable&)),this,SLOT(setStoichiometrySlot(QSemaphore*,QList<ItemHandle*>&,const NumericalDataTable&)));
		connect(&fToS,SIGNAL(getRates(QSemaphore*,QStringList*,const QList<ItemHandle*>&)),this,SLOT(getRatesSlot(QSemaphore*,QStringList*,const QList<ItemHandle*>&)));
		connect(&fToS,SIGNAL(setRates(QSemaphore*,QList<ItemHandle*>&,const QStringList&)),this,SLOT(setRatesSlot(QSemaphore*,QList<ItemHandle*>&,const QStringList&)));
	}

	typedef void (*tc_StoichiometryTool_api)(
		Matrix (*getStoichiometry)(ArrayOfItems ),
		void (*setStoichiometry)(ArrayOfItems ,Matrix ),
		ArrayOfStrings (*getRates)(ArrayOfItems ),
		void (*setRates)(ArrayOfItems ,ArrayOfStrings )
		);

	void StoichiometryTool::setupFunctionPointers( QLibrary * library)
	{
		tc_StoichiometryTool_api f = (tc_StoichiometryTool_api)library->resolve("tc_StoichiometryTool_api");
		if (f)
		{
			f(
				&(_getStoichiometry),
				&(_setStoichiometry),
				&(_getRates),
				&(_setRates)
				);
		}
	}

	void StoichiometryTool::getStoichiometrySlot(QSemaphore * s, NumericalDataTable* p, const QList<ItemHandle*>& items)
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


	void StoichiometryTool::setStoichiometrySlot(QSemaphore * s, QList<ItemHandle*>& items, const NumericalDataTable& table)
	{
		if (mainWindow && mainWindow->currentNetwork())
		{
			setStoichiometry(mainWindow->currentNetwork(),items,table);
		}
		if (s)
			s->release();
	}

	void StoichiometryTool::setRatesSlot(QSemaphore * s, QList<ItemHandle*>& items, const QStringList& rates)
	{
		if (mainWindow && mainWindow->currentNetwork())
			setRates(mainWindow->currentNetwork(),items,rates);
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
				if (connection && connection->handle() && connection->handle()->type == ConnectionHandle::TYPE)
				{
					connectionHandle = static_cast<ConnectionHandle*>(connection->handle());
					if (connectionHandle)
						connectionHandles += connectionHandle;
				}
			}

			updateTable();
		}

		bool reactions = false, species = false;
		ItemHandle * handle;

		for (int i=0; i < list.size(); ++i)
		{
			handle = getHandle(list[i]);
			
			if (ConnectionHandle::cast(handle) && handle->isA(tr("Biochemical")))
				reactions = true;
			
			if (NodeHandle::cast(handle) && handle->isA(tr("Molecule")))
				species = true;
				
			if (reactions && species)
				break;
		}
		
		if (reactions || species)
		{
			if (separator)
				mainWindow->contextItemsMenu.addAction(separator);
			else
				separator = mainWindow->contextItemsMenu.addSeparator();
				
			if (reactions)
				mainWindow->contextItemsMenu.addAction(autoReverse);
			else
				mainWindow->contextItemsMenu.removeAction(autoReverse);
			
			if (species)
				mainWindow->contextItemsMenu.addAction(autoDimer);
			else
				mainWindow->contextItemsMenu.removeAction(autoDimer);
		}
		else
		{
			if (separator)
				mainWindow->contextItemsMenu.removeAction(separator);
			mainWindow->contextItemsMenu.removeAction(autoReverse);
			mainWindow->contextItemsMenu.removeAction(autoDimer);
		}
	}

	void StoichiometryTool::sceneClosing(NetworkHandle * , bool *)
	{
	}

	void StoichiometryTool::itemsInserted(NetworkHandle* win, const QList<ItemHandle*>& handles)
	{
		ConnectionHandle * connectionHandle = 0;
		for (int i=0; i < handles.size(); ++i)
		{
			if (handles[i] && handles[i]->family() && (connectionHandle = ConnectionHandle::cast(handles[i])))
			{
				if (!connectionHandle->tools.contains(this))
					connectionHandle->tools += this;

				if (connectionHandle->data &&
					(
					!(connectionHandle->hasNumericalData(tr("Reactant stoichiometries"))) ||
					!(connectionHandle->hasNumericalData(tr("Product stoichiometries"))) ||
					!(connectionHandle->hasTextData(tr("Rate equations")))
					))
				{
					insertDataMatrix(connectionHandle);
				}
				else  //just for modifier arcs
					if (connectionHandle->data &&
						connectionHandle->hasNumericalData(tr("Reactant stoichiometries")) &&
						connectionHandle->hasNumericalData(tr("Product stoichiometries")) &&
						connectionHandle->hasNumericalData(tr("Numerical Attributes")) &&
						connectionHandle->hasTextData(tr("Rate equations")) &&
						(connectionHandle->nodes().size() > (connectionHandle->nodesIn().size() +connectionHandle->nodesOut().size())) &&
						(connectionHandle->data->textData[tr("Rate equations")].rows() == 1))
					{

						QList<NodeHandle*> nodesIn = connectionHandle->nodesIn(),
							nodesOut = connectionHandle->nodesOut(),
							nodes = connectionHandle->nodes();
						QStringList s1,s2;
						for (int j=0; j < nodesIn.size(); ++j)
							if (nodesIn[j] && !nodesIn[j]->isA(tr("empty")))
								s1 += nodesIn[j]->fullName();
						for (int j=0; j < nodes.size(); ++j)
							if (nodes[j] && !nodesIn.contains(nodes[j]) && !nodesOut.contains(nodes[j]))
								s2 += nodes[j]->fullName();
						QString name = connectionHandle->fullName();

						NumericalDataTable* nDat = new NumericalDataTable(connectionHandle->data->numericalData[tr("Numerical Attributes")]);
						TextDataTable* sDat = new TextDataTable(connectionHandle->data->textData[tr("Rate equations")]);
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
								if (s1.isEmpty())
								{
									sDat->value(0,0) = name + tr(".k0 * ") + s2.join(tr("*"));
								}
								else
								{
									nDat->value(tr("Vmax"),0) = 1.0;
									nDat->value(tr("Km"),0) = 1.0;
									sDat->value(0,0) = name + tr(".Vmax * ") + s2.join(tr("*")) + tr("*") + s1.join(tr("*")) + tr("/(") + name + tr(".Km + ") + s1.join(tr("*")) + tr(")");
								}
								win->changeData(connectionHandle->fullName() + tr("'s kinetics changed"),
												QList<ItemHandle*>() << connectionHandle << connectionHandle,
												QList<QString>() << tr("Numerical Attributes") << tr("Rate equations"),
												QList<NumericalDataTable*>() << nDat,
												QList<TextDataTable*>() << sDat);
								if (console())
                                    console()->message(tr("Rate for ") + name + tr(" = ") + sDat->value(0,0));
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
		NumericalDataTable stoicMatrix;
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

			NumericalDataTable * nDataTable1 = 0, * nDataTable2 = 0;
			QList<NumericalDataTable*> nDataTablesNew, nDataTablesOld;
			QList<ItemHandle*> handles;

			int n2 = 0, j0 = 0;
			for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
			{
				if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
				{
					handles += connectionHandles[i];

					if (connectionHandles[i]->hasNumericalData(tr("Reactant stoichiometries")) && connectionHandles[i]->hasNumericalData(tr("Product stoichiometries")))
					{
						
						nDataTablesOld += &(connectionHandles[i]->data->numericalData[ tr("Reactant stoichiometries") ]);
						nDataTablesOld += &(connectionHandles[i]->data->numericalData[ tr("Product stoichiometries") ]);
						
						nDataTable1 = new NumericalDataTable(connectionHandles[i]->data->numericalData[ tr("Reactant stoichiometries") ]);
						nDataTable2 = new NumericalDataTable(connectionHandles[i]->data->numericalData[ tr("Product stoichiometries") ]);

						for (int k=0; k < nDataTable1->rows() && k < nDataTable2->rows(); ++k)
						{
							for (int j=0; j < nDataTable1->cols(); ++j)     //get unique species
							{
								j0 = stoicMatrix.colNames().indexOf(nDataTable1->colName(j));
								if (j0 >= 0)
									if (stoicMatrix.value(n2,j0) < 0)
										nDataTable1->value(k,j) = -stoicMatrix.value(n2,j0);
									else
										nDataTable1->value(k,j) = 0.0;
							}
							for (int j=0; j < nDataTable2->cols(); ++j)     //get unique species
							{
								j0 = stoicMatrix.colNames().indexOf(nDataTable2->colName(j));
								if (j0 >= 0)
									if (stoicMatrix.value(n2,j0) > 0)
										nDataTable2->value(k,j) = stoicMatrix.value(n2,j0);
									else
										nDataTable2->value(k,j) = 0.0;
							}
							++n2;
						}
						nDataTablesNew += nDataTable1;
						nDataTablesNew += nDataTable2;
					}
				}
			}

			if (currentNetwork() != 0)
				currentNetwork()->changeData(tr("Stoichiometry changed"),handles,nDataTablesOld,nDataTablesNew);

			for (int i=0; i < nDataTablesNew.size(); ++i)
				if (nDataTablesNew[i])
					delete nDataTablesNew[i];
	}

	bool StoichiometryTool::parseRateString(NetworkHandle * win, ItemHandle * handle, QString& s)
	{
		return EquationParser::validate(win, handle, s, QStringList() << "time");
	}

	void StoichiometryTool::setRate(int , int )
	{
		NetworkHandle * win = currentNetwork();
		if (!win) return;

		QStringList rates;

		QTableWidgetItem * tableItem = 0;

		for (int i=0; i < ratesTable.rowCount(); ++i)
		{
			tableItem = ratesTable.item(i,0);
			if (tableItem != 0)
				rates.append(tableItem->text());

		}

		TextDataTable * sDataTable = 0;

		QList< TextDataTable *> sDataTablesNew;
		QList<ItemHandle*> handles;

		int n1 = 0;
		bool change = false;
		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				handles += connectionHandles[i];
				if (connectionHandles[i]->hasTextData(tr("Rate equations")))
				{
					sDataTable = new TextDataTable(connectionHandles[i]->data->textData[ tr("Rate equations") ]);
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

		if (mainWindow != 0 && mainWindow->currentNetwork() != 0)
			mainWindow->currentNetwork()->changeData(tr("Selected kinetics changed"),handles,tr("Rate equations"),sDataTablesNew);

		for (int i=0; i < sDataTablesNew.size(); ++i)
			if (sDataTablesNew[i])
				delete sDataTablesNew[i];
	}


	StoichiometryTool::StoichiometryTool() : Tool(tr("Stoichiometry and Rates"),tr("Modeling")),
		autoReverse(new QAction("Make reversible",this)),
		autoDimer(new QAction("Make dimer",this)),
		separator(0)
	{
		QString appDir = QCoreApplication::applicationDirPath();
		openedByUser = false;
		NodeGraphicsReader reader;
		reader.readXml(&graphics1,appDir + tr("/OtherItems/curve.xml"));
		graphics1.setToolTip(tr("Reaction rates"));
		graphics1.normalize();
		graphics1.scale(40.0/graphics1.sceneBoundingRect().width(),40.0/graphics1.sceneBoundingRect().height());

		reader.readXml(&graphics2,appDir + tr("/OtherItems/curve.xml"));
		graphics2.setToolTip(tr("Reaction stoichiometry"));
		graphics2.normalize();
		graphics2.scale(40.0/graphics2.sceneBoundingRect().width(),40.0/graphics2.sceneBoundingRect().height());
		
		ToolGraphicsItem * toolGraphicsItem = new ToolGraphicsItem(this);
		addGraphicsItem(toolGraphicsItem);
		toolGraphicsItem->addToGroup(&graphics1);
		toolGraphicsItem->setToolTip(tr("Reaction rates"));
		
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

		QToolButton * calc = new QToolButton(this);
		calc->setIcon(QIcon(":/images/calc.png"));
		calc->setToolTip(tr("Get current rates"));

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
		rowButtonLayout->addWidget(calc);
		rowButtonLayout->addStretch(1);
		rowButtonLayout->addWidget(question1);

		connect(addRow,SIGNAL(pressed()),this,SLOT(addRow()));
		connect(removeRow,SIGNAL(pressed()),this,SLOT(removeRow()));
		connect(calc,SIGNAL(pressed()),this,SLOT(eval()));

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

		ratesBox = new QGroupBox(tr(" Reaction rates "),this);
		ratesBox->setMinimumWidth(100);

		matrixBox = new QGroupBox(tr(" Change in values due to each reaction "),this);
		matrixBox->setMinimumWidth(100);
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

		//QSplitter * splitter = new QSplitter;
		tabWidget = new QTabWidget;
		tabWidget->addTab(ratesBox,tr("Rate equations"));
		tabWidget->addTab(matrixBox,tr("Stoichiometry"));
		//layout->addWidget(ratesBox,1);
		//layout->addWidget(matrixBox,2);
		layout->addWidget(tabWidget);
		setLayout(layout);

		connectCFuntions();

		//matrixTable.setItemDelegate(&delegate);

		connect(autoReverse,SIGNAL(triggered()),this,SLOT(addReverseReaction()));
		autoReverse->setIcon(QIcon(":/images/horizontalFlip.png"));
		
		connect(autoDimer,SIGNAL(triggered()),this,SLOT(addDimer()));
		autoDimer->setIcon(QIcon(":/images/plus.png"));
		
		ratePlotWidget = new QWidget(this);
		QVBoxLayout * plotLayout = new QVBoxLayout;
		plotLayout->addWidget(plotWidget = new Plot2DWidget);
		QHBoxLayout * optionsLayout = new QHBoxLayout;
		optionsLayout->addWidget(new QLabel(tr("x-axis:")));
		optionsLayout->addWidget(plotVar = new QComboBox);
		optionsLayout->addWidget(new QLabel(tr("start:")));
		optionsLayout->addWidget(startPlot = new QDoubleSpinBox);
		optionsLayout->addWidget(new QLabel(tr("end:")));
		optionsLayout->addWidget(endPlot = new QDoubleSpinBox);
		endPlot->setValue(10.0);
		plotLayout->addLayout(optionsLayout);
		plotLayout->addWidget(plotLineEdit = new QLineEdit);
		ratePlotWidget->setLayout(plotLayout);
		ratePlotWidget->hide();
	}

	void StoichiometryTool::addReverseReaction()
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		QList<ItemHandle*> items;
		QList<NumericalDataTable*> nDataNew, nDataOld;
		QList<TextDataTable*> sDataNew, sDataOld;
		NumericalDataTable* nDat1, *nDat2;
		TextDataTable* sDat;

		QList<QGraphicsItem*>& selected = scene->selected();
		ItemHandle* handle;
		QStringList cannots;

		for (int i=0; i < selected.size(); ++i)
		{
			if ((handle = getHandle(selected[i])) && handle->data
				&& handle->hasNumericalData(tr("Reactant stoichiometries"))
				&& handle->hasNumericalData(tr("Product stoichiometries"))
				&& handle->hasTextData(tr("Rate equations")))
			{
				items << handle;

				if (handle->data->numericalData[tr("Reactant stoichiometries")].rows() != 1 ||
					handle->data->numericalData[tr("Product stoichiometries")].rows() != 1 ||
					handle->data->textData[tr("Rate equations")].rows() != 1)
				{
					cannots << handle->fullName();
				}
				else
				{
					nDat1 = new NumericalDataTable(handle->data->numericalData[tr("Reactant stoichiometries")]);
					nDat2 = new NumericalDataTable(handle->data->numericalData[tr("Product stoichiometries")]);
					sDat = new TextDataTable(handle->data->textData[tr("Rate equations")]);

					nDat1->rowName(0) = nDat2->rowName(0) = sDat->rowName(0) = tr("forward");
					nDat1->insertRow(1,tr("reverse"));
					nDat2->insertRow(1,tr("reverse"));
					sDat->insertRow(1,tr("reverse"));

					QStringList reactants;
					for (int j=0; j < nDat1->cols(); ++j)
					{
						if (nDat1->value(1,j) > 0)
						{
							if (nDat1->value(1,j) == 1)
								reactants << nDat1->colName(j);
							else
								reactants << ( nDat1->colName(j) + tr("^") + QString::number(nDat1->value(1,j)) );
						}
					}

					if (reactants.isEmpty())
						sDat->value(1,0) = handle->fullName() + tr(".k0");
					else
						sDat->value(1,0) = handle->fullName() + tr(".k0*") + reactants.join(tr("*"));

					nDataNew << nDat2 << nDat1;
					sDataNew << sDat;

					nDataOld << &(handle->data->numericalData[tr("Reactant stoichiometries")]) 
							 << &(handle->data->numericalData[tr("Product stoichiometries")]);
					sDataOld << &(handle->data->textData[tr("Rate equations")]);
				}
			}
		}

		if (nDataNew.size() > 0)
		{
			QUndoCommand * command = new Change2DataCommand<qreal,QString>(tr("Reversible reactions added"),nDataOld,nDataNew,sDataOld,sDataNew);

			if (scene->network)
				scene->network->push(command);
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
			if (console())
                console()->message(tr("Automatic reversibility can only be added to connections with one reaction, so the following were ignored: ") +
            cannots.join(tr(",")));
		}
		else
		{
			QString appDir = QCoreApplication::applicationDirPath();
			QString filename = appDir + tr("/OtherItems/Reversible.xml");
			emit setMiddleBox(1,filename);
		}
	}
	
	void StoichiometryTool::addDimer()
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !mainWindow || !scene->network) return;
		if (!mainWindow->tool(tr("Connections Tree"))) return;

		QWidget * treeWidget = mainWindow->tool(tr("Connections Tree"));
		ConnectionsTree * connectionsTree = static_cast<ConnectionsTree*>(treeWidget);

		if (!connectionsTree->connectionFamilies.contains("Biochemical")) return;

		NodeFamily * nodeFamily = 0;
		ConnectionFamily * connectionFamily = connectionsTree->connectionFamilies["Biochemical"];

		QList<QGraphicsItem*>& selected = scene->selected();
		ItemHandle * handle = 0;

		QList<QGraphicsItem*> list;

		QString appDir = QCoreApplication::applicationDirPath();

		QList<ItemHandle*> visitedHandles;
		
		QStringList newNames;

		for (int i=0; i < selected.size(); ++i)
		{
			handle = getHandle(selected[i]);
			if (NodeGraphicsItem::cast(selected[i]) && handle && handle->isA(tr("Molecule")) && !visitedHandles.contains(handle))
			{
				nodeFamily = NodeFamily::cast(handle->family());
				if (!nodeFamily) continue;

				visitedHandles += handle;
				NodeHandle * node = new NodeHandle(nodeFamily);
				node->name = handle->name + tr("_dimer");
				node->name = scene->network->makeUnique(node->name,newNames);
				newNames << node->name;

				qreal xpos = (selected[i]->sceneBoundingRect().right() + 100.0),
					  ypos = (selected[i]->sceneBoundingRect().top() - 100.0),
					  height = 0.0;

				NodeGraphicsItem * image = 0;

				for (int k=0; k < 2; ++k)
					for (int j=0; j < nodeFamily->graphicsItems.size(); ++j)
					{
						image = (NodeGraphicsItem::topLevelNodeItem(nodeFamily->graphicsItems[j]));
						if (image)
						{
							image = image->clone();

							if (image->defaultSize.width() > 0 && image->defaultSize.height() > 0)
								image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),image->defaultSize.height()/image->sceneBoundingRect().height());

							qreal w = image->sceneBoundingRect().width();

							image->setPos(xpos, ypos);

							image->setBoundingBoxVisible(false);

							if (image->isValid())
							{
								xpos += w;
								setHandle(image,node);
								list += image;
							}
							if (image->sceneBoundingRect().height() > height)
								height = image->sceneBoundingRect().height();
						}
					}
				
				if (image)
				{
					TextGraphicsItem * nameItem;
					QFont font;

					ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
					ConnectionHandle * connection = new ConnectionHandle(connectionFamily,item);

					item->curveSegments +=
						ConnectionGraphicsItem::CurveSegment(1,new ConnectionGraphicsItem::ControlPoint(item,selected[i]));

					item->curveSegments +=
						ConnectionGraphicsItem::CurveSegment(1,new ConnectionGraphicsItem::ControlPoint(item,image));

					ArrowHeadItem * arrow = 0;
					QString nodeImageFile = appDir + tr("/ArrowItems/Reaction.xml");
					NodeGraphicsReader imageReader;
					arrow = new ArrowHeadItem(item);
					imageReader.readXml(arrow,nodeImageFile);
					arrow->normalize();
					double w = 0.1;
					if (arrow->defaultSize.width() > 0 && arrow->defaultSize.height() > 0)
						w = arrow->defaultSize.width()/arrow->sceneBoundingRect().width();
					arrow->scale(w,w);
					item->curveSegments.last().arrowStart = arrow;
					list += arrow;

					connection->name = tr("J_") + node->name + tr("_dimerize");
					item->lineType = ConnectionGraphicsItem::line;
					connection->name = scene->network->makeUnique(connection->name,newNames);
					newNames << connection->name;

					nameItem = new TextGraphicsItem(connection,0);
					list += nameItem;
					nameItem->setPos( 0.5*(image->pos() + selected[i]->scenePos() ) );
					font = nameItem->font();
					font.setPointSize(22);
					nameItem->setFont(font);
					
					nameItem = new TextGraphicsItem(node,0);
					list += nameItem;
					nameItem->setPos( image->sceneBoundingRect().bottomRight() );
					font = nameItem->font();
					font.setPointSize(22);
					nameItem->setFont(font);
					
					//make the rates and stoichiometry table
					
					NumericalDataTable reactants, products;
					TextDataTable rates;
					
					rates.resize(2,1);
					reactants.resize(2,2);
					products.resize(2,2);
					
					rates.colName(0) = tr("rates");
					reactants.rowName(0) = products.rowName(0) = rates.rowName(0) = tr("forwards");
					reactants.rowName(1) = reactants.rowName(1) = rates.rowName(1) = tr("reverse");
					reactants.colName(0) = products.colName(0) = handle->fullName();
					products.colName(1) = reactants.colName(1) = node->fullName();

					reactants.value(0,0) = 2.0;
					products.value(0,1) = 1.0;
					
					reactants.value(1,1) = 1.0;
					products.value(1,0) = 2.0;					
					
					rates.value(0,0) = connection->fullName() + tr(".k0 * ") + handle->fullName() + tr("*") + handle->fullName();
					rates.value(1,0) = connection->fullName() + tr(".k0 * ") + node->fullName();
					
					reactants.description() = QString("Number of each reactant participating in this reaction");
					products.description() = QString("Number of each product participating in this reaction");
					rates.description() = QString("Rates: a set of rates, one for each reaction represented by this item. Row names correspond to reaction names. The number of rows in this table and the stoichiometry table will be the same.");

					connection->data->numericalData.insert(tr("Reactant stoichiometries"),reactants);
					connection->data->numericalData.insert(tr("Product stoichiometries"),products);
					connection->data->textData.insert(tr("Rate equations"),rates);

					list += item;
				}
			}
		}
		if (!list.isEmpty())
		{
			scene->insert(tr("dimer added"),list);
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

	void StoichiometryTool::updateTable()
	{
		updatedColumnNames.clear();
		updatedRowNames.clear();

		if (connectionHandles.size() < 1)
		{
			matrixTable.clearContents();
			ratesTable.clearContents();
			matrixTable.setRowCount(0);
			matrixTable.setColumnCount(0);
			ratesTable.setRowCount(0);
			return;
		}

		QStringList colNames, rowNames, rates;
		NumericalDataTable * nDataTable1 = 0, * nDataTable2 = 0;
		TextDataTable * sDataTable = 0;
		NumericalDataTable combinedTable;

		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				if (connectionHandles[i]->hasNumericalData(tr("Reactant stoichiometries")) &&
					connectionHandles[i]->hasNumericalData(tr("Product stoichiometries")))
				{
					nDataTable1 = &(connectionHandles[i]->data->numericalData[tr("Reactant stoichiometries")]);
					nDataTable2 = &(connectionHandles[i]->data->numericalData[tr("Product stoichiometries")]);
					
					for (int j=0; j < nDataTable1->cols(); ++j) //get unique reactants
						if (!colNames.contains(nDataTable1->colName(j))
							&& !colNames.contains(QString(nDataTable1->colName(j)).replace(".","_"))
							&& !colNames.contains(QString(nDataTable1->colName(j)).replace("_",".")))
							colNames += nDataTable1->colName(j);

					for (int j=0; j < nDataTable2->cols(); ++j) //get unique products
						if (!colNames.contains(nDataTable2->colName(j))
							&& !colNames.contains(QString(nDataTable2->colName(j)).replace(".","_"))
							&& !colNames.contains(QString(nDataTable2->colName(j)).replace("_",".")))
							colNames += nDataTable2->colName(j);
				}

				if (connectionHandles[i]->hasTextData(tr("Rate equations")))
				{
					sDataTable = &(connectionHandles[i]->data->textData[tr("Rate equations")]);
					for (int j=0; j < sDataTable->rows(); ++j) //get rates and reaction names
					{
						if (sDataTable->value(j,0).isEmpty()) continue;

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
				if (connectionHandles[i]->hasNumericalData(tr("Reactant stoichiometries")) &&
					connectionHandles[i]->hasNumericalData(tr("Product stoichiometries")))
				{
					nDataTable1 = &(connectionHandles[i]->data->numericalData[tr("Reactant stoichiometries")]);
					nDataTable2 = &(connectionHandles[i]->data->numericalData[tr("Product stoichiometries")]);

					for (int k=0; k < nDataTable1->rows() && k < nDataTable2->rows(); ++k)
					{
						for (int j=0; j < nDataTable1->cols(); ++j)     //get unique reactants
						{
							j0 = combinedTable.colNames().indexOf(nDataTable1->colName(j));
							if (j0 < 0)
								j0 = combinedTable.colNames().indexOf(QString(nDataTable1->colName(j)).replace(".","_"));
							if (j0 < 0)
								j0 = combinedTable.colNames().indexOf(QString(nDataTable1->colName(j)).replace("_","."));

							if (j0 >= 0)
								combinedTable.value(n,j0) -= nDataTable1->value(k,j);
						}
						
						for (int j=0; j < nDataTable2->cols(); ++j)     //get unique products
						{
							j0 = combinedTable.colNames().indexOf(nDataTable2->colName(j));
							if (j0 < 0)
								j0 = combinedTable.colNames().indexOf(QString(nDataTable2->colName(j)).replace(".","_"));
							if (j0 < 0)
								j0 = combinedTable.colNames().indexOf(QString(nDataTable2->colName(j)).replace("_","."));

							if (j0 >= 0)
								combinedTable.value(n,j0) += nDataTable2->value(k,j);
						}
						++n;
					}
				}

		ratesTable.setRowCount(rowNames.size());
		ratesTable.setColumnCount(1);
		ratesTable.setHorizontalHeaderLabels(QStringList() << "Rate equations");
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
		if (rates.size() == 1 && startPlot && endPlot)
		{
			QList< QPair<QString,qreal> > values;
			QStringList vars = EquationParser::getVariablesInFormula(currentNetwork(),connectionHandles[0],rates[0]);
			
			if (vars.isEmpty())
			{
				ratePlotWidget->setParent(this);
				ratePlotWidget->hide();
				tabWidget->insertTab(0,ratesBox,tr("Rate equations"));
				ratesBox->show();
			}
			else
			{
				console()->message(vars.join(" "));
				ratesBox->setParent(this);
				ratesBox->hide();
				tabWidget->insertTab(0,ratePlotWidget,tr("Rate equation"));
				ratePlotWidget->show();
				NumericalDataTable plot;
				plot.resize(100,2);
			
				values << QPair<QString,qreal>(vars[0],0.0);
			
				plotVar->clear();
				plotVar->addItems(vars);
				qreal min = startPlot->value();
				qreal max = endPlot->value();
				plotLineEdit->setText(rates[0]);
			
				mu::Parser parser;
				bool b = true;
				EquationParser::eval(currentNetwork(),rates[0],&b,values,&parser);
				if (b)
				{
					for (int i=0; i < 100; ++i)
					{
						plot.value(i,0) = values[0].second = i/100.0*(max-min) + min;
						plot.value(i,1) = parser.Eval();
					}
				}
				plot.colName(0) = vars[0];
				plot.colName(1) = connectionHandles[0]->name;
			
				plotWidget->plot(plot,tr("Rate equation"),0);
			}
		}
		else
		{
			ratePlotWidget->setParent(this);
			ratePlotWidget->hide();
			tabWidget->insertTab(0,ratesBox,tr("Rate equations"));
			ratesBox->show();
		}
		
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
		if (lastItem->hasNumericalData(tr("Reactant stoichiometries")) &&
			lastItem->hasNumericalData(tr("Product stoichiometries")) &&
			lastItem->hasTextData(tr("Rate equations")))
		{
			QString rowName;

			NumericalDataTable * nDat1 = &(lastItem->data->numericalData[tr("Reactant stoichiometries")]);
			NumericalDataTable * nDat2 = &(lastItem->data->numericalData[tr("Product stoichiometries")]);
			NumericalDataTable * nDataTable1 = new NumericalDataTable(*nDat1);
			NumericalDataTable * nDataTable2 = new NumericalDataTable(*nDat2);

			QString newName = QInputDialog::getText(this,tr("New Reaction"),tr("Row name (without prefix) :"));

			if (newName.isNull() || newName.isEmpty())
				return;
			else
				rowName = Tinkercell::RemoveDisallowedCharactersFromName(newName);

			if (!nDataTable1->insertRow(nDataTable1->rows(),rowName) || !nDataTable2->insertRow(nDataTable2->rows(),rowName))
			{
				delete nDataTable1;
				delete nDataTable2;
				if (console())
                    console()->error(tr("That row name is already being used."));
				return;
			}

			TextDataTable * sDat = &(lastItem->data->textData[tr("Rate equations")]);
			TextDataTable * sDataTable = new TextDataTable(*sDat);
			if (!sDataTable->insertRow(sDataTable->rows(),rowName))
			{
				delete nDataTable1;
				delete nDataTable2;
				delete sDataTable;
				if (console())
                    console()->error(tr("That row name is already being used."));
				return;
			}

			for (int i=0; i < nDataTable1->cols(); ++i)
				nDataTable1->value(nDataTable1->rows()-1,i) = 0;
			
			for (int i=0; i < nDataTable2->cols(); ++i)
				nDataTable2->value(nDataTable2->rows()-1,i) = 0;

			sDataTable->value(sDataTable->rows()-1,0) = tr("0");


			if (mainWindow && mainWindow->currentNetwork())
			{
				mainWindow->currentNetwork()->changeData(tr("rates row for ") + lastItem->fullName() + tr(" added"),
					QList<ItemHandle*>() << lastItem,
					QList<NumericalDataTable*>() << nDat1 << nDat1,
					QList<NumericalDataTable*>() << nDataTable1 << nDataTable2,
					QList<TextDataTable*>() << sDat,
					QList<TextDataTable*>() << sDataTable);
			}

			delete nDataTable1;
			delete nDataTable2;
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

		NumericalDataTable * nDat1 = 0, * nDat2 = 0;
		TextDataTable * sDat = 0;

		QList< NumericalDataTable* > nDataTablesOld, nDataTablesNew;
		QList< TextDataTable* > sDataTablesOld, sDataTablesNew;
		QList<ItemHandle*> handles;

		int n = 0;

		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			handles += connectionHandles[i];
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				if (connectionHandles[i]->hasNumericalData(tr("Reactant stoichiometries"))
					&& connectionHandles[i]->hasNumericalData(tr("Product stoichiometries"))
					&& connectionHandles[i]->hasTextData(tr("Rate equations")))
				{
					nDat1 = &(connectionHandles[i]->data->numericalData[tr("Reactant stoichiometries")]);
					nDat2 = &(connectionHandles[i]->data->numericalData[tr("Product stoichiometries")]);
					sDat = &(connectionHandles[i]->data->textData[tr("Rate equations")]);

					QList<int> rowsToRemove;
					for (int j=0; j < nDat1->rows() && j < nDat2->rows() && j < sDat->rows(); ++j, ++n)
					{
						for (int k=0; k < selectedItems.size(); ++k)
							if (selectedItems[k] && selectedItems[k]->row() == n)
							{
								rowsToRemove += j;
								break;
							}
					}

					NumericalDataTable * nDataTable1 = new NumericalDataTable(*nDat1),
									   * nDataTable2 = new NumericalDataTable(*nDat2);
					TextDataTable * sDataTable = new TextDataTable(*sDat);

					for (int j=0; j < rowsToRemove.size(); ++j)
					{
						nDataTable1->removeRow(rowsToRemove[j]);
						nDataTable2->removeRow(rowsToRemove[j]);
						sDataTable->removeRow(rowsToRemove[j]);

						for (int k=0; k < rowsToRemove.size(); ++k)
							if (rowsToRemove[k] > rowsToRemove[j])
								rowsToRemove[k] -= 1;
					}

					nDataTablesOld << nDat1 << nDat2;
					sDataTablesOld += sDat;
					nDataTablesNew << nDataTable1 << nDataTable2;
					sDataTablesNew += sDataTable;
				}
			}
		}

		if (nDataTablesNew.size() > 0)
		{
			if (mainWindow != 0 && mainWindow->currentNetwork() != 0)
			{
				mainWindow->currentNetwork()->changeData(tr("Selected reactions removed"), 
					handles,nDataTablesOld,nDataTablesNew,sDataTablesOld,sDataTablesNew);
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

		NumericalDataTable * nDat1 = 0, * nDat2 = 0;

		QList< NumericalDataTable* > nDataTablesNew, nDataTablesOld;

		QString newName = QInputDialog::getText(this,tr("Add Intermediate Species"),tr("Column name (with prefix) :"));

		if (newName.isNull() || newName.isEmpty()) return;

		QString colName = newName;
		QList<ItemHandle*> handles;

		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				if (connectionHandles[i]->hasNumericalData(tr("Reactant stoichiometries")) &&
					connectionHandles[i]->hasNumericalData(tr("Product stoichiometries")))
				{
					handles << connectionHandles[i];
					
					nDat1 = &(connectionHandles[i]->data->numericalData[tr("Reactant stoichiometries")]);
					nDat2 = &(connectionHandles[i]->data->numericalData[tr("Product stoichiometries")]);

					NumericalDataTable * nDataTable1 = new NumericalDataTable(*nDat1),
	 								   * nDataTable2 = new NumericalDataTable(*nDat2);

					nDataTable1->insertCol(nDataTable1->cols(), colName);
					nDataTable2->insertCol(nDataTable2->cols(), colName);

					nDataTablesOld << nDat1 << nDat2;
					nDataTablesNew << nDataTable1 << nDataTable2;
				}
			}
		}

		if (nDataTablesNew.size() > 0)
		{
			if (mainWindow != 0 && mainWindow->currentNetwork() != 0)
			{
				mainWindow->currentNetwork()->changeData(tr("Hidden variables added"), handles, nDataTablesOld, nDataTablesNew);
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

		NumericalDataTable * nDat1 = 0, * nDat2 = 0;
		//TextDataTable * sDataTable = 0;

		QList< NumericalDataTable* > nDataTablesNew, nDataTablesOld;
		//QList< TextDataTable* > sDataTablesNew, sDataTablesOld;

		QList<ItemHandle*> handles;


		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				if (connectionHandles[i]->hasNumericalData(tr("Reactant stoichiometries")) &&
					connectionHandles[i]->hasNumericalData(tr("Product stoichiometries")))
				{
					nDat1 = &(connectionHandles[i]->data->numericalData[tr("Reactant stoichiometries")]);
					nDat2 = &(connectionHandles[i]->data->numericalData[tr("Product stoichiometries")]);

					QList<int> colsToRemove;
					for (int j=0; j < nDat1->cols(); ++j)
					{
						for (int k=0; k < selectedItems.size(); ++k)
							if (selectedItems[k] &&
								matrixTable.horizontalHeaderItem(selectedItems[k]->column())->text()
								== nDat1->colName(j))
							{
								colsToRemove += j;
								break;
							}
					}

					NumericalDataTable * nDataTable1 = new NumericalDataTable(*nDat1);

					for (int j=0; j < colsToRemove.size(); ++j)
					{
						nDataTable1->removeCol(colsToRemove[j]);

						for (int k=0; k < colsToRemove.size(); ++k)
							if (colsToRemove[k] > colsToRemove[j])
								colsToRemove[k] -= 1;
					}
					
					colsToRemove.clear();
					for (int j=0; j < nDat2->cols(); ++j)
					{
						for (int k=0; k < selectedItems.size(); ++k)
							if (selectedItems[k] &&
								matrixTable.horizontalHeaderItem(selectedItems[k]->column())->text()
								== nDat2->colName(j))
							{
								colsToRemove += j;
								break;
							}
					}

					NumericalDataTable * nDataTable2 = new NumericalDataTable(*nDat2);

					for (int j=0; j < colsToRemove.size(); ++j)
					{
						nDataTable2->removeCol(colsToRemove[j]);

						for (int k=0; k < colsToRemove.size(); ++k)
							if (colsToRemove[k] > colsToRemove[j])
								colsToRemove[k] -= 1;
					}

					nDataTablesOld << nDat1 << nDat2;
					nDataTablesNew << nDataTable1 << nDataTable2;
					handles += connectionHandles[i];
				}
			}
		}

		if (nDataTablesNew.size() > 0)
		{
			if (mainWindow != 0 && mainWindow->currentNetwork() != 0)
			{
				mainWindow->currentNetwork()->changeData(tr("stoichiometry columns removed"), handles,nDataTablesOld,nDataTablesNew);
			}

			for (int i=0; i < nDataTablesNew.size(); ++i)
				delete nDataTablesNew[i];
		}

		updateTable();
	}

	//get the stoiciometry of the items and return the matrix
	NumericalDataTable StoichiometryTool::getStoichiometry(const QList<ItemHandle*>& connectionHandles, const QString& replaceDot, bool includeFixed)
	{
		NumericalDataTable combinedTable;

		if (connectionHandles.size() < 1)
		{
			return combinedTable;
		}

		QStringList colNames, rowNames, rates;
		NumericalDataTable * nDataTable1 = 0, * nDataTable2 = 0;
		TextDataTable * sDataTable = 0;
		ConnectionHandle * connection = 0;

		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				QStringList fixedSpecies;
				if (!includeFixed && ConnectionHandle::cast(connectionHandles[i]))
				{
					QList<NodeHandle*> nodes = ConnectionHandle::cast(connectionHandles[i])->nodes();
					for (int j=0; j < nodes.size(); ++j)
						if (NodeHandle::cast(nodes[j])
							&& nodes[j]->hasNumericalData(tr("Fixed"))
							&& nodes[j]->numericalData(tr("Fixed")) > 0)
							fixedSpecies << nodes[j]->fullName();
				}
				if ((connection = ConnectionHandle::cast(connectionHandles[i])) &&
					connection->hasNumericalData(QObject::tr("Reactant stoichiometries")) &&
					connection->hasNumericalData(QObject::tr("Product stoichiometries")))
				{
					nDataTable1 = &(connection->data->numericalData[QObject::tr("Reactant stoichiometries")]);
					nDataTable2 = &(connection->data->numericalData[QObject::tr("Product stoichiometries")]);
					//get unique species names in the stoichiometry matrix
					for (int j=0; j < nDataTable1->cols(); ++j) 
					{
						QString s = nDataTable1->colName(j);

						if (!colNames.contains(s)
							&& !fixedSpecies.contains(s)
							&& !colNames.contains(QString(s).replace(".",replaceDot))
							&& !colNames.contains(QString(s).replace(replaceDot,".")))
						{
							colNames += s;
						}
					}
					for (int j=0; j < nDataTable2->cols(); ++j) 
					{
						QString s = nDataTable2->colName(j);

						if (!colNames.contains(s)
							&& !fixedSpecies.contains(s)
							&& !colNames.contains(QString(s).replace(".",replaceDot))
							&& !colNames.contains(QString(s).replace(replaceDot,".")))
						{
							colNames += s;
						}
					}
					//if any node does not appear in the stoichiometry matrix, add it anyway
					QList<NodeHandle*> connectedNodes = connection->nodes();
					for (int j=0; j < connectedNodes.size(); ++j)
						if (connectedNodes[j] && !connectedNodes[j]->isA(tr("empty")))
						{
							QString s = connectedNodes[j]->fullName();

							if (!colNames.contains(s))
								colNames << s;
						}
				}
				if (connectionHandles[i]->hasTextData(QObject::tr("Rate equations")))
				{
					sDataTable = &(connectionHandles[i]->data->textData[QObject::tr("Rate equations")]);
					for (int j=0; j < sDataTable->rows(); ++j) //get rates and reaction names
					{
						if (sDataTable->value(j,0).isEmpty()) continue;

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
		for (int i=0; i < combinedTable.rows(); ++i)
			for (int j=0; j < combinedTable.cols(); ++j)
				combinedTable.value(i,j) = 0.0;

		for (int i=0; i < colNames.size(); ++i)
			combinedTable.colName(i) = colNames[i];

		for (int i=0; i < rowNames.size(); ++i)
			combinedTable.rowName(i) = rowNames[i];

		int n = 0, j0;
		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
				if (connectionHandles[i]->hasNumericalData(QObject::tr("Reactant stoichiometries")) &&
					connectionHandles[i]->hasNumericalData(QObject::tr("Product stoichiometries")))
				{
					nDataTable1 = &(connectionHandles[i]->data->numericalData[QObject::tr("Reactant stoichiometries")]);
					nDataTable2 = &(connectionHandles[i]->data->numericalData[QObject::tr("Product stoichiometries")]);
					
					for (int k=0; k < nDataTable1->rows() && k < nDataTable2->rows(); ++k)
					{
						for (int j=0; j < nDataTable1->cols(); ++j)     //get unique species
						{
						    QString s = nDataTable1->colName(j);

							j0 = colNames.indexOf(s);
							if (j0 < 0)
								j0 = colNames.indexOf(QString(s).replace(".",replaceDot));
							if (j0 < 0)
								j0 = colNames.indexOf(QString(s).replace(replaceDot,"."));

							if (j0 >= 0)
								combinedTable.value(n,j0) -= nDataTable1->value(k,j);
						}
						for (int j=0; j < nDataTable2->cols(); ++j)     //get unique species
						{
						    QString s = nDataTable2->colName(j);

							j0 = colNames.indexOf(s);
							if (j0 < 0)
								j0 = colNames.indexOf(QString(s).replace(".",replaceDot));
							if (j0 < 0)
								j0 = colNames.indexOf(QString(s).replace(replaceDot,"."));

							if (j0 >= 0)
								combinedTable.value(n,j0) += nDataTable2->value(k,j);
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
	void StoichiometryTool::setStoichiometry(NetworkHandle * win, QList<ItemHandle*>& connectionHandles,const NumericalDataTable& N, const QString& replaceDot)
	{
		NumericalDataTable stoicMatrix = N.transpose();

		if (connectionHandles.size() < 1)
		{
			return;
		}

		NumericalDataTable * nDat1 = 0, * nDat2 = 0;

		QList< NumericalDataTable *> nDataTablesOld, nDataTablesNew;

		if (replaceDot != tr("."))
		{
			QRegExp regex(QString("([A-Za-z0-9])")+replaceDot+QString("([A-Za-z])"));

			for (int i=0; i < stoicMatrix.rows(); ++i)
			{
				stoicMatrix.rowName(i).replace(regex,QString("\\1.\\2"));
			}
			for (int i=0; i < stoicMatrix.cols(); ++i)
			{
				stoicMatrix.colName(i).replace(regex,QString("\\1.\\2"));
			}
		}

		int n=0;
		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				if (connectionHandles[i]->hasNumericalData(QObject::tr("Reactant stoichiometries")) &&
					connectionHandles[i]->hasNumericalData(QObject::tr("Product stoichiometries")))
				{
					nDat1 = new NumericalDataTable(connectionHandles[i]->data->numericalData[ QObject::tr("Reactant stoichiometries") ]);
					nDat2 = new NumericalDataTable(connectionHandles[i]->data->numericalData[ QObject::tr("Product stoichiometries") ]);

					bool last = i == connectionHandles.size() - 1;
					QList<bool> pickCol;
					int cols1 = 0, cols2 = 0, rows = 0;
					for (int j=0; j < stoicMatrix.cols(); ++j)
						pickCol << false;
					int n0 = n;
					int k = 0;

					//determine which cols to pick
					for (k=0; (last || k < nDat1->rows()) && k < nDat2->rows() && n0 < stoicMatrix.rows(); ++k, ++n0, ++rows)
					{
						for (int j=0; j < stoicMatrix.cols(); ++j)
						{
							if (stoicMatrix.at(n0,j) != 0)
							{
								if (!pickCol[j]) 
									if (stoicMatrix.at(n0,j) < 0)
										++cols1;
									else
										++cols2;
								pickCol[j] = true;
							}
						}
					}

					if (nDat1->rows() != rows || nDat1->cols() !=  cols1)
						nDat1->resize(rows,cols1);
					
					if (nDat2->rows() != rows || nDat2->cols() !=  cols2)
						nDat2->resize(rows,cols1);

					for (k=0; k < nDat1->rows() && n < stoicMatrix.rows(); ++k, ++n)
					{
						int j1 = 0, j2 = 0;
						for (int j=0; j < stoicMatrix.cols(); ++j)
							if (pickCol[j])
							{
								if (stoicMatrix.at(n,j) < 0)
								{
									nDat1->colName(j1) = stoicMatrix.colName(j);
									nDat1->value(k,j1) = stoicMatrix.at(n,j);
									++j1;
								}
								else
								{
									nDat2->colName(j2) = stoicMatrix.colName(j);
									nDat2->value(k,j2) = stoicMatrix.at(n,j);
									++j2;
								}
							}
					}

					nDataTablesOld << &(connectionHandles[i]->data->numericalData[ QObject::tr("Reactant stoichiometries") ])
								   << &(connectionHandles[i]->data->numericalData[ QObject::tr("Product stoichiometries") ]);
					nDataTablesNew << nDat1 << nDat2;
				}
			}
		}

		if (win)
		{
			win->changeData(tr("stoichiometry changed"),connectionHandles,nDataTablesOld,nDataTablesNew);
		}

		for (int i=0; i < nDataTablesNew.size(); ++i)
			if (nDataTablesNew[i])
				delete nDataTablesNew[i];
	}

	//get the rates of the given items
	QStringList StoichiometryTool::getRates(const QList<ItemHandle*>& connectionHandles, const QString& replaceDot)
	{
		QStringList rates;
		TextDataTable * sDataTable = 0;

		if (connectionHandles.size() < 1)
		{
			return rates;
		}

		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
			    if (connectionHandles[i]->hasTextData(QObject::tr("Rate equations")))
				{
					sDataTable = &(connectionHandles[i]->data->textData[QObject::tr("Rate equations")]);
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
			rates[i].replace(regex,replaceDot);
		}
		return rates;
	}

	//set the rates of the given items
	void StoichiometryTool::setRates(NetworkHandle * win, QList<ItemHandle*>& connectionHandles,const QStringList& list, const QString& replaceDot)
	{
		if (connectionHandles.size() < 1)
		{
			return;
		}

		TextDataTable * sDataTable = 0;

		QList< TextDataTable *> sDataTablesNew;

		QList<ItemHandle*> handles;

		QRegExp regex(QString("([A-Za-z0-9])") + replaceDot + QString("([A-Za-z])"));

		int n=0;

		bool change = false;
		for (int i=0; i < connectionHandles.size(); ++i) //build combined matrix for all selected reactions
		{
			if (connectionHandles[i] != 0 && connectionHandles[i]->data != 0)
			{
				if (connectionHandles[i]->hasTextData(QObject::tr("Rate equations")))
				{
					//nDataTable = new NumericalDataTable(connectionHandles[i]->data->numericalData[ thisName ]);
					sDataTable = new TextDataTable(connectionHandles[i]->data->textData[ QObject::tr("Rate equations") ]);

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
			win->changeData(tr("Rate equations changed"),handles,QObject::tr("Rate equations"),sDataTablesNew);
		}

		for (int i=0; i < sDataTablesNew.size(); ++i)
			if (sDataTablesNew[i])
				delete sDataTablesNew[i];
	}


	/************************************************************/

	StoichiometryTool_FToS StoichiometryTool::fToS;

	Matrix StoichiometryTool::_getStoichiometry(ArrayOfItems a0)
	{
		return fToS.getStoichiometry(a0);
	}

	Matrix StoichiometryTool_FToS::getStoichiometry(ArrayOfItems a0)
	{
		QList<ItemHandle*> * list = ConvertValue(a0);
		QSemaphore * s = new QSemaphore(1);
		NumericalDataTable * p = new NumericalDataTable;
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

	void StoichiometryTool::_setStoichiometry(ArrayOfItems a0,Matrix a1)
	{
		return fToS.setStoichiometry(a0,a1);
	}

	void StoichiometryTool_FToS::setStoichiometry(ArrayOfItems a0,Matrix a1)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*> * list = ConvertValue(a0);
		NumericalDataTable * dat = ConvertValue(a1);
		s->acquire();
		emit setStoichiometry(s,*list,*dat);
		s->acquire();
		s->release();
		delete s;
		delete dat;
		delete list;
	}

	ArrayOfStrings StoichiometryTool::_getRates(ArrayOfItems a0)
	{
		return fToS.getRates(a0);
	}

	ArrayOfStrings StoichiometryTool_FToS::getRates(ArrayOfItems a0)
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
		return (ArrayOfStrings)ConvertValue(p);
	}

	void StoichiometryTool::_setRates(ArrayOfItems a0,ArrayOfStrings a1)
	{
		return fToS.setRates(a0,a1);
	}

	void StoichiometryTool_FToS::setRates(ArrayOfItems a0,ArrayOfStrings a1)
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

	void StoichiometryTool::eval()
	{
		bool b;
		QStringList values;
		for (int i=0; i < connectionHandles.size() && i < updatedRowNames.size(); ++i)
		{
			if (connectionHandles[i] && connectionHandles[i]->hasTextData(tr("Rate equations")))
			{
				TextDataTable rates = connectionHandles[i]->data->textData[tr("Rate equations")];
				if (rates.cols() == 1)
					for (int j=0; j < rates.rows(); ++j)
					{
						QString s = rates.value(j,0);
						double d = EquationParser::eval(currentNetwork(), s, &b);
						if (b)
							if (rates.rowName(j).isEmpty() || rates.rows() == 1)
								values << connectionHandles[i]->fullName() + tr(" = ") + QString::number(d);
							else
								values << connectionHandles[i]->fullName() + tr(".") + rates.rowName(j) + tr(" = ") + QString::number(d);

					}
			}
		}
		if (values.size() > 0)
			if (console())
                console()->message(values.join(tr("\n")));
	}
}

