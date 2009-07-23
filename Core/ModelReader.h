/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 The ModelReader is used to read an xml file containing the list of handles and their
 data.
 
****************************************************************************/

#ifndef TINKERCELL_MODELDATAREADER_H
#define TINKERCELL_MODELDATAREADER_H

#include <math.h>
#include <QIODevice>
#include <QFile>
#include <QStatusBar>
#include <QXmlStreamReader>
#include <QPair>
#include "GraphicsScene.h"
#include "ItemHandle.h"

namespace Tinkercell
{
/*! \brief reads an xml file with handle names and data table information and generates a list of item handles
	\ingroup io
*/
class ModelReader : public QXmlStreamReader
{
public: 
 /*! \brief Reads a list of <family,handles> pairs from an XML file using the IO device provided 
 * \param QIODevice to use
 * \return list of item handles*/ 
 QList< QPair<QString,ItemHandle*> > readHandles(GraphicsScene * , QIODevice * device);
 /*! \brief Reads up to the next start node
 * \return Token Typer*/ 
 QXmlStreamReader::TokenType readNext();
private:
 /*! \brief Reads a handle from an XML file 
 *	\param existing handles
 * \return item handle*/
 QPair<QString,ItemHandle*> readHandle(QList< QPair<QString,ItemHandle*> >& existingHandles);
 /*! \brief Reads a table from an XML file 
 * \return item handle*/
 void readRealsTable(ItemHandle*); 
 /*! \brief Reads a table from an XML file 
 * \return item handle*/
 void readStringsTable(ItemHandle*); 
};

}
#endif 
