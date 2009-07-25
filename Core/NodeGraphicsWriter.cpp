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
	bool NodeGraphicsWriter::writeXml(NodeGraphicsItem * idrawable,const QString& fileName)
	{
		if (!idrawable) return false;

		QFile file (fileName);

		if (!file.open(QFile::WriteOnly | QFile::Text)) 
		{
			return false;
		}

		setDevice(&file);

		writeStartDocument();
		writeDTD("<!DOCTYPE PartGraphicsItem>");

		idrawable->fileName = fileName;
		writeNodeGraphics(idrawable,&file);

		writeEndDocument();

		return true;
	}

	/*! \brief Writes an NodeGraphicsItem XML file with the document headers
	* \param NodeGraphicsItem pointer to write as XML
	* \param QIODevice to use
	* \return void*/
	bool NodeGraphicsWriter::writeXml(NodeGraphicsItem * idrawable,QIODevice * device)
	{
		if (!idrawable || !device) return false;

		setDevice(device);

		writeStartDocument();
		writeDTD("<!DOCTYPE PartGraphicsItem>");

		writeNodeGraphics(idrawable,device);

		writeEndDocument();

		return true;
	}
	/*! \brief Writes an NodeGraphicsItem as an XML file using the IO device provided
	* \param NodeGraphicsItem pointer to write as XML
	* \param QIODevice to use
	* \return void*/
	bool NodeGraphicsWriter::writeNodeGraphics(NodeGraphicsItem * idrawable,QIODevice * device)
	{
		if (!idrawable || !device) return false;
		setDevice(device);

		return writeNodeGraphics(idrawable,const_cast<NodeGraphicsWriter*>(this));
	}
	/*! \brief Writes an NodeImage as an XML file using the IO device provided 
	* \param NodeImage pointer to write as XML
	* \param XML writer to use
	* \return void*/ 
	bool NodeGraphicsWriter::writeNodeGraphics(NodeGraphicsItem * idrawable, QXmlStreamWriter * writer)
	{
		if (!idrawable || !writer) return false;


		writer->writeStartElement("PartGraphicsItem");
		writer->writeAttribute("version", "1.0");
		writer->writeAttribute("filename", idrawable->fileName);
		writer->writeAttribute("width", QString::number(idrawable->sceneBoundingRect().width()));
		writer->writeAttribute("height", QString::number(idrawable->sceneBoundingRect().height()));

		writer->writeStartElement("ControlPoints");
		for (int i = 0; i < idrawable->controlPoints.size(); ++i)
		{
			try
			{
				writeControlPoint(idrawable, i, writer);
			}
			catch(...)
			{
			}
		}
		writer->writeEndElement();

		writer->writeStartElement("Shapes");
		for (int i = 0; i < idrawable->shapes.size(); ++i)
		{
			try
			{
				writeShape(idrawable, i, writer);
			}
			catch(...)
			{
			}
		}
		writer->writeEndElement();

		writer->writeEndElement();

		return true;
	}
	/*! \brief Writes a control point in an NodeGraphicsItem to an XML file 
	* \param NodeGraphicsItem pointer to write as XML
	* \param index of control point in NodeGraphicsItem's control points' vector
	* \return void*/
	void NodeGraphicsWriter::writeControlPoint(NodeGraphicsItem * idrawable, int i, QXmlStreamWriter * writer)
	{
		if (writer && idrawable)
		{
			NodeGraphicsItem::ControlPoint * ptr = idrawable->controlPoints[i];
			if (!ptr) return;

			writer->writeStartElement("ControlPoint");
			writer->writeAttribute("x",QString::number(ptr->x()));
			writer->writeAttribute("y",QString::number(ptr->y()));
			writer->writeEndElement();
		}
	}

	/*! \brief Writes a shape inside an NodeGraphicsItem to an XML file
	* \param NodeGraphicsItem pointer to write as XML
	* \param index of shape in NodeGraphicsItem's shape vector
	* \return void*/
	void NodeGraphicsWriter::writeShape(NodeGraphicsItem * idrawable, int index, QXmlStreamWriter * writer)
	{
		if (writer && idrawable)
		{
			NodeGraphicsItem::Shape * ptr = idrawable->shapes[index];
			if (!ptr) return;

			QStringList shapeNames;
			shapeNames << "arc" << "line" << "bezier";

			writer->writeStartElement("Shape");
			writer->writeAttribute("Negative",QString::number((int)ptr->negative));
			writer->writeStartElement("ControlPoints");
			QStringList indices;
			for (int i=0; i < ptr->controlPoints.size(); ++i)
			{
				for (int j=0; j < idrawable->controlPoints.size(); ++j)
				{
					if (idrawable->controlPoints[j] == ptr->controlPoints[i])
					{
						indices << QString::number(j);
						break;
					}
				}			
			}
			writer->writeAttribute("indices",indices.join(QString(",")));
			writer->writeEndElement();

			writer->writeStartElement("LineTypes");
			QStringList types;
			for (int i=0; i < ptr->types.size(); ++i)
			{
				if (ptr->types[i] < 3)
					types << shapeNames[ ptr->types[i] ];
			}
			writer->writeAttribute("sequence",types.join(QString(",")));
			writer->writeEndElement();

			writer->writeStartElement("Parameters");
			QStringList params;			
			for (int i=0; i < ptr->parameters.size(); ++i)
			{
				params << QString::number(ptr->parameters[i]);
			}
			writer->writeAttribute("values",params.join(QString(",")));
			writer->writeEndElement();

			QPen pen = ptr->defaultPen;

			writer->writeStartElement("Pen");
			writer->writeAttribute("Width",QString::number(pen.width()));
			writer->writeAttribute("Color",pen.color().name());
			writer->writeAttribute("Alpha",QString::number(pen.color().alpha()));
			writer->writeEndElement();

			const QGradient * gradient = ptr->defaultBrush.gradient();
			if (gradient)			
			{			
				writer->writeStartElement("Gradient");

				writer->writeAttribute("Type",QString::number(gradient->type()));

				QGradientStops stops = gradient->stops();

				if (stops.size() > 0)
				{
					for (int i=0; i < stops.size(); ++i)
					{
						writer->writeStartElement("Stop");
						writer->writeAttribute("Value",QString::number(stops[i].first));
						writer->writeAttribute("Color",stops[i].second.name());
						writer->writeAttribute("Alpha",QString::number(stops[i].second.alpha()));
						writer->writeEndElement();
					}
				}

				writer->writeStartElement("Start");
				writer->writeAttribute("x",QString::number(ptr->gradientPoints.first.x()));
				writer->writeAttribute("y",QString::number(ptr->gradientPoints.first.y()));
				writer->writeEndElement();
				writer->writeStartElement("End");
				writer->writeAttribute("x",QString::number(ptr->gradientPoints.second.x()));
				writer->writeAttribute("y",QString::number(ptr->gradientPoints.second.y()));
				writer->writeEndElement();

				writer->writeEndElement();
			}
			else
			{
				writer->writeStartElement("Fill");
				writer->writeAttribute("Color",ptr->defaultBrush.color().name());
				writer->writeAttribute("Alpha",QString::number(ptr->defaultBrush.color().alpha()));
				writer->writeEndElement();
			}
			writer->writeEndElement();
		}
	}

}
