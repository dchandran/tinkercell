#ifndef TINKERCELL_TEXTEDITORPARSER_H
#define TINKERCELL_TEXTEDITORPARSER_H

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

namespace Tinkercell
{

    class TextEditorParser : public QObject
    {
        Q_OBJECT

    public:
        TextEditorParser(TextEditor *);

    public slots:
        /* !\brief text has changed
        \param QString old text
        \param QString new text
        */
        virtual void textChanged(const QString&,const QString&);
        /* !\brief the current line has changed
        \param QString old text
        \param QString new text
        */
        virtual void lineChanged(int,const QString&);

    protected:
        TextEditorSyntaxHighlighter * syntaxHighlighter;

        QList<TextItem*> parseText(const QString&);
        QHash<QString,TextItem*> items;

        TextEditor * textEditor;
        NodesTree * nodesTree;
        ConnectionsTree * connectionsTree;
        QRegExp regexDeclarations;
        QRegExp regexConnections, regexConnectionName;
        QRegExp regexOp;

        ConnectionFamily* findBestMatch(ConnectionTextItem * conn);

        friend class TextEditorSyntaxHighlighter;
    };
}

#endif // TINKERCELL_TEXTEDITORPARSER_H
