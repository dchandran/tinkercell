/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool inports and exports SBML models

****************************************************************************/

#ifndef TINKERCELL_SBMLIMPORTEXPORTTOOL_H
#define TINKERCELL_SBMLIMPORTEXPORTTOOL_H

#include <QThread>
#include <QSemaphore>
#include <QStringList>
#include "NetworkWindow.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "common/sbmlfwd.h"

namespace Tinkercell
{
	class TINKERCELLEXPORT SBMLImportExport_FtoS : public QObject
	{
			Q_OBJECT

		signals:
			void exportSBML(QSemaphore*, const QString&);
			void importSBML(QSemaphore*, const QString&);
			void simulateODE(QSemaphore*, NumericalDataTable*, double, double);
			void simulateGillespie(QSemaphore*, NumericalDataTable*, double);
			void steadyStateScan(QSemaphore*, NumericalDataTable* , const QString&, double , double );
			
		public:
			void exportSBMLFile(const char *);
			void importSBMLString(const char*);
			tc_matrix ODEsim(double, double);
			tc_matrix GillespieSim(double);
			tc_matrix ScanSS(const char* , double , double );
	};
	
	/*! \brief This class imports and exports SBML file format
	and can performs ODE and stochastic simulations. The ODE simulation
	uses SOSlib and the stochastic uses a custom implementation of
	Gillespie algorithm. The SBML document is updated whenever any data
	in the model is changed or when items are inserted or removed.
	/ingrou plugins
	*/
	class TINKERCELLEXPORT SBMLImportExport : public Tool
	{
		Q_OBJECT

	public:

		SBMLImportExport();
		~SBMLImportExport();
		bool setMainWindow(MainWindow * main);
		
		QList<ItemHandle*> importSBML(const QString&);
		SBMLDocument_t* exportSBML(QList<ItemHandle*>&);
		SBMLDocument_t* exportSBML(NetworkHandle * network = 0);
		
		NumericalDataTable integrateODEs(double time, double stepsize);
		NumericalDataTable Gillespie(double time);

	private slots:

		void windowChanged(NetworkWindow*,NetworkWindow*);
		void historyChanged(int);
		void setupFunctionPointers( QLibrary * );
		void loadSBMLFile();
		void saveSBMLFile();
		void exportSBML(QSemaphore*, const QString&);
		void importSBML(QSemaphore*, const QString&);
		void simulateODE(QSemaphore*, NumericalDataTable*, double, double);
		void simulateGillespie(QSemaphore*, NumericalDataTable*, double);
		void steadyStateScan(QSemaphore*, NumericalDataTable* , const QString&, double , double );

	private:

		bool modelNeedsUpdate;
		void updateSBMLModel();

		SBMLDocument_t * sbmlDocument;
		
		static SBMLImportExport_FtoS fToS;
		static void exportSBMLFile(const char *);
		static void importSBMLString(const char*);
		static tc_matrix ODEsim(double, double);
		static tc_matrix GillespieSim(double);
		static tc_matrix ScanSS(const char* , double , double );
	};

	class SimulationThread : public QThread
	{
		public:

			enum SimulationType { ODE, Gillespie, Scan };
			SimulationThread(QSemaphore * sem, NumericalDataTable *, SBMLDocument_t *, SimulationType , QObject *);
			void setScanVariable(const QString&, double from, double to);
			void setTime(double);
			void setStepSize(double);

		protected:
			void run();

			QSemaphore * semaphore;
			NumericalDataTable * dataTable;
			SBMLDocument_t * sbmlDocument;
			SimulationType simType;
			QString scanParam;
			double from, to;
			double time;
			double stepSize;
	};
}

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif

