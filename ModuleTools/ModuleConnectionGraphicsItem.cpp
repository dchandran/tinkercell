/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 A special ConnectionGraphicsItem for connecting modules

****************************************************************************/

#include "ConsoleWindow.h"
#include "ModuleConnectionGraphicsItem.h"

namespace Tinkercell
{

	QString ModuleConnectionGraphicsItem::CLASSNAME = QString("ModuleConnectionGraphicsItem");
	QString ModuleLinkerItem::CLASSNAME = QString("ModuleLinkerItem");

	bool ModuleConnectionGraphicsItem::isModuleConnection(ConnectionGraphicsItem* connection)
	{
		return (connection &&
				connection->className == ModuleConnectionGraphicsItem::CLASSNAME &&
				connection->data(0).toBool());
	}

	bool ModuleLinkerItem::isModuleLinker(NodeGraphicsItem* node)
	{
		return (node &&
				node->className == ModuleLinkerItem::CLASSNAME &&
				node->data(0).toBool());
	}

	ModuleLinkerItem::ModuleLinkerItem(NodeGraphicsItem * mod, QGraphicsItem * parent, TextGraphicsItem * text) :
		NodeGraphicsItem(parent)
	{
		className = ModuleLinkerItem::CLASSNAME;

		QString appDir = QCoreApplication::applicationDirPath();
		NodeGraphicsReader reader;
		reader.readXml(this,appDir + QString("/OtherItems/moduleLinker.xml"));
		normalize();
		setWidth = 150.0;
		setToolTip(QString("Module interface"));
		module = mod;
		textItem = text;
		lineItem = new QGraphicsLineItem(this);
		lineItem->setPen(QPen(QColor(255,100,0,255),10.0,Qt::DotLine));
		setPosOnEdge();
		setData(0,true);
	}

	ModuleLinkerItem::ModuleLinkerItem(const ModuleLinkerItem& copy) : NodeGraphicsItem(copy)
	{
		className = ModuleLinkerItem::CLASSNAME;
		setWidth = copy.setWidth;
		module = copy.module;
		textItem = copy.textItem;
		lineItem = 0;
		if (copy.lineItem)
		{
			lineItem = new QGraphicsLineItem(this);
			lineItem->setPen(copy.lineItem->pen());
		}
		setPosOnEdge();
		setData(0,true);
	}

	NodeGraphicsItem * ModuleLinkerItem::clone() const
	{
		return new ModuleLinkerItem(*this);
	}

	/*void ModuleLinkerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
	{
		setPosOnEdge();
		NodeGraphicsItem::paint(painter,option,widget);
	}*/

	void ModuleLinkerItem::setPosOnEdge()
	{
		if (boundaryControlPoints.size() > 0)
		{
			for (int i=0; i < boundaryControlPoints.size(); ++i)
				if (boundaryControlPoints[i])
				{
					if (boundaryControlPoints[i]->scene())
						boundaryControlPoints[i]->scene()->removeItem(boundaryControlPoints[i]);
					delete boundaryControlPoints[i];
				}
			boundaryControlPoints.clear();
		}
		if (module && scene() && itemHandle)
		{
			QPointF p1,p2;
			if (!parentItem())
			{
				//qreal 	w = sceneBoundingRect().width(),
					//	h = sceneBoundingRect().height();

				qreal w = setWidth;
				resetTransform();

				scale(w/sceneBoundingRect().width(),w/sceneBoundingRect().width());
				QPointF scenePos = this->scenePos();
				QRectF rect = module->sceneBoundingRect();
				QRectF nodeRect;
				QPointF point;// = pointOnEdge(rect, scenePos);

				qreal dist = -1.0;
				QGraphicsItem * closest = 0;

				for (int i=0; i < itemHandle->graphicsItems.size(); ++i)
					if (itemHandle->graphicsItems[i] && itemHandle->graphicsItems[i] != this)
					{
						nodeRect = itemHandle->graphicsItems[i]->sceneBoundingRect();
						if (qgraphicsitem_cast<NodeGraphicsItem*>(itemHandle->graphicsItems[i]) &&
							rect.intersects(nodeRect))
							if (closest == 0 ||
								((nodeRect.left() - rect.left()) > 0 && (nodeRect.left() - rect.left()) < dist) ||
								((nodeRect.top() - rect.top()) > 0 && (nodeRect.top() - rect.top()) < dist) ||
								((rect.right() - nodeRect.right()) > 0 && (rect.right() - nodeRect.right()) < dist) ||
								((rect.bottom() - nodeRect.bottom()) > 0 && (rect.bottom() - nodeRect.bottom()) < dist))
								{
									closest = itemHandle->graphicsItems[i];
								}
					}

				if (!closest) return;

				nodeRect = closest->sceneBoundingRect();
				dist = (nodeRect.left() - rect.left());
				point.rx() = rect.left();
				point.ry() = nodeRect.center().y();
				p1 = point;
				p2 = QPointF(nodeRect.left(),point.y());

				if (dist < 0 || dist > (nodeRect.top() - rect.top()))
				{
					dist = (nodeRect.top() - rect.top());
					point.rx() = nodeRect.center().rx();
					point.ry() = rect.top();
					p1 = point;
					p2 = QPointF(point.x(),nodeRect.top());
				}
				if (dist < 0 || dist > (rect.right() - nodeRect.right()))
				{
					dist = (rect.right() - nodeRect.right());
					point.rx() = rect.right();
					point.ry() = nodeRect.center().y();
					p1 = point;
					p2 = QPointF(nodeRect.right(),point.y());
				}
				if (dist < 0 || dist > (rect.bottom() - nodeRect.bottom()))
				{
					dist = (rect.bottom() - nodeRect.bottom());
					point.rx() = nodeRect.center().rx();
					point.ry() = rect.bottom();
					p1 = point;
					p2 = QPointF(point.x(),nodeRect.bottom());
				}

				qreal dx=0, dy=0;
				if ((point.rx() - rect.left())*(point.rx() - rect.left()) < 1)
				{
					dx -= w/2.0;
					rotate(180);
				}
				else
				if ((point.ry() - rect.top())*(point.ry() - rect.top()) < 1)
				{
					dy -= w/2.0;
					rotate(-90);
				}
				else
				if ((point.ry() - rect.bottom())*(point.ry() - rect.bottom()) < 1)
				{
					dy += w/2.0;
					rotate(90);
				}
				else
				{
					dx += w/2.0;
				}

				setPos( point );

				if (textItem)
				{
					textItem->setPos( point + QPointF(dx,dy));
					if (itemHandle)
						textItem->setPlainText(itemHandle->name);
				}
			}
			p1 = this->mapFromScene(p1);
			p2 = this->mapFromScene(p2);
			if (lineItem)
				lineItem->setLine(QLineF(p1,p2));
		}
	}

	/*! Constructor: sets the class name as ModuleConnectionGraphicsItem */
    ModuleConnectionGraphicsItem::ModuleConnectionGraphicsItem(QGraphicsItem * parent ) : ConnectionGraphicsItem(parent)
	{
		className = ModuleConnectionGraphicsItem::CLASSNAME;
		setPen(defaultPen = QPen(QColor(255,100,0,255),2.0));
		command = 0;
		setData(0,true);
	}

	ModuleConnectionGraphicsItem::ModuleConnectionGraphicsItem(const ModuleConnectionGraphicsItem& copy) : ConnectionGraphicsItem(copy)
	{
		className = ModuleConnectionGraphicsItem::CLASSNAME;
		setPen(defaultPen = QPen(QColor(255,100,0,255),2.0));
		command = 0;
		setData(0,true);
	}

	ConnectionGraphicsItem* ModuleConnectionGraphicsItem::clone() const
	{
		return new ModuleConnectionGraphicsItem(*this);
	}

	ModuleConnectionGraphicsItem::~ModuleConnectionGraphicsItem()
	{
		//if (command) delete command;
		command = 0;
	}

	void ModuleConnectionGraphicsItem::adjustEndPoints()
	{
		ControlPoint * firstPoint, *lastPoint, * cp0, * cp1;
		NodeGraphicsItem * node, *parentNode;
		for (int i=0; i < curveSegments.size(); ++i)
		{
			firstPoint = curveSegments[i].first();
			lastPoint = curveSegments[i].last();

			if (curveSegments[i].size() < 4 || !lastPoint || !firstPoint) continue;

			cp0 = curveSegments[i][ 3 ];
			cp1 = curveSegments[i][ curveSegments[i].size()-4 ];

			if (firstPoint != 0 && firstPoint->parentItem() != 0)
			{
				QRectF parentRect = firstPoint->parentItem()->sceneBoundingRect();

				if (firstPoint->isVisible()) firstPoint->setVisible(false);
				QPointF p = parentRect.center();
				node = NodeGraphicsItem::topLevelNodeItem(firstPoint->parentItem());
				parentNode = 0;
				if (node && node->className == ModuleLinkerItem::CLASSNAME)
				{
					parentNode = static_cast<ModuleLinkerItem*>(node)->module;
				}
				if (parentNode)
				{
					if (parentRect.right()  > parentNode->sceneBoundingRect().right())
						p.rx() = parentRect.right();
					else
					if (parentRect.left()  < parentNode->sceneBoundingRect().left())
						p.rx() = parentRect.left();
					else
					if (parentRect.top()  < parentNode->sceneBoundingRect().top())
						p.ry() = parentRect.top();
					else
					if (parentRect.bottom()  > parentNode->sceneBoundingRect().bottom())
						p.ry() = parentRect.bottom();
				}
				else
				if (cp0)
				{
					p = pointOnEdge(node->sceneBoundingRect(),cp0->scenePos(),2.0,true);
				}
				firstPoint->setPos( firstPoint->parentItem()->mapFromScene(p) );
			}

			if (lastPoint != 0 && lastPoint->parentItem() != 0)
			{
				QRectF parentRect = lastPoint->parentItem()->sceneBoundingRect();

				if (lastPoint->isVisible()) lastPoint->setVisible(false);
				QPointF p = parentRect.center();
				node = NodeGraphicsItem::topLevelNodeItem(lastPoint->parentItem());
				parentNode = 0;
				if (node && node->className == ModuleLinkerItem::CLASSNAME)
				{
					parentNode = static_cast<ModuleLinkerItem*>(node)->module;
				}
				if (parentNode)
				{
					if (parentRect.right()  > parentNode->sceneBoundingRect().right())
						p.rx() = parentRect.right();
					else
					if (parentRect.left()  < parentNode->sceneBoundingRect().left())
						p.rx() = parentRect.left();
					else
					if (parentRect.top()  < parentNode->sceneBoundingRect().top())
						p.ry() = parentRect.top();
					else
					if (parentRect.bottom()  > parentNode->sceneBoundingRect().bottom())
						p.ry() = parentRect.bottom();
				}
				else
				if (cp1)
				{
					p = pointOnEdge(node->sceneBoundingRect(),cp1->scenePos(),2.0,true);
				}

				lastPoint->setPos( lastPoint->parentItem()->mapFromScene(p) );
			}

		}
	}

}
