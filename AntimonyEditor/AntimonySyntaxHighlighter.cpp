#include "AntimonyTool/AntimonySyntaxHighlighter.h"
namespace Tinkercell
{
	 /********************************
	      SYNTAX HIGHLIGHTER
	 *********************************/
	 AntimonySyntaxHighlighter::AntimonySyntaxHighlighter(QTextDocument *parent)
		 : QSyntaxHighlighter(parent), valid(true)
	 {
		 keywordPatterns << "\\bmodule\\b" << "\\bend\\b" << "\\bmodel\\b" 
						 << "\\bcompartment\\b" << "\\bvar\\b" << "\\bconst\\b"
						 << "\\bext\\b";
		 
		 keywordFormat.setFontWeight(QFont::Bold);
		 keywordFormat.setForeground(QColor(tr("#7E7E7E")));
		 functionFormat.setForeground(QColor(tr("#4800DA")));
		 reactionFormat.setForeground(QColor(tr("#9E3501")));
		 
		 invalidFormat.setForeground(QColor(tr("#DA8000")));
		 
		 reactionRegexp = QRegExp(tr("\\->.*;.+"));
		 assignmentRegexp = QRegExp(tr(".+=.+"));
	 }
	 
	 void AntimonySyntaxHighlighter::setValid(bool b)
	 {
		if (valid != b)
		{
			valid = b;
			rehighlight();
		}
	 }

	 void AntimonySyntaxHighlighter::highlightBlock(const QString &text)
	 {
		 if (!valid)
		 {
			setFormat(0, text.length(), invalidFormat);
			return;
		 }
		 int index = reactionRegexp.indexIn(text);
		 if (index >= 0)
		 {
			index = text.indexOf(tr(";"));
			setFormat(0, index, reactionFormat);
		 }
		 else
		 {
			 index = assignmentRegexp.indexIn(text);
			 if (index >= 0)
			 {
				setFormat(0, text.length(), functionFormat);
			 }
			 else
				 for (int i=0; i < keywordPatterns.size(); ++i)
				 {
					QRegExp expression(keywordPatterns[i]);
					index = text.indexOf(expression);
					while (index >= 0)
					{
						int length = expression.matchedLength();
						setFormat(index, length, keywordFormat);
						index = text.indexOf(expression, index + length);
					}
				 }
		 }
		 setCurrentBlockState(0);
	 }
}
