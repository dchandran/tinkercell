/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)

 see COPYRIGHT.TXT

 The python interpreter that runs as a separate thread and can accept strings to parse and execute
 
****************************************************************************/
#ifndef TINKERCELL_GENERICINTERPRETERTHREAD_H
#define TINKERCELL_GENERICINTERPRETERTHREAD_H

#include <QQueue>
#include <QLibrary>
#include <QSemaphore>
#include <QThread>
#include <QString>
#include <QDir>
#include <QFile>
#include "CThread.h"

#ifdef Q_WS_WIN
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{
	/*! \brief This class is used to run interpreters such as python, perl, octave, R, etc.
	This is the parent class that provides the basic structure for loading the library that will
	embed one of these languages.
	\sa PythonInterpreterThread
	\sa OctaveInterpreterThread
	\ingroup CAPI
	*/
    class TINKERCELLEXPORT InterpreterThread : public CThread
	{
		Q_OBJECT

	public:
		InterpreterThread(const QString&, MainWindow* main);
		virtual ~InterpreterThread();
		virtual void setCPointers();
		
	public slots:
		virtual void initialize();
		virtual void exec(const QString&);
		virtual void finalize();

	protected:
		QString code;
		virtual void run();
		QQueue<QString> codeQueue;
	};
}

#endif
