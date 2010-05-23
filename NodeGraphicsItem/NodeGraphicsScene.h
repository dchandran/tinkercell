/****************************************************************************
 **
 ** Copyright (C) 2008 Deepak Chandran
 ** Contact: Deepak Chandran (dchandran1@gmail.com)
 **
 ** see COPYRIGHT.TXT
 **
 ****************************************************************************/

#ifndef DRAWPANEL_H
#define DRAWPANEL_H

#include <QPainter>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <math.h>
#include "NodeGraphicsItem.h"

#ifdef Q_WS_WIN
#define MY_IMPORT __declspec(dllimport)
#else
#define MY_IMPORT
#endif

namespace NodeImageDesigner
{

typedef Tinkercell::NodeGraphicsItem NodeGraphicsItem;

/*! \brief Scene where a single NodeGraphicsItem can be drawn. This scene contains the node and its control points.
 */
class DrawScene : public QGraphicsScene
{
	Q_OBJECT
	
public:
	/*! Constructor */
    DrawScene(QWidget *parent = 0);
	/*! \brief The node that will be drawn on the screen */
	NodeGraphicsItem node;	
	/*! \brief the start angle for adding a new arc to the NodeGraphicsItem*/
	int arcStart;
	/*! \brief the span angle for adding a new arc to the NodeGraphicsItem*/
	int arcSpan;
	/*! \brief the roundedness of a new rect added to the NodeGraphicsItem*/
	int rectRoundedness;
	/*! \brief the line width for adding a new lines, arcs, or curves to the NodeGraphicsItem*/
	int lineWidth;	
	/*! \brief the start color for the gradient color*/
	QColor color1;
	/*! \brief the end color for the gradient color*/
	QColor color2;	
	/*! \brief the start point for the gradient color*/
	QPointF colorPt1;
	/*! \brief the end color for the gradient color*/
	QPointF colorPt2;
	/*! \brief the control points that are selected*/
	QVector<NodeGraphicsItem::ControlPoint*> currentPoints;
	/*! \brief indicates whether the current shape is a line, arc, or curve*/
	int mode;
	/*! \brief indicates the type of gradient fill to use - none, linear, radial*/
	int fillType;

	~DrawScene();
	
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void keyPressEvent (QKeyEvent * event); 

public:
	/*! \brief the current shape that is being constructed*/
	NodeGraphicsItem::Shape * currentShape;
	/*! \brief the item that is clicked on*/
	QGraphicsItem * selectedItem;
};

}
#endif
