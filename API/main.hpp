/**
 * @mainpage TinkerCell C API
 * @brief 
 The TinkerCell C API is a collection of functions that allow C programs to directly
 interact with TinkerCell's visual interface. SWIG is used to extend this API to other languages, such
 as Python, Perl, R, etc. The functions provided in this API are coverted to Signals, which 
 are much slower than function calls. But they can be used to communicate between threads, which is the
 main reason why they are used in TinkerCell.

The API uses <b>six main data structures</b>: 
 
<b>item</b>: just a reference to a TinkerCell object. Items are represented as integers in Python and Octave and as long ints in C.

<b>string</b>: a string of characters used. Represented as const char* in C.

<b>tc_items</b> array of items
\code
	tc_items A = tc_allItems()         
	A.length    	 
	tc_getItem(A,3)

	long x = tc_find("x")
	tc_setItem(A,3,x)
	tc_items A2 = tc_createItemsArray(10) //array of length 10
\endcode

<b>tc_strings</b>: array of strings
\code
	tc_items A = tc_allItems()
	tc_strings S = tc_getNames( A )
	S.length
	tc_getString(S,3)
	tc_setString(S,3,"hello")
	tc_strings S2 = tc_createStringsArray(10) //array of length 10
\endcode

<b>tc_matrix</b>: Two dimensional array of reals with row and column names. The rownames and colnames fields are tc_strings objects
\code	
	long x = tc_find("x")
	tc_matrix M = tc_getNumericalData( x, "Parameters" )	     
	int r = M.rows
	int c = M.cols
	tc_getColumnName(M,2)
	tc_setColumnName(M,2,"col2")		 
	tc_getRowName(M,1)		 
	tc_setRowName(M,1,"row1")		 
	tc_getMatrixValue(M,2,3)		 
	tc_setMatrixValue(M,2,3,0.5)

	tc_matrix M2 = tc_createMatrix(5,4)
\endcode

<b>tc_table</b>: Two dimensional array of Strings with row and column names. The rownames and colnames fields are tc_strings objects
\code	
	long x = tc_find("x")
	tc_table S = tc_getTextData( x, "Text Attributes" )	     
	S.rows		 
	S.cols		 
	tc_getString( S.rownames, 1)		 
	tc_getString( S.colnames, 2)		 
	tc_getTableValue(S,2,3)		 
	tc_setTableValue(S,2,3,"hello")
	tc_table S2 = tc_createTable(4,5)
\endcode
*/

/*! \defgroup Basic Basic operations
\brief basic functions for getting and setting matrices, arrays, tables, etc.*/

/*! \defgroup Appearance Appearance
\brief get/set position, color, size, etc*/

/*! \defgroup Get Get items
\brief get selected items or items of a family*/

/*! \defgroup Annotation Annotations
\brief get annotation information about items*/

/*! \defgroup Input Input and Output
\brief display dialogs or get user inputs*/

/*! \defgroup System System information
\brief get information about the OS and program directory*/

/*! \defgroup Network Network data 
\brief get/set information about the individual items in the network*/

/*! \defgroup Plotting Graphing
\brief display graphs, save graphs, get graph values*/

/*! \defgroup Modeling Modeling
\brief get/set parameters, equations, and so on*/

/*! \defgroup Connections Connections
\brief change appearance of connection arcs*/

/*! \defgroup Export Import/Export
\brief Import/Export different file formats*/

/*! \defgroup Simulation Simulation
\brief Simulations and other numerical analysis*/

/*! \defgroup Module  Modules
\brief Substitute submodels without affecting higher-level diagram*/

