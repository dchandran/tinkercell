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
	void ModuleCombinatorics::getModelsFor(ItemFamily * root, QList< QPair< QString, QList<ItemHandle*> > >& listOfLists)
	{
		if (!mainWindow) return;

		if (!nodesTree)
		{
			QWidget * tool = mainWindow->tool("Nodes Tree");
			if (tool)
				nodesTree = static_cast<NodesTree*>(tool);
		}
		if (!connectionsTree)
		{
			QWidget * tool = mainWindow->tool("Connections Tree");
			if (tool)
				connectionsTree = static_cast<ConnectionsTree*>(tool);
		}
		
		if (!nodesTree || !connectionsTree) return;
		
		QList<ItemFamily*> families, children;
		families << root;
		
		for (int n=0; n < families.size(); ++n)
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

			NodeFamily * nodeFamily;
			ConnectionFamily * connectionFamily;
			for (int i = 0; i < list.size(); ++i)
			{
				QFileInfo fileInfo = list.at(i);
				if (MainWindow::OPEN_FILE_EXTENSIONS.contains(fileInfo.suffix()))
				{
					QFile file(fileInfo.absoluteFilePath());
					if (file.open(QFile::ReadOnly | QFile::Text))
					{
						ModelReader reader;
						QList< QPair<QString,ItemHandle*> > pairs = reader.readHandles(&file);
						if (!pairs.isEmpty())
						{
							QList<ItemHandle*> handles;
							for (int j=0; j < pairs.size(); ++j)
								if (!handles.contains(pairs[j].second))
								{
									if (nodeFamily = nodesTree->getFamily(pairs[j].first))
									{
										pairs[j].second->setFamily(nodeFamily,false);
										handles << pairs[j].second;
									}
									else
									if (connectionFamily = connectionsTree->getFamily(pairs[j].first))
									{
										pairs[j].second->setFamily(connectionFamily,false);
										handles << pairs[j].second;
									}
								}
							listOfLists << QPair<QString, QList<ItemHandle*> >(fileInfo.baseName(),handles);
						}
						file.close();
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
				std::cout << handles[i]->fullName().toAscii().data() << "\n";
				getModelsFor(handles[i]->family(), list);

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
							if (!list[j].second[k]->parent && list[j].second[k]->family())
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
			QList< QList< QPair< QString, QList<ItemHandle*> > > > lists = handleReplacements.values();
			for (int i=0; i < lists.size(); ++i)
				for (int j=0; j < lists[i].size(); ++j)
					for (int k=0; k < lists[i][j].second.size(); ++k)
						delete lists[i][j].second[k];
			
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
			ModelFileGenerator::generateModelFile(tr("model") + QString::number(k), output);
			
			for (int i=0; i < output.size(); ++i)
				if ((h = output[i]) && handleReplacements.contains(h))
				{
					QList< QPair< QString, QList<ItemHandle*> > > list = handleReplacements[h];
					for (int j=0; j < list.size(); ++j)
						if (stats.contains(list[j].first))
							stats[ list[j].first ] += 10.0;
						else
							stats[ list[j].first ] = 10.0;
				}
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
		nodesTree = 0;
		connectionsTree = 0;
	}
	
	bool ModuleCombinatorics::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		
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

}

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::ModuleCombinatorics * tool = new Tinkercell::ModuleCombinatorics();
	main->addTool(tool);
}


