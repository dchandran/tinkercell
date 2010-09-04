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
#include <QCheckBox>
#include <QGroupBox>
#include <QFile>
#include <QListWidget>

#include "MainWindow.h"
#include "NetworkWindow.h"
#include "GraphicsScene.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "ConsoleWindow.h"
#include "ConvertValue.h"
#include "CThread.h"

namespace Tinkercell
{

	/*!
	\brief Classes that inherit from this class can be used as GUI windows that provide interface to C programs (library files).
	\ingroup io
	\sa LPSolveInput
	*/
	class TINKERCELLEXPORT AbstractInputWindow : public Tool
	{
		Q_OBJECT

	protected:

		/*! \brief constructor
		* \param QString name of this tool
		* \param CThread the target thread to run from this input window
		*/
		AbstractInputWindow(const QString& name = tr(""), CThread * thread = 0);

		/*! \brief Sets the main window. This function will set this tool as a docked widget by default and registed the escapeSignal event.
		Overwrite this function to prevent that default behavior.
		*/
		virtual bool setMainWindow(MainWindow * main);

		/*! \brief set the input for this input window*/
		virtual void setInput(const DataTable<qreal>&);

		/*! \brief set the thread that will be started by this input window*/
		virtual void setThread(CThread *);

		/*! \brief the thread that will be started by this input window*/
		virtual CThread * thread() const;

	public slots:

		/*!
		\brief Escape signal is a request to stop the current process. This class will hide itself as a response.
		*/
		virtual void escapeSignal(const QWidget*);

		/*!
		\brief Executes the CThread
		\sa CThread
		*/
		virtual void exec();

		/*!
		\brief Uses MainWindow's setupNewThread function to setup this window's thread
		*/
		virtual void loadAPI(Tool*);

	protected:
		/*! \brief the target thread*/
		CThread * cthread;
		/*! \brief the docked window for this widget (0 if not a docked widget)*/
		QDockWidget * dockWidget;
	};

	/*! \brief Used to create an input window that can receive user inputs for C plugins
	\ingroup io
	*/
	class TINKERCELLEXPORT SimpleInputWindow : public AbstractInputWindow
	{
		Q_OBJECT
	public:
		/*! \brief Create a simple input window to run a CThread.
		The window can be used to fill in an input matrix.
		* \param MainWindow
		* \param QString title
		* \param QString dynamic library file (will first search if already loaded in MainWindow)
		* \param QString function to run inside library
		* \param DataTable<double> inputs
		* \param QList<QStringList> options for the inputs (optional)
		*/
		static void CreateWindow(MainWindow * main, const QString& title, const QString& libraryFile, const QString& funcName, const DataTable<qreal>&);
		/*! \brief creates a docking window in Tinkercell's mainwindow that can receive inputs from user and
		run a function in a separate thread
		* \param MainWindow
		* \param QString title
		* \param inputtc_matrixFunction* function that is triggered by the run button in the input window
		* \param QDataTable<qreal> input table and its default values
		*/
		static void CreateWindow(MainWindow * main, const QString& title, void (*f)(tc_matrix), const DataTable<qreal>&);
		/*! \brief add a list of options (combo box) to an existing input window
		* \param QString title
		* \param int row
		* \param int column
		* \param QStringList options
		*/
		static void AddOptions(const QString& title, int i, int j, const QStringList& options);
		/*! \brief add a check box to an existing input window
		* \param QString title
		* \param int row
		* \param int column
		*/
		static void AddOptions(const QString& title, int i, int j);
	public slots:
		/*!
		\brief Executes the CThread
		\sa CThread
		*/
		virtual void exec();
	protected:
		/*! \brief constructor that creates a docking window in Tinkercell's mainwindow that can receive inputs from user and
		run a function in a separate thread
		* \param MainWindow
		* \param QString title
		* \param QString dynamic library file
		* \param QString function to run inside library
		* \param QDataTable<qreal> input table and its default values
		*/
		SimpleInputWindow(MainWindow * main, const QString& title, const QString& dllName, const QString& funcName, const DataTable<qreal>&);
		/*! \brief constructor that creates a docking window in Tinkercell's mainwindow that can receive inputs from user and
		run a function in a separate thread
		* \param MainWindow
		* \param QString title
		* \param inputtc_matrixFunction* function that is triggered by the run button in the input window
		* \param QDataTable<qreal> input table and its default values
		*/
		SimpleInputWindow(MainWindow * main, const QString& title, void (*f)(tc_matrix), const DataTable<qreal>&);
		/*! \brief constructor -- does nothing*/
		SimpleInputWindow();
		/*! \brief copy constructor*/
		SimpleInputWindow(const SimpleInputWindow&);
		/*! \brief the input matix*/
		DataTable<qreal> dataTable;
		/*! \brief reinitialize the contents on the input window*/
		virtual void setupDisplay(const DataTable<qreal>&);
		/*! \brief the table displaying the input matrix*/
		QTableWidget tableWidget;
		/*! \brief combo boxes used in input window*/
		QList<QComboBox*> comboBoxes;
	protected slots:
		/*! \brief updates the input matrix when user changes the table*/
		virtual void dataChanged(int,int);
		/*! \brief add a row to the input matrix*/
		virtual void addRow();
		/*! \brief remove a row from the input matrix*/
		virtual void removeRow();
		/*! \brief updates the input matrix when user changes the combo boxes*/
		virtual void comboBoxChanged(int);
	protected:
		/*! \brief delegate used inside the SimpleInputWindow*/
		class TINKERCELLEXPORT ComboBoxDelegate : public QItemDelegate
		{
		public:
			ComboBoxDelegate(QObject *parent = 0);
			/*! \brief options for the combo boxes. Uses line edits if empty. Uses check boxes if just one item*/
			DataTable<QStringList> options;
			QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
			void setEditorData(QWidget *editor, const QModelIndex &index) const;
			void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
			void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const;
		};
		/*! \brief the item delegate that is used to change values in the input window*/
		ComboBoxDelegate delegate;
		/*! \brief make the window transparent when mouse exits the window*/
		void leaveEvent ( QEvent * event );
		/*! \brief make the window transparent when mouse exits the window*/
		void enterEvent ( QEvent * event );
		/*! \brief the set of all simple input windows*/
		static QHash<QString,SimpleInputWindow*> inputWindows;
	};

}
#endif
