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

	Matrix emptyMatrix()
	{
		Matrix m;
		m.values = 0;
		m.rownames.length = 0;
		m.colnames.length = 0;
		m.rownames.strings = 0;
		m.colnames.strings = 0;
		m.cols = 0;
		m.rows = 0;
		return m;
	}

	ItemHandle* ConvertValue(void * o)
	{
		MainWindow * main = MainWindow::instance();
		if (main && 
			main->currentSymbolsTable() && 
			main->currentSymbolsTable()->isValidPointer(o)
			)
			return static_cast<ItemHandle*>(o);
		return 0;
	}

	void * ConvertValue(ItemHandle* item)
	{
		return (void *)(item);
	}

	QList<ItemHandle*>* ConvertValue(ArrayOfItems A)
	{
		MainWindow * main = MainWindow::instance();
		QList<ItemHandle*> * list = new QList<ItemHandle*>();
		if (main)
		{
			SymbolsTable * table = main->currentSymbolsTable();
			if (table)
			{
				for (int i=0; i < A.length; ++i)
					if (table->isValidPointer(A.items[i]))
						(*list) += static_cast<ItemHandle*>(A.items[i]);
			}
		}
		return list;
	}

	ArrayOfItems ConvertValue(const QList<ItemHandle*>& list)
	{
		ArrayOfItems A;
		A.length = list.size();
		A.items = 0;
		if (A.length > 0)
		{
			A.items = new void*[A.length];
			for (int i=0; i < list.size(); ++i)
				A.items[i] = (void*)list[i];
		}
		return A;
	}

	QString ConvertValue(const char* c)
	{
		int sz = 0;
		for (int i=0; c != 0 && c[i] != 0; ++i) ++sz;
		return QString( QByteArray::fromRawData(c,sz) );
	}

	char* ConvertValue(const QString& s)
	{
		char * c = new char[s.length()+1];
		//char * c = (char*)_aligned_malloc( (s.length()+1) * sizeof(char) , 16);
		c[s.length()] = '\0';
		for (int i=0; i < s.length(); ++i)
			c[i] = s[i].toAscii();
		return c;
	}	

	DataTable<qreal>* ConvertValue(Matrix m)
	{
		DataTable<qreal>* D = new DataTable<qreal>;
		if (m.rows < 0 || m.cols < 0) return D;
		D->resize(m.rows,m.cols);

		for (int i=0; i < m.rows && m.rownames.strings && m.rownames.strings[i]; ++i)
			D->rowName(i) = QString(m.rownames.strings[i]);

		for (int i=0; i < m.cols && m.colnames.strings && m.colnames.strings[i]; ++i)
			D->colName(i) = QString(m.colnames.strings[i]);

		for (int i=0; i < m.rows; ++i)
			for (int j=0; j < m.cols; ++j)
			{
				D->value(i,j) = getValue(m,i,j);
			}

		return D;
	}
	
	Matrix ConvertValue(const DataTable<qreal>& D)
	{
		Matrix m;

		m.rownames.length = m.rows = D.rows();
		m.colnames.length = m.cols = D.cols();
		
		if (m.rows > 0)
			m.rownames.strings = new char*[m.rows];	
		else
			m.rownames.strings = 0;
			
		if (m.cols > 0)			
			m.colnames.strings = new char*[m.cols];
		else
			m.colnames.strings = 0;
		
		if (m.rows > 0 && m.cols > 0)
			m.values = new double[m.rows * m.cols];
		else
			m.values = 0;

		for (int i=0; i < m.rows; ++i)
		{
			QString s = D.rowName(i);			
			m.rownames.strings[i] = new char[s.length()+1];
			m.rownames.strings[i][s.length()] = '\0';
			for (int j=0; j < s.length(); ++j)
				m.rownames.strings[i][j] = s[j].toAscii();
		}

		for (int i=0; i < m.cols; ++i)
		{
			QString s = D.colName(i);
			m.colnames.strings[i] = new char[s.length()+1];
			m.colnames.strings[i][s.length()] = '\0';
			for (int j=0; j < s.length(); ++j)
				m.colnames.strings[i][j] = s[j].toAscii();
		}

		for (int i=0; i < m.rows; ++i)
			for (int j=0; j < m.cols; ++j)
			{
				setValue(m,i,j,D.at(i,j));
			}

		return m;
	}
	
	DataTable<QString>* ConvertValue(TableOfStrings m)
	{
		DataTable<QString>* D = new DataTable<QString>;
		if (m.rows < 0 || m.cols < 0) return D;
		D->resize(m.rows,m.cols);

		for (int i=0; i < m.rows && m.rownames.strings && m.rownames.strings[i]; ++i)
			D->rowName(i) = QString(m.rownames.strings[i]);

		for (int i=0; i < m.cols && m.colnames.strings && m.colnames.strings[i]; ++i)
			D->colName(i) = QString(m.colnames.strings[i]);

		for (int i=0; i < m.rows; ++i)
			for (int j=0; j < m.cols; ++j)
			{
				D->value(i,j) = QString(getString(m,i,j));
			}

		return D;
	}
	
	TableOfStrings ConvertValue(const DataTable<QString>& D)
	{
		TableOfStrings m;

		m.rownames.length = m.rows = D.rows();
		m.colnames.length = m.cols = D.cols();		
		
		if (m.rows > 0)
			m.rownames.strings = new char*[m.rows];	
		else
			m.rownames.strings = 0;
			
		if (m.cols > 0)
			m.colnames.strings = new char*[m.cols];
		else
			m.colnames.strings = 0;
		
		if (m.rows > 0 && m.cols > 0)
			m.strings = new char*[m.rows * m.cols];
		else
			m.strings = 0;

		for (int i=0; i < m.rows; ++i)
		{
			QString s = D.rowName(i);			
			m.rownames.strings[i] = new char[s.length()+1];
			m.rownames.strings[i][s.length()] = '\0';
			for (int j=0; j < s.length(); ++j)
				m.rownames.strings[i][j] = s[j].toAscii();
		}

		for (int i=0; i < m.cols; ++i)
		{
			QString s = D.colName(i);
			m.colnames.strings[i] = new char[s.length()+1];
			m.colnames.strings[i][s.length()] = '\0';
			for (int j=0; j < s.length(); ++j)
				m.colnames.strings[i][j] = s[j].toAscii();
		}

		for (int i=0; i < m.rows; ++i)
			for (int j=0; j < m.cols; ++j)
			{
				setString(m,i,j,D.at(i,j).toAscii().data());
			}

		return m;
	}
	
	QStringList ConvertValue(ArrayOfStrings c)
	{
		QStringList slist;
		for (int i=0; i < c.length && c.strings && c.strings[i]; ++i)
			slist += QString(c.strings[i]);
		return slist;
	}

	ArrayOfStrings ConvertValue(const QStringList& list)
	{
		ArrayOfStrings A;
		if (list.size() < 1)
		{
			A.length = 0;
			A.strings = 0;
			return A;
		}
		char ** cs = new char*[list.size()];
		cs[list.size()] = 0;
		for (int i=0; i < list.size(); ++i)
		{
			QString s = list[i];
			cs[i] = new char[s.length()+1];
			cs[i][s.length()] = '\0';
			for (int j=0; j < s.length(); ++j)
				cs[i][j] = s[j].toAscii();
		}
		A.length = list.size();
		A.strings = cs;
		return A;
	}
}

