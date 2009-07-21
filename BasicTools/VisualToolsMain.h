/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 Function that loads dll into main window

****************************************************************************/

#include "Core/MainWindow.h"
#include "BasicTools/BasicGraphicsToolbox.h"
#include "BasicTools/CollisionDetection.h"
#include "BasicTools/GraphicsReplaceTool.h"
#include "BasicTools/GraphicsTransformTool.h"
#include "BasicTools/GroupHandlerTool.h"
#include "BasicTools/TextGraphicsTool.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);
