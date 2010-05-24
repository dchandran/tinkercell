/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

The tool class is the parent class for all plugins.
A Tool is a Qt Widget with a name and pointer to the Tinkercell MainWindow.


****************************************************************************/

#include "MainWindow.h"
#include "TextEditor.h"
#include "NetworkHandle.h"
#include "GraphicsScene.h"
#include "Tool.h"

namespace Tinkercell
{
	ToolGraphicsItem::ToolGraphicsItem(Tool * t) : tool(t)
	{
	}

	void ToolGraphicsItem::select()
	{
		if (tool)
		{
			int k = tool->graphicsItems.indexOf(this);
			if (k > -1)
				tool->select(k);
		}
	}

	void ToolGraphicsItem::deselect()
	{
		if (tool)
		{
			int k = tool->graphicsItems.indexOf(this);
			if (k > -1)
				tool->deselect(k);
		}
	}

	Tool::~Tool()
	{
		for (int i=0; i < graphicsItems.size(); ++i)
			if (graphicsItems[i] && !MainWindow::invalidPointers.contains((void*)(graphicsItems[i])))
			{
				if (graphicsItems[i]->scene())
					graphicsItems[i]->scene()->removeItem(graphicsItems[i]);
					
				MainWindow::invalidPointers[(void*)(graphicsItems[i])] = true;
				delete graphicsItems[i];
			}
	}

	Tool::Tool() : QWidget(), actionsGroup(this)
	{
		setMouseTracking(true);
		name = category = tr("");
		mainWindow = 0;
	}

	Tool::Tool(const QString& Name, const QString& Cat, QWidget * parent): QWidget(parent), name(Name), category(Cat), mainWindow(0), actionsGroup(this)
	{		
	}

	bool Tool::setMainWindow(MainWindow * main)
	{
		if (mainWindow == main)
			return true;
		mainWindow = main;
		connect(&actionsGroup,SIGNAL(triggered(QAction*)),this,SLOT(actionTriggered(QAction*)));
		if (main)
		{
			if (!main->tool(name))
				main->addTool(this);
			return true;
		}
		return false;
	}
	
	void Tool::actionTriggered( QAction * action )
	{
		int k = actionsGroup.actions().indexOf(action);
		if (k > -1)
			select(k);
	}

	void Tool::select(int)
	{
		if (parentWidget() && !parentWidget()->isVisible())
			parentWidget()->show();
		show();
		emit selected();
	}

	void Tool::deselect(int)
	{
		hide();
		emit deselected();
	}

	void ToolGraphicsItem::visible(bool b)
	{
		QGraphicsItemGroup::setVisible(b);
	}
	
	NetworkWindow* Tool::currentWindow() const
	{
		if (mainWindow)
			return mainWindow->currentWindow();
		return 0;		
	}

	GraphicsScene* Tool::currentScene() const
	{
		if (mainWindow)
			return mainWindow->currentScene();
		return 0;
	}

	TextEditor* Tool::currentTextEditor() const
	{
		if (mainWindow)
			return mainWindow->currentTextEditor();
		return 0;
	}

	NetworkHandle * Tool::currentNetwork() const
	{
		if (mainWindow)
			return mainWindow->currentNetwork();
		return 0;
	}
	
	QString Tool::homeDir()
	{
		return MainWindow::homeDir();
	}
	
	QString Tool::tempDir()
	{
		return MainWindow::tempDir();
	}

	ToolGraphicsItem* ToolGraphicsItem::cast(QGraphicsItem* q)
	{
		return qgraphicsitem_cast<ToolGraphicsItem*>(q);
	}

	ConsoleWindow* Tool::console()
	{
	    if (mainWindow)
            return mainWindow->console();
	    return 0;
	}
	
	void Tool::addAction(const QIcon& icon, const QString& text, const QString& tooltip)
	{
		QAction * action = new QAction(icon,text,this);
		action->setToolTip(tooltip);
		actionsGroup.addAction(action);
	}
	
	void Tool::addGraphicsItem(ToolGraphicsItem * item)
	{
		if (item && !graphicsItems.contains(item))
			graphicsItems << item;
	}
}

