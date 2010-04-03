/**
 * @mainpage TinkerCell Core Library
 * @brief 
The TinkerCell core library provides the base classes and functions that are used by other libraries (plug-ins) 
to create the visual interface in TinkerCell. TinkerCell makes extensive uses of QUndoCommand, which is a class
with an undo and redo function. Every modification that is performed on a model is done using one of these
commands. Functions such as move(), insert(), delete(), or changeData() in GraphicsScene and NetworkWindow
call one of these undo command in order to modify the model. Whenver a command makes a change, 
a signal is sent to all the plug-ins informing them of the change. Hence, signals and undo commands are 
the primary players in the core library.

 * @section MainWindow The Main Window
The MainWindow class provides the main window and the sub-windows (or tabs) along with functions for adding
docking windows, menu bars, etc. More importantly, the MainWindow contains various signals and slots that 
inform plug-ins when a change occurs. The plug-ins also inform the MainWindow when they make changes. Hence, 
the MainWindow serves as a central player for plug-in to plug-in communication. The MainWindow also
houses a lot of functions that serve as the C API (see C_API_Slots class)

 * @section GraphicsScene The Graphics Scene and View
The GraphicsScene class draws items on the screen and handles mouse and keyboard events. The events are passed
to MainWindow, which then informs the plug-ins about those events. Each GraphicsScene contains a history stack
where the undo/redo commands are stored. GraphicsScene provides a large number of functions for making changes
to the model. All of these functions use commands with undo/redo, and they send the appropriate signals to the
MainWindow. So using these functions is usually the most convenient way for plug-in writes to make changes.

* @section UndoCommands The Undo/Redo Commands
The various undo/redo commands perform single operations that can be undo/redone without having
indirect affects on other operations. The CompositeCommand can be used to combine several of these commands
into one. Since the core library provides the undo/redo commands for most of the basic operations, a plug-in
writer can usually construct new functions by combining exisitng commands using CompositeCommand. Similarly, the 
ReverseCommand can be used to flip the undo and redo operations of a command.

* @section Handles Item Handles
ItemHandle and classes that inherit from it, NodeHandle and ConnectionHandle, are an integral component of
TinkerCell Core design. A handle stores everything about a model object, which includes the graphical items 
that are used to draw the object on the screen, the data tables that store information about the object, 
the tools that are associated with the object, and the family of the object.

* @section SymbolsTable The Symbols Table
The Symbols Table, located inside each GraphicsScene, stores several hash tables. The hash tables store the names
of items in the scene as well as their data column and row names. This is designed as an easy way to look-up
any string that exists in the model. All full names are stored using the dot delimiter and underscore.

* @section GraphicsItem Graphics Items
The major graphics classes are: NodeGraphicsItem, ConnectionGraphicsItem, ControlPoint, and GraphicalTool. 
NodeGraphicsItem is a class that can be represented in XML format. It is a collection of polygons (class Shape). 
ArrowHeadItem inherits from NodeGraphicsItem and is used inside ConnectionGraphicsItem.
ConnectionGraphicsItem is a collection of path objects with arrow heads on each path and a NodeGraphicsItem at the
center for decoration purpose.
ControlPoint is a graphical item that usually does not have an associated handle. It is used to modify other graphical
items, thus is it not a object in a model.
GraphicalTool is a graphical item that is used for a unique user interface. They pop up when objects in a scene are 
selected. They are usually associated with plug-ins
* @section Tools Tools (plug-ins)
Tools represent a genetic class which can connect with the MainWindow and modify TinkerCell. 
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
