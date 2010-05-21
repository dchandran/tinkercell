/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
The octave interpreter that runs as a separate thread and can accept strings to parse and execute


****************************************************************************/

#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "OctaveInterpreterThread.h"

namespace Tinkercell
{
	
    OctaveInterpreterThread::OctaveInterpreterThread(const QString & dllname, MainWindow* main)
        : InterpreterThread(dllname,main)
    {
    	f = 0;
    }
    
    void OctaveInterpreterThread::run()
    {
        if (!lib || !lib->isLoaded() || code.isEmpty()) return;

        QString script;
		script =  QObject::tr("import sys\n_outfile = open('py.out','w')\nsys.stdout = _outfile;\n");
		script += code;
		script +=  QObject::tr("\n_outfile.close();\nprint _\n");

        if (!f)
            f = (execFunc)lib->resolve("exec");

        if (f)
        {
            QString currentDir = QDir::currentPath();
            QDir::setCurrent(MainWindow::tempDir());

            f(script.toAscii().data(),"octave.out");

            QDir::setCurrent(currentDir);
        }
		
		if (!commandQueue.isEmpty())
		{
			code = codeQueue.dequeue();
			run();
		}
    }

}
