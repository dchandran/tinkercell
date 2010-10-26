/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines an xml reader that reads a ConnectionGraphicsItem 

****************************************************************************/

#include "ConnectionGraphicsReader.h"
#include "ItemHandle.h"
#include <QtDebug>

namespace Tinkercell
{
	/*! \brief Reads up to the next start node
	* \return Token Typer*/ 
	QXmlStreamReader::TokenType ConnectionGraphicsReader::readNext()
	{
		QXmlStreamReader::readNext();	
		while (!atEnd() && !isStartElement() && !isEndElement()) QXmlStreamReader::readNext();
		return tokenType();
	}
	/*! \brief Reads a control point from an XML file 
	* \param XML reader in use
	* \return void*/
	ConnectionGraphicsItem::ControlPoint* ConnectionGraphicsReader::readControlPoint(QXmlStreamReader * reader)
	{
		if (reader)
		{
			QString x = "0", y = "0";

			QXmlStreamAttributes vec = reader->attributes();
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

			ConnectionGraphicsItem::ControlPoint * cp = new ConnectionGraphicsItem::ControlPoint;
			cp->setPos( x.toDouble(), y.toDouble() );
			cp->defaultPen = QPen(cp->pen());
			cp->defaultBrush = QBrush(cp->brush());

			return cp;
		}
		return 0;
	}
	/*! \brief Reads all control points from an XML file 
	* \param xml reader in use
	* \return list of control points*/
	QList<ConnectionGraphicsItem::ControlPoint*> ConnectionGraphicsReader::readControlPoints(QXmlStreamReader * reader)
	{
		QList<ConnectionGraphicsItem::ControlPoint*> list;
		if (reader)
		{
			ConnectionGraphicsItem::ControlPoint* cp = 0;
			if (reader->isStartElement() && reader->name() == "ControlPoints")
			{
				while (!reader->atEnd() && !(reader->isEndElement() && reader->name() == "ControlPoints"))
				{
					if (reader->isStartElement() && reader->name() == "ControlPoint")
					{
						cp = readControlPoint(reader);
						if (cp)
							list << cp;
					}
					reader->readNext();
				}
			}
		}
		return list;
	}
	/*! \brief Reads a shape into an NodeGraphicsItem from an XML file
	* \param hash table of fullname -> node handle
	* \param list of control points to use
	* \param the xml reader in use
	* \return path vector with all the control points and nodes and arrows*/
	ConnectionGraphicsItem::CurveSegment ConnectionGraphicsReader::readCurveSegment(QHash<QString,ItemHandle*>& nodes, QHash<QString,ItemHandle*>& connections, QList<ConnectionGraphicsItem::ControlPoint*>& controlPoints, NodeGraphicsReader * reader)
	{
		ConnectionGraphicsItem::CurveSegment pathVector;
		ItemHandle *startNodeHandle = 0, *endNodeHandle = 0;
		ItemHandle *startConnectionHandle = 0, *endConnectionHandle = 0;
		QPointF startPos, endPos;
		//qDebug() << "control points list: " << controlPoints.size();
		if (reader)
		{
			while (!reader->atEnd() && !(reader->isStartElement() && reader->name() == "Path"))
			{
				reader->readNext();
			}
			QXmlStreamAttributes attribs;
			if (reader->isStartElement() && reader->name() == "Path" && controlPoints.size() > 1)
			{
				attribs = reader->attributes();
				for (int i=0; i < attribs.size(); ++i)
				{
					if (attribs.at(i).name().toString() == "numPoints")
					{
						bool ok;
						int cap = attribs.at(i).value().toString().toInt(&ok);
						if (ok)
							pathVector.reserve(cap);
					}
					else
						if (attribs.at(i).name().toString() == "NodeAtStart")
						{
							QString s = attribs.at(i).value().toString();
							if (nodes.contains(s))
								startNodeHandle = nodes[s];
							if (connections.contains(s))
								startConnectionHandle = connections[s];
						}
						else
							if (attribs.at(i).name().toString() == "NodeAtStartX")
							{
								bool ok;
								qreal x = attribs.at(i).value().toString().toDouble(&ok);
								if (ok)
									startPos.rx() = x;
							}
							else
								if (attribs.at(i).name().toString() == "NodeAtStartY")
								{
									bool ok;
									qreal y = attribs.at(i).value().toString().toDouble(&ok);
									if (ok)
										startPos.ry() = y;
								}
								else
									if (attribs.at(i).name().toString() == "NodeAtEnd")
									{
										QString s = attribs.at(i).value().toString();
										if (nodes.contains(s))
											endNodeHandle = nodes[s];
										if (connections.contains(s))
											endConnectionHandle = connections[s];
									}
									else
										if (attribs.at(i).name().toString() == "NodeAtEndX")
										{
											bool ok;
											qreal x = attribs.at(i).value().toString().toDouble(&ok);
											if (ok)
												endPos.rx() = x;
										}
										else
											if (attribs.at(i).name().toString() == "NodeAtEndY")
											{
												bool ok;
												qreal y = attribs.at(i).value().toString().toDouble(&ok);
												if (ok)
													endPos.ry() = y;
											}
				}
			}

			//qDebug() << "start Handle = " << startNodeHandle << " end Node = " << endNodeHandle;

			while (!reader->atEnd() && !(reader->isEndElement() && reader->name() == "Path"))
			{
				if (reader->isStartElement())
				{
					if (reader->name() == "ControlPoints" && controlPoints.size() > 1)
					{
						attribs = reader->attributes();
						if (attribs.size() == 1 && attribs.at(0).name().toString() == "indices") //comma separated indices
						{
							bool ok;
							QStringList indices = attribs.at(0).value().toString().split(",");
							for (int i=0; i < indices.size(); ++i)
							{
								int k = indices[i].toInt(&ok);
								if (k >= 0 && k < controlPoints.size())
									pathVector.push_back(controlPoints[k]);  //the main step
							}
							//qDebug() << "reading control points : " << pathVector.size();
						}
					}
					else
						if (reader->name() == "ArrowAtStart")
						{
							pathVector.arrowStart = readArrow(*reader,QString("ArrowAtStart"));
						}
						else
							if (reader->name() == "ArrowAtEnd")
							{
								pathVector.arrowEnd = readArrow(*reader,QString("ArrowAtEnd"));
							}
				}
				reader->readNext();
			}
		}

		if (pathVector.size() > 0 && controlPoints.size() > 1)
		{
			if (startNodeHandle && pathVector[0])
			{
				for (int i=0; i < startNodeHandle->graphicsItems.size(); ++i)
				{
					NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(startNodeHandle->graphicsItems[i]);
					if (node)
					{
						pathVector[0]->setParentItem(node);
						if (node->pos() == startPos)
						{
							break;
						}
					}
				}
			}
			else
				if (startConnectionHandle && pathVector[0])
				{
					for (int i=0; i < startConnectionHandle->graphicsItems.size(); ++i)
						if (qgraphicsitem_cast<ConnectionGraphicsItem*>(startConnectionHandle->graphicsItems[i]))
						{
							NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(
								(qgraphicsitem_cast<ConnectionGraphicsItem*>(startConnectionHandle->graphicsItems[i]))->centerRegionItem);
							if (node)
							{
								pathVector[0]->setParentItem(node);
								if (node->pos() == startPos)
								{
									break;
								}
							}
						}
				}
				int k = pathVector.size() - 1;
				if (endNodeHandle && pathVector[k])
				{
					for (int i=0; i < endNodeHandle->graphicsItems.size(); ++i)
					{
						NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(endNodeHandle->graphicsItems[i]);
						if (node)
						{
							pathVector[k]->setParentItem(node);
							if (node->pos() == endPos)
							{
								break;
							}
						}
					}
				}
				else
					if (endConnectionHandle && pathVector[k])
					{
						for (int i=0; i < endConnectionHandle->graphicsItems.size(); ++i)
							if (qgraphicsitem_cast<ConnectionGraphicsItem*>(endConnectionHandle->graphicsItems[i]))
							{
								NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(
									(qgraphicsitem_cast<ConnectionGraphicsItem*>(endConnectionHandle->graphicsItems[i]))->centerRegionItem);
								if (node)
								{
									pathVector[k]->setParentItem(node);
									if (node->pos() == startPos)
									{
										break;
									}
								}
							}
					}
		}

		return pathVector;
	}
	/*! \brief Reads a ConnectionGraphicsItem from XML, given all the nodes for the connection are already in the scene
	* \param list of nodes
	* \param xml reader in use
	* \return list of control points*/
	ConnectionGraphicsItem* ConnectionGraphicsReader::readConnectionGraphics(const QList<NodeGraphicsItem*>& nodes, const QList<ConnectionGraphicsItem*>& connections, NodeGraphicsReader * reader)
	{
		if (!reader) return 0;
		while (!reader->atEnd() && !(reader->isStartElement() && reader->name() == "ConnectionGraphicsItem"))
		{
			reader->readNext();
		}
		if (reader->atEnd())
		{
			return 0;
		}

		ConnectionGraphicsItem * connection = 0;

		if (reader->isStartElement() && reader->name() == "ConnectionGraphicsItem")
		{
			connection = new ConnectionGraphicsItem;
			QStringList types;
			types << "line" << "bezier";

			QXmlStreamAttributes attribs = reader->attributes();

			for (int i=0; i < attribs.size(); ++i)
			{
				if (attribs.at(i).name().toString() == "color")
				{
					connection->defaultPen.setColor(QColor(attribs.at(i).value().toString()));
				}
				else
				if (attribs.at(i).name().toString() == "name")
				{
					connection->name = attribs.at(i).value().toString();
				}
				else
				if (attribs.at(i).name().toString() == "group")
				{
					connection->groupID = attribs.at(i).value().toString();
				}
				else
					if (attribs.at(i).name().toString() == "width")
					{
						bool ok;
						qreal w = attribs.at(i).value().toString().toDouble(&ok);
						if (!ok) 
							w = 2.0;
						connection->defaultPen.setWidthF(w);
					}
					else
						if (attribs.at(i).name().toString() == "type")
						{
							int w = types.indexOf(attribs.at(i).value().toString());
							if (w >= 0) 
								connection->lineType = ConnectionGraphicsItem::LineType(w);
						}
						else
							if (attribs.at(i).name().toString() == "gap")
							{
								bool ok;
								qreal w = attribs.at(i).value().toString().toDouble(&ok);
								if (!ok) 
									w = 2.0;
								connection->arrowHeadDistance = w;
							}
							else
								if (attribs.at(i).name().toString() == "style")
								{
									bool ok;
									int w = attribs.at(i).value().toString().toInt(&ok);
									if (!ok) 
										w = 1;
									connection->defaultPen.setStyle(Qt::PenStyle(w));
								}

			}
			connection->setPen(connection->defaultPen);
			connection->setBrush(connection->defaultBrush);
		}
		else
			return 0;

		QHash<QString,ItemHandle*> nodesHash, connectionsHash;
		ItemHandle * handle;
		for (int i=0; i < nodes.size(); ++i)
		{
			if (nodes.at(i) && (handle = getHandle(nodes.at(i))))
				nodesHash[handle->fullName()] = handle;
		}

		for (int i=0; i < connections.size(); ++i)
		{
			if (connections.at(i) && (handle = getHandle(connections.at(i))))
				connectionsHash[handle->fullName()] = handle;
		}

		QList<ConnectionGraphicsItem::ControlPoint*> controlPoints;
		while (!reader->atEnd() && !(reader->isEndElement() && reader->name() == "ConnectionGraphicsItem"))
		{
			if (reader->isStartElement())
			{
				if (reader->name() == "ControlPoints")
				{		
					controlPoints = readControlPoints(reader);
					for (int i=0; i < controlPoints.size(); ++i)
						controlPoints[i]->connectionItem = connection;
					//qDebug() << "control points = " << controlPoints.size();
				}
				else
					if (reader->name() == "CurveSegments")
					{
						while (!reader->atEnd() && !(reader->isEndElement() && reader->name() == "CurveSegments"))
						{
							connection->curveSegments += readCurveSegment(nodesHash,connectionsHash, controlPoints,reader);
							if (connection->curveSegments.last().arrowStart)
								connection->curveSegments.last().arrowStart->connectionItem = connection;
							if (connection->curveSegments.last().arrowEnd)
								connection->curveSegments.last().arrowEnd->connectionItem = connection;
							reader->readNext();
						}
					}
					else
						if (reader->name() == "CenterRegion")
						{
							readCenterRegion(connection,reader);
						}
			}
			reader->readNext();
		}

		return connection;
	}


	void ConnectionGraphicsReader::readCenterRegion(ConnectionGraphicsItem * connection, NodeGraphicsReader * reader)
	{
		if (!reader || !connection) return;
		while (!reader->atEnd() && !(reader->isEndElement() && reader->name() == "CenterRegion"))
		{
			if (reader->isStartElement() && reader->name() == "Rect")
			{
				QXmlStreamAttributes attribs = reader->attributes();
				qreal w=0,h=0;
				for (int i=0; i < attribs.size(); ++i)
				{
					if (attribs.at(i).name().toString() == "width")
					{
						bool ok;
						qreal n = attribs.at(i).value().toString().toDouble(&ok);
						if (ok) 
							w = n;
					}
					else
						if (attribs.at(i).name().toString() == "height")
						{
							bool ok;
							qreal n = attribs.at(i).value().toString().toDouble(&ok);
							if (ok) 
								h = n;
						}
				}
				connection->centerRegion = QSizeF(w,h);
			}
			else
				if (reader->isStartElement() && reader->name() == "Decorator")
				{
					connection->centerRegionItem = readArrow(*reader,QString("Decorator"));
				}
				reader->readNext();
		}
	}

	/*! \brief Reads an arrow item from xml file. The procedure is very similar to reading a node
	* \param node reader
	* \param name of the entry, i.e. ArrowAtStart or ArrowAtEnd
	* \return arrow item*/
	ArrowHeadItem * ConnectionGraphicsReader::readArrow(NodeGraphicsReader & reader,QString name)
	{
		if (!(reader.isStartElement() && reader.name() == name)) return 0;

		ArrowHeadItem * node = new ArrowHeadItem;

		qreal n=0,m11=0,m12=0,m21=0,m22=0;
		QPointF pos;
		QTransform transform;

		while (!reader.atEnd() && !(reader.isEndElement() && reader.name() == name))
		{
			reader.readNext();

			if (reader.isStartElement())
			{
				if (reader.name() == "listOfRenderInformation")
				{
					reader.readNodeGraphics(node,reader.device());
				}
				else
					if (reader.name() == "pos")
					{
						QXmlStreamAttributes attribs = reader.attributes();
						bool ok;
						if (attribs.size() == 3)
						{
							n = attribs.at(0).value().toString().toDouble(&ok);
							if (ok)
								pos.rx() = n;

							n = attribs.at(1).value().toString().toDouble(&ok);
							if (ok)
								pos.ry() = n;

							n = attribs.at(2).value().toString().toDouble(&ok);
							if (ok)
								node->angle = n;
						}
					}
					else
						if (reader.name() == "transform")
						{
							QXmlStreamAttributes attribs = reader.attributes();
							bool ok;
							if (attribs.size() == 4)
							{
								n = attribs.at(0).value().toString().toDouble(&ok);
								if (ok) m11 = n;

								n = attribs.at(1).value().toString().toDouble(&ok);
								if (ok) m12 = n;

								n = attribs.at(2).value().toString().toDouble(&ok);
								if (ok) m21 = n;

								n = attribs.at(3).value().toString().toDouble(&ok);
								if (ok) m22 = n;
							}
						}
			}
		}
		if (node)
		{
			transform.setMatrix(m11,m12, 0.0, m21, m22, 0.0, 0.0, 0.0, 1.0);
			node->refresh();
			node->setPos(pos);
			node->setTransform(transform);
		}

		return node;
	}

}
