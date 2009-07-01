/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This file defines the class that is used to create new threads in the
 Tinkercell main window. The threads can be associated with a dialog that provides
 users with the option to terminate the thread.


****************************************************************************/


#ifndef TINKERCELL_CTHREAD_H
#define TINKERCELL_CTHREAD_H

#include <QMainWindow>
#include <QTextEdit>
#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>
#include <QDialog>
#include <QCompleter>
#include <QListWidget>
#include <QThread>
#include <QToolBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QActionGroup>
#include <QLibrary>
#include <QProcess>
#include <QProgressBar>
#include <QItemDelegate>
#include "Tool.h"
#include "TCstructs.h"
#include "DataTable.h"

namespace Tinkercell
{
	/*! \brief This class is used to mediate signals to a progress bar when the progress bar exists on a separate thread
		\ingroup helper
	*/
	class ProgressBarSignalItem : public QObject
	{
		Q_OBJECT
	signals:
		void progress(int);
	public:
		ProgressBarSignalItem(QObject * parent = 0): QObject(parent) {}
		ProgressBarSignalItem(const ProgressBarSignalItem & copy): QObject(copy.parent()) {}
		ProgressBarSignalItem& operator = (const ProgressBarSignalItem & copy) { setParent(copy.parent()); return *this; }
		void emitSignal(int i) { emit progress(i); }
                /*! \brief set progress on a thread with the given name*/
                static void setProgress(const char * name, int progress);
                /*! \brief hash stores the name and progress bar pointers for updating progress on different threads*/
                static QHash<QString,ProgressBarSignalItem> progressBars;
	};
	
        /*! \brief
            This class is used to run a particular function in a dynamic library file as a separate thread.
            IMPORTANT: The library is unloaded when the operation is complete.
		\ingroup core
	*/
	class LibraryThread : public QThread
	{
		Q_OBJECT
	public:
		/*! \brief constructor the input matrix, the library file, and the target function in the library 
		* \param QString dynamic library file name
		* \param QString name of funtion to run
		* \param MainWindow main window
		* \param Matrix matrix input
		*/
		LibraryThread(const QString&, const QString&, MainWindow* main, const Matrix& input);
		/*! \brief destructor -- free the library that this thread loaded 
		*/
		virtual ~LibraryThread();
		/*! \brief  creates a dialog that shows the name of the running thread and a button for terminating the thread
		* \param MainWindow main window
		* \param LibraryThread the dynamic library to run
		* \param QString text to display
		* \param QIcon icon to display
		*/
		static QDialog* ThreadDialog(MainWindow *, LibraryThread*, const QString& text = QString("Thread"), QIcon icon = QIcon());
		/*! \brief stores the output from the last run*/
		int lastOutput;
		/*! \brief the title of this program*/
		QString title;
	signals:
		/*! \brief signals the progress of this thread
		* \param int progress 0-100
		*/
		void progress(int);
	protected slots:
		/*! \brief unload the library (if loaded) and delete it*/
		void unloadLibrary();

	protected:
		/*! \brief inputs for the target function in the dll file*/
		Matrix M;
		/*! \brief the name of the dynamic library file without the suffix (Qt will figure this out)*/
		QString dllFile;
		/*! \brief the name of the target function in the dynamic library*/
		QString functionName;
		/*! \brief Tinkercell's main window*/
		MainWindow * mainWindow;
		/*! \brief the library pointer where the target library will be loaded*/
		QLibrary * library;
		/*! \brief initializes the function pointers through the main window and then runs the target function*/
		virtual void run();
	protected:
		/*! \brief signals the progress of this thread
		* \param int progress 1-100
		*/
		//void sendProgress(int);

	};
	
	
        /*! \brief This class is used to run a process (command + args) as a separate thread as a separate thread
		\ingroup core
	*/
	class ProcessThread : public QThread
	{
		Q_OBJECT
	public:
		/*! \brief constructor -- used to initialize the main window, the command name and the args for the command
		* \param QString command
		* \param QString arguments
		* \param MainWindow main window
		*/		
		ProcessThread(const QString&, const QString& ,MainWindow* main);
		/*! \brief destructor -- free the library that this thread loaded 
		*/
		virtual ~ProcessThread();
		/*! \brief  creates a dialog that shows the name of the running thread and a button for terminating the thread
		* \param MainWindow main window
		* \param ProcessThread
		* \param QString text to display
		* \param QIcon icon to display
		*/
		static QDialog* ThreadDialog(MainWindow *, ProcessThread*, const QString& text = QString("Process"), QIcon icon = QIcon());	
	protected slots:
		/*! \brief unload the library (if loaded) and delete it*/
		void stopProcess();
	protected:
		/*! \brief the name of the executable*/
		QString exe;
		/*! \brief the arguments*/
		QString args;
		/*! \brief Tinkercell's main window*/
		MainWindow * mainWindow;
		/*! \brief Tinkercell's main window*/
		QProcess process;
		/*! \brief initializes the function pointers through the main window and then runs the target function*/
		virtual void run();

		
	};

        typedef void (*inputMatrixFunction)(Matrix);

	/*! \brief Used to create an input window that can receive user inputs for C plugins
		\ingroup io
	*/
	class CInputWindow : public Tool
	{
		Q_OBJECT
	public:
		/*! \brief creates a docking window in Tinkercell's mainwindow that can receive inputs from user and 
				run a function in a separate thread
		* \param MainWindow
		* \param QString title
                * \param QString dynamic library file (will first search if already loaded in MainWindow)
		* \param QString function to run inside library
		* \param DataTable<double> inputs
		* \param QList<QStringList> options for the inputs (optional)
		*/
		static void CreateWindow(MainWindow * main, const QString& title, const QString& dllName, const QString& funcName, const DataTable<qreal>&);
                /*! \brief creates a docking window in Tinkercell's mainwindow that can receive inputs from user and
                                run a function in a separate thread
                * \param MainWindow
                * \param QString title
                * \param inputMatrixFunction* function that is triggered by the run button in the input window
                * \param QDataTable<qreal> input table and its default values
                */
                static void CreateWindow(MainWindow * main, const QString& title, inputMatrixFunction * func, const DataTable<qreal>&);
		/*! \brief add a list of options (combo box) to an existing input window
		* \param QString title
		* \param int row
		* \param int column
		* \param QStringList options
		*/
		static void AddOptions(QString title, int i, int j, const QStringList& options);
	protected:
                /*! \brief constructor that creates a docking window in Tinkercell's mainwindow that can receive inputs from user and
                                run a function in a separate thread
		* \param MainWindow
		* \param QString title
		* \param QString dynamic library file
		* \param QString function to run inside library
                * \param QDataTable<qreal> input table and its default values
		*/
		CInputWindow(MainWindow * main, const QString& title, const QString& dllName, const QString& funcName, const DataTable<qreal>&);
                /*! \brief constructor that creates a docking window in Tinkercell's mainwindow that can receive inputs from user and
                                run a function in a separate thread
                * \param MainWindow
                * \param QString title
                * \param inputMatrixFunction* function that is triggered by the run button in the input window
                * \param QDataTable<qreal> input table and its default values
                */
                CInputWindow(MainWindow * main, const QString& title, inputMatrixFunction * func, const DataTable<qreal>&);
		/*! \brief constructor -- does nothing*/
		CInputWindow();
		/*! \brief copy constructor*/
		CInputWindow(const CInputWindow&);
		/*! \brief the input matix*/
		DataTable<qreal> dataTable;
                /*! \brief target dll function -- only required if targetFunction is null*/
		QString dllFunction;
                /*! \brief function pointer -- if this pointer is not null, the dllFunction string is not needed*/
                inputMatrixFunction * targetFunction;
		/*! \brief reinitialize the contents on the input window*/
		virtual void setupDisplay(const QString&, const DataTable<qreal>&);
		/*! \brief the table displaying the input matrix*/
		QTableWidget tableWidget;
		/*! \brief combo boxes used in input window*/
		QList<QComboBox*> comboBoxes;
	protected slots:
		/*! \brief run the thread*/
		virtual void exec();
		/*! \brief updates the input matrix when user changes the table*/
		virtual void dataChanged(int,int);
		/*! \brief add a row to the input matrix*/
		virtual void addRow();
		/*! \brief remove a row from the input matrix*/
		virtual void removeRow();
		/*! \brief updates the input matrix when user changes the combo boxes*/
		virtual void comboBoxChanged(int);
	protected:
                /* \brief thread used to run the function in the CInputWindow*/
                class Thread : public QThread
                {
                    public:
                        Thread(CInputWindow* );
                    protected:
                        CInputWindow * window;
                        virtual void run();
                };
                /*! \brief delegate used inside the CInputWindow*/
		class ComboBoxDelegate : public QItemDelegate
		{
		public:
			ComboBoxDelegate(QObject *parent = 0);
			DataTable<QStringList> options;
			QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
			void setEditorData(QWidget *editor, const QModelIndex &index) const;
			void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
			void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const;
		};
		/*! \brief the item delegate that is used to change values in the input window*/
		ComboBoxDelegate delegate;
		/*! \brief the dock widget*/
		QDockWidget * dockWidget;
		/*! \brief make the window transparent when mouse exits the window*/
		void leaveEvent ( QEvent * event );
		/*! \brief make the window transparent when mouse exits the window*/
		void enterEvent ( QEvent * event );
		/*! \brief the set of all C-based input windows*/
		static QHash<QString,CInputWindow*> inputWindows;
	};
}

#endif
