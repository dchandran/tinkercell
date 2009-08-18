/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines the class that is used as a general output area as well as 
a generic command prompt (e.g. by Python plugin)


****************************************************************************/

#include "MainWindow.h"
#include "ConsoleWindow.h"

namespace Tinkercell
{

	void CommandTextEdit::setCompleter(QCompleter *completer)
	{
		if (c)
			QObject::disconnect(c, 0, this, 0);

		c = completer;

		if (!c)
			return;

		c->setWidget(this);
		c->setCompletionMode(QCompleter::PopupCompletion);
		c->setCaseSensitivity(Qt::CaseInsensitive);
		QObject::connect(c, SIGNAL(activated(const QString&)),
			this, SLOT(insertCompletion(const QString&)));
	}

	QCompleter *CommandTextEdit::completer() const
	{
		return c;
	}

	void CommandTextEdit::insertCompletion(const QString& completion)
	{
		if (c->widget() != this)
			return;
		QTextCursor tc = textCursor();
		int extra = completion.length() - c->completionPrefix().length() - 1;
		tc.movePosition(QTextCursor::Left);
		tc.movePosition(QTextCursor::EndOfWord);
		tc.insertText(completion.right(extra));
		setTextCursor(tc);
	}

	QString CommandTextEdit::textUnderCursor() const
	{
		QTextCursor tc = textCursor();
		tc.select(QTextCursor::WordUnderCursor);
		return tc.selectedText();
	}

	void CommandTextEdit::focusInEvent(QFocusEvent *e)
	{
		if (c)
			c->setWidget(this);
		QTextEdit::focusInEvent(e);
	}


	CommandTextEdit::CommandTextEdit(QWidget * parent): QTextEdit(parent), c(0)
	{
		setUndoRedoEnabled ( false );

		setCursorWidth(2);
		QFont font = this->font();
		font.setPointSize(12);
		setFont(font);

		setPalette(QPalette(QColor("#000000")));

		QTextCursor cursor = textCursor();

		currentHistoryIndex = 0;
		frozen = false;

		errorFormat.setFontWeight(QFont::Bold);
		errorFormat.setForeground(QColor("#FF6F0F"));

		//messageFormat.setFontWeight(QFont::Bold);
		messageFormat.setForeground(QColor("#00FF12"));

		normalFormat.setFontWeight(QFont::Bold);
		normalFormat.setForeground(QColor("#FEFFEC"));

		cursor.setCharFormat(normalFormat);	
		cursor.insertText(tr(">>")); 
		currentPosition = cursor.position();
	}

	void CommandTextEdit::wheelEvent ( QWheelEvent * wheelEvent )
	{
		if (wheelEvent == 0) return;

		if (wheelEvent->modifiers() == Qt::ControlModifier)
		{
			if (wheelEvent->delta() > 0)
				zoomIn();
			else
				zoomOut();
		}
		else
		{
			QTextEdit::wheelEvent(wheelEvent);
		}
	}

	bool CommandTextEdit::isFrozen()
	{
		return frozen;
	}

	void CommandTextEdit::error(const QString& s)
	{
		QTextCursor cursor = textCursor();
		cursor.setPosition(currentPosition);

		cursor.setCharFormat(errorFormat);
		cursor.insertText(tr("\nError: ") + s + tr("\n"));

		cursor.setCharFormat(normalFormat);	
		cursor.insertText(tr(">>"));

		if (cursor.position() > currentPosition)
			currentPosition = cursor.position();
		this->ensureCursorVisible();
	}

	void CommandTextEdit::message(const QString& s)
	{
		QTextCursor cursor = textCursor();
		cursor.setPosition(currentPosition);
		/*if (cursor.block().text() == tr(">>"))
		{
		cursor.setPosition(currentPosition-2,QTextCursor::KeepAnchor);
		cursor.removeSelectedText();
		}*/
		cursor.setCharFormat(messageFormat);
		cursor.insertText(tr("\n") + s + tr("\n"));

		cursor.setCharFormat(normalFormat);	
		cursor.insertText(tr(">>"));

		if (cursor.position() > currentPosition)
			currentPosition = cursor.position();
		this->ensureCursorVisible();
	}

	void CommandTextEdit::clearText()
	{
		clear();

		currentPosition = this->toPlainText().length();

		this->ensureCursorVisible();
	}

	void CommandTextEdit::freeze()
	{
		setFreeze(true);
	}

	void CommandTextEdit::unfreeze()
	{
		setFreeze(false);
	}

	void CommandTextEdit::setFreeze(bool frozen)
	{
		QTextCursor cursor = textCursor();

		if (this->frozen == frozen)
		{
			currentPosition = cursor.position();
			currentHistoryIndex = historyStack.size();
			this->ensureCursorVisible();
		}

		this->frozen = frozen;

		if (frozen)
		{
			if (cursor.block().text() == tr(">>"))
			{
				cursor.setPosition(currentPosition-2,QTextCursor::KeepAnchor);
				cursor.removeSelectedText();
			}
		}
		else
		{
			cursor.setCharFormat(normalFormat);
			if (cursor.block().text() != tr(">>"))
				cursor.insertText(tr(">>"));
		}
		if (cursor.position() > currentPosition)
			currentPosition = cursor.position();
		currentHistoryIndex = historyStack.size();
		this->ensureCursorVisible();
	}

	void CommandTextEdit::keyPressEvent ( QKeyEvent * event )
	{
		if (c && c->popup()->isVisible()) 
		{
			// The following keys are forwarded by the completer to the widget
			switch (event->key()) {
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab:
				event->ignore();
				return; // let the completer do default behavior
			default:
				break;
			}
		}

		int key = event->key();
		QTextCursor cursor = textCursor();

		cursor.setCharFormat(normalFormat);

		if (key == Qt::Key_Return || key == Qt::Key_Enter)
		{
			if (cursor.position() <= currentPosition)
				cursor.setPosition(currentPosition);

			if (frozen) return;
			QString command = cursor.block().text().remove(0,2);

			cursor.movePosition(QTextCursor::EndOfBlock);
			cursor.setPosition(currentPosition,QTextCursor::KeepAnchor);
			cursor.removeSelectedText();
			cursor.insertText(command + tr("\n"));
			cursor.movePosition(QTextCursor::EndOfBlock);
			currentPosition = cursor.position();
			if (!command.isEmpty())
			{	
				if (historyStack.isEmpty() || command != historyStack.back())
				{
					historyStack << command;
					currentHistoryIndex = historyStack.size();
				}

				emit commandExecuted(command);
			}

			if (cursor.block().text() != tr(">>"))
			{
				cursor.setCharFormat(normalFormat);	
				cursor.insertText(tr(">>"));
			}
			if (cursor.position() > currentPosition)
				currentPosition = cursor.position();
			currentHistoryIndex = historyStack.size();
			this->ensureCursorVisible();
		}
		else
			if (key == Qt::Key_Escape)
			{
				if (frozen)
				{
					emit commandInterrupted();
					return;
				}
				cursor.setPosition(currentPosition,QTextCursor::KeepAnchor);
				cursor.removeSelectedText();
				currentHistoryIndex = historyStack.size();
			}
			else
				if ((document()->lastBlock() == cursor.block()) && (key == Qt::Key_Up || key == Qt::Key_Down) && !event->modifiers())
				{
					if (frozen)	return;

					cursor.setPosition(currentPosition);

					if (key == Qt::Key_Up)
						--currentHistoryIndex;

					if (key == Qt::Key_Down)
						++currentHistoryIndex;

					if (currentHistoryIndex > historyStack.size()) currentHistoryIndex = historyStack.size();

					if (currentHistoryIndex < 0) currentHistoryIndex = 0;

					if (historyStack.size() > 0)
					{
						cursor.movePosition(QTextCursor::EndOfBlock);
						cursor.setPosition(currentPosition,QTextCursor::KeepAnchor);
						cursor.removeSelectedText();
						if (currentHistoryIndex < historyStack.size())
							cursor.insertText(historyStack[currentHistoryIndex]);
						cursor.movePosition(QTextCursor::EndOfBlock);
					}
				}
				else
					if (frozen && event->modifiers() == Qt::ControlModifier && (key == Qt::Key_C))
					{
						emit commandInterrupted();
					}
					else
						if (key == Qt::Key_Up || key == Qt::Key_Down || key == Qt::Key_Left || key == Qt::Key_Right 
							|| key == Qt::Key_PageUp || key == Qt::Key_PageDown || key == Qt::Key_End || key == Qt::Key_Home
							|| !(	frozen 
							|| !document() 
							|| cursor.position() < currentPosition
							|| cursor.selectionStart() < currentPosition
							|| (cursor.position() == currentPosition && key == Qt::Key_Backspace)))
						{
							QString completionPrefix = textUnderCursor();
							bool isShortcut = ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_E); // CTRL+E
							if (!c || !isShortcut) // dont process the shortcut when we have a completer
								QTextEdit::keyPressEvent(event);

							const bool ctrlOrShift = event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
							if (!c || (ctrlOrShift && event->text().isEmpty()))
								return;

							//static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-= "); // end of word
							bool hasModifier = (event->modifiers() != Qt::NoModifier) && !ctrlOrShift;


							if (!isShortcut && (hasModifier || event->text().isEmpty() || completionPrefix.length() < 2)) 
							{
								c->popup()->hide();
								return;
							}

							if (completionPrefix != c->completionPrefix()) 
							{
								c->setCompletionPrefix(completionPrefix);
								c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
							}
							QRect cr = cursorRect();
							cr.setWidth(c->popup()->sizeHintForColumn(0)
								+ c->popup()->verticalScrollBar()->sizeHint().width());
							c->complete(cr);
						}

						if (cursor.position() < currentPosition)
							cursor.setPosition(currentPosition);
						this->ensureCursorVisible();
	}

	/***********************************
	OUTPUT WINDOW
	************************************/

	ConsoleWindow * ConsoleWindow::instance = 0;

	ConsoleWindow::ConsoleWindow(MainWindow * main)
		: Tool(tr("Console Window"))
	{
		setMainWindow(main);
		if (mainWindow)
		{
			QDockWidget* dockWidget = mainWindow->addDockingWindow(name,this,Qt::BottomDockWidgetArea,Qt::BottomDockWidgetArea);
			if (dockWidget)
			{
				//dockWidget->setWindowFlags(Qt::Tool);
				//dockWidget->setAttribute(Qt::WA_ContentsPropagated);
				//dockWidget->setPalette(QPalette(QColor("#747689")));
				//dockWidget->setAutoFillBackground(true);
				//dockWidget->setWindowOpacity(0.6);
				//dockWidget->hide();
			}
		}

		QHBoxLayout * layout = new QHBoxLayout;
		layout->addWidget(&commandTextEdit);
		setLayout(layout);

		connect(&commandTextEdit,SIGNAL(commandExecuted(const QString&)),this,SIGNAL(commandExecuted(const QString&)));
		connect(&commandTextEdit,SIGNAL(commandInterrupted()),this,SIGNAL(commandInterrupted()));
		instance = this;
	}

	/*! \brief show a message text in the output window*/
	void ConsoleWindow::message(const QString& s)
	{
		if (!instance) return;

		instance->commandTextEdit.message(s);

		if (!s.isEmpty())
		{
			if (instance->parentWidget())
				instance->parentWidget()->show();
			else
				instance->show();
		}
	}

	void ConsoleWindow::error(const QString& s)
	{
		if (!instance) return;

		instance->commandTextEdit.error(s);

		if (!s.isEmpty())
		{
			if (instance->parentWidget())
				instance->parentWidget()->show();
			else
				instance->show();
		}
	}

	void ConsoleWindow::printTable(const DataTable<qreal>& table)
	{
		if (!instance) return;

		QString outputs;

		QStringList colnames = table.getColNames(), rownames = table.getRowNames();

		for (int i=0; i < colnames.size(); ++i)
		{
			outputs += tr("\t") + colnames.at(i);
		}
		outputs += tr("\n");
		for (int i=0; i < table.rows(); ++i)
		{
			outputs += rownames.at(i);
			for (int j=0; j < table.cols(); ++j)
			{
				outputs += tr("\t") + QString::number(table.at(i,j));
			}
			outputs += tr("\n");
		}

		instance->commandTextEdit.message(outputs);

		if (!outputs.isEmpty())
		{
			if (instance->parentWidget())
				instance->parentWidget()->show();
			else
				instance->show();
		}
	}

	void ConsoleWindow::freeze()
	{	
		if (!instance) return;
		instance->commandTextEdit.freeze();
	}

	void ConsoleWindow::unfreeze()
	{	
		if (!instance) return;
		instance->commandTextEdit.unfreeze();
	}

	void ConsoleWindow::clear()
	{	
		if (!instance) return;
		instance->commandTextEdit.clearText();
	}

	ConsoleWindow * ConsoleWindow::outputWindow()
	{
		return instance;
	}

	CommandTextEdit * ConsoleWindow::outputWindowEditor()
	{
		return &commandTextEdit;
	}

}
