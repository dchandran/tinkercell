/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

****************************************************************************/
#include <math.h>
#include <QRegExp>
#include <QProcess>
#include "ItemFamily.h"
#include "NetworkHandle.h"
#include "ItemHandle.h"
#include "GraphicsScene.h"
#include "TextEditor.h"
#include "ConsoleWindow.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "LoadSaveTool.h"
#include "TreeButton.h"
#include "WetLabTool.h"
#include "GlobalSettings.h"

namespace Tinkercell
{
    WetLabTool::WetLabTool() : Tool(tr("Wet-Lab Data Tool"),tr("Module tools")), nodesTree(0), connectionsTree(0)
    {
        setPalette(QPalette(QColor(255,255,255,255)));
        setAutoFillBackground(true);
    }

	bool WetLabTool::setMainWindow(MainWindow * main)
    {
		Tool::setMainWindow(main);
        if (mainWindow != 0)
        {
			connect(mainWindow, SIGNAL(itemsAboutToBeInserted(GraphicsScene*, QList<QGraphicsItem *>& , QList<ItemHandle*>& , QList<QUndoCommand*>& )),
						 this, SLOT(itemsAboutToBeInserted(GraphicsScene* , QList<QGraphicsItem *>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)));
			connect(mainWindow,SIGNAL(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)),
                    this,SLOT(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)));
            
            connect(mainWindow,SIGNAL(keyPressed(GraphicsScene*,QKeyEvent *)),
				this ,SLOT(keyPressed(GraphicsScene*,QKeyEvent *)));
                    
			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			toolLoaded(0);
        }

        return true;
    }

	void WetLabTool::toolLoaded(Tool*)
	{
		static bool connected1 = false;
		QStringList familyNames;
		QString appDir = QCoreApplication::applicationDirPath();
		QString home = homeDir();
		
		if (mainWindow->tool(tr("Nodes Tree")) && !nodesTree)
		{
			Tool * tool = static_cast<Tool*>(mainWindow->tool(tr("Nodes Tree")));
			nodesTree = static_cast<NodesTree*>(tool);
		}

		if (mainWindow->tool(tr("Connections Tree")) && !connectionsTree)
		{
			Tool * tool = static_cast<Tool*>(mainWindow->tool(tr("Connections Tree")));
			connectionsTree = static_cast<ConnectionsTree*>(tool);
		}

		ConnectionFamily * experimentFamily = connectionsTree->getFamily(tr("Experiment"));
		NodeFamily * dataFamily = nodesTree->getFamily(tr("Data"));

		if (nodesTree && !dataFamily)
		{
			dataFamily = new NodeFamily(tr("Data"));
			dataFamily->pixmap = QPixmap(tr(":/images/statistics.PNG"));
			dataFamily->description = tr("Quantitative results about the system of interest");
			dataFamily->textAttributes[tr("Functional description")] = tr("");
			dataFamily->graphicsItems << new NodeGraphicsItem(tr(":/images/statistics.xml"));
			nodesTree->insertFamily(dataFamily,0);
			if (QFile::exists(home + tr("/Lab/wetlabdata.nt")))
				nodesTree->readTreeFile(home + tr("/Lab/wetlabdata.nt"));
			else
			if (QFile::exists(appDir + tr("/Lab/wetlabdata.nt")))
				nodesTree->readTreeFile(appDir + tr("/Lab/wetlabdata.nt"));
			QList<ItemFamily*> childFamilies = dataFamily->allChildren();
			familyNames << "data";
			for (int i=0; i < childFamilies.size(); ++i)
				familyNames << childFamilies[i]->name();
		}

		if (connectionsTree && dataFamily && !experimentFamily)
		{
			experimentFamily = new ConnectionFamily(tr("Experiment"));
			experimentFamily->pixmap = QPixmap(tr(":/images/lab.png"));
			experimentFamily->description = tr("A protocol used to obtain some quantitative results about the system of interest");
			experimentFamily->textAttributes[tr("Type")] = tr("");
			experimentFamily->numericalAttributes[tr("Time duration")] = 0.0;
			experimentFamily->graphicsItems << new ArrowHeadItem(appDir + tr("/Graphics/") + NodesTree::themeDirectory + tr("/Arrows/default.xml"))
																	<< new ArrowHeadItem(home + tr("/Lab/microscope.xml"));
			connectionsTree->insertFamily(experimentFamily,0);

			if (QFile::exists(home + tr("/Lab/experiments.nt")))
				connectionsTree->readTreeFile(home + tr("/Lab/experiments.nt"));
			else
			if (QFile::exists(appDir + tr("/Lab/experiments.nt")))
				connectionsTree->readTreeFile(appDir + tr("/Lab/experiments.nt"));

			QList<ItemFamily*> childFamilies = experimentFamily->allChildren();
			for (int i=0; i < childFamilies.size(); ++i)
				familyNames << childFamilies[i]->name();
		}

		if (mainWindow->tool(tr("Parts and Connections Catalog")) && !connected1)
		{
			Tool * tool = static_cast<Tool*>(mainWindow->tool(tr("Parts and Connections Catalog")));
			catalogWidget = static_cast<CatalogWidget*>(tool);

			catalogWidget->createNewGroup(
					tr("Wet-Lab"), 
					QStringList() 	<< tr("Experiment") << tr("Data")
			);
			
			if (!familyNames.isEmpty())
				catalogWidget->showButtons(familyNames);

			connected1 = true;
		}
	}
	
	void WetLabTool::keyPressed(GraphicsScene* scene,QKeyEvent * keyEvent)
	{
		if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Space)
		{
			QList<ItemHandle*> handles = getHandle(scene->selected());
			bool containsExp = false;
			for (int i=0; i < handles.size(); ++i)
				if (handles[i] && handles[i]->isA("Experiment"))
				{
					containsExp = true;
					break;
				}
			if (mainWindow && containsExp)
			{
				Tool * tool = mainWindow->tool("Text Attributes");
				if (tool)
				{
					tool->select();
				}
			}
		}
	}

	void WetLabTool::mouseDoubleClicked (GraphicsScene * scene, QPointF , QGraphicsItem * item, Qt::MouseButton, Qt::KeyboardModifiers modifiers)
    {
		QList<ItemHandle*> handles = getHandle(scene->selected());
		bool containsExp = false;
		for (int i=0; i < handles.size(); ++i)
			if (handles[i] && handles[i]->isA("Experiment"))
			{
				containsExp = true;
				break;
			}
		if (mainWindow && containsExp)
		{
			Tool * tool = mainWindow->tool("Text Attributes");
			if (tool)
			{
				tool->select();
			}
		}
    }

		void WetLabTool::itemsAboutToBeInserted(GraphicsScene* scene, QList<QGraphicsItem *>& items, QList<ItemHandle*>& handles, QList<QUndoCommand*>& commands)
	{
		if (!scene || !scene->network) return;

		QString home = homeDir();
		ConnectionGraphicsItem * connection = 0;
		ItemHandle * handle = 0;
		for (int i=0; i < items.size(); ++i)
			if ((connection = ConnectionGraphicsItem::cast(items[i])) &&
				(handle = connection->handle()) &&
				handle->isA("Experiment") &&
				!connection->centerRegionItem)
		{
			ArrowHeadItem * newDecorator = new ArrowHeadItem(home + tr("/Lab/microscope.xml"),connection);
			connection->centerRegionItem = newDecorator;
			items += newDecorator;
		}
	}

}

