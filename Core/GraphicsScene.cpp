/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This is one of the main classes in Tinkercell
This file defines the GraphicsScene class where all the drawing takes place.
In addition to drawing , the GraphicsScene provides serveral signals and functions
that is useful for plugins, eg. move, insert, delete, changeData, etc.

****************************************************************************/

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

	qreal GraphicsScene::MIN_DRAG_DISTANCE = 2.0;

	/*! \brief Returns the currently visible window
	* \param void
	* \return rectangle*/
	QRectF GraphicsScene::viewport() const
	{
		if (!networkWindow || !networkWindow->currentGraphicsView)
			return QRectF();

		GraphicsView * view = networkWindow->currentGraphicsView;

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
	GraphicsScene::GraphicsScene(QWidget * parent) : QGraphicsScene(parent)
	{
		gridSz = GRID;
		mouseDown = false;
		useDefaultBehavior = USE_DEFAULT_BEHAVIOR;
		setFocus();
		//setItemIndexMethod(NoIndex);

		symbolsTable = 0;
		historyStack = 0;
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
		selectedItems.clear();
		movingItems.clear();
		if (movingItemsGroup)
			destroyItemGroup(movingItemsGroup);
		select(0);
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
			if (allitems1[i] && !allitems1[i]->parentItem() && !Tool::GraphicsItem::cast(allitems1[i]))
			{
				allitems2 << allitems1[i];
			}
		}

		qDeleteAll(allitems2);
	}
	/*! \brief Clear all selection and moving items list
	* Precondition: None
	* Postcondition: None
	* \return void*/
	void GraphicsScene::clearSelection()
	{
		selectedItems.clear();
		if (movingItemsGroup)
		{
			destroyItemGroup(movingItemsGroup);
			movingItemsGroup = 0;
		}
		movingItems.clear();
		emit itemsSelected(this,selectedItems,QPointF(),Qt::NoModifier);
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
		clickedScreenPoint = mouseEvent->screenPos();
		clickedPoint = mouseEvent->scenePos();
		clickedButton = mouseEvent->button();
		mouseDown = true;

		QGraphicsItem * item = 0;
		Tool::GraphicsItem * gitem = 0;

		QGraphicsItem * p = itemAt(clickedPoint);

		if (p)
			gitem = Tool::GraphicsItem::cast(p->topLevelItem());

		if (!gitem)
		{
			p = getGraphicsItem(p);
			if (networkWindow &&
				networkWindow->currentGraphicsView &&
				networkWindow->currentGraphicsView->hiddenItems.contains(p))
				p = 0;

			if (!p || p->sceneBoundingRect().width() > 100 || p->sceneBoundingRect().height() > 100)
			{
				QList<QGraphicsItem*> ps = items(QRectF(clickedPoint.rx()-20.0,clickedPoint.ry()-20.0,40.0,40.0));
				if (!ps.isEmpty())
				{
					for (int i=0; i < ps.size(); ++i)
					{
						p = getGraphicsItem(ps[i]);
						if (networkWindow &&
							networkWindow->currentGraphicsView &&
							networkWindow->currentGraphicsView->hiddenItems.contains(p))
							p = 0;

						if (p)// && !TextGraphicsItem::cast(p))
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
					for (QList<QGraphicsItem*>::const_iterator i = selectedItems.constBegin(); i != selectedItems.constEnd(); ++i)
						if (*i && *i != &selectionRect)
						{
							movingItems.append((*i)->topLevelItem());
						}

						if (mouseEvent->button())// == Qt::LeftButton)
							emit itemsSelected(this, selectedItems,clickedPoint,mouseEvent->modifiers());

						if (movingItems.size() > 0)
						{
							movingItemsGroup = createItemGroup(movingItems);
							movingItemsGroup->setZValue(lastZ);
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
					emit itemsSelected(this, selectedItems,clickedPoint,mouseEvent->modifiers());

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
		QPointF point1 = mouseEvent->scenePos(), point0 = mouseEvent->lastScenePos();
		QPointF change = QPointF(point1.x()-point0.x(),point1.y()-point0.y());

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
						(item = getGraphicsItem(itemsInRect[i])) &&
						!(networkWindow &&
						  networkWindow->currentGraphicsView &&
						  networkWindow->currentGraphicsView->hiddenItems.contains(item))
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

	/*! \brief when mouse wheel is turned, zoom
	* Precondition: None
	* Postcondition: None
	* \param mouse wheel event
	* \return void*/
	void GraphicsScene::wheelEvent (QGraphicsSceneWheelEvent * wheelEvent)
	{
		QGraphicsScene::wheelEvent(wheelEvent);
	}
	/*! \brief context menu for the scene
	* Precondition: None
	* Postcondition: None
	* \param context menu event
	* \return void*/
	void GraphicsScene::contextMenuEvent ( QGraphicsSceneContextMenuEvent * mouseEvent )
	{
		if (useDefaultBehavior)
		{
			if (selectedItems.size() > 0)
			{
				if (contextItemsMenu && currentView())
					contextItemsMenu->exec(mouseEvent->screenPos());
			}
			else
			{
				if (contextScreenMenu)
					contextScreenMenu->exec(mouseEvent->screenPos());
			}
		}
		else
		{
			emit escapeSignal(networkWindow);
		}
	}
	/*! \brief zoom
	* Precondition: None
	* Postcondition: None
	* \param scale factor
	* \return void*/
	void GraphicsScene::scaleView(qreal scaleFactor)
	{
		if (!networkWindow || !networkWindow->currentGraphicsView) return;

		QGraphicsView * view = networkWindow->currentGraphicsView;

		qreal factor = networkWindow->currentGraphicsView->matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
		if (!(factor < 0.07 || factor > 100))
			networkWindow->currentGraphicsView->scale(scaleFactor, scaleFactor);
	}
	/*! \brief place center at the point
	* Precondition: None
	* Postcondition: None
	* \param point
	* \return void*/
	void GraphicsScene::centerOn(const QPointF& point)
	{
        if (networkWindow && networkWindow->currentGraphicsView)
            networkWindow->currentGraphicsView->centerOn(point);
	}
	/*! \brief when key is pressed
	* Precondition: None
	* Postcondition: None
	* \param key event
	* \return void*/
	void GraphicsScene::keyPressEvent (QKeyEvent * keyEvent)
	{
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
			if (historyStack)
				historyStack->undo();
			keyEvent->accept();
			return;
		}

		if (keyEvent->matches(QKeySequence::Redo))
		{
			if (historyStack)
				historyStack->redo();
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
			emit escapeSignal(networkWindow);
			keyEvent->accept();
		}

		if ((key == Qt::Key_Plus || key == Qt::Key_Equal || key == Qt::Key_Underscore || key == Qt::Key_Minus)
			&& (keyEvent->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier)))
		{
			if (key == Qt::Key_Plus || key == Qt::Key_Equal)
			{
				scaleView(qreal(1.2));
				keyEvent->accept();
			}
			else
				if (key == Qt::Key_Underscore || key == Qt::Key_Minus)
				{
					scaleView(1/qreal(1.2));
					keyEvent->accept();
				}
		}
		else
			if (useDefaultBehavior && (key == Qt::Key_Delete || key == Qt::Key_Backspace))
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
				if (useDefaultBehavior && !movingItems.isEmpty() &&
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

	void GraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
	{
		event->acceptProposedAction();
	}

	void GraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event)
	{
		/*QList<QUrl> urlList;
		QList<QFileInfo> files;
		QString fName;
		QFileInfo info;

		if (event->mimeData()->hasUrls())
		{
		urlList = event->mimeData()->urls(); // returns list of QUrls

		// if just text was dropped, urlList is empty (size == 0)
		if ( urlList.size() > 0) // if at least one QUrl is present in list
		{
		fName = urlList[0].toLocalFile(); // convert first QUrl to local path
		info.setFile( fName ); // information about file
		if ( info.isFile() )
		files += info;
		}
		}
		emit filesDropped(files);*/
		event->acceptProposedAction();
		//QGraphicsScene::dropEvent(event);
	}

	void GraphicsScene::dragMoveEvent ( QGraphicsSceneDragDropEvent * event )
	{
		//QGraphicsScene::dragMoveEvent(event);
		event->acceptProposedAction();
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
	}

	/*! \brief deselect items*/
	void GraphicsScene::deselect(QGraphicsItem* item)
	{
		if (selectedItems.contains(item))
		{
			selectedItems.removeAll(item);
			emit itemsSelected(this, selectedItems, item->scenePos() , Qt::NoModifier);
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

	}

	/*! \brief adjusts view to include all items*/
	void GraphicsScene::fitAll()
	{
		if (!networkWindow || !networkWindow->currentGraphicsView) return;
		networkWindow->currentGraphicsView->fitAll();
	}

	/*! \brief adjusts view to include all selected items*/
	void GraphicsScene::fitSelected()
	{
		if (!networkWindow || !networkWindow->currentGraphicsView) return;

		if (selectedItems.size() < 1)
			fitAll();
		else
		{
			QRectF rect;
			for (int i=0; i < selectedItems.size(); ++i)
				if (selectedItems[i])
				{
					rect = rect.unite(selectedItems[i]->topLevelItem()->sceneBoundingRect());
				}

				QGraphicsView* view = networkWindow->currentGraphicsView;

				QPoint a = view->mapFromScene(rect.topLeft());
				QPoint b = view->mapFromScene(rect.bottomRight());
				view->fitInView(QRectF(QRect(a,b)),Qt::KeepAspectRatio);
		}
	}


	/*! \brief a simple move operation with undo*/
	void GraphicsScene::move(QGraphicsItem * item, const QPointF& distance)
	{
		if (!item) return;

		QUndoCommand * command = new MoveCommand(this,item,distance);

		if (historyStack)
			historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}

		QList<QGraphicsItem*> items;
		items += item;

		QPointF change = distance;
		if (gridSz > 0)
		{
			change.rx() = gridSz * (int)(change.rx() / (double)gridSz + 0.5);
			change.ry() = gridSz * (int)(change.ry() / (double)gridSz + 0.5);
		}

		QList<QPointF> dists;
		while (dists.size() < items.size()) dists << change;

		emit itemsMoved(this,items,dists,Qt::NoModifier);

		QPointF p = item->scenePos();
		if (p.rx() > this->width() || p.ry() > this->height())
		{
			setSceneRect(0,0,this->width()*2, this->height()*2);
		}
	}
	/*! \brief a simple move operation with undo*/
	void GraphicsScene::move(const QList<QGraphicsItem*>& items, const QPointF& distance)
	{
		QUndoCommand * command = new MoveCommand(this,items, distance);

		if (historyStack)
			historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}

		QPointF change = distance;
		if (gridSz > 0)
		{
			change.rx() = gridSz * (int)(change.rx() / (double)gridSz + 0.5);
			change.ry() = gridSz * (int)(change.ry() / (double)gridSz + 0.5);
		}
		QList<QPointF> dists;
		while (dists.size() < items.size()) dists << change;

		emit itemsMoved(this,items,dists,Qt::NoModifier);

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
	void GraphicsScene::move(const QList<QGraphicsItem*>& items, const QList<QPointF>& distance)
	{
		QUndoCommand * command = new MoveCommand(this,items, distance);

		if (historyStack)
			historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}

		QList<QPointF> dists = distance;

		if (gridSz > 0)
		{
			for (int i=0; i < dists.size(); ++i)
			{
				dists[i].rx() = gridSz * (int)(dists[i].rx() / (double)gridSz + 0.5);
				dists[i].ry() = gridSz * (int)(dists[i].ry() / (double)gridSz + 0.5);
			}
		}

		while (dists.size() < items.size()) dists << QPointF();

		emit itemsMoved(this,items,dists,Qt::NoModifier);

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
		QList<ItemHandle*> handles;
		QList<QGraphicsItem*> items;
		items.append(item);

		ItemHandle* handle = getHandle(item);
		if (handle)
			handles += handle;

		emit itemsAboutToBeInserted(this,items,handles);

		QUndoCommand * command = new InsertGraphicsCommand(name, this, items);
		if (historyStack)
			historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}
		emit itemsInserted(this,items,handles);
	}

	/*! \brief this command performs an insert and allows redo/undo of that insert*/
	void GraphicsScene::insert(const QString& name, const QList<QGraphicsItem*>& items)
	{
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

		emit itemsAboutToBeInserted(this,allItems,handles);

		QUndoCommand * command = new InsertGraphicsCommand(name, this, allItems);

		if (historyStack)
			historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}

		emit itemsInserted(this,allItems,handles);
	}
	/*! \brief this command performs an removal and allows redo/undo of that removal*/
	void GraphicsScene::remove(const QString& name, QGraphicsItem * item)
	{
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

		emit itemsAboutToBeRemoved(this,allitems,handles);

		/*
		for (int i=0; i < allitems.size(); ++i)
		if ((item = allitems[i]) && (handle = getHandle(item)))
		{
		items2 = handle->graphicsItems;
		if (!TextGraphicsItem::cast(item))
		{
		for (int j=0; j < items2.size(); ++j)
		if (items2[j] && !allitems.contains(items2[j]) && item->collidesWithItem(items2[j]))
		{
		allitems << items2[j];
		}
		}
		}*/

		QUndoCommand * command = new RemoveGraphicsCommand(name, this, allitems);

		if (historyStack)
			historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}

		emit itemsRemoved(this,allitems,handles);
	}
	/*! \brief this command performs an removal and allows redo/undo of that removal*/
	void GraphicsScene::remove(const QString& name, const QList<QGraphicsItem*>& items)
	{
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

			emit itemsAboutToBeRemoved(this,allitems,handles);

			QUndoCommand * command = new RemoveGraphicsCommand(name, this, allitems);

			if (historyStack)
				historyStack->push(command);
			else
			{
				command->redo();
				delete command;
			}

			emit itemsRemoved(this,allitems, handles);
	}
	/*! \brief this command changes the brush of an item*/
	void GraphicsScene::setBrush(const QString& name, QGraphicsItem * item, const QBrush& to)
	{
		QUndoCommand * command = new ChangeBrushCommand(name, item, to);

		if (historyStack)
			historyStack->push(command);
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

		if (historyStack)
			historyStack->push(command);
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

		if (historyStack)
			historyStack->push(command);
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

		if (historyStack)
			historyStack->push(command);
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

		if (historyStack)
			historyStack->push(command);
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

		if (historyStack)
			historyStack->push(command);
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

		if (historyStack)
			historyStack->push(command);
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

		if (historyStack)
			historyStack->push(command);
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

		if (historyStack)
			historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}

		emit parentItemChanged(this,items,newParents);
	}

	void GraphicsScene::rename(const QString& oldname, const QString& newname)
	{
		if (networkWindow)
			networkWindow->rename(oldname,newname);
	}

	void GraphicsScene::rename(QGraphicsItem* item, const QString& name)
	{
		if (networkWindow)
			networkWindow->rename(getHandle(item),name);
	}

	void GraphicsScene::rename(ItemHandle* handle, const QString& name)
	{
		if (networkWindow)
			networkWindow->rename(handle,name);
	}

	void GraphicsScene::rename(const QList<QGraphicsItem*>& items, const QList<QString>& names)
	{
		QList<ItemHandle*> handles;
		ItemHandle * handle;
		for (int i=0; i < items.size(); ++i)
			if (!handles.contains( handle = getHandle(items[i]) ))
				handles += handle;
		if (networkWindow)
			networkWindow->rename(handles,names);
	}

	void GraphicsScene::assignHandles(const QList<QGraphicsItem*>& items, ItemHandle* newHandle)
	{
		if (!newHandle) return;
		QList<ItemHandle*> handles;
		for (int i=0; i < items.size(); ++i)
			handles += getHandle(items[i]);

		QUndoCommand * command = new AssignHandleCommand(tr("item defined"),items,newHandle);
		if (historyStack)
			historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}

		emit handlesChanged(this, items, handles);
	}

	void GraphicsScene::setParentHandle(const QList<ItemHandle*>& handles, const QList<ItemHandle*>& parentHandles)
	{
		if (networkWindow)
			networkWindow->setParentHandle(handles,parentHandles);
	}

	void GraphicsScene::setParentHandle(ItemHandle * child, ItemHandle * parent)
	{
		if (networkWindow)
			networkWindow->setParentHandle(child,parent);
	}

	void GraphicsScene::setParentHandle(const QList<ItemHandle*> children, ItemHandle * parent)
	{
		if (networkWindow)
			networkWindow->setParentHandle(children,parent);
	}

	/*! \brief this command changes the z value of an item*/
	void GraphicsScene::setZValue(const QString& name, QGraphicsItem * item, double to)
	{
		QUndoCommand * command = new ChangeZCommand(name, this, item, to);

		if (historyStack)
			historyStack->push(command);
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

		if (historyStack)
			historyStack->push(command);
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
		if (historyStack)
			historyStack->push(command);
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
		bool VFlip, bool HFlip)
	{

		QUndoCommand * command = new TransformCommand(name, this, items,
			sizechange,
			anglechange,
			VFlip, HFlip);

		if (historyStack)
			historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}
	}

	void GraphicsScene::mergeHandles(const QList<ItemHandle*>& handles)
	{
		if (handles.isEmpty()) return;

		MergeHandlesCommand * mergeCommand = new MergeHandlesCommand(tr("items merged"),networkWindow, handles);

		if (!mergeCommand->newHandle)
		{
			delete mergeCommand;
			return;
		}

		QString newName = mergeCommand->newHandle->fullName();
		QList<QString> oldNames,newNames;

		QList<QGraphicsItem*> items;

		for (int i=0; i < handles.size(); ++i)
		{
			newNames += newName;
			if (handles[i])
				items += handles[i]->graphicsItems[0];
			else
				items += 0;
		}

		QList<QUndoCommand*> commands;
		commands += mergeCommand;
		commands += new RenameCommand(tr("name changed"),networkWindow,handles,newNames);
		QUndoCommand * command = new CompositeCommand(tr("items merged"),commands);

		if (historyStack)
			historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}

		clearSelection();
		emit handlesChanged(this, items, handles);
	}

	/*! \brief prints the current scene*/
	void GraphicsScene::print(QPaintDevice * printer, const QRectF& region)
	{
		if (!networkWindow || !networkWindow->currentGraphicsView) return;
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

		QGraphicsView * view = networkWindow->currentGraphicsView;

		if (view)
		{
			QPointF p1 = view->mapFromScene(region.topLeft()),
					p2 = view->mapFromScene(region.bottomRight());
			view->render(&painter,QRectF(p1,p2));
		}
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
        duplicatedHiddenItems.clear();
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
		
		for (int i=0; i < items.size(); ++i)
			if (!isVisible(items[i]))
				GraphicsScene::duplicatedHiddenItems << GraphicsScene::duplicateItems[i];

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

		for (int i=0; i < items.size(); ++i)
			if (!isVisible(items[i]))
				GraphicsScene::duplicatedHiddenItems << GraphicsScene::duplicateItems[i];

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
			if (g && ControlPoint::cast(g) == 0 && Tool::GraphicsItem::cast(g->topLevelItem()) == 0)
				list += g;
		}

		scene->select(list);
	}

	void GraphicsScene::paste()
	{
		if (!symbolsTable) return;

		GraphicsScene * scene = this;

		TextGraphicsItem* textItem = 0;
		QClipboard * clipboard = QApplication::clipboard();
		if (clipboard && !clipboard->text().isEmpty() && scene->selected().size() == 1 && (textItem = TextGraphicsItem::cast(scene->selected()[0])))
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
				if (copiedFromScene != scene)
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
		QList<QGraphicsItem*> hideItems = duplicatedHiddenItems;
		
		GraphicsScene::duplicateItems = cloneGraphicsItems(items,allNewHandles);
		duplicatedHiddenItems.clear();
		
		for (int i=0; i < items.size(); ++i)
			if (hideItems.contains(items[i]))
				GraphicsScene::duplicatedHiddenItems << GraphicsScene::duplicateItems[i];
		
		emit copyItems(this,duplicateItems,allNewHandles);

		QList<QUndoCommand*> commands;

		QPointF center;
		for (int i=0; i < items.size(); ++i)
			if (items[i])
			{
				center += items[i]->scenePos();
				if ((connection = ConnectionGraphicsItem::cast(items[i])))
				{
					QList<ConnectionGraphicsItem::ControlPoint*> cps = connection->controlPoints();
					for (int j=0; j < cps.size(); ++j)
						moveitems += cps[j];
				}
				else
				{
					moveitems += items[i];
				}
			}

		if (items.size() > 1)
			center /= items.size();

		if (!scene->lastPoint().isNull())
		{
			commands << new MoveCommand(scene,moveitems,scene->lastPoint() - center);
		}

		QList<QString> allItems = scene->symbolsTable->handlesFullName.keys();
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

		emit itemsAboutToBeInserted(scene,items,handles);

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

		commands << new RenameCommand(tr("items renamed after pasting"),handles,itemsToRename,newNames);
		commands << new InsertGraphicsCommand(tr("paste items"),scene,items);		
		commands << new SetGraphicsViewVisibilityCommand(currentView(), hideItems, false);
		
		scene->clearSelection();

		QUndoCommand * compositeCommand = new CompositeCommand(tr("paste items"),commands);

		if (scene->historyStack)
			scene->historyStack->push(compositeCommand);
		else
		{
			compositeCommand->redo();
			delete compositeCommand;
		}

        emit itemsInserted(scene,items,handles);

		scene->select(items);
	}

	void GraphicsScene::find(const QString& text)
	{
		if (!networkWindow || !symbolsTable || text.isNull() || text.isEmpty()) return;

		NodeGraphicsItem* node = 0;
		ConnectionGraphicsItem* connection = 0;

		if (symbolsTable->handlesFullName.contains(text) ||
			symbolsTable->handlesFirstName.contains(text))
		{
			ItemHandle * handle = 0;
			if (symbolsTable->handlesFullName.contains(text))
				handle = symbolsTable->handlesFullName[text];
			else
				handle = symbolsTable->handlesFirstName[text];

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
						p += handle->graphicsItems[j]->scenePos();

					p /= handle->graphicsItems.size();
					centerOn(p);

					emit itemsSelected(this,selectedItems,QPointF(),Qt::NoModifier);
					return;
				}
		}

		QList<ItemHandle*> handles = networkWindow->allHandles();

		QRegExp regex(text);

		QList<ItemHandle*> highlightHandles;
		const DataTable<qreal>* nData = 0;
		const DataTable<QString>* sData = 0;
		for (int i=0; i < handles.size(); ++i)
		{
			if (handles[i]->fullName(tr(".")).contains(regex) ||
				handles[i]->fullName(tr("_")).contains(regex) ||
				(handles[i]->family() && handles[i]->family()->name.contains(regex)))
			{
				highlightHandles += handles[i];
				continue;
			}

			bool found = false;

			if (handles[i]->data)
			{
				QHashIterator<QString,DataTable<qreal> > itr1(handles[i]->data->numericalData);
				while (itr1.hasNext() && !found)
				{
					itr1.next();
					nData = &itr1.value();
					for (int j=0; j < nData->rows(); ++j)
					{
						if (nData->rowName(j).contains(regex))
						{
							found = true;
						}
						if (found) break;
					}

					for (int j=0; j < nData->cols(); ++j)
					{
						if (nData->colName(j).contains(regex))
						{
							found = true;
							break;
						}
					}
				}

				QHashIterator<QString,DataTable<QString> > itr2(handles[i]->data->textData);
				while (itr2.hasNext() && !found)
				{
					itr2.next();
					sData = &itr2.value();
					for (int j=0; j < sData->rows(); ++j)
					{
						if (sData->rowName(j).contains(regex))
						{
							found = true;
							break;
						}
					}
					if (found) break;

					for (int j=0; j < sData->cols(); ++j)
					{
						if (sData->colName(j).contains(regex))
						{
							found = true;
							break;
						}
					}
					if (found) break;

					for (int j=0; j < sData->rows(); ++j)
					{
						for (int k=0; k < sData->cols(); ++k)
						{
							if (sData->at(j,k).contains(regex))
							{
								found = true;
								break;
							}
						}
						if (found) break;
					}
				}

				if (found)
					highlightHandles += handles[i];
			}
		}

		if (!highlightHandles.isEmpty())
		{
			selectedItems.clear();
			for (int i=0; i < highlightHandles.size(); ++i)
			{
				if (highlightHandles[i] && highlightHandles[i]->graphicsItems.size() > 0)
				{
					QList<QGraphicsItem*>& list = highlightHandles[i]->graphicsItems;
					for (int j=0; j < list.size(); ++j)
					{
						if ((node = NodeGraphicsItem::cast(list[j])))
						{
							if (node->isVisible() && !selectedItems.contains(node))
								selectedItems += node;
						}
						else
						{
							if ((connection = ConnectionGraphicsItem::cast(list[j])))
							{
								if (connection->isVisible() && !selectedItems.contains(connection))
									selectedItems += connection;
							}
						}
					}
				}
			}
			emit itemsSelected(this,selectedItems,QPointF(),Qt::NoModifier);
		}
	}

	void GraphicsScene::changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const DataTable<qreal>* newdata)
	{
		if (networkWindow)
			networkWindow->changeData(name, handle,hashstring,newdata);
	}

	void GraphicsScene::changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstring, const QList<DataTable<qreal>*>& newdata)
	{
		if (networkWindow)
			networkWindow->changeData(name, handles,hashstring,newdata);
	}

	void GraphicsScene::changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<qreal>*>& newdata)
	{
		if (networkWindow)
			networkWindow->changeData(name, handles,hashstring,newdata);
	}

	void GraphicsScene::changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const DataTable<QString>* newdata)
	{
		if (networkWindow)
			networkWindow->changeData(name, handle,hashstring,newdata);
	}

	void GraphicsScene::changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstring, const QList<DataTable<QString>*>& newdata)
	{
		if (networkWindow)
			networkWindow->changeData(name, handles,hashstring,newdata);
	}

	void GraphicsScene::changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<QString>*>& newdata)
	{
		if (networkWindow)
			networkWindow->changeData(name, handles,hashstring,newdata);
	}

	void GraphicsScene::changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const DataTable<qreal>* newdata1, const DataTable<QString>* newdata2)
	{
		if (networkWindow)
			networkWindow->changeData(name, handle, hashstring, newdata1, newdata2);
	}

	void GraphicsScene::changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstring, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& newdata2)
	{
		if (networkWindow)
			networkWindow->changeData(name, handles, hashstring, newdata1, newdata2);
	}

	void GraphicsScene::changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& newdata2)
	{
		if (networkWindow)
			networkWindow->changeData(name, handles,hashstring,newdata1,newdata2);
	}

	void GraphicsScene::changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<DataTable<qreal>*>& olddata1, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& olddata2, const QList<DataTable<QString>*>& newdata2)
	{
		if (networkWindow)
			networkWindow->changeData(name, handles,olddata1,newdata1,olddata2,newdata2);
	}

	void GraphicsScene::changeData(const QString& name, const QList<ItemHandle*>& handles, DataTable<qreal>* olddata1, const DataTable<qreal>* newdata1, DataTable<QString>* olddata2, const DataTable<QString>* newdata2)
	{
		if (networkWindow)
			networkWindow->changeData(name, handles,olddata1,newdata1,olddata2,newdata2);
	}

	void GraphicsScene::changeData(const QString& name, const QList<ItemHandle*>& handles, DataTable<qreal>* olddata1, const DataTable<qreal>* newdata1)
	{
		if (networkWindow)
			networkWindow->changeData(name, handles,olddata1,newdata1);
	}

	void GraphicsScene::changeData(const QString& name, const QList<ItemHandle*>& handles, DataTable<QString>* olddata1, const DataTable<QString>* newdata1)
	{
		if (networkWindow)
			networkWindow->changeData(name, handles,olddata1,newdata1);
	}

	QList<ItemHandle*> GraphicsScene::allHandles() const
	{
		if (networkWindow)
			return networkWindow->allHandles();
		return QList<ItemHandle*>();
	}

	/*! \brief show item*/
	void GraphicsScene::showItems(const QString& name, QGraphicsItem* item)
	{
		QUndoCommand * command = new SetGraphicsSceneVisibilityCommand(name, item, true);

		if (historyStack)
			historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}
	}
	/*! \brief show items*/
	void GraphicsScene::showItems(const QString& name, const QList<QGraphicsItem*>& items)
	{
		QUndoCommand * command = new SetGraphicsSceneVisibilityCommand(name, items, true);

		if (historyStack)
			historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}
	}
	/*! \brief hide item*/
	void GraphicsScene::hideItems(const QString& name, QGraphicsItem* item)
	{
		QUndoCommand * command = new SetGraphicsSceneVisibilityCommand(name, item, false);

		if (historyStack)
			historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}
	}
	/*! \brief hide items*/
	void GraphicsScene::hideItems(const QString& name, const QList<QGraphicsItem*>& items)
	{
		QUndoCommand * command = new SetGraphicsSceneVisibilityCommand(name, items, false);

		if (historyStack)
			historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}
	}
	/*! \brief show handle that was hidden*/
	void GraphicsScene::showItems(const QString& name, ItemHandle* handle)
	{
		if (networkWindow)
			networkWindow->showItems(name,handle);
	}

	/*! \brief show handles that were hidden*/
	void GraphicsScene::showItems(const QString& name, const QList<ItemHandle*>& handles)
	{
		if (networkWindow)
			networkWindow->showItems(name,handles);
	}

	/*! \brief hide handle*/
	void GraphicsScene::hideItems(const QString& name, ItemHandle* handle)
	{
		if (networkWindow)
			networkWindow->hideItems(name,handle);
	}
	/*! \brief hide handles*/
	void GraphicsScene::hideItems(const QString& name, const QList<ItemHandle*>& handles)
	{
		if (networkWindow)
			networkWindow->hideItems(name,handles);
	}

	QList<QGraphicsItem*> GraphicsScene::duplicateItems;
	QList<QGraphicsItem*> GraphicsScene::duplicatedHiddenItems;
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

	/*! \brief get the console window (same as mainWindow->console())*/
    ConsoleWindow * GraphicsScene::console() const
    {
        if (networkWindow)
            return networkWindow->console();
        return 0;
    }

	GraphicsView * GraphicsScene::currentView() const
	{
		if (networkWindow)
            return networkWindow->currentView();
        return 0;
	}

	bool GraphicsScene::isVisible(QGraphicsItem * item) const
	{
		if (!item || !item->isVisible()) return false;

		if (networkWindow &&
			networkWindow->currentGraphicsView)
			return networkWindow->currentGraphicsView->checkVisibility(item);

        return true;
	}
}
