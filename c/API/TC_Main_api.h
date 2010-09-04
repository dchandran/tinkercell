#ifndef TINKERCELL_TC_MAIN_API_H
#define TINKERCELL_TC_MAIN_API_H

#include "TCstructs.h"
BEGIN_C_DECLS

/*! 
 \brief get all visible items
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_allItems();

/*! 
 \brief get all selected items
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_selectedItems();

/*!
 \brief get all items of the given family items
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_itemsOfFamily(const char* family);

/*! 
 \brief get subset of items that belong to the given family
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_itemsOfFamilyFrom(const char* family, tc_items itemsToSelectFrom);

/*! 
 \brief get the first item with the given name (full name)
 \ingroup Get items
*/
TCAPIEXPORT long tc_find(const char* fullname);

/*! 
 \brief get all items with the given names (full names)
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_findItems(tc_strings names);

/*! 
 \brief select an item
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
 \ingroup Get items
*/
TCAPIEXPORT const char* tc_getName(long item);

/*! 
 \brief get the full name of an item
 \ingroup Get items
*/
TCAPIEXPORT const char* tc_getUniqueName(long item);

/*! 
 \brief set the name of an item (not full name)
 \ingroup Get items
*/
TCAPIEXPORT void tc_rename(long item,const char* name);

/*! 
 \brief get the names of several items
 \ingroup Get items
*/
TCAPIEXPORT tc_strings tc_getNames(tc_items items);

/*! 
 \brief get the full names of several items
 \ingroup Get items
*/
TCAPIEXPORT tc_strings tc_getUniqueNames(tc_items items);

/*! 
 \brief get the family name of an item
 \ingroup Annotation
*/
TCAPIEXPORT const char* tc_getFamily(long item);

/*! 
 \brief check is an item belongs in a family (or in a sub-family)
 \ingroup Annotation
*/
TCAPIEXPORT int tc_isA(long item,const char* family);

/*! 
 \brief show text in the output window.
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_print(const char* text);

/*! 
 \brief show error text in the output window.
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_errorReport(const char* text);

/*! 
 \brief show table in the output window.
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_printTable(tc_matrix data);

/*! 
 \brief show file contents in the output window. 
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_printFile(const char* filename);

/*! 
 \brief cleat the contents in the output window. 
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_clear();

/*! 
 \brief delete an item
 \ingroup Insert and remove
*/
TCAPIEXPORT void tc_remove(long item);

/*! 
 \brief get the x location of an item
 \ingroup Appearance
*/
TCAPIEXPORT double tc_getY(long item);

/*! 
 \brief get the y location of an item
 \ingroup Appearance
*/
TCAPIEXPORT double tc_getX(long item);

/*! 
 \brief get the y location of a list item. Output is a N x 2 matrix
 \ingroup Appearance
*/
TCAPIEXPORT tc_matrix tc_getPos(tc_items items);

/*! 
 \brief set the x and y location of an item
 \ingroup Appearance
*/
TCAPIEXPORT void tc_setPos(long item,double x,double y);

/*! 
 \brief set the x and y location of a list of N items. Input a matrix of positions, with N rows and 2 columns (x,y)
 \ingroup Appearance
*/
TCAPIEXPORT void tc_setPosMulti(tc_items items, tc_matrix positions);

/*! 
 \brief move all the selected items by a given amount
 \ingroup Appearance
*/
TCAPIEXPORT void tc_moveSelected(double dx,double dy);

/*! 
 \brief is this running in MS windows?
 \ingroup System information
*/
TCAPIEXPORT int tc_isWindows();

/*! 
 \brief is this running in a Mac?
 \ingroup System information
*/
TCAPIEXPORT int tc_isMac();

/*! 
 \brief is this running in Linux?
 \ingroup System information
*/
TCAPIEXPORT int tc_isLinux();

/*! 
 \brief TinkerCell application folder
 \ingroup System information
*/
TCAPIEXPORT const char* tc_appDir();

/*! 
 \brief create an input window that can call a dynamic library
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_createInputWindowFromFile(tc_matrix input, const char* filename,const char* functionname, const char* title);

/*!
 \brief create an input window that can call a dynamic library
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_createInputWindow(tc_matrix input, const char* title, void (*f)(tc_matrix));

/*! 
 \brief add options to an existing input window at the i,j-th cell. Options will appear in a list
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_addInputWindowOptions(const char* title, int i, int j, tc_strings options);

/*! 
 \brief add a yes or no type of option to an existing input window at the i,j-th cell
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_addInputWindowCheckbox(const char* title, int i, int j);

/*! 
 \brief open a new graphics window
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_openNewWindow(const char* title);
/*! 
 \brief get child items of the given item
 \ingroup Get items
*/
TCAPIEXPORT tc_items tc_getChildren(long o);

/*! 
 \brief get parent item of the given item
 \ingroup Get items
*/
TCAPIEXPORT long tc_getParent(long o);

/*! 
 \brief get the entire data matrix for the given numerical data table of the given item
 \ingroup Network data
*/
TCAPIEXPORT tc_matrix tc_getNumericalData(long item,const char* data);

/*! 
 \brief set a new data matrix for an item. Use 0 for the global model item.
 \ingroup Network data
*/
TCAPIEXPORT void tc_setNumericalData(long o,const char* title,tc_matrix data);

/*! 
 \brief get the entire data matrix for the given strings data table of the given item
 \ingroup Network data
*/
TCAPIEXPORT tc_table tc_getTextData(long item,const char* data);

/*! 
 \brief set the entire data matrix for the given strings data table of the given item
 \ingroup Network data
*/
TCAPIEXPORT void tc_setTextData(long o,const char* title,tc_table data);

/*! 
 \brief get all the numeric data table names for the given item. Use 0 for the global tables.
 \ingroup Network data
*/
TCAPIEXPORT tc_strings tc_getNumericalDataNames(long o);

/*! 
 \brief get all the text data table names for the given item. Use 0 for the global tables.
 \ingroup Network data
*/
TCAPIEXPORT tc_strings tc_getTextDataNames(long o);

/*! 
 \brief zoom by the given factor (0 - 1)
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_zoom(double factor);

/*! 
 \brief get a text from the user (dialog)
 \ingroup Input and Output
*/
TCAPIEXPORT const char* tc_tc_getTableValue(const char* title);
/*! 
 \brief get a file from the user (dialog)
 \ingroup Input and Output
*/
TCAPIEXPORT const char* tc_getFilename();

/*! 
 \brief get a text from the user (dialog) from a list of selections
 \ingroup Input and Output
*/
TCAPIEXPORT int tc_tc_getTableValueFromList(const char* title, tc_strings list,const char* selectedString);
/*! 
 \brief get a number from the user (dialog)
 \ingroup Input and Output
*/
TCAPIEXPORT double tc_getNumber(const char* title);

/*! 
 \brief get a list of numbers from the user (dialog) into the argument array
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_getNumbers(tc_strings labels, double* result);

/*! 
 \brief display a dialog with a text and a yes and no button
 \param const char* displayed message or question
 \ingroup Input and Output
*/
TCAPIEXPORT int tc_askQuestion(const char* message);

/*! 
 \brief display a dialog with a text message and a close button
 \param const char* displayed message
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_messageDialog(const char* message);

/*!
 \brief get pointer to the current thread
 \ingroup Programming
*/
TCAPIEXPORT long tc_thisThread();

/*!
 \brief create a window with several sliders. when the sliders change, the given function will be called with the values in the sliders
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_createSliders(tc_matrix input, void (*f)(tc_matrix));

/*! 
 \brief get the color of the item
 \ingroup Appearance
*/
TCAPIEXPORT const char* tc_getColor(long item);

/*! 
 \brief set the rgb color  of the item and indicate whether or not the color is permanenet
 \ingroup Appearance
*/
TCAPIEXPORT void tc_setColor(long item,const char* name, int permanent);

/*! 
 \brief change the graphics file for drawing one of the nodes
 \ingroup Appearance
*/
TCAPIEXPORT void tc_changeNodeImage(long item,const char* filename);

/*! 
 \brief change the graphics file for drawing the arrowheads for the given connection
 \ingroup Appearance
*/
TCAPIEXPORT void tc_changeArrowHead(long connection,const char* filename);

/*!
 \brief Change the size of an item
 \ingroup Appearance
*/
TCAPIEXPORT void tc_setSize(long item,double width,double height,int permanent);

/*!
 \brief get the width of an item
 \ingroup Appearance
*/
TCAPIEXPORT double tc_getWidth(long item);

/*!
 \brief get the width of an item
 \ingroup Appearance
*/
TCAPIEXPORT double tc_getHeight(long item);

/*!
 \brief get the width of an item
 \ingroup Appearance
*/
TCAPIEXPORT void tc_setAngle(long item, double t,int permanent);

/*!
 \brief get the angle of an item
 \ingroup Appearance
*/
TCAPIEXPORT double tc_getAngle(long item);

/*! 
 \brief initialize main
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
		
		void (*tc_createInputWindow0)(tc_matrix,const char*,const char*, const char*),
        void (*tc_createInputWindow1)(tc_matrix, const char*, void (*f)(tc_matrix)),
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
		
		const char* (*tc_getTableValue)(const char*),
		int (*getSelectedString)(const char*, tc_strings, const char*),
		double (*getNumber)(const char*),
		void (*getNumbers)( tc_strings, double * ),
		const char* (*getFilename)(),
		
		int (*askQuestion)(const char*),
		void (*messageDialog)(const char*),
		
		void (*setSize0)(long,double,double,int),
		double (*getWidth0)(long),
		double (*getHeight0)(long),
		void (*setAngle0)(long,double,int),
		double (*getAngle0)(long),
		const char* (*getColor)(long),
		void (*setColor0)(long,const char*,int),
		
		void (*changeGraphics0)(long,const char*),
		void (*changeArrowHead0)(long,const char*)
	);

/*! 
 \brief show progress of current operation
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_showProgress(int progress);

/*! 
 \brief initialize main
 \ingroup init
*/
TCAPIEXPORT void tc_CThread_api_initialize( 
	long cthread,
	void (*showProgress)(long, int)	);

END_C_DECLS
#endif

