#ifndef TINKERCELL_TC_DYNAMICLIBRARYTOOL_API_H
#define TINKERCELL_TC_DYNAMICLIBRARYTOOL_API_H

#include "../TCstructs.h"

/*! 
 \brief compile and run a c file
 \ingroup Programming
*/
TCAPIEXPORT int tc_compileAndRun(String command,String args);
/*! 
 \brief compile a c file, generate the library, and load it
 \ingroup Programming
*/
TCAPIEXPORT int tc_compileBuildLoad(String filename,String function,String title);
/*! 
 \brief compile a c file, generate the library, and load it
 \ingroup Programming
*/
TCAPIEXPORT int tc_compileBuildLoadSliders(String filename,String function,String title, TableOfReals inputs);
/*! 
 \brief run the Python code given by the string
 \ingroup Programming
*/
TCAPIEXPORT void tc_runPythonCode(String code);
/*! 
 \brief run the Python code in the given file
 \ingroup Programming
*/
TCAPIEXPORT void tc_runPythonFile(String filename);
/*! 
 \brief add a python script to the functions menu
 \ingroup Programming
*/
TCAPIEXPORT void tc_addPythonPlugin(String file,String name,String description,String category, String icon);
/*! 
 \brief call a function listed in the functions menu, e.g. "Deterministic simulation"
 \ingroup Programming
*/
TCAPIEXPORT void tc_callFunction(String functionTitle);
/*! 
 \brief run a dynamic C library that contains the function "tc_main"
 \ingroup Programming
*/
TCAPIEXPORT void tc_loadLibrary(String filename);
/*! 
 \brief add a function to the menu of functions
 \ingroup Programming
*/
TCAPIEXPORT void tc_addFunction(void (*f)(), String title, String description, String category, String iconFile, String target_family, int show_menu, int in_tool_menu, int make_default);
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
TCAPIEXPORT void tc_DynamicLibraryMenu_api(void (*callFunction)(String));

/*! 
 \brief initialize dialogs and c interface
 \ingroup init
*/
TCAPIEXPORT void tc_LoadCLibraries_api(
		int (*compileAndRun)(String ,String ),
		int (*compileBuildLoad)(String ,String , String),
		int (*compileBuildLoadSliders)(String ,String ,String , TableOfReals ),
		void (*loadLibrary)(String),
		void  (*addFunction)(void (*f)(), String, String, String, String, String, int, int, int),
		void (*callback)(void (*f)(void)),
		void (*unload)(void (*f)(void))
);

/*! 
 \brief initialize dialogs and c interface
 \ingroup init
*/
TCAPIEXPORT void tc_PythonTool_api(
		void (*runPythonCode)(String),
		void (*runPythonFile)(String),
		void (*addPythonPlugin)(String,String,String,String,String)
);

/*! 
 \brief initialize dialogs and c interface
 \ingroup init
*/
TCAPIEXPORT void tc_OctaveTool_api(
		void (*runOctaveCode)(String),
		void (*runPythonFile)(String),
		void (*addOctavePlugin)(String,String,String,String,String)
);


#endif
