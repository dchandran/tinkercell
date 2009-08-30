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
#include <QMainWindow>
#include <QTextEdit>
#include <QCompleter>
#include <QSyntaxHighlighter>
#include <QRegExp>
#include <QCompleter>
#include "DataTable.h"
#include "Tool.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{
	class MainWindow;

	/*! \brief A command-line type text box that other tools can use for scripting interface
	\ingroup io
	*/
	MY_EXPORT class CommandTextEdit : public QTextEdit
	{
		Q_OBJECT;
	public:

		/*! \brief default constructor*/
		CommandTextEdit(QWidget * parent=0);
		/*! \brief  Whether or not this console in the frozen state. The text box will not add or remove text while it is frozen*/	
		bool isFrozen();
		/*! \brief set code completion*/ 
		void setCompleter(QCompleter *c);
		/*! \brief code completion*/ 
		QCompleter *completer() const;

signals:

		/*! \brief the user requested to execute the given command*/
		void commandExecuted(const QString& command);
		/*! \brief the user requested to interrupt the current process*/
		void commandInterrupted();

	public slots:

		/*! \brief post an error message to this console text box*/
		void error(const QString&);
		/*! \brief post a message to this console text box*/
		void message(const QString&);
		/*! \brief clear all text*/
		void clearText();
		/*! \brief equivalent to setFreeze(true)*/
		void freeze();
		/*! \brief equivalent to setFreeze(false)*/
		void unfreeze();
		/*! \brief Set frozen state. The text box will not respond to user inputs while it is frozen*/
		void setFreeze (bool frozen=true);

	protected:

		/*! \brief list of previously executed commands*/
		QStringList historyStack;
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
		/*! \brief font format for user inputs*/
		QTextCharFormat normalFormat;
		/*! \brief manages the console-type interface, where the user is not allowed to type outside the >> */
		virtual void keyPressEvent ( QKeyEvent * event );
		/*! \brief zoom in or out using mouse wheel*/
		virtual void wheelEvent ( QWheelEvent * wheelEvent );
		/*! \brief focus returned from code completer*/
		void focusInEvent(QFocusEvent *e);

	private slots:
		/*! \brief complete the current line*/
		void insertCompletion(const QString &completion);

	private:
		/*! \brief text under the cursor*/
		QString textUnderCursor() const;
		/*! \brief used for code completion*/
		QCompleter *c;
	};

	/*! \brief Used to create an output window that can display outputs
	\ingroup io
	*/
	MY_EXPORT class ConsoleWindow : public Tool
	{
		Q_OBJECT

	public:
		
		/*! \brief the string used at the prompt*/
		static QString Prompt;

		/*! \brief constructor -- initialize main window*/
		ConsoleWindow(MainWindow * main = 0);
		/*! \brief print a message in the output window*/
		static void message(const QString&);
		/*! \brief print an error message in the output window*/
		static void error(const QString&);
		/*! \brief print a data table (tab-delimited) in the output window*/
		static void printTable(const DataTable<qreal>& dataTable);
		/*! \brief clear the output window*/
		static void clear();
		/*! \brief freeze the output window. Frozen window will not be responsive to commands*/
		static void freeze();
		/*! \brief unfreeze the output window. Frozen window will not be responsive to commands*/
		static void unfreeze();
		/*! \brief the global command window*/
		static ConsoleWindow * outputWindow();
		/*! \brief the command window's editor*/
		CommandTextEdit * outputWindowEditor();

	signals:
		/*! \brief the user requested to execute the given command*/
		void commandExecuted(const QString& command);
		/*! \brief the user requested to interrupt the current process*/
		void commandInterrupted();

	protected:
		/*! \brief store pointer to the main window's output window*/
		static ConsoleWindow * instance;
		/*! \brief the command window*/
		CommandTextEdit commandTextEdit;

	};
}

#endif
