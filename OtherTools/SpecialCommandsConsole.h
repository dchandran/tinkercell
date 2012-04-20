/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool updates the x,y attribute for "Cell" items when they are moved

****************************************************************************/

#ifndef TINKERCELL_SPECIALCONSOLECOMMANDS_H
#define TINKERCELL_SPECIALCONSOLECOMMANDS_H

#include <QtGui>
#include <QtDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QTimeLine>
#include <QTextEdit>
#include <QTextCursor>

#include "ConsoleWindow.h"

namespace Tinkercell
{

	class TINKERCELLEXPORT SpecialCommandsConsole : public ConsoleWindow
	{
		Q_OBJECT

	public:
		SpecialCommandsConsole();
		bool setMainWindow(MainWindow * main);
		
	private slots:
		void commandExecutedSlot(const QString&);

	private:
		static QString getCode(const QString&);

	};


}

//extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);


#endif
