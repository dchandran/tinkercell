#include "ConsoleWindow.h"
#include "CombinatoricsOutputWidget.h"

using namespace Tinkercell;

ModuleCombinatoricsOutputWidget::ModuleCombinatoricsOutputWidget(
	const NumericalDataTable& population, 
	const NumericalDataTable& modules, 
	const NumericalDataTable& scores,
	QWidget * parent) : 
	QDialog(parent), 
	population(population), 
	modules(modules), 
	scores(scores)
{
	//setAttribute(Qt::WA_DeleteOnClose);
	QHBoxLayout * layout1 = new QHBoxLayout;
	QHBoxLayout * layout2 = new QHBoxLayout;
	
	for (int i=0; i < scores.columns(); ++i)
	{
		QVBoxLayout * vlayout = new QVBoxLayout;
		
		QSlider * slider = new QSlider(Qt::Vertical);
		connect(slider,SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
		
		QDoubleSpinBox * spinBox = new QDoubleSpinBox;
		connect(spinBox,SIGNAL(editingFinished()), this, SLOT(spinBoxChanged()));
		
		vlayout->addWidget(new QLabel(tr("obj.") + QString::number(i+1)),0);
		vlayout->addWidget(slider,1);
		vlayout->addWidget(spinBox,0);
		layout1->addLayout(vlayout, 0);
		
		QPair<double,double> range = findRange(scores,i);
		slider->setRange(0,100);
		slider->setValue(50);
		
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
	layout->addWidget(group1, 0);
	layout->addWidget(group2, 1);		
	setLayout(layout);
	
	update();
}

QList<int> ModuleCombinatoricsOutputWidget::rowsThatPassThreshold()
{
	QList<int> rows;
	for (int i=0; i < scores.rows(); ++i)
	{
		bool pass = true;
		for (int j=0; j < spinBoxes.size() && j < scores.columns(); ++j)
			if (scores(i,j) < spinBoxes[j]->value())
			{
				pass = false;
				break;
			}
		if (pass)
			rows << i;
	}
	return rows;
}

void ModuleCombinatoricsOutputWidget::sliderMoved(int)
{
	for (int i=0; i < sliders.size() && i < spinBoxes.size() && i < scores.columns(); ++i)
		if (sliders[i] && spinBoxes[i])
		{
			QPair<double,double> range = findRange(scores,i);
			spinBoxes[i]->setValue( sliders[i]->value()/100.0 * (range.second-range.first) + range.first );
		}
	update();
}

void ModuleCombinatoricsOutputWidget::spinBoxChanged()
{
	for (int i=0; i < sliders.size() && i < spinBoxes.size() && i < scores.columns(); ++i)
		if (sliders[i] && spinBoxes[i])
		{
			QPair<double,double> range = findRange(scores,i);
			sliders[i]->setValue( (int)((spinBoxes[i]->value() - range.first)/(range.second-range.first) * 100.0));
		}
	update();
}

void ModuleCombinatoricsOutputWidget::update()
{
	NumericalDataTable moduleCount, points;
	
	QList<int> selectRows = rowsThatPassThreshold();
	
	moduleCount.resize(modules.columns(),1);
	moduleCount.setRowNames(modules.columnNames());
	moduleCount.setColumnName(0,tr("Count"));
	
	points.resize(selectRows.size(), population.columns());
	points.setColumnNames( population.columnNames() );

	for (int i=0; i < selectRows.size(); ++i)
	{
		for (int j=0; j < modules.columns(); ++j)
			moduleCount(j,0) += modules( selectRows[i] , j );
		
		for (int j=0; j < population.columns(); ++j)
			points(i,j) = population( selectRows[i], j );
	}
	
	if (plot1->data()->rows() == 0)
	{
		plot1->plot(moduleCount,tr("Module types"),-1);
		plot2->plot(points,tr("Parameters"),0);
	}
	else
	{
		plot1->updateData(moduleCount);
		plot2->updateData(points);
	}
}

QPair<double,double> ModuleCombinatoricsOutputWidget::findRange(const NumericalDataTable& values, int col)
{
	QPair<double,double> range;
	if (values.rows() > 0)
	{
		range.first = values(0,col);
		range.second = values(0,col);

		for (int i=0; i < values.rows(); ++i)
		{
			if (values(i,col) < range.first)
				range.first = values(i,col);
			if (values(i,col) > range.second)
				range.second = values(i,col);
		}
	}
	return range;
}

