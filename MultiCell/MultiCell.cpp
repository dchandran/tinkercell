/****************************************************************************

 Copyright (C) 2009 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYWRITE.TXT

Grid based multicell visual modeling interface
****************************************************************************/

#include "CellTypeSelector.h"
#include "MultiCell.h"

using namespace Tinkercell;

namespace Multicell
{

	CellNode::CellNode() : NodeGraphicsItem()
	{
		NodeGraphicsReader reader;
		reader.readXml(this,":/images/Block.xml");
		normalize();

		scale(GraphicsScene::GRID/boundingRect().width(),GraphicsScene::GRID/boundingRect().height());
	}

	/*! \brief copy constructed*/
	CellNode::CellNode(const CellNode& copy) : NodeGraphicsItem(copy)
	{
		boundaryControlPoints.clear();
	}

	NodeGraphicsItem * CellNode::clone() const
	{
		return new CellNode(*this);
	}

	void CellNode::setCentralColor(const QColor& color)
	{
		if (shapes.size() > 0 && shapes[0])
			shapes[0]->setBrush( shapes[0]->defaultBrush = QBrush(color) );
	}

	MulticellInterface::MulticellInterface(): Tool(tr("Multicell interface"))
	{
		cellSelector = 0;
		currentColor = QColor();
		currentFamily = 0;
		itemOnTopOf = 0;
	}

	void MulticellInterface::cellTypeSelected(NodeFamily* family,const QColor& color)
	{
		currentColor = color;
		currentFamily = family;

		GraphicsScene * scene = currentScene();
		if (scene)
		{
			scene->useDefaultBehavior = (family == 0);
		}

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

			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
					this,SLOT(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(escapeSignal(const QWidget *)),
					this,SLOT(escapeSignal(const QWidget *)));

			cellSelector = new CellTypeSelector(mainWindow);

			connect(cellSelector,SIGNAL(cellTypeSelected(NodeFamily*,const QColor&)),this,SLOT(cellTypeSelected(NodeFamily*,const QColor&)));

			setWindowTitle(tr("Cell types"));
			mainWindow->addToolWindow(cellSelector,MainWindow::DockWidget, Qt::LeftDockWidgetArea, Qt::LeftDockWidgetArea);

			NodeFamily * family;

			currentFamily = family = new NodeFamily;
			currentColor = QColor(50,250,50,255);
			family->name = tr("green cells");
			cellSelector->addCellType(family,QColor(50,250,50,255));

			family = new NodeFamily;
			family->name = tr("red cells");
			cellSelector->addCellType(family,QColor(250,50,50,255));

			family = new NodeFamily;
			family->name = tr("blue cells");
			cellSelector->addCellType(family,QColor(50,50,250,255));

			return true;
		}
		return false;
	}

	void MulticellInterface::mousePressed(GraphicsScene * scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers)
	{
		if (scene && currentFamily && button == Qt::LeftButton)
		{
			QPointF p = point;
			int gridSz = scene->gridSize();

			//get the grid point
			p.rx() = gridSz * (int)(p.rx()/gridSz);
			p.ry() = gridSz * (int)(p.ry()/gridSz);

			QRectF rect(p.rx(),p.ry(), p.rx() + 20, p.ry() + 20);

			if (scene->items(rect).isEmpty())
			{
				CellNode * cell = new CellNode;
				cell->setCentralColor(currentColor);
				cell->setPos(point);
				scene->insert(tr("new cell created"),cell);
				cell->setBoundingBoxVisible(false);

				QList<NodeGraphicsItem*> adjacentItems = cell->nodesAdjacent();

				ItemHandle * handle = 0;
				for (int i=0; i < adjacentItems.size(); ++i)
				{
					handle = getHandle(adjacentItems[i]);
					if (handle && handle->family() == currentFamily)
						break;
					else
						handle = 0;
				}

				if (!handle)
				{
					NodeHandle * nodeHandle = new NodeHandle;
					nodeHandle->setFamily(currentFamily);
					nodeHandle->name = uniqueName();
					handle = nodeHandle;
				}

				cell->setHandle(handle);
			}
		}
	}

	void MulticellInterface::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF, Qt::KeyboardModifiers modifiers)
	{
		if (!scene) return;

		for (int i=0; i < selectedItems.size(); ++i)
		{
			if (!items.contains(selectedItems[i]))
			{
				selectedItems[i]->resetPen();
				selectedItems[i]->resetBrush();
			}
		}

		QList<NodeGraphicsItem*> oldList = selectedItems;
		selectedItems.clear();

		NodeGraphicsItem * node;

		for (int i=0; i < items.size(); ++i)
		{
			node = qgraphicsitem_cast<NodeGraphicsItem*>(items[i]);
			if (node)
			{
				selectedItems << node;
				if (!oldList.contains(node))
				{
					node->setPen( QPen(QColor(255,10,10,255),5.0,Qt::DotLine) );
					node->setBrush( QBrush(QColor(255,10,10,50)) );
				}
			}
		}


		if (modifiers != 0) return;

		ItemHandle * handle;

		for (int i=0; i < items.size(); ++i)
		{
			handle = getHandle(items[i]);
			if (handle)
			{
				scene->moving() += handle->graphicsItems;
			}
		}

	}

	void MulticellInterface::mouseReleased(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers)
	{
	}

	void MulticellInterface::mouseDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem * item, Qt::MouseButton, Qt::KeyboardModifiers modifiers)
	{
		if (!scene || currentFamily) return;

		ItemHandle * h = getHandle(item);
		if (h && h->family())
		{
			if (console())
                console()->message(h->name + tr("   ") + h->family()->name + tr("  ..."));
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
		if (!scene) return;
		
		if (currentFamily && button == Qt::LeftButton)
		{
			QPointF p = point;
			int gridSz = scene->gridSize();

			//get the grid point
			p.rx() = gridSz * (int)(p.rx()/gridSz);
			p.ry() = gridSz * (int)(p.ry()/gridSz);

			QRectF rect(p.rx(),p.ry(), p.rx(), p.ry() + 20);

			if (!item && scene->items(rect).isEmpty())
			{
				CellNode * cell = new CellNode;
				cell->setCentralColor(currentColor);
				cell->setPos(point);
				scene->insert(tr("new cell created"),cell);
				cell->setBoundingBoxVisible(false);

				QList<NodeGraphicsItem*> adjacentItems = cell->nodesAdjacent();

				ItemHandle * handle = 0;
				for (int i=0; i < adjacentItems.size(); ++i)
				{
					handle = getHandle(adjacentItems[i]);
					if (handle && handle->family() == currentFamily)
						break;
					else
						handle = 0;
				}

				if (!handle)
				{
					handle = new NodeHandle;
					handle->name = uniqueName();
				}

				cell->setHandle(handle);
			}
		}
		else
		{
			NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(item);
			if (node != itemOnTopOf)
			{
				ItemHandle * handle;
				if (itemOnTopOf) //reset the colors
				{
					handle = itemOnTopOf->handle();

					if (handle)
					{
						QList<QGraphicsItem*> graphicsItems = handle->graphicsItems;

						for (int i=0; i < graphicsItems.size(); ++i)
						{
							NodeGraphicsItem * n = qgraphicsitem_cast<NodeGraphicsItem*>(graphicsItems[i]);
							if (n)
								n->resetBrush();
						}
					}
				}

				itemOnTopOf = node;

				if (node)
				{
					handle = itemOnTopOf->handle();

					if (handle)  //new colors
					{
						QList<QGraphicsItem*> graphicsItems = handle->graphicsItems;

						for (int i=0; i < graphicsItems.size(); ++i)
						{
							NodeGraphicsItem * n = qgraphicsitem_cast<NodeGraphicsItem*>(graphicsItems[i]);
							if (n)
								n->setBrush( QBrush(QColor(25,25,25,100)) );
						}
					}
				}
			}
		}

	}

	void MulticellInterface::escapeSignal(const QWidget * sender)
	{
	}

	QString MulticellInterface::uniqueName()
	{
		if (!currentNetwork()) return QString("x");

		QStringList list(currentNetwork()->symbolsTable.uniqueItems.keys());
		list << currentNetwork()->symbolsTable.uniqueData.keys();

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


