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
		history.clear();
	}
	
	QList<ItemHandle*> NetworkHandle::findItem(const QString& s) const
	{
		QList<ItemHandle*> items;
		if (symbolsTable.uniqueItems.contains(s))
		{
			items << symbolsTable.uniqueItems[s];
		}
		else
		if (symbolsTable.nonuniqueItems.contains(s))
		{
			items = symbolsTable.nonuniqueItems.values(s);
		}
		return items;
	}
	
	QList<ItemHandle*> NetworkHandle::findItem(const QStringList& list) const
	{
		QList<ItemHandle*> items;
		
		for (int i=0; i < list.size(); ++i)
		{
			QString s = list[i];
			if (symbolsTable.uniqueItems.contains(s))
			{
				items += symbolsTable.uniqueItems[s];
			}
			else
			if (symbolsTable.nonuniqueItems.contains(s))
			{
				QList<ItemHandle*> items2 = symbolsTable.nonuniqueItems.values(s);
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

		if (symbolsTable.uniqueData.contains(s))
			list = symbolsTable.uniqueData.values(s);
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
			if (symbolsTable.uniqueData.contains(s))
			{
				data += symbolsTable.uniqueData[s];
			}
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
		disconnect();			
		disconnect(&history, SIGNAL(indexChanged(int)), this, SLOT(updateSymbolsTable(int)));
		disconnect(&history, SIGNAL(indexChanged(int)), mainWindow, SIGNAL(historyChanged(int)));
		
		QList<NetworkWindow*> list = networkWindows;
		for (int i=1; i < list.size(); ++i)
			if (list[i])
			{
				list[i]->disconnect();
				list[i]->close();
			}

		if (mainWindow)
		{
			mainWindow->allNetworks.removeAll(this);
		}
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
		
		return createScene(graphicsItems);
	}

	NetworkHandle::NetworkHandle(MainWindow * main) : QObject(main), mainWindow(main), symbolsTable(this)
	{		
		if (main && !main->allNetworks.contains(this))
			main->allNetworks << this;

		connect(&history, SIGNAL(indexChanged(int)), this, SLOT(updateSymbolsTable(int)));
		connect(&history, SIGNAL(indexChanged(int)), mainWindow, SIGNAL(historyChanged(int)));
		
		connect(this,SIGNAL(parentHandleChanged(NetworkHandle*, const QList<ItemHandle*>&, const QList<ItemHandle*>&)),
				main ,SIGNAL(parentHandleChanged(NetworkHandle*, const QList<ItemHandle*>&, const QList<ItemHandle*>&)));

		connect(this,SIGNAL(dataChanged(const QList<ItemHandle*>&)),
			main ,SIGNAL(dataChanged(const QList<ItemHandle*>&)));

		connect(this,SIGNAL(itemsRenamed(NetworkHandle*, const QList<ItemHandle*>&, const QList<QString>&, const QList<QString>&)),
			main ,SIGNAL(itemsRenamed(NetworkHandle*, const QList<ItemHandle*>&, const QList<QString>&, const QList<QString>&)));
	}

	ItemHandle* NetworkHandle::globalHandle()
	{
		return &(symbolsTable.globalItem);
	}

	QList<ItemHandle*> NetworkHandle::handles()
	{
		QList<ItemHandle*> handles = symbolsTable.uniqueItems.values();
		handles << &(symbolsTable.globalItem);
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

	TextEditor * NetworkHandle::currentTextEditor() const
	{
		if (!mainWindow || !mainWindow->currentNetworkWindow || !networkWindows.contains(mainWindow->currentNetworkWindow))
			return 0;
		
		return mainWindow->currentNetworkWindow->editor;
	}

	void NetworkHandle::rename(const QString& oldname, const QString& s)
	{
		if (oldname == s) return;

		QList<ItemHandle*> items;
		QList<QString> oldNames, newNames;
		oldNames += oldname;

		QString newname = Tinkercell::RemoveDisallowedCharactersFromName(s);

		if (symbolsTable.uniqueItems.contains(newname))
		{
			QStringList existingNames = symbolsTable.uniqueItems.keys();

			QString n = newname;

			if (newname[ newname.size()-1 ].isNumber())
				n = newname.left(newname.size()-1);

			int i = 0;

			while (symbolsTable.uniqueItems.contains(n))
				n = newname.left(newname.size()-1) + QString::number(i);

			newname = n;
		}

		newNames += newname;

		QUndoCommand * command = new RenameCommand(tr("name changed"),this->handles(),oldname,newname);

		history.push(command);

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

		newNames += newname;

		QUndoCommand * command = new RenameCommand(tr("name changed"),this,items,newNames);

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

				newNames += newname;
			}

		QUndoCommand * command = new RenameCommand(tr("name changed"),this,items,newNames);

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

	/*! \brief change numerical data table*/
	void NetworkHandle::changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const DataTable<qreal>* newdata)
	{
		if (handle && handle->data && handle->data->numericalData.contains(hashstring))
		{
			QUndoCommand * command = new ChangeDataCommand<qreal>(name,&(handle->data->numericalData[hashstring]),newdata);

			history.push(command);

			QList<ItemHandle*> handles;
			handles += handle;
			emit dataChanged(handles);
		}
	}
	/*! \brief change a list of numerical data tables*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstrings, const QList<DataTable<qreal>*>& newdata)
	{
		QList<DataTable<qreal>*> oldTables, newTables;

		for (int i=0; i < handles.size() && i < hashstrings.size() && i < newdata.size(); ++i)
		{
			if (newdata[i] && handles[i] && handles[i]->data && handles[i]->data->numericalData.contains(hashstrings[i]))
			{
				oldTables += &(handles[i]->data->numericalData[ hashstrings[i] ]);
				newTables += newdata[i];
			}
		}

		if (oldTables.isEmpty() || newTables.isEmpty()) return;


		QUndoCommand * command = new ChangeDataCommand<qreal>(name,oldTables,newTables);

		history.push(command);

		emit dataChanged(handles);
	}
	/*! \brief change a list of numerical data tables*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<qreal>*>& newdata)
	{
		QList<DataTable<qreal>*> oldTables, newTables;

		for (int i=0; i < handles.size() && i < newdata.size(); ++i)
		{
			if (newdata[i] && handles[i] && handles[i]->data && handles[i]->data->numericalData.contains(hashstring))
			{
				oldTables += &(handles[i]->data->numericalData[ hashstring ]);
				newTables += newdata[i];
			}
		}

		if (oldTables.isEmpty() || newTables.isEmpty()) return;


		QUndoCommand * command = new ChangeDataCommand<qreal>(name,oldTables,newTables);

		history.push(command);

		emit dataChanged(handles);
	}
	/*! \brief change text data table*/
	void NetworkHandle::changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const DataTable<QString>* newdata)
	{
		if (handle && handle->data && handle->data->textData.contains(hashstring))
		{
			QUndoCommand * command = new ChangeDataCommand<QString>(name,&(handle->data->textData[hashstring]),newdata);
			history.push(command);

			QList<ItemHandle*> handles;
			handles += handle;
			emit dataChanged(handles);
		}
	}
	/*! \brief change a list of text data tables*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstrings, const QList<DataTable<QString>*>& newdata)
	{
		QList<DataTable<QString>*> oldTables, newTables;

		for (int i=0; i < handles.size() && i < hashstrings.size() && i < newdata.size(); ++i)
		{
			if (newdata[i] && handles[i] && handles[i]->data && handles[i]->data->textData.contains(hashstrings[i]))
			{
				oldTables += &(handles[i]->data->textData[ hashstrings[i] ]);
				newTables += newdata[i];
			}
		}

		if (oldTables.isEmpty() || newTables.isEmpty()) return;


		QUndoCommand * command = new ChangeDataCommand<QString>(name,oldTables,newTables);

		history.push(command);

		emit dataChanged(handles);
	}
	/*! \brief change a list of text data tables*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<QString>*>& newdata)
	{
		QList<DataTable<QString>*> oldTables, newTables;

		for (int i=0; i < handles.size() && i < newdata.size(); ++i)
		{
			if (newdata[i] && handles[i] && handles[i]->data && handles[i]->data->textData.contains(hashstring))
			{
				oldTables += &(handles[i]->data->textData[ hashstring ]);
				newTables += newdata[i];
			}
		}

		if (oldTables.isEmpty() || newTables.isEmpty()) return;


		QUndoCommand * command = new ChangeDataCommand<QString>(name,oldTables,newTables);

		history.push(command);

		emit dataChanged(handles);
	}
	/*! \brief change two types of data tables*/
	void NetworkHandle::changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const DataTable<qreal>* newdata1, const DataTable<QString>* newdata2)
	{
		if (handle && handle->data && handle->data->numericalData.contains(hashstring) && handle->data->textData.contains(hashstring))
		{
			QUndoCommand * command = new Change2DataCommand<qreal,QString>(name,&(handle->data->numericalData[hashstring]), newdata1, &(handle->data->textData[hashstring]),newdata2);

			history.push(command);

			QList<ItemHandle*> handles;
			handles += handle;
			emit dataChanged(handles);
		}
	}
	/*! \brief change a list of two types of data tables*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstrings, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& newdata2)
	{
		QList<DataTable<QString>*> oldTablesS, newTablesS;
		QList<DataTable<qreal>*> oldTablesN, newTablesN;

		int j = 0;
		for (int i=0; j < handles.size() && j < hashstrings.size() && i < newdata1.size(); ++i, ++j)
		{
			if (newdata1[i] && handles[j] && handles[j]->data && handles[j]->data->numericalData.contains(hashstrings[j]))
			{
				oldTablesN += &(handles[j]->data->numericalData[ hashstrings[j] ]);
				newTablesN += newdata1[i];
			}
		}

		for (int i=0; j < handles.size() && j < hashstrings.size() && i < newdata2.size(); ++i, ++j)
		{
			if (newdata2[i] && handles[j] && handles[j]->data && handles[j]->data->textData.contains(hashstrings[j]))
			{
				oldTablesS += &(handles[j]->data->textData[ hashstrings[j] ]);
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
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& newdata2)
	{
		QList<DataTable<QString>*> oldTablesS, newTablesS;

		for (int i=0; i < handles.size() && i < newdata2.size(); ++i)
		{
			if (newdata2[i] && handles[i] && handles[i]->data && handles[i]->data->textData.contains(hashstring))
			{
				oldTablesS += &(handles[i]->data->textData[ hashstring ]);
				newTablesS += newdata2[i];
			}
		}

		QList<DataTable<qreal>*> oldTablesN, newTablesN;

		for (int i=0; i < handles.size()  && i < newdata1.size(); ++i)
		{
			if (newdata1[i] && handles[i] && handles[i]->data && handles[i]->data->numericalData.contains(hashstring))
			{
				oldTablesN += &(handles[i]->data->numericalData[ hashstring ]);
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
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<DataTable<qreal>*>& olddata1, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& olddata2, const QList<DataTable<QString>*>& newdata2)
	{
		if ((olddata1.isEmpty() || newdata1.isEmpty()) &&
			(olddata2.isEmpty() || newdata2.isEmpty())) return;

		QUndoCommand * command = new Change2DataCommand<qreal,QString>(name,olddata1,newdata1,olddata2,newdata2);

		history.push(command);

		emit dataChanged(handles);
	}

	/*! \brief change a two types of data tables and also adds undo command to history window and emits associated signal(s)*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, DataTable<qreal>* olddata1, const DataTable<qreal>* newdata1, DataTable<QString>* olddata2, const DataTable<QString>* newdata2)
	{
		if ((!olddata1 || !newdata1) &&
			(!olddata2 || !newdata2)) return;

		QUndoCommand * command = new Change2DataCommand<qreal,QString>(name,olddata1,newdata1,olddata2,newdata2);

		history.push(command);

		emit dataChanged(handles);
	}

	/*! \brief change a data table and also adds undo command to history window and emits associated signal(s)*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, DataTable<qreal>* olddata1, const DataTable<qreal>* newdata1)
	{
		if (!olddata1 || !newdata1) return;

		QUndoCommand * command = new ChangeDataCommand<qreal>(name,olddata1,newdata1);

		history.push(command);

		emit dataChanged(handles);
	}

	/*! \brief change a data table and also adds undo command to history window and emits associated signal(s)*/
	void NetworkHandle::changeData(const QString& name, const QList<ItemHandle*>& handles, DataTable<QString>* olddata1, const DataTable<QString>* newdata1)
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
					!symbolsTable.uniqueItems.contains(str)) //maybe new symbol in the formula
				{
					if (symbolsTable.uniqueData.contains(str) && symbolsTable.uniqueData[str].first)
					{
						if (! str.contains(QRegExp(tr("^")+symbolsTable.uniqueData[str].first->fullName())) )
						{
							ItemHandle * handle = symbolsTable.uniqueData[str].first;
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
					if (symbolsTable.uniqueData.contains(str2) && symbolsTable.uniqueData[str2].first)
					{
						if (! str2.contains(QRegExp(tr("^")+symbolsTable.uniqueData[str2].first->fullName())) )
						{
							ItemHandle * handle = symbolsTable.uniqueData[str2].first;
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
						if (symbolsTable.nonuniqueItems.contains(str) && symbolsTable.nonuniqueItems[str])
						{
							s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),symbolsTable.nonuniqueItems[str]->fullName() + tr("\\1"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + symbolsTable.nonuniqueItems[str]->fullName() + tr("\\2"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("$")),tr("\\1") + symbolsTable.nonuniqueItems[str]->fullName());
						}
						else
						if (symbolsTable.nonuniqueItems.contains(str2) && symbolsTable.nonuniqueItems[str2])
						{
							s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),symbolsTable.nonuniqueItems[str2]->fullName() + tr("\\1"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + symbolsTable.nonuniqueItems[str2]->fullName() + tr("\\2"));
							s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("$")),tr("\\1") + symbolsTable.nonuniqueItems[str2]->fullName());
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

}

