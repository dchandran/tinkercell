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
#include "OutputWindow.h"
#include "TextItem.h"

namespace Tinkercell
{

    SymbolsTable::SymbolsTable(NetworkWindow * net) : networkWindow(net)
    {
        modelItem.data = new ItemData();
    }

    SymbolsTable::~SymbolsTable()
    {
    }

    void SymbolsTable::update()
    {
        if (networkWindow->scene)
            update(networkWindow->scene);
        else
        if (networkWindow->textEditor)
             update(networkWindow->textEditor);
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
		
		QList<ItemHandle*> handles;
		
		handles << &modelItem;
		
		for (int i=0; i < items.size(); ++i)
        {
            if ((handle = getHandle(items[i])) && items[i]->isVisible() && (handle->parent == 0) && !handles.contains(handle))
            {
				handles << handle;
				handles << handle->allChildren();
			}
		}
		update(handles);
	}
	
	void SymbolsTable::update(TextEditor  * editor)
    {
        if (!editor) return;

        handlesFirstName.clear();
        handlesFullName.clear();
        dataRowsAndCols.clear();
        handlesFamily.clear();

        QList<TextItem*>& items = editor->items();

        ItemHandle * handle;
		
		QList<ItemHandle*> handles;
		
		handles << &modelItem;
		
		for (int i=0; i < items.size(); ++i)
        {
            if ((handle = getHandle(items[i])) && (handle->parent == 0) && !handles.contains(handle))
            {
				handles << handle;
				handles << handle->allChildren();
			}
		}
		update(handles);
    }
	
	void SymbolsTable::update(const QList<ItemHandle*>& items)
	{
		ItemHandle * handle = 0;
        for (int i=0; i < items.size(); ++i)
        {
            if ((handle = items[i]) && !handlesFullName.contains(handle->fullName()))
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

}
