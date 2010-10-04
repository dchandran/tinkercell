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

namespace Tinkercell
{

	class LoadCLibrariesTool_FToS : public QObject
	{
		Q_OBJECT

		typedef void (*VoidFunction)();

		signals:
		
			void compileAndRun(QSemaphore*,int*,const QString&,const QString&);
			void compileBuildLoad(QSemaphore*,int*,const QString&,const QString&,const QString&);
			void compileBuildLoadSliders(QSemaphore*,int*,const QString&,const QString&,const QString&, DataTable<qreal>& );
			void loadLibrary(QSemaphore*,const QString&);
			void addFunction(QSemaphore*,VoidFunction, const QString& , const QString& , const QString& , const QString& ,const QString& , int, int, int);
			
		public slots:
			int compileAndRun(const char * cfile,const char* args);
			int compileBuildLoad(const char * cfile,const char* f,const char* title);
			int compileBuildLoadSliders(const char * cfile,const char* f,const char* title, tc_matrix);
			void loadLibrary(const char*);
			void addFunction(VoidFunction, const char*, const char*, const char*, const char*, const char*, int, int, int);
	};

	class TINKERCELLEXPORT LoadCLibrariesTool : public Tool
	{
		Q_OBJECT

		typedef void (*VoidFunction)();

	public:
		LoadCLibrariesTool();
		~LoadCLibrariesTool();
		bool setMainWindow(MainWindow*);

	public slots:
		void windowChanged(NetworkWindow*,NetworkWindow*);
		void dataChanged(const QList<ItemHandle*>&);
		void itemsInserted(NetworkHandle * , const QList<ItemHandle*>&);
		void itemsRemoved(NetworkHandle * , const QList<ItemHandle*>& );
		void setupFunctionPointers( QLibrary * );
		void toolLoaded(Tool*);
		void compileAndRunC(const QString&,const QString&);
		void compileBuildLoadC(const QString&,const QString&,const QString&);
		bool compile(const QString& filename, QString& output);

	protected slots:
		void compileAndRunC(QSemaphore*,int*,const QString&,const QString&);
		void compileBuildLoadC(QSemaphore*,int*,const QString&,const QString&,const QString&);
		void compileBuildLoadSliders(QSemaphore*,int*,const QString&,const QString&,const QString&, DataTable<qreal>& );
		void loadLibrary(QSemaphore*,const QString&);
		void addFunction(QSemaphore*,VoidFunction, const QString& , const QString& , const QString& , const QString& ,const QString& , int, int, int);
		
	protected:
		void connectTCFunctions();
		QActionGroup actionsGroup;
		QButtonGroup buttonsGroup;
		QStringList dllFileNames;
		QHash<QAction*,QString> hashDll;
        DynamicLibraryMenu * libMenu;
		
	private:
		int numLibFiles;
		static LoadCLibrariesTool_FToS fToS;		
		static int _compileAndRun(const char * cfile,const char* args);
        static int _compileBuildLoad(const char * cfile,const char* f,const char* title);
		static int _compileBuildLoadSliders(const char * cfile,const char* f,const char* title, tc_matrix);
		static void _loadLibrary(const char*);
		static void _addFunction(VoidFunction, const char*, const char*, const char*, const char*, const char *, int, int, int);
     };
}

#endif
