#include "texteditorsyntaxhighlighter.h"
#include "BasicTools/TextEditorListener.h"
#include "BasicTools/TextEditorParser.h"

namespace Tinkercell
{
     TextEditorSyntaxHighlighter::TextEditorSyntaxHighlighter(TextEditorParser * t, QTextDocument * doc)
               : QSyntaxHighlighter(doc), textParser(t)
     {
          commentFormat.setForeground(QBrush(TextEditorListener::CommentsTextColor));
          commentFormat.setBackground(QBrush(TextEditorListener::CommentsBackgroundColor));

          declarationFormat.setForeground(QBrush(TextEditorListener::NodesTextColor));
          declarationFormat.setBackground(QBrush(TextEditorListener::NodesBackgroundColor));

          connectionFormat.setForeground(QBrush(TextEditorListener::CommentsTextColor));
          connectionFormat.setBackground(QBrush(TextEditorListener::CommentsBackgroundColor));

          opFormat.setForeground(QBrush(TextEditorListener::OpTextColor));
          opFormat.setBackground(QBrush(TextEditorListener::OpBackgroundColor));
     }

     void TextEditorSyntaxHighlighter::highlightBlock(const QString &text)
     {
          if (!textParser || !textParser->textEditor || text.isEmpty() || text.isNull()) return;

          TextItem * item;
          if (textParser->items.contains(text))
          {
              item = textParser->items[text];
              if (!item)
                  setFormat(0,text.length(),commentFormat);
              else
                  if (item->asNode())
                      setFormat(0,text.length(),declarationFormat);
                  else
                  if (item->asConnection())
                      setFormat(0,text.length(),connectionFormat);
                  else
                  if (item->asOp())
                      setFormat(0,text.length(),opFormat);
                  else
                     setFormat(0,text.length(),commentFormat);
          }
          else
              setFormat(0,text.length(),commentFormat);
     }
}

