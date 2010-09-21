/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 Provides a text window where C code can be written and run dynamically

****************************************************************************/

#ifndef TINKERCELL_CSCRIPT_H
#define TINKERCELL_CSCRIPT_H

#include <QMainWindow>
#include <QTextEdit>
#include <QSyntaxHighlighter>
#include <QMenu>
#include <QTextCharFormat>
#include <QDialog>
#include <QCompleter>
#include <QListWidget>
#include <QTabWidget>
#include <QTreeWidget>
#include <QThread>
#include <QToolBar>
#include <QTimeLine>
#include <QAction>
#include <QActionGroup>
#include <QLineEdit>
#include <QHash>
#include <QRadioButton>
#include "CodeEditor.h"
#include "Tool.h"
#include "ConsoleWindow.h"
#include "CThread.h"
#include "SyntaxHighlighter.h"

namespace Tinkercell
{
	class RuntimeCodeEditor : public CodeEditor
	{
		Q_OBJECT
	public:
		QCompleter completer;
		QHash<QString,QString> defaultSavedFilename;

	public slots:
		void open(const QString&);
		void save(const QString&);
		void saveAs(const QString&);
		void insertCompletion(const QString& completion);
	};

	class TCFunctionsListView : public QTreeWidget
	{
		Q_OBJECT
	public:
		TCFunctionsListView(MainWindow *, const QString&, CodeEditor * textEdit);
		ConsoleWindow * console;

	signals:
		void insertText(const QString&);

	protected:
		virtual void readCHeaders(const QString& dirName);
		virtual void mouseDoubleClickEvent ( QMouseEvent * event );
		virtual void keyPressEvent ( QKeyEvent * event );
	};

	class CodingWindow : public Tool
	{
	    Q_OBJECT

	public:
		CodingWindow();
		bool setMainWindow(MainWindow*);
		static bool DO_SVN_UPDATE;

	signals:
		void runPython(const QString&);
		void loadPyFromDir( QDir& );
		
		void runOctave(const QString&);
		void loadOctFromDir( QDir& );
		
		void stopPython();
		void stopOctave();
		
		void compileBuildLoadC(const QString&,const QString&,const QString&);
		void compile(const QString&, QString&);

	public slots:
		void run();
		void about();
		void toggleSVNupdate(bool);
		virtual QSize sizeHint() const;
		void newDoc();
		void open();
		void save();
		void undo();
		void redo();
		void selectAll();
		virtual void setVisible(bool);
		void pyscesHelp();
		void enablePython(bool b=true);
        void enableOctave(bool b=true);
        void enableC(bool b=true);

	protected slots:
		void convertCodeToButton();

	protected:
		void setupEditor();
		void setupMenu();
		void runC(const QString&);
		void convertCodeToButtonOctave();
		void convertCodeToButtonPy();
		void convertCodeToButtonC();
		void disablePython();
		void disableOctave();

        QProgressBar * progressBar;
		QTimeLine timer;
		QMainWindow * window;

        enum Languages { C, Octave, Python };
        Languages selectedLanguage;

		QRadioButton * cButton, * octaveButton, *pythonButton;
		QAction * cAction, * octaveAction, *pythonAction; 
		RuntimeCodeEditor *editor;
		QWidget * editorWidget;
		QToolBar * toolBar;

		QLineEdit *commandCEdit, *commandPyEdit, *fileNameEdit;
		CandPythonSyntaxHighlighter *highlighter;

		QString fileName;
		QDialog commandDialog;

	};

}

#endif
