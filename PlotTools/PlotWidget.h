/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.
 
****************************************************************************/

#ifndef TINKERCELL_PLOTWIDGET_H
#define TINKERCELL_PLOTWIDGET_H

#include <QtGui>
#include <QList>
#include <QStringList>
#include <QString>
#include <QTableWidget>
#include <QPainterPath>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSplitter>
#include <QSemaphore>
#include <QMainWindow>
#include <QButtonGroup>
#include "DataTable.h"

namespace Tinkercell
{

	class PlotTool;

	/*!
	\brief A widget containing a data plot, legend and options
	*/
	class PlotWidget : public QWidget
	{
		Q_OBJECT
	public:
		
		PlotWidget(PlotTool * parent = 0);
		virtual DataTable<qreal>* data();
		virtual void plot(const DataTable<qreal>& matrix,const QString& title,int x=0);
		
	protected:
		PlotTool * plotTool;
		
	public slots:
		virtual void copyData();
		virtual void printToFile(const QString&);
		virtual void logAxis(int,bool);
		virtual void setTitle(const QString&);
		virtual void setXLabel(const QString&);
		virtual void setYLabel(const QString&);
	};

}
#endif
