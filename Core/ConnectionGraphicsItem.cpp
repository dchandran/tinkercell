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

namespace Tinkercell
{
	QString ArrowHeadItem::class_name("ArrowHeadItem");
	QString ConnectionGraphicsItem::class_name("ConnectionGraphicsItem");

	ControlPoint * ControlPoint::getControlPoint(QGraphicsItem* item)
	{
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
		className = ArrowHeadItem::class_name;
		connectionItem = connection;
		angle = 0;
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

		className = ConnectionGraphicsItem::class_name;
		lineType = bezier;
		arrowHeadDistance = 10.0;
		centerRegionItem = 0;
		centerRegion = QSizeF(20,20);
		centerRegionVisible = false;
		defaultPen = QPen(QColor(50,50,255,255),5.0);
		defaultPen.setJoinStyle(Qt::RoundJoin);
		setPen(defaultPen);
		setBrush(Qt::NoBrush);
		boundaryPathItem = new QGraphicsPathItem(this);

		boundaryPathItem->setVisible(false);
		boundaryPathItem->setPen(QPen(QColor(255,150,150,150),4.0,Qt::DotLine));

		QString appDir = QCoreApplication::applicationDirPath();

		ArrowHeadItem * node = new ArrowHeadItem;	
		node->connectionItem = this;
		NodeGraphicsReader imageReader;		
		imageReader.readXml(node,appDir + QString("/OtherItems/simplecircle.xml"));
		if (node->isValid())
		{
			node->normalize();
			node->scale(25.0/node->sceneBoundingRect().height(),25.0/node->sceneBoundingRect().height());
		}
		centerRegionItem = node;
	}	

	/*! Copy Constructor: deep copy of all pointers */
	ConnectionGraphicsItem::ConnectionGraphicsItem(const ConnectionGraphicsItem& copy) : QGraphicsPathItem (copy.parentItem())
	{
		setFlag(QGraphicsItem::ItemIsMovable, false);
		setFlag(QGraphicsItem::ItemIsSelectable, false);

		className = copy.className;

		boundaryPathItem = new QGraphicsPathItem(this);
		boundaryPathItem->setVisible(false);
		boundaryPathItem->setPen(QPen(QColor(255,150,150,150),4.0,Qt::DotLine));

		centerRegionItem = 0;
		if (copy.centerRegionItem)
		{
			centerRegionItem = new ArrowHeadItem(*copy.centerRegionItem);
			centerRegionItem->connectionItem = this;
		}

		centerRegion = copy.centerRegion;
		centerRegionVisible = copy.centerRegionVisible;

		itemHandle = copy.itemHandle;

		if (itemHandle)
			setHandle(this,itemHandle);

		pathVectors = copy.pathVectors;
		lineType = copy.lineType;
		arrowHeadDistance = copy.arrowHeadDistance;
		setPen(defaultPen = copy.defaultPen);
		setPos(copy.scenePos());
		setTransform(copy.sceneTransform());

		for (int i=0; i < copy.pathVectors.size(); ++i)
		{
			if (copy.pathVectors[i].arrowStart)
			{
				pathVectors[i].arrowStart = new ArrowHeadItem(*copy.pathVectors[i].arrowStart);
				pathVectors[i].arrowStart->connectionItem = this;
			}
			if (copy.pathVectors[i].arrowEnd)
			{
				pathVectors[i].arrowEnd = new ArrowHeadItem(*copy.pathVectors[i].arrowEnd);
				pathVectors[i].arrowEnd->connectionItem = this;
			}
		}

		QList<ConnectionGraphicsItem::ControlPoint*> uniquePoints;
		QList<QPoint> locations, positions;

		for (int i=0; i < copy.pathVectors.size(); ++i)
			for (int j=0; j < copy.pathVectors[i].size(); ++j)
			{
				positions += QPoint(i,j);

				int k = uniquePoints.indexOf(copy.pathVectors[i][j]);
				if (k > -1)
				{
					locations += locations[k];
					uniquePoints += 0;
				}
				else
				{
					locations += QPoint(i,j);
					uniquePoints += (copy.pathVectors[i][j]);
				}
			}

			NodeGraphicsItem * parentNode = 0;
			for (int k=0; k < uniquePoints.size() && k < locations.size() && k < positions.size(); ++k)
			{
				int i = positions[k].x(),
					j = positions[k].y();
				if (copy.pathVectors[i][j])
				{
					if (uniquePoints[k])
					{
						pathVectors[i][j] = new ConnectionGraphicsItem::ControlPoint(*copy.pathVectors[i][j]);
						pathVectors[i][j]->connectionItem = this;
						pathVectors[i][j]->setVisible(false);

						if (copy.pathVectors[i][j]->parentItem())
						{
							parentNode = qgraphicsitem_cast<NodeGraphicsItem*>(copy.pathVectors[i][j]->parentItem());
							if (parentNode == copy.pathVectors[i].arrowEnd)
								pathVectors[i][j]->setParentItem(pathVectors[i].arrowEnd);
							else
								if (parentNode == copy.pathVectors[i].arrowStart)
									pathVectors[i][j]->setParentItem(pathVectors[i].arrowStart);
								else
									pathVectors[i][j]->setParentItem(copy.pathVectors[i][j]->parentItem());

						}
					}
					else
					{
						pathVectors[i][j] = pathVectors[locations[k].x()][locations[k].y()];
					}
				}
			}
	}

	/*! \brief make a copy of this item*/
	ConnectionGraphicsItem* ConnectionGraphicsItem::clone() const
	{
		return new ConnectionGraphicsItem(*this);
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
				if (ControlPoint::getControlPoint(children[i]))
				{
					children[i]->setParentItem(centerRegionItem);
				}
		}
		else
		{
			QString appDir = QCoreApplication::applicationDirPath();

			ArrowHeadItem * node = new ArrowHeadItem;	
			node->connectionItem = this;
			NodeGraphicsReader imageReader;		
			imageReader.readXml(node,appDir + QString("/OtherItems/simplecircle.xml"));
			if (node->isValid())
			{
				node->normalize();
				node->scale(25.0/node->sceneBoundingRect().height(),25.0/node->sceneBoundingRect().height());
			}
			centerRegionItem = node;
		}
		centerRegion = copy.centerRegion;
		centerRegionVisible = copy.centerRegionVisible;

		itemHandle = copy.itemHandle;

		if (itemHandle)
			setHandle(this,itemHandle);

		pathVectors = copy.pathVectors;
		lineType = copy.lineType;
		arrowHeadDistance = copy.arrowHeadDistance;
		setPen(defaultPen = copy.defaultPen);
		setPos(copy.scenePos());
		setTransform(copy.sceneTransform());

		for (int i=0; i < copy.pathVectors.size(); ++i)
		{
			if (copy.pathVectors[i].arrowStart)
			{
				pathVectors[i].arrowStart = new ArrowHeadItem(*copy.pathVectors[i].arrowStart);
				pathVectors[i].arrowStart->connectionItem = this;
			}
			if (copy.pathVectors[i].arrowEnd)
			{
				pathVectors[i].arrowEnd = new ArrowHeadItem(*copy.pathVectors[i].arrowEnd);
				pathVectors[i].arrowEnd->connectionItem = this;
			}
		}

		QList<ConnectionGraphicsItem::ControlPoint*> uniquePoints;
		QList<QPoint> locations, positions;

		for (int i=0; i < copy.pathVectors.size(); ++i)
			for (int j=0; j < copy.pathVectors[i].size(); ++j)
			{
				positions += QPoint(i,j);

				int k = uniquePoints.indexOf(copy.pathVectors[i][j]);
				if (k > -1)
				{
					locations += locations[k];
					uniquePoints += 0;
				}
				else
				{
					locations += QPoint(i,j);
					uniquePoints += (copy.pathVectors[i][j]);
				}
			}

			NodeGraphicsItem * parentNode = 0;
			for (int k=0; k < uniquePoints.size() && k < locations.size() && k < positions.size(); ++k)
			{
				int i = positions[k].x(),
					j = positions[k].y();
				if (copy.pathVectors[i][j])
				{
					if (uniquePoints[k])
					{
						pathVectors[i][j] = new ConnectionGraphicsItem::ControlPoint(*copy.pathVectors[i][j]);
						pathVectors[i][j]->connectionItem = this;
						pathVectors[i][j]->setVisible(false);

						if (copy.pathVectors[i][j]->parentItem())
						{
							parentNode = qgraphicsitem_cast<NodeGraphicsItem*>(copy.pathVectors[i][j]->parentItem());
							if (parentNode == copy.pathVectors[i].arrowEnd)
								pathVectors[i][j]->setParentItem(pathVectors[i].arrowEnd);
							else
								if (parentNode == copy.pathVectors[i].arrowStart)
									pathVectors[i][j]->setParentItem(pathVectors[i].arrowStart);
								else
									pathVectors[i][j]->setParentItem(copy.pathVectors[i][j]->parentItem());

						}
					}
					else
					{
						pathVectors[i][j] = pathVectors[locations[k].x()][locations[k].y()];
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
		centerRegionVisible = copy.centerRegionVisible;

		//itemHandle = copy.itemHandle;
		setPen(defaultPen = copy.defaultPen);

		for (int i=0; i < copy.pathVectors.size() && i < pathVectors.size(); ++i)
			for (int j=0; j < copy.pathVectors[i].size() && j < pathVectors[i].size(); ++j)
				if (copy.pathVectors[i][j] && pathVectors[i][j] && pathVectors[i][j]->parentItem() == 0)
					pathVectors[i][j]->setPos(copy.pathVectors[i][j]->scenePos());

		return *this;
	}

	/*! Destructor: deletes all shapes and control points */
	ConnectionGraphicsItem::~ConnectionGraphicsItem()
	{
		clear(true);
		if (!itemHandle) return;

		ItemHandle * h = itemHandle;

		setHandle(this,0);

		if (h->graphicsItems.isEmpty() && h->textItems.isEmpty())
			delete h;
	}

	/*! \brief checks that this is a valid drawable*/
	bool ConnectionGraphicsItem::isValid()
	{
		for (int i=0; i < pathVectors.size(); ++i)
			if (pathVectors[i].size() < 4) 
				return false;
		return (pathVectors.size() > 0);
	}

	/*! \brief paint method. Call's parent's paint after setting antialiasing to true*/
	void ConnectionGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
	{
		painter->setClipRect( option->exposedRect );

		//refresh();

		painter->setPen(QPen(QColor(255,255,255,255),pen().width()+4));
		painter->drawPath(path());//this->pathShape);

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
			for (int i=0; i < connectionItem->pathVectors.size(); ++i)
				for (int j=0; j < connectionItem->pathVectors[i].size(); ++j)
					if (connectionItem->pathVectors[i][j] == this)
						connectionItem->pathVectors[i][j] = 0;
		}
	}

	/*! \brief side effect when moved. always call this after moving*/
	/*void ConnectionGraphicsItem::ControlPoint::sideEffect()
	{
	if (connectionItem && connectionItem->lineType == ConnectionGraphicsItem::bezier)
	{
	bool endpt = false, midpt = false;
	for (int i=0; i < connectionItem->pathVectors.size(); ++i)
	{
	int j = connectionItem->pathVectors[i].size() - 1;
	if (j > 1 && connectionItem->pathVectors[i][j-1] == this)
	{
	endpt = true;
	break;
	}
	if (j > 1 && connectionItem->pathVectors[i][j] == this)
	{
	midpt = true;
	}				
	}
	if (endpt)
	{
	for (int i=0; i < connectionItem->pathVectors.size(); ++i)
	{
	int j = connectionItem->pathVectors[i].size() - 1;
	if (j > 1 && connectionItem->pathVectors[i][j-1] != this &&
	connectionItem->pathVectors[i][j-1] &&
	connectionItem->pathVectors[i][j])
	{
	QPointF p0 = connectionItem->pathVectors[i][j]->scenePos(),
	p1 = scenePos();
	QPointF diff = p0 - p1;
	QPointF p2(p0.x() + diff.x(),p0.y() + diff.y());
	connectionItem->pathVectors[i][j-1]->setPos(p2);
	}
	}
	}
	else
	if (midpt)
	{
	ControlPoint * first = 0;
	for (int i=0; i < connectionItem->pathVectors.size(); ++i)
	{
	int j = connectionItem->pathVectors[i].size() - 1;
	if (j > 1 && connectionItem->pathVectors[i][j-1] &&
	connectionItem->pathVectors[i][j-1] != first)
	{
	if (first == 0)
	{
	first = connectionItem->pathVectors[i][j-1];
	}
	else
	{							
	QPointF p0 = scenePos(),
	p1 = first->scenePos();
	QPointF diff = p0 - p1;
	QPointF p2(p0.x() + diff.x(),p0.y() + diff.y());
	connectionItem->pathVectors[i][j-1]->setPos(p2);
	break;
	}
	}
	}
	}
	}
	}*/

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
		for (int i=0; i < pathVectors.size(); ++i)
		{
			if (pathVectors[i].last() && (qgraphicsitem_cast<NodeGraphicsItem*>(pathVectors[i].last()->parentItem()) == 0)
				&& pathVectors.size() > 1 && pathVectors[i].arrowEnd == 0)
				return pathVectors[i].last();
		}
		if (pathVectors[0].size() >= 4)
		{
			int i = pathVectors[0].size()/2;
			if (lineType == line || pathVectors[0].size() > 4)
				i = 3 * (int)(i/3);
			return pathVectors[0][i];
		}
		return 0;
	}

	/*! \brief the center location*/
	QPointF ConnectionGraphicsItem::centerLocation() const
	{
		if (pathVectors.size() == 1 && pathVectors[0].size() < 5)
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
		for (int i=0; i < pathVectors.size(); ++i)
		{
			firstPoint = pathVectors[i].first();
			lastPoint = pathVectors[i].last();

			if (pathVectors[i].size() < 4 || !lastPoint || !firstPoint) continue;

			if (firstPoint->parentItem() != 0 &&  
				((lastPoint->parentItem() != 0 && 
				lastPoint->parentItem() != pathVectors[i].arrowEnd)
				|| (lastPoint->parentItem() == 0)))
				++truePaths;

			if (lineType == bezier)
				cp0 = pathVectors[i][ 1 ];
			else
				cp0 = pathVectors[i][ 3 ];

			if (lineType == bezier)
				cp1 = pathVectors[i][ pathVectors[i].size()-2 ];
			else
				cp1 = pathVectors[i][ pathVectors[i].size()-4 ];

			//adjust firstPoint so that it lies on the boundary rect of the item it points to
			NodeGraphicsItem * node = 0;
			if (firstPoint != 0 && cp0 != 0 && (node = NodeGraphicsItem::topLevelNodeItem(firstPoint->parentItem())) != 0)
			{
				QRectF parentRect = node->sceneBoundingRect();
				//QPainterPath parentShape = node->mapToScene(node->shape());

				if (firstPoint->isVisible())
					firstPoint->setVisible(false);
				if (pathVectors[i].arrowStart != 0)
				{
					QPointF p = pointOnEdge(*node, cp0->scenePos(), arrowHeadDistance + 2.0, lineType == line);

					firstPoint->setPos( node->mapFromScene(p) );

					if (pathVectors[i].arrowStart->scene() != scene() && scene())
					{
						scene()->addItem(pathVectors[i].arrowStart);
					}

					pathVectors[i].arrowStart->setZValue(zValue() + 0.1);

					if (pathVectors[i].arrowStart->itemHandle != 0)  //arrow should not have handles
						setHandle(pathVectors[i].arrowStart,0);

					if (pathVectors[i].arrowStart->parentItem() == 0)
					{
						QPointF p2 = pointOnEdge( *node, cp0->scenePos(),arrowHeadDistance, lineType == line);
						pathVectors[i].arrowStart->setPos(p2);
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


					if (pathVectors[i].arrowStart->angle != angle)
					{
						pathVectors[i].arrowStart->rotate(angle - pathVectors[i].arrowStart->angle);
						pathVectors[i].arrowStart->angle = angle;
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
			if (cp1 != 0 && centerPoint && lastPoint != centerPoint && pathVectors[i].arrowEnd != 0
				&& (lastPoint->parentItem() == 0 || lastPoint->parentItem() == pathVectors[i].arrowEnd)
				&& pathVectors.size() > 1)
			{
				QPointF centerPoint = this->centerPoint()->scenePos();
				QRectF centerRect(centerPoint - QPointF(centerRegion.width(),centerRegion.height())/2.0, centerRegion);
				QPointF p = pointOnEdge(centerRect, cp1->scenePos(), arrowHeadDistance + 2.0,lineType == line);
				QPointF p2 = cp1->scenePos();
				if (lastPoint->parentItem() != pathVectors[i].arrowEnd)
					lastPoint->setParentItem(pathVectors[i].arrowEnd);

				pathVectors[i].arrowEnd->setPos(p);
				lastPoint->setPos(pathVectors[i].arrowEnd->mapFromScene(p) );

				if (pathVectors[i].arrowEnd->scene() != scene() && scene())
				{
					scene()->addItem(pathVectors[i].arrowEnd);
				}

				pathVectors[i].arrowEnd->setZValue(zValue() + 0.1);

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

				if (pathVectors[i].arrowEnd->angle != angle)
				{
					pathVectors[i].arrowEnd->rotate(angle - pathVectors[i].arrowEnd->angle);
					pathVectors[i].arrowEnd->angle = angle;
				}
			}
		}

		if (truePaths == 1 && pathVectors[0].size() >= 4)
		{
			firstPoint = pathVectors[0].last();
			lastPoint = pathVectors[0].first();

			if (lineType == bezier)
				cp0 = pathVectors[0][ pathVectors[0].size()-2 ];
			else
				cp0 = pathVectors[0][ pathVectors[0].size()-4 ];

			NodeGraphicsItem * node = NodeGraphicsItem::topLevelNodeItem(firstPoint->parentItem());
			NodeGraphicsItem * node2 = NodeGraphicsItem::topLevelNodeItem(lastPoint->parentItem());
			if (firstPoint != 0 && cp0 != 0 && node && node2)
			{
				QRectF parentRect1 = node->sceneBoundingRect(),
					parentRect2 = node2->sceneBoundingRect();
				//QPainterPath parentShape = node->mapToScene(node->shape());

				if (firstPoint->isVisible())
					firstPoint->setVisible(false);

				QPointF p0 = cp0->scenePos();

				if (pathVectors[0].arrowEnd != 0)
				{
					QPointF p = pointOnEdge(*node, cp0->scenePos(), arrowHeadDistance+2.0 ,lineType == line);

					if (lineType == line && pathVectors[0].size() == 4)
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

					if (pathVectors[0].arrowEnd->scene() != scene() && scene())
					{
						scene()->addItem(pathVectors[0].arrowEnd);
					}

					if (pathVectors[0].arrowEnd->parentItem() == 0)
					{
						QPointF p2 = pointOnEdge(*node, cp0->scenePos(), arrowHeadDistance,lineType == line);
						pathVectors[0].arrowEnd->setPos(p2);
					}

					pathVectors[0].arrowEnd->setZValue(zValue() + 0.1);

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

					if (pathVectors[0].arrowEnd->angle != angle)
					{
						pathVectors[0].arrowEnd->rotate(angle - pathVectors[0].arrowEnd->angle);
						pathVectors[0].arrowEnd->angle = angle;
					}
				}
				else
				{
					QRectF parentRect1 = node->sceneBoundingRect(),
						parentRect2 = node2->sceneBoundingRect();

					QPointF p = pointOnEdge(*node,cp0->scenePos(),arrowHeadDistance/2.0,lineType == line);

					if (lineType == line && pathVectors[0].size() == 4)
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
		if (!centerRegionItem)
		{
			QString appDir = QCoreApplication::applicationDirPath();
			ArrowHeadItem * node = new ArrowHeadItem;	
			node->connectionItem = this;
			NodeGraphicsReader imageReader;		
			imageReader.readXml(node,appDir + QString("/OtherItems/simplecircle.xml"));
			if (node->isValid())
			{
				node->normalize();
				node->scale(25.0/node->sceneBoundingRect().height(),25.0/node->sceneBoundingRect().height());
			}
			centerRegionItem = node;
		}
		if (centerRegionItem->parentItem() == 0)
		{
			centerRegionItem->connectionItem = this;
			/*if (centerRegionItem->scene() != scene() && scene())
			{
			if (centerRegionItem->scene())
			centerRegionItem->scene()->removeItem(centerRegionItem);
			if (scene())
			{
			scene()->addItem(centerRegionItem);
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
						this->scene()->addItem(centerRegionItem);
						break;
					}
			}
			centerRegionItem->setPos( centerLocation() );
			centerRegionItem->setZValue(z + 0.01);

			QList<ConnectionGraphicsItem*> otherConnections = centerRegionItem->connections();
			for (int i=0; i < otherConnections.size(); ++i)
				if (otherConnections[i] && otherConnections[i] != this)
					otherConnections[i]->refresh();

			//}
			/*
			ControlPoint * cp1 = 0, * cp2 = 0;
			for (int i=0; i < pathVectors.size(); ++i)
			{
			if (pathVectors[i].last() == cp && pathVectors[i].size() > 1)
			{
			if (cp1 == 0)
			cp1 = pathVectors[i][ pathVectors[i].size()-1 ];
			else
			if (cp2 == 0 && cp1 != pathVectors[i][ pathVectors[i].size()-1 ])
			{
			cp2 = pathVectors[i][ pathVectors[i].size()-1 ];
			break;
			}
			}
			}

			if (cp1 && cp2)
			{
			qreal angle;
			if (cp2->x() == cp1->x())
			if (cp1->y() < cp2->y())
			angle = 90.0;
			else
			angle = -90.0;
			else
			angle = atan((cp1->y()-cp2->y())/(cp1->x()-cp2->x())) * 180.0/3.14159;

			if (cp1->x() > cp2->x()) 
			if (cp1->y() < cp2->y())
			angle += 180.0;
			else
			angle -= 180.0;
			if (centerRegionItem->angle != angle)
			{
			centerRegionItem->rotate(angle - centerRegionItem->angle);
			centerRegionItem->angle = angle;
			}
			}*/
		}

		QPainterPath path;

		if (lineType == line)
		{
			QPointF pos;
			for (int i=0; i < pathVectors.size(); ++i)
			{
				/*if (pathVectors[i].arrowStart)
				{	
				if (pathVectors[i].arrowStart->scene() != this->scene() && this->scene())
				this->scene()->addItem(pathVectors[i].arrowStart);
				pathVectors[i].arrowStart->setZValue(z + 0.1);
				}
				if (pathVectors[i].arrowEnd)
				{
				if (pathVectors[i].arrowEnd->scene() != this->scene() && this->scene())
				this->scene()->addItem(pathVectors[i].arrowEnd);
				pathVectors[i].arrowEnd->setZValue(z + 0.1);
				}*/

				NodeGraphicsItem * node = nodeAt(i);
				if (pathVectors[i].size() > 0 && pathVectors[i][0] &&					
					node && node->scene() == scene())
				{
					pathVectors[i][0]->setZValue(z + 0.02);
					pos = pathVectors[i][0]->scenePos();
					path.moveTo(pos);
					for (int j=0; j+3 < pathVectors[i].size(); j+=3)
						if (pathVectors[i][j]) 
						{
							pathVectors[i][j+1]->setZValue(z + 0.02);
							pathVectors[i][j+2]->setZValue(z + 0.02);
							pathVectors[i][j+3]->setZValue(z + 0.02);
							pos = pathVectors[i][j+3]->scenePos();
							path.lineTo(pos);
						}
				}
			}
		}
		else
		{
			QPointF pos1,pos2,pos3;
			for (int i=0; i < pathVectors.size(); ++i)
			{
				/*if (pathVectors[i].arrowStart)
				{	
				if (pathVectors[i].arrowStart->scene() != this->scene() && this->scene())
				this->scene()->addItem(pathVectors[i].arrowStart);
				pathVectors[i].arrowStart->setZValue(z + 0.1);
				}
				if (pathVectors[i].arrowEnd)
				{
				if (pathVectors[i].arrowEnd->scene() != this->scene() && this->scene())
				this->scene()->addItem(pathVectors[i].arrowEnd);
				pathVectors[i].arrowEnd->setZValue(z + 0.1);
				}*/

				NodeGraphicsItem * node = nodeAt(i);
				if (pathVectors[i].size() > 0 && pathVectors[i][0] &&					
					node && node->scene() == scene())
				{
					pathVectors[i][0]->setZValue(z + 0.02);

					pos1 =  pathVectors[i][0]->scenePos();
					pathVectors[i][0]->setZValue(z + 0.02);
					path.moveTo(pos1);					
					for (int j=0; j+3 < pathVectors[i].size(); j+=3)
						if (pathVectors[i][j+1] && pathVectors[i][j+2] && pathVectors[i][j+3])
						{
							pathVectors[i][j+1]->setZValue(z + 0.02);
							pathVectors[i][j+2]->setZValue(z + 0.02);
							pathVectors[i][j+3]->setZValue(z + 0.02);

							pos1 =  pathVectors[i][j+1]->scenePos();
							pos2 =  pathVectors[i][j+2]->scenePos();
							pos3 =  pathVectors[i][j+3]->scenePos();

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
		for (int i=0; i < pathVectors.size(); ++i)
		{	
			for (int j=0; j < pathVectors[i].size(); ++j)
			{
				if (pathVectors[i][j])
					if (j == 0)
						boundary.moveTo(pathVectors[i][j]->scenePos());
					else
						boundary.lineTo(pathVectors[i][j]->scenePos());
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
		for (int i=0; i < pathVectors.size(); ++i)
			for (int j=0; j < pathVectors[i].size(); ++j)
				if (pathVectors[i][j] && !visited.contains(pathVectors[i][j]))
				{
					visited += pathVectors[i][j];

					pathVectors[i][j]->setParentItem(0);

					if (pathVectors[i][j]->scene())
					{
						pathVectors[i][j]->scene()->removeItem(pathVectors[i][j]);
					}

					pathVectors[i][j]->connectionItem = 0;
					delete pathVectors[i][j];

					pathVectors[i][j] = 0;

					if (all)
					{
						if (pathVectors[i].arrowStart && pathVectors[i].arrowStart->scene())
						{
							pathVectors[i].arrowStart->scene()->removeItem(pathVectors[i].arrowStart);
						}

						if (pathVectors[i].arrowEnd && pathVectors[i].arrowEnd->scene())
						{
							pathVectors[i].arrowEnd->scene()->removeItem(pathVectors[i].arrowEnd);
						}
						if (pathVectors[i].arrowStart)
							delete pathVectors[i].arrowStart;
						if (pathVectors[i].arrowEnd)
							delete pathVectors[i].arrowEnd;
						pathVectors[i].arrowStart = 0;
						pathVectors[i].arrowEnd = 0;
					}
				}
				pathVectors.clear();	

				if (centerRegionItem && all)
				{
					centerRegionItem->setParentItem(0);
					centerRegionItem->connectionItem = 0;

					if (centerRegionItem->scene())
						centerRegionItem->scene()->removeItem(centerRegionItem);

					delete centerRegionItem;
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

		for (int i=0; i < pathVectors.size(); ++i)
			for (int j=0; j < pathVectors[i].size(); ++j)
			{
				if (pathVectors[i][j]) 
				{
					if (lineType == bezier || (j%3)==0)
					{
						pathVectors[i][j]->setVisible(visible);
						if (visible && scene() != pathVectors[i][j]->scene())
						{
							if (pathVectors[i][j]->scene())
							{
								pathVectors[i][j]->setParentItem(0);
								pathVectors[i][j]->scene()->removeItem(pathVectors[i][j]);
							}
							if (scene())
								scene()->addItem(pathVectors[i][j]);
						}
					}
					else
					{
						pathVectors[i][j]->setVisible(false);
					}
				}
			}
			refresh();
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
		for (int i=0; i < pathVectors.size(); ++i)
			for (int j=0; j < pathVectors[i].size(); ++j)
			{
				if (pathVectors[i][j] && !list.contains(pathVectors[i][j]) &&
					(pathVectors[i][j]->parentItem() == 0 || includeEnds)
					)
					list.append(pathVectors[i][j]);
			}
			return list;
	}
	/*! \brief list of pointers to all the control points*/
	QList<QGraphicsItem*> ConnectionGraphicsItem::controlPointsAsGraphicsItems(bool includeEnds) const
	{
		QList<QGraphicsItem*> list;
		for (int i=0; i < pathVectors.size(); ++i)
			for (int j=0; j < pathVectors[i].size(); ++j)
			{
				if (pathVectors[i][j] && !list.contains(pathVectors[i][j]) &&
					(pathVectors[i][j]->parentItem() == 0 || includeEnds)
					)
					list.append(pathVectors[i][j]);
			}
			return list;
	}

	AddControlPointCommand::AddControlPointCommand(
		const QString& name, QGraphicsScene * scene,
		ConnectionGraphicsItem::ControlPoint* item)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems.clear();
		graphicsItems += item;	
	}

	AddControlPointCommand::AddControlPointCommand(
		const QString& name, QGraphicsScene * scene,
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

				for (int i=0; i < item->pathVectors.size(); ++i)
					for (int j=0; j < item->pathVectors[i].size(); ++j)
					{
						if (item->pathVectors[i][j])
						{
							p0 = item->pathVectors[i][j]->scenePos();

							if (j > 0 && j < (item->pathVectors[i].size()-1))
								p0 =  (item->pathVectors[i][j+1]->scenePos() + item->pathVectors[i][j-1]->scenePos() + p0)/3.0;
							else
							{
								if (j > 0)
									p0 =  (item->pathVectors[i][j-1]->scenePos() + p0)/2.0;
								else
									if (j < (item->pathVectors[i].size()-1))
										p0 =  (item->pathVectors[i][j+1]->scenePos() + p0)/2.0;
							}

							qreal dist2 = (p0.x() - loc.x())*(p0.x() - loc.x()) + 
								(p0.y() - loc.y())*(p0.y() - loc.y());
							if (dist < 0 || (dist > dist2))
							{
								dist = dist2;
								k1 = i; 
								if ((j+1) < item->pathVectors[i].size())
									k2 = j;
								else
									k2 = j - 1;
							}
						}

						if (item->lineType == ConnectionGraphicsItem::line)
							j += 2;
					}

					//controlPoint->graphicsItem = item;
					while (item->pathVectors.size() <= k1) item->pathVectors.append(ConnectionGraphicsItem::PathVector());

					if (item->pathVectors[k1].size() < k2) k2 = item->pathVectors[k1].size();

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

						//if (item->pathVectors[k1].size() <= k2)
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

						if (item->pathVectors[k1].size() < 4)
						{
							ConnectionGraphicsItem::ControlPoint * cp3 = new ConnectionGraphicsItem::ControlPoint(item);
							cp3->setPos( QPointF(controlPoint->pos().x(), controlPoint->pos().y()) );
							graphicsScene->addItem(cp3);					
							cp3->setZValue(controlPoint->zValue());

							item->pathVectors[k1].insert(0,cp1);
							item->pathVectors[k1].insert(0,cp2);
							item->pathVectors[k1].insert(0,cp3);
							item->pathVectors[k1].insert(0,controlPoint);

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
								item->pathVectors[k1].insert(k2+1,controlPoint);
								item->pathVectors[k1].insert(k2+1,cp2);
								item->pathVectors[k1].insert(k2+1,cp1);
								graphicsItems << controlPoint << cp2 << cp1;
							}
							else
								if (k2 % 3 == 1)
								{
									item->pathVectors[k1].insert(k2+1,cp2);
									item->pathVectors[k1].insert(k2+1,controlPoint);						
									item->pathVectors[k1].insert(k2+1,cp1);
									graphicsItems << cp2 << controlPoint << cp1;
								}
								else
								{
									item->pathVectors[k1].insert(k2+1,cp2);
									item->pathVectors[k1].insert(k2+1,cp1);
									item->pathVectors[k1].insert(k2+1,controlPoint);						
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
				if (listK1[i] >= 0 && listK1[i] < item->pathVectors.size())
				{
					int k = item->pathVectors[ listK1[i] ].indexOf(graphicsItems[i]);
					item->pathVectors[ listK1[i] ].remove(k);
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
		const QString& name, QGraphicsScene * scene,
		ConnectionGraphicsItem::ControlPoint* item)
		: QUndoCommand(name)
	{
		graphicsScene = scene;
		graphicsItems.clear();
		graphicsItems += item;	
	}

	RemoveControlPointCommand::RemoveControlPointCommand(
		const QString& name, QGraphicsScene * scene, 
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
				if (listK1[i] >= 0 && listK1[i] < item->pathVectors.size() && listK2[i]  >= 0 && listK2[i] < item->pathVectors[ listK1[i] ].size() )
				{
					item->pathVectors[ listK1[i] ].insert(listK2[i],graphicsItems[i]);
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

				for (int i=0; i < item->pathVectors.size(); ++i)
				{			
					int index = -1;
					for (int j=3; j < item->pathVectors[i].size()-3; j+=3)
					{
						if (item->pathVectors[i][j] == controlPoint ||
							item->pathVectors[i][j-1] == controlPoint ||
							item->pathVectors[i][j+1] == controlPoint)
						{

							index = j;
							break;
						}
					}	
					if (index > -1)
					{
						graphicsItems << item->pathVectors[i][index-1]
						<< item->pathVectors[i][index]
						<< item->pathVectors[i][index+1];

						listK1 << i << i << i;
						listK2 << (index-1) << index << (index+1);

						item->pathVectors[i][index-1]->setParentItem(0);
						item->pathVectors[i][index]->setParentItem(0);
						item->pathVectors[i][index+1]->setParentItem(0);
						graphicsScene->removeItem(item->pathVectors[i][index-1]);
						graphicsScene->removeItem(item->pathVectors[i][index]);
						graphicsScene->removeItem(item->pathVectors[i][index+1]);

						item->pathVectors[i].remove(index-1,3);
						//item->refresh();

						break;
					}
				}
				item->refresh();
			}
	}

	RemovePathVectorCommand::RemovePathVectorCommand(
		const QString& name, QGraphicsScene * scene,
		ConnectionGraphicsItem::ControlPoint* item)
		: QUndoCommand(name)
	{
		pathVectors.clear();
		graphicsScene = scene;
		connectionItem = item->connectionItem;
		if (connectionItem == 0 || connectionItem->pathVectors.size() < 2) return;

		QList<ArrowHeadItem*> arrowHeads = connectionItem->arrowHeads();
		int only_reactant = -1, only_product = -1;		
		for (int i=0; i < arrowHeads.size(); ++i)
		{
			if (arrowHeads[i] == 0)
			{
				if (only_reactant == -1)
					only_reactant = i;
				else
				{
					only_reactant = -1;
					break;
				}
			}
		}
		for (int i=0; i < arrowHeads.size(); ++i)
		{
			if (arrowHeads[i] != 0)
			{
				if (only_product == -1)
					only_product = i;
				else
				{
					only_product = -1;
					break;
				}
			}
		}

		for (int i=0; i < connectionItem->pathVectors.size(); ++i)
		{
			for (int j=0; j < connectionItem->pathVectors[i].size(); ++j)
			{
				if (connectionItem->pathVectors[i][j] != 0 && connectionItem->pathVectors[i][j] == item)
				{
					if (i != only_reactant && i != only_product)					
						pathVectors.append(connectionItem->pathVectors[i]);
					break;
				}
			}
		}
	}

	RemovePathVectorCommand::RemovePathVectorCommand(
		const QString& name, QGraphicsScene * scene, 
		ConnectionGraphicsItem* connection,
		QList<ConnectionGraphicsItem::ControlPoint*> items)
		: QUndoCommand(name)
	{
		pathVectors.clear();
		graphicsScene = scene;
		connectionItem = connection;
		if (connectionItem == 0 || connectionItem->pathVectors.size() < 2) return;

		bool done = false;
		for (int i=0; i < connectionItem->pathVectors.size(); ++i)
		{
			done = false;
			for (int j=0; j < connectionItem->pathVectors[i].size(); ++j)
			{
				for (int k = 0; k < items.size(); ++k)
				{
					if (connectionItem->pathVectors[i][j] != 0 && connectionItem->pathVectors[i][j] == items[k])
					{
						pathVectors.append(connectionItem->pathVectors[i]);
						done = true;
						break;
					}
				}
				if (done) break;
			}
		}

	}

	void RemovePathVectorCommand::undo()
	{
		if (connectionItem == 0) return;

		if (connectionItem->pathVectors.size() == 1 && connectionItem->pathVectors.size() >= 4 
			&& pathVectors.size() > 0 && pathVectors[0].at(3) != 0
			&& connectionItem->pathVectors[0][3]->scene() != 0)
		{
			connectionItem->pathVectors[0][3]->setParentItem(0);
			connectionItem->pathVectors[0][3]->scene()->removeItem(connectionItem->pathVectors[0][3]);
			connectionItem->pathVectors[0][3] = pathVectors[0].at(3);
		}
		for (int i=0; i < pathVectors.size(); ++i)
		{
			connectionItem->pathVectors.append(pathVectors[i]);
			for (int j=0; j < pathVectors[i].size(); ++j)
				if (pathVectors[i][j] != 0)
					pathVectors[i][j]->setVisible( connectionItem->controlPointsVisible );
			if (pathVectors[i].arrowStart)
				pathVectors[i].arrowStart->setVisible(true);
			if (pathVectors[i].arrowEnd)
				pathVectors[i].arrowEnd->setVisible(true);
		}
		connectionItem->refresh();
	}

	void RemovePathVectorCommand::redo()
	{
		if (connectionItem == 0) return;
		int k;
		for (int i=0; i < pathVectors.size(); ++i)
		{
			k = connectionItem->pathVectors.indexOf(pathVectors[i]);
			if (k >=0 && k < connectionItem->pathVectors.size())
				connectionItem->pathVectors.removeAt(k);
			for (int j=0; j < pathVectors[i].size(); ++j)
				if (pathVectors[i][j] != 0)
					pathVectors[i][j]->setVisible(false);
			if (pathVectors[i].arrowStart)
				pathVectors[i].arrowStart->setVisible(false);
			if (pathVectors[i].arrowEnd)
				pathVectors[i].arrowEnd->setVisible(false);
		}
		if (connectionItem->pathVectors.size() == 1 && connectionItem->pathVectors.size() >= 4 
			&& pathVectors.size() > 0 && pathVectors[0].at(0) != 0)
		{
			connectionItem->pathVectors[0][3] = new ConnectionGraphicsItem::ControlPoint(*connectionItem->pathVectors[0][3]);
			connectionItem->pathVectors[0][3]->setParentItem(pathVectors[0].at(0)->parentItem());
		}	
		connectionItem->refresh();
	}

	AddPathVectorCommand::AddPathVectorCommand(
		const QString& name, QGraphicsScene * scene,
		ConnectionGraphicsItem* connection,
		ConnectionGraphicsItem::PathVector& item)
		: QUndoCommand(name)
	{
		pathVectors.clear();
		graphicsScene = scene;
		connectionItem = connection;
		if (connectionItem == 0) return;
		pathVectors.append(item);
	}

	AddPathVectorCommand::AddPathVectorCommand(
		const QString& name, QGraphicsScene * scene, 
		ConnectionGraphicsItem* connection,
		QList<ConnectionGraphicsItem::PathVector> items)
		: QUndoCommand(name)
	{
		pathVectors.clear();
		graphicsScene = scene;
		connectionItem = connection;
		if (connectionItem == 0) return;
		pathVectors << items;

	}

	void AddPathVectorCommand::redo()
	{
		if (connectionItem == 0) return;

		for (int i=0; i < pathVectors.size(); ++i)
		{
			connectionItem->pathVectors.append(pathVectors[i]);

			if (pathVectors[i].arrowStart)
				pathVectors[i].arrowStart->setVisible(true);
			if (pathVectors[i].arrowEnd)
				pathVectors[i].arrowEnd->setVisible(true);
		}
		connectionItem->refresh();
	}

	void AddPathVectorCommand::undo()
	{
		if (connectionItem == 0) return;
		int k;
		for (int i=0; i < pathVectors.size(); ++i)
		{
			k = connectionItem->pathVectors.indexOf(pathVectors[i]);
			if (k >=0 && k < connectionItem->pathVectors.size())
				connectionItem->pathVectors.removeAt(k);

			if (pathVectors[i].arrowStart)
				pathVectors[i].arrowStart->setVisible(false);
			if (pathVectors[i].arrowEnd)
				pathVectors[i].arrowEnd->setVisible(false);
		}
		connectionItem->refresh();
	}

	AddPathVectorCommand::~AddPathVectorCommand()
	{
		for (int i=0; i < pathVectors.size(); ++i)
		{
			for (int j=0; j < pathVectors[i].size(); ++j)
				if (pathVectors[i][j])
				{
					pathVectors[i][j]->setParentItem(0);
					if (pathVectors[i][j] && !pathVectors[i][j]->connectionItem && !pathVectors[i][j]->scene())
					{
						delete pathVectors[i][j];
					}
				}
				if (pathVectors[i].arrowStart && !pathVectors[i].arrowStart->scene())
				{
					pathVectors[i].arrowStart->setParentItem(0);
					delete pathVectors[i].arrowStart;
				}
				if (pathVectors[i].arrowEnd && !pathVectors[i].arrowEnd->scene())
				{
					pathVectors[i].arrowEnd->setParentItem(0);
					delete pathVectors[i].arrowEnd;
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

	/*! \brief find all the nodes that are connected
	* \return node item list or 0*/
	QList<NodeGraphicsItem*> ConnectionGraphicsItem::nodes() const
	{
		QList<NodeGraphicsItem*> nodes;
		NodeGraphicsItem* node;

		for (int i=0; i < pathVectors.size(); ++i)
			if (pathVectors[i].size() > 0 && pathVectors[i][0])
			{
				node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][0]->parentItem());
				if (node && node->isVisible() && node != pathVectors[i].arrowStart && node != pathVectors[i].arrowEnd)
					nodes += node;
				if (pathVectors[i].size() > 1)
				{
					node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][pathVectors[i].size()-1]->parentItem());
					if (node && node->isVisible() && node != pathVectors[i].arrowStart && node != pathVectors[i].arrowEnd)
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

		for (int i=0; i < pathVectors.size(); ++i)
			if (pathVectors[i].size() > 0 && pathVectors[i][0])
			{
				node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][0]->parentItem());
				if (node && pathVectors[i].arrowStart && node->isVisible() && node != pathVectors[i].arrowStart && node != pathVectors[i].arrowEnd)
					nodes += node;
				if (pathVectors[i].size() > 1)
				{
					node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][pathVectors[i].size()-1]->parentItem());
					if (node && pathVectors[i].arrowEnd && node->isVisible() && node != pathVectors[i].arrowStart && node != pathVectors[i].arrowEnd)
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

		for (int i=0; i < pathVectors.size(); ++i)
			if (pathVectors[i].size() > 0 && pathVectors[i][0])
			{
				node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][0]->parentItem());
				if (node && !pathVectors[i].arrowStart && node->isVisible() && node != pathVectors[i].arrowStart && node != pathVectors[i].arrowEnd)
					nodes += node;
				if (pathVectors[i].size() > 1)
				{
					node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][pathVectors[i].size()-1]->parentItem());
					if (node && !pathVectors[i].arrowEnd && node->isVisible() && node != pathVectors[i].arrowStart && node != pathVectors[i].arrowEnd)
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

		for (int i=0; i < pathVectors.size(); ++i)
			if (pathVectors[i].size() > 1 && pathVectors[i][0] && !pathVectors[i].contains(center))
			{
				node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][0]->parentItem());
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

		for (int i=0; i < pathVectors.size(); ++i)
			if (pathVectors[i].size() > 0 && pathVectors[i][0])
			{
				node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][0]->parentItem());
				if (node && node->isVisible() && node != pathVectors[i].arrowStart && node != pathVectors[i].arrowEnd)
					nodes += node;
				if (pathVectors[i].size() > 1)
				{
					node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][pathVectors[i].size()-1]->parentItem());
					if (node && node->isVisible() && node != pathVectors[i].arrowStart && node != pathVectors[i].arrowEnd)
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

		for (int i=0; i < pathVectors.size(); ++i)
			if (pathVectors[i].size() > 0 && pathVectors[i][0])
			{
				node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][0]->parentItem());
				if (node && node->isVisible() && node != pathVectors[i].arrowStart && node != pathVectors[i].arrowEnd)
					arrows += pathVectors[i].arrowStart;
				if (pathVectors[i].size() > 1)
				{
					node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][pathVectors[i].size()-1]->parentItem());
					if (node && node->isVisible() && node != pathVectors[i].arrowStart && node != pathVectors[i].arrowEnd)
						arrows += pathVectors[i].arrowEnd;
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

		for (int i=0; i < pathVectors.size(); ++i)
			if (pathVectors[i].size() > 0 && pathVectors[i][0])
			{
				node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][0]->parentItem());
				if (node && node->isVisible() && node != pathVectors[i].arrowStart && node != pathVectors[i].arrowEnd)
					arrows += pathVectors[i].arrowStart;
				if (pathVectors[i].size() > 1)
				{
					node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][pathVectors[i].size()-1]->parentItem());
					if (node && node->isVisible() && node != pathVectors[i].arrowStart && node != pathVectors[i].arrowEnd)
						arrows += pathVectors[i].arrowEnd;
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

		for (int i=0; i < pathVectors.size(); ++i)
			if (pathVectors[i].size() > 0 && pathVectors[i][0])
			{
				node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][0]->parentItem());
				if (node && node->isVisible() && node == pathVectors[i].arrowStart)
					arrows += pathVectors[i].arrowStart;
				if (pathVectors[i].size() > 1)
				{
					node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][pathVectors[i].size()-1]->parentItem());
					if (node && node->isVisible() && node == pathVectors[i].arrowEnd)
						arrows += pathVectors[i].arrowEnd;
				}
			}

			return arrows;
	}

	/*! \brief find the node that connected to the particular path
	* \param index less than size of pathVectors
	* \return node item or 0*/
	NodeGraphicsItem* ConnectionGraphicsItem::nodeAt(int index) const
	{
		NodeGraphicsItem * node;
		if (pathVectors.size() == 1)
		{
			if (index < 0 || index > 1 || 
				pathVectors[0].size() < 4 || pathVectors[0][pathVectors[0].size()-1] == 0) return 0;

			if (index == 0)
				node = NodeGraphicsItem::topLevelNodeItem(pathVectors[0][0]->parentItem());				
			else
				node = NodeGraphicsItem::topLevelNodeItem(pathVectors[0][pathVectors[0].size()-1]->parentItem());
			if (node == pathVectors[0].arrowStart || node == pathVectors[0].arrowEnd)
				node = 0;				
		}

		if (index < 0 || index >= pathVectors.size() ||
			pathVectors[index].size() < 4 ||
			pathVectors[index][0] == 0) 
			return 0;

		node = NodeGraphicsItem::topLevelNodeItem(pathVectors[index][0]->parentItem());
		if (node == pathVectors[index].arrowStart || node == pathVectors[index].arrowEnd)
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
		NodeGraphicsItem* node;

		for (int i=0; i < pathVectors.size(); ++i)
			if (pathVectors[i].size() > 0 && pathVectors[i][0])
			{
				node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][0]->parentItem());
				if (node && node == target)
					return i;
				if (pathVectors[i].size() > 1)
				{
					node = NodeGraphicsItem::topLevelNodeItem(pathVectors[i][pathVectors[i].size()-1]->parentItem());
					if (node && node == target)
						return i;
				}
			}

			return -1;
	}

	/*! \brief find the arrow head at the particular index
	* \param index less than size of pathVectors
	* \return node item or 0*/
	ArrowHeadItem* ConnectionGraphicsItem::arrowAt(int index) const
	{
		ArrowHeadItem* arrow = 0;
		NodeGraphicsItem * node;
		if (pathVectors.size() == 1)
		{
			if (index < 0 || index > 1 || 
				pathVectors[0].size() < 4 || pathVectors[0][pathVectors[0].size()-1] == 0) return 0;

			if (index == 0)
			{
				node = NodeGraphicsItem::topLevelNodeItem(pathVectors[0][0]->parentItem());
				arrow = pathVectors[0].arrowStart;
			}
			else
			{
				node = NodeGraphicsItem::topLevelNodeItem(pathVectors[0][pathVectors[0].size()-1]->parentItem());
				arrow = pathVectors[0].arrowEnd;
			}
			if (node == pathVectors[0].arrowStart || node == pathVectors[0].arrowEnd)
			{
				node = 0;
				arrow = 0;	
			}
		}

		if (index < 0 || index >= pathVectors.size() ||
			pathVectors[index].size() < 4 ||
			pathVectors[index][0] == 0) 
			return 0;

		node = NodeGraphicsItem::topLevelNodeItem(pathVectors[index][0]->parentItem());
		arrow = pathVectors[index].arrowStart;
		if (node == pathVectors[index].arrowStart || node == pathVectors[index].arrowEnd)
		{
			node = 0;
			arrow = 0;
		}

		if (arrow && arrow->isVisible())
			return arrow;

		return 0;
	}

	/*! \brief find the modifier arrow head at the particular index
	* \param index less than size of pathVectors
	* \return node item or 0*/
	ArrowHeadItem* ConnectionGraphicsItem::modifierArrowAt(int index) const
	{
		if (index < 0 || index >= pathVectors.size())
			return 0;

		if (arrowAt(index) == pathVectors[index].arrowEnd) return 0;

		return pathVectors[index].arrowEnd;
	}

	void ConnectionGraphicsItem::replaceNode(NodeGraphicsItem* oldNode, NodeGraphicsItem* newNode)
	{
		if (oldNode == 0 || newNode == 0) return;

		for (int i=0; i < pathVectors.size(); ++i)
		{
			if (pathVectors[i].size() > 0)
			{
				if (pathVectors[i][0]->parentItem() == oldNode)
					pathVectors[i][0]->setParentItem(newNode);
				if (pathVectors[i][pathVectors[i].size() - 1]->parentItem() == oldNode)
					pathVectors[i][pathVectors[i].size() - 1]->setParentItem(newNode);
			}
		}
	}

	void ConnectionGraphicsItem::replaceNodeAt(int index, NodeGraphicsItem* nodeItem)
	{
		if (nodeItem == 0) return;

		if (pathVectors.size() == 1)
		{
			if (index < 0 || index > 1 || 
				pathVectors[0].size() < 4 || pathVectors[0][0] == 0) return;

			if (index == 0)
				pathVectors[0][0]->setParentItem(nodeItem);
			else
				pathVectors[0][pathVectors[0].size()-1]->setParentItem(nodeItem);

			return;
		}

		if (index < 0 || index >= pathVectors.size() ||
			pathVectors[index].size() < 4 ||
			pathVectors[index][0] == 0) return;

		pathVectors[index][0]->setParentItem(nodeItem);
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

	ConnectionGraphicsItem::PathVector::PathVector() : QVector<ConnectionGraphicsItem::ControlPoint*>() 
	{ 
		arrowStart = arrowEnd = 0;
	}

	ConnectionGraphicsItem::PathVector::PathVector(int n) : QVector<ConnectionGraphicsItem::ControlPoint*>(n) 
	{ 
		arrowStart = arrowEnd = 0;
	}

	ConnectionGraphicsItem::PathVector::PathVector(int n,ConnectionGraphicsItem::ControlPoint* p) 
		: QVector<ConnectionGraphicsItem::ControlPoint*>(n,p) 
	{ 
		arrowStart = arrowEnd = 0;
	}

	ConnectionGraphicsItem::PathVector::PathVector(const ConnectionGraphicsItem::PathVector& copy) : 
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

}
