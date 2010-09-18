/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 A tool that allows users to construct models using Antimony scripts in the TextEditor

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
#include <QSemaphore>
#include "NetworkHandle.h"
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
		QList<ItemHandle*> parse(const QString& modelString, ItemHandle * parentHandle=0);

		static QString getAntimonyScript(const QList<ItemHandle*>&);

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
		* \brief signals whenever the current window changes
		* \param NetworkWindow* the previous windpw
		* \param NetworkWindow* the current new window
		* \return void
		*/
		void windowChanged(NetworkWindow*,NetworkWindow*);
		/*!
        * \brief insert module(s) in the scene
        */
		//void insertModule();
		/*!
        * \brief copy the antimony script of items as they are copied
        */
		void copyItems(GraphicsScene * scene, QList<QGraphicsItem*>& , QList<ItemHandle*>& );
		/*!
		* \brief load antimony scipt into the list of items
		* \param QList<ItemHandle*>& list of items inside the file
		* \param QString& file that is selected by user
		* \return void
		*/
		void getItemsFromFile(QList<ItemHandle*>&, const QString& filename);

	signals:
		/*! \brief invalid syntax*/
		void validSyntax(bool);

	protected:
		/*! \brief clone given items
			\param QList<TextItem*> items to clone
		*/
		static QList<ItemHandle*> clone(const QList<ItemHandle*>&);

		CodeEditor * scriptDisplayWindow;

	private slots:
		/*! \brief display antimony script when a module info is being displayed (see modelSummaryTool)        */
		//void displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&);
		/*! \brief makes a new text window with the script representing the given items*/
		void getTextVersion(const QList<ItemHandle*>&, QString*);
		/*! \brief used to connect to modelSummaryTool*/
		void toolLoaded(Tool*);

	private:
		void connectTCFunctions();
		static void appendScript(QString&, const QList<ItemHandle*>&);
		QHash<NetworkWindow*,bool> visitedWindows;
	};

}

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif
