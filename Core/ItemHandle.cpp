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
		return s2;
	}

	ItemHandle * getHandle(QGraphicsItem * item)
	{
		if (item == 0) return (0);
		item = getGraphicsItem(item);
		
		NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(item);
		if (node)
		{
			return (node->itemHandle);
		}
		else
		{
			ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(item);
			if (connection)
			{
				return (connection->itemHandle);
			}
			else
			{
				TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(item);
				if (textItem)
				{
					return (textItem->itemHandle);
				}
			}
		}
		return (0);
	}

	void setHandle(QGraphicsItem * item, ItemHandle * handle)
	{
		if (item == 0) return;
		item = getGraphicsItem(item);
		
		if (handle != 0 && !handle->graphicsItems.contains(item))
		{
			handle->graphicsItems += item;
		}
		NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(item);
		if (node)
		{
			if (node->itemHandle)
			{
				if (node->itemHandle != handle)
				{
					node->itemHandle->graphicsItems.removeAll(node);
					node->itemHandle = handle;
				}
			}
			else
			{
				node->itemHandle = handle;
			}
		}
		else
		{
			ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(item);
			if (connection)
			{
				if (connection->itemHandle)
				{
					if (connection->itemHandle != handle)
					{
						connection->itemHandle->graphicsItems.removeAll(connection);
						connection->itemHandle = handle;
					}
				}
				else
				{
					connection->itemHandle = handle;
				}
			}
			else
			{
				TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(item);
				if (textItem)
				{
					if (textItem->itemHandle)
					{
						if (textItem->itemHandle != handle)
						{
							textItem->itemHandle->graphicsItems.removeAll(textItem);
							textItem->itemHandle = handle;
						}
					}
					else
					{
						textItem->itemHandle = handle;
					}
				}
			}
		}
	}

	ItemHandle * getHandle(TextItem * item)
	{
		if (!item) return 0;
		return item->itemHandle;
	}

	void setHandle(TextItem* item, ItemHandle* handle)
	{
		if (!item) return;
		if (!handle)
		{
			if (item->itemHandle)
			{
				item->itemHandle->textItems.removeAll(item);
			}
		}
		else
		{
			if (!handle->textItems.contains(item))
				handle->textItems.append(item);
		}

		item->itemHandle = handle;
	}

	/**********************************
	ITEM HANDLE
	**********************************/

	ItemHandle::~ItemHandle()
	{	
		parent = 0;
		if (data) 
		{
			delete data;
			data = 0;
		}
		
		tools.clear();

		QList<QGraphicsItem*> list = graphicsItems;
		for (int i=0; i < list.size(); ++i)
		{
			setHandle(list[i],0);
		}
		
		graphicsItems.clear();
		
		QList<TextItem*> list2 = textItems;
		for (int i=0; i < list2.size(); ++i)
		{
			setHandle(list2[i],0);
			delete list2[i];
		}
		
		textItems.clear();

		if (!children.isEmpty())
			for (int i=0; i < children.size(); ++i)
			{
				if (children[i] && children[i]->parent == this)
					delete children[i];
			}
	}
	ItemHandle::ItemHandle() : QObject()
	{ 
		parent = 0; 
		data = 0; 
		name = tr(""); 
		type = 0;
	}
	
	ItemHandle::ItemHandle(const ItemHandle& copy) : QObject()
	{
		type = copy.type;
		name = copy.name;
		graphicsItems += copy.graphicsItems;
		tools += copy.tools;
		if (copy.data)
			data = new ItemData(*(copy.data));
		else
			data = 0;
		parent = copy.parent;
	}
	
	/*! \brief operator = */
	ItemHandle& ItemHandle::operator = (const ItemHandle& copy)
	{
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
		parent = copy.parent;
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
			name = p->name + sep + name;
			p = p->parent;
		}
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
		if (q && q->isA(name)) 
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
	
	qreal ItemHandle::getNumericalData(const QString& name, int row, int column) const
	{
		if (data && data->numericalData.contains(name))
		{
			data->numericalData[name].at(row,column);
		}
		return 0.0;
	}
	
	qreal ItemHandle::getNumericalData(const QString& name, const QString& row, const QString& column) const
	{
		if (data && data->numericalData.contains(name))
		{
			data->numericalData[name].at(row,column);
		}
		return 0.0;
	}
	
	QString ItemHandle::getTextData(const QString& name, int row, int column) const
	{
		if (data && data->textData.contains(name))
		{
			data->textData[name].at(row,column);
		}
		return QString();
	}
	
	QString ItemHandle::getTextData(const QString& name, const QString& row, const QString& column) const
	{
		if (data && data->textData.contains(name))
		{
			data->textData[name].at(row,column);
		}
		return QString();
	}
	
	void ItemHandle::setNumericalData(const QString& name, int row, int column, qreal value)
	{
		if (data && data->numericalData.contains(name))
		{
			data->numericalData[name].value(row,column) = value;
		}
	}
	
	void ItemHandle::setNumericalData(const QString& name, const QString& row, const QString& column, qreal value)
	{
		if (data && data->numericalData.contains(name))
		{
			data->numericalData[name].value(row,column) = value;
		}
	}
	
	void ItemHandle::setTextData(const QString& name, int row, int column, const QString& value)
	{
		if (data && data->textData.contains(name))
		{
			data->textData[name].value(row,column) = value;
		}
	}
	
	void ItemHandle::setTextData(const QString& name, const QString& row, const QString& column, const QString& value)
	{
		if (data && data->textData.contains(name))
		{
			data->textData[name].value(row,column) = value;
		}
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

	int NodeHandle::Type = 1;

        NodeHandle * NodeHandle::asNode(ItemHandle * item)
        {
            if (item && item->type == NodeHandle::Type)
                return static_cast<NodeHandle*>(item);
            return 0;
        }

	NodeHandle::NodeHandle()
	{
		parent = 0;
		nodeFamily = 0;
		data = new ItemData();
		type = NodeHandle::Type;
	}

	NodeHandle::NodeHandle(NodeFamily * family)
	{	
		parent = 0;
		nodeFamily = family;
		data = new ItemData();
		type = NodeHandle::Type;
	}
		
	NodeHandle::NodeHandle(NodeFamily * family, NodeGraphicsItem * item)
	{	
		type = NodeHandle::Type;
		parent = 0;
		data = new ItemData();
		nodeFamily = family;
		if (item)
		{
			item->itemHandle = this;
			graphicsItems += item;
		}
	}

	NodeHandle::NodeHandle(NodeFamily * family, NodeTextItem * item)
	{	
		type = NodeHandle::Type;
		parent = 0;
		data = new ItemData();
		nodeFamily = family;
		if (item)
		{
			item->itemHandle = this;
			textItems += item;
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
						if (connections[j] && connections[j]->itemHandle && 
							connections[j]->itemHandle->type == ConnectionHandle::Type)
							list << static_cast<ConnectionHandle*>(connections[j]->itemHandle);
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
							if (connections[j] && connections[j]->itemHandle && 
								connections[j]->itemHandle->type == ConnectionHandle::Type)
								list << static_cast<ConnectionHandle*>(connections[j]->itemHandle);
					}
				}
		}
		
		return list;
	}

	/************************************
	CONNECTION HANDLE
	*************************************/

	int ConnectionHandle::Type = 2;

        ConnectionHandle * ConnectionHandle::asConnection(ItemHandle * item)
        {
            if (item && item->type == ConnectionHandle::Type)
                return static_cast<ConnectionHandle*>(item);
            return 0;
        }

	ConnectionHandle::ConnectionHandle()
	{	
		type = ConnectionHandle::Type;
		parent = 0;
		connectionFamily = 0;
		data = new ItemData();
	}
		
	ConnectionHandle::ConnectionHandle(ConnectionFamily * family)
	{	
		type = ConnectionHandle::Type;
		parent = 0;
		data = new ItemData();
		connectionFamily = family;
	}
		
	ConnectionHandle::ConnectionHandle(ConnectionFamily * family, ConnectionGraphicsItem * item)
	{	
		type = ConnectionHandle::Type;
		parent = 0;
		data = new ItemData();
		connectionFamily = family;
		if (item)
		{
			item->itemHandle = this;
			graphicsItems += item;
		}
	}
	
	ConnectionHandle::ConnectionHandle(ConnectionFamily * family, ConnectionTextItem * item)
	{	
		type = ConnectionHandle::Type;
		parent = 0;
		data = new ItemData();
		connectionFamily = family;
		if (item)
		{
			item->itemHandle = this;
			textItems += item;
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
						if (nodes[i] && nodes[i]->itemHandle && 
							nodes[i]->itemHandle->type == NodeHandle::Type)
							nodeslist << static_cast<NodeHandle*>(nodes[i]->itemHandle);
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
							if (nodes[i] && nodes[i]->itemHandle && 
								nodes[i]->itemHandle->type == NodeHandle::Type)
								nodeslist << static_cast<NodeHandle*>(nodes[i]->itemHandle);
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
						if (nodesOut[i] && nodesOut[i]->itemHandle)
						{
							ok = true;
							break;
						}
					}
					
					if (!ok) continue;
					
					for (int i=0; i < nodesIn.size(); ++i)
					{
						if (nodesIn[i] && !nodesDisconnected.contains(nodesIn[i]) && 
							nodesIn[i]->itemHandle && 
							nodesIn[i]->itemHandle->type == NodeHandle::Type)
							nodesList << static_cast<NodeHandle*>(nodesIn[i]->itemHandle);
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
							if (nodes[i] && nodes[i]->itemHandle && 
								nodes[i]->itemHandle->type == NodeHandle::Type)
								nodesList << static_cast<NodeHandle*>(nodes[i]->itemHandle);
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
					for (int i=0; i < nodesOut.size(); ++i)
					{
						if (nodesOut[i] && nodesOut[i]->itemHandle)
						{
							ok = true;
							break;
						}
					}
					
					if (!ok) continue;
					
					for (int i=0; i < nodesOut.size(); ++i)
					{
						if (nodesOut[i] && !nodesDisconnected.contains(nodesOut[i]) && 
							nodesOut[i]->itemHandle &&
							nodesOut[i]->itemHandle->type == NodeHandle::Type)
							nodesList << static_cast<NodeHandle*>(nodesOut[i]->itemHandle);
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
							if (nodes[i] && nodes[i]->itemHandle && 
								nodes[i]->itemHandle->type == NodeHandle::Type)
								nodesList << static_cast<NodeHandle*>(nodes[i]->itemHandle);
						}
					}
				}
		}
		return nodesList;
	}

}
