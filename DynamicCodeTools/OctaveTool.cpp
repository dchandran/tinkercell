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
#include <QFileInfoList>
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "OctaveTool.h"
#include <QtDebug>

namespace Tinkercell
{
    OctaveTool::OctaveTool() : Tool(tr("Octave Interpreter"),tr("Coding")), actionsGroup(this), buttonsGroup(this)
    {
        octaveInterpreter = 0;

        connect(&actionsGroup,SIGNAL(triggered ( QAction *  )),this,SLOT(actionTriggered ( QAction *  )));
        connect(&buttonsGroup,SIGNAL(buttonPressed ( int  )),this,SLOT(buttonPressed ( int  )));
        connectTCFunctions();
    }
    
    bool OctaveTool::loadFromDir(QDir& dir)
    {
        QWidget * widget = mainWindow->tool(tr("Dynamic Library Menu"));
        if (widget)
        {
            DynamicLibraryMenu * libMenu = static_cast<DynamicLibraryMenu*>(widget);
    		return loadFromDir(libMenu, dir);
    	}
    	return false;
    }

    bool OctaveTool::loadFromDir(DynamicLibraryMenu * libMenu, QDir& dir)
    {
    	bool filesFound = false;
        if (!libMenu) return filesFound;
        
        dir.setFilter(QDir::Files);
		dir.setSorting(QDir::Name);

		QFileInfoList list = dir.entryInfoList();

		QString appDir = QCoreApplication::applicationDirPath();
		QString homeDir = MainWindow::homeDir();
		
		for (int i = 0; i < list.size(); ++i)
		{
			QFileInfo fileInfo = list.at(i);
			
			QString octFile = fileInfo.absoluteFilePath();
			if (octFileNames.contains(octFile)) continue;
			
			QFile file(octFile);
			if (fileInfo.completeSuffix().toLower() != tr("m") || !file.open(QFile::ReadOnly)) continue;
			
			QString category, name, descr, icon, specific;
			bool menu = true, tool = true;
			bool commentsLine = false;

			while (!file.atEnd()) //inside octave script file
			{
				QString line(file.readLine());
				commentsLine = line.toLower().contains(tr("#")) || line.toLower().contains(tr("%"));				
				if (!commentsLine) continue;
				
				line.remove(tr("#"));
				line.remove(tr("%"));
				
				QStringList words = line.split(tr(":"));
				if (words.size() == 2)
				{
					QString s1 = words[0].trimmed(), 
							s2 = words[1].trimmed();

					if (s1 == tr("category"))
						category = s2;
					else
					if (s1 == tr("name"))
						name = s2;
					else
					if (s1 == tr("description"))
						descr = s2;
					else
					if (s1 == tr("icon"))
						icon = s2;
					else
					if (s1 == tr("menu"))
						menu = (s2.toLower().contains("yes"));
					else
					if (s1 == tr("tool"))
						tool = (s2.toLower().contains("yes"));
					else
					if (s1.contains(tr("specific")))
						specific = s2;
				}
			}
			
			file.close();
			
			if (name.isNull() || name.isEmpty()) continue;
			
			filesFound = true;
		
			if (!QFile(icon).exists())
			{
				if (QFile(appDir + tr("/") + icon).exists())
					icon = appDir + tr("/") + icon;
				else
				if (QFile(tr(":/images/") + icon).exists())
					icon = tr(":/images/") + icon;
				else
				if (QFile(homeDir + tr("/") + icon).exists())
					icon = homeDir + tr("/") + icon;
				else
				if (QFile(homeDir + tr("/octave/") + icon).exists())
					icon = homeDir + tr("/octave/") + icon;
			}
			if (icon.isEmpty() || !QFile(icon).exists())
				icon = tr(":/images/function.png");
			QPixmap pixmap(icon);
		
            QToolButton * button = libMenu->addFunction(category, name, QIcon(pixmap));
		
			if (button)
			{
				button->setToolTip(descr);
				buttonsGroup.addButton(button,octFileNames.size());
				octFileNames << octFile;
			}

			if (menu)
			{
				QAction * menuItem = libMenu->addMenuItem(category, name, QIcon(pixmap));
				if (menuItem)
				{
					menuItem->setToolTip(descr);
					actionsGroup.addAction(menuItem);
					hashOctFile[menuItem] = octFile;
				}
			}

			if (!specific.isEmpty())
			{
				QAction * contextAction = libMenu->addContextMenuItem(specific, name, pixmap, tool);
				if (contextAction)
				{
					contextAction->setToolTip(descr);
					actionsGroup.addAction(contextAction);
					hashOctFile[contextAction] = octFile;
				}
			}
		} //done reading one oct script file

		return filesFound;
    }

    bool OctaveTool::setMainWindow(MainWindow * main)
    {
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			QString appDir = QCoreApplication::applicationDirPath();
			
			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			octaveInterpreter = new OctaveInterpreterThread(tr("octave/tinkercell"), tr("octave/libtcoct"), mainWindow);

			if (console())
				console()->message(tr("Running init.m...\n"));
			
			octaveInterpreter->initialize();
			
			toolLoaded(0);

			QString s;
			
			QFile file(appDir + tr("/octave/init.m"));
			if (file.open(QFile::ReadOnly | QFile::Text))
            {
                s += file.readAll();
                file.close();
            }
			
			runOctaveCode(s);
			
			return true;
		}
        
		return false;
    }

    void OctaveTool::toolLoaded(Tool*)
    {
        static bool connected = false;

        if (!connected && mainWindow->tool(tr("Dynamic Library Menu")))
        {
            QWidget * widget = mainWindow->tool(tr("Dynamic Library Menu"));
            if (widget)
            {
                connected = true;

                DynamicLibraryMenu * libMenu = static_cast<DynamicLibraryMenu*>(widget);

                QString appDir = QCoreApplication::applicationDirPath();

                QString name[] = {
				  MainWindow::tempDir() + tr("/octave"),
                  MainWindow::homeDir() + tr("/octave"),
                  QDir::currentPath() + tr("/octave"),
                  appDir + tr("/octave")
               };

                bool opened = false;
                for (int i=0; i < 4; ++i)
                {
                    QDir dir(name[i]);
                    if (dir.exists())
                    {
                        opened = loadFromDir(libMenu,dir) || opened;
                    }
                }
            }
        }
    }

    void OctaveTool::buttonPressed ( int id )
    {
        if (octFileNames.size() <= id)
            return;

        QString octfile = octFileNames[id];

        if (!octfile.isEmpty())
        {
            runOctaveFile(octfile); //go
        }
    }

    void OctaveTool::actionTriggered(QAction * item)
    {
        if (!item || !hashOctFile.contains(item))
            return;

        QString octfile = hashOctFile[item];

        if (!octfile.isEmpty())
        {
            runOctaveFile(octfile); //go
        }
    }

    void OctaveTool::connectTCFunctions()
    {
        connect(&fToS,SIGNAL(runOctaveCode(QSemaphore*,const QString&)),this,SLOT(runOctaveCode(QSemaphore*,const QString&)));
        connect(&fToS,SIGNAL(runOctaveFile(QSemaphore*,const QString&)),this,SLOT(runOctaveFile(QSemaphore*,const QString&)));
        connect(&fToS,SIGNAL(addOctavePlugin(QSemaphore*,const QString&,const QString&,const QString&,const QString&, const QString&)),
        		this,SLOT(addOctavePlugin(QSemaphore*,const QString&,const QString&,const QString&,const QString&, const QString&)));
    }

    typedef void (*tc_OctaveTool_api)(
            void (*runOctaveCode)(const char*),
            void (*runOctaveFile)(const char*),
            void (*addOctavePlugin)(const char*,const char*,const char*,const char*,const char*)
            );

    void OctaveTool::setupFunctionPointers( QLibrary * library)
    {
        tc_OctaveTool_api f = (tc_OctaveTool_api)library->resolve("tc_OctaveTool_api");
        if (f)
        {
            //qDebug() << "tc_OctaveTool_api resolved";
            f(
                &(_runOctaveCode),
                &(_runOctaveFile),
                &(_addOctavePlugin)
                );
        }
    }

    /******************************************************/

    OctaveTool_FToS OctaveTool::fToS;


    void OctaveTool::_runOctaveCode(const char* c)
    {
        return fToS.runOctaveCode(c);
    }

    void OctaveTool::_runOctaveFile(const char* c)
    {
        return fToS.runOctaveFile(c);
    }
    
    void OctaveTool::_addOctavePlugin(const char* file,const char* name,const char* descr,const char* category,const char* icon)
    {
        return fToS.addOctavePlugin(file,name,descr,category,icon);
    }

    void OctaveTool_FToS::runOctaveCode(const char* c)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit runOctaveCode(s,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
    }

    void OctaveTool_FToS::runOctaveFile(const char* c)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit runOctaveFile(s,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
    }
    
    void OctaveTool_FToS::addOctavePlugin(const char* file,const char* name,const char* descr,const char* category,const char* icon)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit addOctavePlugin(s,tr(file),tr(name),tr(descr),tr(category),tr(icon));
        s->acquire();
        s->release();
        delete s;
    }

    /*******************
          OCTAVE STUFF
    *********************/

    void OctaveTool::runOctaveCode(QSemaphore* sem,const QString& code)
    {
        runOctaveCode(code);
        if (sem)
            sem->release();
    }

    void OctaveTool::runOctaveFile(QSemaphore* sem,const QString& file)
    {
        runOctaveFile(file);
        if (sem)
            sem->release();
    }
    
    void OctaveTool::addOctavePlugin(QSemaphore * sem,const QString& octFile,const QString& name,const QString& descr,const QString& category, const QString& icon0)
    {
    	QWidget * widget = mainWindow->tool(tr("Dynamic Library Menu"));
        
        if (!widget) return;
        
        DynamicLibraryMenu * libMenu = static_cast<DynamicLibraryMenu*>(widget);
        
		if (name.isNull() || name.isEmpty())
		{
			if (sem) 
				sem->release();
			return;
		}
		
		QString appDir = QCoreApplication::applicationDirPath();
		QString homeDir = MainWindow::homeDir();
		
		QString icon = icon0;

		if (!QFile(icon).exists())
		{
			if (QFile(appDir + tr("/") + icon).exists())
				icon = appDir + tr("/") + icon;
			else
			if (QFile(tr(":/images/") + icon).exists())
				icon = tr(":/images/") + icon;
			else
			if (QFile(homeDir + tr("/") + icon).exists())
				icon = homeDir + tr("/") + icon;
			else
			if (QFile(homeDir + tr("/octave/") + icon).exists())
				icon = homeDir + tr("/octave/") + icon;
		}
		if (icon.isEmpty() || !QFile(icon).exists())
			icon = tr(":/images/function.png");
		QPixmap pixmap(icon);
	
        QToolButton * button = libMenu->addFunction(category, name, QIcon(pixmap));
	
		if (button)
		{
			button->setToolTip(descr);
			buttonsGroup.addButton(button,octFileNames.size());
			octFileNames << octFile;
		}

		QAction * menuItem = libMenu->addMenuItem(category, name, QIcon(pixmap));
		if (menuItem)
		{
			menuItem->setToolTip(descr);
			actionsGroup.addAction(menuItem);
			hashOctFile[menuItem] = octFile;
		}
		
		/*
		if (!specific.isEmpty())
		{
			QAction * contextAction = libMenu->addContextMenuItem(specific, name, pixmap, tool);
			if (contextAction)
			{
				contextAction->setToolTip(descr);
				actionsGroup.addAction(contextAction);
				hashOctFile[contextAction] = octFile;
			}
		}*/
			
    	if (sem)
    		sem->release();
    }

    void OctaveTool::runOctaveCode(const QString& code)
    {	
        if (octaveInterpreter)
            octaveInterpreter->exec(code);
    }

    void OctaveTool::runOctaveFile(const QString& filename)
    {
        if (octaveInterpreter)
        {
            QString appDir = QCoreApplication::applicationDirPath();

            QString name[] = {	MainWindow::homeDir() + tr("/") + filename,
                                MainWindow::homeDir() + tr("/octave/") + filename,
								MainWindow::tempDir() + tr("/") + filename,
                                MainWindow::tempDir() + tr("/octave/") + filename,
                                filename,
                                QDir::currentPath() + tr("/") + filename,
                                appDir + tr("/octave/") + filename ,
                                appDir + tr("/") + filename };

            QFile file;
            bool opened = false;
            for (int i=0; i < 8; ++i)
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
                if (console())
					console()->error( filename + tr("file not found"));
            }
            else
            {
                QString code(file.readAll());
                runOctaveCode(code);
                file.close();
            }
        }
    }

}
