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
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QHash>
#include "CThread.h"

namespace Tinkercell
{
	/*! \brief This class is used to run specific functions inside a C dynamic library
	as a separate thread. Uses CThread to call the C functions
	\ingroup io
	*/
	class TINKERCELLEXPORT MultithreadedSliderWidget : public QWidget
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
		* \brief setup the sliders options and initial values
		* \param QStringList names for the sliders
		* \param QList<double> minimum value for each of the sliders
		* \param QList<double> maximum value for each of the sliders
		*/
		virtual void setSliders(const QStringList& options, const QList<double>& minValues, const QList<double>& maxValues);
		
		/*!
		* \brief set the sliders visible
		* \param QStringList names for the sliders
		*/
		virtual void setVisibleSliders(const QStringList& options);
		
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
		* \brief the cthread that is run every time the sliders change
		*/
		virtual CThread * thread() const;
		
		/*!
		* \brief table containing the variables, current values, min and max
		*/
		virtual DataTable<qreal> data() const;

	protected slots:

		/*!
		* \brief whenver the value text change, the function in the C library is called
		*/
		virtual void valueChanged();
		
		/*!
		* \brief whenver the sliders change, the function in the C library is called
		*/
		virtual void sliderChanged(int);
		
		/*!
		* \brief whenver the text change, the function in the C library is called
		*/
		virtual void minmaxChanged();

	protected:
	
		/*!
		* \brief whenver the slides change, cthread->start() is called
		*/
		CThread * cthread;	
		/*!
		* \brief orientation of the sliders
		*/
		Qt::Orientation orientation;		
		/*!
		* \brief table storing slider values
		*/
		DataTable<qreal> values;
		/*!
		* \brief slider labels in same order as sliders list
		*/
		QList< QLabel* > labels;
		/*!
		* \brief all the sliders
		*/
		QList< QSlider* > sliders;
		/*!
		* \brief slider min, max, and values in same order as sliders list
		*/
		QList< QLineEdit* > minline, maxline, valueline;
		/*!
		* \brief slider min and max in same order as sliders list
		*/
		QList<double> min, max;
		/*!
		* \brief slider layout
		*/
		QVBoxLayout * slidersLayout;
		/*!
		* \brief sliders by name
		*/
		QHash< QString, QWidget* > sliderWidgets;
		/*!
		* \brief main window
		*/
		MainWindow * mainWindow;
	};
}

#endif
