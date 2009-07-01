/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This file defines the TextEditor class. The TextEditor and Canvas are two ways to define a network.
 The Canvas is used for graphical reprentation of a network, whereas the TextEditor is used for
 text-based representation of a network.

****************************************************************************/


#include "TextEditor.h"
#include "TextItem.h"
#include "ItemFamily.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "UndoCommands.h"
#include <QTextBlock>
#include <QTextCharFormat>
#include <QFont>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QRegExp>
#include <QTextCursor>

namespace Tinkercell
{
//Arnaud: TextEditor does not have any field named syntaxHighlighter
     TextEditor::TextEditor() :
               editor(new Editor(this)), sideBar(new QWidget), scrollArea(new QScrollArea),
               splitter(new QSplitter), /*syntaxHighlighter(this), */informationLine(new QLabel("line:",this))
     {
          alignment = Qt::AlignLeft;

          scrollArea->setWidget(sideBar);
          scrollArea->setMaximumWidth(150);

          QWidget * editorAndLine = new QWidget;
          QVBoxLayout * layout = new QVBoxLayout;
          layout->addWidget(editor);
          layout->addWidget(informationLine);
          editorAndLine->setLayout(layout);

          splitter->addWidget(editorAndLine);
          splitter->addWidget(scrollArea);

          layout = new QVBoxLayout;

          //Arnaud: regularBackgroundColor is not a member of TinkerCell::TextEditor
          scrollArea->setStyleSheet(tr("background-color: qlineargradient(x1: 1, y1: 0, x2: 0, y2: 0, stop: 0 #AAAAAA, stop: 0.8 #888888, stop: 1.0 #D9DDEB);"));
          editor->setStyleSheet(tr("background-color: ") /*+ TextEditor::regularBackgroundColor*/ + tr(";"));

          editor->setCursorWidth(3);

          layout->addWidget(splitter);
          layout->setContentsMargins(0,0,0,0);
          layout->setSpacing(0);
          setLayout(layout);
     }

     void TextEditor::undo()
     {
          if (editor)
               editor->undo();
     }

     void TextEditor::redo()
     {
          if (editor)
               editor->redo();
     }

     void TextEditor::copy()
     {
          if (editor)
               editor->copy();
     }

     void TextEditor::cut()
     {
          if (editor)
               editor->cut();
     }

     void TextEditor::paste()
     {
          if (editor)
               editor->paste();
     }

     void TextEditor::selectAll()
     {
          if (editor)
               editor->selectAll();
     }

     void TextEditor::print(QPrinter * printer)
     {
          if (editor)
               editor->print(printer);
     }

     void TextEditor::find(const QString& s)
     {
      //Arnaud: syntaxHighlighter was not declared in this scope
//           syntaxHighlighter.searchString = s;
//           syntaxHighlighter.rehighlight();
     }

     void TextEditor::replace(const QString& old_string, const QString& new_string)
     {

     }

     void TextEditor::replace(const QRegExp& old_expression, const QString& new_string)
     {
     }

     void TextEditor::addToToolBar(QWidget * widget)
     {
          if (!sideBar) return;
          if (!sideBar->layout())
               sideBar->setLayout(new QVBoxLayout);

          sideBar->layout()->addWidget(widget);
     }

     void TextEditor::removeFromToolBar(QWidget * widget)
     {
          if (!sideBar) return;
          if (!sideBar->layout())
               sideBar->setLayout(new QVBoxLayout);
          sideBar->layout()->removeWidget(widget);
     }

     void TextEditor::enableToolBar(bool show)
     {
          if (!splitter)
               return;

          if (!sideBar)
               sideBar = new QWidget;

          if (!scrollArea)
          {
               scrollArea = new QScrollArea;
               splitter->addWidget(scrollArea);
          }

          if (scrollArea->widget() != sideBar)
               scrollArea->setWidget(sideBar);

          sideBar->setVisible(show);
     }

     QWidget* TextEditor::toolBar() const
     {
          return this->sideBar;
     }

     void TextEditor::alignToolBar(Qt::Alignment align)
     {
          if (!splitter || !sideBar || !scrollArea)
               return;

          switch(align)
          {
          case Qt::AlignLeft:
               splitter->setOrientation(Qt::Vertical);
               splitter->addWidget(scrollArea);
               splitter->addWidget(editor);
               break;
          case Qt::AlignRight:
               splitter->setOrientation(Qt::Vertical);
               splitter->addWidget(editor);
               splitter->addWidget(scrollArea);
               break;
          case Qt::AlignTop:
               splitter->setOrientation(Qt::Horizontal);
               splitter->addWidget(scrollArea);
               splitter->addWidget(editor);
               break;
          default:
               splitter->setOrientation(Qt::Horizontal);
               splitter->addWidget(editor);
               splitter->addWidget(scrollArea);
               break;

          }
     }

     Editor::Editor(TextEditor * t) : QTextEdit(), textEditor(t)
     {
          //setUndoRedoEnabled(false);
          prevBlockNumber = -1;
          changedBlockNumber = -1;
          setWordWrapMode(QTextOption::NoWrap);
          connect(this,SIGNAL(textChanged()),this,SLOT(textChangedSlot()));
     }

     void Editor::wheelEvent ( QWheelEvent * wheelEvent )
     {
          if (wheelEvent == 0) return;

          if (wheelEvent->modifiers() == Qt::ControlModifier)
          {
               if (wheelEvent->delta() > 0)
                    zoomIn();
               else
                    zoomOut();
          }
          else
          {
               QTextEdit::wheelEvent(wheelEvent);
          }
     }

     void Editor::textChangedSlot()
     {
      //Arnaud:  'class Tinkercell::TextEditor' has no member named 'syntaxHighlighter'
//           if (textEditor)
//                textEditor->syntaxHighlighter.searchString = tr("");
          changedBlockNumber = textCursor().blockNumber();
          if (prevBlockNumber == changedBlockNumber)
               changedBlockText = textCursor().block().text();
     }

     void Editor::keyPressEvent ( QKeyEvent * event )
     {
          int n0 = textCursor().blockNumber();

          if (event->matches(QKeySequence::Redo)) { qDebug() << "redo"; }
          else
               if (event->matches(QKeySequence::Undo)) { qDebug() << "undo"; }
          else
               QTextEdit::keyPressEvent ( event );

          int n1 = textCursor().blockNumber();
          if (n0 != n1)
          {
               if (changedBlockNumber > -1)
                    qDebug() << prevBlockText << " ==> " << changedBlockText;
               prevBlockText = textCursor().block().text();
               prevBlockNumber = n1;
               changedBlockNumber = -1;
          }

          if (textEditor && textEditor->informationLine)
               textEditor->informationLine->setText(tr("line: ") + QString::number(n1));
     }

     QTextDocument * TextEditor::document()
     {
          if (!editor)
          {
//Arnaud:  error: invalid conversion from 'QTextEdit*' to 'Tinkercell::Editor*'
//                editor = new QTextEdit(this);
            editor = new Editor( this );
               if (!splitter)
                    splitter = new QSplitter(this);
               splitter->addWidget(editor);
          }
          return editor->document();
     }

     QTextEdit * TextEditor::get_editor()
     {
          if (!editor)
          {
//Arnaud:  error: invalid conversion from 'QTextEdit*' to 'Tinkercell::Editor*'
//                editor = new QTextEdit(this);
               editor = new Editor(this);
               if (!splitter)
                    splitter = new QSplitter(this);
               splitter->addWidget(editor);
          }
          return editor;
     }

     //default global settings

     QString TextEditor::RegularBackgroundColor("#FFFFFF");
     QString TextEditor::RegularTextColor("#032763");
     QString TextEditor::CommentsBackgroundColor("#E3F9E1");
     QString TextEditor::SpecialTextColor("#F9F6DE");
     QString TextEditor::NameBackgroundColor("#8CD5FB");
     QString TextEditor::HighlightBackgroundColor("#FCF00");

     QString TextEditor::ConnectionDescriptionSeparator(";");
     QString TextEditor::MultipleNodeSeparator("+");
     bool TextEditor::MultipleNodesAllowed = true;
     bool TextEditor::ConnectionDescriptionAllowed = true;
     bool TextEditor::ShowNodeDeclarations = true;
     bool TextEditor::ShowOperations = true;
}
