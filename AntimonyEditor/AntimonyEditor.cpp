/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Provides a text window where C code can be written and run dynamically
 
****************************************************************************/

#include "TextEditor.h"
#include "NetworkWindow.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "OutputWindow.h"
#include "NodesTree.h"
#include "ConnectionsTree.h"
#include "AntimonyEditor.h"
#include <QToolButton>
#include <QFile>
#include <QPair>
#include <QRegExp>
#include <QtDebug>
#include <QSemaphore>

//Antimony headers
#include "antimony_api.h"

namespace Tinkercell
{	
	AntimonyEditor::AntimonyEditor() : Tool(tr("Antimony script parser"))
	{
	}
	
	bool AntimonyEditor::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(textChanged(TextEditor *, const QString&, const QString&, const QString&)),
					this,SLOT(textChanged(TextEditor *, const QString&, const QString&, const QString&)));
			connect(mainWindow,SIGNAL(lineChanged(TextEditor *, int, const QString&)),
					this,SLOT(lineChanged(TextEditor *, int, const QString&)));
			connect(mainWindow,SIGNAL(windowOpened(NetworkWindow*)),
					this,SLOT(windowOpened(NetworkWindow*)));
			mainWindow->contextEditorMenu.addAction(tr("To Graphical Mode"),this,SLOT(insertModule()));
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
			if ((handle = getHandle(textItems[i])) && handle->isA(tr("Module")))
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
		if (win && win->textEditor)
		{
			AntimonySyntaxHighlighter * as = new AntimonySyntaxHighlighter(win->textEditor->document());
			connect(this,SIGNAL(validSyntax(bool)),as,SLOT(setValid(bool)));
		}
	}
	
	void AntimonyEditor::parseAndInsert(TextEditor * editor)
	{
		if (!editor) return;
		
		QString modelString = editor->toPlainText() + tr("\n");
		
		QList<TextItem*> itemsToInsert = parse(modelString);
		
		if (!itemsToInsert.isEmpty())
		{
			//OutputWindow::message(QString::number(itemsToInsert.size()) + tr(" items inserted"));
			editor->setItems(itemsToInsert);
		}		
	}
	
	QList<TextItem*> AntimonyEditor::parse(const QString& modelString)
	{
		QString filename = MainWindow::userHome() + tr("/antimony.txt");
		
		QFile antfile(filename);
		if (!antfile.open(QFile::WriteOnly | QFile::Text)) {
			OutputWindow::error(tr("Could not write to the user directory. Check whether you have permission to write to the specified TinkerCell user directory."));
			return QList<TextItem*>();
		}
		antfile.write(modelString.toAscii().data());
		antfile.close();
		
		//parse
		long ok = loadFile(filename.toAscii().data());
		
		if (ok < 0)
		{
			if (modelString.contains(tr("end")))
				OutputWindow::error(tr(getLastError()));
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
			if (partsTree->nodeFamilies.contains(tr("Species")))
				speciesFamily = partsTree->nodeFamilies.value(tr("Species"));
			if (partsTree->nodeFamilies.contains(tr("Module")))
				moduleFamily = partsTree->nodeFamilies.value(tr("Module"));
		}
		
		if (!biochemicalFamily || !speciesFamily || !moduleFamily)
		{
			OutputWindow::error(tr("No parts and connection information"));
			return QList<TextItem*>();
		}
		
		//load		
		
		int nummods = (int)getNumModules();
		char** modnames = getModuleNames();
		QList<TextItem*> itemsToInsert;
		
		for (int i=0; i < nummods; ++i)
		{
			QList<ItemHandle*> handlesInModule;
			ItemHandle * moduleHandle = new NodeHandle(moduleFamily);
			NodeTextItem * moduleText = new NodeTextItem;
			setHandle(moduleText,moduleHandle);
			itemsToInsert += moduleText;
			
			QHash<QString,NodeHandle*> speciesItems;
		
			char * moduleName = modnames[i];
			moduleHandle->name = QString(moduleName);
			
			char ***leftrxnnames = getReactantNames(moduleName);
			char ***rightrxnnames = getProductNames(moduleName);
			char **rxnnames = getSymbolNamesOfType(moduleName,allReactions);
			char **rxnrates = getReactionRates(moduleName);
		  
			double **leftrxnstoichs = getReactantStoichiometries(moduleName);
			double **rightrxnstoichs = getProductStoichiometries(moduleName);
			
			int numrxn = (int)getNumReactions(moduleName);
			
			for (int rxn=0; rxn < numrxn; rxn++)
			{
				int numReactants = getNumReactants(moduleName,rxn);
				int numProducts = getNumProducts(moduleName,rxn);
				
				QList<NodeTextItem*> nodesIn, nodesOut;
				DataTable<qreal> stoichiometry;
				DataTable<QString> rate;
				
				stoichiometry.resize(1,numReactants + numProducts);
				rate.resize(1,1);
				ItemHandle * reactionHandle = new ConnectionHandle(biochemicalFamily);
				handlesInModule << reactionHandle;
				
				reactionHandle->name = rxnnames[rxn];
				
				for (int var=0; var<numReactants; ++var)
				{
					NodeHandle * handle = 0;
					if (!speciesItems.contains(tr(leftrxnnames[rxn][var])))
					{
						handle = new NodeHandle(speciesFamily);
						handlesInModule << handle;
						handle->name = tr(leftrxnnames[rxn][var]);
						speciesItems[tr(leftrxnnames[rxn][var])] = handle;
						NodeTextItem * n = new NodeTextItem(handle);
						nodesIn << n;
						itemsToInsert  += n;
					}
					else
					{
						handle = speciesItems[tr(leftrxnnames[rxn][var])];
					}
					stoichiometry.colName(var) = leftrxnnames[rxn][var];
					stoichiometry.value(0,var) = - leftrxnstoichs[rxn][var];
				}
				
				for (int var=0; var<numProducts; var++)
				{
					NodeHandle * partHandle = 0; 
					if (!speciesItems.contains(tr(rightrxnnames[rxn][var])))
					{
						partHandle = new NodeHandle(speciesFamily);
						handlesInModule << partHandle;
						partHandle->name = tr(rightrxnnames[rxn][var]);
						speciesItems[tr(rightrxnnames[rxn][var])] = partHandle;
						NodeTextItem * n = new NodeTextItem(partHandle);
						nodesOut << n;
						itemsToInsert += n;
					}
					else
					{
						partHandle = speciesItems[tr(rightrxnnames[rxn][var])];
					}
					
					stoichiometry.colName(var+numReactants) = rightrxnnames[rxn][var];
					stoichiometry.value(0,var+numReactants) += rightrxnstoichs[rxn][var];
				}
				
				QString srate = tr(rxnrates[rxn]);
				rate.value(0,0) = srate;
				reactionHandle->data->textData[tr("Rates")] = rate;
				reactionHandle->data->numericalData[tr("Stoichiometry")] = stoichiometry;
				
				ConnectionTextItem * c = new ConnectionTextItem(reactionHandle);
				c->nodesIn = nodesIn;
				c->nodesOut = nodesOut;
				itemsToInsert += c;
				
				int numParams = (int)getNumSymbolsOfType(moduleName,constFormulas);
				char ** paramNames = getSymbolNamesOfType(moduleName,constFormulas);
				char ** paramValues = getSymbolEquationsOfType(moduleName,constFormulas);
				
				DataTable<qreal> paramsTable;
				for (int j=0; j < numParams; ++j)
				{
					bool ok;
					qreal x = QString(paramValues[j]).toDouble(&ok);
					if (ok)
					{
						paramsTable.value(tr(paramNames[j]),0) = x;
						RenameCommand::findReplaceAllHandleData(handlesInModule,tr(paramNames[j]),moduleHandle->name + tr(".") + tr(paramNames[j]));
					}
				}
				moduleHandle->data->numericalData[tr("Numerical Attributes")] = paramsTable;
				
				int numAssignments = (int)getNumSymbolsOfType(moduleName,varFormulas);
				char ** assignmentNames = getSymbolNamesOfType(moduleName,varFormulas);
				char ** assignmentValues = getSymbolEquationsOfType(moduleName,varFormulas);
				
				DataTable<QString> assgnsTable;
				for (int j=0; j < numAssignments; ++j)
				{
					QString x(assignmentValues[j]);
					assgnsTable.value(tr(assignmentNames[j]),0) = x;
					RenameCommand::findReplaceAllHandleData(handlesInModule,tr(assignmentNames[j]),moduleHandle->name + tr(".") + tr(assignmentNames[j]));
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
				
				for (int j=0; j < handlesInModule.size(); ++j)
					if (handlesInModule[j])
					{
						handlesInModule[j]->setParent(moduleHandle);
						RenameCommand::findReplaceAllHandleData(handlesInModule,handlesInModule[j]->name,handlesInModule[j]->fullName());
					}
			}
		}
		
		freeAll();
		return itemsToInsert;
	}
	

	void AntimonyEditor::textChanged(TextEditor * editor, const QString&, const QString&, const QString&)
	{
		parseAndInsert(editor);
	}

	void AntimonyEditor::lineChanged(TextEditor *, int, const QString&)
	{
	}
	
	QList<TextItem*> AntimonyEditor::clone(const QList<TextItem*>& items)
	{
		QList<TextItem*> newItems;
		QList<ItemHandle*> oldHandles;
		
		for (int i=0; i < items.size(); ++i)
		{
			oldHandles << getHandle(items[i]);
			if (items[i])
				newItems << items[i]->clone();
			else
				newItems << 0;
		}
		
		ItemHandle * handle = 0, * handle2 = 0;
		for (int i=0; i< items.size(); ++i)
		{
			handle = getHandle(items[i]);
			if (newItems[i])
				if (handle)
					setHandle(newItems[i],handle->clone());
				else
					setHandle(newItems[i],0);
		}
		
		for (int i=0; i< newItems.size(); ++i)
		{
			handle = getHandle(items[i]);
			handle2 = getHandle(newItems[i]);
			if (handle && handle->parent && handle2)
			{
				int j = oldHandles.indexOf(handle->parent);
				if (j >= 0)
				{
					handle2->setParent(getHandle(newItems[j]));
				}
			}
		}
		
		for (int i=0; i < items.size(); ++i)
		{
			if (items[i] && items[i]->asNode() && newItems[i]->asNode())
			{
				NodeTextItem* newNode = newItems[i]->asNode();
				newNode->connections.clear();
				QList<ConnectionTextItem*>& connections = items[i]->asNode()->connections;
				for(int j=0; j < connections.size(); ++j)
					for (int k=0; k < items.size(); ++k)
						if (connections[j] == items[k] && newItems[k]->asConnection())
						{
							newNode->connections << newItems[k]->asConnection();
						}
			}
			else
			if (items[i] && items[i]->asConnection() && newItems[i]->asConnection())
			{
				ConnectionTextItem* newConnection = newItems[i]->asConnection();
				newConnection->nodesIn.clear();
				newConnection->nodesOut.clear();
				newConnection->nodesOther.clear();
				QList<NodeTextItem*>& nodesIn = items[i]->asConnection()->nodesIn;
				QList<NodeTextItem*>& nodesOut = items[i]->asConnection()->nodesOut;
				QList<NodeTextItem*>& nodesOther = items[i]->asConnection()->nodesOther;
				for(int j=0; j < nodesIn.size(); ++j)
					for (int k=0; k < items.size(); ++k)
						if (nodesIn[j] == items[k] && newItems[k]->asNode())
							newConnection->nodesIn << newItems[k]->asNode();
				for(int j=0; j < nodesOut.size(); ++j)
					for (int k=0; k < items.size(); ++k)
						if (nodesOut[j] == items[k] && newItems[k]->asNode())
							newConnection->nodesOut << newItems[k]->asNode();
				for(int j=0; j < nodesOther.size(); ++j)
					for (int k=0; k < items.size(); ++k)
						if (nodesOther[j] == items[k] && newItems[k]->asNode())
							newConnection->nodesOther << newItems[k]->asNode();
						
			}
		}
		return newItems;
	}

}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::AntimonyEditor * antimonyEditor = new Tinkercell::AntimonyEditor;
	main->addTool(antimonyEditor);

}
