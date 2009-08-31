/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines the TextItem, NodeTextItem, and ConnectionTextItem.
There are two ways to define and view a network - graphical or text-based.
Text items are used in the text-based version.  

****************************************************************************/

#include "ItemHandle.h"
#include "TextItem.h"

namespace Tinkercell
{
	int NodeTextItem::TYPE = 1;
	int ConnectionTextItem::TYPE = 2;
	
	ItemHandle * TextItem::handle() const
	{
		return itemHandle;
	}
	
	void TextItem::setHandle(ItemHandle * handle)
	{
		if (!handle)
		{
			if (itemHandle)
			{
				itemHandle->textItems.removeAll(this);
			}
		}
		else
		{
			if (!handle->textItems.contains(this))
				handle->textItems.append(this);
		}

		itemHandle = handle;
	}

	TextItem::TextItem(): type(0), itemHandle(0) {}

	TextItem::TextItem(ItemHandle * h): type(0), itemHandle(h)
	{
		if (h)
		{
			h->textItems += this;
		}
	}

	TextItem::~TextItem()
	{
		if (!itemHandle) return;

		ItemHandle * h = itemHandle;
		setHandle(0);

		if (h->graphicsItems.isEmpty() && h->textItems.isEmpty())
			delete h;
	}

	NodeTextItem::NodeTextItem(): TextItem() { type = NodeTextItem::TYPE; }
	NodeTextItem::NodeTextItem(ItemHandle * h): TextItem(h) { type = NodeTextItem::TYPE; }

	ConnectionTextItem::ConnectionTextItem(): TextItem() { type = ConnectionTextItem::TYPE; }
	ConnectionTextItem::ConnectionTextItem(ItemHandle * h): TextItem(h) { type = ConnectionTextItem::TYPE; }

	ConnectionTextItem * TextItem::asConnection()
	{
		if (type == ConnectionTextItem::TYPE)
			return static_cast<ConnectionTextItem*>( const_cast<TextItem*>(this) );
		return 0;
	}

	NodeTextItem * TextItem::asNode()
	{
		if (type == NodeTextItem::TYPE)
			return static_cast<NodeTextItem*>( const_cast<TextItem*>(this) );
		return 0;
	}

	QList<NodeTextItem*> ConnectionTextItem::nodes() const
	{
		QList<NodeTextItem*> nodes = nodesIn;
		for (int i=0; i < nodesOut.size(); ++i)
			if (nodesOut[i] && !nodes.contains(nodesOut[i]))
				nodes << nodesOut[i];
		for (int i=0; i < nodesOther.size(); ++i)
			if (nodesOther[i] && !nodes.contains(nodesOut[i]))
				nodes << nodesOther[i];
		return nodes;
	}

	TextItem* TextItem::clone() const
	{
		return new TextItem(*this);
	}

	TextItem* NodeTextItem::clone() const
	{
		return new NodeTextItem(*this);
	}

	TextItem* ConnectionTextItem::clone() const
	{
		return new ConnectionTextItem(*this);
	}
}
