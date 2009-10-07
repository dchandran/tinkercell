#ifndef TINKERCELL_TC_MAIN_API_H
#define TINKERCELL_TC_MAIN_API_H

#include "../TCstructs.h"
/*! 
 \brief get all visible items
 \ingroup Get items
*/
Array (*tc_allItems)() = 0;
/*! 
 \brief get all selected items
 \ingroup Get items
*/
Array (*tc_selectedItems)() = 0;
/*! 
 \brief get all items of the given family items
 \ingroup Get items
*/
Array (*tc_itemsOfFamily)(const char* family) = 0;
/*! 
 \brief get subset of items that belong to the given family
 \ingroup Get items
*/
Array (*tc_itemsOfFamilyFrom)(const char* family, Array itemsToSelectFrom) = 0;
/*! 
 \brief get the first item with the given name (full name)
 \ingroup Get items
*/
OBJ (*tc_find)(const char* fullname) = 0;
/*! 
 \brief get all items with the given names (full names)
 \ingroup Get items
*/
Array (*tc_findItems)(char** names) = 0;
/*! 
 \brief select an item
 \ingroup Get items
*/
void (*tc_select)(OBJ item) = 0;
/*! 
 \brief deselect all items
 \ingroup Get items
*/
void (*tc_deselect)() = 0;
/*! 
 \brief get the full name of an item
 \ingroup Annotation
*/
char* (*tc_getName)(OBJ item) = 0;
/*! 
 \brief set the name of an item (not full name)
 \ingroup Annotation
*/
void (*tc_rename)(OBJ item,const char* name) = 0;
/*! 
 \brief get the full names of several items
 \ingroup Annotation
*/
char** (*tc_getNames)(Array items) = 0;
/*! 
 \brief get the family name of an item
 \ingroup Annotation
*/
char* (*tc_getFamily)(OBJ item) = 0;
/*! 
 \brief check is an item belongs in a family (or in a sub-family)
 \ingroup Annotation
*/
int (*tc_isA)(OBJ item,const char* family) = 0;
/*! 
 \brief show text in the output window.
 \ingroup Input and Output
*/
void (*tc_print)(const char* text) = 0;
/*! 
 \brief show error text in the output window.
 \ingroup Input and Output
*/
void (*tc_errorReport)(const char* text) = 0;
/*! 
 \brief show table in the output window.
 \ingroup Input and Output
*/
void (*tc_printTable)(Matrix data) = 0;
/*! 
 \brief show file contents in the output window. 
 \ingroup Input and Output
*/
void (*tc_printFile)(const char* filename) = 0;
/*! 
 \brief cleat the contents in the output window. 
 \ingroup Input and Output
*/
void (*tc_clear)() = 0;
/*! 
 \brief delete an item
 \ingroup Insert and remove
*/
void (*tc_remove)(OBJ item) = 0;
/*! 
 \brief get the x location of an item
 \ingroup Get and set position
*/
double (*tc_getY)(OBJ item) = 0;
/*! 
 \brief get the y location of an item
 \ingroup Get and set position
*/
double (*tc_getX)(OBJ item) = 0;
/*! 
 \brief get the y location of a list item. Output is a N x 2 matrix
 \ingroup Get and set position
*/
Matrix (*tc_getPos)(Array items) = 0;
/*! 
 \brief set the x and y location of an item
 \ingroup Get and set position
*/
void (*tc_setPos)(OBJ item,double x,double y) = 0;
/*! 
 \brief set the x and y location of a list of N items. Input a matrix of positions, with N rows and 2 columns (x,y)
 \ingroup Get and set position
*/
void (*tc_setPosMulti)(Array items, Matrix positions) = 0;
/*! 
 \brief move all the selected items by a given amount
 \ingroup Get and set position
*/
void (*tc_moveSelected)(double dx,double dy) = 0;
/*! 
 \brief is this running in MS windows?
 \ingroup System information
*/
int (*tc_isWindows)() = 0;
/*! 
 \brief is this running in a Mac?
 \ingroup System information
*/
int (*tc_isMac)() = 0;
/*! 
 \brief is this running in Linux?
 \ingroup System information
*/
int (*tc_isLinux)() = 0;
/*! 
 \brief TinkerCell application folder
 \ingroup System information
*/
char* (*tc_appDir)() = 0;
/*! 
 \brief create an input window that can call a dynamic library
 \ingroup Input and Output
*/
void (*tc_createInputWindowFromFile)(Matrix input, const char* filename,const char* functionname, const char* title) = 0;
/*!
 \brief create an input window that can call a dynamic library
 \ingroup Input and Output
*/
void (*tc_createInputWindow)(Matrix, const char* title, void (*f)(Matrix)) = 0;
/*! 
 \brief create an input window that can call a dynamic library
 \ingroup Input and Output
*/
void (*tc_addInputWindowOptions)(const char*, int i, int j, char **) = 0;
/*! 
 \brief open a new window
 \ingroup Input and Output
*/
void (*tc_openNewWindow)(const char * title) = 0;
/*! 
 \brief get numerical data belonging to a specific tool, e.g. Numerical Attributes
 \ingroup Data
*/
double (*tc_getNumericalData)(OBJ item,const char* data, const char* row_name, const char* column_name) = 0;
/*! 
 \brief get text data belonging to a specific tool, e.g. Text Attributes
 \ingroup Data
*/
char* (*tc_getTextData)(OBJ item,const char* data, const char* row_name, const char* column_name) = 0;
/*! 
 \brief set numerical data belonging to a specific tool, e.g. Numerical Attributes
 \ingroup Data
*/
void (*tc_setNumericalData)(OBJ item,const char* data, const char* row_name, const char* column_name,double value) = 0;
/*! 
 \brief set text data belonging to a specific tool, e.g. Text Attributes
 \ingroup Data
*/
void (*tc_setTextData)(OBJ item,const char* data, const char* row_name, const char* column_name,const char* text) = 0;
/*! 
 \brief get child items of the given item
 \ingroup Get items
*/
Array (*tc_getChildren)(OBJ) = 0;
/*! 
 \brief get parent item of the given item
 \ingroup Get items
*/
OBJ (*tc_getParent)(OBJ) = 0;
/*! 
 \brief get the row names for the given numerical data table of the given item
 \ingroup Data
*/
char** (*tc_getNumericalDataRowNames)(OBJ item,const char* data) = 0;
/*! 
 \brief get the column names for the given numerical data table of the given item
 \ingroup Data
*/
char** (*tc_getNumericalDataColNames)(OBJ item,const char* data) = 0;
/*! 
 \brief get the row names for the given text data table of the given item
 \ingroup Data
*/
char** (*tc_getTextDataRowNames)(OBJ item,const char* data) = 0;
/*! 
 \brief get the column names for the given text data table of the given item
 \ingroup Data
*/
char** (*tc_getTextDataColNames)(OBJ item,const char* data) = 0;
/*! 
 \brief get the entire data matrix for the given numerical data table of the given item
 \ingroup Data
*/
Matrix (*tc_getNumericalDataMatrix)(OBJ item,const char* data) = 0;
/*! 
 \brief set a new data matrix for an item. Use 0 for the global model item.
 \ingroup Data
*/
void (*tc_setNumericalDataMatrix)(OBJ,const char*,Matrix) = 0;
/*! 
 \brief get the values in the given row in the given text data table of the given item
 \ingroup Data
*/
char** (*tc_getTextDataRow)(OBJ item,const char* data,const char* rowname) = 0;
/*! 
 \brief get the values in the given column in the given text data table of the given item
 \ingroup Data
*/
char** (*tc_getTextDataCol)(OBJ item,const char* data,const char* colname) = 0;
/*! 
 \brief get all the numeric data table names for the given item. Use 0 for the global tables.
 \ingroup Data
*/
char** (*tc_getNumericalDataNames)(OBJ) = 0;
/*! 
 \brief get all the text data table names for the given item. Use 0 for the global tables.
 \ingroup Data
*/
char** (*tc_getTextDataNames)(OBJ) = 0;
/*! 
 \brief zoom by the given factor (0 - 1)
 \ingroup Input and Output
*/
void (*tc_zoom)(double factor) = 0;
/*! 
 \brief initialize main
 \ingroup init
*/
void tc_Main_api_initialize(
	    Array (*tc_allItems0)(),
		Array (*tc_selectedItems0)(),
		Array (*tc_itemsOfFamily0)(const char*),
		Array (*tc_itemsOfFamily1)(const char*, Array),
		OBJ (*tc_find0)(const char*),
		Array (*tc_findItems0)(char**),
		void (*tc_select0)(OBJ),
		void (*tc_deselect0)(),
		char* (*tc_getName0)(OBJ),
		void (*tc_setName0)(OBJ item,const char* name),
		char** (*tc_getNames0)(Array),
		char* (*tc_getFamily0)(OBJ),
		int (*tc_isA0)(OBJ,const char*),

		void (*tc_clearText)(),
		void (*tc_outputText0)(const char*),
		void (*tc_errorReport0)(const char*),
		void (*tc_outputTable0)(Matrix),
		void (*tc_printFile0)(const char*),

		void (*tc_removeItem0)(OBJ),

		double (*tc_getY0)(OBJ),
		double (*tc_getX0)(OBJ),
		Matrix (*tc_getPos0)(Array),
		void (*tc_setPos0)(OBJ,double,double),
		void (*tc_setPos1)(Array,Matrix),
		void (*tc_moveSelected0)(double,double),

		int (*tc_isWindows0)(),
		int (*tc_isMac0)(),
		int (*tc_isLinux0)(),
		char* (*tc_appDir0)(),
		
		void (*tc_createInputWindow0)(Matrix,const char*,const char*, const char*),
        void (*tc_createInputWindow1)(Matrix, const char*, void (*f)(Matrix)),
		void (*tc_addInputWindowOptions0)(const char*, int i, int j, char **),
		void (*tc_openNewWindow0)(const char * title),
		
		double (*tc_getNumericalData0)(OBJ,const char*, const char*, const char*),
		char* (*tc_getTextData0)(OBJ,const char*, const char*, const char*),
		void (*tc_setNumericalData0)(OBJ,const char*, const char*, const char*,double),
		void (*tc_setTextData0)(OBJ,const char*, const char*, const char*,const char*),
		Array (*tc_getChildren0)(OBJ),
		OBJ (*tc_getParent0)(OBJ),
		
		char** (*tc_getNumericalDataRows0)(OBJ,const char*),
		char** (*tc_getNumericalDataCols0)(OBJ,const char*),
		char** (*tc_getTextDataRows0)(OBJ,const char*),
		char** (*tc_getTextDataCols0)(OBJ,const char*),
		
		Matrix (*tc_getNumericalDataMatrix0)(OBJ,const char*),
		void (*tc_setNumericalDataMatrix0)(OBJ,const char*,Matrix),
		char** (*tc_getTextDataRow0)(OBJ,const char*,const char*),
		char** (*tc_getTextDataCol0)(OBJ,const char*,const char*),
		
		char** (*tc_getNumericalDataNames0)(OBJ),
		char** (*tc_getTextDataNames0)(OBJ),
		
		void (*tc_zoom0)(double factor)
	)
{
	tc_allItems = tc_allItems0;
	tc_selectedItems = tc_selectedItems0; 
	tc_itemsOfFamily = tc_itemsOfFamily0;
	tc_itemsOfFamilyFrom = tc_itemsOfFamily1;
	tc_find = tc_find0;
	tc_findItems = tc_findItems0;
	tc_select = tc_select0;
	tc_deselect = tc_deselect0;
	tc_getName = tc_getName0;
	tc_rename = tc_setName0;
	
	tc_getNames = tc_getNames0;
	tc_getFamily = tc_getFamily0;
	tc_isA = tc_isA0;

	tc_clear = tc_clearText;
	tc_print = tc_outputText0;
	tc_errorReport = tc_errorReport0;
	tc_printTable = tc_outputTable0;
	tc_printFile = tc_printFile0;

	tc_remove = tc_removeItem0;

	tc_getY = tc_getY0;
	tc_getX = tc_getX0;
	tc_getPos = tc_getPos0;
	tc_setPos = tc_setPos0;
	tc_setPosMulti = tc_setPos1;
	tc_moveSelected = tc_moveSelected0;

	tc_isWindows = tc_isWindows0;
	tc_isMac = tc_isMac0;
	tc_isLinux = tc_isLinux0;
	tc_appDir = tc_appDir0;
    tc_createInputWindow = tc_createInputWindow1;
    tc_createInputWindowFromFile = tc_createInputWindow0;
	tc_addInputWindowOptions = tc_addInputWindowOptions0;
	
	tc_openNewWindow = tc_openNewWindow0;
	
	tc_getNumericalData = tc_getNumericalData0;
	tc_getTextData = tc_getTextData0;
	tc_setNumericalData = tc_setNumericalData0;
	tc_setTextData = tc_setTextData0;
	tc_getChildren = tc_getChildren0;
	tc_getParent = tc_getParent0;
	
	tc_getNumericalDataRowNames = tc_getNumericalDataRows0;
	tc_getNumericalDataColNames = tc_getNumericalDataCols0;
	tc_getTextDataRowNames = tc_getTextDataRows0;
	tc_getTextDataColNames = tc_getTextDataCols0;
	
	tc_getNumericalDataMatrix = tc_getNumericalDataMatrix0;
	tc_setNumericalDataMatrix = tc_setNumericalDataMatrix0;
	tc_getTextDataRow = tc_getTextDataRow0;
	tc_getTextDataCol = tc_getTextDataCol0;
	
	tc_getNumericalDataNames = tc_getNumericalDataNames0;
	tc_getTextDataNames = tc_getTextDataNames0;
	
	tc_zoom = tc_zoom0;
}

/*! 
 \brief show progress of current operation. provide name of this library file.
 \ingroup Input and Output
*/
void (*tc_showProgress)(const char * name, int progress);
/*! 
 \brief initialize main
 \ingroup init
*/
void tc_Progress_api_initialize( void (*tc_showProgress0)(const char * , int) )
{
	tc_showProgress = tc_showProgress0;
}

#endif
