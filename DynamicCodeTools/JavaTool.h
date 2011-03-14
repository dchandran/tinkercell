/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)

 see COPYRIGHT.TXT

****************************************************************************/

#ifndef TINKERCELL_JAVATOOL_H
#define TINKERCELL_JAVATOOL_H

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
#include "JavaInterpreterThread.h"

namespace Tinkercell
{

	class JavaTool_FToS : public QObject
	{
		Q_OBJECT
		signals:
			void runJavaCode(QSemaphore*,const QString&);
			void addJavaPlugin(QSemaphore*,const QString&,const QString&,const QString&,const QString&, const QString&);
		public slots:
			void runJavaCode(const char*);
			void addJavaPlugin(const char*,const char*,const char*,const char*,const char*);
	};

	class TINKERCELLEXPORT JavaTool : public Tool
	{
		Q_OBJECT

	public:
		JavaTool();
		bool setMainWindow(MainWindow*);
		JavaInterpreterThread * javaInterpreter;

	public slots:
		void setupFunctionPointers( QLibrary * );
		bool loadFromDir( QDir& );
		bool loadFromDir(DynamicLibraryMenu* , QDir& );
		void toolLoaded(Tool*);
		void runJavaCode(const QString&);
		void runJavaCode(QSemaphore*,const QString&);
		
	protected slots:
		void buttonPressed (int);	 
		void actionTriggered(QAction*);
		void addJavaPlugin(QSemaphore*,const QString&,const QString&,const QString&,const QString&, const QString&);

	protected:
		void connectTCFunctions();
		QActionGroup actionsGroup;
		QButtonGroup buttonsGroup;
		QStringList javaFileNames;
		QHash<QAction*,QString> hashJavaCode;
		
	private:
		static JavaTool_FToS fToS;
		static void _runJavaCode(const char*);
        static void _addJavaPlugin(const char*,const char*,const char*,const char*,const char*);
	};
}

#endif

