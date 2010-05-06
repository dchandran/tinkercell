/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYWRITE.TXT

This is one of the main classes in Tinkercell
This file defines the ItemFamily, NodeFamily, and ConnectionFamily classes.
Each item in Tinkercell has an associated family.

****************************************************************************/

#include "GraphicsScene.h"
#include "TextGraphicsItem.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextItem.h"
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
		if (!item || MainWindow::invalidPointers.contains((void*)item)) return 0;

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

	void setHandle(QGraphicsItem * item, ItemHandle * handle)
	{
		if (!item || MainWindow::invalidPointers.contains((void*)item)) return;

		item = getGraphicsItem(item);

		NodeGraphicsItem * node = NodeGraphicsItem::cast(item);
		if (node)
		{
			node->setHandle(handle);
			return;
		}

		ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(item);
		if (connection)
		{
			connection->setHandle(handle);
			return;
		}

		TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(item);
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
        numericalData.clear();
        textData.clear();

        QList<QString> keys = copy.numericalData.keys();
        for (int i=0; i < keys.size(); ++i)
            numericalData[ keys[i] ] = DataTable<qreal>(copy.numericalData[ keys[i] ]);

        keys = copy.textData.keys();
        for (int i=0; i < keys.size(); ++i)
            textData[ keys[i] ] = DataTable<QString>(copy.textData[ keys[i] ]);

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
		{
			setHandle(list[i],0);
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
		visible = true;
		parent = 0;
		data = 0;
		name = s;
		type = 0;
	}

	ItemHandle::ItemHandle(const ItemHandle& copy) : QObject()
	{
		visible = copy.visible;
		type = copy.type;
		name = copy.name;
		tools += copy.tools;
		if (copy.data)
			data = new ItemData(*(copy.data));
		else
			data = 0;
		parent = 0;
		setParent(copy.parent);
		
		/*for (int i=0; i < copy.children.size(); ++i)
			if (copy.children[i] && copy.children[i]->graphicsItems.isEmpty() && copy.children[i]->textItems.isEmpty())
			{
				children << copy.children[i]->clone();
			}*/
	}

	/*! \brief operator = */
	ItemHandle& ItemHandle::operator = (const ItemHandle& copy)
	{
		visible = copy.visible;
		if (data)
			delete data;
		
		for (int i=0; i < graphicsItems.size(); ++i)
			if (getHandle(graphicsItems[i]) == this)
				setHandle(graphicsItems[i],0);

		type = copy.type;
		name = copy.name;
		tools += copy.tools;
		if (copy.data)
			data = new ItemData(*(copy.data));
		else
			data = 0;
		parent = 0;
		setParent(copy.parent);
		
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

	void ItemHandle::setParent(ItemHandle * p)
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

	bool ItemHandle::hasNumericalData(const QString& name) const
	{
		return (data && data->numericalData.contains(name));
	}

	bool ItemHandle::hasTextData(const QString& name) const
	{
		return (data && data->textData.contains(name));
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

	qreal ItemHandle::numericalData(const QString& name, int row, int column) const
	{
		if (data && data->numericalData.contains(name))
		{
			return data->numericalData[name].at(row,column);
		}
		return 0.0;
	}

	qreal ItemHandle::numericalData(const QString& name, const QString& row, const QString& column) const
	{
		if (data && data->numericalData.contains(name))
		{
			return data->numericalData[name].at(row,column);
		}
		return 0.0;
	}

	QString ItemHandle::textData(const QString& name, int row, int column) const
	{
		if (data && data->textData.contains(name))
		{
			return data->textData[name].at(row,column);
		}
		return QString();
	}

	QString ItemHandle::textData(const QString& name, const QString& row, const QString& column) const
	{
		if (data && data->textData.contains(name))
		{
			return data->textData[name].at(row,column);
		}
		return QString();
	}

	qreal& ItemHandle::numericalData(const QString& name, int row, int column)
	{
		if (!data) data = new ItemData;

		if (!data->numericalData.contains(name))
		{
			data->numericalData[name] = DataTable<qreal>();
		}

		return data->numericalData[name].value(row,column);
	}

	qreal& ItemHandle::numericalData(const QString& name, const QString& row, const QString& column)
	{
		if (!data) data = new ItemData;

		if (!data->numericalData.contains(name))
		{
			data->numericalData[name] = DataTable<qreal>();
		}

		return data->numericalData[name].value(row,column);
	}

	QString& ItemHandle::textData(const QString& name, int row, int column)
	{
		if (!data) data = new ItemData;

		if (!data->textData.contains(name))
		{
			data->textData[name] = DataTable<QString>();
		}

		return data->textData[name].value(row,column);
	}

	QString& ItemHandle::textData(const QString& name, const QString& row, const QString& column)
	{
		if (!data) data = new ItemData;

		if (!data->textData.contains(name))
		{
			data->textData[name] = DataTable<QString>();
		}

		return data->textData[name].value(row,column);
	}

	DataTable<qreal>& ItemHandle::numericalDataTable(const QString& name)
	{
		if (!data) data = new ItemData;

		if (!data->numericalData.contains(name))
		{
			data->numericalData[name] = DataTable<qreal>();
		}

		return data->numericalData[name];
	}

	DataTable<QString>& ItemHandle::textDataTable(const QString& name)
	{
		if (!data) data = new ItemData;

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

	QList<ItemHandle*> ItemHandle::visibleChildren() const
	{
		QList<ItemHandle*> handles;
		for (int i=0; i < children.size(); ++i)
			if (children[i] && children[i]->visible)
				handles << children[i];

		for (int i=0; i < handles.size(); ++i)
		{
			if (handles[i] && handles[i]->visible)
			{
				for (int j=0; j < handles[i]->children.size(); ++j)
					if ( handles[i]->children[j] &&
						!handles.contains(handles[i]->children[j]) &&
						 handles[i]->children[j]->visible
						)
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

	NodeHandle::NodeHandle(const QString& s) : ItemHandle(s)
	{
		parent = 0;
		nodeFamily = 0;
		data = new ItemData();
		type = NodeHandle::TYPE;
	}

	NodeHandle::NodeHandle(NodeFamily * family)
	{
		parent = 0;
		nodeFamily = family;
		data = new ItemData();
		type = NodeHandle::TYPE;
	}

	NodeHandle::NodeHandle(NodeFamily * family, NodeGraphicsItem * item)
	{
		type = NodeHandle::TYPE;
		parent = 0;
		data = new ItemData();
		nodeFamily = family;
		if (item)
		{
			graphicsItems += item;
			item->setHandle(this);
		}
	}

	NodeHandle::NodeHandle(NodeFamily * family, NodeTextItem * item)
	{
		type = NodeHandle::TYPE;
		parent = 0;
		data = new ItemData();
		nodeFamily = family;
		if (item)
		{
			textItems += item;
			item->setHandle(this);
		}
	}


	bool NodeHandle::setFamily(NodeFamily * itemFamily)
	{
		if (!itemFamily) return false;
		this->nodeFamily = (itemFamily);
		return (this->nodeFamily != 0);
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
				node = NodeGraphicsItem::topLevelNodeItem(graphicsItems[i]);
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
		else
			if (textItems.size() > 0)
			{
				NodeTextItem * node;
				QList<ConnectionTextItem*> connections;
				for (int i=0; i < textItems.size(); ++i)
					if (textItems[i])
					{
						node = textItems[i]->asNode();
						if (node)
						{
							connections = node->connections;
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

	ConnectionHandle::ConnectionHandle(const QString& s) : ItemHandle(s)
	{
		type = ConnectionHandle::TYPE;
		parent = 0;
		connectionFamily = 0;
		data = new ItemData();
	}

	ConnectionHandle::ConnectionHandle(ConnectionFamily * family)
	{
		type = ConnectionHandle::TYPE;
		parent = 0;
		data = new ItemData();
		connectionFamily = family;
	}

	ConnectionHandle::ConnectionHandle(ConnectionFamily * family, ConnectionGraphicsItem * item)
	{
		type = ConnectionHandle::TYPE;
		parent = 0;
		data = new ItemData();
		connectionFamily = family;
		if (item)
		{
			graphicsItems += item;
			item->setHandle(this);
		}
	}

	ConnectionHandle::ConnectionHandle(ConnectionFamily * family, ConnectionTextItem * item)
	{
		type = ConnectionHandle::TYPE;
		parent = 0;
		data = new ItemData();
		connectionFamily = family;
		if (item)
		{
			textItems += item;
			item->setHandle(this);
		}
	}

	bool ConnectionHandle::setFamily(ConnectionFamily * itemFamily)
	{
		if (!itemFamily) return false;
		this->connectionFamily = (itemFamily);
		return (this->connectionFamily != 0);
	}

	ConnectionHandle::ConnectionHandle(const ConnectionHandle & copy) : ItemHandle(copy)
	{
		connectionFamily = copy.connectionFamily;
	}

	ConnectionHandle& ConnectionHandle::operator = (const ConnectionHandle& copy)
	{
	    ItemHandle::operator=(copy);
	    connectionFamily = copy.connectionFamily;
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

	QList<NodeHandle*> ConnectionHandle::nodes() const
	{
		QList<NodeHandle*> nodeslist;

		if (graphicsItems.size() > 0)
		{
			for (int j=0; j < graphicsItems.size(); ++j)
			{
				ConnectionGraphicsItem * connection;
				QList<NodeGraphicsItem*> nodes;
				connection = ConnectionGraphicsItem::topLevelConnectionItem(graphicsItems[j]);
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
			if (textItems.size() > 0)
			{
				ConnectionTextItem * connection;
				QList<NodeTextItem*> nodes;
				for (int i=0; i < textItems.size(); ++i)
					if (textItems[i])
					{
						connection = textItems[i]->asConnection();
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
				connection = ConnectionGraphicsItem::topLevelConnectionItem(graphicsItems[j]);
				if (connection)
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
			if (textItems.size() > 0)
			{
				ConnectionTextItem * connection;
				QList<NodeTextItem*> nodes;
				for (int i=0; i < textItems.size(); ++i)
					if (textItems[i])
					{
						connection = textItems[i]->asConnection();
						if (connection)
						{
							nodes = connection->nodesIn;
							for (int i=0; i < nodes.size(); ++i)
							{
								if (nodes[i] && nodes[i]->handle() &&
									nodes[i]->handle()->type == NodeHandle::TYPE)
									nodesList << static_cast<NodeHandle*>(nodes[i]->handle());
							}
						}
					}
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
				connection = ConnectionGraphicsItem::topLevelConnectionItem(graphicsItems[j]);
				if (connection)
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
			if (textItems.size() > 0)
			{
				ConnectionTextItem * connection;
				QList<NodeTextItem*> nodes;
				for (int i=0; i < textItems.size(); ++i)
					if (textItems[i])
					{
						connection = textItems[i]->asConnection();
						if (connection)
						{
							nodes = connection->nodesOut;
							for (int i=0; i < nodes.size(); ++i)
							{
								if (nodes[i] && nodes[i]->handle() &&
									nodes[i]->handle()->type == NodeHandle::TYPE)
									nodesList << static_cast<NodeHandle*>(nodes[i]->handle());
							}
						}
					}
			}
			return nodesList;
	}
}

