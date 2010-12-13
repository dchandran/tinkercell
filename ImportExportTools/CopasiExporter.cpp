#include <string>
#include <QFileDialog>
#include "BasicInformationTool.h"
#include "StoichiometryTool.h"
#include "CopasiExporter.h"
#include "ConsoleWindow.h"

using namespace Tinkercell;

CopasiExporter::CopasiExporter() : Tool("COPASI","Export")
{
	copasi_init();	
	model.CopasiModelPtr = 0;
	model.CopasiDataModelPtr = 0;
	model.qHash = 0;
	modelNeedsUpdate = true;	
	qRegisterMetaType< copasi_model >("copasi_model");
	connect(&fToS,SIGNAL(getCopasiModel(QSemaphore*, copasi_model *)),this,SLOT(getCopasiModel(QSemaphore*, copasi_model *)));
}

CopasiExporter::~CopasiExporter()
{
	copasi_end();
}

bool CopasiExporter::setMainWindow(MainWindow * main)
{
	Tool::setMainWindow(main);	
	if (!mainWindow) return false;
	
	connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
	connect(main,SIGNAL(historyChanged(int)),this, SLOT(historyChanged(int)));
	connect(main,SIGNAL(windowChanged(NetworkWindow*,NetworkWindow*)),this, SLOT(windowChanged(NetworkWindow*,NetworkWindow*)));

	return true;
}

typedef void (*tc_COPASI_api)(copasi_model (*getCopasiModel)());

void CopasiExporter::setupFunctionPointers( QLibrary * library)
{
	tc_COPASI_api f = (tc_COPASI_api)library->resolve("tc_COPASI_api");
	if (f)
		f(	&getCopasiModel );
}

void CopasiExporter::historyChanged(int)
{
	modelNeedsUpdate = true;
}

void CopasiExporter::windowChanged(NetworkWindow*,NetworkWindow*)
{
	modelNeedsUpdate = true;
}

/*******************************************
    C Interface
********************************************/

CopasiExporter_FtoS CopasiExporter::fToS;

copasi_model CopasiExporter::getCopasiModel()
{
	return fToS.getCopasiModel();
}

copasi_model CopasiExporter_FtoS::getCopasiModel()
{
	copasi_model m;
	QSemaphore * s = new QSemaphore(1);
	s->acquire();
	emit getCopasiModel(s,&m);
	s->acquire();
	s->release();
	delete s;
	return m;
}

void CopasiExporter::getCopasiModel(QSemaphore * sem, copasi_model * m)
{
	if (modelNeedsUpdate)
		updateModel();

	if (m)
		(*m) = model;

	if (sem)
		sem->release();
}

/***************************************************
    Move SBML to TinkerCell format and vice versa
*****************************************************/

void CopasiExporter::updateModel()
{
	if (model.CopasiDataModelPtr)
		removeCopasiModel(model);
		
	model.CopasiModelPtr = 0;
	model.CopasiDataModelPtr = 0;
	model.qHash = 0;
	
	if (!currentNetwork()) return;
	
	model = createCopasiModel("tinkercell");
	
	QList<ItemHandle*> handles = currentNetwork()->handles(true);

	NumericalDataTable params = BasicInformationTool::getUsedParameters(handles);
	NumericalDataTable stoic_matrix = StoichiometryTool::getStoichiometry(handles);
	QStringList rates = StoichiometryTool::getRates(handles);
	QStringList species, eventTriggers, eventActions, assignmentNames,
				assignmentDefs, fixedVars, functionNames, functionDefs, functionArgs;

	species = stoic_matrix.rowNames();
	QVector<double> initialValues(species.size(),0.0);
	QVector<QString> speciesCompartments(species.size(),tr("DefaultCompartment"));
	QVector<double> compartmentVolumes(species.size(),1.0);
	ItemHandle * parentHandle;

	QRegExp regex(tr("\\.(?!\\d)"));	
	int i,j;
	QString s1,s2;

	for (i=0; i < handles.size(); ++i)
	{
		if (handles[i])// && handles[i]->family())
		{
			if (handles[i]->children.isEmpty())
			{
				if (handles[i]->hasNumericalData(tr("Initial Value")))
				{
					int k = species.indexOf(handles[i]->fullName(tr("_")));
					if (k >= 0)
					{
						initialValues[k] = handles[i]->numericalData(tr("Initial Value"));
						if (parentHandle = handles[i]->parentOfFamily(tr("Compartment")))
						{
							speciesCompartments[k] = parentHandle->fullName(tr("_"));
							if (parentHandle->hasNumericalData(tr("Initial Value")))
								compartmentVolumes[k] = parentHandle->numericalData(tr("Initial Value"));
							else
								compartmentVolumes[k] = 1.0;
						}
					}

					if (handles[i]->hasNumericalData(tr("Fixed")) &&
						handles[i]->numericalData(tr("Fixed")) > 0)
					{
						fixedVars << handles[i]->fullName(tr("_"));
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
	
	QHash<QString, copasi_compartment> compartmentHash;
	
	//create list of species
	for (int i=0; i < species.size(); ++i)
	{
		copasi_compartment c;
		
		if (compartmentHash.contains(speciesCompartments[i]))
			c = compartmentHash[ speciesCompartments[i] ];
		else
		{
			c = createCompartment(model, speciesCompartments[i].toAscii().data(), compartmentVolumes[i]);
			compartmentHash[ speciesCompartments[i] ] = c;
		}
		createSpecies(c, species[i].toAscii().data(), initialValues[i]);
		if (fixedVars.contains(species[i]))
			setBoundarySpecies(model, species[i].toAscii().data(), 1);
	}
	
	//create list of parameters
	for (int i=0; i < params.rows(); ++i)
	{
		setGlobalParameter(model, params.rowName(i).toAscii().data(), params.value(i,0));
	}
	
	//list of assignments
	for (int i=0; i < assignmentNames.size(); ++i)
	{
		createVariable(model, assignmentNames[i].toAscii().data(), assignmentDefs[i].toAscii().data());
	}

	//create list of reactions
	for (int i=0; i < stoic_matrix.columns(); ++i)
	{
		copasi_reaction reac = createReaction(model, stoic_matrix.columnName(i).toAscii().data());
		setReactionRate(reac, rates[i].toAscii().data());

		for (int j=0; j < stoic_matrix.rows(); ++j)
			if (stoic_matrix.value(j,i) < 0)
			{
				addReactant(reac, stoic_matrix.rowName(j).toAscii().data(), -stoic_matrix.value(j,i));
			}
			else
			if (stoic_matrix.value(j,i) > 0)
			{
				addProduct(reac, stoic_matrix.rowName(j).toAscii().data(), stoic_matrix.value(j,i));
			}		
	}
	
	//list of events
/*	for (int i=0; i < eventTriggers.size(); ++i)
	{
		QStringList actions = eventActions[i].split(";");
		for (int j=0; j < actions.size(); ++j)
		{
			QStringList words = actions[j].split("=");
			if (words.size() == 2)
			{
				createEvent(model, (QString("event") + QString::number(i)).toAscii().data(), eventTriggers[i].toAscii().data(), words[0].trimmed().toAscii().data(), words[1].trimmed().toAscii().data());
				break;
			}
		}
	}*/
}

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::CopasiExporter * copasiTool = new Tinkercell::CopasiExporter;
	main->addTool(copasiTool);
}

