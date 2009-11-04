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
    ModuleIconTool::ModuleIconTool() : Tool(tr("Module Icon Tool")), buttonGroup(this)
    {
        mode = none;
		separator = 0;
		addToTabMenu = new QAction(tr("Add module to calatog"),this);
		addToTabMenu->setIcon(tr(":/images/plus.png"));
        connect(&buttonGroup,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(insertModuleFromFile(QAbstractButton*)));
    }
	
	bool ModuleTool::setMainWindow(MainWindow * main)
    {
		Tool::setMainWindow(main);
        if (mainWindow != 0)
        {            
            connect(mainWindow,SIGNAL(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)),
                    this,SLOT(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)));
					
			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));
			
			toolLoaded(0);
        }
        return true;
    }
	
	void ModuleTool::toolLoaded(Tool*)
	{
		Tool * tool = mainWindow->tool(tr("Nodes Tree"));
		if (tool)
		{
			NodesTree * nodesTree = static_cast<NodesTree*>(tool);
			connect(this,SIGNAL(addNewButton(const QList<QToolButton*>& ,const QString& )),nodesTree,SLOT(addNewButton(const QList<QToolButton*>& ,const QString& )));
		}
		
		tool = mainWindow->tool(tr("Save and Load"));
		if (tool)
		{
			LoadSaveTool * loadSaveTool = static_cast<LoadSaveTool*>(tool);
			connect(this,SIGNAL(loadItems(QList<QGraphicsItem*>&, const QString& )),loadSaveTool,SLOT(loadItems(QList<QGraphicsItem*>&, const QString& )));
			connect(this,SIGNAL(saveItems(const QList<QGraphicsItem*>&, const QString&)),loadSaveTool,SLOT(saveItems(const QList<QGraphicsItem*>&, const QString&)));
		}
	}
		
	void  ModuleTool::mouseDoubleClicked (GraphicsScene * scene, QPointF , QGraphicsItem * item, Qt::MouseButton, Qt::KeyboardModifiers modifier)
    {
		if (!scene) return;
		ItemHandle * handle = getHandle(item);
		if (modifier == Qt::ControlModifier && handle->isA(tr("Module")) && NodeGraphicsItem::cast(item))
		{
			QList<QGraphicsItem*> allitems = handle->allGraphicsItems(), items;
			QRectF rect = item->sceneBoundingRect();
			for (int i=0; i < allitems.size(); ++i)
				if (allitems[i] && rect.intersects(allitems[i]->sceneBoundingRect()))
					items << allitems[i];
			
			QDir homeDir(MainWindow::userHome());
			if (!homeDir.cd(tr("modules")))
			{
				homeDir.mkdir(tr("modules"));
				homeDir.cd(tr("modules"));
			}
			QString filename = MainWindow::userHome() + tr("/modules/") + handle->fullName(tr("_")) + tr(".xml");
			QRectF viewport = scene->viewport();
			int w = 100;
			int h = (int)(viewport.height() * w/viewport.width());
			QImage image(w,h,QImage::Format_ARGB32);
			scene->print(&image);
			emit saveItems(items, filename);
			
			QToolButton * button = new QToolButton(this);
			button->setIcon(QPixmap::fromImage(image));
			button->setText(handle->fullName());
			button->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
			
			emit addNewButton(QList<QToolButton*>() << button,tr("Modules"));
		}
    }
	
	void ModuleTool::itemsSelected(GraphicsScene * , const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers)
	{
		if (mainWindow)
		{
			NodeGraphicsItem * node;
			ItemHandle * handle;
			
			if (items.size() != 1 || 
				!(node = NodeGraphicsItem::cast(items[0]))
				!(handle = node->handle()))
			{	
				if (separator)
					mainWindow->contextItemsMenu.removeAction(addToTabMenu);
				return;
			}
			if (separator)
				mainWindow->contextItemsMenu.addAction(separator);
			else
				separator = mainWindow->contextItemsMenu.addSeparator();

			mainWindow->contextItemsMenu.addAction(addToTabMenu);
			
			return;
		}
	}

	void ModuleTool::insertModuleFromFile(QAbstractButton* button)
	{
		if (!button) return;
		QString filename = MainWindow::userHome() + tr("/modules/") + button->text() + tr(".xml");
		
		emit 
	}
	
	void ModuleTool::readModuleFiles()
	{
		QDir homeDir(MainWindow::userHome());
		if (!homeDir.cd(tr("modules")))
		{
			homeDir.mkdir(tr("modules"));
			homeDir.cd(tr("modules"));
		}
		
		QList<QToolButton*> buttons;
		
		emit addNewButton(buttons,tr("Module"));
	}
}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
    if (!main) return;

    Tinkercell::ModuleIconTool * tool = new Tinkercell::ModuleIconTool;
    main->addTool(tool);

}
