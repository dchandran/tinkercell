/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Provides a toolbar with buttons that call C functions (run of separate threads)
 
****************************************************************************/
#include <QVBoxLayout>
#include <QDockWidget>
#include <QProcess>
#include <QLibrary>
#include <QRegExp>
#include <QDir>
#include <QFile>
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "CThread.h"
#include "ConsoleWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "LoadCLibraries.h"
#include <QtDebug>

namespace Tinkercell
{
    
    LoadCLibrariesTool::LoadCLibrariesTool() : Tool(tr("Load C Libraries")), actionsGroup(this), buttonsGroup(this)
    {
        //connect(&actionsGroup,SIGNAL(triggered ( QAction *  )),this,SLOT(actionTriggered ( QAction *  )));
        //connect(&buttonsGroup,SIGNAL(buttonPressed ( int  )),this,SLOT(buttonPressed ( int  )));
        connectTCFunctions();
        libMenu = 0;
    }
    
    /*
         void LoadCLibrariesTool::loadFromFile(DynamicLibraryMenu * libMenu, QFile& file)
         {
                 if (!libMenu) return;
                 
                 QStringList lines;
                 
                 while (!file.atEnd()) 
                 {
                         QByteArray line = file.readLine();
                         if (!line.isEmpty())
                                 lines << line.trimmed();
                 }
                 
                 QString desc, dllFile, functionName, categoryName, iconfile, menu,  family, tool;
                 
                 QString appDir = QCoreApplication::applicationDirPath();
                 
                 for (int i=0; i < lines.size(); ++i)
                 {
                        QStringList ls = lines[i].split(tr(";"));
                        
                        if (ls.size() >= 3)
                        {
                                desc = ls[0].trimmed();
                                dllFile = ls[1].trimmed();
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
                                        buttonsGroup.addButton(button,dllFileNames.size());
                                        dllFileNames << dllFile;
                                }
                                
                                if (menu == tr("menu"))
                                {
                                        QAction * menuItem = libMenu->addMenuItem(functionName,QIcon(pixmap));
                                        if (menuItem)
                                        {
                                                menuItem->setToolTip(desc);
                                                actionsGroup.addAction(menuItem);
                                                hashDll[menuItem] = dllFile;
                                        }
                                        
                                        if (!family.isEmpty())
                                        {
                                                QAction * contextAction = libMenu->addContextMenuItem(family, functionName, pixmap, tool==tr("tool"));
                                                if (contextAction) 
                                                {
                                                        contextAction->setToolTip(desc);
                                                        actionsGroup.addAction(contextAction);
                                                        hashDll[contextAction] = dllFile;
                                                }
                                        }
                                }
                        }
                 }
         }*/
    
    bool LoadCLibrariesTool::setMainWindow(MainWindow * main)
    {
        Tool::setMainWindow(main);
        if (mainWindow)
        {
            connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
            connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));
            
            toolLoaded(0);
            
            return true;
        }
        return false;
    }
    
    void LoadCLibrariesTool::addFunction(QSemaphore* s,void (*f)(void), const QString& title, const QString& desc, const QString& cat, const QString& iconFilename,const QString& family, int show_menu, int in_tool_menu, int deft)
    {
        if (libMenu)
        {
            QString  home = MainWindow::userHome(),
                        current = QDir::currentPath(),
                        appDir = QCoreApplication::applicationDirPath();

            QString name[] = {  appDir + tr("/") + iconFilename,
                                         home + tr("/") + iconFilename,
                                         current + tr("/") + iconFilename,
                                         iconFilename };
            QString iconFile("");
            for (int i=0; i < 4; ++i)
            {
                if (QFile::exists(name[i]))
                {
                    iconFile =name[i];
                    break;
                }
            }

            QPixmap pixmap(iconFile);
            QIcon icon(pixmap);
            QToolButton * button = libMenu->addFunction(cat, title, icon);
            button->setToolTip(desc);

            CThread * thread = new CThread(mainWindow,0);
            thread->setFunction(f);
            CThread::dialog(thread,title,icon,true);
            
            connect(button,SIGNAL(pressed()),thread,SLOT(start()));
            
            if (show_menu > 0)
            {
                QAction * action = libMenu->addMenuItem(title,icon, deft > 0);
                if (action)
				{	
					action->setToolTip(desc);
					connect(action,SIGNAL(triggered()),thread,SLOT(start()));
				}
            }
            
            if (in_tool_menu > 0)
            {
                QAction * action = libMenu->addContextMenuItem(family,title,pixmap,true);
                if (action)
				{
					action->setToolTip(desc);
					connect(action,SIGNAL(triggered()),thread,SLOT(start()));
				}
            }
        }
        if (s)
            s->release();
    }
    
    void LoadCLibrariesTool::callback(QSemaphore* s,void (*f)(void))
    {
        if (s)
            s->release();
    }
    
    void LoadCLibrariesTool::toolLoaded(Tool*)
    {
        static bool connected = false;
        
        if (!connected && mainWindow->tool(tr("Dynamic Library Menu")))
        {
            QWidget * widget = mainWindow->tool(tr("Dynamic Library Menu"));
            if (widget)
            {
                libMenu = static_cast<DynamicLibraryMenu*>(widget);
                connected = true;
            }
        }
    }
    
    /*void LoadCLibrariesTool::buttonPressed ( int id )
         {
                if (dllFileNames.size() <= id)
                        return;
                        
                QString dllName = dllFileNames[id];
                QAbstractButton * button = buttonsGroup.button(id);
                
                if (button && !dllName.isEmpty())
                {
                        LibraryThread * newThread = new LibraryThread(dllName,tr("run"),mainWindow,emptyMatrix());
                        if (LibraryThread::ThreadDialog(mainWindow,newThread,button->text(),button->icon()))
                                newThread->start(); //go
                }
         }
         
         void LoadCLibrariesTool::actionTriggered(QAction * item)
         {
                if (!item || !hashDll.contains(item))
                        return;
                        
                QString dllName = hashDll[item];
                
                if (!dllName.isEmpty())
                {
                        LibraryThread * newThread = new LibraryThread(dllName,tr("run"),mainWindow,emptyMatrix());
                        if (LibraryThread::ThreadDialog(mainWindow,newThread,item->text(),item->icon()))
                                newThread->start(); //go
                }
         }
         */
    void LoadCLibrariesTool::connectTCFunctions()
    {
        connect(&fToS,SIGNAL(compileAndRun(QSemaphore*,int*,const QString&,const QString&)),this,SLOT(compileAndRunC(QSemaphore*,int*,const QString&,const QString&)));
        connect(&fToS,SIGNAL(compileBuildLoad(QSemaphore*,int*,const QString&,const QString&,const QString&)),this,SLOT(compileBuildLoadC(QSemaphore*,int*,const QString&,const QString&,const QString&)));
        connect(&fToS,SIGNAL(loadLibrary(QSemaphore*,const QString&)),this,SLOT(loadLibrary(QSemaphore*,const QString&)));
        connect(&fToS,SIGNAL(addFunction(QSemaphore*,VoidFunction, const QString& , const QString& , const QString& , const QString& ,const QString& , int, int,int)),
                   this,SLOT(addFunction(QSemaphore*,VoidFunction,QString,QString,QString,QString,QString,int,int,int)));
        connect(&fToS,SIGNAL(callback(QSemaphore*,VoidFunction)),
                    this,SLOT(callback(QSemaphore*,VoidFunction)));

    }
    
    typedef void (*tc_LoadCLibraries_api)(
            int (*compileAndRun)(const char * ,const char* ),
            int (*compileBuildLoad)(const char *, const char* , const char*),
            void (*loadLib)(const char*),
            void (*addf)(void (*f)(),const char * , const char* , const char* , const char* , const char * , int , int , int ),
            void (*callback)(void (*f)())
            );
    
    void LoadCLibrariesTool::setupFunctionPointers( QLibrary * library)
    {
        tc_LoadCLibraries_api f = (tc_LoadCLibraries_api)library->resolve("tc_LoadCLibraries_api");
        if (f)
        {
            f(
                    &(_compileAndRun),
                    &(_compileBuildLoad),			
                    &(_loadLibrary),
                    &(_addFunction),
                    &(_callback)
                    );
        }
    }
    
    void LoadCLibrariesTool::compileAndRunC(QSemaphore* s,int* r,const QString& filename,const QString& args)
    {
        if (filename.isEmpty() || filename.isNull())
        {
            if (r) (*r) = 0;
            if (s) s->release();
            return;
        }
        QString appDir = QCoreApplication::applicationDirPath();
        QProcess proc;
		proc.setWorkingDirectory(MainWindow::userHome());
		
#ifdef Q_WS_WIN
        proc.start(tr("del a.out"));
        proc.waitForFinished();
		proc.start(tr("\"") + appDir + tr("\"\\win32\\tcc -I\"") + appDir + ("\"/win32/include -I\"") + appDir + ("\"/c -L\"") + appDir + ("\"/win32/lib -r -w ") + filename  + tr(" -o a.out"));        
        proc.waitForFinished();
        QString errors(proc.readAllStandardError());
        QString output(proc.readAllStandardOutput());
#else
        proc.start(tr("rm a.out"));
        proc.waitForFinished();
        proc.start(tr("gcc -o a.out ") + filename);
        proc.waitForFinished();
        QString errors(proc.readAllStandardError());
        QString output(proc.readAllStandardOutput());
#endif
        
        if (!errors.isEmpty())
            ConsoleWindow::error(errors);
        else
            ConsoleWindow::message(output);
        
        if (errors.size() > 0)
        {
            if (r) (*r) = 0;
            if (s) s->release();
            return;
        }
        
        ProcessThread * newThread = new ProcessThread(filename,args,mainWindow);
        
        QRegExp regexp(".*/([^/]+)$");
        QString title = filename;
        if (regexp.indexIn(filename) > -1)
            title = regexp.cap(1);
        if (ProcessThread::dialog(mainWindow,newThread,title))
            newThread->start();

        if (r) (*r) = 1;
        if (s) s->release();		
    }
	
	void LoadCLibrariesTool::compileAndRunC(const QString& s,const QString& a)
	{
		_compileAndRun(s.toAscii().data(),a.toAscii().data());
	}
	
	void LoadCLibrariesTool::compileBuildLoadC(const QString& s,const QString& f,const QString& t)
	{
		_compileBuildLoad(s.toAscii().data(),f.toAscii().data(),t.toAscii().data());
	}
    
    void LoadCLibrariesTool::compileBuildLoadC(QSemaphore* s,int* r,const QString& filename,const QString& funcname, const QString& title)
    {
        if (filename.isEmpty() || filename.isNull())
        {
            if (r) (*r) = 0;
            if (s) s->release();
            return;
        }
        QString dllName("temp");
        QRegExp regex("([A-Za-z0-9_]+)\\.c");
        if (regex.indexIn(filename) > -1)
        {
            dllName = regex.cap(1);
        }
        
        QString errors;
        QString output;
        QProcess proc;
        QString appDir = QCoreApplication::applicationDirPath();
		
		proc.setWorkingDirectory(MainWindow::userHome());
        
#ifdef Q_WS_WIN
        proc.start(tr("del ") + dllName + tr(".dll"));
        proc.waitForFinished();
		proc.start(tr("\"") + appDir + tr("\"\\win32\\tcc -I\"") + appDir + ("\"/win32/include -I\"") + appDir + ("\"/c -L\"") + appDir + ("\"/win32/lib -w -shared -rdynamic ") + filename + tr(" -o ") + dllName + tr(".dll "));
        proc.waitForFinished();
        
        if (!errors.isEmpty())	errors += tr("\n\n");
        errors += (proc.readAllStandardError());
        if (!output.isEmpty())	output += tr("\n\n");
        output += tr("\n\n") + (proc.readAllStandardOutput());
#else
#ifdef Q_WS_MAC
        proc.start(tr("rm ") + dllName + tr(".dylib"));
        proc.waitForFinished();
        proc.start(tr("gcc -bundle -w --shared -o ") + dllName + tr(".dylib ") + filename);
        proc.waitForFinished();
        if (!errors.isEmpty())	errors += tr("\n\n");
        errors += (proc.readAllStandardError());
        if (!output.isEmpty())	output += tr("\n\n");
        output += tr("\n\n") + (proc.readAllStandardOutput());
#else
        proc.start(tr("rm ") + dllName + tr(".so"));
        proc.waitForFinished();
        proc.start(tr("gcc -w --shared -o ") + dllName + tr(".so ") + filename);
        proc.waitForFinished();
        if (!errors.isEmpty())	errors += tr("\n\n");
        errors += (proc.readAllStandardError());
        if (!output.isEmpty())	output += tr("\n\n");
        output += tr("\n\n") + (proc.readAllStandardOutput());
#endif
#endif
        
        if (!errors.isEmpty())
            ConsoleWindow::error(errors);
        else
            ConsoleWindow::message(output);
        
        if (errors.size() > 0)
        {
            if (r) (*r) = 0;
            if (s) s->release();
		    return;
        }
        
        CThread * newThread = new CThread(mainWindow,dllName,true);
        newThread->setVoidFunction(funcname.toAscii().data());
        
        CThread::dialog(newThread,title,QIcon(),true);
        newThread->start();
        
        if (r) (*r) = 1;
		
        if (s) s->release();
    }
    
    void LoadCLibrariesTool::loadLibrary(QSemaphore* s,const QString& file)
    {
        if (mainWindow)
            mainWindow->loadDynamicLibrary(file);
        
        if (s) s->release();
    }
    
    
    /******************************************************/
    
    LoadCLibrariesTool_FToS LoadCLibrariesTool::fToS;
    
    int LoadCLibrariesTool::_compileAndRun(const char * cfile,const char* args)
    {
        return fToS.compileAndRun(cfile,args);
    }
    
    int LoadCLibrariesTool::_compileBuildLoad(const char * cfile,const char* f,const char* t)
    {
        return fToS.compileBuildLoad(cfile,f,t);
    }
    
    void LoadCLibrariesTool::_loadLibrary(const char * c)
    {
        return fToS.loadLibrary(c);
    }

    void  LoadCLibrariesTool::_addFunction(void (*f)(), const char * title, const char* desc, const char* cat, const char* icon, const char * family, int inMenu, int inTool, int deft)
    {
        fToS.addFunction(f, title, desc, cat, icon, family, inMenu, inTool, deft);
    }
    
    void  LoadCLibrariesTool::_callback(void (*f)(void))
    {
    }
    
    int LoadCLibrariesTool_FToS::compileAndRun(const char * cfile,const char* args)
    {
        QSemaphore * s = new QSemaphore(1);
        int p;
        s->acquire();
        emit compileAndRun(s,&p,ConvertValue(cfile),ConvertValue(args));
        s->acquire();
        s->release();
        delete s;
        return p;
    }
    
    int LoadCLibrariesTool_FToS::compileBuildLoad(const char * cfile,const char* f,const char* t)
    {
        QSemaphore * s = new QSemaphore(1);
        int p;
        s->acquire();
        emit compileBuildLoad(s,&p,ConvertValue(cfile),ConvertValue(f),ConvertValue(t));
        s->acquire();
        s->release();
        delete s;
        return p;
    }	

    void LoadCLibrariesTool_FToS::addFunction(void (*f)(), const char * title, const char* desc, const char* cat, const char* icon, const char * family, int inMenu, int inTool, int deft)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit addFunction(s,f,ConvertValue(title),ConvertValue(desc),ConvertValue(cat),ConvertValue(icon),ConvertValue(family),inMenu,inTool, deft);
        s->acquire();
        s->release();
        delete s;
    }

    void LoadCLibrariesTool_FToS::callback(void (*f)())
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit callback(s,f);
        s->acquire();
        s->release();
        delete s;
    }

    void LoadCLibrariesTool_FToS::loadLibrary(const char * c)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit loadLibrary(s,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
    }
    
}
