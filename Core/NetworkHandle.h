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
#include <QToolBar>

#include "DataTable.h"
#include "HistoryWindow.h"
#include "SymbolsTable.h"
#include "NetworkWindow.h"

#ifdef Q_WS_WIN
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{
	class ItemHandle;
	class ItemData;
	class MainWindow;
	class NetworkWindow;
	class ConsoleWindow;
	
	/*! \brief
	A class that is used to store a network. The network is a collection of Item Handles. 
	The history stack is also a key component of a network.
	The network can either be represented as text using TextEditor or visualized with graphical items in the
	GraphicsScene. Each node and connection are contained in a handle, and each handle can either be represented as text or as graphics.
	The two main components of NetworkWindow are the SymbolsTable and HistoryStack
	This class provides functions for inserting items, removing items, and changing information inside the model.
	\ingroup core
	*/
	class TINKERCELLEXPORT NetworkHandle : public QObject
	{
		Q_OBJECT

	private:
		/*! \brief the main window containing this network*/
		MainWindow * mainWindow;
		/*! \brief all the windows used to display this network*/
		QList<NetworkWindow*> networkWindows;
		
	public:
		/*! \brief the undo stack*/
		QUndoStack history;
		/*! \brief holds a hash of all items and data in this scene.
		\sa SymbolsTable*/
		SymbolsTable symbolsTable;
		
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

		/*!
		* \brief gets the current scene that is active
		* \return GraphicsScene* current scene
		*/
		virtual GraphicsScene * currentScene() const;
		/*!
		* \brief gets the text editor that is active
		* \return TextEditor* current editor
		*/
		virtual TextEditor * currentTextEditor() const;
		/*!
		* \brief gets the window that is active
		* \return NetworkWindow* current window
		*/
		virtual NetworkWindow * currentWindow() const;
		/*! \brief same as main window's console()*/
		ConsoleWindow * console() const;

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
		/*! \brief get all the items and corresponding data table name that contains the given string. if non-unique, returns a list
		* \param QString
		* \return QPair<ItemHandle*,QString>*/		
		QList< QPair<ItemHandle*,QString> > findData(const QString&) const;
		/*! \brief get all the items and corresponding data table name that contains the given string. if non-unique, returns a list
		* \param QString
		* \return QPair<ItemHandle*,QString>*/		
		QList< QPair<ItemHandle*,QString> > findData(const QStringList&) const;
		
		/*! \}
			\name create scene or editor
			\{
		*/
		
		/*! \brief this command performs an removal and also adds undo command to history window and emits associated signal(s)*/
		virtual void remove(const QString& name, const QList<QGraphicsItem*>& items);
		/*! \brief get all the graphics scenes used to illustrate this network
		* \return QList<GraphicsScene*>
		*/
		virtual QList<GraphicsScene*> scenes() const;
		/*! \brief get all the text editors used to express this network
		* \return QList<TextEditor*>
		*/
		virtual QList<TextEditor*> editors() const;
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
		/*! \brief create a new text editor for this network
		* \param QString (optional) initial script
		* \return TextEditor* the new scene
		*/
		virtual TextEditor * createTextEditor(const QString& text = QString());
		/*! \brief set all the title for each window representing this network
		* \param QString
		*/
		virtual void setWindowTitle(const QString&);
		/*! \brief get the title for current window representing this network
		* \return QString
		*/
		virtual QString windowTitle() const;
		/*! \brief checks whether a string is a correct formula.
		* \param QString target string (also the output)
		* \param QStringList returns any new variables not found in this network
		* \return Boolean whether or not the string is valid
		*/
		virtual bool parseMath(QString&,QStringList&);		
		/*! \brief checks whether the given string names a unique item or data entry
		* \param QString target string
		* \return QString new string
		*/
		virtual QString makeUnique(const QString&, const QStringList& doNotUseNames=QStringList()) const;
		/*! \brief checks whether the given string names a unique item or data entry
		* \param QStringList target strings
		* \return QStringList new strings
		*/
		virtual QStringList makeUnique(const QStringList&, const QStringList& doNotUseNames=QStringList()) const;
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

		/*! \}
			\name change data in one or more items
			These functions automatically perform history updates and send appropriate signals, which will inform the 
			other tools that an insertion or deletion has taken place. 
			\{
		*/
		
		/*! \brief change numerical data table and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const NumericalDataTable* newdata);
		/*! \brief change a list of numerical data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstring, const QList<NumericalDataTable*>& newdata);
		/*! \brief change a list of numerical data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<NumericalDataTable*>& newdata);
		/*! \brief change text data table and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const TextDataTable* newdata);
		/*! \brief change a list of text data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstring, const QList<TextDataTable*>& newdata);
		/*! \brief change a list of text data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<TextDataTable*>& newdata);
		/*! \brief change two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, ItemHandle* handle, const QString& hashstring, const NumericalDataTable* newdata1, const TextDataTable* newdata2);
		/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<QString>& hashstring, const QList<NumericalDataTable*>& newdata1, const QList<TextDataTable*>& newdata2);
		/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, const QString& hashstring, const QList<NumericalDataTable*>& newdata1, const QList<TextDataTable*>& newdata2);
		/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<NumericalDataTable*>& olddata1, const QList<NumericalDataTable*>& newdata1);
		/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<TextDataTable*>& olddata2, const QList<TextDataTable*>& newdata2);
		/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, const QList<NumericalDataTable*>& olddata1, const QList<NumericalDataTable*>& newdata1, const QList<TextDataTable*>& olddata2, const QList<TextDataTable*>& newdata2);
		/*! \brief change a two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, NumericalDataTable* olddata1, const NumericalDataTable* newdata1, TextDataTable* olddata2, const TextDataTable* newdata2);
		/*! \brief change a data table and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, NumericalDataTable* olddata1, const NumericalDataTable* newdata1);
		/*! \brief change a data table and also adds undo command to history window and emits associated signal(s)*/
		virtual void changeData(const QString& name, const QList<ItemHandle*>& handles, TextDataTable* olddata1, const TextDataTable* newdata1);
		/*! \brief assign the handle for one or more items*/
		virtual void assignHandles(const QList<QGraphicsItem*>& items, ItemHandle* newHandle);
		/*! \brief marge the graphics items and children of two or more handles*/
		virtual void mergeHandles(const QList<ItemHandle*>& handles);
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
		/*! \brief push a new command into the history stack*/
		virtual void push(QUndoCommand*);

		/*! \}
			\name signals
			\{
		*/

	signals:

		/*! \brief signals whenever an item is renamed
		* \param NetworkHandle* network where the event took place
		* \param QList<ItemHandle*>& items
		* \param QList<QString>& old names
		* \param QList<QString>& new names
		* \return void*/
		void itemsRenamed(NetworkHandle * network, const QList<ItemHandle*>& items, const QList<QString>& oldnames, const QList<QString>& newnames);
		
		/*! \brief signals whenever item parent handle is changed
		* \param NetworkHandle* network where the event took place
		* \param QList<ItemHandle*>& child items
		* \param QList<ItemHandle*>& old parents
		* \return void*/
		void parentHandleChanged(NetworkHandle * network, const QList<ItemHandle*>&, const QList<ItemHandle*>&);
		
		/*! \brief signals whenever some data is changed
		* \param QList<ItemHandle*>& items handles
		* \return void*/
		void dataChanged(const QList<ItemHandle*>& items);
		
		/*! \brief signals whenever the handles for graphics items have changed
		* \param NetworkHandle* network where the event took place
		* \param QList<GraphicsItem*>& items that are affected
		* \param QList<ItemHandle*>& old handle for each items
		* \return void*/
		void handlesChanged(NetworkHandle * network, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>& old);

		
	private:

		friend class GraphicsView;
		friend class GraphicsScene;
		friend class TextEditor;
		friend class MainWindow;
		friend class NetworkWindow;
		friend class SymbolsTable;
	};

}

#endif

