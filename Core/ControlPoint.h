/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

One of the main classes in Tinkercell.

ControlPoint is a drawable item that is used by NodeGraphicsItem and ConnectionGraphicsItem
to draw movable points.

****************************************************************************/

#ifndef TINKERCELL_CONTROLPOINTGRAPHICSITEM_H
#define TINKERCELL_CONTROLPOINTGRAPHICSITEM_H

#include <math.h>
#include <QObject>
#include <QtDebug>
#include <QString>
#include <QColor>
#include <QPoint>
#include <QPainter>
#include <QGraphicsPolygonItem>
#include <QAbstractGraphicsShapeItem>
#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QTransform>
#include <QUndoCommand>
#include <QTextCursor>

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{
	class ItemHandle;
	
	/*! \brief A simple circle or square that is used for changing specific locations
	\ingroup core*/
	class MY_EXPORT ControlPoint : public QAbstractGraphicsShapeItem
	{
	public:
		/*! \brief permanent brush for this control point*/
		QBrush defaultBrush;
		/*! \brief permanent pen for this control point*/
		QPen defaultPen;
		/*! \brief default size for this item*/
		QSizeF defaultSize;
		/*! \brief x position */
		virtual qreal x() { return scenePos().x(); }
		/*! \brief y position*/
		virtual qreal y() { return scenePos().y(); }
		/*! \brief Constructor: Setup colors and z value */
		ControlPoint(QGraphicsItem * parent = 0);
		/*! \brief copy constructor */
		ControlPoint(const ControlPoint& copy);
		/*! \brief paint method. Call's parent's paint after setting antialiasing to true*/
		//virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option=new QStyleOptionGraphicsItem() ,QWidget *widget=0);
		/*! \brief for enabling dynamic_cast*/
		enum { Type = UserType + 1 };
		/*! \brief for enabling dynamic_cast*/
		virtual int type() const
		{
			// Enable the use of dynamic_cast with this item.
			return Type;
		}
		/*! \brief Gets the control point item from one of its child items*/
		static ControlPoint * asControlPoint(QGraphicsItem* item);
		/*! \brief side effect when moved. always call this after moving*/
		virtual void sideEffect();
		/*! \brief make a copy of this control point*/
		virtual ControlPoint* clone() const;
		/*! \brief type of shape to paint. */
		enum ShapeType { circle, square, triangle };
		/*! \brief type of shape to paint. */
		ShapeType shapeType;
		/*! \brief paint method. */
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option=new QStyleOptionGraphicsItem() ,QWidget *widget=0);
		/*! \brief bounding rect method. */
		virtual QRectF boundingRect() const;
		/*! \brief set size rect. */
		virtual void setRect(const QRectF&);
		/*! \brief get size rect. */
		virtual QRectF rect() const;
		/*! \brief get the handle of this control point, usually 0 or the parent's handle */
		virtual ItemHandle * handle() const;
		/*! \brief set the handle of this control point, usually sets parent's handle or does nothing*/
		virtual void setHandle(ItemHandle *);
	protected:
		QRectF bounds;
	};
}
#endif
