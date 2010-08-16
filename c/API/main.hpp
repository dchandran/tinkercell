/**
 * @mainpage TinkerCell C API
 * @brief 
 The TinkerCell C API is a collection of functions that allow C programs to directly
 interact with TinkerCell's visual interface. SWIG is used to extend this API to other languages, such
 as Python, Perl, R, etc. The functions provided in this API are coverted to Signals, which 
 are much slower than function calls. But they can be used to communicate between threads, which is the
 main reason why they are used in TinkerCell. 

 The API uses six main data structures: 
 
<Item>: just a reference to a TinkerCell object (memory pointer)

<String>: exactly what the name means

<ArrayOfItems> array of Items

<examples>
\code
         ArrayOfItems A = tc_allItems()         
    	 A.length    	 
    	 nthItem(A,3)
\endcode

<ArrayOfStrings>: array of Strings

<example>
\code
	     ArrayOfStrings S = tc_getNames( tc_allItems() )
		 S.length
		 nthString(S,3)
\endcode

<Matrix>: Two dimensional array of reals with row and column names. The rownames and colnames fields are ArrayOfString objects

<example>
\code	
	     Matrix M = tc_getNumericalData( tc_find("a"), "Numerical Attributes" )	     
		 M.rows		 
		 M.cols		 
		 getColumnName(M,2)
		 setColumnName(M,2,"col2")		 
		 getRowName(M,1)		 
		 setRowName(M,1,"row1")		 
		 getValue(M,2,3)		 
		 setValue(M,2,3,0.5)
\endcode

<TableOfStrings>: Two dimensional array of Strings with row and column names. The rownames and colnames fields are ArrayOfString objects

<example>
\code	
	     TableOfStrings S = tc_getTextData( tc_find("a"), "Text Attributes" )	     
		 S.rows		 
		 S.cols		 
		 nthString( S.rownames, 1)		 
		 nthString( S.colnames, 2)		 
		 getString(S,2,3)		 
		 setString(S,2,3,"hello")
\endcode
*/

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

/*! \defgroup Modeling Modeling
\brief get/set parameters, equations, and so on*/

/*! \defgroup Connections Connections
\brief change appearance of connection arcs*/

/*! \defgroup Antimony Antimony and SBML
\brief SBML and Antimony related functions*/
