/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
The parent class for all text parsers. TextEditor requires at least one
 text parser; otherwise it does no function.
 
****************************************************************************/

#ifndef TINKERCELL_TEXTPARSER_FOR_TEXTEDITOR_H
#define TINKERCELL_TEXTPARSER_FOR_TEXTEDITOR_H

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
#include "TextEditor.h"
#include "Tool.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{
	/*! \brief TextParser is the parent class for all parsers. Parsers
		are classes that interpret the string in a TextEditor and insert
		items or modify items as needed. TinkerCell can support multiple parsers
		through the use of the TextParser interface. 
		\ingroup core
	*/
	class MY_EXPORT TextParser : public Tool
	{
	    Q_OBJECT
		
	public:
		
		/*! \brief constructor
			\param QString name
			\param QWidget* parent*/
		TextParser(const QString& Name, QWidget * parent = 0);
		
		/*! \brief icon for this class*/
		QPixmap icon;
		
	public slots:
		/*! \brief set this parser as the current parser*/
		virtual void activate();
		/*! \brief this parser is no longer the current parser*/
		virtual void deactivate();
		/*! \brief this parser has been requested to parse the text inside the given text editor
            \param TextEditor* the text editor
        */
		virtual void parse(TextEditor*);
		/*! \brief some text inside this editor has been changed
            \param TextEditor* the current editor
			\param QString old text
            \param QString new text
        */
        virtual void textChanged(TextEditor *, const QString&, const QString&, const QString&);
        /*! \brief the cursor has moved to a different line
            \param int index of the current line
            \param QString current line text
        */
        virtual void lineChanged(TextEditor *, int, const QString&);
	signals:
		/*! \brief invalid syntax*/
		void validSyntax(bool);
	
	public:
		/*!
		* \brief set the text parser for all text editors. The current text parser can be obtained
			using TextParser::currentParser();
		*/
		static void setParser(TextParser*);
		/*!
		* \brief The current text parser that is being used (can be 0 if none)
		*/
		static TextParser* currentParser();
	
	private:
		/*!
		* \brief The current text parser that is being used (can be 0 if none)
		*/
		static TextParser * _parser;
	
	};

}
#endif
