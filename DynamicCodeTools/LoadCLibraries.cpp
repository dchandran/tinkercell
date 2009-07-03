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
#include "OutputWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "OutputWindow.h"
#include "LoadCLibraries.h"
#include <QtDebug>

namespace Tinkercell
{

	 LoadCLibrariesTool::LoadCLibrariesTool() : Tool(tr("Load C Libraries")), actionsGroup(this), buttonsGroup(this)
	 {
		connect(&actionsGroup,SIGNAL(triggered ( QAction *  )),this,SLOT(actionTriggered ( QAction *  )));
		connect(&buttonsGroup,SIGNAL(buttonPressed ( int  )),this,SLOT(buttonPressed ( int  )));
		connectTCFunctions();
	 }

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
  		 #ifdef Q_WS_MAC
		 appDir += tr("/../../..");
		 #endif

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
	 }

	 bool LoadCLibrariesTool::setMainWindow(MainWindow * main)
	 {
		 Tool::setMainWindow(main);
		 if (mainWindow)
		 {
			 connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
			 connect(mainWindow,SIGNAL(pluginLoaded(const QString&)),this,SLOT(pluginLoaded(const QString&)));

			 pluginLoaded(tr(""));

			 return true;
		 }
		 return false;
	 }

	 void LoadCLibrariesTool::pluginLoaded(const QString&)
	 {
		static bool connected = false;

// 		if (!connected && mainWindow->tools.contains(tr("Dynamic Library Menu")))
// 		{
// 			QWidget * widget = mainWindow->tools[tr("Dynamic Library Menu")];
// 			if (widget)
// 			{
// 				DynamicLibraryMenu * libMenu = static_cast<DynamicLibraryMenu*>(widget);
//
// 				QString filename(tr("clibraries.txt"));
//
// 				QString appDir = QCoreApplication::applicationDirPath();
// 				#ifdef Q_WS_MAC
// 				appDir += tr("/../../..");
// 				#endif
//
// 				QString name[] = {	MainWindow::userHome() + tr("/") + PROJECTNAME + tr("/") + filename,
// 									filename,
// 									QDir::currentPath() + tr("/") + filename,
// 									appDir + tr("/") + filename };
//
// 				QFile file;
// 				bool opened = false;
// 				for (int i=0; i < 4; ++i)
// 				{
// 					file.setFileName(name[i]);
// 					if (file.open(QFile::ReadOnly | QFile::Text))
// 					{
// 						opened = true;
// 						break;
// 					}
// 				}
// 				if (!opened)
// 				{
// 					OutputWindow::error(filename +  tr(" file not found"));
// 				}
// 				else
// 				{
// 					loadFromFile(libMenu,file);
// 					file.close();
// 					connected = true;
// 				}
//
// 				connected = true;
// 			}
// 		}
	 }

	 void LoadCLibrariesTool::buttonPressed ( int id )
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

     void LoadCLibrariesTool::connectTCFunctions()
	{
		connect(&fToS,SIGNAL(compileAndRun(QSemaphore*,int*,const QString&,const QString&)),this,SLOT(compileAndRunC(QSemaphore*,int*,const QString&,const QString&)));
		connect(&fToS,SIGNAL(compileBuildLoad(QSemaphore*,int*,const QString&,const QString&)),this,SLOT(compileBuildLoadC(QSemaphore*,int*,const QString&,const QString&)));
		connect(&fToS,SIGNAL(loadLibrary(QSemaphore*,const QString&)),this,SLOT(loadLibrary(QSemaphore*,const QString&)));
	}

	typedef void (*tc_LoadCLibraries_api)(
		int (*compileAndRun)(const char * ,const char* ),
		int (*compileBuildLoad)(const char * ,const char* ),
		void (*loadLib)(const char*)
	);

	void LoadCLibrariesTool::setupFunctionPointers( QLibrary * library)
	{
		tc_LoadCLibraries_api f = (tc_LoadCLibraries_api)library->resolve("tc_LoadCLibraries_api");
		if (f)
		{
			//qDebug() << "tc_LoadCLibrariesTool_api resolved";
			f(
				&(_compileAndRun),
				&(_compileBuildLoad),
				&(_loadLibrary)
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
  		#ifdef Q_WS_MAC
		appDir += tr("/../../..");
		#endif
		QProcess proc;
		#ifdef Q_WS_WIN
		proc.start(tr("del a.out"));
		proc.waitForFinished();
		//qDebug() << tr("tcc -r -w ") + filename + tr(" -o a.out");
		proc.start(tr("\"") + appDir + tr("\"\\tcc -r -w TCFunctions.c ") + filename  + tr(" -o a.out"));
		proc.waitForFinished();
		QString errors(proc.readAllStandardError());
		QString output(proc.readAllStandardOutput());
		/*qDebug() << tr("tcc -w -run a.out ") + args;
		proc.start(tr("tcc -w -run a.out ") + args);
		proc.waitForFinished();
		if (!errors.isEmpty())	errors += tr("\n\n");
		errors += (proc.readAllStandardError());
		if (!output.isEmpty())	output += tr("\n\n");
		output += tr("\n\n") + (proc.readAllStandardOutput());*/
        #else
		proc.start(tr("rm a.out"));
		proc.waitForFinished();
		//qDebug() << tr("gcc -o a.out ") + filename;
		proc.start(tr("gcc -o a.out ") + filename);
		proc.waitForFinished();
		QString errors(proc.readAllStandardError());
		QString output(proc.readAllStandardOutput());
		/*qDebug() << tr("./a.out ") + args;
		proc.start(tr("./a.out ") + args);
		proc.waitForFinished();
		if (!errors.isEmpty())	errors += tr("\n\n");
		errors += (proc.readAllStandardError());
		if (!output.isEmpty())	output += tr("\n\n");
		output += tr("\n\n") + (proc.readAllStandardOutput());*/
		#endif

		if (!errors.isEmpty())
			OutputWindow::error(errors);
		else
			OutputWindow::message(output);

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
		if (ProcessThread::ThreadDialog(mainWindow,newThread,title))
			newThread->start();


		if (r) (*r) = 1;
		if (s) s->release();
	}

	void LoadCLibrariesTool::compileBuildLoadC(QSemaphore* s,int* r,const QString& filename,const QString& funcname)
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
  		#ifdef Q_WS_MAC
		appDir += tr("/../../..");
		#endif

		#ifdef Q_WS_WIN
		proc.start(tr("del ") + dllName + tr(".dll"));
		proc.waitForFinished();
		proc.start(tr("\"") + appDir + tr("\"\\tcc -w -shared -rdynamic -o ") + dllName + tr(".dll ") + filename);
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
				OutputWindow::error(errors);
			else
				OutputWindow::message(output);

		if (errors.size() > 0)
		{
			if (r) (*r) = 0;
			if (s) s->release();
			return;
		}

		LibraryThread * newThread = new LibraryThread(dllName,funcname,mainWindow,emptyMatrix());
		if (LibraryThread::ThreadDialog(mainWindow,newThread,dllName))
			newThread->start();

		if (r) (*r) = 1;
		if (s) s->release();
	}

	void LoadCLibrariesTool::loadLibrary(QSemaphore* s,const QString& file)
	{
		QString str = file;
		QRegExp regex("\\.[^\\.]+");
		if (str.contains(regex)) str.replace(regex,tr(""));

		LibraryThread * newThread = new LibraryThread(str,tr("run"),mainWindow,emptyMatrix());
		if (LibraryThread::ThreadDialog(mainWindow,newThread,str))
			newThread->start();

		if (s) s->release();
	}


	/******************************************************/

	LoadCLibrariesTool_FToS LoadCLibrariesTool::fToS;

	int LoadCLibrariesTool::_compileAndRun(const char * cfile,const char* args)
	{
		return fToS.compileAndRun(cfile,args);
	}

	int LoadCLibrariesTool::_compileBuildLoad(const char * cfile,const char* f)
	{
		return fToS.compileBuildLoad(cfile,f);
	}

	void LoadCLibrariesTool::_loadLibrary(const char * c)
	{
		return fToS.loadLibrary(c);
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

	int LoadCLibrariesTool_FToS::compileBuildLoad(const char * cfile,const char* f)
	{
		QSemaphore * s = new QSemaphore(1);
		int p;
		s->acquire();
		emit compileBuildLoad(s,&p,ConvertValue(cfile),ConvertValue(f));
		s->acquire();
		s->release();
		delete s;
		return p;
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
