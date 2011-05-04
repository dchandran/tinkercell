#ifndef TINKERCELL_CSCRIPT_SYNTAXCodingWindowSyntaxHighlighter_H
#define TINKERCELL_CSCRIPT_SYNTAXCodingWindowSyntaxHighlighter_H

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

namespace Tinkercell
{
	class CodingWindowSyntaxHighlighter : public QSyntaxHighlighter
    {
		 Q_OBJECT

	 public:
		 CodingWindowSyntaxHighlighter(QTextDocument *parent = 0);

	 protected:
		 void highlightBlock(const QString &text);

	 private:
		 struct HighlightingRule
		 {
			 QRegExp pattern;
			 QTextCharFormat format;
		 };
		 QVector<HighlightingRule> highlightingRules;

		 QRegExp commentStartExpression;
		 QRegExp commentEndExpression;

		 QTextCharFormat keywordFormat;
		 QTextCharFormat classFormat;
		 QTextCharFormat singleLineCommentFormat;
		 QTextCharFormat multiLineCommentFormat;
		 QTextCharFormat quotationFormat;
		 QTextCharFormat functionFormat;
		 QTextCharFormat loopFormat1, loopFormat2;
	 };
}
#endif
