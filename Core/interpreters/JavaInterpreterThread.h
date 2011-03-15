/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)

 see COPYRIGHT.TXT

 The java interpreter that runs as a separate thread and can accept strings to parse and execute
 
****************************************************************************/
#ifndef TINKERCELL_JAVAINTERPRETERTHREAD_H
#define TINKERCELL_JAVAINTERPRETERTHREAD_H

#include <QRegExp>
#include "InterpreterThread.h"

namespace Tinkercell
{
	/*! \brief This class is used to embed a java interpreter inside a TinkerCell application.
	The C library responsible for embedding octave is called runjava.cpp and is located
	inside the java folder. The interpreter uses two libraries -- one for embedding Java
	and another for extending Java with the TinkerCell C API.
	\sa PythonInterpreterThread
	\ingroup CAPI
	*/
    class TINKERCELLEXPORT JavaInterpreterThread : public InterpreterThread
	{
		Q_OBJECT
		
		typedef int (*initFunc)(const char *);
		typedef int (*execFunc)(const char*,const char*,const char*);
		typedef void (*finalFunc)();
		
	public:
		/*! \brief initialize the thread that will embed and extend octave. 
			The embed library is ASSUMED to be named tinkercell.oct
		* \param QString folder where the two octave libraries are located
		* \param QString name of the octave embed library
		*/
		JavaInterpreterThread(const QString&, const QString&, MainWindow* main);
		/*! \brief the folder where tinkercell will look for java files, defaults to /java*/
		static QString JAVA_FOLDER;
		/*! \brief requests main window to load all the C pointers for the C API inside the embedded library
		*/
		virtual void setCPointers();
	
	public slots:
		virtual void initialize();
		virtual void finalize();
		virtual void toolLoaded(Tool*);

	protected:
		virtual void run();
		execFunc f;
		/*! \brief library with all the C API functions */
		QLibrary * swigLib;
		QRegExp regexp;
	};
}

#endif

