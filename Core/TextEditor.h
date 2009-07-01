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
#include <QLabel>
#include <QSplitter>
#include <QSyntaxHighlighter>
#include <QUndoCommand>

#include "DataTable.h"
#include "HistoryStack.h"
#include "SymbolsTable.h"
#include "TextItem.h"

namespace Tinkercell
{
    class ItemHandle;
    class ItemData;
    class TextEditor;

    /*! \brief this command performs a text change
     * \ingroup undo*/
    class TextUndoCommand : public QUndoCommand
    {
    public:
         /*! \brief constructor
          * \param TextEdit* editor where change happened
          * \param QGraphicsItem * items that are affected
          * \param QPointF& amount to move
          */
         TextUndoCommand(TextEditor * scene, QGraphicsItem * item, const QPointF& distance);
         /*! \brief redo the change*/
         void redo();
         /*! \brief undo the change*/
         void undo();
    private:
         /*! \brief the last text in the text editor*/
         QString prevText;
         /*! \brief the last text in the text editor*/
         QString nextText;
         /*! \brief TextEditor where the change happened*/
         TextEditor * textEditor;
    };

    /*! \brief This class is just for the use of TextEditor.
          It is a simple text editor that records when a line has been changed.
     */
    class Editor : public QTextEdit
    {
        Q_OBJECT

        friend class TextEditor;
        friend class TextUndoCommand;
    private:
        Editor(TextEditor *);
        int prevBlockNumber;
        int changedBlockNumber;
        QString prevBlockText;
        QString changedBlockText;
        TextEditor* textEditor;
        virtual void wheelEvent ( QWheelEvent * wheelEvent );
        virtual void keyPressEvent ( QKeyEvent * event );
    private slots:
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
         /*! \brief background color*/
        static QString RegularBackgroundColor;
        /*! \brief plain text color*/
        static QString RegularTextColor;
        /*! \brief highlighted text (e.g. nodes and connections) color*/
        static QString SpecialTextColor;
        /*! \brief commented text background color*/
        static QString CommentsBackgroundColor;
        /*! \brief name of nodes and connections text background color*/
        static QString NameBackgroundColor;
        /*! \brief background color when highlighting (find/replace)*/
        static QString HighlightBackgroundColor;

        /*! \brief multiple node connections allowed? Otherwise, all connections will be between exactly two nodes*/
        static bool MultipleNodesAllowed;
        /*! \brief syntax to use when multiple node connections are allowed; default is "+"*/
        static QString MultipleNodeSeparator;
        /*! \brief description text on connections allowed? */
        static bool ConnectionDescriptionAllowed;
        /*! \brief syntax for inserting descriptions on connections ; default is ";"*/
        static QString ConnectionDescriptionSeparator;
        /*! \brief whether or not to show the node types at the top of the text; default = true*/
        static bool ShowNodeDeclarations;
        /*! \brief whether or not to show the operations at the top of the text; default = true*/
        static bool ShowOperations;

        /*! \brief all the text items in this network indexed by the text block that contains each*/
        QHash<QString, TextItem*> textItems;
        /*! \brief all the allowed syntax for represecting a conenction*/
        static QStringList ConnectionSyntax;
        /*! \brief all the different types that should be recognized as declarations*/
        static QStringList TypeNames;


        /*! \brief default constructor*/
        TextEditor();
        /*! \brief insert nodes*/
        void insertNode(const QList<NodeTextItem*>&);
        /*! \brief insert node*/
        void insertNode(NodeTextItem*);
        /*! \brief insert connections*/
        void insertConnection(const QList<ConnectionTextItem*>&);
        /*! \brief insert connection*/
        void insertConnection( ConnectionTextItem*);
        /*! \brief document inside this editor*/
        QTextDocument * document();
        /*! \brief the underlying text editor*/
        //Arnaud: There was one conlict with the member editor
        QTextEdit * get_editor();
        /*! \brief the network window containing this text editor*/
        NetworkWindow * networkWindow;
         /*! \brief the undo/redo stack*/
        QUndoStack* historyStack;

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
        /*! \brief one of the operations, e.g. equations, has been modified
            \param TextEditor* where the editting happened
            \param OpTextItem* old operation
            \param OpTextItem* modified operation */
        void operationChanged(TextEditor * editor, OpTextItem * from, OpTextItem * to);

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
          \param QString text to find
          */
        void find(const QString&);
        /*! \brief find and replace specified text
          \param QString text to find
          \param QString text to replace
          */
        void replace(const QString& old_string, const QString& new_string);
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
        /*! \brief the side bar widget.
          */
        virtual QWidget* toolBar() const;
        /*! \brief set position of the side bar.
            \param Alignment flag
          */
        virtual void alignToolBar(Qt::Alignment);

    public:
        /*! \brief the text editor*/
        Editor * editor;

    protected:
        /*! \brief how the side bar is aligned*/
        Qt::Alignment alignment;

        /*! \brief the side bar*/
        QWidget * sideBar;
        /*! \brief the scroll area with the side bar*/
        QScrollArea * scrollArea;
        /*! \brief the splitter on which the editor and side bar are located*/
        QSplitter * splitter;
        /*! \brief used to show line number and other info*/
        QLabel * informationLine;

        friend class Editor;
        friend class TextUndoCommand;
    };
}
#endif
