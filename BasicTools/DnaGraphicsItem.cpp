/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

A special ConnectionGraphicsItem that draws a DNA-like connection

****************************************************************************/

#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "DnaGraphicsItem.h"

namespace Tinkercell
{
	/*! \brief used for checking type before static casts */
	QString DnaGraphicsItem::CLASSNAME("DnaGraphicsItem");

	/*! \brief paint method. Call's parent's paint after setting antialiasing to true*/
	void DnaGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
	{
		//painter->setPen(nucleotidesPen);
		//painter->drawPath(nucleotidesPath);
		ConnectionGraphicsItem::paint(painter,option,widget);
	}

	/*! \brief refresh the path if any controlpoints have moved
	* \param void
	* \return void*/
	void DnaGraphicsItem::refresh()
	{
		setPos(0,0);
		adjustEndPoints();
		qreal z = zValue();

		if (centerRegionItem && centerRegionItem->parentItem() == 0 && centerPoint())
		{
			if (centerRegionItem->scene() != scene())
			{
				if (centerRegionItem->scene())
					centerRegionItem->scene()->removeItem(centerRegionItem);
				if (scene())
				{
					scene()->addItem(centerRegionItem);					
					centerRegionItem->setZValue(z + 0.001);
				}
			}
			centerRegionItem->setPos( centerPoint()->scenePos() );
		}

		QPainterPath dnapath, nucpath;

		QPointF pos1,pos2,pos3;
		for (int i=0; i < pathVectors.size(); ++i)
		{
			QPainterPath path;
			if (pathVectors[i].size() > 0 && pathVectors[i][0])
			{
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

					pos1 =  pathVectors[i][0]->scenePos();
					dnapath.moveTo(pos1);
					qreal h = defaultPen.widthF()*8, dl = defaultPen.widthF()*16;
					for (qreal l=0; l < path.length(); l += dl)
					{
						qreal perc1 = path.percentAtLength(l), perc2 = path.percentAtLength(l+dl/2), perc3 = path.percentAtLength(l+dl);
						qreal slp = path.slopeAtPercent(perc2);
						QPointF pt1 = path.pointAtPercent(perc1), pt2 = path.pointAtPercent(perc2), pt3 = path.pointAtPercent(perc3);
						qreal dx, dy;
						if (slp != 0)
						{
							slp = -1/slp;
							dx = sqrt( h*h/( 1 + slp*slp) );
							dy = slp * dx;
						}
						else
						{
							dx = h;
							dy = 0;
						}
						QPointF midpt1( pt2.x() - dx, pt2.y() - dy );
						dnapath.cubicTo(pt1,midpt1,pt3);
					}
					dnapath.moveTo(pos1);
					for (qreal l=0; l < path.length(); l += dl)
					{
						qreal perc1 = path.percentAtLength(l), perc2 = path.percentAtLength(l+dl/2), perc3 = path.percentAtLength(l+dl);
						qreal slp = path.slopeAtPercent(perc2);
						QPointF pt1 = path.pointAtPercent(perc1), pt2 = path.pointAtPercent(perc2), pt3 = path.pointAtPercent(perc3);
						qreal dx, dy;
						if (slp != 0)
						{
							slp = -1/slp;
							dx = sqrt( h*h/( 1 + slp*slp) );
							dy = slp * dx;
						}
						else
						{
							dx = h;
							dy = 0;
						}
						QPointF midpt1( pt2.x() + dx, pt2.y() + dy );
						dnapath.cubicTo(pt1,midpt1,pt3);
					}

					nucpath.moveTo(pos1);
					for (qreal l=0; l < path.length(); l += dl)
					{
						qreal perc1 = path.percentAtLength(l+dl/4), perc2 = path.percentAtLength(l+dl/2), perc3 = path.percentAtLength(l+3*dl/4);
						qreal slp = path.slopeAtPercent(perc2);
						QPointF pt1 = path.pointAtPercent(perc1), pt2 = path.pointAtPercent(perc2), pt3 = path.pointAtPercent(perc3);
						qreal dx, dy;
						if (slp != 0)
						{
							slp = -1/slp;
							dx = sqrt( h*h/( 1 + slp*slp) );
							dy = slp * dx;
						}
						else
						{
							dx = h;
							dy = 0;
						}
						nucpath.moveTo(QPointF(pt1.x() - dx/5, pt1.y() - dy/5));
						nucpath.lineTo(QPointF(pt1.x() + dx/5, pt1.y() + dy/5));
						nucpath.moveTo(QPointF(pt2.x() - dx/3, pt2.y() - dy/3));
						nucpath.lineTo(QPointF(pt2.x() + dx/3, pt2.y() + dy/3));
						nucpath.moveTo(QPointF(pt3.x() - dx/5, pt3.y() - dy/5));
						nucpath.lineTo(QPointF(pt3.x() + dx/5, pt3.y() + dy/5));
					}
			}
		}

		QPainterPathStroker stroker;
		stroker.setJoinStyle(Qt::RoundJoin);
		stroker.setCapStyle(Qt::RoundCap);
		setPath(stroker.createStroke(dnapath));

		stroker.setWidth(20);
		this->pathShape = stroker.createStroke(dnapath);

		refreshBoundaryPath();

		if (nucleotidesPath)
		{
			nucleotidesPath->setZValue(z - 0.02);
			nucleotidesPath->setPath( nucpath );
		}
	}

	/*! Constructor: does nothing */
	DnaGraphicsItem::DnaGraphicsItem(QGraphicsItem * parent) : ConnectionGraphicsItem(parent)
	{
		className = DnaGraphicsItem::CLASSNAME;
		height = 15;
		width = 60;
		nucleotidesPath = new QGraphicsPathItem(this);
		nucleotidesPath->setPen(QPen(QColor(255,50,50),2));
		defaultPen = QPen(QColor(100,100,255,255),2);
		defaultPen.setJoinStyle(Qt::RoundJoin);
		setPen(defaultPen);
	}
	/*! Copy Constructor */
	DnaGraphicsItem::DnaGraphicsItem(const DnaGraphicsItem& copy): ConnectionGraphicsItem( copy )
	{
		height = copy.height;
		width = copy.width;
		nucleotidesPath = new QGraphicsPathItem(this);
		if (copy.nucleotidesPath != 0)
			nucleotidesPath->setPen(copy.nucleotidesPath->pen());
		else
			nucleotidesPath->setPen(QPen(QColor(255,50,50),2));
	}

	/*! \brief make a copy of this item */
	ConnectionGraphicsItem* DnaGraphicsItem::clone()
	{
		return new DnaGraphicsItem(*this);
	}

	/*! Copy operator */
	DnaGraphicsItem& DnaGraphicsItem::operator = (const DnaGraphicsItem& copy)
	{
		ConnectionGraphicsItem::operator = (copy);
		height = copy.height;
		width = copy.width;
		if (copy.nucleotidesPath != 0)
			nucleotidesPath->setPen(copy.nucleotidesPath->pen());
		else
			nucleotidesPath->setPen(QPen(QColor(255,50,50),2));
		return *this;
	}

}
