/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 The ModelReader is used to write an xml file containing the list of handles and their
 data.

****************************************************************************/

#ifndef ATHENA_MODELDATAWRITER_H
#define ATHENA_MODELDATAWRITER_H

#include <QHash>
#include <QIODevice>
#include <QFile>
#include <QStatusBar>
#include <QXmlStreamWriter>
#include "DataTable.h"
#include "GraphicsScene.h"
#include "ItemHandle.h"

namespace Tinkercell
{
/*! \brief writes to an xml file handle names and data table information from a list of item handles
	\ingroup io
*/
class ModelWriter : public QXmlStreamWriter
{
public:
 /*! \brief default constructor*/
 ModelWriter();
 /*! \brief Writes the handles and data for that handle
 * \param NodeImage pointer to write as XML
 * \param QIODevice device to use
 * \return void*/
 bool writeModel(GraphicsScene * scene,QIODevice * device);
 /*! \brief Writes the handles and data for that handle
 * \param NodeImage pointer to write as XML
 * \param QXmlStreamWriter* xml writer to use
 * \return void*/
 static bool writeModel(GraphicsScene * scene,QXmlStreamWriter *);
 /*! \brief Writes a data table of doubles into an XML file
 * \param NodeImage pointer to write as XML
 * \param index of shape in NodeImage's shape vector
 * \return void*/
 static void writeDataTable(const DataTable<qreal>&, QXmlStreamWriter *);
 /*! \brief Writes a data table of strings into an XML file
 * \param NodeImage pointer to write as XML
 * \param index of shape in NodeImage's shape vector
 * \return void*/
 static void writeDataTable(const DataTable<QString>&, QXmlStreamWriter *);
 /*! \brief Writes a handle and all its children
 * \param Item handle pointer to write as XML
 * \return void*/
 static void writeHandle(ItemHandle *, QXmlStreamWriter *);

};

}
#endif
