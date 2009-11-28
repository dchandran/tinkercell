/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 A tool that allows import and export of sbml

****************************************************************************/

#include "TextEditor.h"
#include "NetworkWindow.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "NodesTree.h"
#include "ConnectionsTree.h"
#include "StoichiometryTool.h"
#include "ModelFileGenerator.h"
#include "ModelSummaryTool.h"
#include "SBMLTool.h"
#include <QToolButton>
#include <QFile>
#include <QPair>
#include <QRegExp>
#include <QSemaphore>

#include "SBMLDocument.h"
#include "SBMLReader.h"
#include "SBMLWriter.h"
#include "Model.h"
#include "Parameter.h"

namespace Tinkercell
{
	SBMLTool_FtoS SBMLTool::fToS;
	
	char* SBMLTool_FtoS::getSBMLSlot()
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		QString sbml;
		emit getSBML(s,sbml);
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(sbml);
	}

	char * SBMLTool::_getSBML()
	{
		return fToS.getSBMLSlot();
	}
		
	void SBMLTool::getSBML(QSemaphore* s,QString& sbml)
	{
		if (s)
			s->release();
	}
		
	
	SBMLTool::SBMLTool() : Tool(tr("SBML Reader/Writer"))
	{
		connect(&fToS,SIGNAL(getSBML(QSemaphore*,QString&)),
				this,SLOT(getSBML(QSemaphore*,QString&)));
	}

	bool SBMLTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			
		}
		return false;
	}
}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::SBMLTool * sbmlTool = new Tinkercell::SBMLTool;
	main->addTool(sbmlTool);

}
