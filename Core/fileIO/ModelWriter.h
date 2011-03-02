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
#include "NetworkHandle.h"
#include "ItemHandle.h"

#ifdef Q_WS_WIN
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{
	/*! \brief writes to an xml file handle names and data table information from a list of item handles
	\ingroup io
	*/
	class TINKERCELLEXPORT ModelWriter : public QXmlStreamWriter
	{
	public: 
		/*! \brief default constructor*/
		ModelWriter();
		/*! \brief Writes the handles and data for that handle 
		* \param NetworkHandle* network
		* \param QIODevice device to use
		* \return void*/ 
		bool writeModel(NetworkHandle * ,QIODevice * device);
		/*! \brief Writes the handles and data for that handle 
		* \param QList<ItemHandle*> list of handles (top level)
		* \param QIODevice device to use
		* \return void*/ 
		bool writeModel(const QList<ItemHandle*>& ,QIODevice * device);
		/*! \brief Writes the handles and data for that handle 
		* \param NetworkHandle* network
		* \param QXmlStreamWriter* xml writer to use
		* \return void*/ 
		static bool writeModel(NetworkHandle * network, QXmlStreamWriter *);
		/*! \brief Writes the handles and data for that handle 
		* \param QList<ItemHandle*> list of handles (top level)
		* \param QXmlStreamWriter* xml writer to use
		* \return void*/ 
		static bool writeModel(const QList<ItemHandle*>&, QXmlStreamWriter *);
		/*! \brief Writes a data table of doubles into an XML file
		* \param DataTable<qreal> datatable
		* \param QXmlStreamWriter* xml writer to use
		* \return void*/
		static void writeDataTable( DataTable<qreal>&, QXmlStreamWriter *);
		/*! \brief Writes a data table of strings into an XML file
		* \param DataTable<QString> datatable
		* \param QXmlStreamWriter* xml writer to use
		* \return void*/
		static void writeDataTable( DataTable<QString>&, QXmlStreamWriter *); 
		/*! \brief Writes a handle and all its children
		* \param Item handle pointer to write as XML
		* \return void*/
		static void writeHandle(ItemHandle *, QXmlStreamWriter *);		
		/*! \brief delimiter*/
		static QString sep;
		static QString sub;
	};

}
#endif 
