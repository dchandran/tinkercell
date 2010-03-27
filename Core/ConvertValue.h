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
extern "C"
{
	#include "TCstructs.h"
}

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{
	class MainWindow;
	class ItemHandle;

	/*! \brief construct a Matrix with 0 rows and columns
	\ingroup helper
	\return Matrix 
	*/
	MY_EXPORT Matrix emptyMatrix();
	/*! \brief convert void* to ItemHandle pointer
	\ingroup helper
	\return ItemHandle*
	*/
	MY_EXPORT ItemHandle* ConvertValue(void*);
	/*! \brief convert ItemHandle pointer to void *
	\ingroup helper
	\return void* 
	*/
	MY_EXPORT void* ConvertValue(ItemHandle*);
	/*! \brief convert ArrayOfItems to QList of ItemHandle pointers
	\ingroup helper
	\return QList<ItemHandle*>
	*/
	MY_EXPORT QList<ItemHandle*>* ConvertValue(ArrayOfItems);
	/*! \brief convert QList of ItemHandle pointers to ArrayOfItems
	\ingroup helper
	\return ArrayOfItems
	*/
	MY_EXPORT ArrayOfItems ConvertValue(const QList<ItemHandle*>&);
	/*! \brief convert char* to QString
	\ingroup helper
	\return QString
	*/
	MY_EXPORT QString ConvertValue(const char*);
	/*! \brief convert QString to null-terminated char*
	\ingroup helper
	\return null-terminated char* 
	*/
	MY_EXPORT char* ConvertValue(const QString&);
	/*! \brief convert TableOfStrings to DataTable of QString
	\ingroup helper
	\return QStringList
	*/
	MY_EXPORT DataTable<QString>* ConvertValue(TableOfStrings);
	/*! \brief convert DataTable of QStrings to TableOfStrings
	\ingroup helper
	\return TableOfStrings
	*/
	MY_EXPORT TableOfStrings ConvertValue(const DataTable<QString>&);
	/*! \brief convert matrix to datatable<double> (see DataTable.h and TCstructs.h)
	\ingroup helper
	\return DataTable of qreals
	*/
	MY_EXPORT DataTable<qreal>* ConvertValue(Matrix);
	/*! \brief convert Datatable<double> to Matrix (see DataTable.h and TCstructs.h)
	\ingroup helper
	\return Matrix
	*/
	MY_EXPORT Matrix ConvertValue(const DataTable<qreal>&);
	/*! \brief convert ArrayOfStrings to QStringList
	\ingroup helper
	\return QStringList
	*/
	MY_EXPORT QStringList ConvertValue(ArrayOfStrings);
	/*! \brief convert QStringList to ArrayOfStrings
	\ingroup helper
	\return ArrayOfStrings
	*/
	MY_EXPORT ArrayOfStrings ConvertValue(const QStringList& );
}
#endif

