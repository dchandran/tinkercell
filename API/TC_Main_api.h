#ifndef TINKERCELL_TC_MAIN_API_H
#define TINKERCELL_TC_MAIN_API_H

#include "TC_structs.h"
BEGIN_C_DECLS

/*! 
 \brief get all visible items
 \return tc_items list of all items in the network
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_allItems();

/*! 
 \brief get all selected items
 \return tc_items list of all items currently selected by user
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_selectedItems();

/*!
 \brief get all items of the given family items
 \param string name of a type
 \return tc_items list of all items in network belonging under the given type
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_itemsOfFamily(const char* family);

/*! 
 \brief get subset of items that belong to the given family
 \param string name of a type
 \param tc_items list of items to select from
 \return tc_items list of all items in the list belonging under the given type
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_itemsOfFamilyFrom(const char* family, tc_items itemsToSelectFrom);

/*! 
 \brief get the first item with the given name (full name)
 \param string name of an item. use full name whenever possible
 \return int address of item with the name
 \ingroup Get items
*/
TCAPIEXPORT long tc_find(const char* name);

/*! 
 \brief get all items with the given names (full names)
 \param tc_string names of one or more items
 \return tc_items addresses of all the items. For nonexistent names, a 0 will be placed in the list
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_findItems(tc_strings names);

/*! 
 \brief select an item
 \param int address of the item
 \ingroup Get items
*/
TCAPIEXPORT void tc_select(long item);

/*! 
 \brief deselect all items
 \ingroup Get items
*/
TCAPIEXPORT void tc_deselect();

/*! 
 \brief get the name of an item
 \param int address of the item
 \return string name (not full name)
 \ingroup Get items
*/
TCAPIEXPORT const char* tc_getName(long item);

/*! 
 \brief get the full name of an item
 \param int address of the item
 \return string full name of the item (always unique)
 \ingroup Get items
*/
TCAPIEXPORT const char* tc_getUniqueName(long item);

/*! 
 \brief set the name of an item (not full name)
  \param int address of item
 \return string new name (not full name)
 \ingroup Get items
*/
TCAPIEXPORT void tc_rename(long item,const char* name);

/*! 
 \brief get the names of several items
 \param tc_items addresses of the items
 \return tc_string list of names (not full names)
 \ingroup Get items
*/
TCAPIEXPORT tc_strings tc_getNames(tc_items items);

/*! 
 \brief get the full names of several items
 \param tc_items addresses of the items
 \return tc_string list of names (unique names)
 \ingroup Get items
*/
TCAPIEXPORT tc_strings tc_getUniqueNames(tc_items items);

/*! 
 \brief get the family name of an item
 \param int address of the item
 \return string type of the item
 \ingroup Annotation
*/
TCAPIEXPORT const char* tc_getFamily(long item);

/*! 
 \brief check is an item belongs in a family (or in a sub-family)
 \param int address of the item
 \param string name of the family type
 \return int 0(no) or 1(yes)
 \ingroup Annotation
*/
TCAPIEXPORT int tc_isA(long item,const char* family);

/*! 
 \brief show text in the output window
 \param string text message
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_print(const char* text);

/*! 
 \brief show error text in the output window
 \param string error message
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_errorReport(const char* text);

/*! 
 \brief show table in the output window
 \param tc_matrix table
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_printMatrix(tc_matrix data);

/*! 
 \brief show file contents in the output window 
 \param string file name
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_printFile(const char* filename);

/*! 
 \brief cleat the contents in the output window
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_clear();

/*! 
 \brief delete an item
 \param int address of item
 \ingroup Insert and remove
*/
TCAPIEXPORT void tc_remove(long item);

/*! 
 \brief get the x location of an item
 \param int address of item
 \return double x position
 \ingroup Get and set position
*/
TCAPIEXPORT double tc_getY(long item);

/*! 
 \brief get the y location of an item
 \param int address of item
 \return double y position
 \ingroup Get and set position
*/
TCAPIEXPORT double tc_getX(long item);

/*! 
 \brief get the y location of a list item. Output is a N x 2 matrix
 \param tc_items addresses of items
 \return tc_matrix x,y positions of items
 \ingroup Get and set position
*/
TCAPIEXPORT tc_matrix tc_getPos(tc_items items);

/*! 
 \brief set the x and y location of an item
 \param int address of item
 \param double x position
 \param double y position
 \ingroup Get and set position
*/
TCAPIEXPORT void tc_setPos(long item,double x,double y);

/*! 
 \brief set the x and y location of a list of N items. Input a matrix of positions, with N rows and 2 columns (x,y)
 \param tc_items addresses of items
 \param tc_matrix x,y positions
 \ingroup Get and set position
*/
TCAPIEXPORT void tc_setPosMulti(tc_items items, tc_matrix positions);

/*! 
 \brief move all the selected items by a given amount
 \param double change in x
 \param double change in y
 \ingroup Get and set position
*/
TCAPIEXPORT void tc_moveSelected(double dx,double dy);

/*! 
 \brief is this running in MS windows?
 \return 0 (not windows OS ) or 1 (is windows OS)
 \ingroup System information
*/
TCAPIEXPORT int tc_isWindows();

/*! 
 \brief is this running in a Mac?
 \return 0 (not Mac OS ) or 1 (is Mac OS)
 \ingroup System information
*/
TCAPIEXPORT int tc_isMac();

/*! 
 \brief is this running in a Unix system (excluding Mac)?
 \return 0 (not Linux) or 1 (is Linux)
 \ingroup System information
*/
TCAPIEXPORT int tc_isLinux();

/*! 
 \brief TinkerCell application folder
 \return string application folder path
 \ingroup System information
*/
TCAPIEXPORT const char* tc_appDir();

/*! 
 \brief TinkerCell home folder
 \return string home folder path
 \ingroup System information
*/
TCAPIEXPORT const char* tc_homeDir();

/*! 
 \brief create an input window that will call a function in the console window with the arguments from the input matrix
 \param tc_matrix input window's arguments a default values
 \param string name of the program
 \param string name of function
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_createInputWindowForScript(tc_matrix input, const char* title, const char* functionname);

/*!
 \brief create an input window that will call a function
 \param tc_matrix input window's arguments a default values
 \param string name of this program
 \param void* pointer to a 1-argument function that takes tc_matrix argument
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_createInputWindow(tc_matrix input, const char* title, void (*f)(tc_matrix));

/*! 
 \brief add options to an existing input window at the i,j-th cell. Options will appear in a list
 \param string name of an input window that was just created
 \param int row number
 \param int column number
 \param tc_string place these options (drop-down meny) at the (row,column) location of the table
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_addInputWindowOptions(const char* title, int i, int j, tc_strings options);

/*! 
 \brief add a yes or no type of option to an existing input window at the i,j-th cell
 \param int row number
 \param int column number
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_addInputWindowCheckbox(const char* title, int i, int j);

/*! 
 \brief open a new graphics window
 \param string title of the new window
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_openNewWindow(const char* title);
/*! 
 \brief get child items of the given item
 \param int address of item
 \return tc_items list of child items
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_getChildren(long o);

/*! 
 \brief get parent item of the given item
 \param int address of item
 \return int address of parent item (0 if no parent)
 \ingroup Get items
*/
TCAPIEXPORT long tc_getParent(long o);

/*! 
 \brief get the entire data matrix for the given numerical data table of the given item
 \param int address of item. use 0 for the model item
 \param string name of numerical data table
 \return tc_matrix the numerical data table for the given item
 \ingroup Data
*/
TCAPIEXPORT tc_matrix tc_getNumericalData(long item,const char* data);

/*! 
 \brief set a new data matrix for an item or replace an existing one
 \param int address of item. use 0 for the model item
 \param string name of numerical data table
 \param tc_matrix the new numerical data table for the given item
 \ingroup Data
*/
TCAPIEXPORT void tc_setNumericalData(long o,const char* title,tc_matrix data);

/*! 
 \brief get the entire data table for the given strings data table of the given item
 \param int address of item. use 0 for the model item
 \param string name of text data table
 \return tc_table the text data table for the given item
 \ingroup Data
*/
TCAPIEXPORT tc_table tc_getTextData(long item,const char* data);

/*! 
 \brief set or replace the entire data matrix for the given strings data table of the given item
 \param int address of item. use 0 for the model item
 \param string name of text data table
 \return tc_table the new text data table for the given item
 \ingroup Data
*/
TCAPIEXPORT void tc_setTextData(long o,const char* title,tc_table data);

/*! 
 \brief get all the numeric data table names for the given item
 \param int address of item. use 0 for the model item
 \return tc_string list of names of all numerical tables inside this item
 \ingroup Data
*/
TCAPIEXPORT tc_strings tc_getNumericalDataNames(long o);

/*! 
 \brief get all the text data table names for the given item
 \param int address of item. use 0 for the model item
 \return tc_string list of names of all text tables inside this item
 \ingroup Data
*/
TCAPIEXPORT tc_strings tc_getTextDataNames(long o);

/*! 
 \brief zoom by the given factor (0 - 1)
 \param double zoom factor between 0 and 1
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_zoom(double factor);

/*! 
 \brief get a text from the user (dialog)
 \ingroup Input and Output
*/
TCAPIEXPORT const char* tc_getStringDialog(const char* title);
/*! 
 \brief popup dialog asking user to select a file
 \return string the filename selected by the user
 \ingroup Input and Output
*/
TCAPIEXPORT const char* tc_getFilename();

/*! 
 \brief popup dialog asking user to select one item from a list
 \param string title of dialog
 \param tc_string list of options
 \param string the option that is selected by default
 \return int index of the user's selection, -1 if canceled
 \ingroup Input and Output
*/
TCAPIEXPORT int tc_getStringFromList(const char* title, tc_strings list,const char* selectedString);
/*! 
 \brief popup dialog asking user for a number
 \param string text presented to the user
 \return double user's response
 \ingroup Input and Output
*/
TCAPIEXPORT double tc_getNumber(const char* title);

/*! 
 \brief popup dialog asking user for several numbers (with labels)
 \param tc_strings labels for each number to get
 \param double* array that will store the results
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_getNumbers(tc_strings labels, double* result);

/*! 
 \brief display a dialog with a text and a yes and no button
 \param string displayed message or question
 \ingroup Input and Output
*/
TCAPIEXPORT int tc_askQuestion(const char* message);

/*! 
 \brief display a dialog with a text message and a close button
 \param string displayed message
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_messageDialog(const char* message);

/*! 
 \brief open a file
 \param string file name
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_openFile(const char* file);

/*! 
 \brief save current network
 \param string filename
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_saveToFile(const char* file);

/*!
 \brief get pointer to the current thread. used for passing this thread as some argument
 \return int pointer
 \ingroup Programming
*/
TCAPIEXPORT long tc_thisThread();

/*!
 \brief create a window with several sliders. when the sliders change, the given function will be called with the values in the sliders
 \param tc_matrix names of variables and initial values for the sliders
 \param void* callback function with tc_matrix as the argument
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_createSliders(tc_matrix input, void (*f)(tc_matrix));

/*! 
 \brief get the color of the item
 \param int address of item, e.g. obtained using tc_find
 \return string Hex code for color
 \ingroup Appearance
*/
TCAPIEXPORT const char* tc_getColor(long item);

/*! 
 \brief set the rgb color  of the item and indicate whether or not the color is permanenet
 \param int address of item, e.g. obtained using tc_find
 \param string Hex code for color
 \param int 0(temporary) or 1 (permenent color change)
 \ingroup Appearance
*/
TCAPIEXPORT void tc_setColor(long item,const char* name, int permanent);

/*! 
 \brief change the graphics file for drawing one of the nodes
 \param int address of item, e.g. obtained using tc_find
 \param string file name of the new graphics file
 \ingroup Appearance
*/
TCAPIEXPORT void tc_changeNodeImage(long item,const char* filename);

/*! 
 \brief change the graphics file for drawing the arrowheads for the given connection
 \param int address of connection, e.g. obtained using tc_find
 \param string file name of the new graphics file
 \ingroup Appearance
*/
TCAPIEXPORT void tc_changeArrowHead(long connection,const char* filename);

/*!
 \brief Change the size of an item
 \param int address of item, e.g. obtained using tc_find
 \param double width
 \param double height
 \ingroup Appearance
*/
TCAPIEXPORT void tc_setSize(long item,double width,double height);

/*!
 \brief get the width of an item
 \param int address of item, e.g. obtained using tc_find
 \return double width
 \ingroup Appearance
*/
TCAPIEXPORT double tc_getWidth(long item);

/*!
 \brief get the width of an item
 \param int address of item, e.g. obtained using tc_find
 \return double height
 \ingroup Appearance
*/
TCAPIEXPORT double tc_getHeight(long item);

/*!
 \brief rotate and item by the given number of degrees
 \param int address of item, e.g. obtained using tc_find
 \param double angle in degrees
 \ingroup Appearance
*/
TCAPIEXPORT void tc_rotate(long item, double t);

/*!
 \brief save screenshot in a file
 \param string filename (PNG)
 \param int width of image
 \param int height of image
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_screenshot(const char * filename, int width, int height);

/*!
 \brief get width of current canvas
 \return int width
 \ingroup Input and Output
*/
TCAPIEXPORT int tc_screenWidth();

/*!
 \brief get height of current canvas
 \return int height
 \ingroup Input and Output
*/
TCAPIEXPORT int tc_screenHeight();

/*!
 \brief get x position of current canvas
 \return int x
 \ingroup Input and Output
*/
TCAPIEXPORT int tc_screenX();

/*!
 \brief get y position of current canvas
 \return int y
 \ingroup Input and Output
*/
TCAPIEXPORT int tc_screenY();
/*! 
 \brief get text displayed on the canvas
 \return const char *
 \ingroup Annotation
*/
TCAPIEXPORT const char * tc_annotations();
/*! 
 \brief show text displayed on the canvas at the given position
 \param double x
 \param double y
 \param const char *
 \ingroup Annotation
*/
TCAPIEXPORT void tc_insertAnnotations(const char *, double, double);
/*! 
 \brief initialize core C api
 \ingroup init
*/
TCAPIEXPORT void tc_Main_api_initialize(
	    tc_items (*tc_allItems0)(),
		tc_items (*tc_selectedItems0)(),
		tc_items (*tc_itemsOfFamily0)(const char*),
		tc_items (*tc_itemsOfFamily1)(const char*, tc_items),
		long (*tc_find0)(const char*),
		tc_items (*tc_findItems0)(tc_strings),
		void (*tc_select0)(long),
		void (*tc_deselect0)(),
		const char* (*tc_getName0)(long),
		const char* (*tc_getUniqueName0)(long),

		void (*tc_setName0)(long item,const char* name),
		tc_strings (*tc_getNames0)(tc_items),
		tc_strings (*tc_getUniqueNames0)(tc_items),
		const char* (*tc_getFamily0)(long),
		int (*tc_isA0)(long,const char*),

		void (*tc_clearText)(),
		void (*tc_outputText0)(const char*),
		void (*tc_errorReport0)(const char*),
		void (*tc_outputTable0)(tc_matrix),
		void (*tc_printFile0)(const char*),

		void (*tc_removeItem0)(long),

		double (*tc_getY0)(long),
		double (*tc_getX0)(long),
		tc_matrix (*tc_getPos0)(tc_items),
		void (*tc_setPos0)(long,double,double),
		void (*tc_setPos1)(tc_items,tc_matrix),
		void (*tc_moveSelected0)(double,double),

		int (*tc_isWindows0)(),
		int (*tc_isMac0)(),
		int (*tc_isLinux0)(),
		const char* (*tc_appDir0)(),
		const char* (*tc_homeDir0)(),

		void (*tc_createInputWindow0)(tc_matrix , const char* , const char* ),
        void (*tc_createInputWindow1)(long, tc_matrix, const char*, void (*f)(tc_matrix)),
		void (*createSliders)(long, tc_matrix, void (*f)(tc_matrix)),
		
		void (*tc_addInputWindowOptions0)(const char*, int i, int j, tc_strings),
		void (*tc_addInputWindowCheckbox0)(const char*, int i, int j),
		void (*tc_openNewWindow0)(const char* title),
		
		tc_items (*tc_getChildren0)(long),
		long (*tc_getParent0)(long),
		
		tc_matrix (*tc_getNumericalData0)(long,const char*),
		void (*tc_setNumericalData0)(long,const char*,tc_matrix),
		tc_table (*tc_getTextData0)(long,const char*),
		void (*tc_setTextData0)(long,const char*, tc_table),
				
		tc_strings (*tc_getNumericalDataNames0)(long),
		tc_strings (*tc_getTextDataNames0)(long),
		
		void (*tc_zoom0)(double factor),
		
		const char* (*tc_getStringDialog0)(const char*),
		int (*getSelectedString)(const char*, tc_strings, const char*),
		double (*getNumber)(const char*),
		void (*getNumbers)( tc_strings, double * ),
		const char* (*getFilename)(),
		
		int (*askQuestion)(const char*),
		void (*messageDialog)(const char*),
		void (*openFile)(const char*),
		void (*saveToFile)(const char*),
		
		void (*setSize0)(long,double,double,int),
		double (*getWidth0)(long),
		double (*getHeight0)(long),
		void (*setAngle0)(long,double,int),
		const char* (*getColor)(long),
		void (*setColor0)(long,const char*,int),
		
		void (*changeGraphics0)(long,const char*),
		void (*changeArrowHead0)(long,const char*),
		
		void (*screenshot)(const char*, int, int),
		int (*screenHeight)(),
		int (*screenWidth)(),
		int (*screenX)(),
		int (*screenY)(),

		const char * (*tc_annotations)(),
		void (*insertAnnotations)(const char *, double, double)
	);

/*! 
 \brief show progress of current operation
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_showProgress(int progress);
/*! 
 \brief this function will be called whenever the model is changed
 \param void* callback function pointer
 \ingroup Programming
*/
TCAPIEXPORT void tc_callback(void (*f)(void));
/*! 
 \brief this function will be called whenever Tinkercell exits. Use it to free memory.
 \param void* callback function pointer
 \ingroup Programming
*/
TCAPIEXPORT void tc_callWhenExiting(void (*f)(void));
/*! 
 \brief initialize main
 \ingroup init
*/
TCAPIEXPORT void tc_CThread_api_initialize( 
	long cthread,
	void (*callback)(long, void (*f)(void)),
	void (*callWhenExiting)(long, void (*f)(void)),
	void (*showProgress)(long, int)	);

END_C_DECLS
#endif

