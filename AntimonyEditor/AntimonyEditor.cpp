/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 A tool that allows users to construct models using Antimony scripts in the TextEditor

****************************************************************************/

#include <QClipboard>
#include "TextEditor.h"
#include "NetworkWindow.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "NodesTree.h"
#include "ConnectionsTree.h"
#include "AntimonyEditor.h"
#include "ModelSummaryTool.h"
#include "ModuleTool.h"
#include "CloneItems.h"
#include <QToolButton>
#include <QRegExp>
#include <QFile>
#include <QPair>
#include <QRegExp>
#include <QtDebug>
#include <QSemaphore>
#include <QFileDialog>

#include "antimony_api.h"


namespace Tinkercell
{
	AntimonyEditor_FtoS AntimonyEditor::fToS;

	AntimonyEditor::AntimonyEditor() : TextParser(tr("Antimony Parser"))
	{
		scriptDisplayWindow = new CodeEditor(this);
		icon = QPixmap(tr(":/images/antimony.png"));
	}

	bool AntimonyEditor::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			mainWindow->addParser(this);

			if (mainWindow->fileMenu)
			{
				QList<QAction*> actions = mainWindow->fileMenu->actions();

				for (int i=0; i < actions.size(); ++i)

					if (actions[i] && actions[i]->text() == tr("&Close page"))
					{
						QMenu * exportmenu = new QMenu(tr("&Export SBML/Antimony"));
						QMenu * importmenu = new QMenu(tr("&Import SBML"));
						mainWindow->fileMenu->insertMenu(actions[i],importmenu);
						mainWindow->fileMenu->insertMenu(actions[i],exportmenu);
						importmenu->addAction(tr("load SBML file"),this,SLOT(loadSBMLFile()));
						importmenu->addAction(tr("get SBML from clipboard"),this,SLOT(pasteSBMLText()));
						exportmenu->addAction(tr("save SBML file"),this,SLOT(saveSBMLFile()));
						exportmenu->addAction(tr("copy SBML text"),this,SLOT(copySBMLText()));
						exportmenu->addAction(tr("save Antimony file"),this,SLOT(saveAntimonyFile()));
						exportmenu->addAction(tr("copy Antimony text"),this,SLOT(copyAntimonyText()));
						break;
					}
			}

			connect(mainWindow,SIGNAL(copyItems(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&)),this,SLOT(copyItems(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&)));
			connect(mainWindow,SIGNAL(windowOpened(NetworkWindow*)),this,SLOT(windowOpened(NetworkWindow*)));
			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			toolLoaded(0);

			connectTCFunctions();
		}
		return false;
	}

	void AntimonyEditor::insertModule()
	{
		if (!currentTextEditor()) return;
		QList<TextItem*>& textItems = currentTextEditor()->items();

		QList<ItemHandle*> modules;
		ItemHandle * handle;
		for (int i=0; i < textItems.size(); ++i)
			if ((handle = getHandle(textItems[i])) && handle->isA(tr("Module")) && !handle->name.isEmpty())
				modules += handle;

		if (modules.isEmpty()) return;

		NodeFamily * moduleFamily = 0;

		if (mainWindow->tool(tr("Nodes Tree")))
		{
			NodesTree * partsTree = static_cast<NodesTree*>(mainWindow->tool(tr("Nodes Tree")));
			if (partsTree->nodeFamilies.contains(tr("Module")))
				moduleFamily = partsTree->nodeFamilies.value(tr("Module"));
		}

		if (!moduleFamily) return;

		QList<NetworkWindow*> windows = mainWindow->allWindows();

		GraphicsScene * scene = 0;

		for (int i = windows.size()-1; i >= 0; --i)
			if (windows[i] && windows[i]->scene)
			{
				scene = windows[i]->scene;
				break;
			}

		if (!scene) return;

		QList<TextItem*> clones = clone(textItems);
		modules.clear();

		QString text;
		QList<QGraphicsItem*> list;

		QPointF point = scene->lastPoint();

		for (int i=0; i < clones.size(); ++i)
			if ((handle = getHandle(clones[i])) && handle->isA(tr("Module")))
			{
                    qreal xpos = point.x();
                    qreal height = 0.0;
                    qreal width = 0.0;

                    for (int i=0; i < moduleFamily->graphicsItems.size(); ++i)
                        if (qgraphicsitem_cast<NodeGraphicsItem*>(moduleFamily->graphicsItems[i]))
                            width += qgraphicsitem_cast<NodeGraphicsItem*>(moduleFamily->graphicsItems[i])->defaultSize.width();

                    xpos -= width/2.0;
                    bool alternate = false;

                    text += handle->name + tr(" ");

					NodeFamily * nodeFamily = moduleFamily;

                    for (int i=0; i < nodeFamily->graphicsItems.size(); ++i)
					{
						NodeGraphicsItem * image = (NodeGraphicsItem::topLevelNodeItem(nodeFamily->graphicsItems[i]));
						if (image)
						{
							   image = image->clone();

							   if (image->defaultSize.width() > 0 && image->defaultSize.height() > 0)
									image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),image->defaultSize.height()/image->sceneBoundingRect().height());

							   qreal w = image->sceneBoundingRect().width();
							   image->setPos(xpos + w/2.0, point.y());
							   image->adjustBoundaryControlPoints();
							   image->setBoundingBoxVisible(false);

							   if (image->isValid())
							   {
									xpos += w;
									setHandle(image,handle);
									list += image;
							   }
							   if (image->sceneBoundingRect().height() > height)
									height = image->sceneBoundingRect().height();
						  }
					 }

					 if (nodeFamily->graphicsItems.size() > 0)
					 {
						  if (handle->family())
						  {
							   TextGraphicsItem * nameItem = new TextGraphicsItem(handle,0);
							   QFont font = nameItem->font();
							   font.setPointSize(22);
							   nameItem->setFont(font);
							   if (alternate)
									nameItem->setPos(xpos - nameItem->boundingRect().width(), point.y() - height/2.0 - 40.0);
							   else
									nameItem->setPos(xpos - nameItem->boundingRect().width(), point.y() + height/2.0 + 5.0);
							   list += nameItem;
							   alternate = !alternate;
						  }
					}
			}

			if (!list.isEmpty())
			{
				scene->insert(text + tr("inserted"),list);
				if (scene->networkWindow)
					mainWindow->setCurrentWindow(scene->networkWindow);
			}
	}

	void AntimonyEditor::windowOpened(NetworkWindow * win)
	{
		if (win && win->textEditor && TextParser::currentParser() == this)
		{
			AntimonySyntaxHighlighter * as = new AntimonySyntaxHighlighter(win->textEditor->document());
			connect(this,SIGNAL(validSyntax(bool)),as,SLOT(setValid(bool)));

			QToolButton * button = new QToolButton;
			button->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
			button->setIcon(QIcon(":/images/antimony.png"));
			button->setIconSize(QSize(30,30));
			button->setText(tr("Compile script"));
			button->setToolTip(tr("interpret script using Antimony language"));
			connect(button,SIGNAL(pressed()),this,SLOT(parse()));
			win->textEditor->addSideBarWidget(button);

			button = new QToolButton;
			button->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
			button->setIcon(QIcon(":/images/module.png"));
			button->setIconSize(QSize(30,30));
			button->setText(tr("Export modules"));
			button->setToolTip(tr("export module(s) to the last graphics window"));
			connect(button,SIGNAL(pressed()),this,SLOT(insertModule()));
			win->textEditor->addSideBarWidget(button);
		}
	}

	void AntimonyEditor::parse()
	{
		parse(currentTextEditor());
	}

	void AntimonyEditor::parse(TextEditor * editor)
	{
		if (!editor) return;

		QString modelString = editor->toPlainText() + tr("\n");

		QList<TextItem*> itemsToInsert = parse(modelString);

		if (!itemsToInsert.isEmpty())
		{
			editor->setItems(itemsToInsert);
		}
	}

	QList<TextItem*> AntimonyEditor::parse(const QString& modelString)
	{
		long ok = loadString(modelString.toAscii().data());

		if (ok < 0)
		{
			if (modelString.contains(tr("end")))
				if (console())
                    console()->error(tr(getLastError()));
			emit validSyntax(false);
			return QList<TextItem*>();
		}
		else
		{
			emit validSyntax(true);
		}

		QString appDir = QCoreApplication::applicationDirPath();

		ConnectionFamily * biochemicalFamily = 0;
		NodeFamily * speciesFamily = 0;
		NodeFamily * moduleFamily = 0;

		if (mainWindow->tool(tr("Nodes Tree")) && mainWindow->tool(tr("Connections Tree")))
		{
			NodesTree * partsTree = static_cast<NodesTree*>(mainWindow->tool(tr("Nodes Tree")));
			ConnectionsTree * connectionsTree = static_cast<ConnectionsTree*>(mainWindow->tool(tr("Connections Tree")));
			if (connectionsTree->connectionFamilies.contains(tr("Biochemical")))
				biochemicalFamily = connectionsTree->connectionFamilies.value(tr("Biochemical"));
			if (partsTree->nodeFamilies.contains(tr("Molecule")))
				speciesFamily = partsTree->nodeFamilies.value(tr("Molecule"));
			if (partsTree->nodeFamilies.contains(tr("Module")))
				moduleFamily = partsTree->nodeFamilies.value(tr("Module"));
		}

		if (!biochemicalFamily || !speciesFamily || !moduleFamily)
		{
			if (console())
                console()->error(tr("No parts and connection information"));
			return QList<TextItem*>();
		}

		//load

		int nummods = (int)getNumModules();
		char** modnames = getModuleNames();

		QList<TextItem*> itemsToInsert;

		for (int i=0; i < nummods; ++i)
		{
			char * moduleName = modnames[i];
			ItemHandle * moduleHandle;

			NodeTextItem * moduleText = 0;

			moduleHandle = new NodeHandle(moduleFamily);
			moduleHandle->name = QString(moduleName);
			moduleText = new NodeTextItem;
			setHandle(moduleText,moduleHandle);
			itemsToInsert += moduleText;

			if (QString(modnames[i]) == tr("__main"))
				moduleHandle->name = tr("main");

			QStringList symbolsInModule;
			symbolsInModule << moduleHandle->name;

			QList<ItemHandle*> handlesInModule;
			QHash<QString,NodeHandle*> speciesItems;

			char ***leftrxnnames = getReactantNames(moduleName);
			char ***rightrxnnames = getProductNames(moduleName);
			char **rxnnames = getSymbolNamesOfType(moduleName,allReactions);
			char **rxnrates = getReactionRates(moduleName);

			double **leftrxnstoichs = getReactantStoichiometries(moduleName);
			double **rightrxnstoichs = getProductStoichiometries(moduleName);

			int numrxn = (int)getNumReactions(moduleName);

			for (int rxn=0; rxn < numrxn; ++rxn)
			{
				int numReactants = getNumReactants(moduleName,rxn);
				int numProducts = getNumProducts(moduleName,rxn);

				QList<NodeTextItem*> nodesIn, nodesOut;
				DataTable<qreal> stoichiometry;
				DataTable<QString> rate;
				QStringList colNames;

				for (int var=0; var<numReactants; ++var)
				{
					if (!colNames.contains(tr(leftrxnnames[rxn][var])))
					{
						colNames << tr(leftrxnnames[rxn][var]);
					}
				}
				for (int var=0; var<numProducts; ++var)
				{
					if (!colNames.contains(tr(rightrxnnames[rxn][var])))
					{
						colNames << tr(rightrxnnames[rxn][var]);
					}
				}

				stoichiometry.resize(1,colNames.size());
				stoichiometry.setColNames(colNames);

				for (int j=0; j < colNames.size(); ++j)
				{
					stoichiometry.value(0,j) = 0;
				}

				rate.resize(1,1);
				ItemHandle * reactionHandle = new ConnectionHandle(biochemicalFamily);
				handlesInModule << reactionHandle;

				reactionHandle->name = rxnnames[rxn];
				stoichiometry.rowName(0) = reactionHandle->name;
				symbolsInModule << reactionHandle->name;

				for (int var=0; var<numReactants; ++var)
				{
					NodeHandle * handle = 0;
					if (!speciesItems.contains(tr(leftrxnnames[rxn][var])))
					{
						handle = new NodeHandle(speciesFamily);
						handlesInModule << handle;
						handle->name = tr(leftrxnnames[rxn][var]);
						symbolsInModule << handle->name;
						speciesItems[tr(leftrxnnames[rxn][var])] = handle;
						NodeTextItem * n = new NodeTextItem(handle);
						nodesIn << n;
						itemsToInsert  += n;
					}
					else
					{
						handle = speciesItems[tr(leftrxnnames[rxn][var])];
					}
					stoichiometry.value(0,tr(leftrxnnames[rxn][var])) -= leftrxnstoichs[rxn][var];
				}

				for (int var=0; var<numProducts; var++)
				{
					NodeHandle * partHandle = 0;
					if (!speciesItems.contains(tr(rightrxnnames[rxn][var])))
					{
						partHandle = new NodeHandle(speciesFamily);
						handlesInModule << partHandle;
						partHandle->name = tr(rightrxnnames[rxn][var]);
						symbolsInModule << partHandle->name;
						speciesItems[tr(rightrxnnames[rxn][var])] = partHandle;
						NodeTextItem * n = new NodeTextItem(partHandle);
						nodesOut << n;
						itemsToInsert += n;
					}
					else
					{
						partHandle = speciesItems[tr(rightrxnnames[rxn][var])];
					}

					stoichiometry.value(0,tr(rightrxnnames[rxn][var])) += rightrxnstoichs[rxn][var];
				}

				QString srate = tr(rxnrates[rxn]);
				rate.rowName(0) = reactionHandle->name;
				rate.colName(0) = tr("rate");
				rate.value(0,0) = srate;
				reactionHandle->data->textData[tr("Rates")] = rate;
				reactionHandle->data->numericalData[tr("Stoichiometry")] = stoichiometry;

				ConnectionTextItem * c = new ConnectionTextItem(reactionHandle);
				c->nodesIn = nodesIn;
				c->nodesOut = nodesOut;
				itemsToInsert += c;
			}

			int numSpecies = (int)getNumSymbolsOfType(moduleName,varSpecies);
			char ** speciesNames = getSymbolNamesOfType(moduleName,varSpecies);
			char ** speciesValues = getSymbolEquationsOfType(moduleName,varSpecies);
			for (int j=0; j < numSpecies; ++j)
			{
				bool ok;
				qreal x = QString(speciesValues[j]).toDouble(&ok);
				QString s(speciesNames[j]);
				if (ok && speciesItems.contains(s))
				{
					speciesItems[s]->numericalData(tr("Initial Value"),speciesFamily->measurementUnit.first,speciesFamily->measurementUnit.second) = x;
					speciesItems[s]->numericalData(tr("Fixed")) = 0;
				}
			}

			int numConstSpecies = (int)getNumSymbolsOfType(moduleName,constSpecies);
			char ** constSpeciesNames = getSymbolNamesOfType(moduleName,constSpecies);
			char ** constSpeciesValues = getSymbolEquationsOfType(moduleName,constSpecies);
			for (int j=0; j < numConstSpecies; ++j)
			{
				bool ok;
				qreal x = QString(constSpeciesValues[j]).toDouble(&ok);
				QString s(constSpeciesNames[j]);
				if (ok && speciesItems.contains(s))
				{
					speciesItems[s]->numericalData(tr("Initial Value")) = x;
					speciesItems[s]->data->numericalData[ tr("Initial Value") ].rowName(0) = tr("concentration");
					speciesItems[s]->data->numericalData[ tr("Initial Value") ].colName(0) = tr("uM");
					speciesItems[s]->numericalData(tr("Fixed")) = 1;
					speciesItems[s]->data->numericalData[ tr("Fixed") ].rowName(0) = tr("fix");
					speciesItems[s]->data->numericalData[ tr("Fixed") ].colName(0) = tr("value");
				}
			}

			int numAssignments = (int)getNumSymbolsOfType(moduleName,varFormulas);
			char ** assignmentNames = getSymbolNamesOfType(moduleName,varFormulas);
			char ** assignmentValues = getSymbolEquationsOfType(moduleName,varFormulas);

			DataTable<QString> assgnsTable;
			QList<ItemHandle*> handlesInModule2 = handlesInModule;
			handlesInModule2 << moduleHandle;

			for (int j=0; j < numAssignments; ++j)
			{
				QString x(assignmentValues[j]);
				assgnsTable.value(tr(assignmentNames[j]),0) = x;
				symbolsInModule << tr(assignmentNames[j]);
				RenameCommand::findReplaceAllHandleData(handlesInModule2,tr(assignmentNames[j]),moduleHandle->name + tr(".") + tr(assignmentNames[j]));
			}

			moduleHandle->data->textData[tr("Assignments")] = assgnsTable;

			int numEvents = (int)getNumEvents(moduleName);
			char ** eventNames = getEventNames(moduleName);

			DataTable<QString> eventsTable;
			for (int j=0; j < numEvents; ++j)
			{
				QString trigger(getTriggerForEvent(moduleName,j));

				int n = (int)getNumAssignmentsForEvent(moduleName,j);

				for (int k=0; k < n; ++k)
				{
					QString x(getNthAssignmentVariableForEvent(moduleName,j,k));
					QString f(getNthAssignmentEquationForEvent(moduleName,j,k));

					eventsTable.value(trigger,0) = x + tr(" = ") + f;
				}
			}
			moduleHandle->data->textData[tr("Events")] = eventsTable;

			int numParams = (int)getNumSymbolsOfType(moduleName,constFormulas);
			char ** paramNames = getSymbolNamesOfType(moduleName,constFormulas);
			char ** paramValues = getSymbolEquationsOfType(moduleName,constFormulas);

			DataTable<qreal> paramsTable;
			for (int j=0; j < numParams; ++j)
			{
				bool ok;
				qreal x = QString(paramValues[j]).toDouble(&ok);
				symbolsInModule << tr(paramNames[j]);
				if (ok)
				{
					paramsTable.value(tr(paramNames[j]),0) = x;
					RenameCommand::findReplaceAllHandleData(handlesInModule2,tr(paramNames[j]),moduleHandle->name + tr(".") + tr(paramNames[j]));
				}
				else
				{
					RenameCommand::findReplaceAllHandleData(handlesInModule2,tr(paramValues[j]),moduleHandle->name + tr(".") + tr(paramValues[j]));
					moduleHandle->data->textData[tr("Assignments")].value(tr(paramNames[j]),0) = paramValues[j];
				}
			}

			numParams = (int)getNumSymbolsOfType(moduleName,allSymbols);
			paramNames = getSymbolNamesOfType(moduleName,allSymbols);
			paramValues = getSymbolEquationsOfType(moduleName,allSymbols);

			for (int j=0; j < numParams; ++j)
			{
				if (!symbolsInModule.contains(tr(paramNames[j])))
				{
					bool ok;
					qreal x = QString(paramValues[j]).toDouble(&ok);
					if (!ok)
						x = 1.0;
					paramsTable.value(tr(paramNames[j]),0) = x;
					RenameCommand::findReplaceAllHandleData(handlesInModule2,tr(paramNames[j]),moduleHandle->name + tr(".") + tr(paramNames[j]));
				}
			}

			moduleHandle->data->numericalData[tr("Numerical Attributes")] = paramsTable;

			for (int j=0; j < handlesInModule.size(); ++j)
				if (handlesInModule[j])
				{
					handlesInModule[j]->setParent(moduleHandle);
					RenameCommand::findReplaceAllHandleData(handlesInModule2,handlesInModule[j]->name,handlesInModule[j]->fullName());
				}

			if (handlesInModule.isEmpty())
			{
				if (moduleText)
					itemsToInsert.removeAll(moduleText);

				if (moduleHandle)
					delete moduleHandle;
			}
		}

		freeAll();
		return itemsToInsert;
	}


	void AntimonyEditor::textChanged(TextEditor * editor, const QString&, const QString&, const QString&)
	{
		if (editor && editor->toPlainText().size() < 1000)
			parse(editor);
	}

	void AntimonyEditor::lineChanged(TextEditor *, int, const QString&)
	{
	}

	QList<TextItem*> AntimonyEditor::clone(const QList<TextItem*>& items)
	{
		return cloneTextItems(items);
	}

	void AntimonyEditor::toolLoaded(Tool*)
	{
		static bool connected1 = false, connected2 = false;
		if (connected1 && connected2) return;

		if (mainWindow && mainWindow->tool(tr("Model Summary")))
		{
			connected1 = true;
			QWidget * widget = mainWindow->tool(tr("Model Summary"));
			ModelSummaryTool * modelSummary = static_cast<ModelSummaryTool*>(widget);
			connect(modelSummary,SIGNAL(displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)),
					this,SLOT(displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)));
		}

		if (mainWindow && mainWindow->tool(tr("Module Connection Tool")))
		{
		    connected2 = true;
			QWidget * widget = mainWindow->tool(tr("Module Connection Tool"));
			ModuleTool * moduleTool = static_cast<ModuleTool*>(widget);
			connect(moduleTool,SIGNAL(createTextWindow(TextEditor*, const QList<ItemHandle*>&)),
					this,SLOT(createTextWindow(TextEditor*, const QList<ItemHandle*>&)));
		}
	}

	void AntimonyEditor::displayModel(QTabWidget& widgets, const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& )
	{
		if (!scriptDisplayWindow || items.size() != 1) return;

		ItemHandle * handle = items[0];

		if (handle && handle->isA(tr("Module")) && currentWindow())
		{
			QString s = getAntimonyScript(items);
			scriptDisplayWindow->setPlainText(s);
			widgets.addTab(scriptDisplayWindow,tr("Antimony script"));
		}
	}

	void AntimonyEditor::copyItems(GraphicsScene * scene, QList<QGraphicsItem*>& , QList<ItemHandle*>& handles)
	{
		if (scene && scene->symbolsTable && handles.size() > 0)
		{
			QClipboard * clipboard = QApplication::clipboard();
			if (clipboard)
			{
				clipboard->setText( getAntimonyScript(handles) );
			}
		}
	}

	void AntimonyEditor::appendScript(QString& s, const QList<ItemHandle*>& childHandles)
	{
		QRegExp regex(tr("\\.(?!\\d)"));
		QString s2;
		QString allEqns;
		QStringList usedSymbols;
		for (int j=0; j < childHandles.size(); ++j)
			if (childHandles[j] && childHandles[j]->isA(tr("Module")))
			{
				s += tr("    ");
				s += childHandles[j]->name;
				s += tr(": ");
				s += childHandles[j]->name;
				s += tr(";\n\n");
			}

		for (int j=0; j < childHandles.size(); ++j)
			if (childHandles[j] && childHandles[j]->isA(tr("compartment")))
			{
				if (s2.isEmpty())
					s2 += tr("    compartment ");
				else
					s2 += tr(", ");
				s2 += childHandles[j]->fullName(tr("_"));

				if (!s2.isEmpty())
				{
					QString s3;
					for (int j=0; j < childHandles.size(); ++j)
						if (childHandles[j] && childHandles[j]->children.isEmpty())
						{
							if (s3.isEmpty())
								s3 += tr("    species ");
							else
								s3 += tr(", ");
							s3 += childHandles[j]->fullName(tr("_"));
							if (childHandles[j]->parent && childHandles[j]->parent->isA(tr("compartment")))
								s3 += tr("in ") + childHandles[j]->parent->fullName(tr("_"));
						}

					s += s2;
					s += tr("\n");
					s += s3;
					s += tr("\n\n");
				}
			}

		QString name, rate;
		QStringList lhs, rhs, species;
		for (int j=0; j < childHandles.size(); ++j)
			if (childHandles[j])
			{
				name = childHandles[j]->fullName(tr("_"));
				if (childHandles[j]->hasNumericalData(tr("Stoichiometry")) &&
					 childHandles[j]->hasTextData(tr("Rates")))
					{
						DataTable<qreal>& N = childHandles[j]->numericalDataTable(tr("Stoichiometry"));
						DataTable<QString>& V = childHandles[j]->textDataTable(tr("Rates"));
						for (int r=0; r < N.rows(); ++r)
						{
							lhs.clear();
							rhs.clear();
							rate = V.value(r,0);
							allEqns += rate;

							for (int c=0; c < N.cols(); ++c)
								if (N.value(r,c) < 0)
								{
									species = N.colName(c);
									lhs += species.replace(tr("."),tr("_"));
								}
								else
								if (N.value(r,c) > 0)
								{
									species = N.colName(c);
									rhs += species.replace(tr("."),tr("_"));
								}

							rate.replace(regex,tr("_"));
							s += tr("    ");
							s += name;
							if (N.rows() > 1)
								s += tr("_") + N.rowName(r);
							s += tr(": ");
							s += lhs.join(tr(" + "));
							s += tr(" -> ");
							s += rhs.join(tr(" + "));
							s += tr(";    ");
							s += rate + tr(";\n");
						}

					}
			}

        s += tr("\n");

        for (int j=0; j < childHandles.size(); ++j)
			if (childHandles[j])
            {
                name = childHandles[j]->fullName(tr("_"));
				if (childHandles[j]->hasTextData(tr("Assignments")))
				{
					DataTable<QString>& assigns = childHandles[j]->textDataTable(tr("Assignments"));

					for (int r=0; r < assigns.rows(); ++r)
					{
						QString rule = assigns.value(r,0);
						rule.replace(regex,tr("_"));
						s += tr("    ");
						if (!name.isEmpty())
						{
							s += name;
							s += tr("_");
						}
						s += assigns.rowName(r);
						s += tr(" = ");
						s += rule;
						s += tr(";\n");

						allEqns += rule;
						usedSymbols << (name + tr(".") + assigns.rowName(r));
					}
				}
            }

        s += tr("\n");

        for (int j=0; j < childHandles.size(); ++j)
			if (childHandles[j])
			{
			    name = childHandles[j]->fullName(tr("_"));
				if (childHandles[j]->hasNumericalData(tr("Numerical Attributes")))
				{
					DataTable<qreal>& params = childHandles[j]->numericalDataTable(tr("Numerical Attributes"));

					for (int r=0; r < params.rows(); ++r)
                        if (allEqns.contains(name + tr(".") + params.rowName(r)) &&
                            !usedSymbols.contains(name + tr(".") + params.rowName(r)))
                        {
                            s += tr("    ");
                            s += name;
                            s += tr("_");
                            s += params.rowName(r);
                            s += tr(" = ");
                            s += QString::number(params.value(r,0));
                            s += tr(";\n");
                            usedSymbols << (name + tr(".") + params.rowName(r));
                        }
				}
			}

        s += tr("\n");

        for (int j=0; j < childHandles.size(); ++j)
			if (childHandles[j])
			{
			    name = childHandles[j]->fullName(tr("_"));
				if (childHandles[j]->hasNumericalData(tr("Initial Value")) && !name.isEmpty())
				{
					s += tr("    ");
					s += name;
					s += tr(" = ");
					s += QString::number(childHandles[j]->numericalData(tr("Initial Value")));
					s += tr(";\n");
				}
			}
	}

	QString AntimonyEditor::getAntimonyScript(const QList<ItemHandle*>& list)
	{
		//QString s("Model M\n");
		QString s;
		/*
		reaction
		formula
		DNA
		gene
		operator
		compartment
		*/

		QList<ItemHandle*> visitedHandles, allHandles, childHandles, temp;
		ItemHandle * root;

		for (int i=0; i < list.size(); ++i)
		{
			root = 0;
			if (list[i]) root = list[i]->root();

			if (root)
			{
				temp = root->allChildren();
				if (!allHandles.contains(root))
					allHandles << root;
			}

			for (int j=0; j < temp.size(); ++j)
				if (temp[j] && !allHandles.contains(temp[j]))
					allHandles << temp[j];
		}

		for (int i=0; i < allHandles.size(); ++i)
			if (allHandles[i]->isA(tr("Module")))
			{
				visitedHandles << allHandles[i];

				s += tr("Module ");
				s += allHandles[i]->name;
				s += tr("()\n");
				childHandles = allHandles[i]->children;

				visitedHandles << childHandles;

				appendScript(s,childHandles);

				s += tr("end;\n\n");
			}

		childHandles.clear();

		for (int i=0; i < allHandles.size(); ++i)
			if (allHandles[i] && !visitedHandles.contains(allHandles[i]))
				childHandles << allHandles[i];

		appendScript(s,childHandles);

		return s;
	}

	void AntimonyEditor::createTextWindow(TextEditor * newEditor, const QList<ItemHandle*>& items)
	{
		if (!newEditor || !newEditor->networkWindow) return;
		newEditor->setText(getAntimonyScript(items));
	}

	void AntimonyEditor::loadSBMLFile()
	{
		QString file = QFileDialog::getOpenFileName (this, tr("Load SBML file"));

		if (!file.isNull() && !file.isEmpty())
			loadSBMLFileSlot(0, file);
	}

	void AntimonyEditor::pasteSBMLText()
	{
		QClipboard * clipboard = QApplication::clipboard();
		if (clipboard)
		{
			QString s = clipboard->text();

			if (!s.isNull() && !s.isEmpty())
				loadSBMLStringSlot(0, s);
		}
	}

	void AntimonyEditor::saveSBMLFile()
	{
		if (!currentNetwork()) return;

		QString file = QFileDialog::getSaveFileName (this, tr("Save SBML file"));

		if (!file.isNull() && !file.isEmpty())
		{
			QList<ItemHandle*> handles = currentNetwork()->allHandles();
			handles << currentNetwork()->modelItem();
			writeSBMLFileSlot(0, handles, file);
		}
	}

	void AntimonyEditor::copySBMLText()
	{
		if (!currentNetwork()) return;

		QList<ItemHandle*> handles = currentNetwork()->allHandles();
		handles << currentNetwork()->modelItem();
		QString s;
		getSBMLStringSlot(0, handles, &s);
		QClipboard * clipboard = QApplication::clipboard();
		if (clipboard)
		{
			clipboard->setText(s);
			if (console())
				console()->message(tr("SBML text copied to clipboard"));
		}
	}

	void AntimonyEditor::saveAntimonyFile()
	{
		if (!currentNetwork()) return;

		QString file = QFileDialog::getSaveFileName (this, tr("Save SBML file"));

		if (!file.isNull() && !file.isEmpty())
		{
			QList<ItemHandle*> handles = currentNetwork()->allHandles();
			handles << currentNetwork()->modelItem();
			writeAntimonyFileSlot(0, handles, file);
		}
	}

	void AntimonyEditor::copyAntimonyText()
	{
		if (!currentNetwork()) return;

		QList<ItemHandle*> handles = currentNetwork()->allHandles();
		handles << currentNetwork()->modelItem();
		QString s;
		getAntimonyStringSlot(0, handles, &s);
		QClipboard * clipboard = QApplication::clipboard();
		if (clipboard)
		{
			clipboard->setText(s);
			if (console())
				console()->message(tr("SBML text copied to clipboard"));
		}
	}

	/**********************
	       C API
	***********************/

	void AntimonyEditor::loadSBMLStringSlot(QSemaphore* s,const QString& sbml)
	{
		if (mainWindow)
			mainWindow->newTextWindow();

		if (currentTextEditor() && loadString (sbml.toAscii().data()) != -1)
		{
			char * ant = getAntimonyString();
			currentTextEditor()->setText(tr(ant));
		}

		if (s) s->release();
	}

	void AntimonyEditor::loadAntimonyStringSlot(QSemaphore* s,const QString& ant)
	{
		if (mainWindow)
			mainWindow->newTextWindow();

		if (currentTextEditor())
		{
			currentTextEditor()->setText(ant);
		}
		if (s) s->release();
	}

	void AntimonyEditor::loadSBMLFileSlot(QSemaphore* s,const QString& file)
	{
		if (mainWindow)
			mainWindow->newTextWindow();

		if (currentTextEditor() && loadFile (file.toAscii().data()) != -1)
		{
			char * ant = getAntimonyString();
			currentTextEditor()->setText(tr(ant));

			if (mainWindow->currentWindow())
			{
				QRegExp regexp(tr("([A-Za-z_0-9]+)\\."));
				regexp.indexIn(file);
				if (regexp.numCaptures() > 0)
					mainWindow->currentWindow()->setWindowTitle(regexp.cap(1));
			}
		}

		if (s) s->release();
	}

	void AntimonyEditor::loadAntimonyFileSlot(QSemaphore* s,const QString& file)
	{
		if (mainWindow)
			mainWindow->newTextWindow();

		if (currentTextEditor() && loadFile (file.toAscii().data()) != -1)
		{
			char * ant = getAntimonyString();
			currentTextEditor()->setText(tr(ant));

			if (mainWindow->currentWindow())
			{
				QRegExp regexp(tr("([A-Za-z_0-9]+)\\."));
				regexp.indexIn(file);
				if (regexp.numCaptures() > 0)
					mainWindow->currentWindow()->setWindowTitle(regexp.cap(1));
			}
		}

		if (s) s->release();
	}

	void AntimonyEditor::getSBMLStringSlot(QSemaphore* s,const QList<ItemHandle*>& items, QString* sbml)
	{
		if (sbml && currentWindow())
		{
			QString ant = getAntimonyScript(items);
			if (loadString(ant.toAscii().data()) != -1)
				(*sbml) = tr(getSBMLString("__main"));
		}
		if (s) s->release();
	}

	void AntimonyEditor::getAntimonyStringSlot(QSemaphore* s,const QList<ItemHandle*>& items, QString* ant)
	{
		if (ant && currentWindow())
		{
			(*ant) = getAntimonyScript(items);
		}
		if (s)
			s->release();
	}

	void AntimonyEditor::writeSBMLFileSlot(QSemaphore* s,const QList<ItemHandle*>& items, const QString& filename)
	{
		if (currentTextEditor() && currentWindow())
		{
			QString ant = getAntimonyScript(items);
			if (loadString(ant.toAscii().data()) != -1)
			{
				QString s = tr(getSBMLString("__main"));
				QFile file(filename);
				if (file.open(QIODevice::WriteOnly))
				{
					file.write(s.toAscii());
					file.close();
				}
			}
		}
		if (s) s->release();
	}

	void AntimonyEditor::writeAntimonyFileSlot(QSemaphore* s,const QList<ItemHandle*>& items, const QString& file)
	{
		if (currentTextEditor() && currentWindow())
		{
			QString ant = getAntimonyScript(items);
			if (loadString(ant.toAscii().data()) != -1)
				writeAntimonyFile(file.toAscii().data(),"__main");
		}
		if (s) s->release();
	}

	typedef void (*tc_Antimony_api)(
				void (*loadAntimonyString)(const char *),
				void (*loadSBMLString)(const char *),
				void (*loadSBMLFile)(const char *),
				void (*loadAntimonyFile)(const char *),
				char* (*getSBMLString)(Array),
				char* (*getAntimonyString)(Array),
				void (*writeSBMLFile)(Array,const char*),
				void (*writeAntimonyFile)(Array,const char*));

	void AntimonyEditor::setupFunctionPointers( QLibrary * library)
	{
		tc_Antimony_api f = (tc_Antimony_api)library->resolve("tc_Antimony_api");
		if (f)
		{
			f(
				&(_loadAntimonyString),
				&(_loadSBMLString),
				&(_loadSBMLFile),
				&(_loadAntimonyFile),
				&(_getSBMLString),
				&(_getAntimonyString),
				&(_writeSBMLFile),
				&(_writeAntimonyFile)
			);
		}
	}

	void AntimonyEditor::connectTCFunctions()
	{
		connect(&fToS,SIGNAL(loadSBMLStringSignal(QSemaphore*,const QString&)),
				this,SLOT(loadSBMLStringSlot(QSemaphore*,const QString&)));

		connect(&fToS,SIGNAL(loadAntimonyStringSignal(QSemaphore*,const QString&)),
				this,SLOT(loadAntimonyStringSlot(QSemaphore*,const QString&)));

		connect(&fToS,SIGNAL(loadSBMLFileSignal(QSemaphore*,const QString&)),
				this,SLOT(loadSBMLFileSlot(QSemaphore*,const QString&)));

		connect(&fToS,SIGNAL(loadAntimonyFileSignal(QSemaphore*,const QString&)),
				this,SLOT(loadAntimonyFileSlot(QSemaphore*,const QString&)));

		connect(&fToS,SIGNAL(getSBMLStringSignal(QSemaphore*,const QList<ItemHandle*>&, QString*)),
				this,SLOT(getSBMLStringSlot(QSemaphore*,const QList<ItemHandle*>&, QString*)));

		connect(&fToS,SIGNAL(getAntimonyStringSignal(QSemaphore*,const QList<ItemHandle*>&, QString*)),
				this,SLOT(getAntimonyStringSlot(QSemaphore*,const QList<ItemHandle*>&, QString*)));

		connect(&fToS,SIGNAL(writeSBMLFileSignal(QSemaphore*,const QList<ItemHandle*>&, const QString&)),
				this,SLOT(writeSBMLFileSlot(QSemaphore*,const QList<ItemHandle*>&, const QString&)));

		connect(&fToS,SIGNAL(writeAntimonyFileSignal(QSemaphore*,const QList<ItemHandle*>&, const QString&)),
				this,SLOT(writeAntimonyFileSlot(QSemaphore*,const QList<ItemHandle*>&, const QString&)));
	}

	void AntimonyEditor_FtoS::loadSBMLString(const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit loadSBMLStringSignal(s,tr(c));
		s->acquire();
		s->release();
		delete s;
	}
	void AntimonyEditor_FtoS::loadAntimonyString(const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit loadAntimonyStringSignal(s,tr(c));
		s->acquire();
		s->release();
		delete s;
	}
	void AntimonyEditor_FtoS::loadSBMLFile(const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit loadSBMLFileSignal(s,tr(c));
		s->acquire();
		s->release();
		delete s;
	}
	void AntimonyEditor_FtoS::loadAntimonyFile(const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit loadAntimonyFileSignal(s,tr(c));
		s->acquire();
		s->release();
		delete s;
	}
	char* AntimonyEditor_FtoS::getSBMLString(Array a)
	{
		QString str;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		QList<ItemHandle*> * list = ConvertValue(a);
		emit getSBMLStringSignal(s,*list,&str);
		delete list;
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(str);
	}
	char* AntimonyEditor_FtoS::getAntimonyString(Array a)
	{
		QString str;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		QList<ItemHandle*> * list = ConvertValue(a);
		emit getAntimonyStringSignal(s,*list,&str);
		delete list;
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(str);
	}
	void AntimonyEditor_FtoS::writeSBMLFile(Array a,const char* c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		QList<ItemHandle*> * list = ConvertValue(a);
		emit writeSBMLFileSignal(s,*list,tr(c));
		delete list;
		s->acquire();
		s->release();
		delete s;
	}
	void AntimonyEditor_FtoS::writeAntimonyFile(Array a,const char* c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		QList<ItemHandle*> * list = ConvertValue(a);
		emit writeAntimonyFileSignal(s,*list,tr(c));
		delete list;
		s->acquire();
		s->release();
		delete s;
	}

	void AntimonyEditor::_loadSBMLString(const char * c)
	{
		fToS.loadSBMLString(c);
	}
	void AntimonyEditor::_loadAntimonyString(const char * c)
	{
		fToS.loadAntimonyString(c);
	}
	void AntimonyEditor::_loadSBMLFile(const char * c)
	{
		fToS.loadSBMLFile(c);
	}
	void AntimonyEditor::_loadAntimonyFile(const char * c)
	{
		fToS.loadAntimonyFile(c);
	}
	char* AntimonyEditor::_getSBMLString(Array a)
	{
		return fToS.getSBMLString(a);
	}
	char* AntimonyEditor::_getAntimonyString(Array a)
	{
		return fToS.getAntimonyString(a);
	}
	void AntimonyEditor::_writeSBMLFile(Array a,const char* c)
	{
		fToS.writeSBMLFile(a,c);
	}
	void AntimonyEditor::_writeAntimonyFile(Array a,const char* c)
	{
		fToS.writeAntimonyFile(a,c);
	}

}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::AntimonyEditor * antimonyEditor = new Tinkercell::AntimonyEditor;
	main->addTool(antimonyEditor);

}
