#include "TC_Main_py.c"
#include "TC_NodeInsertion_py.c"
#include "TC_ConnectionInsertion_py.c"
#include "TC_ConnectionSelection_py.c"
#include "TC_BasicInformationTool_py.c"
#include "TC_DynamicLibraryTool_py.c"
#include "TC_GroupHandlerTool_py.c"
#include "TC_BasicGraphicsToolbox_py.c"
#include "TC_PlotTool_py.c"
#include "TC_ModelFileGenerator_py.c"
#include "TC_StoichiometryTool_py.c"
#include "TC_EventsAssignments_py.c"
#include "TC_AutoGeneRegulatoryTool_py.c"
#include "TC_Antimony_py.c"

int printOutput = 1;
int printError = 1;

static PyObject * outputOn(PyObject *self, PyObject *args) 
{ 
	printOutput = 1; 
	Py_INCREF(Py_None);
	return Py_None;	
}
static PyObject * outputOff(PyObject *self, PyObject *args) 
{ 
	printOutput = 0; 
	Py_INCREF(Py_None);
	return Py_None;	
}
static PyObject * errorOn(PyObject *self, PyObject *args) 
{ 
	printError = 1; 
	Py_INCREF(Py_None);
	return Py_None;	
}
static PyObject * errorOff(PyObject *self, PyObject *args) 
{ 
	printError = 0; 
	Py_INCREF(Py_None);
	return Py_None;	
}

static PyMethodDef pytcMethods[] = {
//misc
	{"zoom",pytc_zoom, METH_VARARGS, "zoom by a factor. example: zoom(1.2);"},
	{"showProgress", pytc_showProgress, METH_VARARGS, "show progress in percent (0-100). example: showProgress(50);"},
	{"clear", pytc_clearScreen, METH_VARARGS, "clear all text. example: clear();"},
	{"write", pytc_print, METH_VARARGS, "print text. example: write(\"hello world\")"},
	{"writeFile", pytc_writeFile, METH_VARARGS, "print text contents of a file. example: write(\"myfile.txt\")"},
	{"errorReport", pytc_errorReport, METH_VARARGS, "print error report. example: errorReport(\"error here\")"},
	{"createInputWindow", pytc_createInputWindow, METH_VARARGS, "create an input window for running another library. example: createInputWindow((\"model\",\"type\",\"time\",\"dt\"),(0,0,100,0.1),\"dlls/mydll\",\"main\",\"My Program\");"},
	{"addInputWindowOptions", pytc_addInputWindowOptions, METH_VARARGS, "create an input window for running another library. example: addInputWindowOptions(\"My Program\",0,0,(\"all\",\"selected\");"},
	{"openNewWindow", pytc_openNewWindow, METH_VARARGS, "open a new window or tab. example: openNewWindow();"},
	{"outputOn", outputOn, METH_VARARGS, "turn on all outputs from Python to console. example: outputOn()"},
	{"outputOff", outputOff, METH_VARARGS, "turn off all outputs from Python to console. example: outputOff()"},
	{"errorOn", errorOn, METH_VARARGS,  "turn on all error messages from Python to console. example: errorOn()"},
	{"errorOff", errorOff, METH_VARARGS,  "turn off all error messages from Python to console. example: errorOff()"},
	{"getString", pytc_getString, METH_VARARGS, "display a dialog and get a string from the user. example: getString(\"please enter text\");"},
	{"getFilename", pytc_getFilename, METH_VARARGS, "display a dialog and get a file name from the user. example: s = getFilename();"},
	{"getNumber", pytc_getNumber, METH_VARARGS, "display a dialog and get a number from the user. example: getNumber(\"please enter number\");"},
	{"getNumbers", pytc_getNumbers, METH_VARARGS, "display a dialog and get a set of numbers from the user. example: l = getNumbers(('A','B','C'));"},
	{"getFromList", pytc_getFromList, METH_VARARGS, "display a dialog and ask the user to make a selection. example: i = getFromList(\"please select one:\",[\"option 1\",\"option 1\",\"option 1\"]);"},
	{"askQuestion", pytc_askQuestion, METH_VARARGS, "display a dialog that asks the user a question. example: p = askQuestion('are you sure?');"},
	{"messageDialog", pytc_messageDialog, METH_VARARGS, "display a message dialog. example: messageDialog('the model has been updated');"},

//get items
	{"allItems", pytc_allItems, METH_VARARGS, "get addresses for all items. example: Array = allItems();"},
	{"selectedItems", pytc_selectedItems, METH_VARARGS, "get addresses for selected items. example: Array = selectedItems();"},
	{"itemsOfFamily", pytc_itemsOfFamily, METH_VARARGS, "get addresses of all items belonging to a family.  example: Array = itemsOfFamily(\"Protein\");"},
	{"find", pytc_find, METH_VARARGS, "get the address of an item using its full name. example: A = find(\"A\") or B = find(\"Cell1.A\");"},

//select and remove
	{"selectItem", pytc_select, METH_VARARGS, "select an item. example: A = find(\"A\"); selectItem(A);"},
	{"removeItem", pytc_remove, METH_VARARGS, "remove the given item. example: A = find(\"A\"); removeItem(A);"},
	{"deselect", pytc_deselect, METH_VARARGS, "deselect all selected items. example: deselectItems();"},

//item name and position
	{"getName", pytc_getName, METH_VARARGS, "get the name of an item. example: Array = selectedItems(); s = getName(Array[0]);"},
	{"rename", pytc_rename, METH_VARARGS, "set the name of an item. example: Array = selectedItems(); rename(Array[0],\"S\");"},
	{"getNames", pytc_getNames, METH_VARARGS, "get the names of given items. example: Array = selectedItems(); names = getNames(Array[0]);"},
	{"family", pytc_getFamily, METH_VARARGS, "get the family of an item. example: A = find(\"A\"); s = getFamily(A);"},
	{"isA", pytc_isA, METH_VARARGS, "checks whether the given item belongs in the given family. example: Array = find(\"A\"); i = isA(A,\"Receptor\");"},
	{"getX", pytc_getX, METH_VARARGS, "get the X coordinate of an item. example: A = find(\"A\"); x = getX(A);"},
	{"getY", pytc_getY, METH_VARARGS, "get the Y coordinate of an item. example: A = find(\"A\"); y = getY(A);"},
	{"getPos", pytc_getPos, METH_VARARGS, "get the X and Y coordinates of a set of items. example: A = (find(\"A\"),find(\"B\")); xy = getPos(A); print xy;"},
	{"setPos", pytc_setPos, METH_VARARGS, "set and X and Y coordinate of a given item. example: A = find(\"A\"); setPos(A,100,200);"},
	{"setPosMulti", pytc_setPosMulti, METH_VARARGS, "set and X and Y coordinate of severak items. example: A = (find(\"A\"),find(\"B\")); setPosMulti(A, ((100,200),(150,500)));"},
	{"moveSelected", pytc_moveSelected, METH_VARARGS, "move all selected items by the given amount in the x and y direction. example: moveSelected(100,200);"},

//os related
	{"isWindows", pytc_isWindows, METH_VARARGS, "returns 1 is this is a Windows operating system. example: i = isWindows();"},
	{"isMac", pytc_isMac, METH_VARARGS, "returns 1 is this is a Mac operating system. example: i = isMac();"},
	{"isLinux", pytc_isLinux, METH_VARARGS, "returns 1 is this is a Linux operating system. example: i = isLinux();"},
	{"appDir", pytc_appDir, METH_VARARGS, "returns the application directory. example: s = appDir()"},

//item information
	{"annotation", pytc_getAnnotation, METH_VARARGS, "get the annotation for an item, e.g. author, etc.. example: A = find(\"A\"); print annotation(A);"},
	{"setAnnotation", pytc_setAnnotation, METH_VARARGS, "set the annotation for an item, e.g. author, etc.. example: A = find(\"A\"); setAnnotation(A,(\"Mickey Mouse\",\"Apr. 27, 2009\",\"my device\",\"more about my device\",\"www.tinkercell.com\",\"ref#12345\"));"},
	{"getNumericalDataNames", pytc_getNumericalDataNames, METH_VARARGS, "get the names of all the numerical data for an item. example: A = find(\"A\"); t = getNumericalDataNames(A);"},
	{"getTextDataNames", pytc_getTextDataNames, METH_VARARGS, "get the names of all the text data for an item. example: A = find(\"A\"); t = getTextDataNames(A);"},
	{"getNumericalData", pytc_getNumericalData, METH_VARARGS, "get a numerical data. example: A = find(\"A\"); t = getNumericalData(A,\"Stoichiometry\",\"J0\",\"A\");"},
	{"getTextData", pytc_getTextData, METH_VARARGS, "get a text data. example: A = find(\"A\"); t = getTextData(A,\"Assignments\",\"f\",\"function\");"},
	{"setNumericalData", pytc_setNumericalData, METH_VARARGS, "set a numerical data. example: A = find(\"A\"); setNumericalData(A,\"Stoichiometry\",\"J0\",\"A\",-4);"},
	{"setTextData", pytc_setTextData, METH_VARARGS, "set a text data. example: A = find(\"A\"); t = setTextData(A,\"Assignments\",\"f\",\"function\",\"sin(time)\");"},
	{"getChildren", pytc_getChildren, METH_VARARGS, "get child items. example: A = find(\"A\"); children = getChildren(A);\");"},
	{"getParent", pytc_getParent, METH_VARARGS, "get child items. example: A = find(\"A\"); p = getParent(A);\");"},
	{"getNumericalDataRowNames", pytc_getNumericalDataRowNames, METH_VARARGS, "get the row names of a numerical data matrix. example: A = find(\"J0\"); getNumericalDataRowNames(A,\"Stoichiometry\");"},
	{"getNumericalDataColNames", pytc_getNumericalDataColNames, METH_VARARGS, "get the column names of a numerical data matrix. example: A = find(\"J0\"); getNumericalDataColNames(A,\"Stoichiometry\");"},
	{"getTextDataRowNames", pytc_getTextDataRowNames, METH_VARARGS, "get the row names of a text data matrix. example: A = find(\"J0\"); getTextDataRowNames(A,\"Rates\");"},
	{"getTextDataColNames", pytc_getTextDataColNames, METH_VARARGS, "get the column names of a text data matrix. example: A = find(\"J0\"); getTextDataColNames(A,\"Rates\");"},
	{"getNumericalDataMatrix", pytc_getNumericalDataMatrix, METH_VARARGS, "get an enitre numerical data matrix. Use 0 as the first argument for global matrices. example: A = find(\"J0\"); getNumericalDataMatrix(A,\"Stoichiometry\");"},
	{"setNumericalDataMatrix", pytc_setNumericalDataMatrix, METH_VARARGS, "set an enitre numerical data matrix. Use 0 as the first argument for global matrices example: A = find(\"J0\"); setNumericalDataMatrix(\"My Data\",(\"col1\",\"col2\"),(\"row1\",\"row2\",\"row3\"),((1,2,3),(4,5,6)),A); print getNumericalDataMatrix(A,\"My Data\");"},
	{"getTextDataRow", pytc_getTextDataRow, METH_VARARGS, "get the values in a particular row of a text data matrix. example: A = find(\"J0\"); getTextDataRow(A,\"Stoichiometry\",\"A\");"},
	{"getTextDataCol", pytc_getTextDataCol, METH_VARARGS, "get the values in a particular column of a text data matrix. example: A = find(\"J0\"); getTextDataCol(A,\"Stoichiometry\",\"J0\");"},

//insert
	{"insertNode", pytc_insert, METH_VARARGS, "insert a new node with the given name and family. example: insertNode(\"A\") or insertNode(\"A\",\"Receptor\");"},

	{"insertConnection", pytc_insertConnection, METH_VARARGS, "insert a connection that connects one set of items to another and has the given name and family. example: Array1 = [find(\"A\"), find(\"B\")]; Array2 = [find(\"C\")]; insertConnection(Array1,Array2,\"name\",\"Reaction\");"},
	{"getConnectedNodes", pytc_getConnectedNodes, METH_VARARGS, "get all the parts in the given connections. example: J = find(\"J0\"); Array = getConnectedParts(J);"},
	{"getConnectedNodesIn", pytc_getConnectedNodesIn, METH_VARARGS, "get all the parts in the given connections that are in-nodes, such as reactants. example: J = find(\"J0\"); Array = getConnectedPartsIn(J);"},
	{"getConnectedNodessOut", pytc_getConnectedNodesOut, METH_VARARGS, "get all the parts in the given connections that are out-nodes, such as products. example: J = find(\"J0\"); Array = getConnectedPartsOut(J);"},
	{"getConnectedNodesOther", pytc_getConnectedNodesOther, METH_VARARGS, "get all the parts in the given connections that are not in- or out-nodes. example: J = find(\"J0\"); Array = getConnectedPartsOther(J);"},
	{"getConnections", pytc_getConnections, METH_VARARGS, "get all the connections for the given part. example: A = find(\"A\"); Array = getConnections(A);"},
	{"getConnectionsIn", pytc_getConnectionsIn, METH_VARARGS, "get all the connections for the given part where the part is an in-node. example: A = find(\"A\"); Array = getConnectionsIn(A);"},
	{"getConnectionsOut", pytc_getConnectionsOut, METH_VARARGS, "get all the connections for the given part where the part is an out-node. example: A = find(\"A\"); Array = getConnectionsOut(A);"},
	{"getConnectionsOther", pytc_getConnectionsOther, METH_VARARGS, "get all the connections for the given part where the part is not an in- or out-node. example: A = find(\"A\"); Array = getConnectionsOther(A);"},

//parameters and text attributes
	{"allParameters", pytc_allParameters, METH_VARARGS, "get all the parameters [ [name1,name2..], [value1,value2..] ]for the given items, whether or not they are used in the model. example: items = allItems(); Array = allParameters(items); write(Array[0][0] + \" = \" + str(Array[1][0])); "},
	{"parameters", pytc_modelParameters, METH_VARARGS, "get all the parameters [ [name1,name2..], [value1,value2..] ] for the given items that are in use. example: items = allItems(); Array = parameters(items); write(Array[0][0] + \" = \" + str(Array[1][0])); "},
	{"initialValues", pytc_allInitialValues, METH_VARARGS, "get initial values for floating variables [ [name1,name2..], [value1,value2..] ] for the given items. example: items = allItems(); Array = initialValues(items); write(Array[0][0] + \" = \" + str(Array[1][0])); "},
	{"setInitialValues", pytc_setInitialValues, METH_VARARGS, "set initial values for floating variables [ [name1,name2..], [value1,value2..] ] in the given items. example: items = find((\"A\",\"B\")); setInitialValues(items,[0.1, 0.2]);"},
	{"parametersAndFixedVariables", pytc_allParamsAndFixedVars, METH_VARARGS, "get all the parameters and fixed variables [ [name1,name2..], [value1,value2..] ] in the given items. example: items = allItems(); Array = getParametersAndFixedVariables(items); write(Array[0][0] + \" = \" + str(Array[1][0])); "},
	{"fixedVariables", pytc_allFixedVars, METH_VARARGS, "get all the fixed variables [ [name1,name2..], [value1,value2..] ] for the given items. example: items = allItems(); Array = fixedVariables(items); write(Array[0] [0]+ \" = \" + str(Array[1][0])); "},
	{"getParameter", pytc_getParameter, METH_VARARGS, "get a specific parameter for the given items. example: A = find(\"A\"); getParameter(A,\"concentration\"); "},
	{"getTextAttribute", pytc_getTextAttribute, METH_VARARGS, "get a specific text attribute for the given items. example: A = find(\"A\"); getTextAttribute(A,\"sequence\"); "},
	{"getParametersNamed", pytc_getParametersNamed, METH_VARARGS, "get the specified parameters for the given items. example: items = allItems(); Array = getParametersNamed(items,[\"concentration\",\"copy\"]); write(Array[0][0] + \" = \" + str(Array[1][0])); "},
	{"getParametersExcept", pytc_getParametersExcept, METH_VARARGS, "get all parameters EXCEPT the ones specified for the given items. example: items = allItems(); Array = getParametersExcept(items,[\"concentration\",\"copy\"]); write(Array[0][0] + \" = \" + str(Array[1][0])); "},
	{"getAllTextNamed", pytc_getAllTextNamed, METH_VARARGS, "get the specified text attributes for the given items. example: items = allItems(); Array = getAllTextNamed(items,[\"concentration\",\"copy\"]); write(Array[0]); "},
	{"setParameter", pytc_setParameter, METH_VARARGS, "set a parameter value for the given items. example: A = find(\"A\"); setParameter(A,\"Ka\",1.2E5);"},
	{"setTextAttribute", pytc_setTextAttribute, METH_VARARGS, "set a text attribute for the given items. example: A = find(\"A\"); setTextAttribute(A,\"sequence\",\"AACGCTGAG\");"},

//connections
	{"getControlPointX", pytc_getControlPointX, METH_VARARGS, "get the X co-ordinate of a control point in a connection. example: J = find(\"J0\"); Array = getConnectedParts(J); A = Array[0]; x = getControlPointX(J,A,1);"},
	{"getControlPointY", pytc_getControlPointY, METH_VARARGS, "get the Y co-ordinate of a control point in a connection. example: J = find(\"J0\"); Array = getConnectedParts(J); A = Array[0]; y = getControlPointY(J,A,1);"},
	{"setControlPoint", pytc_setControlPoint, METH_VARARGS, "set the x,y co-ordinate of a control point in a connection. example: J = find(\"J0\"); Array = getConnectedParts(J); A = Array[0]; y = setControlPoint(J,A,1,100,100);"},
	{"setCenterPoint", pytc_setCenterPoint, METH_VARARGS, "set the x,y co-ordinate of the center point of a connection. example: J = find(\"J0\"); setCenterPoint(J,100,100);"},
	{"getCenterPointX", pytc_getCenterPointX, METH_VARARGS, "get the X co-ordinate of the center point of a connection. example: J = find(\"J0\"); x = getCenterPointX(J);"},
	{"getCenterPointY", pytc_getCenterPointY, METH_VARARGS, "get the Y co-ordinate of the center point of a connection. example: J = find(\"J0\"); y = getCenterPointY(J);"},

//dynamic library functions	
	{"compileAndRun", pytc_compileAndRun, METH_VARARGS, "compile and execute a C program. example: compileAndRun(\"mycode.c -lm\",\"arg1 arg2\"); #will make the executable for mycode.c and run it using arg1 and arg2"},
	{"compileBuildLoad", pytc_compileBuildLoad, METH_VARARGS, "build a C program as a dynamic library and load into TinkerCell. example: compileBuildLoad(\"mycode.c -lm\",\"main\"); #will call the main function in mycode.c"},
	{"callFunction", pytc_callFunction, METH_VARARGS, "execute a program that is listed in the functions menu. example: callFunction(\"Deterministic simulation\");"},
	{"loadLibrary", pytc_loadLibrary, METH_VARARGS, "load a dynamic C library. example: loadLibrary(\"mycode.dll\");"},

//basic graphics
	{"getColorR", pytc_getColorR, METH_VARARGS, "get the Red value of the color of the given object. example: A = find(\"A\"); r = getColorR(A);"},
	{"getColorG", pytc_getColorG, METH_VARARGS, "get the Green value of the color of the given object. example: A = find(\"A\"); r = getColorG(A);"},
	{"getColorB", pytc_getColorB, METH_VARARGS, "get the Blue value of the color of the given object. example: A = find(\"A\"); r = getColorB(A);"},
	{"setColor", pytc_setColor, METH_VARARGS, "set the Red,Green,Blue values for the color of the given object. The last argument indicated whether the color is permanent or termporary. example: A = find(\"A\"); setColor(A,250,100,100,1);"},
	{"setStraight", pytc_setStraight, METH_VARARGS, "toggle between straight of curved lines for drawing a connector. example: J = find(\"J\"); setStraight(J,1);"},
	{"setAllStraight", pytc_setAllStraight, METH_VARARGS, "toggle between straight of curved lines for drawing all connectors. example: setAllStraight(1);"},
	{"setLineWidth", pytc_setLineWidth, METH_VARARGS, "set the line width of a connector. The last argument indicated whether the change is permanent or termporary. example: J = find(\"J0\"); setLineWidth(J,5.0,1);"},
	{"changeNodeImage", pytc_changeNodeImage, METH_VARARGS, "change the image for a node by specifying the arrowhead graphics file. example: A = find(\"A\"); changeNodeImage(A, \"NodeItems/Molecule.xml\");"},
	{"changeArrowHead", pytc_changeArrowHead, METH_VARARGS, "change the arrowhead for a connection by specifying the arrowhead graphics file. example: J = find(\"J0\"); changeArrowHead(J, \"ArrowItems/Binding.xml\");"},
	
//plot
	{"plot", pytc_plot, METH_VARARGS, "Plot the given set of values (2D array) with the given headers and x-axis. example: data = [ [0,1,2,3,4] , [0,1,4,9,16] ]; plot([\"x\",\"y\"],data,\"my plot\");"},
	{"scatterplot", pytc_scatterplot, METH_VARARGS, "Plot the given set of values (2-column array) with the given headers. example: data = [ [0,1,2,3,4] , [0,1,4,9,16] ]; scatterplot([\"x\",\"y\"],data,\"my plot\");"},
	{"surface", pytc_surface, METH_VARARGS, "Plot 3D surface, given a matrix with 3 columns (x,y,z). example: data = [ [0,1,2,3,4] , [1,2,3,4,5] , [2,3,4,5,6] ]; surface([\"x\",\"y\",\"z\"],data,\"my plot\");"},
	{"getPlotData", pytc_getPlotData, METH_VARARGS, "Get the values and headers of the data that is displayed in the plot window. example: data = plotData(); print data;"},
	{"histogram", pytc_hist, METH_VARARGS, "plot a histogram of each column in the data. example: p = plotData(); histogram(p[0],p[1]);"},
	{"multiplot", pytc_multiplot, METH_VARARGS, "setup plot layout for multiple plot. example: multiplot(2,2);"},
	
//model generation in c and python
	{"writeModel", pytc_writeModel, METH_VARARGS, "Write the differential equations and propentity functions to a file (do not include .py suffix). example: writeModel(\"ode\",selectedItems()); or writeModel(\"ode2\"); defaults to all items"},

//stoichiometry and rates
	{"stoichiometry", pytc_getStoichiometry, METH_VARARGS, "get the column names and the linearized stoichiometry matrix for given item(s). example: items = selectedItems(); N = stoichiometry(items); N1 = stoichiometry(items[0]); "},
	{"rates", pytc_getRates, METH_VARARGS, "get the rates for multiple items. example: items = selectedItems(); v = rates(items); "},
	{"getRate", pytc_getRate, METH_VARARGS, "get the rate for a given item. example: item = find('J0'); v = getRate(item);"},
	{"setRates", pytc_setRates, METH_VARARGS, "set the rates for multiple items. example: items = ( find('J0'), find('J1') ); setRates(items,('rate1','rate2')); setRates(items[0],'rate1'); "},
	{"setRate", pytc_setRate, METH_VARARGS, "set the rate for a given item. example: item = find('J0'); setRate(item,'rate1');"},

//events and assignments
	{"eventTriggers", pytc_getEventTriggers, METH_VARARGS, "get all the event triggers. example: print eventTriggers();"},
	{"eventResponses", pytc_getEventResponses, METH_VARARGS, "get all the event responses corresponding to triggers. example: print eventResponses();"},
	{"addEvent", pytc_addEvent, METH_VARARGS, "set an event defined by a trigger and a response. example: addEvent(\"time > 10\",\"a = 0\");"},
	{"forcingFunctionNames", pytc_getForcingFunctionNames, METH_VARARGS, "get all forcing function name for given items. example: A = allItems(); forcingFunctionNames(A);"},
	{"forcingFunctionAssignments", pytc_getForcingFunctionAssignments, METH_VARARGS, "get all forcing function assignment rules for given items. example: A = allItems(); forcingFunctionAssignments(A);"},
	{"setForcingFunction", pytc_addForcingFunction, METH_VARARGS, "add or modify a function for the given item. example: a = find(\"A\"); setForcingFunction(a,\"func\",\"sin(time)*a\");"},

//display number and text
	{"highlight", pytc_highlight, METH_VARARGS, "display a circle around the given item temporarily. example: a = find(\"A\"); highlight(a); or highlight(a,255,0,0); for color"},
	{"displayText", pytc_displayText, METH_VARARGS, "display the given text on top of the given item. example: a = find(\"A\"); displayText(a,\"Hello World\");"},
	{"displayNumber", pytc_displayNumber, METH_VARARGS, "display the given number on top of the given item. example: a = find(\"A\"); displayNumber(a,3.14159);"},

//merge and separate
	{"merge", pytc_merge, METH_VARARGS, "Merge a list of items. example: a = find(\"A\"); a = find(\"B\"); merge((a,b));"},
	{"separate", pytc_separate, METH_VARARGS, "Separate an item. example: a = find(\"A\"); separate(a);"},

//genetic parts
	{"partsIn", pytc_partsIn, METH_VARARGS, "get all the DNA parts inside a Module or Compartment. example: a = find(\"A\"); A = partsIn(a); prints getNames(A);"},
	{"partsUpstream", pytc_partsUpstream, METH_VARARGS, "get all the DNA parts upstream of this part. example: a = find(\"A\"); A = partsUpstream(a); prints getNames(A);"},
	{"partsDownstream", pytc_partsDownstream, METH_VARARGS, "get all the DNA parts downstream of this part. example: a = find(\"A\"); A = partsDownstream(a); prints getNames(A);"},

//Antimony and libSBML
	{"loadSBMLString", pytc_loadSBMLString, METH_VARARGS, "load SBML model using the XML string as input. example: loadSBMLString(sbml_text)"},
	{"loadAntimonyString", pytc_loadAntimonyString, METH_VARARGS, "load Antimony model using the model string as input. example: loadAntimonyString(sbml_text)"},
	{"loadSBMLFile", pytc_loadSBMLFile, METH_VARARGS, "load SBML model using the file name as input. example: loadSBMLString(\"myfile.sbml\")"},
	{"loadAntimonyFile", pytc_loadAntimonyFile, METH_VARARGS, "load Antimony model using the file name as input. example: loadAntimonyFile(\"myfile.txt\");"},
	{"getSBMLString", pytc_getSBMLString, METH_VARARGS, "get SBML as a string from either a subset of items or all items. example: A = selectedItems(); s1 = getSBMLString(A); s2 = getSBMLString();"},
	{"getAntimonyString", pytc_getAntimonyString, METH_VARARGS, "get Antimony model string from either a subset of items or all items. example: A = selectedItems(); s1 = getAntimonyString(A); s2 = getAntimonyString();"},
	{"writeSBMLFile", pytc_writeSBMLFile, METH_VARARGS, "write the SBML to an xml file. example: writeSBMLFile(\"myfile.sbml\")"},
	{"writeAntimonyFile", pytc_writeAntimonyFile, METH_VARARGS, "write the Antimony model to a text file. example: writeAntimonyFile(\"myfile.txt\")"},

	{NULL, NULL, 0, NULL}
};
