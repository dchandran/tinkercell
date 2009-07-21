/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This is the header file for the xml writer that writes a connection item.
 
****************************************************************************/

#include "Core/ItemHandle.h"
#include "Core/ConnectionGraphicsWriter.h"
#include "Core/NodeGraphicsWriter.h"

namespace Tinkercell
{

/*! \brief constructor. Sets autoformatting to true*/
ConnectionGraphicsWriter::ConnectionGraphicsWriter() : QXmlStreamWriter()
{
	setAutoFormatting(true);
}

/*! \brief Writes an ConnectionGraphicsItem XML file with the document headers
* \param ConnectionGraphicsItem pointer to write as XML
* \param QIODevice to use
* \return void*/
bool ConnectionGraphicsWriter::writeXml(ConnectionGraphicsItem * idrawable,const QString& fileName)
{
	if (!idrawable) return false;
	
	QFile file (fileName);
	
	if (!file.open(QFile::WriteOnly | QFile::Text)) 
	{
        return false;
    }
	
	setDevice(&file);

    writeStartDocument();
    writeDTD("<!DOCTYPE ConnectionGraphicsItem>");
	
	writeConnectionGraphics(idrawable,&file);
	
    writeEndDocument();
	
	return true;
}

/*! \brief Writes an ConnectionGraphicsItem XML file with the document headers
* \param ConnectionGraphicsItem pointer to write as XML
* \param QIODevice to use
* \return void*/
bool ConnectionGraphicsWriter::writeXml(ConnectionGraphicsItem * idrawable,QIODevice * device)
{
	if (!idrawable || !device) return false;
		
	setDevice(device);

    writeStartDocument();
    writeDTD("<!DOCTYPE ConnectionGraphicsItem>");
	
	writeConnectionGraphics(idrawable,device);
	
    writeEndDocument();
	
	return true;
}
 /*! \brief Writes an NodeImage as an XML file using the xml writer provided 
 * \param connection item pointer to write as XML
 * \param xml writer in use
 * \return void*/ 
bool ConnectionGraphicsWriter::writeConnectionGraphics(ConnectionGraphicsItem * idrawable,QIODevice * device)
{
	if (!idrawable || !device) return false;
	setDevice(device);
	
	return writeConnectionGraphics(idrawable,const_cast<ConnectionGraphicsWriter*>(this));
}
 /*! \brief Writes an NodeImage as an XML file using the xml writer provided 
 * \param connection item pointer to write as XML
 * \param xml writer in use
 * \return void*/ 
bool ConnectionGraphicsWriter::writeConnectionGraphics(ConnectionGraphicsItem * connection,QXmlStreamWriter * writer)
{
	if (!connection || !writer) return false;
	
	QStringList types;
	types << "line" << "bezier";
	
	writer->writeStartElement("ConnectionGraphicsItem");
    writer->writeAttribute("version", "1.0");
	writer->writeAttribute("color", connection->defaultPen.color().name());
	writer->writeAttribute("width", QString::number(connection->defaultPen.widthF()));
	writer->writeAttribute("type", types[ (int)connection->lineType ]);
	writer->writeAttribute("style", QString::number((int)(connection->defaultPen.style())));
	writer->writeAttribute("gap", QString::number(connection->arrowHeadDistance));
	
	QList<ConnectionGraphicsItem::ControlPoint*> controlPoints = connection->controlPoints(true);
	
	writer->writeStartElement("ControlPoints");
	writeControlPoints(controlPoints,writer);
	writer->writeEndElement();
	
	writer->writeStartElement("PathVectors");
	for (int i=0; i < connection->pathVectors.size(); ++i)
	{
		writePathVector(controlPoints,connection->pathVectors[i],writer);
	}
	writer->writeEndElement();
	
	writer->writeStartElement("CenterRegion");
	writer->writeStartElement("Rect");
	writer->writeAttribute("width", QString::number(connection->centerRegion.width()));
	writer->writeAttribute("height", QString::number(connection->centerRegion.height()));
	writer->writeEndElement();
	if (connection->centerRegionItem)
	{
		writer->writeStartElement("Decorator");
		writeNode(connection->centerRegionItem,writer);
		writer->writeEndElement();
	}
	writer->writeEndElement();
	
	writer->writeEndElement();	
	return true;
}
 /*! \brief Writes all the control points in an ConnectionGraphicsItem to an XML file 
 * \param control points list to write as XML
 * \return void*/
void ConnectionGraphicsWriter::writeControlPoints(QList<ConnectionGraphicsItem::ControlPoint*>& controlPoints , QXmlStreamWriter * writer)
{
	if (writer)
	{
		for (int i=0; i < controlPoints.size(); ++i)
		{
			if (controlPoints[i])
			{
				writer->writeStartElement("ControlPoint");
				writer->writeAttribute("x",QString::number(controlPoints[i]->x()));
				writer->writeAttribute("y",QString::number(controlPoints[i]->y()));
				writer->writeEndElement();
			}
		}
	}
}

 /*! \brief Writes a PathVector of a ConnectionGraphicsItem to an XML file
 * \param control points of the connection item where this PathVector belongs
 * \param xml writer in use
 * \return void*/
void ConnectionGraphicsWriter::writePathVector(QList<ConnectionGraphicsItem::ControlPoint*>& controlPoints, ConnectionGraphicsItem::PathVector& pathVector,QXmlStreamWriter * writer)
{
	if (writer)
	{
		ItemHandle * nodeHandle = 0;
		QStringList controlPointsList;
		writer->writeStartElement("Path");
			writer->writeAttribute("numPoints",QString::number(pathVector.size()));
			//write connected nodes
			if ((nodeHandle = getHandle(NodeGraphicsItem::topLevelNodeItem(pathVector[0]))))
			{
				writer->writeAttribute("NodeAtStart",nodeHandle->fullName());
				writer->writeAttribute("NodeAtStartX",QString::number(NodeGraphicsItem::topLevelNodeItem(pathVector[0])->pos().x()));
				writer->writeAttribute("NodeAtStartY",QString::number(NodeGraphicsItem::topLevelNodeItem(pathVector[0])->pos().y()));
			}
			else
			{
				writer->writeAttribute("NodeAtStart",QString());
				writer->writeAttribute("NodeAtStartX",QString("0"));
				writer->writeAttribute("NodeAtStartY",QString("0"));
			}
				
			if ((pathVector.size() > 1) && (nodeHandle = getHandle(NodeGraphicsItem::topLevelNodeItem(pathVector[pathVector.size()-1]))))
			{
				writer->writeAttribute("NodeAtEnd",nodeHandle->fullName());
				writer->writeAttribute("NodeAtEndX",QString::number(NodeGraphicsItem::topLevelNodeItem(pathVector[pathVector.size()-1])->pos().x()));
				writer->writeAttribute("NodeAtEndY",QString::number(NodeGraphicsItem::topLevelNodeItem(pathVector[pathVector.size()-1])->pos().y()));
			}
			else
			{
				NodeGraphicsItem * node = NodeGraphicsItem::topLevelNodeItem(pathVector[pathVector.size()-1]);
				if (node && node->className == ArrowHeadItem::class_name && 
					static_cast<ArrowHeadItem*>(node)->connectionItem && 
					static_cast<ArrowHeadItem*>(node)->connectionItem->itemHandle && 
					static_cast<ArrowHeadItem*>(node)->connectionItem->centerRegionItem)
				{
					ConnectionGraphicsItem * arrowConnection = static_cast<ArrowHeadItem*>(node)->connectionItem;
					
					writer->writeAttribute("NodeAtEnd",arrowConnection->itemHandle->fullName());
					writer->writeAttribute("NodeAtEndX",QString::number(arrowConnection->pos().x()));
					writer->writeAttribute("NodeAtEndY",QString::number(arrowConnection->pos().y()));
				}
				else
				{
					writer->writeAttribute("NodeAtEnd",QString());
					writer->writeAttribute("NodeAtEndX",QString("0"));
					writer->writeAttribute("NodeAtEndY",QString("0"));
				}
			}
			////
			writer->writeStartElement("ControlPoints");			
			for (int i=0; i < pathVector.size(); ++i)
			{
				for (int j=0; j < controlPoints.size(); ++j)
				{
					if (controlPoints[j] == pathVector[i])
					{
						controlPointsList << (QString::number(j));
						break;
					}
				}
			}
			writer->writeAttribute("indices",controlPointsList.join(QString(",")));
			writer->writeEndElement();
			
			if (pathVector.arrowStart)
			{			
				writer->writeStartElement("ArrowAtStart");
				writeArrowHead(pathVector.arrowStart,writer);
				writer->writeEndElement();
			}
			if (pathVector.arrowEnd)
			{			
				writer->writeStartElement("ArrowAtEnd");				
				writeArrowHead(pathVector.arrowEnd,writer);
				writer->writeEndElement();
			}
			
		writer->writeEndElement();
	}
}

 /*! \brief Writes an arrow head using NodeGraphicsWriter to an XML file 
 * \param arrow head pointer to write
 * \param xml writer in use
 * \return void*/
void ConnectionGraphicsWriter::writeArrowHead(ArrowHeadItem * node, QXmlStreamWriter * writer)
{
	if (node && writer)
	{
		QTransform t1 = node->sceneTransform();
		QPointF pos = node->scenePos();
		
		writer->writeStartElement("pos");
		writer->writeAttribute("x",QString::number(pos.x()));
		writer->writeAttribute("y",QString::number(pos.y()));
		writer->writeAttribute("angle",QString::number(node->angle));
		writer->writeEndElement();
		
		writer->writeStartElement("transform");
		writer->writeAttribute("m11",QString::number(t1.m11()));
		writer->writeAttribute("m12",QString::number(t1.m12()));
		writer->writeAttribute("m21",QString::number(t1.m21()));
		writer->writeAttribute("m22",QString::number(t1.m22()));
		writer->writeEndElement();
		
		NodeGraphicsWriter::writeNodeGraphics(node,writer);
	}
}

void ConnectionGraphicsWriter::writeNode(NodeGraphicsItem* node, QXmlStreamWriter* writer)
{
	
	if (node && writer)
	{
		QTransform t1 = node->sceneTransform();
		QPointF pos = node->scenePos();
		
		writer->writeStartElement("pos");
		writer->writeAttribute("x",QString::number(pos.x()));
		writer->writeAttribute("y",QString::number(pos.y()));
		writer->writeEndElement();
		
		writer->writeStartElement("transform");
		writer->writeAttribute("m11",QString::number(t1.m11()));
		writer->writeAttribute("m12",QString::number(t1.m12()));
		writer->writeAttribute("m21",QString::number(t1.m21()));
		writer->writeAttribute("m22",QString::number(t1.m22()));
		writer->writeEndElement();
		
		NodeGraphicsWriter::writeNodeGraphics(node,writer);
	}
}

}
