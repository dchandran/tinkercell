/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This file defines the class that is used as a general output area as well as
 a generic command prompt (e.g. by Python plugin)


****************************************************************************/

#include "MainWindow.h"
#include "OutputWindow.h"

namespace Tinkercell
{

	CommandTextEdit::CommandTextEdit(QWidget * parent): QTextEdit(parent)
	{
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
			QTextEdit::keyPressEvent(event);
		}

		if (cursor.position() < currentPosition)
			cursor.setPosition(currentPosition);
		this->ensureCursorVisible();
	}

	/***********************************
	    OUTPUT WINDOW
	************************************/

	OutputWindow * OutputWindow::instance = 0;

	OutputWindow::OutputWindow(MainWindow * main)
		: Tool(tr("Output Window"))
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
	void OutputWindow::message(const QString& s)
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

	void OutputWindow::error(const QString& s)
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

	void OutputWindow::printTable(const DataTable<qreal>& table)
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

	void OutputWindow::freeze()
	{
		if (!instance) return;
		instance->commandTextEdit.freeze();
	}

	void OutputWindow::unfreeze()
	{
		if (!instance) return;
		instance->commandTextEdit.unfreeze();
	}

	void OutputWindow::clear()
	{
		if (!instance) return;
		instance->commandTextEdit.clearText();
	}

	CommandTextEdit * OutputWindow::commandWindow()
	{
		return &commandTextEdit;
	}

}
