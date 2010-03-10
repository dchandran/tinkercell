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
#include "MultithreadedSliderWidget.h"
#include <QtDebug>

namespace Tinkercell
{

    LoadCLibrariesTool::LoadCLibrariesTool() : Tool(tr("Load C Libraries"),tr("Coding")), actionsGroup(this), buttonsGroup(this)
    {
		numLibFiles = 0;
        connectTCFunctions();
        libMenu = 0;
    }

	LoadCLibrariesTool::~LoadCLibrariesTool()
	{
		for (int i=0; i < unloadFunctions.size(); ++i)
			if (unloadFunctions[i])
			{
				VoidFunction f = unloadFunctions[i];
				f();
			}
	}

    bool LoadCLibrariesTool::setMainWindow(MainWindow * main)
    {
        Tool::setMainWindow(main);
        if (mainWindow)
        {
            connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
            connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));
			connect(mainWindow,SIGNAL(dataChanged(const QList<ItemHandle*>&)),this,SLOT(dataChanged(const QList<ItemHandle*>&)));
			connect(mainWindow,SIGNAL(windowChanged(NetworkWindow*,NetworkWindow*)),this,SLOT(windowChanged(NetworkWindow*,NetworkWindow*)));
			connect(mainWindow,SIGNAL(itemsInserted(NetworkWindow * , const QList<ItemHandle*>&)),this,SLOT(itemsInserted(NetworkWindow * , const QList<ItemHandle*>&)));
			connect(mainWindow,SIGNAL(itemsRemoved(NetworkWindow * , const QList<ItemHandle*>&)),this,SLOT(itemsRemoved(NetworkWindow * , const QList<ItemHandle*>&)));

            toolLoaded(0);

            return true;
        }
        return false;
    }

	void LoadCLibrariesTool::itemsInserted(NetworkWindow * win, const QList<ItemHandle*>& handles)
	{
		if (win && handles.size() > 0)
			for (int i=0; i < callBackFunctions.size(); ++i)
				if (callBackFunctions[i])
				{
					VoidFunction f = callBackFunctions[i];
					f();
				}
	}

	void LoadCLibrariesTool::itemsRemoved(NetworkWindow * win, const QList<ItemHandle*>& handles)
	{
		if (win && handles.size() > 0)
			for (int i=0; i < callBackFunctions.size(); ++i)
				if (callBackFunctions[i])
				{
					VoidFunction f = callBackFunctions[i];
					f();
				}
	}

	void LoadCLibrariesTool::windowChanged(NetworkWindow* w1,NetworkWindow* w2)
	{
		if (w2)
			for (int i=0; i < callBackFunctions.size(); ++i)
				if (callBackFunctions[i])
				{
					VoidFunction f = callBackFunctions[i];
					f();
				}
	}

	void LoadCLibrariesTool::dataChanged(const QList<ItemHandle*>&)
	{
		for (int i=0; i < callBackFunctions.size(); ++i)
			if (callBackFunctions[i])
			{
				VoidFunction f = callBackFunctions[i];
				f();
			}
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
                QAction * action = libMenu->addMenuItem(cat, title,icon, deft > 0);
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
		if (!callBackFunctions.contains(f))
			callBackFunctions << f;
        if (s)
            s->release();
    }

	void LoadCLibrariesTool::unload(QSemaphore* s,void (*f)(void))
    {
		//if (!unloadFunctions.contains(f))
			//unloadFunctions << f;
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
        connect(&fToS,SIGNAL(compileAndRun(QSemaphore*,int*,const QString&,const QString&)),this,SLOT(compileAndRunC(QSemaphore*,int*,const QString&,const QString&)));
        connect(&fToS,SIGNAL(compileBuildLoad(QSemaphore*,int*,const QString&,const QString&,const QString&)),this,SLOT(compileBuildLoadC(QSemaphore*,int*,const QString&,const QString&,const QString&)));
		connect(&fToS,SIGNAL(compileBuildLoadSliders(QSemaphore*,int*,const QString&,const QString&,const QString&,DataTable<qreal>&)),this,SLOT(compileBuildLoadSliders(QSemaphore*,int*,const QString&,const QString&,const QString&,DataTable<qreal>&)));
        connect(&fToS,SIGNAL(loadLibrary(QSemaphore*,const QString&)),this,SLOT(loadLibrary(QSemaphore*,const QString&)));
        connect(&fToS,SIGNAL(addFunction(QSemaphore*,VoidFunction, const QString& , const QString& , const QString& , const QString& ,const QString& , int, int,int)),
                   this,SLOT(addFunction(QSemaphore*,VoidFunction,QString,QString,QString,QString,QString,int,int,int)));
        connect(&fToS,SIGNAL(callback(QSemaphore*,VoidFunction)),this,SLOT(callback(QSemaphore*,VoidFunction)));
		connect(&fToS,SIGNAL(unload(QSemaphore*,VoidFunction)),this,SLOT(unload(QSemaphore*,VoidFunction)));

    }

    typedef void (*tc_LoadCLibraries_api)(
            int (*compileAndRun)(const char * ,const char* ),
            int (*compileBuildLoad)(const char *, const char* , const char*),
			int (*compileBuildLoadSliders)(const char * ,const char* ,const char* , Matrix),
            void (*loadLib)(const char*),
            void (*addf)(void (*f)(),const char * , const char* , const char* , const char* , const char * , int , int , int ),
            void (*callback)(void (*f)()),
			void (*unload)(void (*f)())
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
                    &(_callback),
					&(_unload)
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
		proc.setWorkingDirectory(MainWindow::userTemp());

#ifdef Q_WS_WIN
		appDir.replace(tr("/"),tr("\\"));
        proc.start(tr("del a.out"));
        proc.waitForFinished();
		proc.start(tr("\"") + appDir + tr("\"\\win32\\gcc -I\"") + appDir + ("\"\\win32\\include -I\"") + appDir + ("\"\\win32\\include\\sys -I\"") + appDir + ("\"/c -L\"") + appDir + ("\"/c -L\"") + appDir + ("\"\\win32\\lib -r -w ") + filename  + tr(" -llapack -lblas -lf2c -o a.out"));
        proc.waitForFinished();
        QString errors(proc.readAllStandardError());
        QString output(proc.readAllStandardOutput());
#else
        proc.start(tr("rm a.out"));
        proc.waitForFinished();
        proc.start(tr("gcc -o a.out -I") + appDir + tr("/c -L") + appDir + tr("/c -llapack -lblas -lf2c ") + filename);
        proc.waitForFinished();
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
		dllName += QString::number(++numLibFiles);

        QString errors;
        QString output;
        QProcess proc;
        QString appDir = QCoreApplication::applicationDirPath();

		QString userHome = MainWindow::userTemp();
		QDir userHomeDir(userHome);

		proc.setWorkingDirectory(userHome);


#ifdef Q_WS_WIN

		appDir.replace(tr("/"),tr("\\"));
		userHome.replace(tr("/"),tr("\\"));

        proc.start(tr("\"") + appDir + tr("\"\\win32\\gcc -I\"") + appDir + ("\"\\win32\\include -I\"") + appDir + ("\"\\win32\\include\\sys -I\"") + appDir + ("\"/c -L\"") + appDir + ("\"/c -L\"") + appDir + ("\"\\win32\\lib -w --shared ") + filename + tr("  -llapack -lblas -lf2c -o ") + dllName + tr(".dll "));		
        proc.waitForFinished();
        errors += (proc.readAllStandardError());
        output += tr("\n\n") + (proc.readAllStandardOutput());
#else
#ifdef Q_WS_MAC

        proc.start(tr("gcc -bundle -w --shared -I") + appDir + tr("/c -L") + appDir + tr("/c  -llapack -lblas -lf2c -o ") + dllName + tr(".dylib ") + filename);
        proc.waitForFinished();
        if (!errors.isEmpty())	errors += tr("\n\n");
        errors += (proc.readAllStandardError());
        if (!output.isEmpty())	output += tr("\n\n");
        output += tr("\n\n") + (proc.readAllStandardOutput());
#else
        proc.start(tr("gcc -w --shared -fPIC -I") + appDir + tr("/c -L") + appDir + tr("/c  -llapack -lblas -lf2c -o ") + dllName + tr(".so ") + filename);
        proc.waitForFinished();
        if (!errors.isEmpty())	errors += tr("\n\n");
        errors += (proc.readAllStandardError());
        if (!output.isEmpty())	output += tr("\n\n");
        output += tr("\n\n") + (proc.readAllStandardOutput());
#endif
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

        CThread * newThread = new CThread(mainWindow,dllName,true);
        newThread->setVoidFunction(funcname.toAscii().data());

        CThread::dialog(newThread,title,QIcon(),true);
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

        QString errors;
        QString output;
        QProcess proc;
        QString appDir = QCoreApplication::applicationDirPath();

		QString userHome = MainWindow::userTemp();
		QDir userHomeDir(userHome);
		proc.setWorkingDirectory(userHome);

#ifdef Q_WS_WIN

		appDir.replace(tr("/"),tr("\\"));
		userHome.replace(tr("/"),tr("\\"));

        proc.start(tr("\"") + appDir + tr("\"\\win32\\gcc -I\"") + appDir + ("\"\\win32\\include -I\"") + appDir + ("\"\\win32\\include\\sys -I\"") + appDir + ("\"/c -L\"") + appDir + ("\"/c -L\"") + appDir + ("\"\\win32\\lib -w --shared ") + filename + tr("  -llapack -lblas -lf2c -o ") + dllName + tr(".dll "));
        proc.waitForFinished();
        errors += (proc.readAllStandardError());
        output += tr("\n\n") + (proc.readAllStandardOutput());
#else
#ifdef Q_WS_MAC

	    proc.start(tr("gcc -bundle -w --shared -I") + appDir + tr("/c -L") + appDir + tr("/c  -llapack -lblas -lf2c -o ") + dllName + tr(".dylib ") + filename);
        proc.waitForFinished();
        if (!errors.isEmpty())	errors += tr("\n\n");
        errors += (proc.readAllStandardError());
        if (!output.isEmpty())	output += tr("\n\n");
        output += tr("\n\n") + (proc.readAllStandardOutput());
#else
	    proc.start(tr("gcc -w --shared -fPIC -I") + appDir + tr("/c -L") + appDir + tr("/c  -llapack -lblas -lf2c -o ") + dllName + tr(".so ") + filename);
        proc.waitForFinished();
        if (!errors.isEmpty())	errors += tr("\n\n");
        errors += (proc.readAllStandardError());
        if (!output.isEmpty())	output += tr("\n\n");
        output += tr("\n\n") + (proc.readAllStandardOutput());
#endif
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

		
		MultithreadedSliderWidget * widget = new MultithreadedSliderWidget(mainWindow, dllName, funcname, Qt::Horizontal);
		
		QStringList names(data.getRowNames());
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
	
	int LoadCLibrariesTool::_compileBuildLoadSliders(const char * cfile,const char* f,const char* t, Matrix m)
	{
		return fToS.compileBuildLoadSliders(cfile,f,t,m);
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
		fToS.callback(f);
    }

	void  LoadCLibrariesTool::_unload(void (*f)(void))
    {
		fToS.unload(f);
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
	
	int LoadCLibrariesTool_FToS::compileBuildLoadSliders(const char * cfile,const char* f,const char* t, Matrix m)
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

    void LoadCLibrariesTool_FToS::callback(void (*f)())
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit callback(s,f);
        s->acquire();
        s->release();
        delete s;
    }

	void LoadCLibrariesTool_FToS::unload(void (*f)())
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit unload(s,f);
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
