/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)

 see COPYRIGHT.TXT

 The python interpreter that runs as a separate thread and can accept strings to parse and execute
 
****************************************************************************/
#ifndef TINKERCELL_PYTHONINTERPRETERTHREAD_H
#define TINKERCELL_PYTHONINTERPRETERTHREAD_H

#include "InterpreterThread.h"

namespace Tinkercell
{
	/*! \brief This class is used to embed an python interpreter inside a TinkerCell application.
	The C library responsible for embedding python is called runpy.c and is located
	inside the python/ folder
	\sa InterpreterThread
	\ingroup CAPI
	*/
    class TINKERCELLEXPORT PythonInterpreterThread : public InterpreterThread
	{
		Q_OBJECT
		
		typedef void (*initFunc)();
		typedef void (*execFunc)(const char*,const char*);
		typedef void (*finalFunc)();
		
	public:
		PythonInterpreterThread(const QString&, MainWindow* main);

	public slots:
		virtual void initialize();
		virtual void finalize();

	protected:
		virtual void run();
		execFunc f;
		bool addpathDone;
	};
}

#endif
