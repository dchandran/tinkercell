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
     int OpTextItem::Type = 3;

     TextItem::TextItem(): itemHandle(0) {}
     TextItem::TextItem(ItemHandle * h): itemHandle(h)
     {
          if (h)
          {
               h->textItems += this;
          }
     }

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

     OpTextItem * TextItem::asOp()
     {
          if (type == OpTextItem::Type)
               return static_cast<OpTextItem*>( const_cast<TextItem*>(this) );
          return 0;
     }

     QList<NodeTextItem*> ConnectionTextItem::nodes() const
     {
          QList<NodeTextItem*> nodes = lhs;
          for (int i=0; i < rhs.size(); ++i)
               if (rhs[i] && !nodes.contains(rhs[i]))
                    nodes << rhs[i];
          return nodes;
     }
}
