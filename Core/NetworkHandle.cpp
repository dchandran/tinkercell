/***************************************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

A class that is used to store a network. The network is a collection of Item Handles. 
The history stack is also a key component of a network.
The network can either be represented as text using TextEditor or visualized with graphical items in the
GraphicsScene. Each node and connection are contained in a handle, and each handle can either be represented as text or as graphics.
The two main components of NetworkWindow are the SymbolsTable and HistoryStack
This class provides functions for inserting items, removing items, and changing information inside the model.

***************************************************************************************************/

#include <QHBoxLayout>
#include "MainWindow.h"
#include "NetworkWindow.h"
#include "ConsoleWindow.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "GraphicsScene.h"
#include "TextEditor.h"
#include "UndoCommands.h"
#include "NetworkHandle.h"
#include "muParserDef.h"
#include "muParser.h"
#include "muParserInt.h"
#include <QRegExp>

namespace Tinkercell
{
	NetworkHandle::~NetworkHandle()
	{
		close();
		/*
		QList<NetworkWindow*> list = networkWindows;
		networkWindows.clear();

		for (int i=0; i < list.size(); ++i)
			if (list[i])
			{
				delete list[i];
			}*/
		history.clear();
	}
	
	QList<ItemHandle*> NetworkHandle::findItem(const QString& s) const
	{
		QList<ItemHandle*> items;
		if (symbolsTable.uniqueHandlesWithDot.contains(s))
			items << symbolsTable.uniqueHandlesWithDot[s];
		else
		if (symbolsTable.uniqueHandlesWithUnderscore.contains(s))
			items << symbolsTable.uniqueHandlesWithUnderscore[s];
		else
		if (symbolsTable.nonuniqueHandles.contains(s))
		{
			items = symbolsTable.nonuniqueHandles.values(s);
		}
		return items;
	}
	
	QList<ItemHandle*> NetworkHandle::findItem(const QStringList& list) const
	{
		QList<ItemHandle*> items;
		
		for (int i=0; i < list.size(); ++i)
		{
			QString s = list[i];
			if (symbolsTable.uniqueHandlesWithDot.contains(s))
				items += symbolsTable.uniqueHandlesWithDot[s];
			else
			if (symbolsTable.uniqueHandlesWithUnderscore.contains(s))
				items += symbolsTable.uniqueHandlesWithUnderscore[s];
			else
			if (symbolsTable.nonuniqueHandles.contains(s))
			{
				QList<ItemHandle*> items2 = symbolsTable.nonuniqueHandles.values(s);
				for (int j=0; j < items2.size(); ++j)
				{
					if (!items.contains(items2[j]))
						items += items2[j];
				}
			}
		}
		return items;
	}
	
	QList< QPair<ItemHandle*,QString> > NetworkHandle::findData(const QString& s) const
	{
		QList< QPair<ItemHandle*,QString> > list;

		if (symbolsTable.uniqueDataWithDot.contains(s))
			list = symbolsTable.uniqueDataWithDot.values(s);
		else
		if (symbolsTable.uniqueDataWithUnderscore.contains(s))
			list = symbolsTable.uniqueDataWithUnderscore.values(s);
		else
			list = symbolsTable.nonuniqueData.values(s);

		return list;
	}
	
	QList< QPair<ItemHandle*,QString> > NetworkHandle::findData(const QStringList& list) const
	{
		QList<ItemHandle*> items;
		QList< QPair<ItemHandle*,QString> > data;
		
		for (int i=0; i < list.size(); ++i)
		{
			QString s = list[i];
			if (symbolsTable.uniqueDataWithDot.contains(s))
				data += symbolsTable.uniqueDataWithDot[s];			
			else
			if (symbolsTable.uniqueDataWithUnderscore.contains(s))
				data += symbolsTable.uniqueDataWithUnderscore[s];			
			else
			if (symbolsTable.nonuniqueData.contains(s))
			{
				QList< QPair<ItemHandle*,QString> > items2 = symbolsTable.nonuniqueData.values(s);
				for (int j=0; j < items2.size(); ++j)
				{
					if (!items.contains(items2[j].first))
					{
						items += items2[j].first;
						data += items2[j];
					}
				}
			}
		}
		return data;
	}

	void NetworkHandle::close()
	{
		disconnect(&history);
		//disconnect(&history, SIGNAL(indexChanged(int)), this, SLOT(updateSymbolsTable(int)));
		//disconnect(&history, SIGNAL(indexChanged(int)), mainWindow, SIGNAL(historyChanged(int)));
		
		QList<NetworkWindow*> & list = networkWindows;

		for (int i=0; i < list.size(); ++i)
			if (list[i])
			{
				list[i]->disconnect();
				list[i]->close();
			}

		if (mainWindow)
			mainWindow->allNetworks.removeAll(this);
	}
	
	void NetworkHandle::setWindowTitle(const QString& title)
	{
		for (int i=0; i < networkWindows.size(); ++i)
			if (networkWindows[i])
				networkWindows[i]->setWindowTitle(title);
	}

	QList<GraphicsScene*> NetworkHandle::scenes() const
	{
		QList<GraphicsScene*> list;
		for (int i=0; i < networkWindows.size(); ++i)
			if (networkWindows[i] && networkWindows[i]->scene)
				list << networkWindows[i]->scene;
		return list;
	}
	
	QList<TextEditor*> NetworkHandle::editors() const
	{
		QList<TextEditor*> list;
		for (int i=0; i < networkWindows.size(); ++i)
			if (networkWindows[i] && networkWindows[i]->editor)
				list << networkWindows[i]->editor;
		return list;
	}
	
	void NetworkHandle::showScene(GraphicsScene * scene)
	{
		for (int i=0; i < networkWindows.size(); ++i)
			if (networkWindows[i] && networkWindows[i]->scene == scene && !networkWindows[i]->isVisible())
				networkWindows[i]->popOut();
	}

	void NetworkHandle::showTextEditor(TextEditor * editor)
	{
		for (int i=0; i < networkWindows.size(); ++i)
			if (networkWindows[i] && networkWindows[i]->editor == editor && !networkWindows[i]->isVisible())
				networkWindows[i]->popOut();
	}
	
	TextEditor * NetworkHandle::createTextEditor(const QString& text)
	{
		if (!mainWindow) return 0;
		
		TextEditor * tedit = new TextEditor(this);
		tedit->setText(text);
		
		networkWindows << (new NetworkWindow(this,tedit));
		
		return tedit;
	}

	GraphicsScene * NetworkHandle::createScene(const QList<QGraphicsItem*>& insertItems)
	{
		if (!mainWindow) return 0;
		
		GraphicsScene * scene = new GraphicsScene(this);
		
		for (int i=0; i < insertItems.size(); ++i)
		{
			scene->addItem(insertItems[i]);
		}
		
		networkWindows << (new NetworkWindow(this,scene));
		
		return scene;
	}
	
	GraphicsScene * NetworkHandle::createScene(ItemHandle * item, const QRectF& boundingRect)
	{
		if (!item) return 0;
		
		QList<QGraphicsItem*> graphicsItems = item->allGraphicsItems();
		QList<QGraphicsItem*> graphicsItems2;
		
		if (boundingRect.width() > 0 && boundingRect.height() > 0)
		{
			for (int i=0; i < graphicsItems.size(); ++i)
				if (graphicsItems[i] && boundingRect.contains(graphicsItems[i]->sceneBoundingRect()))
					graphicsItems2 << graphicsItems[i];
		}
		else
			graphicsItems2 = graphicsItems;
		
		
		GraphicsScene * scene = createScene(graphicsItems2);
		
		if (scene->networkWindow)
			scene->networkWindow->handle = item;
		
		return scene;
	}

	NetworkHandle::NetworkHandle(MainWindow * main) : QObject(main), mainWindow(main), symbolsTable(this)
	{		
		if (main && !main->allNetworks.contains(this))
			main->allNetworks << this;

		connect(&history, SIGNAL(indexChanged(int)), this, SLOT(updateSymbolsTable(int)));
		connect(&history, SIGNAL(indexChanged(int)), mainWindow, SIGNAL(historyChanged(int)));
		
		connect(this,SIGNAL(parentHandleChanged(NetworkHandle *, const QList<ItemHandle*>&, const QList<ItemHandle*>&)),
				main ,SIGNAL(parentHandleChanged(NetworkHandle *, const QList<ItemHandle*>&, const QList<ItemHandle*>&)));
			
		connect(this,SIGNAL(handleFamilyChanged(NetworkHandle *, const QList<ItemHandle*>&, const QList<ItemFamily*>&)),
				main ,SIGNAL(handleFamilyChanged(NetworkHandle *, const QList<ItemHandle*>&, const QList<ItemFamily*>&)));

		connect(this,SIGNAL(dataChanged(const QList<ItemHandle*>&)),
			main ,SIGNAL(dataChanged(const QList<ItemHandle*>&)));

		connect(this,SIGNAL(itemsRenamed(NetworkHandle*, const QList<ItemHandle*>&, const QList<QString>&, const QList<QString>&)),
			main ,SIGNAL(itemsRenamed(NetworkHandle*, const QList<ItemHandle*>&, const QList<QString>&, const QList<QString>&)));
	}

	ItemHandle* NetworkHandle::globalHandle()
	{
		return &(symbolsTable.globalHandle);
	}

	QList<ItemHandle*> NetworkHandle::handles(bool includeGlobal, bool sort)
	{
		QList<ItemHandle*> handles;
		if (includeGlobal)
			handles << &(symbolsTable.globalHandle);

		if (sort)
		{
			QStringList names = symbolsTable.uniqueHandlesWithDot.keys();
			names.sort();			
			for (int i=0; i < names.size(); ++i)
				handles += symbolsTable.uniqueHandlesWithDot[ names[i] ];
		}
		else
		{
			handles += symbolsTable.uniqueHandlesWithDot.values();
		}
		return handles;
	}

	QList<ItemHandle*> NetworkHandle::handlesSortedByFamily() const
	{
		return symbolsTable.allHandlesSortedByFamily();
	}
	
	GraphicsScene * NetworkHandle::currentScene() const
	{
		if (!mainWindow || !mainWindow->currentNetworkWindow || !networkWindows.contains(mainWindow->currentNetworkWindow))
			return 0;
		
		return mainWindow->currentNetworkWindow->scene;
	}
	
	NetworkWindow * NetworkHandle::currentWindow() const
	{
		if (!mainWindow || !mainWindow->currentNetworkWindow || !networkWindows.contains(mainWindow->currentNetworkWindow))
			return 0;
		
		return mainWindow->currentNetworkWindow;
	}

	TextEditor * NetworkHandle::currentTextEditor() const
	{
		if (!mainWindow || !mainWindow->currentNetworkWindow || !networkWindows.contains(mainWindow->currentNetworkWindow))
			return 0;
		
		return mainWindow->currentNetworkWindow->editor;
	}

	void NetworkHandle::rename(const QString& oldname, const QString& s)
	{
		if (oldname == s) return;
		
		if (symbolsTable.uniqueHandlesWithDot.contains(oldname))
		{
			rename(symbolsTable.uniqueHandlesWithDot[oldname],s);
			return;
		}
		
		if (symbolsTable.nonuniqueHandles.contains(oldname))
		{
			rename(symbolsTable.nonuniqueHandles[oldname],s);
			return;
		}

		QList<QString> oldNames, newNames;
		oldNames += oldname;

		QString newname = Tinkercell::RemoveDisallowedCharactersFromName(s);

		newname = makeUnique(newname);
		newNames += newname;

		QUndoCommand * command = new RenameCommand(oldname + tr(" renamed to ") + newname,this,oldname,newname);

		history.push(command);
	
		QList<ItemHandle*> items = handles();
		emit itemsRenamed(this, items, oldNames, newNames);
		emit dataChanged(items);
	}

	void NetworkHandle::rename(ItemHandle* handle, const QString& s)
	{
		if (!handle || (handle->fullName() == s)) return;

		QList<ItemHandle*> items;
		items += handle;
		QList<QString> oldNames, newNames;

		QString newname = s;
		oldNames += handle->fullName();

		if (handle->parent && !newname.contains(handle->parent->fullName()))
			newname = handle->parent->fullName() + tr(".") + Tinkercell::RemoveDisallowedCharactersFromName(newname);
		else
			newname = Tinkercell::RemoveDisallowedCharactersFromName(newname);

		newname = makeUnique(newname);
		newNames += newname;

		QUndoCommand * command = new RenameCommand(handle->name + tr(" renamed to ") + newname,this,items,newNames);

		history.push(command);

		emit itemsRenamed(this, items, oldNames, newNames);
		emit dataChanged(items);
	}

	void NetworkHandle::rename(const QList<ItemHandle*>& items, const QList<QString>& new_names)
	{
		if (items.isEmpty() || items.size() != new_names.size()) return;

		QList<QString> oldNames, newNames;
		ItemHandle * handle;
		QString newname;

		for (int i=0; i < items.size(); ++i)
			if ((handle = items[i]))
			{
				newname = new_names[i];
				oldNames += handle->fullName();

				if (handle->parent && !newname.contains(handle->parent->fullName()))
					newname = handle->parent->fullName() + tr(".") + Tinkercell::RemoveDisallowedCharactersFromName(newname);
				else
					newname = Tinkercell::RemoveDisallowedCharactersFromName(newname);

				newname = makeUnique(newname);
				newNames += newname;
			}

		QUndoCommand * command = new RenameCommand(tr("items renamed"),this,items,newNames);

		history.push(command);

		emit itemsRenamed(this, items, oldNames, newNames);
		emit dataChanged(items);
	}

	void NetworkHandle::setParentHandle(const QList<ItemHandle*>& handles, const QList<ItemHandle*>& parentHandles)
	{
		if (handles.size() != parentHandles.size()) return;

		SetParentHandleCommand * command = new SetParentHandleCommand(tr("parent(s) changed"), this, handles, parentHandles);
		history.push(command);

		emit parentHandleChanged(this, command->children, command->oldParents);
		emit dataChanged(command->children);
	}

	void NetworkHandle::setParentHandle(ItemHandle * child, ItemHandle * parent)
	{
		QList<ItemHandle*> children, parents;
		children << child;
		parents << parent;
		setParentHandle(children,parents);
	}

	void NetworkHandle::setParentHandle(const QList<ItemHandle*> children, ItemHandle * parent)
	{
		QList<ItemHandle*> parents;
		for (int i=0; i < children.size(); ++i)
			parents << parent;
		setParentHandle(children,parents);
	}
	
	void NetworkHandle::setHandleFamily(const QList<ItemHandle*>& handles, const QList<ItemFamily*>& newfamilies)
	{
		if (handles.size() != newfamilies.size()) return;

		SetHandleFamilyCommand * command = new SetHandleFamilyCommand(tr("family changed"), handles, newfamilies);
		history.push(command);

		emit handleFamilyChanged(this, command->handles, command->oldFamily);
	}

	void NetworkHandle::setHandleFamily(ItemHandle * handle, ItemFamily * newfamily)
	{
		setHandleFamily(QList<ItemHandle*>() << handle, QList<ItemFamily*>() << newfamily);
	}

	void NetworkHandle::setHandleFamily(const QList<ItemHandle*> handles, ItemFamily * newfamily)
	{
		QList<ItemFamily*> list;
		for (int i=0; i < handles.size(); ++i)
			list << newfamily;
		setHandleFamily(handles,list);
	}

	/*! \brief change numerical data table*/
	void NetworkHandle::changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const NumericalDataTable* newdata)
	{
		if (handle && handle->hasNumericalData(hashstring))
		{
			QUndoCommand * command = new ChangeDataCommand<qreal>(name,&(handle->numericalDataTable(hashstring)),newdata);

			history.push(command);

			QList<ItemHandle*> handles;
			handles += handle;
			emit dataChanged(handles);
		}
	}
	/*! \brief change a list of numerical data tables*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstrings, const QList<NumericalDataTable*>& newdata)
	{
		QList<NumericalDataTable*> oldTables, newTables;

		for (int i=0; i < handles.size() && i < hashstrings.size() && i < newdata.size(); ++i)
		{
			if (newdata[i] && handles[i] && handles[i]->hasNumericalData(hashstrings[i]))
			{
				oldTables += &(handles[i]->numericalDataTable(hashstrings[i]));
				newTables += newdata[i];
			}
		}

		if (oldTables.isEmpty() || newTables.isEmpty()) return;


		QUndoCommand * command = new ChangeDataCommand<qreal>(name,oldTables,newTables);

		history.push(command);

		emit dataChanged(handles);
	}
	/*! \brief change a list of numerical data tables*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<NumericalDataTable*>& newdata)
	{
		QList<NumericalDataTable*> oldTables, newTables;

		for (int i=0; i < handles.size() && i < newdata.size(); ++i)
		{
			if (newdata[i] && handles[i] && handles[i]->hasNumericalData(hashstring))
			{
				oldTables += &(handles[i]->numericalDataTable( hashstring ));
				newTables += newdata[i];
			}
		}

		if (oldTables.isEmpty() || newTables.isEmpty()) return;


		QUndoCommand * command = new ChangeDataCommand<qreal>(name,oldTables,newTables);

		history.push(command);

		emit dataChanged(handles);
	}
	/*! \brief change text data table*/
	void NetworkHandle::changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const TextDataTable* newdata)
	{
		if (handle && handle->hasTextData(hashstring))
		{
			QUndoCommand * command = new ChangeDataCommand<QString>(name,&(handle->textDataTable(hashstring)),newdata);
			history.push(command);

			QList<ItemHandle*> handles;
			handles += handle;
			emit dataChanged(handles);
		}
	}
	/*! \brief change a list of text data tables*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstrings, const QList<TextDataTable*>& newdata)
	{
		QList<TextDataTable*> oldTables, newTables;

		for (int i=0; i < handles.size() && i < hashstrings.size() && i < newdata.size(); ++i)
		{
			if (newdata[i] && handles[i] && handles[i]->hasTextData(hashstrings[i]))
			{
				oldTables += &(handles[i]->textDataTable( hashstrings[i] ));
				newTables += newdata[i];
			}
		}

		if (oldTables.isEmpty() || newTables.isEmpty()) return;


		QUndoCommand * command = new ChangeDataCommand<QString>(name,oldTables,newTables);

		history.push(command);

		emit dataChanged(handles);
	}
	/*! \brief change a list of text data tables*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<TextDataTable*>& newdata)
	{
		QList<TextDataTable*> oldTables, newTables;

		for (int i=0; i < handles.size() && i < newdata.size(); ++i)
		{
			if (newdata[i] && handles[i] && handles[i]->hasTextData(hashstring))
			{
				oldTables += &(handles[i]->textDataTable( hashstring ));
				newTables += newdata[i];
			}
		}

		if (oldTables.isEmpty() || newTables.isEmpty()) return;


		QUndoCommand * command = new ChangeDataCommand<QString>(name,oldTables,newTables);

		history.push(command);

		emit dataChanged(handles);
	}
	/*! \brief change two types of data tables*/
	void NetworkHandle::changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const NumericalDataTable* newdata1, const TextDataTable* newdata2)
	{
		if (handle && handle->hasNumericalData(hashstring) && handle->hasTextData(hashstring))
		{
			QUndoCommand * command = new Change2DataCommand<qreal,QString>(name,&(handle->numericalDataTable(hashstring)), newdata1, &(handle->textDataTable(hashstring)),newdata2);

			history.push(command);

			QList<ItemHandle*> handles;
			handles += handle;
			emit dataChanged(handles);
		}
	}
	/*! \brief change a list of two types of data tables*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstrings, const QList<NumericalDataTable*>& newdata1, const QList<TextDataTable*>& newdata2)
	{
		QList<TextDataTable*> oldTablesS, newTablesS;
		QList<NumericalDataTable*> oldTablesN, newTablesN;

		int j = 0;
		for (int i=0; j < handles.size() && j < hashstrings.size() && i < newdata1.size(); ++i, ++j)
		{
			if (newdata1[i] && handles[j] && handles[j]->hasNumericalData(hashstrings[j]))
			{
				oldTablesN += &(handles[j]->numericalDataTable( hashstrings[j] ));
				newTablesN += newdata1[i];
			}
		}

		for (int i=0; j < handles.size() && j < hashstrings.size() && i < newdata2.size(); ++i, ++j)
		{
			if (newdata2[i] && handles[j] && handles[j]->hasTextData(hashstrings[j]))
			{
				oldTablesS += &(handles[j]->textDataTable( hashstrings[j] ));
				newTablesS += newdata2[i];
			}
		}

		if ((oldTablesS.isEmpty() || newTablesS.isEmpty()) &&
			(oldTablesN.isEmpty() || newTablesN.isEmpty())) return;

		QUndoCommand * command = new Change2DataCommand<qreal,QString>(name,oldTablesN,newTablesN,oldTablesS,newTablesS);

		history.push(command);

		emit dataChanged(handles);
	}

	/*! \brief change a list of two types of data tables*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<NumericalDataTable*>& newdata1, const QList<TextDataTable*>& newdata2)
	{
		QList<TextDataTable*> oldTablesS, newTablesS;

		for (int i=0; i < handles.size() && i < newdata2.size(); ++i)
		{
			if (newdata2[i] && handles[i] && handles[i]->hasTextData(hashstring))
			{
				oldTablesS += &(handles[i]->textDataTable( hashstring ));
				newTablesS += newdata2[i];
			}
		}

		QList<NumericalDataTable*> oldTablesN, newTablesN;

		for (int i=0; i < handles.size()  && i < newdata1.size(); ++i)
		{
			if (newdata1[i] && handles[i] && handles[i]->hasNumericalData(hashstring))
			{
				oldTablesN += &(handles[i]->numericalDataTable( hashstring ));
				newTablesN += newdata1[i];
			}
		}

		if ((oldTablesS.isEmpty() || newTablesS.isEmpty()) &&
			(oldTablesN.isEmpty() || newTablesN.isEmpty())) return;


		QUndoCommand * command = new Change2DataCommand<qreal,QString>(name,oldTablesN,newTablesN,oldTablesS,newTablesS);

		history.push(command);

		emit dataChanged(handles);
	}

	/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<NumericalDataTable*>& olddata1, const QList<NumericalDataTable*>& newdata1, const QList<TextDataTable*>& olddata2, const QList<TextDataTable*>& newdata2)
	{
		if ((olddata1.isEmpty() || newdata1.isEmpty()) &&
			(olddata2.isEmpty() || newdata2.isEmpty())) return;

		QUndoCommand * command = new Change2DataCommand<qreal,QString>(name,olddata1,newdata1,olddata2,newdata2);

		history.push(command);

		emit dataChanged(handles);
	}

	/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<NumericalDataTable*>& olddata1, const QList<NumericalDataTable*>& newdata1)
	{
		if (olddata1.isEmpty() || newdata1.isEmpty()) return;

		QUndoCommand * command = new ChangeDataCommand<qreal>(name,olddata1,newdata1);

		history.push(command);

		emit dataChanged(handles);
	}
	
	/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<TextDataTable*>& olddata1, const QList<TextDataTable*>& newdata1)
	{
		if (olddata1.isEmpty() || newdata1.isEmpty()) return;

		QUndoCommand * command = new ChangeDataCommand<QString>(name,olddata1,newdata1);

		history.push(command);

		emit dataChanged(handles);
	}

	/*! \brief change a two types of data tables and also adds undo command to history window and emits associated signal(s)*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, NumericalDataTable* olddata1, const NumericalDataTable* newdata1, TextDataTable* olddata2, const TextDataTable* newdata2)
	{
		if ((!olddata1 || !newdata1) &&
			(!olddata2 || !newdata2)) return;

		QUndoCommand * command = new Change2DataCommand<qreal,QString>(name,olddata1,newdata1,olddata2,newdata2);

		history.push(command);

		emit dataChanged(handles);
	}

	/*! \brief change a data table and also adds undo command to history window and emits associated signal(s)*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, NumericalDataTable* olddata1, const NumericalDataTable* newdata1)
	{
		if (!olddata1 || !newdata1) return;

		QUndoCommand * command = new ChangeDataCommand<qreal>(name,olddata1,newdata1);

		history.push(command);

		emit dataChanged(handles);
	}

	/*! \brief change a data table and also adds undo command to history window and emits associated signal(s)*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, TextDataTable* olddata1, const TextDataTable* newdata1)
	{
		if (!olddata1 || !newdata1) return;

		QUndoCommand * command = new ChangeDataCommand<QString>(name,olddata1,newdata1);

		history.push(command);

		emit dataChanged(handles);
	}

	/*! \brief update symbols table*/
	void NetworkHandle::updateSymbolsTable()
	{
		symbolsTable.update();
	}

	/*! \brief update symbols table*/
	void NetworkHandle::updateSymbolsTable(int)
	{
		symbolsTable.update();
	}

	static double d = 1.0;
	static double* AddVariable(const char*, void*)
	{
		return &d;
	}

	bool NetworkHandle::parseMath(QString& s, QStringList& newvars)
	{
		static QStringList reservedWords;
		if (reservedWords.isEmpty())
			reservedWords << "time";

		mu::Parser parser;

		s.replace(QRegExp(tr("\\.(?!\\d)")),tr("_@@@_"));
		parser.SetExpr(s.toAscii().data());
		s.replace(tr("_@@@_"),tr("."));
		parser.SetVarFactory(AddVariable, 0);
		QString str;

		try
		{
			parser.Eval();

			// Get the map with the variables
			mu::varmap_type variables = parser.GetVar();

			// Get the number of variables
			mu::varmap_type::const_iterator item = variables.begin();

			// Query the variables
			for (; item!=variables.end(); ++item)
			{
				str = tr(item->first.data());
				str.replace(QRegExp(tr("[^A-Za-z0-9_]")),tr(""));
				str.replace(tr("_@@@_"),tr("."));
				QString str2 = str;
				str2.replace(tr("_"),tr("."));
				if (!reservedWords.contains(str) &&	
					!symbolsTable.uniqueHandlesWithDot.contains(str) &&
					!symbolsTable.uniqueHandlesWithUnderscore.contains(str)) //maybe new symbol in the formula
				{
					if (symbolsTable.uniqueDataWithDot.contains(str) && symbolsTable.uniqueDataWithDot[str].first)
					{
						if (! str.contains(QRegExp(tr("^")+symbolsTable.uniqueDataWithDot[str].first->fullName())) )
						{
							ItemHandle * handle = symbolsTable.uniqueDataWithDot[str].first;
							s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),handle->fullName() + tr(".") + str + tr("\\1"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + handle->fullName() + tr(".") + str + tr("\\2"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("$")),tr("\\1") + handle->fullName() + tr(".")  + str);
						}
					}
					else
					if (symbolsTable.uniqueDataWithUnderscore.contains(str) && symbolsTable.uniqueDataWithUnderscore[str].first)
					{
						if (! str.contains(QRegExp(tr("^")+symbolsTable.uniqueDataWithUnderscore[str].first->fullName())) )
						{
							ItemHandle * handle = symbolsTable.uniqueDataWithUnderscore[str].first;
							s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),handle->fullName() + tr(".") + str + tr("\\1"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + handle->fullName() + tr(".") + str + tr("\\2"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("$")),tr("\\1") + handle->fullName() + tr(".")  + str);
						}
					}
					
					else
					if (symbolsTable.nonuniqueData.contains(str) && symbolsTable.nonuniqueData[str].first)
					{
						if (! str.contains(QRegExp(tr("^")+symbolsTable.nonuniqueData[str].first->fullName())) )
						{
							ItemHandle * handle = symbolsTable.nonuniqueData[str].first;
							s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),handle->fullName() + tr(".") + str + tr("\\1"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + handle->fullName() + tr(".") + str + tr("\\2"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("$")),tr("\\1") + handle->fullName() + tr(".")  + str);
						}
					}
					else
					if (symbolsTable.uniqueDataWithDot.contains(str2) && symbolsTable.uniqueDataWithDot[str2].first)
					{
						if (! str2.contains(QRegExp(tr("^")+symbolsTable.uniqueDataWithDot[str2].first->fullName())) )
						{
							ItemHandle * handle = symbolsTable.uniqueDataWithDot[str2].first;
							s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),handle->fullName() + tr(".") + str2 + tr("\\1"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + handle->fullName() + tr(".") + str2 + tr("\\2"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("$")),tr("\\1") + handle->fullName() + tr(".")  + str2);
						}
						else
						{
							s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),str2 + tr("\\1"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + str + tr("\\2"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("$")),tr("\\1") + str);
						}
					}
					else
					{
						if (symbolsTable.nonuniqueHandles.contains(str) && symbolsTable.nonuniqueHandles[str])
						{
							s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),symbolsTable.nonuniqueHandles[str]->fullName() + tr("\\1"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + symbolsTable.nonuniqueHandles[str]->fullName() + tr("\\2"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("$")),tr("\\1") + symbolsTable.nonuniqueHandles[str]->fullName());
						}
						else
						if (symbolsTable.nonuniqueHandles.contains(str2) && symbolsTable.nonuniqueHandles[str2])
						{
							s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),symbolsTable.nonuniqueHandles[str2]->fullName() + tr("\\1"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + symbolsTable.nonuniqueHandles[str2]->fullName() + tr("\\2"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("$")),tr("\\1") + symbolsTable.nonuniqueHandles[str2]->fullName());
						}
						else
						{
							newvars << str;
						}
					}
				}
			}
		}
		catch(mu::Parser::exception_type &)
		{
			return false;
		}
		return true;
	}
	
	void NetworkHandle::assignHandles(const QList<QGraphicsItem*>& items, ItemHandle* newHandle)
	{
		if (!newHandle) return;
		QList<ItemHandle*> handles;
		for (int i=0; i < items.size(); ++i)
			handles += getHandle(items[i]);

		QUndoCommand * command = new AssignHandleCommand(tr("item defined"),items,newHandle);

		history.push(command);

		emit handlesChanged(this, items, handles);
	}
	
	void NetworkHandle::mergeHandles(const QList<ItemHandle*>& handles)
	{
		if (handles.isEmpty()) return;

		MergeHandlesCommand * command = new MergeHandlesCommand(tr("items merged"),this, handles);

		if (!command->newHandle)
		{
			delete command;
			return;
		}

		history.push(command);

		QList<QGraphicsItem*> items;
		for (int i=0; i < handles.size(); ++i)
			if (handles[i])
				items << handles[i]->allGraphicsItems();
	
		emit handlesChanged(this, items, handles);
	}
	
	void NetworkHandle::undo()
	{
		history.undo();
	}
	
	void NetworkHandle::redo()
	{
		history.redo();
	}
	
	void NetworkHandle::push(QUndoCommand * cmd)
	{
		history.push(cmd);
	}
	
	QString NetworkHandle::windowTitle() const
	{
		QString s;
		for (int i=0; i < networkWindows.size(); ++i)
			if (networkWindows[i])
			{
				s = networkWindows[i]->windowTitle();
				if (mainWindow && networkWindows[i] == mainWindow->currentNetworkWindow)
					break;
			}
		return s;
	}
	
	void NetworkHandle::remove(const QString& name, const QList<QGraphicsItem*>& items)
	{
		QHash< GraphicsScene*, QList<QGraphicsItem*> > hash;
		GraphicsScene * scene;
		
		for (int i=0; i < items.size(); ++i)
			if (items[i] && items[i]->scene())
			{
				scene = static_cast<GraphicsScene*>(items[i]->scene());
				if (!hash.contains(scene))
					hash[scene] = QList<QGraphicsItem*>();
				hash[scene] += items[i];				
			}
		
		QList<GraphicsScene*> scenes = hash.keys();
		for (int i=0; i < scenes.size(); ++i)
		{
			scenes[i]->remove(name,hash[ scenes[i] ]);
		}
	}
	
	ConsoleWindow * NetworkHandle::console() const
	{
		if (mainWindow)
			return mainWindow->console();
		return 0;
	}
	
	QString NetworkHandle::makeUnique(const QString& str, const QStringList& doNotUse) const
	{
		QString name = str;

		int k = name.length();
		while (k > 0 && name[k-1].isNumber())
			--k;
		if (k < name.length())
			name = name.left(k);

		int c = 1;
		QString str2 = name;
		
		bool taken = symbolsTable.uniqueHandlesWithDot.contains(str) || 
 					 symbolsTable.uniqueHandlesWithUnderscore.contains(str) || 
					 symbolsTable.uniqueDataWithDot.contains(str) ||
					 symbolsTable.uniqueDataWithUnderscore.contains(str) ||  
					 doNotUse.contains(str);
		if (!taken) return str;
		
		while (taken)
		{
			str2 = name + QString::number(c);
			taken = symbolsTable.uniqueHandlesWithDot.contains(str2) || 
 					 symbolsTable.uniqueHandlesWithUnderscore.contains(str2) || 
					 symbolsTable.uniqueDataWithDot.contains(str2) ||
					 symbolsTable.uniqueDataWithUnderscore.contains(str2) ||  
					 doNotUse.contains(str2);
			++c;
		}
		return str2;
	}
	
	QStringList NetworkHandle::makeUnique(const QStringList& oldnames, const QStringList& doNotUse) const
	{
		QStringList newnames;
		bool taken = true;
		int c,k;
		QString str2;
		
		for (int i=0; i < oldnames.size(); ++i)
		{
			QString name = oldnames[i];
			taken = symbolsTable.uniqueHandlesWithDot.contains(name) || 
 					 symbolsTable.uniqueHandlesWithUnderscore.contains(name) || 
					 symbolsTable.uniqueDataWithDot.contains(name) ||
					 symbolsTable.uniqueDataWithUnderscore.contains(name) ||  
					 doNotUse.contains(name);
			if (taken)
			{	
				k = name.length();
				while (k > 0 && name[k-1].isNumber())
					--k;
				if (k < name.length())
					name = name.left(k);
				c = 1;
				str2 = name;
			
				while (taken)
				{
					str2 = name + QString::number(c);
					taken = symbolsTable.uniqueHandlesWithDot.contains(str2) || 
		 					 symbolsTable.uniqueHandlesWithUnderscore.contains(str2) || 
							 symbolsTable.uniqueDataWithDot.contains(str2) ||
							 symbolsTable.uniqueDataWithUnderscore.contains(str2) ||  
							 doNotUse.contains(str2);
					++c;
				}
				newnames += str2;
			}
			else
			 newnames += name;
		}
		
		return newnames;
	}
	
	
}

