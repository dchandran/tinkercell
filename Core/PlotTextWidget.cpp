/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.
 
****************************************************************************/

#include <math.h>
#include <QGroupBox>
#include <QKeySequence>
#include <QTextEdit>
#include "qwt_scale_engine.h"
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "PlotTool.h"
#include "PlotTextWidget.h"

namespace Tinkercell
{	
	DataTable<qreal>* PlotTextWidget::data()
	{
		return &dataTable;
	}
	
	PlotTextWidget::PlotTextWidget(const DataTable<qreal>& table, PlotTool * parent, const QString& text) : PlotWidget(parent), dataTable(table)
	{
		type = PlotTool::Text;
		textEdit = new CodeEditor;
		
		QHBoxLayout * layout = new QHBoxLayout;
		layout->addWidget(textEdit);
		setLayout(layout);
		
		textEdit->clear();
		textEdit->setReadOnly(true);
		
		if (text.size() > 0)
		{
			textEdit->setPlainText(text);
			return;
		}
	
		QTextCursor cursor = textEdit->textCursor();
	
		QString outputs;
		
		QTextCharFormat headerFormat, regularFormat;
		headerFormat.setForeground(QColor("#003AA3"));
		regularFormat.setForeground(QColor("#252F41"));
		regularFormat.setFontWeight(QFont::Bold);

		QStringList colnames = dataTable.columnNames(),
					rownames = dataTable.rowNames();
	
		bool printRows = true;
		for (int i=0; i < rownames.size(); ++i)
			if (rownames.at(i).size() <= 0)
			{
				printRows = false;
				break;
			}
		
		cursor.setCharFormat(headerFormat);
			
		for (int i=0; i < colnames.size(); ++i)
		{
			if (i == 0 && !printRows)
				outputs += colnames.at(i);
			else
				outputs += tr("\t") + colnames.at(i);
		}
	
		cursor.insertText(outputs + tr("\n"));
		
		for (int i=0; i < dataTable.rows(); ++i)
		{
			if (printRows)
			{
				cursor.setCharFormat(headerFormat);
				cursor.insertText(rownames.at(i));
			}
			for (int j=0; j < dataTable.columns(); ++j)
			{
				if (j == 0 && !printRows)
					outputs += QString::number(dataTable.at(i,j));
				else
					outputs += tr("\t") + QString::number(dataTable.at(i,j));
			}
			
			cursor.setCharFormat(regularFormat);
			cursor.insertText(outputs + tr("\n"));
		}
	}
	
	void PlotTextWidget::keyPressEvent ( QKeyEvent * event )
	{
		if (!textEdit || !event) return;
		
		if (event->matches(QKeySequence::SelectAll))
		{
			textEdit->selectAll();
		}
		else
		if (event->matches(QKeySequence::Copy))
		{
			textEdit->copy();
		}
		else
		if (event->matches(QKeySequence::Paste))
		{
			textEdit->paste();
		}
		else
		if (event->matches(QKeySequence::Cut))
		{
			textEdit->cut();
		}
	}
}

