/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This class adds the "events" and "functions" data to each item in Tinkercell and
 provides the user interface and C functions for changing those values.
 
****************************************************************************/

#ifndef TINKERCELL_GRAPHSTRINGTOOL_H
#define TINKERCELL_GRAPHSTRINGTOOL_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHash>
#include <QGroupBox>
#include <QUndoCommand>
#include <QGraphicsRectItem>
#include <QGraphicsWidget>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QToolButton>
#include <QButtonGroup>
#include <QListWidget>

#include "Core/DataTable.h"
#include "Core/ItemHandle.h"
#include "Core/Tool.h"
#include "Core/MainWindow.h"
#include "PlotTools/PlotTool.h"

namespace Tinkercell
{

	class GraphStringTool : public Tool
	{
		Q_OBJECT;

	public:
		GraphStringTool();
		bool setMainWindow(MainWindow * main);
		QSize sizeHint() const;
	
	public slots:
		void visibilityChanged(bool);
		void graph(const QStringList&, const QStringList& names, const QList<qreal>& values);
		void toolLoaded(Tool*);
		void displayModel(QTabWidget& widgets, const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations);
		void historyUpdate(int);
	signals:
		void changeData(const DataTable<qreal>&);
		void plot(const DataTable<qreal>&,int xaxis,const QString& title = QString(),int all = 1);
		void runPythonCode(const QString&);
	private slots:
		void plotFunctions();
		void addFunction();	
		void removeFunctions();
		void editFunctionsList(QListWidgetItem*);
	protected:
		void keyPressEvent(QKeyEvent* keyEvent);
		QListWidget functionsListWidget;
		QStringList names;
		QList<qreal> values;
		QComboBox comboBox;
		QDoubleSpinBox spinBox1, spinBox2;
		QDockWidget * dockWidget;
		QString comboBoxSelection;
		PlotTool * plotTool;
	};


}

#endif
