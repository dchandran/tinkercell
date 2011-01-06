/****************************************************************************

Copyright (c) 2010 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool generates multiple models by trying all available models for each process

****************************************************************************/

#ifndef TINKERCELL_MODULECOMBINATORICS_H
#define TINKERCELL_MODULECOMBINATORICS_H

#include "DataTable.h"
#include "ItemFamily.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "NodesTree.h"
#include "ConnectionsTree.h"
#include "SimulationThread.h"

namespace Tinkercell
{
	class TINKERCELLEXPORT ModuleCombinatorics : public Tool
	{
			Q_OBJECT

		public:

			ModuleCombinatorics();
			bool setMainWindow(MainWindow*);
		
		public slots:
			
			void run();
		
		private:

			void getModelsFor(ItemHandle*, QList< QPair< QString, QList<ItemHandle*> > >&);
			void writeModels();
			void writeModels(int& index, QHash<QString, double>& stats, QList<ItemHandle*>& output, const QList<ItemHandle*>& handles, const QHash< ItemHandle*, QList< QPair< QString, QList<ItemHandle*> > > >& handleReplacements);
			void MonteCarlo(int & index, QList<ItemHandle*>& handles, QList<int> & selectedModules); 
			double computeScore(NumericalDataTable & results, NumericalDataTable & target);
			
			QList<SimulationThread*> simulationThreads;
			NumericalDataTable population, scores, modules;
	};
}

//extern "C"  TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif

