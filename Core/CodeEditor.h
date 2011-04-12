/****************************************************************************
** This file is a combination of two example programs included in the Qt Toolkit.
** Below is the Qt copyright information as included in the example programs:
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the example classes of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT_DEMO_COMPLETER_AND_CODEEDITOR_H
#define QT_DEMO_COMPLETER_AND_CODEEDITOR_H

#include <QPlainTextEdit>
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

	private:
		QString textUnderCursor() const;

	private:
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
		CodeEditor *codeEditor;
	};

}

#endif

