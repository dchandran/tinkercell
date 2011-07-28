/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool exports Octave models

****************************************************************************/

#ifndef TINKERCELL_OCTAVEEXPORTTOOL_H
#define TINKERCELL_OCTAVEEXPORTTOOL_H

#include <QThread>
#include <QSemaphore>
#include <QStringList>
#include "NetworkWindow.h"
#include "ItemHandle.h"
#include "Tool.h"

#ifndef TINKERCELLEXPORT
#ifdef Q_WS_WIN
#   if defined(TC_EXPORTS) || defined(TinkerCell_EXPORTS)
#       define TINKERCELLEXPORT __declspec(dllexport)
#   else
#       define TINKERCELLEXPORT __declspec(dllimport)
#   endif
#else
#    define TINKERCELLEXPORT
#endif
#endif

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
	
	public slots:
		void exportOctave(const QString& filename);
	
	private slots:
		void exportOctave();
	};
}

//extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif

