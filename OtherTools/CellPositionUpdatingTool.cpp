/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool updates the x,y attribute for "Cell" items when they are moved

****************************************************************************/

#include "Core/ItemHandle.h"
#include "Core/GraphicsScene.h"
#include "Core/MainWindow.h"
#include "Core/NodeGraphicsItem.h"
#include "Core/ConnectionGraphicsItem.h"
#include "Core/TextGraphicsItem.h"
#include "Core/OutputWindow.h"
#include "OtherTools/CellPositionUpdatingTool.h"

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
			connect(mainWindow,SIGNAL(itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)),
						  this, SLOT(itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)));
		}
		return true;
	}
	
	void CellPositionUpdateTool::itemsMoved(GraphicsScene* scene, const QList<QGraphicsItem*>& items, const QList<QPointF>& distance, Qt::KeyboardModifiers)
	{
		if (!scene) return;
		
		ItemHandle * handle;
		
		QList<ItemHandle*> handles;
		QList<DataTable<qreal>*> newTables;
		
		for (int i=0; i < items.size(); ++i)
			if (qgraphicsitem_cast<NodeGraphicsItem*>(items[i]) &&
				(handle = getHandle(items[i])) && 
				handle->isA(tr("Cell")) && 
				handle->hasNumericalData(tr("Numerical Attributes")))
			{
				DataTable<qreal> * dat = new DataTable<qreal>(handle->data->numericalData[tr("Numerical Attributes")]);
				QPointF pos = items[i]->scenePos();
				
				dat->value(tr("x"),0) = pos.x();
				dat->value(tr("y"),0) = pos.y();
				
				handles << handle;
				newTables << dat;
			}
			
		if (handles.size() > 0)
		{
			scene->changeData(handles,tr("Numerical Attributes"),newTables);
			for (int i=0; i < newTables.size(); ++i)
				delete newTables[i];
		}
	}
}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::CellPositionUpdateTool * tool = new Tinkercell::CellPositionUpdateTool;
	main->addTool(tool);

}
