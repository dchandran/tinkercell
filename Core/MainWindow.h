/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

MainWindow is the parent container for all the other widgets in TinkerCell
The central widget in MainWindow is a tab widget. Each tab widget can hold
a GraphicsView or a TextEditor.
One of the main roles of MainWindow is to serve as a signal/slot hub for Tools. 


****************************************************************************/

#ifndef TINKERCELL_MAINWINDOW_H
#define TINKERCELL_MAINWINDOW_H

#include <QtGui>
#include <QString>
#include <QFileDialog>
#include <QtDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAction>
#include <QMenu>
#include <QTabWidget>
#include <QThread>
#include <QFile>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QHash>
#include <QUndoCommand>
#include <QToolBar>
#include <QUndoView>
#include <QUndoStack>
#include <QPrintDialog>
#include <QPrinter>
#include <QGridLayout>
#include <QSemaphore>
#include <QLibrary>
#include <QToolBox>
#include <QHash>

#include "HistoryWindow.h"
#include "DataTable.h"
#include "ConvertValue.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{
	static QString PROJECTWEBSITE = QObject::tr("www.tinkercell.com");
	static QString ORGANIZATIONNAME = QObject::tr("TinkerCell");
	static QString PROJECTNAME = QObject::tr("TinkerCell");
	static QString CPP_ENTRY_FUNCTION = QObject::tr("loadTCTool");
	static QString C_ENTRY_FUNCTION = QObject::tr("tc_main");

	class ConsoleWindow;
	class NodeGraphicsItem;
	class ConnectionGraphicsItem;
	class GraphicsScene;
	class ItemHandle;
	class ItemFamily;
	class MainWindow_FtoS;
	class NetworkWindow;
	class TextEditor;
	class Tool;
	class HistoryStack;
	class TextParser;
	class TextItem;
	class GraphicsView;
	class SymbolsTable;
	class CThread;
	class C_API_Slots;

	/*! \brief 
	MainWindow is the parent container for all the other widgets in TinkerCell
	The central widget in MainWindow is a tab widget. Each tab widget can hold
	a GraphicsView or a TextEditor.
	One of the main roles of MainWindow is to serve as a signal/slot hub for Tools. 
	\ingroup core
	*/
	class MY_EXPORT MainWindow : public QMainWindow
	{
		Q_OBJECT

	public:
	
		friend class NetworkWindow;
		friend class NetworkHandle;
		friend class GraphicsScene;
		friend class TextEditor;
		friend class GraphicsView;

		/*! \brief this enum is used to determine how to place a widget when used in addToolWindow.
		             DockWidget = tool window is placed into a dockable widget
					 ToolBoxWidget = tool window is placed in an existing toolbox, if one exists
					 NewToolBoxWidget = tool window is placed inside a new toolbox
		*/
		enum TOOL_WINDOW_OPTION { DockWidget , ToolBoxWidget , NewToolBoxWidget };

		/*! \brief the types of views for multiple documents
		             TabView = tabbed documents
					 WindowView = each documents in a separate subwindow
		*/
		enum VIEW_MODE { TabView , WindowView };

		/*! \brief the default option to use for tools (optional)*/
		static TOOL_WINDOW_OPTION defaultToolWindowOption;

		/*! \brief the default option to use for history window*/
		static TOOL_WINDOW_OPTION defaultHistoryWindowOption;

		/*! \brief the default option to use for console window*/
		static TOOL_WINDOW_OPTION defaultConsoleWindowOption;

		/*! \brief the default extension for saving files*/
		static QString defaultFileExtension;

		/*! \brief register all the TinkerCell data structures with Qt*/
		static void RegisterDataTypes();

		/*!
		* \brief 5-arg (optional) constructor allows disabling of text/graphics modes
		* \param bool enable text-based network construction (default = true)
		* \param bool enable graphics-based network construction (default = true)
		* \param bool enable command-line (default = true)
		* \param bool enable history window (default = true)
		* \param bool allow tabbed and windowed view modes (default = true)
		*/
		MainWindow(bool enableScene = true, bool enableText = true, bool enableConsoleWindow = true, bool showHistory = true, bool views = true);
		/*!
		* \brief allow or disallow changing between different views
		* \param bool
		*/
		virtual void allowMultipleViewModes(bool);
		/*!
		* \brief Destructor: delete all the graphics scenes.
		*/
		virtual ~MainWindow();
		/*!
		* \brief The TinkerCell user directory, which is User's Documents Folder/TinkerCell by default, but users may change this setting
		*/
		static QString userHome();
		/*!
		* \brief The TinkerCell user temporary directory, which is <SYSTEM TEMP FOLDER>/TinkerCell
		*/
		static QString userTemp();
		/*!
		* \brief Add a new docking window to the main window.
		           The name and icon are obtained using the widget's windowTitle and windowIcon, so
				   be sure to set those before calling this function.
		* \param Tool* the new tool
		* \param Qt::DockWidgetArea the initial docking area
		* \param Qt::DockWidgetAreas the allowed docking areas
		* \param bool whether or not to place the docking window in the view menu
		* \param bool use a QToolBox instead of a dock widget. The widget will not be dockable, but the entire toolbox will be dockable.
		* \return QDockWidget* the new docking widget. ToolBoxWidget option is used, the docking widget may be an existing docking widget.
		*/
		QDockWidget * addToolWindow(QWidget * tool, TOOL_WINDOW_OPTION option = DockWidget, Qt::DockWidgetArea initArea = Qt::RightDockWidgetArea, Qt::DockWidgetAreas allowedAreas = Qt::AllDockWidgetAreas, bool inMenu = true);
		/*!
		* \brief set the cursor for all windows
		* \param QCursor cursor
		* \return void
		*/
		void setCursor(QCursor cursor);
		/*!
		* \brief add a new tool to the list of tools stored in the main window
		* \param the name of the new tool
		* \param the new tool
		* \return void
		*/
		void addTool(Tool* tool);
		/*!
		* \brief Initialize the basic menu (save, open, close, exit, etc.).
		* \return void
		*/
		void initializeMenus(bool enableScene = true, bool enableText = true);
		/*!
		* \brief This function is usually called from a new thread. This function allows all the
		plugins to add their functionalities to the C function pointer of the new thread.
		* \param QSemaphore* used to wait for all the plugins to initialize the thread
		* \param QLibrary* the library to load
		* \return void
		*/
		void setupNewThread(QSemaphore*,QLibrary*);
		/*!
		* \brief Load a new plugin (dll)
		* \param the complete path of the dll file
		* \return void
		*/
		void loadDynamicLibrary(const QString&);
		/*!
		* \brief gets the current scene that is active
		* \return GraphicsScene* current scene
		*/
		GraphicsScene * currentScene() const;
		/*!
		* \brief gets the text editor that is active
		* \return TextEditor* current editor
		*/
		TextEditor * currentTextEditor() const;
		/*!
		* \brief (same as currentWindow) gets the current window that is active
		* \return NetworkHandle* current network window
		*/
		NetworkHandle * currentNetwork() const;
		/*!
		* \brief gets all the windows in the main window
		* \return QList<NetworkHandle*> list of windows
		*/
		QList<NetworkHandle*> allNetworks() const;
		/*!
		* \brief the history stack of the current network.
		* \return QUndoStack* current scene's history stack or null if current network is null
		*/
		QUndoStack * historyStack() const;
		/*!
		* \brief the history stack widget of the current window.
		* \return QUndoView* current scene's history stack or null if current network is null
		*/
		QUndoView * historyWidget();
		/*!
		* \brief get a tool
		* \param QString name of the tool
		* \return Tool*
		*/
		virtual Tool * tool(const QString&) const;
		/*!
		* \brief get all tools
		* \return QList<Tool*>
		*/
		virtual QList<Tool*> tools() const;
		/*!
		* \brief the set of all windows inseted in the main window using addToolWindow
		*/
		QList<QWidget*> toolWindows;
		/*!
		* \brief the context menu that is shown during right-click event on selected graphical items.
		Plugins can add new actions to this menu.
		*/
		QMenu contextItemsMenu;
		/*!
		* \brief the context menu that is shown during right-click event on the scene.
		Plugins can add new actions to this menu.
		*/
		QMenu contextScreenMenu;
		/*!
		* \brief the context menu that is shown during right-click event on a text editor with text selected.
		Plugins can add new actions to this menu.
		*/
		QMenu contextSelectionMenu;
		/*!
		* \brief the context menu that is shown during right-click event on a text editor with no text selected.
		Plugins can add new actions to this menu.
		*/
		QMenu contextEditorMenu;
		/*!
		* \brief The file menu. Plugins can add new actions to this menu.
		*/
		QMenu* fileMenu;
		/*!
		* \brief The edit menu. Plugins can add new actions to this menu.
		*/
		QMenu* editMenu;
		/*!
		* \brief The view menu. New docking windows are automatically added here.
		*/
		QMenu* viewMenu;
		/*!
		* \brief The help menu.
		*/
		QMenu* helpMenu;
		/*!
		* \brief the menu for settings such as default plugins, Tinkercell home directory, etc.
		*/
		QMenu* settingsMenu;
		/*!
		* \brief the menu for choosing one of the available parsers (will be 0 if there are no parsers)
		*/
		QMenu* parsersMenu;
		/*!
		* \brief The tool bar that contains new, open, close, etc. actions
		*/
		QToolBar* toolBarBasic;
		/*!
		* \brief The tool bar that contains copy, paste, undo, etc.
		*/
		QToolBar* toolBarEdits;
		/*!
		* \brief One of the initial tool bars which designated for tools that do not want
		to create a new toolbar
		*/
		QToolBar* toolBarForTools;
		
		/*! \}
			\name slots
			\{
		*/

	public slots:
		/*!
		* \brief asks user for a new directory to be used as the user home directory (must be writtable)
		*/
		void setUserHome();
		/*!
		* \brief create new scene
		*/
		GraphicsScene * newGraphicsWindow();
		/*!
		* \brief create new text editor
		*/
		TextEditor * newTextWindow();
		/*!
		* \brief triggered when the close button is clicked. Closes the current window
		*/
		void closeWindow();
		/*!
		* \brief triggered when the save button is clicked. Opens a file dialog and emits the save signal.
		The main window itself does not implement the save.
		*/
		void saveWindow();
		/*!
		* \brief triggered when the save-as button is clicked. Opens a file dialog and emits the save signal.
		The main window itself does not implement the save.
		*/
		void saveWindowAs();
		/*!
		* \brief triggered when the open button is clicked. Opens a file dialog.
		Note: the core library just emits a signal, and other tools are responsible for actually opening a file
		*/
		void open();
		/*!
		* \brief open a file.
		Note: the core library just emits a signal, and other tools are responsible for actually opening a file
		The main window does not implement an function for opening a new file
		*/
		void open(const QString&);
		/*!
		* \brief calls current scene or text editor's undo
		*/
		void undo();
		/*!
		* \brief calls current scene or text editor's redo
		*/
		void redo();
		/*!
		* \brief calls current scene or text editor's copy
		*/
		void copy();
		/*!
		* \brief calls current scene or text editor's cut
		*/
		void cut();
		/*!
		* \brief calls current scene or text editor's paste
		*/
		void paste();
		/*!
		* \brief calls current scene or text editor's selectAll
		*/
		void selectAll();
		/*!
		* \brief calls current scene or text editor's find
		*/
		void remove();
		/*!
		* \brief triggered when the print button is clicked. Calls current scene's print
		*/
		void print();
		/*!
		* \brief triggered when the print-to-file button is clicked. Calls current scene's print on a pdf file
		*/
		void printToFile();
		/*!
		* \brief adjusts the current's scene's view rectangle to fit all the visible items
		*/
		void fitAll();
		/*!
		* \brief sends a signal to all plugins telling them to exit their current processes.
		*/
		void sendEscapeSignal(const QWidget * w = 0);
		/*!
		* \brief add a new text parser to the list of available parsers.
			The current text parser can be obtained using TextParser::currentParser();
		*/
		void addParser(TextParser*);

		/*! \brief change grid mode for current scene to on (>0)*/
		void gridOn();

		/*! \brief change grid mode for current scene to off (=0)*/
		void gridOff();

		/*! \brief set grid size for current scene*/
		void setGridSize();
		
		/*! \brief pop-out the current window*/
		void popOut();

		/*! \brief get the console window*/
		ConsoleWindow * console() const;

		/*! \brief gets the global main window*/
		static MainWindow * instance();

	protected slots:
		/*! \brief pop-out the given window*/
		void popOut(NetworkWindow *);
		
		/*! \brief pop-in the given window*/
		void popIn(NetworkWindow *);
		
		/*!
		* \brief sets the active window
		*/
		void setCurrentWindow(NetworkWindow*);
		/*!
		* \brief loads files (library files or model files)
		* \param QList<QFileInfo>& the name(s) of the file(s)
		* \return void
		*/
		void dragAndDropFiles(const QList<QFileInfo>& files);
		/*!
		* \brief change console background color
		* \return void
		*/
		void changeConsoleBgColor();
		/*!
		* \brief change console text color
		* \return void
		*/
		void changeConsoleTextColor();
		/*!
		* \brief change console message text color
		* \return void
		*/
		void changeConsoleMsgColor();
		/*!
		* \brief change console error text color
		* \return void
		*/
		void changeConsoleErrorMsgColor();
		/*!
		* \brief tab changed
		*/
		virtual void tabIndexChanged(int);
		/*!
		* \brief signals whenever items are deleted
		* \param GraphicsScene * scene where the items were removed
		* \param QList<QGraphicsItem*>& list of items removed
		* \param QList<ItemHandle*>& list of handles removed (does NOT have to be the same number as items removed)
		* \return void*/
		void itemsRemovedSlot(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<ItemHandle*>& handles);
		/*!
		* \brief signals whenever items are added
		* \param GraphicsScene * scene where the items were added
		* \param QList<QGraphicsItem*>& list of new items
		* \param QList<ItemHandle*>& list of new handles (does NOT have to be the same number as items)
		* \return void*/
		void itemsInsertedSlot(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<ItemHandle*>& handles);
		/*!
		* \brief send signal to other tools so that they can connect functions to signals
		* \param QSemaphore* semaphore
		* \param QLibrary * the dynamic library instance
		* \return void
		*/
		void setupFunctionPointersSlot(QSemaphore*,QLibrary *);
		
		/*! \}
			\name signals
			\{
		*/

	signals:

		/*!
		* \brief a new tool is about to be added. This signal can be used to prevent the tool from being added
		* \param Tool the tool itself
		* \param bool& set this bool to false to prevent the tool from loading
		* \return void
		*/
		void toolAboutToBeLoaded( Tool * tool, bool * shouldLoad );
		/*!
		* \brief one of more changed have occurred in the history window of the current scene
		* \param int number of changes (negative = undos, positive = redos)
		* \return void
		*/
		void historyChanged( int );
		/*!
		* \brief used internally by MainWindow in order to move from a thread to the main thread
		* \param QSemaphore* Sempahore that lets the thread run once C API is initialized
		* \param QLibrary * the new FuntionToSignal instance
		* \return void
		*/
		void funtionPointersToMainThread( QSemaphore* , QLibrary * );
		/*! \brief signals when a new tool (plugin) is loaded
		* \param Tool* the new tool
		* \return void
		*/
		void toolLoaded(Tool * tool);
		/*!
		* \brief signals when a new FuntionToSignal is constructed
		* \param QLibrary * the new FuntionToSignal instance
		* \return void
		*/
		void setupFunctionPointers( QLibrary * );
		/*!
		* \brief signals when a window is going to close
		* \param NetworkHandle *  the window that is closing
		* \param Boolean setting to false will prevent this window from closing
		* \return void
		*/
		void windowClosing(NetworkHandle *, bool*);
		/*!
		* \brief signals after a window is closed
		* \param NetworkHandle *  the window that was closed
		* \return void
		*/
		void windowClosed(NetworkHandle *);
		/*!
		* \brief signals used inform that the model is going to be saved as it is
		* \param NetworkHandle *  the window where model was loaded (usually current scene)
		* \return void*/
		void prepareModelForSaving(NetworkHandle*,bool*);
		/*!
		* \brief signals used inform that the model has been saved
		* \param NetworkHandle *  the window where model was loaded (usually current scene)
		* \return void
		*/
		void modelSaved(NetworkHandle*);
		/*!
		* \brief signals used selects a file to save the current model to
		* \param QString& file that is selected by user
		* \return void
		*/
		void saveModel(const QString& filename);
		/*!
		* \brief signals used selects a file to open in the current window
		* \param QString& file that is selected by user
		* \return void
		*/
		void loadModel(const QString& filename);
		/*!
		* \brief signals informs that the current window has just loaded a new model
		* \param NetworkHandle *  the window where model was loaded (usually current scene)
		* \return void
		*/
		void modelLoaded(NetworkHandle*);
		/*!
		* \brief signals whenever the new window is opened
		* \param NetworkHandle* the current new window
		* \return void
		*/
		void windowOpened(NetworkHandle*);
		/*!
		* \brief signals whenever the current window changes
		* \param NetworkHandle* the previous windpw
		* \param NetworkHandle* the current new window
		* \return void
		*/
		void windowChanged(NetworkHandle*,NetworkHandle*);
		/*!
		* \brief signals whenever a new item is selected (item can be sub-item, not top-level)
		* \param GraphicsScene * scene where items are selected
		* \param QList<QGraphicsItem*>& list of all selected item pointers
		* \param QPointF point where mouse is clicked
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		void itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
		/*!
		* \brief signals whenever an empty node of the screen is clicked
		* \param GraphicsScene * scene where the event took place
		* \param QPointF point where mouse is clicked
		* \param Qt::MouseButton which button was pressed
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		void mousePressed(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		/*!
		* \brief signals whenever an empty node of the screen is clicked
		* \param GraphicsScene * scene where the event took place
		* \param QPointF point where mouse is clicked
		* \param Qt::MouseButton which button was pressed
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		void mouseReleased(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		/*!
		* \brief emits event when mouse is double clicked
		* \param GraphicsScene * scene where the event took place
		* \param point where mouse is clicked
		* \param modifier keys being used when mouse clicked
		* \return void*/
		void mouseDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		/*!
		* \brief signals whenever mouse is dragged from one point to another
		* \param GraphicsScene * scene where the event took place
		* \param QPointF point where mouse is clicked first
		* \param QPointF point where mouse is released
		* \param Qt::MouseButton button being pressed
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		void mouseDragged(GraphicsScene * scene, QPointF from, QPointF to, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		/*!
		* \brief signals whenever items are being moved (each item is the top-most item)
		* \param GraphicsScene * scene where the items were moved
		* \param QList<QGraphicsItem*>& list of pointes to all moving items
		* \param QPointF point where the item was
		* \param QPointF point where the item is moved to
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		void itemsMoved(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<QPointF>& distance, Qt::KeyboardModifiers modifiers);
		/*!
		* \brief signals just before items are deleted
		* \param GraphicsScene* scene where the items are going to be removed
		* \param QList<QGraphicsItem*>& list of items going to be removed
		* \param QList<ItemHandle*>& list of handles going to be removed (does NOT have to be the same number as items removed)
		* \return void*/
		void itemsAboutToBeRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& item, QList<ItemHandle*>& handles);
		/*!
		* \brief signals whenever items are deleted
		* \param GraphicsScene * scene where the items were removed
		* \param QList<QGraphicsItem*>& list of items removed
		* \param QList<ItemHandle*>& list of handles removed (does NOT have to be the same number as items removed)
		* \return void*/
		void itemsRemoved(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<ItemHandle*>& handles);
		/*!
		* \brief signals whenever items are going to be added
		* \param GraphicsScene* scene where the items are added
		* \param QList<QGraphicsItem*>& list of new graphics items
		* \param QList<ItemHandle*>& list of new handles (does NOT have to be the same number as items)
		* \return void*/
		void itemsAboutToBeInserted(GraphicsScene * scene, QList<QGraphicsItem*>& , QList<ItemHandle*>& );
		/*!
		* \brief signals whenever items are added
		* \param GraphicsScene * scene where the items were added
		* \param QList<QGraphicsItem*>& list of new items
		* \param QList<ItemHandle*>& list of new handles (does NOT have to be the same number as items)
		* \return void*/
		void itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<ItemHandle*>& handles);
		/*!
		* \brief A convenient signal that is emitted when items are inserted from a GraphicsScene
		or TextEditor. Warning: listening to the other itemsInserted signals may cause redundancy
		* \param NetworkHandle* where the editting happened
		* \param QList<TextItem*> new items
		*/
		void itemsInserted(NetworkHandle * win, const QList<ItemHandle*>&);
		/*!
		* \brief A convenient signal that is emitted when items are removed from a GraphicsScene
				or TextEditor. Warning: listening to the other itemsRemoved signals may cause redundancy
		* \param NetworkHandle* where the editting happened
		* \param ItemHandle* removed items
		*/
		void itemsRemoved(NetworkHandle * win, const QList<ItemHandle*>& );
		/*! \brief signals just before items are copied
		* \param GraphicsScene * scene where the items are going to be copied
		* \param QList<QGraphicsItem*>& list of graphics items going to be copied
		* \param QList<ItemHandle*>& list of handles going to be copied (does NOT have to be the same number as items removed)
		* \return void*/
		void copyItems(GraphicsScene * scene, QList<QGraphicsItem*>& , QList<ItemHandle*>& );
		/*! \brief some text inside this editor has been changed
		\param TextEditor* editor
		\param QString old text (usually a line)
		\param QString new text (usually a line)
		*/
		void textChanged(TextEditor * , const QString&, const QString&, const QString&);
		/*! \brief the cursor has moved to a different line
		\param TextEditor* editor
		\param int index of the current line
		\param QString current line text
		*/
		void lineChanged(TextEditor * , int, const QString&);
		/*! \brief request to parse the text in the current text editor
		\param TextEditor* editor
		*/
		void parse(TextEditor *);
		/*! \brief signals whenever mouse moves, and indicates whether it is on top of an item
		* \param GraphicsScene * scene where the event took place
		* \param QGraphicsItem* pointer to item that mouse is on top of
		* \param QPointF point where mouse is clicked
		* \param Qt::MouseButton button being pressed
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \param QList<QGraphicsItem*>& list of items that are being moved with the mouse
		* \return void*/
		void mouseMoved(GraphicsScene * scene, QGraphicsItem* item, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers, QList<QGraphicsItem*>&);
		/*! \brief signals whenever mouse is on top of an item
		* \param GraphicsScene * scene where the event took place
		* \param QGraphicsItem* pointer to item that mouse is on top of
		* \param QPointF point where mouse is clicked
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \param QList<QGraphicsItem*>& list of items that are being moved with the mouse
		* \return void*/
		void mouseOnTopOf(GraphicsScene * scene, QGraphicsItem* item, QPointF point, Qt::KeyboardModifiers modifiers, QList<QGraphicsItem*>&);
		/*! \brief signals whenever right click is made on an item or sceen
		* \param GraphicsScene * scene where the event took place
		* \param QGraphicsItem* pointer to item that mouse is clicked on
		* \param QPointF point where mouse is clicked
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		void sceneRightClick(GraphicsScene * scene, QGraphicsItem* item, QPointF point, Qt::KeyboardModifiers modifiers);
		/*! \brief signals whenever a key is pressed
		* \param GraphicsScene * scene where the event took place
		* \param QKeyEvent * key that is pressed
		* \return void*/
		void keyPressed(GraphicsScene * scene, QKeyEvent *);
		/*! \brief signals whenever a key is released
		* \param GraphicsScene * scene where the event took place
		* \param QKeyEvent * key that is released
		* \return void*/
		void keyReleased(GraphicsScene * scene, QKeyEvent *);
		/*! \brief signals whenever color of items are changed
		* \param GraphicsScene * scene where the event took place
		* \param QList<QGraphicsItem*>& items that changed color
		* \return void*/
		void colorChanged(GraphicsScene * scene, const QList<QGraphicsItem*>& items);
		/*! \brief signals whenever item parents are changed
		* \param GraphicsScene * scene where the event took place
		* \param QList<QGraphicsItem*>& items
		* \param QList<QGraphicsItem*>& new parents
		* \return void*/
		void parentItemChanged(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<QGraphicsItem*>& parents);
		/*! \brief signals whenever an item is renamed
		* \param NetworkHandle * window where the event took place
		* \param QList<ItemHandle*>& items
		* \param QList<QString>& old names
		* \param QList<QString>& new names
		* \return void*/
		void itemsRenamed(NetworkHandle * window, const QList<ItemHandle*>& items, const QList<QString>& oldnames, const QList<QString>& newnames);
		/*! \brief signals whenever the handles for graphics items have changed
		* \param GraphicsScene* scene where the event took place
		* \param QList<GraphicsItem*>& items that are affected
		* \param QList<ItemHandle*>& old handle for each items
		* \return void*/
		void handlesChanged(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>& old);
		/*! \brief signals whenever item parent handle is changed
		* \param NetworkHandle * window where the event took place
		* \param QList<ItemHandle*>& child items
		* \param QList<ItemHandle*>& old parents
		* \return void*/
		void parentHandleChanged(NetworkHandle * scene, const QList<ItemHandle*>&, const QList<ItemHandle*>&);
		/*! \brief signals whenever some data is changed
		* \param QList<ItemHandle*>& items handles
		* \return void*/
		void dataChanged(const QList<ItemHandle*>& items);
		/*! \brief signals whenever the current activities need to be stopped
		* \param QWidget * the widget that send the signal
		* \return void*/
		void escapeSignal(const QWidget * sender);
		/*! \brief signals whenever file(s) are dropped on the canvas
		* \param QList<QFileInfo>& the name(s) of the file(s)
		* \return void*/
		void filesDropped(const QList<QFileInfo>& files);

	protected:
		/*! \brief allowed views*/
		bool allowViewModeToChange;
		/*! \brief the loaded dynamic libraries indexed by file name*/
		QHash<QString,QLibrary*> dynamicallyLoadedLibraries;
		/*! \brief the general window for command, errors, and messages*/
		ConsoleWindow * consoleWindow;
		/*! \brief read initial settings from settingsFileName
		* \return void*/
		void readSettings();
		/*! \brief save initial settings to settingsFileName
		* \return void*/
		void saveSettings();
		/*! \brief affects drag and drop of files
		* \param drop event
		* \return void*/
		void dropEvent(QDropEvent *);
		/*! \brief affects drag and drop of files
		* \param drag event
		* \return void*/
		void dragEnterEvent(QDragEnterEvent *event);
		/*! \brief close window event -- asks whether to save file
		* \param QCloseEvent * event
		* \return void*/
		void closeEvent(QCloseEvent *event);
		/*! \brief the central multi-document interface widget*/
		QTabWidget * tabWidget;
		/*! \brief the list of all network windows*/
		QList<NetworkHandle*> allNetworks;
		/*! \brief the optional tool box that will only appear if one of the plug-ins uses the toolbox argument in the addToolWindow call*/
		QToolBox * toolBox;
		/*! \brief history view, not the stack itself. The stack is stored within each NetworkHandle*/
		HistoryWindow historyWindow;
		/*! \brief keep pointer to last selected window. Used by windowChanged signal*/
		NetworkWindow * currentNetworkWindow;
		/*! \brief all the tools (plug-ins) are stored here, indexed by their names*/
		QHash<QString,Tool*> toolsHash;
	private:
		/*! \brief home directory path*/
		static QString userHomePath;
		/*! \brief used to rename items*/
		QLineEdit * renameOld;
		/*! \brief used to rename items*/
		QLineEdit * renameNew;
		/*!
		* \brief the global main window
		*/
		static MainWindow * globalInstance;
		/*!
		* \brief C API class
		*/
		C_API_Slots * c_api_slots;
	public:
		/*!
		* \brief stores the last opened directory
		*/
		static QString previousFileName;
		
		/*!\brief stores list of all pointers that have been deleted (to prevent double-deletions)*/
		static QHash<void*,bool> invalidPointers;

	};

}

#endif
