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
ArrayOfItems tc_itemsOfFamily(const char* family);

/*! 
 \brief get subset of items that belong to the given family
 \ingroup Get items
*/
ArrayOfItems tc_itemsOfFamilyFrom(const char* family, ArrayOfItems itemsToSelectFrom);

/*! 
 \brief get the first item with the given name (full name)
 \ingroup Get items
*/
void * tc_find(const char* fullname);

/*! 
 \brief get all items with the given names (full names)
 \ingroup Get items
*/
ArrayOfItems tc_findItems(ArrayOfStrings names);

/*! 
 \brief select an item
 \ingroup Get items
*/
void tc_select(void * item);

/*! 
 \brief deselect all items
 \ingroup Get items
*/
void tc_deselect();

/*! 
 \brief get the full name of an item
 \ingroup Annotation
*/
char* tc_getName(void * item);

/*! 
 \brief set the name of an item (not full name)
 \ingroup Annotation
*/
void tc_rename(void * item,const char* name);

/*! 
 \brief get the full names of several items
 \ingroup Annotation
*/
ArrayOfStrings tc_getNames(ArrayOfItems items);

/*! 
 \brief get the family name of an item
 \ingroup Annotation
*/
char* tc_getFamily(void * item);

/*! 
 \brief check is an item belongs in a family (or in a sub-family)
 \ingroup Annotation
*/
int tc_isA(void * item,const char* family);

/*! 
 \brief show text in the output window.
 \ingroup Input and Output
*/
void tc_print(const char* text);

/*! 
 \brief show error text in the output window.
 \ingroup Input and Output
*/
void tc_errorReport(const char* text);

/*! 
 \brief show table in the output window.
 \ingroup Input and Output
*/
void tc_printTable(Matrix data)

/*! 
 \brief show file contents in the output window. 
 \ingroup Input and Output
*/
void tc_printFile(const char* filename);

/*! 
 \brief cleat the contents in the output window. 
 \ingroup Input and Output
*/
void tc_clear();

/*! 
 \brief delete an item
 \ingroup Insert and remove
*/
void tc_remove(void * item);

/*! 
 \brief get the x location of an item
 \ingroup Get and set position
*/
double tc_getY(void * item);

/*! 
 \brief get the y location of an item
 \ingroup Get and set position
*/
double tc_getX(void * item);

/*! 
 \brief get the y location of a list item. Output is a N x 2 matrix
 \ingroup Get and set position
*/
Matrix tc_getPos(ArrayOfItems items);

/*! 
 \brief set the x and y location of an item
 \ingroup Get and set position
*/
void tc_setPos(void * item,double x,double y);

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
char* tc_appDir();

/*! 
 \brief create an input window that can call a dynamic library
 \ingroup Input and Output
*/
void tc_createInputWindowFromFile(Matrix input, const char* filename,const char* functionname, const char* title);

/*!
 \brief create an input window that can call a dynamic library
 \ingroup Input and Output
*/
void tc_createInputWindow(Matrix input, const char* title, void (*f)(Matrix));

/*! 
 \brief add options to an existing input window at the i,j-th cell. Options will appear in a list
 \ingroup Input and Output
*/
void tc_addInputWindowOptions(const char* title, int i, int j, char ** options);

/*! 
 \brief add a yes or no type of option to an existing input window at the i,j-th cell
 \ingroup Input and Output
*/
void tc_addInputWindowCheckbox(const char * title, int i, int j);

/*! 
 \brief open a new graphics window
 \ingroup Input and Output
*/
void tc_openNewWindow(const char * title);
/*! 
 \brief get child items of the given item
 \ingroup Get items
*/
ArrayOfItems tc_getChildren(void * o);

/*! 
 \brief get parent item of the given item
 \ingroup Get items
*/
void * tc_getParent(void * o);

/*! 
 \brief get the entire data matrix for the given numerical data table of the given item
 \ingroup Data
*/
Matrix tc_getNumericalData(void * item,const char* data);

/*! 
 \brief set a new data matrix for an item. Use 0 for the global model item.
 \ingroup Data
*/
void tc_setNumericalData(void * o,const char* title,Matrix data);

/*! 
 \brief get the entire data matrix for the given strings data table of the given item
 \ingroup Data
*/
TableOfStrings tc_getTextData(void * item,const char* data);

/*! 
 \brief set the entire data matrix for the given strings data table of the given item
 \ingroup Data
*/
void tc_setTextData(void * o,const char* title,TableOfStrings data);

/*! 
 \brief get all the numeric data table names for the given item. Use 0 for the global tables.
 \ingroup Data
*/
ArrayOfStrings tc_getNumericalDataNames(void * o);

/*! 
 \brief get all the text data table names for the given item. Use 0 for the global tables.
 \ingroup Data
*/
ArrayOfStrings tc_getTextDataNames(void * o);

/*! 
 \brief zoom by the given factor (0 - 1)
 \ingroup Input and Output
*/
void tc_zoom(double factor);

/*! 
 \brief get a text from the user (dialog)
 \ingroup Dialogs
*/
char* tc_getString(const char* title);
/*! 
 \brief get a file from the user (dialog)
 \ingroup Dialogs
*/
char* tc_getFilename();

/*! 
 \brief get a text from the user (dialog) from a list of selections
 \ingroup Dialogs
*/
int tc_getFromList(const char* title, ArrayOfStrings list,const char* selectedString, int comboBox);
/*! 
 \brief get a number from the user (dialog)
 \ingroup Dialogs
*/
double tc_getNumber(const char* title);

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
int tc_askQuestion(const char* message);

/*! 
 \brief display a dialog with a text message and a close button
 \param char* displayed message
 \ingroup Dialogs
*/
void tc_messageDialog(const char* message);

/*!
 \brief get pointer to the current thread
 \ingroup Programming interface
*/
void * tc_thisThread();

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
		ArrayOfItems (*tc_itemsOfFamily0)(const char*),
		ArrayOfItems (*tc_itemsOfFamily1)(const char*, ArrayOfItems),
		void * (*tc_find0)(const char*),
		ArrayOfItems (*tc_findItems0)(ArrayOfStrings),
		void (*tc_select0)(void *),
		void (*tc_deselect0)(),
		char* (*tc_getName0)(void *),
		void (*tc_setName0)(void * item,const char* name),
		ArrayOfStrings (*tc_getNames0)(ArrayOfItems),
		char* (*tc_getFamily0)(void *),
		int (*tc_isA0)(void *,const char*),

		void (*tc_clearText)(),
		void (*tc_outputText0)(const char*),
		void (*tc_errorReport0)(const char*),
		void (*tc_outputTable0)(Matrix),
		void (*tc_printFile0)(const char*),

		void (*tc_removeItem0)(void *),

		double (*tc_getY0)(void *),
		double (*tc_getX0)(void *),
		Matrix (*tc_getPos0)(ArrayOfItems),
		void (*tc_setPos0)(void *,double,double),
		void (*tc_setPos1)(ArrayOfItems,Matrix),
		void (*tc_moveSelected0)(double,double),

		int (*tc_isWindows0)(),
		int (*tc_isMac0)(),
		int (*tc_isLinux0)(),
		char* (*tc_appDir0)(),
		
		void (*tc_createInputWindow0)(Matrix,const char*,const char*, const char*),
        void (*tc_createInputWindow1)(Matrix, const char*, void (*f)(Matrix)),
		void (*createSliders)(void*, Matrix, void (*f)(Matrix)),
		
		void (*tc_addInputWindowOptions0)(const char*, int i, int j, char **),
		void (*tc_addInputWindowCheckbox0)(const char*, int i, int j),
		void (*tc_openNewWindow0)(const char * title),
		
		ArrayOfItems (*tc_getChildren0)(void *),
		void * (*tc_getParent0)(void *),
		
		Matrix (*tc_getNumericalData0)(void *,const char*),
		void (*tc_setNumericalData0)(void *,const char*,Matrix),
		TableOfStrings (*tc_getTextData0)(void *,const char*),
		void (*tc_setTextData0)(void *,const char*, TableOfString),
				
		ArrayOfStrings (*tc_getNumericalDataNames0)(void *),
		ArrayOfStrings (*tc_getTextDataNames0)(void *),
		
		void (*tc_zoom0)(double factor),
		
		char* (*getString)(const char*),
		int (*getSelectedString)(const char*, ArrayOfStrings, const char*, int),
		double (*getNumber)(const char*),
		void (*getNumbers)( ArrayOfStrings, double * ),
		char* (*getFilename)(),
		
		int (*askQuestion)(const char*),
		void (*messageDialog)(const char*)
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
	void * cthread,
	void (*showProgress)(void*, int)	);

#endif
