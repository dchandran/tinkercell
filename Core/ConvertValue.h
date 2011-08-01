/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines a set of functions that can be used to convert C data types, 
such as char**, char*, and void* to Qt data types, such as QStringList, QString, 
and QGraphicsItem.


****************************************************************************/

#ifndef TINKERCELL_CONVERTVALUE_H
#define TINKERCELL_CONVERTVALUE_H

#include <stdio.h>
#include <stdlib.h>
#include <QString>
#include <QList>
#include <QGraphicsItem>
#include <QStringList>
#include <math.h>
#include "API/TC_structs.h"

#ifndef TINKERCELLCOREEXPORT
#ifdef Q_WS_WIN
#   if defined(TC_EXPORTS) || defined(TinkerCellCore_EXPORTS)
#       define TINKERCELLCOREEXPORT __declspec(dllexport)
#   else
#       define TINKERCELLCOREEXPORT __declspec(dllimport)
#   endif
#else
#    define TINKERCELLCOREEXPORT
#endif
#endif

namespace Tinkercell
{
	class MainWindow;
	class ItemHandle;

	/*! \brief construct a tc_matrix with 0 rows and columns
	\ingroup helper
	\return tc_matrix 
	*/
	TINKERCELLCOREEXPORT tc_matrix emptyMatrix();
	/*! \brief convert void* to ItemHandle pointer
	\ingroup helper
	\return ItemHandle*
	*/
	TINKERCELLCOREEXPORT ItemHandle* ConvertValue(long);
	/*! \brief convert ItemHandle pointer to void *
	\ingroup helper
	\return void* 
	*/
	TINKERCELLCOREEXPORT long ConvertValue(ItemHandle*);
	/*! \brief convert tc_items to QList of ItemHandle pointers
	\ingroup helper
	\return QList<ItemHandle*>
	*/
	TINKERCELLCOREEXPORT QList<ItemHandle*>* ConvertValue(tc_items);
	/*! \brief convert QList of ItemHandle pointers to tc_items
	\ingroup helper
	\return tc_items
	*/
	TINKERCELLCOREEXPORT tc_items ConvertValue(const QList<ItemHandle*>&);
	/*! \brief convert char* to QString
	\ingroup helper
	\return QString
	*/
	TINKERCELLCOREEXPORT QString ConvertValue(const char*);
	/*! \brief convert QString to null-terminated char*
	\ingroup helper
	\return null-terminated char* 
	*/
	TINKERCELLCOREEXPORT const char* ConvertValue(const QString&);
	/*! \brief convert tc_table to DataTable of QString
	\ingroup helper
	\return QStringList
	*/
	TINKERCELLCOREEXPORT DataTable<QString>* ConvertValue(tc_table);
	/*! \brief convert DataTable of QStrings to tc_table
	\ingroup helper
	\return tc_table
	*/
	TINKERCELLCOREEXPORT tc_table ConvertValue(const DataTable<QString>&);
	/*! \brief convert matrix to datatable<double> (see DataTable.h and TC_structs.h)
	\ingroup helper
	\return DataTable of qreals
	*/
	TINKERCELLCOREEXPORT DataTable<qreal>* ConvertValue(tc_matrix);
	/*! \brief convert Datatable<double> to tc_matrix (see DataTable.h and TC_structs.h)
	\ingroup helper
	\return tc_matrix
	*/
	TINKERCELLCOREEXPORT tc_matrix ConvertValue(const DataTable<qreal>&);
	/*! \brief convert tc_strings to QStringList
	\ingroup helper
	\return QStringList
	*/
	TINKERCELLCOREEXPORT QStringList ConvertValue(tc_strings);
	/*! \brief convert QStringList to tc_strings
	\ingroup helper
	\return tc_strings
	*/
	TINKERCELLCOREEXPORT tc_strings ConvertValue(const QStringList& );
}
#endif

