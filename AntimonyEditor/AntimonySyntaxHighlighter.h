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

namespace Tinkercell
{
	class AntimonySyntaxHighlighter : public QSyntaxHighlighter
    {
		 Q_OBJECT

	 public:
		 AntimonySyntaxHighlighter(QTextDocument *parent = 0);

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
		 QTextCharFormat functionFormat;
	 };
}
#endif
