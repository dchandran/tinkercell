/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This file defines the class that is used to create new threads in the
 Tinkercell main window. The threads can be associated with a dialog that provides
 users with the option to terminate the thread.


****************************************************************************/

#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "CThread.h"
#include "OutputWindow.h"
#include <QVBoxLayout>
#include <QDockWidget>
#include <QDir>
#include <QCoreApplication>
#include <QtDebug>

namespace Tinkercell
{

    /******************
         LIBRARY THREAD
         *******************/

//     QHash<QString,ProgressBarSignalItem> LibraryThread::progressBars;

    void ProgressBarSignalItem::setProgress(const char * name, int progress)
    {
        QString s(name);
//         if (progressBars.contains(s))
//         {
//             progressBars[s].emitSignal(progress);
//         }
    }

    typedef void (*TinkercellPluginRunFunction)(MainWindow*);

    QDialog* LibraryThread::ThreadDialog(MainWindow * mainWindow, LibraryThread * newThread, const QString& text, QIcon icon)
    {
        if (!mainWindow || !newThread) return 0;

        QString home = MainWindow::userHome(),
        current = QDir::currentPath(),
        appDir = QCoreApplication::applicationDirPath();

        QString dllFile = newThread->dllFile;

        QString name[] = {	home + tr("/") + PROJECTNAME + tr("/") + dllFile,
                                current + tr("/") + dllFile,
                                appDir + tr("/") + dllFile,
                                dllFile };

        QLibrary lib;

        bool loaded = false;
        for (int i=0; i < 4; ++i) //try different possibilities
        {
            lib.setFileName(name[i]);
            loaded = lib.load();
            if (loaded)
                break;
        }

        if (loaded)
        {
            TinkercellPluginRunFunction f = (TinkercellPluginRunFunction)lib.resolve("loadTCTool");
            if (f)
            {
                try
                {
                    f(mainWindow);
                }
                catch(...)
                {
                }
                delete newThread;
                return 0;
            }
            else
            {
                lib.unload();
            }
        }

        QDialog * dialog = new QDialog(mainWindow);
        dialog->move(100,100);
        dialog->setWindowIcon(icon);

        QVBoxLayout * layout = new QVBoxLayout;
        QPushButton * killButton = new QPushButton("Terminate Program");
        connect(killButton,SIGNAL(released()),dialog,SLOT(accept()));
        QLabel * label1 = new QLabel(text);
        QProgressBar * progressbar = new QProgressBar;

        layout->addWidget(label1);
        layout->addWidget(progressbar);
        layout->addWidget(killButton);
        dialog->setWindowTitle(text);

        dialog->setLayout(layout);

        connect(dialog,SIGNAL(accepted()),newThread,SLOT(terminate()));
        connect(newThread,SIGNAL(finished()),dialog,SLOT(reject()));

        progressbar->setRange(0,100);

        //connect(newThread,SIGNAL(progress(int)),progressbar,SLOT(setValue(int)));

        newThread->title = text;
//         ProgressBarSignalItem::progressBars[text] = ProgressBarSignalItem();
//         connect(&ProgressBarSignalItem::progressBars[text],SIGNAL(progress(int)),progressbar,SLOT(setValue(int)));
//         connect(&ProgressBarSignalItem::progressBars[text],SIGNAL(progress(int)),newThread,SIGNAL(progress(int)));

        dialog->show();

        return dialog;
    }

    LibraryThread::LibraryThread(const QString& dll, const QString& func, MainWindow* main, const Matrix& input)
        : QThread(main)
    {
        library = 0;
        title = dllFile = dll;
        functionName = func;
        mainWindow = main;
        M = input;
        connect(this,SIGNAL(terminated()),this,SLOT(unloadLibrary()));
        connect(this,SIGNAL(finished()),this,SLOT(unloadLibrary()));
    }

    void LibraryThread::unloadLibrary()
    {
//         progressBars.remove(title);
        disconnect(this);

        if (library)
        {
            if (library->isLoaded())
                library->unload();
            delete library;
            library = 0;
        }
        if (M.rownames)
        {
            for (int i=0; M.rownames != 0 && M.rownames[i] !=0; ++i)
                delete M.rownames[i];
            delete M.rownames;
        }
        if (M.colnames)
        {
            for (int i=0; M.colnames != 0 && M.colnames[i] !=0; ++i)
                delete M.colnames[i];
            delete M.colnames;
        }
        if (M.values) delete M.values;

        M.rownames = 0;
        M.colnames = 0;
        M.values = 0;
    }


    typedef void (*progress_api_initialize)(void (*tc_showProgress)(const char *, int));

    void LibraryThread::run()
    {
        if (mainWindow && !dllFile.isEmpty())
        {
            //setPriority(QThread::LowestPriority);
            QString appDir = QCoreApplication::applicationDirPath();
            library = new QLibrary;

            QString name[] = {	MainWindow::userHome() + tr("/") + PROJECTNAME + tr("/") + dllFile,
                                dllFile,
                                QDir::currentPath() + tr("/") + dllFile,
                                appDir + tr("/") + dllFile };


            bool loaded = false;
            for (int i=0; i < 4; ++i) //try different possibilities
            {
                library->setFileName(name[i]);
                loaded = library->load();
                if (loaded)
                    break;
            }

            if (loaded)
            {


//                 dllFunc f = (dllFunc)library->resolve(functionName.toAscii().data());
//                 if (f)
//                 {
//                     QString currentDir = QDir::currentPath();
//
//                     QString tcdir("Tinkercell");
//
//                     QDir dir(MainWindow::userHome());
//                     if (!dir.exists(tcdir))
//                     {
//                         dir.mkdir(tcdir);
//                     }
//
//                     dir.cd(tcdir);
//                     QDir::setCurrent(dir.absolutePath());
//
//                     try
//                     {
//                         QSemaphore * s = new QSemaphore(1);
//                         s->acquire();
//                         mainWindow->setupNewThread(s,library);
//                         s->acquire();
//                         s->release();
//                         lastOutput = f(M);
//
//                     }
//                     catch(...)
//                     {
//                     }
//
//                     QDir::setCurrent(currentDir);
//                 }
//                 else
//                 {
//                     mainFunc main = (mainFunc)library->resolve("main");
//                     if (main)
//                     {
//                         QString currentDir = QDir::currentPath();
//
//                         QString tcdir("Tinkercell");
//
//                         QDir dir(MainWindow::userHome());
//                         if (!dir.exists(tcdir))
//                         {
//                             dir.mkdir(tcdir);
//                         }
//
//                         dir.cd(tcdir);
//                         QDir::setCurrent(dir.absolutePath());
//
//                         try
//                         {
//                             QSemaphore * s = new QSemaphore(1);
//                             s->acquire();
//                             mainWindow->setupNewThread(s,library);
//                             s->acquire();
//                             s->release();
//                             lastOutput = main();
//                         }
//                         catch(...)
//                         {
//                         }
//
//                         QDir::setCurrent(currentDir);
//
//                     }
//                 }
//                 library->unload();
            }
        }
    }

    LibraryThread::~LibraryThread()
    {
        unloadLibrary();
    }

    /******************
         PROCESS THREAD
         *******************/
    QDialog* ProcessThread::ThreadDialog(MainWindow * mainWindow, ProcessThread * newThread, const QString& text, QIcon icon)
    {
        QDialog * dialog = new QDialog(mainWindow);
        dialog->move(100,100);
        dialog->setWindowIcon(icon);

        QGridLayout * layout = new QGridLayout;
        QPushButton * killButton = new QPushButton("Terminate Program");
        connect(killButton,SIGNAL(released()),dialog,SLOT(accept()));
        QLabel * label1 = new QLabel(text + tr(" is Running..."));

        layout->addWidget(label1,0,0,Qt::AlignCenter);
        layout->addWidget(killButton,1,0,Qt::AlignCenter);
        dialog->setWindowTitle(tr("Program Running"));

        dialog->setLayout(layout);

        connect(dialog,SIGNAL(accepted()),newThread,SLOT(terminate()));
        connect(newThread,SIGNAL(finished()),dialog,SLOT(reject()));

        dialog->show();

        return dialog;
    }

    ProcessThread::ProcessThread(const QString& exe, const QString& args,MainWindow* main)
        : QThread(main)
    {
        this->args = args;
        this->exe = exe;
        mainWindow = main;

        connect(this,SIGNAL(terminated()),this,SLOT(stopProcess()));
        connect(this,SIGNAL(finished()),this,SLOT(stopProcess()));
    }

    void ProcessThread::run()
    {
        if (mainWindow && !exe.isEmpty())
        {
            QString currentDir = QDir::currentPath();

            QString tcdir("Tinkercell");

            QDir dir(MainWindow::userHome());
            if (!dir.exists(tcdir))
            {
                dir.mkdir(tcdir);
            }

            dir.cd(tcdir);
            QDir::setCurrent(dir.absolutePath());

            //setPriority(QThread::LowestPriority);
            connect(this,SIGNAL(terminated()),&process,SLOT(kill()));
            process.start(exe,QStringList() << args);
            process.waitForFinished();
            QString errors(process.readAllStandardError());
            QString output(process.readAllStandardOutput());
            OutputWindow::error(errors);

            QDir::setCurrent(currentDir);
        }
    }

    void ProcessThread::stopProcess()
    {
        if (process.state() != QProcess::NotRunning)
            process.close();
    }

    ProcessThread::~ProcessThread()
    {
        stopProcess();
    }


    /******************************
             INPUT WINDOW
         *******************************/

    QHash<QString,CInputWindow*> CInputWindow::inputWindows;

    CInputWindow::CInputWindow(MainWindow * main, const QString& title, const QString& dllName, const QString& funcName, const DataTable<qreal>& data)
                : Tool(dllName)
    {
        dockWidget = 0;
        setMainWindow(main);
        if (mainWindow)
        {
            dockWidget = mainWindow->addDockingWindow(title,this,Qt::BottomDockWidgetArea,Qt::BottomDockWidgetArea,false);
            dockWidget->setAttribute(Qt::WA_ContentsPropagated);
            dockWidget->setFloating(true);
            dockWidget->setWindowOpacity(0.8);
        }

        QPushButton * runButton = new QPushButton(this);
        runButton->setIcon(QIcon(":/images/play.png"));
        connect(runButton,SIGNAL(released()),this,SLOT(exec()));

        QToolButton * addButton = new QToolButton(this);
        addButton->setIcon(QIcon(":/images/plus.png"));
        connect(addButton,SIGNAL(released()),this,SLOT(addRow()));

        QToolButton * removeButton = new QToolButton(this);
        removeButton->setIcon(QIcon(":/images/minus.png"));
        connect(removeButton,SIGNAL(released()),this,SLOT(removeRow()));

        QVBoxLayout * layout = new QVBoxLayout;
        layout->addWidget(&tableWidget,1);

        QHBoxLayout * hlayout = new QHBoxLayout;
        hlayout->addWidget(runButton,3);
        hlayout->addStretch(6);
        hlayout->addWidget(addButton,0);
        hlayout->addWidget(removeButton,0);
        layout->addLayout(hlayout,0);

        setLayout(layout);

        tableWidget.setAlternatingRowColors(true);
        tableWidget.setItemDelegate(&delegate);
        tableWidget.setEditTriggers ( QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed );

        setupDisplay(funcName,data);

        inputWindows[title] = this;
    }

    void CInputWindow::AddOptions(QString title, int i, int j, const QStringList& options0)
    {
        if (inputWindows.contains(title))
        {
            CInputWindow * win = inputWindows[title];
            if (!win) return;

            QStringList options = options0;
            for (int k=0; k < options.size(); ++k)
                options[k].replace(tr("_"),tr("."));

            win->delegate.options.value(i,j) = options;

            if (win->tableWidget.item(i,j) && !options.contains(win->tableWidget.item(i,j)->text()))
                win->tableWidget.item(i,j)->setText(options[ (int)(win->dataTable.value(i,j)) ]);
        }
    }

    CInputWindow::CInputWindow() : Tool(), dockWidget(0) { }
    CInputWindow::CInputWindow(const CInputWindow&) : Tool(), dockWidget(0) { }

    void CInputWindow::CreateWindow(MainWindow * main, const QString& title, const QString& dllName, const QString& funcName, const DataTable<qreal>& data)
    {
        if (!main || dllName.isEmpty() || funcName.isEmpty()) return;
        CInputWindow * inputWindow = 0;
        if (main->tool(dllName))
        {
            inputWindow = static_cast<CInputWindow*>(main->tool(dllName));
            if (inputWindow)
            {
                //inputWindow->setupDisplay(funcName,data);
                if (inputWindow->parentWidget())
                    inputWindow->parentWidget()->show();
                else
                    inputWindow->show();
            }
        }
        else
        {
            inputWindow = new CInputWindow(main,title,dllName,funcName,data);
        }
    }

    void CInputWindow::addRow()
    {
        int r = tableWidget.rowCount();
        tableWidget.setRowCount(r+1);

        int n = r;
        QString name = tr("row") + QString::number(n);

        if (dataTable.rows() > 0)
        {
            name = dataTable.rowName( dataTable.rows()-1 );
            if (name.at( name.length()-1 ).isNumber())
            {
                name.chop(1);
                name = name + QString::number(n);
            }
            else
            {
                name = tr("row") + QString::number(n);
            }
        }
        QStringList names = dataTable.getRowNames();

        while (names.contains(name))  //find a unique row name
        {
            ++n;
            name.chop(1);
            name = name + QString::number(n);
        }

        tableWidget.setVerticalHeaderItem ( r, new QTableWidgetItem(name) );
        dataTable.insertRow(r,name);

        for (int i=0; i < tableWidget.columnCount(); ++i)
        {
            if (r > 0)
            {
                if (tableWidget.item(r-1,i))
                    tableWidget.setItem(r,i,new QTableWidgetItem( tableWidget.item(r-1,i)->text() ));
                dataTable.value(r,i) = dataTable.value(r-1,i);
                delegate.options.value(r,i) = delegate.options.value(r-1,i);
            }
            else
            {
                tableWidget.setItem(r,i,new QTableWidgetItem("0.0"));
                dataTable.value(r,i) = 0.0;
                delegate.options.value(r,i) = QStringList();
            }
        }
    }

    void CInputWindow::removeRow()
    {
        int r = tableWidget.rowCount()-1;
        tableWidget.removeRow(r);
        dataTable.removeRow(r);
    }

    void CInputWindow::dataChanged(int i,int j)
    {
        QString s = tableWidget.item(i,j)->text();

        if (!delegate.options.value(i,j).isEmpty() && delegate.options.value(i,j).contains(s))
        {
            dataTable.value(i,j) = delegate.options.value(i,j).indexOf(s);
            return;
        }

        bool ok;

        double d = s.toDouble(&ok);

        if (ok)
            dataTable.value(i,j) = d;
    }

    void CInputWindow::setupDisplay(const QString& funcName, const DataTable<qreal>& table)
    {
        tableWidget.disconnect();

        dllFunction = funcName;
        dataTable = table;
        comboBoxes.clear();
        int r = dataTable.rows();
        int c = dataTable.cols();
        tableWidget.clear();
        tableWidget.setColumnCount(c);
        tableWidget.setRowCount(r);
        tableWidget.setVerticalHeaderLabels(dataTable.getRowNames());
        tableWidget.setHorizontalHeaderLabels(dataTable.getColNames());

        for (int i=0; i < dataTable.rows(); ++i)
            for (int j=0; j < dataTable.cols(); ++j)
            {
            tableWidget.setItem(i,j,new QTableWidgetItem(QString::number(dataTable.at(i,j))));
            delegate.options.value(i,j) = QStringList();
        }

        connect(&tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(dataChanged(int,int)));
    }

    void CInputWindow::comboBoxChanged(int)
    {

    }

    void CInputWindow::exec()
    {
//         Matrix m = ConvertValue(dataTable);

//         Thread * newThread = new Thread(name,dllFunction,mainWindow,m);
//         LibraryThread::ThreadDialog(mainWindow,newThread,name + tr(": ") + dllFunction);
//         newThread->start(); //go
    }

    CInputWindow::Thread::Thread(CInputWindow * win) : QThread(win), window(win)
    {
    }

    void CInputWindow::Thread::run()
    {
        if (window && (!window->dllFunction.isEmpty() || window->targetFunction))
        {
             Matrix m = ConvertValue(window->dataTable);
            //setPriority(QThread::LowestPriority);
            if (window->targetFunction)
            {
//                 window->targetFunction(m);
            }

            QString appDir = QCoreApplication::applicationDirPath();
//             library = new QLibrary;

//             QString name[] = {	MainWindow::userHome() + tr("/") + PROJECTNAME + tr("/") + dllFile,
//                                 dllFile,
//                                 QDir::currentPath() + tr("/") + dllFile,
//                                 appDir + tr("/") + dllFile };


            bool loaded = false;
            for (int i=0; i < 4; ++i) //try different possibilities
            {
//                 library->setFileName(name[i]);
//                 loaded = library->load();
                if (loaded)
                    break;
            }

            if (loaded)
            {
//                 progress_api_initialize f0 = (progress_api_initialize)library->resolve("tc_Progress_api_initialize");
//                 if (f0)
//                 {
//                     f0(&(setProgress));
//                 }

// /*                dllFunc f = (dllFunc)library->resolve(functionName.toAscii().data());
//                 if (f)
//                 {
//                     QString currentDir = QDir::currentPath();
//
//                     QString tcdir("Tinkercell");
//
//                     QDir dir(MainWindow::userHome());
//                     if (!dir.exists(tcdir))
//                     {
//                         dir.mkdir(tcdir);
//                     }
//
//                     dir.cd(tcdir);
//                     QDir::setCurrent(dir.absolutePath());
//
//                     try
//                     {
//                         QSemaphore * s = new QSemaphore(1);
//                         s->acquire();
//                         mainWindow->setupNewThread(s,library);
//                         s->acquire();
//                         s->release();
//                         lastOutput = f(M);
//
//                     }
//                     catch(...)
//                     {
//                     }
//
//                     QDir::setCurrent(currentDir);
//                 }
//                 else
//                 {
//                     mainFunc main = (mainFunc)library->resolve("main");
//                     if (main)
//                     {
//                         QString currentDir = QDir::currentPath();
//
//                         QString tcdir("Tinkercell");
//
//                         QDir dir(MainWindow::userHome());
//                         if (!dir.exists(tcdir))
//                         {
//                             dir.mkdir(tcdir);
//                         }
//
//                         dir.cd(tcdir);
//                         QDir::setCurrent(dir.absolutePath());
//
//                         try
//                         {
//                             QSemaphore * s = new QSemaphore(1);
//                             s->acquire();
//                             mainWindow->setupNewThread(s,library);
//                             s->acquire();
//                             s->release();
//                             lastOutput = main();
//                         }
//                         catch(...)
//                         {
//                         }
//
//                         QDir::setCurrent(currentDir);
//
//                     }
//                 }
//                 library->unload();*/
            }
        }
    }

    void CInputWindow::enterEvent ( QEvent * event )
    {
        //if (dockWidget)
        //dockWidget->setWindowOpacity(1.0);
    }

    void CInputWindow::leaveEvent ( QEvent * event )
    {
        //if (dockWidget)
        //dockWidget->setWindowOpacity(0.8);
    }

    /************************************
           ITEM DELEGATE FOR THE INPUT TABLE
        *************************************/

    CInputWindow::ComboBoxDelegate::ComboBoxDelegate(QObject *parent) : QItemDelegate(parent)
    {
    }

    QWidget * CInputWindow::ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex & index) const
    {
        if (options.at(index.row(),index.column()).size() > 0)
        {
            QComboBox *editor = new QComboBox(parent);
            editor->addItems(options.at(index.row(),index.column()));
            return editor;
        }
        else
        {
            /*QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
                        editor->setMinimum(-1e300);
                        editor->setMaximum(1E300);
                        editor->setDecimals(10);*/
            QLineEdit *editor = new QLineEdit(parent);
            return editor;
        }
    }

    void CInputWindow::ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        if (options.at(index.row(),index.column()).size() > 0)
        {
            QString value = index.model()->data(index, Qt::EditRole).toString();
            QComboBox *combo = static_cast<QComboBox*>(editor);
            combo->setCurrentIndex(options.at(index.row(),index.column()).indexOf(value));
        }
        else
        {
            double value = index.model()->data(index, Qt::EditRole).toDouble();
            /*QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
                        spinBox->setValue(value);*/
            QLineEdit * lineEdit = static_cast<QLineEdit*>(editor);
            lineEdit->setText(QString::number(value));
        }
    }

    void CInputWindow::ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        if (options.at(index.row(),index.column()).size() > 0)
        {
            QComboBox *combo = static_cast<QComboBox*>(editor);
            QString value = combo->currentText();
            model->setData(index, value, Qt::EditRole);
        }
        else
        {
            /*QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
                        spinBox->interpretText();
                        double value = spinBox->value();*/
            QLineEdit * lineEdit = static_cast<QLineEdit*>(editor);

            bool ok;
            double value = lineEdit->text().toDouble(&ok);
            if (ok)
            {
                model->setData(index, value, Qt::EditRole);
            }
        }
    }

    void CInputWindow::ComboBoxDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
    {
        editor->setGeometry(option.rect);
    }

}
