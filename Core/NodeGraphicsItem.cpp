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
#include <QFont>
#include "ConsoleWindow.h"
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "ConnectionGraphicsItem.h"
#include "NodeGraphicsItem.h"
#include "ItemHandle.h"
#include "UndoCommands.h"
#include "NodeGraphicsReader.h"
#include "Tool.h"

namespace Tinkercell
{
	const QString NodeGraphicsItem::CLASSNAME = QString("NodeGraphicsItem");

	ItemHandle * NodeGraphicsItem::handle() const
	{
		return itemHandle;
	}

	void NodeGraphicsItem::setHandle(ItemHandle * handle)
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

	ItemHandle * NodeGraphicsItem::ControlPoint::handle() const
	{
		/*if (nodeItem)
			return nodeItem->handle();*/
		return 0;
	}

	void NodeGraphicsItem::ControlPoint::setHandle(ItemHandle * h)
	{
		if (nodeItem)
			nodeItem->setHandle(h);
	}
	
	/*! Constructor: does nothing */
	NodeGraphicsItem::NodeGraphicsItem(QGraphicsItem * parent) : 
		QGraphicsItemGroup (parent), itemHandle(0), boundingBoxItem(0)
	{
		setCacheMode(QGraphicsItem::DeviceCoordinateCache);
		setFlag(QGraphicsItem::ItemIsMovable, false);
		setFlag(QGraphicsItem::ItemIsSelectable, false);

		className = NodeGraphicsItem::CLASSNAME;
	
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
		boundingBoxItem->setPen(QPen(QColor(255,100,100),getPenWidthForBoundingRect(),Qt::DashLine));
		boundingBoxItem->setBrush(Qt::NoBrush);
		boundingBoxItem->setVisible(false);
		boundingBoxItem->setPos(0,0);
		boundingBoxItem->setRect(boundingRect());
		
		setBoundingBoxVisible(false);
	}
	
	NodeGraphicsItem::NodeGraphicsItem(const QString & filename,QGraphicsItem * parent) : 
		QGraphicsItemGroup (parent), itemHandle(0), boundingBoxItem(0)
	{
		setCacheMode(QGraphicsItem::DeviceCoordinateCache);
		setFlag(QGraphicsItem::ItemIsMovable, false);
		setFlag(QGraphicsItem::ItemIsSelectable, false);

		className = NodeGraphicsItem::CLASSNAME;
		
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
		boundingBoxItem->setPen(QPen(QColor(255,100,100),getPenWidthForBoundingRect(),Qt::DashLine));
		boundingBoxItem->setBrush(Qt::NoBrush);
		boundingBoxItem->setVisible(false);
		boundingBoxItem->setPos(0,0);
		boundingBoxItem->setRect(boundingRect());
		
		setBoundingBoxVisible(false);
		
		NodeGraphicsReader reader;
        reader.readXml(this, filename);
        normalize();
        scale(defaultSize.width()/boundingRect().width(),defaultSize.height()/boundingRect().height());
	}

	/*! reset of control points that control the bounding box of this figure */
	void NodeGraphicsItem::adjustBoundaryControlPoints()
	{
		if (boundaryControlPoints.size() == 2
			&& boundaryControlPoints[0]
			&& boundaryControlPoints[1])
		{
			if (scene() && (boundaryControlPoints[0]->scene() != scene()))
				(static_cast<GraphicsScene*>(scene()))->addItem(boundaryControlPoints[0]);

			if (scene() && (boundaryControlPoints[1]->scene() != scene()))
				(static_cast<GraphicsScene*>(scene()))->addItem(boundaryControlPoints[1]);

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
					(static_cast<GraphicsScene*>(s))->addItem(boundaryControlPoints[i]);

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

	void NodeGraphicsItem::showBoundingBox(bool controlPoints)
	{
		setBoundingBoxVisible(true,controlPoints);
	}

	void NodeGraphicsItem::hideBoundingBox(bool controlPoints)
	{
		setBoundingBoxVisible(true,controlPoints);
	}

	/*! Copy Constructor: deep copy of all pointers */
	NodeGraphicsItem::NodeGraphicsItem(const NodeGraphicsItem& copy) : 
		QGraphicsItemGroup (0) , itemHandle(0), boundingBoxItem(0)
	{
		setFlag(QGraphicsItem::ItemIsMovable, false);
		setFlag(QGraphicsItem::ItemIsSelectable, false);
		setVisible(copy.isVisible());
		
		QTransform t0 = copy.sceneTransform();
		QTransform t1(t0.m11(),t0.m12(),0,t0.m21(),t0.m22(),0,0,0,1);
		setPos(copy.scenePos());

#if QT_VERSION > 0x040600
		setTransform(t1);
#endif

		/**copy handle**/
		className = copy.className;
		groupID = copy.groupID;
		itemHandle = copy.itemHandle;
		name = copy.name;
		defaultSize = copy.defaultSize;

		if (itemHandle)
			setHandle(itemHandle);

		/**Copy control points and shapes**/
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

#if QT_VERSION < 0x040600	
		setTransform(t1);
#endif

		ControlPoint * topleft = new ControlPoint(this);
		topleft->setPos( copy.sceneBoundingRect().topLeft() );
		topleft->setVisible(false);
		topleft->scale(0.75,0.75);
		topleft->setPen(topleft->defaultPen = QPen(QColor(255,100,100),2.0));
		topleft->setBrush(topleft->defaultBrush = QBrush(QColor(255,255,255,255)));

		ControlPoint * bottomright = new ControlPoint(this);
		bottomright->setPos( copy.sceneBoundingRect().bottomRight() );
		bottomright->setVisible(false);
		bottomright->scale(0.75,0.75);
		bottomright->setPen(bottomright->defaultPen = QPen(QColor(255,100,100),2.0));
		bottomright->setBrush(bottomright->defaultBrush = QBrush(QColor(255,255,255,255)));

		bottomright->shapeType = topleft->shapeType = ControlPoint::square;
		boundaryControlPoints << topleft << bottomright;

		boundingBoxItem = new QGraphicsRectItem(this);
		boundingBoxItem->setPen(QPen(QColor(255,100,100),getPenWidthForBoundingRect(),Qt::DashLine));
		boundingBoxItem->setBrush(Qt::NoBrush);
		boundingBoxItem->setVisible(false);
		boundingBoxItem->setPos(0,0);
		boundingBoxItem->setRect(boundingRect());
		
		setBoundingBoxVisible(false);
	}
	
	qreal NodeGraphicsItem::getPenWidthForBoundingRect()
	{
		qreal w = (1000.0/((sceneBoundingRect().width() + sceneBoundingRect().height())));
		if (w > 1.0) w = 1.0;
		return w;
	}

	/*! \brief make a copy of this item*/
	NodeGraphicsItem* NodeGraphicsItem::clone() const
	{
		NodeGraphicsItem* node = new NodeGraphicsItem(*this);
		//node->className = NodeGraphicsItem::CLASSNAME;
		return node;
	}

	/*! operator =: deep copy of all pointers */
	NodeGraphicsItem& NodeGraphicsItem::operator = (const NodeGraphicsItem& copy)
	{
		clear();
		name = copy.name;
		//className = copy.className;
		defaultSize = copy.defaultSize;
		/*QList<QGraphicsItem*> children = childItems();
		for (int i=0; i < children.size(); ++i)
		removeFromGroup(children[i]);*/

		/**Copy control points and shapes**/
		QGraphicsItem * parent = parentItem();
		setParentItem(0);
		setBoundingBoxVisible(false);

#if QT_VERSION < 0x040600		
		resetTransform();
#endif

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

#if QT_VERSION < 0x040600
		QTransform t0 = copy.sceneTransform();
		QTransform t1(t0.m11(),t0.m12(),0,t0.m21(),t0.m22(),0,0,0,1);
		setTransform(t1);
#endif

		for (int i=0; i < boundaryControlPoints.size(); ++i)
			if (boundaryControlPoints[i] && !MainWindow::invalidPointers.contains(boundaryControlPoints[i]))
			{
				boundaryControlPoints[i]->nodeItem = 0;
				if (!boundaryControlPoints[i]->scene())
				{
					delete boundaryControlPoints[i];
					MainWindow::invalidPointers[ (void*)(boundaryControlPoints[i]) ] = true;
				}
				boundaryControlPoints[i] = 0;
			}
			boundaryControlPoints.clear();

		ControlPoint * topleft = new ControlPoint(this);
		topleft->setPos( copy.sceneBoundingRect().topLeft() );
		topleft->setVisible(false);
		topleft->scale(0.75,0.75);
		topleft->setPen(topleft->defaultPen = QPen(QColor(255,100,100),2.0));
		topleft->setBrush(topleft->defaultBrush = QBrush(QColor(255,255,255,255)));

		ControlPoint * bottomright = new ControlPoint(this);
		bottomright->setPos( copy.sceneBoundingRect().bottomRight() );
		bottomright->setVisible(false);
		bottomright->scale(0.75,0.75);
		bottomright->setPen(bottomright->defaultPen = QPen(QColor(255,100,100),2.0));
		bottomright->setBrush(bottomright->defaultBrush = QBrush(QColor(255,255,255,255)));

		bottomright->shapeType = topleft->shapeType = ControlPoint::square;
		boundaryControlPoints << topleft << bottomright;

		boundingBoxItem = new QGraphicsRectItem(this);
		boundingBoxItem->setPen(QPen(QColor(255,100,100),getPenWidthForBoundingRect(),Qt::DashLine));
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
		
		QList<QGraphicsItem*> children = this->childItems();
			
		for (int i=0; i < children.size(); ++i)
			if (children[i])
				children[i]->setParentItem(0);

		for (int i=0; i < boundaryControlPoints.size(); ++i)
			if (boundaryControlPoints[i] && !MainWindow::invalidPointers.contains((void*)boundaryControlPoints[i]))
			{
				boundaryControlPoints[i]->nodeItem = 0;
				if (boundaryControlPoints[i]->scene())
					boundaryControlPoints[i]->scene()->removeItem(boundaryControlPoints[i]);
				
				delete boundaryControlPoints[i];
				MainWindow::invalidPointers[ (void*)boundaryControlPoints[i] ] = true;
				
				boundaryControlPoints[i] = 0;
			}

		if (boundingBoxItem && !MainWindow::invalidPointers.contains((void*)(boundingBoxItem)))
		{
			removeFromGroup(boundingBoxItem);
			if (boundingBoxItem->scene())
				boundingBoxItem->scene()->removeItem(boundingBoxItem);
			delete boundingBoxItem;
			MainWindow::invalidPointers[ (void*)(boundingBoxItem) ] = true;
			boundingBoxItem = 0;
		}

		if (!itemHandle) return;
		setHandle(0);
	}

	/*! \brief checks that this is a valid drawable*/
	bool NodeGraphicsItem::isValid() const
	{
		return controlPoints.size() > 1 && shapes.size() > 0;
	}

	/*! set the top left and bottom right corners of this node item*/
	void NodeGraphicsItem::setBoundingRect(const QPointF& p1, const QPointF& p2)
	{
		if (boundaryControlPoints.size() < 2 ||
			!boundaryControlPoints[0] ||
			!boundaryControlPoints[1])
			return;

		boundaryControlPoints[0]->setPos(p1);
		boundaryControlPoints[1]->setPos(p2);

		adjustToBoundaryControlPoints();
	}

	void NodeGraphicsItem::adjustToBoundaryControlPoints()
	{
		if (boundaryControlPoints.size() < 2 ||
			!boundaryControlPoints[0] ||
			!boundaryControlPoints[1])
			return;

		if (scene() && (boundaryControlPoints[0]->scene() != scene()))
			(static_cast<GraphicsScene*>(scene()))->addItem(boundaryControlPoints[0]);

		if (scene() && (boundaryControlPoints[1]->scene() != scene()))
			(static_cast<GraphicsScene*>(scene()))->addItem(boundaryControlPoints[1]);

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
					qreal w = getPenWidthForBoundingRect();
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

	/*! \brief change fill color of all shapes to default*/
	void NodeGraphicsItem::resetBrush()
	{
		for (int i=0; i < shapes.size(); ++i)
		{
			if (shapes[i])// && shapes[i]->contains(point))
			{
				shapes[i]->setBrush(shapes[i]->defaultBrush);
			}
		}
	}

	/*! \brief change outline color of all shapes to default*/
	void NodeGraphicsItem::resetPen()
	{
		for (int i=0; i < shapes.size(); ++i)
		{
			if (shapes[i])// && shapes[i]->contains(point))
			{
				shapes[i]->setPen(shapes[i]->defaultPen);
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
			case rectangle:
				{
					if (pts > j && params > k)
					{
						QRectF rect(controlPoints[j-1]->scenePos(),controlPoints[j]->scenePos());
						path.addRoundedRect(rect,parameters[k],parameters[k]);
					}
					++j;
					++k;
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
		if (controlPoints.size() < 2) return false;
		
		if (types.contains(rectangle) || types.contains(arc)) return true;
		
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
				path = path.united(shapes[i]->mapToParent(shapes[i]->shape()));
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
				if (shapes[i] && 
					!MainWindow::invalidPointers.contains(shapes[i]) &&
					shapes[i]->controlPoints.contains(control))
				{
					removeFromGroup(shapes[i]);
					delete shapes[i];
					shapes.remove(i);
					MainWindow::invalidPointers[ (void*)(shapes[i]) ] = true;
				}
			refresh();
		}
	}
	/*! \brief add a shape to the set of shapes*/
	void NodeGraphicsItem::removeShape(Shape* shape)
	{
		if (shape && !MainWindow::invalidPointers.contains(shape))
		{
			if (shapes.contains(shape))
			{
				shapes.remove( shapes.indexOf(shape));
				removeFromGroup(shape);
				delete shape;
				MainWindow::invalidPointers[ (void*)shape ] = true;
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
			boundingBoxItem->setPen(QPen(QColor(255,100,100),getPenWidthForBoundingRect(),Qt::DashLine));
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
			if (shapes[i] && !MainWindow::invalidPointers.contains( (void*)shapes[i]))
			{
				removeFromGroup(shapes[i]);
				if (shapes[i]->scene())
					shapes[i]->scene()->removeItem(shapes[i]);
				delete shapes[i];
				MainWindow::invalidPointers[ (void*)shapes[i] ] = true;
				
				shapes[i] = 0;
			}
		}
		
		for (int i=0; i < controlPoints.size(); ++i)
		{
			if (controlPoints[i] && !MainWindow::invalidPointers.contains((void*)controlPoints[i]))
			{
				removeFromGroup(controlPoints[i]);
				if (controlPoints[i]->scene())
					controlPoints[i]->scene()->removeItem(controlPoints[i]);
				delete controlPoints[i];
				MainWindow::invalidPointers[ (void*)controlPoints[i] ] = true;
				
				controlPoints[i] = 0;
			}
		}
		shapes.clear();
		controlPoints.clear();
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
			idptr = NodeGraphicsItem::cast(p);
			
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
	QList<ConnectionGraphicsItem*> NodeGraphicsItem::connections() const
	{
		QList<ConnectionGraphicsItem*> connections;
		QList<QGraphicsItem*> children = childItems();
		QGraphicsScene * scene = this->scene();
		for (int i=0; i < children.size(); ++i)
		{
			ConnectionGraphicsItem::ControlPoint * cp =
				qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(children[i]);
			if (cp && cp->connectionItem)
				connections += cp->connectionItem;
		}
		return connections;
	}
	
	/*! \brief get all the connected nodes*/
	QList<NodeGraphicsItem*> NodeGraphicsItem::connectedNodes() const
	{
		QList<ConnectionGraphicsItem*> cs = connections();
		QList<NodeGraphicsItem*> nodes, list;
		
		for (int i=0; i < cs.size(); ++i)
		{
			nodes = cs[i]->nodes();
			for (int j=0; j < nodes.size(); ++j)
				if (!list.contains(nodes[j]) && nodes[j] != this)
					list += nodes[j];
		}
		return list;
	}


	/*! \brief get all the connection items linked to this node*/
	QList<ConnectionGraphicsItem*> NodeGraphicsItem::connectionsWithArrows() const
	{
		QList<ConnectionGraphicsItem*> connections;
		QList<QGraphicsItem*> children = this->childItems();
		QGraphicsScene * scene = this->scene();
		for (int i=0; i < children.size(); ++i)
		{
			ConnectionGraphicsItem::ControlPoint * cp =
				qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(children[i]);
			if (cp && cp->connectionItem
				&& (cp->connectionItem->nodesWithArrows().contains(const_cast<NodeGraphicsItem*>(this))))
				connections += cp->connectionItem;
		}
		return connections;
	}

	/*! \brief get all the connection items linked to this node*/
	QList<ConnectionGraphicsItem*> NodeGraphicsItem::connectionsWithoutArrows() const
	{
		QList<ConnectionGraphicsItem*> connections;
		QList<QGraphicsItem*> children = this->childItems();
		QGraphicsScene * scene = this->scene();
		for (int i=0; i < children.size(); ++i)
		{
			ConnectionGraphicsItem::ControlPoint * cp =
				qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(children[i]);
			if (cp && cp->connectionItem
				&& !(cp->connectionItem->nodesWithArrows().contains(const_cast<NodeGraphicsItem*>(this))))
				connections += cp->connectionItem;
		}
		return connections;
	}

	/*! \brief get all the connection items linked to this node*/
	QList<ConnectionGraphicsItem*> NodeGraphicsItem::connectionsDisconnected() const
	{
		QList<ConnectionGraphicsItem*> connections;
		QList<QGraphicsItem*> children = this->childItems();
		QGraphicsScene * scene = this->scene();
		for (int i=0; i < children.size(); ++i)
		{
			ConnectionGraphicsItem::ControlPoint * cp =
				qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(children[i]);
			if (cp && cp->connectionItem
				&& (cp->connectionItem->nodesDisconnected().contains(const_cast<NodeGraphicsItem*>(this))))
				connections += cp->connectionItem;
		}
		return connections;
	}

	/*! \brief get all the connection items linked to this node*/
	QList<QGraphicsItem*> NodeGraphicsItem::connectionsAsGraphicsItems() const
	{
		QList<QGraphicsItem*> connections;
		QList<QGraphicsItem*> children = this->childItems();
		QGraphicsScene * scene = this->scene();
		for (int i=0; i < children.size(); ++i)
		{
			ConnectionGraphicsItem::ControlPoint * cp =
				qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(children[i]);
			if (cp && cp->connectionItem)
				connections += cp->connectionItem;
		}
		return connections;
	}

	QList<NodeGraphicsItem*> NodeGraphicsItem::nodesAdjacent() const
	{
		QList<NodeGraphicsItem*> nodes;

		QGraphicsScene * scene = this->scene();

		if (!scene) return nodes;

		QRectF rect = this->sceneBoundingRect();
		rect.adjust( -rect.width()/10.0, -rect.height()/10.0, rect.width()/10.0, rect.height()/10.0 );

		QList<QGraphicsItem*> items = scene->items(rect);

		NodeGraphicsItem * node;
		for (int i=0; i < items.size(); ++i)
			if ((node = topLevelNodeItem(items[i])) && node != this && !nodes.contains(node))
				nodes << node;

		return nodes;
	}
	
	QList<NodeGraphicsItem*> NodeGraphicsItem::nodesToLeft() const
	{
		QList<NodeGraphicsItem*> nodes;

		QGraphicsScene * scene = this->scene();

		if (!scene) return nodes;

		QRectF rect;
		QPointF p;
		QList<QGraphicsItem*> items;
		NodeGraphicsItem * node1 = const_cast<NodeGraphicsItem*>(this);
		NodeGraphicsItem * node2 = 0;

		while (node1)
		{
			rect = node1->sceneBoundingRect();
			p = QPointF(rect.left() - 1.0, rect.center().y());
			items = scene->items(p);

			node1 = 0;
			for (int i=0; i < items.size(); ++i)
				if ((node2 = topLevelNodeItem(items[i])) && node2 != this && !nodes.contains(node2))
				{
					node1 = node2;
					break;
				}
			if (node1)
				nodes << node1;
		}

		return nodes;
	}
	
	QList<NodeGraphicsItem*> NodeGraphicsItem::nodesToRight() const
	{
		QList<NodeGraphicsItem*> nodes;

		QGraphicsScene * scene = this->scene();

		if (!scene) return nodes;

		QRectF rect;
		QPointF p;
		QList<QGraphicsItem*> items;
		NodeGraphicsItem * node1 = const_cast<NodeGraphicsItem*>(this);
		NodeGraphicsItem * node2 = 0;

		while (node1)
		{
			rect = node1->sceneBoundingRect();
			p = QPointF(rect.right() + 1.0, rect.center().y());
			items = scene->items(p);

			node1 = 0;
			for (int i=0; i < items.size(); ++i)
				if ((node2 = topLevelNodeItem(items[i])) && node2 != this && !nodes.contains(node2))
				{
					node1 = node2;
					break;
				}
			if (node1)
				nodes << node1;
		}

		return nodes;
	}
	
	QList<NodeGraphicsItem*> NodeGraphicsItem::nodesAbove() const
	{
		QList<NodeGraphicsItem*> nodes;

		QGraphicsScene * scene = this->scene();

		if (!scene) return nodes;

		QRectF rect;
		QPointF p;
		QList<QGraphicsItem*> items;
		NodeGraphicsItem * node1 = const_cast<NodeGraphicsItem*>(this);
		NodeGraphicsItem * node2 = 0;

		while (node1)
		{
			rect = node1->sceneBoundingRect();
			p = QPointF(rect.center().x(), rect.top() - 1.0);
			items = scene->items(p);

			node1 = 0;
			for (int i=0; i < items.size(); ++i)
				if ((node2 = topLevelNodeItem(items[i])) && node2 != this && !nodes.contains(node2))
				{
					node1 = node2;
					break;
				}
			if (node1)
				nodes << node1;
		}

		return nodes;
	}
	
	QList<NodeGraphicsItem*> NodeGraphicsItem::nodesBelow() const
	{
		QList<NodeGraphicsItem*> nodes;

		QGraphicsScene * scene = this->scene();

		if (!scene) return nodes;

		QRectF rect;
		QPointF p;
		QList<QGraphicsItem*> items;
		NodeGraphicsItem * node1 = const_cast<NodeGraphicsItem*>(this);
		NodeGraphicsItem * node2 = 0;

		while (node1)
		{
			rect = node1->sceneBoundingRect();
			p = QPointF(rect.center().x(), rect.bottom() + 1.0);
			items = scene->items(p);

			node1 = 0;
			for (int i=0; i < items.size(); ++i)
				if ((node2 = topLevelNodeItem(items[i])) && node2 != this && !nodes.contains(node2))
				{
					node1 = node2;
					break;
				}
			if (node1)
				nodes << node1;
		}

		return nodes;
	}
	
	QList<NodeGraphicsItem*> NodeGraphicsItem::nodesUpstream() const
	{
		QList<NodeGraphicsItem*> nodes;

		QGraphicsScene * scene = this->scene();

		if (!scene) return nodes;

		QRectF rect;
		QPointF p;
		QList<ConnectionGraphicsItem*> clist;
		QList<NodeGraphicsItem*> nlist;
		NodeGraphicsItem * node1 = const_cast<NodeGraphicsItem*>(this);

		while (node1)
		{
			clist = node1->connectionsWithArrows();

			node1 = 0;
			for (int i=0; i < clist.size(); ++i)
				if (clist[i])
				{	
					nlist = clist[i]->nodesWithoutArrows();
					for (int j=0; j < nlist.size(); ++j)
						if (nlist[j])
						{
							node1 = nlist[j];
							break;
						}
					if (node1)
						break;
				}
			if (node1 && node1 != this && !nodes.contains(node1))
				nodes << node1;
			else
				node1 = 0;
		}

		return nodes;
	}
	
	QList<NodeGraphicsItem*> NodeGraphicsItem::nodesDownstream() const
	{
		QList<NodeGraphicsItem*> nodes;

		QGraphicsScene * scene = this->scene();

		if (!scene) return nodes;

		QRectF rect;
		QPointF p;
		QList<ConnectionGraphicsItem*> clist;
		QList<NodeGraphicsItem*> nlist;
		NodeGraphicsItem * node1 = const_cast<NodeGraphicsItem*>(this);

		while (node1)
		{
			clist = node1->connectionsWithoutArrows();

			node1 = 0;
			for (int i=0; i < clist.size(); ++i)
				if (clist[i])
				{	
					nlist = clist[i]->nodesWithArrows();
					for (int j=0; j < nlist.size(); ++j)
						if (nlist[j])
						{
							node1 = nlist[j];
							break;
						}
					if (node1)
						break;
				}
			if (node1 && node1 != this && !nodes.contains(node1))
				nodes << node1;
			else
				node1 = 0;
		}

		return nodes;
	}

	NodeGraphicsItem::Shape * NodeGraphicsItem::longestShape() const
	{
		NodeGraphicsItem::Shape * shape = 0;
		for (int i=0; i < shapes.size(); ++i)
			if (!shape || shapes[i]->boundingRect().width() > shape->boundingRect().width())
				shape = shapes[i];

		return shape;
	}
	
	NodeGraphicsItem::Shape * NodeGraphicsItem::tallestShape() const
	{
		NodeGraphicsItem::Shape * shape = 0;
		for (int i=0; i < shapes.size(); ++i)
			if (!shape || shapes[i]->boundingRect().height() > shape->boundingRect().height())
				shape = shapes[i];

		return shape;
	}

	NodeGraphicsItem::Shape *  NodeGraphicsItem::leftMostShape() const
	{
		NodeGraphicsItem::Shape * shape = 0;
		for (int i=0; i < shapes.size(); ++i)
			if (!shape || shapes[i]->boundingRect().left() > shape->boundingRect().left())
				shape = shapes[i];

		return shape;
	}

	NodeGraphicsItem::Shape *  NodeGraphicsItem::rightMostShape() const
	{
		NodeGraphicsItem::Shape * shape = 0;
		for (int i=0; i < shapes.size(); ++i)
			if (!shape || shapes[i]->boundingRect().right() > shape->boundingRect().right())
				shape = shapes[i];

		return shape;
	}

	NodeGraphicsItem::Shape *  NodeGraphicsItem::topMostShape() const
	{
		NodeGraphicsItem::Shape * shape = 0;
		for (int i=0; i < shapes.size(); ++i)
			if (!shape || shapes[i]->boundingRect().top() < shape->boundingRect().top())
				shape = shapes[i];

		return shape;
	}

	NodeGraphicsItem::Shape *  NodeGraphicsItem::bottomMostShape() const
	{
		NodeGraphicsItem::Shape * shape = 0;
		for (int i=0; i < shapes.size(); ++i)
			if (!shape || shapes[i]->boundingRect().bottom() > shape->boundingRect().bottom())
				shape = shapes[i];

		return shape;
	}

	QList<Tinkercell::ControlPoint*> NodeGraphicsItem::allControlPoints() const
	{
	    QList<Tinkercell::ControlPoint*> list;

        for (int i=0; i < controlPoints.size(); ++i)
            list << controlPoints[i];

        for (int i=0; i < boundaryControlPoints.size(); ++i)
            list << boundaryControlPoints[i];

	    return list;
    }

	NodeGraphicsItem* NodeGraphicsItem::cast(QGraphicsItem * q)
	{
		//if (MainWindow::invalidPointers.contains( (void*)q )) return 0;
		
		 if (!q || ToolGraphicsItem::cast(q->topLevelItem())) return 0;
		
		NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(q);
		if (!node)
			node = qgraphicsitem_cast<ArrowHeadItem*>(q);

		return node;
	}
	
	QList<NodeGraphicsItem*> NodeGraphicsItem::cast(const QList<QGraphicsItem*>& list)
	{
		QList<NodeGraphicsItem*> nodes;
		NodeGraphicsItem* q;
		for (int i=0; i < list.size(); ++i)
			if (!MainWindow::invalidPointers.contains( (void*)(list[i]) ) && 
				( (q = qgraphicsitem_cast<NodeGraphicsItem*>(list[i])) || (q = qgraphicsitem_cast<ArrowHeadItem*>(list[i])) )
				)
				nodes << q;
		return nodes;
	}
}

