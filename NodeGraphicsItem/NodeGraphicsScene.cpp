/****************************************************************************
 **
 ** Copyright (C) 2008 Deepak Chandran
 ** Contact: Deepak Chandran (dchandran1@gmail.com)
 **
 ** see COPYRIGHT.TXT
 **
 ****************************************************************************/

#include "NodeGraphicsScene.h"
#include <typeinfo>

namespace Tinkercell
{
	void setHandle(QGraphicsItem* item, ItemHandle * handle)
	{
	}
}

namespace NodeImageDesigner
{

/*! \brief Constructor - sets scene size, makes new current shape*/
DrawScene::DrawScene(QWidget *parent) : QGraphicsScene(parent)
{
	mode = -2;
	setSceneRect(0,0,500,500);
	selectedItem = 0;

	currentShape = new NodeGraphicsItem::Shape();

	addItem(&node);
}
/*! \brief Destructor - delete current shape*/
DrawScene::~DrawScene()
{
	if (currentShape && !currentShape->nodeItem)
		delete currentShape;
	node.clear();
	removeItem(&node);
	clear();
}

/*! \brief If mouse is clicked on a control point, then select it. If clicked on a shape, color it.*/
void DrawScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if (mouseEvent->button() != Qt::LeftButton)
		return;

	QPointF point = mouseEvent->scenePos();
	//if (mode == 4) { point.rx() += 20; point.ry() += 30; }
	//if (mode == 5) { point.rx() += 20; }

	if (mode == -1)
	{
		NodeGraphicsItem::ControlPoint * controlPoint = new NodeGraphicsItem::ControlPoint();
		controlPoint->setPos(point);
		addItem(controlPoint);
		node.addControlPoint(controlPoint);
		return;
	}

	selectedItem = itemAt(point);

	if (selectedItem)
	{
		NodeGraphicsItem::Shape tempShape;
		if (typeid(*selectedItem) == typeid(tempShape))
		{
			if (mode == 4)
			{
				colorPt1 = ((NodeGraphicsItem::Shape*)(selectedItem))->mapFromScene(point);

				//selectedItem = 0;
				return;
			}
			else
			if (mode == 5)
			{
				((NodeGraphicsItem::Shape*)(selectedItem))->setPen(QPen(color1,lineWidth));
				selectedItem = 0;
				return;
			}
		}

		NodeGraphicsItem::ControlPoint tempPoint;
		if (typeid(*selectedItem) == typeid(tempPoint))
		{
			if (currentShape && mode >=0 && mode < NodeGraphicsItem::numShapeTypes)
			{
				if (currentShape->controlPoints.size() < 1 ||
					currentShape->controlPoints[currentShape->controlPoints.size()-1] != (NodeGraphicsItem::ControlPoint*)selectedItem)
						currentShape->controlPoints.push_back((NodeGraphicsItem::ControlPoint*)selectedItem);
					else
						currentShape->types.pop_back();

				currentPoints.push_back((NodeGraphicsItem::ControlPoint*)selectedItem);

				((NodeGraphicsItem::ControlPoint*)selectedItem)->setBrush(QBrush(QColor(0,0,255,100)));

				if (currentShape->controlPoints.size() > 0)
				{
					switch (NodeGraphicsItem::ShapeType(mode))
					{
						case NodeGraphicsItem::arc:
							{
								currentShape->types.push_back(NodeGraphicsItem::arc);
								currentShape->parameters.push_back((qreal)arcStart);
								currentShape->parameters.push_back((qreal)arcSpan);
							}
							break;
						case NodeGraphicsItem::line:
							{
								currentShape->types.push_back(NodeGraphicsItem::line);
							}
							break;
						case NodeGraphicsItem::bezier:
							if (currentPoints.size() % 3  == 0)
							{
								currentShape->types.push_back(NodeGraphicsItem::bezier);
							}
							break;
					}
				}
				currentShape->refresh();

				if (currentShape->isClosed())
				{
					for (int i=0; i < currentShape->controlPoints.size(); ++i)
						currentShape->controlPoints[i]->setBrush(QBrush(QColor(0,0,255,10)));

					currentShape->defaultPen = currentShape->pen();
					currentShape->defaultBrush = currentShape->brush();
					node.addShape(currentShape);
					node.refresh();

					currentShape = new NodeGraphicsItem::Shape();

					currentPoints.clear();
				}
				selectedItem = 0;
			}
		}
		else
		{
			selectedItem = 0;
		}
	}
}

/*! \brief if delete pressed, deletes selected control point*/
void DrawScene::keyPressEvent (QKeyEvent * event)
{
	switch (event->key())
	{
		case Qt::Key_Escape:
			{
				for (int i=0; i < node.controlPoints.size(); ++i)
				{
					if (currentShape)
					{
						for (int i=0; i < currentShape->controlPoints.size(); ++i)
							currentShape->controlPoints[i]->setBrush(QBrush(QColor(0,0,255,10)));
						currentShape->controlPoints.clear();
						currentShape->types.clear();
					}
					currentPoints.clear();
				}
			}
			break;
		case Qt::Key_Delete:
			if (selectedItem)
			{
				NodeGraphicsItem::ControlPoint tempPoint;
				if (typeid(*selectedItem) == typeid(tempPoint))
				{
					NodeGraphicsItem::ControlPoint* ptr = (NodeGraphicsItem::ControlPoint*)selectedItem;
					node.removeControlPoint(ptr);
					removeItem(ptr);
					selectedItem = 0;
				}
			}
			break;
		default:
			event->ignore();
			break;

	//if (event->modifiers() == Qt::ShiftModifier)
	//Qt::ControlModifier
	//Qt::AltModifier

	}
}

/*! \brief move selected control point. If control is held, adjusts the point */
void DrawScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	if (mode == -2 && selectedItem && mouseEvent->buttons() == Qt::LeftButton)
	{
		QPointF point1 = mouseEvent->scenePos(), point0 = mouseEvent->lastScenePos();

		QPointF change = QPointF(point1.x()-point0.x(),point1.y()-point0.y());

		selectedItem->moveBy(change.x(),change.y());

		if (mouseEvent->modifiers() == Qt::ControlModifier && node.controlPoints.size() > 0)
		{
			QPointF closest = selectedItem->pos();
			for (int i=0; i < node.controlPoints.size(); ++i)
			{
				if (node.controlPoints[i] != selectedItem)
				{
					QPointF p = node.controlPoints[i]->pos();

					if (abs((long)(p.x() - selectedItem->x())) < abs((long)(p.y() - selectedItem->y())) &&
					    (selectedItem->pos() == closest || abs((long)(p.x() - selectedItem->x())) < abs((long)(closest.x() - selectedItem->x()))) &&
						abs((long)(p.x() - selectedItem->x())) < 10)
						closest.setX(p.x());
					else
						if ((selectedItem->pos() == closest || abs((long)(p.y() - selectedItem->y())) < abs((long)(closest.y() - selectedItem->y())))
							&& abs((long)(p.y() - selectedItem->y())) < 10)
							closest.setY(p.y());
				}
			}
			selectedItem->setPos(closest);
		}

		node.refresh();
	}
}

/*! \brief un-select the selected control point*/
void DrawScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	//selectedItem = 0;
	NodeGraphicsItem::Shape tempShape;
	if (selectedItem && mode == 4 && typeid(*selectedItem) == typeid(tempShape))
	{
		QPointF point = mouseEvent->scenePos();

		colorPt2 = ((NodeGraphicsItem::Shape*)(selectedItem))->mapFromScene(point);

		if (fillType == 0)
		{
			QLinearGradient gradient(colorPt1,colorPt2);
			gradient.setColorAt(0,color1);
			gradient.setColorAt(1,color2);
			//qDebug() << color1.alpha();
			((NodeGraphicsItem::Shape*)(selectedItem))->setBrush(gradient);
			((NodeGraphicsItem::Shape*)(selectedItem))->gradientPoints.first = colorPt1;
			((NodeGraphicsItem::Shape*)(selectedItem))->gradientPoints.second = colorPt2;
		}
		else
		if (fillType == 1)
		{
			QRadialGradient gradient(colorPt1,sqrt( (colorPt2.y()-colorPt1.y())*(colorPt2.y()-colorPt1.y()) +
													(colorPt2.x()-colorPt1.x())*(colorPt2.x()-colorPt1.x())));
			gradient.setColorAt(0,color1);
			gradient.setColorAt(1,color2);
			((NodeGraphicsItem::Shape*)(selectedItem))->setBrush(gradient);
			((NodeGraphicsItem::Shape*)(selectedItem))->gradientPoints.first = colorPt1;
			((NodeGraphicsItem::Shape*)(selectedItem))->gradientPoints.second = colorPt2;
		}
		else
		{
			((NodeGraphicsItem::Shape*)(selectedItem))->setBrush(QBrush(color1));
		}
	}
}

}
