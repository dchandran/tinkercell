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
#include "MainWindow.h"
#include "NetworkWindow.h"
#include "ItemHandle.h"
#include "PlotTool.h"
#include "Tool.h"
#include "SimulationThread.h"

#ifndef TINKERCELLEXPORT
#ifdef Q_WS_WIN
#   if defined(TC_EXPORTS) || defined(TinkerCell_EXPORTS)
#       define TINKERCELLEXPORT __declspec(dllexport)
#   else
#       define TINKERCELLEXPORT __declspec(dllimport)
#   endif
#else
#    define TINKERCELLEXPORT
#endif
#endif

namespace Tinkercell
{	
	class CopasiExporter_FtoS : public QObject
	{
		Q_OBJECT
		signals:
			void simulate(QSemaphore *, double startTime, double endTime, int numSteps, int type, tc_matrix*);
			void steadyStateScan(QSemaphore *, const char * param, double start, double end, int numSteps, tc_matrix*);
			void steadyStateScan2D(QSemaphore *, const char * param1, double start1, double end1, int numSteps1,const char * param2, double start2, double end2, int numSteps2, tc_matrix*);
			void otherAnalysis(QSemaphore *, int type, tc_matrix *);
			void gaOptimize(QSemaphore *, const char*, tc_matrix*);
			void updateParams(QSemaphore *, tc_matrix);
			void updateParam(QSemaphore *, const char *, double);
			void enableAssignmentRulesReordering(QSemaphore *, int);
		public:
			tc_matrix simulate(double startTime, double endTime, int numSteps, int type);
			tc_matrix steadyStateScan(const char * param, double start, double end, int numSteps);
			tc_matrix steadyStateScan2D(const char * param1, double start1, double end1, int numSteps1,const char * param2, double start2, double end2, int numSteps2);
			tc_matrix otherAnalysis(int type);
			tc_matrix gaOptimize(const char*);
			void updateParams(tc_matrix);
			void updateParam(const char *, double);
			void enableAssignmentRulesReordering(int);
	};

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

	private:
		static  CopasiExporter_FtoS fToS;

	private slots:

		void toolLoaded(Tool*);
		void setupFunctionPointers(QLibrary * library);
		void historyChanged(int);
		void windowChanged(NetworkWindow*,NetworkWindow*);
		
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

		void simulate(QSemaphore *, double startTime, double endTime, int numSteps, int type, tc_matrix*);
		void steadyStateScan(QSemaphore *, const char * param, double start, double end, int numSteps, tc_matrix*);
		void steadyStateScan2D(QSemaphore *, const char * param1, double start1, double end1, int numSteps1,const char * param2, double start2, double end2, int numSteps2, tc_matrix*);
		void otherAnalysis(QSemaphore *, int type, tc_matrix *);
		void gaOptimize(QSemaphore *, const char*, tc_matrix*);
		void updateParams(QSemaphore *, tc_matrix);
		void updateParam(QSemaphore *, const char *, double);
		void enableAssignmentRulesReordering(QSemaphore *, int);

	public slots:
		void updateModel();
		void exportSBML(const QString& filename);

	private:

		SimulationThread * simThread;		
		SimulationDialog * simDialog;
		bool needsUpdate;
		bool reorderingEnabled;

	public:
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
		static void updateParam(const char *, double);
		static void enableAssignmentRulesReordering(int);
	};
}

//extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif

