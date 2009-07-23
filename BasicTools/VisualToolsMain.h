/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 Function that loads dll into main window

****************************************************************************/

#include "MainWindow.h"
#include "BasicGraphicsToolbox.h"
#include "CollisionDetection.h"
#include "GraphicsReplaceTool.h"
#include "GraphicsTransformTool.h"
#include "GroupHandlerTool.h"
#include "TextGraphicsTool.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);
