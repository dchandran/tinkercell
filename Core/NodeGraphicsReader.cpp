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
 bool NodeGraphicsReader::readXml(NodeGraphicsItem * idrawable, const QString& fileName)
 {
	QFile file (fileName);

	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
        return false;
    }

	readNodeGraphics(idrawable,&file);

	idrawable->fileName = fileName;

	return true;
 }
/*! \brief Reads an NodeGraphicsItem from an XML file using the IO device provided and adds the information to the provided NodeGraphicsItem
 * \param NodeGraphicsItem pointer that will be read into from XML
 * \param QIODevice to use
 * \return void*/
void NodeGraphicsReader::readNodeGraphics(NodeGraphicsItem * idrawable, QIODevice * device)
{
	if (!device || !idrawable) return;

	idrawable->clear();

	if (this->device() != device)
		setDevice(device);

	qreal width = 0.0, height = 0.0;
	while (!atEnd() && !(isEndElement() && name() == "PartGraphicsItem"))
	{
		if (isStartElement())
		{
			if (name() == "PartGraphicsItem")
			{
				QXmlStreamAttributes vec = attributes();
				for (int i=0; i < vec.size(); ++i)
				{
					if (vec.at(i).name().toString() == QObject::tr("filename"))
					{
						idrawable->fileName = vec.at(i).value().toString();
					}
					else
					if (vec.at(i).name().toString() == QObject::tr("width"))
					{
						bool b;
						width = vec.at(i).value().toString().toDouble(&b);
						if (!b) width = 0.0;
					}
					else
					if (vec.at(i).name().toString() == QObject::tr("height"))
					{
						bool b;
						height = vec.at(i).value().toString().toDouble(&b);
						if (!b) height = 0.0;
					}
				}
				readNext();
				if (isStartElement() && name() == "ControlPoints")
				{
					readNext();
					while (isStartElement() && name() == "ControlPoint")
					{
						readControlPoint(idrawable);
						readNext();
					}
				}

				readNext();
				if (isStartElement() && name() == "Shapes")
				{
					readNext();
					while (isStartElement() && name() == "Shape")
					{
						readShape(idrawable);
						readNext();
					}
					//qDebug() << idrawable->shapes.size();
				}
			}
			/*else
			{
				qDebug() << "file incorrect";
				return; //file error
			}*/
		}
		readNext();
	}
	if (width > 0.0 && height > 0.0)
	{
		idrawable->defaultSize.setWidth(width);
		idrawable->defaultSize.setHeight(height);
	}
}

/*! \brief Reads a control point into an NodeGraphicsItem from an XML file
 * \param NodeGraphicsItem pointer to write as XML
 * \param index of control point in NodeGraphicsItem's control points' vector
 * \return void*/
void NodeGraphicsReader::readControlPoint(NodeGraphicsItem * idrawable)
{
	if (idrawable)
	{
		QString x = "0", y = "0";

		QXmlStreamAttributes vec = attributes();
		for (int i=0; i < vec.size(); ++i)
		{
			if (vec.at(i).name().toString() == QObject::tr("x"))
			{
				x = vec.at(i).value().toString();
			}
			else
			if (vec.at(i).name().toString() == QObject::tr("y"))
			{
				y = vec.at(i).value().toString();
			}
		}

		NodeGraphicsItem::ControlPoint * cp = new NodeGraphicsItem::ControlPoint;
		cp->setPos( x.toDouble(), y.toDouble() );

		cp->defaultPen = QPen(cp->pen());
		cp->defaultBrush = QBrush(cp->brush());
		idrawable->addControlPoint(cp);
		readNext();
	}
}

 /*! \brief Reads a shape into an NodeGraphicsItem from an XML file
 * \param NodeGraphicsItem pointer to write as XML
 * \param index of shape in NodeGraphicsItem's shape vector
 * \return void*/
void NodeGraphicsReader::readShape(NodeGraphicsItem * idrawable)
{
	if (idrawable)
	{
		QStringList shapeNames;
		shapeNames << "arc" << "line" << "bezier";

		NodeGraphicsItem::Shape * shape = new NodeGraphicsItem::Shape;
		idrawable->addShape(shape);

		QXmlStreamAttributes vec = attributes();
		for (int i=0; i < vec.size(); ++i)
		{
			if (vec.at(i).name().toString() == QObject::tr("Negative"))
			{
				int j = vec.at(i).value().toString().toInt();
				shape->negative = (j > 0);
			}
		}

		readNext();
		QXmlStreamAttributes attribs;
		QStringList stringList;
		bool ok;
		while (!(isEndElement() && name() == "Shape"))
		{
			if (isStartElement())
			{
				if (name() == "ControlPoints")
						{
							attribs = attributes();
							if (attribs.size() == 1 && attribs.at(0).name().toString() == "indices")
							{
                                                                stringList = attribs.at(0).value().toString().split(QString(","),QString::SkipEmptyParts);
								for (int i=0; i < stringList.size(); ++i)
								{
									int k = stringList[i].toInt(&ok);
									if (ok && k < idrawable->controlPoints.size())
										shape->controlPoints.push_back(idrawable->controlPoints[k]);
								}
							}
						}
				else
				if (name() == "LineTypes")
						{
							attribs = attributes();
							if (attribs.size() == 1 && attribs.at(0).name().toString() == "sequence")
							{
                                                                stringList = attribs.at(0).value().toString().split(QString(","),QString::SkipEmptyParts);
								for (int i=0; i < stringList.size(); ++i)
								{
									int k = shapeNames.indexOf(stringList[i]);
									if (k >= 0)
										shape->types.push_back(NodeGraphicsItem::ShapeType(k));
								}
							}
						}
				else
				if (name() == "Parameters")
						{
							attribs = attributes();
							if (attribs.size() == 1 && attribs.at(0).name().toString() == "values")
							{
                                                                stringList = attribs.at(0).value().toString().split(QString(","),QString::SkipEmptyParts);
								for (int i=0; i < stringList.size(); ++i)
								{
									qreal k = stringList[i].toDouble(&ok);
									if (ok)
										shape->parameters.push_back(k);
								}
							}
						}
				else
				if (name() == "Gradient")
						{
							int type = 3;
							vec = attributes();

							for (int i=0; i < vec.size(); ++i)
							{
								if (vec.at(i).name().toString() == QObject::tr("Type"))
								{
									type = vec.at(i).value().toString().toInt();
								}
							}
							readNext();

							QGradientStops stops;

							while (isStartElement() && name() == "Stop")
							{
								qreal value = 1;
								int alpha = 255;
								QColor color;
								vec = attributes();

								for (int i=0; i < vec.size(); ++i)
								{
									if (vec.at(i).name().toString() == QObject::tr("Value"))
									{
										value = vec.at(i).value().toString().toDouble();
									}
									else
									if (vec.at(i).name().toString() == QObject::tr("Color"))
									{
										color = QColor(vec.at(i).value().toString());
									}
									else
									if (vec.at(i).name().toString() == QObject::tr("Alpha"))
									{
										alpha = vec.at(i).value().toString().toInt();
									}
								}
								color.setAlpha(alpha);
								stops.push_back( QGradientStop(value,color) );

								readNext();
								readNext();
								//qDebug() << value << " " << tokenType() << " " << name().toString();
							}

							//for (int i=0; i < stops.size(); ++i) stops[i].first /= (stops.size() - 1);

							QPointF start, stop;
							QString x = "0", y = "0";

							if (isStartElement() && name() == "Start")
							{
								vec = attributes();

								for (int i=0; i < vec.size(); ++i)
								{
									if (vec.at(i).name().toString() == QObject::tr("x"))
									{
										x = vec.at(i).value().toString();
									}
									else
									if (vec.at(i).name().toString() == QObject::tr("y"))
									{
										y = vec.at(i).value().toString();
									}
								}

								start.setX(x.toDouble());
								start.setY(y.toDouble());
								readNext();
							}

							readNext();

							if (isStartElement() && name() == "End")
							{
								vec = attributes();

								for (int i=0; i < vec.size(); ++i)
								{
									if (vec.at(i).name().toString() == QObject::tr("x"))
									{
										x = vec.at(i).value().toString();
									}
									else
									if (vec.at(i).name().toString() == QObject::tr("y"))
									{
										y = vec.at(i).value().toString();
									}
								}

								stop.setX(x.toDouble());
								stop.setY(y.toDouble());
							}

							shape->gradientPoints.first = start;
							shape->gradientPoints.second = stop;


							readNext();
							if (type == 0)
							{
								QLinearGradient gradient(start,stop);
								gradient.setStops(stops);
								shape->setBrush(gradient);
							}
							else
							if (type == 1)
							{
								QRadialGradient gradient(start,
													sqrt( (stop.y()-start.y())*(stop.y()-start.y()) +
														  (stop.x()-start.x())*(stop.x()-start.x())));
								gradient.setStops(stops);
								shape->setBrush(gradient);
							}
							else
							{
								QGradient gradient;
								gradient.setStops(stops);
								shape->setBrush(gradient);
							}

							//readNext();
						}
				else
				if (name() == "Pen")
						{
							vec = attributes();
							QColor color;
							int alpha=0, width=0;

							for (int i=0; i < vec.size(); ++i)
							{
								if (vec.at(i).name().toString() == QObject::tr("Width"))
								{
									width = vec.at(i).value().toString().toInt();
								}
								else
								if (vec.at(i).name().toString() == QObject::tr("Color"))
								{
									color = QColor(vec.at(i).value().toString());
								}
								else
								if (vec.at(i).name().toString() == QObject::tr("Alpha"))
								{
									alpha = vec.at(i).value().toString().toInt();
								}
							}

							color.setAlpha(alpha);
							shape->setPen(QPen(color,width));
						}
				if (name() == "Fill")
						{
							vec = attributes();
							QColor color;
							int alpha=0;

							for (int i=0; i < vec.size(); ++i)
							{
								if (vec.at(i).name().toString() == QObject::tr("Color"))
								{
									color = QColor(vec.at(i).value().toString());
								}
								else
								if (vec.at(i).name().toString() == QObject::tr("Alpha"))
								{
									alpha = vec.at(i).value().toString().toInt();
								}
							}

							color.setAlpha(alpha);
							shape->setBrush(QBrush(color));
						}
			}
			readNext();
		}
		shape->defaultPen = QPen(shape->pen());
		shape->defaultBrush = QBrush(shape->brush());
	}
}

}
