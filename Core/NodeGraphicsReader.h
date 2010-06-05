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
#include <QHash>
#include <QList>
#include <QStatusBar>
#include <QXmlStreamReader>
#include "NodeGraphicsItem.h"

#ifdef Q_WS_WIN
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{

	/*! \brief An xml reader that reads a NodeGraphicsItem file
	\ingroup core
	*/
	class TINKERCELLEXPORT NodeGraphicsReader : public QXmlStreamReader
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
		/*! \brief Gets a control point from the list of control points (or adds one if not found)
		* \param NodeGraphicsItem pointer to write as XML
		* \param QPointF point
		* \return void*/
		NodeGraphicsItem::ControlPoint * getControlPoint(NodeGraphicsItem * , const QPointF&);
		/*! \brief Reads a shape into an NodeGraphicsItem from an XML file
		* \param NodeGraphicsItem pointer to write as XML
		* \return NodeGraphicsItem::Shape **/
		NodeGraphicsItem::Shape * readPolygon(NodeGraphicsItem *);
		/*! \brief Reads a rect shape into an NodeGraphicsItem from an XML file
		* \param NodeGraphicsItem pointer to write as XML
		* \return void*/
		void readRect(NodeGraphicsItem *);
		/*! \brief Reads a arc shape into an NodeGraphicsItem from an XML file
		* \param NodeGraphicsItem pointer to write as XML
		* \return void*/
		void readArc(NodeGraphicsItem *);
		/*! \brief Reads all brush information from XML file
		* \return void*/
		void readBrushes();

		struct BrushStruct
		{
			QBrush brush;
			QPointF start, end;
		};

		/*! \brief all the fill types used in the file*/
		QHash<QString, BrushStruct > brushes;
	};

}
#endif 
