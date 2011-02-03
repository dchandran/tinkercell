/****************************************************************************

Copyright (c) 2010 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This widget is used to plot an equation. It automatically looks up parameter values.

****************************************************************************/

#ifndef TINKERCELL_EQUATIONGRAPHWIDGET_H
#define TINKERCELL_EQUATIONGRAPHWIDGET_H

#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPrinter>
#include "Plot2DWidget.h"
#include "ItemHandle.h"
#include "EquationParser.h"
#include "NetworkHandle.h"

namespace Tinkercell
{
	/*! \brief This widget is used to plot an equation. It automatically looks up parameter values.*/
	class TINKERCELLEXPORT EquationGraph : public QWidget
	{
			Q_OBJECT
		public:
			EquationGraph(QWidget * parent=0);
	
		public slots:
			void setFormula(const QString& equation, NetworkHandle*);
			void setTitle(const QString&);
			void setYLabel(const QString&);
			void print(QPaintDevice&);
	
		private slots:
			void xaxisChanged(const QString&);
			void startStopChanged(double);

		private:
			NetworkHandle * currentNetwork;
			QComboBox * plotVar;
			Plot2DWidget * plotWidget;
			QDoubleSpinBox * startPlot, * endPlot;
			QString currentVar;
			QStringList varslist;
			QString equation;
			QString title, ylab;
			bool replot(const QString& xaxis, qreal from, qreal to, QStringList& vars);
	};
}
#endif
