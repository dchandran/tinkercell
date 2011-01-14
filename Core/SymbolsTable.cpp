/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines the class that stores all symbols, such as node and 
connection names and data columns and rows, for each scene

****************************************************************************/
#include <iostream>
#include "MainWindow.h"
#include "NetworkWindow.h"
#include "NetworkHandle.h"
#include "GraphicsScene.h"
#include "TextEditor.h"
#include "Tool.h"
#include "SymbolsTable.h"
#include "ConsoleWindow.h"

namespace Tinkercell
{

	SymbolsTable::SymbolsTable(NetworkHandle * net) : network(net)
	{
		globalHandle.network = net;
	}
	
	void SymbolsTable::update()
	{
		if (!network) return;
		
		GraphicsScene* scene = 0;
		TextEditor * editor = 0;
		QList<NetworkWindow*> windows = network->networkWindows;
		
		QList<ItemHandle*> handles;
		handles << &globalHandle;

		for (int j=0; j < windows.size(); ++j)
			if (windows[j] && !windows[j]->handle)
			{
				if (windows[j]->scene)
				{
					QList<QGraphicsItem*> items = windows[j]->scene->items();
					ItemHandle * handle;

					for (int i=0; i < items.size(); ++i)
						if (handle = getHandle(items[i]))
						{
							handle = handle->root();
							if (handle && !handles.contains(handle))
							{
								handles << handle;
								handles << handle->allChildren();
							}
						}
				}
				else
				if (windows[j]->editor)
				{
					QList<ItemHandle*> items = windows[j]->editor->allItems;
					ItemHandle * handle;

					for (int i=0; i < items.size(); ++i)
						if (handle = items[i])
						{
							handle = handle->root();
							if (handle && !handles.contains(handle))
							{
								handles << handle;
								handles << handle->allChildren();
							}
						}
				}
			}

		update(handles);
	}

	void SymbolsTable::update(const QList<ItemHandle*>& items)
	{
		nonuniqueHandles.clear();
		uniqueHandlesWithDot.clear();
		uniqueHandlesWithUnderscore.clear();
		uniqueDataWithDot.clear();
		uniqueDataWithUnderscore.clear();
		nonuniqueData.clear();
		handlesByFamily.clear();
		handlesAddress.clear();

		ItemHandle * handle = 0;
		for (int i=0; i < items.size(); ++i)
		{
			if ((handle = items[i]) && !handlesAddress.contains((void*)handle))
			{
				handle->network = network;

				handlesAddress[(void*)handle] = handle->fullName();

				uniqueHandlesWithDot[handle->fullName()] = handle;
				uniqueHandlesWithUnderscore[handle->fullName(QObject::tr("_"))] = handle;
				if (handle != &globalHandle)
				{
					nonuniqueHandles.insertMulti(handle->fullName(),handle);
					nonuniqueHandles.insertMulti(handle->fullName(QObject::tr("_")),handle);
					nonuniqueHandles.insertMulti(handle->name,handle);
				}

				if (handle->family())
					handlesByFamily.insertMulti(handle->family()->name(), handle);

				QList<QString> keys = handle->numericalDataNames();
				for (int j=0; j < keys.size(); ++j)
				{
					DataTable<qreal>& nDat = handle->numericalDataTable( keys[j] );

					for (int k=0; k < nDat.rows(); ++k)
					{
						if (!nDat.rowName(k).isEmpty())
						{
							if (!handle->name.isEmpty())
							{
								uniqueDataWithDot[handle->fullName() + QObject::tr(".") + nDat.rowName(k)] = 
									QPair<ItemHandle*,QString>(handle,keys[j]);
								uniqueDataWithUnderscore[handle->fullName(QObject::tr("_")) + QObject::tr("_") + nDat.rowName(k)] = 
									QPair<ItemHandle*,QString>(handle,keys[j]);
								
								nonuniqueData.insertMulti(handle->fullName() + QObject::tr(".") + nDat.rowName(k),
									QPair<ItemHandle*,QString>(handle,keys[j]));
								nonuniqueData.insertMulti(handle->fullName(QObject::tr("_")) + QObject::tr("_") + nDat.rowName(k), 
									QPair<ItemHandle*,QString>(handle,keys[j]));
							}
							else
							{
								uniqueDataWithDot[nDat.rowName(k)] = QPair<ItemHandle*,QString>(handle,keys[j]);
								uniqueDataWithUnderscore[nDat.rowName(k)] = QPair<ItemHandle*,QString>(handle,keys[j]);
							}
							nonuniqueData.insertMulti(nDat.rowName(k),	QPair<ItemHandle*,QString>(handle,keys[j]));
						}
					}

					/*
					for (int k=0; k < nDat.columns(); ++k)
					{						
						if (!nDat.columnName(k).isEmpty())
						{
							if (!handle->name.isEmpty())
							{
								uniqueDataWithDot.insertMulti(handle->fullName() + QObject::tr(".") + nDat.columnName(k),
									QPair<ItemHandle*,QString>(handle,keys[j]));
								uniqueDataWithUnderscore.insertMulti(handle->fullName(QObject::tr("_")) + QObject::tr("_") + nDat.columnName(k),
									QPair<ItemHandle*,QString>(handle,keys[j]));
							}
							nonuniqueData.insertMulti(handle->fullName() + QObject::tr(".") + nDat.columnName(k),
									QPair<ItemHandle*,QString>(handle,keys[j]));
							nonuniqueData.insertMulti(handle->fullName(QObject::tr("_")) + QObject::tr("_") + nDat.columnName(k), 
								QPair<ItemHandle*,QString>(handle,keys[j]));
							nonuniqueData.insertMulti(nDat.columnName(k),
								QPair<ItemHandle*,QString>(handle,keys[j]));
						}
					}
					*/
				}

				keys = handle->textDataNames();
				for (int j=0; j < keys.size(); ++j)
				{
					DataTable<QString>& sDat = handle->textDataTable( keys[j] );

					for (int k=0; k < sDat.rows(); ++k)
					{
						if (!sDat.rowName(k).isEmpty())
						{
							if (!handle->name.isEmpty())
							{
								uniqueDataWithDot[handle->fullName() + QObject::tr(".") + sDat.rowName(k)] =
									QPair<ItemHandle*,QString>(handle,keys[j]);
								uniqueDataWithUnderscore[handle->fullName(QObject::tr("_")) + QObject::tr("_") + sDat.rowName(k)] =
									QPair<ItemHandle*,QString>(handle,keys[j]);
								
								nonuniqueData.insertMulti(handle->fullName() + QObject::tr(".") + sDat.rowName(k),
									QPair<ItemHandle*,QString>(handle,keys[j]));
								nonuniqueData.insertMulti(handle->fullName(QObject::tr("_")) + QObject::tr("_") + sDat.rowName(k),
									QPair<ItemHandle*,QString>(handle,keys[j]));
							}
							else
							{
								uniqueDataWithDot[sDat.rowName(k)] = QPair<ItemHandle*,QString>(handle,keys[j]);
								uniqueDataWithUnderscore[sDat.rowName(k)] = QPair<ItemHandle*,QString>(handle,keys[j]);
							}
							nonuniqueData.insertMulti(sDat.rowName(k),
								QPair<ItemHandle*,QString>(handle,keys[j]));
						}
					}
					
					for (int k=0; k < sDat.columns(); ++k)
					{
						/*
						if (!sDat.columnName(k).isEmpty())
						{
							if (!handle->name.isEmpty())
							{
								uniqueDataWithDot.insertMulti(handle->fullName() + QObject::tr(".") + sDat.columnName(k),
									QPair<ItemHandle*,QString>(handle,keys[j]));
								uniqueDataWothUnderscore.insertMulti(handle->fullName(QObject::tr("_")) + QObject::tr("_") + sDat.columnName(k),
									QPair<ItemHandle*,QString>(handle,keys[j]));
							}
							nonuniqueData.insertMulti(handle->fullName() + QObject::tr(".") + sDat.columnName(k),
								QPair<ItemHandle*,QString>(handle,keys[j]));
							nonuniqueData.insertMulti(handle->fullName(QObject::tr("_")) + QObject::tr("_") + sDat.columnName(k),
								QPair<ItemHandle*,QString>(handle,keys[j]));
							nonuniqueData.insertMulti(sDat.columnName(k),
								QPair<ItemHandle*,QString>(handle,keys[j]));
						}
						*/
						
						for (int l=0; l < sDat.rows(); ++l)
							nonuniqueData.insertMulti(sDat.at(l,k),
								QPair<ItemHandle*,QString>(handle,keys[j]));
					}
				}
			
			}
		}
		
		QList<NetworkWindow*> windows = network->networkWindows;
		for (int i=0; i < windows.size(); ++i)
			if (windows[i] && windows[i]->handle && !handlesAddress.contains(windows[i]->handle))
				windows[i]->close();
	}
	
	static void expandLeftFirst(ItemFamily * family, QList<ItemFamily*>& families)
	{
		if (family && !families.contains(family))
		{
			families << family; 
			QList<ItemFamily*> children = family->children();
			for (int i=0; i < children.size(); ++i)
				expandLeftFirst(children[i],families); 

		}
	}
	
	QList<ItemHandle*> SymbolsTable::allHandlesSortedByFamily() const
	{
		QList<ItemFamily*> allRootFamilies;
		ItemFamily * root = 0;
		
		QStringList keys (uniqueHandlesWithDot.keys());
		QList<ItemHandle*> allHandles = uniqueHandlesWithDot.values();
		
		for (int i=0; i < allHandles.size(); ++i)
		{
			std::cout << "handle " << i << "   "  << keys[i].toAscii().data() << std::endl;
			if (allHandles[i] && allHandles[i]->family())
			{
				/*root = allHandles[i]->family()->root();
				if (!allRootFamilies.contains(root))
					allRootFamilies << root;*/
			}
		}
			/*
		QList<ItemFamily*> sortedFamilies;
		
		for (int i=0; i < allRootFamilies.size(); ++i)
			expandLeftFirst(allRootFamilies[i],sortedFamilies);
		
		allHandles.clear();
		for (int i=0; i < sortedFamilies.size(); ++i)
			allHandles += handlesByFamily.values(sortedFamilies[i]->name());*/
		
		return allHandles;

	}
	
	QList<ItemHandle*> SymbolsTable::allHandlesSortedByName() const
	{
		QStringList names = uniqueHandlesWithDot.keys();
		names.sort();
		
		QList<ItemHandle*> allHandles;
		
		for (int i=0; i < names.size(); ++i)
			allHandles << uniqueHandlesWithDot[ names[i] ];

		return allHandles;
	}

	bool SymbolsTable::isValidPointer(void * p) const
	{
		return (p!=0 && !MainWindow::invalidPointers.contains(p) && handlesAddress.contains(p));
	}

}
