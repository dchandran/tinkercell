/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)

 see COPYRIGHT.TXT

 The perl interpreter that runs as a separate thread and can accept strings to parse and execute
 
****************************************************************************/

#ifndef TINKERCELL_PERLINTERPRETERTHREAD_H
#define TINKERCELL_PERLINTERPRETERTHREAD_H

#include <QQueue>
#include <QLibrary>
#include <QSemaphore>
#include <QThread>
#include <QString>
#include <QDir>
#include <QFile>
#include "Tool.h"
#include "CThread.h"

namespace Tinkercell
{

    class PerlInterpreterThread : public CThread
	{
		Q_OBJECT
		
		typedef void (*initFunc)();
		typedef void (*execFunc)(const char*,const char*);
		typedef void (*finalFunc)();
		
	signals:
	
		void progress(int);
		
	public:
		PerlInterpreterThread(const QString&, MainWindow* main);
		virtual ~PerlInterpreterThread();
		void setCPointers();
		
		static QLibrary * perlLibrary;
		
	public slots:
		void initialize();
		void runCode(const QString&);
		void finalize();

	protected:
		QString perlCode;
		virtual void run();
		execFunc f;
		QQueue<QString> commandQueue;
	};
}

#endif
