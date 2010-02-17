/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

A slider widget that calls a C function whenver values in the slider are changed.
Uses CThread.

****************************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include "ConsoleWindow.h"
#include "MultithreadedSliderWidget.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{
	void MultithreadedSliderWidget::setVisibleSliders(const QStringList& options)
	{
		int k;
		bool visible;
		QList<QString> keys = sliderWidgets.keys();
		
		for (int i=0; i < keys.size(); ++i)
		{
			k = options.indexOf(keys[i]);
			visible = (k > -1);
			sliderWidgets[ keys[i] ]->setVisible(visible);
		}
	}

	MultithreadedSliderWidget::~MultithreadedSliderWidget()
	{
		if (cthread)
		{
			cthread->unload();
			delete cthread;
		}
	}

	MultithreadedSliderWidget::MultithreadedSliderWidget(MainWindow * parent, CThread * thread, Qt::Orientation orientation)
		: QWidget(parent), orientation(orientation), mainWindow(parent)
	{
		setAttribute(Qt::WA_DeleteOnClose);
		cthread = thread;
		setWindowFlags(Qt::Window);
		slidersLayout = 0;
		hide();
	}

	MultithreadedSliderWidget::MultithreadedSliderWidget(MainWindow * parent, const QString & lib, const QString & functionName, Qt::Orientation orientation)
		: QWidget(parent), orientation(orientation), mainWindow(parent)
	{
		setAttribute(Qt::WA_DeleteOnClose);
		cthread = new CThread(parent, lib);
		cthread->setMatrixFunction(functionName.toAscii().data());
		setWindowFlags(Qt::Window);
		slidersLayout = 0;
		hide();
	}

	CThread * MultithreadedSliderWidget::thread() const
	{
		return cthread;
	}
	
	void MultithreadedSliderWidget::minmaxChanged()
	{
		if (!cthread || sliders.isEmpty()) return;
		
		double range,x;
		bool ok;
		
		for (int i=0; i < sliders.size(); ++i)
			if (sliders[i])
				disconnect(sliders[i],SIGNAL(valueChanged(int)),this,SLOT(sliderChanged(int)));
		
		for (int i=0; i < sliders.size() && i < max.size() && i < min.size(); ++i)
			if (sliders[i])
			{
				x = minline[i]->text().toDouble(&ok);
				if (ok)				
					min[i] = x;
				else
					minline[i]->setText(QString::number(min[i]));
					
				x = maxline[i]->text().toDouble(&ok);
				if (ok)				
					max[i] = x;
				else
					maxline[i]->setText(QString::number(max[i]));
					
				range = (max[i]-min[i]);
				sliders[i]->setValue((int)((values.value(i,0) - min[i]) * 100.0/range));
			}
		for (int i=0; i < sliders.size(); ++i)
			if (sliders[i])
				connect(sliders[i],SIGNAL(valueChanged(int)),this,SLOT(sliderChanged(int)));
	}
	
	void MultithreadedSliderWidget::valueChanged()
	{
		if (!cthread || sliders.isEmpty()) return;

		double range,x;
		bool ok;
		
		for (int i=0; i < sliders.size(); ++i)
			if (sliders[i])
				disconnect(sliders[i],SIGNAL(valueChanged(int)),this,SLOT(sliderChanged(int)));
		
		for (int i=0; i < sliders.size() && i < max.size() && i < min.size(); ++i)
			if (sliders[i])
			{
				range = (max[i]-min[i]);
				x = valueline[i]->text().toDouble(&ok);
				if (ok)
				{
					values.value(i,0) = x;
					sliders[i]->setValue((int)((x - min[i]) * 100.0/range));
				}
				else
				{
					valueline[i]->setText(QString::number(values.value(i,0)));
				}
			}
		
		for (int i=0; i < sliders.size(); ++i)
			if (sliders[i])
				connect(sliders[i],SIGNAL(valueChanged(int)),this,SLOT(sliderChanged(int)));
		
		if (cthread->isRunning())
		{
			mainWindow->console()->message(tr("Previous run has not finished yet"));
			return;
		}
		
		cthread->setArg(values);
		cthread->start();
	}

	void MultithreadedSliderWidget::sliderChanged(int)
	{
		if (!cthread || sliders.isEmpty()) return;
		
		double range;
		
		for (int i=0; i < sliders.size() && i < max.size() && i < min.size(); ++i)
			if (sliders[i])
			{
				range = (max[i]-min[i]);
				values.value(i,0) = min[i] + range * (double)(sliders[i]->value())/100.0;
				valueline[i]->setText(QString::number(values.value(i,0)));
			}
		
		if (cthread->isRunning())
		{
			mainWindow->console()->message(tr("Previous run has not finished yet"));
			return;
		}
		
		cthread->setArg(values);		
		cthread->start();
	}
	
	void MultithreadedSliderWidget::setSliders(const QStringList& options, const QList<double>& minValues, const QList<double>& maxValues)
	{
		QSlider * slider;
		QLabel * label;
		QLineEdit * line;

		sliders.clear();
		labels.clear();
		min.clear();
		max.clear();
		minline.clear();
		maxline.clear();
		valueline.clear();
		values.resize(options.size(),1);
		values.setRowNames(options);
		values.setColNames(QStringList() << "value");
		
		QHBoxLayout* layout = new QHBoxLayout;
		
		layout->addWidget(new QLabel(tr("name")));
		layout->addWidget(new QLabel(tr("")));
		layout->addWidget(new QLabel(tr("value")));
		layout->addWidget(new QLabel(tr("min")));
		layout->addWidget(new QLabel(tr("max")));
		
		QVBoxLayout * slidersLayout = new QVBoxLayout;
		QWidget * widget = new QWidget;
		widget->setLayout(layout);
		slidersLayout->addWidget(widget);		

		for (int i=0; i < options.size() && i < minValues.size() && i < maxValues.size(); ++i)
		{
			layout = new QHBoxLayout;
			
			label = new QLabel(options[i]);
			label->setMaximumWidth(80);
			layout->addWidget(label);
			labels << label;

			slider = new QSlider;
			slider->setOrientation(orientation);
			slider->setRange(0,100);
			slider->setValue(50);
			slider->setMinimumWidth(100);
			layout->addWidget(slider,5);
			sliders << slider;
			slider->setTracking(false);
			connect(slider,SIGNAL(valueChanged(int)),this,SLOT(sliderChanged(int)));

			values.value(i,0) = (maxValues[i] + minValues[i])/2.0;
			
			line = new QLineEdit;
			line->setText(QString::number(values.value(i,0) ));
			line->setMaximumWidth(80);
			layout->addWidget(line);
			valueline << line;
			connect(line,SIGNAL(editingFinished()),this,SLOT(valueChanged()));

			line = new QLineEdit;
			line->setText(QString::number(minValues[i]));
			line->setMaximumWidth(80);
			layout->addWidget(line);
			minline << line;
			min << minValues[i];
			connect(line,SIGNAL(editingFinished()),this,SLOT(minmaxChanged()));

			line = new QLineEdit;
			line->setText(QString::number(maxValues[i]));
			line->setMaximumWidth(80);
			layout->addWidget(line);
			maxline << line;
			max << maxValues[i];
			connect(line,SIGNAL(editingFinished()),this,SLOT(minmaxChanged()));
			
			QWidget * widget = new QWidget;
			widget->setLayout(layout);
			slidersLayout->addWidget(widget);
			sliderWidgets[ options[i] ] = widget;
		}
		
		QWidget * slidersWidget = new QWidget;
		slidersWidget->setLayout(slidersLayout);		
		
		QVBoxLayout * mainlayout = new QVBoxLayout;
		QHBoxLayout * buttonLayout = new QHBoxLayout;		
		QPushButton * closeButton = new QPushButton(tr("Close"));
		buttonLayout->addWidget(closeButton);
		buttonLayout->addStretch(1);
		QScrollArea * scrollArea = new QScrollArea;
		scrollArea->setWidget(slidersWidget);
		scrollArea->setWidgetResizable(true);
		mainlayout->addWidget(scrollArea);
		mainlayout->addLayout(buttonLayout);
		connect(closeButton,SIGNAL(pressed()),this,SLOT(close()));
		setLayout(mainlayout);
		
		valueChanged();
	}
	
	DataTable<qreal> MultithreadedSliderWidget::data() const
	{
		return values;
	}
}
