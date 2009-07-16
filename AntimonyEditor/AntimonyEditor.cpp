/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 Provides a text window where C code can be written and run dynamically

****************************************************************************/

#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "OutputWindow.h"
#include "NodesTree.h"
#include "ConnectionsTree.h"
#include "StoichiometryTool.h"
#include "BasicInformationTool.h"
#include "PythonTool.h"
#include "AntimonyEditor.h"
#include <QToolButton>
#include <QFile>
#include <QRegExp>
#include <QtDebug>
#include <QSemaphore>

//Antimony headers
#include "antimony_api.h"

namespace Tinkercell
{
	AntimonyEditor::AntimonyEditor() : Tool(QString("Antimony editor"))
	{
		dockWidget = 0;
		editor = new QTextEdit(this);
		QVBoxLayout * layout = new QVBoxLayout;

		layout->addWidget(editor,10);

		QHBoxLayout * buttonsLayout = new QHBoxLayout;

		QPushButton * newButton = new QPushButton(this);
		newButton->setText(tr("&Create New"));
		connect(newButton,SIGNAL(released()),this,SLOT(makeNew()));
		buttonsLayout->addWidget(newButton,1);

		QPushButton * updateButton = new QPushButton(this);
		updateButton->setText(tr("&Update Script"));
		connect(updateButton,SIGNAL(released()),this,SLOT(update()));
		buttonsLayout->addWidget(updateButton,1);

		QPushButton * commitButton = new QPushButton(this);
		commitButton->setText(tr("&Commit Changes"));
		connect(commitButton,SIGNAL(released()),this,SLOT(commit()));
		buttonsLayout->addWidget(commitButton,1);

		layout->addLayout(buttonsLayout,1);
		setLayout(layout);

		highlighter = new AntimonySyntaxHighlighter(editor->document());
		editor->clear();

		QFont font;
		font.setFamily("Courier");
		font.setFixedPitch(true);
		font.setPointSize(10);
		editor->setFont(font);
	}

	bool AntimonyEditor::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			dockWidget = mainWindow->addDockingWindow(name,this, Qt::BottomDockWidgetArea, Qt::BottomDockWidgetArea);
			dockWidget->setAttribute(Qt::WA_ContentsPropagated);
			dockWidget->setFloating(true);
			dockWidget->move(mainWindow->geometry().bottomRight() - QPoint(sizeHint().width()*2,sizeHint().height()));
			dockWidget->hide();
// 			QToolBar * toolBar = mainWindow->secondToolBar;
// 			QAction * action = new QAction(tr("Antimony editor"),toolBar);
// 			action->setIcon(QIcon(tr(":/images/antimony.png")));
			//action->setCheckable(true);
// 			connect(action,SIGNAL(triggered()),dockWidget,SLOT(show()));
// 			toolBar->addAction(action);

			connect(mainWindow,SIGNAL(pluginLoaded(const QString&)),this,SLOT(pluginLoaded(const QString&)));

			pluginLoaded(QString());
		}
		return false;
	}

	void AntimonyEditor::pluginLoaded(const QString&)
	{
		static bool alreadyConnected = false;

		if (alreadyConnected) return;

// 		QWidget * widget = mainWindow->tools[tr("Python Interpreter")];
// 		if (widget)
// 		{
// 			PythonTool * pyTool = static_cast<PythonTool*>(widget);
// 			connect(this,SIGNAL(runPyFile(const QString&)),pyTool,SLOT(runPythonFile(const QString&)));
// 			connect(this,SIGNAL(runPyString(const QString&)),pyTool,SLOT(runPythonCode(const QString&)));
// 			alreadyConnected = true;
// 		}
	}

	void AntimonyEditor::makeNew()
	{
		GraphicsScene * scene = currentScene();
		if (!editor || !scene) return;

		QString modelString = editor->toPlainText() + tr("\n");

		QString filename = MainWindow::userHome() + tr("/Tinkercell/antimony.txt");

		QFile antfile(filename);
		if (!antfile.open(QFile::WriteOnly | QFile::Text)) {
			OutputWindow::error(tr("Could not write to the user directory. Check whether you have permission to write to the specified TinkerCell user directory."));
			return;
		}
		antfile.write(modelString.toAscii().data());
		antfile.close();

		//parse
		//long ok = loadFile(filename.toAscii().data());

		//if (ok < 0)
		//{
			// //OutputWindow::error(tr(getLastError()));
			//return;
		//}

		QString appDir = QCoreApplication::applicationDirPath();
  		#ifdef Q_WS_MAC
		appDir += tr("/../../..");
		#endif

		ConnectionFamily * biochemicalFamily = 0;
// 		PartFamily * speciesFamily = 0;

// 		if (mainWindow->tools.contains(tr("Parts Tree")) && mainWindow->tools.contains(tr("Connections Tree")))
// 		{
// 			PartsTree * partsTree = static_cast<PartsTree*>(mainWindow->tools.value(tr("Parts Tree")));
// 			ConnectionsTree * connectionsTree = static_cast<ConnectionsTree*>(mainWindow->tools.value(tr("Connections Tree")));
// 			biochemicalFamily = connectionsTree->connectionFamilies.value(tr("Biochemical"));
// 			speciesFamily = partsTree->partFamilies.value(tr("Species"));
// 		}

// 		if (!biochemicalFamily || !speciesFamily || speciesFamily->graphicsItems.isEmpty()
// 			|| !qgraphicsitem_cast<PartGraphicsItem*>(speciesFamily->graphicsItems[0]))
// 		{
// 			return;
// 		}

		//load


		// int nummods = (int)getNumModules();
		// char** modnames = getModuleNames();
/*
		for (int i=0; i < nummods; ++i)
		{
			QHash<QString,NodeGraphicsItem*> speciesItems;
			QList<QGraphicsItem*> itemsToInsert;

		  char * moduleName = modnames[i];
			char ***leftrxnnames = getReactantNames(moduleName);
			char ***rightrxnnames = getProductNames(moduleName);
			char **rxnnames = getReactionNames(moduleName);
			char **rxnrates = getReactionRates(moduleName);

			double **leftrxnstoichs = getReactantStoichiometries(moduleName);
			double **rightrxnstoichs = getProductStoichiometries(moduleName);

			int numrxn = getNumReactions(moduleName);

			for (int rxn=0; rxn < numrxn; rxn++)
			{
				//int numReactants = getNumReactants(moduleName,rxn);
				int numProducts = getNumProducts(moduleName,rxn);

				ConnectionGraphicsItem * reactionItem = new ConnectionGraphicsItem;

				DataTable<qreal> stoichiometry;
				DataTable<QString> rate;

				stoichiometry.resize(1,numReactants + numProducts);
				rate.resize(1,1);
				ItemHandle * reactionHandle = new ConnectionHandle(biochemicalFamily,reactionItem);

				reactionHandle->name = rxnnames[rxn];

				for (int var=0; var<numReactants; var++)
				{
					NodeGraphicsItem * speciesItem = 0;
					if (!speciesItems.contains(tr(leftrxnnames[rxn][var])))
					{
// 						speciesItem = (static_cast<NodeGraphicsItem*>(speciesFamily->graphicsItems[0]))->clone();
// 						//scale to default size
// 						if (speciesItem->defaultSize.width() > 0 && speciesItem->defaultSize.height() > 0)
// 								speciesItem->scale(speciesItem->defaultSize.width()/speciesItem->sceneBoundingRect().width(),speciesItem->defaultSize.height()/speciesItem->sceneBoundingRect().height());
//
// 						ItemHandle * partHandle = new PartHandle(speciesFamily,speciesItem);
// 						partHandle->name = tr(leftrxnnames[rxn][var]);
// 						speciesItems[tr(leftrxnnames[rxn][var])] = speciesItem;
// 						itemsToInsert += speciesItem;
//
// 						TextGraphicsItem * nameItem = new TextGraphicsItem(partHandle,0);
// 						itemsToInsert += nameItem;
// 						nameItem->setPos(speciesItem->scenePos());
// 						QFont font = nameItem->font();
// 						font.setPointSize(22);
// 						nameItem->setFont(font);
					}
					else
					{
						speciesItem = speciesItems[tr(leftrxnnames[rxn][var])];
					}

					reactionItem->pathVectors +=
						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(reactionItem,speciesItem));

					stoichiometry.colName(var) = leftrxnnames[rxn][var];
					stoichiometry.value(0,var) = - leftrxnstoichs[rxn][var];
				}

				for (int var=0; var<numProducts; var++)
				{
					NodeGraphicsItem * speciesItem = 0;
					if (!speciesItems.contains(tr(rightrxnnames[rxn][var])))
					{
// 						speciesItem = (static_cast<NodeGraphicsItem*>(speciesFamily->graphicsItems[0]))->clone();
// 						//scale to default
// 						if (speciesItem->defaultSize.width() > 0 && speciesItem->defaultSize.height() > 0)
// 								speciesItem->scale(speciesItem->defaultSize.width()/speciesItem->sceneBoundingRect().width(),speciesItem->defaultSize.height()/speciesItem->sceneBoundingRect().height());
//
// 						ItemHandle * partHandle = new PartHandle(speciesFamily,speciesItem);
// 						partHandle->name = tr(rightrxnnames[rxn][var]);
// 						speciesItems[tr(rightrxnnames[rxn][var])] = speciesItem;
// 						itemsToInsert += speciesItem;
//
// 						TextGraphicsItem * nameItem = new TextGraphicsItem(partHandle,0);
// 						itemsToInsert += nameItem;
// 						nameItem->setPos(speciesItem->scenePos());
// 						QFont font = nameItem->font();
// 						font.setPointSize(22);
// 						nameItem->setFont(font);
					}
					else
					{
						speciesItem = speciesItems[tr(rightrxnnames[rxn][var])];
					}

					reactionItem->pathVectors +=
						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(reactionItem,speciesItem));

					ArrowHeadItem * arrow = 0;
					if (!biochemicalFamily->graphicsItems.isEmpty() &&
						(arrow = qgraphicsitem_cast<ArrowHeadItem*>(biochemicalFamily->graphicsItems.last())) &&
						arrow->isValid())
					{
						arrow = new ArrowHeadItem(*arrow);
						arrow->connectionItem = reactionItem;
						arrow->scale(0.1,0.1);
					}
					else
					{
						QString partImageFile = appDir + tr("/ArrowItems/Reaction.xml");
						NodeGraphicsReader imageReader;
						arrow = new ArrowHeadItem(reactionItem);
						imageReader.readXml(arrow,partImageFile);
						arrow->normalize();
						arrow->scale(0.1,0.1);
					}
					reactionItem->pathVectors.last().arrowStart = arrow;
					itemsToInsert += arrow;

					stoichiometry.colName(var+numReactants) = rightrxnnames[rxn][var];
					stoichiometry.value(0,var+numReactants) += rightrxnstoichs[rxn][var];
				}

				QString srate = tr(rxnrates[rxn]);
				StoichiometryTool::parseRateString(scene, reactionHandle, srate);
				rate.value(0,0) = srate;
				reactionHandle->data->textData[tr("Rates")] = rate;
				reactionHandle->data->numericalData[tr("Stoichiometry")] = stoichiometry;


				TextGraphicsItem * nameItem2 = new TextGraphicsItem(reactionHandle,0);
				itemsToInsert += nameItem2;
				nameItem2->setPos(reactionItem->scenePos());
				QFont font = nameItem2->font();
				font.setPointSize(22);
				nameItem2->setFont(font);

				itemsToInsert += reactionItem;
			}

			if (!itemsToInsert.isEmpty())
			{
// 				mainWindow->newWindow();
				GraphicsScene * scene;

				if ((scene = currentScene()))
				{
					scene->insert(tr("Antimony file loaded"), itemsToInsert);

					emit runPyString(tr("nxAutoLayout.doLayout('spring');"));

					if (scene->historyStack)
						scene->historyStack->clear();
				}
			}
		}*/


		//freeAll();
	}

	void AntimonyEditor::update()
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

// 		QList<ItemHandle*> handles = scene->symbolsTable.handlesFullName.values();
//
// 		QStringList model;
//
// 		model << "model main()\n";
//
// 		QString replaceDot = tr("_");
//
// 		QStringList rates = StoichiometryTool::getRates(handles, replaceDot);
// 		int i,j;
//
// 		for (i=0; i < rates.size(); ++i)
// 		{
// 			if (rates[i].isEmpty())
// 				rates[i] = QString("0.0");
// 		}
//
// 		DataTable<qreal> N = StoichiometryTool::getStoichiometry(handles,replaceDot);
// 		if (N.rows() < 1 || N.cols() < 1 || rates.isEmpty()) return;
//
// 		DataTable<qreal> params = BasicInformationTool::getParameters(handles,QStringList(), QStringList(), replaceDot);
//
// 		QStringList expressions, assignments;
//
// 		QRegExp regex(tr("\\.(?!\\d)"));
// 		QString s1,s2;
// 		QStringList vars;
// 		QStringList initValues;
//
// 		for (i = 0; i < N.cols() && i < rates.size(); ++i)
// 		{
// 			QStringList lhs,rhs;
// 			model << "   " << N.colName(i) << ": ";
// 			for (j=0; j < N.rows(); ++j)
// 			{
// 				if (N.value(j,i) < 0)
// 					lhs << N.rowName(j);
// 				else
// 				if (N.value(j,i) > 0)
// 					rhs << N.rowName(j);
// 			}
// 			model << lhs.join("+") << " -> " << rhs.join("+") << "; " << rates[i] << ";\n";
// 		}
//
//
// 		QStringList assignmentRules, events;
// 		for (i=0; i < handles.size(); ++i)
// 		{
// 			if (handles[i] && handles[i]->family())
// 			{
// 				if (handles[i]->data)
// 				{
// 					if (handles[i]->data->numericalData.contains(tr("Initial Value")))
// 					{
// 						model << tr("   ") + handles[i]->fullName(replaceDot) + tr(" = ") +
// 									QString::number(handles[i]->data->numericalData[tr("Initial Value")].value(0,0)) + tr(";\n");
// 					}
// 					if (handles[i]->data->numericalData.contains(tr("Fixed")) &&
// 						handles[i]->data->numericalData[tr("Fixed")].at(0,0) > 0)
// 					{
// 						model << "const " + handles[i]->fullName(replaceDot) << "\n";
// 					}
// 				}
// 				if (handles[i]->data && handles[i]->data->textData.contains(tr("Events")))
// 				{
// 					DataTable<QString>& dat = handles[i]->data->textData[tr("Events")];
// 					if (dat.cols() == 1)
// 						for (j=0; j < dat.rows(); ++j)
// 						{
// 							s1 =  dat.rowName(j);
// 							s1.replace(regex,replaceDot);
// 							s2 =  dat.value(j,0);
//
// 							s2.replace(regex,replaceDot);
//
// 							if (!s1.isEmpty() && !s2.isEmpty())
// 							{
// 								model << tr("   at (") + s1 + tr(") ") + s2 + tr(";\n");
// 								events << s2;
// 							}
// 						}
// 				}
// 				if (handles[i]->data && handles[i]->data->textData.contains(tr("Assignments")))
// 				{
// 					DataTable<QString>& dat = handles[i]->data->textData[tr("Assignments")];
// 					if (dat.cols() == 1)
// 						for (j=0; j < dat.rows(); ++j)
// 						{
// 							s1 =  dat.rowName(j);
// 							s2 =  dat.value(j,0);
//
// 							s2.replace(regex,replaceDot);
//
// 							if (s1.isEmpty() || s2.isEmpty()) continue;
//
// 							assignmentRules << s2;
//
// 							if (s1.isEmpty() || s1 == handles[i]->fullName() || s1 == handles[i]->fullName(replaceDot))
// 							{
// 								model << tr("   ") + handles[i]->fullName(replaceDot) + tr(" = ") + s2 + tr(";\n");
// 							}
// 							else
// 								if (handles[i]->data->numericalData.contains(tr("Numerical Attributes")) &&
// 								handles[i]->data->numericalData["Numerical Attributes"].rowNames().contains(dat.rowName(j)))
// 								{
// 									model << tr("   ") + handles[i]->fullName(replaceDot) + replaceDot + s1 + tr(" = ") + s2 + tr(";\n");
// 								}
// 								else
// 								{
// 									model << tr("   ") + handles[i]->fullName(replaceDot) + replaceDot + s1 + tr(" = ") + s2 + tr(";\n");
// 								}
//
// 						}
// 				}
// 			}
// 		}
// 		for (i = 0; i < params.rows(); ++i)
//         {
// 		    bool used = false;
// 		    for (j=0; j < rates.size(); ++j)
// 			{
// 			    if (rates[j].contains(params.rowName(i)))
// 				{
// 					used = true;
// 					break;
// 				}
// 			}
//
// 			if (!used)
// 				for (j=0; j < assignmentRules.size(); ++j)
// 				{
// 					if (assignmentRules[j].contains(params.rowName(i)))
// 					{
// 						used = true;
// 						break;
// 					}
// 				}
//
// 			if (!used)
// 				for (j=0; j < events.size(); ++j)
// 				{
// 					if (events[j].contains(params.rowName(i)))
// 					{
// 						used = true;
// 						break;
// 					}
// 				}
//
// 			if (used)
// 				model << "   " << params.rowName(i) << " = " << QString::number(params.at(i,0)) << ";\n";
//         }
//
// 		model << "end\n";
//
// 		if (editor)
// 			editor->setPlainText(model.join(""));
	}

	void AntimonyEditor::commit()
	{
		if (!editor || !mainWindow) return;
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		QString modelString = editor->toPlainText() + tr("\n");

		QString filename = MainWindow::userHome() + tr("/Tinkercell/antimony.txt");

		QFile antfile(filename);
		if (!antfile.open(QFile::WriteOnly | QFile::Text)) {
			OutputWindow::error(tr("Could not write to the user directory. Check whether you have permission to write to the specified TinkerCell user directory."));
			return;
		}
		antfile.write(modelString.toAscii().data());
		antfile.close();

		//parse
		/*long ok = loadFile(filename.toAscii().data());

		if (ok < 0)
		{
			//OutputWindow::error(tr(getLastError()));
			return;
		}*/

		QString appDir = QCoreApplication::applicationDirPath();
  		#ifdef Q_WS_MAC
		appDir += tr("/../../..");
		#endif

		ConnectionFamily * biochemicalFamily = 0;
		NodeFamily * speciesFamily = 0;

// 		if (mainWindow->tools.contains(tr("Parts Tree")) && mainWindow->tools.contains(tr("Connections Tree")))
// 		{
// 			PartsTree * partsTree = static_cast<PartsTree*>(mainWindow->tools.value(tr("Parts Tree")));
// 			ConnectionsTree * connectionsTree = static_cast<ConnectionsTree*>(mainWindow->tools.value(tr("Connections Tree")));
// 			biochemicalFamily = connectionsTree->connectionFamilies.value(tr("Biochemical"));
// 			speciesFamily = partsTree->partFamilies.value(tr("Species"));
// 		}

		//load


		// int nummods = (int)getNumModules();
		//char** modnames = getModuleNames();
		QString rxnName;

// 		QHash<QString,PartGraphicsItem*> speciesItems;
// 		QList<QGraphicsItem*> itemsToInsert;
//
// 		QList<ItemHandle*> allHandles = scene->symbolsTable.handlesFullName.values();
// 		for (int i=0; i < allHandles.size(); ++i)
// 		{
// 			if (allHandles[i] && allHandles[i]->type == PartHandle::Type)
// 				for (int j=0; j < allHandles[i]->graphicsItems.size(); ++j)
// 					if (qgraphicsitem_cast<PartGraphicsItem*>(allHandles[i]->graphicsItems[j]))
// 					{
// 						speciesItems.insert(allHandles[i]->fullName(), qgraphicsitem_cast<PartGraphicsItem*>(allHandles[i]->graphicsItems[j]));
// 						break;
// 					}
// 		}
//
// 		QList<QUndoCommand*> commands;
//
// 		for (int i=0; i < nummods; ++i)
// 		{
// 			char * moduleName = modnames[i];
// 			char ***leftrxnnames = getReactantNames(moduleName);
// 			char ***rightrxnnames = getProductNames(moduleName);
// 			char **rxnnames = getReactionNames(moduleName);
// 			char **rxnrates = getReactionRates(moduleName);
//
// 			double **leftrxnstoichs = getReactantStoichiometries(moduleName);
// 			double **rightrxnstoichs = getProductStoichiometries(moduleName);
//
// 			int numrxn = getNumReactions(moduleName);
//
// 			for (int rxn=0; rxn < numrxn; rxn++)
// 			{
// 				int numReactants = getNumReactants(moduleName,rxn);
// 				int numProducts = getNumProducts(moduleName,rxn);
// 				bool exists = false;
//
// 				rxnName = rxnnames[rxn];
//
// 				ConnectionGraphicsItem * reactionItem = 0;
// 				ItemHandle * reactionHandle = 0;
// 				DataTable<qreal> stoichiometry;
// 				DataTable<QString> rate;
//
// 				if (scene->symbolsTable.handlesFullName.contains(rxnName))
// 				{
// 					if (scene->symbolsTable.handlesFullName[rxnName]->type == ConnectionHandle::Type)
// 					{
// 						reactionHandle = static_cast<ConnectionHandle*>(scene->symbolsTable.handlesFullName[rxnName]);
// 					}
// 				}
// 				else
// 				if (scene->symbolsTable.dataRowsAndCols.contains(rxnName))
// 				{
// 					if (scene->symbolsTable.dataRowsAndCols[rxnName].first &&
// 						scene->symbolsTable.dataRowsAndCols[rxnName].first->type == ConnectionHandle::Type)
// 					{
// 						reactionHandle = static_cast<ConnectionHandle*>(scene->symbolsTable.dataRowsAndCols[rxnName].first);
// 					}
// 				}
// 				else
// 				{
// 					rxnName.replace(tr("_"),tr("."));
// 					if (scene->symbolsTable.handlesFullName.contains(rxnName))
// 					{
// 						if (scene->symbolsTable.handlesFullName[rxnName]->type == ConnectionHandle::Type)
// 							reactionHandle = static_cast<ConnectionHandle*>(scene->symbolsTable.handlesFullName[rxnName]);
// 						if (reactionHandle->data && reactionHandle->data->numericalData.contains(tr("Stoichiometry")))
// 						{
// 							stoichiometry = reactionHandle->data->numericalData[tr("Stoichiometry")];
// 						}
// 						if (reactionHandle->data && reactionHandle->data->textData.contains(tr("Rates")))
// 						{
// 							rate = reactionHandle->data->textData[tr("Rates")];
// 						}
// 					}
// 					else
// 					if (scene->symbolsTable.dataRowsAndCols.contains(rxnName))
// 					{
// 						if (scene->symbolsTable.dataRowsAndCols[rxnName].first &&
// 							scene->symbolsTable.dataRowsAndCols[rxnName].first->type == ConnectionHandle::Type)
// 						{
// 							reactionHandle = static_cast<ConnectionHandle*>(scene->symbolsTable.dataRowsAndCols[rxnName].first);
// 						}
// 					}
// 					else
// 					{
// 						rxnName = rxnnames[rxn];
// 					}
// 				}
//
// 				if (reactionHandle)
// 				{
// 					exists = true;
// 					if (reactionHandle->data && reactionHandle->data->numericalData.contains(tr("Stoichiometry")))
// 					{
// 						stoichiometry = reactionHandle->data->numericalData[tr("Stoichiometry")];
// 					}
// 					if (reactionHandle->data && reactionHandle->data->textData.contains(tr("Rates")))
// 					{
// 						rate = reactionHandle->data->textData[tr("Rates")];
// 					}
// 				}
// 				else
// 				{
// 					reactionItem = new ConnectionGraphicsItem;
// 					reactionHandle = new ConnectionHandle(biochemicalFamily,reactionItem);
// 					reactionHandle->name = rxnName;
// 					stoichiometry.resize(1,numReactants + numProducts);
// 					rate.resize(1,1);
// 				}
//
// 				QString srate = tr(rxnrates[rxn]);
// 				StoichiometryTool::parseRateString(scene, reactionHandle, srate);
//
// 				if (rate.rows() == 1)
// 					rate.value(0,0) = srate;
// 				else
// 					if (rate.getRowNames().contains(rxnName))
// 					{
// 						rate.value( rate.getRowNames().indexOf(rxnName), 0 ) = srate;
// 					}
// 					else
// 					{
// 						rate.value(0,0) = srate;
// 					}
//
// 				for (int var=0; var<numReactants; var++)
// 				{
// 					PartGraphicsItem * speciesItem = 0;
// 					if (!speciesItems.contains(tr(leftrxnnames[rxn][var])))
// 					{
// 						speciesItem = (static_cast<PartGraphicsItem*>(speciesFamily->graphicsItems[0]))->clone();
// 						//scale to default size
// 						if (speciesItem->defaultSize.width() > 0 && speciesItem->defaultSize.height() > 0)
// 							speciesItem->scale(speciesItem->defaultSize.width()/speciesItem->sceneBoundingRect().width(),speciesItem->defaultSize.height()/speciesItem->sceneBoundingRect().height());
//
// 						ItemHandle * partHandle = new PartHandle(speciesFamily,speciesItem);
// 						partHandle->name = tr(leftrxnnames[rxn][var]);
// 						speciesItems[tr(leftrxnnames[rxn][var])] = speciesItem;
// 						itemsToInsert += speciesItem;
//
// 						TextGraphicsItem * nameItem = new TextGraphicsItem(partHandle,0);
// 						itemsToInsert += nameItem;
// 						nameItem->setPos(speciesItem->scenePos());
// 						QFont font = nameItem->font();
// 						font.setPointSize(22);
// 						nameItem->setFont(font);
// 					}
// 					else
// 					{
// 						speciesItem = speciesItems[tr(leftrxnnames[rxn][var])];
// 					}
//
// 					if (!exists && reactionItem)
// 						reactionItem->pathVectors +=
// 							ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(reactionItem,speciesItem));
//
// 					stoichiometry.colName(var) = leftrxnnames[rxn][var];
// 					if (stoichiometry.rows() == 1)
// 						stoichiometry.value(0,var) = - leftrxnstoichs[rxn][var];
// 					else
// 						if (stoichiometry.getRowNames().contains(rxnName))
// 						{
// 							stoichiometry.value( rate.getRowNames().indexOf(rxnName), var ) = -leftrxnstoichs[rxn][var];
// 						}
// 						else
// 						{
// 							stoichiometry.value(0,var) = - leftrxnstoichs[rxn][var];
// 						}
// 				}
//
// 				for (int var=0; var<numProducts; var++)
// 				{
// 					PartGraphicsItem * speciesItem = 0;
// 					if (!speciesItems.contains(tr(rightrxnnames[rxn][var])))
// 					{
// 						speciesItem = (static_cast<PartGraphicsItem*>(speciesFamily->graphicsItems[0]))->clone();
// 						//scale to default
// 						if (speciesItem->defaultSize.width() > 0 && speciesItem->defaultSize.height() > 0)
// 								speciesItem->scale(speciesItem->defaultSize.width()/speciesItem->sceneBoundingRect().width(),speciesItem->defaultSize.height()/speciesItem->sceneBoundingRect().height());
//
// 						ItemHandle * partHandle = new PartHandle(speciesFamily,speciesItem);
// 						partHandle->name = tr(rightrxnnames[rxn][var]);
// 						speciesItems[tr(rightrxnnames[rxn][var])] = speciesItem;
// 						itemsToInsert += speciesItem;
//
// 						TextGraphicsItem * nameItem = new TextGraphicsItem(partHandle,0);
// 						itemsToInsert += nameItem;
// 						nameItem->setPos(speciesItem->scenePos());
// 						QFont font = nameItem->font();
// 						font.setPointSize(22);
// 						nameItem->setFont(font);
// 					}
// 					else
// 					{
// 						speciesItem = speciesItems[tr(rightrxnnames[rxn][var])];
// 					}
//
// 					if (!exists && reactionItem)
// 					{
// 						reactionItem->pathVectors +=
// 							ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(reactionItem,speciesItem));
//
// 						ArrowHeadItem * arrow = 0;
// 						if (!biochemicalFamily->graphicsItems.isEmpty() &&
// 							(arrow = qgraphicsitem_cast<ArrowHeadItem*>(biochemicalFamily->graphicsItems.last())) &&
// 							arrow->isValid())
// 						{
// 							arrow = new ArrowHeadItem(*arrow);
// 							arrow->connectionItem = reactionItem;
// 							arrow->scale(0.1,0.1);
// 						}
// 						else
// 						{
// 							QString partImageFile = appDir + tr("/ArrowItems/Reaction.xml");
// 							PartGraphicsReader imageReader;
// 							arrow = new ArrowHeadItem(reactionItem);
// 							imageReader.readXml(arrow,partImageFile);
// 							arrow->normalize();
// 							arrow->scale(0.1,0.1);
// 						}
// 						reactionItem->pathVectors.last().arrowStart = arrow;
// 						itemsToInsert += arrow;
// 					}
// 					stoichiometry.colName(var+numReactants) = rightrxnnames[rxn][var];
// 					stoichiometry.colName(var) = leftrxnnames[rxn][var];
// 					if (stoichiometry.rows() == 1)
// 						stoichiometry.value(0,var+numReactants) += rightrxnstoichs[rxn][var];
// 					else
// 						if (stoichiometry.getRowNames().contains(rxnName))
// 						{
// 							stoichiometry.value( rate.getRowNames().indexOf(rxnName),var+numReactants) = rightrxnstoichs[rxn][var];
// 						}
// 						else
// 						{
// 							stoichiometry.value(0,var+numReactants) = rightrxnstoichs[rxn][var];
// 						}
// 				}
//
// 				if (!exists && reactionItem)
// 				{
// 					reactionHandle->data->textData[tr("Rates")] = rate;
// 					reactionHandle->data->numericalData[tr("Stoichiometry")] = stoichiometry;
// 					TextGraphicsItem * nameItem2 = new TextGraphicsItem(reactionHandle,0);
// 					itemsToInsert += nameItem2;
// 					nameItem2->setPos(reactionItem->scenePos());
// 					QFont font = nameItem2->font();
// 					font.setPointSize(22);
// 					nameItem2->setFont(font);
// 					itemsToInsert += reactionItem;
// 				}
// 				else
// 				{
// 					commands << new Change2DataCommand<qreal,QString>(tr("update reactions"),
// 																	&reactionHandle->data->numericalData[tr("Stoichiometry")],
// 																	&stoichiometry,
// 																	&reactionHandle->data->textData[tr("Rates")],
// 																	&rate);
// 				}
// 			}
// 		}
//
// 		if (!itemsToInsert.isEmpty())
// 		{
// 			scene->insert(tr("Antimony file loaded"), itemsToInsert);
// 		}
//
// 		CompositeCommand * command = new CompositeCommand(tr("Antimony script committed"),commands);
// 		if (scene->historyStack)
// 		{
// 			scene->historyStack->push(command);
// 		}
// 		else
// 		{
// 			command->redo();
// 			delete command;
// 		}

		// freeAll();
	}

	void AntimonyEditor::enterEvent ( QEvent * event )
	{
		qDebug() << "entered ant";
		if (dockWidget)
			dockWidget->setWindowOpacity(1.0);
	}

	void AntimonyEditor::leaveEvent ( QEvent * event )
	{
		if (dockWidget)
			dockWidget->setWindowOpacity(0.5);
	}
}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::AntimonyEditor * antimonyEditor = new Tinkercell::AntimonyEditor;
// 	if (main->tools.contains(antimonyEditor->name))
// 		delete antimonyEditor;
// 	else
// 		antimonyEditor->setMainWindow(main);

}
