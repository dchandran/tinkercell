#include "AntimonySyntaxHighlighter.h"

namespace Tinkercell
{
	 /********************************
	      SYNTAX HIGHLIGHTER
	 *********************************/
	 AntimonySyntaxHighlighter::AntimonySyntaxHighlighter(QTextDocument *parent)
		 : QSyntaxHighlighter(parent)
	 {
		 HighlightingRule rule;

		 keywordFormat.setForeground(Qt::darkBlue);
		 keywordFormat.setFontWeight(QFont::Bold);
		 QStringList keywordPatterns;

		 keywordPatterns << "\\bmodule\\b" << "\\bend\\b" << "\\bmodel\\b"
						 << "\\bcompartment\\b" << "\\bvar\\b" << "\\bconst\\b"
						 << "\\bext\\b";
		 foreach (QString pattern, keywordPatterns) {
			 rule.pattern = QRegExp(pattern);
			 rule.format = keywordFormat;
			 highlightingRules.append(rule);
		 }

		 functionFormat.setFontItalic(true);
		 functionFormat.setForeground(Qt::blue);
		 rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\s*\\()");
		 rule.format = functionFormat;
		 highlightingRules.append(rule);
	 }

	 void AntimonySyntaxHighlighter::highlightBlock(const QString &text)
	 {
		 foreach (HighlightingRule rule, highlightingRules) {
			 QRegExp expression(rule.pattern);
			 int index = text.indexOf(expression);
			 while (index >= 0) {
				 int length = expression.matchedLength();
				 setFormat(index, length, rule.format);
				 index = text.indexOf(expression, index + length);
			 }
		 }
		 setCurrentBlockState(0);
	 }
}
