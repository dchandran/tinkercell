#include <iostream>
#include <QFile>
#include <QDir>
#include "UndoCommands.h"
#include "ModelReader.h"
#include "ModuleCombinatorics.h"
#include "ModelFileGenerator.h"
#include "DynamicLibraryMenu.h"
#include "GnuplotTool.h"
#include "ConsoleWindow.h"

namespace Tinkercell
{
	void ModuleCombinatorics::getModelsFor(ItemHandle * root, QList< QPair< QString, QList<ItemHandle*> > >& listOfLists)
	{
		if (!mainWindow || !root) return;
		
		QList<ItemFamily*> families, children;
		families << root->family();
		
		for (int n=0; n < families.size(); ++n)if (stochThread)
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
		{
			children = families[n]->children();
			for (int j=0; j < children.size(); ++j)
				if (!families.contains(children[j]))
					families << children[j];
			
			QString s = families[n]->name();
			s.replace(tr(" "),tr(""));

			QString dirname = homeDir() + tr("/Modules/") + s;
			QDir dir(dirname);

			if (!dir.exists())		
				dir.setPath(homeDir() + tr("/Modules/") + s.toLower());

			if (!dir.exists())
				dir.setPath(QCoreApplication::applicationDirPath() + tr("/Modules/") + s);

			if (!dir.exists())
				dir.setPath(QCoreApplication::applicationDirPath() + tr("/Modules/") + s.toLower());

			if (!dir.exists())
				continue;
			
			dir.setFilter(QDir::Files);
			dir.setSorting(QDir::Time);
			QFileInfoList list = dir.entryInfoList();

			for (int i = 0; i < list.size(); ++i)
			{
				QFileInfo fileInfo = list.at(i);
				if (MainWindow::OPEN_FILE_EXTENSIONS.contains(fileInfo.suffix()))
				{
						QPair< QList<ItemHandle*>, QList<QGraphicsItem*> > items = mainWindow-> getItemsFromFile(fileInfo.absoluteFilePath(), root);
						if (!items.first.isEmpty())
						{
							for (int j=0; j < items.second.size(); ++j)
								if (!getHandle(items.second[j]))
									delete items.second[j];
							listOfLists << QPair<QString, QList<ItemHandle*> >(fileInfo.baseName(),items.first);
						}
				}
			}
		}
	}
	
	void ModuleCombinatorics::writeModels()
	{
		NetworkHandle * network = currentNetwork();
		if (!network) return;
		
		QList<ItemHandle*> handles = network->handles();
		QHash< ItemHandle*, QList< QPair<QString, QList<ItemHandle*> > > > handleReplacements; 
		
		for (int i=0; i < handles.size(); ++i)
			if (handles[i]
				&& ConnectionHandle::cast(handles[i])
				&& !handles[i]->parent
				&& !handles[i]->children.isEmpty()
				&& handles[i]->family()
				&& !handleReplacements.contains(handles[i])
				&& handles[i]->hasTextData("Participants"))
			{
				QList< QPair< QString, QList<ItemHandle*> > > list;
				getModelsFor(handles[i], list);

				if (!list.isEmpty())
				{
					TextDataTable & participants = handles[i]->textDataTable("Participants");

					for (int j=0; j < participants.rows(); ++j)
					{
						for (int k=0; k < list.size(); ++k)
						{
							RenameCommand::findReplaceAllHandleData(list[k].second,participants.rowName(j).toUpper(),participants.at(j,0));
							RenameCommand::findReplaceAllHandleData(list[k].second,participants.rowName(j).toLower(),participants.at(j,0));
							RenameCommand::findReplaceAllHandleData(list[k].second,participants.rowName(j),participants.at(j,0));
						}
					}
					
					for (int j=0; j < list.size(); ++j)
						for (int k=0; k < list[j].second.size(); ++k)
							if (list[j].second[k] && !list[j].second[k]->parent && list[j].second[k]->family())
							{
								QString s = list[j].second[k]->name;
								RenameCommand::findReplaceAllHandleData(list[j].second, list[j].second[k]->name, handles[i]->fullName() + tr(".") + list[j].second[k]->name);
								list[j].second[k]->name = s;
								list[j].second[k]->setParent(handles[i], false);
							}

					handleReplacements[ handles[i] ] = list;
				}
			}
			
			int k = 0;
			QList<ItemHandle*> output;
			QHash<QString, double> stats;
			
			writeModels(k, stats, output, handles, handleReplacements);
			QList<ItemHandle*> keys = handleReplacements.keys();
			QList< QList< QPair< QString, QList<ItemHandle*> > > > lists = handleReplacements.values();
			QList<ItemHandle*> visited;
			
			//cleanup
			for (int i=0; i < lists.size(); ++i)
				for (int j=0; j < lists[i].size(); ++j)
					for (int k=0; k < lists[i][j].second.size(); ++k)
						if (lists[i][j].second[k] && keys[i] == lists[i][j].second[k]->parent && !visited.contains(lists[i][j].second[k]))
						{
							delete lists[i][j].second[k];
							visited += lists[i][j].second[k];
						}
			printStats(stats);
	}
	
	void ModuleCombinatorics::printStats(QHash<QString, double>& stats)
	{
			//set style data histogram 
			//set style fill solid
			//plot 'data' using 2:xtic(1) title 'A', '' using 3:xtic(1) title 'B', '' u 4:xtic(1) title 'C'
			QWidget * tool = mainWindow->tool("Gnuplot");
			if (tool)
			{
				QFile file(tempDir() + tr("/stats.out"));
				if (file.open(QIODevice::WriteOnly))
				{
					QString s;

					QStringList keys(stats.keys());
					for (int i=0; i < keys.size(); ++i)
					{
						s += keys[i];
						s += tr(" ");
						s += QString::number(stats[ keys[i] ]);
						s += tr("\n");
					}
				
					file.write(s.toAscii());
					file.close();
				
					GnuplotTool * gnuplot = static_cast<GnuplotTool*>(tool);
				
					s = tr("set style data histogram\nset style fill solid\nplot 'stats.out' using 2:xtic(1) notitle");
					gnuplot->runScript(s);
				}
			}
	}
	
	void ModuleCombinatorics::writeModels(int& k, QHash<QString, double>& stats, QList<ItemHandle*>& output, const QList<ItemHandle*>& handles, const QHash< ItemHandle*, QList< QPair< QString, QList<ItemHandle*> > > >& handleReplacements)
	{
		ItemHandle * h;
		if (output.size() >= handles.size())
		{
			++k;
			MonteCarlo(k, output);
			return;
		}
		
		int j = output.size();
		h = handles[j];
		output << h;

		if (h->children.isEmpty() || !handleReplacements.contains(h))
		{
			writeModels(k, stats, output, handles, handleReplacements);
		}
		else
		{
			QList<ItemHandle*> originalChildren = h->children;

			QList< QPair< QString, QList<ItemHandle*> > > newChildren = handleReplacements[h];
			for (int i=0; i < newChildren.size(); ++i)
			{
				h->children = newChildren[i].second;
				writeModels(k, stats, output, handles, handleReplacements);
			}

			h->children = originalChildren;
		}
	}
	
	void ModuleCombinatorics::run()
	{
		writeModels();
	}
	
	ModuleCombinatorics::ModuleCombinatorics() : Tool("Module Combinatorics", "Module tools")
	{
	}
	
	bool ModuleCombinatorics::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		
		if (!main) return false;
		QWidget * tool = main->tool(tr("Dynamic Library Menu"));

		if (tool)
		{
			DynamicLibraryMenu * libTool = static_cast<DynamicLibraryMenu*>(tool);
			QToolButton * button = libTool->addFunction(tr("Network structure"), tr("Feature analysis"));
			QAction * action = libTool->addMenuItem(tr("Network structure"), tr("Feature analysis"));
			
			connect(button, SIGNAL(pressed()), this, SLOT(run()));
			connect(action, SIGNAL(triggered()), this, SLOT(run()));
		}
		
		return true;
	}
	
	void MonteCarlo(int & index, QList<ItemHandle*>& handles, QList<int> & selectedModules)
	{
		if (simulationThreads.isEmpty()) return;
		
		for (int i=0; i < simulationThreads.size(); ++i)
		{
			if (simulationThreads[i]->isRunning())
				simulationThreads[i]->terminate();
			simulationThreads[i]->updateModel(handles);
			simulationThreads[i]->setMethod(SimulationThread::Deterministic);
			simulationThreads[i]->setStartTime(startTime);
			simulationThreads[i]->setEndTime(endTime);
			simulationThreads[i]->setNumPoints(numSteps);
		}
		
		QSemaphore sem(simulationThreads.size());
		for (int i=0; i < simulationThreads.size(); ++i)
		{
			sem.acquire();
			simulationThreads[i]->setSemaphore(&sem);
			simulationThread1->start();
		}
		for (int i=0; i < simulationThreads.size(); ++i)
			sem.acquire();
		
		for (int i=0; i < simulationThreads.size(); ++i)
			sem.release();
		
		
		
		NumericalDataTable * results1 = ConvertValue(simulationThread1->result() ),
	 											  * results2 = ConvertValue(simulationThread2->result() );
	 	
	 	
	 	
	 	delete results1;
	 	delete results2;
	}

}
/*
extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::ModuleCombinatorics * tool = new Tinkercell::ModuleCombinatorics();
	main->addTool(tool);
}
*/

