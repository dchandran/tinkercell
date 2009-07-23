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
#include "TCstructs.h"
#include "ItemHandle.h"

namespace Tinkercell
{
	/*! \brief construct a Matrix with 0 rows and columns (see TCstructs.h for Matrix)
	\ingroup helper
	\return empty matrix 
	*/
	Matrix emptyMatrix();
	/*! \brief convert void* to QGraphicsItem (item on the scene) pointer
	\ingroup helper
	\return pointer to an item on the scene
	*/
	ItemHandle* ConvertValue(OBJ);
	/*! \brief convert QGraphicsItem (item on the scene) pointer to void *
	\ingroup helper
	\return pointer to an item on the scene
	*/
	OBJ ConvertValue(ItemHandle*);
	/*! \brief convert void** to QList of QGraphicsItem (item on the scene) pointers
	\ingroup helper
	\return list of pointers to items on the scene
	*/
	QList<ItemHandle*>* ConvertValue(Array);
	/*! \brief convert to list of QGraphicsItem pointers to null-terminated array of void*
	\ingroup helper
	\return null-terminated array of pointers to items on the scene
	*/
	Array ConvertValue(const QList<ItemHandle*>&);
	/*! \brief convert char* to QString
	\ingroup helper
	\return Qt String
	*/
	QString ConvertValue(const char*);
	/*! \brief convert QString to null-terminated char*
	\ingroup helper
	\return null-terminated char* 
	*/
	char* ConvertValue(const QString&);
	/*! \brief convert char** to QStringList
	\ingroup helper
	\return Qt StringList
	*/
	QStringList ConvertValue(char**);
	/*! \brief convert QStringList to null-terminated char**
	\ingroup helper
	\return array of char* 
	*/
	char** ConvertValue(const QStringList&);
	/*! \brief convert matrix to datatable<double> (see DataTable.h and TCstructs.h)
	\ingroup helper
	\return DataTable of qreals
	*/
	DataTable<qreal>* ConvertValue(Matrix);
	/*! \brief convert Datatable<double> to Matrix (see DataTable.h and TCstructs.h)
	\ingroup helper
	\return Matrix with null-terminated rownames, colnames, values
	*/
	Matrix ConvertValue(const DataTable<qreal>&);
	
}
#endif
