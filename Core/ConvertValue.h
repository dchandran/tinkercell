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
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{
	class MainWindow;
	class ItemHandle;
	typedef TableOfReals Matrix;

	/*! \brief construct a Matrix with 0 rows and columns
	\ingroup helper
	\return Matrix 
	*/
	TINKERCELLEXPORT Matrix emptyMatrix();
	/*! \brief convert void* to ItemHandle pointer
	\ingroup helper
	\return ItemHandle*
	*/
	TINKERCELLEXPORT ItemHandle* ConvertValue(void*);
	/*! \brief convert ItemHandle pointer to void *
	\ingroup helper
	\return void* 
	*/
	TINKERCELLEXPORT void* ConvertValue(ItemHandle*);
	/*! \brief convert ArrayOfItems to QList of ItemHandle pointers
	\ingroup helper
	\return QList<ItemHandle*>
	*/
	TINKERCELLEXPORT QList<ItemHandle*>* ConvertValue(ArrayOfItems);
	/*! \brief convert QList of ItemHandle pointers to ArrayOfItems
	\ingroup helper
	\return ArrayOfItems
	*/
	TINKERCELLEXPORT ArrayOfItems ConvertValue(const QList<ItemHandle*>&);
	/*! \brief convert char* to QString
	\ingroup helper
	\return QString
	*/
	TINKERCELLEXPORT QString ConvertValue(const char*);
	/*! \brief convert QString to null-terminated char*
	\ingroup helper
	\return null-terminated char* 
	*/
	TINKERCELLEXPORT const char* ConvertValue(const QString&);
	/*! \brief convert TableOfStrings to DataTable of QString
	\ingroup helper
	\return QStringList
	*/
	TINKERCELLEXPORT DataTable<QString>* ConvertValue(TableOfStrings);
	/*! \brief convert DataTable of QStrings to TableOfStrings
	\ingroup helper
	\return TableOfStrings
	*/
	TINKERCELLEXPORT TableOfStrings ConvertValue(const DataTable<QString>&);
	/*! \brief convert matrix to datatable<double> (see DataTable.h and TCstructs.h)
	\ingroup helper
	\return DataTable of qreals
	*/
	TINKERCELLEXPORT DataTable<qreal>* ConvertValue(Matrix);
	/*! \brief convert Datatable<double> to Matrix (see DataTable.h and TCstructs.h)
	\ingroup helper
	\return Matrix
	*/
	TINKERCELLEXPORT Matrix ConvertValue(const DataTable<qreal>&);
	/*! \brief convert ArrayOfStrings to QStringList
	\ingroup helper
	\return QStringList
	*/
	TINKERCELLEXPORT QStringList ConvertValue(ArrayOfStrings);
	/*! \brief convert QStringList to ArrayOfStrings
	\ingroup helper
	\return ArrayOfStrings
	*/
	TINKERCELLEXPORT ArrayOfStrings ConvertValue(const QStringList& );
}
#endif

