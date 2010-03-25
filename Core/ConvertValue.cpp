/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines a set of functions that can be used to convert C data types, 
such as char**, char*, and void* to Qt data types, such as QStringList, QString, 
and QGraphicsItem.


****************************************************************************/

#include <math.h>
#include <QtDebug>
#include "ItemHandle.h"
#include "MainWindow.h"
#include "SymbolsTable.h"
#include "ConvertValue.h"

namespace Tinkercell
{
	/*! \brief construct a Matrix with 0 rows and columns (see TCstructs.h for Matrix)
	\return empty matrix 
	*/
	Matrix emptyMatrix()
	{
		Matrix m;
		m.values = 0;
		m.rownames = 0;
		m.colnames = 0;
		m.cols = 0;
		m.rows = 0;
		return m;
	}
	/*! \brief convert void* to QGraphicsItem (item on the scene) pointer
	\return pointer to an item on the scene
	*/
	ItemHandle* ConvertValue(OBJ o)
	{
		MainWindow * main = MainWindow::instance();
		if (main && 
			main->currentSymbolsTable() && 
			main->currentSymbolsTable()->isValidPointer(o)
			)
			return static_cast<ItemHandle*>(o);
		return 0;
	}
	/*! \brief convert QGraphicsItem (item on the scene) pointer to void *
	\return pointer to an item on the scene
	*/
	OBJ ConvertValue(ItemHandle* item)
	{
		return (OBJ)(item);
	}
	/*! \brief convert void** to QList of QGraphicsItem (item on the scene) pointers
	\return list of pointers to items on the scene
	*/
	QList<ItemHandle*>* ConvertValue(Array A)
	{
		MainWindow * main = MainWindow::instance();
		QList<ItemHandle*> * list = new QList<ItemHandle*>();
		if (main)
		{
			SymbolsTable * table = main->currentSymbolsTable();
			if (table)
			{
				for (int i=0; A[i] != 0; ++i)
					if (table->isValidPointer(A[i]))
						(*list) += static_cast<ItemHandle*>(A[i]);
			}
		}
		return list;
	}
	/*! \brief convert to list of QGraphicsItem pointers to null-terminated array of void*
	\return null-terminated array of pointers to items on the scene
	*/
	Array ConvertValue(const QList<ItemHandle*>& list)
	{
		Array A = new OBJ[list.size()+1];
		//Array A = (OBJ*)_aligned_malloc( (1+list.size()) * sizeof(OBJ) ,16);
		A[list.size()] = 0; //null terminated
		for (int i=0; i < list.size(); ++i)
			A[i] = (OBJ)list[i];
		return A;
	}
	/*! \brief convert char* to QString
	\return Qt String
	*/
	QString ConvertValue(const char* c)
	{
		int sz = 0;
		for (int i=0; c != 0 && c[i] != 0; ++i) ++sz;
		return QString( QByteArray::fromRawData(c,sz) );
	}
	/*! \brief convert QString to null-terminated char*
	\return null-terminated char* 
	*/
	char* ConvertValue(const QString& s)
	{
		char * c = new char[s.length()+1];
		//char * c = (char*)_aligned_malloc( (s.length()+1) * sizeof(char) , 16);
		c[s.length()] = '\0';
		for (int i=0; i < s.length(); ++i)
			c[i] = s[i].toAscii();
		return c;
	}	
	/*! \brief convert char** to QStringList
	\return Qt StringList
	*/
	QStringList ConvertValue(char** c)
	{
		QStringList slist;
		for (int i=0; c != 0 && c[i] != 0; ++i)
			slist += QString(c[i]);
		return slist;
	}
	/*! \brief convert QStringList to null-terminated char**
	\return array of char* 
	*/
	char** ConvertValue(const QStringList& list)
	{
		//qDebug() << "converting string list";
		char ** cs = new char*[list.size() + 1];
		//char ** cs = (char**)_aligned_malloc((list.size() + 1)*sizeof(char*),16);
		cs[list.size()] = 0;
		for (int i=0; i < list.size(); ++i)
		{
			QString s = list[i];
			cs[i] = new char[s.length()+1];
			//cs[i] = (char*)_aligned_malloc((s.length()+1)*sizeof(char),16);
			cs[i][s.length()] = '\0';
			for (int j=0; j < s.length(); ++j)
				cs[i][j] = s[j].toAscii();
		}
		return cs;
	}
	/*! \brief convert matrix to datatable<double> (see DataTable.h and TCstructs.h)
	\return DataTable of qreals
	*/
	DataTable<qreal>* ConvertValue(Matrix m)
	{
		DataTable<qreal>* D = new DataTable<qreal>;
		if (m.rows < 0 || m.cols < 0) return D;
		try
		{
			D->resize(m.rows,m.cols);

			for (int i=0; i < m.rows && m.rownames && m.rownames[i]; ++i)
				D->rowName(i) = QString(m.rownames[i]);

			for (int i=0; i < m.cols && m.colnames && m.colnames[i]; ++i)
				D->colName(i) = QString(m.colnames[i]);

			for (int i=0; i < m.rows; ++i)
				for (int j=0; j < m.cols; ++j)
				{
					D->value(i,j) = getValue(m,i,j);
				}
		}
		catch(...)
		{
		}
		return D;
	}
	/*! \brief convert Datatable<double> to Matrix (see DataTable.h and TCstructs.h)
	\return Matrix with null-terminated rownames, colnames, values
	*/
	Matrix ConvertValue(const DataTable<qreal>& D)
	{
		Matrix m;

		m.rows = D.rows();
		m.cols = D.cols();		
		m.rownames = new char*[m.rows+1];
		//m.rownames = (char**)_aligned_malloc((m.rows+1)*sizeof(char*),16);
		m.rownames[m.rows] = 0;
		m.colnames = new char*[m.cols+1];
		//m.colnames = (char**)_aligned_malloc((m.cols+1)*sizeof(char*),16);
		m.colnames[m.cols] = 0;
		m.values = new double[m.rows * m.cols + 1];
		//m.values = (double*)_aligned_malloc((m.rows * m.cols)*sizeof(double),16);

		for (int i=0; i < m.rows; ++i)
		{
			QString s = D.rowName(i);			
			m.rownames[i] = new char[s.length()+1];
			//m.rownames[i] = (char*)_aligned_malloc((s.length()+1)*sizeof(char),16);
			m.rownames[i][s.length()] = '\0';
			for (int j=0; j < s.length(); ++j)
				m.rownames[i][j] = s[j].toAscii();
		}

		for (int i=0; i < m.cols; ++i)
		{
			QString s = D.colName(i);
			m.colnames[i] = new char[s.length()+1];
			//m.colnames[i] = (char*)_aligned_malloc((s.length()+1)*sizeof(char),16);
			m.colnames[i][s.length()] = '\0';
			for (int j=0; j < s.length(); ++j)
				m.colnames[i][j] = s[j].toAscii();
		}

		for (int i=0; i < m.rows; ++i)
			for (int j=0; j < m.cols; ++j)
			{
				setValue(m,i,j,D.at(i,j));
			}

			return m;
	}
}
