/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This file defines the TextItem, NodeTextItem, and ConnectionTextItem.
 There are two ways to define and view a network - graphical or text-based.
 Text items are used in the text-based version.

****************************************************************************/

#ifndef TINKERCELL_TEXTIEMS_H
#define TINKERCELL_TEXTIEMS_H

#include <QPair>
#include <QList>
#include <QHash>
#include <QUndoCommand>
#include <QGraphicsItem>

namespace Tinkercell
{

     class Tool;
     class ItemHandle;
     class NodeHandle;
     class ConnectionHandle;
     class ConnectionTextItem;
     class NodeTextItem;
     class OpTextItem;

     /*! \brief Text items represent strings in the text-based representation of a network.
          \ingroup Core
     */
     class TextItem
     {
     public:
          TextItem();
          TextItem(ItemHandle *);
          /*! \brief used to identify the type of text item (Node or connection)*/
          int type;
          /*! \brief the handle for this text item*/
          ItemHandle * itemHandle;
          /*! \brief cast this text item to connection text item */
          ConnectionTextItem * asConnection();
          /*! \brief cast this text item to Node text item */
          NodeTextItem * asNode();
          /*! \brief cast this text item to Operator text item */
          OpTextItem * asOp();
     };

     /*! \brief Text items that represent Nodes.
          \ingroup Core
     */
     class NodeTextItem : public TextItem
     {
     public:
          NodeTextItem();
          NodeTextItem(ItemHandle *);
          /*! \brief this variable is used to determine whether a TextItem is a NodeTextItem*/
          static int Type;
          /*! \brief all the connection that this Node is connected to*/
          QList<ConnectionTextItem*> connections;
     };

     /*! \brief Text items that represent connections, usually a single line on the TextEditor.
          \ingroup Core
     */
     class ConnectionTextItem : public TextItem
     {
     public:
          ConnectionTextItem();
          ConnectionTextItem(ItemHandle *);
          /*! \brief this variable is used to determine whether a TextItem is a ConnectionTextItem*/
          static int Type;
          /*! \brief the Nodes on the left-hand-side of this connection. Corresponds to NodesIn() in ConnectionGraphicsItem*/
          QList<NodeTextItem*> lhs;
          /*! \brief the Nodes on the right-hand-side of this connection. Corresponds to NodesOut() in ConnectionGraphicsItem*/
          QList<NodeTextItem*> rhs;
          /*! \brief the string that represents the connection between the lhs and rhs*/
          QString arrow;
          /*! \brief other information needed to describe this connection in text*/
          QStringList descriptors;
          /*! \brief the Nodes on the right-hand-side and left-hand-size of this connection. Corresponds to Nodes() in ConnectionGraphicsItem*/
          QList<NodeTextItem*> nodes() const;
     };

     /*! \brief Text items that represent operations, such as equations or assignments.
               These text items are defined by a left side, right side, and an operation
          \ingroup helper
     */
     class OpTextItem : public TextItem
     {
     public:
          OpTextItem();
          OpTextItem(ItemHandle *);
          /*! \brief this variable is used to determine whether a TextItem is a OpTextItem*/
          static int Type;
          /*! \brief the left-hand side*/
          QString lhs;
          /*! \brief the right-hand side*/
          QString rhs;
          /*! \brief the operator*/
          QString op;
          /*! \brief a short description of the operation, e.g. function declaration*/
          QString description;
     };
}

#endif
