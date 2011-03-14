/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

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
#include "JavaTool.h"

namespace Tinkercell
{
    JavaTool::JavaTool() : Tool(tr("Java Interpreter"),tr("Coding")), actionsGroup(this), buttonsGroup(this)
    {
        javaInterpreter = 0;

        connect(&actionsGroup,SIGNAL(triggered ( QAction *  )),this,SLOT(actionTriggered ( QAction *  )));
        connect(&buttonsGroup,SIGNAL(buttonPressed ( int  )),this,SLOT(buttonPressed ( int  )));
        connectTCFunctions();
    }
    
    bool JavaTool::loadFromDir(QDir& dir)
    {
        QWidget * widget = mainWindow->tool(tr("Dynamic Library Menu"));
        if (widget)
        {
            DynamicLibraryMenu * libMenu = static_cast<DynamicLibraryMenu*>(widget);
    		return loadFromDir(libMenu, dir);
    	}
    	return false;
    }

    bool JavaTool::loadFromDir(DynamicLibraryMenu * libMenu, QDir& dir)
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
			
			QString javaFile = fileInfo.absoluteFilePath();
			if (javaFileNames.contains(javaFile)) continue;
			
			QFile file(javaFile);
			if (fileInfo.completeSuffix().toLower() != tr("m") || !file.open(QFile::ReadOnly)) continue;
			
			QString category, name, descr, icon, specific;
			bool menu = true, tool = true;
			bool commentsLine = false;

			while (!file.atEnd()) //inside java script file
			{
				QString line(file.readLine());
				commentsLine = line.toLower().contains(tr("#")) || line.toLower().contains(tr("%"));				
				if (!commentsLine) continue;
				
				line.remove(tr("//"));
				line.remove(tr("/*"));
				line.remove(tr("*/"));
				
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
				if (QFile(homeDir + tr("/java/") + icon).exists())
					icon = homeDir + tr("/java/") + icon;
			}
			if (icon.isEmpty() || !QFile(icon).exists())
				icon = tr(":/images/function.png");
			QPixmap pixmap(icon);
		
            QToolButton * button = libMenu->addFunction(category, name, QIcon(pixmap));
		
			if (button)
			{
				button->setToolTip(descr);
				buttonsGroup.addButton(button,javaFileNames.size());
				javaFileNames << javaFile;
			}

			if (menu)
			{
				QAction * menuItem = libMenu->addMenuItem(category, name, QIcon(pixmap));
				if (menuItem)
				{
					menuItem->setToolTip(descr);
					actionsGroup.addAction(menuItem);
					hashJavaCode[menuItem] = javaFile;
				}
			}

			if (!specific.isEmpty())
			{
				QAction * contextAction = libMenu->addContextMenuItem(specific, name, pixmap, tool);
				if (contextAction)
				{
					contextAction->setToolTip(descr);
					actionsGroup.addAction(contextAction);
					hashJavaCode[contextAction] = javaFile;
				}
			}
		} //done reading one java script file

		return filesFound;
    }

    bool JavaTool::setMainWindow(MainWindow * main)
    {
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			QString appDir = QCoreApplication::applicationDirPath();
			
			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			javaInterpreter = new JavaInterpreterThread(tr("java/tinkercell"), tr("java/libtcjava"), mainWindow);

			if (console())
				console()->message(tr("Running init.m...\n"));
			
			javaInterpreter->initialize();
			
			toolLoaded(0);

			QString s;
			
			QFile file(appDir + tr("/java/init.m"));
			if (file.open(QFile::ReadOnly | QFile::Text))
            {
                s += file.readAll();
                file.close();
            }
			
			runJavaCode(s);
			
			return true;
		}
        
		return false;
    }

    void JavaTool::toolLoaded(Tool*)
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
					  MainWindow::tempDir() + tr("/java"),
                  MainWindow::homeDir() + tr("/java"),
                  QDir::currentPath() + tr("/java"),
                  appDir + tr("/java")
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

    void JavaTool::buttonPressed ( int id )
    {
        if (javaFileNames.size() <= id)
            return;

        QString java = javaFileNames[id];

        if (!java.isEmpty())
        {
            runJavaCode(java); //go
        }
    }

    void JavaTool::actionTriggered(QAction * item)
    {
        if (!item || !hashJavaCode.contains(item))
            return;

        QString java = hashJavaCode[item];

        if (!java.isEmpty())
        {
            runJavaCode(java); //go
        }
    }

    void JavaTool::connectTCFunctions()
    {
        connect(&fToS,SIGNAL(runJavaCode(QSemaphore*,const QString&)),this,SLOT(runJavaCode(QSemaphore*,const QString&)));
        connect(&fToS,SIGNAL(addJavaPlugin(QSemaphore*,const QString&,const QString&,const QString&,const QString&, const QString&)),
        		this,SLOT(addJavaPlugin(QSemaphore*,const QString&,const QString&,const QString&,const QString&, const QString&)));
    }

    typedef void (*tc_JavaTool_api)(
            void (*runJavaCode)(const char*),
            void (*addJavaPlugin)(const char*,const char*,const char*,const char*,const char*)
            );

    void JavaTool::setupFunctionPointers( QLibrary * library)
    {
        tc_JavaTool_api f = (tc_JavaTool_api)library->resolve("tc_JavaTool_api");
        if (f)
        {
            f(
                &(_runJavaCode),
                &(_addJavaPlugin)
                );
        }
    }

    /******************************************************/

    JavaTool_FToS JavaTool::fToS;


    void JavaTool::_runJavaCode(const char* c)
    {
        return fToS.runJavaCode(c);
    }
    
    void JavaTool::_addJavaPlugin(const char* file,const char* name,const char* descr,const char* category,const char* icon)
    {
        return fToS.addJavaPlugin(file,name,descr,category,icon);
    }

    void JavaTool_FToS::runJavaCode(const char* c)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit runJavaCode(s,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
    }
    
    void JavaTool_FToS::addJavaPlugin(const char* file,const char* name,const char* descr,const char* category,const char* icon)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit addJavaPlugin(s,tr(file),tr(name),tr(descr),tr(category),tr(icon));
        s->acquire();
        s->release();
        delete s;
    }

    /*******************
          OCTAVE STUFF
    *********************/

    void JavaTool::runJavaCode(QSemaphore* sem,const QString& code)
    {
        runJavaCode(code);
        if (sem)
            sem->release();
    }
    
    void JavaTool::addJavaPlugin(QSemaphore * sem,const QString& javaFile,const QString& name,const QString& descr,const QString& category, const QString& icon0)
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
			if (QFile(homeDir + tr("/java/") + icon).exists())
				icon = homeDir + tr("/java/") + icon;
		}
		if (icon.isEmpty() || !QFile(icon).exists())
			icon = tr(":/images/function.png");
		QPixmap pixmap(icon);
	
        QToolButton * button = libMenu->addFunction(category, name, QIcon(pixmap));
	
		if (button)
		{
			button->setToolTip(descr);
			buttonsGroup.addButton(button,javaFileNames.size());
			javaFileNames << javaFile;
		}

		QAction * menuItem = libMenu->addMenuItem(category, name, QIcon(pixmap));
		if (menuItem)
		{
			menuItem->setToolTip(descr);
			actionsGroup.addAction(menuItem);
			hashJavaCode[menuItem] = javaFile;
		}
		
		/*
		if (!specific.isEmpty())
		{
			QAction * contextAction = libMenu->addContextMenuItem(specific, name, pixmap, tool);
			if (contextAction)
			{
				contextAction->setToolTip(descr);
				actionsGroup.addAction(contextAction);
				hashJavaCode[contextAction] = javaFile;
			}
		}*/
			
    	if (sem)
    		sem->release();
    }

    void JavaTool::runJavaCode(const QString& code)
    {	
        if (javaInterpreter)
            javaInterpreter->exec(code);
    }

}
