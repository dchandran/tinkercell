/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Function that loads dll into main window

****************************************************************************/

#include "Core/MainWindow.h"
#include "DynamicCodeTools/LoadCLibraries.h"
#include "DynamicCodeTools/PythonTool.h"
#include "DynamicCodeTools/CodingWindow.h"
#include "DynamicCodeTools/DynamicLibraryMenu.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);
