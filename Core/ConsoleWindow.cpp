/****************************************************************************
Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines the class that is used as a general output area as well as
a generic command prompt (e.g. by Python plugin)
****************************************************************************/
#include <QSettings>
#include "NetworkHandle.h"
#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "GlobalSettings.h"

namespace Tinkercell
{
	QString ConsoleWindow::Prompt(">");
	
	QColor ConsoleWindow::BackgroundColor = QColor("#000000");
	QColor ConsoleWindow::PlainTextColor = QColor("#FEFFEC");
	QColor ConsoleWindow::ErrorTextColor = QColor("#FF6F0F");
	QColor ConsoleWindow::OutputTextColor = QColor("#33FF00");
	QColor ConsoleWindow::TableTextColor = QColor("#FFFF00");
	
	void CommandTextEdit::setBackgroundColor(const QColor& c)
	{
		ConsoleWindow::BackgroundColor = c;
		setStyleSheet( tr("background : ") + ConsoleWindow::BackgroundColor.name());
		update();
	}
	
	void CommandTextEdit::setPlainTextColor(const QColor& c)
	{
		ConsoleWindow::PlainTextColor = c;
		normalFormat.setForeground(c);
		update();
	}
	
	void CommandTextEdit::setOutputTextColor(const QColor& c)
	{
		messageFormat.setForeground( ConsoleWindow::OutputTextColor = c );
		update();
	}
	
	void CommandTextEdit::setErrorTextColor(const QColor& c)
	{
		errorFormat.setForeground( ConsoleWindow::ErrorTextColor = c );
		update();
	}

	void CommandTextEdit::setTableTextColor(const QColor& c)
	{
		tableHeaderFormat.setForeground( ConsoleWindow::TableTextColor = c );
		update();
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


	CommandTextEdit::CommandTextEdit(MainWindow * parent): QTextEdit(parent), c(0), mainWindow(parent)
	{
		setUndoRedoEnabled ( false );
		
		QSettings settings(GlobalSettings::ORGANIZATIONNAME, GlobalSettings::ORGANIZATIONNAME);

		settings.beginGroup("MainWindow");
		
		ConsoleWindow::BackgroundColor =  QColor(settings.value("ConsoleWindow::BackgroundColor", ConsoleWindow::BackgroundColor.name()).toString());
		ConsoleWindow::PlainTextColor =  QColor(settings.value("ConsoleWindow::PlainTextColor", ConsoleWindow::PlainTextColor.name()).toString());
		ConsoleWindow::ErrorTextColor =  QColor(settings.value("ConsoleWindow::ErrorTextColor",ConsoleWindow::ErrorTextColor.name()).toString());
		ConsoleWindow::OutputTextColor =  QColor(settings.value("ConsoleWindow::OutputTextColor",ConsoleWindow::OutputTextColor.name()).toString());
		ConsoleWindow::TableTextColor =  QColor(settings.value("ConsoleWindow::TableTextColor", ConsoleWindow::TableTextColor.name()).toString());
		
		settings.endGroup();

		setTextInteractionFlags(Qt::TextEditorInteraction);

		setCursorWidth(2);
		QFont font = this->font();
		font.setPointSize(12);
		setFont(font);

		setPalette(ConsoleWindow::BackgroundColor);

		QTextCursor cursor = textCursor();

		currentHistoryIndex = 0;
		frozen = false;

		//errorFormat.setFontWeight(QFont::Bold);
		errorFormat.setForeground(ConsoleWindow::ErrorTextColor);

		//messageFormat.setFontWeight(QFont::Bold);
		messageFormat.setForeground(ConsoleWindow::OutputTextColor);
		
		tableHeaderFormat.setFontWeight(QFont::Bold);
		tableHeaderFormat.setForeground(ConsoleWindow::TableTextColor);

		normalFormat.setFontWeight(QFont::Bold);
		normalFormat.setForeground(ConsoleWindow::PlainTextColor);

		cursor.setCharFormat(normalFormat);
		cursor.insertText(ConsoleWindow::Prompt);
		alreadyInsertedPrompt = true;
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
		if (s.isEmpty()) return;
		
		if (frozen)
		{
			errorsStack << s;
			return;
		}
		
		_lastError = s;
		
		QTextCursor cursor = textCursor();
		cursor.setPosition(currentPosition);

		cursor.setCharFormat(errorFormat);
		cursor.insertText(tr("Error: ") + s + tr("\n"));

		cursor.setCharFormat(normalFormat);
    	cursor.insertText(ConsoleWindow::Prompt);
    	alreadyInsertedPrompt = true;

		if (cursor.position() > currentPosition)
			currentPosition = cursor.position();
		this->ensureCursorVisible();
	}

	void CommandTextEdit::message(const QString& s)
	{
		if (s.isEmpty()) return;
				
		if (frozen)
		{
			messagesStack << s;
			return;
		}
		
		_lastOutput = s;
		
		QTextCursor cursor = textCursor();
		QTextDocument * doc = cursor.document();
		if (doc)
			cursor.setPosition(doc->characterCount());
		
		if (cursor.position() > currentPosition)
			currentPosition = cursor.position();

		cursor.setCharFormat(messageFormat);
		cursor.insertText(s + tr("\n"));

		cursor.setCharFormat(normalFormat);
    	cursor.insertText(ConsoleWindow::Prompt);
    	alreadyInsertedPrompt = true;

		if (cursor.position() > currentPosition)
			currentPosition = cursor.position();
		this->ensureCursorVisible();
	}

	void CommandTextEdit::clearText()
	{
		clear();

		QTextCursor cursor = textCursor();
		cursor.setCharFormat(normalFormat);
		cursor.insertText(ConsoleWindow::Prompt);
		alreadyInsertedPrompt = true;
		cursor.movePosition(QTextCursor::EndOfBlock);
		currentPosition = cursor.position();

		 if (cursor.position() < currentPosition)
            cursor.setPosition(currentPosition);
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
		QTextDocument * doc = document();
		if (doc)
			cursor.setPosition(doc->characterCount()-1);

		if (!frozen)
		{
			if (!messagesStack.isEmpty())
			{
				_lastOutput = messagesStack.last();
				cursor.setCharFormat(messageFormat);
				cursor.insertText(messagesStack.join(tr("\n")) + tr("\n"));
				alreadyInsertedPrompt = false;
				messagesStack.clear();
			}

			if (!errorsStack.isEmpty())
			{
				_lastError = errorsStack.last();
				cursor.setCharFormat(errorFormat);
				cursor.insertText(tr("Error: ") + errorsStack.join(tr("\n")) +  tr("\n"));
				alreadyInsertedPrompt = false;
				errorsStack.clear();
			}
		}

		if (this->frozen == frozen)
		{
			currentHistoryIndex = historyStack.size();
			this->ensureCursorVisible();
		}
		
		currentPosition = doc->characterCount();
		this->frozen = frozen;

		if (!frozen)		
		{
			if (!alreadyInsertedPrompt)
			{
				cursor.setCharFormat(normalFormat);
				cursor.insertText(ConsoleWindow::Prompt);
				alreadyInsertedPrompt = true;
			}
		}

		if (cursor.position() > currentPosition)
			currentPosition = cursor.position();

		currentHistoryIndex = historyStack.size();
		this->ensureCursorVisible();
	}

	void CommandTextEdit::eval(const QString& command)
	{
		QTextDocument * doc = document();
        if (!doc || frozen || command.isEmpty()) return;

	    QTextCursor cursor = textCursor();
        cursor.setCharFormat(normalFormat);

	    if (cursor.position() <= currentPosition)
			cursor.setPosition(currentPosition);

        cursor.movePosition(QTextCursor::EndOfBlock);
        cursor.insertText(command + tr("\n"));
        alreadyInsertedPrompt = false;
        currentPosition = cursor.position();
		if (historyStack.isEmpty() || command != historyStack.back())
        {
            historyStack << command;
            currentHistoryIndex = historyStack.size();
        }
        
        if (command.trimmed().toLower() == tr("clear"))
        	clearText();
		else
			if (mainWindow && QFile::exists(command.trimmed()))
				mainWindow->open(command.trimmed());
			else
				emit commandExecuted(command);
    
        if (!alreadyInsertedPrompt)
        {
            cursor.setCharFormat(normalFormat);
            cursor.insertText(ConsoleWindow::Prompt);
            alreadyInsertedPrompt = true;
        }
        
        if (cursor.position() > currentPosition)
            currentPosition = cursor.position();
        currentHistoryIndex = historyStack.size();
        this->ensureCursorVisible();
	}

	void CommandTextEdit::keyPressEvent ( QKeyEvent * event )
	{
		QTextDocument * doc = document();
		
		if (!event || !doc) return;
		
		currentPosition = doc->lastBlock().position() + ConsoleWindow::Prompt.size();

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
		
		if (event->modifiers() == Qt::ControlModifier)
		{
			if (key == Qt::Key_Equal || key == Qt::Key_Plus)
			{
				zoomIn();
				return;
			}
			if (key == Qt::Key_Minus)
			{
				zoomOut();
				return;
			}
		}

		if (key == Qt::Key_Return || key == Qt::Key_Enter)
		{
			if (frozen || cursor.position() < currentPosition) return;

			QString command = cursor.block().text().remove(0,ConsoleWindow::Prompt.size());			
			cursor.movePosition(QTextCursor::EndOfBlock);
			cursor.insertText(tr("\n"));
			alreadyInsertedPrompt = false;
			currentPosition = cursor.position();

			if (!command.isEmpty())
			{
				if (historyStack.isEmpty() || command != historyStack.back())
				{
					historyStack << command;
					currentHistoryIndex = historyStack.size();
				}
				
				if (command.trimmed().toLower() == tr("clear"))
	            	clearText();
	            else
				if (mainWindow && QFile::exists(command.trimmed()))
					mainWindow->open(command.trimmed());
				else
		            if (!printValue(cursor,command))
						emit commandExecuted(command);
			}

			if (!alreadyInsertedPrompt)
			{
				cursor.setCharFormat(normalFormat);
				cursor.insertText(ConsoleWindow::Prompt);
				alreadyInsertedPrompt = true;
				cursor.movePosition(QTextCursor::EndOfBlock);
				if (cursor.position() > currentPosition)
					currentPosition = cursor.position();
			}

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
				if (key == Qt::Key_Tab && mainWindow && mainWindow->currentNetwork())
				{
					QString text = cursor.block().text().remove(0,ConsoleWindow::Prompt.size());
					QStringList keys = mainWindow->currentNetwork()->symbolsTable.uniqueHandlesWithDot.keys();
					QStringList options;
					for (int i=0; i < keys.size(); ++i)
						if (keys[i].startsWith(text))
						{
							//cursor.insertText(keys[i].right(keys[i].size() - text.size()));
							options << keys[i];
						}
					if (options.isEmpty())
					{
						keys = mainWindow->currentNetwork()->symbolsTable.uniqueDataWithDot.keys();
						for (int i=0; i < keys.size(); ++i)
							if (keys[i].startsWith(text))
							{
								//cursor.insertText(keys[i].right(keys[i].size() - text.size()));
								options << keys[i];
							}
					}
					
					if (!options.isEmpty())
					{
						if (options.size() == 1)
						{
							cursor.insertText(options[0].right(options[0].size() - text.size()));
							alreadyInsertedPrompt = false;
						}
						else
						{
							cursor.setCharFormat(messageFormat);
							cursor.insertText(tr("\n") + options.join(tr("\n")) + tr("\n"));
							cursor.setCharFormat(normalFormat);
							cursor.insertText(ConsoleWindow::Prompt + text);
							alreadyInsertedPrompt = false;
						}
					}					
				}
				else
					if (frozen && event->modifiers() == Qt::ControlModifier && (key == Qt::Key_Z))
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
							bool hasModifier = false;//(event->modifiers() != Qt::NoModifier) && !ctrlOrShift;


							if (!isShortcut && (hasModifier || event->text().isEmpty() || completionPrefix.length() < 3))
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
		: Tool(tr("Console Window")), commandTextEdit(main), _interpreter(0)
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

		/*if (!s.isEmpty())
		{
			if (parentWidget())
				parentWidget()->show();
			else
				show();
		}*/
	}

	void ConsoleWindow::printTable(const DataTable<qreal>& table)
	{
		QString outputs;

		QStringList colnames = table.columnNames(), rownames = table.rowNames();

		outputs += tr("\n");
		for (int i=0; i < colnames.size(); ++i)
		{
			outputs += tr("\t") + colnames.at(i);
		}
		outputs += tr("\n");
		for (int i=0; i < table.rows(); ++i)
		{
			outputs += rownames.at(i);
			for (int j=0; j < table.columns(); ++j)
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
	
	bool CommandTextEdit::printValue(QTextCursor& cursor, const QString& s)
	{
		if (s.isEmpty() || !mainWindow) return false;
		
		NetworkHandle * network = mainWindow->currentNetwork();
		
		if (!network) return false;
		
		QList<ItemHandle*> list = network->findItem(s);		
		ItemHandle * h = 0;
		
		if (!list.isEmpty()) h = list[0];
		
		if (h)
		{
			printHandleSummary(cursor,h);
			return true;
		}
		
		QList< QPair<ItemHandle*,QString> > pairs = network->findData(s);
		
		if (pairs.isEmpty() || !pairs[0].first || pairs[0].second.isEmpty()) return false;
		
		h = pairs[0].first;
		QString id = pairs[0].second;
		
		cursor.setCharFormat(messageFormat);
		if (h->hasTextData(id))
		{
			QString s2 = s;
			s2.remove(h->fullName(".") + tr("."));
			s2.remove(h->fullName("_") + tr("_"));
			if (h->textDataTable(id).rowNames().contains(s2))
			{
				cursor.insertText(h->textData(id,s2) + tr("\n"));
				cursor.setCharFormat(normalFormat);
				cursor.insertText(ConsoleWindow::Prompt);
				alreadyInsertedPrompt = true;
				return true;
			}
		}
		
		if (h->hasNumericalData(id))
		{
			QString s2 = s;
			s2.remove(h->fullName(".") + tr("."));
			s2.remove(h->fullName("_") + tr("_"));
			if (h->numericalDataTable(id).rowNames().contains(s2))
			{
				cursor.insertText(QString::number(h->numericalData(id,s2)) + tr("\n"));
				cursor.setCharFormat(normalFormat);
				cursor.insertText(ConsoleWindow::Prompt);
				alreadyInsertedPrompt = true;
				return true;
			}
		}
		return false;
	}
	
	void CommandTextEdit::printHandleSummary(QTextCursor& cursor, ItemHandle * h)
	{
		if (!h) return;
		
		QString s;
		
		if (h->family())
		{
			cursor.setCharFormat(tableHeaderFormat);
			cursor.insertText(tr("family: "));
			cursor.setCharFormat(messageFormat);
			cursor.insertText(h->family()->name() + tr("\n"));
		}

		QList<QString> keys = h->numericalDataNames();
		for (int i=0; i < keys.size(); ++i)
		{
			NumericalDataTable & dat = h->numericalDataTable( keys[i] );
			if (dat.rows() > 0 && dat.columns() > 0)
			{
				int maxsz = 0;
				for (int j=0; j < dat.rows(); ++j)
					if (dat.rowName(j).size() > maxsz)
						maxsz = dat.rowName(j).size();
				s = QString("").leftJustified(maxsz+1);
				
				cursor.setCharFormat(tableHeaderFormat);
				cursor.insertText(keys[i] + tr(":\n"));
				
				cursor.setCharFormat(messageFormat);
				for (int k=0; k < dat.columns(); ++k)
					s += tr("    ") + dat.columnName(k);
				s += tr("\n");
				for (int j=0; j < dat.rows(); ++j)
				{
					s += dat.rowName(j) + tr(":    ");
					
					for (int k=0; k < dat.columns(); ++k)
						s += QString::number(dat.at(j,k)) + tr("    ");
					s += tr("\n");
				}
				cursor.insertText(s);
			}
		}
		
		keys = h->textDataNames();
		for (int i=0; i < keys.size(); ++i)
		{
			TextDataTable & dat = h->textDataTable( keys[i] );
			if (dat.rows() > 0 && dat.columns() > 0)
			{
				int maxsz = 0;
				for (int j=0; j < dat.rows(); ++j)
					if (dat.rowName(j).size() > maxsz)
						maxsz = dat.rowName(j).size();
				s = QString("").leftJustified(maxsz+1);
				
				cursor.setCharFormat(tableHeaderFormat);
				cursor.insertText(keys[i] + tr(":\n"));
				
				cursor.setCharFormat(messageFormat);
				for (int k=0; k < dat.columns(); ++k)
					s += tr("    ") + dat.columnName(k);
				s += tr("\n");
				for (int j=0; j < dat.rows(); ++j)
				{
					s += dat.rowName(j) + tr(":    ");
					
					for (int k=0; k < dat.columns(); ++k)
						s += (dat.at(j,k)) + tr("    ");
					s += tr("\n");
				}
				cursor.insertText(s);
			}
		}

		cursor.setCharFormat(normalFormat);
		cursor.insertText(ConsoleWindow::Prompt);
		alreadyInsertedPrompt = true;
	}
	
	InterpreterThread * ConsoleWindow::interpreter() const
	{
		return _interpreter;
	}
	
	void ConsoleWindow::setInterpreter(InterpreterThread * newInterpreter)
	{
		if (_interpreter)
		{
			disconnect(this,SIGNAL(commandExecuted(const QString&)),_interpreter,SLOT(exec(const QString&)));
			disconnect(this,SIGNAL(commandInterrupted()),_interpreter,SLOT(terminate()));					
			disconnect(_interpreter,SIGNAL(started()),this->editor(),SLOT(freeze()));
			disconnect(_interpreter,SIGNAL(finished()),this->editor(),SLOT(unfreeze()));
			disconnect(_interpreter,SIGNAL(terminated()),this->editor(),SLOT(unfreeze()));
		}
		
		_interpreter = newInterpreter;
		
		if (_interpreter)
		{
			connect(this,SIGNAL(commandExecuted(const QString&)),_interpreter,SLOT(exec(const QString&)));
			connect(this,SIGNAL(commandInterrupted()),_interpreter,SLOT(terminate()));					
			connect(_interpreter,SIGNAL(started()),this->editor(),SLOT(freeze()));
			connect(_interpreter,SIGNAL(finished()),this->editor(),SLOT(unfreeze()));
			connect(_interpreter,SIGNAL(terminated()),this->editor(),SLOT(unfreeze()));
		}
	}

	QString CommandTextEdit::lastError() const
	{
		return _lastError;
	}

	QString CommandTextEdit::lastMessage() const
	{
		return _lastOutput;
	}

	QString ConsoleWindow::lastError() const
	{
		return commandTextEdit._lastError;
	}

	QString ConsoleWindow::lastMessage() const
	{
		return commandTextEdit._lastOutput;
	}
}

