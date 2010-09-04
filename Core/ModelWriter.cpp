/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

The ModelReader is used to write an xml file containing the list of handles and their
data.

****************************************************************************/

#include "NetworkHandle.h"
#include "ModelWriter.h"

namespace Tinkercell
{

	/*! \brief constructor. Sets autoformatting to true*/
	ModelWriter::ModelWriter() : QXmlStreamWriter()
	{
		setAutoFormatting(true);
	}
	
	bool ModelWriter::writeModel(NetworkHandle * network, QIODevice * device)
	{
		if (!network || !device) return false;

		setDevice(device);

		return writeModel(network,const_cast<ModelWriter*>(this));
	}
	
	bool ModelWriter::writeModel(const QList<ItemHandle*>& list, QIODevice * device)
	{
		if (list.isEmpty() || !device) return false;

		setDevice(device);

		return writeModel(list,const_cast<ModelWriter*>(this));
	}

	bool ModelWriter::writeModel(NetworkHandle * network, QXmlStreamWriter * writer)
	{
		if (!network || !writer) return false;

		QList<ItemHandle*> allItems = network->handles();

		QList<ItemHandle*> topLevelHandles, childHandles;

		if (network)
			writeHandle(network->globalHandle(),writer);

		ItemHandle* handle = 0;
		for (int i=0; i < allItems.size(); ++i)
		{
			handle = allItems[i];
			if (handle && !topLevelHandles.contains(handle) && !handle->parent)
			{
				writeHandle(handle,writer);
				topLevelHandles << handle;
			}
		}
		for (int i=0; i < topLevelHandles.size(); ++i)
		{
			childHandles << topLevelHandles[i]->children;
		}
		for (int i=0; i < childHandles.size(); ++i)
		{
			handle = childHandles[i];
			if (handle)
			{
				writeHandle(handle,writer);
				childHandles << handle->children; //queue -- assures that parents are written first
			}
		}

		return true;
	}
	
	bool ModelWriter::writeModel(const QList<ItemHandle*>& allItems,QXmlStreamWriter * writer)
	{
		if (allItems.isEmpty() || !writer) return false;

		QList<ItemHandle*> topLevelHandles, childHandles;

		ItemHandle* handle = 0;
		for (int i=0; i < allItems.size(); ++i)
		{
			handle = allItems[i];
			if (handle && !topLevelHandles.contains(handle) && !handle->parent)
			{
				writeHandle(handle,writer);
				topLevelHandles << handle;
			}
		}

		for (int i=0; i < topLevelHandles.size(); ++i)
		{
			childHandles << topLevelHandles[i]->children;
		}

		for (int i=0; i < childHandles.size(); ++i)
		{
			handle = childHandles[i];
			if (handle)
			{
				writeHandle(handle,writer);
				childHandles << handle->children; //queue -- assures that parents are written first
			}
		}

		return true;
	}
	
	void ModelWriter::writeHandle(ItemHandle * handle, QXmlStreamWriter * writer)
	{
		if (handle && writer)
		{
			writer->writeStartElement("Handle");
			writer->writeAttribute("type", QString::number(handle->type));
			writer->writeAttribute("name",handle->name);
			if (handle->parent)
				writer->writeAttribute("parent",handle->parent->fullName());
			else
				writer->writeAttribute("parent","");
			if (handle->family())
				writer->writeAttribute("family",handle->family()->name);
			else
				writer->writeAttribute("family","Node");
			
			ConnectionHandle * connection = ConnectionHandle::cast(handle);
			if (connection && !connection->nodesWithRoles.isEmpty())
			{
				QStringList names, roles;
				for (int i=0; i < connection->nodesWithRoles.size(); ++i)
					if (connection->nodesWithRoles[i].first)
					{
						names << connection->nodesWithRoles[i].first->fullName();
						roles << QString::number(connection->nodesWithRoles[i].second);
					}
				writer->writeAttribute("nodes",names.join(sep));
				writer->writeAttribute("roles",roles.join(sep));
			}

			if (handle->data)
			{
				QList<QString> nkeys = handle->data->numericalData.keys();
				QList<QString> skeys = handle->data->textData.keys();

				if (nkeys.size() > 0)
				{
					writer->writeStartElement("tc_matrix");
					for (int i=0; i < nkeys.size(); ++i)
					{
						writer->writeStartElement("Table");
						writer->writeAttribute("key",nkeys[i]);
						writeDataTable(handle->data->numericalData[ nkeys[i] ],writer);
						writer->writeEndElement();
					}
					writer->writeEndElement();
				}

				if (skeys.size() > 0)
				{
					writer->writeStartElement("tc_table");
					for (int i=0; i < skeys.size(); ++i)
					{
						writer->writeStartElement("Table");
						writer->writeAttribute("key",skeys[i]);
						writeDataTable(handle->data->textData[ skeys[i] ],writer);
						writer->writeEndElement();
					}
					writer->writeEndElement();
				}
			}
			writer->writeEndElement();
		}
	}

	/*! \brief Writes a data table of doubles into an XML file
	* \param NodeImage pointer to write as XML
	* \param index of shape in NodeImage's shape vector
	* \return void*/
	void ModelWriter::writeDataTable(const DataTable<qreal>& table, QXmlStreamWriter * writer)
	{
		//writeStartElement("Table");
		writer->writeAttribute("rows",QString::number(table.rows()));
		writer->writeAttribute("cols",QString::number(table.cols()));
		writer->writeAttribute("rowNames",table.tc_getRowNames().join(sep));
		writer->writeAttribute("colNames",table.getColNames().join(sep));
		writer->writeAttribute("desc",table.description());

		QStringList values;

		for (int i=0; i < table.rows(); ++i)
			for (int j=0; j < table.cols(); ++j)
			{
				values << QString::number(table.at(i,j));
			}
			writer->writeAttribute("values",values.join(sep));

		//writeEndElement();
	}

	/*! \brief Writes a data table of strings into an XML file
	* \param NodeImage pointer to write as XML
	* \param index of shape in NodeImage's shape vector
	* \return void*/
	void ModelWriter::writeDataTable(const DataTable<QString>& table, QXmlStreamWriter * writer)
	{
		//writeStartElement("tc_table");
		writer->writeAttribute("rows",QString::number(table.rows()));
		writer->writeAttribute("cols",QString::number(table.cols()));
		writer->writeAttribute("rowNames",table.tc_getRowNames().join(sep));
		writer->writeAttribute("colNames",table.getColNames().join(sep));

		QStringList values;

		for (int i=0; i < table.rows(); ++i)
			for (int j=0; j < table.cols(); ++j)
			{
				values << table.at(i,j);
			}
			writer->writeAttribute("values",values.join(sep));

		//writeEndElement();
	}
	
	QString ModelWriter::sep(";;");
}
