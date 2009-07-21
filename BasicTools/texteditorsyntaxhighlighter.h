#ifndef TINKERCELL_TEXTEDITORSYNTAXHIGHLIGHTER_H
#define TINKERCELL_TEXTEDITORSYNTAXHIGHLIGHTER_H

#include <QTextDocument>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include "Core/TextEditor.h"
#include "Core/TextItem.h"

namespace Tinkercell
{
     class TextEditorParser;
     /*! \brief used to highlight node and connections and other syntax
               in the TextEditor
     \ingroup plugins*/
     class TextEditorSyntaxHighlighter : public QSyntaxHighlighter
     {
     public:
          QString searchString;
          /*! \brief constructor*/
          TextEditorSyntaxHighlighter(TextEditorParser * listener, QTextDocument * );

     protected:
          /*! \brief highlights based on the families and connectionSyntax hash tables*/
          virtual void highlightBlock(const QString &text);

          TextEditorParser * textParser;
          QTextCharFormat commentFormat;
          QTextCharFormat declarationFormat;
          QTextCharFormat connectionFormat;
          QTextCharFormat opFormat;
     };
}
#endif // TINKERCELL_TEXTEDITORSYNTAXHIGHLIGHTER_H

