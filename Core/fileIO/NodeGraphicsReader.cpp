/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines an xml reader that reads a NodeGraphicsItem file

****************************************************************************/

#include "NodeGraphicsReader.h"
#include <QtDebug>

namespace Tinkercell
{
	/*! \brief Reads up to the next start node
	* \return Token Typer*/ 
	QXmlStreamReader::TokenType NodeGraphicsReader::readNext()
	{
		QXmlStreamReader::readNext();	
		while (!atEnd() && !isStartElement() && !isEndElement()) QXmlStreamReader::readNext();
		return tokenType();
	}
	/*! \brief Reads an NodeGraphicsItem from an XML file using the IO device provided and adds the information to the provided NodeGraphicsItem
	* \param NodeGraphicsItem pointer that will be read into from XML
	* \param QIODevice to use
	* \return void*/ 
	bool NodeGraphicsReader::readXml(NodeGraphicsItem * node, const QString& fileName)
	{
		QFile file (fileName);

		if (!file.open(QFile::ReadOnly | QFile::Text)) 
		{
			return false;
		}

		node->name = fileName;
		readNodeGraphics(node,&file);

		return true;
	}
	/*! \brief Reads an NodeGraphicsItem from an XML file using the IO device provided and adds the information to the provided NodeGraphicsItem
	* \param NodeGraphicsItem pointer that will be read into from XML
	* \param QIODevice to use
	* \return void*/ 
	void NodeGraphicsReader::readNodeGraphics(NodeGraphicsItem * node, QIODevice * device)
	{
		if (!device || !node) return;

		node->clear();
		QXmlStreamAttributes vec;

		if (this->device() != device)
			setDevice(device);

		bool ok;
		qreal x = 0.0, y = 0.0, width = 0.0, height = 0.0, rx = 0.0, ry = 0.0, arcStart = 0.0, arcStop = 0.0;
		QString fill, stroke, s;
		QBrush brush;
		QPen pen;
		
		while (!atEnd() && !(isEndElement() && name() == "listOfRenderInformation"))
		{
			if (isStartElement())
			{
				if (name() == QObject::tr("listOfColorDefinitions"))
				{
					readBrushes();
				}
				else
				if (name() == QObject::tr("listOfGradientDefinitions"))
				{
					readBrushes();
				}
				else
				if (name() == QObject::tr("polygon"))
				{
					vec = attributes();
					NodeGraphicsItem::Shape * shape = readPolygon(node);
					node->addShape(shape);
					for (int i=0; i < vec.size(); ++i)
					{
						if (vec.at(i).name().toString() == QObject::tr("fill"))
						{
							fill = vec.at(i).value().toString();
							if (brushes.contains(fill))
							{
								shape->defaultBrush = brushes[fill].brush;
								shape->gradientPoints.first = brushes[fill].start;
								shape->gradientPoints.second = brushes[fill].end;
								shape->setBrush(shape->defaultBrush);
							}
						}
						if (vec.at(i).name().toString() == QObject::tr("stroke"))
						{
							pen.setColor(QColor(vec.at(i).value().toString()));
						}
						if (vec.at(i).name().toString() == QObject::tr("stroke-width"))
						{
							width = vec.at(i).value().toString().toDouble(&ok);
							if (ok)
                            {
								pen.setWidthF(width);
                                if (width == 0.0)
                                {
									QColor color = pen.color();
									color.setAlphaF(0.0);
									pen.setColor(color);
								}
							}
						}
                        if (vec.at(i).name().toString() == QObject::tr("stroke-alpha"))
						{
							width = vec.at(i).value().toString().toDouble(&ok);
							if (ok)
                            {
								QColor color = pen.color();
								color.setAlphaF(width);
								pen.setColor(color);
							}
						}
					}
					shape->defaultPen = pen;
					shape->setPen(pen);
				}
				else
				if (name() == QObject::tr("rectangle"))
				{
					vec = attributes();
					QPointF p1, p2;
					NodeGraphicsItem::Shape * shape = new NodeGraphicsItem::Shape;
					shape->types << NodeGraphicsItem::rectangle;
					node->addShape(shape);
					for (int i=0; i < vec.size(); ++i)
					{
						if (vec.at(i).name().toString() == QObject::tr("fill"))
						{
							fill = vec.at(i).value().toString();
							if (brushes.contains(fill))
							{
								shape->defaultBrush = brushes[fill].brush;
								shape->gradientPoints.first = brushes[fill].start;
								shape->gradientPoints.second = brushes[fill].end;
								shape->setBrush(shape->defaultBrush);
							}
						}
						if (vec.at(i).name().toString() == QObject::tr("stroke"))
						{
							pen.setColor(QColor(vec.at(i).value().toString()));
						}
						if (vec.at(i).name().toString() == QObject::tr("stroke-width"))
						{
							s = vec.at(i).value().toString();
							s.remove("%");
							width = s.toDouble(&ok);
							if (ok)
								pen.setWidthF(width);
						}
						if (vec.at(i).name().toString() == QObject::tr("x"))
						{
							s = vec.at(i).value().toString();
							s.remove("%");
							x = s.toDouble(&ok);
							if (ok)
								p1.rx() = x;
						}
						if (vec.at(i).name().toString() == QObject::tr("y"))
						{
							s = vec.at(i).value().toString();
							s.remove("%");
							y = s.toDouble(&ok);
							if (ok)
								p1.ry() = y;
						}
						if (vec.at(i).name().toString() == QObject::tr("width"))
						{
							s = vec.at(i).value().toString();
							s.remove("%");
							width = s.toDouble(&ok);
							if (ok)
								p2.rx() = width;
						}
						if (vec.at(i).name().toString() == QObject::tr("height"))
						{
							s = vec.at(i).value().toString();
							s.remove("%");
							height = s.toDouble(&ok);
							if (ok)
								p2.ry() = height;
						}
						if (vec.at(i).name().toString() == QObject::tr("rx"))
						{
							s = vec.at(i).value().toString();
							s.remove("%");
							rx = s.toDouble(&ok);
							if (!ok)
								rx = 0.0;
						}
						if (vec.at(i).name().toString() == QObject::tr("ry"))
						{
							s = vec.at(i).value().toString();
							s.remove("%");
							rx = s.toDouble(&ok);
							if (!ok)
								rx = 0.0;
						}
					}
					shape->defaultPen = pen;
					shape->setPen(pen);
					p2.rx() += p1.x();
					p2.ry() += p1.y();
					shape->controlPoints << getControlPoint(node,p1) << getControlPoint(node,p2);
					shape->parameters << rx;
				}
				else
				if (name() == QObject::tr("ellipse"))
				{
					vec = attributes();
					QPointF p1, p2;
					NodeGraphicsItem::Shape * shape = new NodeGraphicsItem::Shape;
					shape->types << NodeGraphicsItem::arc;
					node->addShape(shape);
					for (int i=0; i < vec.size(); ++i)
					{
						if (vec.at(i).name().toString() == QObject::tr("fill"))
						{
							fill = vec.at(i).value().toString();
							if (brushes.contains(fill))
							{
								shape->defaultBrush = brushes[fill].brush;
								shape->gradientPoints.first = brushes[fill].start;
								shape->gradientPoints.second = brushes[fill].end;
								shape->setBrush(shape->defaultBrush);
							}
						}
						if (vec.at(i).name().toString() == QObject::tr("stroke"))
						{
							pen.setColor(QColor(vec.at(i).value().toString()));
						}
						if (vec.at(i).name().toString() == QObject::tr("stroke-width"))
						{
							s = vec.at(i).value().toString();
							s.remove("%");
							width = s.toDouble(&ok);
							if (ok)
								pen.setWidthF(width);
						}
						if (vec.at(i).name().toString() == QObject::tr("cx"))
						{
							s = vec.at(i).value().toString();
							s.remove("%");
							x = s.toDouble(&ok);
							if (ok)
								p1.rx() = x;
						}
						if (vec.at(i).name().toString() == QObject::tr("cy"))
						{
							s = vec.at(i).value().toString();
							s.remove("%");
							y = s.toDouble(&ok);
							if (ok)
								p1.ry() = y;
						}
						if (vec.at(i).name().toString() == QObject::tr("rx"))
						{
							s = vec.at(i).value().toString();
							s.remove("%");
							rx = s.toDouble(&ok);
							if (!ok)
								ry = 0.0;
						}
						if (vec.at(i).name().toString() == QObject::tr("ry"))
						{
							s = vec.at(i).value().toString();
							s.remove("%");
							ry = s.toDouble(&ok);
							if (!ok)
								ry = 0.0;
						}
						if (vec.at(i).name().toString() == QObject::tr("angleStart"))
						{
							arcStart = vec.at(i).value().toString().toDouble(&ok);
							if (!ok)
								arcStart = 0.0;
						}
						if (vec.at(i).name().toString() == QObject::tr("angleEnd"))
						{
							arcStop = vec.at(i).value().toString().toDouble(&ok);
							if (!ok)
								arcStop = 0.0;
						}
					}
					shape->defaultPen = pen;
					shape->setPen(pen);
					p1.rx() = p1.x() - rx;
					p1.ry() = p1.y() - ry;
					p2.rx() = p1.x() + rx + rx;
					p2.ry() = p1.y() + ry + ry;
					shape->controlPoints << getControlPoint(node,p1) << getControlPoint(node,p2);
					shape->parameters << arcStart << arcStop;
				}
				else
				if (name() == QObject::tr("style"))
				{
					vec = attributes();
					for (int i=0; i < vec.size(); ++i)
					{
						if (vec.at(i).name().toString() == QObject::tr("idList"))
							node->name = vec.at(i).value().toString();
						else
						if (vec.at(i).name().toString() == QObject::tr("group"))
							node->groupID = vec.at(i).value().toString();
						else
						if (vec.at(i).name().toString() == QObject::tr("width"))
						{
							width = vec.at(i).value().toString().toDouble(&ok);
							if (!ok)
								width = 100.0;
							node->defaultSize.setWidth(width);
						}
						else
						if (vec.at(i).name().toString() == QObject::tr("height"))
						{
							height = vec.at(i).value().toString().toDouble(&ok);
							if (!ok)
								height = 100.0;
							node->defaultSize.setHeight(height);
						}
					}
				}
			}
			readNext();
		}
	}

	/*! \brief Reads a shape into an NodeGraphicsItem from an XML file
	* \param NodeGraphicsItem pointer to write as XML
	* \param index of shape in NodeGraphicsItem's shape vector
	* \return void*/
	NodeGraphicsItem::Shape * NodeGraphicsReader::readPolygon(NodeGraphicsItem * node)
	{
		if (node)
		{
			NodeGraphicsItem::Shape * shape = new NodeGraphicsItem::Shape;
			
			QXmlStreamAttributes vec;

			bool ok;
			bool hasControl1 = false, hasControl2 = false;
			qreal x;
			QPointF start, end, control1, control2;
			NodeGraphicsItem::ControlPoint * point;
			QString s;

			while (!atEnd() && !(isEndElement() && name() == QObject::tr("listOfCurveSegments")))
			{
				if (isStartElement() && name() == QObject::tr("curveSegment"))
				{
					control1 = control2 = start = end = QPointF();
					hasControl1 = hasControl2 = false;
					while (!atEnd() && !(isEndElement() && name() == QObject::tr("curveSegment")))
					{
						if (isStartElement())
						{
							if (name() == QObject::tr("start"))
							{
								vec = attributes();
								for (int i=0; i < vec.size(); ++i)
								{
									if (vec.at(i).name().toString() == QObject::tr("x"))
									{
										s = vec.at(i).value().toString();
										s.remove("%");
										x = s.toDouble(&ok);
										if (ok)
											start.rx() = x;
									}
									if (vec.at(i).name().toString() == QObject::tr("y"))
									{
										s = vec.at(i).value().toString();
										s.remove("%");
										x = s.toDouble(&ok);
										if (ok)
											start.ry() = x;
									}
								}
							}
							if (name() == QObject::tr("end"))
							{
								vec = attributes();
								for (int i=0; i < vec.size(); ++i)
								{
									if (vec.at(i).name().toString() == QObject::tr("x"))
									{
										s = vec.at(i).value().toString();
										s.remove("%");
										x = s.toDouble(&ok);
										if (ok)
											end.rx() = x;
									}
									if (vec.at(i).name().toString() == QObject::tr("y"))
									{
										s = vec.at(i).value().toString();
										s.remove("%");
										x = s.toDouble(&ok);
										if (ok)
											end.ry() = x;
									}
								}
							}
							if (name() == QObject::tr("basePoint1"))
							{
								hasControl1 = true;
								vec = attributes();
								for (int i=0; i < vec.size(); ++i)
								{
									if (vec.at(i).name().toString() == QObject::tr("x"))
									{
										s = vec.at(i).value().toString();
										s.remove("%");
										x = s.toDouble(&ok);
										if (ok)
											control1.rx() = x;
									}
									if (vec.at(i).name().toString() == QObject::tr("y"))
									{
										s = vec.at(i).value().toString();
										s.remove("%");
										x = s.toDouble(&ok);
										if (ok)
											control1.ry() = x;
									}
								}
							}
							if (name() == QObject::tr("basePoint2"))
							{
								hasControl2 = true;
								vec = attributes();
								for (int i=0; i < vec.size(); ++i)
								{
									if (vec.at(i).name().toString() == QObject::tr("x"))
									{
										s = vec.at(i).value().toString();
										s.remove("%");
										x = s.toDouble(&ok);
										if (ok)
											control2.rx() = x;
									}
									if (vec.at(i).name().toString() == QObject::tr("y"))
									{
										s = vec.at(i).value().toString();
										s.remove("%");
										x = s.toDouble(&ok);
										if (ok)
											control2.ry() = x;
									}
								}
							}
						}
						readNext();
					}
					
					if (shape->controlPoints.isEmpty())
					{
						point = getControlPoint(node,start);
						if (point)
							shape->controlPoints << point;
					}
					if (hasControl1 && hasControl2)
					{
						point = getControlPoint(node,control1);
						if (point)
							shape->controlPoints << point;
						point = getControlPoint(node,control2);
						if (point)
							shape->controlPoints << point;
						shape->types << NodeGraphicsItem::bezier;
					}
					else
					{
						shape->types << NodeGraphicsItem::line;
					}
					point = getControlPoint(node,end);
					if (point)
						shape->controlPoints << point;
				}
				
				readNext();
			}
			return shape;
		}
		return 0;
	}

	NodeGraphicsItem::ControlPoint * NodeGraphicsReader::getControlPoint(NodeGraphicsItem * node, const QPointF& p)
	{
		if (!node) return 0;

		for (int i=0; i < node->controlPoints.size(); ++i)
			if (node->controlPoints[i] && node->controlPoints[i]->pos() == p)
				return node->controlPoints[i];
		
		NodeGraphicsItem::ControlPoint * newPoint = new NodeGraphicsItem::ControlPoint(node);
		node->controlPoints << newPoint;
		newPoint->setPos(p);
		return newPoint;
	}

	void NodeGraphicsReader::readBrushes()
	{
		QXmlStreamAttributes vec;
		QString id;
		QGradientStops stops;
		bool ok;
		bool radialGradient;
		QString s;

		while (!atEnd() && 
				!(isEndElement() && 
					(name() == QObject::tr("listOfColorDefinitions") || name() == QObject::tr("listOfGradientDefinitions"))))
		{
			if (isStartElement())
			{
				if (name() == QObject::tr("colorDefinition"))
				{
					QColor color;
					int alpha = 255;
					BrushStruct brushStruct;
					vec = attributes();
					id = QString();
					for (int i=0; i < vec.size(); ++i)
					{
						if (vec.at(i).name().toString() == QObject::tr("id"))
						{
							id = vec.at(i).value().toString();
						}
						if (vec.at(i).name().toString() == QObject::tr("value"))
						{
							color = QColor(vec.at(i).value().toString());
						}
						if (vec.at(i).name().toString() == QObject::tr("alpha"))
						{
							alpha = vec.at(i).value().toString().toInt(&ok);
							if (!ok)
								alpha = 255;
						}
					}
					if (!id.isEmpty() && !id.isNull())
					{
						color.setAlpha(alpha);
						brushStruct.brush = QBrush(color);
						brushes[id] = brushStruct;
					}
				}
				if (name() == QObject::tr("linearGradient") || name() == QObject::tr("radialGradient"))
				{
					BrushStruct brushStruct;
					radialGradient = (name() == QObject::tr("radialGradient"));
					stops.clear();
					vec = attributes();
					id = QString();
					for (int i=0; i < vec.size(); ++i)
					{
						if (vec.at(i).name().toString() == QObject::tr("id"))
						{
							id = vec.at(i).value().toString();
						}
						if (vec.at(i).name().toString() == QObject::tr("x1"))
						{
							s = vec.at(i).value().toString();
							s.remove("%");
							qreal x1 = s.toDouble(&ok);
							if (ok)
								brushStruct.start.rx() = x1;
						}
						if (vec.at(i).name().toString() == QObject::tr("y1"))
						{
							s = vec.at(i).value().toString();
							s.remove("%");
							qreal y1 = s.toDouble(&ok);
							if (ok)
								brushStruct.start.ry() = y1;
						}
						if (vec.at(i).name().toString() == QObject::tr("x2"))
						{
							s = vec.at(i).value().toString();
							s.remove("%");
							qreal x2 = s.toDouble(&ok);
							if (ok)
								brushStruct.end.rx() = x2;
						}
						if (vec.at(i).name().toString() == QObject::tr("y2"))
						{
							s = vec.at(i).value().toString();
							s.remove("%");
							qreal y2 = s.toDouble(&ok);
							if (ok)
								brushStruct.end.ry() = y2;
						}
					}
					while (!(isEndElement() && 
							(name() == QObject::tr("linearGradient") ||  name() == QObject::tr("radialGradient"))))
					{
						if (isStartElement() && name() == QObject::tr("stop"))
						{
							QColor color;
							qreal offset;
							vec = attributes();					
							for (int i=0; i < vec.size(); ++i)
							{
								if (vec.at(i).name().toString() == QObject::tr("offset"))
								{
									s = vec.at(i).value().toString();
									s.remove("%");
									offset = s.toDouble(&ok);
									if (!ok)
										offset = 0.0;
								}
								if (vec.at(i).name().toString() == QObject::tr("stop-color"))
								{
									color = QColor(vec.at(i).value().toString());
								}
								if (vec.at(i).name().toString() == QObject::tr("stop-alpha"))
								{
									color.setAlphaF(vec.at(i).value().toString().toDouble(&ok));
									if (!ok)
										color.setAlphaF(1.0);
								}
							}
							stops << QGradientStop(offset/100.0,color);
						}
						readNext();
					}

					if (!id.isEmpty() && !id.isNull())
					{
						if (radialGradient)
						{
							QRadialGradient gradient(brushStruct.start,
												sqrt( (brushStruct.end.y()-brushStruct.start.y())*(brushStruct.end.y()-brushStruct.start.y()) + 
												(brushStruct.end.x()-brushStruct.start.x())*(brushStruct.end.x()-brushStruct.start.x())));
							gradient.setStops(stops);
							brushStruct.brush = QBrush(gradient);
						}
						else
						{
							QLinearGradient gradient(brushStruct.start,brushStruct.end);
							gradient.setStops(stops);
							brushStruct.brush = QBrush(gradient);
						}
						brushes[id] = brushStruct;
					}
				}
			}

			readNext();
		}

	}

}
