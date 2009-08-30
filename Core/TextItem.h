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

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

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
	class MY_EXPORT TextItem
	{
	public:
		TextItem();
		virtual ~TextItem();
		TextItem(ItemHandle *);
		/*! \brief clone */
		virtual TextItem* clone() const;
		/*! \brief used to identify the type of text item (Node or connection)*/
		int type;
		/*! \brief the handle for this text item*/
		ItemHandle * itemHandle;
		/*! \brief cast this text item to connection text item */
		ConnectionTextItem * asConnection();
		/*! \brief cast this text item to Node text item */
		NodeTextItem * asNode();
		/*! \brief other information needed to describe this connection in text*/
		QStringList descriptors;
		/*! \brief get handle for this item*/
		virtual ItemHandle * handle() const;
		/*! \brief set handle for this item*/
		virtual void setHandle(ItemHandle *);
	};

	/*! \brief Text items that represent Nodes.
	\ingroup Core
	*/
	class MY_EXPORT NodeTextItem : public TextItem
	{
	public:
		NodeTextItem();
		NodeTextItem(ItemHandle *);
		/*! \brief clone */
		virtual TextItem* clone() const;
		/*! \brief this variable is used to determine whether a TextItem is a NodeTextItem*/
		static int Type;
		/*! \brief all the connection that this Node is connected to*/
		QList<ConnectionTextItem*> connections;
	};

	/*! \brief Text items that represent connections, usually a single line on the TextEditor.
	\ingroup Core
	*/
	class MY_EXPORT ConnectionTextItem : public TextItem
	{
	public:
		ConnectionTextItem();
		ConnectionTextItem(ItemHandle *);
		/*! \brief clone */
		virtual TextItem* clone() const;
		/*! \brief this variable is used to determine whether a TextItem is a ConnectionTextItem*/
		static int Type;
		/*! \brief corresponds to nodesWithArrow() in ConnectionGraphicsItem*/
		QList<NodeTextItem*> nodesIn;
		/*! \brief corresponds to nodesWithoutArrow() in ConnectionGraphicsItem*/
		QList<NodeTextItem*> nodesOut;
		/*! \brief corresponds to nodesDisconnected() in ConnectionGraphicsItem*/
		QList<NodeTextItem*> nodesOther;
		/*! \brief corresponds to nodes() in ConnectionGraphicsItem*/
		QList<NodeTextItem*> nodes() const;
	};
}

#endif
