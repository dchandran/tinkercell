/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Provides a toolbar with buttons that call C functions (run of separate threads)
 
****************************************************************************/
#include <QVBoxLayout>
#include <QDockWidget>
#include <QProcess>
#include <QCompleter>
#include <QStringListModel>
#include <QLibrary>
#include <QRegExp>
#include <QDir>
#include <QFile>
#include "Core/GraphicsScene.h"
#include "Core/MainWindow.h"
#include "Core/OutputWindow.h"
#include "Core/NodeGraphicsItem.h"
#include "Core/ConnectionGraphicsItem.h"
#include "Core/TextGraphicsItem.h"
#include "Core/OutputWindow.h"
#include "DynamicCodeTools/PythonTool.h"
#include <QtDebug>

namespace Tinkercell
{
    PythonTool::PythonTool() : Tool(tr("Python Interpreter")), actionsGroup(this), buttonsGroup(this)
    {
        pythonInterpreter = 0;

        connect(&actionsGroup,SIGNAL(triggered ( QAction *  )),this,SLOT(actionTriggered ( QAction *  )));
        connect(&buttonsGroup,SIGNAL(buttonPressed ( int  )),this,SLOT(buttonPressed ( int  )));

        connectTCFunctions();
    }

    void PythonTool::loadFromFile(DynamicLibraryMenu * libMenu, QFile& file)
    {
        if (!libMenu) return;

        QStringList lines;

        while (!file.atEnd())
        {
            QByteArray line = file.readLine();
            if (!line.isEmpty())
                lines << line.trimmed();
        }
		
		QString desc, pyFile, functionName, categoryName, iconfile, menu,  family, tool;

        QString appDir = QCoreApplication::applicationDirPath();

        for (int i=0; i < lines.size(); ++i)
        {
            QStringList ls = lines[i].split(tr(";"));

            if (ls.size() >= 3)
            {
                desc = ls[0].trimmed();
                pyFile = ls[1].trimmed();
                functionName = ls[2].trimmed();
                QStringList functionNameSplit = functionName.split(tr("::"));

                if (functionNameSplit.size() >= 2)
                {
                    functionName = functionNameSplit[1];
                    categoryName = functionNameSplit[0];
                }
                else
                {
                    functionName = functionNameSplit[0];
                    categoryName = tr("misc.");
                }

                menu = tr("menu");
                family = tr("");
                tool = tr("notool");
                iconfile = tr("");
                if (ls.size() > 3)
                {
                    iconfile = ls[3].trimmed();

                    if (!QFile(iconfile).exists())
                        iconfile = appDir + tr("/") + iconfile;

                    if (ls.size() > 4)
                    {
                        menu = ls[4].trimmed();

                        if (ls.size() > 5)
                        {
                            family = ls[5].trimmed();

                            if (ls.size() > 6)
                                tool = ls[6].trimmed();
                        }
                    }
                }
		
				QPixmap pixmap(iconfile);

                QToolButton * button = libMenu->addFunction(categoryName, functionName, QIcon(pixmap));

                if (button)
                {
                    button->setToolTip(desc);
                    buttonsGroup.addButton(button,pyFileNames.size());
                    pyFileNames << pyFile;
                }

                if (menu == tr("menu"))
                {
                    QAction * menuItem = libMenu->addMenuItem(functionName,QIcon(pixmap));
                    if (menuItem)
                    {
                        menuItem->setToolTip(desc);
                        actionsGroup.addAction(menuItem);
                        hashPyFile[menuItem] = pyFile;
                    }

                    if (!family.isEmpty())
                    {
                        QAction * contextAction = libMenu->addContextMenuItem(family, functionName, pixmap, tool==tr("tool"));
                        if (contextAction)
                        {
                            contextAction->setToolTip(desc);
                            actionsGroup.addAction(contextAction);
                            hashPyFile[contextAction] = pyFile;
                        }
                    }
                }
            }
        }
		
    }

    bool PythonTool::setMainWindow(MainWindow * main)
    {
        Tool::setMainWindow(main);
        if (mainWindow)
        {
            QString appDir = QCoreApplication::applicationDirPath();
            pythonInterpreter = new PythonInterpreterThread(appDir + tr("/Plugins/c/runpy"), mainWindow);
            pythonInterpreter->initialize();

            connect(pythonInterpreter,SIGNAL(started()),this,SIGNAL(pythonStarted()));
            connect(pythonInterpreter,SIGNAL(finished()),this,SIGNAL(pythonFinished()));
            connect(pythonInterpreter,SIGNAL(terminated()),this,SIGNAL(pythonFinished()));

            connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
            connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

            toolLoaded(0);

            OutputWindow::message(tr("Python initializing (init.py) ... DO NOT EXIT AT THIS TIME"));

#ifdef Q_WS_WIN
            QString pydir = appDir.replace("/","\\\\") + tr("\\\\py");
#else
            QString pydir = appDir + tr("/py");
#endif
            runPythonCode((tr("import sys\nsys.path.append(\"")+pydir+tr("\")\nfrom pytc import *\nimport init")).toAscii().data());

            return true;
        }
        
		return false;
    }

    void PythonTool::toolLoaded(Tool*)
    {
        static bool connected = false;

        if (!connected && mainWindow->tool(tr("Dynamic Library Menu")))
        {
            QWidget * widget = mainWindow->tool(tr("Dynamic Library Menu"));
            if (widget)
            {
                connected = true;

                DynamicLibraryMenu * libMenu = static_cast<DynamicLibraryMenu*>(widget);

                QString filename(tr("pythonscripts.txt"));

                QString appDir = QCoreApplication::applicationDirPath();

                QString name[] = {	MainWindow::userHome() + tr("/") + filename,
                                        filename,
                                        QDir::currentPath() + tr("/") + filename,
                                        appDir + tr("/") + filename };

                QFile file;
                bool opened = false;
                for (int i=0; i < 4; ++i)
                {
                    file.setFileName(name[i]);
                    if (file.open(QFile::ReadOnly | QFile::Text))
                    {
                        opened = true;
                        break;
                    }
                }
                if (!opened)
                {
                    OutputWindow::error(filename +  tr(" file not found"));
                }
                else
                {
                    loadFromFile(libMenu,file);
                    file.close();
                }
            }
        }

        if (pythonInterpreter)
            pythonInterpreter->setCPointers();
    }

    void PythonTool::buttonPressed ( int id )
    {
        if (pyFileNames.size() <= id)
            return;

        QString pyfile = pyFileNames[id];

        if (!pyfile.isEmpty())
        {
            runPythonFile(pyfile); //go
        }
    }

    void PythonTool::actionTriggered(QAction * item)
    {
        if (!item || !hashPyFile.contains(item))
            return;

        QString pyfile = hashPyFile[item];

        if (!pyfile.isEmpty())
        {
            runPythonFile(pyfile); //go
        }
    }

    void PythonTool::connectTCFunctions()
    {
        connect(&fToS,SIGNAL(runPythonCode(QSemaphore*,const QString&)),this,SLOT(runPythonCode(QSemaphore*,const QString&)));
        connect(&fToS,SIGNAL(runPythonFile(QSemaphore*,const QString&)),this,SLOT(runPythonFile(QSemaphore*,const QString&)));
    }

    typedef void (*tc_PythonTool_api)(
            void (*runPythonCode)(const char*),
            void (*runPythonFile)(const char*)
            );

    void PythonTool::setupFunctionPointers( QLibrary * library)
    {
        tc_PythonTool_api f = (tc_PythonTool_api)library->resolve("tc_PythonTool_api");
        if (f)
        {
            //qDebug() << "tc_PythonTool_api resolved";
            f(
                    &(_runPythonCode),
                    &(_runPythonFile)
                    );
        }
    }

    /******************************************************/

    PythonTool_FToS PythonTool::fToS;


    void PythonTool::_runPythonCode(const char* c)
    {
        return fToS.runPythonCode(c);
    }

    void PythonTool::_runPythonFile(const char* c)
    {
        return fToS.runPythonFile(c);
    }

    void PythonTool_FToS::runPythonCode(const char* c)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit runPythonCode(s,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
    }

    void PythonTool_FToS::runPythonFile(const char* c)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit runPythonFile(s,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
    }

    /*******************
          PYTHON STUFF
    *********************/

    void PythonTool::stopPython()
    {
        if (pythonInterpreter && pythonInterpreter->isRunning())
        {
            pythonInterpreter->terminate();
        }
    }

    void PythonTool::runPythonCode(QSemaphore* sem,const QString& code)
    {
        runPythonCode(code);
        if (sem)
            sem->release();
    }

    void PythonTool::runPythonFile(QSemaphore* sem,const QString& file)
    {
        runPythonFile(file);
        if (sem)
            sem->release();
    }

    void PythonTool::runPythonCode(const QString& code)
    {
        OutputWindow::freeze();
        if (pythonInterpreter)
            pythonInterpreter->runCode(code);
    }

    void PythonTool::runPythonFile(const QString& filename)
    {
        if (pythonInterpreter)
        {
            QString appDir = QCoreApplication::applicationDirPath();

            QString name[] = {	MainWindow::userHome() + tr("/") + filename,
                                MainWindow::userHome() + tr("/py/") + filename,
                                filename,
                                QDir::currentPath() + tr("/") + filename,
                                appDir + tr("/py/") + filename ,
                                appDir + tr("/") + filename };

            QFile file;
            bool opened = false;
            for (int i=0; i < 6; ++i)
            {
                file.setFileName(name[i]);
                if (file.open(QFile::ReadOnly | QFile::Text))
                {
                    opened = true;
                    break;
                }
            }
            if (!opened)
            {
                OutputWindow::error(tr("file not found"));
            }
            else
            {
                QString code(file.readAll());
                pythonInterpreter->runCode(code);
                file.close();
            }
        }
    }

}
