/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 Provides a text window where C code can be written and run dynamically

****************************************************************************/

#ifndef TINKERCELL_CSCRIPT_H
#define TINKERCELL_CSCRIPT_H

#include <QPair>
#include <QMainWindow>
#include <QTextEdit>
#include <QSyntaxHighlighter>
#include <QMenu>
#include <QTextCharFormat>
#include <QDialog>
#include <QLabel>
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
#include <QPushButton>
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
		static QString prefix;

	signals:
		void insertText(const QString&);

	protected:
		virtual void readCHeaders(const QString& dirName);
		virtual void mouseDoubleClickEvent ( QMouseEvent * event );
		virtual void keyPressEvent ( QKeyEvent * event );
	};

	class TINKERCELLEXPORT CodingWindow : public Tool
	{
	    Q_OBJECT

	public:
		CodingWindow();
		bool setMainWindow(MainWindow*);
		static bool DO_SVN_UPDATE;

	signals:
		void runPython(const QString&);
		void loadPyFromDir( QDir& );

		void runRuby(const QString&);
		void loadRubyFromDir( QDir& );
		
		void runOctave(const QString&);
		void loadOctFromDir( QDir& );
		
		//void stop();
		
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
		void enableRuby(bool b=true);
        void enableOctave(bool b=true);
	    void enableC(bool b=true);
		void toolLoaded(Tool *);

	protected slots:
		void convertCodeToButton();

	protected:
		void setupEditor();
		void setupMenu();
		QPair<QString,QString> requestLoginInfo();
		void runC(const QString&);
		void convertCodeToButtonOctave();
		void convertCodeToButtonPy();
		void convertCodeToButtonRuby();
		void convertCodeToButtonC();

		QTimeLine timer;
		QMainWindow * window;

		enum Languages { None, C, Octave, Python, Ruby};
		Languages selectedLanguage;

		QRadioButton * cButton, * octaveButton, *pythonButton, *rubyButton;
		QAction * cAction, * octaveAction, *pythonAction, *rubyAction; 
		RuntimeCodeEditor *editor;
		QWidget * editorWidget;
		QToolBar * toolBar;

		QLineEdit *commandCEdit, *commandPyEdit, *fileNameEdit;
		CodingWindowSyntaxHighlighter *highlighter;

		QString fileName;
		QDialog * usernameDialog;
		QLineEdit * usernameLine, *passwordLine;

	};

}

#endif
