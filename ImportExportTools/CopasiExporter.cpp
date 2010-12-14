#include <string>
#include <QFileDialog>
#include "BasicInformationTool.h"
#include "StoichiometryTool.h"
#include "CopasiExporter.h"
#include "ConsoleWindow.h"

using namespace Tinkercell;

void model1(copasi_model model)
{
	//species
	copasi_compartment cell = createCompartment(model, "cell", 1.0);
	createSpecies(cell, "mRNA", 0);
	createSpecies(cell, "Protein", 0);
	
	//parameters	
	setGlobalParameter(model, "d1", 1.0);
	setGlobalParameter(model, "d2", 0.2);  
	setGlobalParameter(model, "k0", 2.0);
	setGlobalParameter(model, "k1", 1.0);
	setGlobalParameter(model, "h", 4.0);  
	setGlobalParameter(model, "Kd", 1.0);
	setGlobalParameter(model, "leak", 0.1);  
	
	//reactions -- make sure all parameters or species are defined BEFORE this step
	copasi_reaction R1 = createReaction(model, "R1");  //  mRNA production
	addProduct(R1, "mRNA", 1.0);
	setReactionRate(R1, "leak + k0 * (Protein^h) / (Kd + (Protein^h))");

	copasi_reaction R2 = createReaction(model, "R2");  // Protein production
	addProduct(R2, "Protein", 1.0);
	setReactionRate(R2, "k1*mRNA");

	copasi_reaction R3 = createReaction(model, "R3"); // mRNA degradation
	addReactant(R3, "mRNA", 1.0);
	setReactionRate(R3, "d1*mRNA");
	
	copasi_reaction R4 = createReaction(model, "R4"); // Protein degradation
	addReactant(R4, "Protein", 1.0);
	setReactionRate(R4, "d2*Protein");
}


CopasiExporter::CopasiExporter() : Tool("COPASI","Export")
{
	modelNeedsUpdate = true;	
	qRegisterMetaType< copasi_model >("copasi_model");
	qRegisterMetaType< copasi_model* >("copasi_model*");
}

CopasiExporter::~CopasiExporter()
{
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

typedef void (*tc_COPASI_api)(
	tc_matrix (*simulateDeterministic)(double startTime, double endTime, int numSteps),
	tc_matrix (*simulateStochastic)(double startTime, double endTime, int numSteps),
	tc_matrix (*simulateHybrid)(double startTime, double endTime, int numSteps),
	tc_matrix (*simulateTauLeap)(double startTime, double endTime, int numSteps),
	tc_matrix (*getSteadyState)(),
	tc_matrix (*steadyStateScan)(const char * param, double start, double end, int numSteps),
	tc_matrix (*steadyStateScan2D)(const char * param1, double start1, double end1, int numSteps1,const char * param2, double start2, double end2, int numSteps2),
	tc_matrix (*getJacobian)(),
	tc_matrix (*getEigenvalues)(),
	tc_matrix (*getUnscaledElasticities)(),
	tc_matrix (*getUnscaledConcentrationCC)(),
	tc_matrix (*getUnscaledFluxCC)(),
	tc_matrix (*getScaledElasticities)(),
	tc_matrix (*getScaledConcentrationCC)(),
	tc_matrix (*getScaledFluxCC)()
);

void CopasiExporter::setupFunctionPointers( QLibrary * library)
{
	tc_COPASI_api f = (tc_COPASI_api)library->resolve("tc_COPASI_api");
	if (f)
		f(
			&simulateDeterministic,
			&simulateStochastic,
			&simulateHybrid,
			&simulateTauLeap,
			&getSteadyState,
			&steadyStateScan,
			&steadyStateScan2D,
			&getJacobian,
			&getEigenvalues,
			&getUnscaledElasticities,
			&getUnscaledConcentrationCC,
			&getUnscaledFluxCC,
			&getScaledElasticities,
			&getScaledConcentrationCC,
			&getScaledFluxCC
		);
}

void CopasiExporter::historyChanged(int)
{
	modelNeedsUpdate = true;
}

void CopasiExporter::windowChanged(NetworkWindow*,NetworkWindow*)
{
	modelNeedsUpdate = true;
}


/***************************************************
      Copasi export
*****************************************************/

void SimulationThread::updateModel(QList<ItemHandle*> & handles)
{
	model.CopasiModelPtr = 0;
	model.CopasiDataModelPtr = 0;
	model.qHash = 0;
	
	++totalModelCount;
	QString modelName = tr("tinkercell") + QString::number(totalModelCount);
	model = createCopasiModel(modelName.toAscii().data());

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
	QString s1,s2,name;

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
	
	QFile fout("copasi.txt");
	fout.open(QFile::WriteOnly | QFile::Text );
	QString commands;
	QHash<QString, copasi_compartment> compartmentHash;

	//create list of species
	for (int i=0; i < species.size(); ++i)
	{
		copasi_compartment c;
		
		if (compartmentHash.contains(speciesCompartments[i]))
		{
			c = compartmentHash[ speciesCompartments[i] ];
		}
		else
		{
			c = createCompartment(model, speciesCompartments[i].toAscii().data(), compartmentVolumes[i]);
			compartmentHash[ speciesCompartments[i] ] = c;
			commands += speciesCompartments[i] + tr(" = createCompartment(model,") + speciesCompartments[i] + tr(",") + QString::number(compartmentVolumes[i]) + tr(");\n");
		}
		createSpecies(c, species[i].toAscii().data(), initialValues[i]);
		commands += tr("createSpecies(") + speciesCompartments[i] + tr(",") + species[i] + tr(",") + QString::number(initialValues[i]) + tr(");\n");
		if (fixedVars.contains(species[i]))
		{
			setBoundarySpecies(model, species[i].toAscii().data(), 1);
			commands += tr("setBoundarySpecies(model, ") + species[i] + tr("1);\n");
		}
	}
	
	//create list of parameters
	for (int i=0; i < params.rows(); ++i)
	{
		setGlobalParameter(model, params.rowName(i).toAscii().data(), params.value(i,0));
		commands += tr("setGlobalParameter(model,") + params.rowName(i) + tr(",") + QString::number(params.value(i,0)) + tr(");\n");
	}
	
	//list of assignments
	for (int i=0; i < assignmentNames.size(); ++i)
	{
		createVariable(model, assignmentNames[i].toAscii().data(), assignmentDefs[i].toAscii().data());
		commands += tr("createVariable(model, ") + assignmentNames[i] + tr(",") + assignmentDefs[i] + tr(");\n");
	}

	//create list of reactions
	for (int i=0; i < stoic_matrix.columns(); ++i)
	{
		copasi_reaction reac = createReaction(model, stoic_matrix.columnName(i).toAscii().data());
		setReactionRate(reac, rates[i].toAscii().data());
		
		commands += tr("r") + QString::number(i) + tr(" = createReaction(model, ") + stoic_matrix.columnName(i) + tr(");\n");
		commands += tr("setReactionRate(") + tr("r") + QString::number(i) + tr(",") + rates[i] + tr(");\n");

		for (int j=0; j < stoic_matrix.rows(); ++j)
			if (stoic_matrix.value(j,i) < 0)
			{
				addReactant(reac, stoic_matrix.rowName(j).toAscii().data(), -stoic_matrix.value(j,i));
				commands += tr("addReactant(") + tr("r") + QString::number(i) + tr(",") + stoic_matrix.rowName(j) + tr(",") + QString::number(-stoic_matrix.value(j,i)) + tr(");\n");
			}
			else
			if (stoic_matrix.value(j,i) > 0)
			{
				addProduct(reac, stoic_matrix.rowName(j).toAscii().data(), stoic_matrix.value(j,i));
				commands += tr("addProduct(") + tr("r") + QString::number(i) + tr(",") + stoic_matrix.rowName(j) + tr(",") + QString::number(stoic_matrix.value(j,i)) + tr(");\n");
			}
	}
	
	fout.write(commands.toAscii());
	fout.close();
	
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

SimulationThread * CopasiExporter::odeThread = 0;
SimulationThread * CopasiExporter::stochThread = 0;
SimulationThread * CopasiExporter::ssThread = 0;
SimulationThread * CopasiExporter::jacThread = 0;
SimulationThread * CopasiExporter::mcaThread = 0;

tc_matrix CopasiExporter::simulateDeterministic(double startTime, double endTime, int numSteps)
{
	if (odeThread)
	{
		if (odeThread->isRunning())
			odeThread->terminate();
		odeThread->updateModel();
		odeThread->setMethod(SimulationThread::DeterministicSimulation);
		odeThread->setStartTime(startTime);
		odeThread->setEndTime(endTime);
		odeThread->setNumPoints(numSteps);
		QSemaphore sem(1);
		sem.acquire();
		odeThread->setSemaphore(&sem);
		odeThread->start();
		sem.acquire();
		sem.release();
		return ConvertValue(odeThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::simulateStochastic(double startTime, double endTime, int numSteps)
{
	if (stochThread)
	{
		if (stochThread->isRunning())
			stochThread->terminate();
		stochThread->updateModel();
		stochThread->setMethod(SimulationThread::StochasticSimulation);
		stochThread->setStartTime(startTime);
		stochThread->setEndTime(endTime);
		stochThread->setNumPoints(numSteps);
		QSemaphore sem(1);
		sem.acquire();
		stochThread->setSemaphore(&sem);
		stochThread->start();
		sem.acquire();
		sem.release();
		return ConvertValue(stochThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::simulateHybrid(double startTime, double endTime, int numSteps)
{
	if (odeThread)
	{
		if (odeThread->isRunning())
			odeThread->terminate();
		odeThread->updateModel();
		odeThread->setMethod(SimulationThread::HybridSimulation);
		odeThread->setStartTime(startTime);
		odeThread->setEndTime(endTime);
		odeThread->setNumPoints(numSteps);
		QSemaphore sem(1);
		sem.acquire();
		odeThread->setSemaphore(&sem);
		odeThread->start();
		sem.acquire();
		sem.release();
		return ConvertValue(odeThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::simulateTauLeap(double startTime, double endTime, int numSteps)
{
	if (stochThread)
	{
		if (stochThread->isRunning())
			stochThread->terminate();
		stochThread->updateModel();
		stochThread->setMethod(SimulationThread::TauLeapSimulation);
		stochThread->setStartTime(startTime);
		stochThread->setEndTime(endTime);
		stochThread->setNumPoints(numSteps);
		QSemaphore sem(1);
		sem.acquire();
		stochThread->setSemaphore(&sem);
		stochThread->start();
		sem.acquire();
		sem.release();
		return ConvertValue(stochThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getSteadyState()
{
	if (ssThread)
	{
		if (ssThread->isRunning())
			ssThread->terminate();
		ssThread->updateModel();
		ssThread->setMethod(SimulationThread::SteadyState);
		QSemaphore sem(1);
		sem.acquire();
		ssThread->setSemaphore(&sem);
		ssThread->start();
		sem.acquire();
		sem.release();
		return ConvertValue(ssThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::steadyStateScan(const char * param, double start, double end, int numSteps)
{
	if (ssThread)
	{
		if (ssThread->isRunning())
			ssThread->terminate();
		ssThread->updateModel();
		ssThread->setMethod(SimulationThread::SteadyState);
		ssThread->setParameterRange(QString(param), start, end, numSteps);
		QSemaphore sem(1);
		sem.acquire();
		ssThread->setSemaphore(&sem);
		ssThread->start();
		sem.acquire();
		sem.release();
		return ConvertValue(ssThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::steadyStateScan2D(const char * param1, double start1, double end1, int numSteps1,const char * param2, double start2, double end2, int numSteps2)
{
	if (ssThread)
	{
		if (ssThread->isRunning())
			ssThread->terminate();
		ssThread->updateModel();
		ssThread->setMethod(SimulationThread::SteadyState);
		ssThread->setParameterRange(QString(param1), start1, end1, numSteps1);
		ssThread->setParameterRange(QString(param2), start2, end2, numSteps2);
		QSemaphore sem(1);
		sem.acquire();
		ssThread->setSemaphore(&sem);
		ssThread->start();
		sem.acquire();
		sem.release();
		return ConvertValue(ssThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getJacobian()
{
	if (ssThread)
	{
		if (ssThread->isRunning())
			ssThread->terminate();
		ssThread->updateModel();
		ssThread->setMethod(SimulationThread::Jacobian);
		QSemaphore sem(1);
		sem.acquire();
		ssThread->setSemaphore(&sem);
		ssThread->start();
		sem.acquire();
		sem.release();
		return ConvertValue(ssThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getEigenvalues()
{
	if (ssThread)
	{
		if (ssThread->isRunning())
			ssThread->terminate();
		ssThread->updateModel();
		ssThread->setMethod(SimulationThread::Eigenvalues);
		QSemaphore sem(1);
		sem.acquire();
		ssThread->setSemaphore(&sem);
		ssThread->start();
		sem.acquire();
		sem.release();
		return ConvertValue(ssThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getUnscaledElasticities()
{
	if (mcaThread)
	{
		if (mcaThread->isRunning())
			mcaThread->terminate();
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::UnscaledElasticities);
		QSemaphore sem(1);
		sem.acquire();
		mcaThread->setSemaphore(&sem);
		mcaThread->start();
		sem.acquire();
		sem.release();
		return ConvertValue(mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getUnscaledConcentrationCC()
{
	if (mcaThread)
	{
		if (mcaThread->isRunning())
			mcaThread->terminate();
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::UnscaledConcentrationCC);
		QSemaphore sem(1);
		sem.acquire();
		mcaThread->setSemaphore(&sem);
		mcaThread->start();
		sem.acquire();
		sem.release();
		return ConvertValue(mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getUnscaledFluxCC()
{
	if (mcaThread)
	{
		if (mcaThread->isRunning())
			mcaThread->terminate();
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::UnscaledFluxCC);
		QSemaphore sem(1);
		sem.acquire();
		mcaThread->setSemaphore(&sem);
		mcaThread->start();
		sem.acquire();
		sem.release();
		return ConvertValue(mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getScaledElasticities()
{
	if (mcaThread)
	{
		if (mcaThread->isRunning())
			mcaThread->terminate();
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::ScaledElasticities);
		QSemaphore sem(1);
		sem.acquire();
		mcaThread->setSemaphore(&sem);
		mcaThread->start();
		sem.acquire();
		sem.release();
		return ConvertValue(mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getScaledConcentrationCC()
{
	if (mcaThread)
	{
		if (mcaThread->isRunning())
			mcaThread->terminate();
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::ScaledConcentrationCC);
		QSemaphore sem(1);
		sem.acquire();
		mcaThread->setSemaphore(&sem);
		mcaThread->start();
		sem.acquire();
		sem.release();
		return ConvertValue(mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getScaledFluxCC()
{
	if (mcaThread)
	{
		if (mcaThread->isRunning())
			mcaThread->terminate();
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::ScaledFluxCC);
		QSemaphore sem(1);
		sem.acquire();
		mcaThread->setSemaphore(&sem);
		mcaThread->start();
		sem.acquire();
		sem.release();
		return ConvertValue(mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

void SimulationThread::updateModel()
{
	QSemaphore sem(1);
	bool changed = true;
	QList<ItemHandle*> handles;
	sem.acquire();
	emit getHandles( &sem, handles, &changed);
	sem.acquire();
	sem.release();
	
	if (changed)
		updateModel(handles);
}

void SimulationThread::setMethod(AnalysisMethod)
{
}

void SimulationThread::setSemaphore(QSemaphore*)
{
}

void SimulationThread::setStartTime(double)
{
}

void SimulationThread::setEndTime(double)
{
}

void SimulationThread::setNumPoints(int)
{
}

void SimulationThread::setParameterRange(const QString& param, double start, double end, int numPoints)
{
}

NumericalDataTable& SimulationThread::result()
{
}

int SimulationThread::totalModelCount = 0;



