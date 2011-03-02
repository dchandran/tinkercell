/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.
 
****************************************************************************/

#ifndef TINKERCELL_PLOTTEXTWIDGET_H
#define TINKERCELL_PLOTTEXTWIDGET_H

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
#include "PlotWidget.h"
#include "CodeEditor.h"

namespace Tinkercell
{

	/*!
	\brief A PlotWidget used to display tab delimited text
	*/
	class PlotTextWidget : public PlotWidget
	{
		Q_OBJECT
	public:
		
		/*! \brief constructor with data table and plot tool as parent*/
		PlotTextWidget(const DataTable<qreal>&, PlotTool * parent = 0, const QString& text=QString());
		/*! \brief get the data*/
		virtual DataTable<qreal>* data();
		/*! \brief update displayed data*/
		void updateData(const DataTable<qreal>&);
		
	private:
		DataTable<qreal> dataTable;
		CodeEditor * textEdit;
	
	protected:
		virtual void keyPressEvent ( QKeyEvent * event );
	
	};

}
#endif
