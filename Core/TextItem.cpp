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
     int NodeTextItem::Type = 1;
     int ConnectionTextItem::Type = 2;

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
		setHandle(this,0);
		
		if (h->graphicsItems.isEmpty() && h->textItems.isEmpty())
			delete h;
	 }

     NodeTextItem::NodeTextItem(): TextItem() { type = NodeTextItem::Type; }
     NodeTextItem::NodeTextItem(ItemHandle * h): TextItem(h) { type = NodeTextItem::Type; }

     ConnectionTextItem::ConnectionTextItem(): TextItem() { type = ConnectionTextItem::Type; }
     ConnectionTextItem::ConnectionTextItem(ItemHandle * h): TextItem(h) { type = ConnectionTextItem::Type; }

     ConnectionTextItem * TextItem::asConnection()
     {
          if (type == ConnectionTextItem::Type)
               return static_cast<ConnectionTextItem*>( const_cast<TextItem*>(this) );
          return 0;
     }

     NodeTextItem * TextItem::asNode()
     {
          if (type == NodeTextItem::Type)
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
