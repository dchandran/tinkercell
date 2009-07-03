/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This tool allows insertion of parts from the NodesTree

****************************************************************************/

#ifndef TINKERCELL_TEXTEDITORLISTENERTOOL_H
#define TINKERCELL_TEXTEDITORLISTENERTOOL_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
#include <QPixmap>
#include <QString>
#include <QtDebug>
#include <QAction>
#include <QFile>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QHash>
#include <QRegExp>

#include "TextItem.h"
#include "TextEditor.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "ConnectionsTree.h"
#include "NodesTree.h"
#include "texteditorsyntaxhighlighter.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

    /*! \brief This class is required for providing proper responses from the TextEditor.
       If this class is not loaded, another must be loaded in order for the TextEditor to be
       operable.
        \ingroup plugins
     */
    class MY_EXPORT TextEditorListener : public Tool
    {
        Q_OBJECT

    public:
        /*! \brief background color*/
        static QColor CommentsBackgroundColor;
        /*! \brief plain text color*/
        static QColor CommentsTextColor;
        /*! \brief declarations background color*/
        static QColor NodesBackgroundColor;
        /*! \brief declarations text color*/
        static QColor NodesTextColor;
        /*! \brief connections background color*/
        static QColor ConnectionsBackgroundColor;
        /*! \brief connections text color*/
        static QColor ConnectionsTextColor;
        /*! \brief operations background color*/
        static QColor OpBackgroundColor;
        /*! \brief operations text color*/
        static QColor OpTextColor;

        /*! \brief multiple node connections allowed? Otherwise, all connections will be between exactly two nodes*/
        static bool MultipleNodesAllowed;
        /*! \brief syntax to use when multiple node connections are allowed; default is "+"*/
        static QString MultipleNodeSeparator;
        /*! \brief description text on connections allowed? */
        static bool ConnectionDescriptionAllowed;
        /*! \brief syntax for inserting descriptions on connections ; default is ";"*/
        static QString ConnectionDescriptionSeparator;
        /*! \brief whether or not to show the node types at the top of the text; default = true*/
        static bool ShowNodeDeclarations;
        /*! \brief whether or not to show the operations at the top of the text; default = true*/
        static bool ShowOperations;

        /* !\brief default constructor. */
        TextEditorListener();

        /* !\brief load this plug-in into the TinkerCell main window
        \param MainWindow* TinkerCell's main window*/
        bool setMainWindow(MainWindow * main);

    public slots:
        void newWindow(NetworkWindow * network);

    };

}


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif
