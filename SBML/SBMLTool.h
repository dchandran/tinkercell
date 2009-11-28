/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Provides a text window where C code can be written and run dynamically
 
****************************************************************************/

#ifndef TINKERCELL_SBMLEXPORTIMPORTTOOL_H
#define TINKERCELL_SBMLEXPORTIMPORTTOOL_H

#include <QHash>
#include "NetworkWindow.h"
#include "MainWindow.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{
	class SBMLTool_FtoS : public QObject
	{
		Q_OBJECT
		
	signals:
		void getSBML(QSemaphore*,QString&);
		
	public:
		char* getSBMLSlot();
	};
	
	class MY_EXPORT SBMLTool : public Tool
	{
	    Q_OBJECT

	public:
		SBMLTool();
		
	private slots:
		void getSBML(QSemaphore*,QString&);
		
	private:
		static char* _getSBML();
		static SBMLTool_FtoS fToS;
	};

}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif
