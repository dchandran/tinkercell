#include <QFile>
#include <QDir>
#include "ModelReader.h"
#include "ModuleCombinatorics.h"

namespace Tinkercell
{
	void ModuleCombinatorics::getModelsFor(ItemFamily * root, QList< QList<ItemHandle*> >& listOfLists)
	{
		QList<ItemFamily*> families, children;
		families << root;
		
		for (int i=0; i < families.size(); ++i)
		{
			children = families[i]->children();
			for (int j=0; j < children.size(); ++j)
				if (!families.contains(children[j]))
					families << children[j];
			
			QString s = families[i]->name();
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
					QFile file(fileInfo.absoluteFilePath());
					if (file.open(QFile::ReadOnly | QFile::Text))
					{
						ModelReader reader;
						QList< QPair<QString,ItemHandle*> > pairs = reader.readHandles(file);
						if (!pairs.isEmpty())
						{
							QList<ItemHandle*> handles;
							for (int j=0; j < pairs.size(); ++j)
								handles << pairs[j].second;
							listOfLists << handles;
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
		QHash< ItemHandle*, QList< QList<ItemHandle*> > > handleReplacements; 
		QList<ItemHandle*> modules;
		
		for (int i=0; i < handles.size(); ++i)
			if (handles[i] 
				&& ConnectionHandle::cast(handles[i]) 
				&& !handles[i]->parent
				&& !handles[i]->children.isEmpty()
				&& handles[i]->family()
				&& !handleReplacements.contains(handles[i]))
			{
				QList< QList<ItemHandle*> > list;
				getModelsFor(handles[i]->family(),list);
				handleReplacements[ handles[i] ] = list;
				if (!list.isEmpty())
					modules << handles[i];
			}
		
	}

}

