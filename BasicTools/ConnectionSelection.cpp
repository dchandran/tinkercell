/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This class performs some housekeeping operations for selecting and moving connection items.
Connection items cannot be moved by themselves since their location is defined by the control
points.

****************************************************************************/
#include <math.h>
#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConnectionSelection.h"
#include "CollisionDetection.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "NodeGraphicsWriter.h"

namespace Tinkercell
{

	ConnectionSelection::ConnectionSelection() : Tool(tr("Connection Selection"),tr("Basic GUI")),
		addControlPointAction("Add control point",this),
		useStraightLinesAction("Use straight lines",this),
		useCurvesAction("Use Bezier curves",this),
		showMiddleRect("Show middle region",this),
		hideMiddleRect("Hide middle region",this)
		//connectorsMenu("Connectors appearance",this)
	{
		separator = 0;
		mainWindow = 0;
		controlHeld = false;
		gridDist = 100.0;
		connectTCFunctions();

		addControlPointAction.setIcon(QIcon(":/images/blueDot.png"));
		addControlPointAction.setToolTip("Insert new control point");
		connect(&addControlPointAction,SIGNAL(triggered()),this,SLOT(newControlPoint()));

		useStraightLinesAction.setToolTip("Use straight lines to draw the selected connectors");
		connect(&useStraightLinesAction,SIGNAL(triggered()),this,SLOT(setLineTypeStraight()));

		useCurvesAction.setToolTip("Use Bezier curves to draw the selected connectors");
		connect(&useCurvesAction,SIGNAL(triggered()),this,SLOT(setLineTypeCurved()));

		showMiddleRect.setIcon(QIcon(":/images/blueRect.png"));
		showMiddleRect.setToolTip("Show information box for connector(s)");
		connect(&showMiddleRect,SIGNAL(triggered()),this,SLOT(showMiddleBox()));

		hideMiddleRect.setToolTip("Hide information box for connector(s)");
		connect(&hideMiddleRect,SIGNAL(triggered()),this,SLOT(hideMiddleBox()));

		/*connectorsMenu.addAction(&addControlPointAction);
		connectorsMenu.addAction(&useStraightLinesAction);
		connectorsMenu.addAction(&useCurvesAction);
		connectorsMenu.addAction(&showMiddleRect);
		connectorsMenu.addAction(&hideMiddleRect);*/
	}

	bool ConnectionSelection::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSignal(const QWidget*)));

			connect(mainWindow,SIGNAL(keyPressed(GraphicsScene *, QKeyEvent*)),
				this,SLOT(sceneKeyPressed(GraphicsScene *, QKeyEvent*)));

			connect (mainWindow,SIGNAL(itemsAboutToBeRemoved(GraphicsScene*, QList<QGraphicsItem*>&, QList<ItemHandle*>&)),
				this,SLOT(itemsRemoved(GraphicsScene*, QList<QGraphicsItem*>&, QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(keyReleased(GraphicsScene *, QKeyEvent*)),
				this,SLOT(sceneKeyReleased(GraphicsScene *, QKeyEvent*)));

			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				this,SLOT(itemsSelected(GraphicsScene *,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)),
				this,SLOT(sceneDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(this, SIGNAL(itemsInserted(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
				mainWindow, SIGNAL(itemsInserted(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)),
				this ,SLOT(itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(mouseMoved(GraphicsScene*, QGraphicsItem*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)),
				this,SLOT(mouseMoved(GraphicsScene*, QGraphicsItem*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)));

			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			connectCollisionDetector();

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			return true;
		}

		return false;
	}

	void ConnectionSelection::toolLoaded(Tool*)
	{
		connectCollisionDetector();
	}

	typedef void (*tc_ConnectionSelection_api)(
		double (*getControlPointX)(void*,void*,int),
		double (*getControlPointY)(void*,void*,int),
		void (*setControlPoint)(void*,void*,int,double,double),
		void (*setCenterPoint)(void*,double,double),
		double (*getCenterPointX)(void*),
		double (*getCenterPointY)(void*),
		void (*setStraight)(void*,int),
		void (*setAllStraight)(int),
		void (*setLineWidth)(void*,double,int)
		);


	void ConnectionSelection::setupFunctionPointers( QLibrary * library )
	{
		tc_ConnectionSelection_api f = (tc_ConnectionSelection_api)library->resolve("tc_ConnectionSelection_api");
		if (f)
		{
			//qDebug() << "tc_ConnectionSelection_api resolved";
			f(
				&(_getControlPointX),
				&(_getControlPointY),
				&(_setControlPoint),
				&(_setCenterPoint),
				&(_getCenterPointX),
				&(_getCenterPointY),
				&(_setStraight),
				&(_setAllStraight),
				&(_setLineWidth)
				);
		}
	}

	void ConnectionSelection::setStraight(QSemaphore* sem,ItemHandle* h,int value)
	{
		if (h && !h->graphicsItems.isEmpty() && currentScene())
		{
			LineTypeChanged * command = new LineTypeChanged;
			command->straight = value;
			if (value)
				command->setText("make straight");
			else
				command->setText("make curved");

			for (int j=0; j < h->graphicsItems.size(); ++j)
			{
				QGraphicsItem * item1 = h->graphicsItems[j];
				ConnectionGraphicsItem * connection = ConnectionGraphicsItem::topLevelConnectionItem(item1);
				if (connection)
					command->list += connection;
			}

			if (!command->list.isEmpty())
			{
				if (currentScene()->historyStack)
					currentScene()->historyStack->push(command);
				else
				{
					command->redo();
					delete command;
				}
			}
		}
		if (sem)
			sem->release();
	}

	void ConnectionSelection::setAllStraight(QSemaphore* sem,int value)
	{
		if (currentScene())
		{
			QList<QGraphicsItem*> items = currentScene()->items();
			LineTypeChanged * command = new LineTypeChanged;
			command->straight = value;
			if (value)
				command->setText("make straight");
			else
				command->setText("make curved");

			for (int j=0; j < items.size(); ++j)
			{
				QGraphicsItem * item1 = items[j];
				ConnectionGraphicsItem * connection = ConnectionGraphicsItem::topLevelConnectionItem(item1);
				if (connection)
					command->list += connection;
			}

			if (!command->list.isEmpty())
			{
				if (currentScene()->historyStack)
					currentScene()->historyStack->push(command);
				else
				{
					command->redo();
					delete command;
				}
			}
		}
		if (sem)
			sem->release();
	}

	void ConnectionSelection::setLineWidth(QSemaphore* sem,ItemHandle* h,qreal value,int permanent)
	{
		if (h && !h->graphicsItems.isEmpty() && currentScene())
		{
			for (int j=0; j < h->graphicsItems.size(); ++j)
			{
				QGraphicsItem * item1 = h->graphicsItems[j];
				ConnectionGraphicsItem * connection = ConnectionGraphicsItem::topLevelConnectionItem(item1);
				if (connection)
				{
					QPen pen;
					if (currentScene()->selected().contains(connection))
						pen = connection->defaultPen;
					else
						pen = connection->pen();
					pen.setWidthF(value);

					if (permanent > 0)
						currentScene()->setPen(h->name + tr(" pen changed"),connection,pen);
					else
					{
						connection->setPen(pen);
						temporarilyChangedConnections << connection;
					}
				}
			}
		}
		if (sem)
			sem->release();
	}

	void ConnectionSelection::connectTCFunctions()
	{
		connect(&fToS,SIGNAL(getControlPointX(QSemaphore*,qreal*,ItemHandle*,ItemHandle*,int)),
			this,SLOT(getControlPointX(QSemaphore*,qreal*,ItemHandle*,ItemHandle*,int)));

		connect(&fToS,SIGNAL(getControlPointY(QSemaphore*,qreal*,ItemHandle*,ItemHandle*,int)),
			this,SLOT(getControlPointY(QSemaphore*,qreal*,ItemHandle*,ItemHandle*,int)));

		connect(&fToS,SIGNAL(setControlPoint(QSemaphore*,ItemHandle*,ItemHandle*,int,qreal,qreal)),
			this,SLOT(setControlPoint(QSemaphore*,ItemHandle*,ItemHandle*,int,qreal,qreal)));

		connect(&fToS,SIGNAL(getCenterPointX(QSemaphore*,qreal*,ItemHandle*)),
			this,SLOT(getCenterPointX(QSemaphore*,qreal*,ItemHandle*)));

		connect(&fToS,SIGNAL(getCenterPointY(QSemaphore*,qreal*,ItemHandle*)),
			this,SLOT(getCenterPointY(QSemaphore*,qreal*,ItemHandle*)));

		connect(&fToS,SIGNAL(setCenterPoint(QSemaphore*,ItemHandle*,qreal,qreal)),
			this,SLOT(setCenterPoint(QSemaphore*,ItemHandle*,qreal,qreal)));

		connect(&fToS,SIGNAL(setStraight(QSemaphore*,ItemHandle*,int)),
			this,SLOT(setStraight(QSemaphore*,ItemHandle*,int)));

		connect(&fToS,SIGNAL(setAllStraight(QSemaphore*,int)),
			this,SLOT(setAllStraight(QSemaphore*,int)));

		connect(&fToS,SIGNAL(setLineWidth(QSemaphore*,ItemHandle*,qreal,int)),
			this,SLOT(setLineWidth(QSemaphore*,ItemHandle*,qreal,int)));
	}

	void ConnectionSelection::getControlPointX(QSemaphore* sem,qreal* ptr,ItemHandle* h1,ItemHandle* h2,int index)
	{
		if (ptr && h1 && h2 && !h1->graphicsItems.isEmpty() && !h2->graphicsItems.isEmpty())
			for (int j=0; j < h1->graphicsItems.size(); ++j)
			{
				QGraphicsItem * item1 = h1->graphicsItems[j];
				ConnectionGraphicsItem * connection = ConnectionGraphicsItem::topLevelConnectionItem(item1);
				for (int j2=0; j2 < h2->graphicsItems.size(); ++j2)
				{
					NodeGraphicsItem * node = NodeGraphicsItem::topLevelNodeItem(h2->graphicsItems[j2]);
					if (ptr && node && connection)
					{
						int i = connection->indexOf(node);

						if (i > -1 && index > -1 && index < connection->curveSegments[i].size() && connection->curveSegments[i][index])
						{
							if (ptr)
								(*ptr) = connection->curveSegments[i][index]->x();
							if (sem)
								sem->release();
							return;
						}
					}
				}
			}
			if (sem)
				sem->release();
	}

	void ConnectionSelection::getControlPointY(QSemaphore* sem,qreal* ptr,ItemHandle* h1,ItemHandle* h2,int index)
	{
		if (ptr && h1 && h2 && !h1->graphicsItems.isEmpty() && !h2->graphicsItems.isEmpty())
			for (int j=0; j < h1->graphicsItems.size(); ++j)
			{
				QGraphicsItem * item1 = h1->graphicsItems[j];
				ConnectionGraphicsItem * connection = ConnectionGraphicsItem::topLevelConnectionItem(item1);
				for (int j2=0; j2 < h2->graphicsItems.size(); ++j2)
				{
					NodeGraphicsItem * node = NodeGraphicsItem::topLevelNodeItem(h2->graphicsItems[j2]);
					if (ptr && node && connection)
					{
						int i = connection->indexOf(node);
						if (i > -1 && index > -1 && index < connection->curveSegments[i].size() && connection->curveSegments[i][index])
						{
							(*ptr) = connection->curveSegments[i][index]->y();
							if (sem)
								sem->release();
							return;
						}
					}
				}
			}
			if (sem)
				sem->release();
	}

	void ConnectionSelection::setControlPoint(QSemaphore* sem,ItemHandle* h1,ItemHandle* h2,int index,qreal x,qreal y)
	{
		if (h1 && h2 && !h1->graphicsItems.isEmpty() && !h2->graphicsItems.isEmpty())
			for (int j=0; j < h1->graphicsItems.size(); ++j)
			{
				QGraphicsItem * item1 = h1->graphicsItems[j];
				ConnectionGraphicsItem * connection = ConnectionGraphicsItem::topLevelConnectionItem(item1);
				for (int j2=0; j2 < h2->graphicsItems.size(); ++j2)
				{
					NodeGraphicsItem * node = NodeGraphicsItem::topLevelNodeItem(h2->graphicsItems[j2]);
					if (node && connection)
					{
						int i = connection->indexOf(node);
						if (i > -1 && index > -1 && index < connection->curveSegments[i].size() && connection->curveSegments[i][index])
						{
							QPointF diff(x - connection->curveSegments[i][index]->x(),
								y - connection->curveSegments[i][index]->y());

							GraphicsScene * scene = currentScene();
							if (scene)
								scene->move(connection->curveSegments[i][index],diff);
							else
							{
								connection->curveSegments[i][index]->setPos(QPointF(x,y));
							}
							if (sem)
								sem->release();
							return;
						}
					}
				}
			}
			if (sem)
				sem->release();
	}

	void ConnectionSelection::setCenterPoint(QSemaphore* sem,ItemHandle* h1,qreal x,qreal y)
	{
		if (h1)
			for (int i=0; i < h1->graphicsItems.size(); ++i)
			{
				ConnectionGraphicsItem * connection = ConnectionGraphicsItem::topLevelConnectionItem(h1->graphicsItems[i]);
				if (connection)
				{
					ControlPoint * cp = connection->centerPoint();
					if (cp)
					{
						GraphicsScene * scene = currentScene();
						if (scene)
							scene->move(cp,(QPointF(x,y) - cp->scenePos()));
						else
							cp->setPos(QPointF(x,y));
					}
				}
			}
			if (sem)
				sem->release();
	}

	void ConnectionSelection::getCenterPointX(QSemaphore* sem,qreal* ptr, ItemHandle* h1)
	{
		if (ptr && h1)
			for (int i=0; i < h1->graphicsItems.size(); ++i)
			{
				ConnectionGraphicsItem * connection = ConnectionGraphicsItem::topLevelConnectionItem(h1->graphicsItems[i]);
				if (connection && ptr)
				{
					ControlPoint * cp = connection->centerPoint();
					if (cp)
					{
						(*ptr) = cp->x();
					}
				}
			}
			if (sem)
				sem->release();
	}

	void ConnectionSelection::getCenterPointY(QSemaphore* sem,qreal* ptr, ItemHandle* h1)
	{
		if (h1)
			for (int i=0; i < h1->graphicsItems.size(); ++i)
			{
				ConnectionGraphicsItem * connection = ConnectionGraphicsItem::topLevelConnectionItem(h1->graphicsItems[i]);
				if (connection && ptr)
				{
					ControlPoint * cp = connection->centerPoint();
					if (cp)
					{
						(*ptr) = cp->y();
					}
				}
			}
			if (sem)
				sem->release();
	}

	void ConnectionSelection::connectCollisionDetector()
	{
		static bool alreadyConnected = false;
		if (alreadyConnected || !mainWindow) return;

		if (mainWindow->tool(tr("Collision Detection")))
		{
			QWidget * widget = mainWindow->tool(tr("Collision Detection"));
			CollisionDetection * collisionDetection = static_cast<CollisionDetection*>(widget);
			if (collisionDetection)
			{
				alreadyConnected = true;
				//connect(collisionDetection,SIGNAL(nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& , Qt::KeyboardModifiers )),
					//this, SLOT( nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& , Qt::KeyboardModifiers )));
			}
		}
	}

	void ConnectionSelection::newControlPoint()
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !scene->historyStack) return;

		QPointF point = scene->lastPoint();
		QList<QGraphicsItem*>& selected = scene->selected();

		for (int i=0; i < selected.size(); ++i)
		{
			ConnectionGraphicsItem * item = ConnectionGraphicsItem::topLevelConnectionItem(selected[i]);
			if (item)
			{
				ConnectionGraphicsItem::ControlPoint * cp = new ConnectionGraphicsItem::ControlPoint(item);
				cp->setPos(point);
				scene->addItem(cp);
				scene->historyStack->push(new AddControlPointCommand("control point added",scene,cp));
			}
		}
	}

	void ConnectionSelection::sceneDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem * qitem, Qt::MouseButton , Qt::KeyboardModifiers modifiers)
	{
		ConnectionGraphicsItem * item = ConnectionGraphicsItem::topLevelConnectionItem(qitem);
		if (scene && item)
		{
			if (modifiers == Qt::ControlModifier)
			{
				ConnectionGraphicsItem::ControlPoint * cp = new ConnectionGraphicsItem::ControlPoint(item);
				cp->setPos(point);
				scene->addItem(cp);
				if (scene->historyStack)
					scene->historyStack->push(new AddControlPointCommand("control point added",scene,cp));
			}
		}
	}

	void ConnectionSelection::adjustConnectorPoints(const QList<QGraphicsItem*>& movingItems)
	{
		QList<ConnectionGraphicsItem*> visited;
		for (int i=0; i < movingItems.size(); ++i)
		{
			ConnectionGraphicsItem::ControlPoint * cp = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(movingItems[i]);
			if (cp != 0 && cp->connectionItem != 0 && !visited.contains(cp->connectionItem))
			{
				if (cp->connectionItem->lineType == ConnectionGraphicsItem::line)
				{
					QPointF closest, p1,p2,p;
					p1 = cp->scenePos();
					for (int i1=0; i1 < cp->connectionItem->curveSegments.size(); ++i1)
					{
						for (int j=0; j < cp->connectionItem->curveSegments[i1].size(); ++j)
							if (cp->connectionItem->curveSegments[i1][j] && (cp->connectionItem->curveSegments[i1][j]->parentItem() == 0) &&
								cp->connectionItem->lineType == ConnectionGraphicsItem::line && (j%3) == 0)
							{
								p2 = cp->connectionItem->curveSegments[i1][j]->scenePos();

								if (p1 != p2)
								{
									if ((p1.rx() - p2.rx())*(p1.rx() - p2.rx()) <
										(p1.rx() - p.rx())*(p1.rx() - p.rx()))
										p.rx() = p2.rx();
									if ((p1.ry() - p2.ry())*(p1.ry() - p2.ry()) <
										(p1.ry() - p.ry())*(p1.ry() - p.ry()))
										p.ry() = p2.ry();
								}
							}
					}
					if (!p.isNull())
					{
						if ((p.rx() - p1.rx())*(p.rx() - p1.rx()) < gridDist*2.0)
							cp->topLevelItem()->moveBy( p.x() - p1.x(), 0 );
						if ((p.ry() - p1.ry())*(p.ry() - p1.ry()) < gridDist*2.0)
							cp->topLevelItem()->moveBy( 0 , p.y() - p1.y() );
					}
				}
				cp->connectionItem->refresh();
				visited.append(cp->connectionItem);
			}
		}
	}

	void ConnectionSelection::itemsMoved(GraphicsScene * ,const QList<QGraphicsItem*>& movingItems, const QList<QPointF>& , Qt::KeyboardModifiers )
	{
		adjustConnectorPoints(movingItems);
	}

	void ConnectionSelection::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers )
	{
		if (mainWindow && scene && scene->useDefaultBehavior)
		{
			bool hasConnections = false;
			for (int i=0; i < items.size(); ++i)
				if (ConnectionGraphicsItem::topLevelConnectionItem(items[i]))
				{
					hasConnections = true;
					break;
				}

				if (hasConnections)
				{
					if (separator)
						mainWindow->contextItemsMenu.addAction(separator);
					else
						separator = mainWindow->contextItemsMenu.addSeparator();
					if (items.size() == 1 && ConnectionGraphicsItem::topLevelConnectionItem(items[0]))
						mainWindow->contextItemsMenu.addAction(&addControlPointAction);
					else
						mainWindow->contextItemsMenu.removeAction(&addControlPointAction);
					mainWindow->contextItemsMenu.addAction(&useStraightLinesAction);
					mainWindow->contextItemsMenu.addAction(&useCurvesAction);
					mainWindow->contextItemsMenu.addAction(&showMiddleRect);
					mainWindow->contextItemsMenu.addAction(&hideMiddleRect);
				}
				else
				{
					if (separator)
						mainWindow->contextItemsMenu.removeAction(separator);
					mainWindow->contextItemsMenu.removeAction(&addControlPointAction);
					mainWindow->contextItemsMenu.removeAction(&useStraightLinesAction);
					mainWindow->contextItemsMenu.removeAction(&useCurvesAction);
					mainWindow->contextItemsMenu.removeAction(&showMiddleRect);
					mainWindow->contextItemsMenu.removeAction(&hideMiddleRect);
				}

				ConnectionGraphicsItem * connection = 0;
				NodeGraphicsItem * node = 0;
				QList<QGraphicsItem*>& movingItems = scene->moving();
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
								if (list[i] && list[i]->isVisible() && !list[i]->parentItem())
									controlPointsExist = true;
								
								if (list[i] && list[i]->isVisible() && list[i]->sceneBoundingRect().contains(point))
								{
									noControlsSelected = false;
									break;
								}
							}

							if (noControlsSelected)
							{
								//connection->setControlPointsVisible(true);

								for (int i=0; i < list.size(); ++i)
									if (!movingItems.contains(list[i]) && list[i]->scene() == scene)
										movingItems += list[i];

								ItemHandle * handle = connection->handle();
								if (handle && controlPointsExist)
									for (int i=0; i < handle->graphicsItems.size(); ++i)
									{
										if (TextGraphicsItem::cast(handle->graphicsItems[i])
											&& !movingItems.contains(handle->graphicsItems[i])
											&& handle->graphicsItems[i]->scene() == scene)
											movingItems += handle->graphicsItems[i];
									}
							}
						}
						else
						{
							if ((node = NodeGraphicsItem::topLevelNodeItem(movingItems[i])) && (node->className == ArrowHeadItem::CLASSNAME))
							{
								movingItems.removeAll(node);
							}
						}
					}

		}
	}

	void ConnectionSelection::itemsRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& items, QList<ItemHandle*>& handles)
	{
		if (!scene || items.isEmpty()) return;

		ConnectionGraphicsItem::ControlPoint * p = 0;
		QList<ConnectionGraphicsItem::ControlPoint*> points;
		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;

		for (int i=0; i < items.size(); ++i)
		{
			p = 0;
			points.clear();
			connection = ConnectionGraphicsItem::cast(items[i]);

			if (connection)
			{
				connection->setControlPointsVisible(false);
				continue;
			}

			node = NodeGraphicsItem::cast(items[i]);

			if (node)
			{
				QList<QGraphicsItem*> children = node->childItems();
				for (int j=0; j < children.size(); ++j)
				{
					if ((p = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(children[j])) &&
						p->connectionItem && p->connectionItem->scene() == scene)
						points << p;
				}
			}
			else
			{
				p = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(items[i]);
				if (p)
					points << p;
			}
			for (int j=0; j < points.size(); ++j)
			{
				if ((p = points[j]) && !items.contains(p->connectionItem))
				{
					//items.removeAt(i);
					RemoveControlPointCommand * cmmd1 = new RemoveControlPointCommand("control point removed",scene,p);
					cmmd1->redo();
					cmmd1->undo();

					if (cmmd1->graphicsItems.size() >= 1)
					{
						if (scene->historyStack)
							scene->historyStack->push(cmmd1);
					}
					else
					{
						RemoveCurveSegmentCommand * cmmd2 = new RemoveCurveSegmentCommand("path removed",scene,p);
						if (cmmd2->curveSegments.size() > 0)
						{
							if (scene->historyStack)
								scene->historyStack->push(cmmd2);
							else
							{
								cmmd2->redo();
								delete cmmd2;
							}
						}
						else
						{
							if (p->connectionItem && !items.contains(p->connectionItem))
							{
								QString s;
								if (p->connectionItem->handle())
									handles += p->connectionItem->handle();

								items += p->connectionItem;
							}
						}
					}

					if (p)
						p->setVisible(false);
				}
				else
				{
					connection = ConnectionGraphicsItem::topLevelConnectionItem(items[i]);
					if (connection != 0)
						connection->setControlPointsVisible(false);
				}
			}
		}
	}

	void ConnectionSelection::escapeSignal(const QWidget*)
	{
		if (temporarilyChangedConnections.isEmpty()) return;

		for (int i=0; i < temporarilyChangedConnections.size(); ++i)
			if (temporarilyChangedConnections[i])
			{
				temporarilyChangedConnections[i]->setPen(temporarilyChangedConnections[i]->defaultPen);
				temporarilyChangedConnections[i]->setBrush(temporarilyChangedConnections[i]->defaultBrush);
				temporarilyChangedConnections[i]->update();
			}
			temporarilyChangedConnections.clear();
	}

	void ConnectionSelection::sceneKeyPressed(GraphicsScene *scene, QKeyEvent* keyEvent)
	{
		if (!scene || !keyEvent || !scene->useDefaultBehavior) return;

		if (keyEvent->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
		{
			QList<QGraphicsItem*> items = scene->items( scene->viewport() );
			NodeGraphicsItem * node = 0;
			ConnectionGraphicsItem * connection = 0;

			for (int i=0; i < items.size(); ++i)
			{
				node = NodeGraphicsItem::topLevelNodeItem(items[i]);
				if (node && (Tool::GraphicsItem::cast(node->topLevelItem()) == 0))
				{
					node->setBoundingBoxVisible(true);
				}
				else
				{
					connection = ConnectionGraphicsItem::topLevelConnectionItem(items[i]);
					if (connection)
					{
						connection->setControlPointsVisible(true);
					}
				}
			}
			controlHeld = true;
			return;

		}
	}

	void ConnectionSelection::sceneKeyReleased(GraphicsScene * scene, QKeyEvent * )
	{
		if (controlHeld && scene)
		{
			QList<QGraphicsItem*> items = scene->items( scene->viewport() );
			NodeGraphicsItem * node = 0;
			ConnectionGraphicsItem * connection = 0;

			for (int i=0; i < items.size(); ++i)
			{
				node = NodeGraphicsItem::topLevelNodeItem(items[i]);
				if (node && !scene->selected().contains(node))
				{
					node->setBoundingBoxVisible(false);
				}
				else
				{
					connection = ConnectionGraphicsItem::topLevelConnectionItem(items[i]);
					if (connection && !scene->selected().contains(connection))
					{
						connection->setControlPointsVisible(false);
					}
				}
			}
			controlHeld = false;
		}
	}

	void ConnectionSelection::nodeCollided(const QList<QGraphicsItem*>& movingItems, NodeGraphicsItem * nodeBelow, const QList<QPointF>& , Qt::KeyboardModifiers )
	{
		if (!nodeBelow || !mainWindow) return;
		GraphicsScene * scene = mainWindow->currentScene();
		ItemHandle * nodeHandle = getHandle(nodeBelow);
		if (!nodeHandle || !nodeHandle->family() || !scene) return;

		for (int i=0; i < movingItems.size(); ++i)
		{
			NodeGraphicsItem * node = NodeGraphicsItem::cast(movingItems[i]);
			if (node && node->className == ArrowHeadItem::CLASSNAME)
			{
				ArrowHeadItem * arrow = static_cast<ArrowHeadItem*>(node);
				if (arrow && arrow->connectionItem)
				{
					ItemHandle * handle = 0;
					int k = arrow->connectionItem->arrowHeads().indexOf(arrow);
					if (k > -1)
					{
						handle = getHandle( arrow->connectionItem->nodeAt(k) );
					}

					if (handle && handle->family() && nodeHandle->family()->isA(handle->family()))
					{
						QList<ArrowHeadItem*> arrows = arrow->connectionItem->arrowHeads();
						int k = arrows.indexOf(arrow);
						if (k > -1)
						{
							ConnectionGraphicsItem * copy = arrow->connectionItem->clone();
							copy->replaceNodeAt(k,nodeBelow);
							scene->remove(tr("remove old reaction"),arrow->connectionItem);
							scene->insert(tr("insert new reaction"),copy);
						}
					}
				}
			}
		}
	}

	void ConnectionSelection::mouseMoved(GraphicsScene * scene, QGraphicsItem* , QPointF , Qt::MouseButton, Qt::KeyboardModifiers modifiers, QList<QGraphicsItem*>& moving)
	{
		if (!scene || moving.isEmpty()) return;

		if (!moving.at(0)) return;

		MoveCommand::refreshAllConnectionIn(moving);

		QGraphicsItem * cp = 0;
		for (int i=0; i < moving.size(); ++i)
		{
			if (ControlPoint::cast(moving.at(i)))
			{
				cp = moving.at(i);
				break;
			}
			else
				if (NodeGraphicsItem::topLevelNodeItem(moving.at(i)))
				{
					cp = NodeGraphicsItem::topLevelNodeItem(moving.at(i));
					break;
				}
		}

		if (!cp) return;

		bool avoidBoundary = false;

		if (!controlHeld && !(modifiers == (Qt::ControlModifier | Qt::ShiftModifier)))
		{
			return;
		}

		QRectF viewport = scene->viewport();
		QList<QGraphicsItem*> items = scene->items( viewport );
		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;

		QPointF p = cp->scenePos();
		QPointF closest;

		for (int i=0; i < items.size(); ++i)
		{
			node = NodeGraphicsItem::topLevelNodeItem(items[i]);
			if (node && !scene->moving().contains(node) && (Tool::GraphicsItem::cast(node->topLevelItem()) == 0))
			{
				if (!avoidBoundary)
					for (int j=0; j < node->boundaryControlPoints.size(); ++j)
					{
						if (node->boundaryControlPoints[j] && !scene->moving().contains(node->boundaryControlPoints[j]))
						{
							if ((closest.isNull() || ((p.x() - closest.x())*(p.x() - closest.x()) > (p.x() - node->boundaryControlPoints[j]->x())*(p.x() - node->boundaryControlPoints[j]->x())))
								&& (gridDist > (p.x() - node->boundaryControlPoints[j]->x())*(p.x() - node->boundaryControlPoints[j]->x()))
								)
								closest.rx() = node->boundaryControlPoints[j]->x();

							if ((closest.isNull() || ((p.y() - closest.y())*(p.y() - closest.y()) > (p.y() - node->boundaryControlPoints[j]->y())*(p.y() - node->boundaryControlPoints[j]->y())))
								&& (gridDist > (p.y() - node->boundaryControlPoints[j]->y())*(p.y() - node->boundaryControlPoints[j]->y()))
								)
								closest.ry() = node->boundaryControlPoints[j]->y();
						}
					}

					if ((closest.isNull() || ((p.x() - closest.x())*(p.x() - closest.x()) > (p.x() - node->scenePos().x())*(p.x() - node->scenePos().x())))
						&& (gridDist > (p.x() - node->scenePos().x())*(p.x() - node->scenePos().x()))
						)
						closest.rx() = node->scenePos().x();

					if ((closest.isNull() || ((p.y() - closest.y())*(p.y() - closest.y()) > (p.y() - node->scenePos().y())*(p.y() - node->scenePos().y())))
						&& (gridDist > (p.y() - node->scenePos().y())*(p.y() - node->scenePos().y()))
						)
						closest.ry() = node->scenePos().y();
			}
			else
			{
				connection = ConnectionGraphicsItem::topLevelConnectionItem(items[i]);
				if (connection && !scene->moving().contains(connection))
				{
					QList<ConnectionGraphicsItem::ControlPoint*> controlPoints = connection->controlPoints();
					for (int j=0; j < controlPoints.size(); ++j)
					{
						if (controlPoints[j] && controlPoints[j]->isVisible() && !scene->moving().contains(controlPoints[j]))
						{
							if ((closest.isNull() || ((p.x() - closest.x())*(p.x() - closest.x()) > (p.x() - controlPoints[j]->x())*(p.x() - controlPoints[j]->x())))
								&& (gridDist > (p.x() - controlPoints[j]->x())*(p.x() - controlPoints[j]->x()))
								)
								closest.rx() = controlPoints[j]->x();

							if ((closest.isNull() || ((p.y() - closest.y())*(p.y() - closest.y()) > (p.y() - controlPoints[j]->y())*(p.y() - controlPoints[j]->y())))
								&& (gridDist > (p.y() - controlPoints[j]->y())*(p.y() - controlPoints[j]->y()))
								)
								closest.ry() = controlPoints[j]->y();
						}
					}
				}
			}
		}

		if (closest.x() == 0)
			closest.rx() = p.x();

		if (closest.y() == 0)
			closest.ry() =p.y();

		cp->topLevelItem()->moveBy( closest.x() - p.x(), closest.y() - p.y() );
	}

	void ConnectionSelection::connectionCollided(const QList<QGraphicsItem*>& , ConnectionGraphicsItem * , QPointF , Qt::KeyboardModifiers )
	{
	}

	/****************************************/

	ConnectionSelection_FToS ConnectionSelection::fToS;

	double ConnectionSelection::_getControlPointX(void* a,void* b,int c)
	{
		return fToS.getControlPointX(a,b,c);
	}

	double ConnectionSelection::_getControlPointY(void* a,void* b,int c)
	{
		return fToS.getControlPointY(a,b,c);
	}

	void ConnectionSelection::_setControlPoint(void* a,void* b,int i, double x,double y)
	{
		return fToS.setControlPoint(a,b,i,x,y);
	}

	double ConnectionSelection::_getCenterPointX(void* x)
	{
		return fToS.getCenterPointX(x);
	}

	double ConnectionSelection::_getCenterPointY(void* x)
	{
		return fToS.getCenterPointY(x);
	}

	void ConnectionSelection::_setCenterPoint(void* a,double x,double y)
	{
		return fToS.setCenterPoint(a,x,y);
	}

	double ConnectionSelection_FToS::getControlPointX(void* a0,void* a1,int a2)
	{
		QSemaphore * s = new QSemaphore(1);
		qreal p = 0.0;
		s->acquire();
		emit getControlPointX(s,&p,ConvertValue(a0),ConvertValue(a1),a2);
		s->acquire();
		s->release();
		delete s;
		return (double)p;
	}

	double ConnectionSelection_FToS::getControlPointY(void* a0,void* a1,int a2)
	{
		QSemaphore * s = new QSemaphore(1);
		qreal p = 0.0;
		s->acquire();
		emit getControlPointY(s,&p,ConvertValue(a0),ConvertValue(a1),a2);
		s->acquire();
		s->release();
		delete s;
		return (double)p;
	}


	void ConnectionSelection_FToS::setControlPoint(void* a0,void* a1,int i,double a2,double a3)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setControlPoint(s,ConvertValue(a0),ConvertValue(a1),i,a2,a3);
		s->acquire();
		s->release();
		delete s;
	}

	void ConnectionSelection_FToS::setCenterPoint(void* a0,double a1,double a2)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setCenterPoint(s,ConvertValue(a0),a1,a2);
		s->acquire();
		s->release();
		delete s;
	}


	double ConnectionSelection_FToS::getCenterPointX(void* a0)
	{
		QSemaphore * s = new QSemaphore(1);
		qreal x = 0.0;
		s->acquire();
		emit getCenterPointX(s,&x,ConvertValue(a0));
		s->acquire();
		s->release();
		delete s;
		return (double)x;
	}

	double ConnectionSelection_FToS::getCenterPointY(void* a0)
	{
		QSemaphore * s = new QSemaphore(1);
		qreal x = 0.0;
		s->acquire();
		emit getCenterPointY(s,&x,ConvertValue(a0));
		s->acquire();
		s->release();
		delete s;
		return (double)x;
	}

	void ConnectionSelection::_setStraight(void* o,int v)
	{
		return fToS.setStraight(o,v);
	}


	void ConnectionSelection_FToS::setStraight(void* o,int v)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setStraight(s,ConvertValue(o),v);
		s->acquire();
		s->release();
		return;
	}

	void ConnectionSelection::_setAllStraight(int v)
	{
		return fToS.setAllStraight(v);
	}

	void ConnectionSelection_FToS::setAllStraight(int v)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setAllStraight(s,v);
		s->acquire();
		s->release();
		return;
	}

	void ConnectionSelection::_setLineWidth(void* o,double v, int b)
	{
		return fToS.setLineWidth(o,v,b);
	}

	void ConnectionSelection_FToS::setLineWidth(void* o,double v, int b)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setLineWidth(s,ConvertValue(o),qreal(v),b);
		s->acquire();
		s->release();
		return;
	}

	/**Undo commands***/

	void ConnectionSelection::LineTypeChanged::undo()
	{
		for (int i=0; i < list.size(); ++i)
		{
			ConnectionGraphicsItem * connectionPtr = list[i];
			if (connectionPtr)
			{
				if (!straight)
					connectionPtr->lineType = ConnectionGraphicsItem::line;
				else
					connectionPtr->lineType = ConnectionGraphicsItem::bezier;
				connectionPtr->refresh();
			}
		}
	}

	void ConnectionSelection::LineTypeChanged::redo()
	{
		for (int i=0; i < list.size(); ++i)
		{
			ConnectionGraphicsItem * connectionPtr = list[i];
			if (connectionPtr)
			{
				if (straight)
					connectionPtr->lineType = ConnectionGraphicsItem::line;
				else
					connectionPtr->lineType = ConnectionGraphicsItem::bezier;
				connectionPtr->refresh();
			}
		}
	}
	/*
	void ConnectionSelection::ShowHideMiddleRegion::undo()
	{
	for (int i=0; i < list.size(); ++i)
	{
	ConnectionGraphicsItem * connectionPtr = list[i];
	if (!connectionPtr) continue;

	if (!show)
	{
	if (!connectionPtr->centerRegionItem)
	{
	ArrowHeadItem * node = new ArrowHeadItem;
	NodeGraphicsReader imageReader;
	imageReader.readXml(node,filename);
	if (node->isValid())
	{
	node->normalize();
	node->scale(25.0/node->sceneBoundingRect().height(),25.0/node->sceneBoundingRect().height());
	connectionPtr->centerRegionItem = node;
	}
	else
	{
	delete node;
	}
	}
	else
	if (currentScene())
	{
	currentScene()->addItem(connectionPtr->centerRegionItem);
	connectionPtr->centerRegionItem->setVisible(true);
	}
	}
	else
	{
	if (connectionPtr->centerRegionItem)
	{
	if (currentScene())
	currentScene()->remove(connectionPtr->centerRegionItem);
	//connectionPtr->centerRegionItem->setVisible(false);
	//connectionPtr->centerRegionItem = 0;
	}
	}
	connectionPtr->refresh();
	}
	}

	void ConnectionSelection::ShowHideMiddleRegion::redo()
	{
	for (int i=0; i < list.size(); ++i)
	{
	ConnectionGraphicsItem * connectionPtr = list[i];
	if (!connectionPtr) continue;

	if (show)
	{
	if (!connectionPtr->centerRegionItem)
	{
	ArrowHeadItem * node = new ArrowHeadItem;
	NodeGraphicsReader imageReader;
	imageReader.readXml(node,filename);
	if (node->isValid())
	{
	node->normalize();
	node->scale(25.0/node->sceneBoundingRect().height(),25.0/node->sceneBoundingRect().height());
	connectionPtr->centerRegionItem = node;
	}
	else
	{
	delete node;
	}
	}
	else
	if (currentScene())
	{
	currentScene()->addItem(connectionPtr->centerRegionItem);
	connectionPtr->centerRegionItem->setVisible(true);
	}
	}
	else
	{
	if (connectionPtr->centerRegionItem)
	{
	if (currentScene())
	currentScene()->remove(connectionPtr->centerRegionItem);
	//connectionPtr->centerRegionItem->setVisible(false);
	//connectionPtr->centerRegionItem = 0;
	}
	}
	connectionPtr->refresh();
	}
	}
	*/
	void ConnectionSelection::ChangeArrowHeadDistance::undo()
	{
		for (int i=0; i < list.size() && i < dists.size(); ++i)
		{
			ConnectionGraphicsItem * connectionPtr = list[i];
			if (connectionPtr)
			{
				connectionPtr->arrowHeadDistance -= dists[i];
				connectionPtr->refresh();
			}
		}
	}

	void ConnectionSelection::ChangeArrowHeadDistance::redo()
	{
		for (int i=0; i < list.size() && i < dists.size(); ++i)
		{
			ConnectionGraphicsItem * connectionPtr = list[i];
			if (connectionPtr)
			{
				connectionPtr->arrowHeadDistance += dists[i];
				connectionPtr->refresh();
			}
		}
	}

	void ConnectionSelection::showMiddleBox()
	{
		showMiddleBox(1);
	}

	void ConnectionSelection::hideMiddleBox()
	{
		showMiddleBox(0);
	}

	void ConnectionSelection::showMiddleBox(int value, const QString& filename)
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		/*ShowHideMiddleRegion * command = new ShowHideMiddleRegion;
		command->show = (value == 1);
		QString appDir = QCoreApplication::applicationDirPath();
		if (filename.isNull() || filename.isEmpty())
		command->filename = appDir + tr("/NodeItems/Rect.xml");
		else
		command->filename = filename;

		if (command->show)
		command->setText("show middle box");
		else
		command->setText("hide middle box");

		QList<QGraphicsItem*> &targetItems = scene->selected();

		for (int i=0; i < targetItems.size(); ++i)
		{
			ConnectionGraphicsItem * connectionPtr = 0;
			if (targetItems[i] &&
				(connectionPtr = ConnectionGraphicsItem::cast(targetItems[i])))
			{
				command->list += connectionPtr;
			}
		}

		if (command->list.isEmpty())
		{
			delete command;
			return;
		}


		if (scene->historyStack)
		scene->historyStack->push(command);
		else
		{
		command->redo();
		delete command;
		}*/

		QList<QGraphicsItem*> centerBoxes;
		QList<QGraphicsItem*> &targetItems = scene->selected();

		QList<QGraphicsItem*> items;
		QList<NodeGraphicsItem*> nodeItems;

		for (int i=0; i < targetItems.size(); ++i)
		{
			ConnectionGraphicsItem * connectionPtr = 0;
			if (targetItems[i] &&
				(connectionPtr = ConnectionGraphicsItem::cast(targetItems[i])))
			{
				if (!connectionPtr->centerRegionItem)
				{
					NodeGraphicsReader imageReader;
					ArrowHeadItem * node = new ArrowHeadItem(connectionPtr);
					imageReader.readXml(node,ConnectionGraphicsItem::DefaultMiddleItemFile);
					if (node->isValid())
					{
						node->normalize();
						node->scale(25.0/node->sceneBoundingRect().height(),25.0/node->sceneBoundingRect().height());
						connectionPtr->centerRegionItem = node;
					}
				}
				if (connectionPtr->centerRegionItem)
				{
					if (connectionPtr->centerRegionItem->scene() != scene)
						items << connectionPtr->centerRegionItem;
					nodeItems << connectionPtr->centerRegionItem;
				}
			}
		}

		if (value)
		{
			QList<ItemHandle*> handles;
			QStringList filenames;


			for (int i=0; i < items.size(); ++i)
			{
				handles << getHandle(items[i]);
				filenames << filename;
			}

			QList<QUndoCommand*> commands;


			if (!filename.isNull() && !filename.isEmpty())
				commands << new ReplaceNodeGraphicsCommand(tr("center box changed"),nodeItems,filenames);

			commands << new InsertGraphicsCommand(tr("center boxes added"),scene,items);

			QUndoCommand * command = new CompositeCommand(tr("center box added"),commands);

			if (scene->historyStack)
				scene->historyStack->push(command);
			else
			{
				command->redo();
				delete command;
			}

			emit itemsInserted(scene, items, handles);

			//scene->insert(tr("center box added"),centerBoxes);
		}
		else
		{
			scene->remove(tr("center box removed"),items);
		}
	}

	void ConnectionSelection::setLineTypeStraight()
	{
		setLineType(1);
	}

	void ConnectionSelection::setLineTypeCurved()
	{
		setLineType(0);
	}

	void ConnectionSelection::setLineType(int value)
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		QList<QGraphicsItem*> &targetItems = scene->selected();

		LineTypeChanged * command = new LineTypeChanged;
		command->straight = (value == 1);
		if (command->straight)
			command->setText("make straight");
		else
			command->setText("make curved");

		for (int i=0; i < targetItems.size(); ++i)
		{
			ConnectionGraphicsItem * connectionPtr = 0;
			if (targetItems[i] &&
				(connectionPtr = ConnectionGraphicsItem::cast(targetItems[i])))
			{
				command->list += connectionPtr;
			}
		}

		if (command->list.isEmpty())
		{
			delete command;
			return;
		}


		if (scene->historyStack)
			scene->historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}
	}

	void ConnectionSelection::arrowHeadDistance(double value)
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		QList<QGraphicsItem*> &targetItems = scene->selected();

		ChangeArrowHeadDistance * command = new ChangeArrowHeadDistance;
		command->setText("change arrow distance");

		for (int i=0; i < targetItems.size(); ++i)
		{
			ConnectionGraphicsItem * connectionPtr = 0;
			if (targetItems[i] &&
				(connectionPtr = ConnectionGraphicsItem::cast(targetItems[i])))
			{
				command->dists += value - connectionPtr->arrowHeadDistance;
				command->list += connectionPtr;
			}
		}

		if (command->list.isEmpty())
		{
			delete command;
			return;
		}

		if (scene->historyStack)
			scene->historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}
	}
}

