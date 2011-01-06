/****************************************************************************

Copyright (c) 2010 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

A widget displaying plots and controls from the output of the ModuleCombinatorics tool

****************************************************************************/

#ifndef TINKERCELL_MODULECOMBINATORICSOUTPUTWIDGET_H
#define TINKERCELL_MODULECOMBINATORICSOUTPUTWIDGET_H

#include <QSlider>
#include <QPair>
#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QComboBox>
#include <QDoubleSpinBox>
#include "DataTable.h"
#include "Plot2DWidget.h"
#include "PlotTool.h"

namespace Tinkercell
{
	class TINKERCELLEXPORT ModuleCombinatoricsOutputWidget : public QDialog
	{
		Q_OBJECT
		
		public:

			ModuleCombinatoricsOutputWidget(const NumericalDataTable& population, const NumericalDataTable& modules, const NumericalDataTable& scores);
		
		private slots:

			void update();
			void sliderMoved();
			void spinBoxChanged();

		private:

			QList<QDoubleSpinBox*> spinBoxes;
			QList<QSlider*> sliders;
			NumericalDataTable population, modules, scores;
			Plot2DWidget * plot1, *plot2;
			static QPair<double,double> findRange(const NumericalDataTable&, int col=0);
			QList<int> rowsThatPassThreshold();
	};
}

#endif

