/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool provides interface into COPASI's functions
****************************************************************************/

#ifndef TINKERCELL_COPASIEXPORTTOOL_H
#define TINKERCELL_COPASIEXPORTTOOL_H

#include <QThread>
#include <QSemaphore>
#include <QStringList>
#include "NetworkWindow.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "SimulationThread.h"

namespace Tinkercell
{
	/*! \brief This class links C programs to the SimulationThread, which uses COPASI
	/ingrou simulations
	*/
	class TINKERCELLEXPORT CopasiExporter : public Tool
	{
		Q_OBJECT

	public:

		CopasiExporter();
		~CopasiExporter();
		bool setMainWindow(MainWindow * main);
	
	private slots:

		void toolLoaded(Tool*);
		void setupFunctionPointers(QLibrary * library);
		void historyChanged(int);/*! \brief make the window transparent when mouse exits the window*/
		void windowChanged(NetworkWindow*,NetworkWindow*);
		void getHandles( const SimulationThread *, QSemaphore*, QList<ItemHandle*>*, bool * changed);
		
		void getEig();
		void getJac();
		void scan2D();
		void scan1D();
		void getSS();
		void hybridSim();
		void tauleapSim();
		void odeSim();
		void ssaSim();
		void scaledElasticities();
		void scaledConcentrationCC();
		void scaledFluxCC();
		void redStoic();
		void getELM();
		void optimize();

	private:
		QHash<const SimulationThread*, bool> updatedThreads;
		static CopasiExporter * _instance;
		
		SimulationDialog * simDialog;
		static QList<SimulationThread*> runningThreads;
		static SimulationThread * getSimulationThread();
		
		static tc_matrix simulateDeterministic(double startTime, double endTime, int numSteps);
		static tc_matrix simulateStochastic(double startTime, double endTime, int numSteps);
		static tc_matrix simulateHybrid(double startTime, double endTime, int numSteps);
		static tc_matrix simulateTauLeap(double startTime, double endTime, int numSteps);
		static tc_matrix getSteadyState();
		static tc_matrix steadyStateScan(const char * param, double start, double end, int numSteps);
		static tc_matrix steadyStateScan2D(const char * param1, double start1, double end1, int numSteps1,const char * param2, double start2, double end2, int numSteps2);
		static tc_matrix getJacobian();
		static tc_matrix getEigenvalues();
		static tc_matrix getUnscaledElasticities();
		static tc_matrix getUnscaledConcentrationCC();
		static tc_matrix getUnscaledFluxCC();
		static tc_matrix getScaledElasticities();
		static tc_matrix getScaledConcentrationCC();
		static tc_matrix getScaledFluxCC();
		static tc_matrix reducedStoichiometry();
		static tc_matrix elementaryFluxModes();
		static tc_matrix KMatrix();
		static tc_matrix LMatrix();
		static tc_matrix gaOptimize(const char*);
		static void updateParams(tc_matrix);
	};
}

//extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif

