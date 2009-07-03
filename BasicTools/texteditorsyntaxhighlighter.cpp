#include "texteditorsyntaxhighlighter.h"

namespace Tinkercell
{
     TextEditorSyntaxHighlighter::TextEditorSyntaxHighlighter(TextEditor * t)
               : QSyntaxHighlighter(t->document()), textEditor(t)
     {
     }

      void TextEditorSyntaxHighlighter::highlightBlock(const QString &text)
     {
          if (!textEditor || !textEditor->editor || text.isEmpty() || text.isNull()) return;

          int k;
          QTextCharFormat format0, format1;

          format0.setForeground(QBrush(QColor(TextEditor::RegularTextColor)));
          format0.setBackground(QBrush(QColor(TextEditor::RegularBackgroundColor)));
          setFormat(0,text.size(),format0);

          if (!searchString.isNull() && !searchString.isEmpty())
          {
               format1.setForeground(QBrush(QColor(TextEditor::RegularTextColor)));
               format1.setBackground(QBrush(QColor(TextEditor::HighlightBackgroundColor)));

               QRegExp regex(searchString,Qt::CaseInsensitive);
               k = regex.indexIn(text);
               if (k > -1)
                    setFormat(k,regex.cap(0).length(),format1);
               return;
          }

          format1.setForeground(QBrush(QColor(TextEditor::SpecialTextColor)));

          if (text.contains(QRegExp(tr("^\\s*#"))))
          {
               format0.setBackground(QBrush(QColor(TextEditor::CommentsBackgroundColor)));
               setFormat(0,text.size(),format0);
               return;
          }

          QColor color;
          QString string;

          for (int i=0; i < nodeTypes.size(); ++i)
          {
               format1.setBackground(QBrush(color));
                    QRegExp regex(tr("^\\s*") + string + tr("\\s+"),Qt::CaseInsensitive);
                    k = regex.indexIn(text);
                    if (k > -1)
                    {
                         setFormat(0,regex.cap(0).length(),format1);
                         return;
                    }
          }

//           for (int i=0; i < connectionFamilies.size(); ++i)
//           {
//                string = connectionFamilies[i].first;
//                color = connectionFamilies[i].second;
//                if (string.isEmpty())
//                {
//                     k = text.indexOf(string,0,Qt::CaseInsensitive);
//                     if ( !string.isEmpty() &&
//                          k > 0 && (text.length() > k+string.length()) &&
//                          (text.at(k-1).isSpace() || text.at(k-1).isLetterOrNumber()) &&
//                          (text.at(k+string.length()).isSpace() ||
//                           text.at(k+string.length()).isLetterOrNumber())
//                          )
//                          {
//                          format1.setBackground(QBrush(color));
//                          setFormat(k,string.length(),format1);
//                          break;
//                     }
//                }
//           }

          format1.setBackground(QBrush(QColor(TextEditor::NameBackgroundColor)));
          QRegExp regex(tr("^\\s*\\w+\\s*:"),Qt::CaseInsensitive);
          k = regex.indexIn(text);
          if (k > -1)
          {
               setFormat(0,regex.cap(0).length(),format1);
          }
     }
}

