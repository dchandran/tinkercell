/****************************************************************************

Copyright (c) 2010 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

A widget displaying plots and controls from the output of the ModuleCombinatorics tool

****************************************************************************/

#ifndef TINKERCELL_MODULECOMBINATORICSOUTPUTWIDGET_H
#define TINKERCELL_MODULECOMBINATORICSOUTPUTWIDGET_H

#include <QSlider>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include "DataTable.h"
#include "Plot2DWidget.h"

namespace Tinkercell
{
	class TINKERCELLEXPORT ModuleCombinatoricsOutputWidget : public QDialog
	{
		Q_OBJECT
		
		public:

			ModuleCombinatoricsOutputWidget(const NumericalDataTable& population, const NumericalDataTable& modules, const QList<qreal>& scores);
		
		private slots:

			void sliderMoved();
	};
}

#endif

