/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 One of the main classes in Tinkercell.

 There are two classes defined in this file: ControlPoint and NodeGraphicsItem

 The NodeGraphicsItem is a group made up of Shapes. Each Shape is a polygon item.
 Each shape has a default color. The purpose of the default color is to allow plugins
 to change color temporarily and then revert back to the default.

 ControlPoint is a drawable item that is used by NodeGraphicsItem and ConnectionGraphicsItem
 to draw movable points.

****************************************************************************/

#include "GraphicsScene.h"
#include "MainWindow.h"
#include "ConnectionGraphicsItem.h"
#include "NodeGraphicsItem.h"
#include "ItemHandle.h"

namespace Tinkercell
{
	QString NodeGraphicsItem::class_name("NodeGraphicsItem");

	/*! Constructor: does nothing */
	NodeGraphicsItem::NodeGraphicsItem(QGraphicsItem * parent) : QGraphicsItemGroup (parent), itemHandle(0), boundingBoxItem(0)
	{
		setCacheMode(QGraphicsItem::DeviceCoordinateCache);
		setFlag(QGraphicsItem::ItemIsMovable, false);
		setFlag(QGraphicsItem::ItemIsSelectable, false);

		className = NodeGraphicsItem::class_name;
		ControlPoint * topleft = new ControlPoint(this);
		topleft->setPos( boundingRect().topLeft() );
		topleft->setVisible(false);
		topleft->scale(0.75,0.75);
		topleft->setPen(topleft->defaultPen = QPen(QColor(255,100,100),2.0));
		topleft->setBrush(topleft->defaultBrush = QBrush(QColor(255,255,255,255)));

		ControlPoint * bottomright = new ControlPoint(this);
		bottomright->setPos( boundingRect().bottomRight() );
		bottomright->setVisible(false);
		bottomright->scale(0.75,0.75);
		bottomright->setPen(bottomright->defaultPen = QPen(QColor(255,100,100),2.0));
		bottomright->setBrush(bottomright->defaultBrush = QBrush(QColor(255,255,255,255)));

		bottomright->shapeType = topleft->shapeType = ControlPoint::square;
		boundaryControlPoints << topleft << bottomright;

		boundingBoxItem = new QGraphicsRectItem(this);
		qreal w = (1000.0/((sceneBoundingRect().width() + sceneBoundingRect().height())));
		if (w > 300.0) w = 300.0;
		boundingBoxItem->setPen(QPen(QColor(255,100,100),w,Qt::DashLine));
		boundingBoxItem->setBrush(Qt::NoBrush);
		boundingBoxItem->setVisible(false);
		boundingBoxItem->setPos(0,0);
		boundingBoxItem->setRect(boundingRect());
	}

	/*! reset of control points that control the bounding box of this figure */
	void NodeGraphicsItem::adjustBoundaryControlPoints()
	{
		if (boundaryControlPoints.size() == 2
			&& boundaryControlPoints[0]
			&& boundaryControlPoints[1])
			{
				QRectF bounds = sceneBoundingRect();
				QPointF p1 = boundaryControlPoints[0]->scenePos(),
						p2 = boundaryControlPoints[1]->scenePos();
				if ( p1.rx() == 0 || p2.rx() == 0 || p1.ry() == 0 || p2.ry() == 0 ||
					( (p1.rx() - bounds.left())*(p1.rx() - bounds.left()) <
						(p1.rx() - bounds.right())*(p1.rx() - bounds.right()) ) )
				{
					p1.rx() = bounds.left();
					p2.rx() = bounds.right();
				}

				if ( p1.rx() == 0 || p2.rx() == 0 || p1.ry() == 0 || p2.ry() == 0 ||
					 ((p1.ry() - bounds.top())*(p1.ry() - bounds.top()) <
						(p1.ry() - bounds.bottom())*(p1.ry() - bounds.bottom()) ) )
				{
					p1.ry() = bounds.top();
					p2.ry() = bounds.bottom();
				}

				boundaryControlPoints[0]->setPos(p1);
				boundaryControlPoints[1]->setPos(p2);

				boundaryControlPoints[0]->setZValue(zValue() + 0.1);
				boundaryControlPoints[1]->setZValue(zValue() + 0.1);
			}
		if (boundingBoxItem)
		{
			if (boundingBoxItem->parentItem() != this)
				addToGroup(boundingBoxItem);
			boundingBoxItem->setPos(0,0);
			boundingBoxItem->setRect(boundingRect());
		}
	}

	/*! show or hide the bounding box of this figure */
	void NodeGraphicsItem::setBoundingBoxVisible(bool visible, bool controlPoints)
	{
		QGraphicsScene * s = scene();
		adjustBoundaryControlPoints();
		for (int i=0; i < boundaryControlPoints.size(); ++i)
			if (boundaryControlPoints[i])
			{
				if (s && boundaryControlPoints[i]->scene() != s)
					s->addItem(boundaryControlPoints[i]);

				boundaryControlPoints[i]->setVisible(controlPoints && visible);
				//boundaryControlPoints[i]->setZValue(zValue() + 0.1);
			}
		if (boundingBoxItem)
		{
			if (boundingBoxItem->parentItem() != this)
				addToGroup(boundingBoxItem);
			boundingBoxItem->setZValue(zValue() + 0.1);
			boundingBoxItem->setVisible(visible);
			boundingBoxItem->setPos(0,0);
			boundingBoxItem->setRect(boundingRect());
		}
	}

	/*! Copy Constructor: deep copy of all pointers */
	NodeGraphicsItem::NodeGraphicsItem(const NodeGraphicsItem& copy) : QGraphicsItemGroup (0) , itemHandle(0), boundingBoxItem(0)
	{
		setFlag(QGraphicsItem::ItemIsMovable, false);
		setFlag(QGraphicsItem::ItemIsSelectable, false);

		/**copy handle**/
		className = copy.className;
		itemHandle = copy.itemHandle;
		fileName = copy.fileName;
		defaultSize = copy.defaultSize;

		if (itemHandle)
			setHandle(this,itemHandle);

		/**Copy control points and shapes**/
		setPos(copy.scenePos());

		for (int i=0; i < copy.controlPoints.size(); ++i)
			if (copy.controlPoints[i])
				addControlPoint(new NodeGraphicsItem::ControlPoint(*copy.controlPoints[i]));

		for (int i=0; i < copy.shapes.size(); ++i)
			if (copy.shapes[i])
			{
				Shape * newShape = new Shape(*copy.shapes[i]);
				for (int j=0; j < newShape->controlPoints.size(); ++j)
					for (int k=0; k < copy.controlPoints.size(); ++k)
						if (newShape->controlPoints[j] == copy.controlPoints[k])
						{
							newShape->controlPoints[j] = controlPoints[k];
						}
				addShape(newShape);
			}

		refresh();
		QTransform t0 = copy.sceneTransform();
		QTransform t1(t0.m11(),t0.m12(),0,t0.m21(),t0.m22(),0,0,0,1);
		setTransform(t1);

		ControlPoint * topleft = new ControlPoint(this);
		topleft->setPos( boundingRect().topLeft() );
		topleft->setVisible(false);
		topleft->scale(0.75,0.75);
		topleft->setPen(topleft->defaultPen = QPen(QColor(255,100,100),2.0));
		topleft->setBrush(topleft->defaultBrush = QBrush(QColor(255,255,255,255)));

		ControlPoint * bottomright = new ControlPoint(this);
		bottomright->setPos( boundingRect().bottomRight() );
		bottomright->setVisible(false);
		bottomright->scale(0.75,0.75);
		bottomright->setPen(bottomright->defaultPen = QPen(QColor(255,100,100),2.0));
		bottomright->setBrush(bottomright->defaultBrush = QBrush(QColor(255,255,255,255)));

		bottomright->shapeType = topleft->shapeType = ControlPoint::square;
		boundaryControlPoints << topleft << bottomright;

		boundingBoxItem = new QGraphicsRectItem(this);
		qreal w = (1000.0/((sceneBoundingRect().width() + sceneBoundingRect().height())));
		if (w > 300.0) w = 300.0;
		boundingBoxItem->setPen(QPen(QColor(255,100,100),w,Qt::DashLine));
		boundingBoxItem->setBrush(Qt::NoBrush);
		boundingBoxItem->setVisible(false);
		boundingBoxItem->setPos(0,0);
		boundingBoxItem->setRect(boundingRect());
	}

	/*! \brief make a copy of this item*/
	NodeGraphicsItem* NodeGraphicsItem::clone() const
	{
		return new NodeGraphicsItem(*this);
	}

	/*! operator =: deep copy of all pointers */
	NodeGraphicsItem& NodeGraphicsItem::operator = (const NodeGraphicsItem& copy)
	{
		clear();
		fileName = copy.fileName;
		defaultSize = copy.defaultSize;
		/*QList<QGraphicsItem*> children = childItems();
		for (int i=0; i < children.size(); ++i)
			removeFromGroup(children[i]);*/

		/**Copy control points and shapes**/
		QGraphicsItem * parent = parentItem();
		setParentItem(0);
		setBoundingBoxVisible(false);

		resetTransform();
		setPos(copy.scenePos());

		for (int i=0; i < copy.controlPoints.size(); ++i)
			if (copy.controlPoints[i])
				addControlPoint(new NodeGraphicsItem::ControlPoint(*copy.controlPoints[i]));

		for (int i=0; i < copy.shapes.size(); ++i)
			if (copy.shapes[i])
			{
				Shape * newShape = new Shape(*copy.shapes[i]);
				for (int j=0; j < newShape->controlPoints.size(); ++j)
					for (int k=0; k < copy.controlPoints.size(); ++k)
						if (newShape->controlPoints[j] == copy.controlPoints[k])
						{
							newShape->controlPoints[j] = controlPoints[k];
						}
				addShape(newShape);
			}

		refresh();
		QTransform t0 = copy.sceneTransform();
		QTransform t1(t0.m11(),t0.m12(),0,t0.m21(),t0.m22(),0,0,0,1);
		setTransform(t1);

		for (int i=0; i < boundaryControlPoints.size(); ++i)
			if (boundaryControlPoints[i])
			{
				boundaryControlPoints[i]->nodeItem = 0;
				if (!boundaryControlPoints[i]->scene())
					delete boundaryControlPoints[i];
				boundaryControlPoints[i] = 0;
			}
		boundaryControlPoints.clear();

		ControlPoint * topleft = new ControlPoint(this);
		topleft->setPos( boundingRect().topLeft() );
		topleft->setVisible(false);
		topleft->scale(0.75,0.75);
		topleft->setPen(topleft->defaultPen = QPen(QColor(255,100,100),2.0));
		topleft->setBrush(topleft->defaultBrush = QBrush(QColor(255,255,255,255)));

		ControlPoint * bottomright = new ControlPoint(this);
		bottomright->setPos( boundingRect().bottomRight() );
		bottomright->setVisible(false);
		bottomright->scale(0.75,0.75);
		bottomright->setPen(bottomright->defaultPen = QPen(QColor(255,100,100),2.0));
		bottomright->setBrush(bottomright->defaultBrush = QBrush(QColor(255,255,255,255)));

		bottomright->shapeType = topleft->shapeType = ControlPoint::square;
		boundaryControlPoints << topleft << bottomright;

		boundingBoxItem = new QGraphicsRectItem(this);
		qreal w = (1000.0/((sceneBoundingRect().width() + sceneBoundingRect().height())));
		if (w > 300.0) w = 300.0;
		boundingBoxItem->setPen(QPen(QColor(255,100,100),w,Qt::DashLine));
		boundingBoxItem->setBrush(Qt::NoBrush);
		boundingBoxItem->setVisible(false);
		boundingBoxItem->setPos(0,0);
		boundingBoxItem->setRect(boundingRect());

		setParentItem(parent);
		/*QPointF center = sceneBoundingRect().center();
		for (int i=0; i < children.size(); ++i)
			if (children[i])
			{
				children[i]->setPos( center );
				addToGroup(children[i]);
			}*/

		return *this;
	}

	/*! Destructor: deletes all shapes and control points */
	NodeGraphicsItem::~NodeGraphicsItem()
	{
		clear();

		for (int i=0; i < boundaryControlPoints.size(); ++i)
			if (boundaryControlPoints[i])
			{
				boundaryControlPoints[i]->nodeItem = 0;
				if (!boundaryControlPoints[i]->scene())
					delete boundaryControlPoints[i];
				boundaryControlPoints[i] = 0;
			}

		if (boundingBoxItem)
		{
			removeFromGroup(boundingBoxItem);
			if (boundingBoxItem->scene())
				boundingBoxItem->scene()->removeItem(boundingBoxItem);
			delete boundingBoxItem;
			boundingBoxItem = 0;
		}


		if (!itemHandle) return;
		if (itemHandle->graphicsItems.isEmpty())
		{
			delete itemHandle;
			itemHandle = 0;
		}
		else
		{
			itemHandle->graphicsItems.removeAll(this);
			itemHandle = 0;
		}
	}

	/*! \brief checks that this is a valid drawable*/
	bool NodeGraphicsItem::isValid() const
	{
		return controlPoints.size() > 1 && shapes.size() > 0;
	}

	void NodeGraphicsItem::adjustToBoundaryControlPoints()
	{
		if (boundaryControlPoints.size() < 2 ||
			!boundaryControlPoints[0] ||
			!boundaryControlPoints[1])
			return;

		boundaryControlPoints[0]->setZValue(zValue() + 0.1);
		boundaryControlPoints[1]->setZValue(zValue() + 0.1);

		QRectF bounds = sceneBoundingRect();

		QPointF p1 = boundaryControlPoints[0]->scenePos(),
			    p2 = boundaryControlPoints[1]->scenePos();

		if (p1.rx() > p2.rx())
		{
			double x = p1.rx();
			p1.rx() = p2.rx();
			p2.rx() = x;
		}

		if (p1.ry() > p2.ry())
		{
			double y = p1.ry();
			p1.ry() = p2.ry();
			p2.ry() = y;
		}

		if (p1.x() == p2.x()) p2.rx() += 1; //prevent 0 width/height
		if (p1.y() == p2.y()) p2.ry() += 1;

		QRectF rect = QRectF(p1,p2);


		if (rect.center().isNull() || sceneTransform().isRotating()) //the boundary points are not correct
		{
			adjustBoundaryControlPoints();
		}
		else
		if (bounds != rect)
		{
			QPointF p = rect.center() - scenePos();
			moveBy(p.x(),p.y());
			scale(rect.width()/bounds.width(),
				  rect.height()/bounds.height());
			if (boundingBoxItem)
			{
				QPen pen = boundingBoxItem->pen();
				qreal w = 1000.0/(rect.width() + rect.height());
				if (w > 100.0)
					pen.setWidthF(2.0);
				else
					pen.setWidthF(w);
				boundingBoxItem->setPen(pen);
			}
		}
	}

	/*! \brief paint method. Call's parent's paint after setting antialiasing to true*/
	void NodeGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
	{
		if ((boundaryControlPoints.size() == 2)
			&& !sceneTransform().isRotating()
			&& boundaryControlPoints[0] && boundaryControlPoints[0]->isVisible()
			&& boundaryControlPoints[1] && boundaryControlPoints[1]->isVisible() )
		{
			adjustToBoundaryControlPoints();
			//painter->setPen(QPen(boundaryControlPoints[0]->defaultPen.color(),1.0));
			//painter->drawRoundRect(boundingRect());
			//painter->drawRect(boundingRect());
		}
		QGraphicsItemGroup::paint(painter,option,widget);
	}

	/*! \brief Constructor: Setup colors and z value */
	NodeGraphicsItem::ControlPoint::ControlPoint(NodeGraphicsItem * image_ptr, QGraphicsItem * parent) :
		Tinkercell::ControlPoint(parent)
	{
		nodeItem = image_ptr;
		setPen(defaultPen = QPen( QColor(100,100,255) ));
		setBrush(defaultBrush = QBrush( QColor(0,0,255,10)) );
		setRect(QRectF(-10,-10,20,20));
		setZValue(10);
	}

	/*! \brief Constructor: Setup colors and z value */
	ControlPoint::ControlPoint(QGraphicsItem * parent) :
			QAbstractGraphicsShapeItem(parent)
	{
		setFlag(QGraphicsItem::ItemIsMovable, false);
		setFlag(QGraphicsItem::ItemIsSelectable, false);

		setPen(defaultPen = QPen( QColor(100,100,255) ));
		setBrush(defaultBrush = QBrush( QColor(0,0,255,10)) );
		setRect(QRectF(-10,-10,20,20));
		setZValue(10);
		shapeType = this->circle;
	}

	/*! \brief Copy Constructor */
	ControlPoint::ControlPoint(const ControlPoint& copy) : QAbstractGraphicsShapeItem(copy.parentItem())
	{
		setFlag(QGraphicsItem::ItemIsMovable, false);
		setFlag(QGraphicsItem::ItemIsSelectable, false);

		setPos(copy.pos());
		setRect(copy.rect());
		setPen(defaultPen = copy.defaultPen);
		setBrush(defaultBrush = copy.defaultBrush);
		//setTransform(copy.transform());
		shapeType = copy.shapeType;
		bounds = copy.bounds;
		defaultSize = copy.defaultSize;
	}

	/*! \brief bounding rect method. */
	QRectF ControlPoint::boundingRect() const
	{
		qreal w = pen().widthF();
		return bounds.adjusted(-w,-w,w,w);
	}

	/*! \brief bounding rect method. */
	QRectF ControlPoint::rect() const
	{
		return bounds;
	}

	/*! \brief set size. */
	void ControlPoint::setRect(const QRectF& rect)
	{
		this->bounds = rect;
	}

	/*! \brief make a copy of this item*/
	ControlPoint* ControlPoint::clone() const
	{
		return new ControlPoint(*this);
	}

	/*! \brief side effect when moved. always call this after moving*/
	void ControlPoint::sideEffect()
	{
	}

	/*! \brief side effect when moved. always call this after moving*/
	void NodeGraphicsItem::ControlPoint::sideEffect()
	{
		if (nodeItem && nodeItem->scene())
		{
			for (int i=0; i < nodeItem->boundaryControlPoints.size(); ++i)
				if (nodeItem->boundaryControlPoints[i] == this)
				{
					nodeItem->adjustToBoundaryControlPoints();
					break;
				}
		}
	}

	/*! \brief Copy Constructor */
	NodeGraphicsItem::ControlPoint::ControlPoint(const ControlPoint& copy) : Tinkercell::ControlPoint(copy.parentItem())
	{
		nodeItem = copy.nodeItem;
		setPos(copy.pos());
		setRect(copy.rect());
		setPen(defaultPen = copy.defaultPen);
		setBrush(defaultBrush = copy.defaultBrush);
		//setTransform(copy.transform());
	}

	/*! \brief make a copy of this item*/
	Tinkercell::ControlPoint* NodeGraphicsItem::ControlPoint::clone() const
	{
		return new NodeGraphicsItem::ControlPoint(*this);
	}

	/*! \brief destructor */
	NodeGraphicsItem::ControlPoint::~ControlPoint()
	{
		if (nodeItem)
		{
			for (int i=0; i < nodeItem->controlPoints.size(); ++i)
				if (nodeItem->controlPoints[i] == this)
					nodeItem->controlPoints[i] = 0;
			for (int i=0; i < nodeItem->boundaryControlPoints.size(); ++i)
				if (nodeItem->boundaryControlPoints[i] == this)
					nodeItem->boundaryControlPoints[i] = 0;
			nodeItem = 0;
		}
	}

	/*! \brief operator = */
	NodeGraphicsItem::ControlPoint& NodeGraphicsItem::ControlPoint::operator =(const ControlPoint& copy)
	{
		nodeItem = copy.nodeItem;
		setPos(copy.pos());
		setRect(copy.rect());
		setPen(defaultPen = copy.defaultPen);
		setBrush(defaultBrush = copy.defaultBrush);
		setTransform(copy.transform());
		defaultSize = copy.defaultSize;
		return *this;
	}

	/*! \brief paint method. draw one of the shapes*/
	void ControlPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *,QWidget *)
	{
		if (painter)
		{
			QRectF rect = boundingRect();

			painter->setBrush(brush());
			painter->setPen(pen());

			if (shapeType == circle)
				painter->drawEllipse(rect);
			else
			if (shapeType == square)
				painter->drawRect(rect);
			else
			{
				painter->drawLine(rect.bottomLeft(),rect.bottomRight());
				painter->drawLine(rect.bottomRight(),QPointF(rect.center().x(),rect.top()));
				painter->drawLine(QPointF(rect.center().x(),rect.top()),rect.bottomLeft());
			}
		}
	}

	/*! \brief paint method. Call's parent's*/
	void NodeGraphicsItem::ControlPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
	{
		if (nodeItem && nodeItem->scene() == scene() && nodeItem->isVisible())
			Tinkercell::ControlPoint::paint(painter,option,widget);
	}


	/*! \brief change fill color of all shapes*/
	void NodeGraphicsItem::setBrush(const QBrush& newBrush)
	{
		for (int i=0; i < shapes.size(); ++i)
		{
			if (shapes[i])// && shapes[i]->contains(point))
			{
				shapes[i]->setBrush(newBrush);
			}
		}
	}

	/*! \brief change outline color of all shapes*/
	void NodeGraphicsItem::setPen(const QPen& newPen)
	{
		for (int i=0; i < shapes.size(); ++i)
		{
			if (shapes[i])// && shapes[i]->contains(point))
			{
				shapes[i]->setPen(newPen);
			}
		}
	}

	/*! \brief change alpha value for brush of all shapes*/
	void NodeGraphicsItem::setAlpha(int value)
	{
		for (int i=0; i < shapes.size(); ++i)
		{
			if (shapes[i])
			{
				QPen pen = shapes[i]->pen();
				QColor col1 = pen.color();
				col1.setAlpha(value);
				pen.setColor(col1);
				shapes[i]->setPen(pen);

				QBrush brush = shapes[i]->brush();
				QColor col2 = brush.color();
				col2.setAlpha(value);
				brush.setColor(col2);
				if(brush.gradient())
				{
					QGradient g(*brush.gradient());
					QGradientStops stops = brush.gradient()->stops();
					for (int i=0; i < stops.size(); ++i)
						stops[i].second.setAlpha(value);
					g.setStops(stops);
					brush = QBrush(g);
				}

				shapes[i]->setBrush(brush);
			}
		}
	}

	/*! Constructor: sets angle to 0 and scale to 1*/
	NodeGraphicsItem::Shape::Shape(NodeGraphicsItem * idrawable_ptr, QGraphicsItem * parent, QGraphicsScene * scene) :
		QGraphicsPolygonItem(parent, scene)
	{
		negative = false;
		nodeItem = idrawable_ptr;
		setPen(defaultPen = QPen(QColor(100,100,100)));
		setBrush(defaultBrush = QBrush(QColor(100,100,255,50)));
	}
	/*! Copy Constructor : shallow copy of all vectors*/
	NodeGraphicsItem::Shape::Shape(const Shape& copy): QGraphicsPolygonItem(copy.parentItem(), copy.scene())
	{
		negative = copy.negative;
		setPos(copy.pos());
		defaultPen = copy.defaultPen;
		defaultBrush = copy.defaultBrush;
		setBrush(defaultBrush);
		setPen(defaultPen);
		nodeItem = copy.nodeItem;
		controlPoints = copy.controlPoints;
		parameters = copy.parameters;
		types = copy.types;
		gradientPoints = copy.gradientPoints;
		polygon = copy.polygon;
	}

	/*! operator = shallow copy of all vectors */
	NodeGraphicsItem::Shape& NodeGraphicsItem::Shape::operator = (const Shape& copy)
	{
		negative = copy.negative;
		setPos(copy.pos());
		nodeItem = copy.nodeItem;
		controlPoints = copy.controlPoints;
		parameters = copy.parameters;
		types = copy.types;
		gradientPoints = copy.gradientPoints;
		polygon = copy.polygon;
		setPen(defaultPen = copy.defaultPen);
		setBrush(defaultBrush = copy.defaultBrush);
		setTransform(copy.transform());
		return *this;
	}

	/*! \brief paint method. Call's parent's paint after setting antialiasing to true*/
/*	void NodeGraphicsItem::Shape::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
	{
		painter->setRenderHint(QPainter::Antialiasing);
		QGraphicsPolygonItem::paint(painter,option,widget);
	}*/
	/*! \brief Generates a new polygon using the points and types vectors
	* Precondition: controlPoints.size > 1
	* Postcondition: NA
	* \param void
	* \return void*/
	void NodeGraphicsItem::Shape::refresh()
	{
		if (controlPoints.size() < 2) return;

		int i = 0, j = 1, k = 0, pts = controlPoints.size(), ops = types.size(), params = parameters.size();

		QPainterPath path;
		path.moveTo(QPointF(controlPoints[0]->scenePos().x(),controlPoints[0]->scenePos().y()));

		while (j < pts && i < ops)
		{
			switch(types[i])
			{
				case arc:
					{
						if (pts > j && params > (k+1))
						{
							qreal start = parameters[k], span = parameters[k+1];
							/*qreal prop = 360.0/span;
							qreal w = (controlPoints[j]->scenePos().x()-controlPoints[j-1]->scenePos().x()) * prop/2.0,
								  h = (controlPoints[j]->scenePos().x()-controlPoints[j-1]->scenePos().x()) * prop/2.0;
							path.arcMoveTo(QRectF(
											  controlPoints[j]->scenePos().x() - cos(parameters[k])*w,
											  controlPoints[j-1]->scenePos().x() - sin(parameters[k])*h,
											  w,
											  h),start);
							path.arcTo(QRectF(controlPoints[j]->scenePos().x() - cos(parameters[k])*w,
											  controlPoints[j-1]->scenePos().x() - sin(parameters[k])*h,
											  w,
											  h),start,span);*/
							qreal w = (controlPoints[j]->scenePos().x()-controlPoints[j-1]->scenePos().x()),
								  h = (controlPoints[j]->scenePos().y()-controlPoints[j-1]->scenePos().y());
							path.arcMoveTo(QRectF(controlPoints[j-1]->scenePos().x(),controlPoints[j-1]->scenePos().y(),w,h),start);
							path.arcTo(QRectF(controlPoints[j-1]->scenePos().x(),controlPoints[j-1]->scenePos().y(),w,h),start,span);
							path.moveTo(controlPoints[j]->scenePos().x(),controlPoints[j]->scenePos().y());
						}
						j += 2;
						k += 2;
					}
					break;
				case line:
					{
						path.lineTo(QPointF(controlPoints[j]->scenePos().x(),controlPoints[j]->scenePos().y()));
						++j;
					}
					break;
				case bezier:
					{
						if (controlPoints.size() > (j+2))
							path.cubicTo(QPointF(controlPoints[j]->scenePos().x(),controlPoints[j]->scenePos().y()),
										 QPointF(controlPoints[j+1]->scenePos().x(),controlPoints[j+1]->scenePos().y()),
										 QPointF(controlPoints[j+2]->scenePos().x(),controlPoints[j+2]->scenePos().y()));
						j += 3;
					}
					break;
			}
			++i;
		}
		path.closeSubpath();
		this->path = path;
		polygon = path.toFillPolygon();
		setPolygon(polygon);
		setFlag(QGraphicsItem::ItemIsMovable, false);
		setFlag(QGraphicsItem::ItemIsSelectable, false);
		recomputeBoundingRect();
	}
	/*! \brief bounding rectangle*/
	QRectF NodeGraphicsItem::Shape::boundingRect() const
	{
		//return boundingRectangle;
		return QGraphicsPolygonItem::boundingRect();//.adjusted(-pen().widthF(),-pen().widthF(),2.0*pen().widthF(),2.0*pen().widthF());
	}

	/*! \brief Checks if the polygon is closed*/
	bool NodeGraphicsItem::Shape::isClosed() const
	{
		/*for (int i=0; i < controlPoints.size()-1; ++i)
		{
			for (int j=i+1; j < controlPoints.size(); ++j)
			{
				if (controlPoints[i] && controlPoints[i] == controlPoints[j])
					return true;
			}
		}
		return false;*/
		if (controlPoints.size() < 2) return false;
		return (controlPoints.first() == controlPoints.last());
	}

	/*! \brief gets a path that represents this graphicsItem*/
	QPainterPath NodeGraphicsItem::Shape::shape() const
	{
		return path;
	}

	/*! \brief add a new control point*/
	void NodeGraphicsItem::addControlPoint(ControlPoint * control)
	{
		if (control)
		{
			control->nodeItem = this;
			controlPoints.push_back(control);
		}
	}
	/*! \brief reconstruct bounding rect*/
	void NodeGraphicsItem::Shape::recomputeBoundingRect()
	{
		qreal minx = 0, maxx = 0, miny = 0, maxy = 0;
		for (int i=0; i < controlPoints.size(); ++i)
		{
			if (i == 0)
			{
				minx = controlPoints[i]->x() - controlPoints[i]->rect().width();
				maxx = controlPoints[i]->x() + controlPoints[i]->rect().width();
				miny = controlPoints[i]->y() - controlPoints[i]->rect().height();
				maxy = controlPoints[i]->y() + controlPoints[i]->rect().height();
			}
			else
			{
				if (controlPoints[i]->x() - controlPoints[i]->rect().width() < minx)
					minx = controlPoints[i]->x() - controlPoints[i]->rect().width();
				if (controlPoints[i]->x() + controlPoints[i]->rect().width() > maxx)
					maxx = controlPoints[i]->x() + controlPoints[i]->rect().width();
				if (controlPoints[i]->y() - controlPoints[i]->rect().height() < miny)
					miny = controlPoints[i]->y() - controlPoints[i]->rect().height();
				if (controlPoints[i]->y() + controlPoints[i]->rect().height() > maxy)
					maxy = controlPoints[i]->y() + controlPoints[i]->rect().height();
			}
		}

		boundingRectangle.setLeft(minx);
		boundingRectangle.setTop(miny);
		boundingRectangle.setRight(maxx);
		boundingRectangle.setBottom(maxy);
	}
	/*! \brief reconstruct bounding rect*/
	void NodeGraphicsItem::recomputeBoundingRect()
	{
		/*qreal minx = 0, maxx = 0, miny = 0, maxy = 0;
		for (int i=0; i < controlPoints.size(); ++i)
		{
			if (i == 0)
			{
				minx = controlPoints[i]->x() - controlPoints[i]->rect().width();
				maxx = controlPoints[i]->x() + controlPoints[i]->rect().width();
				miny = controlPoints[i]->y() - controlPoints[i]->rect().height();
				maxy = controlPoints[i]->y() + controlPoints[i]->rect().height();
			}
			else
			{
				if (controlPoints[i]->x() - controlPoints[i]->rect().width() < minx)
					minx = controlPoints[i]->x() - controlPoints[i]->rect().width();
				if (controlPoints[i]->x() + controlPoints[i]->rect().width() > maxx)
					maxx = controlPoints[i]->x() + controlPoints[i]->rect().width();
				if (controlPoints[i]->y() - controlPoints[i]->rect().height() < miny)
					miny = controlPoints[i]->y() - controlPoints[i]->rect().height();
				if (controlPoints[i]->y() + controlPoints[i]->rect().height() > maxy)
					maxy = controlPoints[i]->y() + controlPoints[i]->rect().height();
			}
		}

		boundingRectangle.setLeft(minx);
		boundingRectangle.setTop(miny);
		boundingRectangle.setRight(maxx);
		boundingRectangle.setBottom(maxy);

		for (int i=0; i < boundaryControlPoints.size(); ++i)
			if (boundaryControlPoints[i])
			{
				if (i==0)
					boundaryControlPoints[i]->setPos(minx,miny);
				else
					boundaryControlPoints[i]->setPos(maxx,maxy);
			}*/

		boundingRectangle = QRectF(0,0,0,0);
		for (int i=0; i < shapes.size(); ++i)
		{
			if (shapes[i])
				boundingRectangle = boundingRectangle.unite(shapes[i]->boundingRect());
		}
	}

	/*! \brief bounding rect*/
	QRectF NodeGraphicsItem::boundingRect() const
	{
		return boundingRectangle;
		//return QGraphicsItemGroup::boundingRect();
	}

	/*! \brief gets a polygon that is constructed by uniting all the shapes*/
	QPolygonF NodeGraphicsItem::polygon() const
	{
		QPolygonF polygon;
		for (int i=0; i < shapes.size(); ++i)
			if (shapes[i])
				polygon = polygon.united(shapes[i]->polygon);
		return polygon;
	}
	/*! \brief gets a path that is constructed by uniting all the shape paths*/
	QPainterPath NodeGraphicsItem::shape() const
	{
		QPainterPath path;
		for (int i=0; i < shapes.size(); ++i)
			if (shapes[i])
				path = path.united(shapes[i]->mapToParent(shapes[i]->path));
		return path;
	}
	/*! \brief add a shape to the set of shapes*/
	void NodeGraphicsItem::addShape(Shape* shape)
	{
		if (shape)
		{
			shape->nodeItem = this;
			shapes.push_back(shape);
		}
	}
	/*! \brief remove a control point*/
	void NodeGraphicsItem::removeControlPoint(ControlPoint * control)
	{
		if (control)
		{
			if (controlPoints.contains(control))
				controlPoints.remove( controlPoints.indexOf(control) );
			for (int i=shapes.size()-1; i >= 0; --i)
				if (shapes[i] && shapes[i]->controlPoints.contains(control))
				{
					removeFromGroup(shapes[i]);
					delete shapes[i];
					shapes.remove(i);
				}
			refresh();
		}
	}
	/*! \brief add a shape to the set of shapes*/
	void NodeGraphicsItem::removeShape(Shape* shape)
	{
		if (shape)
		{
			if (shapes.contains(shape))
			{
				shapes.remove( shapes.indexOf(shape));
				removeFromGroup(shape);
				delete shape;
			}
			refresh();
		}
	}

	/*! \brief Updates the graphicsItem by re-initializing the vector of shapes
	* Precondition: shapes.size > 1
	* Postcondition: NA
	* \param void
	* \return void*/
	void NodeGraphicsItem::refresh()
	{
		for (int i=0; i < shapes.size(); ++i)
		{
			if (shapes[i])
			{
				removeFromGroup(shapes[i]);
				shapes[i]->refresh();
				shapes[i]->setZValue(i);
				addToGroup(shapes[i]);
			}
			else
				shapes.remove(i);
		}
		recomputeBoundingRect();

		if (boundingBoxItem)
		{
			boundingBoxItem->setPen(QPen(QColor(255,100,100),(1000.0/(0.5*(sceneBoundingRect().width() + sceneBoundingRect().height()))),Qt::DashLine));
			boundingBoxItem->setBrush(Qt::NoBrush);
			boundingBoxItem->setVisible(false);
			boundingBoxItem->setPos(0,0);
			boundingBoxItem->setRect(boundingRect());
		}
	}

	/*! \brief Clear all shapes and control points
	* \param void
	* \return void*/
	void NodeGraphicsItem::clear()
	{
		for (int i=0; i < shapes.size(); ++i)
		{
			removeFromGroup(shapes[i]);
			if (shapes[i])
			{
				if (shapes[i]->scene())
					shapes[i]->scene()->removeItem(shapes[i]);
				delete shapes[i];
			}
		}
		for (int i=0; i < controlPoints.size(); ++i)
		{
			if (controlPoints[i])
			{
				removeFromGroup(controlPoints[i]);
				if (controlPoints[i]->scene())
					controlPoints[i]->scene()->removeItem(controlPoints[i]);
				delete controlPoints[i];
			}
		}
		/*for (int i=0; i < boundaryControlPoints.size(); ++i)
		{
			if (boundaryControlPoints[i])
			{
				removeFromGroup(boundaryControlPoints[i]);
				if (boundaryControlPoints[i]->scene())
					boundaryControlPoints[i]->scene()->removeItem(boundaryControlPoints[i]);
				delete boundaryControlPoints[i];
			}
		}*/
		shapes.clear();
		controlPoints.clear();
		//boundaryControlPoints.clear();
	}
	/*! \brief normalizes a node graphics item so that its center is 0,0 and width*height is 10
	* \param NodeImage pointer to normalize
	* \return void*/
	void  NodeGraphicsItem::normalize()
	{
		qreal minx = -1, maxx = -1, miny = -1, maxy = -1;
		for (int i=0; i < controlPoints.size(); ++i)
			if (controlPoints[i] != 0)
			{
				QPointF point = controlPoints[i]->pos();
				if (maxx < 0 || maxx < point.x()) maxx = point.x();
				if (maxy < 0 || maxy < point.y()) maxy = point.y();
				if (minx < 0 || minx > point.x()) minx = point.x();
				if (miny < 0 || miny > point.y()) miny = point.y();
			}
		QPointF center( (minx + maxx)/2, (miny + maxy)/2 );
		for (int i=0; i < controlPoints.size(); ++i)
			if (controlPoints[i] != 0)
			{
				QPointF point = controlPoints[i]->pos();
				controlPoints[i]->setPos( point - center );
			}

		for (int i=0; i < shapes.size(); ++i)
			if (shapes[i] != 0)
			{
				Shape * shape = shapes[i];
				shape->gradientPoints.first -= center;
				shape->gradientPoints.second -= center;
				QPointF start = shape->gradientPoints.first;
				QPointF stop = shape->gradientPoints.second;
				QBrush brush = shape->brush();
				if (brush.gradient() != 0)
				{
					QGradient * oldGradient = new QGradient(*brush.gradient());
					if (oldGradient->type() == QGradient::LinearGradient)
					{
						QLinearGradient gradient(start,stop);
						gradient.setStops(oldGradient->stops());
						shape->setBrush(gradient);
					}
					else
					if (oldGradient->type() == QGradient::RadialGradient)
					{
						QRadialGradient gradient(start,
											sqrt( (stop.y()-start.y())*(stop.y()-start.y()) +
													(stop.x()-start.x())*(stop.x()-start.x())));
						gradient.setStops(oldGradient->stops());
						shape->setBrush(gradient);
					}
					else
					{
						QGradient gradient;
						gradient.setStops(oldGradient->stops());
						shape->setBrush(gradient);
					}
					delete oldGradient;
				}
				shape->defaultBrush = shape->brush();
			}
		//		shapes[i]->translate(-center.x(), -center.y());
		refresh();
	}

	/*! \brief gets the node graphics item from its child item*/
	NodeGraphicsItem * NodeGraphicsItem::topLevelNodeItem(QGraphicsItem* item,bool includeControlPoints)
	{
		NodeGraphicsItem * idptr = 0, * idptr2 = 0;
		NodeGraphicsItem::ControlPoint * cp = 0;

		QGraphicsItem * p = item;

		while (p)
		{
			idptr = qgraphicsitem_cast<NodeGraphicsItem*>(p);
			if (!idptr && includeControlPoints)
			{
				cp = qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(p);
				if (cp && cp->nodeItem)
					idptr = cp->nodeItem;
			}

			if (idptr != 0) idptr2 = idptr;

			if (p != p->parentItem())
				p = p->parentItem();
			else
				break;
		}

		return idptr2;
	}

	/*! \brief get all the connection items linked to this node*/
	QList<ConnectionGraphicsItem*> NodeGraphicsItem::connections()
	{
		QList<ConnectionGraphicsItem*> connections;
		QList<QGraphicsItem*> children = this->childItems();
		QGraphicsScene * scene = this->scene();
		for (int i=0; i < children.size(); ++i)
		{
			ConnectionGraphicsItem::ControlPoint * cp =
				qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(children[i]);
			if (cp && cp->connectionItem && cp->connectionItem->scene() && cp->connectionItem->isVisible() && (cp->connectionItem->scene() == scene))
				connections += cp->connectionItem;
		}
		return connections;
	}

	/*! \brief get all the connection items linked to this node*/
	QList<ConnectionGraphicsItem*> NodeGraphicsItem::connectionsWithArrows()
	{
		QList<ConnectionGraphicsItem*> connections;
		QList<QGraphicsItem*> children = this->childItems();
		QGraphicsScene * scene = this->scene();
		for (int i=0; i < children.size(); ++i)
		{
			ConnectionGraphicsItem::ControlPoint * cp =
				qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(children[i]);
			if (cp && cp->connectionItem && cp->connectionItem->scene() && cp->connectionItem->isVisible()
				&& (cp->connectionItem->scene() == scene)
				&& (cp->connectionItem->nodesWithArrows().contains(this)))
				connections += cp->connectionItem;
		}
		return connections;
	}

	/*! \brief get all the connection items linked to this node*/
	QList<ConnectionGraphicsItem*> NodeGraphicsItem::connectionsWithoutArrows()
	{
		QList<ConnectionGraphicsItem*> connections;
		QList<QGraphicsItem*> children = this->childItems();
		QGraphicsScene * scene = this->scene();
		for (int i=0; i < children.size(); ++i)
		{
			ConnectionGraphicsItem::ControlPoint * cp =
				qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(children[i]);
			if (cp && cp->connectionItem && cp->connectionItem->scene() && cp->connectionItem->isVisible()
				&& (cp->connectionItem->scene() == scene)
				&& !(cp->connectionItem->nodesWithArrows().contains(this)))
				connections += cp->connectionItem;
		}
		return connections;
	}

	/*! \brief get all the connection items linked to this node*/
	QList<ConnectionGraphicsItem*> NodeGraphicsItem::connectionsDisconnected()
	{
		QList<ConnectionGraphicsItem*> connections;
		QList<QGraphicsItem*> children = this->childItems();
		QGraphicsScene * scene = this->scene();
		for (int i=0; i < children.size(); ++i)
		{
			ConnectionGraphicsItem::ControlPoint * cp =
				qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(children[i]);
			if (cp && cp->connectionItem && cp->connectionItem->scene() && cp->connectionItem->isVisible()
				&& (cp->connectionItem->scene() == scene)
				&& (cp->connectionItem->nodesDisconnected().contains(this)))
				connections += cp->connectionItem;
		}
		return connections;
	}

	/*! \brief get all the connection items linked to this node*/
	QList<QGraphicsItem*> NodeGraphicsItem::connectionsAsGraphicsItems()
	{
		QList<QGraphicsItem*> connections;
		QList<QGraphicsItem*> children = this->childItems();
		QGraphicsScene * scene = this->scene();
		for (int i=0; i < children.size(); ++i)
		{
			ConnectionGraphicsItem::ControlPoint * cp =
				qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(children[i]);
			if (cp && cp->connectionItem && cp->connectionItem->scene() && cp->connectionItem->isVisible() && (cp->connectionItem->scene() == scene))
				connections += cp->connectionItem;
		}
		return connections;
	}


}
