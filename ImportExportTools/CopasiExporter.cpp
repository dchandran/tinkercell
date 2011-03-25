#include <string>
#include <QInputDialog>
#include <QFileDialog>
#include "PlotTool.h"
#include "BasicInformationTool.h"
#include "StoichiometryTool.h"
#include "CopasiExporter.h"
#include "ConsoleWindow.h"
#include "DynamicLibraryMenu.h"

using namespace Tinkercell;

CopasiExporter::CopasiExporter() : Tool("COPASI","Export")
{
	modelNeedsUpdate = true;	
	//qRegisterMetaType< copasi_model >("copasi_model");
	//qRegisterMetaType< copasi_model* >("copasi_model*");
	//copasi_init();
}

void CopasiExporter::getHandles(QSemaphore * sem, QList<ItemHandle*>* handles, bool * b)
{
	if (currentNetwork() && handles && modelNeedsUpdate)
		(*handles) = currentNetwork()->handles(true);
	if (b)
		(*b) = modelNeedsUpdate;
	if (sem)
		sem->release();
}

CopasiExporter::~CopasiExporter()
{
	if (odeThread) 	delete odeThread;
	if (stochThread) delete stochThread;
	if (ssThread) delete ssThread;
	if (jacThread) delete jacThread;
	if (mcaThread) delete mcaThread;

	copasi_end();
}

bool CopasiExporter::setMainWindow(MainWindow * main)
{
	Tool::setMainWindow(main);	
	if (!mainWindow) return false;
	connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
	connect(main,SIGNAL(historyChanged(int)),this, SLOT(historyChanged(int)));
	connect(main,SIGNAL(windowChanged(NetworkWindow*,NetworkWindow*)),this, SLOT(windowChanged(NetworkWindow*,NetworkWindow*)));
	
	odeThread = new SimulationThread(mainWindow);
	simDialog = new SimulationDialog(mainWindow);

	connect(odeThread,SIGNAL(getHandles(QSemaphore*, QList<ItemHandle*>*, bool *)),
					this, SLOT(getHandles(QSemaphore*, QList<ItemHandle*>*, bool *)));
	
	stochThread = new SimulationThread(mainWindow);
	connect(stochThread,SIGNAL(getHandles(QSemaphore*, QList<ItemHandle*>*, bool *)),
					this, SLOT(getHandles(QSemaphore*, QList<ItemHandle*>*, bool *)));

	ssThread = new SimulationThread(mainWindow);
	connect(ssThread,SIGNAL(getHandles(QSemaphore*, QList<ItemHandle*>*, bool *)),
					this, SLOT(getHandles(QSemaphore*, QList<ItemHandle*>*, bool *)));

	jacThread = new SimulationThread(mainWindow);
	connect(jacThread,SIGNAL(getHandles(QSemaphore*, QList<ItemHandle*>*, bool *)),
					this, SLOT(getHandles(QSemaphore*, QList<ItemHandle*>*, bool *)));

	mcaThread = new SimulationThread(mainWindow);
	connect(mcaThread,SIGNAL(getHandles(QSemaphore*, QList<ItemHandle*>*, bool *)),
					this, SLOT(getHandles(QSemaphore*, QList<ItemHandle*>*, bool *)));
					
	optimThread = new SimulationThread(mainWindow);
	connect(optimThread,SIGNAL(getHandles(QSemaphore*, QList<ItemHandle*>*, bool *)),
					this, SLOT(getHandles(QSemaphore*, QList<ItemHandle*>*, bool *)));

	connect(mainWindow,SIGNAL(toolLoaded(Tool *)),
					this, SLOT(toolLoaded(Tool*)));

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
	tc_matrix (*Optimize)(const char * )
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
			&gaOptimize
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

SimulationThread * CopasiExporter::odeThread = 0;
SimulationThread * CopasiExporter::stochThread = 0;
SimulationThread * CopasiExporter::ssThread = 0;
SimulationThread * CopasiExporter::jacThread = 0;
SimulationThread * CopasiExporter::mcaThread = 0;
SimulationThread * CopasiExporter::optimgThread = 0;

void CopasiExporter::odeSim()
{
	simDialog->setThread(odeThread);
	simDialog->setMethod(SimulationThread::DeterministicSimulation);
}

void CopasiExporter::getEig()
{
	simDialog->setThread(jacThread);
	simDialog->setMethod(SimulationThread::Eigenvalues);
}

void CopasiExporter::getJac()
{
	simDialog->setThread(jacThread);
	simDialog->setMethod(SimulationThread::Jacobian);
}

void CopasiExporter::scan2D()
{
	simDialog->setThread(ssThread);
	simDialog->setMethod(SimulationThread::SteadyStateScan2D);
}

void CopasiExporter::scan1D()
{
	simDialog->setThread(ssThread);
	simDialog->setMethod(SimulationThread::SteadyStateScan1D);
}

void CopasiExporter::getSS()
{
	simDialog->setThread(ssThread);
	simDialog->setMethod(SimulationThread::SteadyState);
}

void CopasiExporter::hybridSim()
{
	simDialog->setThread(odeThread);
	simDialog->setMethod(SimulationThread::HybridSimulation);
}

void CopasiExporter::tauleapSim()
{
	simDialog->setThread(stochThread);
	simDialog->setMethod(SimulationThread::TauLeapSimulation);
}

void CopasiExporter::ssaSim()
{
	simDialog->setThread(stochThread);
	simDialog->setMethod(SimulationThread::StochasticSimulation);
}

void CopasiExporter::scaledElasticities()
{
	simDialog->setThread(mcaThread);
	simDialog->setMethod(SimulationThread::ScaledElasticities);
}

void CopasiExporter::scaledConcentrationCC()
{
	simDialog->setThread(mcaThread);
	simDialog->setMethod(SimulationThread::ScaledConcentrationCC);
}

void CopasiExporter::scaledFluxCC()
{
	simDialog->setThread(mcaThread);
	simDialog->setMethod(SimulationThread::ScaledFluxCC);
}

void CopasiExporter::redStoic()
{
	tc_matrix m = reducedStoichiometry();
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
	tc_matrix m = elementaryFluxModes();
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

	
	if (optimThread)
	{
		if (optimThread->isRunning())
			optimThread->terminate();
		//optimThread->plot = true;
		optimThread->updateModel();
		optimThread->setObjective(input);
		optimThread->setMethod(SimulationThread::GA);
		optimThread->setSemaphore(0);
		optimThread->start();
		NumericalDataTable * N = ConvertValue(optimThread->result());
		delete N;
	}
}

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
		return (odeThread->result());
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
		return (stochThread->result());
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
		return (odeThread->result());
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
		return (stochThread->result());
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
		return (ssThread->result());
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
		ssThread->setMethod(SimulationThread::SteadyStateScan1D);
		ssThread->setParameterRange(QString(param), start, end, numSteps);
		QSemaphore sem(1);
		sem.acquire();
		ssThread->setSemaphore(&sem);
		ssThread->start();
		sem.acquire();
		sem.release();
		return (ssThread->result());
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
		ssThread->setMethod(SimulationThread::SteadyStateScan2D);
		ssThread->setParameterRange(QString(param1), start1, end1, numSteps1);
		ssThread->setParameterRange(QString(param2), start2, end2, numSteps2);
		QSemaphore sem(1);
		sem.acquire();
		ssThread->setSemaphore(&sem);
		ssThread->start();
		sem.acquire();
		sem.release();
		return (ssThread->result());
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
		return (ssThread->result());
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
		return (ssThread->result());
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
		return (mcaThread->result());
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
		return (mcaThread->result());
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
		return (mcaThread->result());
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
		return (mcaThread->result());
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
		return (mcaThread->result());
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
		return (mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::reducedStoichiometry()
{
	if (mcaThread)
	{
		if (mcaThread->isRunning())
			mcaThread->terminate();
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::ReducedStoichiometry);
		QSemaphore sem(1);
		sem.acquire();
		mcaThread->setSemaphore(&sem);
		mcaThread->start();
		sem.acquire();
		sem.release();
		return (mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::elementaryFluxModes()
{
	if (mcaThread)
	{
		if (mcaThread->isRunning())
			mcaThread->terminate();
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::ElementaryFluxModes);
		QSemaphore sem(1);
		sem.acquire();
		mcaThread->setSemaphore(&sem);
		mcaThread->start();
		sem.acquire();
		sem.release();
		return (mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::KMatrix()
{
	if (mcaThread)
	{
		if (mcaThread->isRunning())
			mcaThread->terminate();
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::KMatrix);
		QSemaphore sem(1);
		sem.acquire();
		mcaThread->setSemaphore(&sem);
		mcaThread->start();
		sem.acquire();
		sem.release();
		return (mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::LMatrix()
{
	if (mcaThread)
	{
		if (mcaThread->isRunning())
			mcaThread->terminate();
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::LMatrix);
		QSemaphore sem(1);
		sem.acquire();
		mcaThread->setSemaphore(&sem);
		mcaThread->start();
		sem.acquire();
		sem.release();
		return (mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::gaOptimize(const char * s)
{
	if (optimThread)
	{
		if (optimThread->isRunning())
			optimThread->terminate();
//		optimThread->plot = false;
		optimThread->updateModel();
		optimThread->setObjective(QString(s));
		optimThread->setMethod(SimulationThread::GA);
		QSemaphore sem(1);
		sem.acquire();
		optimThread->setSemaphore(&sem);
		optimThread->start();
		sem.acquire();
		sem.release();
		return (optimThread->result());
	}
	return tc_createMatrix(0,0);
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

				menuItem = libMenu->addMenuItem(tr("Simulation"), tr("Deterministic"), QIcon(odepng));
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
				button = libMenu->addFunction(tr("Steady state"), tr("Get state"), QIcon(scan));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(getSS()));
				}

				menuItem = libMenu->addMenuItem(tr("Steady state"), tr("Get state"), QIcon(scan));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(getSS()));
				}
				
				//Steady State Scan
				button = libMenu->addFunction(tr("Steady state"), tr("Parameter scan"), QIcon(scan));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(scan1D()));
				}

				menuItem = libMenu->addMenuItem(tr("Steady state"), tr("Parameter scan"), QIcon(scan));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(scan1D()));
				}
				
				//Steady State Scan 2D
				button = libMenu->addFunction(tr("Steady state"), tr("2D parameter scan"), QIcon(scan));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(scan2D()));
				}

				menuItem = libMenu->addMenuItem(tr("Steady state"), tr("2D parameter scan"), QIcon(scan));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(scan2D()));
				}
				
				//Jacobian
				button = libMenu->addFunction(tr("Steady state"), tr("Jacobian"), QIcon(odepng));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(getJac()));
				}

				menuItem = libMenu->addMenuItem(tr("Steady state"), tr("Jacobian"), QIcon(odepng));
				if (menuItem)
				{
					menuItem->setToolTip(tr("using COPASI"));
					connect(menuItem,SIGNAL(triggered()),this,SLOT(getJac()));
				}
				
				//Eigenvalues
				button = libMenu->addFunction(tr("Steady state"), tr("Eigenvalues"), QIcon(odepng));
				if (button)
				{
					button->setToolTip(tr("using COPASI"));
					connect(button,SIGNAL(pressed()),this,SLOT(getEig()));
				}

				menuItem = libMenu->addMenuItem(tr("Steady state"), tr("Eigenvalues"), QIcon(odepng));
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

