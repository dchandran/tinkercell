/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines the DataTable class. 
The DataTable is a template class that contains a 2D vector (values) and two vectors
of QStrings (row names and column names). It provides various functions for manipulating
the table. 

****************************************************************************/

#ifndef TINKERCELL_DATATABLE_H
#define TINKERCELL_DATATABLE_H

#include <QList>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QUndoCommand>
#include <QDebug>

#ifdef Q_WS_WIN
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{
	/*! \brief DataTable is a 2D vector with row names and column names	
	\ingroup core
	*/
	template <typename T>
	class TINKERCELLEXPORT DataTable 
	{
	protected:
		/*! \brief the values in the table*/
		QVector<T> dataMatrix;
		/*! \brief the column names*/
		QVector<QString> colHeaders;
		/*! \brief the row names*/
		QVector<QString> rowHeaders;
		/*! \brief a description of this table (optional)*/
		QString desc;
	public:
	
		/*! \brief get description of this table*/		
		virtual QString description() const;
		
		/*! \brief get or set description of this table*/
		virtual QString& description();
		
		/*! \brief get the column names
		\return QVector reference to the actural column names
		*/
		virtual const QVector<QString>& colNames() const;
		/*! \brief get the row names
		\return QVector reference to the actural row names
		*/
		virtual const QVector<QString>& rowNames() const;
		/*! \brief get the column names
		\return QStringList column names as QStringList (copy)
		*/
		virtual QStringList getColNames() const;
		/*! \brief get the row names
		\return QStringList row names as QStringList (copy)
		*/
		virtual QStringList tc_getRowNames() const;
		/*! \brief get the ith column name reference. can be used to change the column name
		\param int col number
		\return QString reference to the ith column name
		*/
		virtual QString& colName(int i);
		
		/*! \brief get the ith row name reference. can be used to change the row name
		\param int col number
		\return QString copy to the ith row name
		*/
		virtual QString rowName(int i) const;
		
		/*! \brief get the ith column name. cannot be used to change the column name
		\param int col number
		\return QString copy of the ith column name
		*/
		virtual QString colName(int i) const;
		
		/*! \brief get the ith row name. cannot be used to change the row name
		\param int row number
		\return QString reference of the ith row name
		*/
		virtual QString& rowName(int i);
		
		/*! \brief set all the column names. 
		\param QVector vector of strings
		\return void
		*/
		virtual void setColNames(const QVector<QString>& names);
		
		/*! \brief set all the row names.
		\param QVector vector of strings
		\return void
		*/
		virtual void tc_setRowNames(const QVector<QString>& names);
		
		/*! \brief set all the column names. 
		\param QVector vector of strings
		\return void
		*/
		virtual void setColNames(const QStringList& names);
		
		/*! \brief set all the row names. 
		\param QVector vector of strings
		\return void
		*/
		virtual void tc_setRowNames(const QStringList& names);
		
		/*! \brief get the number of rows
		\return int number of rows
		*/
		virtual int rows() const;
		
		/*! \brief get the number of columns
		\return int number of columns
		*/
		virtual int cols() const;
		
		/*! \brief get the value at the ith row and jth column. can also be used to set the value
		\param int row number (i)
		\param int column number (j)
		\return T reference to value at ith row and jth column. returns value at 0 if i or j are not inside the table
		*/
		virtual T& value (int i, int j) ;
		
		/*! \brief get the value using row and column names. can also be used to set the value. Slower than
		using value(int,int)
		\param QString row name
		\param QString column name
		\return T reference to value at given row and column. returns value at 0 if row and column are not in the table
		*/
		virtual T& value (const QString& r, const QString& c);
		
		/*! \brief get the value using row name. can also be used to set the value. Slower than
		using value(int,int)
		\param QString row name
		\param int column number
		\return T reference to value at given row and column. returns value at 0 if row and column are not in the table
		*/
		virtual T& value (const QString& r, int j);
		
		/*! \brief get the value using column name. can also be used to set the value. Slower than
		using value(int,int)
		\param int row number
		\param QString column name
		\return T reference to value at given row and column. returns value at 0 if row and column are not in the table
		*/
		virtual T& value (int i, const QString& c);
		
		/*! \brief checks if the two data table's headers and contents are the same
		\param DataTable<T>
		\return bool
		*/
		virtual bool operator == (const DataTable<T>& D);
		
		/*! \brief exactly opposite of operator == 
		\param DataTable<T>
		\return bool
		*/
		virtual bool operator != (const DataTable<T>& D);
		
		/*! \brief get the value using row and column number. cannot also be used to set the value. 
		\param int row number
		\param int column number
		\return T copy of value at given row and column. returns value at 0 if row and column are not in the table
		*/
		virtual T at (int i, int j) const;
		
		/*! \brief get the value using row and column name. cannot also be used to set the value. 
		\param QString row name
		\param QString column name
		\return T copy of value at given row and column. returns value at 0 if row and column are not in the table
		*/
		virtual T at (const QString& r, const QString& c) const;
		
		/*! \brief get the value using row name. cannot also be used to set the value. 
		\param QString row name
		\param int column number
		\return T copy of value at given row and column. returns value at 0 if row and column are not in the table
		*/
		virtual T at (const QString& r, int j) const;
		
		/*! \brief get the value using column name. cannot also be used to set the value. 
		\param int row number
		\param int column name
		\return T copy of value at given row and column. returns value at 0 if row and column are not in the table
		*/
		virtual T at (int i, const QString& c) const;
		
		/*! \brief set the size of the data table 
		\param int row count
		\param int column count
		\return void
		*/
		virtual void resize(int m, int n);
		
		/*! \brief insert a new row at the given location with the given name. Insertion will fail if there is already
		a row with the same name
		\param int row number
		\param QString row name
		\return int 0 if failed, 1 if successful
		*/
		virtual int insertRow(int k, const QString& row);
		
		/*! \brief insert a new column at the given location with the given name. Insertion will fail if there is already
		a column with the same name
		\param int column number
		\param QString column name
		\return int 0 if failed, 1 if successful
		*/
		virtual int insertCol(int k, const QString& col);
		
		/*! \brief remove an existing row at the given index.
		\param int row number
		\return int 0 if failed, 1 if successful
		*/
		virtual int removeRow(int k);
		
		/*! \brief remove an existing row with the given name.
		\param QString row name
		\return int 0 if failed, 1 if successful
		*/
		virtual int removeRow(const QString& name);
		
		/*! \brief remove an existing column at the given index.
		\param int column number
		\return int 0 if failed, 1 if successful
		*/
		virtual int removeCol(int k);
		
		/*! \brief remove an existing col with the given name.
		\param QString row name
		\return int 0 if failed, 1 if successful
		*/
		virtual int removeCol(const QString& name);
		
		/*! \brief swap two rows. Nothing will happen if the given numbers are outside the table
		\param int first row number
		\param int second row number
		\return void
		*/
		virtual void swapRows(int i1, int i2);
		
		/*! \brief swap two columns. Nothing will happen if the given numbers are outside the table
		\param int first column number
		\param int second column number
		\return void
		*/
		virtual void swapCols(int j1, int j2);
		
		/*! \brief swap two rows using their name. Nothing will happen if the given numbers are outside the table
		\param int first row name
		\param int second row name
		\return void
		*/
		virtual void swapRows(const QString& s1, const QString& s2);
		
		/*! \brief swap two columns using their name. Nothing will happen if the given numbers are outside the table
		\param int first column name
		\param int second column name
		\return void
		*/
		virtual void swapCols(const QString& s1, const QString& s2);
		
		/*! \brief get transpose of the table. complexity = n*m (use sparingly)
		\return new data table
		*/
		virtual DataTable<T> transpose() const;

	};


	/*! 
		\brief This template class allows undo and redo of a change made to a data table. 
		\ingroup undo 
	*/
	template <typename T>
	class TINKERCELLEXPORT ChangeDataCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		\param name of the change
		\param old tables
		\param new tables
		*/
		ChangeDataCommand(const QString& name, DataTable<T>* oldDataTable, const DataTable<T>* newDataTable);
		/*! \brief constructor
		\param name of the change
		\param old table
		\param new table 
		*/
		ChangeDataCommand(const QString& name,const QList< DataTable<T>* >& oldDataTable,const QList< DataTable<T>* >& newDataTable);
		/*! \brief redo the changes*/
		void redo();
		/*! \brief undo the changes*/
		void undo();
		/*! \brief pointers to target tables*/
		QList< DataTable<T>* > targetDataTable;
		/*! \brief new tables*/
		QList< DataTable<T> > newDataTable;
		/*! \brief old tables*/
		QList< DataTable<T> > oldDataTable;
	};

	/*! \brief Changes two different data tables.
		\ingroup undo
	*/
	template <typename T1, typename T2>
	class TINKERCELLEXPORT Change2DataCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		\param name of the command
		\param old table of type T1
		\param new table of type T1
		\param old table of type T2
		\param new table of type T2
		*/
		Change2DataCommand(const QString& name, DataTable<T1>* oldDataTable1, const DataTable<T1>* newDataTable1, DataTable<T2>* oldDataTable2, const DataTable<T2>* newDataTable2);
		/*! \brief constructor
		\param name of the command
		\param old tables of type T1
		\param new tables of type T1
		\param old tables of type T2
		\param new tables of type T2
		*/
		Change2DataCommand(const QString& name,const QList< DataTable<T1>* >& oldDataTable1,const QList< DataTable<T1>* >& newDataTable1,const QList< DataTable<T2>* >& oldDataTable2,const QList< DataTable<T2>* >& newDataTable2);
		/*! \brief redo the changes*/
		void redo();
		/*! \brief undo the changes*/
		void undo();
		/*! \brief target tables of type T1*/
		QList< DataTable<T1>* > targetDataTable1;
		/*! \brief new tables of type T1*/
		QList< DataTable<T1> > newDataTable1;
		/*! \brief old tables of type T1*/
		QList< DataTable<T1> > oldDataTable1;
		/*! \brief target tables of type T2*/
		QList< DataTable<T2>* > targetDataTable2;
		/*! \brief new tables of type T2*/
		QList< DataTable<T2> > newDataTable2;
		/*! \brief old tables of type T2*/
		QList< DataTable<T2> > oldDataTable2;
	};
	
	
	template <typename T> QString DataTable<T>::description() const { return desc; }
		
	/*! \brief get or set description of this table*/
	template <typename T>  QString& DataTable<T>::description() { return desc; }
	
	/*! \brief get the column names
	\return QVector reference to the actural column names
	*/
	template <typename T>  const QVector<QString>& DataTable<T>::colNames() const { return colHeaders; }
	/*! \brief get the row names
	\return QVector reference to the actural row names
	*/
	template <typename T>  const QVector<QString>& DataTable<T>::rowNames() const { return rowHeaders; }
	/*! \brief get the column names
	\return QStringList column names as QStringList (copy)
	*/
	template <typename T>  QStringList DataTable<T>::getColNames() const { return QStringList::fromVector(colHeaders); }
	/*! \brief get the row names
	\return QStringList row names as QStringList (copy)
	*/
	template <typename T>  QStringList DataTable<T>::tc_getRowNames() const { return QStringList::fromVector(rowHeaders); }
	/*! \brief get the ith column name reference. can be used to change the column name
	\param int col number
	\return QString reference to the ith column name
	*/
	template <typename T>  QString& DataTable<T>::colName(int i) 
	{ 
		if (i < 0 || i >= colHeaders.size())
		{
			if (colHeaders.isEmpty())
				colHeaders += QString();
			return colHeaders[ colHeaders.size() - 1 ];
		}
		return colHeaders[i];
	}		
	/*! \brief get the ith row name reference. can be used to change the row name
	\param int col number
	\return QString copy to the ith row name
	*/
	template <typename T>  QString DataTable<T>::rowName(int i) const
	{ 
		if (i < 0 || i >= rowHeaders.size())
		{
			return QString();
		}
		return rowHeaders.at(i);
	}
	/*! \brief get the ith column name. cannot be used to change the column name
	\param int col number
	\return QString copy of the ith column name
	*/
	template <typename T>  QString DataTable<T>::colName(int i) const
	{ 
		if (i < 0 || i >= colHeaders.size())
			return QString();
		return colHeaders.at(i);
	}
	/*! \brief get the ith row name. cannot be used to change the row name
	\param int row number
	\return QString reference of the ith row name
	*/
	template <typename T>  QString& DataTable<T>::rowName(int i) 
	{ 
		if (i < 0 || i >= rowHeaders.size())
		{
			if (rowHeaders.isEmpty())
				rowHeaders += QString();
			return rowHeaders[ rowHeaders.size() - 1 ];
		}
		return rowHeaders[i];
	}
	/*! \brief set all the column names. 
	\param QVector vector of strings
	\return void
	*/
	template <typename T>  void DataTable<T>::setColNames(const QVector<QString>& names)
	{
		if (names.size() != colHeaders.size())
			resize(rowHeaders.size(),names.size());
		colHeaders = names;
	}
	/*! \brief set all the row names.
	\param QVector vector of strings
	\return void
	*/
	template <typename T>  void DataTable<T>::tc_setRowNames(const QVector<QString>& names)
	{
		if (names.size() != colHeaders.size())
			resize(names.size(),colHeaders.size());
		rowHeaders = names;	
	}
	/*! \brief set all the column names. 
	\param QVector vector of strings
	\return void
	*/
	template <typename T>  void DataTable<T>::setColNames(const QStringList& names)
	{
		if (names.size() != colHeaders.size())
			resize(rowHeaders.size(),names.size());
		for (int i=0; i < names.size(); ++i)
			colHeaders[i] = names[i];
	}
	/*! \brief set all the row names. 
	\param QVector vector of strings
	\return void
	*/
	template <typename T>  void DataTable<T>::tc_setRowNames(const QStringList& names)
	{
		if (names.size() != rowHeaders.size())
			resize(names.size(),colHeaders.size());
		for (int i=0; i < names.size(); ++i)
			rowHeaders[i] = names[i];	
	}
	/*! \brief get the number of rows
	\return int number of rows
	*/
	template <typename T>  int DataTable<T>::rows() const
	{
		return rowHeaders.size();
	}
	/*! \brief get the number of columns
	\return int number of columns
	*/
	template <typename T>  int DataTable<T>::cols() const
	{
		return colHeaders.size();
	}
	/*! \brief get the value at the ith row and jth column. can also be used to set the value
	\param int row number (i)
	\param int column number (j)
	\return T reference to value at ith row and jth column. returns value at 0 if i or j are not inside the table
	*/
	template <typename T>  T& DataTable<T>::value (int i, int j) 
	{
		if (i < 0) i = 0;
		if (j < 0) j = 0;

		int k = i*colHeaders.size()+j;
		if (k >= dataMatrix.size() || j >= colHeaders.size() || i >= rowHeaders.size()) 
		{
			int m = rowHeaders.size(), n = colHeaders.size();
			if (i >= rowHeaders.size()) m = i + 1;
			if (j >= colHeaders.size()) n = j + 1;
			while (k >= m*n) ++m;
			resize(m,n);
		}

		return dataMatrix[k];
	}
	/*! \brief get the value using row and column names. can also be used to set the value. Slower than
	using value(int,int)
	\param QString row name
	\param QString column name
	\return T reference to value at given row and column. returns value at 0 if row and column are not in the table
	*/
	template <typename T>  T& DataTable<T>::value (const QString& r, const QString& c)
	{
		int m = rowHeaders.size(), n = colHeaders.size();
		if (!rowHeaders.contains(r))
			++m;
		if (!colHeaders.contains(c))
			++n;
		if (m != rowHeaders.size() || n != colHeaders.size())
		{
			resize(m,n);
			if (!rowHeaders.contains(r))
				rowHeaders[ rowHeaders.size()-1 ] = r;
			if (!colHeaders.contains(c))
				colHeaders[ colHeaders.size()-1 ] = c;
		}
		int i = rowHeaders.indexOf(r),
			j = colHeaders.indexOf(c);
		return value(i,j);
	}
	/*! \brief get the value using row name. can also be used to set the value. Slower than
	using value(int,int)
	\param QString row name
	\param int column number
	\return T reference to value at given row and column. returns value at 0 if row and column are not in the table
	*/
	template <typename T>  T& DataTable<T>::value (const QString& r, int j)
	{
		int m = rowHeaders.size(), n = colHeaders.size();
		if (!rowHeaders.contains(r))
			++m;
		if (m != rowHeaders.size() || n != colHeaders.size())
		{
			resize(m,n);
			if (!rowHeaders.contains(r))
				rowHeaders[ rowHeaders.size()-1 ] = r;
		}
		int i = rowHeaders.indexOf(r);
		return value(i,j);
	}
	/*! \brief get the value using column name. can also be used to set the value. Slower than
	using value(int,int)
	\param int row number
	\param QString column name
	\return T reference to value at given row and column. returns value at 0 if row and column are not in the table
	*/
	template <typename T>  T& DataTable<T>::value (int i, const QString& c)
	{
		int m = rowHeaders.size(), n = colHeaders.size();
		if (!colHeaders.contains(c))
			++n;
		if (m != rowHeaders.size() || n != colHeaders.size())
		{
			resize(m,n);
			if (!colHeaders.contains(c))
				colHeaders[ colHeaders.size()-1 ] = c;
		}
		int j = colHeaders.indexOf(c);
		return value(i,j);
	}
	/*! \brief checks if the two data table's headers and contents are the same
	\param DataTable<T>
	\return bool
	*/
	template <typename T>  bool DataTable<T>::operator == (const DataTable<T>& D)
	{
		return dataMatrix == D.dataMatrix && colHeaders == D.colHeaders && rowHeaders == D.rowHeaders;
	}
	/*! \brief exactly opposite of operator == 
	\param DataTable<T>
	\return bool
	*/
	template <typename T>  bool DataTable<T>::operator != (const DataTable<T>& D)
	{
		return !( operator==(D) );
	}
	/*! \brief get the value using row and column number. cannot also be used to set the value. 
	\param int row number
	\param int column number
	\return T copy of value at given row and column. returns value at 0 if row and column are not in the table
	*/
	template <typename T>  T DataTable<T>::at (int i, int j) const
	{
		if (i < 0) i = 0;
		if (j < 0) j = 0;

		int k = i*colHeaders.size()+j;
		if (k >= dataMatrix.size() || j >= colHeaders.size() || i >= rowHeaders.size()) 
		{
			return T();
		}

		return dataMatrix.at(k);
	}
	/*! \brief get the value using row and column name. cannot also be used to set the value. 
	\param QString row name
	\param QString column name
	\return T copy of value at given row and column. returns value at 0 if row and column are not in the table
	*/
	template <typename T>  T DataTable<T>::at (const QString& r, const QString& c) const
	{
		int i = rowHeaders.indexOf(r),
			j = colHeaders.indexOf(c);
		return at(i,j);
	}
	/*! \brief get the value using row name. cannot also be used to set the value. 
	\param QString row name
	\param int column number
	\return T copy of value at given row and column. returns value at 0 if row and column are not in the table
	*/
	template <typename T>  T DataTable<T>::at (const QString& r, int j) const
	{
		int i = rowHeaders.indexOf(r);
		return at(i,j);
	}
	/*! \brief get the value using column name. cannot also be used to set the value. 
	\param int row number
	\param int column name
	\return T copy of value at given row and column. returns value at 0 if row and column are not in the table
	*/
	template <typename T>  T DataTable<T>::at (int i, const QString& c) const
	{
		int j = colHeaders.indexOf(c);
		return at(i,j);
	}
	/*! \brief set the size of the data table 
	\param int row count
	\param int column count
	\return void
	*/
	template <typename T>  void DataTable<T>::resize(int m, int n)
	{
		if (m < 0 || n < 0) return;
		int m0 = rowHeaders.size(), n0 = colHeaders.size();
		QVector<T> oldMatrix(dataMatrix);
		dataMatrix.clear();
		dataMatrix.resize(m*n);
		rowHeaders.resize(m);
		colHeaders.resize(n);

		int k0=0, k1=0;	
		for (int i=0; i < m0 && i < m; ++i)
			for (int j=0; j < n0 && j < n; ++j)
			{
				k0 = i*n0 + j;
				k1 = i*n + j;
				dataMatrix[k1] = oldMatrix[k0];
			}
	}
	/*! \brief insert a new row at the given location with the given name. Insertion will fail if there is already
	a row with the same name
	\param int row number
	\param QString row name
	\return int 0 if failed, 1 if successful
	*/
	template <typename T>  int DataTable<T>::insertRow(int k, const QString& row)
	{
		if (rowHeaders.contains(row)) return 0;

		int n = rowHeaders.size()+1;

		QVector<T> dataMatrix2( n * colHeaders.size() );

		for (int i=0; i < rowHeaders.size(); ++i)
			for (int j=0; j < colHeaders.size(); ++j)
			{
				if (i < k)
					dataMatrix2[ i*colHeaders.size()+j ] = dataMatrix[ i*colHeaders.size()+j ];
				else
					dataMatrix2[ (i+1)*colHeaders.size()+j ] = dataMatrix[ i*colHeaders.size()+j ];
			}

			dataMatrix = dataMatrix2;
			rowHeaders.insert(k,row);

			return 1;			
	}
	/*! \brief insert a new column at the given location with the given name. Insertion will fail if there is already
	a column with the same name
	\param int column number
	\param QString column name
	\return int 0 if failed, 1 if successful
	*/
	template <typename T>  int DataTable<T>::insertCol(int k, const QString& col)
	{
		if (colHeaders.contains(col)) return 0;

		int n = colHeaders.size()+1;

		QVector<T> dataMatrix2( n * rowHeaders.size() );

		for (int i=0; i < rowHeaders.size(); ++i)
			for (int j=0; j < colHeaders.size(); ++j)
			{
				if (j < k)
					dataMatrix2[ i*n+j ] = dataMatrix[ i*colHeaders.size()+j ];
				else
					dataMatrix2[ i*n+j+1 ] = dataMatrix[ i*colHeaders.size()+j ];
			}

			dataMatrix = dataMatrix2;
			colHeaders.insert(k,col);

			return 1;			
	}
	/*! \brief remove an existing row at the given index.
	\param int row number
	\return int 0 if failed, 1 if successful
	*/
	template <typename T>  int DataTable<T>::removeRow(int k)
	{
		if (k < 0 || k >= rowHeaders.size()) return 0;

		int n = rowHeaders.size()-1;

		QVector<T> dataMatrix2( n * colHeaders.size() );

		for (int i=0; i < rowHeaders.size(); ++i)
			for (int j=0; j < colHeaders.size(); ++j)
			{
				if (i < k)
					dataMatrix2[ i*colHeaders.size()+j ] = dataMatrix[ i*colHeaders.size()+j ];
				else
					if (i > k)
						dataMatrix2[ (i-1)*colHeaders.size()+j ] = dataMatrix[ i*colHeaders.size()+j ];
			}

			dataMatrix = dataMatrix2;
			rowHeaders.remove(k);

			return 1;
	}
	/*! \brief remove an existing row with the given name.
	\param QString row name
	\return int 0 if failed, 1 if successful
	*/
	template <typename T>  int DataTable<T>::removeRow(const QString& name)
	{
		if (rowHeaders.contains(name))
			return removeRow(rowHeaders.indexOf(name));
		return -1;
	}
	/*! \brief remove an existing column at the given index.
	\param int column number
	\return int 0 if failed, 1 if successful
	*/
	template <typename T>  int DataTable<T>::removeCol(int k)
	{
		if (k < 0 || k >= colHeaders.size()) return 0;

		int n = colHeaders.size()-1;

		QVector<T> dataMatrix2( n * rowHeaders.size() );

		for (int i=0; i < rowHeaders.size(); ++i)
			for (int j=0; j < colHeaders.size(); ++j)
			{
				if (j < k)
					dataMatrix2[ i*n+j ] = dataMatrix[ i*colHeaders.size()+j ];
				else
					if (j > k)
						dataMatrix2[ i*n+j-1 ] = dataMatrix[ i*colHeaders.size()+j ];
			}

			dataMatrix = dataMatrix2;
			colHeaders.remove(k);

			return 1;
	}
	/*! \brief remove an existing col with the given name.
	\param QString row name
	\return int 0 if failed, 1 if successful
	*/
	template <typename T>  int DataTable<T>::removeCol(const QString& name)
	{
		if (colHeaders.contains(name))
			return removeCol(colHeaders.indexOf(name));
		return -1;
	}
	/*! \brief swap two rows. Nothing will happen if the given numbers are outside the table
	\param int first row number
	\param int second row number
	\return void
	*/
	template <typename T>  void DataTable<T>::swapRows(int i1, int i2)
	{
		if (i1 < 0 || i2 < 0 || i1 >= rowHeaders.size() || i2 >= rowHeaders.size()) return;

		QString str = rowHeaders[i1];
		rowHeaders[i1] = rowHeaders[i2];
		rowHeaders[i2] = str;

		int n = colHeaders.size();
		int k1,k2;
		T temp;
		for (int j=0; j < n; ++j)
		{
			k1 = i1*n + j;
			k2 = i2*n + j;
			temp = dataMatrix[k1];
			dataMatrix[k1] = dataMatrix[k2];
			dataMatrix[k2] = temp;
		}
	}
	/*! \brief swap two columns. Nothing will happen if the given numbers are outside the table
	\param int first column number
	\param int second column number
	\return void
	*/
	template <typename T>  void DataTable<T>::swapCols(int j1, int j2)
	{
		if (j1 < 0 || j2 < 0 || j1 >= colHeaders.size() || j2 >= colHeaders.size()) return;

		QString str = colHeaders[j1];
		colHeaders[j1] = colHeaders[j2];
		colHeaders[j2] = str;

		int n = colHeaders.size(), m = rowHeaders.size();
		int k1,k2;
		T temp;
		for (int i=0; i < m; ++i)
		{
			k1 = i*n + j1;
			k2 = i*n + j2;
			temp = dataMatrix[k1];
			dataMatrix[k1] = dataMatrix[k2];
			dataMatrix[k2] = temp;
		}
	}
	/*! \brief swap two rows using their name. Nothing will happen if the given numbers are outside the table
	\param int first row name
	\param int second row name
	\return void
	*/
	template <typename T>  void DataTable<T>::swapRows(const QString& s1, const QString& s2)
	{
		swapRows( rowHeaders.indexOf(s1), rowHeaders.indexOf(s2) );
	}
	/*! \brief swap two columns using their name. Nothing will happen if the given numbers are outside the table
	\param int first column name
	\param int second column name
	\return void
	*/
	template <typename T>  void DataTable<T>::swapCols(const QString& s1, const QString& s2)
	{
		swapCols( colHeaders.indexOf(s1), colHeaders.indexOf(s2) );
	}
	/*! \brief get transpose of the table. complexity = n*m (use sparingly)
	\return new data table
	*/
	template <typename T>  DataTable<T> DataTable<T>::transpose() const
	{
		DataTable<T> newTable;
		newTable.resize( cols(), rows() );

		newTable.colHeaders = rowHeaders;
		newTable.rowHeaders = colHeaders;

		for (int i=0; i < rows(); ++i)
			for (int j=0; j < cols(); ++j)
				newTable.value(j,i) = at(i,j);

		return newTable;
	}


	template <typename T>
	ChangeDataCommand<T>::ChangeDataCommand(const QString& name, DataTable<T>* oldDataTable, const DataTable<T>* newDataTable)
		: QUndoCommand(name)
	{
		if (newDataTable && oldDataTable)
		{
			this->newDataTable += *newDataTable;
			this->oldDataTable += *oldDataTable;
			this->targetDataTable += oldDataTable;
		}
	}
	
	template <typename T>
	ChangeDataCommand<T>::ChangeDataCommand(const QString& name,const QList< DataTable<T>* >& oldDataTable,const QList< DataTable<T>* >& newDataTable)
		: QUndoCommand(name)
	{
		for (int i=0; i < oldDataTable.size() && i < newDataTable.size(); ++i)
			if (newDataTable[i] && oldDataTable[i])
			{
				this->newDataTable += *(newDataTable[i]);
				this->oldDataTable += *(oldDataTable[i]);
				this->targetDataTable += (oldDataTable[i]);
			}
	}
	
	template <typename T>
	void ChangeDataCommand<T>::redo()
	{
		for (int i=0; i < targetDataTable.size() && i < newDataTable.size(); ++i)
			if (targetDataTable[i])
				(*(targetDataTable[i])) = newDataTable[i];
	}
	
	template <typename T>
	void ChangeDataCommand<T>::undo()
	{
		for (int i=0; i < targetDataTable.size() && i < oldDataTable.size(); ++i)
			if (targetDataTable[i])
				(*(targetDataTable[i])) = oldDataTable[i];
	}

	template <typename T1, typename T2>
	Change2DataCommand<T1,T2>::Change2DataCommand(const QString& name, DataTable<T1>* oldDataTable1, const DataTable<T1>* newDataTable1, DataTable<T2>* oldDataTable2, const DataTable<T2>* newDataTable2)
		: QUndoCommand(name)
	{
		if (newDataTable1 && oldDataTable1)
		{
			DataTable<T1> dat1n(*(newDataTable1));
			DataTable<T1> dat1o(*(oldDataTable1));

			this->newDataTable1 += dat1n;
			this->oldDataTable1 += dat1o;
			this->targetDataTable1 += oldDataTable1;
		}

		if (newDataTable2 && oldDataTable2)
		{
			DataTable<T2> dat2n(*(newDataTable2));
			DataTable<T2> dat2o(*(oldDataTable2));

			this->newDataTable2 += dat2n;
			this->oldDataTable2 += dat2o;
			this->targetDataTable2 += oldDataTable2;
		}
	}

	template <typename T1, typename T2>
	Change2DataCommand<T1,T2>::Change2DataCommand(const QString& name,const QList< DataTable<T1>* >& oldDataTable1,const QList< DataTable<T1>* >& newDataTable1,const QList< DataTable<T2>* >& oldDataTable2,const QList< DataTable<T2>* >& newDataTable2)
		: QUndoCommand(name)
	{
		for (int i=0; i < newDataTable1.size() && i < oldDataTable1.size(); ++i)
			if (newDataTable1[i] && oldDataTable1[i])
			{
				DataTable<T1> dat1n(*(newDataTable1[i]));
				DataTable<T1> dat1o(*(oldDataTable1[i]));

				this->newDataTable1 += dat1n;
				this->oldDataTable1 += dat1o;
				this->targetDataTable1 += (oldDataTable1.at(i));
			}

			for (int i=0; i < newDataTable2.size() && i < oldDataTable2.size(); ++i)
				if (newDataTable2[i] && oldDataTable2[i])
				{
					DataTable<T2> dat2n(*(newDataTable2[i]));
					DataTable<T2> dat2o(*(oldDataTable2[i]));

					this->newDataTable2 += dat2n;
					this->oldDataTable2 += dat2o;
					this->targetDataTable2 += (oldDataTable2[i]);
				}
	}
	
	template <typename T1, typename T2>
	void Change2DataCommand<T1,T2>::redo()
	{
		for (int i=0; i < targetDataTable1.size() && i < newDataTable1.size(); ++i)
			if (targetDataTable1[i])
				(*(targetDataTable1[i])) = newDataTable1[i];

		for (int i=0; i < targetDataTable2.size() && i < newDataTable2.size(); ++i)
			if (targetDataTable2[i])
				(*(targetDataTable2[i])) = newDataTable2[i];
	}
	
	template <typename T1, typename T2>
	void Change2DataCommand<T1,T2>::undo()
	{
		for (int i=0; i < targetDataTable1.size() && i < oldDataTable1.size(); ++i)
			if (targetDataTable1[i])
				(*(targetDataTable1[i])) = oldDataTable1[i];

		for (int i=0; i < targetDataTable2.size() && i < oldDataTable2.size(); ++i)
			if (targetDataTable2[i])
				(*(targetDataTable2[i])) = oldDataTable2[i];
	}
	
	/*! \brief a numerical data table
	* \ingroup core
	*/
	typedef DataTable<QString> TextDataTable;
	
	/*! \brief a numerical data table
	* \ingroup core
	*/
	typedef DataTable<qreal> NumericalDataTable;
	
	/*! \brief this command is used to replace text data inside a handle
	* \ingroup undo
	*/
	typedef ChangeDataCommand<QString> ChangeTextDataCommand;
	
	/*! \brief this command is used to replace numerical data inside a handle
	* \ingroup undo
	*/
	typedef ChangeDataCommand<qreal> ChangeNumericalDataCommand;
}

#endif
