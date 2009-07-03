#ifndef TINKERCELL_TEXTEDITORSYNTAXHIGHLIGHTER_H
#define TINKERCELL_TEXTEDITORSYNTAXHIGHLIGHTER_H

#include <QTextDocument>
#include <QSyntaxHighlighter>
#include "TextEditor.h"
#include "TextItem.h"

namespace Tinkercell
{
     /*! \brief used to highlight node and connections and other syntax
               in the TextEditor
     \ingroup plugins*/
     class TextEditorSyntaxHighlighter : public QSyntaxHighlighter
     {
     public:
          QString searchString;
          /*! \brief constructor*/
          TextEditorSyntaxHighlighter(TextEditor * editor);
          /*! \brief pointer to the TextEditor that this highlighter belongs with*/
          TextEditor * textEditor;
          /*! \brief list of node types*/
          QStringList nodeTypes;
          /*! \brief list of lines defining connections*/
          QHash<QString,ConnectionTextItem*> connections;
          /*! \brief list of lines defining operations*/
          QHash<QString,OpTextItem*> operations;
     protected:
          /*! \brief highlights based on the families and connectionSyntax hash tables*/
          virtual void highlightBlock(const QString &text);
     };
}
#endif // TINKERCELL_TEXTEDITORSYNTAXHIGHLIGHTER_H

