/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines the output window, which is a text area where other tools can post messages or use as a
scripting interface.


****************************************************************************/


#ifndef TINKERCELL_GENERICCOMMANDANDOUTPUTWINDOW_H
#define TINKERCELL_GENERICCOMMANDANDOUTPUTWINDOW_H

#include <QtGui>
#include <QColor>
#include <QMainWindow>
#include <QTextCursor>
#include <QTextEdit>
#include <QCompleter>
#include <QSyntaxHighlighter>
#include <QRegExp>
#include <QCompleter>
#include "DataTable.h"
#include "Tool.h"
#include "InterpreterThread.h"

#ifdef Q_WS_WIN
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{
	class MainWindow;
	class ConsoleWindow;
	/*! \brief A command-line type text box that other tools can use for scripting interface
	\ingroup io
	*/
	class TINKERCELLEXPORT CommandTextEdit : public QTextEdit
	{
		Q_OBJECT;
	public:

		/*! \brief default constructor*/
		CommandTextEdit(MainWindow * parent=0);
		/*! \brief  Whether or not this console in the frozen state. The text box will not add or remove text while it is frozen*/
		virtual bool isFrozen();
		/*! \brief set code completion*/
		void setCompleter(QCompleter *c);
		/*! \brief code completion*/
		QCompleter *completer() const;
		/*! \brief last error message*/
		virtual QString lastError() const;
		/*! \brief last message*/
		virtual QString lastMessage() const;

	signals:

		/*! \brief the user requested to execute the given command*/
		void commandExecuted(const QString& command);
		/*! \brief the user requested to interrupt the current process*/
		void commandInterrupted();

	public slots:

        /*! \brief evaluate a command (just emits a commandExecuted signal)*/
		virtual void eval(const QString&);
		/*! \brief post an error message to this console text box*/
		virtual void error(const QString&);
		/*! \brief post a message to this console text box*/
		virtual void message(const QString&);
		/*! \brief clear all text*/
		virtual void clearText();
		/*! \brief equivalent to setFreeze(true)*/
		virtual void freeze();
		/*! \brief equivalent to setFreeze(false)*/
		virtual void unfreeze();
		/*! \brief Set frozen state. The text box will not respond to user inputs while it is frozen*/
		virtual void setFreeze (bool frozen=true);
		/*! \brief set background color*/
		virtual void setBackgroundColor(const QColor&);
		/*! \brief set plain text color*/
		virtual void setPlainTextColor(const QColor&);
		/*! \brief set output message color*/
		virtual void setOutputTextColor(const QColor&);
		/*! \brief set error message color*/
		virtual void setErrorTextColor(const QColor&);
		/*! \brief set table headers color*/
		virtual void setTableTextColor(const QColor&);

	protected:

		/*! \brief list of previously executed commands*/
		QStringList historyStack;
		/*! \brief list of messages pending*/
		QStringList messagesStack;
		/*! \brief list of errors pending*/
		QStringList errorsStack;
		/*! \brief last error message*/
		QString _lastError;
		/*! \brief last output message*/
		QString _lastOutput;
		/*! \brief current position in the history of commands*/
		int currentHistoryIndex;
		/*! \brief current position of the cursor in the text box*/
		int currentPosition;
		/*! \brief frozen state = 0 or 1*/
		bool frozen;
		/*! \brief font format for error messages*/
		QTextCharFormat errorFormat;
		/*! \brief font format for regular messages*/
		QTextCharFormat messageFormat;
		/*! \brief font format for table headers*/
		QTextCharFormat tableHeaderFormat;
		/*! \brief font format for user inputs*/
		QTextCharFormat normalFormat;
		/*! \brief manages the console-type interface, where the user is not allowed to type outside the >> */
		virtual void keyPressEvent ( QKeyEvent * event );
		/*! \brief zoom in or out using mouse wheel*/
		virtual void wheelEvent ( QWheelEvent * wheelEvent );
		/*! \brief focus returned from code completer*/
		virtual void focusInEvent(QFocusEvent *e);

	private slots:
		/*! \brief complete the current line*/
		void insertCompletion(const QString &completion);

	private:
		/*! \brief text under the cursor*/
		QString textUnderCursor() const;
		/*! \brief used for auto completion*/
		QCompleter *c;
		/*! \brief used for auto completion*/
		MainWindow * mainWindow;
		/*! \brief used for auto completion*/
		bool printValue(QTextCursor&, const QString&);
		/*! \brief used for auto completion*/
		void printHandleSummary(QTextCursor&, ItemHandle *);
		/*! \brief used to insert the prompt in the confusion of multiple threads*/
		bool alreadyInsertedPrompt;

		friend class ConsoleWindow;
	};

	/*! \brief Used to create an output window that can display outputs
	\ingroup io
	*/
	class TINKERCELLEXPORT ConsoleWindow : public Tool
	{
		Q_OBJECT

	public:

		/*! \brief the string used at the prompt*/
		static QString Prompt;
		
		/*! \brief the background color for console*/
		static QColor BackgroundColor;
		
		/*! \brief the font color for plain text*/
		static QColor PlainTextColor;
		
		/*! \brief the font color for error messages*/
		static QColor ErrorTextColor;
		
		/*! \brief the font color for outputs*/
		static QColor OutputTextColor;
		
		/*! \brief the font color for table headers*/
		static QColor TableTextColor;

		/*! \brief constructor -- initialize main window*/
		ConsoleWindow(MainWindow * main = 0);

		/*! \brief the command window's editor*/
		virtual CommandTextEdit * editor();
		
		/*! \brief set the interpreter for the console window, e.g. new PythonInterpreterThread*/
		virtual void setInterpreter(InterpreterThread * );
		
		/*! \brief get the interpreter for the console window*/
		InterpreterThread * interpreter() const;

		/*! \brief last error message*/
		virtual QString lastError() const;
		
		/*! \brief last message*/
		virtual QString lastMessage() const;

    public slots:
		/*! \brief send a command to the console window to be evaluated*/
		virtual void eval(const QString&);
		/*! \brief print a message in the output window*/
		virtual void message(const QString&);
		/*! \brief print an error message in the output window*/
		virtual void error(const QString&);
		/*! \brief print a data table (tab-delimited) in the output window*/
		virtual void printTable(const DataTable<qreal>& dataTable);
		/*! \brief clear the output window*/
		virtual void clear();
		/*! \brief freeze the output window. Frozen window will not be responsive to commands*/
		virtual void freeze();
		/*! \brief unfreeze the output window. Frozen window will not be responsive to commands*/
		virtual void unfreeze();

	signals:
		/*! \brief the user requested to execute the given command*/
		void commandExecuted(const QString& command);
		/*! \brief the user requested to interrupt the current process*/
		void commandInterrupted();

	protected:
		/*! \brief the command window*/
		CommandTextEdit commandTextEdit;
		/*! \brief the optional interpreter for processing commands*/
		InterpreterThread * _interpreter;

	};
}

#endif
