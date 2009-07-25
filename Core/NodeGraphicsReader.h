/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines an xml reader that reads a NodeGraphicsItem file

****************************************************************************/

#ifndef TINKERCELL_PARTGRAPHICSITEMREADER_H
#define TINKERCELL_PARTGRAPHICSITEMREADER_H

#include <math.h>
#include <QIODevice>
#include <QFile>
#include <QStatusBar>
#include <QXmlStreamReader>
#include "NodeGraphicsItem.h"

namespace Tinkercell
{

	/*! \brief An xml reader that reads a NodeGraphicsItem file
	\ingroup core
	*/
	class NodeGraphicsReader : public QXmlStreamReader
	{
	public: 
		/*! \brief Reads an NodeGraphicsItem from an XML file using the IO device provided 
		* \param NodeGraphicsItem pointer to write as XML
		* \param QIODevice to use
		* \return NodeGraphicsItem pointer*/ 
		bool readXml(NodeGraphicsItem * idrawable, const QString& fileName);
		/*! \brief Reads an NodeGraphicsItem from an XML file using the IO device provided
		* \param NodeGraphicsItem pointer to write as XML
		* \param QIODevice to use
		* \return NodeGraphicsItem pointer*/ 
		void readNodeGraphics(NodeGraphicsItem * idrawable, QIODevice * device);
		/*! \brief Reads up to the next start node
		* \return Token Typer*/ 
		QXmlStreamReader::TokenType readNext();
	private:
		/*! \brief Reads a control point into an NodeGraphicsItem from an XML file 
		* \param NodeGraphicsItem pointer to write as XML
		* \param index of control point in NodeGraphicsItem's control points' vector
		* \return void*/
		void readControlPoint(NodeGraphicsItem * idrawable);
		/*! \brief Reads a shape into an NodeGraphicsItem from an XML file
		* \param NodeGraphicsItem pointer to write as XML
		* \param index of shape in NodeGraphicsItem's shape vector
		* \return void*/
		void readShape(NodeGraphicsItem * idrawable);

	};

}
#endif 
