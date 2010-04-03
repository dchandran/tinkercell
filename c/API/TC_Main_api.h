#ifndef TINKERCELL_TC_MAIN_API_H
#define TINKERCELL_TC_MAIN_API_H

#include "../TCstructs.h"

/*! 
 \brief get all visible items
 \ingroup Get items
*/
ArrayOfItems tc_allItems();

/*! 
 \brief get all selected items
 \ingroup Get items
*/
ArrayOfItems tc_selectedItems();

/*!
 \brief get all items of the given family items
 \ingroup Get items
*/
ArrayOfItems tc_itemsOfFamily(String family);

/*! 
 \brief get subset of items that belong to the given family
 \ingroup Get items
*/
ArrayOfItems tc_itemsOfFamilyFrom(String family, ArrayOfItems itemsToSelectFrom);

/*! 
 \brief get the first item with the given name (full name)
 \ingroup Get items
*/
Item tc_find(String fullname);

/*! 
 \brief get all items with the given names (full names)
 \ingroup Get items
*/
ArrayOfItems tc_findItems(ArrayOfStrings names);

/*! 
 \brief select an item
 \ingroup Get items
*/
void tc_select(Item item);

/*! 
 \brief deselect all items
 \ingroup Get items
*/
void tc_deselect();

/*! 
 \brief get the full name of an item
 \ingroup Annotation
*/
String tc_getName(Item item);

/*! 
 \brief set the name of an item (not full name)
 \ingroup Annotation
*/
void tc_rename(Item item,String name);

/*! 
 \brief get the full names of several items
 \ingroup Annotation
*/
ArrayOfStrings tc_getNames(ArrayOfItems items);

/*! 
 \brief get the family name of an item
 \ingroup Annotation
*/
String tc_getFamily(Item item);

/*! 
 \brief check is an item belongs in a family (or in a sub-family)
 \ingroup Annotation
*/
int tc_isA(Item item,String family);

/*! 
 \brief show text in the output window.
 \ingroup Input and Output
*/
void tc_print(String text);

/*! 
 \brief show error text in the output window.
 \ingroup Input and Output
*/
void tc_errorReport(String text);

/*! 
 \brief show table in the output window.
 \ingroup Input and Output
*/
void tc_printTable(Matrix data);

/*! 
 \brief show file contents in the output window. 
 \ingroup Input and Output
*/
void tc_printFile(String filename);

/*! 
 \brief cleat the contents in the output window. 
 \ingroup Input and Output
*/
void tc_clear();

/*! 
 \brief delete an item
 \ingroup Insert and remove
*/
void tc_remove(Item item);

/*! 
 \brief get the x location of an item
 \ingroup Get and set position
*/
double tc_getY(Item item);

/*! 
 \brief get the y location of an item
 \ingroup Get and set position
*/
double tc_getX(Item item);

/*! 
 \brief get the y location of a list item. Output is a N x 2 matrix
 \ingroup Get and set position
*/
Matrix tc_getPos(ArrayOfItems items);

/*! 
 \brief set the x and y location of an item
 \ingroup Get and set position
*/
void tc_setPos(Item item,double x,double y);

/*! 
 \brief set the x and y location of a list of N items. Input a matrix of positions, with N rows and 2 columns (x,y)
 \ingroup Get and set position
*/
void tc_setPosMulti(ArrayOfItems items, Matrix positions);

/*! 
 \brief move all the selected items by a given amount
 \ingroup Get and set position
*/
void tc_moveSelected(double dx,double dy);

/*! 
 \brief is this running in MS windows?
 \ingroup System information
*/
int tc_isWindows();

/*! 
 \brief is this running in a Mac?
 \ingroup System information
*/
int tc_isMac();

/*! 
 \brief is this running in Linux?
 \ingroup System information
*/
int tc_isLinux();

/*! 
 \brief TinkerCell application folder
 \ingroup System information
*/
String tc_appDir();

/*! 
 \brief create an input window that can call a dynamic library
 \ingroup Input and Output
*/
void tc_createInputWindowFromFile(Matrix input, String filename,String functionname, String title);

/*!
 \brief create an input window that can call a dynamic library
 \ingroup Input and Output
*/
void tc_createInputWindow(Matrix input, String title, void (*f)(Matrix));

/*! 
 \brief add options to an existing input window at the i,j-th cell. Options will appear in a list
 \ingroup Input and Output
*/
void tc_addInputWindowOptions(String title, int i, int j, ArrayOfStrings options);

/*! 
 \brief add a yes or no type of option to an existing input window at the i,j-th cell
 \ingroup Input and Output
*/
void tc_addInputWindowCheckbox(String title, int i, int j);

/*! 
 \brief open a new graphics window
 \ingroup Input and Output
*/
void tc_openNewWindow(String title);
/*! 
 \brief get child items of the given item
 \ingroup Get items
*/
ArrayOfItems tc_getChildren(Item o);

/*! 
 \brief get parent item of the given item
 \ingroup Get items
*/
Item tc_getParent(Item o);

/*! 
 \brief get the entire data matrix for the given numerical data table of the given item
 \ingroup Data
*/
Matrix tc_getNumericalData(Item item,String data);

/*! 
 \brief set a new data matrix for an item. Use 0 for the global model item.
 \ingroup Data
*/
void tc_setNumericalData(Item o,String title,Matrix data);

/*! 
 \brief get the entire data matrix for the given strings data table of the given item
 \ingroup Data
*/
TableOfStrings tc_getTextData(Item item,String data);

/*! 
 \brief set the entire data matrix for the given strings data table of the given item
 \ingroup Data
*/
void tc_setTextData(Item o,String title,TableOfStrings data);

/*! 
 \brief get all the numeric data table names for the given item. Use 0 for the global tables.
 \ingroup Data
*/
ArrayOfStrings tc_getNumericalDataNames(Item o);

/*! 
 \brief get all the text data table names for the given item. Use 0 for the global tables.
 \ingroup Data
*/
ArrayOfStrings tc_getTextDataNames(Item o);

/*! 
 \brief zoom by the given factor (0 - 1)
 \ingroup Input and Output
*/
void tc_zoom(double factor);

/*! 
 \brief get a text from the user (dialog)
 \ingroup Dialogs
*/
String tc_getString(String title);
/*! 
 \brief get a file from the user (dialog)
 \ingroup Dialogs
*/
String tc_getFilename();

/*! 
 \brief get a text from the user (dialog) from a list of selections
 \ingroup Dialogs
*/
int tc_getFromList(String title, ArrayOfStrings list,String selectedString, int comboBox);
/*! 
 \brief get a number from the user (dialog)
 \ingroup Dialogs
*/
double tc_getNumber(String title);

/*! 
 \brief get a list of numbers from the user (dialog) into the argument array
 \ingroup Dialogs
*/
void tc_getNumbers(ArrayOfStrings labels, double* result);

/*! 
 \brief display a dialog with a text and a yes and no button
 \param char* displayed message or question
 \ingroup Dialogs
*/
int tc_askQuestion(String message);

/*! 
 \brief display a dialog with a text message and a close button
 \param char* displayed message
 \ingroup Dialogs
*/
void tc_messageDialog(String message);

/*!
 \brief get pointer to the current thread
 \ingroup Programming interface
*/
Item tc_thisThread();

/*!
 \brief create a window with several sliders. when the sliders change, the given function will be called with the values in the sliders
 \ingroup Input and Output
*/
void tc_createSliders(Matrix input, void (*f)(Matrix));

/*! 
 \brief initialize main
 \ingroup init
*/
void tc_Main_api_initialize(
	    ArrayOfItems (*tc_allItems0)(),
		ArrayOfItems (*tc_selectedItems0)(),
		ArrayOfItems (*tc_itemsOfFamily0)(String),
		ArrayOfItems (*tc_itemsOfFamily1)(String, ArrayOfItems),
		Item (*tc_find0)(String),
		ArrayOfItems (*tc_findItems0)(ArrayOfStrings),
		void (*tc_select0)(Item),
		void (*tc_deselect0)(),
		String (*tc_getName0)(Item),
		void (*tc_setName0)(Item item,String name),
		ArrayOfStrings (*tc_getNames0)(ArrayOfItems),
		String (*tc_getFamily0)(Item),
		int (*tc_isA0)(Item,String),

		void (*tc_clearText)(),
		void (*tc_outputText0)(String),
		void (*tc_errorReport0)(String),
		void (*tc_outputTable0)(Matrix),
		void (*tc_printFile0)(String),

		void (*tc_removeItem0)(Item),

		double (*tc_getY0)(Item),
		double (*tc_getX0)(Item),
		Matrix (*tc_getPos0)(ArrayOfItems),
		void (*tc_setPos0)(Item,double,double),
		void (*tc_setPos1)(ArrayOfItems,Matrix),
		void (*tc_moveSelected0)(double,double),

		int (*tc_isWindows0)(),
		int (*tc_isMac0)(),
		int (*tc_isLinux0)(),
		String (*tc_appDir0)(),
		
		void (*tc_createInputWindow0)(Matrix,String,String, String),
        void (*tc_createInputWindow1)(Matrix, String, void (*f)(Matrix)),
		void (*createSliders)(Item, Matrix, void (*f)(Matrix)),
		
		void (*tc_addInputWindowOptions0)(String, int i, int j, ArrayOfStrings),
		void (*tc_addInputWindowCheckbox0)(String, int i, int j),
		void (*tc_openNewWindow0)(String title),
		
		ArrayOfItems (*tc_getChildren0)(Item),
		Item (*tc_getParent0)(Item),
		
		Matrix (*tc_getNumericalData0)(Item,String),
		void (*tc_setNumericalData0)(Item,String,Matrix),
		TableOfStrings (*tc_getTextData0)(Item,String),
		void (*tc_setTextData0)(Item,String, TableOfStrings),
				
		ArrayOfStrings (*tc_getNumericalDataNames0)(Item),
		ArrayOfStrings (*tc_getTextDataNames0)(Item),
		
		void (*tc_zoom0)(double factor),
		
		String (*getString)(String),
		int (*getSelectedString)(String, ArrayOfStrings, String, int),
		double (*getNumber)(String),
		void (*getNumbers)( ArrayOfStrings, double * ),
		String (*getFilename)(),
		
		int (*askQuestion)(String),
		void (*messageDialog)(String),
		
		void (*setSize0)(void*,double,double,int),
		double (*getWidth0)(void*),
		double (*getHeight0)(void*),
		void (*setAngle0)(void*,double,int),
		double (*getAngle0)(void*),
		int (*getColorR0)(void*),
		int (*getColorG0)(void*),
		int (*getColorB0)(void*),
		void (*setColor0)(void*,int,int,int,int),
		
		void (*changeGraphics0)(void*,const char*),
		void (*changeArrowHead0)(void*,const char*)
	);

/*! 
 \brief show progress of current operation
 \ingroup Input and Output
*/
void tc_showProgress(int progress);

/*! 
 \brief initialize main
 \ingroup init
*/
void tc_CThread_api_initialize( 
	Item cthread,
	void (*showProgress)(Item, int)	);

#endif
