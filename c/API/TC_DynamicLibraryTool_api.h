#ifndef TINKERCELL_TC_DYNAMICLIBRARYTOOL_API_H
#define TINKERCELL_TC_DYNAMICLIBRARYTOOL_API_H

#include "../TCstructs.h"

/*! 
 \brief compile and run a c file
 \ingroup Programming interface
*/
int (*tc_compileAndRun)(const char * command,const char* args) = 0;
/*! 
 \brief compile a c file, generate the library, and load it
 \ingroup Programming interface
*/
int (*tc_compileBuildLoad)(const char * filename,const char* function,const char* title) = 0;
/*! 
 \brief get a text from the user (dialog)
 \ingroup Dialogs
*/
char* (*tc_getString)(const char* title) = 0;
/*! 
 \brief get a file from the user (dialog)
 \ingroup Dialogs
*/
char* (*tc_getFilename)() = 0;
/*! 
 \brief get a text from the user (dialog) from a list of selections
 \ingroup Dialogs
*/
int (*tc_getFromList)(const char* title, char** list,const char* selectedString, int comboBox) = 0;
/*! 
 \brief get a number from the user (dialog)
 \ingroup Dialogs
*/
double (*tc_getNumber)(const char* title) = 0;
/*! 
 \brief get a list of numbers from the user (dialog) into the argument array
 \ingroup Dialogs
*/
void (*tc_getNumbers)(char** labels, double* result) = 0;
/*! 
 \brief run the Python code given by the string
 \ingroup Programming interface
*/
void (*tc_runPythonCode)(const char* code) = 0;
/*! 
 \brief run the Python code in the given file
 \ingroup Programming interface
*/
void  (*tc_runPythonFile)(const char* filename) = 0;
/*! 
 \brief call a function listed in the functions menu, e.g. "Deterministic simulation"
 \ingroup Programming interface
*/
void (*tc_callFunction)(const char* functionTitle) = 0;
/*! 
 \brief run a dynamic C library that contains the function "tc_main"
 \ingroup Programming interface
*/
void  (*tc_loadLibrary)(const char* filename) = 0;
/*! 
 \brief add a function to the menu of functions
 \ingroup Programming interface
*/
void  (*tc_addFunction)(void (*f)(), const char* title, const char* description, const char* category, const char* iconFile, const char * target_family, int show_menu, int in_tool_menu, int make_default) = 0;
/*! 
 \brief this function will be called whenever the model is changed
 \ingroup Programming interface
*/
void  (*tc_callback)(void (*f)(void)) = 0;
/*! 
 \brief this function will be called whenever Tinkercell exits. Use it to free memory.
 \ingroup Programming interface
*/
void  (*tc_callWhenExiting)(void (*f)(void)) = 0;
/*! 
 \brief initialize dialogs and c interface
 \ingroup init
*/
void tc_DynamicLibraryMenu_api(
		char* (*getString)(const char*),
		int (*getSelectedString)(const char*, char**,const char*, int),
		double (*getNumber)(const char*),
		void (*getNumbers)( char**, double * ),
		char* (*getFilename)(),
		void (*callFunction)(const char*)
)
{
	tc_getString = getString;
	tc_getFromList = getSelectedString;
	tc_getNumber = getNumber;
	tc_getNumbers = getNumbers;
	tc_getFilename = getFilename;
	tc_callFunction = callFunction;
}

/*! 
 \brief initialize dialogs and c interface
 \ingroup init
*/
void tc_LoadCLibraries_api(
		int (*compileAndRun)(const char * ,const char* ),
		int (*compileBuildLoad)(const char * ,const char* , const char*),
		void (*loadLibrary)(const char*),
		void  (*addFunction)(void (*f)(), const char*, const char*, const char*, const char*, const char *, int, int, int),
		void (*callback)(void (*f)(void)),
		void (*unload)(void (*f)(void))
)
{
	tc_compileAndRun = compileAndRun;
	tc_compileBuildLoad = compileBuildLoad;
	tc_loadLibrary = loadLibrary;
	tc_addFunction = addFunction;
	tc_callback = callback;
	tc_callWhenExiting = unload;
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
	tc_runPythonCode = runPythonCode;
	tc_runPythonFile = runPythonFile;
}

#endif
