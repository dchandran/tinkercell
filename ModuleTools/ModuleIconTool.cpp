/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This tool handles module connections that merge items from two modules

****************************************************************************/

#include <math.h>
#include <QDir>
#include "ItemHandle.h"
#include "GraphicsScene.h"
#include "ConsoleWindow.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "LoadSaveTool.h"
#include "ModuleIconTool.h"

namespace Tinkercell
{
    ModuleIconTool::ModuleIconTool() : Tool(tr("Module Icon Tool"),tr("Module tools")), buttonGroup(this)
    {
        mode = none;
		moduleItem = 0;
		moduleHandle = 0;
		separator = 0;
		addToTabMenu = new QAction(tr("Add module to calatog"),this);
		addToTabMenu->setIcon(QIcon(tr(":/images/plus.png")));
		connect(addToTabMenu,SIGNAL(triggered()),this,SLOT(addNewButton()));
        connect(&buttonGroup,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(enterInsertMode(QAbstractButton*)));
    }

	bool ModuleIconTool::setMainWindow(MainWindow * main)
    {
		Tool::setMainWindow(main);
        if (mainWindow != 0)
        {
            connect(mainWindow,SIGNAL(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)),
                    this,SLOT(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)));
			connect(mainWindow,SIGNAL(mousePressed(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
					this,SLOT(mousePressed(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));
			connect(mainWindow,SIGNAL(escapeSignal(const QWidget *)),
					this,SLOT(escapeSignal(const QWidget *)));
			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>& , QPointF, Qt::KeyboardModifiers)),
					this,SLOT(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>& , QPointF, Qt::KeyboardModifiers)));
			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));
			toolLoaded(0);
        }
        return true;
    }

	void ModuleIconTool::toolLoaded(Tool*)
	{
		static bool tool1 = false, tool2 = false;

		if (tool1 && tool2) return;

		Tool * tool = mainWindow->tool(tr("Nodes Tree"));
		if (tool && !tool1)
		{
			tool1 = true;
			NodesTree * nodesTree = static_cast<NodesTree*>(tool);
			connect(this,SIGNAL(addNewButton(const QList<QToolButton*>& ,const QString& )),nodesTree,SLOT(addNewButton(const QList<QToolButton*>& ,const QString& )));
			readModuleFiles();
		}

		tool = mainWindow->tool(tr("Save and Load"));
		if (tool && !tool2)
		{
			tool2 = true;
			LoadSaveTool * loadSaveTool = static_cast<LoadSaveTool*>(tool);
			connect(this,SIGNAL(loadItems(QList<QGraphicsItem*>&, const QString& )),loadSaveTool,SLOT(loadItems(QList<QGraphicsItem*>&, const QString& )));
			connect(this,SIGNAL(saveItems(const QList<QGraphicsItem*>&, const QString&)),loadSaveTool,SLOT(saveItems(const QList<QGraphicsItem*>&, const QString&)));
		}
	}

	void ModuleIconTool::addNewButton(GraphicsScene * scene, ItemHandle * handle, const QRectF& rect0)
	{
		if (!handle || !scene || !handle->isA(tr("Module"))) return;

		QRectF rect = rect0;
		rect.adjust(-20,-20,20,20);

		QList<QGraphicsItem*> allitems = handle->allGraphicsItems(), items;

		for (int i=0; i < allitems.size(); ++i)
			if (allitems[i] && rect.intersects(allitems[i]->sceneBoundingRect()))
				items << allitems[i];

		QDir homeDir(MainWindow::userHome());
		if (!homeDir.cd(tr("modules")))
		{
			homeDir.mkdir(tr("modules"));
			homeDir.cd(tr("modules"));
		}
		QString filename = MainWindow::userHome() + tr("/modules/") + handle->fullName(tr("_")) + tr(".xml"),
				iconfile = MainWindow::userHome() + tr("/modules/") + handle->fullName(tr("_")) + tr(".png");
		/*
		QRectF viewport = scene->viewport();
		int w = 100;
		int h = (int)(viewport.height() * w/viewport.width());
		QImage image(w,h,QImage::Format_ARGB32);
		scene->print(&image,rect0);
		QPixmap pixmap = QPixmap::fromImage(image);
		*/
		QPixmap pixmap(tr(":/images/module.png"));
		pixmap.save(iconfile,"png");

		emit saveItems(items, filename);

		QToolButton * button = new QToolButton(this);
		button->setIcon(QIcon(pixmap));
		button->setText(handle->fullName());
		button->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
		buttonGroup.addButton(button);
		emit addNewButton(QList<QToolButton*>() << button,tr("Modules"));
	}

	void  ModuleIconTool::mouseDoubleClicked (GraphicsScene * scene, QPointF , QGraphicsItem * item, Qt::MouseButton, Qt::KeyboardModifiers modifier)
    {
		if (!scene || !item) return;
		ItemHandle * handle = getHandle(item);
		if (modifier == Qt::ControlModifier && handle && handle->isA(tr("Module")) && NodeGraphicsItem::cast(item))
		{
			moduleItem = NodeGraphicsItem::cast(item);
			addNewButton(scene,handle,moduleItem->sceneBoundingRect());
		}
    }

	void ModuleIconTool::addNewButton()
	{
		if (moduleItem && moduleHandle)
			addNewButton(currentScene(),moduleHandle,moduleItem->sceneBoundingRect());
	}

	void ModuleIconTool::itemsSelected(GraphicsScene * , const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers)
	{
		if (mainWindow)
		{
			NodeGraphicsItem * node;
			ItemHandle * handle;

			if (items.size() != 1 ||
				!(node = NodeGraphicsItem::cast(items[0])) ||
				!(handle = node->handle()) ||
				!handle->isA(tr("Module")))
			{
				if (separator)
					mainWindow->contextItemsMenu.removeAction(addToTabMenu);
				mainWindow->contextItemsMenu.removeAction(addToTabMenu);
				return;
			}
			if (separator)
				mainWindow->contextItemsMenu.addAction(separator);
			else
				separator = mainWindow->contextItemsMenu.addSeparator();

			mainWindow->contextItemsMenu.addAction(addToTabMenu);
			moduleHandle = handle;
			moduleItem = node;

			return;
		}
	}

	void ModuleIconTool::mousePressed(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers)
	{
		if (mode != inserting || !scene || scene->useDefaultBehavior) return;

		loadModule(scene,point);
	}

	void ModuleIconTool::escapeSignal(const QWidget * )
	{
		if (inserting)
		{
			GraphicsScene * scene = currentScene();
			if (scene)
			{
				scene->useDefaultBehavior = true;
				mainWindow->setCursor(Qt::ArrowCursor);
			}
			mode = none;

			filename = tr("");

			if (insertList.size() > 0)
				qDeleteAll(insertList);

            insertList.clear();
            moduleHandle = 0;
		}
	}

    void ModuleIconTool::loadModule(GraphicsScene * scene, QPointF point)
    {
        if (!scene || !scene->symbolsTable) return;

        if (insertList.isEmpty() && !filename.isEmpty() && !filename.isNull())
        {
            emit loadItems(insertList,filename);
        }

        NodeGraphicsItem * node;
        ConnectionGraphicsItem * connection;
		ItemHandle * handle;
		QPointF pos;

        QStringList allNames = scene->symbolsTable->handlesFullName.keys();
        QString moduleName = filename;

		if (insertList.size() > 0)
		{
		    for (int i=0; i < insertList.size(); ++i)
                if (NodeGraphicsItem::cast(insertList[i]) &&
                    (handle = getHandle(insertList[i])) &&
                    !handle->parent)
                {
                    pos = insertList[i]->pos();

                    if (allNames.contains(handle->name))
                    {
                        QString oldName = handle->name;
                        handle->name = RenameCommand::assignUniqueName(handle->name,allNames);
                        handle->name = RemoveDisallowedCharactersFromName(handle->name);
                        allNames << handle->name;

                        TextGraphicsItem * textItem;
                        for (int j=0; j < handle->graphicsItems.size(); ++j)
                            if ((textItem = TextGraphicsItem::cast(handle->graphicsItems[j]))
                                && textItem->text() == oldName)
                                textItem->setText(handle->name);
                    }

                    if (handle->isA(tr("module")))
                        moduleName = handle->name;
                }
		}

		pos = point - pos;

		for (int i=0; i < insertList.size(); ++i)
		{
		    if (connection = ConnectionGraphicsItem::cast(insertList[i]))
		    {
		        QList<QGraphicsItem*> controlpts = connection->controlPointsAsGraphicsItems();
		        for (int j=0; j < controlpts.size(); ++j)
                    if (controlpts[j] && !controlpts[j]->parentItem())
                        controlpts[j]->setPos( controlpts[j]->pos() + pos );
		    }
		    else
		    {
                insertList[i]->setPos( insertList[i]->pos() + pos );
                if (node = NodeGraphicsItem::cast(insertList[i]))
                    node->adjustBoundaryControlPoints();
		    }
		}

        QList<ItemHandle*> handles;
		QList<QGraphicsItem*> newList = cloneGraphicsItems(insertList,handles);

		scene->insert(moduleName + tr(" inserted"),insertList);
		insertList.clear();
		insertList = newList;

    }

	void ModuleIconTool::enterInsertMode(QAbstractButton* button)
	{
		if (!button) return;
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		mainWindow->sendEscapeSignal();
		filename = MainWindow::userHome() + tr("/modules/") + button->text().replace(tr("."),tr("_")) + tr(".xml");

        scene->useDefaultBehavior = false;
        mode = inserting;
        mainWindow->setCursor(QCursor(button->icon().pixmap(64,64)));
	}

	void ModuleIconTool::readModuleFiles()
	{
		QDir homeDir(MainWindow::userHome());
		if (!homeDir.cd(tr("modules")))
		{
			homeDir.mkdir(tr("modules"));
			homeDir.cd(tr("modules"));
		}

		QList<QToolButton*> buttons;

		QStringList files = homeDir.entryList(QStringList() << tr("*.xml"));

		for (int i=0; i < files.size(); ++i)
		{
			files[i].remove(tr(".xml"));
			QString filename = MainWindow::userHome() + tr("/modules/") + files[i] + tr(".xml"),
					iconfile = MainWindow::userHome() + tr("/modules/") + files[i] + tr(".png");

			QPixmap pixmap(iconfile);
			QToolButton * button = new QToolButton(this);
			button->setIcon(QIcon(pixmap));
			button->setText(files[i]);
			button->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
			buttonGroup.addButton(button);
			buttons << button;
		}

		emit addNewButton(buttons,tr("Modules"));
	}
}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
    if (!main) return;

    Tinkercell::ModuleIconTool * tool = new Tinkercell::ModuleIconTool;
    main->addTool(tool);

}
