/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.
 
****************************************************************************/

#include <math.h>
#include <QGroupBox>
#include "qwt_scale_engine.h"
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "PlotTool.h"
#include "PlotWidget.h"

namespace Tinkercell
{
	
	PlotWidget::PlotWidget(PlotTool * parent) : QWidget(parent)
	{
	}
	
	void PlotWidget::plot(const DataTable<qreal>& ,const QString& ,int)
	{
	}
	
	DataTable<qreal>* PlotWidget::data()
	{
		return 0;
	}
	
	void PlotWidget::printToFile(const QString&)
	{	
	}
	
	void PlotWidget::copyData()
	{
	}
	
	void PlotWidget::logAxis(int,bool)
	{
	}
	
	void PlotWidget::setTitle(const QString&)
	{
	}
	
	void PlotWidget::setXLabel(const QString&)
	{		
	}
	
	void PlotWidget::setYLabel(const QString&)
	{
	}
}
