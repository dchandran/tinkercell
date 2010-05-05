/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

A class that is used to store a network. The network is a collection of Item Handles. 
The history stack is also a key component of a network.

A class that is used to store a network. The network is a collection of Item Handles. 
The history stack is also a key component of a network.
The network can either be represented as text using TextEditor or visualized with graphical items in the
GraphicsScene. Each node and connection are contained in a handle, and each handle can either be represented as text or as graphics.
The two main components of NetworkWindow are the SymbolsTable and HistoryStack
This class provides functions for inserting items, removing items, and changing information inside the model.

****************************************************************************/

#ifndef TINKERCELL_CORENETWORKHANDLE_H
#define TINKERCELL_CORENETWORKHANDLE_H

#include <stdlib.h>
#include <QtGui>
#include <QObject>
#include <QString>
#include <QHash>
#include <QUndoCommand>

#include "DataTable.h"
#include "HistoryWindow.h"
#include "SymbolsTable.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{
	class ItemHandle;
	class ItemData;
	class MainWindow;
	class NetworkWindow;
	
	/*! \brief
	A class that is used to store a network. The network is a collection of Item Handles. 
	The history stack is also a key component of a network.
	The network can either be represented as text using TextEditor or visualized with graphical items in the
	GraphicsScene. Each node and connection are contained in a handle, and each handle can either be represented as text or as graphics.
	The two main components of NetworkWindow are the SymbolsTable and HistoryStack
	This class provides functions for inserting items, removing items, and changing information inside the model.
	\ingroup core
	*/
	class MY_EXPORT NetworkHandle : public QObject
	{
		Q_OBJECT

	private:
		/*! \brief the file name where this network is saved*/
		QString _filename;
		/*! \brief the main window containing this network*/
		MainWindow * _mainWindow;
		/*! \brief all the windows used to display this network*/
		QList<NetworkWindow*> _windows;
		/*! \brief the undo stack*/
		QUndoStack _history;
		/*! \brief holds a hash of all items and data in this scene.
		\sa SymbolsTable*/
		SymbolsTable _symbolsTable;
		/*! \brief pointer to current scene*/
		GraphicsScene * _currentScene;
		/*! \brief calls mainWindow's setCurrentWindow method*/
		virtual void setAsCurrentNetwork();
		
	public:
		/*! \name Constructor and destructor
			\{
		*/
		/*! \brief constructor*/
		NetworkHandle(MainWindow *);
		/*! \brief destructor*/
		virtual ~NetworkHandle();
		
		/*! \}
			\name Get items
			get the set of items in the model
			\{
		*/
		
		/*! \brief get all the visible items in this network window*/
		virtual QList<ItemHandle*> handles();
		/*! \brief get list of all items sorted according to family*/
		virtual QList<ItemHandle*> handlesSortedByFamily() const;
		/*! \brief the model global item*/
		virtual ItemHandle* globalHandle();
		/*! \brief gets all the selected items from each graphics scene
		* \return QList<ItemHandle*> list of selected item handles*/
		virtual QList<GraphicsItem*> & selectedItems();
		/*! \brief gets all the moving items from each graphics scene
		* \return QList<ItemHandle*> list of selected item handles*/
		virtual QList<GraphicsItem*> & movingItems() const;
		
		/*! \}
			\name find item handles and data tables
			\{
		*/
		
		/*! \brief get all the items with the given name. Returns a list for non-unique names
		* \param QString
		* \return QList<ItemHandle*>*/
		QList<ItemHandle*> findItem(const QString&) const;
		/*! \brief get all the items with the given name. returned list may be longer if names are non-unique
		* \param QStringList
		* \return QList<ItemHandle*>*/		
		QList<ItemHandle*> findItem(const QStringList&) const;
		/*! \brief get all the items and corresponding data table name that contains the given string. if non-unique, returns nothing
		* \param QString
		* \return QPair<ItemHandle*,QString>*/		
		QPair<ItemHandle*,QString> findData(const QString&) const;
		
		/*! \}
			\name graphics scenes for the network
			get graphics scene or create graphics scenes
			\{
		*/

		/*! \brief get all the graphics scenes used to illustrate this network
		* \return QList<GraphicsScene*>
		*/
		virtual QList<GraphicsScene*> scenes() const;
		/*! \brief create a new scene for this network
		* \param QList<QGraphicsItem*> items to initialize the network with
		* \return GraphicsScene* the new scene
		*/
		virtual GraphicsScene * createScene(const QList<QGraphicsItem*>& insertItems = QList<QGraphicsItem*>());
		/*! \brief create a new scene that gets all the items inside the given item handle.
		* \param ItemHandle * 
		* \param QRectF only include the graphicss items 
		* \return GraphicsScene* the new scene
		*/
		virtual GraphicsScene * createScene(ItemHandle *, const QRectF& boundingRect=QRectF());
		/*! \brief checks whether a string is a correct formula.
		* \param QString target string (also the output)
		* \param QStringList returns any new variables not found in this network
		* \return Boolean whether or not the string is valid*/
		virtual bool parseMath(QString&,QStringList&);
		
		/*! \}
			\name insert and delete
			These functions will insert or delete graphics items or item handles. The functions
			automatically perform history updates and send appropriate signals, which will inform the 
			other tools that an insertion or deletion has taken place. 
			\{
		*/
		
		/*! \brief insert graphics items. This function will automatically add a command to the history window and emit itemsInserted() signal*/
		virtual void insert(const QString& desc, const QList<QGraphicsItem*> items);
		/*! \brief insert a graphics item. This function will automatically add a command to the history window and emit itemsInserted() signal*/
		virtual void insert(const QString& desc, QGraphicsItem*);
		/*! \brief remove graphics items. This function will automatically add a command to the history window and emit itemsRemoved() signal*/
		virtual void remove(const QString& desc, const QList<QGraphicsItem*> items);
		/*! \brief remove a graphics item. This function will automatically add a command to the history window and emit itemsRemoved() signal*/
		virtual void remove(const QString& desc, QGraphicsItem* items);
		
		/*! \brief insert items. This function will automatically add a command to the history window and emit itemsInserted() signal*/
		virtual void insert(const QString& desc, const QList<ItemHandle*> items);
		/*! \brief insert an item. This function will automatically add a command to the history window and emit itemsInserted() signal*/
		virtual void insert(const QString& desc, ItemHandle*);
		/*! \brief remove items. This function will automatically add a command to the history window and emit itemsRemoved() signal*/
		virtual void remove(const QString& desc, const QList<ItemHandle*> items);
		/*! \brief remove an item. This function will automatically add a command to the history window and emit itemsRemoved() signal*/
		virtual void remove(const QString& desc, ItemHandle* items);
	
		/*! \}
			\name transform and move items in the scene
			These functions	automatically perform history updates and send appropriate signals, which will inform the 
			other tools that an insertion or deletion has taken place. 
			\{
		*/
		
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
			qreal anglechange=0.0,
			bool VFlip=false, bool HFlip=false);
		/*! \brief this command changes the size, angle, and orientation of an item and also adds undo command to history window and emits associated signal(s)*/
		virtual void transform(const QString& name, const QList<QGraphicsItem *>& items,
			const QList<QPointF>& sizechange,
			const QList<qreal>& anglechange=QList<qreal>(),
			bool VFlip=false, bool HFlip=false);

		/*! \}
			\name rename items
			These functions automatically perform history updates and send appropriate signals, which will inform the 
			other tools that an insertion or deletion has taken place. 
			\{
		*/

		/*! \brief rename item and also adds undo command to history window and emits associated signal(s)*/
		virtual void rename(const QString& oldname, const QString& new_name);
		/*! \brief rename an item and also adds undo command to history window and emits associated signal(s)*/
		virtual void rename(ItemHandle * item, const QString& new_name);
		/*! \brief rename items and also adds undo command to history window and emits associated signal(s)*/
		virtual void rename(const QList<ItemHandle*>& items, const QList<QString>& new_names);

		/*! \}
			\name change parents of items
			These functions automatically perform history updates and send appropriate signals, which will inform the 
			other tools that an insertion or deletion has taken place. 
			\{
		*/
		
		/*! \brief change parent handles and also adds undo command to history window and emits associated signal(s)*/
		virtual void setParentHandle(const QList<ItemHandle*>& handles, const QList<ItemHandle*>& parentHandles);
		/*! \brief change parent handle and also adds undo command to history window and emits associated signal(s)*/
		virtual void setParentHandle(ItemHandle * child, ItemHandle * parent);
		/*! \brief change parconst ent handles and also adds undo command to history window and emits associated signal(s)*/
		virtual void setParentHandle(const QList<ItemHandle*> children, ItemHandle * parent);
		/*! \brief places all the graphics items in the given list of handles under the new handle*/
		virtual  void mergeHandles(const QList<ItemHandle*>& handles);
		/*! \brief places all the graphics items under the new handle*/
		virtual  void assignHandles(const QList<QGraphicsItem*>& items, ItemHandle* newHandle);

		/*! \}
			\name change data in one or more items
			These functions automatically perform history updates and send appropriate signals, which will inform the 
			other tools that an insertion or deletion has taken place. 
			\{
		*/
		
		/*! \brief change numerical data table and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const DataTable<qreal>* newdata);
		/*! \brief change a list of numerical data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstring, const QList<DataTable<qreal>*>& newdata);
		/*! \brief change a list of numerical data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<qreal>*>& newdata);
		/*! \brief change text data table and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const DataTable<QString>* newdata);
		/*! \brief change a list of text data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstring, const QList<DataTable<QString>*>& newdata);
		/*! \brief change a list of text data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<QString>*>& newdata);
		/*! \brief change two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const DataTable<qreal>* newdata1, const DataTable<QString>* newdata2);
		/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstring, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& newdata2);
		/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& newdata2);
		/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<DataTable<qreal>*>& olddata1, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& olddata2, const QList<DataTable<QString>*>& newdata2);
		/*! \brief change a two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, DataTable<qreal>* olddata1, const DataTable<qreal>* newdata1, DataTable<QString>* olddata2, const DataTable<QString>* newdata2);
		/*! \brief change a data table and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, DataTable<qreal>* olddata1, const DataTable<qreal>* newdata1);
		/*! \brief change a data table and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, DataTable<QString>* olddata1, const DataTable<QString>* newdata1);

		/*! \}
			\name slots
			update the symbols table that stores all the symbols in the network
			\{
		*/

	public slots:
		/*! \brief updates the symbols table*/
		virtual void updateSymbolsTable();
		/*! \brief updates the symbols table. The int argument is so that this can be connected to the history changed signal*/
		virtual void updateSymbolsTable(int);
		/*! \brief updates the symbols table. The int argument is so that this can be connected to the history changed signal*/
		virtual void close();
		/*! \brief undo last command*/
		virtual void undo();
		/*! \brief redo last command*/
		virtual void redo();

		/*! \}
			\name signals
			\{
		*/

	signals:

		/*! \brief signals whenever an item is renamed
		* \param NetworkHandle* window where the event took place
		* \param QList<ItemHandle*>& items
		* \param QList<QString>& old names
		* \param QList<QString>& new names
		* \return void*/
		void itemsRenamed(NetworkHandle * window, const QList<ItemHandle*>& items, const QList<QString>& oldnames, const QList<QString>& newnames);
		
		/*! \brief signals whenever item parent handle is changed
		* \param NetworkHandle* window where the event took place
		* \param QList<ItemHandle*>& child items
		* \param QList<ItemHandle*>& old parents
		* \return void*/
		void parentHandleChanged(NetworkHandle * window, const QList<ItemHandle*>&, const QList<ItemHandle*>&);
		
		/*! \brief signals whenever some data is changed
		* \param QList<ItemHandle*>& items handles
		* \return void*/
		void dataChanged(const QList<ItemHandle*>& items);
		
		/*! \brief signals whenever the handles for graphics items have changed
		* \param GraphicsScene* scene where the event took place
		* \param QList<GraphicsItem*>& items that are affected
		* \param QList<ItemHandle*>& old handle for each items
		* \return void*/
		void handlesChanged(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>& old);

	private:

		friend class GraphicsView;
		friend class GraphicsScene;
		friend class MainWindow;
		friend class NetworkWindow;
	};

}

#endif

