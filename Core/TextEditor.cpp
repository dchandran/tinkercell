/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines the TextEditor class. The TextEditor and Canvas are two ways to define a network.
The Canvas is used for graphical reprentation of a network, whereas the TextEditor is used for
text-based representation of a network.

****************************************************************************/

#include "NetworkHandle.h"
#include "ConsoleWindow.h"
#include "NetworkWindow.h"
#include "MainWindow.h"
#include "TextEditor.h"
#include "ItemFamily.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "UndoCommands.h"
#include <QTextBlock>
#include <QTextCharFormat>
#include <QFont>
#include <QVBoxLayout>
#include <QRegExp>
#include <QGroupBox>
#include <QTextCursor>
#include <QListWidgetItem>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

namespace Tinkercell
{
	bool TextEditor::SideBarEnabled = true;
	void TextEditor::push(QUndoCommand * c)
	{
		if (!network)
		{
			delete c;
			return;
		}
		
		QString text = tr("text changed");
		if (c)
			text = c->text();
		QUndoCommand * composite = new CompositeCommand(text, new TextUndoCommand(this, prevText, toPlainText()), c);

		network->history.push( composite );
	}

	void TextEditor::undo()
	{
		if (network)
			network->history.undo();
	}

	void TextEditor::redo()
	{
		if (network)
			network->history.redo();
	}

	void TextEditor::copy()
	{
		CodeEditor::copy();
	}

	void TextEditor::cut()
	{
		CodeEditor::cut();
		int i = -1;
		if (network)
			i = network->history.count();
		emit textChanged(this, tr(""), tr(""), prevText);
		if (network && i > -1 && i == network->history.count())
			push(0);
		prevText = toPlainText();
	}

	void TextEditor::paste()
	{
		CodeEditor::paste();
		int i = -1;
		if (network)
			i = network->history.count();
		emit textChanged(this, tr(""), tr(""), prevText);
		if (network && i > -1 && i == network->history.count())
			push(0);
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

	TextEditor::TextEditor( NetworkHandle * network, QWidget * parent) : CodeEditor(parent), network(network), networkWindow(0)
	{
		contextEditorMenu = 0;
		contextSelectionMenu = 0;
		setUndoRedoEnabled(false);
		prevBlockNumber = -1;
		changedBlockNumber = -1;
		setWordWrapMode(QTextOption::NoWrap);
		connect(this,SIGNAL(textChanged()),this,SLOT(textChangedSlot()));
	}

	TextEditor::~TextEditor()
	{
		/*for (int i=0; i < allItems.size(); ++i)
			if (allItems[i])
			{
				for (int j=(i+1); j < allItems.size(); ++j)
				{
					if (allItems[j] == allItems[i])
						allItems[j] = 0;
				}
				if (!allItems[i]->handle() || (allItems[i]->handle() && allItems[i]->handle()->parent == 0))
					delete allItems[i];
				allItems[i] = 0;
			}*/
	}

	QString TextEditor::selectedText() const
	{
		return textCursor().selectedText();
	}

	void TextEditor::contextMenuEvent ( QContextMenuEvent * event )
	{
		if (!network || !network->mainWindow || !event) return;

		if (selectedText().isEmpty())
			network->mainWindow->contextEditorMenu.exec(event->globalPos());
		else
			network->mainWindow->contextSelectionMenu.exec(event->globalPos());
	}

	QList<ItemHandle*>& TextEditor::items()
	{
		return allItems;
	}

	void TextEditor::setItems( const QList<ItemHandle*>& newItems)
	{
		QList<QUndoCommand*> commands;

		commands << new RemoveHandlesCommand(this,allItems,false)
		  		  << new InsertHandlesCommand(this,newItems,false);
	
		emit itemsRemoved(network, allItems);
	
		if (prevBlockText.isEmpty())
			push( new CompositeCommand(tr("line ") + QString::number(prevBlockNumber+1) + tr(" changed"),commands)  );
		else
			push( new CompositeCommand(prevBlockText,commands)  );

		emit itemsInserted(network, newItems);
	}

	void TextEditor::insert( ItemHandle * item )
	{
		if (item && !allItems.contains(item))
		{
			push( new InsertHandlesCommand(this,item) );

			QList<ItemHandle*> handles;
			handles << item;
			emit itemsInserted(network, handles);
		}
	}

	void TextEditor::insert( const QList<ItemHandle*>& list)
	{
		push( new InsertHandlesCommand(this,list) );

		emit itemsInserted(network, list);
	}

	void TextEditor::remove( ItemHandle * item)
	{
		if (item && allItems.contains(item))
		{
			push( new RemoveHandlesCommand(this,item) );

			QList<ItemHandle*> handles;
			handles << item;
			emit itemsRemoved(network, handles);
		}

	}

	void TextEditor::remove( const QList<ItemHandle*>& handles)
	{
		push( new RemoveHandlesCommand(this,handles) );

		emit itemsRemoved(network, handles);
	}

	void TextEditor::textChangedSlot()
	{
		changedBlockNumber = textCursor().blockNumber();
		if (prevBlockNumber == changedBlockNumber)
			changedBlockText = textCursor().block().text();
	}
	
	void TextEditor::mousePressEvent ( QMouseEvent * event )
	{
		if (networkWindow)
			networkWindow->setAsCurrentWindow();
		
		CodeEditor::mousePressEvent(event);
	}

	void TextEditor::keyPressEvent ( QKeyEvent * event )
	{
		if (networkWindow)
			networkWindow->setAsCurrentWindow();

		int n0 = textCursor().blockNumber();

		if (event->matches(QKeySequence::Redo)) //redo
		{
			if (network)
				network->history.redo();
			return;
		}
		else
			if (event->matches(QKeySequence::Undo))  //undo
			{
				if (network)
					network->history.undo();
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
				int i = -1;
				if (network)
					i = network->history.count();
				emit textChanged(this, prevBlockText, changedBlockText, prevText);
				if (network && i > -1 && i == network->history.count())
					push(0);
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
	
	MainWindow * TextEditor::mainWindow() const
	{
		if (network)
			return network->mainWindow;
		return 0;
	}
	
	ConsoleWindow * TextEditor::console() const
	{
		if (network && network->mainWindow)
			return network->mainWindow->console();
		return 0;
	}
	
	ItemHandle * TextEditor::localHandle() const
	{
		if (networkWindow)
			return networkWindow->handle;
		return 0;
	}
	
	ItemHandle * TextEditor::globalHandle() const
	{
		if (network)
			return network->globalHandle();
		return 0;
	}
	
	void TextEditor::popOut()
	{
		if (networkWindow)
			networkWindow->popOut();
	}
	
	void TextEditor::popIn()
	{
		if (networkWindow)
			networkWindow->popIn();
	}

}
