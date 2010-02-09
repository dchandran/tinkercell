/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

A slider widget that calls a C function whenver values in the slider are changed.
Uses CThread.

****************************************************************************/

#ifndef TINKERCELL_MULTITHREADEDSLIDER_H
#define TINKERCELL_MULTITHREADEDSLIDER_H

#include <QWidget>
#include <QLineEdit>
#include <QSlider>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include "CThread.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{
	/*! \brief This class is used to run specific functions inside a C dynamic library
	as a separate thread. Uses CThread to call the C functions
	\ingroup core
	*/
	class MY_EXPORT MultithreadedSliderWidget : public QWidget
	{
		Q_OBJECT

	signals:
		
		/*!
		* \brief the options in the slider have changed
		*/
		void optionsChanged(const QStringList&);
		
		/*!
		* \brief the values in the slider have changed
		*/
		void valuesChanged(const QList<double>&);

	public slots:
	
		/*!
		* \brief setup the slide options and initial values
		* \param QStringList names for the sliders
		* \param QList<double> minimum value for each of the sliders
		* \param QList<double> maximum value for each of the sliders
		*/
		virtual void setSliders(const QStringList& options, const QList<double>& minValues, const QList<double>& maxValues);
		
	public:

		/*!
		* \brief constructor
		* \param QWidget * parent
		* \param CThread * the thread that is already setup with the correct library and function
		* \param Qt::Orientation orientation
		*/
		MultithreadedSliderWidget(MainWindow * parent, CThread * thread, Qt::Orientation orientation = Qt::Horizontal);

		/*!
		* \brief constructor
		* \param QWidget * parent
		* \param QString the name of the dynamic library to load
		* \param QString name of function in the library with signature void f(Matrix)
		* \param Qt::Orientation orientation
		*/
		MultithreadedSliderWidget(MainWindow * parent, const QString & lib, const QString & functionName, Qt::Orientation orientation = Qt::Horizontal);
		
		/*!
		* \brief destructor
		*/
		~MultithreadedSliderWidget();
		
		/*!
		* \brief the cthread that is run every time the sliders change
		*/
		virtual CThread * thread() const;

	protected slots:

		/*!
		* \brief whenver the value text change, the function in the C library is called
		*/
		virtual void valueChanged();
		
		/*!
		* \brief whenver the sliders change, the function in the C library is called
		*/
		virtual void sliderChanged();
		
		/*!
		* \brief whenver the text change, the function in the C library is called
		*/
		virtual void minmaxChanged();

	protected:
	
		/*!
		* \brief whenver the slides change, cthread->start() is called
		*/
		CThread * cthread;
	
		Qt::Orientation orientation;
		
		DataTable<qreal> values;
		
		QList< QLabel* > labels;
 
		QList< QSlider* > sliders;
		
		QList< QLineEdit* > minline, maxline, valueline;
		
		QList<double> min, max;
		
		QWidget * slidersWidget;
		
		QGridLayout * slidersLayout;	
		
		virtual void initialLayout(const QStringList& options, const QList<double>& minValues, const QList<double>& maxValues);
	};
}

#endif
