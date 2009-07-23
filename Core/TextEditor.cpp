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

    void TextEditor::push(QUndoCommand * c)
    {
        if (!c) return;

        QUndoCommand * composite = new CompositeCommand(c->text(), new TextUndoCommand(this, prevText, toPlainText()), c);

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
        if (networkWindow)
            networkWindow->history.redo();
    }

    void TextEditor::copy()
    {
        CodeEditor::copy();
    }

    void TextEditor::cut()
    {
        CodeEditor::cut();
		emit textChanged(this, tr(""), tr(""), prevText);
		prevText = toPlainText();
    }

    void TextEditor::paste()
    {
        CodeEditor::paste();
		emit textChanged(this, tr(""), tr(""), prevText);
		prevText = toPlainText();
    }

    void TextEditor::selectAll()
    {
        CodeEditor::selectAll();
    }

    void TextEditor::print(QPrinter * printer)
    {
        CodeEditor::print(printer);
    }

    void TextEditor::find(const QString& s)
    {
        CodeEditor::find(s);
    }

    void TextEditor::replace(const QString& old_string, const QString& new_string)
    {
		QString text = toPlainText();
		QString oldText = text;
		QRegExp regex1(tr("[^A-Za-z_]") + old_string + tr("[^A-Za-z_0-9]"));
		QRegExp regex2(tr("^") + old_string + tr("[^A-Za-z_0-9]"));
		QRegExp regex3(tr("[^A-Za-z_]") + old_string + tr("$"));
		QRegExp regex4(tr("^") + old_string + tr("$"));
		text.replace(regex1,new_string);
		text.replace(regex2,new_string);
		text.replace(regex3,new_string);
		text.replace(regex4,new_string);
		setPlainText(text);
		emit textChanged(this, tr(""), tr(""), oldText);
    }

    TextEditor::TextEditor()
    {
        setUndoRedoEnabled(false);
		prevBlockNumber = -1;
        changedBlockNumber = -1;		
        setWordWrapMode(QTextOption::NoWrap);
        connect(this,SIGNAL(textChanged()),this,SLOT(textChangedSlot()));
    }
	
	TextEditor::~TextEditor()
    {
		
	}
	
	QString TextEditor::selectedText() const
	{
		return textCursor().selectedText();
	}
	
	void TextEditor::contextMenuEvent ( QContextMenuEvent * event )
	{
		if (!networkWindow || !networkWindow->mainWindow || !event) return;
		
		if (selectedText().isEmpty())
			networkWindow->mainWindow->contextEditorMenu.exec(event->globalPos());
		else
			networkWindow->mainWindow->contextSelectionMenu.exec(event->globalPos());
	}
	
	QList<TextItem*>& TextEditor::items()
	{
		return allItems;
	}
	
	void TextEditor::setItems( const QList<TextItem*>& newItems)
	{
		QList<QUndoCommand*> commands;
		commands << new RemoveItemsCommand(this,allItems)
				 << new InsertItemsCommand(this,newItems);
		
		ItemHandle * h = 0;
		QList<ItemHandle*> handles;
		for (int i=0; i < newItems.size(); ++i)
			if ( (h = getHandle(newItems[i])) 
				&& !handles.contains(h))
			{
				handles << h;
			}
		emit itemsRemoved(this, newItems, handles);
		if (prevBlockText.isEmpty())
			push( new CompositeCommand(tr("line ") + QString::number(prevBlockNumber) + tr(" changed"),commands)  );
		else
			push( new CompositeCommand(prevBlockText,commands)  );
		emit itemsInserted(this, newItems, handles);
	}
	
	void TextEditor::insertItem( TextItem * item )
	{
		if (item && !allItems.contains(item))
		{
			push( new InsertItemsCommand(this,item) );
			
			QList<TextItem*> list;
			list << item;
			QList<ItemHandle*> handles;
			handles << getHandle(item);
			emit itemsInserted(this, list, handles);
		}
	}

	void TextEditor::insertItems( const QList<TextItem*>& list)
	{
		push( new InsertItemsCommand(this,list) );
			
		ItemHandle * h = 0;
		QList<ItemHandle*> handles;
		for (int i=0; i < list.size(); ++i)
			if ((h = getHandle(list[i])) && 
				!handles.contains(h))
				handles << h;
		emit itemsInserted(this, list, handles);
	}

	void TextEditor::removeItem( TextItem * item)
	{
		if (item && allItems.contains(item))
		{
			push( new RemoveItemsCommand(this,item) );
			
			QList<TextItem*> list;
			list << item;
			QList<ItemHandle*> handles;
			handles << getHandle(item);
			emit itemsRemoved(this, list, handles);
		}
		
	}

	void TextEditor::removeItems( const QList<TextItem*>& list)
	{
		push( new RemoveItemsCommand(this,list) );
			
		ItemHandle * h = 0;
		QList<ItemHandle*> handles;
		for (int i=0; i < list.size(); ++i)
			if ((h = getHandle(list[i])) && 
				!handles.contains(h))
				handles << h;
		emit itemsRemoved(this, list, handles);
	}
	
    void TextEditor::textChangedSlot()
    {
        changedBlockNumber = textCursor().blockNumber();
        if (prevBlockNumber == changedBlockNumber)
            changedBlockText = textCursor().block().text();
    }

    void TextEditor::keyPressEvent ( QKeyEvent * event )
    {
        int n0 = textCursor().blockNumber();

        if (event->matches(QKeySequence::Redo)) //redo
        {
            if (networkWindow)
                networkWindow->history.redo();
            return;
        }
        else
        if (event->matches(QKeySequence::Undo))  //undo
        {
            if (networkWindow)
                networkWindow->history.undo();
            return;
        }
        else
		if (event->matches(QKeySequence::Copy)) //redo
        {
            copy();
            return;
        }
        else
        if (event->matches(QKeySequence::Cut))  //undo
        {
            cut();
            return;
        }
        else
        if (event->matches(QKeySequence::Paste))  //undo
        {
            paste();
            return;
        }
        else
			CodeEditor::keyPressEvent(event);

        int n1 = textCursor().blockNumber();

        if (n0 != n1)
        {
            if (changedBlockNumber > -1)
            {
                emit textChanged(this, prevBlockText, changedBlockText, prevText);
				prevText = toPlainText();
            }
            prevBlockText = textCursor().block().text();
            prevBlockNumber = n1;
            changedBlockNumber = -1;

            emit lineChanged(this, n1,prevBlockText);
        }
    }
	
	void TextEditor::mouseReleaseEvent ( QMouseEvent * event )
	{
		int n1 = textCursor().blockNumber();
		if (changedBlockNumber > -1)
		{
			emit textChanged(this, prevBlockText, changedBlockText, prevText);
			prevText = toPlainText();
		}
		
		prevBlockText = textCursor().block().text();
		prevBlockNumber = n1;
		changedBlockNumber = -1;

		emit lineChanged(this, n1,prevBlockText);
	}

    TextUndoCommand::TextUndoCommand(TextEditor * editor, const QString& oldText, const QString& newText)
        : textEdit(editor)
    {
        if (editor)
        {
            this->oldText = oldText;
			this->newText = newText;
        }
    }

    void TextUndoCommand::redo()
    {
        if (textEdit)
		{
			int pos = textEdit->textCursor().position();
            textEdit->setPlainText( newText );
			QTextCursor cursor = textEdit->textCursor();
			cursor.setPosition(pos);
			textEdit->setTextCursor(cursor);
		}
    }

    void TextUndoCommand::undo()
    {
        if (textEdit)
		{
			int pos = textEdit->textCursor().position();
            textEdit->setPlainText( oldText );
			QTextCursor cursor = textEdit->textCursor();
			cursor.setPosition(pos);
			textEdit->setTextCursor(cursor);
		}
    }
}
