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
	QString ConsoleWindow::Prompt(">>");
	
	QColor ConsoleWindow::BackgroundColor = QColor("#000000");
	QColor ConsoleWindow::PlainTextColor = QColor("#FEFFEC");
	QColor ConsoleWindow::ErrorTextColor = QColor("#FF6F0F");
	QColor ConsoleWindow::OutputTextColor = QColor("#00FF12");
	
	void CommandTextEdit::setBackgroundColor(const QColor& c)
	{
		setPalette(QPalette(ConsoleWindow::BackgroundColor = c));
	}
	
	void CommandTextEdit::setPlainTextColor(const QColor& c)
	{
		normalFormat.setForeground( ConsoleWindow::PlainTextColor = c );
	}
	
	void CommandTextEdit::setOutputTextColor(const QColor& c)
	{
		messageFormat.setForeground( ConsoleWindow::OutputTextColor = c );
	}
	
	void CommandTextEdit::setErrorTextColor(const QColor& c)
	{
		errorFormat.setForeground( ConsoleWindow::ErrorTextColor = c );
	}

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

		setTextInteractionFlags(Qt::TextEditorInteraction);

		setCursorWidth(2);
		QFont font = this->font();
		font.setPointSize(12);
		setFont(font);

		setPalette(QPalette(ConsoleWindow::BackgroundColor));

		QTextCursor cursor = textCursor();

		currentHistoryIndex = 0;
		frozen = false;

		errorFormat.setFontWeight(QFont::Bold);
		errorFormat.setForeground(ConsoleWindow::ErrorTextColor);

		//messageFormat.setFontWeight(QFont::Bold);
		messageFormat.setForeground(ConsoleWindow::OutputTextColor);

		normalFormat.setFontWeight(QFont::Bold);
		normalFormat.setForeground(ConsoleWindow::PlainTextColor);

		cursor.setCharFormat(normalFormat);
		cursor.insertText(ConsoleWindow::Prompt);
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
		cursor.insertText(ConsoleWindow::Prompt);

		if (cursor.position() > currentPosition)
			currentPosition = cursor.position();
		this->ensureCursorVisible();
	}

	void CommandTextEdit::message(const QString& s)
	{
		QTextCursor cursor = textCursor();
		cursor.setPosition(currentPosition);

		cursor.setCharFormat(messageFormat);

		if (s.right(1) != QChar('\n'))
            cursor.insertText(s + tr("\n"));
		else
            cursor.insertText(s);

        cursor.setCharFormat(normalFormat);
        cursor.insertText(ConsoleWindow::Prompt);

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
			if (cursor.position() > currentPosition)
				currentPosition = cursor.position();	
			currentHistoryIndex = historyStack.size();
			this->ensureCursorVisible();
		}

		this->frozen = frozen;

		if (frozen)
		{
			if (cursor.block().text() == ConsoleWindow::Prompt)
			{
				cursor.setPosition(currentPosition-2,QTextCursor::KeepAnchor);
				cursor.removeSelectedText();
			}
		}
		else
		{
			cursor.setCharFormat(normalFormat);
			if (cursor.block().text() != ConsoleWindow::Prompt)
				cursor.insertText(ConsoleWindow::Prompt);
		}
		if (cursor.position() > currentPosition)
			currentPosition = cursor.position();
		currentHistoryIndex = historyStack.size();
		this->ensureCursorVisible();
	}

	void CommandTextEdit::eval(const QString& command)
	{
        if (frozen) return;

	    QTextCursor cursor = textCursor();
        cursor.setCharFormat(normalFormat);

	    if (cursor.position() <= currentPosition)
			cursor.setPosition(currentPosition);

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

        if (cursor.block().text() != ConsoleWindow::Prompt)
        {
            cursor.setCharFormat(normalFormat);
            cursor.insertText(ConsoleWindow::Prompt);
        }
        cursor.movePosition(QTextCursor::EndOfBlock);
        if (cursor.position() > currentPosition)
            currentPosition = cursor.position();
        currentHistoryIndex = historyStack.size();
        this->ensureCursorVisible();
	}

	void CommandTextEdit::keyPressEvent ( QKeyEvent * event )
	{
		if (!event || !document()) return;
		
		currentPosition = document()->lastBlock().position() + 2;

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

		if (event->matches(QKeySequence::Copy))
		{
			QTextEdit::copy();
			return;
		}

		if (event->matches(QKeySequence::SelectAll))
		{
			QTextEdit::selectAll();
			return;
		}

		if (key == Qt::Key_Return || key == Qt::Key_Enter)
		{
			if (frozen || (cursor.position() < currentPosition)) return;
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

			if (cursor.block().text() != ConsoleWindow::Prompt)
			{
				cursor.setCharFormat(normalFormat);
				cursor.insertText(ConsoleWindow::Prompt);
			}
			cursor.movePosition(QTextCursor::EndOfBlock);
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
					{
						if (key == Qt::Key_Up || key == Qt::Key_Down || key == Qt::Key_Left || key == Qt::Key_Right
							|| key == Qt::Key_PageUp || key == Qt::Key_PageDown || key == Qt::Key_End || key == Qt::Key_Home
							|| !(	frozen
									|| cursor.position() < currentPosition
									|| cursor.selectionStart() < currentPosition
									|| (cursor.position() <= currentPosition && key == Qt::Key_Backspace)))
						{
							if (cursor.position() < currentPosition)
							{
								cursor.setPosition(currentPosition);
								this->ensureCursorVisible();
							}
							QString completionPrefix = textUnderCursor();
							bool isShortcut = ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_E); // CTRL+E

							if (!c || !isShortcut) // dont process the shortcut when we have a completer
								QTextEdit::keyPressEvent(event);

							if (key == Qt::Key_Home)
								cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,ConsoleWindow::Prompt.size());

							bool ctrlOrShift = event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
							if (!c || (ctrlOrShift && event->text().isEmpty()))
							{
								if (cursor.position() < currentPosition)
									cursor.setPosition(currentPosition);
								this->ensureCursorVisible();
								return;
							}

							//static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-= "); // end of word
							bool hasModifier = (event->modifiers() != Qt::NoModifier) && !ctrlOrShift;


							if (!isShortcut && (hasModifier || event->text().isEmpty() || completionPrefix.length() < 2))
							{
								c->popup()->hide();
								if (cursor.position() < currentPosition)
									cursor.setPosition(currentPosition);
								this->ensureCursorVisible();
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
					}
        if (cursor.position() < currentPosition)
            cursor.setPosition(currentPosition);
        this->ensureCursorVisible();
	}

	/***********************************
	OUTPUT WINDOW
	************************************/

	ConsoleWindow::ConsoleWindow(MainWindow * main)
		: Tool(tr("Console Window"))
	{
		setMainWindow(main);
		if (mainWindow)
		{
			setWindowTitle(name);
			setWindowIcon(QIcon(tr(":/images/cmd.png")));
			mainWindow->addToolWindow(this,MainWindow::defaultConsoleWindowOption,Qt::RightDockWidgetArea);
		}

		QHBoxLayout * layout = new QHBoxLayout;
		layout->addWidget(&commandTextEdit);
		layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
		setLayout(layout);

		connect(&commandTextEdit,SIGNAL(commandExecuted(const QString&)),this,SIGNAL(commandExecuted(const QString&)));
		connect(&commandTextEdit,SIGNAL(commandInterrupted()),this,SIGNAL(commandInterrupted()));
	}

	/*! \brief show a message text in the output window*/
	void ConsoleWindow::message(const QString& s)
	{
		commandTextEdit.message(s);

		if (!s.isEmpty())
		{
			if (parentWidget())
				parentWidget()->show();
			else
				show();
		}
	}

	void ConsoleWindow::error(const QString& s)
	{
		commandTextEdit.error(s);

		if (!s.isEmpty())
		{
			if (parentWidget())
				parentWidget()->show();
			else
				show();
		}
	}

	void ConsoleWindow::eval(const QString& s)
	{
		commandTextEdit.eval(s);

		if (!s.isEmpty())
		{
			if (parentWidget())
				parentWidget()->show();
			else
				show();
		}
	}

	void ConsoleWindow::printTable(const DataTable<qreal>& table)
	{
		QString outputs;

		QStringList colnames = table.getColNames(), rownames = table.getRowNames();

		outputs += tr("\n");
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

		commandTextEdit.message(outputs);

		if (!outputs.isEmpty())
		{
			if (parentWidget())
				parentWidget()->show();
			else
				show();
		}
	}

	void ConsoleWindow::freeze()
	{
		commandTextEdit.freeze();
	}

	void ConsoleWindow::unfreeze()
	{
		commandTextEdit.unfreeze();
	}

	void ConsoleWindow::clear()
	{
        commandTextEdit.clearText();
	}

	CommandTextEdit * ConsoleWindow::editor()
	{
		return &commandTextEdit;
	}

}
