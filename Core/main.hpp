/**
 * @mainpage TinkerCell Core Library
 * @brief 
The TinkerCell Core library is a set of C++ classes that utilize Nokia's Qt Toolkit. The classes provide functions for drawing networks as well as storing information associated with each node and connection in the network.

Being built using Qt Toolkit, the Core library makes extensive use of Qt's Signal/Slot framework. When signals are emitted, e.g. mousePressed(...), the signals are received by one or more slots. Slots are functions that respond to the signals. In the Core library, the MainWindow class acts like a "signal hub". Numerous Tools classes (aka "plug-ins") implement the slots for processing the MainWindow's signals. The Core library does not do anything by itself, except display the main window. Tools, or plug-ins, perform all the work. The set of plug-ins in the "BasicTools" folder perform numerous tasks such as inserting, highlighting selected items, renaming an item when the text is changed, etc. Other folders such as "ModelingTools" consist of plug-ins that are used to generate dynamic models of biological system. These plug-ins are not part of TinkerCellCore, but they are very important for the TinkerCell application.

The MainWindow class provides the top-level window. It is also a "hub" for numerous signals. Any programmer writing a plug-in must be familiar with all of these signals in order to utilize the Core library well. The MainWindow holds multiple NetworkHandle class instances. The NetworkHandle class is basically what defines a "network". The NetworkHandle stores a collection of ItemHandle instances. The ItemHandle class represents individual nodes (NodeHandle) or connections (ConnectionHandle). It is important to understand that each network can be displayed in multiple windows and each node or connection can be displayed using multiple graphical items on the screen. The NetworkWindow class is a single window that represents either the entire network or just part of a network. A NetworkHandle contains one or more NetworkWindow instances. Each NetworkWindow hold either a GraphicsView or a TextEditor, but never both. Therefore, a "network" (i.e. NetworkHandle) can displayed to the user using one or more graphical diagrams (GraphicsView) or text (TextEditor).

To understand the design of the Core library, it is imperative to understand ItemHandle. To build well-behaved plug-ins, it is imperative to understand how the Core library uses Undo Commands and Signals. It is also important to review the functions available in the MainWindow, GraphicsScene, and NetworkHandle classes.

<b>DataTable<T></b>

This is a template class that stores a 2 dimensional table, including the row and column headers. The contents of the table can belong to any type. Typically, TinkerCell only uses double and QString types because those are the two allowed data types in the ItemHandle class. The DataTable class is composed of three vectors: the data, the column headers, and the row headers. The class provides functions for obtaining the data values using header names or index values, removing or adding rows and columns, swapping rows and columns, and resizing the table. NumericalDataTable is an alias for DataTable<double> and TextDataTable is for DataTable<QString>.

\code

NumericalDataTable * dat = new NumericalDataTable;
dat->resize(10,4);
dat->colName(0) = "column 1";
dat->seRowNames( QStringList() << "row A" << "row B" << "row C" );
dat->value("row A", "column 1") = 10.0;
dat->removeCol(2);
dat->addCol(3,"column 3");  //insert new column at position 3
dat->value("X", "Y") = 5.0;   //automatically creates a new row called X and new column called Y
int r = dat->rows();
int c = dat->cols();
NumericalDataTable dat2 = dat->transpose();

\endcode

<b>Undo Commands</b>

Numerous classes are defined in the UndoCommands.h file that inherit from QUndoCommand. These classes contain an undo() and a redo() method. These functions undo and redo a single action without any other side effects. All changes made to a network are generally done using one of these QUndoCommand classes. Examples of undo command classes include MoveCommand, InsertGraphicsCommand and RemoveGraphicsCommand, InsertTextCommand and RemoveTextCommand, ChangeDataCommand, and RenameCommand. There are several more, one for each "atomic" operation. CompositeCommand can be used to construct a more complex command from atomic commands. For example, the "paste" operation is a composite command made from InsertCommand, MoveCommand, and RenameCommand (for renaming newly inserted items). Other plug-ins also use the composite command. 

The common procedure for using an undo command is as follows:

\code

            QList<QGraphicsItem*> graphicsItems;
            //add some items into graphicsItems
            QUndoCommand * cmd = new InsertGraphicsCommand("some informative message here",graphicsItems,handles);

            if (mainWindow && mainWindow->historyStack())
                mainWindow->historyStack()->push(cmd);
\endcode
Alternatively, the NetworkHandle class and GraphicsScene class provide functions that automatically do the same operations:

\code
            QList<QGraphicsItem*> graphicsItems;
            //add some items into graphicsItems
            GraphicsScene * scene = currentScene();
            scene->insert("informative message here", graphicsItems);

\endcode

<b>ItemHandle class</b>

This class is arguable the most integral aspect in the TinkerCell Core library. The ItemHandle can be thought of as a "package" with four important components: the graphics items for drawing a node or connection, the data table associated with that node or connection, the tools associated with the node of connection, and the family that the node or connection is identified with. The ItemHandle is the complete package that is required to obtain all the information about any item in the network. Since TinkerCell networks can be constructed using text of graphics interface, the ItemHandle is not required to have graphical items. For networks constructed using the text editor, the data inside each ItemHandle is what is most important. 

NodeHandle and ConnectionHandle inherit from ItemHandle. For text based models, it is possible to store connections between nodes and connections using ConnectionHandle::addNode() method, which takes a NodeHandle and an integer describing the "role" of that node in the connection. The interpretation of the "role" is open to the plug-in using it.

Here is a code example, where two graphics items are placed inside a handle, and a new table is added to the handle:

\code
            NodeHandle * nodeHandle = new NodeHandle;

            //make a node item from an XML file
            NodeGraphicsItem * node = new NodeGraphicsItem;
            NodeGraphicsReader reader;
            reader.readXML(node,"mynode.xml");

            //make a text graphics item
            TextGraphicsItem * text = new TextGraphicsItem("hello world");
            
            //add graphics items to the handle
            nodeHandle->graphicsItems << node << text;

            nodeHandle->textData("magic word") = "please";
            nodeHandle->numericalData("magic numbers","pi","value") = 3.141593;
            nodeHandle->numericalData("magic numbers","e","value") = 2.718282;

            //get the entire table
            DataTable<qreal> magicNumbers = nodeHandle->numericalDataTable("magic numbers");
            //set the entire table
            nodeHandle->numericalDataTable("magic numbers") = magicNumbers;
            
            //get list of all tables
            nodeHandle->getNumericalDataNames();
            nodeHandle->getTextDataNames();

\endcode

ItemHandle contains several functions for conveniently retrieving information or the list of child items. Please see the ItemHandle documentation . Each ItemHandle instance contains a list of pointers to tools, or classes that inherit from class Tool. These tools are associated with this item. When items are selected by a user, the list of contextMenuActions from each of these tools is placed in context menu and the list of graphics items are displayed to the side.

<b>ItemFamily class</b>

The ItemFamily class is used to describe a family that a node or connection belongs in. Nodes and connections are not required to belong in a family. Each family can have multiple parent families. The two main child classes are NodeFamily and ConnectionFamily. NodeFamily stores the default graphics item(s) that is used to draw an item of that family, and ConnectionFamily stores the default arrow head that is used when drawing connections of a given family. The family information is useful for tools in order to distinguish items and insert data tables according to the family of the item.

\code
            NodeFamily * f1 = new NodeFamily("family A");
            NodeFamily * f2 = new NodeFamily("family B");
            f2->setParent(f1);  //family B is a sub-family of family A

            NodeHandle * node = new NodeHandle("x",f2);

            if (node->isA("family A")) // will return true
                //do something
\endcode


<b>ItemData</b>

The "Data" inside an ItemHandle is an instance of class ItemData. This class is just composed of two hash tables, numericalData and textData. Each hash table maps a string to a DataTable. These hash tables store all the information needed to describe a node or connection. For example, numericalData["parameters"] might contain all the parameters belonging to this item. The data tables inside each item are added by tools, which often use the family information to decide what data tables to insert in a given item. For example, connections might contain textData["rates"] to describe the flux equations whereas nodes of a particular family might contains some other information, such as textData["DNA sequence"]. It is important to note that each entry is a 2D table of strings or numbers; of course, they can be a 1x1 table as well. 

<b>MainWindow class</b>

The MainWindow is always the top-most widget that is created in the main() function. The central widget inside the MainWindow is a Tab Widget with windows that can be popped out. Each widget inside the tab widget is a NetworkWindow. Each NetworkWindow can contain a TextEditor or a GraphicsScene. The MainWindow constructor has two arguments for specifying whether the documents should only contain TextEditors or only GraphicsScene or both. Each GraphicsScene is displayed using a GraphicsView. 

The MainWindow class inherits from Qt's QMainWindow. The MainWindow has two main functions: 
 
1. Provide the main window for the docking windows, menus, text editors, and drawing canvas

2. Serve as a Signal hub that routes the signals from each scene or text editor to the plug-ins listening to those signals. Thus, the plug-ins do not need to connect to every single scene and text editor; they only need to connect to the MainWindow's signals. These connections are made in a plug-in's setMainWindow() method.

The MainWindow also provides several Slots that are connected to C function pointers via the C_API_Slots class. These functions include find, rename, move, remove, and other functions for changing the data tables within an item in the network.

Nearly all the members in the MainWindow class are public. This includes the three toolbars: 1. toolBarBasic, which stores buttons for basic functions such as new, open, and save; 2. toolBarEdits, which stores buttons such as copy and pase; 3. toolBarForTools, which is intended for other tools. Tools may also add new toolbars using the addToolBar method in QMainWindow. The context menu (mouse right button) for TextEditor and GraphicsScene are also defined in MainWindow. The menus named contextItemsMenu and contextScreenMenu are used by GraphicsScene when items are selected and when no item is selected, resp.. The menus named contextSelectionMenu and contextEditorMenu are used by TextEditor when text is highlighted and when no text is highlighted, resp. Menu items such as file menu, edit menu, settings menu, and view menu are also public, allowing tools to add new actions to them. 

When items are inserted or removed from a GraphicsScene or TextEditor, each class emits a signal indicating that graphics item(s) have been removed and text item(s) have been removed, resp. These signals are connected to signals in the MainWindow with the same names. In addition, MainWindow also emits two signals called itemsInserted and itemsRemoved that only contain the ItemHandles instead of the graphics items or text items. Signals that contain only ItemHandles are useful for tools that do not need to know whether the network was constructed using text or graphical interface. 

<b>itemsAboutToBeInserted and itemsAboutToBeRemoved</b>: these signal are emitted just before items are inserted or removed from a network, respectively. It can be used to automatically add or remove items from the list. The signal contains a list of QUndoCommands; new commands can be added to this list to perform additional actions along with the insertion event.
<b>itemsInserted and itemsRemoved</b>: these signals are emitted after items are inserted or removed from a network, respectively. It can used to modify the items that have been inserted based on the placement of the items or other conditions. It is also used to add tools to the handle::tools list of the new items.
<b>dataChanged</b>: this signal is emitted whenever any handle's data entry is changed. It is also emitted when items are inserted or removed. This signal can be used to check when a model need to be updated. Note that undo events are not captured by this signal, which is only captured by historyChanged signal.
<b>historyChanged</b>: this signal is emitted whenever any recorded change occurs. This signal can be used to check when a model need to be updated.
<b>networkOpened, networkClosed, and networkChanged</b>: these signals are emitted whenever a new network is opened, a network has been closed, or a the user has clicked on a different network window (respectively). These signals are usually used to reset contents of widgets that display information about a network. 
<b>networkOpening and networkClosing</b>: these signals are sent before opening or closing networks (respectively). They can be used to check if the network has been saved.
<b>mousePressed, mouseReleased, mouseDragged, mouseDoubleClicked, sceneRightClicked</b>: These signals are emitted due to mouse events. These signals are emitted even if the useDefaultBehavior switch is off in GraphicsScene.
<b>keyPressed, keyReleased</b>: These signals are emitted due to keyboard events. These signals are emitted even if the useDefaultBehavior switch is off in GraphicsScene.

<b>NetworkHandle</b>

The NetworkHandle is used to store all the information inside a network. The three main components of a NetworkHandle are: historyStack, symbolsTable, and networkWindows. The history stack is used to store the QUndoCommands that provide the undo/redo capabilities. The symbolsTable stores all the nodes and connections in the network. The list networkWindows stores all the windows that are used to display the network to the user. The NetworkHandle provides convenience functions such as changeData(...) or rename(...). These functions create a QUndoCommand, add it to the history stack. Each NetworkHandle can be represented using one or more windows. All of these windows are connected to the same symbols table and the same history stack. NetworkHandle also contains functions such as find() for finding any string in the network and parseMath for validating a mathematical expression (uses muparser).

<b>NetworkWindow</b>

The NetworkWindow is a window (QMainWindow) inside the MainWindow's tab widget. This window can contain either a TextEditor or a GraphicsScene, but not both. Each NetworkWindow can contain its own toolbar or dock widgets. Each NetworkWindow has functions for replacing its current scene or text editor (warning: this operation cannot be undone). Each NetworkWindow can contain an ItemHandle pointer. This handle can be used for multiple purposes. It is designed for particular scenarios in which each individual window is associated with a handle. By default, this pointer is zero. 

<b>SymbolsTable</b>

The SymbolsTable class is used to store all the string found in a network model. These strings include the node and connection names and the row names and column names of all the data contained within each node and connection. The purpose of the symbols table is to easily look-up a symbol and find the network objects associated with that symbol. The symbols table keeps a hash table of names and pointers to the node or connection with that name. 

The SymbolsTable is also used to get all the ItemHandles in a network, except for "hidden" ItemHandles. ItemHandles represent objects in a network, whether the model is represented as text or graphics. 

Full names are always unique, e.g. Cell1.p1. Just the first name, e.g p1, need not be unique. The symbols table keeps a one-to-one hash table that maps full names to object pointers and a one-to-many that maps the first names to object pointers. The uniqueData hash table stores prefixed strings, e.g. p1.param1, as well as non-prefixed strings, e.g. param1. For each string, the hash table stores all the objects that contain that string and the name of the data table which contains that string. 

Each NetworkWindow contains one SymbolsTable instance. This instance is updated during any change (history update) to the network.
 
<b>GraphicsScene</b>

The GraphicsScene class is used to construct a network visually. It is one of the largest classes in TinkerCell. The GraphicsScene inherits from Qt's QGraphicsScene. The primary duty of the GraphicsScene class is to receive mouse and keyboard events and emit necessary signals such as itemsSelected, itemsMoved, or mouseOverItem. 

The GraphicsScene also handles selection of objects on the scene and moving objects on the scene. The selected objects are placed in the selected() list, and the moving objects are placed in the moving() list. These lists can be modified by plug-ins in order to modify which objects are selected or moved. Moving items are always grouped together when moving; this makes the movement smoother. For example, if a node has other nodes attached to it, a plug-in can ensure that all the nodes move together by adding each node to the moving() list when any one of them is selected. The GraphicsScene's selection and moving operations can be disabled by setting useDefaultBehavior = false.

In addition to emitting signals and handling selection and moving, the GraphicsScene houses numerous functions for conveniently making changes to a network. The functions include insert, remove, move, rename, and changeData. Each of these functions do three things: make a QUndoCommand object, push the undo command to the history stack, and emit the necessary signal(s) such as itemsInserted or itemsRemoved. 

The GraphicsScene is always contained inside a NetworkWindow. Therefore it uses the parent NetworkWindow's history stack and symbols table. Many functions such as changeData, rename, or allHandles simple call the parent NetworkWindow's function. 

<b>Configuring GraphicsScene</b>

Various visual features, such as the color of the selection rectangle in a scene and default grid size can be set using global variables: GraphicsScene::SelectionRectangleBrush, GraphicsScene::SelectionRectanglePen, GraphicsScene::BackgroundBrush, GraphicsScene::ForegroundBrush, GraphicsScene::GRID,  GraphicsScene::GridPen. GraphicsScene::MIN_DRAG_DISTANCE can be used to set the minimum distance that is considered a valid drag, i.e. moving the mouse less than this distance will be considered an accidental movement of the mouse and ignored. 

<b>GraphicsView</b>

The GraphicsView is a class for viewing a GraphicsScene. It inherits from QGraphicsView, and provides a few extra features such as drag-and-drop and zooming.

<b>Graphics items</b>

Qt's QGraphicsItem class is used to draw all the items in the GraphicsScene. The two main graphics item classes are NodeGraphicsItem and ConnectionGraphicsItem. Supporting graphics items are TextGraphicsItem and ControlPoint. 

The qgraphicsitem_cast<class> function can used to cast a generic QGraphicsItem to one of these four classes. In addition, NodeGraphicsItem::cast and ConnectionGraphicsItem::cast can also be used to get the top-most node or connection item from a generic QGraphicsItem instance. Each NodeGraphicsItem and ConnectionGraphicsItem also contains a string named ClassType, which is used to statically cast sub-classes of Node or Connection. For example, ArrowHeadItem is a NodeGraphicsItem with classType = "Arrow Head Item". example usage: if (node->className == ArrowHeadItem::CLASSNAME) static_cast<ArrowHeadItem*>(node)

<b>ControlPoint</b>

The ControlPoint class is used to identify key locations of a NodeGraphicsItem or ConnectionGraphicsItem that can be used to change the appearance of that item. For example, NodeGraphicsItem uses control points around its 
bounding box, allowing a user to drag the control points in order to resize the item. ConnectionGraphicsItem uses control points to define the line or beziers used to draw the connection. See image to the right: the small squares and circles are control points. Control points are generally not child items of the item that they belong with. The two main sub-classes of ControlPoint are NodeGraphicsItem::ControlPoint and ConnectionGraphicsItem::ControlPoint. 

<b>NodeGraphicsItem</b>

This class is used to draw nodes on the GraphicsScene. NodeGraphicsItem inherits from QGraphicsItemGroup, which is used to group several graphics items together. Each NodeGraphicsItem is a set of points and a set of shapes that are defined using those points. The points belong to the ControlPoint class and the shapes belong to the Shape class. The entire NodeGraphicsItem can be saved as an XML file using NodeGraphicsItemWriter (and NodeGraphicsItemReader for reading the XML). The XML file uses the SBML render extension format, which is similar to SVG. 

The NodeGraphicsItem has convenient functions such as connections(). The set of connections connected to a given node is retrieved by looking at the control points that are child items of that node. Each connection must have a control point that is the child item of the node that is it connected to.

<b>Shape</b>
This class is a polygon constructed using lines, beziers, or arcs. The Shape class inherits from QGraphicsPolygonItem. The polygon must be closed. The refresh() method is used whenever the shape's control points are changed. This updates the shape's polygon.

<b>ConnectionGraphicsItem</b>

This class is used to draw connections between nodes. ConnectionGraphicsItem is composed of a list of CurveSegment instances. Each CurveSegment is a collection of control points that define a single path, usually with the same central control point. Each curve segment also has two arrow head items -- one at either ends (they can be null). If there is a node at the end of any of the paths, then the control points at the end will be child items (see QGraphicsItem) of that node; so, looking at the parent items of each of the control points at the ends is the correct way to find all the nodes that are connected by a connection. 
 
The ConnectionGraphicsItem also contains an optional  centerRegionItem, which is a node that sits at the center of the connection. This node is used when one connection item needs to connect to another connection item. Since connections can only be connected to nodes, the center region item is used when connecting a connection to another. 

The control points that constitute a connection are generally parent-free, except for the end control points. As mentioned earlier, if a control point is at the end of a connection and is connected to a node, then the control point will be set as the child of the node item. This allows the control point to move along with the node. The ConnectionGraphicsItem class retrieves all the nodes that it is connected to by looking at the parent items of each of its end control points. ConnectionGraphicsItem provides convenient functions such as nodes(), nodesWithArrows(), nodesWithoutArrows(), where "WithArrows" means that there is an arrow head at the arc leading to the node. It is important to understand that these functions do not imply that the curve segments represent a reaction or some other specific process. They indicate the visual representation, which is then translated to more specific meanings by the plug-ins.

refresh() is used whenever the connection is changed. This function updates the arcs and the shape() of the connection using the control point positions.

The ConnectionGraphicsReader and Writer can be used to read and write a connection item to an XML file. 

The default arrow head can be set using ConnectionGraphicsItem::DefaultArrowHeadFile. Similarly, the default middle item (the box at the center) can be set using ConnectionGraphicsItem::DefaultMiddleItemFile. For example:

ConnectionGraphicsItem::DefaultArrowHeadFile = appDir + QString("/ArrowItems/Reaction.xml");
ConnectionGraphicsItem::DefaultMiddleItemFile = appDir + QString("/OtherItems/simplecircle.xml");
TextEditor class

<b>TextEditor</b>

The TextEditor class is used to construct a network using a text-based language. The syntax is not defined by TextEditor and must be provided by a supporting plug-in. The supporting plug-in is expected to make use of the lineChanged(...) and textChanged(...) signals emitted by TextEditor to identify changes by a user and call the insertItem(...), removeItem(...), or setItem(...) methods in order to modify the network.

<b>Tool (plug-in)</b>

Tool is the parent class for all TinkerCell "plug-ins". The most important method in the Tool class is setMainWindow(), which is used by a new tool to connect with the MainWindow's signals and slots. 

Each Tool can choose to create instances of Tool::GraphicsItem and place them on the scene. When these graphics items are selected by the user, TinkerCell Core will call the select(int) method of the Tool that is associated with the graphics item.

<b>Console Window</b>

The ConsoleWindow class provides a generic framework for Tools to receive command-line input as well as display messages or execute commands. Each tool can access the ConsoleWindow using console() or mainWindow->console(). For example:

 if (console())
    console()->message("hello world");    //print a message on the console window
 if (console())
    console()->error("incorrect response");  //print an error message on the console window
 if (console())
    console()->eval("print 1+2");  //evaluate this expression (only runs if a plugin such as python plugin is available)
 DataTable<double> data;
 //fill in data
 if (console())
    console()->printTable(data); //print a table (tab-delimited)

Tools can also interact with the user by connecting to the ConsoleWindow's commandExecuted signal. This signal is emitted whenever the user pressed <return> after entering a text at the command prompt. The Tools can process the string and carry out necessary operations. 

/code
	ConsoleWindow * console = console();
	if (console)
	{
		 connect(editor, SIGNAL( commandExecuted(const QString&) ),
				 this, SLOT( commandExecuted(const QString&) ));
	}

	Tools may also disable and re-enable the ConsoleWindow while they are processing the command by using:

	console()->freeze();    //lock the console window
	console()->unfreeze();  //unlock the console window

	Alternatively, Tools may also connect with the freeze() and unfreeze() slots:

	CommandTextEdit * editor = console()->editor();
	if (editor)
	{
		 connect(this, SIGNAL(freeze()), editor,SLOT(freeze()));
		 connect(this, SIGNAL(unfreeze()), editor,SLOT(unfreeze()));
		 connect(this, SIGNAL(setFreeze(bool)), editor,SLOT(setFreeze(bool)));
		 connect(editor, SIGNAL( commandExecuted(const QString&) ),
				 this, SLOT( commandExecuted(const QString&) ));
	}
/endcode

<b>CThread</b>

This class is used to run C plugins as separate threads. 

<b>InterpreterThread</b>

This class inherits from CThread. It is used to run interpreters such as Python and Octave interpreter.

<b>PythonInterpreterThread</b>

This class inherits from InterpreterThread. It is used to embed Python interpreter. This class uses the C program python/runpy.c.in

<b>OctaveInterpreterThread</b>

This class inherits from CThreads. It is used to embed Octave interpreter. This class uses the C++ program octave/runOctave.cpp (for embedding Octave) and assumes that SWIG has been used to generate tinkercell.oct library (which extends Octave).
*/

/*! \defgroup core TinkerCell Core classes
   \brief The main classes in TinkerCell Core. These form the base for all the plug-ins.
 */

/*! \defgroup helper Helper functions and classes
   \brief Helper classes and functions that are used by the core classes.
 */

/*! \defgroup io Input and output
   \brief Classes that read/write graphics information and data information from/to files as well as serve as input/output devices for C functions.
 */
 
 /*! \defgroup undo Undo commands
   \brief A set of classes that allow undo/redo (using Qt Undo framework).
 */

/*! \defgroup CAPI C API
   \brief C functions that are provided by the TinkerCell Core library and Plug-ins (tools)
 */

/*! \defgroup plugins TinkerCell plug-ins
   \brief Plug-ins, which are classes that inheir from Tool class, provide the large majority of the important features in TinkerCell
 */
 
