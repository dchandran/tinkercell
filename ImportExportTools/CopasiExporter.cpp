#include <string>
#include <QInputDialog>
#include <QFileDialog>
#include "PlotTool.h"
#include "BasicInformationTool.h"
#include "StoichiometryTool.h"
#include "CopasiExporter.h"
#include "ConsoleWindow.h"
#include "DynamicLibraryMenu.h"

namespace Tinkercell
{

CopasiExporter_FtoS CopasiExporter::fToS;

CopasiExporter::CopasiExporter() : Tool("COPASI","Export"), simThread(0), simDialog(0), reorderingEnabled(true)
{
	connect(&fToS, SIGNAL(simulate(QSemaphore *, double, double, int, int, tc_matrix*)),
					this, SLOT(simulate(QSemaphore *, double, double, int, int, tc_matrix*)));

	connect(&fToS, SIGNAL(otherAnalysis(QSemaphore *, int, tc_matrix*)),
					this, SLOT(otherAnalysis(QSemaphore *, int, tc_matrix*)));

	connect(&fToS, SIGNAL(steadyStateScan(QSemaphore *, const char *, double , double, int , tc_matrix*)),
					this, SLOT(steadyStateScan(QSemaphore *, const char *, double , double, int , tc_matrix*)));

	connect(&fToS, SIGNAL(steadyStateScan2D(QSemaphore *, const char *, double , double, int ,const char * , double , double , int , tc_matrix*)),
					this, SLOT(steadyStateScan2D(QSemaphore *, const char *, double , double, int ,const char * , double , double , int , tc_matrix*)));

	connect(&fToS, SIGNAL(gaOptimize(QSemaphore *, const char*, tc_matrix*)),
					this, SLOT(gaOptimize(QSemaphore *, const char*, tc_matrix*)));

	connect(&fToS, SIGNAL(updateParams(QSemaphore *, tc_matrix)),
					this, SLOT(updateParams(QSemaphore *, tc_matrix)));

	connect(&fToS, SIGNAL(updateParam(QSemaphore *, const char*, double)),
					this, SLOT(updateParam(QSemaphore *, const char*, double)));

	connect(&fToS, SIGNAL(enableAssignmentRulesReordering(QSemaphore *, int)),
					this, SLOT(enableAssignmentRulesReordering(QSemaphore *, int)));

	needsUpdate = true;
}

CopasiExporter::~CopasiExporter()
{
	delete simThread;
	copasi_end();
}

bool CopasiExporter::setMainWindow(MainWindow * main)
{
	Tool::setMainWindow(main);	
	if (!mainWindow) return false;
	
	simDialog = new SimulationDialog(mainWindow);
	simThread = new SimulationThread(mainWindow);
	connect(simDialog, SIGNAL(updateModel()), this, SLOT(updateModel()));
	simDialog->setThread(simThread);

	connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
	connect(main,SIGNAL(historyChanged(int)),this, SLOT(historyChanged(int)));
	connect(main,SIGNAL(windowChanged(NetworkWindow*,NetworkWindow*)),this, SLOT(windowChanged(NetworkWindow*,NetworkWindow*)));
	connect(mainWindow,SIGNAL(toolLoaded(Tool *)),this, SLOT(toolLoaded(Tool*)));

	toolLoaded(0);

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
	tc_matrix (*getScaledFluxCC)(),
	tc_matrix (*reducedStoichiometry)(),
	tc_matrix (*elementaryFluxModes)(),
	tc_matrix (*LMat)(),
	tc_matrix (*KMat)(),
	tc_matrix (*Optimize)(const char * ),
	void (*updateParameters)(tc_matrix),
	void (*updateParameter)(const char *, double),
	void (*enableAssignmentRulesOrdering)(int)
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
			&getScaledFluxCC,
			&reducedStoichiometry,
			&elementaryFluxModes,
			&KMatrix,
			&LMatrix,
			&gaOptimize,
			&updateParams,
			&updateParam,
			&enableAssignmentRulesReordering
		);
}

void CopasiExporter::enableAssignmentRulesReordering(QSemaphore * sem, int a)
{
	bool reorderingEnabled2 = (a > 0);
	if (reorderingEnabled != reorderingEnabled2)
	{
		reorderingEnabled = reorderingEnabled2;

		if (reorderingEnabled)
			cEnableAssignmentRuleReordering();
		else
			cDisableAssignmentRuleReordering();

		needsUpdate = true;
	}

	if (sem)
		sem->release();
}

void CopasiExporter::enableAssignmentRulesReordering(int a)
{
	fToS.enableAssignmentRulesReordering(a);
}

void CopasiExporter::historyChanged(int)
{
	needsUpdate = true;
}

void CopasiExporter::windowChanged(NetworkWindow*,NetworkWindow*)
{
	needsUpdate = true;
}

/***************************************************
      Copasi export
*****************************************************/

void CopasiExporter::odeSim()
{
	simDialog->setMethod(SimulationThread::DeterministicSimulation);
}

void CopasiExporter::getEig()
{
	simDialog->setMethod(SimulationThread::Eigenvalues);
}

void CopasiExporter::getJac()
{
	simDialog->setMethod(SimulationThread::Jacobian);
}

void CopasiExporter::scan2D()
{
	simDialog->setMethod(SimulationThread::SteadyStateScan2D);
}

void CopasiExporter::scan1D()
{
	simDialog->setMethod(SimulationThread::SteadyStateScan1D);
}

void CopasiExporter::getSS()
{
	simDialog->setMethod(SimulationThread::SteadyState);
}

void CopasiExporter::hybridSim()
{
	simDialog->setMethod(SimulationThread::HybridSimulation);
}

void CopasiExporter::tauleapSim()
{
	simDialog->setMethod(SimulationThread::TauLeapSimulation);
}

void CopasiExporter::ssaSim()
{
	simDialog->setMethod(SimulationThread::StochasticSimulation);
}

void CopasiExporter::scaledElasticities()
{
	simDialog->setMethod(SimulationThread::ScaledElasticities);
}

void CopasiExporter::scaledConcentrationCC()
{
	simDialog->setMethod(SimulationThread::ScaledConcentrationCC);
}

void CopasiExporter::scaledFluxCC()
{
	simDialog->setMethod(SimulationThread::ScaledFluxCC);
}

void CopasiExporter::redStoic()
{
	tc_matrix m;
	otherAnalysis(0, SimulationThread::ReducedStoichiometry, &m);
	NumericalDataTable * N = ConvertValue(m);
	
	if (console())
		console()->printTable(*N);
	
	if (currentScene())
	{
		currentScene()->find(N->rowNames());
	}
		
	delete N;
}

void CopasiExporter::getELM()
{
	tc_matrix m;
	otherAnalysis(0, SimulationThread::ElementaryFluxModes, &m);
	NumericalDataTable * N = ConvertValue(m);

	if (console())
		console()->printTable(*N);

	if (currentScene())
	{
		currentScene()->find(N->rowNames());
	}

	delete N;
}

void CopasiExporter::optimize()
{
	QString input = QInputDialog::getText(this, "Optimize", "Enter an objective function or a filename (least squares fit)");
	if (input.isNull() || input.isEmpty()) return;

	tc_matrix m;
	gaOptimize(0, input.toAscii().data(), &m);
}

void CopasiExporter::exportSBML(const QString& file)
{
	if (needsUpdate)
		updateModel();
	if (simThread)
		simThread->exportSBML(file);
}

void CopasiExporter::updateModel()
{
	if (simThread)
	{
		simThread->plotResult(false);
		if (currentNetwork() && needsUpdate)
		{
			QList<ItemHandle*> lst = currentNetwork()->handles(true);
			simThread->updateModel(lst);
		}
	}
	needsUpdate = false;
}

void CopasiExporter::simulate(QSemaphore * sem, double startTime, double endTime, int numSteps, int type, tc_matrix * m)
{
	if (simThread)
	{
		updateModel();
		simThread->setMethod(SimulationThread::AnalysisMethod(type));
		simThread->setStartTime(startTime);
		simThread->setEndTime(endTime);
		simThread->setNumPoints(numSteps);
		simThread->run();
		(*m) = simThread->result();
	}
	if (sem)
		sem->release();
}

void CopasiExporter::otherAnalysis(QSemaphore * sem, int type, tc_matrix * m)
{
	if (simThread)
	{
		updateModel();
		simThread->setMethod(SimulationThread::AnalysisMethod(type));
		simThread->run();
		(*m) = simThread->result();
	}
	if (sem)
		sem->release();
}

void CopasiExporter::steadyStateScan(QSemaphore * sem, const char * param, double start, double end, int numSteps, tc_matrix * m)
{
	if (simThread)
	{
		updateModel();
		simThread->setMethod(SimulationThread::SteadyStateScan1D);
		simThread->setParameterRange(tr(param),start,end,numSteps);
		simThread->run();
		(*m) = simThread->result();
	}
	if (sem)
		sem->release();
}

void CopasiExporter::steadyStateScan2D(QSemaphore * sem, const char * param1, double start1, double end1, int numSteps1,const char * param2, double start2, double end2, int numSteps2, tc_matrix * m)
{
	if (simThread)
	{
		updateModel();
		simThread->setMethod(SimulationThread::SteadyStateScan2D);
		simThread->setParameterRange(tr(param1),start1,end1,numSteps1);
		simThread->setParameterRange(tr(param2),start2,end2,numSteps2);
		simThread->run();
		(*m) = simThread->result();
	}
	if (sem)
		sem->release();
}

void CopasiExporter::gaOptimize(QSemaphore * sem, const char * f, tc_matrix* m)
{
	if (simThread)
	{
		updateModel();
		simThread->setMethod(SimulationThread::GA);
		simThread->setObjective(tr(f));
		simThread->run();
		(*m) = simThread->result();
	}
	if (sem)
		sem->release();
}

void CopasiExporter::updateParams(QSemaphore * sem, tc_matrix m)
{
	if (simThread)
	{
		updateModel();
		DataTable<qreal>* dat = ConvertValue(m);
		simThread->updateModelParameters(*dat);
		delete dat;
	}
	if (sem)
		sem->release();
}

void CopasiExporter::updateParam(QSemaphore * sem, const char * s, double v)
{
	if (simThread)
	{
		updateModel();
		simThread->updateModelParameter(tr(s), v);
	}
	if (sem)
		sem->release();
}

tc_matrix CopasiExporter_FtoS::simulate(double startTime, double endTime, int numSteps, int type)
{
	tc_matrix m;
	QSemaphore * sem = new QSemaphore(1);
	sem->acquire();
	emit simulate(sem, startTime, endTime, numSteps, type, &m);
	sem->acquire();
	sem->release();
	delete sem;
	return m;
}

tc_matrix CopasiExporter_FtoS::steadyStateScan(const char * param, double start, double end, int numSteps)
{
	tc_matrix m;
	QSemaphore * sem = new QSemaphore(1);
	sem->acquire();
	emit steadyStateScan(sem, param, start, end, numSteps, &m);
	sem->acquire();
	sem->release();
	delete sem;
	return m;
}

tc_matrix CopasiExporter_FtoS::steadyStateScan2D(const char * param1, double start1, double end1, int numSteps1,const char * param2, double start2, double end2, int numSteps2)
{
	tc_matrix m;
	QSemaphore * sem = new QSemaphore(1);
	sem->acquire();
	emit steadyStateScan2D(sem, param1, start1, end1, numSteps1, param2, start2, end2, numSteps2, &m);
	sem->acquire();
	sem->release();
	delete sem;
	return m;
}

tc_matrix CopasiExporter_FtoS::otherAnalysis(int type)
{
	tc_matrix m;
	QSemaphore * sem = new QSemaphore(1);
	sem->acquire();
	emit otherAnalysis(sem, type, &m);
	sem->acquire();
	sem->release();
	delete sem;
	return m;
}

tc_matrix CopasiExporter_FtoS::gaOptimize(const char* c)
{
	tc_matrix m;
	QSemaphore * sem = new QSemaphore(1);
	sem->acquire();
	emit gaOptimize(sem, c, &m);
	sem->acquire();
	sem->release();
	delete sem;
	return m;
}

void CopasiExporter_FtoS::updateParams(tc_matrix m)
{
	QSemaphore * sem = new QSemaphore(1);
	sem->acquire();
	emit updateParams(sem, m);
	sem->acquire();
	sem->release();
	delete sem;
}

void CopasiExporter_FtoS::updateParam(const char * s, double v)
{
	QSemaphore * sem = new QSemaphore(1);
	sem->acquire();
	emit updateParam(sem, s, v);
	sem->acquire();
	sem->release();
	delete sem;
}

void CopasiExporter_FtoS::enableAssignmentRulesReordering(int i)
{
	QSemaphore * sem = new QSemaphore(1);
	sem->acquire();
	emit enableAssignmentRulesReordering(sem, i);
	sem->acquire();
	sem->release();
	delete sem;
}

tc_matrix CopasiExporter::simulateDeterministic(double startTime, double endTime, int numSteps)
{
	return fToS.simulate(startTime, endTime, numSteps, (int)(SimulationThread::DeterministicSimulation));
}

tc_matrix CopasiExporter::simulateStochastic(double startTime, double endTime, int numSteps)
{
	return fToS.simulate(startTime, endTime, numSteps, (int)(SimulationThread::StochasticSimulation));
}

tc_matrix CopasiExporter::simulateHybrid(double startTime, double endTime, int numSteps)
{
	return fToS.simulate(startTime, endTime, numSteps, (int)(SimulationThread::HybridSimulation));
}

tc_matrix CopasiExporter::simulateTauLeap(double startTime, double endTime, int numSteps)
{
	return fToS.simulate(startTime, endTime, numSteps, (int)(SimulationThread::TauLeapSimulation));
}

tc_matrix CopasiExporter::getSteadyState()
{
	return fToS.otherAnalysis((int)(SimulationThread::SteadyState));
}

tc_matrix CopasiExporter::steadyStateScan(const char * param, double start, double end, int numSteps)
{
	return fToS.steadyStateScan(param, start, end, numSteps);
}

tc_matrix CopasiExporter::steadyStateScan2D(const char * param1, double start1, double end1, int numSteps1,const char * param2, double start2, double end2, int numSteps2)
{
	return fToS.steadyStateScan2D(param1, start1, end1, numSteps1, param2, start2, end2, numSteps2);
}

tc_matrix CopasiExporter::getJacobian()
{
	return fToS.otherAnalysis((int)(SimulationThread::Jacobian));
}

tc_matrix CopasiExporter::getEigenvalues()
{
	return fToS.otherAnalysis((int)(SimulationThread::Eigenvalues));
}

tc_matrix CopasiExporter::getUnscaledElasticities()
{
	return fToS.otherAnalysis((int)(SimulationThread::UnscaledElasticities));
}

tc_matrix CopasiExporter::getUnscaledConcentrationCC()
{
	return fToS.otherAnalysis((int)(SimulationThread::UnscaledConcentrationCC));
}

tc_matrix CopasiExporter::getUnscaledFluxCC()
{
	return fToS.otherAnalysis((int)(SimulationThread::UnscaledFluxCC));
}

tc_matrix CopasiExporter::getScaledElasticities()
{
	return fToS.otherAnalysis((int)(SimulationThread::ScaledElasticities));
}

tc_matrix CopasiExporter::getScaledConcentrationCC()
{
	return fToS.otherAnalysis((int)(SimulationThread::ScaledConcentrationCC));
}

tc_matrix CopasiExporter::getScaledFluxCC()
{
	return fToS.otherAnalysis((int)(SimulationThread::ScaledFluxCC));
}

tc_matrix CopasiExporter::reducedStoichiometry()
{
	return fToS.otherAnalysis((int)(SimulationThread::ReducedStoichiometry));
}

tc_matrix CopasiExporter::elementaryFluxModes()
{
	return fToS.otherAnalysis((int)(SimulationThread::ElementaryFluxModes));
}

tc_matrix CopasiExporter::KMatrix()
{
	return fToS.otherAnalysis((int)(SimulationThread::KMatrix));
}

tc_matrix CopasiExporter::LMatrix()
{
	return fToS.otherAnalysis((int)(SimulationThread::LMatrix));
}

tc_matrix CopasiExporter::gaOptimize(const char * s)
{
	return fToS.gaOptimize(s);
}

void CopasiExporter::updateParams(tc_matrix params)
{
	return fToS.updateParams(params);
}

void CopasiExporter::updateParam(const char * s, double v)
{
	return fToS.updateParam(s, v);
}

void CopasiExporter::toolLoaded(Tool*)
{
    static bool connected = false;
    
    if (!connected && mainWindow->tool(tr("Dynamic Library Menu")))
	{
    	QWidget * widget = mainWindow->tool(tr("Dynamic Library Menu"));
        if (widget)
        {
				connected = true;
				DynamicLibraryMenu * libMenu = static_cast<DynamicLibraryMenu*>(widget);
				QString appDir = QCoreApplication::applicationDirPath();
				QPixmap odepng(tr(":/images/cvode.png"));
				QPixmap stochastic(tr(":/images/stochastic.png"));
				QPixmap scan(tr(":/images/steadystate.png"));
				QPixmap mcaicon(tr(":/images/fullBinding.png"));
				QPixmap nodedges(tr(":/images/nodedges.png"));
				QToolButton * button;
				QAction * menuItem;
				
				//ODE
				button = libMenu->addFunction(tr("Simulation"), tr("Deterministic"), QIcon(odepng));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(odeSim()));
				}

				menuItem = libMenu->addMenuItem(tr("Simulation"), tr("Deterministic"), QIcon(odepng), true);
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(odeSim()));
				}
				
				//SSA
				button = libMenu->addFunction(tr("Simulation"), tr("Stochastic (exact)"), QIcon(stochastic));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(ssaSim()));
				}

				menuItem = libMenu->addMenuItem(tr("Simulation"), tr("Stochastic (exact)"), QIcon(stochastic));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(ssaSim()));
				}
				
				//TauLeap
				button = libMenu->addFunction(tr("Simulation"), tr("Stochastic (tau-leap)"), QIcon(stochastic));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(tauleapSim()));
				}

				menuItem = libMenu->addMenuItem(tr("Simulation"), tr("Stochastic (tau-leap)"), QIcon(stochastic));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(tauleapSim()));
				}
				
				//Hybrid
				button = libMenu->addFunction(tr("Simulation"), tr("Hybrid"), QIcon(stochastic));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(hybridSim()));
				}

				menuItem = libMenu->addMenuItem(tr("Simulation"), tr("Hybrid"), QIcon(stochastic));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(hybridSim()));
				}
				
				//Steady State
				button = libMenu->addFunction(tr("Steady State"), tr("Get state"), QIcon(scan));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(getSS()));
				}

				menuItem = libMenu->addMenuItem(tr("Steady State"), tr("Get state"), QIcon(scan));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(getSS()));
				}
				
				//Steady State Scan
				button = libMenu->addFunction(tr("Steady State"), tr("Parameter scan"), QIcon(scan));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(scan1D()));
				}

				menuItem = libMenu->addMenuItem(tr("Steady State"), tr("Parameter scan"), QIcon(scan));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(scan1D()));
				}
				
				//Steady State Scan 2D
				button = libMenu->addFunction(tr("Steady State"), tr("2D parameter scan"), QIcon(scan));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(scan2D()));
				}

				menuItem = libMenu->addMenuItem(tr("Steady State"), tr("2D parameter scan"), QIcon(scan));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(scan2D()));
				}
				
				//Jacobian
				button = libMenu->addFunction(tr("Steady State"), tr("Jacobian"), QIcon(odepng));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(getJac()));
				}

				menuItem = libMenu->addMenuItem(tr("Steady State"), tr("Jacobian"), QIcon(odepng));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(getJac()));
				}
				
				//Eigenvalues
				button = libMenu->addFunction(tr("Steady State"), tr("Eigenvalues"), QIcon(odepng));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(getEig()));
				}

				menuItem = libMenu->addMenuItem(tr("Steady State"), tr("Eigenvalues"), QIcon(odepng));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(getEig()));
				}
				
				//MCA -- senstivities
				button = libMenu->addFunction(tr("Metabolic Control Analysis"), tr("Elasticities (scaled)"), QIcon(mcaicon));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(scaledElasticities()));
				}

				menuItem = libMenu->addMenuItem(tr("Metabolic Control Analysis"), tr("Elasticities (scaled)"), QIcon(mcaicon));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(scaledElasticities()));
				}
				
				//MCA -- concentration CC
				button = libMenu->addFunction(tr("Metabolic Control Analysis"), tr("Concentration control coefficients (scaled)"), QIcon(mcaicon));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(scaledConcentrationCC()));
				}

				menuItem = libMenu->addMenuItem(tr("Metabolic Control Analysis"), tr("Concentration control coefficients (scaled)"), QIcon(mcaicon));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(scaledConcentrationCC()));
				}

				//MCA -- flux CC
				button = libMenu->addFunction(tr("Metabolic Control Analysis"), tr("Flux control coefficients (scaled)"), QIcon(mcaicon));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(scaledFluxCC()));
				}

				menuItem = libMenu->addMenuItem(tr("Metabolic Control Analysis"), tr("Flux control coefficients (scaled)"), QIcon(mcaicon));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(scaledFluxCC()));
				}
				
				//Reduced Stoic
				button = libMenu->addFunction(tr("Network Structure"), tr("Reduced stoichiometry"), QIcon(nodedges));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(redStoic()));
				}

				menuItem = libMenu->addMenuItem(tr("Network Structure"), tr("Reduced stoichiometry"), QIcon(nodedges));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(redStoic()));
				}
				
				//elementary flux modes
				button = libMenu->addFunction(tr("Network Structure"), tr("Elementary Flux Modes"), QIcon(nodedges));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(getELM()));
				}

				menuItem = libMenu->addMenuItem(tr("Network Structure"), tr("Elementary Flux Modes"), QIcon(nodedges));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(getELM()));
				}
		}
    }
}
}
