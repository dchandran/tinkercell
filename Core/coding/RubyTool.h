/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)

 see COPYRIGHT.TXT
 
****************************************************************************/

#ifndef TINKERCELL_RUBYTOOL_H
#define TINKERCELL_RUBYTOOL_H

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
#include "RubyInterpreterThread.h"

namespace Tinkercell
{
	class RubyTool_FToS : public QObject
	{
		Q_OBJECT
		signals:
			void runRubyCode(QSemaphore*,const QString&);
			void runRubyFile(QSemaphore*,const QString&);
			void addRubyPlugin(QSemaphore*,const QString&,const QString&,const QString&,const QString&, const QString&);
		public slots:
			void runRubyCode(const char*);
			void runRubyFile(const char*);
			void addRubyPlugin(const char*,const char*,const char*,const char*,const char*);
	};

	class TINKERCELLEXPORT RubyTool : public Tool
	{
		Q_OBJECT

	public:
		RubyTool();
		bool setMainWindow(MainWindow*);
		RubyInterpreterThread * rubythonInterpreter;

	public slots:
		void setupFunctionPointers( QLibrary * );
		bool loadFromDir( QDir& );
		bool loadFromDir(DynamicLibraryMenu* , QDir& );
		void toolLoaded(Tool*);
		void runRubyCode(const QString&);
		void runRubyFile(const QString&);
		void runRubyCode(QSemaphore*,const QString&);
		void runRubyFile(QSemaphore*,const QString&);
		
	protected slots:
		void buttonPressed (int);	 
		void actionTriggered(QAction *);
		void addRubyPlugin(QSemaphore*,const QString&,const QString&,const QString&,const QString&, const QString&);
	
	protected:
		void connectTCFunctions();
		QActionGroup actionsGroup;
		QButtonGroup buttonsGroup;
		QStringList rubyFileNames;
		QHash<QAction*,QString> hashPyFile;
		
	private:
		static RubyTool_FToS * fToS;
		static void _runRubyCode(const char*);
        static void _runRubyFile(const char*);
        static void _addRubyPlugin(const char*,const char*,const char*,const char*,const char*);
	};
}

#endif

