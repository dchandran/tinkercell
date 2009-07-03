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
#include <QListWidget>
#include <QSyntaxHighlighter>
#include <QUndoCommand>

#include "DataTable.h"
#include "CodeEditor.h"
#include "HistoryStack.h"
#include "SymbolsTable.h"
#include "TextItem.h"

namespace Tinkercell
{
    class ItemHandle;
    class ItemData;
    class TextEditor;

    class TextUndoCommand;

    /*! \brief This class is just for the use of TextEditor.
          It is a simple text editor that records when a line has been changed.
     */
    class Editor : public CodeEditor
    {
        Q_OBJECT

        friend class TextEditor;
        friend class TextUndoCommand;
    protected:
        /*! \brief constructor. A TextEdior is required*/
        Editor(TextEditor *);
        /*! \brief previously accessed line number. This is to keep track of when a line is modified*/
        int prevBlockNumber;
        /*! \brief current line number. This is to keep track of when a line is modified*/
        int changedBlockNumber;
        /*! \brief previously accessed line. This is to keep track of when a line is modified*/
        QString prevBlockText;
        /*! \brief current line. This is to keep track of when a line is modified*/
        QString changedBlockText;
        /*! \brief the text editor containing this text edit*/
        TextEditor* textEditor;
        /*! \brief listens to keyboard events in order to determine when the current line has changed*/
        virtual void keyPressEvent ( QKeyEvent * event );
    protected slots:
        /*! \brief listens to textChanged signal to detemine when current line has been modified*/
        void textChangedSlot();
    };


    /*! \brief This is the window that allows used to construct networks using text, as
          opposed to graphics, which is done by GraphicsScene. The syntax defined three
          items: nodes, connections, and operations. Nodes are defined by a name and family.
          Connections are defined by the nodes that is connected to them and the family.
          Operations, which include equations or assignments, are defined by a left-hand side,
          a right-hand side, and an operator. The minimum syntax:

          Syntax for nodes: <family>: <name1>, <name2>...
          Syntax for connections: <name>: <node> + <nodes> + ... <arrow> <node> + <node> ... ; <description1>; <description2>; ...
          Syntax for operations: <name> <op> <description>

          The minumum syntax is used to construct a network with nodes and connections and
          family information for each. The <arrow> is a set of allowed arrow, e.g. ->, which should
          be provided by a plug-in. The additional descriptions of The <op> is also a set of operators, e.g. =, which should also
          be provided by plug-ins.
     */
    class TextEditor : public QWidget
    {
        Q_OBJECT

    public:
        /*! \brief all the text items in this network indexed by the text block that contains each*/
        QSet<TextItem*>& items();
        /*! \brief all the allowed syntax for represecting a conenction*/
        //static QStringList ConnectionSyntax;
        /*! \brief all the different types that should be recognized as declarations*/
        //static QStringList TypeNames;

        /*! \brief default constructor*/
        TextEditor();
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
        /*! \brief document inside this editor*/
        QTextDocument * document();
        /*! \brief the network window containing this text editor*/
        NetworkWindow * networkWindow;
        /*! \brief push a command to the undo/redo stack
             \param QUndoCommand* */
        void push(QUndoCommand*);

    signals:
        /*! \brief new nodes or connections have been inserted
            \param TextEditor* where the editting happened
            \param TextItem* new items*/
        void itemsInserted(TextEditor * editor, const QList<TextItem*>& , const QList<ItemHandle*>&);
        /*! \brief new nodes or connections have been removed
            \param TextEditor* where the editting happened
            \param TextItem* new items*/
        void itemsRemoved(TextEditor * editor, const QList<TextItem*>& , const QList<ItemHandle*>& );
        /*! \brief an operations, e.g. equations, has been inserted
            \param TextEditor* where the editting happened
            \param OpTextItem* operation*/
        void operationInserted(TextEditor * editor, OpTextItem * );
        /*! \brief one of the connections has been modified
            \param TextEditor* where the editting happened
            \param ConnectionTextItem* old connection
            \param ConnectionTextItem* modified connection */
        void connectionChanged(TextEditor * editor, ConnectionTextItem * from, ConnectionTextItem * to);
        /*! \brief one of the operations, e.g. equations, has been deleted
            \param TextEditor* where the editting happened
            \param OpTextItem* operation that was removed*/
        void operationRemoved(TextEditor * editor, OpTextItem * );
        /*! \brief one of the operations, e.g. equations, has been modified
            \param TextEditor* where the editting happened
            \param OpTextItem* old operation
            \param OpTextItem* modified operation */
        void operationChanged(TextEditor * editor, OpTextItem * from, OpTextItem * to);
        /*! \brief request to find the given text
            \param QString string to find*/
        void findText(const QString&);
        /*! \brief request to find and replace the given text
            \param QRegExp regex to find
            \param QString string to replace with*/
        void replaceText(const QRegExp&, const QString&);
        /*! \brief new text has been inserted
            \param QString new text
        */
        void textInserted(const QString&);
        /*! \brief some text has been removed
            \param QString new text
        */
        void textRemoved(const QString&);
        /*! \brief some text inside this editor has been changed
            \param QString old text
            \param QString new text
        */
        void textChanged(const QString&, const QString&);
        /*! \brief the cursor has moved to a different line
            \param int index of the current line
            \param QString current line text
        */
        void lineChanged(int, const QString&);

    public slots:
        /*! \brief set the text in the status bar for this text editor*/
        virtual void setStatusBarText(const QString&);
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
          \param QString text to find
          */
        void find(const QString&);
        /*! \brief find and replace specified text
          \param QRegExp text to find
          \param QString text to replace
          */
        void replace(const QRegExp& old_expression, const QString& new_string);
        /*! \brief print text
          \param QPrinter
          */
        virtual void print(QPrinter * printer);
        /*! \brief add a widget to the side bar. This is designed
            for Tools to place their buttons.
          \param QWidget* usually a button, but can be anything else
          */
        virtual void addToToolBar(QWidget * widget);
        /*! \brief remove a widget to the side bar. This is designed
            for Tools to remove their buttons when inactive.
          \param QWidget* usually a button, but can be anything else
          */
        virtual void removeFromToolBar(QWidget * widget);
        /*! \brief disable or enable the side bar.
            \param bool true = enable, false = disable
          */
        virtual void enableToolBar(bool show=true);
        /*! \brief set position of the side bar.
            \param Alignment flag
          */
        virtual void alignToolBar(Qt::Alignment);

    protected:
        /*! \brief list of all items in the model*/
        QSet<TextItem*> textItems;
        /*! \brief history of document states*/
        QStringList textHistory;
        /*! \brief how the side bar is aligned*/
        Qt::Alignment alignment;
        /*! \brief the text editor*/
        Editor * editor;
        /*! \brief the scroll area with the side bar*/
        QListWidget * listWidget;
        /*! \brief the splitter on which the editor and side bar are located*/
        QSplitter * splitter;
        /*! \brief used to show line number and other info*/
        QLabel * informationLine;
        /*! \brief some text inside this editor has been changed
            \param QString old text
            \param QString new text
        */
        void emitTextChanged(const QString&, const QString&);
        /*! \brief the cursor has moved to a different line
            \param int index of the current line
            \param QString current line text
        */
        void emitLineChanged(int, const QString&);

        friend class Editor;
        friend class TextUndoCommand;
    };

    /*! \brief this command performs a text change
     * \ingroup undo*/
    class TextUndoCommand : public QUndoCommand
    {
    public:
        /*! \brief constructor
          * \param TextEditor* editor where change happened
          */
        TextUndoCommand(TextEditor * );
        /*! \brief redo the change*/
        void redo();
        /*! \brief undo the change*/
        void undo();
    private:
        /*! \brief the number in the history stack represented by this undo*/
        int historyPosition;
        /*! \brief TextEditor where the change happened*/
        TextEditor * textEdit;
    };
}
#endif
