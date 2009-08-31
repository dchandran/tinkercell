/****************************************************************************

 Copyright (C) 2009 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYWRITE.TXT

Grid based multicell visual modeling interface
****************************************************************************/

#include "MutliCell.h"

using namespace Tinkercell;

namespace Multicell
{
	
	CellNode::CellNode() : NodeGraphicsItem()
	{
		NodeGraphicsReader reader;
		reader.readXML(this,":/images/Block.xml");
		normalize();
		
		for (int i=0; i < boundaryControlPoints.size(); ++i)
			if (boundaryControlPoints[i])
				delete boundaryControlPoints[i];
		
		boundaryControlPoints.clear();
	}
		
	/*! \brief copy constructed*/
	CellNode::CellNode(const CellNode& copy) : NodeGraphicsItem(copy)
	{
		for (int i=0; i < boundaryControlPoints.size(); ++i)
			if (boundaryControlPoints[i])
				delete boundaryControlPoints[i];
		
		boundaryControlPoints.clear();
	}

	MulticellInterface::MulticellInterface(): Tool(tr("Multicell interface"))
	{
		
	}

	bool MulticellInterface::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(mousePressed(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
					this,SLOT(mousePressed(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));
			
			connect(mainWindow,SIGNAL(mouseReleased(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
					this,SLOT(mouseReleased(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));
			
			connect(mainWindow,SIGNAL(mouseDoubleClicked(GraphicsScene *, QPointF, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers)),
					this,SLOT(mouseDoubleClicked(GraphicsScene *, QPointF, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers)));
					
			connect(mainWindow,SIGNAL(mouseDragged(GraphicsScene *, QPointF, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
					this,SLOT(mouseDragged(GraphicsScene *, QPointF, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));
					
			connect(mainWindow,SIGNAL(itemsMoved(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)),
					this,SLOT(itemsMoved(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)));
				
			connect(mainWindow,SIGNAL(mouseMoved(GraphicsScene *, QGraphicsItem*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)),
					this,SLOT(mouseMoved(GraphicsScene *, QGraphicsItem*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)));
				
			connect(mainWindow,SIGNAL(escapeSignal(const QWidget *)),
					this,SLOT(escapeSignal(const QWidget *)));
			
			return true;
		}
		return false;
	}

	void MulticellInterface::mousePressed(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers)
	{
		if (scene && button == Qt::LeftButton)
		{
			if (scene->items(point).isEmpty())
			{
				CellNode * cell = new CellNode;
				cell->setPos(point);
				scene->insert(tr("new cell created"),cell);
				
				QList<NodeGraphicsItem*> adjacentItems = cell->adjacentNodeItems();
				
				ItemHandle * handle = 0;
				for (int i=0; i < adjacentItems.size(); ++i)
				{
					handle = getHandle(adjacentItems[i]);
					if (handle)
						break;
				}
				
				if (!handle)
				{
					handle = new NodeHandle;
					handle->name = uniqueName();
				}
				
				cell->setHandle(handle);
			}
		}
	}
		
	void MulticellInterface::mouseReleased(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers)
	{
	}
	
	void MulticellInterface::mouseDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem * item, Qt::MouseButton, Qt::KeyboardModifiers modifiers)
	{
		ItemHandle * h = getHandle(item);
		if (h)
		{
			ConsoleWindow::message(h->name + tr(" information"));
		}
	}
	
	void MulticellInterface::mouseDragged(GraphicsScene * scene, QPointF from, QPointF to, Qt::MouseButton, Qt::KeyboardModifiers modifiers)
	{
	}
	
	void MulticellInterface::itemsMoved(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<QPointF>& distance, Qt::KeyboardModifiers modifiers)
	{
	}
	
	void MulticellInterface::mouseMoved(GraphicsScene * scene, QGraphicsItem* item, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers, QList<QGraphicsItem*>&)
	{
		if (scene && button == Qt::LeftButton)
		{
			if (!item)
			{
				CellNode * cell = new CellNode;
				cell->setPos(point);
				scene->insert(tr("new cell created"),cell);
				
				QList<NodeGraphicsItem*> adjacentItems = cell->adjacentNodeItems();
				
				ItemHandle * handle = 0;
				for (int i=0; i < adjacentItems.size(); ++i)
				{
					handle = getHandle(adjacentItems[i]);
					if (handle)
						break;
				}
				
				if (!handle)
				{
					handle = new NodeHandle;
					handle->name = uniqueName();
				}
				
				cell->setHandle(handle);
			}
		}
		
	}
		
	void MulticellInterface::escapeSignal(const QWidget * sender)
	{
	}

	QString MulticellInterface::uniqueName()
	{
		if (!currentWindow() || !currentWindow()->symbolsTable) return tr("x");
		
		QStringList list(currentWindow()->symbolsTable.fullNames.keys());
		
		int i = 1;
		QString name = tr("cell") + QString::number(i);
		while (list.contains(name))
		{
			++i;
			name = tr("cell") + QString::number(i);
		}
		return name;
	}

}


