/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This file defines the class that stores all symbols, such as node and
 connection names and data columns and rows, for each scene


****************************************************************************/

#include "MainWindow.h"
#include "NetworkWindow.h"
#include "GraphicsScene.h"
#include "Tool.h"
#include "SymbolsTable.h"
#include "TextItem.h"

namespace Tinkercell
{

	SymbolsTable::SymbolsTable(NetworkWindow * net) : network(net)
	{
		modelItem.data = new ItemData();
	}

	SymbolsTable::~SymbolsTable()
	{
	}

     void SymbolsTable::update(GraphicsScene * scene)
	{
          if (!scene) return;

		handlesFirstName.clear();
		handlesFullName.clear();
		dataRowsAndCols.clear();
		handlesFamily.clear();

		QList<QGraphicsItem*> items = scene->items();

		ItemHandle * handle;

		for (int i=0; i < items.size(); ++i)
		{
			if ((handle = getHandle(items[i])) && items[i]->isVisible() && !handlesFullName.contains(handle->fullName()))
			{
				handlesFullName[handle->fullName()] = handle;
				handlesFirstName.insertMulti(handle->name,handle);

				if (handle->family())
					handlesFamily.insertMulti(handle->family()->name, handle);

				if (handle->data)
				{
					QList<QString> keys = handle->data->numericalData.keys();
					for (int j=0; j < keys.size(); ++j)
					{
						DataTable<qreal>& nDat = handle->data->numericalData[ keys[j] ];

						for (int k=0; k < nDat.rows(); ++k)
						{
							if (!nDat.rowName(k).isEmpty())
							{
								dataRowsAndCols.insertMulti(handle->fullName() + QObject::tr(".") + nDat.rowName(k),
													QPair<ItemHandle*,QString>(handle,keys[j]));
								dataRowsAndCols.insertMulti(nDat.rowName(k),
													QPair<ItemHandle*,QString>(handle,keys[j]));
							}
						}

						for (int k=0; k < nDat.cols(); ++k)
						{
							if (!nDat.colName(k).isEmpty())
							{
								dataRowsAndCols.insertMulti(handle->fullName() + QObject::tr(".") + nDat.colName(k),
													QPair<ItemHandle*,QString>(handle,keys[j]));
								dataRowsAndCols.insertMulti(nDat.colName(k),
													QPair<ItemHandle*,QString>(handle,keys[j]));
							}
						}
					}

					keys = handle->data->textData.keys();
					for (int j=0; j < keys.size(); ++j)
					{
						DataTable<QString>& sDat = handle->data->textData[ keys[j] ];

						for (int k=0; k < sDat.rows(); ++k)
						{
							if (!sDat.rowName(k).isEmpty())
							{
								dataRowsAndCols.insertMulti(handle->fullName() + QObject::tr(".") + sDat.rowName(k),
													QPair<ItemHandle*,QString>(handle,keys[j]));
								dataRowsAndCols.insertMulti(sDat.rowName(k),
													QPair<ItemHandle*,QString>(handle,keys[j]));
							}
						}

						for (int k=0; k < sDat.cols(); ++k)
						{
							if (!sDat.colName(k).isEmpty())
							{
								dataRowsAndCols.insertMulti(handle->fullName() + QObject::tr(".") + sDat.colName(k),
													QPair<ItemHandle*,QString>(handle,keys[j]));
								dataRowsAndCols.insertMulti(sDat.colName(k),
													QPair<ItemHandle*,QString>(handle,keys[j]));
							}
						}
					}
				}
			}
		}
	}

     void SymbolsTable::update(TextEditor  * editor)
     {
          if (!editor) return;

          handlesFirstName.clear();
          handlesFullName.clear();
          dataRowsAndCols.clear();
          handlesFamily.clear();

//Arnaud: there is no method items in the class TextEditor
//and the TextItems is not defined

//           QList<TextItems*>& items = editor->items();
//
//           ItemHandle * handle;
//
//           for (int i=0; i < items.size(); ++i)
//           {
//                if ((handle = getHandle(items[i])) && items[i]->isVisible() && !handlesFullName.contains(handle->fullName()))
//                {
//                     handlesFullName[handle->fullName()] = handle;
//                     handlesFirstName.insertMulti(handle->name,handle);
//
//                     if (handle->family())
//                          handlesFamily.insertMulti(handle->family()->name, handle);
//
//                     if (handle->data)
//                     {
//                          QList<QString> keys = handle->data->numericalData.keys();
//                          for (int j=0; j < keys.size(); ++j)
//                          {
//                               DataTable<qreal>& nDat = handle->data->numericalData[ keys[j] ];
//
//                               for (int k=0; k < nDat.rows(); ++k)
//                               {
//                                    if (!nDat.rowName(k).isEmpty())
//                                    {
//                                         dataRowsAndCols.insertMulti(handle->fullName() + QObject::tr(".") + nDat.rowName(k),
//                                                                  QPair<ItemHandle*,QString>(handle,keys[j]));
//                                         dataRowsAndCols.insertMulti(nDat.rowName(k),
//                                                                  QPair<ItemHandle*,QString>(handle,keys[j]));
//                                    }
//                               }
//
//                               for (int k=0; k < nDat.cols(); ++k)
//                               {
//                                    if (!nDat.colName(k).isEmpty())
//                                    {
//                                         dataRowsAndCols.insertMulti(handle->fullName() + QObject::tr(".") + nDat.colName(k),
//                                                                  QPair<ItemHandle*,QString>(handle,keys[j]));
//                                         dataRowsAndCols.insertMulti(nDat.colName(k),
//                                                                  QPair<ItemHandle*,QString>(handle,keys[j]));
//                                    }
//                               }
//                          }
//
//                          keys = handle->data->textData.keys();
//                          for (int j=0; j < keys.size(); ++j)
//                          {
//                               DataTable<QString>& sDat = handle->data->textData[ keys[j] ];
//
//                               for (int k=0; k < sDat.rows(); ++k)
//                               {
//                                    if (!sDat.rowName(k).isEmpty())
//                                    {
//                                         dataRowsAndCols.insertMulti(handle->fullName() + QObject::tr(".") + sDat.rowName(k),
//                                                                  QPair<ItemHandle*,QString>(handle,keys[j]));
//                                         dataRowsAndCols.insertMulti(sDat.rowName(k),
//                                                                  QPair<ItemHandle*,QString>(handle,keys[j]));
//                                    }
//                               }
//
//                               for (int k=0; k < sDat.cols(); ++k)
//                               {
//                                    if (!sDat.colName(k).isEmpty())
//                                    {
//                                         dataRowsAndCols.insertMulti(handle->fullName() + QObject::tr(".") + sDat.colName(k),
//                                                                  QPair<ItemHandle*,QString>(handle,keys[j]));
//                                         dataRowsAndCols.insertMulti(sDat.colName(k),
//                                                                  QPair<ItemHandle*,QString>(handle,keys[j]));
//                                    }
//                               }
//                          }
//                     }
//                }
//           }
     }
}
