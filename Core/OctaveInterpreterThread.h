/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)

 see COPYRIGHT.TXT

 The python interpreter that runs as a separate thread and can accept strings to parse and execute
 
****************************************************************************/
#ifndef TINKERCELL_OCTAVEINTERPRETERTHREAD_H
#define TINKERCELL_OCTAVEINTERPRETERTHREAD_H

#include "InterpreterThread.h"

namespace Tinkercell
{

    class OctaveInterpreterThread : public InterpreterThread
	{
		Q_OBJECT
		
		typedef void (*execFunc)(const char*,const char*);
		
	public:
		OctaveInterpreterThread(const QString&, MainWindow* main);

	protected:
		virtual void run();
		execFunc f;
	};
}

#endif
