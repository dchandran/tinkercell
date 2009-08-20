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

#include "DataTable.h"
#include "ItemFamily.h"
#include "ItemHandle.h"
#include "DataTable.h"
#include "NodeGraphicsItem.h"

namespace Tinkercell
{
	class NetworkWindow;
	class GraphicsScene;
	class TextEditor;

	/*! \brief this command inserts new handles to a NetworkWindow
	* \ingroup undo*/
	class InsertItemsCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param NetworkWindow* window where items are inserted
		* \param QList<ItemHandle*> new items
		*/
		InsertItemsCommand(TextEditor *, const QList<TextItem*> & );
		/*! \brief constructor
		* \param NetworkWindow* window where items are inserted
		* \param ItemHandle* new item
		*/
		InsertItemsCommand(TextEditor *, TextItem*);
		/*! \brief redo the change*/
		void redo();
		/*! \brief undo the change*/
		void undo();

	private:
		/*! \brief inserted handles*/
		QList<TextItem*> items;
		/*! \brief TextEditor where the change happened*/
		TextEditor * textEditor;
	};

	/*! \brief this command inserts new handles to a NetworkWindow
	* \ingroup undo*/
	class RemoveItemsCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param NetworkWindow* window where items are inserted
		* \param QList<ItemHandle*> deleted items
		*/
		RemoveItemsCommand(TextEditor *, const QList<TextItem*> & );
		/*! \brief constructor
		* \param NetworkWindow* window where items are inserted
		* \param ItemHandle* deleted item
		*/
		RemoveItemsCommand(TextEditor *, TextItem*);
		/*! \brief redo the change*/
		void redo();
		/*! \brief undo the change*/
		void undo();

	private:
		/*! \brief inserted handles*/
		QList<TextItem*> items;
		/*! \brief TextEditor where the change happened*/
		TextEditor * textEditor;
	};

	/*! \brief this command performs a move and allows redo/undo of that move
	* \ingroup undo*/
	class MoveCommand : public QUndoCommand
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
	class InsertGraphicsCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* where change happened
		* \param QGraphicsItem* item that is inserted
		*/
		InsertGraphicsCommand(const QString& name, GraphicsScene * scene, QGraphicsItem * item);
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* where change happened
		* \param QList<QGraphicsItem*>& items that are inserted
		*/
		InsertGraphicsCommand(const QString& name, GraphicsScene * scene, const QList<QGraphicsItem*>& items);
		/*! \brief redo the change*/
		void redo();
		/*! \brief undo the change*/
		void undo();
		/*! \brief destructor*/
		virtual ~InsertGraphicsCommand();
	private:
		/*! \brief scene where change happened*/
		GraphicsScene * graphicsScene;
		/*! \brief items that were inserted*/
		QList<QGraphicsItem*> graphicsItems;
		/*! \brief item handles of the items that were inserted*/
		QList<ItemHandle*> handles;
	};
	/*! \brief this command performs an removal and allows redo/undo of that removal
	* \ingroup undo*/
	class RemoveGraphicsCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* where change happened
		* \param QGraphicsItem* item that is removed
		*/
		RemoveGraphicsCommand(const QString& name, GraphicsScene * scene, QGraphicsItem * item);
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* where change happened
		* \param QList<QGraphicsItem*>& items that are removed
		*/
		RemoveGraphicsCommand(const QString& name, GraphicsScene * scene, const QList<QGraphicsItem*>& items);
		/*! \brief redo the change*/
		void redo();
		/*! \brief undo the change*/
		void undo();
		/*! \brief destructor*/
		~RemoveGraphicsCommand();
	private:
		/*! \brief scene where change happened*/
		GraphicsScene * graphicsScene;
		/*! \brief items that were removed*/
		QList<QGraphicsItem*> graphicsItems;
		/*! \brief removed items' parents*/
		QList<QGraphicsItem*> itemParents;
		/*! \brief removed items' handles*/
		QList<ItemHandle*> itemHandles;
		/*! \brief the set of data that are changed as a result of the removal*/
		QList< ItemHandle* > affectedHandles;
		/*! \brief the original set of data that were changed as a result of the removal*/
		QList< ItemData > oldData;
		/*! \brief the new changed set of data that were changed as a result of the removal*/
		QList< ItemData > newData;
	};


	/*! \brief this command changes the brush of an item
	* \ingroup undo*/
	class ChangeBrushCommand : public QUndoCommand
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
	class ChangePenCommand : public QUndoCommand
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
	class ChangeBrushAndPenCommand : public QUndoCommand
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
		void redo();
		void undo();
	private:
		QList<QGraphicsItem*> graphicsItems;
		QList<QPen> oldPen, newPen;
		QList<QBrush> oldBrush, newBrush;
	};

	/*! \brief this command changes the pen of an item
	* \ingroup undo*/
	class ChangeZCommand : public QUndoCommand
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
	class TransformCommand : public QUndoCommand
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
	class ChangeParentCommand : public QUndoCommand
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
	class RenameCommand : public QUndoCommand
	{
	public:
		virtual ~RenameCommand();
		/*! \brief constructor
		* \param QString name of command
		* \param QList affected items
		* \param QString old name
		* \param QString new name
		*/
		RenameCommand(const QString& name, const QList<ItemHandle*>& allItems, const QString& oldname, const QString& newname);
		/*! \brief constructor
		* \param QString name of command
		* \param QList affected items
		* \param QString old name
		* \param QString new name
		*/
		RenameCommand(const QString& name, const QList<ItemHandle*>& allItems, const QList<QString>& oldname, const QList<QString>& newname);
		/*! \brief constructor
		* \param QString name of command
		* \param GraphicsScene* scene where change happened
		* \param ItemHandle* target item handle
		* \param QString new name
		*/
		RenameCommand(const QString& name, NetworkWindow * win, ItemHandle * itemHandle, const QString& newname);
		/*! \brief constructor
		* \param QString name of command
		* \param QList<QGraphicsItem*>& all the items to modify if they contain the new name
		* \param ItemHandle* target item
		* \param QString new name
		*/
		RenameCommand(const QString& name, const QList<ItemHandle*>& allItems, ItemHandle * item, const QString& newname);
		/*! \brief constructor
		* \param QString name of command
		* \param NetworkWindow* where change happened
		* \param QList<ItemHandle*>& target items
		* \param QList<QString> new names (one for each item)
		*/
		RenameCommand(const QString& name, NetworkWindow * win, const QList<ItemHandle*>& itemhandles, const QList<QString>& newnames);
		/*! \brief constructor
		* \param QString name of command
		* \param QList<ItemHandle*>& all the items to modify if they contain the new name
		* \param QList<ItemHandle*>& target items
		* \param QList<QString> new names (one for each item)
		*/
		RenameCommand(const QString& name, const QList<ItemHandle*>& allItems, const QList<ItemHandle*>& itemhandles, const QList<QString>& newnames);
		void redo();
		void undo();
		static void findReplaceAllHandleData(const QList<ItemHandle*>& allItems,const QString& oldName,const QString& newName);
		static void substituteString(QString& targetString, const QString& oldName,const QString& newName);
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

		static QString assignUniqueName(const QString&,const QStringList&);
	};

	/*! \brief this command can be used to combine multiple commands into one command
	* \ingroup undo*/
	class CompositeCommand : public QUndoCommand
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
	class ReverseUndoCommand : public QUndoCommand
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
	class ReplaceNodeGraphicsCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor
		* \param QString name of command
		* \param NodeGraphicsItem* the target node
		* \param QString xml file name
		*/
		ReplaceNodeGraphicsCommand(const QString&,NodeGraphicsItem*,const QString&);
		/*! \brief constructor
		* \param QString name of command
		* \param QList<NodeGraphicsItem*> the target nodes
		* \param QStringList xml file names
		*/
		ReplaceNodeGraphicsCommand(const QString&,const QList<NodeGraphicsItem*>&,const QList<QString>&);
		void undo();
		void redo();
		~ReplaceNodeGraphicsCommand();
	private:
		QList<NodeGraphicsItem*> targetNodes;
		QList<NodeGraphicsItem> oldNodes, newNodes;
		QList< QGraphicsItem* > itemsToDelete;
		//QList< QVector<NodeGraphicsItem::Shape*> > oldShapes, newShapes;
		//QList< QVector<NodeGraphicsItem::ControlPoint*> > 	oldBoundaryControlPoints, oldControlPoints,
		//													newBoundaryControlPoints, newControlPoints;
		void loadFromFile(NodeGraphicsItem*,const QString&);
	};

	/*! \brief this command assigns handles to items
	* \ingroup undo*/
	class AssignHandleCommand : public QUndoCommand
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
	class MergeHandlersCommand : public QUndoCommand
	{
	public:
		MergeHandlersCommand(const QString& text, NetworkWindow * win, const QList<ItemHandle*>& handles);
		void redo();
		void undo();
		~MergeHandlersCommand();
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
	class SetParentHandleCommand : public QUndoCommand
	{
	public:
		/*! \brief constructor*/
		SetParentHandleCommand(const QString& name, NetworkWindow * currentWindow, ItemHandle * child, ItemHandle * parent);
		/*! \brief constructor*/
		SetParentHandleCommand(const QString& name, NetworkWindow * currentWindow, const QList<ItemHandle*>& children, ItemHandle * parent);
		/*! \brief constructor*/
		SetParentHandleCommand(const QString& name, NetworkWindow * currentWindow, const QList<ItemHandle*>& children, const QList<ItemHandle*>& parents);
		/*! \brief redo parent change*/
		void redo();
		/*! \brief undo parent change*/
		void undo();
		/*! \brief changed children handles*/
		QList<ItemHandle*> children;
		/*! \brief assigned parent handles*/
		QList<ItemHandle*> newParents;
		/*! \brief changed parent handles*/
		QList<ItemHandle*> oldParents;
	private:
		NetworkWindow * net;
	};

}

#endif
