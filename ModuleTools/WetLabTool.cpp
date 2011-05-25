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
	#define WINDOW_WIDTH 200

    WetLabTool::WetLabTool() : Tool(tr("Wet-Lab Data Tool"),tr("Module tools"))
    {
        setPalette(QPalette(QColor(255,255,255,255)));
        setAutoFillBackground(true);
    }

	//insert interface node
    void WetLabTool::select(int)
    {
    }

	bool WetLabTool::setMainWindow(MainWindow * main)
    {
		Tool::setMainWindow(main);
        if (mainWindow != 0)
        {
        	connect(mainWindow,SIGNAL(itemsDropped(GraphicsScene *, const QString&, const QPointF&)),
				this, SLOT(itemsDropped(GraphicsScene *, const QString&, const QPointF&)));
        	
			connect(mainWindow,SIGNAL(mousePressed(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
					this,SLOT(sceneClicked(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)),
                    this,SLOT(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)));
            
            connect(mainWindow,SIGNAL(keyPressed(GraphicsScene*,QKeyEvent *)),
				this ,SLOT(keyPressed(GraphicsScene*,QKeyEvent *)));

			connect(mainWindow,SIGNAL(mouseMoved(GraphicsScene* , QGraphicsItem*, QPointF , Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)),
                    this,SLOT(mouseMoved(GraphicsScene* , QGraphicsItem*, QPointF , Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)));

			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),
					this,SLOT(escapeSignal(const QWidget*)));

            connect(mainWindow,SIGNAL(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)),
					this, SLOT(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)));

			connect(mainWindow,SIGNAL(itemsInserted(NetworkHandle*, const QList<ItemHandle*>&)),
                    this, SLOT(itemsInserted(NetworkHandle*, const QList<ItemHandle*>&)));
                    
			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			toolLoaded(mainWindow->tool(tr("")));
        }

        return true;
    }

	void WetLabTool::toolLoaded(Tool*)
	{
		static bool connected1 = false;
		
		if (mainWindow->tool(tr("Nodes Tree")) && !nodesTree)
		{
			Tool * tool = static_cast<Tool*>(mainWindow->tool(tr("Nodes Tree")));
			nodesTree = static_cast<NodesTree*>(tool);
		}
		
		QStringList familyNames;
		if (mainWindow->tool(tr("Connections Tree")) && !connectionsTree)
		{
			Tool * tool = static_cast<Tool*>(mainWindow->tool(tr("Connections Tree")));
			connectionsTree = static_cast<ConnectionsTree*>(tool);
		}

		QString appDir = QCoreApplication::applicationDirPath();
		QString home = homeDir();
		
		if (connectionsTree)
		{
			ConnectionFamily * experimentFamily = connectionsTree->getFamily(tr("Experiment"));
			ConnectionFamily * dataFamily = connectionsTree->getFamily(tr("Data"));
			if (!experimentFamily)
			{
				experimentFamily = new ConnectionFamily(tr("Experiment"));
				experimentFamily->pixmap = QPixmap(tr(":/images/lab.png"));
				experimentFamily->description = tr("A protocol used to obtain some quantitative results about the system of interest");
				experimentFamily->textAttributes[tr("Type")] = tr("");
				experimentFamily->numericalAttributes[tr("Time duration")] = 0.0;
				experimentFamily->graphicsItems << new ArrowHeadItem(appDir + tr("/Graphics/") + NodesTree::themeDirectory + tr("/Arrows/default.xml"))
											 							<< new ArrowHeadItem(home + tr("/Lab/microscope.xml"));
			}
			
			if (!dataFamily)
			{
				dataFamily = new ConnectionFamily(tr("Data"));
				dataFamily->pixmap = QPixmap(tr(":/images/statistics.PNG"));
				dataFamily->description = tr("Quantitative results about the system of interest");
				dataFamily->textAttributes[tr("Functional description")] = tr("");
				dataFamily->graphicsItems << new ArrowHeadItem(appDir + tr("/Graphics/") + NodesTree::themeDirectory + tr("/Arrows/default.xml"))
											 				<< new ArrowHeadItem(tr(":/images/statistics.xml"));
			}
			
			if (QFile::exists(home + tr("/Lab/lab.nt")))
				connectionsTree->readTreeFile(home + tr("/Lab/lab.nt"));
			else
			if (QFile::exists(appDir + tr("/Lab/lab.nt")))
				connectionsTree->readTreeFile(appDir + tr("/Lab/lab.nt"));
			
			QList<ItemFamily*> childFamilies;
			if (experimentFamily)
				childFamilies += experimentFamily->allChildren();
			if (dataFamily)
				childFamilies += dataFamily->allChildren();

			for (int i=0; i < childFamilies.size(); ++i)
				familyNames << childFamilies[i]->name();
		}

		if (mainWindow->tool(tr("Parts and Connections Catalog")) && !connected1)
		{
			Tool * tool = static_cast<Tool*>(mainWindow->tool(tr("Parts and Connections Catalog")));
			catalogWidget = static_cast<CatalogWidget*>(tool);

			connect(catalogWidget, SIGNAL(buttonPressed(const QString&)), this, SLOT(labButtonPressed(const QString&)));

			QList<QToolButton*> newButtons = catalogWidget->addNewButtons(
					tr("Lab"),
					QStringList() 	<< tr("New experiment") << tr("New data"),
					QList<QIcon>() << QIcon(QPixmap(tr(":/images/lab.png"))) << QIcon(QPixmap(tr(":/images/statistics.PNG"))),
					QStringList() 	<< tr("A wet-lab experiment") << tr("Data obtained from a wet-lab experiment")
				);
			
			if (!familyNames.isEmpty())
				catalogWidget->showButtons(familyNames);

			connected1 = true;
		}
	}

    void WetLabTool::escapeSignal(const QWidget* )
    {
    }

	void WetLabTool::sceneClicked(GraphicsScene *scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
	{
	}

	void WetLabTool::itemsAboutToBeInserted(GraphicsScene* scene, QList<QGraphicsItem *>& items, QList<ItemHandle*>& handles, QList<QUndoCommand*>& commands)
	{
	}

	void WetLabTool::itemsAboutToBeRemoved(GraphicsScene* scene, QList<QGraphicsItem *>& items, QList<ItemHandle*>& handles, QList<QUndoCommand*>& commands)
	{
	}
	
    void WetLabTool::itemsInserted(NetworkHandle * network, const QList<ItemHandle*>& handles)
	{
    }

    void WetLabTool::mouseMoved(GraphicsScene* scene, QGraphicsItem * hoverOverItem, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>& items)
    {
    }

	void WetLabTool::labButtonClicked ( QAbstractButton * button )
	{
		if (button)
		{
			QString filename = button->toolTip();
			NetworkHandle * network = currentNetwork();
			
			if (QFile::exists(filename) && network)
			{
				NetworkWindow * window = network->currentWindow();
				
				if (!window || !window->handle || !window->handle->family()) return;
				ItemHandle * parentHandle = window->handle;
				
				substituteModel(parentHandle, filename, window);
				if (currentScene())
					currentScene()->fitAll();
			}
		}
	}
	
	void WetLabTool::keyPressed(GraphicsScene* scene,QKeyEvent * keyEvent)
	{
		if (!keyEvent || keyEvent->modifiers() || !scene || !scene->useDefaultBehavior()) return;

		QList<QGraphicsItem*> & selected = scene->selected();
		
		if (selected.size() == 1 && keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Space)
		{
			mouseDoubleClicked(scene,QPointF(),selected[0],Qt::LeftButton,(Qt::KeyboardModifiers)0);
		}
	}

	void WetLabTool::mouseDoubleClicked (GraphicsScene * scene, QPointF , QGraphicsItem * item, Qt::MouseButton, Qt::KeyboardModifiers modifiers)
    {
		if (!scene || !scene->network || !item || !mainWindow || modifiers || !(ArrowHeadItem::cast(item) || ConnectionGraphicsItem::cast(item))) return;

		ItemHandle * handle = getHandle(item);
		
		if (!handle)
		{
			if (qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(item))
				handle = getHandle(qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(item)->connectionItem);
			else
			{
				ArrowHeadItem * arrow = ArrowHeadItem::cast(item);
				if (arrow)
				{
					if (arrow->connectionItem && arrow->connectionItem->centerRegionItem == arrow)
						handle = getHandle(arrow->connectionItem);
				}
			}
		}

		ConnectionHandle * chandle = ConnectionHandle::cast(handle);

		if (chandle && handle->family() && !handle->children.isEmpty())
		{
			QList<TextEditor*> editors = scene->network->editors();
			QList<GraphicsScene*> scenes = scene->network->scenes();

			for (int i=0; i < editors.size(); ++i)
				if (editors[i]->localHandle() == handle && !editors[i]->text().isEmpty())
				{
					editors[i]->popOut();
					return;
				}

			for (int i=0; i < scenes.size(); ++i)
				if (scenes[i]->localHandle() == handle && !scenes[i]->items().isEmpty())
				{
					scenes[i]->popOut();
					scenes[i]->fitAll();
					return;
				}
		}
    }

	void WetLabTool::moduleButtonPressed(const QString& name)
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		if (name == tr("New module"))
		{
			mainWindow->setCursor(Qt::ArrowCursor);
			exportModule();
		}

		if (mode != none)
			scene->useDefaultBehavior(false);
	}

	void WetLabTool::itemsDropped(GraphicsScene * scene, const QString& family, const QPointF& point)
	{
		if (scene && scene->network && family == tr("New module") && mode == none)
		{
			mode = inserting;
			sceneClicked(scene, point, Qt::LeftButton, 0);
			mode = none;
		}
	}
}

