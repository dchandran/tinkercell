/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This file defines an abstract class that is used to create interfaces for C functions.
 LPSolveInputWindow is a good example.

****************************************************************************/

#ifndef TINKERCELL_ABSTRACTINPUTWINDOW_H
#define TINKERCELL_ABSTRACTINPUTWINDOW_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
#include <QPixmap>
#include <QString>
#include <QGroupBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QToolButton>
#include <QTableWidget>
#include <QListWidget>
#include <QAction>
#include <QRadioButton>
#include <QComboBox>
#include <QGroupBox>
#include <QFile>
#include <QListWidget>

#include "muParserDef.h"
#include "muParser.h"
#include "muParserInt.h"
#include "MainWindow.h"
#include "NetworkWindow.h"
#include "GraphicsScene.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "OutputWindow.h"
#include "ConvertValue.h"


namespace Tinkercell
{

/*!
  \brief Classes that inherit from this class can be used as GUI windows that provide interface to C programs (library files).
  \sa LPSolveInput
*/
class AbstractInputWindow : public Tool
{
	Q_OBJECT

protected:

	/*! \brief constructor
	* \param QString name of this tool, e.g. "Flux Balance Analysis Input Window"
	* \param QString name of the C library file (WITHOUT the suffix .dll, .so, .dylib), e.g. "dlls/lpsolve"
	* \param QString name of the function to call inside the C library file, e.g. "run"
	*/
	AbstractInputWindow(const QString& name, const QString& fileName, const QString& functionName);

	/*! \brief Sets the main window. This function will set this tool as a docked widget by default and registed the escapeSignal event.
		Overwrite this function to prevent that default behavior.
	*/
	virtual bool setMainWindow(MainWindow * main);

protected slots:

	/*!
		\brief Escape signal is a request to stop the current process. This class will hide itself as a response.
	*/
	virtual void escapeSignal(const QWidget*);

	/*!
		\brief Executes the library file as a new thread (LibraryThread)
		\sa LibraryThread
	*/
	virtual void exec();

protected:
	/*! \brief mouse enter event*/
	void enterEvent ( QEvent * event );
	/*! \brief mouse exit event*/
	void leaveEvent ( QEvent * event );
	/*! \brief the C library file name*/
	QString fileName;
	/*! \brief the function name inside the library*/
	QString functionName;
	/*! \brief the input that the C function will receive*/
	DataTable<qreal> dataTable;
	/*! \brief the docked window for this widget (0 if not a docked widget)*/
	QDockWidget * dockWidget;
	/*! \brief checks whether a string is a correct formula*/
	static bool parseMath(NetworkWindow* scene,QString& s);
};

}
#endif
