/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

Function that loads dll into main window

****************************************************************************/

#include "MainWindow.h"
#include "CollisionDetection.h"
#include "ConnectionInsertion.h"
#include "ConnectionMaker.h"
#include "NodeInsertion.h"
#include "GraphicsReplaceTool.h"
#include "GraphicsTransformTool.h"
#include "GroupHandlerTool.h"
#include "TextGraphicsTool.h"
#include "NameFamilyDialog.h"
#include "ConnectionSelection.h"
#include "NodeSelection.h"
#include "TinkerCellAboutBox.h"

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);

