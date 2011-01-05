#include "CombinatoricsOutputWidget.h"

using namespace Tinkercell;

ModuleCombinatoricsOutputWidget::ModuleCombinatoricsOutputWidget(
	const NumericalDataTable& population, 
	const NumericalDataTable& modules, 
	const QList< QList<qreal> >& scores) : 
	QDialog()
{
	setAttribute(Qt::WA_DeleteOnClose);
	QHBoxLayout * layout1 = new QHBoxLayout;
	QHBoxLayout * layout2 = new QHBoxLayout;
	
	for (int i=0; i < scores.size(); ++i)
	{
		QVBoxLayout * vlayout = new QVBoxLayout;
		
		QSlider * slider = new QSlider(Qt::Vertical);
		connect(slider,SIGNAL(sliderMoved()), this, SLOT(update()));
		
		QDoubleSpinBox * spinBox = new QDoubleSpinBox;
		connect(slider,SIGNAL(editingFinished()), this, SLOT(update()));
		
		vlayout->addWidget(new QLabel(tr("obj.") + QString::number(i+1)),0);
		vlayout->addWidget(slider,1);
		vlayout->addWidget(spinBox,0);
		layout1->addLayout(layout1, 0);
		
		QPair<double,double> range = findRange(scores[i]);
		slider->setRange(range.first,range.second);
		slider->setValue((range.first + range.second)/2.0);
		
		spinBox->setRange(range.first,range.second);
		spinBox->setSingleStep((range.second - range.first)/100.0);
		
		spinBoxes << spinBox;
		sliders << slider;
	}
	
	QTabWidget * tabs = new QTabWidget;
	plot1 = new Plot2DWidget;
	plot2 = new Plot2DWidget;
	
	plot1->type = PlotTool::BarPlot;
	plot2->type = PlotTool::ScatterPlot;
	
	tabs->addTab(plot1, " sub-models ");
	tabs->addTab(plot2, " parameters ");
	
	QGroupBox * group1 = new QGroupBox(" set threshold ");
	QGroupBox * group2 = new QGroupBox(" view results ");
	
	layout2->addWidget(tabs);	
	group1->setLayout(layout1);
	group2->setLayout(layout2);
	
	QHBoxLayout * layout = new QHBoxLayout;
	layout->addWidget(group1, 1);
	layout->addWidget(group2, 1);		
	setLayout(layout);
	
	update();
}

void ModuleCombinatoricsOutputWidget::update()
{
}

QPair<double,double> ModuleCombinatoricsOutputWidget::findRange(const QList<qreal>& values)
{
	QPair<double,double> range;
	if (!values.isEmpty())
	{
		range.first = values[0];
		range.second = values[0];
	
		for (int i=0; i < values.size(); ++i)
		{
			if (values[i] < range.first)
				range.first = values[i];
			if (values[i] > range.second)
				range.second = values[i];
		}
	}
	return range;
}

