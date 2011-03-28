/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool supporting COPASI based simulations (multithreaded)
****************************************************************************/

#ifndef TINKERCELL_COPASISIMULATIONTOOL_H
#define TINKERCELL_COPASISIMULATIONTOOL_H

#include <QThread>
#include <QSemaphore>
#include <QStringList>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include "NetworkWindow.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "CThread.h"
#include "PlotTool.h"
#include "MultithreadedSliderWidget.h"
#include "copasi/copasi_api.h"

namespace Tinkercell
{
	class SimulationDialog;
	
	/*!\brief simulation class that uses COPASI as its backend. This class extends the thread class, so the simulations
			always run as a separate thread.
		\ingroup simulation*/
	class TINKERCELLEXPORT SimulationThread : public CThread
	{
		Q_OBJECT
		
	public: 
		
		/*! \brief The set of available analysis method*/
		enum AnalysisMethod
		{
			None=0,
			DeterministicSimulation,
			StochasticSimulation,
			HybridSimulation,
			TauLeapSimulation, 
			SteadyState,
			SteadyStateScan1D,
			SteadyStateScan2D,
			Jacobian,
			Eigenvalues,
			UnscaledElasticities,
			UnscaledConcentrationCC,
			UnscaledFluxCC,
			ScaledElasticities,
			ScaledConcentrationCC,
			ScaledFluxCC,
			ReducedStoichiometry,
			ElementaryFluxModes,
			KMatrix,
			LMatrix,
			K0Matrix,
			L0Matrix,
			GammaMatrix,
			GA
		};
		
		/*! \brief Constructor requires MainWindow pointer*/
		SimulationThread(MainWindow * parent=0);
		
		/*! \brief destructor removes copasi model*/
		~SimulationThread();
		
		/*! \brief Updates the COPASI model using the updateModel(QList<ItemHandle*>) function*/
		void updateModel();
		
		/*! \brief Updates the COPASI model parameters */
		void updateModelParameters(const NumericalDataTable & params);
		
		/*! \brief Updates the COPASI model using the list of handles provided
		* \param QList<ItemHandle*> all items in the model
		*/
		static void updateModel(QList<ItemHandle*>&, copasi_model & model, NumericalDataTable &);
		
		/*! \brief Updates the COPASI model using the list of handles provided
		* \param QList<ItemHandle*> all items in the model
		*/
		void updateModel(QList<ItemHandle*>&);
		
		/*! \brief Select the type of analysis this thread should run
		* \param AnalysisMethod
		*/
		void setMethod(AnalysisMethod);
		/*! \brief Provide a semaphore (optional) so that the calling thread can wait until simulation is finished
		* \param QSemaphote*
		*/
		void setSemaphore(QSemaphore*);
		/*! \brief set the time-course simulation start time
		* \param double time
		*/
		void setStartTime(double);
		/*! \brief set the time-course simulation end time
		* \param double time
		*/
		void setEndTime(double);
		/*! \brief set the number of points in the final output
		* \param int points
		*/
		void setNumPoints(int);
		/*! \brief if this option is set to true, then this thread will request the PlotTool to plot the results
		* \param bool
		*/
		void plotResult(bool b=true);
		/*! \brief add a parameter scan variable
		* \param QString parameter name
		* \param double start value
		* \param double end value
		* \param int number of points in the output
		*/
		void setParameterRange(const QString& param, double start, double end, int numPoints);
		/*! \brief set objective function or filename for fitting the data
		* \param QString objective function or filename for fitting the data
		*/
		void setObjective(const QString& );
		/*! \brief get the results matrix. Use ConvertValue to convert it to a NumericalDataTable
		* \return tc_matrix
		*/
		tc_matrix result();
	
	signals:
		/*! \brief get the item handles in the model
		* \param SimulationThread * this thread
		* \param QSemaphore * semaphore that should be released by the thread recieving this signal
		* \param QList<ItemHandle> reference to list of handles
		* \param bool receiving thread should set this to true is any changes to the model is made
		*/
		void getHandles( const SimulationThread *, QSemaphore*, QList<ItemHandle*>*, bool * changed);
		/*! \brief graph the results matrix. This signal is connected to the PlotTool's slot
		*/
		void graph(const DataTable<qreal>&,const QString& title,int xaxis, PlotTool::PlotType type);
		
	protected:

		/*! \brief perform the analysis*/
		virtual void run();
		/*! \brief the analysis method*/
		AnalysisMethod method;
		/*! \brief the copasi model*/
		copasi_model model;
		/*! \brief start time for simulations*/
		double startTime;
		/*! \brief end time for simulations*/
		double endTime;
		/*! \brief number of points in simulation output*/
		int numPoints;
		/*! \brief whether or not to plot results*/
		bool plot;
		/*! \brief results matrix (see copasi_api.h)*/
		tc_matrix resultMatrix;
		/*! \brief if a semaphore is set by calling thread*/
		QSemaphore * semaphore;
		/*! \brief for optimization*/
		QString objective;
		/*! \brief all params such that their min != max*/
		NumericalDataTable optimizationParameters;
		
		/*! \brief this class is used for each parameter over which a scan needs to be performed*/
		struct ScanItem { QString name; double start; double end; int numPoints; };
		/*! \brief the set of scan parameters*/
		QList<ScanItem> scanItems;
		/*! \brief the total number of copasi models created so far*/
		static int totalModelCount;
		
		friend class SimulationDialog;
	}; 

	/*!\brief a container dialog for running functions provided by SimulationThread
		\ingroup plugins*/
	class TINKERCELLEXPORT SimulationDialog : public QDialog
	{
		Q_OBJECT

		public:
			/*! \brief constructor*/
			SimulationDialog(MainWindow * parent);
			/*! \brief destructor*/
			~SimulationDialog();
		public slots:
			/*! \brief set the simulation thread*/
			void setThread(SimulationThread *);
			/*! \brief set the analysis method*/
			void setMethod(SimulationThread::AnalysisMethod);
			/*! \brief run the analysis*/
			void run();
		protected:
			/*! \brief whenever the mouse enters the dialog, it updates the list of parameters inside the combo boxes*/
			void enterEvent ( QEvent * event );
			/*! \brief the thread*/
			SimulationThread * thread;
			QLabel * methodLabel;
			QGroupBox * simBox, * param1Box, * param2Box;
			QDoubleSpinBox * simStart, * simEnd, * param1Start, * param1End, * param2Start, * param2End;
			QSpinBox * numPoints1, * numPoints2, * numPoints3;
			QComboBox * menu1, * menu2, *menu3;
			/*! \brief values for the sliders*/
			NumericalDataTable sliderValues;
			MultithreadedSliderWidget * sliderWidget;
	};
}

#endif

