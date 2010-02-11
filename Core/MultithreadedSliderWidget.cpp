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

	void MultithreadedSliderWidget::setSliders(const QStringList& options, const QList<double>& minValues, const QList<double>& maxValues)
	{
		QSlider * slider;
		QLabel * label;
		QLineEdit * line;
	
		if (!slidersLayout)  //initialize
		{
			initialLayout(options,minValues,maxValues);
			return;
		}

		int k;
		bool visible;
		for (int i=0; i < sliders.size(); ++i)
		{
			k = options.indexOf(values.colName(i));
			visible = (k > -1);
			sliders[i]->setVisible(visible);
			labels[i]->setVisible(visible);
			minline[i]->setVisible(visible);
			maxline[i]->setVisible(visible);
			valueline[i]->setVisible(visible);
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
		slidersLayout = 0;	
		setWindowFlags(Qt::Window);
		hide();
	}

	MultithreadedSliderWidget::MultithreadedSliderWidget(MainWindow * parent, const QString & lib, const QString & functionName, Qt::Orientation orientation)
		: QWidget(parent), orientation(orientation), mainWindow(parent)
	{
		setAttribute(Qt::WA_DeleteOnClose);
		cthread = new CThread(parent, lib);
		cthread->setMatrixFunction(functionName.toAscii().data());
		slidersLayout = 0;	
		setWindowFlags(Qt::Window);
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
	}
	
	void MultithreadedSliderWidget::valueChanged()
	{
		if (!cthread || sliders.isEmpty()) return;

		double range,x;
		bool ok;
		
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
		cthread->setArg(values);
		
		if (cthread->isRunning())
		{
			mainWindow->console()->message(tr("Previous run has not finished yet"));
			return;
		}
		
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
		cthread->setArg(values);
		
		if (cthread->isRunning())
		{
			mainWindow->console()->message(tr("Previous run has not finished yet"));
			return;
		}
		
		cthread->start();
	}
	
	void MultithreadedSliderWidget::initialLayout(const QStringList& options, const QList<double>& minValues, const QList<double>& maxValues)
	{
		QSlider * slider;
		QLabel * label;
		QLineEdit * line;

		slidersLayout = new QGridLayout;
		
		sliders.clear();
		labels.clear();
		min.clear();
		max.clear();
		minline.clear();
		maxline.clear();
		valueline.clear();
		values.resize(options.size(),1);
		values.setColNames(options);
			
		slidersLayout->addWidget(new QLabel(tr("name")),0,0,Qt::AlignCenter);
		slidersLayout->addWidget(new QLabel(tr("")),0,1,0,10,Qt::AlignCenter);
		slidersLayout->addWidget(new QLabel(tr("value")),0,11,Qt::AlignCenter);
		slidersLayout->addWidget(new QLabel(tr("min")),0,12,Qt::AlignCenter);
		slidersLayout->addWidget(new QLabel(tr("max")),0,13,Qt::AlignCenter);
			
		for (int i=0; i < options.size() && i < minValues.size() && i < maxValues.size(); ++i)
		{
			label = new QLabel(options[i]);
			slidersLayout->addWidget(label,1+i,0,Qt::AlignRight);
			labels << label;

			slider = new QSlider;
			slider->setOrientation(orientation);
			slider->setRange(0,100);
			slider->setValue(50);
			slidersLayout->addWidget(slider,1+i,1,1+i,10);
			sliders << slider;
			slider->setTracking(false);
			connect(slider,SIGNAL(valueChanged(int)),this,SLOT(sliderChanged(int)));

			values.value(i,0) = (maxValues[i] + minValues[i])/2.0;
			
			line = new QLineEdit;
			line->setText(QString::number(values.value(i,0) ));
			slidersLayout->addWidget(line,1+i,11,Qt::AlignLeft);
			valueline << line;
			connect(line,SIGNAL(editingFinished()),this,SLOT(valueChanged()));

			line = new QLineEdit;
			line->setText(QString::number(minValues[i]));
			slidersLayout->addWidget(line,1+i,12,Qt::AlignCenter);
			minline << line;
			min << minValues[i];
			connect(line,SIGNAL(editingFinished()),this,SLOT(minmaxChanged()));

			line = new QLineEdit;
			line->setText(QString::number(maxValues[i]));
			slidersLayout->addWidget(line,1+i,13,Qt::AlignLeft);
			maxline << line;
			max << maxValues[i];
			connect(line,SIGNAL(editingFinished()),this,SLOT(minmaxChanged()));
		}

		slidersWidget = new QWidget;
		slidersWidget->setLayout(slidersLayout);		
		
		QVBoxLayout * layout = new QVBoxLayout;
		QHBoxLayout * buttonLayout = new QHBoxLayout;		
		QPushButton * closeButton = new QPushButton(tr("Close"));
		buttonLayout->addWidget(closeButton);
		buttonLayout->addStretch(1);
		QScrollArea * scrollArea = new QScrollArea;
		scrollArea->setWidget(slidersWidget);
		scrollArea->setWidgetResizable(true);
		layout->addWidget(scrollArea);
		layout->addLayout(buttonLayout);
		connect(closeButton,SIGNAL(pressed()),this,SLOT(close()));
		setLayout(layout);
		
		valueChanged();
	}
}
