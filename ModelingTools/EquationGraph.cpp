#include "MainWindow.h"
#include "EquationGraph.h"
#include "muParserDef.h"
#include "muParser.h"
#include "muParserInt.h"

namespace Tinkercell
{
	EquationGraph::EquationGraph(QWidget * parent) : QWidget(parent), currentNetwork(0), equation(), title(), ylab()
	{
		QVBoxLayout * plotLayout = new QVBoxLayout;
		plotLayout->addWidget(plotWidget = new Plot2DWidget);	
		
		QHBoxLayout * optionsLayout = new QHBoxLayout;
		optionsLayout->addWidget(new QLabel(tr("x-axis:")),0,Qt::AlignRight);
		optionsLayout->addWidget(plotVar = new QComboBox,0,Qt::AlignLeft);
		plotVar->setDuplicatesEnabled(false);
		connect(plotVar,SIGNAL(activated(const QString & )),this,SLOT(xaxisChanged(const QString&)));
		
		optionsLayout->addSpacing(50);
		
		optionsLayout->addWidget(new QLabel(tr("start:")),0,Qt::AlignRight);
		optionsLayout->addWidget(startPlot = new QDoubleSpinBox,0,Qt::AlignLeft);
		startPlot->setValue(0.0);
		connect(startPlot,SIGNAL(valueChanged(double)),this,SLOT( startStopChanged(double) ));
		
		optionsLayout->addWidget(new QLabel(tr("end:")),0,Qt::AlignRight);
		optionsLayout->addWidget(endPlot = new QDoubleSpinBox,0,Qt::AlignLeft);
		endPlot->setValue(10.0);
		connect(endPlot,SIGNAL(valueChanged(double)),this,SLOT( startStopChanged(double) ));
		
		optionsLayout->addSpacing(50);
		
		QCheckBox * logx = new QCheckBox;
		optionsLayout->addWidget(new QLabel(tr("log(x):")),0,Qt::AlignRight);
		optionsLayout->addWidget(logx,0,Qt::AlignLeft);
		connect(logx,SIGNAL(toggled(bool)),plotWidget,SLOT(logX(bool)));
		
		QCheckBox * logy = new QCheckBox;
		optionsLayout->addWidget(new QLabel(tr("log(y):")),0,Qt::AlignRight);
		optionsLayout->addWidget(logy,0,Qt::AlignLeft);
		connect(logy,SIGNAL(toggled(bool)),plotWidget,SLOT(logY(bool)));

		plotLayout->addLayout(optionsLayout);
		setLayout(plotLayout);
	}

	void EquationGraph::xaxisChanged(const QString& s)
	{
		if (!s.isEmpty() && s != currentVar)
		{
			QStringList vars;
			qreal min = startPlot->value();
			qreal max = endPlot->value();		
			replot(currentVar = s,min,max,vars);
		}
	}

	void EquationGraph::startStopChanged(double)
	{
		QStringList vars;
		qreal min = startPlot->value();
		qreal max = endPlot->value();
		replot(currentVar,min,max,vars);
	}
	
	void EquationGraph::setFormula(const QString& e, NetworkHandle* network)
	{
		currentNetwork = network;
		equation = e;
		varslist.clear();
		qreal min = startPlot->value();
		qreal max = endPlot->value();
		
		disconnect(plotVar,SIGNAL(activated(const QString & )),this,SLOT(xaxisChanged(const QString&)));
		
		plotVar->clear();
		replot(currentVar,min,max,varslist);
		plotVar->addItems(varslist);
		
		connect(plotVar,SIGNAL(activated(const QString & )),this,SLOT(xaxisChanged(const QString&)));
	}
	
	void EquationGraph::setTitle(const QString& s)
	{
		title = s;
		if (plotWidget)
			plotWidget->setTitle(s);
	}

	void EquationGraph::setYLabel(const QString& s)
	{
		ylab = s;
		if (plotWidget)
			plotWidget->setYLabel(s);
	}
	
	void EquationGraph::print(QPaintDevice& printer)
	{
		if (plotWidget)
			plotWidget->print(printer);
	}
	
	bool EquationGraph::replot(const QString& xaxis, qreal min, qreal max, QStringList & vars)
	{
		if (equation.isEmpty() || !currentNetwork) return false;
		
		QList< QPair<QString,qreal> > values;

		mu::Parser parser;
		bool b = true;
		
		EquationParser::eval(currentNetwork,equation,&b,values,&parser);
		
		if (!b || values.isEmpty())
			return false;
		
		for (int i=0; i < values.size(); ++i)
		{
			vars += values[i].first;
			if (values[i].second == 0)
				values[i].second = 1.0;
		}
		
		int k = vars.indexOf(xaxis);
		
		if (k < 0)
			k = 0;
		
		currentVar = vars[k];
		
		NumericalDataTable plot;
		plot.resize(100,2);
		
		if (b)
		{
			for (int i=0; i < 100; ++i)
			{
				plot.value(i,0) = values[k].second = i/100.0*(max-min) + min;
				plot.value(i,1) = parser.Eval();
			}
		}
		plot.setColumnName(0,currentVar);
		plot.setColumnName(1,ylab);
		plotWidget->plot(plot,title,0);
		
		return true;
	}

}

