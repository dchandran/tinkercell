/***************************************************************************
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
#include "CodingWindow.h"
#include "LoadCLibraries.h"
#include "MultithreadedSliderWidget.h"
#include <QtDebug>
#include "GlobalSettings.h"

namespace Tinkercell
{

    LoadCLibrariesTool::LoadCLibrariesTool() : Tool(tr("Load C Libraries"),tr("Coding")), actionsGroup(this), buttonsGroup(this)
    {
    	LoadCLibrariesTool::fToS = new LoadCLibrariesTool_FToS;
    	LoadCLibrariesTool::fToS->setParent(this);
		numLibFiles = 0;
        connectTCFunctions();
        libMenu = 0;
    }

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
            QString  home = GlobalSettings::homeDir(),
                        current = QDir::currentPath(),
                        appDir = QCoreApplication::applicationDirPath();

            QString name[] = {  iconFilename,
            							appDir + tr("/") + iconFilename,
                                         tr(":/images/") + iconFilename,
                                         home + tr("/icons/") + iconFilename,
                                         home + tr("/") + iconFilename,
                                         home + tr("/plugins/") + iconFilename,
                                         appDir + tr("/plugins/") + iconFilename,
                                         current + tr("/") + iconFilename};
            QString iconFile("");
			bool found = false;
            
            for (int i=0; i < 8; ++i)
            {
                if (QFile::exists(name[i]) || name[i].startsWith(tr(":/images")))
                {
                    iconFile = name[i];
                    found = true;
                    break;
                }
            }

            if (!found)
            	iconFile = tr(":/images/function.png");


            QPixmap pixmap(iconFile);
            QIcon icon(pixmap);
            QToolButton * button = libMenu->addFunction(cat, title, icon);
            button->setToolTip(desc);

            CThread * thread = new CThread(mainWindow,0);
            thread->setFunction(f);

            connect(button,SIGNAL(pressed()),thread,SLOT(start()));

            if (show_menu > 0)
            {
                QAction * action = libMenu->addMenuItem(cat, title, icon, deft > 0);
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

    void LoadCLibrariesTool::connectTCFunctions()
    {
        connect(fToS,SIGNAL(compileAndRun(QSemaphore*,int*,const QString&,const QString&)),this,SLOT(compileAndRunC(QSemaphore*,int*,const QString&,const QString&)));
        connect(fToS,SIGNAL(compileBuildLoad(QSemaphore*,int*,const QString&,const QString&,const QString&)),this,SLOT(compileBuildLoadC(QSemaphore*,int*,const QString&,const QString&,const QString&)));
		connect(fToS,SIGNAL(compileBuildLoadSliders(QSemaphore*,int*,const QString&,const QString&,const QString&,DataTable<qreal>&)),this,SLOT(compileBuildLoadSliders(QSemaphore*,int*,const QString&,const QString&,const QString&,DataTable<qreal>&)));
        connect(fToS,SIGNAL(loadLibrary(QSemaphore*,const QString&)),this,SLOT(loadLibrary(QSemaphore*,const QString&)));
        connect(fToS,SIGNAL(displayCode(QSemaphore*,const QString&)),this,SLOT(displayCode(QSemaphore*,const QString&)));
        connect(fToS,SIGNAL(addFunction(QSemaphore*,VoidFunction, const QString& , const QString& , const QString& , const QString& ,const QString& , int, int,int)),
                   this,SLOT(addFunction(QSemaphore*,VoidFunction,QString,QString,QString,QString,QString,int,int,int)));
    }

    typedef void (*tc_LoadCLibraries_api)(
            int (*compileAndRun)(const char * ,const char* ),
            int (*compileBuildLoad)(const char *, const char* , const char*),
			int (*compileBuildLoadSliders)(const char * ,const char* ,const char* , tc_matrix),
            void (*loadLib)(const char*),
            void (*addf)(void (*f)(),const char * , const char* , const char* , const char* , const char * , int , int , int ),
			void (*displayCode)(const char*)
            );

    void LoadCLibrariesTool::setupFunctionPointers( QLibrary * library)
    {
		tc_LoadCLibraries_api f = (tc_LoadCLibraries_api)library->resolve("tc_LoadCLibraries_api");
        if (f)
        {
            f(
                    &(_compileAndRun),
                    &(_compileBuildLoad),
					&(_compileBuildLoadSliders),
                    &(_loadLibrary),
                    &(_addFunction),
                    &(_displayCode)
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
		proc.setWorkingDirectory(GlobalSettings::tempDir());
		
		QString libs;
		
		if (filename.contains(tr("ode")) || filename.contains(tr("ssa")))
			libs = tr(" -lm -ltinkercellapi -lsundials_cvode -lsundials_nvecserial -llapack -lblas -lf2c -fpic");
		else
			libs = tr(" -lm -ltinkercellapi -fpic");
		

#ifdef Q_WS_WIN
		appDir.replace(tr("/"),tr("\\"));
		proc.start(tr("\"") + appDir + tr("\"\\win32\\gcc -I\"") + appDir + ("\"\\win32\\include -I\"") + appDir + ("\"\\win32\\include\\sys -I\"") + appDir + ("\"/c -L\"") + appDir + ("\" -L\"") + appDir + ("\"/c -L\"") + appDir + ("\"/lib -L\"") + appDir + ("\"\\win32\\lib -r -w ") + filename  + libs);
        proc.waitForFinished();
        QString exeName = tr("a.exe");
        QString errors(proc.readAllStandardError());
        QString output(proc.readAllStandardOutput());
#else
        proc.start(tr("gcc -o a.out -I") + appDir + tr("/c -L") + appDir + tr(" -L") + appDir + tr("/c -L") + appDir + tr("/lib ") + filename + libs);
        proc.waitForFinished();
        QString exeName = tr("a.out");
        QString errors(proc.readAllStandardError());
        QString output(proc.readAllStandardOutput());
#endif

        if (console())
            if (!errors.isEmpty())
                console()->error(errors);
            else
                console()->message(output);

        if (errors.size() > 0)
        {
            if (r) (*r) = 0;
            if (s) s->release();
            return;
        }

        ProcessThread * newThread = new ProcessThread(exeName,args,mainWindow);

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
	
	bool LoadCLibrariesTool::compile(const QString& filename, QString& dllName)
	{
		QString errors;
        QString output;
        QProcess proc;
        QString appDir = QCoreApplication::applicationDirPath();

		QString homeDir = GlobalSettings::tempDir();
		QDir homeDirDir(homeDir);

		proc.setWorkingDirectory(homeDir);
		
		QString libs;
		
		//if (filename.contains(tr("ode")) || filename.contains(tr("ssa")))
			libs = tr(" -lm -ltinkercellapi -lsundials_cvode -lsundials_nvecserial -llapack -lblas -lf2c");
		//else
			//libs = tr(" -lm -ltinkercellapi -fpic");


#ifdef Q_WS_WIN

		dllName.replace(tr("/"),tr("\\"));
		appDir.replace(tr("/"),tr("\\"));
		homeDir.replace(tr("/"),tr("\\"));

        proc.start(tr("\"") + appDir + tr("\"\\win32\\gcc -I\"") + appDir + ("\"\\win32\\include -I\"") + appDir + ("\"\\win32\\include\\sys -I\"") + appDir + ("\"/c -L\"") + appDir + ("\" -L\"") + appDir + ("\"/c -L\"") + appDir + ("\"\\win32\\lib -w --shared ") + filename + tr(" -o ") + dllName + tr(".dll") + libs);
        proc.waitForFinished();
        errors += (proc.readAllStandardError());
        output += tr("\n\n") + (proc.readAllStandardOutput());
#else
#ifdef Q_WS_MAC

        proc.start(tr("gcc -bundle --shared -fPIC -I") + appDir + tr("/c -L") + appDir + tr(" -L") + appDir + tr("/c -L") + appDir + tr("/lib -o ") + dllName + tr(".dylib ") + filename + libs);
        proc.waitForFinished();
        if (!errors.isEmpty())	errors += tr("\n\n");
        errors += (proc.readAllStandardError());
        if (!output.isEmpty())	output += tr("\n\n");
        output += tr("\n\n") + (proc.readAllStandardOutput());
#else
        proc.start(tr("gcc --shared -fPIC -I") + appDir + tr("/c -L") + appDir + tr(" -L") + appDir + tr("/c -L") + appDir + tr("/lib -o ") + dllName + tr(".so ") + filename + libs);
        proc.waitForFinished();
        if (!errors.isEmpty())	errors += tr("\n\n");
        errors += (proc.readAllStandardError());
        if (!output.isEmpty())	output += tr("\n\n");
        output += tr("\n\n") + (proc.readAllStandardOutput());
#endif
#endif

        if (console())
            if (!errors.isEmpty())
            {
                console()->error(errors);
                return false;
            }
            else
            {
                console()->message(output);
            }
       return true;
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
		dllName += QString::number(++numLibFiles);

        if (!compile(filename,dllName))
        {
            if (r) (*r) = 0;
            if (s) s->release();
		    return;
        }

        CThread * newThread = new CThread(mainWindow,dllName,true);
        newThread->setVoidFunction(funcname.toAscii().data());

        newThread->start();

        if (r) (*r) = 1;

        if (s) s->release();
    }
	
	void LoadCLibrariesTool::compileBuildLoadSliders(QSemaphore* s,int* r,const QString& filename,const QString& funcname, const QString& title, DataTable<qreal>& data)
    {
        if (filename.isEmpty() || filename.isNull())
        {
            if (r) (*r) = 0;
            if (s) s->release();
            return;
        }

		QString dllName("temp");
		dllName += QString::number(++numLibFiles);

        if (!compile(filename,dllName))
        {
            if (r) (*r) = 0;
            if (s) s->release();
		    return;
        }
		
		MultithreadedSliderWidget * widget = new MultithreadedSliderWidget(mainWindow, dllName, funcname, Qt::Horizontal);
		
		QStringList names(data.rowNames());
		QList<double> min, max;
		for (int i=0; i < names.size(); ++i)
		{
			names[i].replace(tr("_"),tr("."));
			names[i].replace(tr(".."),tr("_"));
			min <<  data.value(i,0);
			max << data.value(i,1);
		}
		widget->setSliders(names, min, max);
		
		widget->show();

        if (r) (*r) = 1;

        if (s) s->release();
    }

    void LoadCLibrariesTool::loadLibrary(QSemaphore* s,const QString& file)
    {
        if (mainWindow)
            mainWindow->loadDynamicLibrary(file);

        if (s) s->release();
    }

    void LoadCLibrariesTool::displayCode(QSemaphore* s,const QString& code)
    {
        if (mainWindow)
		{
            QWidget * tool = mainWindow->tool(tr("Coding Window"));
			if (tool)
			{
				CodingWindow * codingWindow = static_cast<CodingWindow*>(tool);
				if (!codingWindow->isVisible())
				{
					if (codingWindow->parentWidget())
						codingWindow->parentWidget()->show();
					else
						codingWindow->show();
				}
				codingWindow->setCode(code);
			}
        }

        if (s) s->release();
    }


    /******************************************************/

    LoadCLibrariesTool_FToS * LoadCLibrariesTool::fToS;

    int LoadCLibrariesTool::_compileAndRun(const char * cfile,const char* args)
    {
        return fToS->compileAndRun(cfile,args);
    }

    int LoadCLibrariesTool::_compileBuildLoad(const char * cfile,const char* f,const char* t)
    {
        return fToS->compileBuildLoad(cfile,f,t);
    }
	
	int LoadCLibrariesTool::_compileBuildLoadSliders(const char * cfile,const char* f,const char* t, tc_matrix m)
	{
		return fToS->compileBuildLoadSliders(cfile,f,t,m);
	}

    void LoadCLibrariesTool::_loadLibrary(const char * c)
    {
        return fToS->loadLibrary(c);
    }

    void LoadCLibrariesTool::_displayCode(const char * c)
    {
        return fToS->displayCode(c);
    }

    void  LoadCLibrariesTool::_addFunction(void (*f)(), const char * title, const char* desc, const char* cat, const char* icon, const char * family, int inMenu, int inTool, int deft)
    {
        fToS->addFunction(f, title, desc, cat, icon, family, inMenu, inTool, deft);
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
	
	int LoadCLibrariesTool_FToS::compileBuildLoadSliders(const char * cfile,const char* f,const char* t, tc_matrix m)
    {
        QSemaphore * s = new QSemaphore(1);
        int p;
		DataTable<qreal> * dat = ConvertValue(m);
        s->acquire();
        emit compileBuildLoadSliders(s,&p,ConvertValue(cfile),ConvertValue(f),ConvertValue(t),*dat);
        s->acquire();
        s->release();
        delete s;
		delete dat;
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

    void LoadCLibrariesTool_FToS::loadLibrary(const char * c)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit loadLibrary(s,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
    }

    void LoadCLibrariesTool_FToS::displayCode(const char * c)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit displayCode(s,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
    }
}

