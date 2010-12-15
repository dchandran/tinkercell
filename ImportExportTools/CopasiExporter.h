/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool exports COPASI models
****************************************************************************/

#ifndef TINKERCELL_COPASIEXPORTTOOL_H
#define TINKERCELL_COPASIEXPORTTOOL_H

#include <QThread>
#include <QSemaphore>
#include <QStringList>
#include "NetworkWindow.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "CThread.h"
#include "copasi/copasi_api.h"

namespace Tinkercell
{
	class TINKERCELLEXPORT SimulationThread : public QThread
	{
		Q_OBJECT
		
	public: 
		
		enum AnalysisMethod
		{
			None=0,
			DeterministicSimulation,
			StochasticSimulation,
			HybridSimulation,
			TauLeapSimulation, 
			SteadyState,
			Jacobian,
			Eigenvalues,
			UnscaledElasticities,
			UnscaledConcentrationCC,
			UnscaledFluxCC,
			ScaledElasticities,
			ScaledConcentrationCC,
			ScaledFluxCC
		};
		
		SimulationThread(QObject * parent=0);
		void updateModel();
		void setMethod(AnalysisMethod);
		void setSemaphore(QSemaphore*);
		void setStartTime(double);
		void setEndTime(double);
		void setNumPoints(int);
		void setParameterRange(const QString& param, double start, double end, int numPoints);
		tc_matrix result();
	
	signals:
		void getHandles( QSemaphore*, QList<ItemHandle*>&, bool * changed);
		
	protected:

		virtual void run();
		void updateModel(QList<ItemHandle*>&);
		AnalysisMethod method;
		copasi_model model;
		double startTime;
		double endTime;
		int numPoints;		
		tc_matrix resultMatrix;
		QSemaphore * semaphore;
		
		struct ScanItem { QString name; double start; double end; int numPoints; };
		QList<ScanItem> scanItems;
		
		static int totalModelCount;
	}; 
	
	/*! \brief This class links C programs to the SimulationThread, which uses COPASI
	/ingrou plugins
	*/
	class TINKERCELLEXPORT CopasiExporter : public Tool
	{
		Q_OBJECT

	public:

		CopasiExporter();
		~CopasiExporter();
		bool setMainWindow(MainWindow * main);
	
	private slots:

		void setupFunctionPointers(QLibrary * library);
		void historyChanged(int);
		void windowChanged(NetworkWindow*,NetworkWindow*);
		
		void getHandles( QSemaphore*, QList<ItemHandle*>&, bool * changed);

	private:
		bool modelNeedsUpdate;
		
		static SimulationThread * odeThread;
		static SimulationThread * stochThread;
		static SimulationThread * ssThread;
		static SimulationThread * jacThread;
		static SimulationThread * mcaThread;
		
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
	};
}

//extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif

