/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines the GraphicsScene class where all the drawing takes place.
In addition to drawing , the GraphicsScene provides serveral signals and functions
that is useful for plugins, eg. move, insert, delete, changeData, etc.

****************************************************************************/
#include "DataTable.h"
#include "NetworkHandle.h"
#include "NetworkWindow.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "UndoCommands.h"
#include "ConsoleWindow.h"
#include "CloneItems.h"
#include "SymbolsTable.h"
#include "HistoryWindow.h"
#include "GraphicsView.h"
#include "GraphicsScene.h"
#include <QRegExp>

namespace Tinkercell
{
	bool GraphicsScene::USE_DEFAULT_BEHAVIOR = true;

	int GraphicsScene::GRID = 0;

	QPen GraphicsScene::SelectionRectanglePen = Qt::NoPen;

	QBrush GraphicsScene::SelectionRectangleBrush = QBrush(QColor(0,132,255,50));

	QBrush GraphicsScene::BackgroundBrush = Qt::NoBrush; //QBrush(Qt::lightGray,Qt::CrossPattern);

	QPen GraphicsScene::GridPen = QPen(Qt::lightGray,2);

	QBrush GraphicsScene::ForegroundBrush = Qt::NoBrush; //QBrush(Qt::lightGray,Qt::CrossPattern);
	
	QBrush GraphicsScene::ToolTipBackgroundBrush = QBrush(QColor(36,28,28,125));
	
	QBrush GraphicsScene::ToolTipTextBrush = QBrush(QColor(255,255,255,255));

	qreal GraphicsScene::MIN_DRAG_DISTANCE = 2.0;

	/*! \brief Returns the currently visible window
	* \param void
	* \return rectangle*/
	QRectF GraphicsScene::viewport() const
	{
		QGraphicsView * view = 0;
		
		QList<QGraphicsView*> list = views();
	
		if (list.size() > 0)
			view = list[0];

		if (view)
		{
			QRect rect = view->viewport()->rect();
			return QRectF(view->mapToScene(rect.topLeft()),view->mapToScene(rect.bottomRight()));
		}

		return QRect();
	}

	/*! \brief Returns the point where mouse was clicked last
	* Precondition: None
	* Postcondition: None
	* \param void
	* \return ref to last clicked point*/
	QPointF& GraphicsScene::lastPoint()
	{
		return clickedPoint;
	}

	/*! \brief Returns the point where mouse was clicked last
	* Precondition: None
	* Postcondition: None
	* \param void
	* \return ref to last clicked point*/
	QPoint& GraphicsScene::lastScreenPoint()
	{
		return clickedScreenPoint;
	}

	/*! \brief Returns the list of pointers to items that are currently selected
	* Precondition: None
	* Postcondition: None
	* \param void
	* \return list of pointers to selected items*/
	QList<QGraphicsItem*>& GraphicsScene::selected()
	{
		return selectedItems;
	}


	/*! \brief Returns a rectangle that includes all the selected items
	* Precondition: None
	* Postcondition: None
	* \param void
	* \return bounding rect for selected items*/
	QRectF GraphicsScene::selectedRect()
	{
		QRectF rect;
		for (int i=0; i < selectedItems.size();	++i)
			if (selectedItems[i] != 0)
				rect = rect.united(selectedItems[i]->topLevelItem()->sceneBoundingRect());
		return rect;
	}
	/*! \brief Returns the list of pointers to items that are currently being moved
	* Precondition: None
	* Postcondition: None
	* \param void
	* \return list of pointers to moving items*/
	QList<QGraphicsItem*>& GraphicsScene::moving()
	{
		return movingItems;
	}

	/*! \brief Constructor: sets 10000x10000 scene */
	GraphicsScene::GraphicsScene(NetworkHandle * net) : QGraphicsScene(net), networkWindow(0), network(net)
	{
		gridSz = GRID;
		mouseDown = false;
		contextMenuJustActivated = false;
		useDefaultBehavior = USE_DEFAULT_BEHAVIOR;
		setFocus();
		//setItemIndexMethod(NoIndex);

		contextItemsMenu = 0;
		contextScreenMenu = 0;
		setSceneRect(0,0,10000,10000);

		lastZ = 1.0;

		setBackgroundBrush(BackgroundBrush);
		setForegroundBrush(ForegroundBrush);
		selectionRect.setBrush(SelectionRectangleBrush);
		selectionRect.setPen(SelectionRectanglePen);

		addItem(&selectionRect);

		selectionRect.setVisible(false);
		movingItemsGroup = 0;
	}
	/*! \brief destructor */
	GraphicsScene::~GraphicsScene()
	{
		if (!toolTips.isEmpty()) hideToolTips();
		visibleTools.clear();
		selectedItems.clear();
		movingItems.clear();
		if (movingItemsGroup)
		{
			destroyItemGroup(movingItemsGroup);
			movingItemsGroup = 0;
		}
		select(0);
		if (GraphicsScene::copiedFromScene == this)
			GraphicsScene::copiedFromScene = 0;
		
		QList<QGraphicsItem *> allitems = items();
		for (int i=0; i < allitems.size(); ++i)
			removeItem(allitems[i]);
		
		/*	
		QList<ItemHandle*> handles = network->handles();
		
		for (int i=0; i < handles.size(); ++i)
		{
			QList<QGraphicsItem*> & items = handles[i]->graphicsItems;
			for (int j=0; j < items.size(); ++j)
				if (items[j]->scene() == this)
					removeItem(items[j]);
		}
		
		removeItem(&selectionRect);
		QList<QGraphicsItem *> allitems1 = items();
		QList<QGraphicsItem *> allitems2;

		for (int i=0; i < allitems1.size(); ++i)
		{
			if (allitems1[i] && ConnectionGraphicsItem::cast(allitems1[i]) && !allitems1[i]->parentItem())
			{
				allitems2 << allitems1[i];
			}
		}

		qDeleteAll(allitems2);
		allitems1 = items();
		allitems2.clear();

		for (int i=0; i < allitems1.size(); ++i)
		{
			if (allitems1[i] && NodeGraphicsItem::cast(allitems1[i]) && !allitems1[i]->parentItem())
			{
				allitems2 << allitems1[i];
			}
		}

		qDeleteAll(allitems2);
		allitems1 = items();
		allitems2.clear();

		for (int i=0; i < allitems1.size(); ++i)
		{
			if (allitems1[i] && TextGraphicsItem::cast(allitems1[i]) && !allitems1[i]->parentItem())
			{
				allitems2 << allitems1[i];
			}
		}

		qDeleteAll(allitems2);
		allitems1 = items();
		allitems2.clear();

		for (int i=0; i < allitems1.size(); ++i)
		{
			if (allitems1[i] && !allitems1[i]->parentItem() && !ToolGraphicsItem::cast(allitems1[i]))
			{
				allitems2 << allitems1[i];
			}
		}

		qDeleteAll(allitems2);*/
	}
	/*! \brief Clear all selection and moving items list
	* Precondition: None
	* Postcondition: None
	* \return void*/
	void GraphicsScene::clearSelection()
	{
		if (!toolTips.isEmpty()) hideToolTips();
		selectedItems.clear();
		if (movingItemsGroup)
		{
			destroyItemGroup(movingItemsGroup);
			movingItemsGroup = 0;
		}
		movingItems.clear();
		emit itemsSelected(this,selectedItems,QPointF(),Qt::NoModifier);
		hideGraphicalTools();
	}
	/*! \brief Add a new item to the scene
	* Precondition: None
	* Postcondition: None
	* \param Tinkercell object
	* \return void*/
	void GraphicsScene::addItem(QGraphicsItem * item)
	{
		if (!item || item->scene() == this) return;
		QGraphicsScene::addItem(item);

		item->setVisible( item->isVisible() );

		if (item->zValue())
		{
			item->setZValue(item->zValue());
			if (item->zValue() > lastZ)
				lastZ = item->zValue();
		}
		else
		{
			item->setZValue(lastZ);
			lastZ += 1.0;
		}

		QGraphicsItem * item2 = getGraphicsItem(item);

		if (!item2) return;

		QPointF p = item2->scenePos();
		if (p.rx() > this->width() || p.ry() > this->height())
		{
			setSceneRect(0,0,this->width()*2, this->height()*2);
		}
		if (p.rx() < 0 || p.ry() < 0)
		{
			QPointF dp;
			if (p.rx() < 0) dp.rx() = - p.rx() + item2->boundingRect().width()/2.0 ;
			if (p.ry() < 0) dp.ry() = - p.ry() + item2->boundingRect().height()/2.0 ;

			move(item2,dp);
		}

		snapToGrid(item2);
	}
	/*! \brief top Z value
	* Precondition: None
	* Postcondition: None
	* \return double*/
	qreal GraphicsScene::ZValue()
	{
		return lastZ;
	}
	/*! \brief when mouse is pressed, the item at the position is added to selected list and moving list
	* Precondition: None
	* Postcondition: None
	* \param mouse event
	* \return void*/
	void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
	{
		if (contextMenuJustActivated)
		{
			contextMenuJustActivated = false;
			deselect();
			return;
		}
		
		if (!toolTips.isEmpty()) hideToolTips();

		clickedScreenPoint = mouseEvent->screenPos();
		clickedPoint = mouseEvent->scenePos();
		clickedButton = mouseEvent->button();
		mouseDown = true;

		QGraphicsItem * item = 0;
		ToolGraphicsItem * gitem = 0;

		QGraphicsItem * p = itemAt(clickedPoint);

		if (p)
			gitem = ToolGraphicsItem::cast(p->topLevelItem());

		if (!gitem)
		{
			p = 0;//getGraphicsItem(p);
			//if (!p || p->sceneBoundingRect().width() > 100 || p->sceneBoundingRect().height() > 100)
			{
				QList<QGraphicsItem*> ps = items(QRectF(clickedPoint.rx()-10.0,clickedPoint.ry()-10.0,20.0,20.0));
				if (!ps.isEmpty())
				{
					for (int i=0; i < ps.size(); ++i)
					{
						if (i > 0 && p && ps[i] && (ps[i]->sceneBoundingRect().width() > 80 || ps[i]->sceneBoundingRect().height() > 80))
							break;

						p = getGraphicsItem(ps[i]);

						if (p && !TextGraphicsItem::cast(p))
							break;
					}
				}
			}

			item = p;
		}

		if (movingItemsGroup)
		{
			destroyItemGroup(movingItemsGroup);
			movingItemsGroup = 0;
		}
		movingItems.clear();

		if (gitem && useDefaultBehavior)
		{
			if (mouseEvent->button() == Qt::LeftButton)
			{
				if (gitem->tool)
				{
					gitem->select();
					mouseDown = false;
				}
				//emit toolSelected(this,gitem,clickedPoint,mouseEvent->modifiers());
			}
		}
		else
			if (item && useDefaultBehavior)
			{
				if (mouseEvent->button())// == Qt::LeftButton)
				{
					if (!selectedItems.contains(item) && !selectedItems.contains(item->parentItem()) && !selectedItems.contains(item->topLevelItem()))
					{
						if (!(mouseEvent->modifiers() == Qt::ShiftModifier || mouseEvent->modifiers() == Qt::ControlModifier))
							selectedItems.clear();

						selectedItems.append(item);
					}
					else
					{
						if (mouseEvent->modifiers() == Qt::ControlModifier)// || mouseEvent->modifiers() == Qt::ShiftModifier)
						{
							if (item->parentItem() != 0 && item->parentItem() != item)
							{
								selectedItems.removeAll(item->parentItem());
								QList<QGraphicsItem*> childs = item->parentItem()->childItems();
								for (int i = 0; i < childs.size(); ++i)
									selectedItems.removeAll(childs[i]);
							}
							else
								selectedItems.removeAll(item);
						}
					}
				}

				if (selectedItems.size() > 0)
				{
					ArrowHeadItem * arrow;
					QGraphicsItem * topLevelItem;
					for (QList<QGraphicsItem*>::const_iterator i = selectedItems.constBegin(); i != selectedItems.constEnd(); ++i)
						if (*i && *i != &selectionRect)
							if (topLevelItem = (*i)->topLevelItem())
							{
								arrow = ArrowHeadItem::cast(topLevelItem);
								if (!arrow || selectedItems.contains(arrow->connectionItem))
									movingItems.append(topLevelItem);
							}

						if (mouseEvent->button())// == Qt::LeftButton)
						{
							emit itemsSelected(this, selectedItems,clickedPoint,mouseEvent->modifiers());
							showGraphicalTools();
						}
						
						selectConnections(clickedPoint);
						
						if (movingItems.size() > 0)
						{
							movingItemsGroup = createItemGroup(movingItems);
							movingItemsGroup->setZValue(lastZ);
						}
				}
			}
			else
			{
				if (useDefaultBehavior && !(mouseEvent->modifiers() == Qt::ShiftModifier || mouseEvent->modifiers() == Qt::ControlModifier))
				{
					selectedItems.clear();
				}

				emit mousePressed(this, clickedPoint, clickedButton, mouseEvent->modifiers());
				if (mouseEvent->button() == Qt::LeftButton)
				{
					emit itemsSelected(this, selectedItems,clickedPoint,mouseEvent->modifiers());
					showGraphicalTools();
				}

				selectionRect.setZValue(lastZ);
			}
		if (clickedButton == Qt::RightButton)
		{
			emit sceneRightClick(this, item, clickedPoint, mouseEvent->modifiers());
		}
		QGraphicsScene::mousePressEvent(mouseEvent);
	}

	/*! \brief when mouse is moving, all items in moving list are moved
	* Precondition: None
	* Postcondition: None
	* \param mouse event
	* \return void*/
	void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
	{
		if (contextMenuJustActivated)
		{
			contextMenuJustActivated = false;
			deselect();
			return;
		}

		QPointF point1 = mouseEvent->scenePos(), point0 = mouseEvent->lastScenePos();
		QPointF change = QPointF(point1.x()-point0.x(),point1.y()-point0.y());
		
		//if (!toolTips.isEmpty() && ((change.x()*change.x() + change.y()*change.y()) > MIN_DRAG_DISTANCE*20.0))
			//hideToolTips();

		if (useDefaultBehavior)
		{
			if (movingItems.size() > 0 && movingItemsGroup != 0)
			{
				movingItemsGroup->moveBy(change.x(),change.y());
			}
			else
			{
				if (mouseDown && clickedButton == Qt::LeftButton)
					//if (mouseEvent->modifiers() == 0 || selectionRect.isVisible())
				{
					if (!selectionRect.isVisible())
						selectionRect.setVisible(true);
					qreal x,y,w,h;
					if (clickedPoint.rx() > point1.rx())
					{
						x = point1.rx();
						w = clickedPoint.rx() - point1.rx();
					}
					else
					{
						x = clickedPoint.rx();
						w = point1.rx() - clickedPoint.rx();
					}
					if (clickedPoint.ry() > point1.ry())
					{
						y = point1.ry();
						h = clickedPoint.ry() - point1.ry();
					}
					else
					{
						y = clickedPoint.ry();
						h = point1.ry() - clickedPoint.ry();
					}
					selectionRect.setRect(x,y,w,h);
				}
			}
		}

		QList<QGraphicsItem*> itemList = items(point1);
		QGraphicsItem * item = 0;

		for (int i=0; i < itemList.size(); ++i)
			if (itemList[i]->topLevelItem() != movingItemsGroup)
			{
				item = itemList[i]->topLevelItem();
				break;
			}
		
		MoveCommand::refreshAllConnectionIn(movingItems);
			
		emit mouseMoved(this, item, point1, clickedButton, mouseEvent->modifiers(), movingItems);

		if (item)
			emit mouseOnTopOf(this, item, point1, mouseEvent->modifiers(), movingItems);
		
		QGraphicsScene::mouseMoveEvent(mouseEvent);
	}
	/*! \brief when mouse is released, moving list is cleared
	* Precondition: None
	* Postcondition: None
	* \param mouse event
	* \return void*/
	void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
	{
		if (contextMenuJustActivated)
		{
			contextMenuJustActivated = false;
			deselect();
			return;
		}

		mouseDown = false;

		QPointF point1 = mouseEvent->scenePos(), point0 = clickedPoint;
		QPointF change = QPointF(point1.x()-point0.x(),point1.y()-point0.y());

		if (useDefaultBehavior && movingItems.size() > 0 && movingItemsGroup)
		{
			if (movingItemsGroup)
			{
				movingItemsGroup->moveBy(-change.x(),-change.y());
				destroyItemGroup(movingItemsGroup);
				movingItemsGroup = 0;
				if (gridSz > 0)
				{
					change.rx() = gridSz * (int)(change.rx() / (double)gridSz + 0.5);
					change.ry() = gridSz * (int)(change.ry() / (double)gridSz + 0.5);
				}
			}

			if ((change.x()*change.x() + change.y()*change.y()) > MIN_DRAG_DISTANCE/2.0)
			{
				move(movingItems,change);
			}
			
			movingItems.clear();
		}
		else
		{
			if (useDefaultBehavior && selectionRect.isVisible())
			{
				QRectF rect = selectionRect.rect();
				selectionRect.setVisible(false);

				qreal x1 = std::min(rect.left(),rect.right()),
					x2 = std::max(rect.left(),rect.right()),
					y1 = std::min(rect.bottom(),rect.top()),
					y2 = std::max(rect.bottom(),rect.top());

				QList<QGraphicsItem*> itemsInRect = items(QRectF(x1,y1,x2-x1,y2-y1),Qt::ContainsItemShape);
				QList<QGraphicsItem*> itemsIntersected = items(QRectF(x1,y1,x2-x1,y2-y1),Qt::IntersectsItemShape);
				for (int i=0; i < itemsIntersected.size(); ++i)
					if (ConnectionGraphicsItem::cast(itemsIntersected[i]))
						itemsInRect.append(itemsIntersected[i]);

				QGraphicsItem* item = 0;
				for (int i=0; i < itemsInRect.size(); ++i)
					if (itemsInRect[i] != &selectionRect &&
						(item = getGraphicsItem(itemsInRect[i]))
						)
					{
						if (item && itemsInRect[i] == item)
							if (!selectedItems.contains(item))
								selectedItems.append(item);
							else
								if (mouseEvent->modifiers() == Qt::ShiftModifier)
									selectedItems.removeAll(item);
					}
				emit itemsSelected(this, selectedItems,point1,mouseEvent->modifiers());
				showGraphicalTools();
			}
			if ((change.x()*change.x() + change.y()*change.y()) > MIN_DRAG_DISTANCE)
				emit mouseDragged(this, point0, point1, clickedButton, mouseEvent->modifiers());
			else
				emit mouseReleased(this, clickedPoint, clickedButton, mouseEvent->modifiers());
		}
		clickedButton = Qt::NoButton;
		selectionRect.setVisible(false);
		QGraphicsScene::mouseReleaseEvent(mouseEvent);
	}
	/*! \brief emits signal when mouse is double clicked
	* Precondition: None
	* Postcondition: None
	* \param mouse event
	* \return void*/
	void GraphicsScene::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * mouseEvent )
	{
		if (!toolTips.isEmpty()) hideToolTips();

		QPointF point1 = mouseEvent->scenePos();
		QGraphicsItem * p = itemAt(clickedPoint);
		if (!p || p->sceneBoundingRect().width() > 500 || p->sceneBoundingRect().height() > 500)
		{
			QList<QGraphicsItem*> ps = items(QRectF(clickedPoint.rx()-20.0,clickedPoint.ry()-20.0,40.0,40.0));
			if (!ps.isEmpty()) p = ps[0];
		}
		emit mouseDoubleClicked(this, point1, getGraphicsItem(p) , mouseEvent->button(), mouseEvent->modifiers());
		QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
	}
	
	/*! \brief context menu for the scene
	* Precondition: None
	* Postcondition: None
	* \param context menu event
	* \return void*/
	void GraphicsScene::contextMenuEvent ( QGraphicsSceneContextMenuEvent * mouseEvent )
	{
		if (!toolTips.isEmpty()) hideToolTips();

		if (useDefaultBehavior)
		{
			if (selectedItems.size() > 0)
			{
				populateContextMenu();
				if (contextItemsMenu)
				{
					contextMenuJustActivated = true;
					contextItemsMenu->exec(mouseEvent->screenPos());
				}
			}
			else
			{
				if (contextScreenMenu)
				{
					contextScreenMenu->exec(mouseEvent->screenPos());
				}
			}
		}
		else
		{
			emit escapeSignal(network->mainWindow->currentNetworkWindow);
		}
	}
	/*! \brief zoom
	* \param scale factor
	* \return void*/
	void GraphicsScene::zoom(qreal scaleFactor)
	{
		if (!toolTips.isEmpty()) hideToolTips();

		QList<QGraphicsView*> list = views();
		
		if (!list.isEmpty() && list[0])
		{
			QGraphicsView * view = list[0];
			qreal factor = view->matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
			if (!(factor < 0.07 || factor > 100))
				view->scale(scaleFactor, scaleFactor);
		}
		scaleGraphicalTools();
	}
	/*! \brief zoom in
	* \param scale factor
	* \return void*/
	void GraphicsScene::zoomIn()
	{
		zoom(1.2);
	}
	/*! \brief zoom out
	* \param scale factor
	* \return void*/
	void GraphicsScene::zoomOut()
	{
		zoom(0.8);
	}

	/*! \brief place center at the point
	* Precondition: None
	* Postcondition: None
	* \param point
	* \return void*/
	void GraphicsScene::centerOn(const QPointF& point)  const
	{
		QList<QGraphicsView*> list = views();
		
		for (int i=0; i < list.size(); ++i)
		{
			QGraphicsView * view = list[i];
			if (view)
				view->centerOn(point);
		}
	}
	/*! \brief when key is pressed
	* Precondition: None
	* Postcondition: None
	* \param key event
	* \return void*/
	void GraphicsScene::keyPressEvent (QKeyEvent * keyEvent)
	{
		if (!toolTips.isEmpty()) hideToolTips();

		if (!keyEvent) return;
		keyEvent->setAccepted(false);

		emit keyPressed(this, keyEvent);

		if (keyEvent->isAccepted())
		{
			QGraphicsScene::keyPressEvent(keyEvent);
			return;
		}

		if (keyEvent->matches(QKeySequence::Undo))
		{
			if (network)
				network->undo();
			keyEvent->accept();
			return;
		}

		if (keyEvent->matches(QKeySequence::Redo))
		{
			if (network)
				network->redo();
			keyEvent->accept();
			return;
		}

		if (keyEvent->matches(QKeySequence::Copy))
		{
			copy();
			keyEvent->accept();
			return;
		}

		if (keyEvent->matches(QKeySequence::Cut))
		{
			cut();
			keyEvent->accept();
			return;
		}

		if (keyEvent->matches(QKeySequence::Paste))
		{
			paste();
			keyEvent->accept();
			return;
		}

		if (keyEvent->matches(QKeySequence::SelectAll))
		{
			selectAll();
			keyEvent->accept();
			return;
		}

		int key = keyEvent->key();

		if (key == Qt::Key_Escape || key == Qt::Key_Space)
		{
			emit escapeSignal(network->mainWindow->currentNetworkWindow);
			keyEvent->accept();
		}

		if (useDefaultBehavior)
		{
			if ((key == Qt::Key_Plus || key == Qt::Key_Equal || key == Qt::Key_Underscore || key == Qt::Key_Minus)
				&& (selectedItems.isEmpty() || (keyEvent->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier))))
			{
				if (key == Qt::Key_Plus || key == Qt::Key_Equal)
				{
					zoom(qreal(1.2));
					keyEvent->accept();
				}
				else
					if (key == Qt::Key_Underscore || key == Qt::Key_Minus)
					{
						zoom(1/qreal(1.2));
						keyEvent->accept();
					}
			}
			else
				if (key == Qt::Key_Delete || key == Qt::Key_Backspace)
				{
					if (selectedItems.size() > 0)
					{
						if (movingItemsGroup)
						{
							destroyItemGroup(movingItemsGroup);
							movingItemsGroup = 0;
						}
						movingItems.clear();
						remove(tr("items deleted"), selectedItems);
						selectedItems.clear();
						keyEvent->accept();
					}
				}
				else
				{
					if (!movingItems.isEmpty() &&
						(key == Qt::Key_Up || key == Qt::Key_Down ||
						key == Qt::Key_Left || key == Qt::Key_Right))
					{
						qreal dx = 1;
						if (keyEvent->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier))
							dx = 20;
						QPointF change;
						if (keyEvent->key() == Qt::Key_Up) change = QPointF(0,-dx);
						if (keyEvent->key() == Qt::Key_Down) change = QPointF(0,dx);
						if (keyEvent->key() == Qt::Key_Left) change = QPointF(-dx,0);
						if (keyEvent->key() == Qt::Key_Right) change = QPointF(dx,0);

						if (gridSz > 0)
						{
							change.rx() = gridSz * (int)(change.rx() / (double)gridSz + 0.5);
							change.ry() = gridSz * (int)(change.ry() / (double)gridSz + 0.5);
						}

						move(movingItems,change);

						keyEvent->accept();
					}
					else
						QGraphicsScene::keyPressEvent(keyEvent);
				}
		}
	}
	/*! \brief when key is released
	* Precondition: None
	* Postcondition: None
	* \param key event
	* \return void*/
	void GraphicsScene::keyReleaseEvent (QKeyEvent * keyEvent)
	{
		emit keyReleased(this, keyEvent);
		QGraphicsScene::keyReleaseEvent(keyEvent);
	}

	/*! \brief select items*/
	void GraphicsScene::select(QGraphicsItem* item)
	{
		if (item != 0 && !selectedItems.contains(item))
		{
			selectedItems += item;
			if (!movingItems.contains(item))
				movingItems += item;
		}
		if (item != 0)
			emit itemsSelected(this, selectedItems, item->scenePos() , Qt::NoModifier);
		else
			emit itemsSelected(this, selectedItems, QPointF() , Qt::NoModifier);

		showGraphicalTools();
	}

	/*! \brief select items*/
	void GraphicsScene::select(const QList<QGraphicsItem*>& items)
	{
		for (int i=0; i < items.size(); ++i)
		{
			QGraphicsItem* item = items[i];
			if (item != 0 && !selectedItems.contains(item))
			{
				selectedItems += item;
				if (!movingItems.contains(item))
					movingItems += item;
			}
		}
		
		emit itemsSelected(this, selectedItems, QPointF() , Qt::NoModifier);
		showGraphicalTools();
	}

	/*! \brief deselect items*/
	void GraphicsScene::deselect(QGraphicsItem* item)
	{
		if (selectedItems.contains(item))
		{
			selectedItems.removeAll(item);
			emit itemsSelected(this, selectedItems, item->scenePos() , Qt::NoModifier);
			showGraphicalTools();
		}
	}

	/*! \brief deselect items*/
	void GraphicsScene::deselect()
	{
		selectedItems.clear();
		movingItems.clear();
		if (movingItemsGroup)
		{
			destroyItemGroup(movingItemsGroup);
			movingItemsGroup = 0;
		}
		emit itemsSelected(this, selectedItems, QPointF() , Qt::NoModifier);
		hideGraphicalTools();
	}
	
	/*! \brief adjusts view to include rect*/
	void GraphicsScene::fitInView(const QRectF& rect) const
	{
		QList<QGraphicsView*> list = views();
		for (int i=0; i < list.size(); ++i)
			if (list[i])
			{
				list[i]->fitInView(rect,Qt::KeepAspectRatio);
			}
	}

	/*! \brief adjusts view to include all items*/
	void GraphicsScene::fitAll() const
	{
		if (!networkWindow) return;
		QRectF rect;
		QPointF topLeft(0,0), bottomRight(0,0);
		QGraphicsItem * parent;
		QList<QGraphicsItem*> allItems = items();
		for (int i=0; i < allItems.size(); ++i)
		{
			parent = NodeGraphicsItem::cast(allItems[i]);
			if (!parent)
				parent = TextGraphicsItem::cast(allItems[i]);
			if (parent)
			{
				rect = parent->sceneBoundingRect();
				if (topLeft.x() == 0 || rect.left() < topLeft.x()) topLeft.rx() = rect.left();
				if (bottomRight.x() == 0 || rect.right() > bottomRight.x()) bottomRight.rx() = rect.right();

				if (topLeft.y() == 0 || rect.top() < topLeft.y()) topLeft.ry() = rect.top();
				if (bottomRight.y() == 0 || rect.bottom() > bottomRight.y()) bottomRight.ry() = rect.bottom();
			}
		}

        rect = QRectF(topLeft, bottomRight);
        
        QList<QGraphicsView*> list = views();
        for (int i=0; i < list.size(); ++i)
        	if (list[i])
		    {
		    	list[i]->fitInView(rect,Qt::KeepAspectRatio);
				list[i]->centerOn(rect.center());
			}
	}

	/*! \brief a simple move operation with undo*/
	void GraphicsScene::move(QGraphicsItem * item, const QPointF& distance)
	{
		if (!item) return;

		QPointF change = distance;
		if (gridSz > 0)
		{
			change.rx() = gridSz * (int)(change.rx() / (double)gridSz + 0.5);
			change.ry() = gridSz * (int)(change.ry() / (double)gridSz + 0.5);
		}

		QList<QPointF> dists;
		QList<QGraphicsItem*> items;
		items += item;
		dists += change;
		
		QList<QUndoCommand*> commands;
		emit itemsAboutToBeMoved(this,items,dists,commands);

		QUndoCommand * command = new MoveCommand(this,item,distance);
		
		if (!commands.isEmpty())
		{
			QString name = QObject::tr("items moved by (") + QString::number(distance.x()) + QObject::tr(",") + QString::number(distance.y()) + QObject::tr(")");
			commands << command;
			command = new CompositeCommand(name,commands);
		}

		if (network)
			network->history.push(command);
		else
		{
			command->redo();
			delete command;
		}
		
		NodeGraphicsItem::ControlPoint * nodePoint;
		ConnectionGraphicsItem::ControlPoint * connectionPoint;
		for (int i=0; i < items.size(); ++i)
		{
			nodePoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(items[i]);
			if (nodePoint && !items.contains(nodePoint->nodeItem))
			{
				items += nodePoint->nodeItem;
				dists += QPointF(0,0);
			}
			else
			{
				connectionPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(items[i]);
				if (connectionPoint && !items.contains(connectionPoint->connectionItem))
				{
					items += connectionPoint->connectionItem;
					dists += QPointF(0,0);
				}
			}
		}
		emit itemsMoved(this,items,dists);

		QPointF p = item->scenePos();
		if (p.rx() > this->width() || p.ry() > this->height())
		{
			setSceneRect(0,0,this->width()*2, this->height()*2);
		}
	}
	/*! \brief a simple move operation with undo*/
	void GraphicsScene::move(const QList<QGraphicsItem*>& items0, const QPointF& distance0)
	{
		QList<QGraphicsItem*> items = items0;
		QPointF distance = distance0;
		
		QPointF change = distance;
		if (gridSz > 0)
		{
			change.rx() = gridSz * (int)(change.rx() / (double)gridSz + 0.5);
			change.ry() = gridSz * (int)(change.ry() / (double)gridSz + 0.5);
		}
		
		QList<QPointF> dists;
		while (dists.size() < items.size()) dists << change;

		QList<QUndoCommand*> commands;
		
		emit itemsAboutToBeMoved(this,items,dists,commands);

		QUndoCommand * command = new MoveCommand(this,items, distance);
		
		if (!commands.isEmpty())
		{
			QString name = QObject::tr("items moved by (") + QString::number(distance.x()) + QObject::tr(",") + QString::number(distance.y()) + QObject::tr(")");
			commands << command;
			command = new CompositeCommand(name,commands);
		}

		if (network)
			network->history.push(command);
		else
		{
			command->redo();
			delete command;
		}
		
		NodeGraphicsItem::ControlPoint * nodePoint;
		ConnectionGraphicsItem::ControlPoint * connectionPoint;
		for (int i=0; i < items.size(); ++i)
		{
			nodePoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(items[i]);
			if (nodePoint && !items.contains(nodePoint->nodeItem))
			{
				items += nodePoint->nodeItem;
				dists += QPointF(0,0);
			}
			else
			{
				connectionPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(items[i]);
				if (connectionPoint && !items.contains(connectionPoint->connectionItem))
				{
					items += connectionPoint->connectionItem;
					dists += QPointF(0,0);
				}
			}
		}
		emit itemsMoved(this,items,dists);

		QPointF p;
		for (int i=0; i < items.size(); ++i)
			if (items[i])
			{
				QPointF p = items[i]->scenePos();
				if (p.rx() > this->width() || p.ry() > this->height())
				{
					setSceneRect(0,0,this->width()*2, this->height()*2);
					return;
				}
			}

	}
	/*! \brief a simple move operation with undo*/
	void GraphicsScene::move(const QList<QGraphicsItem*>& items0, const QList<QPointF>& distance)
	{
		QList<QGraphicsItem*> items = items0;
		QList<QPointF> dists = distance;

		if (gridSz > 0)
		{
			for (int i=0; i < dists.size(); ++i)
			{
				dists[i].rx() = gridSz * (int)(dists[i].rx() / (double)gridSz + 0.5);
				dists[i].ry() = gridSz * (int)(dists[i].ry() / (double)gridSz + 0.5);
			}
		}

		QList<QUndoCommand*> commands;
		emit itemsAboutToBeMoved(this,items,dists,commands);

		QUndoCommand * command = new MoveCommand(this,items, distance);
		
		if (!commands.isEmpty())
		{
			QString name = QObject::tr("items moved by ...");
			commands << command;
			command = new CompositeCommand(name,commands);
		}

		if (network)
			network->history.push(command);
		else
		{
			command->redo();
			delete command;
		}

		while (dists.size() < items.size()) dists << QPointF();

		NodeGraphicsItem::ControlPoint * nodePoint;
		ConnectionGraphicsItem::ControlPoint * connectionPoint;
		for (int i=0; i < items.size(); ++i)
		{
			nodePoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(items[i]);
			if (nodePoint && !items.contains(nodePoint->nodeItem))
			{
				items += nodePoint->nodeItem;
				dists += QPointF(0,0);
			}
			else
			{
				connectionPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(items[i]);
				if (connectionPoint && !items.contains(connectionPoint->connectionItem))
				{
					items += connectionPoint->connectionItem;
					dists += QPointF(0,0);
				}
			}
		}
		emit itemsMoved(this,items,dists);

		QPointF p;
		for (int i=0; i < items.size(); ++i)
			if (items[i])
			{
				QPointF p = items[i]->scenePos();
				if (p.rx() > this->width() || p.ry() > this->height())
				{
					setSceneRect(0,0,this->width()*2, this->height()*2);
					return;
				}
			}
	}

	void GraphicsScene::insert(const QString& name, QGraphicsItem * item)
	{
		if (!network) return;
		
		QList<ItemHandle*> handles;
		QList<QGraphicsItem*> items;
		items.append(item);

		ItemHandle* handle = getHandle(item);
		if (handle)
			handles += handle;

		QList<QUndoCommand*> commands;
		emit itemsAboutToBeInserted(this,items,handles,commands);
		
		QUndoCommand * command = new InsertGraphicsCommand(name, this, items);
		
		if (!commands.isEmpty())
		{
			commands << command;
			command = new CompositeCommand(name,commands);
		}
		
		network->history.push(command);
		emit itemsInserted(this,items,handles);
		network->symbolsTable.update();
	}

	/*! \brief this command performs an insert and allows redo/undo of that insert*/
	void GraphicsScene::insert(const QString& name, const QList<QGraphicsItem*>& items)
	{
		if (!network) return;
		QList<ItemHandle*> handles;
		QList<QGraphicsItem*> allItems = items;

		ItemHandle * handle;

		for (int i=0; i < items.size(); ++i)
		{
			handle = getHandle(items[i]);
			if (handle && !handles.contains(handle))
			{
				handles += handle;
			}
		}

		QList<QUndoCommand*> commands;
		emit itemsAboutToBeInserted(this,allItems,handles,commands);

		QUndoCommand * command = new InsertGraphicsCommand(name, this, allItems);
		
		if (!commands.isEmpty())
		{
			commands << command;
			command = new CompositeCommand(name,commands);
		}

		network->history.push(command);

		emit itemsInserted(this,allItems,handles);
		network->symbolsTable.update();
	}
	/*! \brief this command performs an removal and allows redo/undo of that removal*/
	void GraphicsScene::remove(const QString& name, QGraphicsItem * item)
	{
		if (!network) return;
		
		ItemHandle * handle = getHandle(item);

		QList<QGraphicsItem*> allitems;
		QList<QGraphicsItem*> items2;

		QList<ItemHandle*> handles;
		allitems << item;

		if (handle)
		{
			handles << handle;
			QList<ItemHandle*> handles2 = handle->allChildren();
			for (int j=0; j < handles2.size(); ++j)
				if (handles2[j])
					allitems << handles2[j]->allGraphicsItems();
		}

		QList<QUndoCommand*> commands;		
		emit itemsAboutToBeRemoved(this,allitems,handles,commands);

		QUndoCommand * command = new RemoveGraphicsCommand(name, allitems);
		
		if (!commands.isEmpty())
		{
			commands << command;
			command = new CompositeCommand(name,commands);
		}

		network->history.push(command);
		deselect();
		emit itemsRemoved(this,allitems,handles);
		network->symbolsTable.update();
	}
	/*! \brief this command performs an removal and allows redo/undo of that removal*/
	void GraphicsScene::remove(const QString& name, const QList<QGraphicsItem*>& items)
	{
		if (!network) return;

		QList<QGraphicsItem*> allitems, items2;

		QList<ItemHandle*> handles, handles2;
		ItemHandle * handle = 0;

		for (int i=0; i < items.size(); ++i)
			if (items[i])
			{
				handle = getHandle(items[i]);
				allitems << items[i];

				if (handle)
				{
					handles << handle;
					handles2 = handle->allChildren();
					for (int j=0; j < handles2.size(); ++j)
						if (handles2[j])
							allitems << handles2[j]->allGraphicsItems();
				}
			}

		QList<QUndoCommand*> commands;		
		emit itemsAboutToBeRemoved(this,allitems,handles,commands);

		QUndoCommand * command = new RemoveGraphicsCommand(name, allitems);
		
		if (!commands.isEmpty())
		{
			commands << command;
			command = new CompositeCommand(name,commands);
		}

		network->history.push(command);
		deselect();
		emit itemsRemoved(this,allitems, handles);
		network->symbolsTable.update();
	}
	/*! \brief this command changes the brush of an item*/
	void GraphicsScene::setBrush(const QString& name, QGraphicsItem * item, const QBrush& to)
	{
		QUndoCommand * command = new ChangeBrushCommand(name, item, to);

		if (network)
			network->history.push(command);
		else
		{
			command->redo();
			delete command;
		}

		QList<QGraphicsItem*> list;
		list += item;
		emit colorChanged(this,list);
	}
	/*! \brief this command changes the brush of an item*/
	void GraphicsScene::setBrush(const QString& name, const QList<QGraphicsItem*>& items, const QList<QBrush>& to)
	{
		QUndoCommand * command = new ChangeBrushCommand(name, items, to);

		if (network)
			network->history.push(command);
		else
		{
			command->redo();
			delete command;
		}

		emit colorChanged(this,items);
	}
	/*! \brief this command changes the pen of an item*/
	void GraphicsScene::setPen(const QString& name, QGraphicsItem * item, const QPen& to)
	{	
		QUndoCommand * command = new ChangePenCommand(name, item, to);

		if (network)
			network->history.push(command);
		else
		{
			command->redo();
			delete command;
		}

		QList<QGraphicsItem*> list;
		list += item;
		emit colorChanged(this,list);
	}
	/*! \brief this command changes the pen of an item*/
	void GraphicsScene::setPen(const QString& name, const QList<QGraphicsItem*>& items, const QList<QPen>& to)
	{
		QUndoCommand * command = new ChangePenCommand(name, items, to);

		if (network)
			network->history.push(command);
		else
		{
			command->redo();
			delete command;
		}

		emit colorChanged(this,items);
	}

	/*! \brief this command changes the pen of an item*/
	void GraphicsScene::setBrushAndPen(const QString& name, QGraphicsItem * item, const QBrush& newBrush, const QPen& newPen)
	{
		QUndoCommand * command = new ChangeBrushAndPenCommand(name, item, newBrush, newPen);

		if (network)
			network->history.push(command);
		else
		{
			command->redo();
			delete command;
		}

		QList<QGraphicsItem*> list;
		list += item;
		emit colorChanged(this,list);
	}
	/*! \brief this command changes the pen of an item*/
	void GraphicsScene::setBrushAndPen(const QString& name, const QList<QGraphicsItem*>& items, const QList<QBrush>& newBrushes, const QList<QPen>& newPens)
	{
		QUndoCommand * command = new ChangeBrushAndPenCommand(name, items, newBrushes, newPens);

		if (network)
			network->history.push(command);
		else
		{
			command->redo();
			delete command;
		}

		emit colorChanged(this,items);
	}

	/*! \brief this command changes the parent of an item*/
	void GraphicsScene::setParentItem(const QString& name, QGraphicsItem* item, QGraphicsItem* newParent)
	{
		QUndoCommand * command = new ChangeParentCommand(name, this, item, newParent);

		if (network)
			network->history.push(command);
		else
		{
			command->redo();
			delete command;
		}

		QList<QGraphicsItem*> items, newParents;
		items += item;
		newParents += newParent;
		emit parentItemChanged(this,items,newParents);
	}
	/*! \brief this command changes the parent of an item*/
	void GraphicsScene::setParentItem(const QString& name, const QList<QGraphicsItem*>& items, QGraphicsItem* newParent)
	{
		QList<QGraphicsItem*> newParents;
		for (int i=0; i < items.size(); ++i)
			newParents += newParent;

		QUndoCommand * command = new ChangeParentCommand(name, this, items, newParents);

		if (network)
			network->history.push(command);
		else
		{
			command->redo();
			delete command;
		}

		emit parentItemChanged(this,items,newParents);
	}
	/*! \brief this command changes the parent of an item*/
	void GraphicsScene::setParentItem(const QString& name, const QList<QGraphicsItem*>& items, const QList<QGraphicsItem*>& newParents)
	{
		QUndoCommand * command = new ChangeParentCommand(name, this, items, newParents);

		if (network)
			network->history.push(command);
		else
		{
			command->redo();
			delete command;
		}

		emit parentItemChanged(this,items,newParents);
	}

	/*! \brief this command changes the z value of an item*/
	void GraphicsScene::setZValue(const QString& name, QGraphicsItem * item, double to)
	{
		QUndoCommand * command = new ChangeZCommand(name, this, item, to);

		if (network)
			network->history.push(command);
		else
		{
			command->redo();
			delete command;
		}
	}

	/*! \brief this command changes the z value of an item*/
	void GraphicsScene::setZValue(const QString& name, const QList<QGraphicsItem*>& items, const QList<double>& to)
	{
		QUndoCommand * command = new ChangeZCommand(name, this, items, to);

		if (network)
			network->history.push(command);
		else
		{
			command->redo();
			delete command;
		}

	}

	/*! \brief this command changes the size, angle, and orientation of an item*/
	void GraphicsScene::transform(const QString& name, QGraphicsItem * item,
		const QPointF& sizechange,
		qreal anglechange,
		bool VFlip, bool HFlip)
	{

		QUndoCommand * command = new TransformCommand(name, this, item,
			sizechange,
			anglechange,
			VFlip, HFlip);
		if (network)
			network->history.push(command);
		else
		{
			command->redo();
			delete command;
		}
	}
	/*! \brief this command changes the size, angle, and orientation of an item*/
	void GraphicsScene::transform(const QString& name, const QList<QGraphicsItem*>& items,
		const QList<QPointF>& sizechange,
		const QList<qreal>& anglechange,
		const QList<bool>& VFlip,
		const QList<bool>& HFlip)
	{

		QUndoCommand * command = new TransformCommand(name, this, items,
			sizechange,
			anglechange,
			VFlip, HFlip);

		if (network)
			network->history.push(command);
		else
		{
			command->redo();
			delete command;
		}
	}

	/*! \brief prints the current scene*/
	void GraphicsScene::print(QPaintDevice * printer, const QRectF& region) const
	{
		if (!network) return;
		
		QList<QGraphicsView*> list = views();
		
		if (list.isEmpty() || !list[0]) return;
		
		QPainter painter(printer);
		//painter.setBackgroundMode(Qt::OpaqueMode);
		painter.setBackground(QBrush(Qt::white));
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setRenderHints(QPainter::NonCosmeticDefaultPen | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
		/*
		QList<QGraphicsItem*> itemsToDraw = items(rect);

		QList<QGraphicsItem*> list1, list2;

		for (int i=0; i < itemsToDraw.size(); ++i)
		if (ConnectionGraphicsItem::cast(itemsToDraw[i]))
		list2 << itemsToDraw[i];
		else
		list1 << itemsToDraw[i];


		for (int i=0; i < list1.size(); ++i)
		list1[i]->setVisible(false);

		render(&painter,QRectF(),rect);

		for (int i=0; i < list1.size(); ++i)
		list1[i]->setVisible(true);

		for (int i=0; i < list2.size(); ++i)
		list2[i]->setVisible(false);

		painter.setRenderHint(QPainter::Antialiasing);

		render(&painter,QRectF(),rect); */

		QRectF rect( 0, 0, printer->width(), printer->height());

		painter.fillRect(rect,QBrush(Qt::white));

		QGraphicsView * view = list[0];

		QPointF p1 = view->mapFromScene(region.topLeft()),
				p2 = view->mapFromScene(region.bottomRight());
		view->render(&painter,QRectF(p1,p2));
	}

	void GraphicsScene::clearStaticItems()
	{
		ConnectionGraphicsItem * connection = 0;

		for (int i=0; i < duplicateItems.size(); ++i)
			if ((connection = ConnectionGraphicsItem::cast(duplicateItems[i])))
			{
				QList<ArrowHeadItem*> arrowHeads = connection->arrowHeads();
				for (int j=0; j < arrowHeads.size(); ++j)
				{
					duplicateItems.removeAll(arrowHeads[j]);
				}
				duplicateItems.removeAll(connection->centerRegionItem);
			}

        for (int i=0; i < duplicateItems.size(); ++i)
            if (duplicateItems[i])
                delete duplicateItems[i];

        duplicateItems.clear();
	}

	void GraphicsScene::copy()
	{
		GraphicsScene * scene = this;

		QList<QGraphicsItem*> items = scene->selected();

		TextGraphicsItem* textItem = 0;
		QClipboard * clipboard = QApplication::clipboard();
		if (clipboard)
		{
			if (items.size() == 1 && (textItem = TextGraphicsItem::cast(items[0])))
			{
				clipboard->setText( textItem->toPlainText() );
			}
			else
			{
				QRectF viewport = this->viewport();
				/*if (items.size() == 1 && items[0])
					viewport = items[0]->sceneBoundingRect().normalized();
				else
					viewport = QRectF( clickedPoint, selectionRect.sceneBoundingRect().normalized().size() );*/
				int w = 640;
				int h = (int)(viewport.height() * w/viewport.width());
				QImage image(w,h,QImage::Format_ARGB32);
				scene->print(&image);
				clipboard->setImage(image);
			}
		}

		if (items.size() < 1) return;

		clearStaticItems();


		QList<ItemHandle*> allNewHandles;
		GraphicsScene::duplicateItems = cloneGraphicsItems(items,allNewHandles);

		emit copyItems(this,duplicateItems,allNewHandles);	

		GraphicsScene::copiedFromScene = scene;
	}

	void GraphicsScene::cut()
	{
		GraphicsScene * scene = this;

		QList<QGraphicsItem*> items = scene->selected();

		if (items.size() < 1) return;

		TextGraphicsItem* textItem = 0;
		QClipboard * clipboard = QApplication::clipboard();

		if (clipboard && !clipboard->text().isEmpty() && items.size() == 1 && (textItem = TextGraphicsItem::cast(items[0])))
		{
			clipboard->setText( textItem->toPlainText() );
		}

		clearStaticItems();

		QList<ItemHandle*> allNewHandles;

		GraphicsScene::duplicateItems = cloneGraphicsItems(items,allNewHandles);

		emit copyItems(this,duplicateItems,allNewHandles);

		scene->remove(tr("cut items"),scene->selected());

		GraphicsScene::copiedFromScene = scene;
	}

	void GraphicsScene::removeSelected()
	{
		if (selectedItems.size() < 1) return;

		remove(tr("items deleted"),selectedItems);

		selectedItems.clear();

		select(0);
	}

	void GraphicsScene::selectAll()
	{
		GraphicsScene * scene = this;

		QList<QGraphicsItem*> list;
		QList<QGraphicsItem*> items = scene->items();

		for (int i=0; i < items.size(); ++i)
		{
			QGraphicsItem * g = getGraphicsItem(items[i]);
			if (g && ControlPoint::cast(g) == 0 && ToolGraphicsItem::cast(g->topLevelItem()) == 0)
				list += g;
		}

		scene->select(list);
	}

	void GraphicsScene::paste()
	{
		if (!network) return;

		TextGraphicsItem* textItem = 0;
		QClipboard * clipboard = QApplication::clipboard();
		if (clipboard && !clipboard->text().isEmpty() && selectedItems.size() == 1 && (textItem = TextGraphicsItem::cast(selectedItems[0])))
		{
			textItem->setPlainText( textItem->toPlainText() + clipboard->text() );
			return;
		}

		QList<QGraphicsItem*> items = duplicateItems;

		QString name;
		QList<ItemHandle*> handles;
		QList<QGraphicsItem*> moveitems;
		ConnectionGraphicsItem * connection = 0;

		if (items.size() < 1) return;

		for (int i=0; i < duplicateItems.size(); ++i)
		{
			if ((connection = ConnectionGraphicsItem::cast(duplicateItems[i])))
			{
				if (copiedFromScene != this)
				{
					QList<NodeGraphicsItem*> allNodes = connection->nodes();
					for (int j=0; j < allNodes.size(); ++j)
						if (allNodes[j] && !duplicateItems.contains(allNodes[j]))
						{
							items.removeAll(connection);
							delete connection;
							break;
						}
				}
			}
		}

		QList<ItemHandle*> allNewHandles;
		
		GraphicsScene::duplicateItems = cloneGraphicsItems(items,allNewHandles);
		
		emit copyItems(this,duplicateItems,allNewHandles);

		QList<QUndoCommand*> commands;

		QPointF center;
		int n = 0;
		for (int i=0; i < items.size(); ++i)
			if (items[i])
			{
				if ((connection = ConnectionGraphicsItem::cast(items[i])))
				{
					QList<ConnectionGraphicsItem::ControlPoint*> cps = connection->controlPoints();
					for (int j=0; j < cps.size(); ++j)
					{
						moveitems += cps[j];
						center += cps[j]->scenePos();
						++n;
					}
				}
				else
				{
					moveitems += items[i];
					center += items[i]->scenePos();
					++n;
				}
			}

		if (n > 1)
			center /= n;

		if (!lastPoint().isNull())
		{
			commands << new MoveCommand(this,moveitems,lastPoint() - center);
			lastPoint() += QPointF(10.0,10.0);
		}

		QStringList allItems (network->symbolsTable.uniqueHandlesWithDot.keys());
		allItems << network->symbolsTable.uniqueHandlesWithUnderscore.keys()
				 << network->symbolsTable.uniqueDataWithDot.keys()
				 << network->symbolsTable.uniqueDataWithUnderscore.keys();
		QList<ItemHandle*> itemsToRename;
		QList<QString> newNames;
		ItemHandle *handle1;
		for (int i=0; i < items.size(); ++i)
		{
			handle1 = getHandle(items[i]);
			if (handle1 && !handles.contains(handle1))
			{
				handles << handle1;
				handles << handle1->allChildren();
			}
		}

		emit itemsAboutToBeInserted(this,items,handles,commands);

		for (int i=0; i < handles.size(); ++i)
		{
			handle1 = handles[i];
			if (!handle1->parent || !handles.contains(handle1->parent))
			{
				QString name0 = handle1->fullName();
				name = name0;

				while (name0.size() > 1 && name0[name0.length()-1].isNumber())
					name0.chop(1);

				bool unique = false;
				int c = 1;
				while (!unique)
				{
					unique = true;
					for (int j=0; j < allItems.size(); ++j)
					{
						if (allItems[j] == name)
						{
							unique = false;
							break;
						}
					}
					if (!unique)
					{
						name = name0 + QString::number(c);
						++c;
					}
				}
				allItems << name;
				itemsToRename << handles[i];
				newNames << name;
			}
		}

		commands << new RenameCommand(tr("items renamed after pasting"),network,handles,itemsToRename,newNames);
		commands << new InsertGraphicsCommand(tr("paste items"),this,items);
		
		clearSelection();

		QUndoCommand * compositeCommand = new CompositeCommand(tr("paste items"),commands);

		network->push(compositeCommand);

        emit itemsInserted(this,items,handles);
        network->symbolsTable.update();
		select(items);
	}

	void GraphicsScene::find(const QString& text)
	{
		if (!network || text.isNull() || text.isEmpty()) return;
		
		SymbolsTable * symbolsTable = &network->symbolsTable;
		NodeGraphicsItem* node = 0;
		ConnectionGraphicsItem* connection = 0;

		if (symbolsTable->uniqueHandlesWithDot.contains(text) ||
			symbolsTable->uniqueHandlesWithUnderscore.contains(text) ||
			symbolsTable->uniqueDataWithDot.contains(text) || 
			symbolsTable->uniqueDataWithUnderscore.contains(text))
		{
			ItemHandle * handle = 0;
			if (symbolsTable->uniqueHandlesWithDot.contains(text))
				handle = symbolsTable->uniqueHandlesWithDot[text];
			else
			if (symbolsTable->uniqueHandlesWithUnderscore.contains(text))
				handle = symbolsTable->uniqueHandlesWithUnderscore[text];
			else
			if (symbolsTable->uniqueDataWithDot.contains(text))
				handle = symbolsTable->uniqueDataWithDot[text].first;
			else
			if (symbolsTable->uniqueDataWithUnderscore.contains(text))
				handle = symbolsTable->uniqueDataWithUnderscore[text].first;
				
			if (!handle) return;

			bool alreadySelected = true;
			for (int i=0; i < handle->graphicsItems.size(); ++i)
				if (handle->graphicsItems[i])
					if (!selectedItems.contains(handle->graphicsItems[i]))
					{
						alreadySelected = false;
						break;
					}
				if (!alreadySelected)
				{
					selectedItems.clear();
					selectedItems = handle->graphicsItems;
					QPointF p(0,0);
					for (int j=0; j < handle->graphicsItems.size(); ++j)
						p += handle->graphicsItems[j]->sceneBoundingRect().center();

					p /= handle->graphicsItems.size();
					centerOn(p);

					emit itemsSelected(this,selectedItems,QPointF(),Qt::NoModifier);
					showGraphicalTools();
					return;
				}
		}
		
		if (symbolsTable->nonuniqueHandles.contains(text) ||
			symbolsTable->nonuniqueData.contains(text))
		{

			selectedItems.clear();

			QList<ItemHandle*> items;
			
			if (symbolsTable->nonuniqueHandles.contains(text))
			{
				items = symbolsTable->nonuniqueHandles.values(text);
			}
			
			if (symbolsTable->nonuniqueData.contains(text))
			{
				QList< QPair<ItemHandle*,QString> > pairs = symbolsTable->nonuniqueData.values(text);
				for (int i=0; i < pairs.size(); ++i)
					items << pairs[i].first;
			}
			
			for (int i=0; i < items.size(); ++i)
			{
				ItemHandle * handle = items[i];				
				if (!handle) continue;

				bool alreadySelected = true;
				for (int i=0; i < handle->graphicsItems.size(); ++i)
					if (handle->graphicsItems[i])
						if (!selectedItems.contains(handle->graphicsItems[i]))
						{
							alreadySelected = false;
							break;
						}
				if (!alreadySelected)
					selectedItems += handle->graphicsItems;
			}
			
			QPointF p(0,0);
			for (int j=0; j < selectedItems.size(); ++j)
				p += selectedItems[j]->scenePos();
			p /= selectedItems.size();
			emit itemsSelected(this,selectedItems,QPointF(),Qt::NoModifier);
			showGraphicalTools();
		}
	}

	QList<QGraphicsItem*> GraphicsScene::duplicateItems;
	GraphicsScene* GraphicsScene::copiedFromScene;

	void GraphicsScene::enableGrid(int sz)
	{
		setGridSize(sz);
	}

	void GraphicsScene::disableGrid()
	{
		setGridSize(0);
	}

	void GraphicsScene::setGridSize(int sz)
	{
		gridSz = sz;
	}

	int GraphicsScene::gridSize() const
	{
		return gridSz;
	}

	void GraphicsScene::snapToGrid(QGraphicsItem * item)
	{
		if (!item || gridSz < 1) return;

		ControlPoint * cp = ControlPoint::cast(item);

		if (cp)
		{
			QPointF p1 = cp->scenePos();

			p1.rx() = gridSz * (int)(p1.rx() / (double)gridSz + 0.5);
			p1.ry() = gridSz * (int)(p1.ry() / (double)gridSz + 0.5);

			cp->setPos(p1);

			return;
		}

		NodeGraphicsItem * node = NodeGraphicsItem::cast(item);

		if (node)
		{
			QPointF p1 = node->sceneBoundingRect().topLeft();
			QPointF p2 = node->sceneBoundingRect().bottomRight();

			p1.rx() = gridSz * (int)(p1.rx() / (double)gridSz + 0.5);
			p1.ry() = gridSz * (int)(p1.ry() / (double)gridSz + 0.5);
			p2.rx() = gridSz * (int)(p2.rx() / (double)gridSz + 0.5);
			p2.ry() = gridSz * (int)(p2.ry() / (double)gridSz + 0.5);

			if (p2.rx() == p1.rx()) p2.rx() += gridSz;
			if (p2.ry() == p1.ry()) p2.ry() += gridSz;

			node->setBoundingRect(p1,p2);
		}
	}

	void GraphicsScene::drawBackground( QPainter* painter, const QRectF & rect)
	{
		if (gridSz < 1) return;

		painter->setPen(GridPen);
		qreal left = rect.left(), right = rect.right(),
			  top = rect.top(), bottom = rect.bottom();
		QPointF p1,p2;

		p1.rx() = left;
		p1.ry() = top;
		p2.rx() = left;
		p2.ry() = bottom;

		left = gridSz * (int)(left/gridSz);

		for (qreal x = left; x < right; x += gridSz)
		{
			p1.rx() = p2.rx() = x;
			painter->drawLine(p1,p2);
		}

		p1.rx() = left;
		p1.ry() = top;
		p2.rx() = right;
		p2.ry() = top;

		top = gridSz * (int)(top/gridSz);

		for (qreal y = top; y < bottom; y += gridSz)
		{
			p1.ry() = p2.ry() = y;
			painter->drawLine(p1,p2);
		}
	}
	
	MainWindow * GraphicsScene::mainWindow() const
	{
		if (network)
			return network->mainWindow;
		return 0;
	}
	
	ConsoleWindow * GraphicsScene::console() const
	{
		if (network && network->mainWindow)
			return network->mainWindow->console();
		return 0;
	}
	
	ItemHandle * GraphicsScene::localHandle() const
	{
		if (networkWindow)
			return networkWindow->handle;
		return 0;
	}
	
	ItemHandle * GraphicsScene::globalHandle() const
	{
		if (network)
			return network->globalHandle();
		return 0;
	}
	
	void GraphicsScene::selectConnections(const QPointF& point)
	{
		ConnectionGraphicsItem * connection = 0;
		NodeGraphicsItem * node = 0;

		for (int i=movingItems.size()-1; i >= 0; --i)
			if (movingItems[i] != 0)
			{
				if ((connection = ConnectionGraphicsItem::topLevelConnectionItem(movingItems[i])))
				{
					movingItems.removeAt(i);

					for (int i=0; i < connection->curveSegments.size(); ++i)
					{
						if (connection->curveSegments[i].arrowStart)
							movingItems.removeAll(connection->curveSegments[i].arrowStart);
						if (connection->curveSegments[i].arrowEnd)
							movingItems.removeAll(connection->curveSegments[i].arrowEnd);
					}

					QList<ConnectionGraphicsItem::ControlPoint*> list = connection->controlPoints();

					bool noControlsSelected = true;
					bool controlPointsExist = false;
					for (int i=0; i < list.size(); ++i)
					{
						if (!controlPointsExist && list[i] && list[i]->isVisible() && !list[i]->parentItem())
							controlPointsExist = true;
						
						if (list[i] && list[i]->isVisible() && list[i]->sceneBoundingRect().contains(point))
						{
							if (!movingItems.contains(list[i]) && list[i]->scene() == this)
								movingItems += list[i];
							noControlsSelected = false;
							//break;
						}
					}

					if (noControlsSelected)
					{
						//connection->setControlPointsVisible(true);

						for (int i=0; i < list.size(); ++i)
							if (!movingItems.contains(list[i]) && list[i]->scene() == this)
								movingItems += list[i];

						ItemHandle * handle = connection->handle();
						if (handle && controlPointsExist)
							for (int i=0; i < handle->graphicsItems.size(); ++i)
							{
								if (TextGraphicsItem::cast(handle->graphicsItems[i])
									&& !movingItems.contains(handle->graphicsItems[i])
									&& handle->graphicsItems[i]->scene() == this)
									movingItems += handle->graphicsItems[i];
							}
					}
				}
				else
				{
					if (ArrowHeadItem::cast(movingItems[i]))
					{
						movingItems.removeAll(node);
					}
				}
			}
	}
	
	void GraphicsScene::setBackground(const QPixmap& image) const
	{
		QList<QGraphicsView*> list = views();
		GraphicsView * view;
		for (int i=0; i < list.size(); ++i)
			if (list[i])
			{
				view = static_cast<GraphicsView*>(list[i]);
				view->background = image;
			}
	}
	
	void GraphicsScene::setForeground(const QPixmap& image) const
	{
		QList<QGraphicsView*> list = views();
		GraphicsView * view;
		for (int i=0; i < list.size(); ++i)
			if (list[i])
			{
				view = static_cast<GraphicsView*>(list[i]);
				view->foreground = image;
			}
	}
	
	void GraphicsScene::popOut()
	{
		if (networkWindow)
			networkWindow->popOut();
	}
	
	void GraphicsScene::popIn()
	{
		if (networkWindow)
			networkWindow->popIn();
	}
	
	void GraphicsScene::hideGraphicalTools()
	{
		for (int i=0; i < visibleTools.size(); ++i)
		{
			ToolGraphicsItem * tool = visibleTools[i];
			if (tool && tool->tool)
			{
				if (tool->scene())
				{
					tool->scene()->removeItem(tool);
				}
				tool->visible(false);
				tool->deselect();
			}
		}
		visibleTools.clear();
	}

	void GraphicsScene::showGraphicalTools()
	{
		hideGraphicalTools();

		ItemHandle * itemHandle = 0;
		Tool * tool;
		for (int i=0; i < selectedItems.size(); ++i)
		{
			if (itemHandle = getHandle(selectedItems[i]))
			{
				for (int j=0; j < itemHandle->tools.size(); ++j)
				{
					tool = itemHandle->tools[j];
					if (tool && !tool->graphicsItems.isEmpty())
						for (int k=0; k < tool->graphicsItems.size(); ++k)
						{
							if (!visibleTools.contains(tool->graphicsItems[k]))
								visibleTools += tool->graphicsItems[k];
						}
				}
			}
		}
		
		scaleGraphicalTools();
	}

	void GraphicsScene::scaleGraphicalTools()
	{
		qreal scalex = 1, scaley = 1;
		QRectF viewport = this->viewport();
		scalex = viewport.width();
		scaley = viewport.height();
		qreal maxx = viewport.right() - 0.05*scalex,
			  miny = viewport.top() + 0.05*scaley,
			  w = 0;

		for (int i=0; i < visibleTools.size(); ++i)
		{
			ToolGraphicsItem * tool = visibleTools[i];
			if (tool)
			{
				if (tool->scene() != this)
				{
					if (tool->parentItem())
						tool->setParentItem(0);

					if (tool->scene() != 0)
						tool->scene()->removeItem(tool);

					addItem(tool);
				}

				tool->visible(true);
				tool->setZValue(ZValue()+0.1);
				
				QRectF bounds = tool->sceneBoundingRect();
				tool->resetTransform();
				
				qreal ratio = bounds.height()/bounds.width();
				qreal scale = (scaley + scalex)/2.0;
				
				tool->scale(0.001*(scale),0.001*(scale));

				tool->setPos(QPointF(maxx,miny));
				bounds = tool->sceneBoundingRect();

				if (tool->isVisible() && visibleTools.size() > 1)
				{
					if (bounds.height() < 500.0)
						miny += bounds.height() * 1.5;

					if (bounds.width() > w && bounds.width() < 500) w = bounds.width();

					if (miny > viewport.bottom() - 50.0)
					{
						miny = viewport.top() + 0.05*scale;
						maxx -= w * 1.5;
					}
				}
			}
		}
	}
	
	void GraphicsScene::populateContextMenu()
	{
		static QList<QAction*> separators, actions;
		
		if (!contextItemsMenu) return;

		Tool * tool = 0;
		QHash<QString,QAction*> hash;
		QList<QAction*> list;
		
		for (int i=0; i < actions.size(); ++i)
			if (actions[i])
				contextItemsMenu->removeAction(actions[i]);
		
		for (int i=0; i < separators.size(); ++i)
			if (separators[i])
				contextItemsMenu->removeAction(separators[i]);

		actions.clear();
		
		ItemHandle * handle = 0;
		QList<Tool*> visited;
		
		for (int i=0; i < selectedItems.size(); ++i)
			if (handle = getHandle(selectedItems[i]))
			{
				for (int j=0; j < handle->tools.size(); ++j)
					if ((tool = handle->tools[j]) && !visited.contains(tool) && !tool->actionsGroup.actions().isEmpty())
					{
						visited << tool;
						list = tool->actionsGroup.actions();
						for (int k=0; k < list.size(); ++k)
							hash.insertMulti(tool->category,list[k]);
					}
			}

		QStringList keys = hash.keys();
		
		for (int i=0; i < keys.size(); ++i)
		{
			if (separators.size() > i)
				contextItemsMenu->addAction(separators[i]);
			else
				separators << contextItemsMenu->addSeparator(); 
			
			list = hash.values(keys[i]);
			for (int j=0; j < list.size(); ++j)
			{
				contextItemsMenu->addAction(list[j]);
				actions << list[j];
			}
		}
	}
	
	void GraphicsScene::showToolTip(QPointF p, const QString & text)
	{
		QGraphicsSimpleTextItem * textItem = new QGraphicsSimpleTextItem(text);
		textItem->setPen(Qt::NoPen);
		textItem->setBrush(ToolTipTextBrush);
		
		QRectF viewport = this->viewport();
		qreal scale = 0.002 * (viewport.width());		
		textItem->scale(scale,scale);
		QGraphicsScene::addItem(textItem);
		textItem->setPos(p);
		
		QGraphicsRectItem * rectItem = new QGraphicsRectItem;
		rectItem->setPen(Qt::NoPen);
		rectItem->setBrush(ToolTipBackgroundBrush);		
		QGraphicsScene::addItem(rectItem);
		rectItem->setRect(textItem->sceneBoundingRect().adjusted(-5,-5,10,10));
		
		rectItem->setZValue(lastZ + 1.0);
		textItem->setZValue(lastZ + 2.0);
		
		toolTips << rectItem << textItem;
	}

	void GraphicsScene::hideToolTips()
	{
		if (toolTips.isEmpty()) return;
		
		for (int i=0; i < toolTips.size(); ++i)
			removeItem(toolTips[i]);
		qDeleteAll(toolTips);
		toolTips.clear();
	}
}

