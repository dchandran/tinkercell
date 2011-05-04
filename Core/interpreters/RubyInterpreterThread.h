/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)

 see COPYRIGHT.TXT

 The ruby interpreter that runs as a separate thread and can accept strings to parse and execute
 
****************************************************************************/
#ifndef TINKERCELL_RUBYINTERPRETERTHREAD_H
#define TINKERCELL_RUBYINTERPRETERTHREAD_H

#include "InterpreterThread.h"

namespace Tinkercell
{
	/*! \brief This class is used to embed an ruby interpreter inside a TinkerCell application.
	The C library responsible for embedding ruby is called runruby.c and is located
	inside the ruby/ folder
	\sa InterpreterThread
	\ingroup CAPI
	*/
    class TINKERCELLEXPORT RubyInterpreterThread : public InterpreterThread
	{
		Q_OBJECT
		
		typedef void (*initFunc)();
		typedef void (*execFunc)(const char*);
		typedef void (*finalFunc)();
		
	public:
		RubyInterpreterThread(const QString&, MainWindow* main);

		/*! \brief the folder where tinkercell will look for ruby files, defaults to /ruby*/
		static QString RUBY_FOLDER;
		/*! \brief the file where tinkercell will write outputs from ruby, defaults to tmp/ruby.out*/
		static QString RUBY_OUTPUT_FILE;
		/*! \brief the file where tinkercell will write errors from ruby, defaults to tmp/ruby.err*/
		static QString RUBY_ERROR_FILE;

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
