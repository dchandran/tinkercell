/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYWRITE.TXT

This is one of the main classes in Tinkercell
This file defines the ItemFamily, NodeFamily, and ConnectionFamily classes.
Each item in Tinkercell has an associated family.

****************************************************************************/
#include "ConsoleWindow.h"
#include "NetworkHandle.h"
#include "GraphicsScene.h"
#include "TextGraphicsItem.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "Tool.h"
#include "ItemHandle.h"
#include "UndoCommands.h"
#include <QRegExp>

namespace Tinkercell
{
	/********************************
	GLOBAL FUNCTIONS
	********************************/

	QString RemoveDisallowedCharactersFromName(const QString& s)
	{
 		QRegExp regex("[^0-9a-zA-z\\.\\_]");
		QString s2 = s;
		s2.replace(regex,"_");
		if (s2.length() > 0 && s2[0].isNumber())
			s2 = QString("_") + s2;
		s2.replace("^","_");
		s2.replace("\\","_");
		s2.replace("[","_");
		s2.replace("]","_");
		return s2;
	}

	ItemHandle * getHandle(QGraphicsItem * item)
	{
		if (!item/* || MainWindow::invalidPointers.contains((void*)item)*/) return 0;

		item = getGraphicsItem(item);

		NodeGraphicsItem * node = NodeGraphicsItem::cast(item);
		if (node)
			return (node->handle());

		ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(item);
		if (connection)
			return (connection->handle());

		TextGraphicsItem * textItem = TextGraphicsItem::cast(item);
		if (textItem)
			return (textItem->handle());

		ControlPoint * cp = ControlPoint::cast(item);
		if (cp)
		{
			return (cp->handle());
		}
		return 0;
	}
	
	QList<ItemHandle*> getHandle(const QList<QGraphicsItem*>& items)
	{
		QList<ItemHandle*> list;
		ItemHandle * h = 0;
		for (int i=0; i < items.size(); ++i)
				list << getHandle(items[i]);
		return list;
	}

	void setHandle(QGraphicsItem * item, ItemHandle * handle)
	{
		if (!item/* || MainWindow::invalidPointers.contains((void*)item)*/) return;

		item = getGraphicsItem(item);
		
		NodeGraphicsItem * node = NodeGraphicsItem::cast(item);
		if (node)
		{
			node->setHandle(handle);
			return;
		}

		ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(item);
		if (connection)
		{
			connection->setHandle(handle);
			return;
		}

		TextGraphicsItem * textItem = TextGraphicsItem::cast(item);
		if (textItem)
		{
			textItem->setHandle(handle);
			return;
		}
	}

    /**********************************
	ITEM DATA
	**********************************/
	ItemData::ItemData()
	{
	    numericalData.clear();
	    textData.clear();
	}

    ItemData::ItemData(const ItemData& copy)
    {
        QList<QString> keys = copy.numericalData.keys();
        for (int i=0; i < keys.size(); ++i)
            numericalData[ keys[i] ] = DataTable<qreal>(copy.numericalData[ keys[i] ]);

        keys = copy.textData.keys();
        for (int i=0; i < keys.size(); ++i)
            textData[ keys[i] ] = DataTable<QString>(copy.textData[ keys[i] ]);
    }

    ItemData& ItemData::operator = (const ItemData& copy)
    {
        numericalData = copy.numericalData;
        textData = copy.textData;
        return (*this);
    }

	/**********************************
	ITEM HANDLE
	**********************************/

	ItemHandle::~ItemHandle()
	{
        if (parent)
			parent->children.removeAll(this);

		parent = 0;
		if (data)
		{
            delete data;
			data = 0;
		}

		tools.clear();

		QList<QGraphicsItem*> list = graphicsItems;
		graphicsItems.clear();

		for (int i=0; i < list.size(); ++i)
			if (list[i] && !MainWindow::invalidPointers.contains((void*)list[i]))
			{
				setHandle(list[i],0);
				MainWindow::invalidPointers[ (void*)list[i] ] = true;
				delete list[i];
			}

		for (int i=0; i < children.size(); ++i)
		{
			if (children[i] && !MainWindow::invalidPointers.contains((void*)children[i]))
			{
				children[i]->parent = 0;
			    MainWindow::invalidPointers[ (void*)children[i] ] = true;
				delete children[i];
			}
		}
		
		children.clear();
	}
	ItemHandle::ItemHandle(const QString& s) : QObject()
	{
		network = 0;
		parent = 0;
		name = s;
		type = 0;
		data = new ItemData;
	}

	ItemHandle::ItemHandle(const ItemHandle& copy) : QObject()
	{
		network = copy.network;
		type = copy.type;
		name = copy.name;
		tools += copy.tools;
		if (copy.data)
			data = new ItemData(*(copy.data));
		else
			data = 0;
		parent = 0;
		setParent(copy.parent,false);
		
	}

	/*! \brief operator = */
	ItemHandle& ItemHandle::operator = (const ItemHandle& copy)
	{
		if (data)
			delete data;
		data = 0;
		
		for (int i=0; i < graphicsItems.size(); ++i)
			if (getHandle(graphicsItems[i]) == this)
				setHandle(graphicsItems[i],0);

		type = copy.type;
		name = copy.name;
		tools += copy.tools;

		if (copy.data)
			data = new ItemData(*(copy.data));

		setParent(copy.parent,false);
		
		return *this;
	}

	ItemHandle * ItemHandle::clone() const
	{
		return new ItemHandle(*this);
	}
	
	ItemFamily* ItemHandle::family() const
	{
		return 0;
	}
	
	void ItemHandle::setFamily(ItemFamily*, bool)
	{
	}

	void ItemHandle::rename(const QString& s)
	{
		if (network)
			network->rename(this,s);
	}
	
	void ItemHandle::changeData(const QString& hashstring0, const DataTable<qreal>* newdata)
	{
		if (!newdata) return;
		
		if (!data) data = new ItemData;
		
		QString hashstring = hashstring0.toLower();
		
		if (network)
		{
			if (name.isEmpty())
				network->changeData( hashstring + QString(" changed"), this, hashstring, newdata);
			else
				network->changeData( name + QString("'s ") + hashstring + QString(" changed"), this, hashstring, newdata);
		}
		else
			data->numericalData[hashstring] = (*newdata);
	}
	
	void ItemHandle::changeData(const QString& hashstring0, const DataTable<QString>* newdata)
	{
		if (!newdata) return;
		
		if (!data) data = new ItemData;
		
		QString hashstring = hashstring0.toLower();
		
		if (network)
		{
			if (name.isEmpty())
				network->changeData( hashstring + QString(" changed"), this, hashstring, newdata);
			else
				network->changeData( name + QString("'s ") + hashstring + QString(" changed"), this, hashstring, newdata);
		}
		else
			data->textData[hashstring] = (*newdata);			
	}

	void ItemHandle::setParent(ItemHandle * p, bool useCommand)
	{
		if (useCommand && network)
		{
			network->setParentHandle(this,p);
		}
		else
		{
			if (parent)
			{
				parent->children.removeAll(this);
			}
			parent = p;
			if (p && !p->children.contains(this))
			{
				p->children.append(this);
			}
		}
	}

	bool ItemHandle::isChildOf(ItemHandle * handle) const
	{
		if (!handle) return 0;

		ItemHandle * p = parent;

		while (p)
		{
			if (p == handle) return true;
			p = p->parent;
		}
		return false;
	}
	
	int ItemHandle::depth() const
	{
		int n = 0;
		ItemHandle * p = parent;

		while (p)
		{
			p = p->parent;
			++n;
		}
		return n;
	}

	QString ItemHandle::fullName(const QString& sep) const
	{
		ItemHandle * p = parent;
		QString name = this->name;
		while (p != 0)
		{
			if (!p->name.isEmpty())
				name = p->name + sep + name;
			p = p->parent;
		}
		if (name.isEmpty())
			name = QString("_");
		return name;
	}

	bool ItemHandle::isA(const ItemFamily* name) const
	{
		return (family() && family()->isA(name));
	}

	bool ItemHandle::isA(const QString& name) const
	{
		return (family() && family()->isA(name));
	}

	ItemHandle* ItemHandle::root(const QString& name) const
	{
		ItemHandle * p = const_cast<ItemHandle*>(this);
		ItemHandle * q = 0;
		while (p)
		{
			if (name.isEmpty() || p->isA(name))
				q = p;
			p = p->parent;
		}
		if (q && (name.isEmpty() || q->isA(name)))
			return q;
		return 0;
	}

	ItemHandle* ItemHandle::parentOfFamily(const QString& family) const
	{
		ItemHandle * p = parent;
		while (p)
		{
			if (p->isA(family)) return p;
			p = p->parent;
		}
		return 0;
	}
	
	QStringList ItemHandle::numericalDataNames() const
	{
		if (data)
			return data->numericalData.keys();
		return QStringList();
	}
	
	QStringList ItemHandle::textDataNames() const
	{
		if (data)
			return data->textData.keys();
		return QStringList();
	}

	bool ItemHandle::hasNumericalData(const QString& name) const
	{
		return (data && data->numericalData.contains(name.toLower()));
	}

	bool ItemHandle::hasTextData(const QString& name) const
	{
		return (data && data->textData.contains(name.toLower()));
	}

	QList<QGraphicsItem*> ItemHandle::allGraphicsItems() const
	{
		QList<QGraphicsItem*> list = graphicsItems;
		QList<ItemHandle*> handles = children;
		for (int i=0; i < handles.size(); ++i)
		{
			if (handles[i])
			{
				for (int j=0; j < handles[i]->children.size(); ++j)
					if (!handles.contains(handles[i]->children[j]))
						handles << handles[i]->children[j];
				for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
					if (!list.contains(handles[i]->graphicsItems[j]))
						list << handles[i]->graphicsItems[j];
			}
		}
		return list;
	}

	qreal ItemHandle::numericalData(const QString& name0, int row, int column) const
	{
		QString name = name0.toLower();
		if (data && data->numericalData.contains(name))
		{
			return data->numericalData[name].at(row,column);
		}
		return 0.0;
	}

	qreal ItemHandle::numericalData(const QString& name0, const QString& row, const QString& column) const
	{
		QString name = name0.toLower();

		if (data && data->numericalData.contains(name))
		{
			if (!row.isEmpty() && !column.isEmpty())
				return data->numericalData[name].at(row,column);
				
			if (row.isEmpty() && column.isEmpty())
				return data->numericalData[name].at(0,0);
			else
				if (row.isEmpty())
					return data->numericalData[name].at(0,column);
				else
				if (column.isEmpty())
					return data->numericalData[name].at(row,0);
		}
		return 0.0;
	}

	QString ItemHandle::textData(const QString& name0, int row, int column) const
	{
		QString name = name0.toLower();

		if (data && data->textData.contains(name))
		{
			return data->textData[name].at(row,column);
		}
		return QString();
	}

	QString ItemHandle::textData(const QString& name0, const QString& row, const QString& column) const
	{
		QString name = name0.toLower();
				
		if (data && data->textData.contains(name))
		{
			if (!row.isEmpty() && !column.isEmpty())
				return data->textData[name].at(row,column);
				
			if (row.isEmpty() && column.isEmpty())
				return data->textData[name].at(0,0);
			else
				if (row.isEmpty())
					return data->textData[name].at(0,column);
				else
				if (column.isEmpty())
					return data->textData[name].at(row,0);
		}
		return QString();
	}

	qreal& ItemHandle::numericalData(const QString& name0, int row, int column)
	{
		if (!data) data = new ItemData;
		QString name = name0.toLower();

		if (!data->numericalData.contains(name))
		{
			data->numericalData[name] = DataTable<qreal>();
		}

		return data->numericalData[name].value(row,column);
	}

	qreal& ItemHandle::numericalData(const QString& name0, const QString& row, const QString& column)
	{
		if (!data) data = new ItemData;
		QString name = name0.toLower();

		if (!data->numericalData.contains(name))
		{
			data->numericalData[name] = DataTable<qreal>();
		}
				
		if (row.isEmpty() && column.isEmpty())
			return data->numericalData[name].value(0,0);
		else
			if (row.isEmpty())
				return data->numericalData[name].value(0,column);
			else
			if (column.isEmpty())
				return data->numericalData[name].value(row,0);

		return data->numericalData[name].value(row,column);
	}

	QString& ItemHandle::textData(const QString& name0, int row, int column)
	{
		if (!data) data = new ItemData;
		QString name = name0.toLower();

		if (!data->textData.contains(name))
		{
			data->textData[name] = DataTable<QString>();
		}

		return data->textData[name].value(row,column);
	}

	QString& ItemHandle::textData(const QString& name0, const QString& row, const QString& column)
	{
		if (!data) data = new ItemData;
		QString name = name0.toLower();

		if (!data->textData.contains(name))
		{
			data->textData[name] = DataTable<QString>();
		}
		
		if (row.isEmpty() && column.isEmpty())
			return data->textData[name].value(0,0);
		else
			if (row.isEmpty())
				return data->textData[name].value(0,column);
			else
			if (column.isEmpty())
				return data->textData[name].value(row,0);

		return data->textData[name].value(row,column);
	}

	DataTable<qreal>& ItemHandle::numericalDataTable(const QString& name0)
	{
		if (!data) data = new ItemData;
		QString name = name0.toLower();

		if (!data->numericalData.contains(name))
		{
			data->numericalData[name] = DataTable<qreal>();
		}

		return data->numericalData[name];
	}

	DataTable<QString>& ItemHandle::textDataTable(const QString& name0)
	{
		if (!data) data = new ItemData;
		QString name = name0.toLower();

		if (!data->textData.contains(name))
		{
			data->textData[name] = DataTable<QString>();
		}

		return data->textData[name];
	}

	QList<ItemHandle*> ItemHandle::allChildren() const
	{
		QList<ItemHandle*> handles = children;
		for (int i=0; i < handles.size(); ++i)
		{
			if (handles[i])
			{
				for (int j=0; j < handles[i]->children.size(); ++j)
					if (!handles.contains(handles[i]->children[j]))
						handles << handles[i]->children[j];
			}
		}
		return handles;
	}

	/**********************************
	PART HANDLE
	**********************************/

	NodeHandle * NodeHandle::cast(ItemHandle * item)
	{
		if (item && item->type == NodeHandle::TYPE)
			return static_cast<NodeHandle*>(item);
		return 0;
	}

	QList<NodeHandle*> NodeHandle::cast(const QList<ItemHandle*>& items)
	{
		QList<NodeHandle*> nodes;
		
		for (int i=0; i < items.size(); ++i)
			if (items[i] && items[i]->type == NodeHandle::TYPE)
				nodes << static_cast<NodeHandle*>(items[i]);
		return nodes;
	}

	NodeHandle::NodeHandle(const QString& s, NodeFamily * family) : ItemHandle(s)
	{
		nodeFamily = family;
		type = NodeHandle::TYPE;
	}

	NodeHandle::NodeHandle(NodeFamily * family, NodeGraphicsItem * item) : ItemHandle()
	{
		type = NodeHandle::TYPE;
		nodeFamily = family;
		if (item)
		{
			graphicsItems += item;
			item->setHandle(this);
		}
	}
	
	NodeHandle::NodeHandle(NodeFamily * family, const QString& s) : ItemHandle(s)
	{
		nodeFamily = family;
		type = NodeHandle::TYPE;
	}

	void NodeHandle::setFamily(ItemFamily * p, bool useCommand)
	{
		if (nodeFamily == p) return;

		if (useCommand && network)
		{
			ItemHandle * item = const_cast<NodeHandle*>(this);
			network->setHandleFamily(item,p);
		}
		else
		{
			if (!p)
				nodeFamily = 0;
			else
			{
				NodeFamily * itemFamily = NodeFamily::cast(p);
				if (itemFamily)
					nodeFamily = itemFamily;
			}
		}
	}

	ItemFamily* NodeHandle::family() const
	{
		return nodeFamily;
	}

	NodeHandle::NodeHandle(const NodeHandle & copy) : ItemHandle(copy)
	{
		nodeFamily = copy.nodeFamily;
	}

	NodeHandle& NodeHandle::operator = (const NodeHandle& copy)
	{
	    ItemHandle::operator=(copy);
	    nodeFamily = copy.nodeFamily;
		return *this;
	}

	ItemHandle * NodeHandle::clone() const
	{
		return (new NodeHandle(*this));
	}

	QList<ConnectionHandle*> NodeHandle::connections() const
	{
		QList<ConnectionHandle*> list;

		if (graphicsItems.size() > 0)
		{
			QList<ConnectionGraphicsItem*> connections;
			NodeGraphicsItem * node = 0;
			for (int i=0; i < graphicsItems.size(); ++i)
			{
				node = NodeGraphicsItem::cast(graphicsItems[i]);
				if (node)
				{
					connections = node->connections();
					for (int j=0; j < connections.size(); ++j)
						if (connections[j] && connections[j]->handle() &&
							connections[j]->handle()->type == ConnectionHandle::TYPE)
							list << static_cast<ConnectionHandle*>(connections[j]->handle());
				}
			}
		}
		return list;
	}

	/************************************
	CONNECTION HANDLE
	*************************************/

	ConnectionHandle * ConnectionHandle::cast(ItemHandle * item)
	{
		if (item && item->type == ConnectionHandle::TYPE)
			return static_cast<ConnectionHandle*>(item);
		return 0;
	}
	
	QList<ConnectionHandle*> ConnectionHandle::cast(const QList<ItemHandle*>& items)
	{
		QList<ConnectionHandle*> nodes;
		
		for (int i=0; i < items.size(); ++i)
			if (items[i] && items[i]->type == ConnectionHandle::TYPE)
				nodes << static_cast<ConnectionHandle*>(items[i]);
		return nodes;
	}

	ConnectionHandle::ConnectionHandle(const QString& s, ConnectionFamily * family) : ItemHandle(s)
	{
		type = ConnectionHandle::TYPE;
		parent = 0;
		connectionFamily = family;
	}
	

	ConnectionHandle::ConnectionHandle(ConnectionFamily * family, const QString& s) : ItemHandle(s)
	{
		type = ConnectionHandle::TYPE;
		connectionFamily = family;
	}

	ConnectionHandle::ConnectionHandle(ConnectionFamily * family, ConnectionGraphicsItem * item) : ItemHandle()
	{
		type = ConnectionHandle::TYPE;
		connectionFamily = family;
		if (item)
		{
			graphicsItems += item;
			item->setHandle(this);
		}
	}
	
	void ConnectionHandle::setFamily(ItemFamily * p, bool useCommand)
	{	
		if (connectionFamily == p) return;

		if (useCommand && network)
		{
			ItemHandle * item = const_cast<ConnectionHandle*>(this);
			network->setHandleFamily(item,p);
		}
		else
		{
			if (!p)
				connectionFamily = 0;
			else
			{
				ConnectionFamily * itemFamily = ConnectionFamily::cast(p);
				if (itemFamily)
					this->connectionFamily = itemFamily;
			}
		}
	}

	ConnectionHandle::ConnectionHandle(const ConnectionHandle & copy) : ItemHandle(copy)
	{
		connectionFamily = copy.connectionFamily;
		nodesWithRoles = copy.nodesWithRoles;
	}

	ConnectionHandle& ConnectionHandle::operator = (const ConnectionHandle& copy)
	{
	    ItemHandle::operator=(copy);
	    connectionFamily = copy.connectionFamily;
		nodesWithRoles = copy.nodesWithRoles;
		return *this;
	}

	ItemHandle * ConnectionHandle::clone() const
	{
		return (new ConnectionHandle(*this));
	}

	ItemFamily* ConnectionHandle::family() const
	{
		return connectionFamily;
	}

	QList<NodeHandle*> ConnectionHandle::nodes(int role) const
	{
		QList<NodeHandle*> nodeslist;

		if (graphicsItems.size() > 0)
		{
			if (role == -1 ) return nodesIn();
			if (role == 1 ) return nodesOut();
			for (int j=0; j < graphicsItems.size(); ++j)
			{
				ConnectionGraphicsItem * connection;
				QList<NodeGraphicsItem*> nodes;
				connection = ConnectionGraphicsItem::cast(graphicsItems[j]);
				if (connection)
				{
					nodes = connection->nodes();
					for (int i=0; i < nodes.size(); ++i)
					{
						if (nodes[i] && nodes[i]->handle() &&
							nodes[i]->handle()->type == NodeHandle::TYPE)
							nodeslist << static_cast<NodeHandle*>(nodes[i]->handle());
					}
				}
			}
		}
		else
		{
			for (int i=0; i < nodesWithRoles.size(); ++i)
				if (nodesWithRoles[i].first && (role == 0 || nodesWithRoles[i].second == role))
					nodeslist << nodesWithRoles[i].first;
		}
		return nodeslist;
	}

	QList<NodeHandle*> ConnectionHandle::nodesIn() const
	{
		QList<NodeHandle*> nodesList;
		if (graphicsItems.size() > 0)
		{
			ConnectionGraphicsItem * connection;
			QList<NodeGraphicsItem*> nodesDisconnected, nodesIn, nodesOut;
			for (int j=0; j < graphicsItems.size(); ++j)
			{
				connection = ConnectionGraphicsItem::cast(graphicsItems[j]);
				if (connection && !connection->isModifier())
				{
					nodesIn = connection->nodesWithoutArrows();
					nodesOut = connection->nodesWithArrows();
					nodesDisconnected = connection->nodesDisconnected();

					bool ok = false;
					for (int i=0; i < nodesOut.size(); ++i)
					{
						if (nodesOut[i] && nodesOut[i]->handle())
						{
							ok = true;
							break;
						}
					}

					if (!ok) continue;

					for (int i=0; i < nodesIn.size(); ++i)
					{
						if (nodesIn[i] && !nodesDisconnected.contains(nodesIn[i]) &&
							nodesIn[i]->handle() &&
							nodesIn[i]->handle()->type == NodeHandle::TYPE)
							nodesList << static_cast<NodeHandle*>(nodesIn[i]->handle());
					}
				}
			}
		}
		else
		{
			nodesList = nodes(-1);
		}	
		return nodesList;
	}

	QList<NodeHandle*> ConnectionHandle::nodesOut() const
	{
		QList<NodeHandle*> nodesList;
		if (graphicsItems.size() > 0)
		{
			QList<NodeGraphicsItem*> nodesIn, nodesOut, nodesDisconnected;
			ConnectionGraphicsItem * connection;
			for (int j=0; j < graphicsItems.size(); ++j)
			{
				connection = ConnectionGraphicsItem::cast(graphicsItems[j]);
				if (connection && !connection->isModifier())
				{
					nodesIn = connection->nodesWithoutArrows();
					nodesOut = connection->nodesWithArrows();
					nodesDisconnected = connection->nodesDisconnected();

					bool ok = false;
					for (int i=0; i < nodesIn.size(); ++i)
					{
						if (nodesIn[i] && nodesIn[i]->handle())
						{
							ok = true;
							break;
						}
					}

					if (!ok) continue;

					for (int i=0; i < nodesOut.size(); ++i)
					{
						if (nodesOut[i] && !nodesDisconnected.contains(nodesOut[i]) &&
							nodesOut[i]->handle() &&
							nodesOut[i]->handle()->type == NodeHandle::TYPE)
							nodesList << static_cast<NodeHandle*>(nodesOut[i]->handle());
					}
				}
			}
		}
		else
		{
			nodesList = nodes(1);
		}
		return nodesList;
	}
	
	void ConnectionHandle::addNode(NodeHandle * h, int role)
	{
		if (h)
			nodesWithRoles << QPair<NodeHandle*,int>(h,role);
	}
	
	void ConnectionHandle::clearNodes()
	{
		nodesWithRoles.clear();
	}

	QList<ItemFamily*> ConnectionHandle::findValidChildFamilies() const
	{
		QList<ItemFamily*> list;
		ConnectionFamily * connection = ConnectionFamily::cast(family());
		if (connection)
			list = connection->findValidChildFamilies(nodes());
		return list;
	}

}

