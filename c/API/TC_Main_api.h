#ifndef TINKERCELL_TC_MAIN_API_H
#define TINKERCELL_TC_MAIN_API_H

#include "../TCstructs.h"
BEGIN_C_DECLS

/*! 
 \brief get all visible items
 \ingroup Get items
*/
TCAPIEXPORT ArrayOfItems tc_allItems();

/*! 
 \brief get all selected items
 \ingroup Get items
*/
TCAPIEXPORT ArrayOfItems tc_selectedItems();

/*!
 \brief get all items of the given family items
 \ingroup Get items
*/
TCAPIEXPORT ArrayOfItems tc_itemsOfFamily(const char* family);

/*! 
 \brief get subset of items that belong to the given family
 \ingroup Get items
*/
TCAPIEXPORT ArrayOfItems tc_itemsOfFamilyFrom(const char* family, ArrayOfItems itemsToSelectFrom);

/*! 
 \brief get the first item with the given name (full name)
 \ingroup Get items
*/
TCAPIEXPORT int tc_find(const char* fullname);

/*! 
 \brief get all items with the given names (full names)
 \ingroup Get items
*/
TCAPIEXPORT ArrayOfItems tc_findItems(ArrayOfStrings names);

/*! 
 \brief select an item
 \ingroup Get items
*/
TCAPIEXPORT void tc_select(int item);

/*! 
 \brief deselect all items
 \ingroup Get items
*/
TCAPIEXPORT void tc_deselect();

/*! 
 \brief get the name of an item
 \ingroup Get items
*/
TCAPIEXPORT const char* tc_getName(int item);

/*! 
 \brief get the full name of an item
 \ingroup Get items
*/
TCAPIEXPORT const char* tc_getUniqueName(int item);

/*! 
 \brief set the name of an item (not full name)
 \ingroup Get items
*/
TCAPIEXPORT void tc_rename(int item,const char* name);

/*! 
 \brief get the names of several items
 \ingroup Get items
*/
TCAPIEXPORT ArrayOfStrings tc_getNames(ArrayOfItems items);

/*! 
 \brief get the full names of several items
 \ingroup Get items
*/
TCAPIEXPORT ArrayOfStrings tc_getUniqueNames(ArrayOfItems items);

/*! 
 \brief get the family name of an item
 \ingroup Annotation
*/
TCAPIEXPORT const char* tc_getFamily(int item);

/*! 
 \brief check is an item belongs in a family (or in a sub-family)
 \ingroup Annotation
*/
TCAPIEXPORT int tc_isA(int item,const char* family);

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
TCAPIEXPORT void tc_printTable(TableOfReals data);

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
TCAPIEXPORT void tc_remove(int item);

/*! 
 \brief get the x location of an item
 \ingroup Appearance
*/
TCAPIEXPORT double tc_getY(int item);

/*! 
 \brief get the y location of an item
 \ingroup Appearance
*/
TCAPIEXPORT double tc_getX(int item);

/*! 
 \brief get the y location of a list item. Output is a N x 2 matrix
 \ingroup Appearance
*/
TCAPIEXPORT TableOfReals tc_getPos(ArrayOfItems items);

/*! 
 \brief set the x and y location of an item
 \ingroup Appearance
*/
TCAPIEXPORT void tc_setPos(int item,double x,double y);

/*! 
 \brief set the x and y location of a list of N items. Input a matrix of positions, with N rows and 2 columns (x,y)
 \ingroup Appearance
*/
TCAPIEXPORT void tc_setPosMulti(ArrayOfItems items, TableOfReals positions);

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
TCAPIEXPORT void tc_createInputWindowFromFile(TableOfReals input, const char* filename,const char* functionname, const char* title);

/*!
 \brief create an input window that can call a dynamic library
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_createInputWindow(TableOfReals input, const char* title, void (*f)(TableOfReals));

/*! 
 \brief add options to an existing input window at the i,j-th cell. Options will appear in a list
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_addInputWindowOptions(const char* title, int i, int j, ArrayOfStrings options);

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
TCAPIEXPORT ArrayOfItems tc_getChildren(int o);

/*! 
 \brief get parent item of the given item
 \ingroup Get items
*/
TCAPIEXPORT int tc_getParent(int o);

/*! 
 \brief get the entire data matrix for the given numerical data table of the given item
 \ingroup Network data
*/
TCAPIEXPORT TableOfReals tc_getNumericalData(int item,const char* data);

/*! 
 \brief set a new data matrix for an item. Use 0 for the global model item.
 \ingroup Network data
*/
TCAPIEXPORT void tc_setNumericalData(int o,const char* title,TableOfReals data);

/*! 
 \brief get the entire data matrix for the given strings data table of the given item
 \ingroup Network data
*/
TCAPIEXPORT TableOfStrings tc_getTextData(int item,const char* data);

/*! 
 \brief set the entire data matrix for the given strings data table of the given item
 \ingroup Network data
*/
TCAPIEXPORT void tc_setTextData(int o,const char* title,TableOfStrings data);

/*! 
 \brief get all the numeric data table names for the given item. Use 0 for the global tables.
 \ingroup Network data
*/
TCAPIEXPORT ArrayOfStrings tc_getNumericalDataNames(int o);

/*! 
 \brief get all the text data table names for the given item. Use 0 for the global tables.
 \ingroup Network data
*/
TCAPIEXPORT ArrayOfStrings tc_getTextDataNames(int o);

/*! 
 \brief zoom by the given factor (0 - 1)
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_zoom(double factor);

/*! 
 \brief get a text from the user (dialog)
 \ingroup Input and Output
*/
TCAPIEXPORT const char* tc_getString(const char* title);
/*! 
 \brief get a file from the user (dialog)
 \ingroup Input and Output
*/
TCAPIEXPORT const char* tc_getFilename();

/*! 
 \brief get a text from the user (dialog) from a list of selections
 \ingroup Input and Output
*/
TCAPIEXPORT int tc_getStringFromList(const char* title, ArrayOfStrings list,const char* selectedString);
/*! 
 \brief get a number from the user (dialog)
 \ingroup Input and Output
*/
TCAPIEXPORT double tc_getNumber(const char* title);

/*! 
 \brief get a list of numbers from the user (dialog) into the argument array
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_getNumbers(ArrayOfStrings labels, double* result);

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
TCAPIEXPORT int tc_thisThread();

/*!
 \brief create a window with several sliders. when the sliders change, the given function will be called with the values in the sliders
 \ingroup Input and Output
*/
TCAPIEXPORT void tc_createSliders(TableOfReals input, void (*f)(TableOfReals));

/*! 
 \brief get the color of the item
 \ingroup Appearance
*/
TCAPIEXPORT const char* tc_getColor(int item);

/*! 
 \brief set the rgb color  of the item and indicate whether or not the color is permanenet
 \ingroup Appearance
*/
TCAPIEXPORT void tc_setColor(int item,const char* name, int permanent);

/*! 
 \brief change the graphics file for drawing one of the nodes
 \ingroup Appearance
*/
TCAPIEXPORT void tc_changeNodeImage(int item,const char* filename);

/*! 
 \brief change the graphics file for drawing the arrowheads for the given connection
 \ingroup Appearance
*/
TCAPIEXPORT void tc_changeArrowHead(int connection,const char* filename);

/*!
 \brief Change the size of an item
 \ingroup Appearance
*/
TCAPIEXPORT void tc_setSize(int item,double width,double height,int permanent);

/*!
 \brief get the width of an item
 \ingroup Appearance
*/
TCAPIEXPORT double tc_getWidth(int item);

/*!
 \brief get the width of an item
 \ingroup Appearance
*/
TCAPIEXPORT double tc_getHeight(int item);

/*!
 \brief get the width of an item
 \ingroup Appearance
*/
TCAPIEXPORT void tc_setAngle(int item, double t,int permanent);

/*!
 \brief get the angle of an item
 \ingroup Appearance
*/
TCAPIEXPORT double tc_getAngle(int item);

/*! 
 \brief initialize main
 \ingroup init
*/
TCAPIEXPORT void tc_Main_api_initialize(
	    ArrayOfItems (*tc_allItems0)(),
		ArrayOfItems (*tc_selectedItems0)(),
		ArrayOfItems (*tc_itemsOfFamily0)(const char*),
		ArrayOfItems (*tc_itemsOfFamily1)(const char*, ArrayOfItems),
		int (*tc_find0)(const char*),
		ArrayOfItems (*tc_findItems0)(ArrayOfStrings),
		void (*tc_select0)(int),
		void (*tc_deselect0)(),
		const char* (*tc_getName0)(int),
		const char* (*tc_getUniqueName0)(int),
		void (*tc_setName0)(int item,const char* name),
		ArrayOfStrings (*tc_getNames0)(ArrayOfItems),
		ArrayOfStrings (*tc_getUniqueNames0)(ArrayOfItems),
		const char* (*tc_getFamily0)(int),
		int (*tc_isA0)(int,const char*),

		void (*tc_clearText)(),
		void (*tc_outputText0)(const char*),
		void (*tc_errorReport0)(const char*),
		void (*tc_outputTable0)(TableOfReals),
		void (*tc_printFile0)(const char*),

		void (*tc_removeItem0)(int),

		double (*tc_getY0)(int),
		double (*tc_getX0)(int),
		TableOfReals (*tc_getPos0)(ArrayOfItems),
		void (*tc_setPos0)(int,double,double),
		void (*tc_setPos1)(ArrayOfItems,TableOfReals),
		void (*tc_moveSelected0)(double,double),

		int (*tc_isWindows0)(),
		int (*tc_isMac0)(),
		int (*tc_isLinux0)(),
		const char* (*tc_appDir0)(),
		
		void (*tc_createInputWindow0)(TableOfReals,const char*,const char*, const char*),
        void (*tc_createInputWindow1)(TableOfReals, const char*, void (*f)(TableOfReals)),
		void (*createSliders)(int, TableOfReals, void (*f)(TableOfReals)),
		
		void (*tc_addInputWindowOptions0)(const char*, int i, int j, ArrayOfStrings),
		void (*tc_addInputWindowCheckbox0)(const char*, int i, int j),
		void (*tc_openNewWindow0)(const char* title),
		
		ArrayOfItems (*tc_getChildren0)(int),
		int (*tc_getParent0)(int),
		
		TableOfReals (*tc_getNumericalData0)(int,const char*),
		void (*tc_setNumericalData0)(int,const char*,TableOfReals),
		TableOfStrings (*tc_getTextData0)(int,const char*),
		void (*tc_setTextData0)(int,const char*, TableOfStrings),
				
		ArrayOfStrings (*tc_getNumericalDataNames0)(int),
		ArrayOfStrings (*tc_getTextDataNames0)(int),
		
		void (*tc_zoom0)(double factor),
		
		const char* (*getString)(const char*),
		int (*getSelectedString)(const char*, ArrayOfStrings, const char*),
		double (*getNumber)(const char*),
		void (*getNumbers)( ArrayOfStrings, double * ),
		const char* (*getFilename)(),
		
		int (*askQuestion)(const char*),
		void (*messageDialog)(const char*),
		
		void (*setSize0)(int,double,double,int),
		double (*getWidth0)(int),
		double (*getHeight0)(int),
		void (*setAngle0)(int,double,int),
		double (*getAngle0)(int),
		const char* (*getColor)(int),
		void (*setColor0)(int,const char*,int),
		
		void (*changeGraphics0)(int,const char*),
		void (*changeArrowHead0)(int,const char*)
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
	int cthread,
	void (*showProgress)(int, int)	);

END_C_DECLS
#endif

