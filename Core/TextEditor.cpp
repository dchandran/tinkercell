/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This file defines the TextEditor class. The TextEditor and Canvas are two ways to define a network.
 The Canvas is used for graphical reprentation of a network, whereas the TextEditor is used for
 text-based representation of a network.

****************************************************************************/

#include "OutputWindow.h"
#include "NetworkWindow.h"
#include "TextEditor.h"
#include "TextItem.h"
#include "ItemFamily.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "UndoCommands.h"
#include <QTextBlock>
#include <QTextCharFormat>
#include <QFont>
#include <QVBoxLayout>
#include <QRegExp>
#include <QTextCursor>
#include <QListWidgetItem>

namespace Tinkercell
{
    TextEditor::TextEditor() :
            networkWindow(0),
			editor(new Editor(this)), 
			listWidget(0),
			splitter(0), 
			informationLine(new QLabel("line:",this))
    {
		alignment = Qt::AlignLeft;

        //listWidget->setMaximumWidth(150);

        //QWidget * editorAndLine = new QWidget;
        QVBoxLayout * layout = new QVBoxLayout;
        //layout->addWidget(editor);
        //layout->addWidget(informationLine);
        //editorAndLine->setLayout(layout);

        //splitter->addWidget(editorAndLine);
        //splitter->addWidget(listWidget);

        //layout = new QVBoxLayout;

        //listWidget->setStyleSheet(tr("background-color: qlineargradient(x1: 1, y1: 0, x2: 0, y2: 0, stop: 0 #AAAAAA, stop: 0.8 #888888, stop: 1.0 #D9DDEB);"));
        //editor->setStyleSheet(tr("background-color: #FFFFFF;"));
		//editor->setStyleSheet(tr("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FFFFFF, stop: 0.8 #FFFFFF, stop: 0.9 #C9E8FA, stop: 1.0 #E5F6F9);"));
		
        editor->setCursorWidth(3);

        //layout->addWidget(splitter);
		layout->addWidget(editor);
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
        setLayout(layout);

        textHistory << tr(""); //history at index=0
		
		//enableToolBar();
    }

    TextEditor::~TextEditor()
    {
        for (QSet<TextItem*>::iterator i = textItems.begin();
                i != textItems.end();
                ++i)
        {
            if (*i)
                delete (*i);
        }
        textItems.clear();
    }

    QSet<TextItem*>& TextEditor::items()
    {
        return textItems;
    }

    void TextEditor::push(QUndoCommand * c)
    {
        if (!c) return;

        QUndoCommand * composite = new CompositeCommand(c->text(), new TextUndoCommand(this), c);

        if (networkWindow)
            networkWindow->history.push( composite );
        else
        {
            composite->redo();
            delete composite;
        }
    }

    void TextEditor::undo()
    {
        if (networkWindow)
            networkWindow->history.undo();
    }

    void TextEditor::redo()
    {
        if (editor)
            editor->redo();
    }

    void TextEditor::copy()
    {
        if (editor)
            editor->copy();
    }

    void TextEditor::cut()
    {
        if (editor)
            editor->cut();
    }

    void TextEditor::paste()
    {
        if (editor)
            editor->paste();
    }

    void TextEditor::selectAll()
    {
        if (editor)
            editor->selectAll();
    }

    void TextEditor::print(QPrinter * printer)
    {
        if (editor)
            editor->print(printer);
    }

    void TextEditor::find(const QString& s)
    {
        emit findText(s);
    }

    void TextEditor::replace(const QRegExp& old_expression, const QString& new_string)
    {
        emit replaceText(old_expression,new_string);
    }

    void TextEditor::insertItem( TextItem* item)
    {
        textItems.insert(item);

        QList<TextItem*> list;
        list << item;

        ItemHandle * handle = getHandle(item);
        QList<ItemHandle*> handles;

        if (handle)
            handles << handle;

        emit itemsInserted(this,list,handles);

        if (item->asOp())
            emit operationInserted(this,item->asOp());
    }

    void TextEditor::insertItems( const QList<TextItem*>& items)
    {
        ItemHandle * handle = 0;
        QList<ItemHandle*> handles;

        for (int i=0; i < items.size(); ++i)
            if (items[i])
            {
                textItems.insert(items[i]);
                handle = getHandle(items[i]);
                if (handle)
                    handles << handle;
            }

        emit itemsInserted(this,items,handles);

        for (int i=0; i < items.size(); ++i)
            if (items[i])
            {
                if (items[i]->asOp())
                    emit operationInserted(this,items[i]->asOp());
            }
    }

    void TextEditor::removeItem( TextItem* item )
    {
        textItems.remove(item);

        QList<TextItem*> list, ops;
        list << item;

        ItemHandle * handle = getHandle(item);
        QList<ItemHandle*> handles;

        if (handle)
            handles << handle;

        emit itemsRemoved(this,list,handles);

         if (item->asOp())
             emit operationRemoved(this,item->asOp());
    }

    void TextEditor::removeItems( const QList<TextItem*>& items )
    {
        ItemHandle * handle = 0;
        QList<ItemHandle*> handles;

        for (int i=0; i < items.size(); ++i)
            if (items[i])
            {
                textItems.remove(items[i]);
                handle = getHandle(items[i]);
                if (handle)
                    handles << handle;
            }

        emit itemsRemoved(this,items,handles);

        for (int i=0; i < items.size(); ++i)
            if (items[i])
            {
                if (items[i]->asOp())
                    emit operationRemoved(this,items[i]->asOp());
            }
    }

    void TextEditor::addToToolBar(QWidget * widget)
    {
		if (!widget || !listWidget) return;
        
		QListWidgetItem * item = new QListWidgetItem;
		listWidget->addItem(item);
		listWidget->setItemWidget(item, widget);
    }

    void TextEditor::removeFromToolBar(QWidget * widget)
    {
        if (!listWidget) return;
		QList<QListWidgetItem*> items;
		
		int count = listWidget->count();
		
		for (int i=0; i < count; ++i)
			if (listWidget->item(i) && 
				listWidget->itemWidget(listWidget->item(i)) && 
				(listWidget->itemWidget(listWidget->item(i)) != widget))
				items << listWidget->item(i);
		
		listWidget->clear();
		
		for (int i=0; i < items.size(); ++i)
			listWidget->addItem(items[i]);
    }

    void TextEditor::enableToolBar(bool show)
    {
        if (!splitter || !listWidget)
            return;

        listWidget->setVisible(show);
    }

    void TextEditor::alignToolBar(Qt::Alignment align)
    {
        if (!splitter || !listWidget)
            return;

        switch(align)
        {
        case Qt::AlignLeft:
            splitter->setOrientation(Qt::Vertical);
            splitter->addWidget(listWidget);
            splitter->addWidget(editor);
            break;
        case Qt::AlignRight:
            splitter->setOrientation(Qt::Vertical);
            splitter->addWidget(editor);
            splitter->addWidget(listWidget);
            break;
        case Qt::AlignTop:
            splitter->setOrientation(Qt::Horizontal);
            splitter->addWidget(listWidget);
            splitter->addWidget(editor);
            break;
        default:
            splitter->setOrientation(Qt::Horizontal);
            splitter->addWidget(editor);
            splitter->addWidget(listWidget);
            break;

        }
    }

    Editor::Editor(TextEditor * t) : CodeEditor(), textEditor(t)
    {
        setUndoRedoEnabled(false);
        prevBlockNumber = -1;
        changedBlockNumber = -1;
        setWordWrapMode(QTextOption::NoWrap);
        connect(this,SIGNAL(textChanged()),this,SLOT(textChangedSlot()));
    }

    void Editor::textChangedSlot()
    {
        changedBlockNumber = textCursor().blockNumber();
        if (prevBlockNumber == changedBlockNumber)
            changedBlockText = textCursor().block().text();
    }

    void Editor::keyPressEvent ( QKeyEvent * event )
    {
        int n0 = textCursor().blockNumber();

        if (event->matches(QKeySequence::Redo)) //redo
        {
            if (textEditor && textEditor->networkWindow)
                textEditor->networkWindow->history.redo();
            return;
        }
        else
            if (event->matches(QKeySequence::Undo))  //undo
            {
            if (textEditor && textEditor->networkWindow)
                textEditor->networkWindow->history.undo();
            return;
        }
        else
            CodeEditor::keyPressEvent ( event );

        int n1 = textCursor().blockNumber();
        if (n0 != n1)
        {
            if (changedBlockNumber > -1)
            {
                if (textEditor)
                    textEditor->emitTextChanged(prevBlockText,changedBlockText);
            }
            prevBlockText = textCursor().block().text();
            prevBlockNumber = n1;
            changedBlockNumber = -1;

            if (textEditor)
                textEditor->emitLineChanged(n1,prevBlockText);
        }
    }

    QTextDocument * TextEditor::document()
    {
        if (!editor)
        {
            editor = new Editor(this);
            if (!splitter)
                splitter = new QSplitter(this);
            splitter->addWidget(editor);
        }
        return editor->document();
    }

    void TextEditor::emitTextChanged(const QString& s0, const QString& s1)
    {
        emit textChanged(s0,s1);
    }

    void TextEditor::emitLineChanged(int i, const QString& s)
    {
        emit lineChanged(i,s);
    }

    void TextEditor::setStatusBarText(const QString& s)
    {
        if (informationLine)
            informationLine->setText(s);
    }

    TextUndoCommand::TextUndoCommand(TextEditor * editor)
        : textEdit(editor)
    {
        if (editor && editor->editor)
        {
            historyPosition = editor->textHistory.size();
            editor->textHistory += editor->editor->toPlainText();
        }
    }

    void TextUndoCommand::redo()
    {
        if (textEdit && textEdit->editor && textEdit->textHistory.size() > historyPosition)
            textEdit->editor->setPlainText( textEdit->textHistory.value(historyPosition) );
    }

    void TextUndoCommand::undo()
    {
        if (textEdit && textEdit->editor &&
            textEdit->textHistory.size() > historyPosition &&
            historyPosition >= 1)
            textEdit->editor->setPlainText( textEdit->textHistory.value(historyPosition-1) );
    }


}
