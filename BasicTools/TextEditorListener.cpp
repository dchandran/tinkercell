/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool allows insertion of parts from the NodesTree

****************************************************************************/

#include <QtDebug>
#include "ItemFamily.h"
#include "GraphicsScene.h"
#include "NetworkWindow.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "TextEditorListener.h"
#include "TextEditorParser.h"

namespace Tinkercell
{

    TextEditorListener::TextEditorListener() : Tool("Default Text Parser")
    {

    }

    bool TextEditorListener::setMainWindow(MainWindow * main)
    {
        Tool::setMainWindow(main);
        if (main)
        {
            connect(main,SIGNAL(windowOpened(NetworkWindow*)),
                    this,SLOT(newWindow(NetworkWindow *)));
            return true;
        }
        return false;
    }

    void TextEditorListener::newWindow(NetworkWindow * network)
    {
        if (network->textEditor)
            new TextEditorParser(network->textEditor);
    }

    QColor TextEditorListener::CommentsBackgroundColor(QString("#FFFFFF"));
    QColor TextEditorListener::CommentsTextColor(QString("#2E369D"));
    QColor TextEditorListener::NodesBackgroundColor(QString("#168311"));
    QColor TextEditorListener::NodesTextColor(QString("#FCFCDA"));
    QColor TextEditorListener::ConnectionsBackgroundColor(QString("#004871"));
    QColor TextEditorListener::ConnectionsTextColor(QString("#FFFFFF"));
    QColor TextEditorListener::OpBackgroundColor(QString("#A36F0C"));
    QColor TextEditorListener::OpTextColor(QString("#FFFFFF"));

    QString TextEditorListener::ConnectionDescriptionSeparator(";");
    QString TextEditorListener::MultipleNodeSeparator("+");
    bool TextEditorListener::MultipleNodesAllowed = true;
    bool TextEditorListener::ConnectionDescriptionAllowed = true;
    bool TextEditorListener::ShowNodeDeclarations = true;
    bool TextEditorListener::ShowOperations = true;

}


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
    if (!main) return;

    Tinkercell::TextEditorListener * textlistener = new Tinkercell::TextEditorListener;
    main->addTool(textlistener);

}

