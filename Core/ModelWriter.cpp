/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

The ModelReader is used to write an xml file containing the list of handles and their
data.

****************************************************************************/

#include "ModelWriter.h"

namespace Tinkercell
{

	/*! \brief constructor. Sets autoformatting to true*/
	ModelWriter::ModelWriter() : QXmlStreamWriter()
	{
		setAutoFormatting(true);
	}
	
	bool ModelWriter::writeModel(TextEditor * editor,QIODevice * device)
	{
		if (!editor || !device) return false;

		setDevice(device);

		return writeModel(editor,const_cast<ModelWriter*>(this));
	}
	
	bool ModelWriter::writeModel(TextEditor * editor,QXmlStreamWriter * writer)
	{
		if (!editor || !writer) return false;

		QList<TextItem*> allItems = editor->items();

		QList<ItemHandle*> topLevelHandles, childHandles;

		if (editor->symbolsTable)
			writeHandle(&(editor->symbolsTable->modelItem),writer);

		ItemHandle* handle = 0;
		for (int i=0; i < allItems.size(); ++i)
		{
			handle = getHandle(allItems[i]);
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

	bool ModelWriter::writeModel(GraphicsScene * scene, QIODevice * device)
	{
		if (!scene || !device) return false;

		setDevice(device);

		return writeModel(scene,const_cast<ModelWriter*>(this));
	}
	
	
	bool ModelWriter::writeModel(GraphicsScene * scene, QXmlStreamWriter * writer)
	{
		if (!scene || !writer) return false;

		QList<QGraphicsItem*> allItems = scene->items();

		QList<ItemHandle*> topLevelHandles, childHandles;

		if (scene->symbolsTable)
			writeHandle(&(scene->symbolsTable->modelItem),writer);

		ItemHandle* handle = 0;
		for (int i=0; i < allItems.size(); ++i)
		{
			handle = getHandle(allItems[i]);
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
			
			if (handle->visible)
				writer->writeAttribute("visible",QString("true"));
			else
				writer->writeAttribute("visible",QString("false"));

			if (handle->data)
			{
				QList<QString> nkeys = handle->data->numericalData.keys();
				QList<QString> skeys = handle->data->textData.keys();

				if (nkeys.size() > 0)
				{
					writer->writeStartElement("TableOfReals");
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
					writer->writeStartElement("TableOfStrings");
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
		QString sep(";");
		//writeStartElement("Table");
		writer->writeAttribute("rows",QString::number(table.rows()));
		writer->writeAttribute("cols",QString::number(table.cols()));
		writer->writeAttribute("rowNames",table.getRowNames().join(sep));
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
		QString sep(";");
		//writeStartElement("TableOfStrings");
		writer->writeAttribute("rows",QString::number(table.rows()));
		writer->writeAttribute("cols",QString::number(table.cols()));
		writer->writeAttribute("rowNames",table.getRowNames().join(sep));
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
}
