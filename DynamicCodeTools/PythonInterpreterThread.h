/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)

 see COPYRIGHT.TXT

 The python interpreter that runs as a separate thread and can accept strings to parse and execute

****************************************************************************/


#ifndef TINKERCELL_PYTHONINTERPRETERTHREAD_H
#define TINKERCELL_PYTHONINTERPRETERTHREAD_H

#include <QSemaphore>
#include <QThread>
#include <QDir>
#include <QFile>
#include "Tool.h"
#include "CThread.h"

namespace Tinkercell
{

	class PythonInterpreterThread : public LibraryThread
	{
  Q_OBJECT

	public:
		PythonInterpreterThread(const QString&, const QString&, MainWindow* main);
		virtual ~PythonInterpreterThread();
		void setCPointers();
		QString outputFile;
	public slots:
		void initialize();
		void runCode(const QString&);
		void finalize();
	protected:
		QString pythonCode;
		virtual void run();
	};
}

#endif
