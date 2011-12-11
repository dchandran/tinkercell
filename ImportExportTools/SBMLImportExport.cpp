
#include <iostream>
#include <QDesktopServices>
#include <QFileDialog>
#include "sbml/SBMLReader.h"
#include "sbml/SBMLWriter.h"
#include "sbml/SBMLDocument.h"
#include "sbml/Species.h"
#include "sbml/SpeciesReference.h"
#include "sbml/ListOf.h"
#include "sbml/Model.h"
#include "sbml/Rule.h"
#include "SBMLImportExport.h"
#include "BasicInformationTool.h"
#include "StoichiometryTool.h"
#include "TextEditor.h"
#include "LoadSaveTool.h"
#include "ConsoleWindow.h"
#include "SimulationThread.h"
#include "AntimonyEditor.h"
#include "OctaveExporter.h"
#include "UndoCommands.h"
#include "CopasiExporter.h"
#include "antimony_api.h"

using namespace std;

namespace Tinkercell
{
	SBMLImportExport::SBMLImportExport() : Tool("SBML Tool","Export")
	{
		modelNeedsUpdate = true;
		sbmlDocument = 0;
		SBMLImportExport::fToS = new SBMLImportExport_FtoS;
		SBMLImportExport::fToS->setParent(this);
		connect(fToS,SIGNAL(exportSBML(QSemaphore*, const QString&)),this,SLOT(exportSBML(QSemaphore*, const QString&)));
		connect(fToS,SIGNAL(importSBML(QSemaphore*, const QString&)),this,SLOT(importSBML(QSemaphore*, const QString&)));
		connect(fToS,SIGNAL(exportAntimony(QSemaphore*, const QString&)),this,SLOT(exportAntimony(QSemaphore*, const QString&)));
		connect(fToS,SIGNAL(importAntimony(QSemaphore*, const QString&)),this,SLOT(importAntimony(QSemaphore*, const QString&)));
		connect(fToS,SIGNAL(exportMath(QSemaphore*, const QString&)),this,SLOT(exportMath(QSemaphore*, const QString&)));
		connect(fToS,SIGNAL(exportSBMLString(QSemaphore*, QString*)),this,SLOT(exportSBMLString(QSemaphore*, QString*)));
		connect(fToS,SIGNAL(exportAntimonyString(QSemaphore*, QString*)),this,SLOT(exportAntimonyString(QSemaphore*, QString*)));
	}

	SBMLImportExport::~SBMLImportExport()
	{
		if (sbmlDocument)
			delete (sbmlDocument);
	}

	bool SBMLImportExport::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);	
		if (!mainWindow) return false;
	
		GlobalSettings::OPEN_FILE_EXTENSIONS << "SBML" << "sbml";

		if (mainWindow->fileMenu)
		{
			QList<QAction*> actions = mainWindow->fileMenu->actions();

			QAction * targetAction = 0;
			QMenu * exportmenu = 0;//, * importmenu = 0;
		
			for (int i=0; i < actions.size(); ++i)
				if (actions[i] && actions[i]->menu())
				{
					if (actions[i]->text() == tr("&Export"))
					{
						exportmenu = actions[i]->menu();
						targetAction = actions[i];
					}
					else
						if (actions[i]->text() == tr("&Import"))
						{
							//importmenu = actions[i]->menu();
							targetAction = actions[i];
						}
				}
		
			if (!exportmenu)
			{
				for (int i=0; i < actions.size(); ++i)
					if (actions[i] && actions[i]->text() == tr("&Close page"))
					{
						exportmenu = new QMenu(tr("&Export"));
						//importmenu = new QMenu(tr("&Import"));
						//mainWindow->fileMenu->insertMenu(actions[i],importmenu);
						mainWindow->fileMenu->insertMenu(actions[i],exportmenu);
					}
			}

			if (!exportmenu)
			{
				exportmenu = new QMenu(tr("&Export"));
				mainWindow->fileMenu->insertMenu(targetAction,exportmenu);
			}
		
			if (exportmenu)
			{
				//importmenu->addAction(tr("load SBML file"),this,SLOT(loadSBMLFile()));
				exportmenu->addAction(tr("SBML"),this,SLOT(saveSBMLFile()));
			}
		}

		connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
		connect(mainWindow,SIGNAL(loadNetwork(const QString&, bool*)),this,SLOT(loadNetwork(const QString&, bool*)));
		connect(main,SIGNAL(historyChanged(int)),this, SLOT(historyChanged(int)));
		connect(main,SIGNAL(windowChanged(NetworkWindow*,NetworkWindow*)),this, SLOT(windowChanged(NetworkWindow*,NetworkWindow*)));
		connect(mainWindow,SIGNAL(getItemsFromFile(QList<ItemHandle*>&, QList<QGraphicsItem*>&, const QString&,ItemHandle*)),
						this,SLOT(getItemsFromFile(QList<ItemHandle*>&, QList<QGraphicsItem*>&, const QString&,ItemHandle*)));
		return true;
	}

	typedef void (*tc_SBML_api)(
			void (*exportSBMLFile)(const char *),
			void (*importSBMLString)(const char*),
			void (*exportAntimonyFile)(const char *),
			void (*importAntimonyString)(const char*),
			void (*exportMathFile)(const char *),
			const char* (*exportSBMLString)(),
			const char* (*exportAntimonyString)()
			);

	void SBMLImportExport::setupFunctionPointers( QLibrary * library)
	{
		tc_SBML_api f = (tc_SBML_api)library->resolve("tc_SBML_api");
		if (f)
		{
			f(	&exportSBMLFile, &importSBMLString, &exportAntimonyFile, &importAntimonyString, &exportMathFile, &_exportSBMLString, &_exportAntimonyString );
		}
	}

	void SBMLImportExport::loadSBMLFile()
	{
		QString file = QFileDialog::getOpenFileName (this, tr("Load SBML file"));
		if (file.isNull() || file.isEmpty()) return;
	
		importSBML(0, file);
	}

	void SBMLImportExport::saveSBMLFile()
	{
		QString file = QFileDialog::getSaveFileName (this, tr("Save SBML file"), homeDir());
		if (file.isNull() || file.isEmpty()) return;
	
		exportSBML(0, file);
		QDesktopServices::openUrl(QUrl(file));
	}

	void SBMLImportExport::historyChanged(int)
	{
		modelNeedsUpdate = true;
	}

	void SBMLImportExport::windowChanged(NetworkWindow*,NetworkWindow*)
	{
		modelNeedsUpdate = true;
	}


	/*******************************************
		C Interface
	********************************************/

	SBMLImportExport_FtoS * SBMLImportExport::fToS = 0;

	void SBMLImportExport::exportSBMLFile(const char * s)
	{
		return fToS->exportSBMLFile(s);
	}

	void SBMLImportExport::importSBMLString(const char* s)
	{
		fToS->importSBMLString(s);
	}

	void SBMLImportExport::exportAntimonyFile(const char * s)
	{
		return fToS->exportAntimonyFile(s);
	}

	const char * SBMLImportExport::_exportAntimonyString()
	{
		return fToS->exportAntimonyString();
	}

	const char * SBMLImportExport::_exportSBMLString()
	{
		return fToS->exportSBMLString();
	}

	void SBMLImportExport::importAntimonyString(const char* s)
	{
		fToS->importAntimonyString(s);
	}

	void SBMLImportExport::exportMathFile(const char * s)
	{
		return fToS->exportMathFile(s);
	}


	void SBMLImportExport_FtoS::exportSBMLFile(const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit exportSBML(s,ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
	}

	const char * SBMLImportExport_FtoS::exportSBMLString()
	{
		QSemaphore * s = new QSemaphore(1);
		QString c;
		s->acquire();
		emit exportSBMLString(s,&c);
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(c);
	}

	void SBMLImportExport_FtoS::importSBMLString(const char* c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit importSBML(s,ConvertValue(c));
		s->acquire();
		s->release();
	}

	void SBMLImportExport_FtoS::exportAntimonyFile(const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit exportAntimony(s,ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
	}

	const char * SBMLImportExport_FtoS::exportAntimonyString()
	{
		QSemaphore * s = new QSemaphore(1);
		QString c;
		s->acquire();
		emit exportAntimonyString(s,&c);
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(c);
	}

	void SBMLImportExport_FtoS::importAntimonyString(const char* c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit importAntimony(s,ConvertValue(c));
		s->acquire();
		s->release();
	}

	void SBMLImportExport_FtoS::exportMathFile(const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit exportMath(s,ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
	}

	void SBMLImportExport::exportMath(QSemaphore * sem, const QString & str)
	{
		QWidget * tool = mainWindow->tool("Octave Export Tool");
		if (tool)
		{
			OctaveExporter * octTool = static_cast<OctaveExporter*>(tool);
			octTool->exportOctave(str);	
		}
		if (sem)
			sem->release();
	}

	void SBMLImportExport::exportAntimony(QSemaphore * sem, const QString & str)
	{
		if (sbmlDocument)
		{
			loadString(writeSBMLToString(sbmlDocument));
			QString text(getAntimonyString(NULL));
			QFile file(str);
			if (file.open(QFile::WriteOnly | QFile::Text))
			{
				file.write(text.toUtf8());
				file.close();
			}
		}

		if (sem)
			sem->release();
	}

	void SBMLImportExport::exportSBML(const QString & str)
	{
		if (modelNeedsUpdate)
			updateSBMLModel();

		if (sbmlDocument)
			writeSBML (sbmlDocument, ConvertValue(str) );
/*
		QWidget * tool = mainWindow->tool("COPASI");
		if (tool)
		{
			CopasiExporter * copasi = static_cast<CopasiExporter*>(tool);
			copasi->exportSBML(str);	
		}
*/
	}

	void SBMLImportExport::exportSBML(QSemaphore * sem, const QString & str)
	{
		exportSBML(str);
		if (sem)
			sem->release();
	}
	
	void SBMLImportExport::exportSBMLString(QSemaphore * sem, QString * str)
	{
		if (modelNeedsUpdate)
			updateSBMLModel();

		if (sbmlDocument && str)
			(*str) = QString(writeSBMLToString(sbmlDocument));
/*
		QWidget * tool = mainWindow->tool("COPASI");
		if (tool && str)
		{
			CopasiExporter * copasi = static_cast<CopasiExporter*>(tool);

			QString filename = tempDir() + tr("/temp.sbml");
			copasi->exportSBML(filename);	
			QFile file(filename);
			if (file.open(QFile::ReadOnly | QFile::Text))
			{
				(*str) = file.readAll();
				file.close();
			}
		}
*/			
		if (sem)
			sem->release();
	}
	
	void SBMLImportExport::exportAntimonyString(QSemaphore * sem, QString * str)
	{
		if (modelNeedsUpdate)
			updateSBMLModel();

		if (sbmlDocument && str)
		{
			loadString(writeSBMLToString(sbmlDocument));
			QString text(getAntimonyString(NULL));
			
			(*str) = text;
		}
			
		if (sem)
			sem->release();
	}

	void SBMLImportExport::loadNetwork(const QString& filename, bool * b)
	{
		if (b && (*b)) return;
		(*b) = importSBML(0,filename);
	}

	void SBMLImportExport::importAntimony(QSemaphore * sem, const QString& str)
	{
		QWidget * tool = mainWindow->tool("Antimony Parser");
		if (tool)
		{
			AntimonyEditor * antEdit = static_cast<AntimonyEditor*>(tool);
			antEdit->loadNetwork(str);	
		}
		if (sem)
			sem->release();	
	}

	bool SBMLImportExport::importSBML(QSemaphore * sem, const QString& str)
	{
		bool b = false;
		QWidget * tool = mainWindow->tool("Antimony Parser");
		if (tool)
		{
			AntimonyEditor * antEdit = static_cast<AntimonyEditor*>(tool);
			antEdit->loadNetwork(str, &b);
		}
		if (sem)
			sem->release();	

		return b;
	}

	/***************************************************
		Move SBML to TinkerCell format and vice versa
	*****************************************************/

	void SBMLImportExport::updateSBMLModel()
	{
		if (sbmlDocument)
			delete sbmlDocument;
		sbmlDocument = 0;
		sbmlDocument = exportSBML();
		modelNeedsUpdate = false;
	}

	void SBMLImportExport::getItemsFromFile(QList<ItemHandle*>& handles, QList<QGraphicsItem*>&, const QString& filename,ItemHandle * root)
	{
		if (!handles.isEmpty()) return;

		if (!root && currentWindow())
			root = currentWindow()->handle;

		handles = importSBML(filename, root);
	}

	QList<ItemHandle*> SBMLImportExport::importSBML(const QString& sbml_text, ItemHandle * global)
	{
		QList<ItemHandle*> handles;
	
		ConnectionFamily * defaultReactionFamily = LoadSaveTool::getConnectionFamily(tr("biochemical reaction"));
		NodeFamily * defaultSpeciesFamily = LoadSaveTool::getNodeFamily(tr("molecule"));

		if (!defaultSpeciesFamily || !defaultReactionFamily)
		{
			return QList<ItemHandle*>();
		}

		SBMLReader * sbmlreader = new SBMLReader;
		SBMLDocument * doc;
		std::string s1,s2;
	
		if (QFile::exists(sbml_text))
			doc = sbmlreader->readSBML(ConvertValue(sbml_text));
		else
			doc = sbmlreader->readSBMLFromString(ConvertValue(sbml_text)); 
	
		if (!doc || doc->getNumErrors() > 0)
		{
			//if (console())
				//console()->error(tr("Failed to load SBML using libsbml"));
		}
		else
		{
			if (!global)
			{
				global = new ItemHandle("");
				handles << global;
			}

			Model * model = doc->getModel();
			ListOfParameters * params = model->getListOfParameters();
			ListOfReactions * reacs = model->getListOfReactions();
			ListOfSpecies * species = model->getListOfSpecies();
			ListOfSpeciesTypes * types = model->getListOfSpeciesTypes();
			ListOfEvents * events = model->getListOfEvents();
			ListOfRules * rules = model->getListOfRules();

			if (events)
			{
				TextDataTable & globalEvents = global->textDataTable("Events");
				for (int i=0; i < events->size(); ++i)
				{
					Event * e = events->get(i);
					s1 = SBML_formulaToString( e->getTrigger()->getMath() );
					QString trigger( s1.c_str() );
					QStringList responses;
					ListOfEventAssignments * eventAssn = e->getListOfEventAssignments();
					for (int j=0; j < eventAssn->size(); ++j)
					{
						s1 = eventAssn->get(j)->getVariable();
						s1.append("=");
						s1.append( SBML_formulaToString( eventAssn->get(j)->getMath() ) );
						responses << tr( s1.c_str() );
					}

					if (globalEvents.value(trigger,0).isEmpty())
						globalEvents.value(trigger,0) = responses.join(";");
					else
						globalEvents.value(trigger,0) += tr(";") + responses.join(";");
				}
			}

			if (rules)
			{
				TextDataTable & globalAssignments = global->textDataTable("Assignments");
				for (int i=0; i < rules->size(); ++i)
				{
					Rule * r = rules->get(i);
					if (r->isAssignment())
					{
						AssignmentRule * ar  = (AssignmentRule*)r;
						s1 = ar->getVariable();
						s2 = ar->getFormula();
						globalAssignments.value( tr(s1.c_str()) , 0 ) = tr(s2.c_str());
					}
				}
			}

			if (species)
				for (int i=0; i < species->size(); ++i)
				{
					s1 = species->get(i)->getId();
					QString name(s1.c_str());
					ItemHandle * h = new NodeHandle(defaultSpeciesFamily, name);
					handles << h;
					
					double d = 0.0;
				
					if (species->get(i)->isSetInitialAmount())
						d = species->get(i)->getInitialAmount();
					else
					if (species->get(i)->isSetInitialConcentration())
						d = species->get(i)->getInitialConcentration();
				
					h->numericalData("Initial Value") = d;
					h->numericalData("Fixed") = 0;
					if (species->get(i)->getConstant() || species->get(i)->getBoundaryCondition())
						h->numericalData("Fixed") = 1;
				}

			if (params)
			{
				NumericalDataTable & globalParams = global->numericalDataTable("Parameters");
				for (int i=0; i < params->size(); ++i)
				{
					s1 = params->get(i)->getId();
					globalParams.value(tr(s1.c_str()),0) = params->get(i)->getValue();
				}
			}

			int numReacs = 0;
		
			if (reacs)
				numReacs = reacs->size();

			for (int i=0; i < numReacs; ++i)
			{
				Reaction * r = reacs->get(i);
				s1 = r->getId();
			
				ConnectionHandle * h = new ConnectionHandle(defaultReactionFamily,tr(s1.c_str()));
				handles << h;
			
				h->textData("Rate equations") = tr(r->getKineticLaw()->getFormula().c_str());
				NumericalDataTable & reacStoic = h->numericalDataTable("Reactant stoichiometries"),
									& prodStoic = h->numericalDataTable("Product stoichiometries"); 

				ListOfSpeciesReferences * reactants = r->getListOfReactants(),
										* products  = r->getListOfProducts();

				for (int k=0; k < reactants->size(); ++k)
					if (reactants->get(k))
					{
						s1 = reactants->get(k)->getSpecies();
						reacStoic.value(0, tr(s1.c_str())) += 1.0; 
					}
				for (int k=0; k < products->size(); ++k)
					if (products->get(k))
					{
						s1 = products->get(k)->getSpecies();
						prodStoic.value(0, tr(s1.c_str())) += 1.0; 
					}
			}

		}

		return handles;
	}

	static void substituteEqualitySymbols(QString& s)
	{
		s.replace(QRegExp("(.*)>(.*)"), "gt(\\1,\\2)");
		s.replace(QRegExp("(.*)<(.*)"), "lt(\\1,\\2)");
		s.replace(QRegExp("(.*)>=(.*)"), "ge(\\1,\\2)");
		s.replace(QRegExp("(.*)<=(.*)"), "le(\\1,\\2)");
		s.replace(QRegExp("(.*)!=(.*)"), "ne(\\1,\\2)");
		s.replace(QRegExp("(.*)=(.*)"), "eq(\\1,\\2)");
	}

	static void matchTypesToNames(ASTNode_t* node)  
	{
	  if (node->isOperator() == false && node->isNumber() == false) {
		if (string(node->getName()) == "time") {
		  node->setType(AST_NAME_TIME);
		}
		if (string(node->getName()) == "avogadro") {
		  node->setType(AST_NAME_AVOGADRO);
		}
		if (string(node->getName()) == "delay") {
		  node->setType(AST_FUNCTION_DELAY);
		}
	  }
	  for (unsigned int c = 0; c < node->getNumChildren() ; c++) {
		matchTypesToNames(node->getChild(c));
	  }
	}

	static ASTNode* parseStringToASTNode(const string& formula)
	{
	  ASTNode* rootnode = SBML_parseFormula(formula.c_str());
	  if (rootnode == NULL) return NULL;
	  if (formula.find("time") != string::npos ||
		  formula.find("avogadro") != string::npos ||
		  formula.find("delay") != string::npos) {
		matchTypesToNames(rootnode);
	  }
	  return rootnode;
	}

	static void caratToPower(ASTNode* node)
	{
	  if (!node) return;
	  if (node->getType() == AST_POWER) {
		node->setType(AST_FUNCTION_POWER);
	  }
	  for (unsigned int c = 0; c < node->getNumChildren() ; c++) {
		caratToPower(node->getChild(c));
	  }
	}

	SBMLDocument_t* SBMLImportExport::exportSBML( QList<ItemHandle*>& handles)
	{
		SBMLDocument_t * doc = SBMLDocument_createWithLevelAndVersion(2,1);

		Model_t * model = SBMLDocument_createModel(doc);
	
		if (!model) return doc;
	
		if (currentWindow())
		{
			QString s = currentWindow()->windowTitle();		
			if (s.isEmpty())
				s = tr("TinkerCell_model");
			Model_setName( model, s.toAscii().data() );
			s.replace(".","_");
			Model_setId( model, RemoveDisallowedCharactersFromName(s).toAscii().data() );
		}

		NumericalDataTable params = BasicInformationTool::getUsedParameters(0,handles);
		NumericalDataTable stoicMatrix = StoichiometryTool::getStoichiometry(handles);
		QStringList rates = StoichiometryTool::getRates(handles);
		QStringList species, compartments, compartmentFormula, eventTriggers, eventActions, assignmentNames,
					assignmentDefs, fixedVars, functionNames, functionDefs, functionArgs;

		species = stoicMatrix.rowNames();
		QVector<double> initialValues(species.size(),0.0);
		QVector<QString> speciesCompartments(species.size(),tr("DefaultCompartment"));
		QList<double> fixedValues, compartmentVolumes;
		ItemHandle * parentHandle;

		QRegExp regex(tr("\\.(?!\\d)"));	
		int i,j;
		QString s1,s2;

		for (i=0; i < handles.size(); ++i)
		{
			if (handles[i])// && handles[i]->family())
			{
				if (!handles[i]->children.isEmpty() && handles[i]->isA(tr("Compartment")))
				{
					compartments << handles[i]->fullName(tr("_"));
					if (handles[i]->hasNumericalData(tr("Initial Value")))
						compartmentVolumes += handles[i]->numericalData(tr("Initial Value"));

					if (handles[i]->hasTextData(tr("Assignments")) && handles[i]->textDataTable(tr("Assignments")).hasRow(tr("self")))
						compartmentFormula += handles[i]->textData(tr("Assignments"),tr("self"));
					else
						compartmentFormula += tr("");
					
				}
				if (handles[i]->children.isEmpty())
				{
					if (handles[i]->hasNumericalData(tr("Initial Value")))
					{
						int k = species.indexOf(handles[i]->fullName(tr("_")));
						if (k >= 0)
						{
							initialValues[k] = handles[i]->numericalData(tr("Initial Value"));
							if (parentHandle = handles[i]->parentOfFamily(tr("Compartment")))
								speciesCompartments[k] = parentHandle->fullName(tr("_"));
						}

						if (handles[i]->hasNumericalData(tr("Fixed")) &&
							handles[i]->numericalData(tr("Fixed")) > 0)
						{
							fixedVars << handles[i]->fullName(tr("_"));						
							fixedValues << handles[i]->numericalData(tr("Initial Value"));
						}
					}
				}
				if (handles[i]->hasTextData(tr("Events")))
				{
					DataTable<QString>& dat = handles[i]->textDataTable(tr("Events"));
					if (dat.columns() == 1)
						for (j=0; j < dat.rows(); ++j)
						{
							s1 =  dat.rowName(j);
							s2 =  dat.value(j,0);

							s1.replace(regex,tr("_"));
							s2.replace(regex,tr("_"));

							if (s1.isEmpty() || s2.isEmpty()) continue;

							substituteEqualitySymbols(s1);
							eventTriggers << s1;
							eventActions << s2;
						}
				}
				if (handles[i]->hasTextData(tr("Functions")))
				{
					DataTable<QString>& dat = handles[i]->textDataTable(tr("Functions"));

					if (dat.columns() == 2)
					{
						for (j=0; j < dat.rows(); ++j)
						{
							name = handles[i]->fullName(tr("_"));
							s1 = dat.value(j,1);
							s1.replace(regex,tr("_"));

							s2 = dat.value(j,0);

							functionNames << name + tr("_") + dat.rowName(j);
							functionArgs << s2;
							functionDefs << s1;
						}
					}
				}

				if (handles[i]->hasTextData(tr("Assignments")))
				{
					DataTable<QString>& dat = handles[i]->textDataTable(tr("Assignments"));
					if (dat.columns() == 1)
						for (j=0; j < dat.rows(); ++j)
						{
							name = handles[i]->fullName(tr("_"));

							s1 =  dat.rowName(j);
							s2 =  dat.value(j,0);

							s1.replace(regex,tr("_"));
							s2.replace(regex,tr("_"));

							if (s2.isEmpty() || s2.size() < 2) continue;

							if (s1.isEmpty() || s1 == tr("self"))
							{
								assignmentNames << name;
								assignmentDefs << s2;

								if (!fixedVars.contains(name))
								{
									fixedVars << name;
									fixedValues << 1.0;
								}
							}
							else								
							{
								assignmentNames << name + tr("_") + s1;
								assignmentDefs << s2;
							}
						}
				}
			}
		}
	
		if (compartments.isEmpty())
		{
			compartments << tr("DefaultCompartment");
			compartmentVolumes << 1.0;
			compartmentFormula << tr("");
		}
	
		//Make list of species types and units
		QList<ItemFamily*> families;
	
		for (int i=0; i < families.size(); ++i)
		{
			SpeciesType_t * s = Model_createSpeciesType(model);
			if (s)
			{
				SpeciesType_setId(s, ConvertValue(tr("family_") + families[i]->name()));
				SpeciesType_setName(s, ConvertValue(families[i]->name()));
				/*if (!families[i]->measurementUnit.name.isEmpty())
				{
					UnitDefinition_t * unitDef = Model_createUnitDefinition(model);
					UnitDefinition_setId(unitDef, ConvertValue(families[i]->measurementUnit.name)); 
					UnitDefinition_setName(unitDef, ConvertValue(families[i]->measurementUnit.name));
				}*/
			}
		}
	
		//create compartments
		for (int i=0; i < compartments.size(); ++i)
		{
			Compartment_t * comp = Model_createCompartment (model);
			if (comp)
			{
				Compartment_setId(comp, ConvertValue(compartments[i]));
				Compartment_setName(comp, ConvertValue(compartments[i]));
				Compartment_setVolume(comp, compartmentVolumes[i]);
				if (compartmentFormula[i].isEmpty())
				{
					Compartment_setConstant(comp, 1);
				}
				else
				{
					Compartment_setConstant(comp,0);
					assignmentNames << compartments[i];
					assignmentDefs << compartmentFormula[i];
				}
				//Compartment_setUnits(comp, "uL");
			}
		}
	
		QVector<ItemHandle*> speciesHandles(species.size(),0);
		for (int i=0,k=0; i < handles.size(); ++i)
		{
			k = species.indexOf(handles[i]->fullName(tr("_")));
			if (k >= 0 && handles[i]->family())
			{
				speciesHandles[k] = handles[i];
				if (!families.contains(handles[i]->family()))
					families.append(handles[i]->family());
			}
		}

		//create list of species
		for (int i=0; i < species.size(); ++i)
		{
			Species_t * s = Model_createSpecies(model);
			if (s)
			{
				Species_setId(s,ConvertValue(species[i]));
				Species_setName(s,ConvertValue(species[i]));
				Species_setConstant(s,0);
				Species_setInitialConcentration(s,initialValues[i]);
				//Species_setInitialAmount(s,initialValues[i]);		
				Species_setCompartment(s, ConvertValue(speciesCompartments[i]));

				if (fixedVars.contains(species[i]))
					Species_setBoundaryCondition(s,1);
				else
					Species_setBoundaryCondition(s,0);

				Species_setHasOnlySubstanceUnits(s,0);
				if (speciesHandles[i] && speciesHandles[i]->family())
				{
					Species_setSpeciesType(s,ConvertValue(tr("family_") + speciesHandles[i]->family()->name()));
					//if (!speciesHandles[i]->family()->measurementUnit.name.isEmpty())
						//Species_setUnits(s, ConvertValue(speciesHandles[i]->family()->measurementUnit.name));
				}
			}
		}
	
		//create list of reactions
		for (int i=0; i < stoicMatrix.columns(); ++i)
		{
			Reaction * reac = Model_createReaction(model);
			Reaction_setReversible(reac,0);
			Reaction_setFast(reac,0);
			if (!reac)
				continue;
			Reaction_setId(reac, ConvertValue(stoicMatrix.columnName(i)));
			Reaction_setName(reac, ConvertValue(stoicMatrix.columnName(i)));
			Reaction_setId(reac, ConvertValue(stoicMatrix.columnName(i)));
			KineticLaw_t  * kinetic = Reaction_createKineticLaw(reac);
			std::string formula(rates[i].toAscii().data());
			ASTNode_t* ASTform = parseStringToASTNode(formula);
 			caratToPower(ASTform);
			KineticLaw_setMath(kinetic, ASTform);			
			//KineticLaw_setFormula( kinetic, formula.c_str());

			for (int j=0; j < stoicMatrix.rows(); ++j)
			{
				QRegExp regex1(QString("^") + stoicMatrix.rowName(j) + QString("$")),  //just name
								regex2(QString("^") + stoicMatrix.rowName(j) + QString("([^A-Za-z0-9_])")),  //name+(!letter/num)
								regex3(QString("([^A-Za-z0-9_.])") + stoicMatrix.rowName(j) + QString("$")), //(!letter/num)+name
								regex4(QString("([^A-Za-z0-9_.])") + stoicMatrix.rowName(j) + QString("([^A-Za-z0-9_])")); //(!letter/num)+name+(!letter/num)
				if (stoicMatrix.value(j,i) < 0)
				{
					SpeciesReference_t * sref = reac->createReactant();
					SpeciesReference_setId(sref, ConvertValue(stoicMatrix.columnName(i) + QString("_") + stoicMatrix.rowName(j)));
					SpeciesReference_setName(sref, ConvertValue(stoicMatrix.rowName(j)));
					SpeciesReference_setSpecies(sref, ConvertValue(stoicMatrix.rowName(j)));
					SpeciesReference_setStoichiometry( sref, -stoicMatrix.value(j,i) );
					SpeciesReference_setConstant( sref, 0 );
				}
				else
				if (stoicMatrix.value(j,i) > 0)
				{
					SpeciesReference_t * sref = reac->createProduct();
					Reaction_addProduct(reac, sref);
					SpeciesReference_setId(sref, ConvertValue(stoicMatrix.columnName(i) + QString("_") + stoicMatrix.rowName(j)));
					SpeciesReference_setName(sref, ConvertValue(stoicMatrix.rowName(j)));
					SpeciesReference_setSpecies(sref, ConvertValue(stoicMatrix.rowName(j)));
					SpeciesReference_setStoichiometry( sref, stoicMatrix.value(j,i) );
					SpeciesReference_setConstant( sref, 0 );
				}
				else
				{
					if ((rates[i].contains(regex1) || rates[i].contains(regex2) || rates[i].contains(regex3) || rates[i].contains(regex4)))
					{
						SpeciesReference_t * sref = Reaction_createModifier(reac);
						SpeciesReference_setId(sref, ConvertValue(stoicMatrix.columnName(i) + QString("_") + stoicMatrix.rowName(j)));
						SpeciesReference_setName(sref, ConvertValue(stoicMatrix.rowName(j)));
						SpeciesReference_setSpecies(sref, ConvertValue(stoicMatrix.rowName(j)));
						SpeciesReference_setConstant(sref, 0);
					}
				}
			}
		}
	
		//create list of parameters
		for (int i=0; i < params.rows(); ++i)
		{
			Parameter_t * p = Model_createParameter(model);
			if (p)
			{
				Parameter_setId(p, ConvertValue(params.rowName(i)));
				Parameter_setName(p, ConvertValue(params.rowName(i)));
				Parameter_setValue(p, params.value(i,0));
				Parameter_setConstant(p, 1);
			}
		}
	
		/*for (int i=0; i < fixedVars.size(); ++i)
		{
			Parameter_t * p = Model_createParameter(model);
			if (p)
			{
				Parameter_setId(p, ConvertValue(fixedVars[i]));
				Parameter_setName(p, ConvertValue(fixedVars[i]));
				Parameter_setValue(p, fixedValues[i]);
				Parameter_setConstant(p, 0);
			}
		}*/
	
		//list of assignments
		for (int i=0; i < assignmentNames.size(); ++i)
		{
			Rule_t * rule = Model_createAssignmentRule(model);
			if (rule)
			{
				std::string formula(assignmentDefs[i].toAscii().data());
				ASTNode_t* ASTform = parseStringToASTNode(formula);
	 			caratToPower(ASTform);
				Rule_setVariable(rule, ConvertValue(assignmentNames[i]));
				//Rule_setFormula(rule, formula.c_str());	
				Rule_setMath(rule, ASTform);	
			}
		}
	
		//list of events
		for (int i=0; i < eventTriggers.size(); ++i)
		{
			Event_t * event = Model_createEvent(model);
			Trigger_t * trigger = Event_createTrigger(event);
			if (event && trigger)
			{
				std::string formula(eventTriggers[i].toAscii().data());
				ASTNode_t* ASTform = parseStringToASTNode(formula);
				caratToPower(ASTform);
				Trigger_setMath(trigger, ASTform);
				QStringList actions = eventActions[i].split(";");
				for (int j=0; j < actions.size(); ++j)
				{
					QStringList words = actions[j].split("=");
					if (words.size() == 2)
					{
						EventAssignment_t * assignment = Event_createEventAssignment(event);
						if (assignment)
						{
							formula = std::string(words[1].toAscii().data());
							ASTform = parseStringToASTNode(formula);
				 			caratToPower(ASTform);
							EventAssignment_setVariable(assignment, ConvertValue(words[0].trimmed()));
							EventAssignment_setMath(assignment, ASTform);
						}
					}
				}
			}
		}
	
		return doc;
	}

	SBMLDocument_t* SBMLImportExport::exportSBML(NetworkHandle * network)
	{
		if (!network)
			network = currentNetwork();
		
		if (network)
		{
			QList<ItemHandle*> handles = network->handles();
			return exportSBML(handles);
		}
	
		SBMLDocument_t * doc = SBMLDocument_createWithLevelAndVersion(2,1);
		return doc;
	}
}

/*
extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::SBMLImportExport * sbmlTool = new Tinkercell::SBMLImportExport;
	main->addTool(sbmlTool);

}*/

