#include "TC_DynamicLibraryTool_api.h"

int (*_tc_compileAndRun)(String command,String args) = 0;
/*! 
 \brief compile and run a c file
 \ingroup Programming interface
*/
int tc_compileAndRun(String command,String args)
{
	if (_tc_compileAndRun)
		return _tc_compileAndRun(command,args);
	return 0;
}

int (*_tc_compileBuildLoad)(String filename,String function,String title) = 0;
/*! 
 \brief compile a c file, generate the library, and load it
 \ingroup Programming interface
*/
int tc_compileBuildLoad(String filename,String function,String title)
{
	if (_tc_compileBuildLoad)
		return _tc_compileBuildLoad(filename,function,title);
	return 0;
}

int (*_tc_compileBuildLoadSliders)(String filename,String function,String title, TableOfReals inputs) = 0;
/*! 
 \brief compile a c file, generate the library, and load it
 \ingroup Programming interface
*/
int tc_compileBuildLoadSliders(String filename,String function,String title, TableOfReals inputs)
{
	if (_tc_compileBuildLoadSliders)
		return _tc_compileBuildLoadSliders(filename,function,title,inputs);
	return 0;
}

void (*_tc_runPythonCode)(String code) = 0;
/*! 
 \brief run the Python code given by the string
 \ingroup Programming interface
*/
void tc_runPythonCode(String code)
{
	if (_tc_runPythonCode)
		_tc_runPythonCode(code);
}

void  (*_tc_runPythonFile)(String filename) = 0;
/*! 
 \brief run the Python code in the given file
 \ingroup Programming interface
*/
void  tc_runPythonFile(String filename)
{
	if (_tc_runPythonFile)
		_tc_runPythonFile(filename);
}

void  (*_tc_addPythonPlugin)(String,String,String,String, String) = 0;
/*! 
 \brief add a python script to the functions menu
 \ingroup Programming interface
*/
void  tc_addPythonPlugin(String file,String name,String description,String category, String icon)
{
	if (_tc_addPythonPlugin)
		_tc_addPythonPlugin(file,name,description,category,icon);
}

void (*_tc_runOctaveCode)(String code) = 0;
/*! 
 \brief run the Octave code given by the string
 \ingroup Programming interface
*/
void tc_runOctaveCode(String code)
{
	if (_tc_runOctaveCode)
		_tc_runOctaveCode(code);
}

void  (*_tc_runOctaveFile)(String filename) = 0;
/*! 

 \brief run the Octave code in the given file

 \ingroup Programming interface

*/
void  tc_runOctaveFile(String filename)
{
	if (_tc_runOctaveFile)
		_tc_runOctaveFile(filename);
}

void  (*_tc_addOctavePlugin)(String,String,String,String, String) = 0;
/*! 

 \brief add a Octave script to the functions menu

 \ingroup Programming interface

*/
void  tc_addOctavePlugin(String file,String name,String description,String category, String icon)
{
	if (_tc_addOctavePlugin)
		_tc_addOctavePlugin(file,name,description,category,icon);
}


void (*_tc_callFunction)(String functionTitle) = 0;
/*! 
 \brief call a function listed in the functions menu, e.g. "Deterministic simulation"
 \ingroup Programming interface
*/
void tc_callFunction(String functionTitle)
{
	if (_tc_callFunction)
		_tc_callFunction(functionTitle);
}

void  (*_tc_loadLibrary)(String filename) = 0;
/*! 
 \brief run a dynamic C library that contains the function "tc_main"
 \ingroup Programming interface
*/
void  tc_loadLibrary(String filename)
{
	if (_tc_loadLibrary)
		_tc_loadLibrary(filename);
}

void  (*_tc_addFunction)(void (*f)(), String title, String description, String category, String iconFile, String target_family, int show_menu, int in_tool_menu, int make_default) = 0;
/*! 
 \brief add a function to the menu of functions
 \ingroup Programming interface
*/
void  tc_addFunction(void (*f)(), String title, String description, String category, String iconFile, String target_family, int show_menu, int in_tool_menu, int make_default)
{
	if (_tc_addFunction)
		_tc_addFunction(f,title,description, category, iconFile, target_family, show_menu, in_tool_menu, make_default);
}

void  (*_tc_callback)(void (*f)(void)) = 0;
/*! 
 \brief this function will be called whenever the model is changed
 \ingroup Programming interface
*/
void  tc_callback(void (*f)(void))
{
	if (_tc_callback)
		_tc_callback(f);
}

void  (*_tc_callWhenExiting)(void (*f)(void)) = 0;
/*! 
 \brief this function will be called whenever Tinkercell exits. Use it to free memory.
 \ingroup Programming interface
*/
void  tc_callWhenExiting(void (*f)(void))
{
	if (_tc_callWhenExiting)
		_tc_callWhenExiting(f);
}

/*! 
 \brief initialize dialogs and c interface
 \ingroup init
*/
void tc_DynamicLibraryMenu_api(
		void (*callFunction)(String)
)
{
	_tc_callFunction = callFunction;
}

/*! 
 \brief initialize dialogs and c interface
 \ingroup init
*/
void tc_LoadCLibraries_api(
		int (*compileAndRun)(String ,String ),
		int (*compileBuildLoad)(String ,String , String),
		int (*compileBuildLoadSliders)(String ,String ,String , TableOfReals ),
		void (*loadLibrary)(String),
		void  (*addFunction)(void (*f)(), String, String, String, String, String, int, int, int),
		void (*callback)(void (*f)(void)),
		void (*unload)(void (*f)(void))
)
{
	_tc_compileAndRun = compileAndRun;
	_tc_compileBuildLoad = compileBuildLoad;
	_tc_compileBuildLoadSliders = compileBuildLoadSliders;
	_tc_loadLibrary = loadLibrary;
	_tc_addFunction = addFunction;
	_tc_callback = callback;
	_tc_callWhenExiting = unload;
}

/*! 
 \brief initialize dialogs and c interface
 \ingroup init
*/
void tc_PythonTool_api(
		void (*runPythonCode)(String),
		void (*runPythonFile)(String),
		void (*addPythonPlugin)(String,String,String,String,String)
)
{
	_tc_runPythonCode = runPythonCode;
	_tc_runPythonFile = runPythonFile;
	_tc_addPythonPlugin = addPythonPlugin;
}


/*! 
 \brief initialize dialogs and c interface
 \ingroup init
*/
void tc_OctaveTool_api(
		void (*runOctaveCode)(String),
		void (*runOctaveFile)(String),
		void (*addOctavePlugin)(String,String,String,String,String)
)
{
	_tc_runOctaveCode = runOctaveCode;
	_tc_runOctaveFile = runOctaveFile;
	_tc_addOctavePlugin = addOctavePlugin;
}


