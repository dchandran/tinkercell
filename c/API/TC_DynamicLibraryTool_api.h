#ifndef TINKERCELL_TC_DYNAMICLIBRARYTOOL_API_H
#define TINKERCELL_TC_DYNAMICLIBRARYTOOL_API_H

#include "../TCstructs.h"
BEGIN_C_DECLS

/*! 
 \brief compile and run a c file
 \ingroup Programming
*/
TCAPIEXPORT int tc_compileAndRun(const char* command,const char* args);
/*! 
 \brief compile a c file, generate the library, and load it
 \ingroup Programming
*/
TCAPIEXPORT int tc_compileBuildLoad(const char* filename,const char* function,const char* title);
/*! 
 \brief compile a c file, generate the library, and load it
 \ingroup Programming
*/
TCAPIEXPORT int tc_compileBuildLoadSliders(const char* filename,const char* function,const char* title, TableOfReals inputs);
/*! 
 \brief run the Python code given by the string
 \ingroup Programming
*/
TCAPIEXPORT void tc_runPythonCode(const char* code);
/*! 
 \brief run the Python code in the given file
 \ingroup Programming
*/
TCAPIEXPORT void tc_runPythonFile(const char* filename);
/*! 
 \brief add a python script to the functions menu
 \ingroup Programming
*/
TCAPIEXPORT void tc_addPythonPlugin(const char* file,const char* name,const char* description,const char* category, const char* icon);
/*! 
 \brief call a function listed in the functions menu, e.g. "Deterministic simulation"
 \ingroup Programming
*/
TCAPIEXPORT void tc_callFunction(const char* functionTitle);
/*! 
 \brief run a dynamic C library that contains the function "tc_main"
 \ingroup Programming
*/
TCAPIEXPORT void tc_loadLibrary(const char* filename);
/*! 
 \brief add a function to the menu of functions
 \ingroup Programming
*/
TCAPIEXPORT void tc_addFunction(void (*f)(), const char* title, const char* description, const char* category, const char* iconFile, const char* target_family, int show_menu, int in_tool_menu, int make_default);
/*! 
 \brief this function will be called whenever the model is changed
 \ingroup Programming
*/
TCAPIEXPORT void tc_callback(void (*f)(void));
/*! 
 \brief this function will be called whenever Tinkercell exits. Use it to free memory.
 \ingroup Programming
*/
TCAPIEXPORT void tc_callWhenExiting(void (*f)(void));

/*! 
 \brief initialize dialogs and c interface
 \ingroup init
*/
TCAPIEXPORT void tc_DynamicLibraryMenu_api(void (*callFunction)(const char*));

/*! 
 \brief initialize dialogs and c interface
 \ingroup init
*/
TCAPIEXPORT void tc_LoadCLibraries_api(
		int (*compileAndRun)(const char* ,const char* ),
		int (*compileBuildLoad)(const char* ,const char* , const char*),
		int (*compileBuildLoadSliders)(const char* ,const char* ,const char* , TableOfReals ),
		void (*loadLibrary)(const char*),
		void  (*addFunction)(void (*f)(), const char*, const char*, const char*, const char*, const char*, int, int, int),
		void (*callback)(void (*f)(void)),
		void (*unload)(void (*f)(void))
);

/*! 
 \brief initialize dialogs and c interface
 \ingroup init
*/
TCAPIEXPORT void tc_PythonTool_api(
		void (*runPythonCode)(const char*),
		void (*runPythonFile)(const char*),
		void (*addPythonPlugin)(const char*,const char*,const char*,const char*,const char*)
);

/*! 
 \brief initialize dialogs and c interface
 \ingroup init
*/
TCAPIEXPORT void tc_OctaveTool_api(
		void (*runOctaveCode)(const char*),
		void (*runPythonFile)(const char*),
		void (*addOctavePlugin)(const char*,const char*,const char*,const char*,const char*)
);

END_C_DECLS
#endif

