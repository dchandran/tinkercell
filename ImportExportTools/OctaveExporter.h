/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool exports Octave models

****************************************************************************/

#ifndef TINKERCELL_SBMLIMPORTEXPORTTOOL_H
#define TINKERCELL_SBMLIMPORTEXPORTTOOL_H

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
	class TINKERCELLEXPORT OctaveExporter : public Tool
	{
		Q_OBJECT

	public:

		OctaveExporter();
		bool setMainWindow(MainWindow * main);
		
	private slots:
		void exportOctave();
	};
}

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif

