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
#include "ControlPoint.h"
#include "ItemHandle.h"

namespace Tinkercell
{
	ItemHandle * ControlPoint::handle() const
	{
		return 0;
	}

	void ControlPoint::setHandle(ItemHandle *)
	{
		
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
		setVisible(copy.isVisible());

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

}
