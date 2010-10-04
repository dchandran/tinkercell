#ifndef TINKERCELL_TC_DYNAMICLIBRARYTOOL_API_H
#define TINKERCELL_TC_DYNAMICLIBRARYTOOL_API_H

#include "TC_structs.h"
BEGIN_C_DECLS

/*! 
 \brief compile and run a c file
 \param string command
 \param string arguments
 \ingroup Programming
*/
TCAPIEXPORT int tc_compileAndRun(const char* command,const char* args);
/*! 
 \brief compile a c file, generate the library, and load it
 \param string C code file name
 \param string main function inside C code 
 \param string title of the program
 \ingroup Programming
*/
TCAPIEXPORT int tc_compileBuildLoad(const char* filename,const char* function,const char* title);
/*! 
 \brief compile a c file, generate the library, and load it as callback function for sliders
 \param string C code file name
 \param string callback function inside C code that will get called when slider values change
 \param string title of the program
 \param tc_matrix input of values for the sliders
 \ingroup Programming
*/
TCAPIEXPORT int tc_compileBuildLoadSliders(const char* filename,const char* function,const char* title, tc_matrix inputs);
/*! 
 \brief run the Python code given by the string
 \param string python code
 \ingroup Programming
*/
TCAPIEXPORT void tc_runPythonCode(const char* code);
/*! 
 \brief run the Python code in the given file
 \param string python script file
 \ingroup Programming
*/
TCAPIEXPORT void tc_runPythonFile(const char* filename);
/*! 
 \brief add a python script to the functions menu
 \param string python script file
 \param string name of program
 \param string description of program
 \param string category where the program belongs (in the functione menu)
 \ingroup Programming
*/
TCAPIEXPORT void tc_addPythonPlugin(const char* file,const char* name,const char* description,const char* category, const char* icon);
/*! 
 \brief call a function listed in the functions menu, e.g. "Deterministic simulation"
 \param string name of function
 \ingroup Programming
*/
TCAPIEXPORT void tc_callFunction(const char* functionTitle);
/*! 
 \brief run a dynamic C library that contains the function "tc_main"
 \param string name of C library
 \ingroup Programming
*/
TCAPIEXPORT void tc_loadLibrary(const char* filename);
/*! 
 \brief add a function to the menu of functions
 \param void* pointer to function
 \param string name of program
 \param string description of program
 \param string category of program (in the functions menu)
 \param string icon file (png file) -- use empty string for default
 \param string type of items in model that this function is specific for. use empty for no specifications
 \param int 0 or 1 (show in tool's menu)
 \param int 0 or 1 (make the default function when tinkercell loads)
 \ingroup Programming
*/
TCAPIEXPORT void tc_addFunction(void (*f)(), const char* title, const char* description, const char* category, const char* iconFile, const char* target_family, int show_menu, int in_tool_menu, int make_default);

/*! 
 \brief run the Octave code given by the string
 \param string octave code
 \ingroup Programming interface
*/
void tc_runOctaveCode(const char* code);

/*! 
 \brief run the Octave code in the given file
 \param string octave file
 \ingroup Programming interface
*/
void  tc_runOctaveFile(const char* filename);

/*! 
 \brief add a Octave script to the functions menu
 \param string python script file
 \param string name of program
 \param string description of program
 \param string category where the program belongs (in the functione menu)
 \ingroup Programming interface
*/
void  tc_addOctavePlugin(const char* file,const char* name,const char* description,const char* category, const char* icon);

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
		int (*compileBuildLoadSliders)(const char* ,const char* ,const char* , tc_matrix ),
		void (*loadLibrary)(const char*),
		void  (*addFunction)(void (*f)(), const char*, const char*, const char*, const char*, const char*, int, int, int)
);

/*! 
 \brief initialize python plug-in
 \ingroup init
*/
TCAPIEXPORT void tc_PythonTool_api(
		void (*runPythonCode)(const char*),
		void (*runPythonFile)(const char*),
		void (*addPythonPlugin)(const char*,const char*,const char*,const char*,const char*)
);

/*! 
 \brief initialize octave plug-in
 \ingroup init
*/
TCAPIEXPORT void tc_OctaveTool_api(
		void (*runOctaveCode)(const char*),
		void (*runOctaveFile)(const char*),
		void (*addOctavePlugin)(const char*,const char*,const char*,const char*,const char*)
);

END_C_DECLS
#endif

