/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This is one of the main classes in Tinkercell
This file defines the GraphicsScene class where all the drawing takes place.
In addition to drawing , the GraphicsScene provides serveral signals and functions
that is useful for plugins, eg. move, insert, delete, changeData, etc.

****************************************************************************/

#include "ConsoleWindow.h"
#include "NetworkWindow.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "UndoCommands.h"
#include "GraphicsScene.h"
#include <QRegExp>

namespace Tinkercell
{
	QPen GraphicsScene::SelectionRectanglePen = QPen(Qt::NoPen);
	
	QBrush GraphicsScene::SelectionRectangleBrush = QBrush(QColor(0,132,255,50));
	
	/*
	* \param a pointer to a QGraphicsItem
	* \return a QGraphicsItem that is one of the Tinkercell Graphics Items
	*/
	QGraphicsItem * getGraphicsItem( QGraphicsItem * item )
	{
		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;
		ControlPoint * controlPoint = 0;
		TextGraphicsItem * text = 0;

		QGraphicsItem * p = item;

		while (p && (qgraphicsitem_cast<Tool::GraphicsItem*>(p->topLevelItem()) == 0))
		{
			text = qgraphicsitem_cast<TextGraphicsItem*>(p);
			if (text) return (QGraphicsItem*)(text);

			connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(p);
			if (connection) return (QGraphicsItem*)connection;

			node = qgraphicsitem_cast<NodeGraphicsItem*>(p);
			if (node) return (QGraphicsItem*)node;

			controlPoint = qgraphicsitem_cast<ControlPoint*>(p);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(p);
			if (controlPoint == 0) controlPoint = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(p);
			if (controlPoint) return (QGraphicsItem*)controlPoint;

			if (p != p->parentItem())
				p = p->parentItem();
			else
				p = 0;
		}

		return (0);
	}

	/* Clone a graphics item
	* \param a pointer to a QGraphicsItem
	* \return a QGraphicsItem that is one of the Tinkercell Graphics Items
	*/
	QGraphicsItem * cloneGraphicsItem( QGraphicsItem * item )
	{
		//ArrowHeadItem* arrow = 0;
		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;
		ControlPoint * controlPoint0 = 0;
		ConnectionGraphicsItem::ControlPoint * controlPoint1 = 0;
		NodeGraphicsItem::ControlPoint * controlPoint2 = 0;
		TextGraphicsItem * text = 0;

		QGraphicsItem * p = item;

		while (p && (qgraphicsitem_cast<Tool::GraphicsItem*>(p->topLevelItem()) == 0))
		{
			text = qgraphicsitem_cast<TextGraphicsItem*>(p);
			if (text) return (QGraphicsItem*)(text->clone());

			connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(p);
			if (connection) return (QGraphicsItem*)(connection->clone());

			node = qgraphicsitem_cast<NodeGraphicsItem*>(p);
			if (node) return (QGraphicsItem*)(node->clone());

			//arrow = qgraphicsitem_cast<ArrowHeadItem*>(p);
			//if (arrow) return (QGraphicsItem*)(arrow->clone());

			controlPoint0 = qgraphicsitem_cast<ControlPoint*>(p);
			if (controlPoint0)
				return (QGraphicsItem*)(controlPoint0->clone());

			controlPoint1 = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(p);
			if (controlPoint1)
				return (QGraphicsItem*)(controlPoint1->clone());

			controlPoint2 = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(p);
			if (controlPoint2)
				return (QGraphicsItem*)(controlPoint2->clone());

			if (p != p->parentItem())
				p = p->parentItem();
			else
				p = 0;
		}

		return (0);
	}

	/*! \brief Returns the currently visible window
	* \param void
	* \return rectangle*/
	QRectF GraphicsScene::viewport()
	{
		QGraphicsView * view = 0;
		if (views().size() > 0)
			view = views().first();

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
		mouseDown = false;
		useDefaultBehavior = true;
		setFocus();
		//setItemIndexMethod(NoIndex);

		symbolsTable = 0;
		historyStack = 0;
		contextItemsMenu = 0;
		contextScreenMenu = 0;
		setSceneRect(0,0,10000,10000);
		
		lastZ = 1.0;

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
		//QGraphicsScene::clear();
		QList<QGraphicsItem *> allitems1 = items();
		QList<QGraphicsItem *> allitems2;
		for (int i=0; i < allitems1.size(); ++i)
		{
			if (allitems1[i] && qgraphicsitem_cast<ConnectionGraphicsItem*>(allitems1[i]) && !allitems1[i]->parentItem())
			{
				allitems2 << allitems1[i];
			}
		}
		qDeleteAll(allitems2);

		allitems1 = items();
		allitems2.clear();
		for (int i=0; i < allitems1.size(); ++i)
		{
			if (allitems1[i] && !allitems1[i]->parentItem() && !qgraphicsitem_cast<Tool::GraphicsItem*>(allitems1[i]))
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
		if (!item) return;
		QGraphicsScene::addItem(item);
		item->setVisible(true);
		item->setZValue(lastZ);
		lastZ += 1.0;
		
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
		clickedPoint = mouseEvent->scenePos();
		clickedButton = mouseEvent->button();
		mouseDown = true;

		QGraphicsItem * item = 0;
		Tool::GraphicsItem * gitem = 0;

		QGraphicsItem * p = itemAt(clickedPoint);

		if (!p || p->sceneBoundingRect().width() > 100 || p->sceneBoundingRect().height() > 100)
		{
			QList<QGraphicsItem*> ps = items(QRectF(clickedPoint.rx()-20.0,clickedPoint.ry()-20.0,40.0,40.0));
			if (!ps.isEmpty())
			{
				int i=0;
				p = ps[i];
				while (i < ps.size() && qgraphicsitem_cast<TextGraphicsItem*>(p))
				{
					p = ps[i];
					++i;
				}
			}
		}

		if (p)
		{
			item = getGraphicsItem(p);
			if (!item)
				gitem = qgraphicsitem_cast<Tool::GraphicsItem*>(p->topLevelItem());
		}

		if (movingItemsGroup)
		{
			destroyItemGroup(movingItemsGroup);
			movingItemsGroup = 0;
		}
		movingItems.clear();

		if (clickedButton == Qt::RightButton)
		{
			emit sceneRightClick(this, item, clickedPoint, mouseEvent->modifiers());
		}
		//else
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
				/*else
				{
				QList<QGraphicsView*> vs = views();
				for (int i=0; i < vs.size(); ++i)
				vs[i]->setDragMode(QGraphicsView::ScrollHandDrag);
				}*/
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

		/*QList<QGraphicsView*> vs = views();
		for (int i=0; i < vs.size(); ++i)
		vs[i]->setDragMode(QGraphicsView::NoDrag);*/

		QPointF point1 = mouseEvent->scenePos(), point0 = clickedPoint;
		QPointF change = QPointF(point1.x()-point0.x(),point1.y()-point0.y());

		if (useDefaultBehavior && movingItems.size() > 0 && movingItemsGroup)
		{
			if (movingItemsGroup)
			{
				movingItemsGroup->moveBy(-change.x(),-change.y());
				destroyItemGroup(movingItemsGroup);
				movingItemsGroup = 0;
			}

			if ((change.x()*change.x() + change.y()*change.y()) > 1)
			{
				move(movingItems,change);
			}

			//movingItems.clear();
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
					if (qgraphicsitem_cast<ConnectionGraphicsItem*>(itemsIntersected[i]))
						itemsInRect.append(itemsIntersected[i]);

				for (int i=0; i < itemsInRect.size(); ++i)
					if (itemsInRect[i] != &selectionRect)
					{
						QGraphicsItem* item = getGraphicsItem(itemsInRect[i]);
						if (item != 0 && itemsInRect[i] == item)
							if (!selectedItems.contains(item))
								selectedItems.append(item);
							else
								if (mouseEvent->modifiers() == Qt::ShiftModifier)
									selectedItems.removeAll(item);
					}
					emit itemsSelected(this, selectedItems,point1,mouseEvent->modifiers());
			}
			if ((change.x()*change.x() + change.y()*change.y()) > 2)
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
	void GraphicsScene::contextMenuEvent ( QGraphicsSceneContextMenuEvent * contextMenuEvent )
	{
		if (useDefaultBehavior)
		{
			if (selectedItems.size() > 0)
			{
				if (contextItemsMenu)
					contextItemsMenu->exec(contextMenuEvent->screenPos());
			}
			else
			{
				if (contextScreenMenu)
					contextScreenMenu->exec(contextMenuEvent->screenPos());
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
		QList<QGraphicsView*> list = views();

		//QPen pen = selectionRect.pen();
		//pen.setWidthF( pen.widthF() / scaleFactor );
		//selectionRect.setPen(pen);

		for (int i=0; i < list.size(); ++i)
			if (list[i])
			{
				qreal factor = list[i]->matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
				if (!(factor < 0.07 || factor > 100))
					list[i]->scale(scaleFactor, scaleFactor);
			}
	}
	/*! \brief place center at the point
	* Precondition: None
	* Postcondition: None
	* \param point
	* \return void*/
	void GraphicsScene::centerOn(const QPointF& point)
	{
		QList<QGraphicsView*> list = views();

		for (int i=0; i < list.size(); ++i)
			if (list[i])
			{
				list[i]->centerOn(point);
			}
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

		QList<QGraphicsView*> list = views();
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
		if (movingItemsGroup != 0)
			destroyItemGroup(movingItemsGroup);
		emit itemsSelected(this, selectedItems, QPointF() , Qt::NoModifier);

	}

	/*! \brief adjusts view to include all items*/
	void GraphicsScene::fitAll()
	{
		QList<QGraphicsView*> allViews = views();
		for (int i=0; i < allViews.size(); ++i)
		{
			allViews[i]->fitInView(itemsBoundingRect(),Qt::KeepAspectRatio);
		}
	}

	/*! \brief adjusts view to include all selected items*/
	void GraphicsScene::fitSelected()
	{
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
				QList<QGraphicsView*> allViews = views();
				for (int i=0; i < allViews.size(); ++i)
				{
					QPoint a = allViews[i]->mapFromScene(rect.topLeft());
					QPoint b = allViews[i]->mapFromScene(rect.bottomRight());
					allViews[i]->fitInView(QRectF(QRect(a,b)),Qt::KeepAspectRatio);
				}
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

		QList<QPointF> dists;
		while (dists.size() < items.size()) dists << distance;

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

		QList<QPointF> dists;
		while (dists.size() < items.size()) dists << distance;

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
		if (!qgraphicsitem_cast<TextGraphicsItem*>(item))
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

			emit itemsRemoved(this,items, handles);
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

		MergeHandlersCommand * mergeCommand = new MergeHandlersCommand(tr("items merged"),networkWindow, handles);

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
	void GraphicsScene::print(QPaintDevice * printer)
	{
		QPainter painter(printer);
		//painter.setBackgroundMode(Qt::OpaqueMode);
		//painter.setBackground(QBrush(Qt::white));
		painter.setRenderHint(QPainter::Antialiasing);
		//painter.setRenderHints(QPainter::NonCosmeticDefaultPen | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
		/*
		QList<QGraphicsItem*> itemsToDraw = items(rect);

		QList<QGraphicsItem*> list1, list2;

		for (int i=0; i < itemsToDraw.size(); ++i)
		if (qgraphicsitem_cast<ConnectionGraphicsItem*>(itemsToDraw[i]))
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

		QGraphicsView * view = 0;
		if (views().size() > 0)
			view = views().first();

		if (view)
			view->render(&painter);
	}

	void GraphicsScene::clearStaticItems()
	{
		ConnectionGraphicsItem * connection = 0;

		for (int i=0; i < duplicateItems.size(); ++i)
			if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(duplicateItems[i])))
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

		if (items.size() < 1) return;

		TextGraphicsItem* textItem = 0;
		QClipboard * clipboard = QApplication::clipboard();
		if (clipboard && !clipboard->text().isEmpty() && items.size() == 1 && (textItem = qgraphicsitem_cast<TextGraphicsItem*>(items[0])))
		{
			clipboard->setText( textItem->toPlainText() );
		}

		clearStaticItems();

		//for connections
		QList< QPair<NodeGraphicsItem*,NodeGraphicsItem*> > originalsAndClones;
		QList< QPair<ItemHandle*,ItemHandle*> > originalAndCloneHandles;
		QList< ConnectionGraphicsItem* > connectionItems;
		NodeGraphicsItem *node1 = 0, *node2 = 0;
		ConnectionGraphicsItem* connection = 0;
		QList<ArrowHeadItem*> arrowHeads;
		
		for (int i = 0; i < items.size(); ++i)
			if (items[i])
			{
				ItemHandle * handle = getHandle(items[i]);
				if (handle)
				{
					for (int j=0; j < handle->children.size(); ++j)
						if (handle->children[j] && handle->children[j]->graphicsItems.size() > 0)
							for (int k=0; k < handle->children[j]->graphicsItems.size(); ++k)
								if (!items.contains(handle->children[j]->graphicsItems[k]) &&
									items[i]->collidesWithItem(handle->children[j]->graphicsItems[k]))
									items << handle->children[j]->graphicsItems[k];

					ItemHandle * handleClone = handle->clone();
					QList<ItemHandle*> childHandles = handle->allChildren();
					for (int j=0; j < childHandles.size(); ++j)
						if (childHandles[j] && childHandles[j]->graphicsItems.isEmpty())
						{
							ItemHandle * childHandleClone = childHandles[j]->clone();
							childHandleClone->setParent(handleClone);
						}

						handleClone->setParent( handle->parent);
						originalAndCloneHandles += QPair<ItemHandle*,ItemHandle*>(handle,handleClone);

						QList<QGraphicsItem*> list = handle->graphicsItems;
						//new handle only has selected graphics items of the original handle
						for (int j=0; j < list.size(); ++j)
							if (!items.contains(list[j]) && !qgraphicsitem_cast<TextGraphicsItem*>(list[j]))
								handleClone->graphicsItems.removeAll(list[j]);

						list = handleClone->graphicsItems;

						//remove visited items from the list of items
						int k = -1;
						for (int j=0; j < list.size(); ++j)
							while ( (k = items.indexOf(list[j])) > -1 )
							{
								items[k] = 0;
							}
							//clone each graphics item and assign handleClone
							handleClone->graphicsItems.clear();
							handleClone->tools.clear();
							for (int j=0; j < list.size(); ++j)
							{
								QGraphicsItem * itemClone = cloneGraphicsItem(list[j]);
								setHandle(itemClone,handleClone);
								duplicateItems << itemClone;

								if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(itemClone)))
								{
									connectionItems += connection;
									if (connection->centerRegionItem)
									{
										node1 = connection->centerRegionItem;
										connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(list[j]);
										if (connection && connection->centerRegionItem && connection->centerRegionItem->scene())
										{
											node2 = connection->centerRegionItem;
											if (node1 && node2)
											{
												originalsAndClones += QPair<NodeGraphicsItem*,NodeGraphicsItem*>(node2,node1);
												duplicateItems << node1;
											}
										}
									}
									/*connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(list[j]);
									if (connection)
									{
									arrowHeads << connection->arrowHeads() << connection->modifierArrowHeads();
									}*/
								}
								else
								{
									if ((node1 = qgraphicsitem_cast<NodeGraphicsItem*>(list[j]))
										&& (node2 = qgraphicsitem_cast<NodeGraphicsItem*>(itemClone)))
									{
										originalsAndClones += QPair<NodeGraphicsItem*,NodeGraphicsItem*>(node1,node2);
									}
								}
							}
				}
				else  //items without a handle
				{
					if (qgraphicsitem_cast<TextGraphicsItem*>(items[i]) || qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i]))
					{
						QGraphicsItem * itemClone = cloneGraphicsItem(items[i]);
						if (itemClone)
						{
							duplicateItems << itemClone;
							if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(itemClone)))
							{
								connectionItems += connection;
								if (connection->centerRegionItem)
								{
									node1 = connection->centerRegionItem;
									connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i]);
									if (connection && connection->centerRegionItem && connection->centerRegionItem->scene())
									{
										node2 = connection->centerRegionItem;
										if (node1 && node2)
										{
											originalsAndClones += QPair<NodeGraphicsItem*,NodeGraphicsItem*>(node2,node1);
											duplicateItems << node1;
										}
									}
								}
							}
							else
								if ((node1 = qgraphicsitem_cast<NodeGraphicsItem*>(items[i]))
									&& (node2 = qgraphicsitem_cast<NodeGraphicsItem*>(itemClone)))
								{
									originalsAndClones += QPair<NodeGraphicsItem*,NodeGraphicsItem*>(node1,node2);
								}
						}
					}
				}
			}
			/*
			//remove duplicated arrowheads
			for (int i=0; i < arrowHeads.size(); ++i)
			{
			if (arrowHeads[i])
			for (int j=0; j < originalsAndClones.size(); ++j)
			{
			if (arrowHeads[i] == originalsAndClones[j].first)
			{
			duplicateItems.removeAll(originalsAndClones[j].second);
			delete originalsAndClones[j].second;
			}
			}
			}*/
			//replace the duplicated connections' nodes with the duplicate nodes
			for (int i=0; i < connectionItems.size(); ++i)
			{
				QList<NodeGraphicsItem*> nodes = connectionItems[i]->nodes();
				for (int k=0; k < originalsAndClones.size(); ++k)
				{
					if (nodes.contains(originalsAndClones[k].first) && originalsAndClones[k].first && originalsAndClones[k].second)
					{
						connectionItems[i]->replaceNode(originalsAndClones[k].first,originalsAndClones[k].second);
					}
				}
			}

			QList<ItemHandle*> allNewHandles;
			for (int i=0; i < originalAndCloneHandles.size(); ++i)
			{
				if (originalAndCloneHandles[i].second && !allNewHandles.contains(originalAndCloneHandles[i].second))
				{
					allNewHandles << originalAndCloneHandles[i].second;
					allNewHandles << originalAndCloneHandles[i].second->allChildren();
				}
			}
			//replace parent handles
			bool parentCopied = false;
			for (int i=0; i < originalAndCloneHandles.size(); ++i)
			{
				if (originalAndCloneHandles[i].first->parent != 0)
				{
					parentCopied = false;
					for (int j=0; j < originalAndCloneHandles.size(); ++j)
					{
						if (originalAndCloneHandles[i].first->parent == originalAndCloneHandles[j].first)
						{
							originalAndCloneHandles[i].second->setParent(originalAndCloneHandles[j].second);
							parentCopied = true;
							break;
						}
					}
					if (!parentCopied)
					{
						QString oldName = originalAndCloneHandles[i].second->fullName();
						originalAndCloneHandles[i].second->setParent(0);
						QString newName = originalAndCloneHandles[i].second->fullName();
						RenameCommand::findReplaceAllHandleData(allNewHandles,oldName,newName);
					}
				}
			}
			GraphicsScene::copiedFromScene = scene;
	}

	void GraphicsScene::cut()
	{
		GraphicsScene * scene = this;

		QList<QGraphicsItem*> items = scene->selected();

		if (items.size() < 1) return;

		TextGraphicsItem* textItem = 0;
		QClipboard * clipboard = QApplication::clipboard();

		if (clipboard && !clipboard->text().isEmpty() && items.size() == 1 && (textItem = qgraphicsitem_cast<TextGraphicsItem*>(items[0])))
		{
			clipboard->setText( textItem->toPlainText() );
		}

		clearStaticItems();

		//for connections
		QList< QPair<NodeGraphicsItem*,NodeGraphicsItem*> > originalsAndClones;
		QList< QPair<ItemHandle*,ItemHandle*> > originalAndCloneHandles;
		QList< ConnectionGraphicsItem* > connectionItems;
		NodeGraphicsItem *node1 = 0, *node2 = 0;
		ConnectionGraphicsItem* connection = 0;
		QList<ArrowHeadItem*> arrowHeads;

		for (int i = 0; i < items.size(); ++i)
			if (items[i])
			{
				ItemHandle * handle = getHandle(items[i]);
				if (handle)
				{
					for (int j=0; j < handle->children.size(); ++j)
						if (handle->children[j] && handle->children[j]->graphicsItems.size() > 0)
							for (int k=0; k < handle->children[j]->graphicsItems.size(); ++k)
								if (!items.contains(handle->children[j]->graphicsItems[k]) &&
									items[i]->collidesWithItem(handle->children[j]->graphicsItems[k]))
									items << handle->children[j]->graphicsItems[k];


					ItemHandle * handleClone = handle->clone();
					QList<ItemHandle*> childHandles = handle->allChildren();
					for (int j=0; j < childHandles.size(); ++j)
						if (childHandles[j] && childHandles[j]->graphicsItems.isEmpty())
						{
							ItemHandle * childHandleClone = childHandles[j]->clone();
							childHandleClone->setParent(handleClone);
						}

						handleClone->setParent( handle->parent);
						originalAndCloneHandles += QPair<ItemHandle*,ItemHandle*>(handle,handleClone);

						QList<QGraphicsItem*> list = handle->graphicsItems;
						//new handle only has selected graphics items of the original handle
						for (int j=0; j < list.size(); ++j)
							if (!items.contains(list[j]) && !qgraphicsitem_cast<TextGraphicsItem*>(list[j]))
								handleClone->graphicsItems.removeAll(list[j]);

						list = handleClone->graphicsItems;

						//remove visited items from the list of items
						int k = -1;
						for (int j=0; j < list.size(); ++j)
							while ( (k = items.indexOf(list[j])) > -1 )
							{
								items[k] = 0;
							}
							//clone each graphics item and assign handleClone
							handleClone->graphicsItems.clear();
							handleClone->tools.clear();
							for (int j=0; j < list.size(); ++j)
							{
								QGraphicsItem * itemClone = cloneGraphicsItem(list[j]);
								setHandle(itemClone,handleClone);
								duplicateItems << itemClone;

								if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(itemClone)))
								{
									connectionItems += connection;
									if (connection->centerRegionItem)
									{
										node1 = connection->centerRegionItem;
										connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(list[j]);
										if (connection && connection->centerRegionItem && connection->centerRegionItem->scene())
										{
											node2 = connection->centerRegionItem;
											if (node1 && node2)
											{
												originalsAndClones += QPair<NodeGraphicsItem*,NodeGraphicsItem*>(node2,node1);
												duplicateItems << node1;
											}
										}
									}
									/*if (connection)
									{
									arrowHeads << connection->arrowHeads() << connection->modifierArrowHeads();
									}*/
								}
								else
								{
									if ((node1 = qgraphicsitem_cast<NodeGraphicsItem*>(list[j]))
										&& (node2 = qgraphicsitem_cast<NodeGraphicsItem*>(itemClone)))
									{
										originalsAndClones += QPair<NodeGraphicsItem*,NodeGraphicsItem*>(node1,node2);
									}
								}

							}
				}
				else  //items without a handle
				{
					if (qgraphicsitem_cast<TextGraphicsItem*>(items[i]) || qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i]))
					{
						QGraphicsItem * itemClone = cloneGraphicsItem(items[i]);
						if (itemClone)
						{
							duplicateItems << itemClone;
							if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(itemClone)))
							{
								connectionItems += connection;
								if (connection->centerRegionItem)
								{
									node1 = connection->centerRegionItem;
									connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i]);
									if (connection && connection->centerRegionItem  && connection->centerRegionItem->scene())
									{
										node2 = connection->centerRegionItem;
										if (node1 && node2)
										{
											originalsAndClones += QPair<NodeGraphicsItem*,NodeGraphicsItem*>(node2,node1);
											duplicateItems << node1;
										}
									}
								}
							}
							else
								if ((node1 = qgraphicsitem_cast<NodeGraphicsItem*>(items[i]))
									&& (node2 = qgraphicsitem_cast<NodeGraphicsItem*>(itemClone)))
								{
									originalsAndClones += QPair<NodeGraphicsItem*,NodeGraphicsItem*>(node1,node2);
								}
						}
					}
				}
			}
			/*
			//remove duplicated arrowheads
			for (int i=0; i < arrowHeads.size(); ++i)
			{
			if (arrowHeads[i])
			for (int j=0; j < originalsAndClones.size(); ++j)
			{
			if (arrowHeads[i] == originalsAndClones[j].first)
			{
			duplicateItems.removeAll(originalsAndClones[j].second);
			delete originalsAndClones[j].second;
			}
			}
			}*/
			//replace the duplicated connections' nodes with the duplicate nodes
			for (int i=0; i < connectionItems.size(); ++i)
			{
				QList<NodeGraphicsItem*> nodes = connectionItems[i]->nodes();
				for (int k=0; k < originalsAndClones.size(); ++k)
				{
					if (nodes.contains(originalsAndClones[k].first) && originalsAndClones[k].first && originalsAndClones[k].second)
					{
						connectionItems[i]->replaceNode(originalsAndClones[k].first,originalsAndClones[k].second);
					}
				}
			}

			QList<ItemHandle*> allNewHandles;
			for (int i=0; i < originalAndCloneHandles.size(); ++i)
			{
				if (originalAndCloneHandles[i].second && !allNewHandles.contains(originalAndCloneHandles[i].second))
				{
					allNewHandles << originalAndCloneHandles[i].second;
					allNewHandles << originalAndCloneHandles[i].second->allChildren();
				}
			}
			//replace parent handles
			bool parentCopied = false;
			for (int i=0; i < originalAndCloneHandles.size(); ++i)
			{
				if (originalAndCloneHandles[i].first->parent != 0)
				{
					parentCopied = false;
					for (int j=0; j < originalAndCloneHandles.size(); ++j)
					{
						if (originalAndCloneHandles[i].first->parent == originalAndCloneHandles[j].first)
						{
							originalAndCloneHandles[i].second->setParent(originalAndCloneHandles[j].second);
							parentCopied = true;
							break;
						}
					}
					if (!parentCopied)
					{
						QString oldName = originalAndCloneHandles[i].second->fullName();
						originalAndCloneHandles[i].second->setParent(0);
						QString newName = originalAndCloneHandles[i].second->fullName();
						RenameCommand::findReplaceAllHandleData(allNewHandles,oldName,newName);
					}
				}
			}
			copiedFromScene = scene;

			scene->remove(tr("cut items"),scene->selected());
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
			if (g && ControlPoint::asControlPoint(g) == 0 && qgraphicsitem_cast<Tool::GraphicsItem*>(g->topLevelItem()) == 0)
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
		if (clipboard && !clipboard->text().isEmpty() && scene->selected().size() == 1 && (textItem = qgraphicsitem_cast<TextGraphicsItem*>(scene->selected()[0])))
		{
			textItem->setPlainText( textItem->toPlainText() + clipboard->text() );
			return;
		}

		QString name;
		QList<ItemHandle*> handles;
		QList<QGraphicsItem*> items = duplicateItems;
		QList<QGraphicsItem*> moveitems;
		ConnectionGraphicsItem * connection = 0;

		if (items.size() < 1) return;

		for (int i=0; i < duplicateItems.size(); ++i)
		{
			if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(duplicateItems[i])))
			{
				if (copiedFromScene != scene)
				{
					QList<NodeGraphicsItem*> allNodes = connection->nodes();
					for (int j=0; j < allNodes.size(); ++j)
						if (allNodes[j] && !duplicateItems.contains(allNodes[j]))
						{
							items.removeAll(connection);
							break;
						}
				}
			}
		}

		QList<QUndoCommand*> commands;

		QPointF center;
		for (int i=0; i < items.size(); ++i)
			if (items[i])
			{
				center += items[i]->scenePos();
				if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i])))
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
		
		QList<QGraphicsItem*> items2 = items;
		QList<ItemHandle*> handles2 = handles;
		
		emit itemsAboutToBeInserted(scene,items2,handles2);

		commands << new InsertGraphicsCommand(tr("paste items"),scene,items2);

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
		scene->clearSelection();
			
		//for connections
		QList< QPair<NodeGraphicsItem*,NodeGraphicsItem*> > originalsAndClones;
		QList< ConnectionGraphicsItem* > connectionItems;
		NodeGraphicsItem *node1 = 0, *node2 = 0;
		QList<ArrowHeadItem*> arrowHeads;
			
		QUndoCommand * compositeCommand = new CompositeCommand(tr("paste items"),commands);

		items2 = items;
		duplicateItems.clear();
		QList< QPair<ItemHandle*,ItemHandle*> > originalAndCloneHandles;

		if (scene->historyStack)
			scene->historyStack->push(compositeCommand);
		else
			compositeCommand->redo();
			
		for (int i = 0; i < items.size(); ++i)
			if (items[i])
			{
				ItemHandle * handle = getHandle(items[i]);
				if (handle)
				{
					for (int j=0; j < handle->children.size(); ++j)
						if (handle->children[j] && handle->children[j]->graphicsItems.size() > 0)
							for (int k=0; k < handle->children[j]->graphicsItems.size(); ++k)
								if (!items.contains(handle->children[j]->graphicsItems[k]) &&
									items[i]->collidesWithItem(handle->children[j]->graphicsItems[k]))
									items << handle->children[j]->graphicsItems[k];

					ItemHandle * handleClone = handle->clone();
					QList<ItemHandle*> childHandles = handle->allChildren();
					for (int j=0; j < childHandles.size(); ++j)
						if (childHandles[j] && childHandles[j]->graphicsItems.isEmpty())
						{
							ItemHandle * childHandleClone = childHandles[j]->clone();
							childHandleClone->setParent(handleClone);
						}

						handleClone->setParent( handle->parent);
						originalAndCloneHandles += QPair<ItemHandle*,ItemHandle*>(handle,handleClone);

						QList<QGraphicsItem*> list = handle->graphicsItems;
						//new handle only has selected graphics items of the original handle
						for (int j=0; j < list.size(); ++j)
							if (!items.contains(list[j]) && !qgraphicsitem_cast<TextGraphicsItem*>(list[j]))
								handleClone->graphicsItems.removeAll(list[j]);

						list = handleClone->graphicsItems;

						//remove visited items from the list of items
						int k = -1;
						for (int j=0; j < list.size(); ++j)
							while ( (k = items.indexOf(list[j])) > -1 )
							{
								items[k] = 0;
							}
							//clone each graphics item and assign handleClone
							handleClone->graphicsItems.clear();
							handleClone->tools.clear();
							for (int j=0; j < list.size(); ++j)
							{
								QGraphicsItem * itemClone = cloneGraphicsItem(list[j]);
								setHandle(itemClone,handleClone);
								duplicateItems << itemClone;

								if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(itemClone)))
								{
									connectionItems += connection;
									if (connection->centerRegionItem)
									{
										node1 = connection->centerRegionItem;
										connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(list[j]);
										if (connection && connection->centerRegionItem && connection->centerRegionItem->scene())
										{
											node2 = connection->centerRegionItem;
											if (node1 && node2)
											{
												originalsAndClones += QPair<NodeGraphicsItem*,NodeGraphicsItem*>(node2,node1);
												duplicateItems << node1;
											}
										}
									}
									/*connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(list[j]);
									if (connection)
									{
									arrowHeads << connection->arrowHeads() << connection->modifierArrowHeads();
									}*/
								}
								else
								{
									if ((node1 = qgraphicsitem_cast<NodeGraphicsItem*>(list[j]))
										&& (node2 = qgraphicsitem_cast<NodeGraphicsItem*>(itemClone)))
									{
										originalsAndClones += QPair<NodeGraphicsItem*,NodeGraphicsItem*>(node1,node2);
									}
								}
							}
				}
				else  //items without a handle
				{
					if (qgraphicsitem_cast<TextGraphicsItem*>(items[i]) || qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i]))
					{
						QGraphicsItem * itemClone = cloneGraphicsItem(items[i]);
						if (itemClone)
						{
							duplicateItems << itemClone;
							if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(itemClone)))
							{
								connectionItems += connection;
								if (connection->centerRegionItem)
								{
									node1 = connection->centerRegionItem;
									connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i]);
									if (connection && connection->centerRegionItem && connection->centerRegionItem->scene())
									{
										node2 = connection->centerRegionItem;
										if (node1 && node2)
										{
											originalsAndClones += QPair<NodeGraphicsItem*,NodeGraphicsItem*>(node2,node1);
											duplicateItems << node1;
										}
									}
								}
							}
							else
								if ((node1 = qgraphicsitem_cast<NodeGraphicsItem*>(items[i]))
									&& (node2 = qgraphicsitem_cast<NodeGraphicsItem*>(itemClone)))
								{
									originalsAndClones += QPair<NodeGraphicsItem*,NodeGraphicsItem*>(node1,node2);
								}
						}
					}
				}
			}
		/*
		//remove duplicated arrowheads
		for (int i=0; i < arrowHeads.size(); ++i)
		{
		if (arrowHeads[i])
		for (int j=0; j < originalsAndClones.size(); ++j)
		{
		if (arrowHeads[i] == originalsAndClones[j].first)
		{
		duplicateItems.removeAll(originalsAndClones[j].second);
		delete originalsAndClones[j].second;
		}
		}
		}*/
		//replace the duplicated connections' nodes with the duplicate nodes
		for (int i=0; i < connectionItems.size(); ++i)
		{
			QList<NodeGraphicsItem*> nodes = connectionItems[i]->nodes();
			for (int k=0; k < originalsAndClones.size(); ++k)
			{
				if (nodes.contains(originalsAndClones[k].first) && originalsAndClones[k].first && originalsAndClones[k].second)
				{
					connectionItems[i]->replaceNode(originalsAndClones[k].first,originalsAndClones[k].second);
				}
			}
		}

		QList<ItemHandle*> allNewHandles;
		for (int i=0; i < originalAndCloneHandles.size(); ++i)
		{
			if (originalAndCloneHandles[i].second && !allNewHandles.contains(originalAndCloneHandles[i].second))
			{
				allNewHandles << originalAndCloneHandles[i].second;
				allNewHandles << originalAndCloneHandles[i].second->allChildren();
			}
		}
		//replace parent handles
		bool parentCopied = false;
		for (int i=0; i < originalAndCloneHandles.size(); ++i)
		{
			if (originalAndCloneHandles[i].first->parent != 0)
			{
				parentCopied = false;
				for (int j=0; j < originalAndCloneHandles.size(); ++j)
				{
					if (originalAndCloneHandles[i].first->parent == originalAndCloneHandles[j].first)
					{
						originalAndCloneHandles[i].second->setParent(originalAndCloneHandles[j].second);
						parentCopied = true;
						break;
					}
				}
				if (!parentCopied)
				{
					QString oldName = originalAndCloneHandles[i].second->fullName();
					originalAndCloneHandles[i].second->setParent(0);
					QString newName = originalAndCloneHandles[i].second->fullName();
					RenameCommand::findReplaceAllHandleData(allNewHandles,oldName,newName);
				}
			}
		}

		emit itemsInserted(scene,items2,handles);
		scene->select(items2);
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
				if (!selectedItems.contains(handle->graphicsItems[i]))
				{
					alreadySelected = false;
					break;
				}
				if (!alreadySelected)
				{
					selectedItems.clear();
					selectedItems = handle->graphicsItems;
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
						if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(list[j])))
						{
							if (node->isVisible() && !selectedItems.contains(node))
								selectedItems += node;
						}
						else
						{
							if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(list[j])))
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

	QList<QGraphicsItem*> GraphicsScene::duplicateItems;
	GraphicsScene* GraphicsScene::copiedFromScene;
}
