/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Function that loads dll into main window

****************************************************************************/

#include "NodesTreeMain.h"


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

     Tinkercell::NodesTree * nodesTree = new Tinkercell::NodesTree;
     /*if (main->tools.contains(nodesTree->name))
          delete nodesTree;
	else
          nodesTree->setMainWindow(main);*/

     Tinkercell::ConnectionsTree * connectionsTree = new Tinkercell::ConnectionsTree;
     /*if (main->tools.contains(connectionsTree->name))
          delete connectionsTree;
     else
          connectionsTree->setMainWindow(main);*/
}

