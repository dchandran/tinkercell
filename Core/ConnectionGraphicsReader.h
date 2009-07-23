/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 
 This file defines an xml reader that reads a ConnectionGraphicsItem
 
****************************************************************************/

#ifndef TINKERCELL_CONNECTIONGRAPHICSITEMREADER_H
#define TINKERCELL_CONNECTIONGRAPHICSITEMREADER_H

#include <math.h>
#include <QIODevice>
#include <QFile>
#include <QList>
#include <QHash>
#include <QXmlStreamReader>
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "NodeGraphicsReader.h"


namespace Tinkercell
{

/*! \brief An xml reader that reads a NodeGraphicsItem file
	\ingroup io
	*/
class ConnectionGraphicsReader : public QXmlStreamReader
{
public: 
 /*! \brief Reads up to the next start node
 * \return Token Typer*/ 
 QXmlStreamReader::TokenType readNext();
 /*! \brief Reads a ConnectionGraphicsItem from XML, given all the nodes for the connection are already in the scene
 * \param list of nodes
 * \param list of other connections
 * \param xml reader in use
 * \return list of control points*/
 static ConnectionGraphicsItem* readConnectionGraphics(const QList<NodeGraphicsItem*>& nodes, const QList<ConnectionGraphicsItem*>& connections,NodeGraphicsReader * reader);
 /*! \brief Reads all control points from an XML file 
 * \param xml reader in use
 * \return list of control points*/
 static QList<ConnectionGraphicsItem::ControlPoint*> readControlPoints(QXmlStreamReader *);
 /*! \brief Reads a shape into an NodeGraphicsItem from an XML file
 * \param hash table of fullname -> node handle
 * \param list of control points to use
 * \param the xml reader in use
 * \return path vector with all the control points and nodes and arrows*/
 static ConnectionGraphicsItem::PathVector readPathVector(QHash<QString,ItemHandle*>& nodes, QHash<QString,ItemHandle*>& connections, QList<ConnectionGraphicsItem::ControlPoint*>& controlPoints, NodeGraphicsReader *);
 /*! \brief Reads a control point from an XML file 
 * \param XML reader in use
 * \return control point*/
 static ConnectionGraphicsItem::ControlPoint* readControlPoint(QXmlStreamReader *);
 /*! \brief Reads an arrow item from xml file. The procedure is very similar to reading a node
* \param node reader
* \param name of the entry, i.e. ArrowAtStart or ArrowAtEnd
* \return arrow item*/
static ArrowHeadItem * readArrow(NodeGraphicsReader & reader,QString name);
 /*! \brief Reads a node item from xml file. 
* \param node reader
* \param name of the entry
* \return arrow item*/
static NodeGraphicsItem * readNode(NodeGraphicsReader & reader,QString name);
 /*! \brief Reads the center region of a connection from xml file. 
* \param target connection
* \param name of the entry
* \return arrow item*/
static void readCenterRegion(ConnectionGraphicsItem * connection, NodeGraphicsReader * reader);
};

}
#endif 
