#ifndef TINKERCELL_ANTIOMONY_SYNTAXHIGHLIGHTER_H
#define TINKERCELL_ANTIOMONY_SYNTAXHIGHLIGHTER_H

#include <QMainWindow>
#include <QTextEdit>
#include <QHash>
#include <QTextCharFormat>
#include <QDialog>
#include <QCompleter>
#include <QListWidget>
#include <QThread>
#include <QToolBar>
#include <QActionGroup>
#include <QRegExp>
#include <QSyntaxHighlighter>

#ifndef TINKERCELLEXPORT
#ifdef Q_WS_WIN
#   if defined(TC_EXPORTS) || defined(TinkerCell_EXPORTS)
#       define TINKERCELLEXPORT __declspec(dllexport)
#   else
#       define TINKERCELLEXPORT __declspec(dllimport)
#   endif
#else
#    define TINKERCELLEXPORT
#endif
#endif

namespace Tinkercell
{
	class AntimonySyntaxHighlighter : public QSyntaxHighlighter
    {
		 Q_OBJECT

	 public:
		 AntimonySyntaxHighlighter(QTextDocument *parent = 0);
	 public slots:
		 void setValid(bool);

	 protected:
		 void highlightBlock(const QString &text);

	 private:
		 bool valid;
		 QStringList keywordPatterns;
		 
		 QRegExp reactionRegexp;
		 QRegExp assignmentRegexp;
		 
		 QTextCharFormat invalidFormat;
		 QTextCharFormat keywordFormat;
		 QTextCharFormat functionFormat;
		 QTextCharFormat reactionFormat;
	 };
}
#endif
