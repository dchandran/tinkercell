/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYWRITE.TXT

Function that loads dll into main window

****************************************************************************/

#include "MainWindow.h"
#include "ConnectionSelection.h"
#include "NodeSelection.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);
