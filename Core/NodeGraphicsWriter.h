/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines an xml writer that writes a NodeGraphicsItem file

****************************************************************************/

#ifndef ATHENA_PARTGRAPHICSITEMWRITER_H
#define ATHENA_PARTGRAPHICSITEMWRITER_H

#include <QIODevice>
#include <QFile>
#include <QList>
#include <QXmlStreamWriter>
#include "NodeGraphicsItem.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	/*! \brief An xml reader that reads a NodeGraphicsItem file
	\ingroup io
	*/
	class MY_EXPORT NodeGraphicsWriter : public QXmlStreamWriter
	{
	public: 
		/*! \brief default constructor*/
		NodeGraphicsWriter();
		/*! \brief Writes an Node graphics item XML file with the document headers
		* \param NodeImage pointer to write as XML
		* \param QIODevice to use
		* \return void*/ 
		bool writeXml(NodeGraphicsItem * idrawable,const QString& fileName, bool normalize=true);
		/*! \brief Writes an Node graphics item XML file with the document headers
		* \param NodeImage pointer to write as XML
		* \param QIODevice to use
		* \return void*/ 
		bool writeXml(NodeGraphicsItem * idrawable,QIODevice * device, bool normalize=true);
		/*! \brief Writes an NodeImage as an XML file using the IO device provided 
		* \param NodeImage pointer to write as XML
		* \param QIODevice to use
		* \return void*/ 
		bool writeNodeGraphics(NodeGraphicsItem * idrawable,QIODevice * device, bool normalize=false);
		/*! \brief Writes an NodeImage as an XML file using the xml writer provided 
		* \param NodeImage pointer to write as XML
		* \param XML writer to use
		* \return void*/ 
		static bool writeNodeGraphics(NodeGraphicsItem * idrawable,QXmlStreamWriter *, bool normalize=false);

	private:
		/*! \brief Writes a shape inside an Node graphics item to an XML file
		* \param NodeImage pointer to write as XML
		* \param index of shape in NodeImage's shape vector
		* \param the xml writer in use
		* \return void*/
		static void writeShape(NodeGraphicsItem * idrawable, int i, QXmlStreamWriter*, bool normalize);
		/*! \brief Writes the fill color used in a shape inside an Node graphics item to an XML file
		* \param NodeImage pointer to write as XML
		* \param index of shape in NodeImage's shape vector
		* \param the xml writer in use
		* \return void*/
		static void writeShapeColors(NodeGraphicsItem * idrawable, int i, QXmlStreamWriter*);
		/*! \brief Writes the fill gradient used in a shape inside an Node graphics item to an XML file
		* \param NodeImage pointer to write as XML
		* \param index of shape in NodeImage's shape vector
		* \param the xml writer in use
		* \return void*/
		static void writeShapeGradients(NodeGraphicsItem * idrawable, int i, QXmlStreamWriter*, bool normalize);


	};

}
#endif 
