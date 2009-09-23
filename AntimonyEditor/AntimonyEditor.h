/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Provides a text window where C code can be written and run dynamically
 
****************************************************************************/

#ifndef TINKERCELL_ANTIOMNYEDITOR_H
#define TINKERCELL_ANTIOMNYEDITOR_H

#include <QList>

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
#include <QTextEdit>
#include <QThread>
#include <QToolBar>
#include <QTimeLine>
#include <QActionGroup>
#include <QLineEdit>
#include <QHash>
#include "NetworkWindow.h"
#include "TextEditor.h"
#include "ItemHandle.h"
#include "TextParser.h"
#include "AntimonySyntaxHighlighter.h"

namespace Tinkercell
{
	class AntimonyEditor : public TextParser
	{
	    Q_OBJECT

	public:
		AntimonyEditor();
		/*! \brief make necessary signal/slot connections*/
		bool setMainWindow(MainWindow*);
		/*! \brief parse text and convert them to items*/
		QList<TextItem*> parse(const QString& modelString, ItemHandle * mainItem = 0);

	public slots:
		/*! \brief parse text and insert items*/
		void parse(TextEditor * editor);
		
		/*! \brief parse text and insert items*/
		void parse();
		
		/*! \brief some text inside this editor has been changed
            \param QString old text
            \param QString new text
        */
        void textChanged(TextEditor *, const QString&, const QString&, const QString&);
        /*! \brief the cursor has moved to a different line
            \param int index of the current line
            \param QString current line text
        */
        void lineChanged(TextEditor *, int, const QString&);
		/*!
        * \brief if text editor is opened, sets its syntax highlighter
        * \param NetworkWindow* the current new window
        * \return void
        */
        void windowOpened(NetworkWindow*);
		/**/
		void insertModule();
	signals:
		/*! \brief invalid syntax*/
		void validSyntax(bool);
	protected:
		/*! \brief clone given items
			\param QList<TextItem*> items to clone
		*/
		static QList<TextItem*> clone(const QList<TextItem*>&);
		
		CodeEditor * scriptDisplayWindow;
		
		static QString getAntimonyScript(ItemHandle *);
		
	private slots:
		/*! \brief display antimony script when a module info is being displayed (see modelSummaryTool)        */
		void displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&);
		/*! \brief used to connect to modelSummaryTool*/
		void toolLoaded(Tool*);

	};

}

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif
