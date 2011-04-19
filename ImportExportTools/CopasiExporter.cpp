#include <string>
#include <QInputDialog>
#include <QFileDialog>
#include "PlotTool.h"
#include "BasicInformationTool.h"
#include "StoichiometryTool.h"
#include "CopasiExporter.h"
#include "ConsoleWindow.h"
#include "DynamicLibraryMenu.h"
#include "LabelingTool.h"

using namespace Tinkercell;

CopasiExporter * CopasiExporter::_instance = 0;

CopasiExporter::CopasiExporter() : Tool("COPASI","Export")
{
	CopasiExporter::_instance = this;
	//qRegisterMetaType< copasi_model >("copasi_model");
	//qRegisterMetaType< copasi_model* >("copasi_model*");
	//copasi_init();
}

void CopasiExporter::getHandles(const SimulationThread * thread, QSemaphore * sem, QList<ItemHandle*>* handles, bool * b)
{
	bool needUpdate = true;
	if (updatedThreads.contains(thread))
		needUpdate = updatedThreads[thread];
	else
		updatedThreads[thread] = true;
	if (currentNetwork() && handles && needUpdate)
		(*handles) = currentNetwork()->handles(true);
	if (b)
		(*b) = needUpdate;
	updatedThreads[thread] = false;
	if (sem)
		sem->release();
}

CopasiExporter::~CopasiExporter()
{
	for (int i=0; i < runningThreads.size(); ++i)
		if (runningThreads[i])
		{
			if (runningThreads[i]->isRunning())
				runningThreads[i]->terminate();
			delete runningThreads[i];
		}

	copasi_end();
}

QList<SimulationThread*> CopasiExporter::runningThreads;

SimulationThread * CopasiExporter::getSimulationThread()
{
	for (int i=0; i < runningThreads.size(); ++i)
		if (runningThreads[i] && !runningThreads[i]->isRunning())
			return runningThreads[i];
	
	SimulationThread * thread = new SimulationThread(MainWindow::instance());
	connect(thread,
			SIGNAL(getHandles(const SimulationThread *, QSemaphore*, QList<ItemHandle*>*, bool *)),
			CopasiExporter::_instance, 
			SLOT(getHandles(const SimulationThread *, QSemaphore*, QList<ItemHandle*>*, bool *)));

	runningThreads += thread;
	return thread;
}

bool CopasiExporter::setMainWindow(MainWindow * main)
{
	Tool::setMainWindow(main);	
	if (!mainWindow) return false;
	
	simDialog = new SimulationDialog(mainWindow);
	
	connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
	connect(main,SIGNAL(historyChanged(int)),this, SLOT(historyChanged(int)));
	connect(main,SIGNAL(windowChanged(NetworkWindow*,NetworkWindow*)),this, SLOT(windowChanged(NetworkWindow*,NetworkWindow*)));
	connect(mainWindow,SIGNAL(toolLoaded(Tool *)),this, SLOT(toolLoaded(Tool*)));
	
	Tool * tool = mainWindow->tool(tr("Labeling Tool"));
	if (tool)
	{
		connect(this, SIGNAL(displayFire(ItemHandle*, double)), tool, SLOT(displayFire(ItemHandle*, double)));
	}

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
	void (*update)(tc_matrix)
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
			&updateParams
		);
}

void CopasiExporter::historyChanged(int)
{
	QList<const SimulationThread*> threads = updatedThreads.keys();
	for (int i=0; i < threads.size(); ++i)
		updatedThreads[ threads[i] ] = true;
}

void CopasiExporter::windowChanged(NetworkWindow*,NetworkWindow*)
{
	QList<const SimulationThread*> threads = updatedThreads.keys();
	for (int i=0; i < threads.size(); ++i)
		updatedThreads[ threads[i] ] = true;
}

/***************************************************
      Copasi export
*****************************************************/

void CopasiExporter::odeSim()
{
	simDialog->setThread(getSimulationThread());
	simDialog->setMethod(SimulationThread::DeterministicSimulation);
}

void CopasiExporter::getEig()
{
	simDialog->setThread(getSimulationThread());
	simDialog->setMethod(SimulationThread::Eigenvalues);
}

void CopasiExporter::getJac()
{
	simDialog->setThread(getSimulationThread());
	simDialog->setMethod(SimulationThread::Jacobian);
}

void CopasiExporter::scan2D()
{
	simDialog->setThread(getSimulationThread());
	simDialog->setMethod(SimulationThread::SteadyStateScan2D);
}

void CopasiExporter::scan1D()
{
	simDialog->setThread(getSimulationThread());
	simDialog->setMethod(SimulationThread::SteadyStateScan1D);
}

void CopasiExporter::getSS()
{
	SimulationThread * thread = getSimulationThread();
	simDialog->setThread(thread);
	simDialog->setMethod(SimulationThread::SteadyState);
	
	if (currentNetwork())
	{
		NumericalDataTable * dat = ConvertValue(thread->result());
		QList<ItemHandle*> items = currentNetwork()->findItem(dat->rowNames());
		
		double max = 0.0;
		for (int i=0; i < dat->rows(); ++i)
			if (dat->at(i,0) > max)
				max = dat->at(i,0);
		
		for (int i=0; i < items.size(); ++i)
			emit displayFire( items[i], dat->at(i,0)/max);
	
		delete dat;
	}
}

void CopasiExporter::hybridSim()
{
	simDialog->setThread(getSimulationThread());
	simDialog->setMethod(SimulationThread::HybridSimulation);
}

void CopasiExporter::tauleapSim()
{
	simDialog->setThread(getSimulationThread());
	simDialog->setMethod(SimulationThread::TauLeapSimulation);
}

void CopasiExporter::ssaSim()
{
	simDialog->setThread(getSimulationThread());
	simDialog->setMethod(SimulationThread::StochasticSimulation);
}

void CopasiExporter::scaledElasticities()
{
	simDialog->setThread(getSimulationThread());
	simDialog->setMethod(SimulationThread::ScaledElasticities);
}

void CopasiExporter::scaledConcentrationCC()
{
	simDialog->setThread(getSimulationThread());
	simDialog->setMethod(SimulationThread::ScaledConcentrationCC);
}

void CopasiExporter::scaledFluxCC()
{
	simDialog->setThread(getSimulationThread());
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

	SimulationThread * optimThread = getSimulationThread();
	if (optimThread && !optimThread->isRunning())
	{
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
	SimulationThread * odeThread = getSimulationThread();
	if (odeThread && !odeThread->isRunning())
	{
		odeThread->updateModel();
		odeThread->setMethod(SimulationThread::DeterministicSimulation);
		odeThread->setStartTime(startTime);
		odeThread->setEndTime(endTime);
		odeThread->setNumPoints(numSteps);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		odeThread->setSemaphore(sem);
		odeThread->start();
		sem->acquire();
		sem->release();
		odeThread->setSemaphore(0);		
		delete sem;
		return (odeThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::simulateStochastic(double startTime, double endTime, int numSteps)
{
	SimulationThread * stochThread = getSimulationThread();
	if (stochThread && !stochThread->isRunning())
	{
		stochThread->updateModel();
		stochThread->setMethod(SimulationThread::StochasticSimulation);
		stochThread->setStartTime(startTime);
		stochThread->setEndTime(endTime);
		stochThread->setNumPoints(numSteps);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		stochThread->setSemaphore(sem);
		stochThread->start();
		sem->acquire();
		sem->release();
		stochThread->setSemaphore(0);		
		delete sem;
		return (stochThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::simulateHybrid(double startTime, double endTime, int numSteps)
{
	SimulationThread * odeThread = getSimulationThread();
	if (odeThread && !odeThread->isRunning())
	{
		odeThread->updateModel();
		odeThread->setMethod(SimulationThread::HybridSimulation);
		odeThread->setStartTime(startTime);
		odeThread->setEndTime(endTime);
		odeThread->setNumPoints(numSteps);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		odeThread->setSemaphore(sem);
		odeThread->start();
		sem->acquire();
		sem->release();
		odeThread->setSemaphore(0);		
		delete sem;
		return (odeThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::simulateTauLeap(double startTime, double endTime, int numSteps)
{
	SimulationThread * stochThread = getSimulationThread();
	if (stochThread && !stochThread->isRunning())
	{
		stochThread->updateModel();
		stochThread->setMethod(SimulationThread::TauLeapSimulation);
		stochThread->setStartTime(startTime);
		stochThread->setEndTime(endTime);
		stochThread->setNumPoints(numSteps);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		stochThread->setSemaphore(sem);
		stochThread->start();
		sem->acquire();
		sem->release();
		stochThread->setSemaphore(0);		
		delete sem;
		return (stochThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getSteadyState()
{
	SimulationThread * ssThread = getSimulationThread();
	if (ssThread && !ssThread->isRunning())
	{
		ssThread->updateModel();
		ssThread->setMethod(SimulationThread::SteadyState);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		ssThread->setSemaphore(sem);
		ssThread->start();
		sem->acquire();
		sem->release();
		ssThread->setSemaphore(0);
		delete sem;
		return (ssThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::steadyStateScan(const char * param, double start, double end, int numSteps)
{
	SimulationThread * ssThread = getSimulationThread();
	if (ssThread && !ssThread->isRunning())
	{
		ssThread->updateModel();
		ssThread->setMethod(SimulationThread::SteadyStateScan1D);
		ssThread->setParameterRange(QString(param), start, end, numSteps);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		ssThread->setSemaphore(sem);
		ssThread->start();
		sem->acquire();
		sem->release();
		ssThread->setSemaphore(0);		
		delete sem;
		return (ssThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::steadyStateScan2D(const char * param1, double start1, double end1, int numSteps1,const char * param2, double start2, double end2, int numSteps2)
{
	SimulationThread * ssThread = getSimulationThread();
	if (ssThread && !ssThread->isRunning())
	{
		ssThread->updateModel();
		ssThread->setMethod(SimulationThread::SteadyStateScan2D);
		ssThread->setParameterRange(QString(param1), start1, end1, numSteps1);
		ssThread->setParameterRange(QString(param2), start2, end2, numSteps2);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		ssThread->setSemaphore(sem);
		ssThread->start();
		sem->acquire();
		sem->release();
		ssThread->setSemaphore(0);		
		delete sem;
		return (ssThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getJacobian()
{
	SimulationThread * ssThread = getSimulationThread();
	if (ssThread && !ssThread->isRunning())
	{
		ssThread->updateModel();
		ssThread->setMethod(SimulationThread::Jacobian);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		ssThread->setSemaphore(sem);
		ssThread->start();
		sem->acquire();
		sem->release();
		ssThread->setSemaphore(0);		
		delete sem;
		return (ssThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getEigenvalues()
{
	SimulationThread * ssThread = getSimulationThread();
	if (ssThread && !ssThread->isRunning())
	{
		ssThread->updateModel();
		ssThread->setMethod(SimulationThread::Eigenvalues);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		ssThread->setSemaphore(sem);
		ssThread->start();
		sem->acquire();
		sem->release();
		ssThread->setSemaphore(0);		
		delete sem;
		return (ssThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getUnscaledElasticities()
{
	SimulationThread * mcaThread = getSimulationThread();
	if (mcaThread && !mcaThread->isRunning())
	{
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::UnscaledElasticities);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		mcaThread->setSemaphore(sem);
		mcaThread->start();
		sem->acquire();
		sem->release();
		mcaThread->setSemaphore(0);		
		delete sem;
		return (mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getUnscaledConcentrationCC()
{
	SimulationThread * mcaThread = getSimulationThread();
	if (mcaThread && !mcaThread->isRunning())
	{
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::UnscaledConcentrationCC);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		mcaThread->setSemaphore(sem);
		mcaThread->start();
		sem->acquire();
		sem->release();
		mcaThread->setSemaphore(0);		
		delete sem;
		return (mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getUnscaledFluxCC()
{
	SimulationThread * mcaThread = getSimulationThread();
	if (mcaThread && !mcaThread->isRunning())
	{
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::UnscaledFluxCC);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		mcaThread->setSemaphore(sem);
		mcaThread->start();
		sem->acquire();
		sem->release();
		mcaThread->setSemaphore(0);		
		delete sem;
		return (mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getScaledElasticities()
{
	SimulationThread * mcaThread = getSimulationThread();
	if (mcaThread && !mcaThread->isRunning())
	{
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::ScaledElasticities);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		mcaThread->setSemaphore(sem);
		mcaThread->start();
		sem->acquire();
		sem->release();
		mcaThread->setSemaphore(0);		
		delete sem;
		return (mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getScaledConcentrationCC()
{
	SimulationThread * mcaThread = getSimulationThread();
	if (mcaThread && !mcaThread->isRunning())
	{
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::ScaledConcentrationCC);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		mcaThread->setSemaphore(sem);
		mcaThread->start();
		sem->acquire();
		sem->release();
		mcaThread->setSemaphore(0);		
		delete sem;
		return (mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::getScaledFluxCC()
{
	SimulationThread * mcaThread = getSimulationThread();
	if (mcaThread && !mcaThread->isRunning())
	{
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::ScaledFluxCC);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		mcaThread->setSemaphore(sem);
		mcaThread->start();
		sem->acquire();
		sem->release();
		mcaThread->setSemaphore(0);		
		delete sem;
		return (mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::reducedStoichiometry()
{
	SimulationThread * mcaThread = getSimulationThread();
	if (mcaThread && !mcaThread->isRunning())
	{
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::ReducedStoichiometry);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		mcaThread->setSemaphore(sem);
		mcaThread->start();
		sem->acquire();
		sem->release();
		mcaThread->setSemaphore(0);		
		delete sem;
		return (mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::elementaryFluxModes()
{
	SimulationThread * mcaThread = getSimulationThread();
	if (mcaThread && !mcaThread->isRunning())
	{
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::ElementaryFluxModes);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		mcaThread->setSemaphore(sem);
		mcaThread->start();
		sem->acquire();
		sem->release();
		mcaThread->setSemaphore(0);		
		delete sem;
		return (mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::KMatrix()
{
	SimulationThread * mcaThread = getSimulationThread();
	if (mcaThread && !mcaThread->isRunning())
	{
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::KMatrix);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		mcaThread->setSemaphore(sem);
		mcaThread->start();
		sem->acquire();
		sem->release();
		mcaThread->setSemaphore(0);		
		delete sem;
		return (mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::LMatrix()
{
	SimulationThread * mcaThread = getSimulationThread();
	if (mcaThread && !mcaThread->isRunning())
	{
		mcaThread->updateModel();
		mcaThread->setMethod(SimulationThread::LMatrix);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		mcaThread->setSemaphore(sem);
		mcaThread->start();
		sem->acquire();
		sem->release();
		mcaThread->setSemaphore(0);		
		delete sem;
		return (mcaThread->result());
	}
	return tc_createMatrix(0,0);
}

tc_matrix CopasiExporter::gaOptimize(const char * s)
{
	SimulationThread * optimThread = getSimulationThread();
	if (optimThread && !optimThread->isRunning())
	{
//		optimThread->plot = false;
		optimThread->updateModel();
		optimThread->setObjective(QString(s));
		optimThread->setMethod(SimulationThread::GA);
		QSemaphore * sem = new QSemaphore(1);
		sem->acquire();
		optimThread->setSemaphore(sem);
		optimThread->start();
		sem->acquire();
		sem->release();
		optimThread->setSemaphore(0);		
		delete sem;
		return (optimThread->result());
	}
	return tc_createMatrix(0,0);
}

void CopasiExporter::updateParams(tc_matrix params)
{
	NumericalDataTable * dat = ConvertValue(params);
	
	for (int i=0; i < runningThreads.size(); ++i)
		if (runningThreads[i])
			runningThreads[i]->setArg(*dat);

	delete dat;
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

