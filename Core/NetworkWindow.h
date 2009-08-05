/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

An MDI sub window that can either be represented as text using TextEditor or visualized with graphical items in the 
GraphicsScene. Each node and connection are contained in a handle, and each handle can either be represented as text or as graphics. 
This class provides functions for editing handles, such as changing names, data, etc. 

****************************************************************************/

#ifndef TINKERCELL_MAINNETWORKWINDOW_H
#define TINKERCELL_MAINNETWORKWINDOW_H

#include <stdlib.h>
#include <QtGui>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QString>
#include <QFileDialog>
#include <QtDebug>
#include <QTextEdit>
#include <QAction>
#include <QMenu>
#include <QFile>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QHash>
#include <QUndoCommand>
#include <QMdiSubWindow>
#include <QGraphicsView>

#include "DataTable.h"
#include "HistoryStack.h"
#include "SymbolsTable.h"
#include "GraphicsScene.h"
#include "TextEditor.h"

namespace Tinkercell
{
	class ItemHandle;
	class ItemData;
	class MainWindow;

	/*! \brief 
	An MDI sub window that can either be represented as text using TextEditor or visualized with graphical items in the 
	GraphicsScene. Each node and connection are contained in a handle, and each handle can either be represented as text or as graphics. 
	This class provides functions for editing handles, such as changing names, data, etc. 
	\ingroup core
	*/
	/*! \brief An MDI sub window that just signals before closing */
	class NetworkWindow : public QMdiSubWindow
	{
		Q_OBJECT

	public:
		/*! \brief constructor*/
		NetworkWindow(MainWindow *, GraphicsScene * scene);
		/*! \brief constructor*/
		NetworkWindow(MainWindow *, TextEditor * editor);
		/*! \brief destructor*/
		~NetworkWindow();
		/*! \brief the file name for this window*/	
		QString filename;
		/*! \brief the main window containing this network window*/	
		MainWindow * mainWindow;
		/*! \brief the graphics scene displayed in this window*/	
		GraphicsScene * scene;
		/*! \brief the text editor displayed in this window*/	
		TextEditor * textEditor;
		/*! \brief the undo stack*/
		QUndoStack history;
		/*! \brief holds a hash of all items and data in this scene. 
		\sa SymbolsTable*/
		SymbolsTable symbolsTable;
		/*! \brief get all the items in this network window*/
		QList<ItemHandle*> allHandles() const;
		/*! \brief the model item*/
		ItemHandle* modelItem();
		/*! \brief checks whether a string is a correct formula.
		\param QString target string
		\param QStringList returns any new variables not found in this network
		\return Boolean whether or not the string is valid*/
		bool parseMath(QString&,QStringList&);
		/*! \brief get all the selected items in this network window. The selected items
		are determined differently depending on whether this window has a GraphicsScene
		or a TextEditor. The selectedItems() from each is used to generate the selected handles*/
		QList<ItemHandle*> selectedHandles() const;
		/*! \brief rename item and also adds undo command to history window and emits associated signal(s)*/
		void rename(const QString& oldname, const QString& new_name);
		/*! \brief rename an item and also adds undo command to history window and emits associated signal(s)*/
		void rename(ItemHandle * item, const QString& new_name);
		/*! \brief rename items and also adds undo command to history window and emits associated signal(s)*/
		void rename(const QList<ItemHandle*>& items, const QList<QString>& new_names);		
		/*! \brief change parent handles and also adds undo command to history window and emits associated signal(s)*/
		void setParentHandle(const QList<ItemHandle*>& handles, const QList<ItemHandle*>& parentHandles);
		/*! \brief change parent handle and also adds undo command to history window and emits associated signal(s)*/
		void setParentHandle(ItemHandle * child, ItemHandle * parent);
		/*! \brief change parconst ent handles and also adds undo command to history window and emits associated signal(s)*/
		void setParentHandle(const QList<ItemHandle*> children, ItemHandle * parent);
		/*! \brief change numerical data table and also adds undo command to history window and emits associated signal(s)*/	
		void changeData(ItemHandle* handle, const QString& hashstring, const DataTable<qreal>* newdata);
		/*! \brief change a list of numerical data tables and also adds undo command to history window and emits associated signal(s)*/
		void changeData(const QList<ItemHandle*>& handles, const QList<QString>& hashstring, const QList<DataTable<qreal>*>& newdata);
		/*! \brief change a list of numerical data tables and also adds undo command to history window and emits associated signal(s)*/
		void changeData(const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<qreal>*>& newdata);
		/*! \brief change text data table and also adds undo command to history window and emits associated signal(s)*/	
		void changeData(ItemHandle* handle, const QString& hashstring, const DataTable<QString>* newdata);
		/*! \brief change a list of text data tables and also adds undo command to history window and emits associated signal(s)*/
		void changeData(const QList<ItemHandle*>& handles, const QList<QString>& hashstring, const QList<DataTable<QString>*>& newdata);
		/*! \brief change a list of text data tables and also adds undo command to history window and emits associated signal(s)*/
		void changeData(const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<QString>*>& newdata);
		/*! \brief change two types of data tables and also adds undo command to history window and emits associated signal(s)*/
		void changeData(ItemHandle* handle, const QString& hashstring, const DataTable<qreal>* newdata1, const DataTable<QString>* newdata2);
		/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/	
		void changeData(const QList<ItemHandle*>& handles, const QList<QString>& hashstring, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& newdata2);
		/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/	
		void changeData(const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& newdata2);
		/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/	
		void changeData(const QList<ItemHandle*>& handles, const QList<DataTable<qreal>*>& olddata1, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& olddata2, const QList<DataTable<QString>*>& newdata2);
		/*! \brief change a two types of data tables and also adds undo command to history window and emits associated signal(s)*/	
		void changeData(const QList<ItemHandle*>& handles, DataTable<qreal>* olddata1, const DataTable<qreal>* newdata1, DataTable<QString>* olddata2, const DataTable<QString>* newdata2);
		/*! \brief change a data table and also adds undo command to history window and emits associated signal(s)*/	
		void changeData(const QList<ItemHandle*>& handles, DataTable<qreal>* olddata1, const DataTable<qreal>* newdata1);
		/*! \brief change a data table and also adds undo command to history window and emits associated signal(s)*/	
		void changeData(const QList<ItemHandle*>& handles, DataTable<QString>* olddata1, const DataTable<QString>* newdata1);

	public slots:
		/*! \brief updates the symbols table*/
		void updateSymbolsTable();
		/*! \brief updates the symbols table. The int argument is so that this can be connected to the history changed signal*/
		void updateSymbolsTable(int);
		/*!	\brief close this window */
		void closeWindow();

	signals:
		/*! \brief signal sent before closing
		* \param Boolean setting to false will prevent this window from closing*/
		void closing(bool * );
		/*! \brief signals whenever an item is renamed
		* \param NetworkWindow* window where the event took place
		* \param QList<ItemHandle*>& items
		* \param QList<QString>& old names
		* \param QList<QString>& new names
		* \return void*/
		void itemsRenamed(NetworkWindow * window, const QList<ItemHandle*>& items, const QList<QString>& oldnames, const QList<QString>& newnames);
		/*! \brief signals whenever item parent handle is changed
		* \param NetworkWindow* window where the event took place
		* \param QList<ItemHandle*>& child items
		* \param QList<ItemHandle*>& old parents
		* \return void*/
		void parentHandleChanged(NetworkWindow * window, const QList<ItemHandle*>&, const QList<ItemHandle*>&);
		/*! \brief signals whenever some data is changed
		* \param QList<ItemHandle*>& items handles
		* \return void*/
		void dataChanged(const QList<ItemHandle*>& items);
	protected:
		/*! \brief view for a graphics scene
		\ingroup helper*/
		class GraphicsView : public QGraphicsView
		{
		public:
			/*!
			* \brief default constructor
			*/	
			GraphicsView(GraphicsScene * scene = 0, QWidget * parent = 0);
		protected:
			/*!
			* \brief drag on top event
			*/	
			virtual void dropEvent(QDropEvent *);
			/*!
			* \brief drag and drop event
			*/	
			virtual void dragEnterEvent(QDragEnterEvent *event);
			/*!
			* \brief mouse wheel event
			*/	
			virtual void wheelEvent(QWheelEvent * event);
		};
	};

}

#endif

