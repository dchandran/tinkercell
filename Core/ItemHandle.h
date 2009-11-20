/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This is one of the main classes in Tinkercell
This file defines the ItemHandle, NodeHandle, and ConnectionHandle classes.
Each item in Tinkercell has a graphics item for drawing and a handle. The 
handle stores data information and family information about the item that is displayed. 

A handle can also have multiple items associate with it. This just means that multiple
graphics are used to draw a single item.

****************************************************************************/

#ifndef TINKERCELL_HANDLER_H
#define TINKERCELL_HANDLER_H

#include <QList>
#include <QTreeWidgetItem>
#include <QUndoCommand>
#include <QGraphicsItem>

#include "DataTable.h"
#include "ItemFamily.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{
	class GraphicsScene;
	class Tool;
	class TextGraphicsItem;
	class ItemHandle;
	class NodeHandle;
	class ConnectionHandle;
	class NodeGraphicsItem;
	class ConnectionGraphicsItem;
	class TextItem;
	class NodeTextItem;
	class ConnectionTextItem;

	/*! \brief This function replaces disallowed characters in a name string
	* \ingroup helper
	* \param QString original string
	*/
	MY_EXPORT QString RemoveDisallowedCharactersFromName(const QString&);

	/*! \brief 
	* This class is used to store information about nodes or connections.
	* It contains a hashtable of data tables, which is used by different tools to store
	* specific data.
	* \ingroup helper
	*/
	class MY_EXPORT ItemData
	{
	public:
		/*! \brief hash table that stores the numerical data for each tool*/
		QHash<QString,DataTable<qreal> > numericalData;
		/*! \brief hash table that stores the text data for each tool*/
		QHash<QString,DataTable<QString> > textData;
	};

	/*! \brief 
	The ItemHandle represents a complete object in the network, whether it is a node or a 
	connection. The ItemHandle contains the name of the object and pointers to all the 
	QGraphicsItems and TextItems that are used to represent the object. Tools associated with
	the object can be stored within the ItemHandle as well. The ItemHandle can also 
	optionally contain an ItemFamily, which can be used to distinguish different types of nodes or
	connections, if needed. Each ItemHandle can contain one parent. Several functions are 
	available for convinently getting the parents and children of an ItemHandle. 
	
	Use setHandle and getHandle functions to get and set the handles for 
	QGraphicsItems or TextItems. Use h->data->numericalData[string] or h->data->textData[string] to
	get the DataTable with the particular name. Alternatively, h->numericalData(string) or h->textData(string)
	can be used to access the data conviniently. 
	
	The SymbolsTable is used to store all the handles in a network. 
	Setting visible=false will prevent the SymbolsTable from loading that ItemHandle.
	\ingroup core
	*/
	class MY_EXPORT ItemHandle: public QObject
	{
		Q_OBJECT

	public:
		/*! \brief name of this item*/
		QString name;
		/*! \brief list of graphical items used to draw this handle*/
		QList<QGraphicsItem*> graphicsItems;
		/*! \brief list of graphical items used to draw this handle*/
		QList<TextItem*> textItems;
		/*! \brief list of tools associated with this handle*/
		QList<Tool*> tools;
		/*! \brief the data (from each tool) for this handle
		\sa ItemData*/
		ItemData* data;
		/*! \brief this handles immediate parent (main parent if there are more than one)*/
		ItemHandle * parent;
		/*! \brief child handles that have this handle as a parent*/
		QList<ItemHandle*> children;
		/*! \brief this property must be true in order for this handle to appear in the symbols table*/
		bool visible;
		/*! \brief type of this handle (sub-classes can specify type)*/
		int type;

		/*! \brief default constructor
			\param QString name*/
		ItemHandle(const QString& name = QString());
		/*! \brief copy constructor */
		ItemHandle(const ItemHandle&);
		/*! \brief operator = */
		virtual ItemHandle& operator = (const ItemHandle&);
		/*! \brief destructor -- does nothing*/
		virtual ~ItemHandle();
		/*! \brief clone the data and lists*/
		virtual ItemHandle * clone() const;
		/*! \brief family that this items belongs in. Used for characterizing the nodes and connections.*/
		virtual ItemFamily* family() const;
		/*! \brief determines whether this handle belongs to the speicific family.
		\param QString the family*/
		virtual bool isA(const ItemFamily* family) const;
		/*! \brief determines whether this handle belongs to the speicific family.
		\param QString the family name*/
		virtual bool isA(const QString& family) const;
		/*! \brief The full name includes all the parent names appended using a dot
		\param QString replace the dot with some other separator */
		virtual QString fullName(const QString& sep = QString(".")) const;
		/*! \brief Set the parent for this handle
		\param ItemHandle* parent handle */
		virtual void setParent(ItemHandle * parent);
		/*! \brief get the top-level handle such that it is of the specified family. If no family is specified, then gets the top-level handle
		\param ItemHandle* the family name */
		virtual ItemHandle* root(const QString& family=QString("")) const;
		/*! \brief get the bottom-most parent handle such that it is of the specified family. If no family is specified, then gets the top-level handle
		\param ItemHandle* the family name */
		virtual ItemHandle* parentOfFamily(const QString& family) const;
		/*! \brief checks if an item is the parent or parent's parent, or parent's parent's parent, etc. Note: self->isChildOf(self) is false
		\param ItemHandle* parent handle 
		\return Boolean is child*/
		virtual bool isChildOf(ItemHandle * handle) const;
		/*! \brief gets the graphics items belonging to this handle and all child handes 
		\return QList<QGraphicsItem*> list of graphics items*/
		virtual QList<QGraphicsItem*> allGraphicsItems() const;
		/*! \brief gets the all child handles and their child handles
		\return QList<ItemHandle*> list of handles*/
		virtual QList<ItemHandle*> allChildren() const;
		/*! \brief gets the all child handles and their child handles such that each handle's visible=true
		\return QList<ItemHandle*> list of handles*/
		virtual QList<ItemHandle*> visibleChildren() const;
		/*! \brief does this handle have a numerical data table with this name?
		\param QString name of tool, e.g. "Numerical Attributes"
		\return bool true = has a numerical table by this name. false = does not have a numerical table by this name*/
		virtual bool hasNumericalData(const QString& name) const;
		/*! \brief does this handle have a text data table with this name?
		\param QString name of tool, e.g. "Text Attributes"
		\return bool true = has a text table by this name. false = does not have a text table by this name*/
		virtual bool hasTextData(const QString& name) const;
		/*! \brief gets a numerical attribute with the given name, row, column
		\param QString name of tool, e.g. "Numerical Attributes"
		\param int row in data table
		\param int column in data table
		\return double value*/
		virtual qreal numericalData(const QString& name, int row=0, int column=0) const;
		/*! \brief gets a numerical attribute with the given name, row, column
		\param QString name of tool, e.g. "Numerical Attributes"
		\param QString row name in data table
		\param QString column name data table
		\return double value*/
		virtual qreal numericalData(const QString& name, const QString& row, const QString& column) const;
		/*! \brief gets a text attribute with the given name, row, column
		\param QString name of tool, e.g. "Text Attributes"
		\param int row in data table
		\param int column in data table
		\return QString value*/
		virtual QString textData(const QString& name, int row=0, int column=0) const;
		/*! \brief gets a text attribute with the given name, row, column
		\param QString name of tool, e.g. "Text Attributes"
		\param QString row name in data table
		\param QString column name data table
		\return QString value*/
		virtual QString textData(const QString& name, const QString& row, const QString& column) const;
		/*! \brief gets a reference to the numerical attribute with the given name, row, column
		\param QString name of tool, e.g. "Numerical Attributes"
		\param int row in data table
		\param int column in data table
		\return double reference value*/
		virtual qreal& numericalData(const QString& name, int row=0, int column=0);
		/*! \brief gets a reference to the numerical attribute with the given name, row, column
		\param QString name of tool, e.g. "Numerical Attributes"
		\param QString row name in data table
		\param QString column name data table
		\return double reference value*/
		virtual qreal& numericalData(const QString& name, const QString& row, const QString& column);
		/*! \brief gets a reference to the text attribute with the given name, row, column
		\param QString name of tool, e.g. "Text Attributes"
		\param int row in data table
		\param int column in data table
		\return QString reference value*/
		virtual QString& textData(const QString& name, int row=0, int column=0);
		/*! \brief gets a reference to the text attribute with the given name, row, column
		\param QString name of tool, e.g. "Text Attributes"
		\param QString row name in data table
		\param QString column name data table
		\return QString& reference value*/
		virtual QString& textData(const QString& name, const QString& row, const QString& column);
		/*! \brief gets reference to a numerical table with the given name. Makes the table if needed
		\param QString name of tool, e.g. "Numerical Attributes"
		\return DataTable<double>& reference of table*/
		virtual DataTable<qreal>& numericalDataTable(const QString& name);
		/*! \brief gets reference to a text table with the given name. Makes the table if needed
		\param QString name of tool, e.g. "Numerical Attributes"
		\return DataTable<QString>& reference of table*/
		virtual DataTable<QString>& textDataTable(const QString& name);
	};

	/*! \brief 
	* The handles are used to bring together data and graphics items.  
	* Node Handle contains pointers to all the graphics items that belong to it, the tools
	* that apply to this item, the data for this item, and the family that it belongs with
	* \ingroup core
	*/
	class MY_EXPORT NodeHandle : public ItemHandle
	{
		Q_OBJECT

	public:
		/*! \brief this number is used to identify when a handle is a node handle*/
		MY_EXPORT static const int TYPE = 1;
		/*! \brief funcion that returns all the connections from all the nodes in this handle
		\return QList<ConnectionHandle*> list of connection handles
		*/
		virtual QList<ConnectionHandle*> connections() const;
		/*! \brief node family for this node handle*/
		NodeFamily* nodeFamily;
		/*! \brief default constructor -- initialize everything*/
		NodeHandle(const QString& name = QString());
		/*! \brief constructor with initial family
		\param NodeFamily* family for this handle*/
		NodeHandle(NodeFamily * nodeFamily);	
		/*! \brief copy constructor -- copies all the data (deep). graphic items are shallow copies*/
		NodeHandle(const NodeHandle & copy);
		/*! \brief constructor using initial family and graphics item*/
		NodeHandle(NodeFamily * nodeFamily, NodeGraphicsItem * item);
		/*! \brief constructor using initial family and text item*/
		NodeHandle(NodeFamily * nodeFamily, NodeTextItem * item);
		/*! \brief return a clone of this handle
		\return ItemFamily* node handle as item handle*/
		virtual ItemHandle * clone() const;
		/*! \brief get the node family for this handle
		\return ItemFamily* node family as item family*/
		virtual ItemFamily* family() const;
		/*! \brief set the node family for this handle
		\param NodeFamily* node family*/
		virtual bool setFamily(NodeFamily *);
		/*! \brief checks if the item handle is a node handle and casts it as a node item.
		Returns 0 if it is not a node item
		\param ItemHandle* item*/
		MY_EXPORT static NodeHandle* asNode(ItemHandle *);
	};

	/*! \brief 
	* The handles are used to bring together data and graphics items.  
	* Connection Handle contains pointers to all the graphics items that belong to it, the tools
	* that apply to this item, the data for this item, the family that it belongs with, and pointers
	* to nodes connected (in and out)
	* \ingroup core
	*/
	class MY_EXPORT ConnectionHandle : public ItemHandle
	{
	public:
		/*! \brief this number is used to identify when an item handle is a connection handle*/
		MY_EXPORT static const int TYPE = 2;
		/*! \brief returns all the nodes connected to all the connectors in this handle
		\return QList<NodeHandle*> list of node handles*/
		virtual QList<NodeHandle*> nodes() const;
		/*! \brief 
		returns all the nodes that are on the "input" side of this connection. 
		This is determined by looking at which nodes have an arrow-head associated with them in graphics items
		or by looking at the lhs and rhs lists in text itesm
		\return QList<NodeHandle*> list of node handles*/
		virtual QList<NodeHandle*> nodesIn() const;
		/*! \brief 
		returns all the nodes that are on the "output" side of this connection. 
		This is determined by looking at which nodes have an arrow-head associated with them in graphics items
		or by looking at the lhs and rhs lists in text itesm
		\return QList<NodeHandle*> list of node handles*/
		virtual QList<NodeHandle*> nodesOut() const;
		/*! \brief the family for this connection handle*/
		ConnectionFamily* connectionFamily;
		/*! \brief default constructor -- initializes everything*/
		ConnectionHandle(const QString& name = QString());
		/*! \brief one parameter constructor -- initializes everything
		\param ConnectionFamily* connection family*/
		ConnectionHandle(ConnectionFamily * family);
		/*! \brief copy constructor -- deep copy of data, but shallow copy of graphics items*/
		ConnectionHandle(const ConnectionHandle&);
		/*! \brief two parameter constructor
		\param ConnectionFamily* initial family
		\param ConnectionGraphicsItem* connection graphics item*/
		ConnectionHandle(ConnectionFamily * family, ConnectionGraphicsItem * item);
		/*! \brief two parameter constructor
		\param ConnectionFamily* initial family
		\param ConnectionGraphicsItem* connection text item*/
		ConnectionHandle(ConnectionFamily * family, ConnectionTextItem * item);
		/*! \brief set the family for this handle
		\param ConnectionFamily* connection family*/
		virtual bool setFamily(ConnectionFamily * family);
		/*! \brief clone of this handle
		\return ItemFamily* connection handle as item handle*/
		virtual ItemHandle * clone() const;
		/*! \brief family for this handle
		\return ItemFamily* connection family as item family*/
		virtual ItemFamily* family() const;
		/*! \brief checks if the item handle is a node handle and casts it as a node item.
		Returns 0 if it is not a node item
		\param ItemHandle* item*/
		MY_EXPORT static ConnectionHandle* asConnection(ItemHandle *);
	};

	/*! \brief get the handle from a graphics item
	* \param QGraphicsItem* graphics item
	* \ingroup core
	* \return ItemHandle* item handle (0 if none)
	*/
	MY_EXPORT ItemHandle * getHandle(QGraphicsItem*);
	/*! \brief set the handle of a graphics item (use 0 to remove handle)
	* \param QGraphicsItem* graphics item
	* \param ItemHandle* handle (use 0 to remove handle)
	* \ingroup core
	*/
	MY_EXPORT void setHandle(QGraphicsItem*, ItemHandle*);
	/*! \brief get the handle from a text item
	* \param TextItem* text item
	* \ingroup core
	* \return ItemHandle* item handle (0 if none)
	*/
	MY_EXPORT ItemHandle * getHandle(TextItem*);
	/*! \brief set the handle of a text item (use 0 to remove handle)
	* \param TextItem* text item
	* \param ItemHandle* handle (use 0 to remove handle)
	* \ingroup core
	*/
	MY_EXPORT void setHandle(TextItem*, ItemHandle*);
}

#endif
