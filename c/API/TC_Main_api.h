#ifndef TINKERCELL_TC_MAIN_API_H
#define TINKERCELL_TC_MAIN_API_H

#include "../TCstructs.h"
/*! 
 \brief get all visible items
 \ingroup Get items
*/
Array (*tc_allItems)();
/*! 
 \brief get all selected items
 \ingroup Get items
*/
Array (*tc_selectedItems)();
/*! 
 \brief get all items of the given family items
 \ingroup Get items
*/
Array (*tc_itemsOfFamily)(const char* family);
/*! 
 \brief get the first item with the given name (full name)
 \ingroup Get items
*/
OBJ (*tc_find)(const char* fullname);
/*! 
 \brief select an item
 \ingroup Get items
*/
void (*tc_select)(OBJ item);
/*! 
 \brief deselect all items
 \ingroup Get items
*/
void (*tc_deselect)();
/*! 
 \brief get the full name of an item
 \ingroup Annotation
*/
char* (*tc_getName)(OBJ item);
/*! 
 \brief set the name of an item (not full name)
 \ingroup Annotation
*/
void (*tc_rename)(OBJ item,const char* name);
/*! 
 \brief get the full names of several items
 \ingroup Annotation
*/
char** (*tc_getNames)(Array items);
/*! 
 \brief get the family name of an item
 \ingroup Annotation
*/
char* (*tc_getFamily)(OBJ item);
/*! 
 \brief check is an item belongs in a family (or in a sub-family)
 \ingroup Annotation
*/
int (*tc_isA)(OBJ item,const char* family);
/*! 
 \brief show text in the output window.
 \ingroup Input and output
*/
void (*tc_print)(const char* text);
/*! 
 \brief show error text in the output window.
 \ingroup Input and output
*/
void (*tc_errorReport)(const char* text);
/*! 
 \brief show table in the output window.
 \ingroup Input and output
*/
void (*tc_printTable)(Matrix data);
/*! 
 \brief show file contents in the output window. 
 \ingroup Input and output
*/
void (*tc_printFile)(const char* filename);
/*! 
 \brief cleat the contents in the output window. 
 \ingroup Input and output
*/
void (*tc_clear)();
/*! 
 \brief delete an item
 \ingroup Insert and remove
*/
void (*tc_remove)(OBJ item);
/*! 
 \brief get the x location of an item
 \ingroup Get and set position
*/
double (*tc_getY)(OBJ item);
/*! 
 \brief get the y location of an item
 \ingroup Get and set position
*/
double (*tc_getX)(OBJ item);
/*! 
 \brief get the y location of a list item. Output is a N x 2 matrix
 \ingroup Get and set position
*/
Matrix (*tc_getPos)(Array items);
/*! 
 \brief set the x and y location of an item
 \ingroup Get and set position
*/
void (*tc_setPos)(OBJ item,double x,double y);
/*! 
 \brief set the x and y location of a list of N items. Input a matrix of positions, with N rows and 2 columns (x,y)
 \ingroup Get and set position
*/
void (*tc_setPosMulti)(Array items, Matrix positions);
/*! 
 \brief move all the selected items by a given amount
 \ingroup Get and set position
*/
void (*tc_moveSelected)(double dx,double dy);
/*! 
 \brief is this running in MS windows?
 \ingroup System information
*/
int (*tc_isWindows)();
/*! 
 \brief is this running in a Mac?
 \ingroup System information
*/
int (*tc_isMac)();
/*! 
 \brief is this running in Linux?
 \ingroup System information
*/
int (*tc_isLinux)();
/*! 
 \brief TinkerCell application folder
 \ingroup System information
*/
char* (*tc_appDir)();
/*! 
 \brief create an input window that can call a dynamic library
 \ingroup Input and output
*/
void (*tc_createInputWindowFromFile)(Matrix input, const char* filename,const char* functionname, const char* title);
/*!
 \brief create an input window that can call a dynamic library
 \ingroup Input and output
*/
void (*tc_createInputWindow)(Matrix, const char* title, void (*f)(Matrix));
/*! 
 \brief create an input window that can call a dynamic library
 \ingroup Input and output
*/
void (*tc_addInputWindowOptions)(const char*, int i, int j, char **);
/*! 
 \brief open a new window
 \ingroup Input and output
*/
void (*tc_openNewWindow)(const char * title);
/*! 
 \brief get numerical data belonging to a specific tool, e.g. Numerical Attributes
 \ingroup Data
*/
double (*tc_getNumericalData)(OBJ item,const char* data, const char* row_name, const char* column_name);
/*! 
 \brief get text data belonging to a specific tool, e.g. Text Attributes
 \ingroup Data
*/
char* (*tc_getTextData)(OBJ item,const char* data, const char* row_name, const char* column_name);
/*! 
 \brief set numerical data belonging to a specific tool, e.g. Numerical Attributes
 \ingroup Data
*/
void (*tc_setNumericalData)(OBJ item,const char* data, const char* row_name, const char* column_name,double value);
/*! 
 \brief set text data belonging to a specific tool, e.g. Text Attributes
 \ingroup Data
*/
void (*tc_setTextData)(OBJ item,const char* data, const char* row_name, const char* column_name,const char* text);
/*! 
 \brief get child items of the given item
 \ingroup Get items
*/
Array (*tc_getChildren)(OBJ);
/*! 
 \brief get parent item of the given item
 \ingroup Get items
*/
OBJ (*tc_getParent)(OBJ);
/*! 
 \brief get the row names for the given numerical data table of the given item
 \ingroup Data
*/
char** (*tc_getNumericalDataRowNames)(OBJ item,const char* data);
/*! 
 \brief get the column names for the given numerical data table of the given item
 \ingroup Data
*/
char** (*tc_getNumericalDataColNames)(OBJ item,const char* data);
/*! 
 \brief get the row names for the given text data table of the given item
 \ingroup Data
*/
char** (*tc_getTextDataRowNames)(OBJ item,const char* data);
/*! 
 \brief get the column names for the given text data table of the given item
 \ingroup Data
*/
char** (*tc_getTextDataColNames)(OBJ item,const char* data);
/*! 
 \brief get the entire data matrix for the given numerical data table of the given item
 \ingroup Data
*/
Matrix (*tc_getNumericalDataMatrix)(OBJ item,const char* data);
/*! 
 \brief set a new data matrix for an item. Use 0 for the global model item.
 \ingroup Data
*/
void (*tc_setNumericalDataMatrix)(OBJ,const char*,Matrix);
/*! 
 \brief get the values in the given row in the given text data table of the given item
 \ingroup Data
*/
char** (*tc_getTextDataRow)(OBJ item,const char* data,const char* rowname);
/*! 
 \brief get the values in the given column in the given text data table of the given item
 \ingroup Data
*/
char** (*tc_getTextDataCol)(OBJ item,const char* data,const char* colname);
/*! 
 \brief get all the numeric data table names for the given item. Use 0 for the global tables.
 \ingroup Data
*/
char** (*tc_getNumericalDataNames)(OBJ);
/*! 
 \brief get all the text data table names for the given item. Use 0 for the global tables.
 \ingroup Data
*/
char** (*tc_getTextDataNames)(OBJ);
/*! 
 \brief zoom by the given factor (0 - 1)
 \ingroup Input and Output
*/
void (*tc_zoom)(double factor);
/*! 
 \brief initialize main
 \ingroup init
*/
void tc_Main_api_initialize(
	    Array (*tc_allItems0)(),
		Array (*tc_selectedItems0)(),
		Array (*tc_itemsOfFamily0)(const char*),
		OBJ (*tc_find0)(const char*),
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
	tc_find = tc_find0;
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
 \ingroup Input and output
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
