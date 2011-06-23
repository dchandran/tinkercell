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

using namespace Tinkercell;
using namespace std;

SBMLImportExport::SBMLImportExport() : Tool("SBML Tool","Export")
{
	modelNeedsUpdate = true;
	sbmlDocument = 0;
	SBMLImportExport::fToS = new SBMLImportExport_FtoS;
	SBMLImportExport::fToS->setParent(this);
	connect(fToS,SIGNAL(exportSBML(QSemaphore*, const QString&)),this,SLOT(exportSBML(QSemaphore*, const QString&)));
	connect(fToS,SIGNAL(importSBML(QSemaphore*, const QString&)),this,SLOT(importSBML(QSemaphore*, const QString&)));
	connect(fToS,SIGNAL(exportText(QSemaphore*, const QString&)),this,SLOT(exportText(QSemaphore*, const QString&)));
	connect(fToS,SIGNAL(importText(QSemaphore*, const QString&)),this,SLOT(importText(QSemaphore*, const QString&)));
	connect(fToS,SIGNAL(exportMath(QSemaphore*, const QString&)),this,SLOT(exportMath(QSemaphore*, const QString&)));
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
	connect(mainWindow,SIGNAL(loadNetwork(const QString&)),this,SLOT(loadNetwork(const QString&)));
	connect(main,SIGNAL(historyChanged(int)),this, SLOT(historyChanged(int)));
	connect(main,SIGNAL(windowChanged(NetworkWindow*,NetworkWindow*)),this, SLOT(windowChanged(NetworkWindow*,NetworkWindow*)));
	
	return true;
}

typedef void (*tc_SBML_api)(
		void (*exportSBMLFile)(const char *),
		void (*importSBMLString)(const char*),
		void (*exportTextFile)(const char *),
		void (*importTextString)(const char*),
		void (*exportMathFile)(const char *));

void SBMLImportExport::setupFunctionPointers( QLibrary * library)
{
	tc_SBML_api f = (tc_SBML_api)library->resolve("tc_SBML_api");
	if (f)
	{
		f(	&exportSBMLFile, &importSBMLString, &exportTextFile, &importTextString, &exportMathFile );
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

void SBMLImportExport::exportTextFile(const char * s)
{
	return fToS->exportTextFile(s);
}

void SBMLImportExport::importTextString(const char* s)
{
	fToS->importTextString(s);
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

void SBMLImportExport_FtoS::importSBMLString(const char* c)
{
	QSemaphore * s = new QSemaphore(1);
	s->acquire();
	emit importSBML(s,ConvertValue(c));
	s->acquire();
	s->release();
}

void SBMLImportExport_FtoS::exportTextFile(const char * c)
{
	QSemaphore * s = new QSemaphore(1);
	s->acquire();
	emit exportText(s,ConvertValue(c));
	s->acquire();
	s->release();
	delete s;
}

void SBMLImportExport_FtoS::importTextString(const char* c)
{
	QSemaphore * s = new QSemaphore(1);
	s->acquire();
	emit importText(s,ConvertValue(c));
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

void SBMLImportExport::exportText(QSemaphore * sem, const QString & str)
{
	if (currentNetwork())
	{
		QString text;
		emit getTextVersion(currentNetwork()->handles(), &text);
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

void SBMLImportExport::exportSBML(QSemaphore * sem, const QString & str)
{
	if (modelNeedsUpdate)
		updateSBMLModel();

	if (sbmlDocument)
		writeSBML (sbmlDocument, ConvertValue(str) );

	if (sem)
		sem->release();
}

void SBMLImportExport::loadNetwork(const QString& filename)
{
	importSBML(0,filename);
}

void SBMLImportExport::importText(QSemaphore * sem, const QString& str)
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

void SBMLImportExport::importSBML(QSemaphore * sem, const QString& str)
{
	QList<ItemHandle*> items = importSBML(str);
	
	if (items.size() > 0)
	{
		QString text;
		emit getTextVersion(items, &text);
	
		TextEditor * editor = mainWindow->newTextEditor();
		if (editor)
		{
			editor->setItems(items);
			editor->setText(text);
		}
	}
	else
	{
		if (sem && console())
			console()->error("Failed to load SBML file");
	}

	if (sem)
		sem->release();
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

QList<ItemHandle*> SBMLImportExport::importSBML(const QString& sbml_text)
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
		ItemHandle * global = new ItemHandle("");
		handles << global;

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
				ItemHandle * h = new NodeHandle(defaultSpeciesFamily, tr( s1.c_str() ));
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

SBMLDocument_t* SBMLImportExport::exportSBML( QList<ItemHandle*>& handles)
{
	SBMLDocument_t * doc = SBMLDocument_create();
	Model_t * model = SBMLDocument_createModel(doc);
	
	if (!model) return doc;
	
	if (currentWindow() && currentWindow()->windowTitle())
	{
		QString s = currentWindow()->windowTitle();		
		Model_setId( model, RemoveDisallowedCharactersFromName(s) );
		Model_setName( model, s );
	}
	else
	{
		Model_setId( model, "TinkerCell_model" );
		Model_setName( model, "TinkerCell_model" );
	}

	NumericalDataTable params = BasicInformationTool::getUsedParameters(0,handles);
	NumericalDataTable stoicMatrix = StoichiometryTool::getStoichiometry(handles);
	QStringList rates = StoichiometryTool::getRates(handles);
	QStringList species, compartments, eventTriggers, eventActions, assignmentNames,
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
			if (handles[i]->isA(tr("Compartment")))
			{
				compartments << handles[i]->fullName(tr("_"));
				if (handles[i]->hasNumericalData(tr("Initial Value")))
					compartmentVolumes += handles[i]->numericalData(tr("Initial Value"));
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
	
	if (compartments.isEmpty() || !fixedVars.isEmpty())
	{
		compartments << tr("DefaultCompartment");
		compartmentVolumes << 1.0;
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
		if (!fixedVars.contains(species[i]))
		{
			Species_t * s = Model_createSpecies(model);
			if (s)
			{
				Species_setId(s,ConvertValue(species[i]));
				Species_setName(s,ConvertValue(species[i]));
				Species_setConstant(s,0);
				Species_setInitialConcentration(s,initialValues[i]);
				Species_setInitialAmount(s,initialValues[i]);		
				Species_setCompartment(s, ConvertValue(speciesCompartments[i]));
				if (speciesHandles[i] && speciesHandles[i]->family())
				{
					Species_setSpeciesType(s,ConvertValue(tr("family_") + speciesHandles[i]->family()->name()));
					//if (!speciesHandles[i]->family()->measurementUnit.name.isEmpty())
						//Species_setUnits(s, ConvertValue(speciesHandles[i]->family()->measurementUnit.name));
				}
			}
		}

	//create list of fixed species
	/*for (int i=0; i < fixedVars.size(); ++i)
	{
		Species_t * s = Model_createSpecies(model);
		if (s)
		{
			Species_setId(s,ConvertValue(fixedVars[i]));
			Species_setName(s,ConvertValue(fixedVars[i]));
			Species_setConstant(s,1);
			Species_setInitialConcentration(s,fixedValues[i]);
			Species_setInitialAmount(s,fixedValues[i]);
			Species_setCompartment(s, "DefaultCompartment");

			for (int j=0; j < handles.size(); ++j)
				if (handles[j] && 
					handles[j]->family() && 
					fixedVars[i] == handles[j]->fullName(tr("_")))
				{
					if (!families.contains(handles[j]->family()))
					{
						SpeciesType_t * s = Model_createSpeciesType(model);
						if (s)
						{
							SpeciesType_setId(s, ConvertValue(tr("family_") + handles[j]->family()->name()));
							SpeciesType_setName(s, ConvertValue(handles[j]->family()->name()));
							if (!handles[j]->family()->measurementUnit.name.isEmpty())
							{
								UnitDefinition_t * unitDef = Model_createUnitDefinition(model);
								UnitDefinition_setId(unitDef, ConvertValue(handles[j]->family()->measurementUnit.name)); 
								UnitDefinition_setName(unitDef, ConvertValue(handles[j]->family()->measurementUnit.name));
							}
						}
					}
					Species_setSpeciesType(s,ConvertValue(tr("family_") + handles[j]->family()->name()));
					if (!handles[j]->family()->measurementUnit.name.isEmpty())
						Species_setUnits(s, ConvertValue(handles[j]->family()->measurementUnit.name));
				}
			}
	}*/
	
	//create list of reactions
	for (int i=0; i < stoicMatrix.columns(); ++i)
	{
		Reaction * reac = Model_createReaction(model);
        Reaction_setReversible(reac,0);
		if (!reac)
			continue;
		Reaction_setId(reac, ConvertValue(stoicMatrix.columnName(i)));
		Reaction_setName(reac, ConvertValue(stoicMatrix.columnName(i)));
		Reaction_setId(reac, ConvertValue(stoicMatrix.columnName(i)));
		KineticLaw_t  * kinetic = Reaction_createKineticLaw(reac);
		KineticLaw_setFormula( kinetic, ConvertValue( rates[i] ));

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
			}
			else
			if ((rates[i].contains(regex1) || rates[i].contains(regex2) || rates[i].contains(regex2) || rates[i].contains(regex3)) && !fixedVars.contains(stoicMatrix.rowName(j)))
			{
				SpeciesReference_t * sref = Reaction_createModifier(reac);
				SpeciesReference_setId(sref, ConvertValue(stoicMatrix.columnName(i) + QString("_") + stoicMatrix.rowName(j)));
				SpeciesReference_setName(sref, ConvertValue(stoicMatrix.rowName(j)));
				SpeciesReference_setSpecies(sref, ConvertValue(stoicMatrix.rowName(j)));
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
	
	for (int i=0; i < fixedVars.size(); ++i)
	{
		Parameter_t * p = Model_createParameter(model);
		if (p)
		{
			Parameter_setId(p, ConvertValue(fixedVars[i]));
			Parameter_setName(p, ConvertValue(fixedVars[i]));
			Parameter_setValue(p, fixedValues[i]);
			Parameter_setConstant(p, 0);
		}
	}
	
	//list of assignments
	for (int i=0; i < assignmentNames.size(); ++i)
	{
		Rule_t * rule = Model_createAssignmentRule(model);
		if (rule)
		{
			Rule_setVariable(rule, ConvertValue(assignmentNames[i]));
			Rule_setFormula(rule, ConvertValue(assignmentDefs[i]));		
		}
	}
	
	//list of events
	for (int i=0; i < eventTriggers.size(); ++i)
	{
		Event_t * event = Model_createEvent(model);
		Trigger_t * trigger = Event_createTrigger(event);
		if (event && trigger)
		{
			Trigger_setMath(trigger, SBML_parseFormula (ConvertValue(eventTriggers[i])));
			QStringList actions = eventActions[i].split(";");
			for (int j=0; j < actions.size(); ++j)
			{
				QStringList words = actions[j].split("=");
				if (words.size() == 2)
				{
					EventAssignment_t * assignment = Event_createEventAssignment(event);
					if (assignment)
					{
						EventAssignment_setVariable(assignment, ConvertValue(words[0].trimmed()));
						EventAssignment_setMath(assignment, SBML_parseFormula (ConvertValue(words[1].trimmed())));
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
	
	return 	SBMLDocument_create();
}

/*
extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::SBMLImportExport * sbmlTool = new Tinkercell::SBMLImportExport;
	main->addTool(sbmlTool);

}*/

