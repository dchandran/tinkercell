/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

The ModelReader is used to read an xml file containing the list of handles and their
data.

****************************************************************************/

#include "ModelReader.h"
#include "ConsoleWindow.h"
#include <QtDebug>

namespace Tinkercell
{
	QString ModelReader::sep(";;");
	/*! \brief Reads up to the next start node
	* \return Token Typer*/ 
	QXmlStreamReader::TokenType ModelReader::readNext()
	{
		QXmlStreamReader::readNext();	
		while (!atEnd() && !isStartElement() && !isEndElement())
		{
			QXmlStreamReader::readNext();
		}
		return tokenType();
	}
	/*! \brief Reads a list of <family,handles> pairs from an XML file using the IO device provided 
	* \param QIODevice to use
	* \return list of item handles*/ 
	QList< QPair<QString,ItemHandle*> > ModelReader::readHandles(QIODevice * device)
	{
		QList< QPair<QString,ItemHandle*> > list;
		if (!device) return list;

		if (this->device() != device)
			setDevice(device);

		QPair<QString,ItemHandle*> handle(QString(),0);
		
		QList< QPair<ItemHandle*,QStringList> > nodes, roles;
		
		handle = readHandle(list,nodes,roles);
		while (handle.second)
		{
			list << handle;
			handle = readHandle(list,nodes,roles);
		}

		ConnectionHandle * connection;		
		for (int i=0; i < nodes.size() && i < roles.size(); ++i)
			if (connection = ConnectionHandle::cast(nodes[i].first))
			{
				NodeHandle * node;
				for (int j=0; j < list.size(); ++j)
				{
					if (node = NodeHandle::cast(list[j].second))
					{
						int k = nodes[i].second.indexOf(node->fullName());
						if (k > -1)
						{
							connection->addNode(node, roles[i].second[k].toInt());
						}
					}
				}
			}
		
		return list;
	}

	QPair<QString,ItemHandle*> ModelReader::readHandle(QList< QPair<QString,ItemHandle*> >& existingHandles, QList< QPair<ItemHandle*,QStringList> >& nodes, QList< QPair<ItemHandle*,QStringList> >& roles)
	{
		while (!atEnd() && !(isStartElement() && name() == "Handle"))
		{
			readNext();
		}

		QPair<QString,ItemHandle*> pair(QString(),0);

		if (!(isStartElement() && name() == "Handle"))
		{
			//if (tokenType() == 1)
			//	qDebug() << errorString();
			return pair;
		}

		QXmlStreamAttributes vec = attributes();

		ItemHandle * handle = 0;

		QString itemName, family, parent;
		
		for (int i=0; i < vec.size(); ++i)
		{
			if (!handle && vec.at(i).name().toString() == QObject::tr("type"))
			{
				QString str = vec.at(i).value().toString();
				bool ok;
				int n = str.toInt(&ok);
				if (n == NodeHandle::TYPE)
					handle = new NodeHandle;
				else
					if (n == ConnectionHandle::TYPE)
						handle = new ConnectionHandle;
					else
						handle = new ItemHandle;
			}
			else
				if (vec.at(i).name().toString() == QObject::tr("name"))
				{
					itemName = vec.at(i).value().toString();
				}
				else
					if (vec.at(i).name().toString() == QObject::tr("family"))
					{
						family = vec.at(i).value().toString();
					}
					else
						if (vec.at(i).name().toString() == QObject::tr("parent"))
						{
							parent = vec.at(i).value().toString();
						}
						else
							if (vec.at(i).name().toString() == QObject::tr("nodes"))
							{
								nodes += QPair<ItemHandle*,QStringList>(handle, vec.at(i).value().toString().split(sep));
							}
							else
								if (vec.at(i).name().toString() == QObject::tr("roles"))
								{
									roles += QPair<ItemHandle*,QStringList>(handle, vec.at(i).value().toString().split(sep));
								}
		}

		if (handle)
		{
			handle->name = itemName;
			pair.first = family;
			pair.second = handle;
			for (int i=0; i < existingHandles.size(); ++i)
			{
				if (existingHandles[i].second && existingHandles[i].second->fullName() == parent)
				{
					handle->setParent(existingHandles[i].second,false);
					break;
				}
			}
		}
		readNext();

		while (handle && !(isEndElement() && name() == "Handle"))
		{
			if (isStartElement() && name() == "tc_matrix")
			{
				while (!(isEndElement() && name() == "tc_matrix"))
				{
					readRealsTable(handle);
					readNext();
				}
			}
			if (isStartElement() && name() == "tc_table")
			{
				while (!(isEndElement() && name() == "tc_table"))
				{
					readStringsTable(handle);
					readNext();
				}
			}
			readNext();
		}
		//qDebug() << "done reading handle : " << name().toString();
		return pair;
	}

	/*! \brief Reads a table from an XML file 
	* \return item handle*/
	void ModelReader::readRealsTable(ItemHandle* handle)
	{
		if (handle && isStartElement() && name() == "Table")
		{
			QXmlStreamAttributes vec = attributes();
			QString name, desc;
			QStringList rownames, colnames, values;
			int rows = 0, cols = 0;
			bool ok;

			for (int i=0; i < vec.size(); ++i)
			{
				if (handle && vec.at(i).name().toString() == QObject::tr("key"))
				{
					name = vec.at(i).value().toString();
				}
				else
					if (handle && vec.at(i).name().toString() == QObject::tr("cols"))
					{
						cols = vec.at(i).value().toString().toInt(&ok);
						if (!ok)
							cols = 0;
					}
					else
						if (handle && vec.at(i).name().toString() == QObject::tr("rows"))
						{
							rows = vec.at(i).value().toString().toInt(&ok);
							if (!ok)
								rows = 0;
						}
						else
							if (handle && vec.at(i).name().toString() == QObject::tr("rowNames"))
							{
								rownames = vec.at(i).value().toString().split(sep);
							}
							else
								if (handle && vec.at(i).name().toString() == QObject::tr("columnNames"))
								{
									colnames = vec.at(i).value().toString().split(sep);
								}
								else
									if (handle && vec.at(i).name().toString() == QObject::tr("values"))
									{
										values = vec.at(i).value().toString().split(sep);
									}
									else
										if (handle && vec.at(i).name().toString() == QObject::tr("desc"))
										{
											desc = vec.at(i).value().toString();
										}
			}

			if (!name.isEmpty() && colnames.size() >= cols && rownames.size() >= rows && values.size() >= rows*cols)
			{
				qreal x;
				DataTable<qreal> data;
				data.resize(rows,cols);
				for (int i=0; i < rows; ++i) data.rowName(i) = rownames[i];
				for (int i=0; i < cols; ++i) data.columnName(i) = colnames[i];
				int k = 0;
				for (int i=0; i < rows; ++i)
				{
					for (int j=0; j < cols; ++j)
					{
						x = values[k].toDouble(&ok);
						if (ok)
							data.value(i,j) = x;
						else
							data.value(i,j) = 0.0;
						++k;
					}
				}
				data.description() = desc;
				handle->numericalDataTable(name) = data;
			}
		}
	}
	/*! \brief Reads a table from an XML file 
	* \return item handle*/
	void ModelReader::readStringsTable(ItemHandle* handle)
	{
		if (handle && isStartElement() && name().toString() == QObject::tr("Table"))
		{
			QXmlStreamAttributes vec = attributes();
			QString name;
			QStringList rownames, colnames, values;
			int rows = 0, cols = 0;
			bool ok;

			for (int i=0; i < vec.size(); ++i)
			{
				if (handle && vec.at(i).name().toString() == QObject::tr("key"))
				{
					name = vec.at(i).value().toString();
				}
				else
					if (handle && vec.at(i).name().toString() == QObject::tr("cols"))
					{
						cols = vec.at(i).value().toString().toInt(&ok);
						if (!ok)
							cols = 0;
					}
					else
						if (handle && vec.at(i).name().toString() == QObject::tr("rows"))
						{
							rows = vec.at(i).value().toString().toInt(&ok);
							if (!ok)
								rows = 0;
						}
						else
							if (handle && vec.at(i).name().toString() == QObject::tr("rowNames"))
							{
								rownames = vec.at(i).value().toString().split(sep);
							}
							else
								if (handle && vec.at(i).name().toString() == QObject::tr("columnNames"))
								{
									colnames = vec.at(i).value().toString().split(sep);
								}
								else
									if (handle && vec.at(i).name().toString() == QObject::tr("values"))
									{
										values = vec.at(i).value().toString().split(sep);
									}
			}

			if (!name.isEmpty() && colnames.size() >= cols && rownames.size() >= rows && values.size() >= rows*cols)
			{
				DataTable<QString> data;
				data.resize(rows,cols);
				for (int i=0; i < rows; ++i) data.rowName(i) = rownames[i];
				for (int i=0; i < cols; ++i) data.columnName(i) = colnames[i];
				int k = 0;
				for (int i=0; i < rows; ++i)
				{
					for (int j=0; j < cols; ++j)
					{
						data.value(i,j) = values[k];
						++k;
					}
				}
				handle->textDataTable(name) = data;
			}
		}
	}

}
