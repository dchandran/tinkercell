#include "TC_Main_api.h"

static long _cthread_ptr = 0;

tc_items (*_tc_allItems)() = 0;
/*! 
 \brief get all visible items
 \ingroup Get items
*/ TCAPIEXPORT 
tc_items tc_allItems()
{
	if (_tc_allItems)
		return _tc_allItems();
	return tc_createItemsArray(0);
}

tc_items (*_tc_selectedItems)() = 0;
/*! 
 \brief get all selected items
 \ingroup Get items
*/ TCAPIEXPORT 
tc_items tc_selectedItems()
{
	if (_tc_selectedItems)
		return _tc_selectedItems();
	return tc_createItemsArray(0);
}

tc_items (*_tc_itemsOfFamily)(const char* family) = 0;
/*!
 \brief get all items of the given family items
 \ingroup Get items
*/ TCAPIEXPORT 
tc_items tc_itemsOfFamily(const char* family)
{
	if (_tc_itemsOfFamily)
		return _tc_itemsOfFamily(family);
	return tc_createItemsArray(0);
}

tc_items (*_tc_itemsOfFamilyFrom)(const char* family, tc_items itemsToSelectFrom) = 0;
/*! 
 \brief get subset of items that belong to the given family
 \ingroup Get items
*/ TCAPIEXPORT 
tc_items tc_itemsOfFamilyFrom(const char* family, tc_items itemsToSelectFrom)
{
	if (_tc_itemsOfFamilyFrom)
		return _tc_itemsOfFamilyFrom(family,itemsToSelectFrom);
	return tc_createItemsArray(0);
}

long (*_tc_find)(const char* fullname) = 0;
/*! 
 \brief get the first item with the given name (full name)
 \ingroup Get items
*/ TCAPIEXPORT 
long tc_find(const char* fullname)
{
	if (_tc_find)
		return _tc_find(fullname);
	return 0;
}

tc_items (*_tc_findItems)(tc_strings names) = 0;
/*! 
 \brief get all items with the given names (full names)
 \ingroup Get items
*/ TCAPIEXPORT 
tc_items tc_findItems(tc_strings names)
{
	if (_tc_findItems)
		return _tc_findItems(names);
	return tc_createItemsArray(0);
}

void (*_tc_select)(long item) = 0;
/*! 
 \brief select an item
 \ingroup Get items
*/ TCAPIEXPORT 
void tc_select(long item)
{
	if (_tc_select && item)
		_tc_select(item);
}

void (*_tc_deselect)() = 0;
/*! 
 \brief deselect all items
 \ingroup Get items
*/ TCAPIEXPORT 
void tc_deselect()
{
	if (_tc_deselect)
		_tc_deselect();
}

const char* (*_tc_getName)(long item) = 0;
/*! 
 \brief get the full name of an item
 \ingroup Annotation
*/ TCAPIEXPORT 
const char* tc_getName(long item)
{
	if (_tc_getName)
		return _tc_getName(item);
	return 0;
}

const char* (*_tc_getUniqueName)(long item) = 0;
/*! 
 \brief get the full name of an item
 \ingroup Annotation
*/ TCAPIEXPORT 
const char* tc_getUniqueName(long item)
{
	if (_tc_getUniqueName)
		return _tc_getUniqueName(item);
	return 0;
}

void (*_tc_rename)(long item,const char* name) = 0;
/*! 
 \brief set the name of an item (not full name)
 \ingroup Annotation
*/ TCAPIEXPORT 
void tc_rename(long item,const char* name)
{
	if (_tc_rename)
		_tc_rename(item,name);
}

tc_strings (*_tc_getNames)(tc_items items) = 0;
/*! 
 \brief get the full names of several items
 \ingroup Annotation
*/ TCAPIEXPORT 
tc_strings tc_getNames(tc_items items)
{
	if (_tc_getNames)
		return _tc_getNames(items);
	return tc_createStringsArray(0);
}

tc_strings (*_tc_getUniqueNames)(tc_items items) = 0;
/*! 
 \brief get the full names of several items
 \ingroup Annotation
*/ TCAPIEXPORT 
tc_strings tc_getUniqueNames(tc_items items)
{
	if (_tc_getUniqueNames)
		return _tc_getUniqueNames(items);
	return tc_createStringsArray(0);
}


const char* (*_tc_getFamily)(long item) = 0;
/*! 
 \brief get the family name of an item
 \ingroup Annotation
*/ TCAPIEXPORT 
const char* tc_getFamily(long item)
{
	if (_tc_getFamily)
		return _tc_getFamily(item);
	return 0;
}

int (*_tc_isA)(long item,const char* family) = 0;
/*! 
 \brief check is an item belongs in a family (or in a sub-family)
 \ingroup Annotation
*/ TCAPIEXPORT 
int tc_isA(long item,const char* family)
{
	if (_tc_isA)
		return _tc_isA(item,family);
	return 0;
}

void (*_tc_print)(const char* text) = 0;
/*! 
 \brief show text in the output window.
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_print(const char* text)
{
	if (_tc_print && text)
		_tc_print(text);
}

void (*_tc_errorReport)(const char* text) = 0;
/*! 
 \brief show error text in the output window.
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_errorReport(const char* text)
{
	if (_tc_errorReport && text)
		_tc_errorReport(text);
}

void (*_tc_printMatrix)(tc_matrix data) = 0;
/*! 
 \brief show table in the output window.
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_printMatrix(tc_matrix data)
{
	if (_tc_printMatrix)
		_tc_printMatrix(data);
}

void (*_tc_printFile)(const char* filename) = 0;
/*! 
 \brief show file contents in the output window. 
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_printFile(const char* filename)
{
	if (_tc_printFile)
		_tc_printFile(filename);
}

void (*_tc_clear)() = 0;
/*! 
 \brief cleat the contents in the output window. 
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_clear()
{
	if (_tc_clear)
		_tc_clear();
}

void (*_tc_remove)(long item) = 0;
/*! 
 \brief delete an item
 \ingroup Insert and remove
*/ TCAPIEXPORT 
void tc_remove(long item)
{
	if (_tc_remove)
		_tc_remove(item);
}

double (*_tc_getY)(long item) = 0;

/*! 
 \brief get the x location of an item
 \ingroup Appearance
*/ TCAPIEXPORT 
double tc_getY(long item)
{
	if (_tc_getY && item)
		return _tc_getY(item);
	return 0.0;
}

double (*_tc_getX)(long item) = 0;
/*! 
 \brief get the y location of an item
 \ingroup Appearance
*/ TCAPIEXPORT 
double tc_getX(long item)
{
	if (_tc_getX)
		return _tc_getX(item);
	return 0.0;
}

tc_matrix (*_tc_getPos)(tc_items items) = 0;
/*! 
 \brief get the y location of a list item. Output is a N x 2 matrix
 \ingroup Appearance
*/ TCAPIEXPORT 
tc_matrix tc_getPos(tc_items items)
{
	if (_tc_getPos)
		return _tc_getPos(items);
	return tc_createMatrix(0,0);
}

void (*_tc_setPos)(long item,double x,double y) = 0;
/*! 
 \brief set the x and y location of an item
 \ingroup Appearance
*/ TCAPIEXPORT 
void tc_setPos(long item,double x,double y)
{
	if (_tc_setPos && item)
		_tc_setPos(item,x,y);
}

void (*_tc_setPosMulti)(tc_items items, tc_matrix positions) = 0;
/*! 
 \brief set the x and y location of a list of N items. Input a matrix of positions, with N rows and 2 columns (x,y)
 \ingroup Appearance
*/ TCAPIEXPORT 
void tc_setPosMulti(tc_items items, tc_matrix positions)
{
	if (_tc_setPosMulti && items.length > 0 && items.items && positions.rows == items.length)
		_tc_setPosMulti(items,positions);
}

void (*_tc_moveSelected)(double dx,double dy) = 0;
/*! 
 \brief move all the selected items by a given amount
 \ingroup Appearance
*/ TCAPIEXPORT 
void tc_moveSelected(double dx,double dy)
{
	if (_tc_moveSelected)
		_tc_moveSelected(dx,dy);
}

int (*_tc_isWindows)() = 0;
/*! 
 \brief is this running in MS windows?
 \ingroup System information
*/ TCAPIEXPORT 
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
*/ TCAPIEXPORT 
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
*/ TCAPIEXPORT 
int tc_isLinux()
{
	if (_tc_isLinux)
		return _tc_isLinux();
	return 0;
}

const char* (*_tc_appDir)() = 0;
/*! 
 \brief TinkerCell application folder
 \ingroup System information
*/ TCAPIEXPORT 
const char* tc_appDir()
{
	if (_tc_appDir)
		return _tc_appDir();
	return 0;
}

const char* (*_tc_homeDir)() = 0;
/*! 
 \brief TinkerCell home folder
 \ingroup System information
*/ TCAPIEXPORT 
const char* tc_homeDir()
{
	if (_tc_homeDir)
		return _tc_homeDir();
	return 0;
}

void (*_tc_createInputWindowForScript)(tc_matrix input, const char* title,const char* functionname) = 0;
/*! 
 \brief create an input window that can call a dynamic library
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_createInputWindowForScript(tc_matrix input, const char* filename,const char* functionname)
{
	if (_tc_createInputWindowForScript)
		_tc_createInputWindowForScript(input,filename,functionname);
}

void (*_tc_createInputWindow)(long ptr, tc_matrix, const char* title, void (*f)(tc_matrix)) = 0;
/*!
 \brief create an input window that can call a dynamic library
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_createInputWindow(tc_matrix input, const char* title, void (*f)(tc_matrix))
{
	if (_tc_createInputWindow &&  _cthread_ptr)
		_tc_createInputWindow( _cthread_ptr, input,title,f);
}

void (*_tc_addInputWindowOptions)(const char*, int i, int j, tc_strings) = 0;
/*! 
 \brief add options to an existing input window at the i,j-th cell. Options will appear in a list
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_addInputWindowOptions(const char* title, int i, int j, tc_strings options)
{
	if (_tc_addInputWindowOptions)
		_tc_addInputWindowOptions(title,i,j,options);
}

void (*_tc_addInputWindowCheckbox)(const char*, int i, int j) = 0;
/*! 
 \brief add a yes or no type of option to an existing input window at the i,j-th cell
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_addInputWindowCheckbox(const char* title, int i, int j)
{
	if (_tc_addInputWindowCheckbox)
		_tc_addInputWindowCheckbox(title,i,j);
}

void (*_tc_openNewWindow)(const char* title) = 0;
/*! 
 \brief open a new graphics window
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_openNewWindow(const char* title)
{
	if (_tc_openNewWindow)
		_tc_openNewWindow(title);
}

tc_items (*_tc_getChildren)(long) = 0;
/*! 
 \brief get child items of the given item
 \ingroup Get items
*/ TCAPIEXPORT 
tc_items tc_getChildren(long o)
{
	if (_tc_getChildren)
		return _tc_getChildren(o);
	return tc_createItemsArray(0);
}

long (*_tc_getParent)(long) = 0;
/*! 
 \brief get parent item of the given item
 \ingroup Get items
*/ TCAPIEXPORT 
long tc_getParent(long o)
{
	if (_tc_getParent)
		return _tc_getParent(o);
	return 0;
}

tc_matrix (*_tc_getNumericalData)(long item,const char* data) = 0;
/*! 
 \brief get the entire data matrix for the given numerical data table of the given item
 \ingroup Data
*/ TCAPIEXPORT 
tc_matrix tc_getNumericalData(long item,const char* data)
{
	if (_tc_getNumericalData)
		return _tc_getNumericalData(item,data);
	return tc_createMatrix(0,0);
}

void (*_tc_setNumericalData)(long,const char*,tc_matrix) = 0;
/*! 
 \brief set a new data matrix for an item. Use 0 for the global model item.
 \ingroup Data
*/ TCAPIEXPORT 
void tc_setNumericalData(long o,const char* title,tc_matrix data)
{
	if (_tc_setNumericalData)
		_tc_setNumericalData(o, title, data);
}

tc_table (*_tc_getTextData)(long item,const char* data) = 0;
/*! 
 \brief get the entire data matrix for the given strings data table of the given item
 \ingroup Data
*/ TCAPIEXPORT 
tc_table tc_getTextData(long item,const char* data)
{
	if (_tc_getTextData)
		return _tc_getTextData(item,data);
	return tc_createTable(0,0);
}

void (*_tc_setTextData)(long,const char*,tc_table) = 0;
/*! 
 \brief set the entire data matrix for the given strings data table of the given item
 \ingroup Data
*/ TCAPIEXPORT 
void tc_setTextData(long o,const char* title,tc_table data)
{
	if (_tc_setTextData)
		_tc_setTextData(o, title, data);
}


tc_strings (*_tc_getNumericalDataNames)(long) = 0;
/*! 
 \brief get all the numeric data table names for the given item. Use 0 for the global tables.
 \ingroup Data
*/ TCAPIEXPORT 
tc_strings tc_getNumericalDataNames(long o)
{
	if (_tc_getNumericalDataNames)
		return _tc_getNumericalDataNames(o);
	return tc_createStringsArray(0);
}

tc_strings (*_tc_getTextDataNames)(long) = 0;
/*! 
 \brief get all the text data table names for the given item. Use 0 for the global tables.
 \ingroup Data
*/ TCAPIEXPORT 
tc_strings tc_getTextDataNames(long o)
{
	if (_tc_getTextDataNames)
		return _tc_getTextDataNames(o);
	return tc_createStringsArray(0);
}

void (*_tc_zoom)(double factor) = 0;
/*! 
 \brief zoom by the given factor (0 - 1)
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_zoom(double factor)
{
	if (_tc_zoom)
		_tc_zoom(factor);
}

const char* (*_tc_getStringDialog)(const char* title) = 0;
/*! 
 \brief get a text from the user (dialog)
 \ingroup Input and Output
*/ TCAPIEXPORT 
const char* tc_getStringDialog(const char* title)
{
	if (_tc_getStringDialog)
		return _tc_getStringDialog(title);
	return 0;
}

const char* (*_tc_getFilename)() = 0;
/*! 
 \brief get a file from the user (dialog)
 \ingroup Input and Output
*/ TCAPIEXPORT 
const char* tc_getFilename()
{
	if (_tc_getFilename)
		return _tc_getFilename();
	return 0;
}

int (*_tc_getStringFromList)(const char* title, tc_strings list,const char* selectedString) = 0;
/*! 
 \brief get a text from the user (dialog) from a list of selections
 \ingroup Input and Output
*/ TCAPIEXPORT 
int tc_getStringFromList(const char* title, tc_strings list,const char* selectedString)
{
	if (_tc_getStringFromList)
		return _tc_getStringFromList(title,list,selectedString);
	return 0;
}

double (*_tc_getNumber)(const char* title) = 0;
/*! 
 \brief get a number from the user (dialog)
 \ingroup Input and Output
*/ TCAPIEXPORT 
double tc_getNumber(const char* title)
{
	if (_tc_getNumber)
		return _tc_getNumber(title);
	return 0.0;
}

void (*_tc_getNumbers)(tc_strings labels, double* result) = 0;
/*! 
 \brief get a list of numbers from the user (dialog) into the argument array
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_getNumbers(tc_strings labels, double* result)
{
	if (_tc_getNumbers && result)
		_tc_getNumbers(labels,result);
}

int (*_tc_askQuestion)(const char*) = 0;
/*! 
 \brief display a dialog with a text and a yes and no button
 \param const char* displayed message or question
 \ingroup Input and Output
*/ TCAPIEXPORT 
int tc_askQuestion(const char* message)
{
	if (_tc_askQuestion && message)
		return _tc_askQuestion(message);
	return 0;
}

void (*_tc_messageDialog)(const char*) = 0;
/*! 
 \brief display a dialog with a text message and a close button
 \param const char* displayed message
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_messageDialog(const char* message)
{
	if (_tc_messageDialog && message)
		_tc_messageDialog(message);
}

void (*_tc_openFile)(const char*) = 0;
/*! 
 \brief open file
 \param const char* file
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_openFile(const char* message)
{
	if (_tc_openFile && message)
		_tc_openFile(message);
}

void (*_tc_saveToFile)(const char*) = 0;
/*! 
 \brief save to file
 \param const char* file
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_saveToFile(const char* message)
{
	if (_tc_saveToFile && message)
		_tc_saveToFile(message);
}

/*!
 \brief get pointer to the current thread
 \ingroup Programming interface
*/ TCAPIEXPORT 
long tc_thisThread()
{
	return _cthread_ptr;
}


void (*_tc_createSliders)(long, tc_matrix, void (*f)(tc_matrix)) = 0;
/*!
 \brief create a window with several sliders. when the sliders change, the given function will be called with the values in the sliders
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_createSliders(tc_matrix input, void (*f)(tc_matrix))
{
	if (_tc_createSliders && _cthread_ptr)
		_tc_createSliders(_cthread_ptr, input,f);
}

void (*_tc_setSize)(long,double,double,int) = 0;
/*!
 \brief Change the size of an item
 \ingroup Appearance
*/ TCAPIEXPORT 
void tc_setSize(long item,double width,double height)
{
	if (_tc_setSize)
		_tc_setSize(item,width,height,1);
}

double (*_tc_getWidth)(long) = 0;
/*!
 \brief get the width of an item
 \ingroup Appearance
*/ TCAPIEXPORT 
double tc_getWidth(long item)
{
	if (_tc_getWidth)
		return _tc_getWidth(item);
	return 1.0;
}

double (*_tc_getHeight)(long) = 0;
/*!
 \brief get the width of an item
 \ingroup Appearance
*/ TCAPIEXPORT 
double tc_getHeight(long item)
{
	if (_tc_getHeight)
		return _tc_getHeight(item);
	return 1.0;
}

void (*_tc_setAngle)(long,double,int) = 0;
/*!
 \brief get the width of an item
 \ingroup Appearance
*/ TCAPIEXPORT 
void tc_rotate(long item, double t)
{
	if (_tc_setAngle)
		_tc_setAngle(item,t, 1);
}

const char* (*_tc_getColor)(long item) = 0;
/*! 
 \brief get the color of the item
 \ingroup Appearance
*/ TCAPIEXPORT 
const char* tc_getColor(long item)
{
	if (_tc_getColor)
		return _tc_getColor(item);
	return "#000000";
}

void (*_tc_setColor)(long item,const char* name, int permanent) = 0;
/*! 
 \brief set the color of the item and indicate whether or not the color is permanenet
 \ingroup Appearance
*/ TCAPIEXPORT 
void tc_setColor(long item,const char* name, int permanent)
{
	if (_tc_setColor)
		_tc_setColor(item,name,permanent);
}

void (*_tc_changeNodeImage)(long,const char*) = 0;
/*! 
 \brief change the graphics file for drawing one of the nodes
 \ingroup Appearance
*/ TCAPIEXPORT 
void tc_changeNodeImage(long item,const char* filename)
{
	if (_tc_changeNodeImage)
		_tc_changeNodeImage(item,filename);
}

void (*_tc_changeArrowHead)(long,const char*) = 0;
/*! 
 \brief change the graphics file for drawing the arrowheads for the given connection
 \ingroup Appearance
*/ TCAPIEXPORT 
void tc_changeArrowHead(long connection,const char* filename)
{
	if (_tc_changeArrowHead)
		_tc_changeArrowHead(connection,filename);
}

void (*_tc_screenshot)(const char * filename, int width, int height) = 0;
/*!
 \brief save screenshot in a file
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_screenshot(const char * filename, int width, int height)
{
	if (_tc_screenshot)
		_tc_screenshot(filename, width, height);
}

int (*_tc_screenWidth)(void) = 0;
/*!
 \brief get width of current canvas
*/ TCAPIEXPORT 
int tc_screenWidth()
{
	if (_tc_screenWidth)
		return _tc_screenWidth();
	return 0;
}

int (*_tc_screenHeight)(void) = 0;
/*!
 \brief get height of current canvas
*/ TCAPIEXPORT 
int tc_screenHeight()
{
	if (_tc_screenHeight)
		return _tc_screenHeight();
	return 0;
}

int (*_tc_screenX)(void) = 0;
/*!
 \brief get x of current canvas
*/ TCAPIEXPORT 
int tc_screenX()
{
	if (_tc_screenX)
		return _tc_screenX();
	return 0;
}

int (*_tc_screenY)(void) = 0;
/*!
 \brief get y of current canvas
*/ TCAPIEXPORT 
int tc_screenY()
{
	if (_tc_screenY)
		return _tc_screenY();
	return 0;
}

const char * (*_tc_annotations)() = 0;
/*! 
 \brief get text displayed on the canvas
*/ TCAPIEXPORT 
const char * tc_annotations()
{
	if (_tc_annotations)
		return _tc_annotations();
	return "";
}

void (*_tc_insertAnnotations)(const char *, double, double) = 0;
/*! 
 \brief show text displayed on the canvas at the given position
*/ TCAPIEXPORT
void tc_insertAnnotations(const char * s, double x, double y)
{
	if (_tc_insertAnnotations)
		_tc_insertAnnotations(s,x,y);
}

/*! 
 \brief initialize main
 \ingroup init
*/ TCAPIEXPORT 
void tc_Main_api_initialize(
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

		void (*tc_createInputWindow0)(tc_matrix,const char*,const char*),
        void (*tc_createInputWindow1)(long ptr, tc_matrix, const char*, void (*f)(tc_matrix)),
		void (*createSliders0)(long, tc_matrix, void (*f)(tc_matrix)),
		
		void (*tc_addInputWindowOptions0)(const char*, int i, int j, tc_strings),
		void (*tc_addInputWindowCheckbox0)(const char*, int i, int j),
		void (*tc_openNewWindow0)(const const char* title),
		
		tc_items (*tc_getChildren0)(long),
		long (*tc_getParent0)(long),
		
		tc_matrix (*tc_getNumericalData0)(long,const char*),
		void (*tc_setNumericalData0)(long,const char*,tc_matrix),
		tc_table (*tc_getTextData0)(long,const char*),
		void (*tc_setTextData0)(long,const char*, tc_table),
				
		tc_strings (*tc_getNumericalDataNames0)(long),
		tc_strings (*tc_getTextDataNames0)(long),
		
		void (*tc_zoom0)(double factor),
		
		const char* (*tc_getString0)(const char*),
		int (*getSelectedString0)(const char*, tc_strings, const char*),
		double (*getNumber0)(const char*),
		void (*getNumbers0)( tc_strings, double * ),
		const char* (*getFilename0)(),
		
		int (*askQuestion0)(const char*),
		void (*messageDialog0)(const char*),
		void (*openFile0)(const char*),
		void (*saveToFile0)(const char*),
		
		void (*setSize0)(long,double,double,int),
		double (*getWidth0)(long),
		double (*getHeight0)(long),
		void (*setAngle0)(long,double,int),
		const char* (*getColor0)(long),
		void (*setColor0)(long,const char*,int),
		
		void (*changeGraphics0)(long,const char*),
		void (*changeArrowHead0)(long,const char*),
		
		void (*screenshot)(const char*, int, int),
		int (*screenWidth)(),
		int (*screenHeight)(),
		int (*screenX)(),
		int (*screenY)(),
		
		const char * (*annotations)(),
		void (*insertAnnotations)(const char *, double, double)
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
	
	_tc_getUniqueName = tc_getUniqueName0;
	_tc_getUniqueNames = tc_getUniqueNames0;
	
	_tc_getNames = tc_getNames0;
	_tc_getFamily = tc_getFamily0;
	_tc_isA = tc_isA0;

	_tc_clear = tc_clearText;
	_tc_print = tc_outputText0;
	_tc_errorReport = tc_errorReport0;
	_tc_printMatrix = tc_outputTable0;
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
	_tc_homeDir = tc_homeDir0;
	
    _tc_createInputWindow = tc_createInputWindow1;
    _tc_createInputWindowForScript = tc_createInputWindow0;
	_tc_addInputWindowOptions = tc_addInputWindowOptions0;
	_tc_addInputWindowCheckbox = tc_addInputWindowCheckbox0;
	
	_tc_openNewWindow = tc_openNewWindow0;
	
	_tc_getNumericalData = tc_getNumericalData0;
	_tc_getTextData = tc_getTextData0;
	_tc_setNumericalData = tc_setNumericalData0;
	_tc_setTextData = tc_setTextData0;
	_tc_getChildren = tc_getChildren0;
	_tc_getParent = tc_getParent0;

	_tc_getNumericalDataNames = tc_getNumericalDataNames0;
	_tc_getTextDataNames = tc_getTextDataNames0;
	
	_tc_zoom = tc_zoom0;
	
	_tc_getStringDialog = tc_getString0;
	_tc_getStringFromList = getSelectedString0;
	_tc_getNumber = getNumber0;
	_tc_getNumbers = getNumbers0;
	_tc_getFilename = getFilename0;
	
	_tc_askQuestion = askQuestion0;
	_tc_messageDialog = messageDialog0;
	_tc_openFile = openFile0;
	_tc_saveToFile = saveToFile0;
	
	_tc_createSliders = createSliders0;
	
	_tc_setSize = setSize0;
	_tc_getWidth = getWidth0;
	_tc_getHeight = getHeight0;
	_tc_setAngle = setAngle0;
	_tc_getColor = getColor0;
	_tc_setColor = setColor0;
	
	_tc_changeNodeImage = changeGraphics0;
	_tc_changeArrowHead = changeArrowHead0;
	
	_tc_screenshot = screenshot;
	_tc_screenWidth = screenWidth;
	_tc_screenHeight = screenHeight;
	
	_tc_screenX = screenX;
	_tc_screenY = screenY;
	
	_tc_annotations = annotations;
	_tc_insertAnnotations = insertAnnotations;
}

void (*_tc_showProgress)(long thread, int progress) = 0;
/*! 
 \brief show progress of current operation
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_showProgress(int progress)
{
	if (_tc_showProgress && _cthread_ptr)
		_tc_showProgress(_cthread_ptr,progress);
}

void (*_tc_setProgessBarTitle)(long thread, const char * s) = 0;
/*! 
 \brief set title for progress bar
 \ingroup Input and Output
*/ TCAPIEXPORT 
void tc_setProgessBarTitle(const char * s)
{
	if (_tc_setProgessBarTitle && _cthread_ptr)
		_tc_setProgessBarTitle(_cthread_ptr,s);
}

void (*_tc_callback)(long, void (*f)(void)) = 0;
/*! 
 \brief this function will be called whenever the model is changed
 \param void* callback function pointer
 \ingroup Programming
*/ TCAPIEXPORT 
void tc_callback(void (*f)(void))
{
	if (_tc_callback && _cthread_ptr)
		_tc_callback(_cthread_ptr, f);
}

void (*_tc_callWhenExiting)(long, void (*f)(void)) = 0;
/*! 
 \brief this function will be called whenever Tinkercell exits. Use it to free memory.
 \param void* callback function pointer
 \ingroup Programming
*/ TCAPIEXPORT 
void tc_callWhenExiting(void (*f)(void))
{
	if (_tc_callWhenExiting && _cthread_ptr)
		_tc_callWhenExiting(_cthread_ptr, f);
}

/*! 
 \brief initialize main
 \ingroup init
*/ TCAPIEXPORT 
void tc_CThread_api_initialize(
	long cthread,
	void (*callback)(long, void (*f)(void)),
	void (*callWhenExiting)(long, void (*f)(void)),
	void (*showProgress)(long, int),
	void (*setTitle)(long,const char*))
{
	_tc_showProgress = showProgress;
	_tc_callback = callback;
	_tc_callWhenExiting = callWhenExiting;
	_tc_setProgessBarTitle = setTitle;
	_cthread_ptr = cthread;
}


