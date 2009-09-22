/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.
 
****************************************************************************/

#include <math.h>
#include <QGroupBox>
#include <QTextEdit>
#include "qwt_scale_engine.h"
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "PlotTool.h"
#include "PlotTextWidget.h"

namespace Tinkercell
{	
	PlotWidget::PlotWidget(PlotTool * parent) : QWidget(parent), plotTool(parent)
	{
	}
	
	DataTable<qreal>* PlotWidget::data()
	{
		return 0;
	}
	
	void PlotWidget::exportData(const QString & type)
	{
		DataTable<qreal> * dat = data();
		
		if (!dat) return;
	
		DataTable<qreal>& table = *dat;
		
		QString output;
		
		QStringList colnames = table.getColNames(), 
					rownames = table.getRowNames();
	
		bool printRows = true;
		for (int i=0; i < rownames.size(); ++i)
			if (rownames.at(i).size() <= 0)
			{
				printRows = false;
				break;
			}
	
		for (int i=0; i < colnames.size(); ++i)
		{
			if (i == 0 && !printRows)
				outputs += colnames.at(i);
			else
				outputs += tr("\t") + colnames.at(i);
		}
	
		for (int i=0; i < table.rows(); ++i)
		{
			if (printRows)
			{
				outputs += rownames.at(i);
			}
		
			for (int j=0; j < table.cols(); ++j)
			{
				if (i == 0 && !printRows)
					outputs += QString::number(table.at(i,j));
				else
					outputs += tr("\t") + QString::number(table.at(i,j));
			}
		
			outputs += tr("\n");
		}
		
		if (type.toLower() == tr("clipboard"))
		{
			QClipboard * clipboard = QApplication::clipboard();
		
			if (!clipboard)
			{
				ConsoleWindow::error(tr("No clipboard available."));
				return;
			}

			clipboard->setText(outputs);
			
			ConsoleWindow::message(tr("Tab-delimited data copied to clipboard."));

		}
		else
		if (plotTool && type.toLower() == tr("text"))
		{
			plotTool->addWidget(new PlotTextWidget(table));
		}
		else
		if (plotTool && type.toLower() == tr("latex"))
		{
			plotTool->addWidget(new PlotTextWidget(table,true));
		}
	}
}

