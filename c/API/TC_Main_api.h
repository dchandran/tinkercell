#ifndef TINKERCELL_TC_MAIN_API_H
#define TINKERCELL_TC_MAIN_API_H

#include "../TCstructs.h"

Array (*_tc_allItems)() = 0;
/*! 
 \brief get all visible items
 \ingroup Get items
*/
Array tc_allItems()
{
	if (_tc_allItems)
		return _tc_allItems();
	return 0;
}

Array (*_tc_selectedItems)() = 0;
/*! 
 \brief get all selected items
 \ingroup Get items
*/
Array tc_selectedItems()
{
	if (_tc_selectedItems)
		return _tc_selectedItems();
	return 0;
}

Array (*_tc_itemsOfFamily)(const char* family) = 0;
/*!
 \brief get all items of the given family items
 \ingroup Get items
*/
Array tc_itemsOfFamily(const char* family)
{
	if (_tc_itemsOfFamily)
		return _tc_itemsOfFamily(family);
	return 0;
}

Array (*_tc_itemsOfFamilyFrom)(const char* family, Array itemsToSelectFrom) = 0;
/*! 
 \brief get subset of items that belong to the given family
 \ingroup Get items
*/
Array tc_itemsOfFamilyFrom(const char* family, Array itemsToSelectFrom)
{
	if (_tc_itemsOfFamilyFrom)
		return _tc_itemsOfFamilyFrom(family,itemsToSelectFrom);
	return 0;
}

OBJ (*_tc_find)(const char* fullname) = 0;
/*! 
 \brief get the first item with the given name (full name)
 \ingroup Get items
*/
OBJ tc_find(const char* fullname)
{
	if (_tc_find)
		return _tc_find(fullname);
	return 0;
}

Array (*_tc_findItems)(char** names) = 0;
/*! 
 \brief get all items with the given names (full names)
 \ingroup Get items
*/
Array tc_findItems(char** names)
{
	if (_tc_findItems)
		return _tc_findItems(names);
	return 0;
}

void (*_tc_select)(OBJ item) = 0;
/*! 
 \brief select an item
 \ingroup Get items
*/
void tc_select(OBJ item)
{
	if (_tc_select && item)
		return _tc_select(item);
}

void (*_tc_deselect)() = 0;
/*! 
 \brief deselect all items
 \ingroup Get items
*/
void tc_deselect()
{
	if (_tc_deselect)
		return _tc_deselect();
}

char* (*_tc_getName)(OBJ item) = 0;
/*! 
 \brief get the full name of an item
 \ingroup Annotation
*/
char* tc_getName(OBJ item)
{
	if (_tc_getName)
		return _tc_getName(item);
	return 0;
}

void (*_tc_rename)(OBJ item,const char* name) = 0;
/*! 
 \brief set the name of an item (not full name)
 \ingroup Annotation
*/
void tc_rename(OBJ item,const char* name)
{
	if (_tc_rename)
		return _tc_rename(item,name);
}

char** (*_tc_getNames)(Array items) = 0;
/*! 
 \brief get the full names of several items
 \ingroup Annotation
*/
char** tc_getNames(Array items)
{
	if (_tc_getNames)
		return _tc_getNames(items);
	return 0;
}

char* (*_tc_getFamily)(OBJ item) = 0;
/*! 
 \brief get the family name of an item
 \ingroup Annotation
*/
char* tc_getFamily(OBJ item)
{
	if (_tc_getFamily)
		return _tc_getFamily(item);
	return 0;
}

int (*_tc_isA)(OBJ item,const char* family) = 0;
/*! 
 \brief check is an item belongs in a family (or in a sub-family)
 \ingroup Annotation
*/
int tc_isA(OBJ item,const char* family)
{
	if (_tc_isA)
		return _tc_isA(item,family);
	return 0;
}

void (*_tc_print)(const char* text) = 0;
/*! 
 \brief show text in the output window.
 \ingroup Input and Output
*/
void tc_print(const char* text)
{
	if (_tc_print && text)
		return _tc_print(text);
}

void (*_tc_errorReport)(const char* text) = 0;
/*! 
 \brief show error text in the output window.
 \ingroup Input and Output
*/
void tc_errorReport(const char* text)
{
	if (_tc_errorReport && text)
		_tc_errorReport(text);
}

void (*_tc_printTable)(Matrix data) = 0;
/*! 
 \brief show table in the output window.
 \ingroup Input and Output
*/
void tc_printTable(Matrix data)
{
	if (_tc_printTable)
		_tc_printTable(data);
}

void (*_tc_printFile)(const char* filename) = 0;
/*! 
 \brief show file contents in the output window. 
 \ingroup Input and Output
*/
void tc_printFile(const char* filename)
{
	if (_tc_printFile)
		_tc_printFile(filename);
}

void (*_tc_clear)() = 0;
/*! 
 \brief cleat the contents in the output window. 
 \ingroup Input and Output
*/
void tc_clear()
{
	if (_tc_clear)
		_tc_clear();
}

void (*_tc_remove)(OBJ item) = 0;
/*! 
 \brief delete an item
 \ingroup Insert and remove
*/
void tc_remove(OBJ item)
{
	if (_tc_remove)
		_tc_remove(item);
}

double (*_tc_getY)(OBJ item) = 0;

/*! 
 \brief get the x location of an item
 \ingroup Get and set position
*/
double tc_getY(OBJ item)
{
	if (_tc_getY && item)
		return _tc_getY(item);
	return 0.0;
}

double (*_tc_getX)(OBJ item) = 0;
/*! 
 \brief get the y location of an item
 \ingroup Get and set position
*/
double tc_getX(OBJ item)
{
	if (_tc_getX)
		return _tc_getX(item);
	return 0.0;
}

Matrix (*_tc_getPos)(Array items) = 0;
/*! 
 \brief get the y location of a list item. Output is a N x 2 matrix
 \ingroup Get and set position
*/
Matrix tc_getPos(Array items)
{
	if (_tc_getPos)
		return _tc_getPos(items);
	Matrix M;
	M.rows = M.cols = 0;
	M.rownames = M.colnames = 0;
	return M;
}

void (*_tc_setPos)(OBJ item,double x,double y) = 0;
/*! 
 \brief set the x and y location of an item
 \ingroup Get and set position
*/
void tc_setPos(OBJ item,double x,double y)
{
	if (_tc_setPos && item)
		_tc_setPos(item,x,y);
}

void (*_tc_setPosMulti)(Array items, Matrix positions) = 0;
/*! 
 \brief set the x and y location of a list of N items. Input a matrix of positions, with N rows and 2 columns (x,y)
 \ingroup Get and set position
*/
void tc_setPosMulti(Array items, Matrix positions)
{
	if (_tc_setPosMulti && items)
		_tc_setPosMulti(items,positions);
}

void (*_tc_moveSelected)(double dx,double dy) = 0;
/*! 
 \brief move all the selected items by a given amount
 \ingroup Get and set position
*/
void tc_moveSelected(double dx,double dy)
{
	if (_tc_moveSelected)
		_tc_moveSelected(dx,dy);
}

int (*_tc_isWindows)() = 0;
/*! 
 \brief is this running in MS windows?
 \ingroup System information
*/
int tc_isWindows()
{
	if (_tc_isWindows)
		return _tc_isWindows();
	return 0;
}

int (*_tc_isMac)() = 0;
/*! 
 \brief is this running in a Mac?
 \ingroup System information
*/
int tc_isMac()
{
	if (_tc_isMac)
		return _tc_isMac();
	return 0;
}

int (*_tc_isLinux)() = 0;
/*! 
 \brief is this running in Linux?
 \ingroup System information
*/
int tc_isLinux()
{
	if (_tc_isLinux)
		return _tc_isLinux();
	return 0;
}

char* (*_tc_appDir)() = 0;
/*! 
 \brief TinkerCell application folder
 \ingroup System information
*/
char* tc_appDir()
{
	if (_tc_appDir)
		return _tc_appDir();
	return 0;
}

void (*_tc_createInputWindowFromFile)(Matrix input, const char* filename,const char* functionname, const char* title) = 0;
/*! 
 \brief create an input window that can call a dynamic library
 \ingroup Input and Output
*/
void tc_createInputWindowFromFile(Matrix input, const char* filename,const char* functionname, const char* title)
{
	if (_tc_createInputWindowFromFile)
		_tc_createInputWindowFromFile(input,filename,functionname,title);
}

void (*_tc_createInputWindow)(Matrix, const char* title, void (*f)(Matrix)) = 0;
/*!
 \brief create an input window that can call a dynamic library
 \ingroup Input and Output
*/
void tc_createInputWindow(Matrix input, const char* title, void (*f)(Matrix))
{
	if (_tc_createInputWindow)
		_tc_createInputWindow(input,title,f);
}

void (*_tc_addInputWindowOptions)(const char*, int i, int j, char **) = 0;
/*! 
 \brief add options to an existing input window at the i,j-th cell. Options will appear in a list
 \ingroup Input and Output
*/
void tc_addInputWindowOptions(const char* title, int i, int j, char ** options)
{
	if (_tc_addInputWindowOptions)
		_tc_addInputWindowOptions(title,i,j,options);
}

void (*_tc_addInputWindowCheckbox)(const char*, int i, int j) = 0;
/*! 
 \brief add a yes or no type of option to an existing input window at the i,j-th cell
 \ingroup Input and Output
*/
void tc_addInputWindowCheckbox(const char * title, int i, int j)
{
	if (_tc_addInputWindowCheckbox)
		_tc_addInputWindowCheckbox(title,i,j);
}

void (*_tc_openNewWindow)(const char * title) = 0;
/*! 
 \brief open a new graphics window
 \ingroup Input and Output
*/
void tc_openNewWindow(const char * title)
{
	if (_tc_openNewWindow)
		_tc_openNewWindow(title);
}

double (*_tc_getNumericalData)(OBJ item,const char* data, const char* row_name, const char* column_name) = 0;
/*! 
 \brief get numerical data belonging to a specific tool, e.g. Numerical Attributes
 \ingroup Data
*/
double tc_getNumericalData(OBJ item,const char* data, const char* row_name, const char* column_name)
{
	if (_tc_getNumericalData)
		return _tc_getNumericalData(item,data,row_name,column_name);
	return 0.0;
}

char* (*_tc_getTextData)(OBJ item,const char* data, const char* row_name, const char* column_name) = 0;
/*! 
 \brief get text data belonging to a specific tool, e.g. Text Attributes
 \ingroup Data
*/
char* tc_getTextData(OBJ item,const char* data, const char* row_name, const char* column_name)
{
	if (_tc_getTextData)
		return _tc_getTextData(item,data,row_name,column_name);
	return 0;
}

void (*_tc_setNumericalData)(OBJ item,const char* data, const char* row_name, const char* column_name,double value) = 0;
/*!
 \brief set numerical data belonging to a specific tool, e.g. Numerical Attributes
 \ingroup Data
*/
void tc_setNumericalData(OBJ item,const char* data, const char* row_name, const char* column_name,double value)
{
	if (_tc_setNumericalData)
		_tc_setNumericalData(item,data,row_name,column_name,value);
}

void (*_tc_setTextData)(OBJ item,const char* data, const char* row_name, const char* column_name,const char* text) = 0;
/*! 
 \brief set text data belonging to a specific tool, e.g. Text Attributes
 \ingroup Data
*/
void tc_setTextData(OBJ item,const char* data, const char* row_name, const char* column_name,const char* text)
{
	if (_tc_setTextData)
		_tc_setTextData(item,data,row_name,column_name,text);
}

Array (*_tc_getChildren)(OBJ) = 0;
/*! 
 \brief get child items of the given item
 \ingroup Get items
*/
Array tc_getChildren(OBJ o)
{
	if (_tc_getChildren)
		return _tc_getChildren(o);
	return 0;
}

OBJ (*_tc_getParent)(OBJ) = 0;
/*! 
 \brief get parent item of the given item
 \ingroup Get items
*/
OBJ tc_getParent(OBJ o)
{
	if (_tc_getParent)
		return _tc_getParent(o);
	return 0;
}

char** (*_tc_getNumericalDataRowNames)(OBJ item,const char* data) = 0;
/*! 
 \brief get the row names for the given numerical data table of the given item
 \ingroup Data
*/
char** tc_getNumericalDataRowNames(OBJ item,const char* data)
{
	if (_tc_getNumericalDataRowNames)
		return _tc_getNumericalDataRowNames(item,data);
	return 0;
}

char** (*_tc_getNumericalDataColNames)(OBJ item,const char* data) = 0;
/*! 
 \brief get the column names for the given numerical data table of the given item
 \ingroup Data
*/
char** tc_getNumericalDataColNames(OBJ item,const char* data)
{
	if (_tc_getNumericalDataColNames)
		return _tc_getNumericalDataColNames(item,data);
	return 0;
}

char** (*_tc_getTextDataRowNames)(OBJ item,const char* data) = 0;
/*!
 \brief get the row names for the given text data table of the given item
 \ingroup Data
*/
char** tc_getTextDataRowNames(OBJ item,const char* data)
{
	if (_tc_getTextDataRowNames)
		return _tc_getTextDataRowNames(item,data);
	return 0;
}

char** (*_tc_getTextDataColNames)(OBJ item,const char* data) = 0;
/*! 
 \brief get the column names for the given text data table of the given item
 \ingroup Data
*/
char** tc_getTextDataColNames(OBJ item,const char* data)
{
	if (_tc_getTextDataColNames)
		return _tc_getTextDataColNames(item,data);
	return 0;
}

Matrix (*_tc_getNumericalDataMatrix)(OBJ item,const char* data) = 0;
/*! 
 \brief get the entire data matrix for the given numerical data table of the given item
 \ingroup Data
*/
Matrix tc_getNumericalDataMatrix(OBJ item,const char* data)
{
	if (_tc_getNumericalDataMatrix)
		return _tc_getNumericalDataMatrix(item,data);
	Matrix M;
	M.rows = M.cols = 0;
	M.rownames = M.colnames = 0;
	M.values = 0;
	return M;
}

void (*_tc_setNumericalDataMatrix)(OBJ,const char*,Matrix) = 0;
/*! 
 \brief set a new data matrix for an item. Use 0 for the global model item.
 \ingroup Data
*/
void tc_setNumericalDataMatrix(OBJ o,const char* title,Matrix data)
{
	if (_tc_setNumericalDataMatrix)
		_tc_setNumericalDataMatrix(o, title, data);
}

char** (*_tc_getTextDataRow)(OBJ item,const char* data,const char* rowname) = 0;
/*! 
 \brief get the values in the given row in the given text data table of the given item
 \ingroup Data
*/
char** tc_getTextDataRow(OBJ item,const char* data,const char* rowname)
{
	if (_tc_getTextDataRow)
		return _tc_getTextDataRow(item,data,rowname);
	return 0;
}

char** (*_tc_getTextDataCol)(OBJ item,const char* data,const char* colname) = 0;
/*! 
 \brief get the values in the given column in the given text data table of the given item
 \ingroup Data
*/
char** tc_getTextDataCol(OBJ item,const char* data,const char* colname)
{
	if (_tc_getTextDataCol)
		return _tc_getTextDataCol(item,data,colname);
	return 0;
}

char** (*_tc_getNumericalDataNames)(OBJ) = 0;
/*! 
 \brief get all the numeric data table names for the given item. Use 0 for the global tables.
 \ingroup Data
*/
char** tc_getNumericalDataNames(OBJ o)
{
	if (_tc_getNumericalDataNames)
		return _tc_getNumericalDataNames(o);
	return 0;
}

char** (*_tc_getTextDataNames)(OBJ) = 0;
/*! 
 \brief get all the text data table names for the given item. Use 0 for the global tables.
 \ingroup Data
*/
char** tc_getTextDataNames(OBJ o)
{
	if (_tc_getTextDataNames)
		return _tc_getTextDataNames(o);
	return 0;
}

void (*_tc_zoom)(double factor) = 0;
/*! 
 \brief zoom by the given factor (0 - 1)
 \ingroup Input and Output
*/
void tc_zoom(double factor)
{
	if (_tc_zoom)
		_tc_zoom(factor);
}

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
		void (*tc_addInputWindowCheckbox0)(const char*, int i, int j),
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
	_tc_allItems = tc_allItems0;
	_tc_selectedItems = tc_selectedItems0; 
	_tc_itemsOfFamily = tc_itemsOfFamily0;
	_tc_itemsOfFamilyFrom = tc_itemsOfFamily1;
	_tc_find = tc_find0;
	_tc_findItems = tc_findItems0;
	_tc_select = tc_select0;
	_tc_deselect = tc_deselect0;
	_tc_getName = tc_getName0;
	_tc_rename = tc_setName0;
	
	_tc_getNames = tc_getNames0;
	_tc_getFamily = tc_getFamily0;
	_tc_isA = tc_isA0;

	_tc_clear = tc_clearText;
	_tc_print = tc_outputText0;
	_tc_errorReport = tc_errorReport0;
	_tc_printTable = tc_outputTable0;
	_tc_printFile = tc_printFile0;

	_tc_remove = tc_removeItem0;

	_tc_getY = tc_getY0;
	_tc_getX = tc_getX0;
	_tc_getPos = tc_getPos0;
	_tc_setPos = tc_setPos0;
	_tc_setPosMulti = tc_setPos1;
	_tc_moveSelected = tc_moveSelected0;

	_tc_isWindows = tc_isWindows0;
	_tc_isMac = tc_isMac0;
	_tc_isLinux = tc_isLinux0;
	_tc_appDir = tc_appDir0;
    _tc_createInputWindow = tc_createInputWindow1;
    _tc_createInputWindowFromFile = tc_createInputWindow0;
	_tc_addInputWindowOptions = tc_addInputWindowOptions0;
	_tc_addInputWindowCheckbox = tc_addInputWindowCheckbox0;
	
	_tc_openNewWindow = tc_openNewWindow0;
	
	_tc_getNumericalData = tc_getNumericalData0;
	_tc_getTextData = tc_getTextData0;
	_tc_setNumericalData = tc_setNumericalData0;
	_tc_setTextData = tc_setTextData0;
	_tc_getChildren = tc_getChildren0;
	_tc_getParent = tc_getParent0;
	
	_tc_getNumericalDataRowNames = tc_getNumericalDataRows0;
	_tc_getNumericalDataColNames = tc_getNumericalDataCols0;
	_tc_getTextDataRowNames = tc_getTextDataRows0;
	_tc_getTextDataColNames = tc_getTextDataCols0;
	
	_tc_getNumericalDataMatrix = tc_getNumericalDataMatrix0;
	_tc_setNumericalDataMatrix = tc_setNumericalDataMatrix0;
	_tc_getTextDataRow = tc_getTextDataRow0;
	_tc_getTextDataCol = tc_getTextDataCol0;
	
	_tc_getNumericalDataNames = tc_getNumericalDataNames0;
	_tc_getTextDataNames = tc_getTextDataNames0;
	
	_tc_zoom = tc_zoom0;
}

void (*_tc_showProgress)(const char * name, int progress);
/*! 
 \brief show progress of current operation. provide name of this library file.
 \ingroup Input and Output
*/
void tc_showProgress(const char * name, int progress)
{
	if (_tc_showProgress)
		_tc_showProgress(name,progress);
}
/*! 
 \brief initialize main
 \ingroup init
*/
void tc_Progress_api_initialize( void (*tc_showProgress0)(const char * , int) )
{
	_tc_showProgress = tc_showProgress0;
}

#endif
