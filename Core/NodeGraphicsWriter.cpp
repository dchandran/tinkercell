/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines an xml writer that writes a NodeGraphicsItem file

****************************************************************************/

#include "NodeGraphicsWriter.h"

namespace Tinkercell
{

	/*! \brief constructor. Sets autoformatting to true*/
	NodeGraphicsWriter::NodeGraphicsWriter() : QXmlStreamWriter()
	{
		setAutoFormatting(true);
	}

	/*! \brief Writes an NodeGraphicsItem XML file with the document headers
	* \param NodeGraphicsItem pointer to write as XML
	* \param QIODevice to use
	* \return void*/
	bool NodeGraphicsWriter::writeXml(NodeGraphicsItem * node,const QString& fileName, bool normalize)
	{
		if (!node) return false;

		QFile file (fileName);

		if (!file.open(QFile::WriteOnly | QFile::Text)) 
		{
			return false;
		}

		setDevice(&file);

		writeStartDocument();
		writeDTD("<!DOCTYPE NodeGraphicsItem>");

		node->name = fileName;
		writeNodeGraphics(node,&file,normalize);

		writeEndDocument();

		return true;
	}

	/*! \brief Writes an NodeGraphicsItem XML file with the document headers
	* \param NodeGraphicsItem pointer to write as XML
	* \param QIODevice to use
	* \return void*/
	bool NodeGraphicsWriter::writeXml(NodeGraphicsItem * node,QIODevice * device, bool normalize)
	{
		if (!node || !device) return false;

		setDevice(device);

		writeStartDocument();
		writeDTD("<!DOCTYPE NodeGraphicsItem>");

		writeNodeGraphics(node,device,normalize);

		writeEndDocument();

		return true;
	}
	/*! \brief Writes an NodeGraphicsItem as an XML file using the IO device provided
	* \param NodeGraphicsItem pointer to write as XML
	* \param QIODevice to use
	* \return void*/
	bool NodeGraphicsWriter::writeNodeGraphics(NodeGraphicsItem * node,QIODevice * device, bool normalize)
	{
		if (!node || !device) return false;
		setDevice(device);

		return writeNodeGraphics(node,const_cast<NodeGraphicsWriter*>(this),normalize);
	}
	/*! \brief Writes an NodeImage as an XML file using the IO device provided 
	* \param NodeImage pointer to write as XML
	* \param XML writer to use
	* \return void*/ 
	bool NodeGraphicsWriter::writeNodeGraphics(NodeGraphicsItem * node, QXmlStreamWriter * writer, bool normalize)
	{
		if (!node || !writer) return false;

		writer->writeStartElement("listOfRenderInformation");
		writer->writeAttribute("xmlns", "http://projects.eml.org/bcb/sbml/render/level2");

		writer->writeStartElement("renderInformation");
			writer->writeAttribute("id","TinkerCell_Style");
			writer->writeAttribute("programName","TinkerCell");

		bool writeColors = false, writeGradients = false;

		for (int i = 0; i < node->shapes.size(); ++i)
		{
			if (!writeColors && node->shapes[i] && !node->shapes[i]->defaultBrush.gradient())
				writeColors = true;
			if (!writeGradients && node->shapes[i] && node->shapes[i]->defaultBrush.gradient())
				writeGradients = true;

			if (writeColors && writeGradients)
				break;
		}

		if (writeColors)
		{
			writer->writeStartElement("listOfColorDefinitions");
			for (int i = 0; i < node->shapes.size(); ++i)
				writeShapeColors(node, i, writer);
			writer->writeEndElement();
		}

		if (writeGradients)
		{
			writer->writeStartElement("listOfGradientDefinitions");
			for (int i = 0; i < node->shapes.size(); ++i)
				writeShapeGradients(node, i, writer,normalize);
			writer->writeEndElement();
		}

		writer->writeStartElement("listOfStyles");
		writer->writeStartElement("style");
			writer->writeAttribute("idList", node->name);
			writer->writeAttribute("width", QString::number(node->defaultSize.width()));
			writer->writeAttribute("height", QString::number(node->defaultSize.height()));
		writer->writeStartElement("g");
		
		//normalize
		QPointF pos;
		QPointF min = node->sceneBoundingRect().topLeft();
		QPointF max = node->sceneBoundingRect().bottomRight();
		QSizeF size(max.x() - min.x(), max.y() - min.y());
		
		if (normalize)
		{
			for (int i=0; i < node->controlPoints.size(); ++i)
			{
				pos = node->controlPoints[i]->scenePos();
				if (pos.x() < min.x()) min.rx() = pos.x();
				if (pos.y() < min.y()) min.ry() = pos.y();
				if (pos.x() > max.x()) max.rx() = pos.x();
				if (pos.y() > max.y()) max.ry() = pos.y();
			}
			
			size = QSizeF(max.x() - min.x(), max.y() - min.y());

			for (int i=0; i < node->controlPoints.size(); ++i)
			{
				pos = node->controlPoints[i]->scenePos() - min;
				pos.rx() = pos.x() / size.width() * 100.0;
				pos.ry() = pos.y() / size.height() * 100.0;
				node->controlPoints[i]->setPos(pos);
			}
		}
		else
		{
			/*QPointF diff(50.0,50.0);
			for (int i=0; i < node->controlPoints.size(); ++i)
			{
				pos = node->controlPoints[i]->pos() + diff;
				node->controlPoints[i]->setPos(pos);
			}*/
		}
		
		//write
		for (int i = 0; i < node->shapes.size(); ++i)
		{
			writeShape(node, i, writer,normalize);
		}

		//undo normalization
		if (normalize)
		{
			for (int i=0; i < node->controlPoints.size(); ++i)
			{
				pos = node->controlPoints[i]->scenePos();
				pos.rx() = pos.x() * size.width() / 100.0;
				pos.ry() = pos.y() * size.height() / 100.0;
				node->controlPoints[i]->setPos(pos + min);
			}
		}
		else
		{
			/*QPointF diff(50.0,50.0);
			for (int i=0; i < node->controlPoints.size(); ++i)
			{
				pos = node->controlPoints[i]->pos() - diff;
				node->controlPoints[i]->setPos(pos);
			}*/
		}
		
		writer->writeEndElement(); //g
		writer->writeEndElement(); //style
		writer->writeEndElement(); //list of styles
		writer->writeEndElement(); //render information
		writer->writeEndElement(); //list of render styles
		return true;
	}

	void NodeGraphicsWriter::writeShapeGradients(NodeGraphicsItem * node, int index, QXmlStreamWriter * writer, bool normalize)
	{
		if (writer && node)
		{
			NodeGraphicsItem::Shape * ptr = node->shapes[index];
			if (!ptr || !ptr->defaultBrush.gradient()) return;

			const QGradient * gradient = ptr->defaultBrush.gradient();
		
			if (gradient->type() == QGradient::LinearGradient)
				writer->writeStartElement("linearGradient");
			else
				writer->writeStartElement("radialGradient");
				
			QString unit;
			
			QRectF rect;
			QPointF min, max;
			
			if (normalize)
			{
				unit = QObject::tr("%");
				
				rect = node->sceneBoundingRect();
				min = node->sceneBoundingRect().topLeft();
				max = node->sceneBoundingRect().bottomRight();

				ptr->gradientPoints.first.rx() = (ptr->gradientPoints.first.rx() - rect.left())/rect.width() * 100.0;
				ptr->gradientPoints.first.ry() = (ptr->gradientPoints.first.ry() - rect.top())/rect.height() * 100.0;
				ptr->gradientPoints.second.rx() = (ptr->gradientPoints.second.rx() - rect.left())/rect.width() * 100.0;
				ptr->gradientPoints.second.ry() = (ptr->gradientPoints.second.ry() - rect.top())/rect.height() * 100.0;
			}

			writer->writeAttribute("id",QString("shape") + QString::number(index) + QString("color"));
			writer->writeAttribute("x1",QString::number(ptr->gradientPoints.first.x()) + unit);
			writer->writeAttribute("y1",QString::number(ptr->gradientPoints.first.y()) + unit);
			writer->writeAttribute("x2",QString::number(ptr->gradientPoints.second.x()) + unit);
			writer->writeAttribute("y2",QString::number(ptr->gradientPoints.second.y()) + unit);

			QGradientStops stops = gradient->stops();
			for (int i=0; i < stops.size(); ++i)
			{
				writer->writeStartElement("stop");
				writer->writeAttribute("offset",QString::number(stops[i].first * 100.0) + QString("%"));
				writer->writeAttribute("stop-color",stops[i].second.name());
				writer->writeAttribute("stop-alpha",QString::number(stops[i].second.alphaF()));
				writer->writeEndElement();
			}

			if (normalize)
			{
				ptr->gradientPoints.first.rx() = ptr->gradientPoints.first.rx() * 100.0/rect.width() + rect.left();
				ptr->gradientPoints.first.ry() = ptr->gradientPoints.first.ry() * 100.0/rect.height() + rect.top();
				ptr->gradientPoints.second.rx() = ptr->gradientPoints.second.rx() * 100.0/rect.width() + rect.left();
				ptr->gradientPoints.second.ry() = ptr->gradientPoints.second.ry() * 100.0/rect.height() + rect.top();
			}
			
			writer->writeEndElement();
		}
	}

	void NodeGraphicsWriter::writeShapeColors(NodeGraphicsItem * node, int index, QXmlStreamWriter * writer)
	{
		if (writer && node)
		{
			NodeGraphicsItem::Shape * ptr = node->shapes[index];
			if (!ptr || ptr->defaultBrush.gradient()) return;

			writer->writeStartElement("colorDefinition");
			writer->writeAttribute("id",QString("shape") + QString::number(index) + QString("color"));
			writer->writeAttribute("value",ptr->defaultBrush.color().name());
			writer->writeAttribute("alpha",QString::number(ptr->defaultBrush.color().alpha()));
			writer->writeEndElement();
		}
	}

	/*! \brief Writes a shape inside an NodeGraphicsItem to an XML file
	* \param NodeGraphicsItem pointer to write as XML
	* \param index of shape in NodeGraphicsItem's shape vector
	* \return void*/
	void NodeGraphicsWriter::writeShape(NodeGraphicsItem * node, int index, QXmlStreamWriter * writer, bool normalize)
	{
		if (writer && node)
		{
			NodeGraphicsItem::Shape * ptr = node->shapes[index];
			if (!ptr) return;

			bool isPolygon = ptr->types.contains(Tinkercell::NodeGraphicsItem::bezier) ||
							 ptr->types.contains(Tinkercell::NodeGraphicsItem::line);

			QPen pen = ptr->defaultPen;
			
			QString unit;
			
			if (normalize)
				unit = QObject::tr("%");

			if (isPolygon)
			{
				writer->writeStartElement("polygon");
				writer->writeAttribute("fill",QString("shape") + QString::number(index) + QString("color"));
				writer->writeAttribute("stroke",pen.color().name());
				writer->writeAttribute("stroke-width",QString::number(pen.widthF()));
				writer->writeAttribute("stroke-alpha",QString::number(pen.color().alphaF()));
				writer->writeStartElement("listOfCurveSegments");
			}
			
			for (int i=0, j=1, p=0; i < ptr->types.size() && j < ptr->controlPoints.size(); ++i)
			{
				if (ptr->types[i] == Tinkercell::NodeGraphicsItem::line)
				{
					if (isPolygon)
					{
						writer->writeStartElement("curveSegment");
						writer->writeAttribute("xsi:type","LineSegment");
						writer->writeAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
							writer->writeStartElement("start");
							writer->writeAttribute("x",QString::number(ptr->controlPoints[j-1]->pos().x()) + unit);
							writer->writeAttribute("y",QString::number(ptr->controlPoints[j-1]->pos().y()) + unit);
							writer->writeEndElement();
							writer->writeStartElement("end");
							writer->writeAttribute("x",QString::number(ptr->controlPoints[j]->pos().x()) + unit);
							writer->writeAttribute("y",QString::number(ptr->controlPoints[j]->pos().y()) + unit);
							writer->writeEndElement();
						writer->writeEndElement();
					}
					++j;
				}
				if (ptr->types[i] == Tinkercell::NodeGraphicsItem::bezier && (j+2 < ptr->controlPoints.size()))
				{
					if (isPolygon)
					{
						writer->writeStartElement("curveSegment");
						writer->writeAttribute("xsi:type","CubicBezier");
						writer->writeAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
							writer->writeStartElement("start");
							writer->writeAttribute("x",QString::number(ptr->controlPoints[j-1]->pos().x()) + unit);
							writer->writeAttribute("y",QString::number(ptr->controlPoints[j-1]->pos().y()) + unit);
							writer->writeEndElement();
							writer->writeStartElement("basePoint1");
							writer->writeAttribute("x",QString::number(ptr->controlPoints[j]->pos().x()) + unit);
							writer->writeAttribute("y",QString::number(ptr->controlPoints[j]->pos().y()) + unit);
							writer->writeEndElement();
							writer->writeStartElement("basePoint2");
							writer->writeAttribute("x",QString::number(ptr->controlPoints[j+1]->pos().x()) + unit);
							writer->writeAttribute("y",QString::number(ptr->controlPoints[j+1]->pos().y()) + unit);
							writer->writeEndElement();
							writer->writeStartElement("end");
							writer->writeAttribute("x",QString::number(ptr->controlPoints[j+2]->pos().x()) + unit);
							writer->writeAttribute("y",QString::number(ptr->controlPoints[j+2]->pos().y()) + unit);
							writer->writeEndElement();
						writer->writeEndElement();
					}
					j += 3;
				}
				if (ptr->types[i] == Tinkercell::NodeGraphicsItem::arc && (p+1 < ptr->parameters.size()))
				{
					if (!isPolygon)
					{
						QPointF p1 = ptr->controlPoints[j-1]->pos();
						QPointF p2 = ptr->controlPoints[j]->pos();

						writer->writeStartElement("ellipse");
						writer->writeAttribute("cx",QString::number(((p1+p2)/2).x()) + unit);
						writer->writeAttribute("cy",QString::number(((p1+p2)/2).y()) + unit);
						writer->writeAttribute("rx",QString::number(((p2-p1)/2).x()) + unit);
						writer->writeAttribute("ry",QString::number(((p2-p1)/2).y()) + unit);
						writer->writeAttribute("angleStart",QString::number(ptr->parameters[p]));
						writer->writeAttribute("angleEnd",QString::number(ptr->parameters[p+1]));

						writer->writeAttribute("fill",QString("shape") + QString::number(index) + QString("color"));
						writer->writeAttribute("stroke",pen.color().name());
						writer->writeAttribute("stroke-width",QString::number(pen.widthF()));
						writer->writeAttribute("stroke-alpha",QString::number(pen.color().alphaF()));
						writer->writeEndElement();
					}
					++j;
					p += 2;
				}
				if (ptr->types[i] == Tinkercell::NodeGraphicsItem::rectangle && (p < ptr->parameters.size()))
				{
					if (!isPolygon)
					{
						qreal w =	ptr->controlPoints[j]->scenePos().x() - ptr->controlPoints[j-1]->scenePos().x();
						qreal h =	ptr->controlPoints[j]->scenePos().y() - ptr->controlPoints[j-1]->scenePos().y();

						writer->writeStartElement("rectangle");
						writer->writeAttribute("x",QString::number(ptr->controlPoints[j-1]->scenePos().x()) + unit);
						writer->writeAttribute("y",QString::number(ptr->controlPoints[j-1]->scenePos().y()) + unit);
						writer->writeAttribute("width",QString::number(w));
						writer->writeAttribute("height",QString::number(h));
						writer->writeAttribute("rx",QString::number(ptr->parameters[p]));
						writer->writeAttribute("ry",QString::number(ptr->parameters[p]));

						writer->writeAttribute("fill",QString("shape") + QString::number(index) + QString("color"));
						writer->writeAttribute("stroke",pen.color().name());
						writer->writeAttribute("stroke-width",QString::number(pen.widthF()));
						writer->writeAttribute("stroke-alpha",QString::number(pen.color().alphaF()));
						writer->writeEndElement();
					}
					++j;
					++p;
				}
			}
			if (isPolygon)
			{
				writer->writeEndElement(); //list of curve segments
				writer->writeEndElement(); //polygon
			}
		}
	}

}
