/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool updates the x,y attribute for "Cell" items when they are moved

****************************************************************************/

#include "ItemHandle.h"
#include "NetworkHandle.h"
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "CellPositionUpdatingTool.h"

namespace Tinkercell
{
	CellPositionUpdateTool::CellPositionUpdateTool() : 
		Tool(tr("Cell X,Y Position Tool"))
	{
	}

	bool CellPositionUpdateTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow != 0)
		{	
			connect(mainWindow,SIGNAL(itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<QPointF>&)),
						  this, SLOT(itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<QPointF>&)));

			connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
						  this, SLOT(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));
		}
		return true;
	}

	void CellPositionUpdateTool::itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>& handles)
	{
		itemsMoved(scene, items, QList<QPointF>());
	}
	
	void CellPositionUpdateTool::itemsMoved(GraphicsScene* scene, const QList<QGraphicsItem*>& items, const QList<QPointF>& )
	{
		if (!scene) return;
		
		ItemHandle * handle;
		
		QList<ItemHandle*> handles;
		QList<NumericalDataTable*> newTables;
		
		for (int i=0; i < items.size(); ++i)
			if (NodeGraphicsItem::cast(items[i]) &&
				items[i]->scene() == scene &&
				(handle = getHandle(items[i])) && 
				handle->isA(tr("Cell")))
			{
				NumericalDataTable * dat = new NumericalDataTable(handle->numericalDataTable(tr("Position")));
				QPointF pos = items[i]->scenePos();
				
				dat->value(tr("x"),tr("value")) = pos.x();
				dat->value(tr("y"),tr("value")) = pos.y();
				
				handles << handle;
				newTables << dat;
			}
			
		if (handles.size() > 0)
		{
			scene->network->changeData(tr("cell position changed"),handles,tr("Positions"),newTables);
			for (int i=0; i < newTables.size(); ++i)
				delete newTables[i];
		}
	}
}

/*
extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::CellPositionUpdateTool * tool = new Tinkercell::CellPositionUpdateTool;
	main->addTool(tool);

}*/

