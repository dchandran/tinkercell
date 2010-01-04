/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This is one of the main classes in Tinkercell
This file defines the GraphicsScene class where all the drawing takes place.
In addition to drawing , the GraphicsScene provides serveral signals and functions
that is useful for plugins, eg. move, insert, delete, changeData, etc.

****************************************************************************/

#ifndef TINKERCELL_GRAPHICSSCENE_H
#define TINKERCELL_GRAPHICSSCENE_H

#include <stdlib.h>
#include <QtGui>
#include <QString>
#include <QPair>
#include <QFileDialog>
#include <QtDebug>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAction>
#include <QFile>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QHash>
#include <QUndoCommand>
#include <QGraphicsItemAnimation>
#include <QPrinter>

#include "DataTable.h"
#include "HistoryWindow.h"
#include "SymbolsTable.h"
#include "CloneItems.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	class NodeGraphicsItem;
	class ConnectionGraphicsItem;
	class ItemHandle;
	class ItemData;
	class NetworkWindow;
	class GraphicsView;
	class ConsoleWindow;
	
	/*! \brief The primary task of the graphics scene is to draws items.
	It also provides functions for conveniently moving, deleting, editing, changind data, etc.
	The graphics scene sends signals for key events, mouse events, save events, etc. It also provides access to the
	items currently selected and currently being moved so a plug-in can easily add items to the list
	of moving items or selected items.
	The default behaviors of the GraphicsScene (e.g selecting, moving) can be turned off by setting useDefaultBehavior=false. If
	the default behaviors are off, GraphicsScene still emits the mouse and keyboard signals -- this allows another tool
	to perform the actions.
	The historyStack pointer can be used to add undo commands to the scene.
	\ingroup core
	*/
	class MY_EXPORT GraphicsScene : public QGraphicsScene
	{
		Q_OBJECT

	public:
		/*! \brief each graphics scene has a default behavior, i.e. moving, selecing, deleting. Whether or not
			to use the default behavior is set using scene->useDefaultBehavior. This static variable is the
			default value for each scene's useDefaultBehavior variable, i.e. setting this to true will cause a
			newly constructed graphics scene to NOT use default behaviors.*/
		static bool USE_DEFAULT_BEHAVIOR;
		/*! \brief setting grid to a non-zero value forces node items to "fit" on the grid, where
			the gap between the grid lines is determined by this variable. The default is 0, i.e. no grid*/
		static int GRID;
		/*! \brief pen that is used to draw the selection rectangle*/
		static QPen SelectionRectanglePen;
		/*! \brief brush that is used to color the selection rectangle*/
		static QBrush SelectionRectangleBrush;
		/*! \brief brush used to draw the background for the scene*/
		static QBrush BackgroundBrush;
		/*! \brief pen used to draw the grid for the scene*/
		static QPen GridPen;
		/*! \brief brush used to draw the foreground for the scene*/
		static QBrush ForegroundBrush;
		/*! \brief the minimum distance that gets classified as a "drag". Anything less will be considered just a click.*/
		static qreal MIN_DRAG_DISTANCE;

		/*! \brief the containing network window*/
		NetworkWindow * networkWindow;
		/*! \brief the containing network window's symbols table*/
		SymbolsTable * symbolsTable;
		/*! \brief indicates whether this scene is free to perform actions*/
		bool useDefaultBehavior;
		/*! \brief a pointer to the NetworkWindow's history*/
		QUndoStack* historyStack;
		/*!
		* \brief the context menu that is shown during right-click event on selected graphical items.
		Plugins can add new actions to this menu.
		*/
		QMenu * contextItemsMenu;
		/*!
		* \brief the context menu that is shown during right-click event on the scene.
		Plugins can add new actions to this menu.
		*/
		QMenu * contextScreenMenu;
		/*! \brief Returns the currently visible window
		* \param void
		* \return QRectF rectangle*/
		virtual QRectF viewport();
		/*! \brief Returns the point where mouse was clicked last
		* \param void
		* \return QPointF& ref to last clicked point*/
		virtual QPointF& lastPoint();
		/*! \brief Returns the list of pointers to items that are currently selected
		* \param void
		* \return QList<QGraphicsItem*>& list of pointers to selected items*/
		virtual QList<QGraphicsItem*>& selected();
		/*! \brief Returns a rectangle that includes all the selected items
		* \param void
		* \return QRectF bounding rect for selected items*/
		virtual QRectF selectedRect();
		/*! \brief Returns the list of pointers to items that are currently being moved
		* \param void
		* \return QList<QGraphicsItem*>& list of pointers to moving items*/
		virtual QList<QGraphicsItem*>& moving();
		/*! \brief top Z value
		* \return double*/
		virtual qreal ZValue();
		/*! \brief get the console window (same as mainWindow->console())*/
		ConsoleWindow * console();
		/*! \brief Constructor: sets 10000x10000 scene */
		GraphicsScene(QWidget * parent = 0);
		/*! \brief destructor */
		virtual ~GraphicsScene();

	public:
		/*! \brief set the grid mode ON with the given grid size
		* \param double grid size (0 will disable grid)
		* \return void*/
		virtual void enableGrid(int sz=100);
		/*! \brief set the grid mode OFF, which is same as setting grid size to 0
		* \return void*/
		virtual void disableGrid();
		/*! \brief set the grid size. If > 0, grid will be enabled. If 0, grid will be disabled
		* \param double grid size (0 will disable grid)
		* \return void*/
		virtual void setGridSize(int sz=100);
		/*! \brief get the grid size being used (0 = no grid)
		* \return int*/
		virtual int gridSize() const;
		/*! \brief Add a new item to the scene (different from insert)
		* \sa insert
		* \param QGraphicsItem* Tinkercell object
		* \return void*/
		virtual void addItem(QGraphicsItem * item);
		/*! \brief place center at the point
		* \param QPointF point
		* \return void*/
		virtual void centerOn(const QPointF& point);
		/*! \brief adjusts view to include all items
		* \return void*/
		virtual void fitAll();
		/*! \brief adjusts view to include all selected items
		* \return void*/
		virtual void fitSelected();
		/*! \brief Clear all selection and moving items list
		* \return void*/
		virtual void clearSelection();
		/*! \brief send everything on the screen to a printer
		* \param QPaintDevice * printer
		* \param  QRectF region to print
		* \return void */
		virtual void print(QPaintDevice* printer,const QRectF& rect = QRectF());
		/*! \brief select one item (does not deselect other items)
		* \param QGraphicsItem* item to select
		* \return void*/
		virtual void select(QGraphicsItem* item);
		/*! \brief select items (does not deselect other items)
		* \param QList<QGraphicsItem*>& items to select
		* \return void*/
		virtual void select(const QList<QGraphicsItem*>& item);
		/*! \brief select all items*/
		virtual void selectAll();
		/*! \brief select items with the given text */
		virtual void find(const QString&);
		/*! \brief deselect one item
		* \param QGraphicsItem* item to deselect
		* \return void*/
		virtual void deselect(QGraphicsItem* item);
		/*! \brief deselect all selected items
		* \return void*/
		virtual void deselect();
		/*! \brief copy selected items*/
		virtual void copy();
		/*! \brief cut selected items*/
		virtual void cut();
		/*! \brief paste copied items*/
		virtual void paste();
		/*! \brief get all the handles in the current network*/
		virtual QList<ItemHandle*> allHandles() const;
		/*! \brief a simple move operation that also adds undo command to history window and emits associated signal(s)
		* \param QGraphicsItem * item to move
		* \param QPointF distance to move the item
		* \return void
		*/
		virtual void move(QGraphicsItem * item, const QPointF& distance);
		/*! \brief a simple move operation that also adds undo command to history window and emits associated signal(s)
		* \param QList<QGraphicsItem*>& items to move
		* \param QPointF distance to move the items (same for all items)
		* \return void
		*/
		virtual void move(const QList<QGraphicsItem*>& items, const QPointF& distance);
		/*! \brief a simple move operation that also adds undo command to history window and emits associated signal(s)
		* \param QList<QGraphicsItem*>& items to move
		* \param QList<QPointF>& distance to move the items specified for each item
		* \return void
		*/
		virtual void move(const QList<QGraphicsItem*>& items, const QList<QPointF>& distance);
		/*! \brief this command performs an insert and also adds undo command to history window and emits associated signal(s)
		* \param QString name of new item
		* \param QList<QPointF>& distance to move the items specified for each item
		* \return void
		*/
		virtual void insert(const QString& name, QGraphicsItem * item);
		/*! \brief this command performs an insert and also adds undo command to history window and emits associated signal(s)*/
		virtual void insert(const QString& name, const QList<QGraphicsItem*>& items);
		/*! \brief this command performs an removal and also adds undo command to history window and emits associated signal(s)*/
		virtual void remove(const QString& name, QGraphicsItem * item);
		/*! \brief this command performs an removal and also adds undo command to history window and emits associated signal(s)*/
		virtual void remove(const QString& name, const QList<QGraphicsItem*>& items);
		/*! \brief remove selected items*/
		virtual void removeSelected();
		/*! \brief this command changes the brush of an item*/
		virtual void setBrush(const QString& name, QGraphicsItem * item, const QBrush& to);
		/*! \brief this command changes the brush of an item and also adds undo command to history window and emits associated signal(s)*/
		virtual void setBrush(const QString& name, const QList<QGraphicsItem*>& items, const QList<QBrush>& to);
		/*! \brief this command changes the z value of an item and also adds undo command to history window and emits associated signal(s)*/
		virtual  void setZValue(const QString& name, QGraphicsItem * item, qreal to);
		/*! \brief this command changes the z value of an item and also adds undo command to history window and emits associated signal(s)*/
		virtual  void setZValue(const QString& name, const QList<QGraphicsItem*>& items, const QList<qreal>& to);
		/*! \brief this command changes the pen of an item and also adds undo command to history window and emits associated signal(s)*/
		virtual  void setPen(const QString& name, QGraphicsItem * item, const QPen& to);
		/*! \brief this command changes the pen of an item and also adds undo command to history window and emits associated signal(s)*/
		virtual  void setPen(const QString& name, const QList<QGraphicsItem*>& items, const QList<QPen>& to);
		/*! \brief this command changes the pen and/or brush of an item and also adds undo command to history window and emits associated signal(s)*/
		virtual  void setBrushAndPen(const QString& name, QGraphicsItem * item, const QBrush& brush, const QPen& pen);
		/*! \brief this command changes the pen and/or brush of an item and also adds undo command to history window and emits associated signal(s)*/
		virtual  void setBrushAndPen(const QString& name, const QList<QGraphicsItem*>& items, const QList<QBrush>& brushes, const QList<QPen>& pens);
		/*! \brief this command changes the size, angle, and orientation of an item and also adds undo command to history window and emits associated signal(s)*/
		virtual  void transform(const QString& name, QGraphicsItem * item,
			const QPointF& sizechange,
			qreal anglechange,
			bool VFlip, bool HFlip);
		/*! \brief this command changes the size, angle, and orientation of an item and also adds undo command to history window and emits associated signal(s)*/
		virtual void transform(const QString& name, const QList<QGraphicsItem *>& items,
			const QList<QPointF>& sizechange,
			const QList<qreal>& anglechange,
			bool VFlip, bool HFlip);
		/*! \brief this command changes the parent of an item and also adds undo command to history window and emits associated signal(s)*/
		virtual  void setParentItem(const QString& name, QGraphicsItem * item, QGraphicsItem * newParent);
		/*! \brief this command changes the parent of an item and also adds undo command to history window and emits associated signal(s)*/
		virtual  void setParentItem(const QString& name, const QList<QGraphicsItem*>& items, QGraphicsItem * newParent);
		/*! \brief this command changes the parent of an item and also adds undo command to history window and emits associated signal(s)*/
		virtual  void setParentItem(const QString& name, const QList<QGraphicsItem*>& items, const QList<QGraphicsItem*>& newParents);
		/*! \brief rename item and also adds undo command to history window and emits associated signal(s)*/
		virtual  void rename(const QString& oldname, const QString& new_name);
		/*! \brief rename an item and also adds undo command to history window and emits associated signal(s)*/
		virtual  void rename(QGraphicsItem * item, const QString& new_name);
		/*! \brief rename an item and also adds undo command to history window and emits associated signal(s)*/
		virtual  void rename(ItemHandle * item, const QString& new_name);
		/*! \brief rename items and also adds undo command to history window and emits associated signal(s)*/
		virtual  void rename(const QList<QGraphicsItem *>& items, const QList<QString>& new_names);
		/*! \brief places all the graphics items in the given list of handles under the new handle*/
		virtual  void mergeHandles(const QList<ItemHandle*>& handles);
		/*! \brief places all the graphics items under the new handle*/
		virtual  void assignHandles(const QList<QGraphicsItem*>& items, ItemHandle* newHandle);
		/*! \brief change parent handles and also adds undo command to history window and emits associated signal(s)*/
		virtual  void setParentHandle(const QList<ItemHandle*>& handles, const QList<ItemHandle*>& parentHandles);
		/*! \brief change parent handle and also adds undo command to history window and emits associated signal(s)*/
		virtual  void setParentHandle(ItemHandle * child, ItemHandle * parent);
		/*! \brief change parconst ent handles and also adds undo command to history window and emits associated signal(s)*/
		virtual  void setParentHandle(const QList<ItemHandle*> children, ItemHandle * parent);
		/*! \brief change numerical data table and also adds undo command to history window and emits associated signal(s)*/
		virtual  void changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const DataTable<qreal>* newdata);
		/*! \brief change a list of numerical data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual  void changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstring, const QList<DataTable<qreal>*>& newdata);
		/*! \brief change a list of numerical data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual  void changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<qreal>*>& newdata);
		/*! \brief change text data table and also adds undo command to history window and emits associated signal(s)*/
		virtual  void changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const DataTable<QString>* newdata);
		/*! \brief change a list of text data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual  void changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstring, const QList<DataTable<QString>*>& newdata);
		/*! \brief change a list of text data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual  void changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<QString>*>& newdata);
		/*! \brief change two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual  void changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const DataTable<qreal>* newdata1, const DataTable<QString>* newdata2);
		/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual  void changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstring, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& newdata2);
		/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual  void changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& newdata2);
		/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual  void changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<DataTable<qreal>*>& olddata1, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& olddata2, const QList<DataTable<QString>*>& newdata2);
		/*! \brief change a two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual  void changeData(const QString& name, const QList<ItemHandle*>& handles, DataTable<qreal>* olddata1, const DataTable<qreal>* newdata1, DataTable<QString>* olddata2, const DataTable<QString>* newdata2);
		/*! \brief change a data table and also adds undo command to history window and emits associated signal(s)*/
		virtual  void changeData(const QString& name, const QList<ItemHandle*>& handles, DataTable<qreal>* olddata1, const DataTable<qreal>* newdata1);
		/*! \brief change a data table and also adds undo command to history window and emits associated signal(s)*/
		virtual  void changeData(const QString& name, const QList<ItemHandle*>& handles, DataTable<QString>* olddata1, const DataTable<QString>* newdata1);
		/*! \brief show item that were hidden*/
		virtual  void showItems(const QString& name, QGraphicsItem* item);
		/*! \brief show items that were hidden*/
		virtual  void showItems(const QString& name, const QList<QGraphicsItem*>& items);
		/*! \brief hide item*/
		virtual  void hideItems(const QString& name, QGraphicsItem* item);
		/*! \brief hide items*/
		virtual  void hideItems(const QString& name, const QList<QGraphicsItem*>& items);
		/*! \brief show handle that was hidden*/
		virtual  void showItems(const QString& name, ItemHandle* handle);
		/*! \brief show handles that were hidden*/
		virtual  void showItems(const QString& name, const QList<ItemHandle*>& handles);
		/*! \brief hide handle*/
		virtual  void hideItems(const QString& name, ItemHandle* handle);
		/*! \brief hide handles*/
		virtual void hideItems(const QString& name, const QList<ItemHandle*>& handles);

	signals:
		/*! \brief signals just before items are copied
		* \param GraphicsScene * scene where the items are going to be copied
		* \param QList<QGraphicsItem*>& list of graphics items going to be copied
		* \param QList<ItemHandle*>& list of handles going to be copied (does NOT have to be the same number as items removed)
		* \return void*/
		virtual void copyItems(GraphicsScene * scene, QList<QGraphicsItem*>& , QList<ItemHandle*>& );
		/*! \brief signals just before items are deleted
		* \param GraphicsScene * scene where the items are going to be removed
		* \param QList<QGraphicsItem*>& list of graphics items going to be removed
		* \param QList<ItemHandle*>& list of handles going to be removed (does NOT have to be the same number as items removed)
		* \return void*/
		virtual void itemsAboutToBeRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& , QList<ItemHandle*>& );
		/*! \brief signals whenever items are deleted
		* \param GraphicsScene* scene where the items were removed
		* \param QList<QGraphicsItem*>& list of items removed
		* \param QList<ItemHandle*>& list of handles removed (does NOT have to be the same number as items removed)
		* \return void*/
		virtual void itemsRemoved(GraphicsScene * scene, const QList<QGraphicsItem*>& , const QList<ItemHandle*>& );
		/*! \brief signals whenever items are going to be added
		* \param GraphicsScene* scene where the items are added
		* \param QList<QGraphicsItem*>& list of new graphics items
		* \param QList<ItemHandle*>& list of new handles (does NOT have to be the same number as items)
		* \return void*/
		virtual void itemsAboutToBeInserted(GraphicsScene * scene, QList<QGraphicsItem*>& , QList<ItemHandle*>& );
		/*! \brief signals whenever items are added
		* \param GraphicsScene* scene where the items were added
		* \param QList<QGraphicsItem*>& list of new graphics items
		* \param QList<ItemHandle*>& list of new handles (does NOT have to be the same number as items)
		* \return void*/
		virtual void itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& , const QList<ItemHandle*>& );
		/*! \brief signals whenever items are selected (item can be sub-item, not top-level)
		* \param GraphicsScene* scene where items are selected
		* \param QList<QGraphicsItem*>& list of all selected item pointers
		* \param QPointF point where mouse is clicked
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		virtual void itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
		/*! \brief signals whenever an empty node of the screen is clicked
		* \param GraphicsScene* scene where the event took place
		* \param QPointF point where mouse is clicked
		* \param Qt::MouseButton which button was pressed
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		virtual void mousePressed(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		/*! \brief signals whenever an empty node of the screen is clicked
		* \param GraphicsScene* scene where the event took place
		* \param QPointF point where mouse is clicked
		* \param Qt::MouseButton which button was pressed
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		virtual void mouseReleased(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		/*! \brief emits event when mouse is double clicked
		* \param GraphicsScene* scene where the event took place
		* \param point where mouse is clicked
		* \param modifier keys being used when mouse clicked
		* \return void*/
		virtual void mouseDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		/*! \brief signals whenever mouse is dragged from one point to another
		* \param GraphicsScene* scene where the event took place
		* \param QPointF point where mouse is clicked first
		* \param QPointF point where mouse is released
		* \param Qt::MouseButton button being pressed
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		virtual void mouseDragged(GraphicsScene * scene, QPointF from, QPointF to, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		/*! \brief signals whenever items are being moved (each item is the top-most item)
		* \param GraphicsScene* scene where the items were moved
		* \param QList<QGraphicsItem*>& list of pointes to all moving items
		* \param QPointF point where the item was
		* \param QPointF point where the item is moved to
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		virtual void itemsMoved(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<QPointF>& distance, Qt::KeyboardModifiers modifiers);
		/*! \brief signals whenever mouse moves, and indicates whether it is on top of an item
		* \param GraphicsScene* scene where the event took place
		* \param QGraphicsItem* pointer to item that mouse is on top of
		* \param QPointF point where mouse is clicked
		* \param Qt::MouseButton button being pressed
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \param QList<QGraphicsItem*>& list of items that are being moved with the mouse
		* \return void*/
		virtual void mouseMoved(GraphicsScene * scene, QGraphicsItem* item, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers, QList<QGraphicsItem*>&);
		/*! \brief signals whenever mouse is on top of an item
		* \param GraphicsScene* scene where the event took place
		* \param QGraphicsItem* pointer to item that mouse is on top of
		* \param QPointF point where mouse is clicked
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \param QList<QGraphicsItem*>& list of items that are being moved with the mouse
		* \return void*/
		virtual void mouseOnTopOf(GraphicsScene * scene, QGraphicsItem* item, QPointF point, Qt::KeyboardModifiers modifiers, QList<QGraphicsItem*>&);
		/*! \brief signals whenever right click is made on an item or sceen
		* \param GraphicsScene* scene where the event took place
		* \param QGraphicsItem* pointer to item that mouse is clicked on
		* \param QPointF point where mouse is clicked
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		virtual void sceneRightClick(GraphicsScene * scene, QGraphicsItem* item, QPointF point, Qt::KeyboardModifiers modifiers);
		/*! \brief signals whenever a key is pressed
		* \param GraphicsScene* scene where the event took place
		* \param QKeyEvent * key that is pressed
		* \return void*/
		virtual void keyPressed(GraphicsScene * scene, QKeyEvent *);
		/*! \brief signals whenever a key is released
		* \param GraphicsScene* scene where the event took place
		* \param QKeyEvent * key that is released
		* \return void*/
		virtual void keyReleased(GraphicsScene * scene, QKeyEvent *);
		/*! \brief signals whenever color of items are changed
		* \param GraphicsScene* scene where the event took place
		* \param QList<QGraphicsItem*>& items that changed color
		* \return void*/
		virtual void colorChanged(GraphicsScene * scene, const QList<QGraphicsItem*>& items);
		/*! \brief signals whenever item parents are changed
		* \param GraphicsScene* scene where the event took place
		* \param QList<QGraphicsItem*>& items
		* \param QList<QGraphicsItem*>& new parents
		* \return void*/
		virtual void parentItemChanged(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<QGraphicsItem*>& parents);
		/*! \brief signals whenever the handles for graphics items have changed
		* \param GraphicsScene* scene where the event took place
		* \param QList<GraphicsItem*>& items that are affected
		* \param QList<ItemHandle*>& old handle for each items
		* \return void*/
		virtual void handlesChanged(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>& old);
		/*! \brief signals whenever the current activities need to be stopped
		* \param QWidget * the widget that send the signal
		* \return void*/
		virtual void escapeSignal(const QWidget * sender);
		/*! \brief signals whenever file(s) are dropped on the canvas
		* \param QList<QFileInfo>& the name(s) of the file(s)
		* \return void*/
		virtual void filesDropped(const QList<QFileInfo>& files);

	protected:
		/*! \brief grid size. If zero, then disabled*/
		int gridSz;
		/*! \brief topmost Z value*/
		qreal lastZ;
		/*! \brief rectanglular selection area*/
		QGraphicsRectItem selectionRect;
		/*! \brief used to store copied items*/
		static QList<QGraphicsItem*> duplicateItems;
		/*! \brief used to store copied items*/
		static GraphicsScene * copiedFromScene;
		/*! \brief clears copied items*/
		static void clearStaticItems();
		/*! \brief point where mouse is clicked*/
		QPointF clickedPoint;
		/*! \brief button that was used when mouse was clicked*/
		Qt::MouseButton clickedButton;
		/*! \brief mouse is being pressed*/
		bool mouseDown;
		/*! \brief list of pointers to selected items*/
		QList<QGraphicsItem*> selectedItems;
		/*! \brief list of pointers to moving items*/
		QList<QGraphicsItem*> movingItems;
		/*! \brief group of moving items*/
		QGraphicsItemGroup * movingItemsGroup;
		/*! \brief when mouse is pressed, the item at the position is added to selected list and moving list
		* \param QGraphicsSceneMouseEvent * mouse event
		* \return void*/
		virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
		/*! \brief when mouse is double clicked, the item at the position is added to selected list and moving list
		* \param QGraphicsSceneMouseEvent * mouse event
		* \return void*/
		virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * mouseEvent );
		/*! \brief when mouse is moving, all items in moving list are moved
		* \param QGraphicsSceneMouseEvent * mouse event
		* \return void*/
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
		/*! \brief when mouse is released, moving list is cleared
		* \param QGraphicsSceneMouseEvent * mouse event
		* \return void*/
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
		/*! \brief when key is pressed
		* \param QKeyEvent *  key event
		* \return void*/
		virtual void keyPressEvent (QKeyEvent * event);
		/*! \brief when key is released
		* \param QKeyEvent *  key event
		* \return void*/
		virtual void keyReleaseEvent (QKeyEvent * event);
		/*! \brief when mouse wheel is turned, zoom
		* \param QGraphicsSceneWheelEvent * mouse wheel event
		* \return void*/
		virtual void wheelEvent (QGraphicsSceneWheelEvent * wheelEvent);
		/*! \brief context menu for the scene
		* \param QGraphicsSceneContextMenuEvent * context menu event
		* \return void*/
		virtual void contextMenuEvent ( QGraphicsSceneContextMenuEvent * contextMenuEvent );
		/*! \brief drag and drop event
		* \param QGraphicsSceneDragDropEvent * drag and drop event
		* \return void*/
		virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
		/*! \brief drag and drop event
		* \param QGraphicsSceneDragDropEvent * drag and drop event
		* \return void*/
		virtual void dropEvent(QGraphicsSceneDragDropEvent *event);
		/*! \brief drag and drop event
		* \param QGraphicsSceneDragDropEvent * drag and drop event
		* \return void*/
		virtual void dragMoveEvent ( QGraphicsSceneDragDropEvent * event);
		/*! \brief draw background grid if in grid mode*/
		virtual void drawBackground( QPainter*, const QRectF &);
	public:
		/*! \brief snap the node item to the grid
		* \param NodeGraphicsItem*
		* \return void*/
		virtual void snapToGrid(QGraphicsItem*);
		/*! \brief zoom
		* Precondition: None
		* Postcondition: None
		* \param scale factor
		* \return void*/
		virtual void scaleView(qreal scaleFactor);

		friend class MainWindow;
		friend class NetworkWindow;
		friend class GraphicsView;
	};
}

#endif
