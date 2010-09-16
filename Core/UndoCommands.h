/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file contains a collection of commands that perform simple operations that can be redone and undone.

****************************************************************************/

#ifndef TINKERCELL_BASICUNDOCOMMANDS_H
#define TINKERCELL_BASICUNDOCOMMANDS_H

#include <stdlib.h>
#include <QtGui>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QString>
#include <QFileDialog>
#include <QtDebug>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAction>
#include <QMenu>
#include <QFile>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QHash>
#include <QUndoCommand>
#include <QGraphicsItemAnimation>
#include <QPrinter>

#include "ItemFamily.h"
#include "ItemHandle.h"
#include "DataTable.h"
#include "NodeGraphicsItem.h"

#ifdef Q_WS_WIN
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{
	class NetworkHandle;
	class GraphicsScene;
	class TextEditor;
	class RenameCommand;
	class ConnectionGraphicsItem;

	/*! \brief this command inserts new handles to a NetworkHandle
	* \ingroup undo*/
	class TINKERCELLEXPORT InsertHandlesCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param NetworkHandle* window where items are inserted
		* \param QList<ItemHandle*> new items
		* \param bool check for uniqueness of names before inserting
		*/
		InsertHandlesCommand(TextEditor *, const QList<ItemHandle*> &, bool checkNames=true);
		/*! \brief constructor
		* \param NetworkHandle* window where items are inserted
		* \param ItemHandle* new item
		* \param bool check for uniqueness of names before inserting
		*/
		InsertHandlesCommand(TextEditor *, ItemHandle*, bool checkNames=true);
		/*! \brief destructor. deletes all text items and their handles (if not containing any graphics items)*/
		~InsertHandlesCommand();
		/*! \brief redo the change*/
		void redo();
		/*! \brief undo the change*/
		void undo();

	private:
		/*! \brief inserted handles*/
		QList<ItemHandle*> items;
		/*! \brief parent handles of the items that were inserted*/
		QList<ItemHandle*> parentHandles;
		/*! \brief TextEditor where the change happened*/
		TextEditor * textEditor;
		/*! \brief network where change happened*/
		NetworkHandle * network;
		/*! \brief Rename any duplicate names*/
		RenameCommand * renameCommand;
		/*! \brief check names before inserting*/
		bool checkNames;
	};

	/*! \brief this command inserts new handles to a NetworkHandle
	* \ingroup undo*/
	class TINKERCELLEXPORT RemoveHandlesCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param TextEditor* window where items are deleted
		* \param QList<ItemHandle*> deleted items
		* \param bool update data of other items where removed items might occur (default=true)
		*/
		RemoveHandlesCommand(TextEditor *, const QList<ItemHandle*> & , bool updateDataFields=true);
		/*! \brief constructor
		* \param TextEditor* window where items are deleted
		* \param ItemHandle* deleted item
		* \param bool update data of other items where removed items might occur (default=true)
		*/
		RemoveHandlesCommand(TextEditor *, ItemHandle*, bool updateDataFields=true);
		/*! \brief redo the change*/
		void redo();
		/*! \brief undo the change*/
		void undo();

	private:
		/*! \brief used to update information*/
		Change2DataCommand<qreal,QString> * changeDataCommand;
		/*! \brief removed handles*/
		QList<ItemHandle*> items;
		/*! \brief parent handles of the items that were removed*/
		QList<ItemHandle*> parentHandles;
		/*! \brief TextEditor where the change happened*/
		TextEditor * textEditor;
		/*! \brief network where change happened*/
		NetworkHandle * network;
		/*! \breif update data */
		bool updateData;
	};

	/*! \brief this command performs a move and allows redo/undo of that move
	* \ingroup undo*/
	class TINKERCELLEXPORT MoveCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param GraphicsScene* scene where change happened
		* \param QGraphicsItem * items that are affected
		* \param QPointF& amount to move
		*/
		MoveCommand(GraphicsScene * scene, QGraphicsItem * item, const QPointF& distance);
		/*! \brief constructor
		* \param scene where change happened
		* \param items that are affected
		* \param QPointF& amount to move
		*/
		MoveCommand(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QPointF& distance);
		/*! \brief constructor
		* \param GraphicsScene* scene where change happened
		* \param  QList<QGraphicsItem*>& items that are affected
		* \param QPointF& amount to move
		*/
		MoveCommand(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<QPointF>& distance);
		/*! \brief redo the change*/
		void redo();
		/*! \brief undo the change*/
		void undo();
		/*! \brief refresh all connectors that are attached to any of the items in the list
		* \param items list to check
		*/
		void static refreshAllConnectionIn(const QList<QGraphicsItem*>&);
	private:
		/*! \brief scene where the move happened*/
		GraphicsScene * graphicsScene;
		/*! \brief items that were moved*/
		QList<QGraphicsItem*> graphicsItems;
		/*! \brief amount by which each item was moved*/
		QList<QPointF> change;
	};
	/*! \brief this command performs an insert and allows redo/undo of that insert
	* \ingroup undo*/
	class TINKERCELLEXPORT InsertGraphicsCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* where change happened
		* \param QGraphicsItem* item that is inserted
		* \param bool check for uniqueness of names before inserting (default = true)
		*/
		InsertGraphicsCommand(const QString& name, GraphicsScene * scene, QGraphicsItem * item, bool checkNames=true);
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* where change happened
		* \param QList<QGraphicsItem*>& items that are inserted
		* \param bool check for uniqueness of names before inserting (default = true)
		*/
		InsertGraphicsCommand(const QString& name, GraphicsScene * scene, const QList<QGraphicsItem*>& items, bool checkNames=true);
		/*! \brief redo the change*/
		void redo();
		/*! \brief undo the change*/
		void undo();
		/*! \brief destructor*/
		virtual ~InsertGraphicsCommand();
	private:
		/*! \brief scene where change happened*/
		GraphicsScene * graphicsScene;
		/*! \brief network where change happened*/
		NetworkHandle * network;
		/*! \brief items that were inserted*/
		QList<QGraphicsItem*> graphicsItems;
		/*! \brief parent items of the items that were inserted*/
		QList<QGraphicsItem*> parentGraphicsItems;
		/*! \brief item handles of the items that were inserted*/
		QList<ItemHandle*> handles;
		/*! \brief parent handles of the items that were inserted*/
		QList<ItemHandle*> parentHandles;
		/*! \brief Rename any duplicate names*/
		RenameCommand * renameCommand;
		/*! \brief check names before inserting*/
		bool checkNames;
	};
	/*! \brief this command performs an removal and allows redo/undo of that removal
	* \ingroup undo*/
	class TINKERCELLEXPORT RemoveGraphicsCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* where change happened
		* \param QGraphicsItem* item that is removed
		* \param bool update data of other items where removed items might occur (default=true)
		*/
		RemoveGraphicsCommand(const QString& name, QGraphicsItem * item, bool updataDataFields=true);
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* where change happened
		* \param QList<QGraphicsItem*>& items that are removed
		* \param bool update data of other items where removed items might occur (default=true)
		*/
		RemoveGraphicsCommand(const QString& name, const QList<QGraphicsItem*>& items, bool updateDataFields=true);
		/*! \brief redo the change*/
		void redo();
		/*! \brief undo the change*/
		void undo();
	private:
		/*! \brief used to update information*/
		Change2DataCommand<qreal,QString> * changeDataCommand;
		/*! \brief scene where change happened*/
		QList<GraphicsScene*> graphicsScenes;
		/*! \brief items that were removed*/
		QList<QGraphicsItem*> graphicsItems;
		/*! \brief removed items' parents*/
		QList<QGraphicsItem*> itemParents;
		/*! \brief removed items' handles*/
		QList<ItemHandle*> itemHandles;
		/*! \brief parent handles of the items that were inserted*/
		QList<ItemHandle*> parentHandles;
		/*! \brief the set of data that are changed as a result of the removal*/
		QList< ItemHandle* > affectedHandles;
		/* \param bool update data of other items where removed items might occur (default=true)*/
		bool updateData;
	};


	/*! \brief this command changes the brush of an item
	* \ingroup undo*/
	class TINKERCELLEXPORT ChangeBrushCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* scene where change happened
		* \param QGraphicsItem* item that is affected
		* \param QBrush new brush
		*/
		ChangeBrushCommand(const QString& name, QGraphicsItem * item, const QBrush& to);
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* scene where change happened
		* \param QList<QGraphicsItem*>& items that are affected
		* \param QList<QBrush>& new brushes (one for each item)
		*/
		ChangeBrushCommand(const QString& name, const QList<QGraphicsItem*>& items, const QList<QBrush>& to);
		void redo();
		void undo();
	private:
		QList<QGraphicsItem*> graphicsItems;
		QList<QBrush> oldBrush, newBrush;
	};

	/*! \brief this command changes the pen of an item
	* \ingroup undo*/
	class TINKERCELLEXPORT ChangePenCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* scene where change happened
		* \param QGraphicsItem* item that is affected
		* \param QBrush new pen
		*/
		ChangePenCommand(const QString& name, QGraphicsItem * item, const QPen& to);
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* scene where change happened
		* \param QList<QGraphicsItem*>& items that are affected
		* \param QList<QPen>& new pens (one for each item)
		*/
		ChangePenCommand(const QString& name, const QList<QGraphicsItem*>& items, const QList<QPen>& to);
		void redo();
		void undo();
	private:
		QList<QGraphicsItem*> graphicsItems;
		QList<QPen> oldPen, newPen;
	};

	/*! \brief this command changes the pen and/or brush of an item
	* \ingroup undo*/
	class TINKERCELLEXPORT ChangeBrushAndPenCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* scene where change happened
		* \param QGraphicsItem* item that is affected
		* \param QBrush new brushes (one for each item)
		* \param QPen new pens (one for each item)
		*/
		ChangeBrushAndPenCommand(const QString& name, QGraphicsItem * item, const QBrush& brush, const QPen& pen);
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* scene where change happened
		* \param QList<QGraphicsItem*>& items that are affected
		* \param QList<QBrush>& new brushes (one for each item)
		* \param QList<QPen>& new pens (one for each item)
		*/
		ChangeBrushAndPenCommand(const QString& name, const QList<QGraphicsItem*>& items, const QList<QBrush>& brushes, const QList<QPen>& pens);
		~ChangeBrushAndPenCommand();
		void redo();
		void undo();
	private:
		ChangeBrushCommand * changeBrushCommand;
		ChangePenCommand * changePenCommand;
	};

	/*! \brief this command changes the pen of an item
	* \ingroup undo*/
	class TINKERCELLEXPORT ChangeZCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* scene where change happened
		* \param QGraphicsItem* item that is affected
		* \param double new Z value
		*/
		ChangeZCommand(const QString& name, QGraphicsScene * scene, QGraphicsItem * item, qreal to);
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* scene where change happened
		* \param QList<QGraphicsItem*>& item that is affected
		* \param QList<qreal>& new Z (one for each item)
		*/
		ChangeZCommand(const QString& name, QGraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<qreal>& to);
		void redo();
		void undo();
	private:
		QGraphicsScene * graphicsScene;
		QList<QGraphicsItem*> graphicsItems;
		QList<qreal> oldZ, newZ;
	};

	/*! \brief this command changes the size, angle, and orientation of an item
	* \ingroup undo*/
	class TINKERCELLEXPORT TransformCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* scene where change happened
		* \param QGraphicsItem* item that is affected
		* \param QPointF change in size (w,h)
		* \param double angle change
		* \param boolean flip vertically
		* \param boolean flip horizontally
		*/
		TransformCommand(const QString& name, QGraphicsScene * scene, QGraphicsItem * item,
			const QPointF& sizechange,
			qreal anglechange,
			bool VFlip, bool HFlip);
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* scene where change happened
		* \param QList<QGraphicsItem *>& items that are affected
		* \param QList<QPointF>&  change in size (w,h)
		* \param QList<qreal>& angle change
		* \param boolean flip vertically (all items)
		* \param boolean flip horizontally (all items)
		*/
		TransformCommand(const QString& name, QGraphicsScene * scene, const QList<QGraphicsItem *>& items,
			const QList<QPointF>& sizechange,
			const QList<qreal>& anglechange,
			bool VFlip, bool HFlip);
		void redo();
		void undo();
	private:
		QGraphicsScene * graphicsScene;
		QList<QGraphicsItem*> graphicsItems;
		QList<QPointF> sizeFactor;
		QList<qreal> angleChange;
		bool vFlip, hFlip;
	};

	/*! \brief this command changes the parent of a graphics item (not handles)
	* \ingroup undo*/
	class TINKERCELLEXPORT ChangeParentCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* scene where change happened
		* \param QGraphicsItem* item that is affected
		* \param QGraphicsItem* new parent item
		*/
		ChangeParentCommand(const QString& name, QGraphicsScene * scene, QGraphicsItem * item, QGraphicsItem * newParent);
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* scene where change happened
		* \param QList<QGraphicsItem *>& items that are affected
		* \param QList<QGraphicsItem *>& new parent items
		*/
		ChangeParentCommand(const QString& name, QGraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<QGraphicsItem*>& newParents);
		void redo();
		void undo();
	private:
		QList<QGraphicsItem*> graphicsItems;
		QList<QGraphicsItem*> oldParents;
		QList<QGraphicsItem*> newParents;
		QGraphicsScene * scene;
	};

	/*! \brief this command changes the name of the handle of an item. important: use full name of the items!
	* \ingroup undo*/
	class TINKERCELLEXPORT RenameCommand : public QUndoCommand
	{
	public:
		virtual ~RenameCommand();
		/*! \brief constructor
		* \param QString name of command
		* \param NetworkHandle * network
		* \param QList affected items
		* \param QString old name
		* \param QString new name
		*/
		RenameCommand(const QString& name, NetworkHandle *, const QList<ItemHandle*>& allItems, const QString& oldname, const QString& newname);
		/*! \brief constructor
		* \param QString name of command
		* \param NetworkHandle * network
		* \param QString old name
		* \param QString new name
		*/
		RenameCommand(const QString& name, NetworkHandle *, const QString& oldname, const QString& newname);
		/*! \brief constructor
		* \param QString name of command
		* \param NetworkHandle * network
		* \param QList affected items
		* \param QString old name
		* \param QString new name
		*/
		RenameCommand(const QString& name, NetworkHandle *,  const QList<ItemHandle*>& allItems, const QList<QString>& oldname, const QList<QString>& newname);
				/*! \brief constructor
		* \param QString name of command
		* \param NetworkHandle * network
		* \param QString old name
		* \param QString new name
		*/
		RenameCommand(const QString& name, NetworkHandle *,  const QList<QString>& oldname, const QList<QString>& newname);
		/*! \brief constructor
		* \param QString name of command
		* \param NetworkHandle * network
		* \param ItemHandle* target item handle
		* \param QString new name
		*/
		RenameCommand(const QString& name, NetworkHandle * , ItemHandle * itemHandle, const QString& newname);
		/*! \brief constructor
		* \param QString name of command
		* \param NetworkHandle * network
		* \param QList<ItemHandle*>& all the items to modify if they contain the new name
		* \param ItemHandle* target item
		* \param QString new name
		*/
		RenameCommand(const QString& name, NetworkHandle *,  const QList<ItemHandle*>& allItems, ItemHandle * item, const QString& newname);
		/*! \brief constructor
		* \param QString name of command
		* \param NetworkHandle * network
		* \param QList<ItemHandle*>& target items
		* \param QList<QString> new names (one for each item)
		*/
		RenameCommand(const QString& name, NetworkHandle * , const QList<ItemHandle*>& itemhandles, const QList<QString>& newnames);
		/*! \brief constructor
		* \param QString name of command
		* \param NetworkHandle * network
		* \param QList<ItemHandle*>& all the items to modify if they contain the new name
		* \param QList<ItemHandle*>& target items
		* \param QList<QString> new names (one for each item)
		*/
		RenameCommand(const QString& name, NetworkHandle *, const QList<ItemHandle*>& allItems, const QList<ItemHandle*>& itemhandles, const QList<QString>& newnames);
		void redo();
		void undo();
		static void findReplaceAllHandleData(const QList<ItemHandle*>& allItems,const QString& oldName,const QString& newName);
		static void substituteString(QString& targetValue, const QString& oldName,const QString& newName);
	private:
		QList<ItemHandle*> allhandles;
		QList<ItemHandle*> handles;
		QList<QString> oldNames;
		QList<QString> newNames;
		QList< QPair<ItemHandle *, QString> > newItemNames;
		QList< QPair<ItemHandle *, QString> > oldItemNames;
		QList< QPair<TextGraphicsItem *, QString> > newTextItemsNames;
		QList< QPair<TextGraphicsItem *, QString> > oldTextItemsNames;
		Change2DataCommand<qreal,QString> * changeDataCommand;
		NetworkHandle * network;
	};

	/*! \brief this command can be used to combine multiple commands into one command
	* \ingroup undo*/
	class TINKERCELLEXPORT CompositeCommand : public QUndoCommand
	{
	public:
		/*! \brief Constructor. Composite command takes ownership of these commands unless specified otherwise.
		* \param QString name of command
		* \param QList<QUndoCommand*>& the commands that make up this composite command
		* \param QList<QUndoCommand*>& the commands that should not be deleted by composite command's destructor (default = none)
		*/
		CompositeCommand(const QString&, const QList<QUndoCommand*>&, const QList<QUndoCommand*>& noClear = QList<QUndoCommand*>());
		/*! \brief constructor for grouping two commands. Composite command takes ownership of these commands unless specified otherwise.
		* \param QString name of command
		* \param QUndoCommand* a command to be gouped
		* \param QUndoCommand* another command to be gouped
		* \param bool delete both commands automatically (default = true)
		*/
		CompositeCommand(const QString&, QUndoCommand*, QUndoCommand*, bool deleteCommands = true);
		/*! \brief destructor automatically deletes any command not in the doNotDelete list*/
		~CompositeCommand();
		/*! \brief undo*/
		void redo();
		/*! \brief undo*/
		void undo();
		/*! \brief commands grouped inside this composite command*/
		QList<QUndoCommand*> commands;
		/*! \brief commands that should not be deleted along with the composite command*/
		QList<QUndoCommand*> doNotDelete;
	};

	/*! \brief this command can be used to invert another undo command (i.e. flip the redo/undo)
	* \ingroup undo*/
	class TINKERCELLEXPORT ReverseUndoCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param QString name of command
		* \param QList<QUndoCommand*>& the command to invert
		* \param bool whether or not to delete the inverted command (true = DO delete)
		*/
		ReverseUndoCommand(const QString&, QUndoCommand*, bool deleteCommand = true);
		~ReverseUndoCommand();
		void redo();
		void undo();
		QUndoCommand* command;
		bool deleteCommand;
	};

	/*! \brief this command can be used to replace the graphical representation of a node from an xml file
	* \ingroup undo*/
	class TINKERCELLEXPORT ReplaceNodeGraphicsCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param QString name of command
		* \param NodeGraphicsItem* the target node
		* \param QString xml file name
		* \param bool whether or not to transform the new graphics item to the original item's angle and size
		*/
		ReplaceNodeGraphicsCommand(const QString&,NodeGraphicsItem*,const QString&,bool transform=true);
		/*! \brief constructor
		* \param QString name of command
		* \param QList<NodeGraphicsItem*> the target nodes
		* \param QStringList xml file names
		* \param bool whether or not to transform the new graphics item to the original item's angle and size
		*/
		ReplaceNodeGraphicsCommand(const QString&,const QList<NodeGraphicsItem*>&,const QList<QString>&,bool transform=true);
		void undo();
		void redo();
		~ReplaceNodeGraphicsCommand();
	private:
		QList<NodeGraphicsItem*> targetNodes;
		QList<NodeGraphicsItem> oldNodes, newNodes;
		QList< QGraphicsItem* > itemsToDelete;
		void loadFromFile(NodeGraphicsItem*,const QString&);
		bool transform;
	};

	/*! \brief this command assigns handles to items
	* \ingroup undo*/
	class TINKERCELLEXPORT AssignHandleCommand : public QUndoCommand
	{
	public:
		AssignHandleCommand(const QString& text, QGraphicsItem* item, ItemHandle* handle);
		AssignHandleCommand(const QString& text, const QList<QGraphicsItem*>& items, ItemHandle* handle);
		AssignHandleCommand(const QString& text, const QList<QGraphicsItem*>& items, QList<ItemHandle*>& handles);
		void redo();
		void undo();
		~AssignHandleCommand();
		QList<QGraphicsItem*> graphicsItems;
		QList<ItemHandle*> oldHandles;
		QList<ItemHandle*> newHandles;
	private:
		QList< QPair< QGraphicsItem*, ItemHandle*> > oldItemHandles;
		QList< QPair< QGraphicsItem*, ItemHandle*> > newItemHandles;
	};

	/*! \brief this command places all the graphics items inside one handle into the other
	* \ingroup undo*/
	class TINKERCELLEXPORT MergeHandlesCommand : public QUndoCommand
	{
	public:
		MergeHandlesCommand(const QString& text, NetworkHandle * , const QList<ItemHandle*>& handles);
		void redo();
		void undo();
		~MergeHandlesCommand();
		QList<ItemHandle*> oldHandles;
		ItemHandle* newHandle;
	private:
		QHash< ItemHandle*, QList<QGraphicsItem*> > oldGraphicsItems;
		QHash< ItemHandle*, QList<ItemHandle*> > oldChildren;
		QHash< ItemHandle*, ItemHandle* > oldParents;
		QList< ItemHandle* > allChildren;
		QList<QGraphicsItem*> allGraphicsItems;
		RenameCommand * renameCommand;
	};

	/*! \brief this command assigns parent(s) to one or more handles
	* \ingroup undo
	*/
	class TINKERCELLEXPORT SetParentHandleCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor*/
		SetParentHandleCommand(const QString& name, NetworkHandle * , ItemHandle * child, ItemHandle * parent);
		/*! \brief constructor*/
		SetParentHandleCommand(const QString& name, NetworkHandle * , const QList<ItemHandle*>& children, ItemHandle * parent);
		/*! \brief constructor*/
		SetParentHandleCommand(const QString& name, NetworkHandle * , const QList<ItemHandle*>& children, const QList<ItemHandle*>& parents);
		/*! \brief destructor*/
		~SetParentHandleCommand();
		/*! \brief redo parent change*/
		void redo();
		/*! \brief undo parent change*/
		void undo();
	private:
		/*! \brief changed children handles*/
		QList<ItemHandle*> children;
		/*! \brief assigned parent handles*/
		QList<ItemHandle*> newParents;
		/*! \brief changed parent handles*/
		QList<ItemHandle*> oldParents;
		NetworkHandle * net;
		RenameCommand * renameCommand;
		
		friend class NetworkHandle;
	};
	
	/*! \brief this command is used to hide graphics items. 
		Hidden graphics items will be part (unless their handles are also hidden) of the network but not visible on the screen.
	* \ingroup undo
	*/
	class TINKERCELLEXPORT SetGraphicsSceneVisibilityCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor*/
		SetGraphicsSceneVisibilityCommand(const QString& name, const QList<QGraphicsItem*>&, const QList<bool>&);
		/*! \brief constructor*/
		SetGraphicsSceneVisibilityCommand(const QString& name, QGraphicsItem*, bool);
		/*! \brief constructor*/
		SetGraphicsSceneVisibilityCommand(const QString& name, const QList<QGraphicsItem*>&, bool);
		/*! \brief redo parent change*/
		void redo();
		/*! \brief undo parent change*/
		void undo();
	private:
		QList<QGraphicsItem*> items;
		QList<bool> before;
	};
	
	/*! \brief this command is used to hide graphics items. 
		Hidden graphics items will be part (unless their handles are also hidden) of the network but not visible on the screen.
	* \ingroup undo
	*/
	class TINKERCELLEXPORT SetHandleFamilyCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor*/
		SetHandleFamilyCommand(const QString& name, const QList<ItemHandle*>&, const QList<ItemFamily*>&);
		/*! \brief constructor*/
		SetHandleFamilyCommand(const QString& name, ItemHandle*, ItemFamily*);
		/*! \brief redo parent change*/
		void redo();
		/*! \brief undo parent change*/
		void undo();
	private:
		QList<ItemHandle*> handles;
		QList<ItemFamily*> oldFamily, newFamily;
		
		friend class NetworkHandle;
	};

}

#endif
