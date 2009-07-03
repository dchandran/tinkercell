/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)

 see COPYRIGHT.TXT

 Provides a toolbar with buttons that call C functions (run of separate threads)

****************************************************************************/


#ifndef TINKERCELL_LOADCLIBRARIES_H
#define TINKERCELL_LOADCLIBRARIES_H

#include <QMainWindow>
#include <QHash>
#include <QToolButton>
#include <QSemaphore>
#include <QThread>
#include <QMenu>
#include <QFile>
#include <QActionGroup>
#include <QButtonGroup>
#include "Tool.h"
#include "DynamicLibraryMenu.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	class LoadCLibrariesTool_FToS : public QObject
	{
		Q_OBJECT
		signals:
			void compileAndRun(QSemaphore*,int*,const QString&,const QString&);
			void compileBuildLoad(QSemaphore*,int*,const QString&,const QString&);
			void loadLibrary(QSemaphore*,const QString&);
		public slots:
			int compileAndRun(const char * cfile,const char* args);
			int compileBuildLoad(const char * cfile,const char* f);
			void loadLibrary(const char*);
	};

	class MY_EXPORT LoadCLibrariesTool : public Tool
	{
		Q_OBJECT

	public:
		LoadCLibrariesTool();
		bool setMainWindow(MainWindow*);

	public slots:
		void setupFunctionPointers( QLibrary * );
		void loadFromFile(DynamicLibraryMenu* , QFile& file);
		void pluginLoaded(const QString&);

	protected slots:
		void compileAndRunC(QSemaphore*,int*,const QString&,const QString&);
		void compileBuildLoadC(QSemaphore*,int*,const QString&,const QString&);
		void loadLibrary(QSemaphore*,const QString&);
		void buttonPressed (int);
		void actionTriggered(QAction *);

	protected:
		void connectTCFunctions();
		QActionGroup actionsGroup;
		QButtonGroup buttonsGroup;
		QStringList dllFileNames;
		QHash<QAction*,QString> hashDll;

	private:
		static LoadCLibrariesTool_FToS fToS;

		static int _compileAndRun(const char * cfile,const char* args);
		static int _compileBuildLoad(const char * cfile,const char* f);
		static void _loadLibrary(const char*);

	};
}

#endif
