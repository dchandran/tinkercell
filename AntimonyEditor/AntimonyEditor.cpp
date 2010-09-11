/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 A tool that allows users to construct models using Antimony scripts in the TextEditor

****************************************************************************/

#include <QClipboard>
#include "TextEditor.h"
#include "NetworkHandle.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "NodesTree.h"
#include "ConnectionsTree.h"
#include "AntimonyEditor.h"
#include "ModelSummaryTool.h"
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
			connect(mainWindow,SIGNAL(copyItems(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&)),this,SLOT(copyItems(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&)));
			connect(mainWindow,SIGNAL(networkOpened(NetworkHandle*)),this,SLOT(networkOpened(NetworkHandle*)));
			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			toolLoaded(0);

		}
		return false;
	}

	void AntimonyEditor::networkOpened(NetworkHandle * win)
	{
		if (win && win->currentTextEditor() && TextParser::currentParser() == this)
		{
			AntimonySyntaxHighlighter * as = new AntimonySyntaxHighlighter(win->currentTextEditor()->document());
			connect(this,SIGNAL(validSyntax(bool)),as,SLOT(setValid(bool)));

			QToolButton * button = new QToolButton;
			button->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
			button->setIcon(QIcon(":/images/antimony.png"));
			button->setIconSize(QSize(30,30));
			button->setText(tr("Compile script"));
			button->setToolTip(tr("interpret script using Antimony language"));
			connect(button,SIGNAL(pressed()),this,SLOT(parse()));
			win->currentTextEditor()->addSideBarWidget(button);

			button = new QToolButton;
			button->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
			button->setIcon(QIcon(":/images/module.png"));
			button->setIconSize(QSize(30,30));
			button->setText(tr("Export modules"));
			button->setToolTip(tr("export module(s) to the last graphics window"));
			//connect(button,SIGNAL(pressed()),this,SLOT(insertModule()));
			win->currentTextEditor()->addSideBarWidget(button);
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

		QList<ItemHandle*> itemsToInsert = parse(modelString);

		if (!itemsToInsert.isEmpty())
		{
			editor->setItems(itemsToInsert);
		}
	}

	QList<ItemHandle*> AntimonyEditor::parse(const QString& modelString)
	{
		long ok = loadString(modelString.toAscii().data());

		if (ok < 0)
		{
			if (modelString.contains(tr("end")))
				if (console())
                    console()->error(tr(getLastError()));
			emit validSyntax(false);
			return QList<ItemHandle*>();
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
			biochemicalFamily = connectionsTree->getFamily(tr("Biochemical"));
			speciesFamily = partsTree->getFamily(tr("Molecule"));
			moduleFamily = partsTree->getFamily(tr("Module"));
		}

		if (!biochemicalFamily || !speciesFamily || !moduleFamily)
		{
			if (console())
                console()->error(tr("No parts and connection information"));
			return QList<ItemHandle*>();
		}

		//load

		int nummods = (int)getNumModules();
		char** modnames = getModuleNames();

		QList<ItemHandle*> itemsToInsert;

		for (int i=0; i < nummods; ++i)
		{
			char * moduleName = modnames[i];
			ItemHandle * moduleHandle = 0;

			moduleHandle = new NodeHandle(moduleFamily);
			moduleHandle->name = QString(moduleName);
			itemsToInsert += moduleHandle;

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

				QList<NodeHandle*> nodesIn, nodesOut;
				NumericalDataTable reactants, products;
				TextDataTable rate;
				QStringList colNames;

				for (int var=0; var<numReactants; ++var)
				{
					if (!colNames.contains(tr(leftrxnnames[rxn][var])))
					{
						colNames << tr(leftrxnnames[rxn][var]);
					}
				}
				
				reactants.resize(1,colNames.size());
				reactants.setColNames(colNames);
				colNames.clear();
				
				for (int var=0; var<numProducts; ++var)
				{
					if (!colNames.contains(tr(rightrxnnames[rxn][var])))
					{
						colNames << tr(rightrxnnames[rxn][var]);
					}
				}

				products.resize(1,colNames.size());
				products.setColNames(colNames);

				for (int j=0; j < colNames.size(); ++j)
				{
					products.value(0,j) = 0;
				}

				rate.resize(1,1);
				ConnectionHandle * reactionHandle = new ConnectionHandle(biochemicalFamily);
				handlesInModule << reactionHandle;

				reactionHandle->name = rxnnames[rxn];
				reactants.rowName(0) = products.rowName(0) = reactionHandle->name;
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
						nodesIn << handle;
						itemsToInsert  += handle;
					}
					else
					{
						handle = speciesItems[tr(leftrxnnames[rxn][var])];
					}
					reactants.value(0,tr(leftrxnnames[rxn][var])) = leftrxnstoichs[rxn][var];
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
						nodesOut << partHandle;
						itemsToInsert += partHandle;
					}
					else
					{
						partHandle = speciesItems[tr(rightrxnnames[rxn][var])];
					}

					products.value(0,tr(rightrxnnames[rxn][var])) += rightrxnstoichs[rxn][var];
				}

				QString srate = tr(rxnrates[rxn]);
				rate.rowName(0) = reactionHandle->name;
				rate.colName(0) = tr("rate");
				rate.value(0,0) = srate;
				reactionHandle->textDataTable(tr("Rate equations")) = rate;
				reactionHandle->numericalDataTable(tr("Reactant stoichiometries")) = reactants;
				reactionHandle->numericalDataTable(tr("Product stoichiometries")) = products;

				for (int var=0; var < nodesIn.size(); ++var)
					reactionHandle->addNode(nodesIn[var],-1);
				
				for (int var=0; var < nodesOut.size(); ++var)
					reactionHandle->addNode(nodesOut[var],1);
				itemsToInsert += reactionHandle;
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
					speciesItems[s]->numericalData(tr("Initial Value"),speciesFamily->measurementUnit.name,speciesFamily->measurementUnit.property) = x;
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
					speciesItems[s]->numericalDataTable(tr("Initial Value")).rowName(0) = tr("concentration");
					speciesItems[s]->numericalDataTable(tr("Initial Value")).colName(0) = tr("uM");
					speciesItems[s]->numericalData(tr("Fixed")) = 1;
					speciesItems[s]->numericalDataTable(tr("Fixed")).rowName(0) = tr("fix");
					speciesItems[s]->numericalDataTable(tr("Fixed")).colName(0) = tr("value");
				}
			}

			int numAssignments = (int)getNumSymbolsOfType(moduleName,varFormulas);
			char ** assignmentNames = getSymbolNamesOfType(moduleName,varFormulas);
			char ** assignmentValues = getSymbolEquationsOfType(moduleName,varFormulas);

			TextDataTable assgnsTable;
			QList<ItemHandle*> handlesInModule2 = handlesInModule;
			handlesInModule2 << moduleHandle;

			for (int j=0; j < numAssignments; ++j)
			{
				QString x(assignmentValues[j]);
				assgnsTable.value(tr(assignmentNames[j]),0) = x;
				symbolsInModule << tr(assignmentNames[j]);
				RenameCommand::findReplaceAllHandleData(handlesInModule2,tr(assignmentNames[j]),moduleHandle->name + tr(".") + tr(assignmentNames[j]));
			}

			moduleHandle->textDataTable(tr("Assignments")) = assgnsTable;

			int numEvents = (int)getNumEvents(moduleName);
			char ** eventNames = getEventNames(moduleName);

			TextDataTable eventsTable;
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
			moduleHandle->textDataTable(tr("Events")) = eventsTable;

			int numParams = (int)getNumSymbolsOfType(moduleName,constFormulas);
			char ** paramNames = getSymbolNamesOfType(moduleName,constFormulas);
			char ** paramValues = getSymbolEquationsOfType(moduleName,constFormulas);

			NumericalDataTable paramsTable;
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
					moduleHandle->textDataTable(tr("Assignments")).value(tr(paramNames[j]),0) = paramValues[j];
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

			moduleHandle->numericalDataTable(tr("Numerical Attributes")) = paramsTable;

			for (int j=0; j < handlesInModule.size(); ++j)
				if (handlesInModule[j])
				{
					handlesInModule[j]->setParent(moduleHandle);
					RenameCommand::findReplaceAllHandleData(handlesInModule2,handlesInModule[j]->name,handlesInModule[j]->fullName());
				}

			if (handlesInModule.isEmpty())
			{
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

	QList<ItemHandle*> AntimonyEditor::clone(const QList<ItemHandle*>& items)
	{
		return cloneHandles(items);
	}

	void AntimonyEditor::toolLoaded(Tool*)
	{
		static bool connected1 = false;
		if (connected1) return;

		if (mainWindow && mainWindow->tool(tr("Model Summary")))
		{
			connected1 = true;
			QWidget * widget = mainWindow->tool(tr("Model Summary"));
			ModelSummaryTool * modelSummary = static_cast<ModelSummaryTool*>(widget);
			//connect(modelSummary,SIGNAL(displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)),
					//this,SLOT(displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)));
		}

	}

	void AntimonyEditor::copyItems(GraphicsScene * scene, QList<QGraphicsItem*>& , QList<ItemHandle*>& handles)
	{
		if (scene && handles.size() > 0)
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

		QString name, rate, species;
		QStringList lhs, rhs;
		for (int j=0; j < childHandles.size(); ++j)
			if (childHandles[j])
			{
				name = childHandles[j]->fullName(tr("_"));
				if (childHandles[j]->hasNumericalData(tr("Stoichiometry")) &&
					 childHandles[j]->hasTextData(tr("Rate equations")))
					{
						NumericalDataTable& N = childHandles[j]->numericalDataTable(tr("Stoichiometry"));
						TextDataTable& V = childHandles[j]->textDataTable(tr("Rate equations"));
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
					TextDataTable& assigns = childHandles[j]->textDataTable(tr("Assignments"));

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
					NumericalDataTable& params = childHandles[j]->numericalDataTable(tr("Numerical Attributes"));

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


}

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::AntimonyEditor * antimonyEditor = new Tinkercell::AntimonyEditor;
	main->addTool(antimonyEditor);

}
