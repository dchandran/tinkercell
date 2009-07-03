/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 Provides a text window where C code can be written and run dynamically

****************************************************************************/

#ifndef TINKERCELL_ANTIOMNYEDITOR_H
#define TINKERCELL_ANTIOMNYEDITOR_H

#include <QMainWindow>
#include <QTextEdit>
#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>
#include <QDialog>
#include <QCompleter>
#include <QListWidget>
#include <QTabWidget>
#include <QTreeWidget>
#include <QThread>
#include <QToolBar>
#include <QTimeLine>
#include <QActionGroup>
#include <QLineEdit>
#include <QHash>
#include "Tool.h"
#include "CThread.h"
#include "AntimonySyntaxHighlighter.h"

namespace Tinkercell
{
	class AntimonyEditor : public Tool
	{
	    Q_OBJECT

	public:
		AntimonyEditor();
    ~AntimonyEditor() {}

		bool setMainWindow(MainWindow*);

	signals:
		void runPyFile(const QString&);
		void runPyString(const QString&);

	public slots:
		void makeNew();
		void update();
		void commit();
		void pluginLoaded(const QString&);

	protected:
		AntimonySyntaxHighlighter * highlighter;
		QTextEdit *editor;
		QDockWidget * dockWidget;

		void leaveEvent ( QEvent * event );

		void enterEvent ( QEvent * event );

	};

}

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif
