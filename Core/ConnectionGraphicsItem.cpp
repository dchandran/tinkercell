/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This is the source file for the connection graphics item, which is a Qt graphics path item
that draws a connection between two or more nodes.

The connection graphics items defines a new control point item inside itself.

This header file also contains the arrow head item class. The arrow head item inherits from
node graphics item and is used to draw the arrow heads at the end of the connection items.


****************************************************************************/

#include <math.h>
#include <QPainterPathStroker>
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "ConnectionGraphicsItem.h"
#include "ItemHandle.h"
#include "NodeGraphicsReader.h"
#include "UndoCommands.h"

namespace Tinkercell
{
	const QString ArrowHeadItem::CLASSNAME = QString("ArrowHeadItem");
	const QString ConnectionGraphicsItem::CLASSNAME = QString("ConnectionGraphicsItem");
	QString ConnectionGraphicsItem::DefaultMiddleItemFile("");
	QString ConnectionGraphicsItem::DefaultArrowHeadFile("");

	ItemHandle * ConnectionGraphicsItem::handle() const
	{
		return itemHandle;
	}

	void ConnectionGraphicsItem::setHandle(ItemHandle * handle)
	{
		if (handle != 0 && !handle->graphicsItems.contains(this))
		{
			handle->graphicsItems += this;
		}

		if (itemHandle)
		{
			if (itemHandle != handle)
			{
				itemHandle->graphicsItems.removeAll(this);
				itemHandle = handle;
			}
		}
		else
		{
			itemHandle = handle;
		}
	}

	ItemHandle * ConnectionGraphicsItem::ControlPoint::handle() const
	{
		if (connectionItem)
			return connectionItem->handle();
		return 0;
	}

	void ConnectionGraphicsItem::ControlPoint::setHandle(ItemHandle * h)
	{
		if (connectionItem)
			connectionItem->setHandle(h);
	}


	ControlPoint * ControlPoint::cast(QGraphicsItem* item)
	{
		if (MainWindow::invalidPointers.contains( (void*)item )) return 0;
		
		ControlPoint * idptr = 0;

		QGraphicsItem * p = item;

		while (p)
		{
			idptr = qgraphicsitem_cast<ControlPoint*>(p);
			if (idptr != 0) return idptr;

			idptr = (ControlPoint*)qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(p);
			if (idptr != 0) return idptr;

			idptr = (ControlPoint*)qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(p);
			if (idptr != 0) return idptr;

			if (p != p->parentItem())
				p = p->parentItem();
			else
				break;
		}

		return 0;

	}

	/*! Constructor: init everything */
	ArrowHeadItem::ArrowHeadItem(ConnectionGraphicsItem* connection) : NodeGraphicsItem()
	{
		className = ArrowHeadItem::CLASSNAME;
		connectionItem = connection;
		angle = 0.0;
	}
	
	/*! Constructor: init everything */
	ArrowHeadItem::ArrowHeadItem(const QString& filename, ConnectionGraphicsItem* connection) : NodeGraphicsItem(filename)
	{
		className = ArrowHeadItem::CLASSNAME;
		connectionItem = connection;
		angle = 0.0;
	}

	/*! Constructor: init everything */
	ArrowHeadItem::ArrowHeadItem(const ArrowHeadItem& copy) : NodeGraphicsItem(copy)
	{
		connectionItem = copy.connectionItem;
		angle = copy.angle;
	}
	
	/*! \brief make a copy of this item*/
	NodeGraphicsItem* ArrowHeadItem::clone() const
	{
		return new ArrowHeadItem(*this);
	}

	/*! Constructor: initialize everything */
	ConnectionGraphicsItem::ConnectionGraphicsItem(QGraphicsItem * parent) : QGraphicsPathItem (parent), itemHandle(0)
	{
		setCacheMode(QGraphicsItem::DeviceCoordinateCache);
		setFlag(QGraphicsItem::ItemIsMovable, false);
		setFlag(QGraphicsItem::ItemIsSelectable, false);

		className = ConnectionGraphicsItem::CLASSNAME;
		lineType = bezier;
		arrowHeadDistance = 10.0;
		centerRegionItem = 0;
		centerRegion = QSizeF(20,20);
		defaultPen = QPen(QColor(50,50,255,255),5.0);
		defaultPen.setJoinStyle(Qt::RoundJoin);
		setPen(defaultPen);
		setBrush(defaultBrush = Qt::NoBrush);
		boundaryPathItem = new QGraphicsPathItem(this);

		boundaryPathItem->setVisible(false);
		boundaryPathItem->setPen(QPen(QColor(255,150,150,150),4.0,Qt::DotLine));
		boundaryPathItem->setBrush(Qt::NoBrush);

		/*ArrowHeadItem * node = new ArrowHeadItem;
		node->connectionItem = this;
		NodeGraphicsReader imageReader;
		imageReader.readXml(node,DefaultMiddleItemFile);
		if (node->isValid())
		{
			node->normalize();
			node->scale(25.0/node->sceneBoundingRect().height(),25.0/node->sceneBoundingRect().height());
		}*/
		centerRegionItem = 0;
	}

	/*! Copy Constructor: deep copy of all pointers */
	ConnectionGraphicsItem::ConnectionGraphicsItem(const ConnectionGraphicsItem& copy) : QGraphicsPathItem ()
	{
		setFlag(QGraphicsItem::ItemIsMovable, false);
		setFlag(QGraphicsItem::ItemIsSelectable, false);
		setVisible(copy.isVisible());

		className = copy.className;
		centerRegionItem = 0;

		boundaryPathItem = new QGraphicsPathItem(this);
		boundaryPathItem->setVisible(false);
		boundaryPathItem->setPen(QPen(QColor(255,150,150,150),4.0,Qt::DotLine));
		boundaryPathItem->setBrush(Qt::NoBrush);

		if (copy.centerRegionItem)
		{
			centerRegionItem = new ArrowHeadItem(*copy.centerRegionItem);
			centerRegionItem->connectionItem = this;
		}

		centerRegion = copy.centerRegion;
		
		itemHandle = copy.itemHandle;

		if (itemHandle)
			setHandle(itemHandle);

		curveSegments = copy.curveSegments;
		lineType = copy.lineType;
		arrowHeadDistance = copy.arrowHeadDistance;
		setPen(defaultPen = copy.defaultPen);
		setBrush(defaultBrush = copy.defaultBrush);
		setPos(copy.scenePos());
		setTransform(copy.sceneTransform());

		for (int i=0; i < copy.curveSegments.size(); ++i)
		{
			if (copy.curveSegments[i].arrowStart)
			{
				curveSegments[i].arrowStart = new ArrowHeadItem(*copy.curveSegments[i].arrowStart);
				curveSegments[i].arrowStart->connectionItem = this;
			}
			if (copy.curveSegments[i].arrowEnd)
			{
				curveSegments[i].arrowEnd = new ArrowHeadItem(*copy.curveSegments[i].arrowEnd);
				curveSegments[i].arrowEnd->connectionItem = this;
			}
		}

		QList<ConnectionGraphicsItem::ControlPoint*> uniquePoints;
		QList<QPoint> locations, positions;

		for (int i=0; i < copy.curveSegments.size(); ++i)
			for (int j=0; j < copy.curveSegments[i].size(); ++j)
			{
				positions += QPoint(i,j);

				int k = uniquePoints.indexOf(copy.curveSegments[i][j]);
				if (k > -1)
				{
					locations += locations[k];
					uniquePoints += 0;
				}
				else
				{
					locations += QPoint(i,j);
					uniquePoints += (copy.curveSegments[i][j]);
				}
			}

			NodeGraphicsItem * parentNode = 0;
			for (int k=0; k < uniquePoints.size() && k < locations.size() && k < positions.size(); ++k)
			{
				int i = positions[k].x(),
					j = positions[k].y();
				if (copy.curveSegments[i][j])
				{
					if (uniquePoints[k])
					{
						curveSegments[i][j] = new ConnectionGraphicsItem::ControlPoint(*copy.curveSegments[i][j]);
						curveSegments[i][j]->connectionItem = this;
						curveSegments[i][j]->setVisible(false);

						if (copy.curveSegments[i][j]->parentItem())
						{
							parentNode = qgraphicsitem_cast<NodeGraphicsItem*>(copy.curveSegments[i][j]->parentItem());
							if (parentNode == copy.curveSegments[i].arrowEnd)
								curveSegments[i][j]->setParentItem(curveSegments[i].arrowEnd);
							else
								if (parentNode == copy.curveSegments[i].arrowStart)
									curveSegments[i][j]->setParentItem(curveSegments[i].arrowStart);
								else
									curveSegments[i][j]->setParentItem(copy.curveSegments[i][j]->parentItem());

						}
					}
					else
					{
						curveSegments[i][j] = curveSegments[locations[k].x()][locations[k].y()];
					}
				}
			}
	}

	/*! \brief make a copy of this item*/
	ConnectionGraphicsItem* ConnectionGraphicsItem::clone() const
	{
		ConnectionGraphicsItem * c = new ConnectionGraphicsItem(*this);
		//c->className = ConnectionGraphicsItem::CLASSNAME;
		return c;
	}

	/*! operator =: copy just the control point positions and pen */
	ConnectionGraphicsItem& ConnectionGraphicsItem::operator = (const ConnectionGraphicsItem& copy)
	{
		clear(true);
		//className = copy.className;

		//boundaryPathItem = new QGraphicsPathItem(this);
		//boundaryPathItem->setVisible(false);
		//boundaryPathItem->setPen(QPen(QColor(255,150,150,150),2.5,Qt::DotLine));

		if (copy.centerRegionItem)
		{
			QList<QGraphicsItem*> children;
			if (centerRegionItem)
				children = centerRegionItem->childItems();
			centerRegionItem = new ArrowHeadItem(*copy.centerRegionItem);
			centerRegionItem->connectionItem = this;
			for (int i=0; i < children.size(); ++i)
				if (ControlPoint::cast(children[i]))
				{
					children[i]->setParentItem(centerRegionItem);
				}
		}
		else
		{
			/*ArrowHeadItem * node = new ArrowHeadItem;
			node->connectionItem = this;
			NodeGraphicsReader imageReader;
			imageReader.readXml(node,DefaultMiddleItemFile);
			if (node->isValid())
			{
				node->normalize();
				node->scale(5.0/node->sceneBoundingRect().height(),5.0/node->sceneBoundingRect().height());
			}*/
			centerRegionItem = 0;
		}
		centerRegion = copy.centerRegion;

		itemHandle = copy.itemHandle;

		if (itemHandle)
			setHandle(itemHandle);

		curveSegments = copy.curveSegments;
		lineType = copy.lineType;
		arrowHeadDistance = copy.arrowHeadDistance;
		setPen(defaultPen = copy.defaultPen);
		setPos(copy.scenePos());
		setTransform(copy.sceneTransform());

		for (int i=0; i < copy.curveSegments.size(); ++i)
		{
			if (copy.curveSegments[i].arrowStart)
			{
				curveSegments[i].arrowStart = new ArrowHeadItem(*copy.curveSegments[i].arrowStart);
				curveSegments[i].arrowStart->connectionItem = this;
			}
			if (copy.curveSegments[i].arrowEnd)
			{
				curveSegments[i].arrowEnd = new ArrowHeadItem(*copy.curveSegments[i].arrowEnd);
				curveSegments[i].arrowEnd->connectionItem = this;
			}
		}

		QList<ConnectionGraphicsItem::ControlPoint*> uniquePoints;
		QList<QPoint> locations, positions;

		for (int i=0; i < copy.curveSegments.size(); ++i)
			for (int j=0; j < copy.curveSegments[i].size(); ++j)
			{
				positions += QPoint(i,j);

				int k = uniquePoints.indexOf(copy.curveSegments[i][j]);
				if (k > -1)
				{
					locations += locations[k];
					uniquePoints += 0;
				}
				else
				{
					locations += QPoint(i,j);
					uniquePoints += (copy.curveSegments[i][j]);
				}
			}

			NodeGraphicsItem * parentNode = 0;
			for (int k=0; k < uniquePoints.size() && k < locations.size() && k < positions.size(); ++k)
			{
				int i = positions[k].x(),
					j = positions[k].y();
				if (copy.curveSegments[i][j])
				{
					if (uniquePoints[k])
					{
						curveSegments[i][j] = new ConnectionGraphicsItem::ControlPoint(*copy.curveSegments[i][j]);
						curveSegments[i][j]->connectionItem = this;
						curveSegments[i][j]->setVisible(false);

						if (copy.curveSegments[i][j]->parentItem())
						{
							parentNode = qgraphicsitem_cast<NodeGraphicsItem*>(copy.curveSegments[i][j]->parentItem());
							if (parentNode == copy.curveSegments[i].arrowEnd)
								curveSegments[i][j]->setParentItem(curveSegments[i].arrowEnd);
							else
								if (parentNode == copy.curveSegments[i].arrowStart)
									curveSegments[i][j]->setParentItem(curveSegments[i].arrowStart);
								else
									curveSegments[i][j]->setParentItem(copy.curveSegments[i][j]->parentItem());

						}
					}
					else
					{
						curveSegments[i][j] = curveSegments[locations[k].x()][locations[k].y()];
					}
				}
			}
			return *this;
	}
	/*! operator =: copy just the control point positions and pen */
	ConnectionGraphicsItem& ConnectionGraphicsItem::copyPoints (const ConnectionGraphicsItem& copy)
	{
		clear();

		lineType = copy.lineType;
		arrowHeadDistance = copy.arrowHeadDistance;

		centerRegion = copy.centerRegion;

		//itemHandle = copy.itemHandle;
		setPen(defaultPen = copy.defaultPen);

		for (int i=0; i < copy.curveSegments.size() && i < curveSegments.size(); ++i)
			for (int j=0; j < copy.curveSegments[i].size() && j < curveSegments[i].size(); ++j)
				if (copy.curveSegments[i][j] && curveSegments[i][j] && curveSegments[i][j]->parentItem() == 0)
					curveSegments[i][j]->setPos(copy.curveSegments[i][j]->scenePos());

		return *this;
	}

	/*! Destructor: deletes all shapes and control points */
	ConnectionGraphicsItem::~ConnectionGraphicsItem()
	{
		clear(true);
		if (!itemHandle) return;
		setHandle(0);
	}

	/*! \brief checks that this is a valid drawable*/
	bool ConnectionGraphicsItem::isValid()
	{
		for (int i=0; i < curveSegments.size(); ++i)
			if (curveSegments[i].size() < 4)
				return false;
		return (curveSegments.size() > 0);
	}

	/*! \brief paint method. Call's parent's paint after setting antialiasing to true*/
	void ConnectionGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
	{
		//painter->setClipRect( option->exposedRect );

		//refresh();

		painter->setBrush(Qt::NoBrush);
		painter->setPen(QPen(QColor(255,255,255,255),pen().width()+4));
		painter->drawPath(path());//this->pathShape);

		painter->setBrush(brush());
		painter->setPen(pen());
		painter->drawPath(path());//this->pathShape);
	}

	/*! \brief Constructor: Setup colors and z value */
	ConnectionGraphicsItem::ControlPoint::ControlPoint(ConnectionGraphicsItem * reaction_ptr, QGraphicsItem * parent) :
	Tinkercell::ControlPoint(parent)
	{
		connectionItem = reaction_ptr;
		setPen( defaultPen = QPen( QColor(100,100,255) ) );
		setBrush( defaultBrush = QBrush( QColor(0,0,255,10)) );
		setRect(QRectF(-10,-10,20,20));
		setZValue(1);
	}

	/*! \brief Constructor: constructor with position */
	ConnectionGraphicsItem::ControlPoint::ControlPoint(const QPointF& pos, ConnectionGraphicsItem * reaction_ptr, QGraphicsItem * parent) :
	Tinkercell::ControlPoint(parent)
	{
		connectionItem = reaction_ptr;
		setPen( defaultPen = QPen( QColor(100,100,255) ) );
		setBrush( defaultBrush = QBrush( QColor(0,0,255,10)) );
		setRect(QRectF(-10,-10,20,20));
		setZValue(1);
		setPos(pos);
	}
	/*! \brief Copy Constructor */
	ConnectionGraphicsItem::ControlPoint::ControlPoint(const ControlPoint& copy) : Tinkercell::ControlPoint(copy.parentItem())
	{
		connectionItem = copy.connectionItem;
		setPos(copy.scenePos());
		setRect(copy.rect());
		setBrush(defaultBrush = copy.defaultBrush);
		setPen(defaultPen = copy.defaultPen);
		//setTransform(copy.transform());
	}

	/*! \brief make a copy of this item*/
	ControlPoint* ConnectionGraphicsItem::ControlPoint::clone() const
	{
		return new ConnectionGraphicsItem::ControlPoint(*this);
	}

	/*! \brief destructor */
	ConnectionGraphicsItem::ControlPoint::~ControlPoint()
	{
		if (connectionItem)
		{
			for (int i=0; i < connectionItem->curveSegments.size(); ++i)
				for (int j=0; j < connectionItem->curveSegments[i].size(); ++j)
					if (connectionItem->curveSegments[i][j] == this)
						connectionItem->curveSegments[i][j] = 0;
		}
	}

	/*! \brief operator = */
	ConnectionGraphicsItem::ControlPoint& ConnectionGraphicsItem::ControlPoint::operator =(const ControlPoint& copy)
	{
		connectionItem = copy.connectionItem;
		setPos(copy.scenePos());
		setRect(copy.rect());
		setBrush(defaultBrush = copy.defaultBrush);
		setPen(defaultPen = copy.defaultPen);
		return *this;
	}

	/*! \brief gets a path that is constructed by uniting all the shape paths*/
	QPainterPath ConnectionGraphicsItem::shape() const
	{
		return pathShape;
	}

	/*! \brief the center point (if one exists)*/
	ConnectionGraphicsItem::ControlPoint * ConnectionGraphicsItem::centerPoint() const
	{
		for (int i=0; i < curveSegments.size(); ++i)
		{
			if (curveSegments[i].last() && (qgraphicsitem_cast<NodeGraphicsItem*>(curveSegments[i].last()->parentItem()) == 0)
				&& curveSegments.size() > 1 && curveSegments[i].arrowEnd == 0)
				return curveSegments[i].last();
		}
		if (curveSegments.size() > 0 && curveSegments[0].size() >= 4)
		{
			int i = curveSegments[0].size()/2;
			if (lineType == line || curveSegments[0].size() > 4)
				i = 3 * (int)(i/3);
			return curveSegments[0][i];
		}
		return 0;
	}

	/*! \brief the center location*/
	QPointF ConnectionGraphicsItem::centerLocation() const
	{
		if (pathShape.elementCount() < 1)
		{
			QPointF p(0,0);
			NodeGraphicsItem * node;
			int l = 0;
			for (int i=0; i < curveSegments.size(); ++i)
				if (curveSegments[i].size() > 0 && curveSegments[i][0])
				{
					node = NodeGraphicsItem::cast(curveSegments[i][0]->parentItem());
					if (node)
					{
						++l;
						p += node->scenePos();
					}
				}
			return p/l;
		}
		else
		if (curveSegments.size() == 1 && curveSegments[0].size() < 5)
		{
			//if (pen().style() == Qt::SolidLine)
			return ( pathShape.pointAtPercent(0.25) );
			//else
			//return ( pathShape.pointAtPercent(0.5) );
		}
		else
		{
			ConnectionGraphicsItem::ControlPoint * cp  = centerPoint();
			if (cp)
				return ( centerPoint()->scenePos() );
		}
		return QPointF(0,0);
	}

	/*! \brief adjust the end control points so that they point straight
	* \param void
	* \return void*/
	void ConnectionGraphicsItem::adjustEndPoints()
	{
		if (!scene()) return;

		//firstPoint = point on the node, lastPoint = point at the center (might be modifier)
		//cp0 = point just before firstPoint, cp1 = point just before lastPoint
		ControlPoint * firstPoint, *lastPoint, * cp0, * cp1;
		int truePaths = 0; //number of non-modifier paths

		//when there is only one path (1-to-1), the situation is totally different
		for (int i=0; i < curveSegments.size(); ++i)
		{
			firstPoint = curveSegments[i].first();
			lastPoint = curveSegments[i].last();

			if (curveSegments[i].size() < 4 || !lastPoint || !firstPoint) continue;

			if (firstPoint->parentItem() != 0 &&
				((lastPoint->parentItem() != 0 &&
				lastPoint->parentItem() != curveSegments[i].arrowEnd)
				|| (lastPoint->parentItem() == 0)))
				++truePaths;

			if (lineType == bezier)
				cp0 = curveSegments[i][ 1 ];
			else
				cp0 = curveSegments[i][ 3 ];

			if (lineType == bezier)
				cp1 = curveSegments[i][ curveSegments[i].size()-2 ];
			else
				cp1 = curveSegments[i][ curveSegments[i].size()-4 ];

			//adjust firstPoint so that it lies on the boundary rect of the item it points to
			NodeGraphicsItem * node = 0;
			if (firstPoint != 0 && cp0 != 0 && (node = NodeGraphicsItem::cast(firstPoint->parentItem())) != 0)
			{
				QRectF parentRect = node->sceneBoundingRect();
				//QPainterPath parentShape = node->mapToScene(node->shape());

				if (firstPoint->isVisible())
					firstPoint->setVisible(false);
				if (curveSegments[i].arrowStart != 0)
				{
					QPointF p = pointOnEdge(*node, cp0->scenePos(), arrowHeadDistance , lineType == line);

					firstPoint->setPos( node->mapFromScene(p) );

					if (curveSegments[i].arrowStart->scene() != scene() && scene())
					{
						(static_cast<GraphicsScene*>(scene()))->addItem(curveSegments[i].arrowStart);
					}

					curveSegments[i].arrowStart->setZValue(zValue() + 0.1);

					if (curveSegments[i].arrowStart->handle() != 0)  //arrow should not have handles
						Tinkercell::setHandle(curveSegments[i].arrowStart,0);

					if (curveSegments[i].arrowStart->parentItem() == 0)
					{
						QPointF p2 = p;//pointOnEdge( *node, cp0->scenePos(), arrowHeadDistance, lineType == line);
						curveSegments[i].arrowStart->setPos(p2);
					}

					qreal angle;
					if (cp0->x() == p.x())
						if (cp0->y() < p.y())
							angle = 90.0;
						else
							angle = -90.0;

					else
						angle = atan((cp0->y()-p.y())/(cp0->x()-p.x())) * 180.0/3.14159;

					if (cp0->x() > p.x())
						if (cp0->y() < p.y())
							angle += 180.0;
						else
							angle -= 180.0;


					if (curveSegments[i].arrowStart->angle != angle)
					{
						double dx = angle - curveSegments[i].arrowStart->angle;
						double sinx = sin(dx * 3.14/180.0),
							  cosx = cos(dx * 3.14/180.0);
						QTransform rotate(cosx, sinx, -sinx, cosx, 0, 0);
						QTransform t = curveSegments[i].arrowStart->transform();
						curveSegments[i].arrowStart->setTransform(t * rotate);
						curveSegments[i].arrowStart->angle = angle;
					}
				}
				else
				{
					firstPoint->setPos(
						node->mapFromScene(
						pointOnEdge(*node, cp0->scenePos(), arrowHeadDistance/2.0, lineType == line)));
				}
			}

			//adjust modifier arrows (if exists) by using center region
			ControlPoint * centerPoint = this->centerPoint();
			if (cp1 != 0 && centerPoint && lastPoint != centerPoint && curveSegments[i].arrowEnd != 0
				&& (lastPoint->parentItem() == 0 || lastPoint->parentItem() == curveSegments[i].arrowEnd)
				&& curveSegments.size() > 1)
			{
				QPointF centerPoint = this->centerPoint()->scenePos();
				QRectF centerRect(centerPoint - QPointF(centerRegion.width(),centerRegion.height())/2.0, centerRegion);
				QPointF p = pointOnEdge(centerRect, cp1->scenePos(), arrowHeadDistance + 2.0,lineType == line);
				QPointF p2 = cp1->scenePos();
				if (lastPoint->parentItem() != curveSegments[i].arrowEnd)
					lastPoint->setParentItem(curveSegments[i].arrowEnd);

				curveSegments[i].arrowEnd->setPos(p);
				lastPoint->setPos(curveSegments[i].arrowEnd->mapFromScene(p) );

				if (curveSegments[i].arrowEnd->scene() != scene() && scene())
				{
					(static_cast<GraphicsScene*>(scene()))->addItem(curveSegments[i].arrowEnd);
				}

				curveSegments[i].arrowEnd->setZValue(zValue() + 0.1);

				qreal angle;
				if (cp1->x() == p.x())
					if (cp1->y() < p.y())
						angle = 90.0;
					else
						angle = -90.0;
				else
					angle = atan((cp1->y() - p.y())/(cp1->x() - p.x())) * 180.0/3.14159;
				if (cp1->x() > p.x())
					if (cp1->y() < p.y())
						angle += 180.0;
					else
						angle -= 180.0;

				if (curveSegments[i].arrowEnd->angle != angle)
				{
					double dx = angle - curveSegments[i].arrowEnd->angle;
					double sinx = sin(dx * 3.14/180.0),
					  	   cosx = cos(dx * 3.14/180.0);
					QTransform rotate(cosx, sinx, -sinx, cosx, 0, 0);
					QTransform t = curveSegments[i].arrowEnd->transform();
					curveSegments[i].arrowEnd->setTransform(t * rotate);				
					curveSegments[i].arrowEnd->angle = angle;
				}
			}
		}

		if (truePaths == 1 && curveSegments[0].size() >= 4)
		{
			firstPoint = curveSegments[0].last();
			lastPoint = curveSegments[0].first();

			if (lineType == bezier)
				cp0 = curveSegments[0][ curveSegments[0].size()-2 ];
			else
				cp0 = curveSegments[0][ curveSegments[0].size()-4 ];

			NodeGraphicsItem * node = NodeGraphicsItem::cast(firstPoint->parentItem());
			NodeGraphicsItem * node2 = NodeGraphicsItem::cast(lastPoint->parentItem());
			if (firstPoint != 0 && cp0 != 0 && node && node2)
			{
				QRectF parentRect1 = node->sceneBoundingRect(),
					parentRect2 = node2->sceneBoundingRect();
				//QPainterPath parentShape = node->mapToScene(node->shape());

				if (firstPoint->isVisible())
					firstPoint->setVisible(false);

				QPointF p0 = cp0->scenePos();

				if (curveSegments[0].arrowEnd != 0)
				{
					QPointF p = pointOnEdge(*node, cp0->scenePos(), arrowHeadDistance,lineType == line);

					if (lineType == line && curveSegments[0].size() == 4)
					{
						if (p.rx() == p0.rx())
						{
							if (parentRect1.center().rx() > parentRect2.left() && parentRect1.center().rx() < parentRect2.right())
								p.rx() = parentRect1.center().rx();
							else
								if (parentRect2.center().rx() > parentRect1.left() && parentRect2.center().rx() < parentRect1.right())
									p.rx() = parentRect2.center().rx();
						}
						else
							if (p.ry() == p0.ry())
							{
								if (parentRect1.center().ry() > parentRect2.top() && parentRect1.center().ry() < parentRect2.bottom())
									p.ry() = parentRect1.center().ry();
								else
									if (parentRect2.center().ry() > parentRect1.top() && parentRect2.center().ry() < parentRect1.bottom())
										p.ry() = parentRect2.center().ry();
							}
					}

					firstPoint->setPos( node->mapFromScene(p) );

					if (curveSegments[0].arrowEnd->scene() != scene() && scene())
					{
						(static_cast<GraphicsScene*>(scene()))->addItem(curveSegments[0].arrowEnd);
					}

					if (curveSegments[0].arrowEnd->parentItem() == 0)
					{
						QPointF p2 = p;//pointOnEdge(*node, cp0->scenePos(), arrowHeadDistance,lineType == line);
						curveSegments[0].arrowEnd->setPos(p2);
					}

					curveSegments[0].arrowEnd->setZValue(zValue() + 0.1);

					qreal angle;
					if (cp0->x() == p.x())
						if (cp0->y() < p.y())
							angle = 90.0;
						else
							angle = -90.0;
					else
						angle = atan((cp0->y()-p.y())/(cp0->x()-p.x())) * 180.0/3.14159;
					if (cp0->x() > p.x())
						if (cp0->y() < p.y())
							angle += 180.0;
						else
							angle -= 180.0;

					if (curveSegments[0].arrowEnd->angle != angle)
					{
						double dx = angle - curveSegments[0].arrowEnd->angle;
						double sinx = sin(dx * 3.14/180.0),
							   cosx = cos(dx * 3.14/180.0);
						QTransform rotate(cosx, sinx, -sinx, cosx, 0, 0);
						QTransform t = curveSegments[0].arrowEnd->transform();
						curveSegments[0].arrowEnd->setTransform(t * rotate);
						curveSegments[0].arrowEnd->angle = angle;
					}
				}
				else
				{
					QRectF parentRect1 = node->sceneBoundingRect(),
						parentRect2 = node2->sceneBoundingRect();

					QPointF p = pointOnEdge(*node,cp0->scenePos(),arrowHeadDistance/2.0,lineType == line);

					if (lineType == line && curveSegments[0].size() == 4)
					{
						if (p.rx() == p0.rx())
						{
							if (parentRect1.center().rx() > parentRect2.left() && parentRect1.center().rx() < parentRect2.right())
								p.rx() = parentRect1.center().rx();
							else
								if (parentRect2.center().rx() > parentRect1.left() && parentRect2.center().rx() < parentRect1.right())
									p.rx() = parentRect2.center().rx();
						}
						else
							if (p.ry() == p0.ry())
							{
								if (parentRect1.center().ry() > parentRect2.top() && parentRect1.center().ry() < parentRect2.bottom())
									p.ry() = parentRect1.center().ry();
								else
									if (parentRect2.center().ry() > parentRect1.top() && parentRect2.center().ry() < parentRect1.bottom())
										p.ry() = parentRect2.center().ry();
							}
					}

					firstPoint->setPos( node->mapFromScene(p) );
				}
			}
		}

	}

	/*! \brief refresh the path if any controlpoints have moved
	* \param void
	* \return void*/
	void ConnectionGraphicsItem::refresh()
	{
		setPos(0,0);
		adjustEndPoints();
		qreal z = zValue();
		/*if (!centerRegionItem)
		{
			ArrowHeadItem * node = new ArrowHeadItem;
			node->connectionItem = this;
			NodeGraphicsReader imageReader;
			imageReader.readXml(node,DefaultMiddleItemFile);
			if (node->isValid())
			{
				node->normalize();
				node->scale(25.0/node->sceneBoundingRect().height(),25.0/node->sceneBoundingRect().height());
			}
			centerRegionItem = 0;
		}*/
		if (centerRegionItem && centerRegionItem->parentItem() == 0)
		{
			centerRegionItem->connectionItem = this;
			/*if (centerRegionItem->scene() != scene() && scene())
			{
			if (centerRegionItem->scene())
			centerRegionItem->scene()->removeItem(centerRegionItem);
			if (scene())
			{
			(static_cast<GraphicsScene*>(scene()))->addItem(centerRegionItem);
			}
			}*/

			//if (centerRegionItem->scene() == scene() && centerRegionItem->isVisible())
			//{
			if (centerRegionItem->scene() != this->scene() && this->scene())
			{
				QList<QGraphicsItem*> children = centerRegionItem->childItems();
				for (int i=0; i < children.size(); ++i)
					if (qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(children[i]))
					{
						(static_cast<GraphicsScene*>(scene()))->addItem(centerRegionItem);
						break;
					}
			}
			centerRegionItem->setPos( centerLocation() );
			centerRegionItem->setZValue(z + 0.01);

			QList<ConnectionGraphicsItem*> otherConnections = centerRegionItem->connections();
			for (int i=0; i < otherConnections.size(); ++i)
				if (otherConnections[i] && otherConnections[i] != this)
					otherConnections[i]->refresh();
		}

		QPainterPath path;

		if (lineType == line)
		{
			QPointF pos;
			for (int i=0; i < curveSegments.size(); ++i)
			{
				
				if (curveSegments[i].arrowStart)
				{
					if (curveSegments[i].arrowStart->scene() != this->scene() && this->scene())
						(static_cast<GraphicsScene*>(scene()))->addItem(curveSegments[i].arrowStart);
					curveSegments[i].arrowStart->setZValue(z + 0.1);
				}
				if (curveSegments[i].arrowEnd)
				{
					if (curveSegments[i].arrowEnd->scene() != this->scene() && this->scene())
						(static_cast<GraphicsScene*>(scene()))->addItem(curveSegments[i].arrowEnd);
						curveSegments[i].arrowEnd->setZValue(z + 0.1);
				}
				

				NodeGraphicsItem * node = nodeAt(i);
				if (curveSegments[i].size() > 0 && curveSegments[i][0] &&
					node && node->scene() == scene())
				{
					curveSegments[i][0]->setZValue(z + 0.02);
					pos = curveSegments[i][0]->scenePos();
					path.moveTo(pos);
					for (int j=0; j+3 < curveSegments[i].size(); j+=3)
						if (curveSegments[i][j])
						{
							curveSegments[i][j+1]->setZValue(z + 0.02);
							curveSegments[i][j+2]->setZValue(z + 0.02);
							curveSegments[i][j+3]->setZValue(z + 0.02);
							pos = curveSegments[i][j+3]->scenePos();
							path.lineTo(pos);
						}
				}
			}
		}
		else
		{
			QPointF pos1,pos2,pos3;
			for (int i=0; i < curveSegments.size(); ++i)
			{
				
				if (curveSegments[i].arrowStart)
				{
					if (curveSegments[i].arrowStart->scene() != this->scene() && this->scene())
						(static_cast<GraphicsScene*>(scene()))->addItem(curveSegments[i].arrowStart);
					curveSegments[i].arrowStart->setZValue(z + 0.1);
				}
				if (curveSegments[i].arrowEnd)
				{
					if (curveSegments[i].arrowEnd->scene() != this->scene() && this->scene())
						(static_cast<GraphicsScene*>(scene()))->addItem(curveSegments[i].arrowEnd);
					curveSegments[i].arrowEnd->setZValue(z + 0.1);
				}
				
				NodeGraphicsItem * node = nodeAt(i);
				if (curveSegments[i].size() > 0 && curveSegments[i][0] &&
					node && node->scene() == scene())
				{
					curveSegments[i][0]->setZValue(z + 0.02);

					pos1 =  curveSegments[i][0]->scenePos();
					curveSegments[i][0]->setZValue(z + 0.02);
					path.moveTo(pos1);
					for (int j=0; j+3 < curveSegments[i].size(); j+=3)
						if (curveSegments[i][j+1] && curveSegments[i][j+2] && curveSegments[i][j+3])
						{
							curveSegments[i][j+1]->setZValue(z + 0.02);
							curveSegments[i][j+2]->setZValue(z + 0.02);
							curveSegments[i][j+3]->setZValue(z + 0.02);

							pos1 =  curveSegments[i][j+1]->scenePos();
							pos2 =  curveSegments[i][j+2]->scenePos();
							pos3 =  curveSegments[i][j+3]->scenePos();

							path.cubicTo(pos1,pos2,pos3);
						}
				}
			}
		}

		QPainterPathStroker stroker;
		stroker.setJoinStyle(Qt::RoundJoin);
		stroker.setCapStyle(Qt::RoundCap);
		//stroker.setDashPattern(pen().style());
		stroker.setWidth(0.0);
		this->pathShape = stroker.createStroke(path);

		if (pen().style() == Qt::SolidLine)
			setPath(this->pathShape);
		else
			setPath(path);

		pathBoundingRect = this->pathShape.controlPointRect().adjusted(-10,-10,10,10);
		refreshBoundaryPath();
		update();
	}

	/*! \brief update the boundary path*/
	void ConnectionGraphicsItem::refreshBoundaryPath()
	{
		//boundaryPathItem->setVisible(true);
		QPainterPath boundary;
		for (int i=0; i < curveSegments.size(); ++i)
		{
			for (int j=0; j < curveSegments[i].size(); ++j)
			{
				if (curveSegments[i][j])
					if (j == 0)
						boundary.moveTo(curveSegments[i][j]->scenePos());
					else
						boundary.lineTo(curveSegments[i][j]->scenePos());
			}
		}
		boundaryPathItem->setPath(boundary);
		boundaryPathItem->setZValue(zValue() - 0.02);
		boundaryPathItem->update();
	}

	/*! \brief Clear all shapes and control points
	* \param void
	* \return void*/
	void ConnectionGraphicsItem::clear(bool all)
	{
		QList<ConnectionGraphicsItem::ControlPoint*> visited;
		for (int i=0; i < curveSegments.size(); ++i)
			for (int j=0; j < curveSegments[i].size(); ++j)
				if (curveSegments[i][j] && !visited.contains(curveSegments[i][j]))
				{
					visited += curveSegments[i][j];

					curveSegments[i][j]->setParentItem(0);

					if (curveSegments[i][j]->scene())
					{
						curveSegments[i][j]->scene()->removeItem(curveSegments[i][j]);
					}

					curveSegments[i][j]->connectionItem = 0;
					
					if (!MainWindow::invalidPointers.contains( (void*)curveSegments[i][j]) )
					{
						delete curveSegments[i][j];
						MainWindow::invalidPointers[ (void*)curveSegments[i][j] ] = true;
					}

					curveSegments[i][j] = 0;

					if (all)
					{
						if (curveSegments[i].arrowStart && 
							!MainWindow::invalidPointers.contains( (void*)curveSegments[i].arrowStart) )
						{
							if (curveSegments[i].arrowStart->scene())
								curveSegments[i].arrowStart->scene()->removeItem(curveSegments[i].arrowStart);
							delete curveSegments[i].arrowStart;
							MainWindow::invalidPointers[ curveSegments[i].arrowStart ] = true;
						}
							
						if (curveSegments[i].arrowEnd &&
							!MainWindow::invalidPointers.contains( (void*)curveSegments[i].arrowEnd) )
						{
							if (curveSegments[i].arrowEnd->scene())
								curveSegments[i].arrowEnd->scene()->removeItem(curveSegments[i].arrowEnd);
							delete curveSegments[i].arrowEnd;
							MainWindow::invalidPointers[ (void*)curveSegments[i].arrowEnd ] = true;
						}

						curveSegments[i].arrowStart = 0;
						curveSegments[i].arrowEnd = 0;
					}
				}
		curveSegments.clear();

		if (centerRegionItem && all && !MainWindow::invalidPointers.contains(centerRegionItem))
		{
			centerRegionItem->setParentItem(0);
			centerRegionItem->connectionItem = 0;

			if (centerRegionItem->scene())
				centerRegionItem->scene()->removeItem(centerRegionItem);
			
			delete centerRegionItem;
			MainWindow::invalidPointers[ (void*)centerRegionItem ] = true;
			
			centerRegionItem = 0;
		}
	}

	/*! \brief find slope at the given point (or closest point)*/
	qreal ConnectionGraphicsItem::slopeAtPoint(const QPointF& point)
	{
		qreal percent = 1, dpercent = 1;
		QPainterPath path = this->path();
		QPointF p1 = path.pointAtPercent(0), p2;

		while (percent < 100)
		{
			p2 = path.pointAtPercent(percent);
			if ( ((point.x() - p1.x())*(point.x() - p1.x()) + (point.y() - p1.y())*(point.y() - p1.y())) <
				((point.x() - p2.x())*(point.x() - p2.x()) + (point.y() - p2.y())*(point.y() - p2.y())) )

				break;

			percent += dpercent;
		}
		return path.slopeAtPercent(percent);
	}

	/*! \brief set visibility of control points
	* \param visible = true, invisible = false
	* \return void*/
	void ConnectionGraphicsItem::setControlPointsVisible(bool visible)
	{
		controlPointsVisible = visible;
		if (boundaryPathItem && lineType == bezier)
			boundaryPathItem->setVisible(visible);
		else
			boundaryPathItem->setVisible(false);

		for (int i=0; i < curveSegments.size(); ++i)
			for (int j=0; j < curveSegments[i].size(); ++j)
			{
				if (curveSegments[i][j])
				{
					if (lineType == bezier || (j%3)==0)
					{
						curveSegments[i][j]->setVisible(visible);
						if (visible && scene() != curveSegments[i][j]->scene())
						{
							if (curveSegments[i][j]->scene())
							{
								curveSegments[i][j]->setParentItem(0);
								curveSegments[i][j]->scene()->removeItem(curveSegments[i][j]);
							}
							if (scene())
								(static_cast<GraphicsScene*>(scene()))->addItem(curveSegments[i][j]);
						}
					}
					else
					{
						curveSegments[i][j]->setVisible(false);
					}
				}
			}
		refresh();
	}

	void ConnectionGraphicsItem::showControlPoints()
	{
		setControlPointsVisible(true);
	}

	void ConnectionGraphicsItem::hideControlPoints()
	{
		setControlPointsVisible(false);
	}

	/*! \brief bounding rect*/
	QRectF ConnectionGraphicsItem::boundingRect() const
	{
		//return pathShape.controlPointRect();
		return pathBoundingRect;
	}
	/*! \brief scene bounding rect*/
	QRectF ConnectionGraphicsItem::sceneBoundingRect() const
	{
		//return pathShape.controlPointRect();
		return pathBoundingRect;
	}
	/*! \brief list of pointers to all the control points*/
	QList<ConnectionGraphicsItem::ControlPoint*> ConnectionGraphicsItem::controlPoints(bool includeEnds) const
	{
		QList<ControlPoint*> list;
		for (int i=0; i < curveSegments.size(); ++i)
			for (int j=0; j < curveSegments[i].size(); ++j)
			{
				if (curveSegments[i][j] && !list.contains(curveSegments[i][j]) &&
					(curveSegments[i][j]->parentItem() == 0 || includeEnds)
					)
					list.append(curveSegments[i][j]);
			}
			return list;
	}
	/*! \brief list of pointers to all the control points*/
	QList<QGraphicsItem*> ConnectionGraphicsItem::controlPointsAsGraphicsItems(bool includeEnds) const
	{
		QList<QGraphicsItem*> list;
		for (int i=0; i < curveSegments.size(); ++i)
			for (int j=0; j < curveSegments[i].size(); ++j)
			{
				if (curveSegments[i][j] && !list.contains(curveSegments[i][j]) &&
					(curveSegments[i][j]->parentItem() == 0 || includeEnds)
					)
					list.append(curveSegments[i][j]);
			}
			return list;
	}

	AddControlPointCommand::AddControlPointCommand(
		const QString& name, GraphicsScene * scene,
		ConnectionGraphicsItem::ControlPoint* item)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems.clear();
		graphicsItems += item;
	}

	AddControlPointCommand::AddControlPointCommand(
		const QString& name, GraphicsScene * scene,
		QList<ConnectionGraphicsItem::ControlPoint*> items)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems = items;
	}

	void AddControlPointCommand::redo()
	{
		QList<ConnectionGraphicsItem::ControlPoint*> controlPointItems = graphicsItems;
		graphicsItems.clear();

		for (int k=0; k < controlPointItems.size(); k+=3)

			if (controlPointItems[k] && graphicsScene && controlPointItems[k]->connectionItem)
			{
				ConnectionGraphicsItem::ControlPoint* controlPoint = controlPointItems[k];
				ConnectionGraphicsItem * item = controlPoint->connectionItem;

				QPointF loc = controlPoint->scenePos();
				qreal dist = -1;
				int k1 = 0, k2 = 0;
				QPointF p0,p1;

				for (int i=0; i < item->curveSegments.size(); ++i)
					for (int j=0; j < item->curveSegments[i].size(); ++j)
					{
						if (item->curveSegments[i][j])
						{
							p0 = item->curveSegments[i][j]->scenePos();

							if (j > 0 && j < (item->curveSegments[i].size()-1))
								p0 =  (item->curveSegments[i][j+1]->scenePos() + item->curveSegments[i][j-1]->scenePos() + p0)/3.0;
							else
							{
								if (j > 0)
									p0 =  (item->curveSegments[i][j-1]->scenePos() + p0)/2.0;
								else
									if (j < (item->curveSegments[i].size()-1))
										p0 =  (item->curveSegments[i][j+1]->scenePos() + p0)/2.0;
							}

							qreal dist2 = (p0.x() - loc.x())*(p0.x() - loc.x()) +
								(p0.y() - loc.y())*(p0.y() - loc.y());
							if (dist < 0 || (dist > dist2))
							{
								dist = dist2;
								k1 = i;
								if ((j+1) < item->curveSegments[i].size())
									k2 = j;
								else
									k2 = j - 1;
							}
						}

						if (item->lineType == ConnectionGraphicsItem::line)
							j += 2;
					}

					//controlPoint->graphicsItem = item;
					while (item->curveSegments.size() <= k1) item->curveSegments.append(ConnectionGraphicsItem::CurveSegment());

					if (item->curveSegments[k1].size() < k2) k2 = item->curveSegments[k1].size();

					if (graphicsScene)
					{
						ConnectionGraphicsItem::ControlPoint * cp1, * cp2;

						if (controlPointItems.size() > k+2)
						{
							cp1 = controlPointItems[k+1];
							cp2 = controlPointItems[k+2];
						}
						else
						{
							cp1 = new ConnectionGraphicsItem::ControlPoint(item);
							cp2 = new ConnectionGraphicsItem::ControlPoint(item);
						}

						qreal dx1 = 50.0;
						//qreal dx2 = 25.0;
						int d1 = -1, d2 = 1;
						if (k2 % 3 == 0) { d1 = -2; d2 = -1; }
						if (k2 % 3 == 1) { d1 = -1; d2 = 1; }
						if (k2 % 3 == 2) { d1 = 1; d2 = 2; }

						//if (item->curveSegments[k1].size() <= k2)
						//{
						cp1->setPos( QPointF(controlPoint->pos().x()+d1*dx1, controlPoint->pos().y() ) );
						cp2->setPos( QPointF(controlPoint->pos().x()+d2*dx1, controlPoint->pos().y() ) );
						/*}
						else
						{
						qreal slopeAtCp = item->slopeAtPoint(controlPoint->pos());
						if (slopeAtCp == 0) slopeAtCp = 0.001;
						QPointF pos1 = QPointF(controlPoint->pos().x() + d1*dx2, controlPoint->pos().y() + d1*dx2*slopeAtCp),
						pos2 = QPointF(controlPoint->pos().x() + d2*dx2, controlPoint->pos().y() + d2*dx2*slopeAtCp);
						cp1->setPos( QPointF(0.4*pos1.x() + 0.6*controlPoint->pos().x(), 0.4*pos1.y() + 0.6*controlPoint->pos().y()) );
						cp2->setPos( QPointF(0.4*pos2.x() + 0.6*controlPoint->pos().x(), 0.4*pos2.y() + 0.6*controlPoint->pos().y()) );
						}*/

						graphicsScene->addItem(controlPoint);
						graphicsScene->addItem(cp1);
						graphicsScene->addItem(cp2);

						controlPoint->setZValue(item->zValue()+0.02);
						cp1->setZValue(controlPoint->zValue());
						cp2->setZValue(controlPoint->zValue());

						if (item->curveSegments[k1].size() < 4)
						{
							ConnectionGraphicsItem::ControlPoint * cp3 = new ConnectionGraphicsItem::ControlPoint(item);
							cp3->setPos( QPointF(controlPoint->pos().x(), controlPoint->pos().y()) );
							graphicsScene->addItem(cp3);
							cp3->setZValue(controlPoint->zValue());

							item->curveSegments[k1].insert(0,cp1);
							item->curveSegments[k1].insert(0,cp2);
							item->curveSegments[k1].insert(0,cp3);
							item->curveSegments[k1].insert(0,controlPoint);

							listK1 << k1 << k1 << k1;
							listK2 << 0 << 0 << 0 << 0;

							graphicsItems << cp1 << cp2 << cp3 << controlPoint;
						}
						else
						{
							listK1 << k1 << k1 << k1;
							listK2 << (k2+1) << (k2+1)<< (k2+1);

							if (k2 % 3 == 0)
							{
								item->curveSegments[k1].insert(k2+1,controlPoint);
								item->curveSegments[k1].insert(k2+1,cp2);
								item->curveSegments[k1].insert(k2+1,cp1);
								graphicsItems << controlPoint << cp2 << cp1;
							}
							else
								if (k2 % 3 == 1)
								{
									item->curveSegments[k1].insert(k2+1,cp2);
									item->curveSegments[k1].insert(k2+1,controlPoint);
									item->curveSegments[k1].insert(k2+1,cp1);
									graphicsItems << cp2 << controlPoint << cp1;
								}
								else
								{
									item->curveSegments[k1].insert(k2+1,cp2);
									item->curveSegments[k1].insert(k2+1,cp1);
									item->curveSegments[k1].insert(k2+1,controlPoint);
									graphicsItems << cp2 << cp1 << controlPoint;
								}
						}
					}
					item->setControlPointsVisible(true);
					item->refresh();
			}
	}

	void AddControlPointCommand::undo()
	{
		for (int i=0; i < graphicsItems.size(); ++i)
		{
			if (graphicsItems[i] && graphicsItems[i]->connectionItem && graphicsScene)
			{
				ConnectionGraphicsItem * item = graphicsItems[i]->connectionItem;
				if (listK1[i] >= 0 && listK1[i] < item->curveSegments.size())
				{
					int k = item->curveSegments[ listK1[i] ].indexOf(graphicsItems[i]);
					item->curveSegments[ listK1[i] ].remove(k);
				}

				if (graphicsScene)
				{
					graphicsItems[i]->setParentItem(0);
					graphicsScene->removeItem(graphicsItems[i]);
				}

				if (graphicsItems[i]->scene())
				{
					graphicsItems[i]->setParentItem(0);
					graphicsItems[i]->scene()->removeItem(graphicsItems[i]);
				}

				item->refresh();
			}
		}
	}

	AddControlPointCommand::~AddControlPointCommand()
	{
		for (int i=0; i < graphicsItems.size(); ++i)
		{
			graphicsItems[i]->setParentItem(0);
			if (graphicsItems[i] && !graphicsItems[i]->connectionItem && !graphicsItems[i]->scene())
			{
				delete graphicsItems[i];
			}
		}
	}

	RemoveControlPointCommand::RemoveControlPointCommand(
		const QString& name, GraphicsScene * scene,
		ConnectionGraphicsItem::ControlPoint* item)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems.clear();
		graphicsItems += item;
	}

	RemoveControlPointCommand::RemoveControlPointCommand(
		const QString& name, GraphicsScene * scene,
		QList<ConnectionGraphicsItem::ControlPoint*> items)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems = items;
	}

	void RemoveControlPointCommand::undo()
	{
		for (int i=0; i < graphicsItems.size(); ++i)

			if (graphicsItems[i] && graphicsItems[i]->connectionItem && graphicsScene)
			{
				ConnectionGraphicsItem * item = graphicsItems[i]->connectionItem;
				if (listK1[i] >= 0 && listK1[i] < item->curveSegments.size() && listK2[i]  >= 0 && listK2[i] < item->curveSegments[ listK1[i] ].size() )
				{
					item->curveSegments[ listK1[i] ].insert(listK2[i],graphicsItems[i]);
					graphicsScene->addItem(graphicsItems[i]);
				}
				item->refresh();
			}
	}

	void RemoveControlPointCommand::redo()
	{
		QList<ConnectionGraphicsItem::ControlPoint*> controlPointItems = graphicsItems;
		graphicsItems.clear();

		for (int i=0; i < controlPointItems.size(); ++i)

			if (controlPointItems[i] && controlPointItems[i]->connectionItem  && graphicsScene)
			{
				ConnectionGraphicsItem::ControlPoint * controlPoint = controlPointItems[i];
				ConnectionGraphicsItem * item = controlPointItems[i]->connectionItem;

				for (int i=0; i < item->curveSegments.size(); ++i)
				{
					int index = -1;
					for (int j=3; j < item->curveSegments[i].size()-3; j+=3)
					{
						if (item->curveSegments[i][j] == controlPoint ||
							item->curveSegments[i][j-1] == controlPoint ||
							item->curveSegments[i][j+1] == controlPoint)
						{

							index = j;
							break;
						}
					}
					if (index > -1)
					{
						graphicsItems << item->curveSegments[i][index-1]
						<< item->curveSegments[i][index]
						<< item->curveSegments[i][index+1];

						listK1 << i << i << i;
						listK2 << (index-1) << index << (index+1);

						item->curveSegments[i][index-1]->setParentItem(0);
						item->curveSegments[i][index]->setParentItem(0);
						item->curveSegments[i][index+1]->setParentItem(0);
						graphicsScene->removeItem(item->curveSegments[i][index-1]);
						graphicsScene->removeItem(item->curveSegments[i][index]);
						graphicsScene->removeItem(item->curveSegments[i][index+1]);

						item->curveSegments[i].remove(index-1,3);
						//item->refresh();

						break;
					}
				}
				item->refresh();
			}
	}

	RemoveCurveSegmentCommand::RemoveCurveSegmentCommand(
		const QString& name, GraphicsScene * scene,
		ConnectionGraphicsItem::ControlPoint* item)
		: QUndoCommand(name)
	{
		curveSegments.clear();
		graphicsScene = scene;
		connectionItem = item->connectionItem;
		if (connectionItem == 0 || connectionItem->curveSegments.size() < 2) return;
		
		QList<ArrowHeadItem*> arrowHeads = connectionItem->arrowHeads();
		int only_in_node = -1, only_out_node = -1;
		for (int i=0; i < arrowHeads.size(); ++i)
		{
			if (arrowHeads[i] == 0)
			{
				if (only_in_node == -1)
					only_in_node = i;
				else
				{
					only_in_node = -1;
					break;
				}
			}
		}
		for (int i=0; i < arrowHeads.size(); ++i)
		{
			if (arrowHeads[i] != 0)
			{
				if (only_out_node == -1)
					only_out_node = i;
				else
				{
					only_out_node = -1;
					break;
				}
			}
		}

		for (int i=0; i < connectionItem->curveSegments.size(); ++i)
		{
			for (int j=0; j < connectionItem->curveSegments[i].size(); ++j)
			{
				if (connectionItem->curveSegments[i][j] != 0 && connectionItem->curveSegments[i][j] == item)
				{
					if (i != only_in_node && i != only_out_node)
					{
						curveSegments.append(connectionItem->curveSegments[i]);
						if (connectionItem->curveSegments[i][0])
							parentsAtStart.append(connectionItem->curveSegments[i][0]->parentItem());
						else
							parentsAtStart.append(0);
						if (connectionItem->curveSegments[i].last())
							parentsAtEnd.append(connectionItem->curveSegments[i].last()->parentItem());
						else
							parentsAtEnd.append(0);
					}
					break;
				}
			}
		}
	}

	RemoveCurveSegmentCommand::RemoveCurveSegmentCommand(
		const QString& name, GraphicsScene * scene,
		ConnectionGraphicsItem* connection,
		QList<ConnectionGraphicsItem::ControlPoint*> items)
		: QUndoCommand(name)
	{
		curveSegments.clear();
		graphicsScene = scene;
		connectionItem = connection;
		if (connectionItem == 0 || connectionItem->curveSegments.size() < 2) return;

		bool done = false;
		for (int i=0; i < connectionItem->curveSegments.size(); ++i)
		{
			done = false;
			for (int j=0; j < connectionItem->curveSegments[i].size(); ++j)
			{
				for (int k = 0; k < items.size(); ++k)
				{
					if (connectionItem->curveSegments[i][j] != 0 && connectionItem->curveSegments[i][j] == items[k])
					{
						curveSegments.append(connectionItem->curveSegments[i]);
						if (connectionItem->curveSegments[i][0])
							parentsAtStart.append(connectionItem->curveSegments[i][0]->parentItem());
						else
							parentsAtStart.append(0);
						if (connectionItem->curveSegments[i].last())
							parentsAtEnd.append(connectionItem->curveSegments[i].last()->parentItem());
						else
							parentsAtEnd.append(0);
						done = true;
						break;
					}
				}
				if (done) break;
			}
		}
	}

	void RemoveCurveSegmentCommand::undo()
	{
		if (connectionItem == 0) return;

		if (connectionItem->curveSegments.size() == 1 && connectionItem->curveSegments.size() >= 4
			&& curveSegments.size() > 0 && curveSegments[0].at(3) != 0
			&& connectionItem->curveSegments[0][3]->scene() != 0)
		{
			connectionItem->curveSegments[0][3]->setParentItem(0);
			connectionItem->curveSegments[0][3]->scene()->removeItem(connectionItem->curveSegments[0][3]);
			connectionItem->curveSegments[0][3] = curveSegments[0].at(3);
		}
		for (int i=0; i < curveSegments.size(); ++i)
		{
			if (curveSegments[i][0] && parentsAtStart.size() > i && parentsAtStart[i] &&
				!MainWindow::invalidPointers.contains((void*)parentsAtStart[i]))
				curveSegments[i][0]->setParentItem(parentsAtStart[i]);
			if (curveSegments[i].last() && parentsAtEnd.size() > i && parentsAtEnd[i] &&
				!MainWindow::invalidPointers.contains((void*)parentsAtEnd[i]))
				curveSegments[i].last()->setParentItem(parentsAtStart[i]);

			connectionItem->curveSegments.append(curveSegments[i]);
			for (int j=0; j < curveSegments[i].size(); ++j)
				if (curveSegments[i][j] != 0)
					curveSegments[i][j]->setVisible( connectionItem->controlPointsVisible );
			if (curveSegments[i].arrowStart)
				curveSegments[i].arrowStart->setVisible(true);
			if (curveSegments[i].arrowEnd)
				curveSegments[i].arrowEnd->setVisible(true);
		}
		connectionItem->refresh();
	}

	void RemoveCurveSegmentCommand::redo()
	{
		if (connectionItem == 0) return;
		int k;
		for (int i=0; i < curveSegments.size(); ++i)
		{
			k = connectionItem->curveSegments.indexOf(curveSegments[i]);
			if (k >=0 && k < connectionItem->curveSegments.size())
				connectionItem->curveSegments.removeAt(k);
			for (int j=0; j < curveSegments[i].size(); ++j)
				if (curveSegments[i][j] != 0)
				{
					curveSegments[i][j]->setVisible(false);
					if (curveSegments[i][j]->parentItem())
						curveSegments[i][j]->setParentItem(0);
				}
			if (curveSegments[i].arrowStart)
				curveSegments[i].arrowStart->setVisible(false);
			if (curveSegments[i].arrowEnd)
				curveSegments[i].arrowEnd->setVisible(false);
		}
		if (connectionItem->curveSegments.size() == 1 && connectionItem->curveSegments.size() >= 4
			&& curveSegments.size() > 0 && curveSegments[0].at(0) != 0)
		{
			connectionItem->curveSegments[0][3] = new ConnectionGraphicsItem::ControlPoint(*connectionItem->curveSegments[0][3]);
			connectionItem->curveSegments[0][3]->setParentItem(curveSegments[0].at(0)->parentItem());
		}
		connectionItem->refresh();
	}

	AddCurveSegmentCommand::AddCurveSegmentCommand(
		const QString& name, GraphicsScene * scene,
		ConnectionGraphicsItem* connection,
		ConnectionGraphicsItem::CurveSegment& item)
		: QUndoCommand(name)
	{
		curveSegments.clear();
		graphicsScene = scene;
		connectionItem = connection;
		if (connectionItem == 0) return;
		curveSegments.append(item);
	}

	AddCurveSegmentCommand::AddCurveSegmentCommand(
		const QString& name, GraphicsScene * scene,
		ConnectionGraphicsItem* connection,
		QList<ConnectionGraphicsItem::CurveSegment> items)
		: QUndoCommand(name)
	{
		curveSegments.clear();
		graphicsScene = scene;
		connectionItem = connection;
		if (connectionItem == 0) return;
		curveSegments << items;

	}

	void AddCurveSegmentCommand::redo()
	{
		if (connectionItem == 0) return;

		for (int i=0; i < curveSegments.size(); ++i)
		{
			connectionItem->curveSegments.append(curveSegments[i]);

			if (curveSegments[i].arrowStart)
				curveSegments[i].arrowStart->setVisible(true);
			if (curveSegments[i].arrowEnd)
				curveSegments[i].arrowEnd->setVisible(true);
		}
		connectionItem->refresh();
	}

	void AddCurveSegmentCommand::undo()
	{
		if (connectionItem == 0) return;
		int k;
		for (int i=0; i < curveSegments.size(); ++i)
		{
			k = connectionItem->curveSegments.indexOf(curveSegments[i]);
			if (k >=0 && k < connectionItem->curveSegments.size())
				connectionItem->curveSegments.removeAt(k);

			if (curveSegments[i].arrowStart)
				curveSegments[i].arrowStart->setVisible(false);
			if (curveSegments[i].arrowEnd)
				curveSegments[i].arrowEnd->setVisible(false);
		}
		connectionItem->refresh();
	}

	AddCurveSegmentCommand::~AddCurveSegmentCommand()
	{
		for (int i=0; i < curveSegments.size(); ++i)
		{
			for (int j=0; j < curveSegments[i].size(); ++j)
				if (curveSegments[i][j])
				{
					curveSegments[i][j]->setParentItem(0);
					if (curveSegments[i][j] && !curveSegments[i][j]->connectionItem && !curveSegments[i][j]->scene())
					{
						delete curveSegments[i][j];
					}
				}
				if (curveSegments[i].arrowStart && !curveSegments[i].arrowStart->scene())
				{
					curveSegments[i].arrowStart->setParentItem(0);
					delete curveSegments[i].arrowStart;
				}
				if (curveSegments[i].arrowEnd && !curveSegments[i].arrowEnd->scene())
				{
					curveSegments[i].arrowEnd->setParentItem(0);
					delete curveSegments[i].arrowEnd;
				}
		}
	}

	/*! \brief gets the connection graphics item from its child item*/
	ConnectionGraphicsItem * ConnectionGraphicsItem::topLevelConnectionItem(QGraphicsItem* item,bool includeControlPoints)
	{
		ConnectionGraphicsItem * idptr = 0, * idptr2 = 0;
		ConnectionGraphicsItem::ControlPoint * cp = 0;

		QGraphicsItem * p = item;

		while (p)
		{
			idptr = qgraphicsitem_cast<ConnectionGraphicsItem*>(p);
			if (!idptr && includeControlPoints)
			{
				cp = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(p);
				if (cp && cp->connectionItem)
					idptr = cp->connectionItem;
			}

			if (idptr != 0) idptr2 = idptr;

			if (p != p->parentItem())
				p = p->parentItem();
			else
				break;
		}

		return idptr2;
	}
	
	bool ConnectionGraphicsItem::isModifier() const
	{
		NodeGraphicsItem* node;
		ArrowHeadItem * arrow;
		
		for (int i=0; i < curveSegments.size(); ++i)
			if (curveSegments[i].size() > 0 && curveSegments[i][0])
			{
				node = NodeGraphicsItem::cast(curveSegments[i][0]->parentItem());
				if (node && node->isVisible() && node->className == ArrowHeadItem::CLASSNAME)
				{
					arrow = static_cast<ArrowHeadItem*>(node);
					if (arrow->connectionItem && arrow->connectionItem->centerRegionItem == arrow)
						return true;
				}
				if (curveSegments[i].size() > 1 && curveSegments[i][curveSegments[i].size()-1])
				{
					node = NodeGraphicsItem::cast(curveSegments[i][curveSegments[i].size()-1]->parentItem());
					if (node && node->isVisible() && node->className == ArrowHeadItem::CLASSNAME)
					{
						arrow = static_cast<ArrowHeadItem*>(node);
						if (arrow->connectionItem && arrow->connectionItem->centerRegionItem == arrow)
							return true;
					}
				}
			}
		return false;
	}

	/*! \brief find all the nodes that are connected
	* \return node item list or 0*/
	QList<NodeGraphicsItem*> ConnectionGraphicsItem::nodes() const
	{
		QList<NodeGraphicsItem*> nodes;
		NodeGraphicsItem* node;

		for (int i=0; i < curveSegments.size(); ++i)
			if (curveSegments[i].size() > 0 && curveSegments[i][0])
			{
				node = NodeGraphicsItem::cast(curveSegments[i][0]->parentItem());
				if (node && node->isVisible() && node != curveSegments[i].arrowStart && node != curveSegments[i].arrowEnd)
					nodes += node;
				if (curveSegments[i].size() > 1 && curveSegments[i][curveSegments[i].size()-1])
				{
					node = NodeGraphicsItem::cast(curveSegments[i][curveSegments[i].size()-1]->parentItem());
					if (node && node->isVisible() && node != curveSegments[i].arrowStart && node != curveSegments[i].arrowEnd)
						nodes += node;
				}
			}

		return nodes;
	}


	/*! \brief find all the nodes that are connected
	* \return node item list or 0*/
	QList<NodeGraphicsItem*> ConnectionGraphicsItem::nodesWithArrows() const
	{
		QList<NodeGraphicsItem*> nodes;
		NodeGraphicsItem* node;

		for (int i=0; i < curveSegments.size(); ++i)
			if (curveSegments[i].size() > 0 && curveSegments[i][0])
			{
				node = NodeGraphicsItem::cast(curveSegments[i][0]->parentItem());
				if (node && curveSegments[i].arrowStart && node->isVisible() && node != curveSegments[i].arrowStart && node != curveSegments[i].arrowEnd)
					nodes += node;
				if (curveSegments[i].size() > 1 && curveSegments[i][curveSegments[i].size()-1])
				{
					node = NodeGraphicsItem::cast(curveSegments[i][curveSegments[i].size()-1]->parentItem());
					if (node && curveSegments[i].arrowEnd && node->isVisible() && node != curveSegments[i].arrowStart && node != curveSegments[i].arrowEnd)
						nodes += node;
				}
			}

			return nodes;
	}

	/*! \brief find all the nodes that are connected
	* \return node item list or 0*/
	QList<NodeGraphicsItem*> ConnectionGraphicsItem::nodesWithoutArrows() const
	{
		QList<NodeGraphicsItem*> nodes;
		NodeGraphicsItem* node;

		for (int i=0; i < curveSegments.size(); ++i)
			if (curveSegments[i].size() > 0 && curveSegments[i][0])
			{
				node = NodeGraphicsItem::cast(curveSegments[i][0]->parentItem());
				if (node && !curveSegments[i].arrowStart && node->isVisible() && node != curveSegments[i].arrowStart && node != curveSegments[i].arrowEnd)
					nodes += node;
				if (curveSegments[i].size() > 1 && curveSegments[i][curveSegments[i].size()-1])
				{
					node = NodeGraphicsItem::cast(curveSegments[i][curveSegments[i].size()-1]->parentItem());
					if (node && !curveSegments[i].arrowEnd && node->isVisible() && node != curveSegments[i].arrowStart && node != curveSegments[i].arrowEnd)
						nodes += node;
				}
			}

			return nodes;
	}

	/*! \brief find all the nodes that are connected
	* \return node item list or 0*/
	QList<NodeGraphicsItem*> ConnectionGraphicsItem::nodesDisconnected() const
	{
		QList<NodeGraphicsItem*> nodes;
		NodeGraphicsItem* node;

		ControlPoint * center = centerPoint();

		for (int i=0; i < curveSegments.size(); ++i)
			if (curveSegments[i].size() > 1 && curveSegments[i][0] && !curveSegments[i].contains(center))
			{
				node = NodeGraphicsItem::cast(curveSegments[i][0]->parentItem());
				if (node)
					nodes += node;
			}

			return nodes;
	}

	/*! \brief find all the nodes that are connected
	* \return node item list or 0*/
	QList<QGraphicsItem*> ConnectionGraphicsItem::nodesAsGraphicsItems() const
	{
		QList<QGraphicsItem*> nodes;
		NodeGraphicsItem* node;

		for (int i=0; i < curveSegments.size(); ++i)
			if (curveSegments[i].size() > 0 && curveSegments[i][0])
			{
				node = NodeGraphicsItem::cast(curveSegments[i][0]->parentItem());
				if (node && node->isVisible() && node != curveSegments[i].arrowStart && node != curveSegments[i].arrowEnd)
					nodes += node;
				if (curveSegments[i].size() > 1)
				{
					node = NodeGraphicsItem::cast(curveSegments[i][curveSegments[i].size()-1]->parentItem());
					if (node && node->isVisible() && node != curveSegments[i].arrowStart && node != curveSegments[i].arrowEnd)
						nodes += node;
				}
			}

			return nodes;
	}

	/*! \brief get all the arrow heads in the same order as nodes
	* \return node item list*/
	QList<ArrowHeadItem*> ConnectionGraphicsItem::arrowHeads() const
	{
		QList<ArrowHeadItem*> arrows;
		NodeGraphicsItem* node;

		for (int i=0; i < curveSegments.size(); ++i)
			if (curveSegments[i].size() > 0 && curveSegments[i][0])
			{
				node = NodeGraphicsItem::cast(curveSegments[i][0]->parentItem());
				if (node && node->isVisible() && node != curveSegments[i].arrowStart && node != curveSegments[i].arrowEnd)
					arrows += curveSegments[i].arrowStart;
				if (curveSegments[i].size() > 1)
				{
					node = NodeGraphicsItem::cast(curveSegments[i][curveSegments[i].size()-1]->parentItem());
					if (node && node->isVisible() && node != curveSegments[i].arrowStart && node != curveSegments[i].arrowEnd)
						arrows += curveSegments[i].arrowEnd;
				}
			}

			return arrows;
	}

	/*! \brief get all the arrow heads in the same order as nodes
	* \return node item list*/
	QList<QGraphicsItem*> ConnectionGraphicsItem::arrowHeadsAsGraphicsItems() const
	{
		QList<QGraphicsItem*> arrows;
		NodeGraphicsItem* node;

		for (int i=0; i < curveSegments.size(); ++i)
			if (curveSegments[i].size() > 0 && curveSegments[i][0])
			{
				node = NodeGraphicsItem::cast(curveSegments[i][0]->parentItem());
				if (node && node->isVisible() && node != curveSegments[i].arrowStart && node != curveSegments[i].arrowEnd)
					arrows += curveSegments[i].arrowStart;
				if (curveSegments[i].size() > 1)
				{
					node = NodeGraphicsItem::cast(curveSegments[i][curveSegments[i].size()-1]->parentItem());
					if (node && node->isVisible() && node != curveSegments[i].arrowStart && node != curveSegments[i].arrowEnd)
						arrows += curveSegments[i].arrowEnd;
				}
			}

			return arrows;
	}

	/*! \brief find all the modifier arrow heads in the same order as nodes
	* \return node item list*/
	QList<ArrowHeadItem*> ConnectionGraphicsItem::modifierArrowHeads() const
	{
		QList<ArrowHeadItem*> arrows;
		NodeGraphicsItem* node;

		for (int i=0; i < curveSegments.size(); ++i)
			if (curveSegments[i].size() > 0 && curveSegments[i][0])
			{
				node = NodeGraphicsItem::cast(curveSegments[i][0]->parentItem());
				if (node && node->isVisible() && node == curveSegments[i].arrowStart)
					arrows += curveSegments[i].arrowStart;
				if (curveSegments[i].size() > 1)
				{
					node = NodeGraphicsItem::cast(curveSegments[i][curveSegments[i].size()-1]->parentItem());
					if (node && node->isVisible() && node == curveSegments[i].arrowEnd)
						arrows += curveSegments[i].arrowEnd;
				}
			}

			return arrows;
	}

	/*! \brief find the node that connected to the particular path
	* \param index less than size of curveSegments
	* \return node item or 0*/
	NodeGraphicsItem* ConnectionGraphicsItem::nodeAt(int index) const
	{
		NodeGraphicsItem * node = 0;
		if (curveSegments.size() == 1)
		{
			if (index < 0 || index > 1 ||
				curveSegments[0].size() < 4 || curveSegments[0][curveSegments[0].size()-1] == 0) return 0;

			if (index == 0)
				node = NodeGraphicsItem::cast(curveSegments[0][0]->parentItem());
			else
				node = NodeGraphicsItem::cast(curveSegments[0][curveSegments[0].size()-1]->parentItem());

			if (node == curveSegments[0].arrowStart || node == curveSegments[0].arrowEnd)
				node = 0;
			
			return node;
		}

		if (index < 0 || index >= curveSegments.size() ||
			curveSegments[index].size() < 4 ||
			curveSegments[index][0] == 0)
			return 0;

		node = NodeGraphicsItem::cast(curveSegments[index][0]->parentItem());
		if (node == curveSegments[index].arrowStart || node == curveSegments[index].arrowEnd)
			node = 0;

		if (node && node->isVisible())
			return node;

		return 0;
	}

	/*! \brief find the index of the node
	* \param node in this connection
	* \return index, -1 if node not found*/
	int ConnectionGraphicsItem::indexOf(QGraphicsItem * target) const
	{
		if (!target) return -1;
		
		if (curveSegments.size() == 1 && (curveSegments[0].size() > 1 && curveSegments[0][0]))
		{
			for (int i=0; i < curveSegments[0].size(); ++i)
				if (target == curveSegments[0][i])
					return 0;

			if (target == curveSegments[0].arrowStart) return 0;
			
			if (target == curveSegments[0].arrowEnd) return 1;

			if (target == curveSegments[0][0]->parentItem()) return 0;
			
			if (curveSegments[0][curveSegments[0].size()-1] && 
				(target == curveSegments[0][curveSegments[0].size()-1]->parentItem())) return 1;
		}

		for (int i=0; i < curveSegments.size(); ++i)
			if (curveSegments[i].size() > 1 && curveSegments[i][0])
			{
				if (target == curveSegments[i].arrowStart || 
					target == curveSegments[i].arrowEnd ||
					target == curveSegments[i][0]->parentItem()) return i;
				
				if (curveSegments[i][curveSegments[i].size()-1] &&
					target == curveSegments[i][curveSegments[i].size()-1]->parentItem()) return i;
					
				for (int j=0; j < curveSegments[i].size(); ++j)
					if (target == curveSegments[i][j])
						return i;
			}

		return -1;
	}
	
	/*! \brief find the arrow head at the particular index
	* \param index less than size of curveSegments
	* \return node item or 0*/
	ArrowHeadItem* ConnectionGraphicsItem::arrowAt(int index) const
	{
		ArrowHeadItem* arrow = 0;
		NodeGraphicsItem * node;
		if (curveSegments.size() == 1)
		{
			if (index < 0 || index > 1 ||
				curveSegments[0].size() < 4 || curveSegments[0][curveSegments[0].size()-1] == 0) return 0;

			if (index == 0)
			{
				node = NodeGraphicsItem::cast(curveSegments[0][0]->parentItem());
				arrow = curveSegments[0].arrowStart;
			}
			else
			{
				node = NodeGraphicsItem::cast(curveSegments[0][curveSegments[0].size()-1]->parentItem());
				arrow = curveSegments[0].arrowEnd;
			}
			if (node == curveSegments[0].arrowStart || node == curveSegments[0].arrowEnd)
			{
				node = 0;
				arrow = 0;
			}
		}

		if (index < 0 || index >= curveSegments.size() ||
			curveSegments[index].size() < 4 ||
			curveSegments[index][0] == 0)
			return 0;

		node = NodeGraphicsItem::cast(curveSegments[index][0]->parentItem());
		arrow = curveSegments[index].arrowStart;
		if (node == curveSegments[index].arrowStart || node == curveSegments[index].arrowEnd)
		{
			node = 0;
			arrow = 0;
		}

		if (arrow && arrow->isVisible())
			return arrow;

		return 0;
	}

	/*! \brief find the modifier arrow head at the particular index
	* \param index less than size of curveSegments
	* \return node item or 0*/
	ArrowHeadItem* ConnectionGraphicsItem::modifierArrowAt(int index) const
	{
		if (index < 0 || index >= curveSegments.size())
			return 0;

		if (arrowAt(index) == curveSegments[index].arrowEnd) return 0;

		return curveSegments[index].arrowEnd;
	}

	void ConnectionGraphicsItem::replaceNode(NodeGraphicsItem* oldNode, NodeGraphicsItem* newNode)
	{
		if (oldNode == 0 || newNode == 0) return;

		for (int i=0; i < curveSegments.size(); ++i)
		{
			if (curveSegments[i].size() > 0)
			{
				if (curveSegments[i][0]->parentItem() == oldNode)
					curveSegments[i][0]->setParentItem(newNode);
				if (curveSegments[i][curveSegments[i].size() - 1]->parentItem() == oldNode)
					curveSegments[i][curveSegments[i].size() - 1]->setParentItem(newNode);
			}
		}
	}

	void ConnectionGraphicsItem::replaceNodeAt(int index, NodeGraphicsItem* nodeItem)
	{
		if (nodeItem == 0) return;

		if (curveSegments.size() == 1)
		{
			if (index < 0 || index > 1 ||
				curveSegments[0].size() < 4 || curveSegments[0][0] == 0) return;

			if (index == 0)
				curveSegments[0][0]->setParentItem(nodeItem);
			else
				curveSegments[0][curveSegments[0].size()-1]->setParentItem(nodeItem);

			return;
		}

		if (index < 0 || index >= curveSegments.size() ||
			curveSegments[index].size() < 4 ||
			curveSegments[index][0] == 0) return;

		curveSegments[index][0]->setParentItem(nodeItem);
	}

	void ArrowHeadItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option ,QWidget *widget)
	{
		if (!boundaryControlPoints.isEmpty())
		{
			for (int i=0; i < boundaryControlPoints.size(); ++i)
				if (boundaryControlPoints[i])
				{
					boundaryControlPoints[i]->nodeItem = 0;
					boundaryControlPoints[i]->setParentItem(0);
					if (!boundaryControlPoints[i]->scene())
						delete boundaryControlPoints[i];
				}
				boundaryControlPoints.clear();
		}
		if (connectionItem && connectionItem->scene() == this->scene())
		{
			NodeGraphicsItem::paint(painter,option,widget);
		}
		else
		{
			this->setParentItem(0);
			if (this->scene())
				scene()->removeItem(this);
		}
	}

	ConnectionGraphicsItem::CurveSegment::CurveSegment() : QVector<ConnectionGraphicsItem::ControlPoint*>()
	{
		arrowStart = arrowEnd = 0;
	}

	ConnectionGraphicsItem::CurveSegment::CurveSegment(int n) : QVector<ConnectionGraphicsItem::ControlPoint*>(n)
	{
		arrowStart = arrowEnd = 0;
	}

	ConnectionGraphicsItem::CurveSegment::CurveSegment(int n,ConnectionGraphicsItem::ControlPoint* p)
		: QVector<ConnectionGraphicsItem::ControlPoint*>(n,p)
	{
		arrowStart = arrowEnd = 0;
	}

	ConnectionGraphicsItem::CurveSegment::CurveSegment(const ConnectionGraphicsItem::CurveSegment& copy) :
	QVector<ConnectionGraphicsItem::ControlPoint*>(copy)
	{
		arrowStart = copy.arrowStart;
		arrowEnd = copy.arrowEnd;
	}

	/*! \brief gets the point on the edge of the rect such that it is in the same
	line as the center of the rect and the point (arg)
	\param rectangle
	\param point outside rectangle
	\return the point on the edge of the rectangle
	*/
	QPointF pointOnEdge(const QRectF& rect0, const QPointF& p1, qreal dist, bool straight)
	{
		QRectF rect1 = rect0;

		if (dist > 0)
		{
			rect1.adjust(-dist,-dist,dist,dist);
		}

		if (straight)
		{
			if (p1.x() > rect0.left() && p1.x() < rect0.right())
				if (p1.y() > rect0.center().y())
					return QPointF(p1.x(),rect1.bottom());
				else
					return QPointF(p1.x(),rect1.top());

			if (p1.y() > rect0.top() && p1.y() < rect0.bottom())
				if (p1.x() > rect0.center().x())
					return QPointF(rect1.right(),p1.y());
				else
					return QPointF(rect1.left(),p1.y());
		}

		QPointF p0 = rect1.center(), p;
		qreal w1 = rect1.width() / 2.0,
			h1 = rect1.height() / 2.0;

		if (p1.x() < p0.x()) { w1 = -w1; }
		if (p1.y() < p0.y()) { h1 = -h1; }

		if (p1.x() != p0.x())
		{
			qreal slope = (p1.y() - p0.y())/(p1.x() - p0.x());
			if (abs((int)(w1 * slope)) < abs((int)h1))
			{
				p.rx() = p0.x() + w1;
				p.ry() = p0.y() + (w1*slope);

			}
			else
			{
				p.rx() = p0.x() + (h1/slope);
				p.ry() = p0.y() + h1;
			}
		}
		else
		{
			p.rx() = p0.x();
			p.ry() = p0.y() + h1;
		}

		return p;
	}

	/*! \brief gets the point on the edge of the shape such that it is in the same
	line as the center of the rect and the point (arg)
	\param shape
	\param point outside rectangle
	\return the point on the edge of the shape
	*/
	QPointF pointOnEdge(const NodeGraphicsItem& node, const QPointF& pt, qreal dist, bool straight)
	{
		QRectF rect0 = node.sceneBoundingRect();
		//QPointF p0 = pointOnEdge(rect0,pt,dist,straight);
		QPointF p0 = pt;
		QPointF p1, p;
		qreal d = 0;

		for (int i=0; i < node.shapes.size(); ++i)
			if (node.shapes[i])
			{
				QRectF rect1 = node.shapes[i]->sceneBoundingRect().adjusted(-dist,-dist,dist,dist);

				if (!straight || d == 0 ||
					(straight &&
					(	(pt.x() > rect1.left() && pt.x() < rect1.right()) ||
					(pt.y() > rect1.top() && pt.y() < rect1.bottom())) ))
				{
					p1 = pointOnEdge(rect1,pt,dist,straight);
					if (d==0 || (((p1.rx() - p0.rx())*(p1.rx() - p0.rx()) + (p1.ry() - p0.ry())*(p1.ry() - p0.ry())) < d))
					{
						d = ((p1.rx() - p0.rx())*(p1.rx() - p0.rx()) + (p1.ry() - p0.ry())*(p1.ry() - p0.ry()));
						p = p1;
					}
				}
			}

			return p;
	}

	ConnectionGraphicsItem* ConnectionGraphicsItem::cast(QGraphicsItem * q)
	{
		if (MainWindow::invalidPointers.contains( (void*)q )) return 0;
		return qgraphicsitem_cast<ConnectionGraphicsItem*>(q);
	}
	
	QList<ConnectionGraphicsItem*> ConnectionGraphicsItem::cast(const QList<QGraphicsItem*>& list)
	{
		QList<ConnectionGraphicsItem*> connections;
		ConnectionGraphicsItem* q;
		for (int i=0; i < list.size(); ++i)
			if (!MainWindow::invalidPointers.contains( (void*)(list[i]) ) && 
				(q = qgraphicsitem_cast<ConnectionGraphicsItem*>(list[i])))
				connections << q;
		return connections;
	}

	ConnectionGraphicsItem::ConnectionGraphicsItem(const QList<NodeGraphicsItem*>& from, const QList<NodeGraphicsItem*>& to, QGraphicsItem * parent) :
		QGraphicsPathItem (parent), itemHandle(0)
	{
		setCacheMode(QGraphicsItem::DeviceCoordinateCache);
		setFlag(QGraphicsItem::ItemIsMovable, false);
		setFlag(QGraphicsItem::ItemIsSelectable, false);

		className = ConnectionGraphicsItem::CLASSNAME;
		lineType = line;
		arrowHeadDistance = 10.0;
		centerRegionItem = 0;
		centerRegion = QSizeF(20,20);
		defaultPen = QPen(QColor(50,50,255,255),5.0);
		defaultPen.setJoinStyle(Qt::RoundJoin);
		setPen(defaultPen);
		setBrush(defaultBrush = Qt::NoBrush);
		boundaryPathItem = new QGraphicsPathItem(this);

		boundaryPathItem->setVisible(false);
		boundaryPathItem->setPen(QPen(QColor(255,150,150,150),4.0,Qt::DotLine));
		boundaryPathItem->setBrush(Qt::NoBrush);

		NodeGraphicsReader imageReader;
		/*ArrowHeadItem * node = new ArrowHeadItem;
		node->connectionItem = this;
		imageReader.readXml(node,DefaultMiddleItemFile);
		if (node->isValid())
		{
			node->normalize();
			node->scale(25.0/node->sceneBoundingRect().height(),25.0/node->sceneBoundingRect().height());
		}*/
		centerRegionItem = 0;

		if (from.size() < 1 || to.size() < 1) return;

		QPointF center;

		for (int i=0; i < from.size(); ++i)
			if (from[i])
				center += from[i]->scenePos();

		for (int i=0; i < to.size(); ++i)
			if (to[i])
				center += to[i]->scenePos();

		center /= (from.size() + to.size());
		ControlPoint * centerPoint = new ControlPoint(center,this,0);

		for (int i=0; i < from.size(); ++i)
			if (from[i])
			{
				CurveSegment cv;
				cv 	<< new ControlPoint(from[i]->scenePos(),this,from[i])
					<< new ControlPoint((from[i]->scenePos() + center)/2.0,this,0)
					<< new ControlPoint((from[i]->scenePos() + center)/2.0,this,0)
					<< centerPoint;
				curveSegments.append(cv);
			}

		for (int i=0; i < to.size(); ++i)
			if (to[i])
			{
				CurveSegment cv;
				cv 	<< new ControlPoint(to[i]->scenePos(),this,to[i])
					<< new ControlPoint((to[i]->scenePos() + center)/2.0,this,0)
					<< new ControlPoint((to[i]->scenePos() + center)/2.0,this,0)
					<< centerPoint;
				cv.arrowStart = new ArrowHeadItem(this);
				imageReader.readXml(cv.arrowStart,DefaultArrowHeadFile);
				if (cv.arrowStart->isValid())
				{
					cv.arrowStart->normalize();
					cv.arrowStart->scale(25.0/cv.arrowStart->sceneBoundingRect().height(),25.0/cv.arrowStart->sceneBoundingRect().height());
				}
				curveSegments.append(cv);
			}
	}
}
