/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool exports English description of models

****************************************************************************/

#ifndef TINKERCELL_ENGLISHEXPORTTOOL_H
#define TINKERCELL_ENGLISHEXPORTTOOL_H

#include <QThread>
#include <QSemaphore>
#include <QStringList>
#include "NetworkWindow.h"
#include "ItemHandle.h"
#include "Tool.h"

namespace Tinkercell
{	
	/*! \brief This class exports Octave ODE file
	/ingrou plugins
	*/
	class TINKERCELLEXPORT EnglishExporter : public Tool
	{
		Q_OBJECT

	public:

		EnglishExporter();
		bool setMainWindow(MainWindow * main);
		
	private slots:
		void exportEnglish();
	};
}

//extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif

