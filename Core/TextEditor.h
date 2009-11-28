/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines the TextEditor class. The TextEditor and Canvas are two ways to define a network.
The Canvas is used for graphical reprentation of a network, whereas the TextEditor is used for
text-based representation of a network.

****************************************************************************/

#ifndef TINKERCELL_TEXTEDITOR_H
#define TINKERCELL_TEXTEDITOR_H

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
#include <QPair>
#include <QSet>
#include <QLabel>
#include <QSplitter>
#include <QScrollArea>
#include <QListWidget>
#include <QSyntaxHighlighter>
#include <QUndoCommand>

#include "DataTable.h"
#include "CodeEditor.h"
#include "HistoryWindow.h"
#include "SymbolsTable.h"
#include "Tool.h"
#include "TextItem.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{
	class ItemHandle;
	class ItemData;
	class TextEditor;
	class TextUndoCommand;
	class ConsoleWindow;

	/*! \brief This is the window that allows used to construct networks using text, as
	opposed to graphics, which is done by GraphicsScene. The TextEditor requires a supporting
	tool that parses the text and calls the itemsInserted or itemsRemoved methods. Without a
	supporting parser tool, the TextEditor will not do anything.
	\ingroup core
	*/
	class MY_EXPORT TextEditor : public CodeEditor
	{
		Q_OBJECT
		friend class TextUndoCommand;

	public:

		static bool SideBarEnabled;

		/*! \brief default constructor*/
		TextEditor( QWidget * parent = 0);
		/*! \brief destructor -- removes all the text items*/
		~TextEditor();
		/*! \brief insert a text item
		\param TextItem* the item*/
		void insertItem( TextItem* );
		/*! \brief insert text items
		\param QList<TextItem*> the items*/
		void insertItems( const QList<TextItem*>& );
		/*! \brief remove an item
		\param TextItem* the item*/
		void removeItem( TextItem* );
		/*! \brief remove text items
		\param QList<TextItem*> the items*/
		void removeItems( const QList<TextItem*>& );
		/*! \brief clear existing items and insert new items
		\param QList<TextItem*> the new items*/
		void setItems( const QList<TextItem*>& );
		/*! \brief the network window containing this text editor*/
		NetworkWindow * networkWindow;
		/*! \brief all the items represented by the text in this TextEditor*/
		QList<TextItem*>& items();
		/*! \brief get the console window (same as mainWindow->console())*/
		ConsoleWindow * console();
		/*! \brief push a command to the undo/redo stack
		\param QUndoCommand* */
		void push(QUndoCommand*);
		/*! \brief gets the selected text*/
		QString selectedText() const;
		/*! \brief a pointer to the NetworkWindow SymbolsTable*/
		SymbolsTable * symbolsTable;
		/*! \brief a pointer to the NetworkWindow undo stack */
		QUndoStack* historyStack;
		/*!
		* \brief the context menu that is shown during right-click event on a text editor with text selected.
		Plugins can add new actions to this menu.
		*/
		QMenu * contextSelectionMenu;
		/*!
		* \brief the context menu that is shown during right-click event on a text editor with no text selected.
		Plugins can add new actions to this menu.
		*/
		QMenu * contextEditorMenu;

	signals:
		/*! \brief some text inside this editor has been changed
		\param QString old text
		\param QString new text
		*/
		void textChanged(TextEditor *, const QString&, const QString&, const QString&);
		/*! \brief the cursor has moved to a different line
		\param int index of the current line
		\param QString current line text
		*/
		void lineChanged(TextEditor *, int, const QString&);
		/*!
		* \brief signal that is emitted when items are inserted in this TextEditor.
		* \param TextEditor* where the editting happened
		* \param QList<TextItem*> new items
		* \param QList<ItemHandle*> new item handles
		*/
		void itemsInserted(TextEditor *, const QList<TextItem*>& , const QList<ItemHandle*>&);
		/*!
		* \brief signal that is emitted when items are removed from this TextEditor.
		* \param TextEditor* where the editting happened
		* \param QList<TextItem*> removed items
		* \param QList<ItemHandle*> removed item handles
		*/
		void itemsRemoved(TextEditor *, const QList<TextItem*>& , const QList<ItemHandle*>&);
		/*! \brief request to parse the text in the current text editor
		\param TextEditor* editor
		*/
		void parse(TextEditor *);

	public slots:

		/*! \brief undo last edit*/
		virtual void undo();
		/*! \brief redo last undo*/
		virtual void redo();
		/*! \brief select all text*/
		virtual void selectAll();
		/*! \brief copy selected text*/
		virtual void copy();
		/*! \brief cut selected text*/
		virtual void cut();
		/*! \brief paste text from clipboard*/
		virtual void paste();
		/*! \brief find specified text
		\param QString text to find */
		void find(const QString&);
		/*! \brief find and replace specified text
		\param QRegExp text to find
		\param QString text to replace */
		void replace(const QString& old_string, const QString& new_string);
		/*! \brief print text
		\param QPrinter */
		virtual void print(QPrinter * printer);
		/*! \brief add widget to the side bar
		\param QWidget */
		virtual void addSideBarWidget(QWidget * );
		/*! \brief remove a widget from the side bar
		\param QWidget */
		virtual void removeSideBarWidget(QWidget * );
		/*! \brief set the orientation of the side bar
		\param Qt::Orientation */
		virtual void setSideBarOrientation(Qt::Orientation orientation);
		/*! \brief make a widget containing this text editor and the side bar
		\param Qt::Orientation orientation of the side bar
		\return QWidget */
		virtual QWidget* widget(Qt::Orientation orientation = Qt::Horizontal);

	protected:

		class TextEditorWidget : public QWidget
		{
		public:
			/*! \brief constructor*/
			TextEditorWidget(TextEditor*,Qt::Orientation orientation = Qt::Horizontal);
			/*! \brief the main text editor*/
			TextEditor * textEditor;
			/*! \brief the side bar*/
			QWidget * sideBar;
			/*! \brief splitter contains the main text editor and the side bar*/
			QSplitter * splitter;
			/*! \brief widgets located on the side bar*/
			QList<QWidget*> sideBarWidgets;
			/*! \brief add widget to the side bar
			\param QWidget */
			virtual void addSideBarWidget(QWidget * );
			/*! \brief remove a widget from the side bar
			\param QWidget */
			virtual void removeSideBarWidget(QWidget * );
			/*! \brief set the orientation of the side bar
			\param Qt::Orientation */
			virtual void setSideBarOrientation(Qt::Orientation orientation);
			/*! \brief setup the side bar*/
			virtual void setup();
		};

		/*! \brief the side bar*/
		TextEditorWidget * editorWidget;
		/*! \brief previously accessed line number. This is to keep track of when a line is modified*/
		int prevBlockNumber;
		/*! \brief current line number. This is to keep track of when a line is modified*/
		int changedBlockNumber;
		/*! \brief previously accessed line. This is to keep track of when a line is modified*/
		QString prevBlockText;
		/*! \brief current line. This is to keep track of when a line is modified*/
		QString changedBlockText;
		/*! \brief current text. This is to keep track of when the text is modified*/
		QString prevText;
		/*! \brief listens to keyboard events in order to determine when the current line has changed*/
		virtual void keyPressEvent ( QKeyEvent * event );
		/*! \brief all the items represented by the text in this TextEditor*/
		QList<TextItem*> allItems;
		/*! \brief creates context menu with actions in the contextMenu member*/
		virtual void contextMenuEvent ( QContextMenuEvent * event );
		/*! \brief emits line changed and text changed if needed*/
		virtual void mouseReleaseEvent ( QMouseEvent * event );

	private slots:
		/*! \brief finds out whether a whole line has changed */
		void textChangedSlot();
	};

	/*! \brief this command performs a text change
	* \ingroup undo*/
	class MY_EXPORT TextUndoCommand : public QUndoCommand
	{

	public:
		/*! \brief constructor
		* \param TextEditor* editor where change happened
		* \param QString new text
		*/
		TextUndoCommand(TextEditor *, const QString&, const QString&);
		/*! \brief redo the change*/
		void redo();
		/*! \brief undo the change*/
		void undo();

	private:
		/*! \brief text before*/
		QString oldText;
		/*! \brief text after*/
		QString newText;
		/*! \brief TextEditor where the change happened*/
		TextEditor * textEdit;
	};
}
#endif
