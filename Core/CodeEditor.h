/****************************************************************************
This file is a combination of two example programs included in the Qt Toolkit.
A few modifications have been added, but the majority of the code is from Qt's
demo programs
****************************************************************************/
#ifndef QT_MODIEIFED_DEMO_COMPLETER_AND_CODEEDITOR_H
#define QT_MODIEIFED_DEMO_COMPLETER_AND_CODEEDITOR_H

#include <QPlainTextEdit>
#include <QDialog>
#include <QLineEdit>
#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;


#ifdef Q_WS_WIN
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif


namespace Tinkercell
{

	class LineNumberArea;

	class TINKERCELLEXPORT CodeEditor : public QPlainTextEdit
	{
		Q_OBJECT

	public:
		CodeEditor(QWidget *parent = 0);

		void lineNumberAreaPaintEvent(QPaintEvent *event);
		int lineNumberAreaWidth();

		void setCompleter(QCompleter *c);
		QCompleter *completer() const;

		void zoomIn(int r = 1);
		void zoomOut(int r = 1);
		QString text() const;

		QWidget *lineNumberArea;

		QColor lineHighlightColor;
		QColor lineNumberBackground;
		QColor lineNumberText;
	
	public slots:
		void setText(const QString&);
		void showFindReplaceDialog();
		bool find(const QString&);
		bool replace(const QString&,const QString&);

	protected:
		void resizeEvent(QResizeEvent *event);
		virtual void wheelEvent ( QWheelEvent * wheelEvent );
		void keyPressEvent(QKeyEvent *e);
		void focusInEvent(QFocusEvent *e);

	private slots:
		void updateLineNumberAreaWidth(int newBlockCount);
		void highlightCurrentLine();
		void updateLineNumberArea(const QRect &, int);
		void insertCompletion(const QString &completion);
		void find();
		void replace();

	private:
		QString textUnderCursor() const;
		QDialog * findReplaceDialog;
		QLineEdit * findLineEdit;
		QLineEdit * replaceLineEdit;
		QCompleter *c;

	};


	class TINKERCELLEXPORT LineNumberArea : public QWidget
	{
	public:

		LineNumberArea(CodeEditor *editor) : QWidget(editor)
		{
			codeEditor = editor;
		}

		QSize sizeHint() const 
		{
			return QSize(codeEditor->lineNumberAreaWidth(), 0);
		}

	protected:
		void paintEvent(QPaintEvent *event) {
			codeEditor->lineNumberAreaPaintEvent(event);
		}

	private:
		CodeEditor * codeEditor;
	};

}

#endif

