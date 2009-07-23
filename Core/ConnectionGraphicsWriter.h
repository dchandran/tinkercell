/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This is the header file for the xml writer that writes a connection item.
 
 
****************************************************************************/

#ifndef ATHENA_CONNECTIONGRAPHICSITEMWRITER_H
#define ATHENA_CONNECTIONGRAPHICSITEMWRITER_H

#include <QIODevice>
#include <QFile>
#include <QList>
#include <QXmlStreamWriter>
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"

namespace Tinkercell
{

/*! \brief This class is an xml writer that specifically writes a ConnectionGraphicsItem
	\ingroup io
	*/ 
class ConnectionGraphicsWriter : public QXmlStreamWriter
{
public: 
 /*! \brief default constructor*/
 ConnectionGraphicsWriter();
 /*! \brief Writes an Connection item XML file with the document headers
 * \param connection item pointer to write as XML
 * \param QIODevice to use
 * \return void*/ 
 bool writeXml(ConnectionGraphicsItem * connection,const QString& fileName);
 /*! \brief Writes an Connection item XML file with the document headers
 * \param connection item pointer to write as XML
 * \param QIODevice to use
 * \return void*/ 
 bool writeXml(ConnectionGraphicsItem * connection,QIODevice * device);
 /*! \brief Writes an Connection as an XML file using the IO device provided 
 * \param connection item pointer to write as XML
 * \param QIODevice to use
 * \return void*/ 
 bool writeConnectionGraphics(ConnectionGraphicsItem * connection,QIODevice * device);
 /*! \brief Writes an NodeImage as an XML file using the xml writer provided 
 * \param connection item pointer to write as XML
 * \param xml writer in use
 * \return void*/ 
 static bool writeConnectionGraphics(ConnectionGraphicsItem * connection,QXmlStreamWriter * );
private:
 /*! \brief Writes all the control points in an ConnectionGraphicsItem to an XML file 
 * \param control points list to write as XML
 * \return void*/
 static void writeControlPoints(QList<ConnectionGraphicsItem::ControlPoint*>& controlPoints, QXmlStreamWriter * );
 /*! \brief Writes an arrow head using NodeGraphicsWriter to an XML file 
 * \param ArrowHeadItem* arrow head pointer to write
 * \param xml writer in use
 * \return void*/
 static void writeArrowHead(ArrowHeadItem * arrowHead, QXmlStreamWriter * );
  /*! \brief Writes a node using NodeGraphicsWriter to an XML file 
 * \param NodeGraphicsItem * node pointer to write
 * \param xml writer in use
 * \return void*/
 static void writeNode(NodeGraphicsItem * node, QXmlStreamWriter * );
 /*! \brief Writes a PathVector of a ConnectionGraphicsItem to an XML file
 * \param control points of the connection item where this PathVector belongs
 * \param xml writer in use
 * \return void*/
 static void writePathVector(QList<ConnectionGraphicsItem::ControlPoint*>& controlPoints, ConnectionGraphicsItem::PathVector& pathVector,QXmlStreamWriter * );
 
 
};

}
#endif 
