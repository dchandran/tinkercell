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
#include "RubyTool.h"
#include <QtDebug>

namespace Tinkercell
{
    RubyTool::RubyTool() : Tool(tr("Ruby Interpreter"),tr("Coding")), actionsGroup(this), buttonsGroup(this)
    {
    	RubyTool::fToS = new RubyTool_FToS;
    	RubyTool::fToS->setParent(this);
        rubyInterpreter = 0;

        connect(&actionsGroup,SIGNAL(triggered ( QAction *  )),this,SLOT(actionTriggered ( QAction *  )));
        connect(&buttonsGroup,SIGNAL(buttonPressed ( int  )),this,SLOT(buttonPressed ( int  )));
        connectTCFunctions();
    }
    
    bool RubyTool::loadFromDir(QDir& dir)
    {
        QWidget * widget = mainWindow->tool(tr("Dynamic Library Menu"));
        if (widget)
        {
            DynamicLibraryMenu * libMenu = static_cast<DynamicLibraryMenu*>(widget);
    		   return loadFromDir(libMenu, dir);
    	}
    	return false;
    }
  
    bool RubyTool::loadFromDir(DynamicLibraryMenu * libMenu, QDir& dir)
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
			
			QString rubyFile = fileInfo.absoluteFilePath();
			if (rubyFileNames.contains(rubyFile)) continue;
			
			QFile file(rubyFile);
			if (fileInfo.completeSuffix().toLower() != tr("rb") || !file.open(QFile::ReadOnly)) continue;
			
			QString category, name, descr, icon, specific;
			bool menu = true, tool = true;
			bool startedParsing = false;

			while (!file.atEnd()) //inside ruby script file
			{
				QString line(file.readLine());
				if (line.toLower().contains(tr("\"\"\"")))
					if (startedParsing)
						break;
					else
						startedParsing = true;
				
				if (!startedParsing) continue;
				
				QStringList words = line.split(tr(":"));
				if (words.size() == 2)
				{
					QString s1 = words[0].remove(tr("#")).trimmed(), 
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
		
			if (!icon.isEmpty() && !QFile(icon).exists())
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
				if (QFile(homeDir + tr("/ruby/") + icon).exists())
					icon = homeDir + tr("/ruby/") + icon;
			}

			if (icon.isEmpty() || !QFile(icon).exists())
				icon = tr(":/images/function.png");
			QPixmap pixmap(icon);
		
         QToolButton * button = libMenu->addFunction(category, name, QIcon(pixmap));
		
			if (button)
			{
				button->setToolTip(descr);
				buttonsGroup.addButton(button,rubyFileNames.size());
				rubyFileNames << rubyFile;
			}

			if (menu)
			{
				QAction * menuItem = libMenu->addMenuItem(category, name, QIcon(pixmap));
				if (menuItem)
				{
					menuItem->setToolTip(descr);
					actionsGroup.addAction(menuItem);
					hashPyFile[menuItem] = rubyFile;
				}
			}
			
			if (!specific.isEmpty())
			{
				QAction * contextAction = libMenu->addContextMenuItem(specific, name, pixmap, tool);
				if (contextAction)
				{
					contextAction->setToolTip(descr);
					actionsGroup.addAction(contextAction);
					hashPyFile[contextAction] = rubyFile;
				}
			}
		} //done reading one ruby script file

		return filesFound;
    }

    bool RubyTool::setMainWindow(MainWindow * main)
    {
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			QString appDir = QCoreApplication::applicationDirPath();
			rubyInterpreter = new RubyInterpreterThread(tr("ruby/_tinkercell"), mainWindow);
			rubyInterpreter->initialize();

			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			toolLoaded(0);
			
			if (console())
			{
				if (!(rubyInterpreter->library() && rubyInterpreter->library()->isLoaded()))
					console()->message(tr("Ruby not loaded\n"));
			}
			
			QFile file(appDir + tr("/ruby/init.rb"));
			if (file.open(QFile::ReadOnly | QFile::Text))
            {
                QString s = file.readAll();
                file.close();
                runRubyCode(s);
            }
            
            ConsoleWindow * outWin = console();
			if (outWin && !outWin->interpreter() && rubyInterpreter)
			{
				outWin->setInterpreter(rubyInterpreter);
			}
			
			return true;
		}
        
		return false;
    }

    void RubyTool::toolLoaded(Tool*)
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
				  MainWindow::tempDir() + tr("/ruby"),
                  MainWindow::homeDir() + tr("/ruby"),
                  QDir::currentPath() + tr("/ruby"),
                  appDir + tr("/ruby")
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
             	if (!opened)
                {
                    if (console())
						console()->message(tr("No ruby plugins found (located in the /ruby folder)"));
                }
            }
        }
    }

    void RubyTool::buttonPressed ( int id )
    {
        if (rubyFileNames.size() <= id)
            return;

        QString rubyfile = rubyFileNames[id];

        if (!rubyfile.isEmpty())
        {
            runRubyFile(rubyfile); //go
        }
    }

    void RubyTool::actionTriggered(QAction * item)
    {
        if (!item || !hashPyFile.contains(item))
            return;

        QString rubyfile = hashPyFile[item];

        if (!rubyfile.isEmpty())
        {
            runRubyFile(rubyfile); //go
        }
    }

    void RubyTool::connectTCFunctions()
    {
        connect(fToS,SIGNAL(runRubyCode(QSemaphore*,const QString&)),this,SLOT(runRubyCode(QSemaphore*,const QString&)));
        connect(fToS,SIGNAL(runRubyFile(QSemaphore*,const QString&)),this,SLOT(runRubyFile(QSemaphore*,const QString&)));
        connect(fToS,SIGNAL(addRubyPlugin(QSemaphore*,const QString&,const QString&,const QString&,const QString&, const QString&)),
        		this,SLOT(addRubyPlugin(QSemaphore*,const QString&,const QString&,const QString&,const QString&, const QString&)));
    }

    typedef void (*tc_RubyTool_api)(
            void (*runRubyCode)(const char*),
            void (*runRubyFile)(const char*),
            void (*addRubyPlugin)(const char*,const char*,const char*,const char*,const char*)
            );

    void RubyTool::setupFunctionPointers( QLibrary * library)
    {
        tc_RubyTool_api f = (tc_RubyTool_api)library->resolve("tc_RubyTool_api");
        if (f)
        {
            //qDebug() << "tc_RubyTool_api resolved";
            f(
                &(_runRubyCode),
                &(_runRubyFile),
                &(_addRubyPlugin)
                );
        }
    }

    /******************************************************/

    RubyTool_FToS * RubyTool::fToS;


    void RubyTool::_runRubyCode(const char* c)
    {
        return fToS->runRubyCode(c);
    }

    void RubyTool::_runRubyFile(const char* c)
    {
        return fToS->runRubyFile(c);
    }
    
    void RubyTool::_addRubyPlugin(const char* file,const char* name,const char* descr,const char* category,const char* icon)
    {
        return fToS->addRubyPlugin(file,name,descr,category,icon);
    }

    void RubyTool_FToS::runRubyCode(const char* c)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit runRubyCode(s,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
    }

    void RubyTool_FToS::runRubyFile(const char* c)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit runRubyFile(s,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
    }
    
    void RubyTool_FToS::addRubyPlugin(const char* file,const char* name,const char* descr,const char* category,const char* icon)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit addRubyPlugin(s,tr(file),tr(name),tr(descr),tr(category),tr(icon));
        s->acquire();
        s->release();
        delete s;
    }

    /*******************
          RUBY STUFF
    *********************/

    void RubyTool::runRubyCode(QSemaphore* sem,const QString& code)
    {
        runRubyCode(code);
        if (sem)
            sem->release();
    }

    void RubyTool::runRubyFile(QSemaphore* sem,const QString& file)
    {
        runRubyFile(file);
        if (sem)
            sem->release();
    }
    
    void RubyTool::addRubyPlugin(QSemaphore * sem,const QString& rubyFile,const QString& name,const QString& descr,const QString& category, const QString& icon0)
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
			if (QFile(homeDir + tr("/ruby/") + icon).exists())
				icon = homeDir + tr("/ruby/") + icon;
		}

		if (icon.isEmpty() || !QFile(icon).exists())
			icon = tr(":/images/function.png");
		QPixmap pixmap(icon);
	
        QToolButton * button = libMenu->addFunction(category, name, QIcon(pixmap));
	
		if (button)
		{
			button->setToolTip(descr);
			buttonsGroup.addButton(button,rubyFileNames.size());
			rubyFileNames << rubyFile;
		}

		QAction * menuItem = libMenu->addMenuItem(category, name, QIcon(pixmap));
		if (menuItem)
		{
			menuItem->setToolTip(descr);
			actionsGroup.addAction(menuItem);
			hashPyFile[menuItem] = rubyFile;
		}
		
		/*
		if (!specific.isEmpty())
		{
			QAction * contextAction = libMenu->addContextMenuItem(specific, name, pixmap, tool);
			if (contextAction)
			{
				contextAction->setToolTip(descr);
				actionsGroup.addAction(contextAction);
				hashPyFile[contextAction] = rubyFile;
			}
		}*/
			
    	if (sem)
    		sem->release();
    }

    void RubyTool::runRubyCode(const QString& code)
    {	
        if (rubyInterpreter)
            rubyInterpreter->exec(code);
    }

    void RubyTool::runRubyFile(const QString& filename)
    {
        if (rubyInterpreter)
        {
            QString appDir = QCoreApplication::applicationDirPath();

            QString name[] = {	MainWindow::homeDir() + tr("/") + filename,
                                MainWindow::homeDir() + tr("/ruby/") + filename,
								MainWindow::tempDir() + tr("/") + filename,
                                MainWindow::tempDir() + tr("/ruby/") + filename,
                                filename,
                                QDir::currentPath() + tr("/") + filename,
                                appDir + tr("/ruby/") + filename ,
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
                runRubyCode(code);
                file.close();
            }
        }
    }

}
