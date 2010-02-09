#ifndef TINKERCELL_TC_DYNAMICLIBRARYTOOL_API_H
#define TINKERCELL_TC_DYNAMICLIBRARYTOOL_API_H

#include "../TCstructs.h"

int (*_tc_compileAndRun)(const char * command,const char* args) = 0;
/*! 
 \brief compile and run a c file
 \ingroup Programming interface
*/
int tc_compileAndRun(const char * command,const char* args)
{
	if (_tc_compileAndRun)
		return _tc_compileAndRun(command,args);
	return 0;
}

int (*_tc_compileBuildLoad)(const char * filename,const char* function,const char* title) = 0;
/*! 
 \brief compile a c file, generate the library, and load it
 \ingroup Programming interface
*/
int tc_compileBuildLoad(const char * filename,const char* function,const char* title)
{
	if (_tc_compileBuildLoad)
		return _tc_compileBuildLoad(filename,function,title);
	return 0;
}

int (*_tc_compileBuildLoadSliders)(const char * filename,const char* function,const char* title, Matrix inputs) = 0;
/*! 
 \brief compile a c file, generate the library, and load it
 \ingroup Programming interface
*/
int tc_compileBuildLoadSliders(const char * filename,const char* function,const char* title, Matrix inputs)
{
	if (_tc_compileBuildLoadSliders)
		return _tc_compileBuildLoadSliders(filename,function,title,inputs);
	return 0;
}

void (*_tc_runPythonCode)(const char* code) = 0;
/*! 
 \brief run the Python code given by the string
 \ingroup Programming interface
*/
void tc_runPythonCode(const char* code)
{
	if (_tc_runPythonCode)
		_tc_runPythonCode(code);
}

void  (*_tc_runPythonFile)(const char* filename) = 0;
/*! 
 \brief run the Python code in the given file
 \ingroup Programming interface
*/
void  tc_runPythonFile(const char* filename)
{
	if (_tc_runPythonFile)
		_tc_runPythonFile(filename);
}

void (*_tc_callFunction)(const char* functionTitle) = 0;
/*! 
 \brief call a function listed in the functions menu, e.g. "Deterministic simulation"
 \ingroup Programming interface
*/
void tc_callFunction(const char* functionTitle)
{
	if (_tc_callFunction)
		_tc_callFunction(functionTitle);
}

void  (*_tc_loadLibrary)(const char* filename) = 0;
/*! 
 \brief run a dynamic C library that contains the function "tc_main"
 \ingroup Programming interface
*/
void  tc_loadLibrary(const char* filename)
{
	if (_tc_loadLibrary)
		_tc_loadLibrary(filename);
}

void  (*_tc_addFunction)(void (*f)(), const char* title, const char* description, const char* category, const char* iconFile, const char * target_family, int show_menu, int in_tool_menu, int make_default) = 0;
/*! 
 \brief add a function to the menu of functions
 \ingroup Programming interface
*/
void  tc_addFunction(void (*f)(), const char* title, const char* description, const char* category, const char* iconFile, const char * target_family, int show_menu, int in_tool_menu, int make_default)
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
		void (*callFunction)(const char*)
)
{
	_tc_callFunction = callFunction;
}

/*! 
 \brief initialize dialogs and c interface
 \ingroup init
*/
void tc_LoadCLibraries_api(
		int (*compileAndRun)(const char * ,const char* ),
		int (*compileBuildLoad)(const char * ,const char* , const char*),
		int (*compileBuildLoadSliders)(const char * ,const char* ,const char* , Matrix ),
		void (*loadLibrary)(const char*),
		void  (*addFunction)(void (*f)(), const char*, const char*, const char*, const char*, const char *, int, int, int),
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
		void (*runPythonCode)(const char*),
		void (*runPythonFile)(const char*)
)
{
	_tc_runPythonCode = runPythonCode;
	_tc_runPythonFile = runPythonFile;
}

#endif
