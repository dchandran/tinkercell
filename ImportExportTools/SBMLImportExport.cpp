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
#include "sbml_sim.h"
#include "TextEditor.h"
#include "NodesTree.h"
#include "ConnectionsTree.h"
#include "ConsoleWindow.h"

using namespace Tinkercell;
using namespace std;

SBMLImportExport::SBMLImportExport() : Tool("SBML Tool","Export")
{
	modelNeedsUpdate = true;
	sbmlDocument = 0;

	connect(&fToS,SIGNAL(exportSBML(QSemaphore*, const QString&)),this,SLOT(exportSBML(QSemaphore*, const QString&)));
	connect(&fToS,SIGNAL(importSBML(QSemaphore*, const QString&)),this,SLOT(importSBML(QSemaphore*, const QString&)));
	connect(&fToS,SIGNAL(simulateODE(QSemaphore*, NumericalDataTable*,double, double)),this,SLOT(simulateODE(QSemaphore*, NumericalDataTable*,double, double)));
	connect(&fToS,SIGNAL(simulateGillespie(QSemaphore*, NumericalDataTable*,double)),this,SLOT(simulateGillespie(QSemaphore*, NumericalDataTable*,double)));
	connect(&fToS,SIGNAL(steadyStateScan(QSemaphore*, NumericalDataTable* , const QString&, double , double )),this,SLOT(steadyStateScan(QSemaphore*, NumericalDataTable* , const QString&, double , double )));
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
	
	MainWindow::OPEN_FILE_EXTENSIONS << "SBML" << "sbml";

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
		tc_matrix (*ODEsim)(double, double),
		tc_matrix (*GillespieSim)(double),
		tc_matrix (*steadyStateScan)(const char* , double , double ));

void SBMLImportExport::setupFunctionPointers( QLibrary * library)
{
	tc_SBML_api f = (tc_SBML_api)library->resolve("tc_SBML_api");
	if (f)
	{
		f(	&exportSBMLFile, &importSBMLString, &ODEsim, &GillespieSim, &ScanSS );
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
	QString file = QFileDialog::getSaveFileName (this, tr("Save SBML file"));
	if (file.isNull() || file.isEmpty()) return;
	
	exportSBML(0, file);
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

SBMLImportExport_FtoS SBMLImportExport::fToS;

void SBMLImportExport::exportSBMLFile(const char * s)
{
	return fToS.exportSBMLFile(s);
}

void SBMLImportExport::importSBMLString(const char* s)
{
	fToS.importSBMLString(s);
}

tc_matrix SBMLImportExport::ODEsim(double time, double dt)
{
	return fToS.ODEsim(time,dt);
}

tc_matrix SBMLImportExport::GillespieSim(double time)
{
	return fToS.GillespieSim(time);
}

tc_matrix SBMLImportExport::ScanSS(const char* var, double a, double b)
{
	return fToS.ScanSS(var,a,b);
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

tc_matrix SBMLImportExport_FtoS::ODEsim(double time, double dt)
{
	QSemaphore * s = new QSemaphore(1);
	NumericalDataTable t;
	s->acquire();
	emit simulateODE(s,&t,time,dt);
	s->acquire();
	s->release();
	delete s;
	return ConvertValue(t);
}

tc_matrix SBMLImportExport_FtoS::GillespieSim(double time)
{
	QSemaphore * s = new QSemaphore(1);
	NumericalDataTable t;
	s->acquire();
	emit simulateGillespie(s,&t,time);
	s->acquire();
	s->release();
	delete s;
	return ConvertValue(t);
}

tc_matrix SBMLImportExport_FtoS::ScanSS(const char* var, double a, double b)
{
	QSemaphore * s = new QSemaphore(1);
	NumericalDataTable t;
	s->acquire();
	emit steadyStateScan(s,&t,ConvertValue(var),a,b);
	s->acquire();
	s->release();
	delete s;
	return ConvertValue(t);
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

void SBMLImportExport::simulateODE(QSemaphore * sem, NumericalDataTable * dat, double time, double dt)
{
	if (modelNeedsUpdate)
		updateSBMLModel();
	SimulationThread * thread = new SimulationThread(sem, dat, sbmlDocument, SimulationThread::ODE, mainWindow);
	thread->setTime(time);
	thread->setStepSize(dt);
	CThread::dialog(thread,"ODE simulation",QIcon(),true);
	thread->start();
}

void SBMLImportExport::simulateGillespie(QSemaphore * sem, NumericalDataTable * dat, double time)
{
	if (modelNeedsUpdate)
		updateSBMLModel();
	SimulationThread * thread = new SimulationThread(sem, dat, sbmlDocument, SimulationThread::Gillespie, mainWindow);
	thread->setTime(time);
	CThread::dialog(thread,"Gillespie simulation",QIcon(),true);
	thread->start();
}

void SBMLImportExport::steadyStateScan(QSemaphore* sem, NumericalDataTable* dat, const QString& var, double start, double end)
{
	if (modelNeedsUpdate)
		updateSBMLModel();
	SimulationThread * thread = new SimulationThread(sem, dat, sbmlDocument, SimulationThread::Scan, mainWindow);
	thread->setScanVariable(var, start, end);
	CThread::dialog(thread,"Steady state scan",QIcon(),true);
	thread->start();
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
	
	ConnectionFamily * defaultReactionFamily = 0;
	NodeFamily * defaultSpeciesFamily = 0;

	if (mainWindow->tool(tr("Nodes Tree")) && mainWindow->tool(tr("Connections Tree")))
	{
		NodesTree * partsTree = static_cast<NodesTree*>(mainWindow->tool(tr("Nodes Tree")));
		ConnectionsTree * connectionsTree = static_cast<ConnectionsTree*>(mainWindow->tool(tr("Connections Tree")));
		defaultReactionFamily = connectionsTree->getFamily(tr("Biochemical"));
		defaultSpeciesFamily = partsTree->getFamily(tr("Molecule"));
	}

	if (!defaultSpeciesFamily || !defaultReactionFamily)
	{
		if (console())
            console()->error(tr("No parts and connection information"));
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

	NumericalDataTable params = BasicInformationTool::getUsedParameters(handles);
	NumericalDataTable stoictc_matrix = StoichiometryTool::getStoichiometry(handles);
	QStringList rates = StoichiometryTool::getRates(handles);
	QStringList species, compartments, eventTriggers, eventActions, assignmentNames,
				assignmentDefs, fixedVars, functionNames, functionDefs, functionArgs;

	species = stoictc_matrix.rowNames();
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

						if (s1.isEmpty() || s2.isEmpty()) continue;

						if (s1.isEmpty() || s1 == name)
						{
							assignmentNames << name;
							assignmentDefs << s2;

							if (!fixedVars.contains(name))
								fixedVars << name;
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
	}
	
	//Make list of species types and units
	QVector<ItemHandle*> speciesHandles(species.size(),0);
	QList<ItemFamily*> families;
	
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
	
	for (int i=0; i < families.size(); ++i)
	{
		SpeciesType_t * s = Model_createSpeciesType(model);
		SpeciesType_setId(s, ConvertValue(families[i]->name()));
		SpeciesType_setName(s, ConvertValue(families[i]->name()));
		if (!families[i]->measurementUnit.name.isEmpty())
		{
			UnitDefinition_t * unitDef = Model_createUnitDefinition(model);
			UnitDefinition_setId(unitDef, ConvertValue(families[i]->measurementUnit.name)); 
			UnitDefinition_setName(unitDef, ConvertValue(families[i]->measurementUnit.name));
		}
	}
	
	//create compartments
	for (int i=0; i < compartments.size(); ++i)
	{
		Compartment_t * comp = Model_createCompartment (model);
		Compartment_setId(comp, ConvertValue(compartments[i]));
		Compartment_setName(comp, ConvertValue(compartments[i]));
		Compartment_setVolume(comp, compartmentVolumes[i]);
		Compartment_setUnits(comp, "uL");
	}

	//create list of species
	for (int i=0; i < species.size(); ++i)
	{
		Species_t * s = Model_createSpecies(model);
		Species_setId(s,ConvertValue(species[i]));
		Species_setName(s,ConvertValue(species[i]));
		Species_setConstant(s,0);
		Species_setInitialConcentration(s,initialValues[i]);
		Species_setInitialAmount(s,initialValues[i]);		
		Species_setCompartment(s, ConvertValue(speciesCompartments[i]));
		if (speciesHandles[i] && speciesHandles[i]->family())
		{
			Species_setSpeciesType(s,ConvertValue(speciesHandles[i]->family()->name()));
			if (!speciesHandles[i]->family()->measurementUnit.name.isEmpty())
				Species_setUnits(s, ConvertValue(speciesHandles[i]->family()->measurementUnit.name));
		}
	}

	//create list of fixed species
	for (int i=0; i < fixedVars.size(); ++i)
	{
		Species_t * s = Model_createSpecies(model);
		Species_setId(s,ConvertValue(fixedVars[i]));
		Species_setName(s,ConvertValue(fixedVars[i]));
		Species_setConstant(s,1);
		Species_setInitialConcentration(s,fixedValues[i]);
		Species_setInitialAmount(s,fixedValues[i]);
		
		for (int j=0; j < handles.size(); ++j)
			if (handles[j] && 
				handles[j]->family() && 
				fixedVars[i] == handles[i]->fullName(tr("_")))
			{
				if (!families.contains(handles[j]->family()))
				{
					SpeciesType_t * s = Model_createSpeciesType(model);
					SpeciesType_setId(s, ConvertValue(handles[j]->family()->name()));
					SpeciesType_setName(s, ConvertValue(handles[j]->family()->name()));
					if (!handles[j]->family()->measurementUnit.name.isEmpty())
					{
						UnitDefinition_t * unitDef = Model_createUnitDefinition(model);
						UnitDefinition_setId(unitDef, ConvertValue(handles[j]->family()->measurementUnit.name)); 
						UnitDefinition_setName(unitDef, ConvertValue(handles[j]->family()->measurementUnit.name));
					}
				}
				Species_setSpeciesType(s,ConvertValue(handles[j]->family()->name()));
				if (!handles[j]->family()->measurementUnit.name.isEmpty())
					Species_setUnits(s, ConvertValue(handles[j]->family()->measurementUnit.name));
			}
	}
	
	//create list of reactions
	for (int i=0; i < stoictc_matrix.columns(); ++i)
	{
		Reaction_t * reac = Model_createReaction(model);
		Reaction_setId(reac, ConvertValue(stoictc_matrix.columnName(i)));
		Reaction_setName(reac, ConvertValue(stoictc_matrix.columnName(i)));
		Reaction_setId(reac, ConvertValue(stoictc_matrix.columnName(i)));
		KineticLaw_t  * kinetic = Reaction_createKineticLaw(reac);
		KineticLaw_setFormula( kinetic, ConvertValue( rates[i] ));

		for (int j=0; j < stoictc_matrix.rows(); ++j)
			if (stoictc_matrix.value(j,i) < 0)
			{
				for (int k=0; k < -stoictc_matrix.value(j,i); ++k)
				{ 
					SpeciesReference_t * sref = Reaction_createReactant(reac);
					SpeciesReference_setId(sref, ConvertValue(stoictc_matrix.rowName(j)));
					SpeciesReference_setName(sref, ConvertValue(stoictc_matrix.rowName(j)));
					SpeciesReference_setSpecies(sref, ConvertValue(stoictc_matrix.rowName(j)));
					//SpeciesReference_setStoichiometry( sref, -stoictc_matrix.value(j,i) );
				}
			}
			else
			if (stoictc_matrix.value(j,i) > 0)
			{
				for (int k=0; k < stoictc_matrix.value(j,i); ++k)
				{
					SpeciesReference_t * sref = Reaction_createProduct(reac);
					SpeciesReference_setId(sref, ConvertValue(stoictc_matrix.rowName(j)));
					SpeciesReference_setName(sref, ConvertValue(stoictc_matrix.rowName(j)));
					SpeciesReference_setSpecies(sref, ConvertValue(stoictc_matrix.rowName(j)));
					//SpeciesReference_setStoichiometry( sref, stoictc_matrix.value(j,i) );
				}
			}		
	}
	
	//create list of parameters
	for (int i=0; i < params.rows(); ++i)
	{
		Parameter_t * p = Model_createParameter(model);
		Parameter_setId(p, ConvertValue(params.rowName(i)));
		Parameter_setName(p, ConvertValue(params.rowName(i)));
		Parameter_setValue(p, params.value(i,0));
	}
	
	//list of assignments
	for (int i=0; i < assignmentNames.size(); ++i)
	{
		Rule_t * rule = Model_createAssignmentRule(model);
		Rule_setVariable(rule, ConvertValue(assignmentNames[i]));
		Rule_setFormula(rule, ConvertValue(assignmentDefs[i]));		
	}
	
	//list of events
	for (int i=0; i < eventTriggers.size(); ++i)
	{
		Event_t * event = Model_createEvent(model);
		Trigger_t * trigger = Event_createTrigger(event);
		Trigger_setMath(trigger, SBML_parseFormula (ConvertValue(eventTriggers[i])));
		QStringList actions = eventActions[i].split(";");
		for (int j=0; j < actions.size(); ++j)
		{
			QStringList words = actions[j].split("=");
			if (words.size() == 2)
			{
				EventAssignment_t * assignment = Event_createEventAssignment(event);
				EventAssignment_setVariable(assignment, ConvertValue(words[0].trimmed()));
				EventAssignment_setMath(assignment, SBML_parseFormula (ConvertValue(words[1].trimmed())));
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

NumericalDataTable SBMLImportExport::integrateODEs(double time, double printstep)
{
	if (modelNeedsUpdate)
		updateSBMLModel();
	NumericalDataTable dat;
	QSemaphore * s = new QSemaphore(1);
	s->acquire();
	SimulationThread * thread = new SimulationThread(s, &dat, sbmlDocument, SimulationThread::ODE, mainWindow);
	thread->setTime(time);
	thread->setStepSize(printstep);
	thread->start();
	s->acquire();
	s->release();
	delete s;
	return dat;
}

NumericalDataTable SBMLImportExport::Gillespie(double time)
{
	if (modelNeedsUpdate)
		updateSBMLModel();
	NumericalDataTable dat;
	QSemaphore * s = new QSemaphore(1);
	s->acquire();
	SimulationThread * thread = new SimulationThread(s, &dat, sbmlDocument, SimulationThread::Gillespie, mainWindow);
	thread->setTime(time);
	thread->start();
	s->acquire();
	s->release();
	delete s;
	return dat;
}

SimulationThread::SimulationThread(QSemaphore * sem, NumericalDataTable * dat, SBMLDocument_t * doc, SimulationType ty, MainWindow * parent) :
 CThread(parent,0), semaphore(sem), dataTable(dat), sbmlDocument(doc), simType(ty)
{
	time = 100.0;
	stepSize = 0.1;
}

void SimulationThread::setScanVariable(const QString& s, double a, double b)
{
	scanParam = s;
	scanParam.replace(tr("."),tr("_"));
	from = a;
	to = b;
}

void SimulationThread::setTime(double d)
{
	time = d;
}

void SimulationThread::setStepSize(double d)
{
	stepSize = d;
}

void SimulationThread::run()
{
	SBMLDocument_t * d = sbmlDocument;
	SBML_sim sim(d);
	
	vector<string> names = sim.getVariableNames();	
	vector< vector<double> > output;	
	NumericalDataTable results;
	
	if (simType == Scan && !scanParam.isEmpty())
	{
		int k1=-1,k2=-1;
		vector<double> x0 = sim.getVariableValues();
		vector<double> x = x0;
		results.resize(100,names.size()+1);
		for (int i=0; i < names.size(); ++i)
		{
			results.setColumnName(i+1,QString(names[i].c_str()));
			if (names[i].compare( ConvertValue(scanParam) ) == 0)
				k1 = i;
		}
		results.setColumnName(0, scanParam);
		vector<double> params = sim.getParameterValues();
		names = sim.getParameterNames();	
		for (int i=0; i < names.size(); ++i)
			if (names[i].compare( ConvertValue(scanParam) ) == 0)
			{
				k2 = i;
				break;
			}
		if (k1 > -1 || k2 > -1)
		{
			for (int i=0; i < 100; ++i)
			{
				if (k1 > -1)
					x0[k1] = from + (i/100.0) * (to-from);
				else
					params[k2] = from + (i/100.0) * (to-from);
				sim.setParameters(params);
				sim.setVariableValues(x0);
				x = sim.steadyState();
				if (k1 > -1)
					results.value(i,0) = x0[k1];
				else
					results.value(i,0) = params[k2];
				for (int j=0; j < x.size(); ++j)
					results.value(i,j+1) = x[j];
			}
		}
	}
	else
	{
		try
		{
			if (simType == Gillespie)
				output = sim.ssa(time);
			else
				output = sim.simulate(time,stepSize);
		}
		catch(...)
		{
		}

		if (output.size() > 0)
		{
			int sz = output[0].size();
	
			results.resize(sz,output.size());
			for (int i=0; i < names.size(); ++i)
				results.setColumnName(i+1 , QString(names[i].c_str()));
			results.setColumnName(0, tr("time"));
	
			for (int i=0; i < output.size(); ++i)
				for (int j=0; j < sz; ++j)
					results.value(j,i) = output[i][j];
		}
	}
		
	if (dataTable)
		(*dataTable) = results;
	if (semaphore)
		semaphore->release();
}


extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::SBMLImportExport * sbmlTool = new Tinkercell::SBMLImportExport;
	main->addTool(sbmlTool);

}

