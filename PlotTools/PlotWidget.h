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
#include <QTextCharFormat>
#include "DataTable.h"

namespace Tinkercell
{

	class PlotTool;

	/*!
	\brief A widget containing a data plot, legend and options. This class does not perform any plotting. 
			This class serves as a template for other widgets that perform the plotting. 
	*/
	class PlotWidget : public QWidget
	{
		Q_OBJECT
	public:
		
		/*! \brief tool bar containing all the options for this widget*/
		QToolBar toolBar;
		/*! \brief constructor with plot tool as parent*/
		PlotWidget(PlotTool * parent = 0);
		/*! \brief constructor with plot tool as parent*/
		PlotWidget(const DataTable<qreal>&, PlotTool * parent = 0);
		/*! \brief get the data inside this plot*/
		virtual DataTable<qreal>* data();
		
	protected:
		/*! \brief the plot tool that contains this widget*/
		PlotTool * plotTool;
		
	public slots:
		/*! \brief export data is some format
			\param QString format*/
		virtual void exportData(const QString&);
		/*! \brief Child classes implement this*/
		virtual void logAxis(int,bool);
		/*! \brief Child classes implement this*/
		virtual void setPlotSettings();
		/*! \brief Child classes implement this*/
		virtual void setDisplaySettings();
	
	};

}
#endif
