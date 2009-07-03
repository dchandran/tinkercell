/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This is header file for Tinkercell's main window
 The MainWindow contains a set of GraphicScenes, which is the class
 that performs all the drawing. Each GraphicsScene emits various signals. Those
 signals are then emitted by the MainWindow; in this way, a plugin does not need
 to listen to each of the GraphicsScene signals but only the MainWindow's signals.

 
 The MainWindow also has its own signals, such as a toolLoaded, modelSaved, etc.
 
 The MainWindow keeps a list of all plugins, and it is also responsible for loading plugins. 
 
 
****************************************************************************/

#ifndef TINKERCELL_MAINWINDOW_H
#define TINKERCELL_MAINWINDOW_H

#include <QtGui>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QString>
#include <QFileDialog>
#include <QtDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAction>
#include <QMenu>
#include <QMdiArea>
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

#include "HistoryStack.h"
#include "DataTable.h"
#include "ConvertValue.h"

namespace Tinkercell
{
    static QString PROJECTWEBSITE = QObject::tr("www.tinkercell.com");
    static QString ORGANIZATIONNAME = QObject::tr("TinkerCell");
    static QString PROJECTNAME = QObject::tr("Tinkercell");
    static QString CPP_ENTRY_FUNCTION = QObject::tr("loadTCTool");
    static QString C_ENTRY_FUNCTION = QObject::tr("tc_main");
    typedef void (*MatrixInputFunction)(Matrix);

    class OutputWindow;
    class NodeGraphicsItem;
    class ConnectionGraphicsItem;
    class TextItem;
    class OpTextItem;
    class GraphicsScene;
    class ItemHandle;
    class ItemFamily;
    class MainWindow_FtoS;
    class NetworkWindow;
    class TextEditor;
    class Tool;
    class HistoryStack;

    /*! \brief The MainWindow contains a set of GraphicScenes and/or TextEditors.
         Each GraphicsScene and TextEditor is contained inside a NetworkWindow.
         All three of these classes emit various signals. Those signals are relayed by the MainWindow.
         In this way, a Tool does not need to listen to each of the GraphicsScene and TextEditor signals
         but only the MainWindow's signals.	 The MainWindow also has its own signals, such as a toolLoaded, modelSaved, etc.
         The MainWindow keeps a list of all plugins, and it is also responsible for loading plugins.
         \ingroup core
        */
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:

        /*!
                * \brief Defaut constructor: initialize menubars, create first graphics scene.
                * \param bool enable text-based network construction
                * \param bool enable graphics-based network construction
                */
        MainWindow();
        /*!
        * \brief 2-arg constructor allows disabling of text/graphics modes
        * \param bool enable text-based network construction
        * \param bool enable graphics-based network construction
        */
        MainWindow(bool enableScene, bool enableText);
        /*!
        * \brief Destructor: delete all the graphics scenes.
        */
        virtual ~MainWindow();
        /*!
        * \brief The TinkerCell user directory, which is $HOME/Tinkercell by default, but users may change this setting
        */
        static QString userHome();
        /*!
        * \brief add a new docking window to the main window.
        * \param QString the name of the new tool (the title of the docking window)
        * \param Tool* the new tool
        * \param Qt::DockWidgetArea the initial docking area
        * \param Qt::DockWidgetAreas the allowed docking areas
        * \param bool place the docking window in the view menu
        * \return QDockWidget* the new docking window
        */
        QDockWidget * addDockingWindow(const QString& name, QWidget * tool, Qt::DockWidgetArea initArea = Qt::LeftDockWidgetArea, Qt::DockWidgetAreas allowedAreas = Qt::AllDockWidgetAreas, bool inMenu = true);
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
        * \return current scene
        */
        GraphicsScene * currentScene();
        /*!
        * \brief gets the text editor that is active
        * \return current scene
        */
        TextEditor * currentTextEditor();
        /*!
        * \brief gets the current window that is active
        * \return current MDI window
        */
        NetworkWindow * currentWindow();
        /*!
        * \brief gets all the windows in the main window
        * \return list of windows
        */
        QList<NetworkWindow*> allWindows();
        /*!
        * \brief the history stack of the current scene.
        * \return current scene's history stack or null if current scene is null
        */
        QUndoStack * historyStack();
        /*!
        * \brief get a tool
        * \param QString name of the tool
        */
        virtual Tool * tool(const QString&) const;
        /*!
        * \brief get all tools
        */
        virtual QList<Tool*> tools() const;
        /*!
        * \brief the set of all docking windows in the main window
        */
        QList<QDockWidget*> dockedWindows;
        /*!
        * \brief the menu for settings such as default plugins, Tinkercell home directory, etc.
        */
        QMenu* settingsMenu;
        /*!
        * \brief the context menu that is shown during right-click event on an item.
                        Plugins can add new actions to this menu.
        */
        QMenu contextItemsMenu;
        /*!
        * \brief the context menu that is shown during right-click event on the scene.
                        Plugins can add new actions to this menu.
        */
        QMenu contextScreenMenu;
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

    public slots:
        /*!
        * \brief asks user for a new directory to be used as the user home directory (must be writtable)
        */
        void setUserHome();
        /*!
        * \brief create new scene
        */
        void newGraphicsWindow();
        /*!
        * \brief create new text editor
        */
        void newTextWindow();
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
        * \brief triggered when the open button is clicked. Opens a file dialog and emits the open signal.
                        The main window does not implement an function for opening a new file
        */
        void open();
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
        * \brief adjusts the current's scene's view rectangle to fit all the selected items
        */
        void fitSelected();
        /*!
        * \brief change the current view -- tabbed view, multi-window view, etc.
        */
        void changeView();
        /*!
        * \brief sends a signal to all plugins telling them to exit their current processes.
        */
        void sendEscapeSignal(const QWidget * w = 0);
        /*!
        * \brief sends a signal indicating that the current scene has changed
        */
        void windowChanged(QMdiSubWindow*);
    private slots:
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
        * \brief new nodes or connections have been inserted
        * \param TextEditor* where the editting happened
        * \param QList<TextItem*> new test items
        * \param QList<TextItem*> new item handles*/
        void itemsInsertedSlot(TextEditor * editor, const QList<TextItem*>& , const QList<ItemHandle*>&);
        /*!
        * \brief nodes or connections have been removed
        * \param TextEditor* where the editting happened
        * \param QList<TextItem*> removed text items
        * \param QList<TextItem*> removed item handles
        */
        void itemsRemovedSlot(TextEditor * editor, const QList<TextItem*>& , const QList<ItemHandle*>& );
        /*! \brief emit items removed */
        /*!
        * \brief informs the plugins that the current window is about to close
        * \param Boolean setting to false will prevent this window from closing
        */
        void emitWindowClosing(bool*);
        /*!
        * \brief send signal to other tools so that they can connect functions to signals
        * \param QSemaphore* semaphore
        * \param QLibrary * the dynamic library instance
        * \return void
        */
        void setupFunctionPointersSlot(QSemaphore*,QLibrary *);
        /*!
        * \brief zoom or unzoom. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param factor
        * \return void
        */
        void zoom(QSemaphore*,qreal);
        /*!
        * \brief gets name of given item. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param return value
        * \param item pointer
        * \return void
        */
        void itemName(QSemaphore*,QString*,ItemHandle*);
        /*!
        * \brief set name of given item. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param QString new name
        * \param item pointer
        * \return void
        */
        void setName(QSemaphore*,ItemHandle*,const QString&);
        /*!
        * \brief gets names of given items. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param return value
        * \param item pointers
        * \return void
        */
        void itemNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
        /*!
        * \brief gets family of given item. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param return value
        * \param item pointer
        * \return void
        */
        void itemFamily(QSemaphore*,QString*,ItemHandle*);
        /*!
        * \brief Checks whether the given item belongs to the given family. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param return value
        * \param item pointer
        * \param name of family
        * \return void
        */
        void isA(QSemaphore*,int*,ItemHandle*,const QString& );
        /*!
        * \brief Finds the first graphics item with the name. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param return value
        * \param name of item
        * \return void
        */
        void findItem(QSemaphore*,ItemHandle**,const QString& name);
        /*!
        * \brief selects the given item. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param item to select value
        * \return void
        */
        void select(QSemaphore*,ItemHandle* item);
        /*!
        * \brief deselects all selected items. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \return void
        */
        void deselect(QSemaphore*);
        /*!
        * \brief returns a list of currently selected items. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param return value
        * \return void
        */
        void selectedItems(QSemaphore*,QList<ItemHandle*>*);
        /*!
        * \brief returns a list of all items. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param return value
        * \return void
        */
        void allItems(QSemaphore*,QList<ItemHandle*>*);
        /*!
        * \brief returns a list of items of the specified family. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param return value
        * \param family to filter by
        * \return void
        */
        void itemsOfFamily(QSemaphore*,QList<ItemHandle*>*,const QString&);
        /*!
        * \brief sets the x,y position of the item. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param ItemHandle* item pointer
        * \param double  new x position
        * \param double new y position
        * \return void
        */
        void setPos(QSemaphore*,ItemHandle* item, qreal X, qreal Y);
        /*!
        * \brief sets the x,y position of several items. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param QList<ItemHandle*> item pointers
        * \param DataTable<qreal> positions (n x 2 matrix)
        * \return void
        */
        void setPos(QSemaphore*,const QList<ItemHandle*>& items, DataTable<qreal>& pos);
        /*!
        * \brief gets the x position of an item. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param return variable
        * \param item pointer
        * \return void
        */
        void getX(QSemaphore*,qreal*,ItemHandle* item);
        /*!
        * \brief gets the y position of an item. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param return variable
        * \param item pointer
        * \return void
        */
        void getY(QSemaphore*,qreal*,ItemHandle* item);
        /*!
        * \brief gets the x and y position of items. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param QList<ItemHandle*> item pointers
        * \param DataTable return variable (n x 2)
        * \return void
        */
        void getPos(QSemaphore*,const QList<ItemHandle*>& item, DataTable<qreal>* pos);
        /*!
        * \brief removes the given item. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param item pointer
        * \return void
        */
        void removeItem(QSemaphore*,ItemHandle* item);
        /*!
        * \brief moves all selected items. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param delta x
        * \param delta y
        * \return void
        */
        void moveSelected(QSemaphore*,qreal x, qreal y);
        /*!
        * \brief show text in output window. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \return void
        */
        void outputText(QSemaphore*,const QString&);
        /*!
        * \brief clear text in output window. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \return void
        */
        void clearText(QSemaphore* sem);
        /*!
        * \brief show text in output window as error message. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \return void
        */
        void errorReport(QSemaphore*,const QString&);
        /*!
        * \brief show text in file in output window. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \return void
        */
        void printFile(QSemaphore*,const QString&);
        /*!
        * \brief show table in output window. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \return void
        */
        void outputTable(QSemaphore*,const DataTable<qreal>&);
        /*!
        * \brief make a new input window. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \return void
        */
        void createInputWindow(QSemaphore*,const DataTable<qreal>&,const QString&,const QString&,const QString&);
        /*!
        * \brief make a new input window. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \return void
        */
        void createInputWindow(QSemaphore*,const DataTable<qreal>&,const QString&,MatrixInputFunction);
        /*!
        * \brief change a input window. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \return void
        */
        void addInputWindowOptions(QSemaphore*,const QString& name, int i, int j, const QStringList&);
        /*!
        * \brief opens a new window. This function is designed to be used with the C API framework
        * \param QSemaphore * semaphore
        * \param return value
        * \param item pointer
        * \return void
        */
        void openNewWindow(QSemaphore*,const QString&);
        /*!
        * \brief returns 1 if current OS is Windows. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param int* Boolean (1 or 0)
        * \return void
        */
        void isWindows(QSemaphore*,int*);
        /*!
        * \brief returns 1 if current OS is Mac. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param int*  Boolean (1 or 0)
        * \return void
        */
        void isMac(QSemaphore*,int*);
        /*!
        * \brief returns 1 if current OS is Linux. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param int*  Boolean (1 or 0)
        * \return void
        */
        void isLinux(QSemaphore*,int*);
        /*!
        * \brief returns the application directory. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param QString*  place to store the directory
        * \return void
        */
        void appDir(QSemaphore*,QString*);
        /*!
        * \brief returns the name of all data entries for given item. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param QStringList*  return value
        * \param ItemHandle* item to change
        * \return void
        */
        void getNumericalDataNames(QSemaphore*,QStringList*,ItemHandle*);
        /*!
        * \brief returns the name of all data entries for given item. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param QStringList*  return value
        * \param ItemHandle* item to change
        * \return void
        */
        void getStringDataNames(QSemaphore*,QStringList*,ItemHandle*);
        /*!
        * \brief returns the data value. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param qreal*  return value
        * \param ItemHandle* item to change
        * \param QString tool name
        * \param QString row name
        * \param QString column name
        * \return void
        */
        void getNumericalData(QSemaphore*,qreal*,ItemHandle*,const QString&, const QString&, const QString&);
        /*!
        * \brief returns the data value. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param qreal*  return value
        * \param ItemHandle* item to change
        * \param QString tool name
        * \param QString row name
        * \param QString column name
        * \return void
        */
        void getStringData(QSemaphore*,QString*,ItemHandle*,const QString&, const QString&, const QString&);
        /*!
        * \brief returns the data headers. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param QStringList*  return value
        * \param ItemHandle* item
        * \param QString tool name
        * \return void
        */
        void getNumericalDataRows(QSemaphore*,QStringList*,ItemHandle*,const QString&);
        /*!
        * \brief returns the data headers. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param QStringList*  return value
        * \param ItemHandle* item
        * \param QString tool name
        * \return void
        */
        void getNumericalDataCols(QSemaphore*,QStringList*,ItemHandle*,const QString&);
        /*!
        * \brief returns the data headers. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param QStringList*  return value
        * \param ItemHandle* item
        * \param QString tool name
        * \return void
        */
        void getStringDataRows(QSemaphore*,QStringList*,ItemHandle*,const QString&);
        /*!
        * \brief returns the data headers. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param QStringList*  return value
        * \param ItemHandle* item
        * \param QString tool name
        * \return void
        */
        void getStringDataCols(QSemaphore*,QStringList*,ItemHandle*,const QString&);
        /*!
        * \brief returns the data matrix. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param DataTable* return value
        * \param ItemHandle* item
        * \param QString tool name
        * \return void
        */
        void getNumericalDataMatrix(QSemaphore*,DataTable<qreal>*,ItemHandle*,const QString&);
        /*!
        * \brief sets a data matrix for an item. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param ItemHandle* item
        * \param QString tool name
        * \param DataTable new table to insert
        * \return void
        */
        void setNumericalDataMatrix(QSemaphore* sem,ItemHandle* item,const QString& tool, const DataTable<qreal>& dat);
        /*!
        * \brief returns a data matrix row. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param QStringList* return value
        * \param ItemHandle* item
        * \param QString tool name
        * \return void
        */
        void getStringDataRow(QSemaphore*,QStringList*,ItemHandle*,const QString&,const QString&);
        /*!
        * \brief returns a data matrix col. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param QStringList* return value
        * \param ItemHandle* item
        * \param QString tool name
        * \return void
        */
        void getStringDataCol(QSemaphore*,QStringList*,ItemHandle*,const QString&,const QString&);
        /*!
        * \brief sets the data value. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param ItemHandle* item to change
        * \param QString tool name
        * \param QString row name
        * \param QString column name
        * \param qreal value
        * \return void
        */
        void setNumericalData(QSemaphore*,ItemHandle*,const QString&, const QString&, const QString&,qreal);
        /*!
        * \brief sets the data value. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param ItemHandle* item to change
        * \param QString tool name
        * \param QString row name
        * \param QString column name
        * \param qreal value
        * \return void
        */
        void setStringData(QSemaphore*,ItemHandle*,const QString&, const QString&, const QString&,const QString&);
        /*!
        * \brief get children of an item. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param QList<ItemHandle*>* return value
        * \param ItemHandle* item
        * \return void
        */
        void getChildren(QSemaphore*,QList<ItemHandle*>*,ItemHandle*);
        /*!
        * \brief get parent of an item. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param ItemHandle** return value
        * \param ItemHandle* item
        * \return void
        */
        void getParent(QSemaphore*,ItemHandle**,ItemHandle*);
        /*!
        * \brief loads files (library files or model files)
        * \param QList<QFileInfo>& the name(s) of the file(s)
        * \return void
        */
        void dragAndDropFiles(const QList<QFileInfo>& files);
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
        * \brief signals when a window is going to closed
        * \param NetworkWindow *  the window that is closing
        * \param Boolean setting to false will prevent this window from closing
        * \return void
        */
        void windowClosing(NetworkWindow * window,bool*);
        /*!
        * \brief signals used inform that the model is going to be saved as it is
        * \param NetworkWindow *  the window where model was loaded (usually current scene)
        * \return void*/
        void prepareModelForSaving(NetworkWindow*);
        /*!
        * \brief signals used inform that the model has been saved
        * \param NetworkWindow *  the window where model was loaded (usually current scene)
        * \return void
        */
        void modelSaved(NetworkWindow*);
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
        * \param NetworkWindow *  the window where model was loaded (usually current scene)
        * \return void
        */
        void modelLoaded(NetworkWindow*);
         /*!
        * \brief signals whenever the new window is opened
        * \param NetworkWindow* the current new window
        * \return void
        */
        void windowOpened(NetworkWindow*);
        /*!
        * \brief signals whenever the current window changes
        * \param NetworkWindow* the previous windpw
        * \param NetworkWindow* the current new window
        * \return void
        */
        void windowChanged(NetworkWindow*,NetworkWindow*);
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
        * \brief new nodes or connections have been inserted
        * \param TextEditor* where the editting happened
        * \param QList<TextItem*> new test items
        * \param QList<TextItem*> new item handles*/
        void itemsInserted(TextEditor * editor, const QList<TextItem*>& , const QList<ItemHandle*>&);
        /*!
        * \brief nodes or connections have been removed
        * \param TextEditor* where the editting happened
        * \param QList<TextItem*> removed text items
        * \param QList<TextItem*> removed item handles
        */
        void itemsRemoved(TextEditor * editor, const QList<TextItem*>& , const QList<ItemHandle*>& );
        /*!
        * \brief A convenient signal that is emitted when items are inserted from a GraphicsScene
            or TextEditor. Warning: listening to the other itemsInserted signals may cause redundancy
        * \param TextEditor* where the editting happened
        * \param QList<TextItem*> new items
        */
        void itemsInserted(NetworkWindow * win, const QList<ItemHandle*>&);
        /*!
        * \brief A convenient signal that is emitted when items are removed from a GraphicsScene
            or TextEditor. Warning: listening to the other itemsRemoved signals may cause redundancy
        * \param TextEditor* where the editting happened
        * \param TextItem* new items
        */
        void itemsRemoved(NetworkWindow * win, const QList<ItemHandle*>& );
        /*!
        * \brief an operations, e.g. equations, has been inserted
        * \param TextEditor* where the editting happened
        * \param OpTextItem* operation
        */
        void operationInserted(TextEditor * editor, OpTextItem * );
        /*!
        * \brief one of the connections has been modified
        * \param TextEditor* where the editting happened
        * \param ConnectionTextItem* old connection
        * \param ConnectionTextItem* modified connection
        */
        void connectionChanged(TextEditor * editor, ConnectionTextItem * from, ConnectionTextItem * to);
        /*! \brief one of the operations, e.g. equations, has been modified
        * \param TextEditor* where the editting happened
        * \param OpTextItem* old operation
        * \param OpTextItem* modified operation */
        void operationChanged(TextEditor * editor, OpTextItem * from, OpTextItem * to);
        /*! \brief request to find the given text
            \param QString string to find*/
        void findText(const QString&);
        /*! \brief request to find and replace the given text
            \param QRegExp regex to find
            \param QString string to replace with*/
        void replaceText(const QRegExp&, const QString&);
        /*! \brief new text has been inserted
            \param QString new text
        */
        void textInserted(const QString&);
        /*! \brief some text has been removed
            \param QString new text
        */
        void textRemoved(const QString&);
        /*! \brief some text inside this editor has been changed
            \param QString old text
            \param QString new text
        */
        void textChanged(const QString&, const QString&);
        /*! \brief the cursor has moved to a different line
            \param int index of the current line
            \param QString current line text
        */
        void lineChanged(int, const QString&);
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
        * \param NetworkWindow * window where the event took place
        * \param QList<ItemHandle*>& items
        * \param QList<QString>& old names
        * \param QList<QString>& new names
        * \return void*/
        void itemsRenamed(NetworkWindow * window, const QList<ItemHandle*>& items, const QList<QString>& oldnames, const QList<QString>& newnames);
        /*! \brief signals whenever the handles for graphics items have changed
        * \param GraphicsScene* scene where the event took place
        * \param QList<GraphicsItem*>& items that are affected
        * \param QList<ItemHandle*>& old handle for each items
        * \return void*/
        void handlesChanged(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>& old);
        /*! \brief signals whenever item parent handle is changed
        * \param NetworkWindow * window where the event took place
        * \param QList<ItemHandle*>& child items
        * \param QList<ItemHandle*>& old parents
        * \return void*/
        void parentHandleChanged(NetworkWindow * scene, const QList<ItemHandle*>&, const QList<ItemHandle*>&);
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
        /*! \brief the loaded dynamic libraries indexed by file name*/
        QHash<QString,QLibrary*> dynamicallyLoadedLibraries;
        /*! \brief the general window for command, errors, and messages*/
        OutputWindow * outputWindow;
        /*! \brief the current layout */
        QGridLayout layout;
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
        * \param drag event
        * \return void*/
        void closeEvent(QCloseEvent *event);
        /*! \brief all windows in different windows*/
        QMdiArea mdiArea;
        /*! \brief history window*/
        HistoryStack historyWindow;
        /*! \brief keep pointer to previous scene for windowChanged events*/
        NetworkWindow * prevWindow;
        /*! \brief tools (plug-ins)*/
        QHash<QString,Tool*> toolsHash;
    private:
        /*! \brief used to rename items*/
        QLineEdit * renameOld;
        /*! \brief used to rename items*/
        QLineEdit * renameNew;
        /*!
                * \brief part of the C API framework.
                */
        static void _zoom(double);
        /*!
                * \brief part of the C API framework.
                */
        static void _deleteArray(Array);
        /*!
                * \brief part of the C API framework.
                */
        static void _deleteMatrix(Matrix);
        /*!
                * \brief part of the C API framework.
                */
        static void _deleteStrings(char**);
        /*!
                * \brief part of the C API framework.
                */
        static Array _allItems();
        /*!
                * \brief part of the C API framework.
                */
        static Array _itemsOfFamily(const char*);
        /*!
                * \brief part of the C API framework.
                */
        static Array _selectedItems();
        /*!
                * \brief part of the C API framework.
                */
        static OBJ _find(const char*);
        /*!
                * \brief part of the C API framework.
                */
        static void _select(OBJ);
        /*!
                * \brief part of the C API framework.
                */
        static void _deselect();
        /*!
                * \brief part of the C API framework.
                */
        static char* _getName(OBJ);
        /*!
                * \brief part of the C API framework.
                */
        static void _setName(OBJ,const char*);
        /*!
                * \brief part of the C API framework.
                */
        static char** _getNames(Array);
        /*!
                * \brief part of the C API framework.
                */
        static char* _getFamily(OBJ);
        /*!
                * \brief part of the C API framework.
                */
        static int _isA(OBJ,const char*);
        /*!
                * \brief part of the C API framework.
                */
        static void _removeItem(OBJ);
        /*!
                * \brief part of the C API framework.
                */
        static void _setPos(OBJ,double ,double );
        /*!
                * \brief part of the C API framework.
                */
        static void _setPos2(Array,Matrix);
        /*!
                * \brief part of the C API framework.
                */
        static Matrix _getPos(Array);
        /*!
                * \brief part of the C API framework.
                */
        static double _getY(OBJ);
        /*!
                * \brief part of the C API framework.
                */
        static double _getX(OBJ);
        /*!
                * \brief part of the C API framework.
                */
        static void _moveSelected(double ,double );
        /*!
                * \brief part of the C API framework.
                */
        static void _outputTable(Matrix m);
        /*!
                * \brief part of the C API framework.
                */
        static void _outputText(const char*);
        /*!
                * \brief part of the C API framework.
                */
        static void _errorReport(const char*);
        /*!
                * \brief part of the C API framework.
                */
        static void _clearText();
        /*!
                * \brief part of the C API framework.
                */
        static void _printFile(const char*);
        /*!
                * \brief part of the C API framework.
                */
        static void _createInputWindow1(Matrix, const char*, const char*,const char*);
        /*!
                * \brief part of the C API framework.
                */
        static void _createInputWindow2(Matrix, const char*, MatrixInputFunction);
        /*!
                * \brief part of the C API framework.
                */
        static void _addInputWindowOptions(const char*, int i, int j, char **);
        /*!
                * \brief part of the C API framework.
                */
        static void _openNewWindow(const char*);
        /*!
                * \brief part of the C API framework.
                */
        static int _isWindows();
        /*!
                * \brief part of the C API framework.
                */
        static int _isMac();
        /*!
                * \brief part of the C API framework.
                */
        static int _isLinux();
        /*!
                * \brief part of the C API framework.
                */
        static char* _appDir();
        /*!
                * \brief part of the C API framework.
                */
        static char** _getNumericalDataNames(OBJ);
        /*!
                * \brief part of the C API framework.
                */
        static char** _getStringDataNames(OBJ);
        /*!
                * \brief part of the C API framework.
                */
        static double _getNumericalData(OBJ,const char*, const char*, const char*);
        /*!
                * \brief part of the C API framework.
                */
        static char* _getStringData(OBJ,const char*, const char*, const char*);
        /*!
                * \brief part of the C API framework.
                */
        static char** _getNumericalDataRows(OBJ,const char*);
        /*!
                * \brief part of the C API framework.
                */
        static char** _getNumericalDataCols(OBJ,const char*);
        /*!
                * \brief part of the C API framework.
                */
        static char** _getStringDataRows(OBJ,const char*);
        /*!
                * \brief part of the C API framework.
                */
        static char** _getStringDataCols(OBJ,const char*);
        /*!
                * \brief part of the C API framework.
                */
        static Matrix _getNumericalDataMatrix(OBJ,const char*);
        /*!
                * \brief part of the C API framework.
                */
        static void _setNumericalDataMatrix(OBJ, const char *, Matrix);
        /*!
                * \brief part of the C API framework.
                */
        static char** _getStringDataRow(OBJ,const char*,const char*);
        /*!
                * \brief part of the C API framework.
                */
        static char** _getStringDataCol(OBJ,const char*,const char*);
        /*!
                * \brief part of the C API framework.
                */
        static void _setNumericalData(OBJ,const char*, const char*, const char*,double);
        /*!
                * \brief part of the C API framework.
                */
        static void _setStringData(OBJ,const char*, const char*, const char*,const char*);
        /*!
                * \brief part of the C API framework.
                */
        static Array _getChildren(OBJ);
        /*!
                * \brief part of the C API framework.
                */
        static OBJ _getParent(OBJ);
        /*!
                * \brief part of the C API framework. Converts static functions to signals
                */
        static MainWindow_FtoS fToS;
        /*!
                * \brief initializes all the functions in the fToS object
                */
        void connectTCFunctions();
        /*!
                * \brief stores the last opened directory
                */
        static QString previousFileName;

    };



    /*! \brief Function to Signal converter for MainWindow*/
    class MainWindow_FtoS : public QObject
    {
        Q_OBJECT
                signals:
        void allItems(QSemaphore*,QList<ItemHandle*>*);
        void selectedItems(QSemaphore*,QList<ItemHandle*>*);
        void itemsOfFamily(QSemaphore*,QList<ItemHandle*>*,const QString&);
        void find(QSemaphore*,ItemHandle**,const QString&);
        void select(QSemaphore*,ItemHandle*);
        void deselect(QSemaphore*);
        void removeItem(QSemaphore*,ItemHandle* );
        void setPos(QSemaphore*,ItemHandle* ,qreal ,qreal );
        void setPos(QSemaphore*,const QList<ItemHandle*>& , DataTable<qreal>&);
        void getPos(QSemaphore*,const QList<ItemHandle*>& , DataTable<qreal>*);
        void getY(QSemaphore*,qreal*,ItemHandle* );
        void getX(QSemaphore*,qreal*,ItemHandle* );
        void moveSelected(QSemaphore*,qreal ,qreal );
        void getFamily(QSemaphore*,QString*,ItemHandle* );
        void getName(QSemaphore*,QString*,ItemHandle* );
        void setName(QSemaphore*,ItemHandle*,const QString&);
        void getNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
        void isA(QSemaphore*,int*,ItemHandle*, const QString&);
        void outputText(QSemaphore*,const QString&);
        void errorReport(QSemaphore*,const QString&);
        void printFile(QSemaphore*,const QString&);
        void clearText(QSemaphore*);
        void outputTable(QSemaphore*,const DataTable<qreal>&);
        void createInputWindow(QSemaphore*,const DataTable<qreal>&, const QString&,const QString&,const QString&);
        void createInputWindow(QSemaphore*,const DataTable<qreal>&, const QString &, MatrixInputFunction);
        void addInputWindowOptions(QSemaphore*, const QString&, int i, int j, const QStringList&);
        void openNewWindow(QSemaphore*,const QString&);
        void isWindows(QSemaphore*,int*);
        void isMac(QSemaphore*,int*);
        void isLinux(QSemaphore*,int*);
        void appDir(QSemaphore*,QString*);
        void zoom(QSemaphore*,qreal);

        void getNumericalDataNames(QSemaphore*,QStringList*,ItemHandle*);
        void getStringDataNames(QSemaphore*,QStringList*,ItemHandle*);
        void getNumericalData(QSemaphore*,qreal*,ItemHandle*,const QString&, const QString&, const QString&);
        void getStringData(QSemaphore*,QString*,ItemHandle*,const QString&, const QString&, const QString&);
        void setNumericalData(QSemaphore*,ItemHandle*,const QString&, const QString&, const QString&,qreal);
        void setStringData(QSemaphore*,ItemHandle*,const QString&, const QString&, const QString&,const QString&);

        void getNumericalDataRows(QSemaphore*,QStringList*,ItemHandle*,const QString&);
        void getNumericalDataCols(QSemaphore*,QStringList*,ItemHandle*,const QString&);
        void getStringDataRows(QSemaphore*,QStringList*,ItemHandle*,const QString&);
        void getStringDataCols(QSemaphore*,QStringList*,ItemHandle*,const QString&);

        void getNumericalDataMatrix(QSemaphore*,DataTable<qreal>*,ItemHandle*,const QString&);
        void setNumericalDataMatrix(QSemaphore*,ItemHandle*,const QString&,const DataTable<qreal>&);
        void getStringDataRow(QSemaphore*,QStringList*,ItemHandle*,const QString&,const QString&);
        void getStringDataCol(QSemaphore*,QStringList*,ItemHandle*,const QString&,const QString&);


        void getChildren(QSemaphore*,QList<ItemHandle*>*,ItemHandle*);
        void getParent(QSemaphore*,ItemHandle**,ItemHandle*);

                public slots:
        void zoom(double);
        Array allItems();
        Array itemsOfFamily(const char*);
        Array selectedItems();
        OBJ find(const char*);
        void select(OBJ);
        void deselect();
        char* getName(OBJ);
        void setName(OBJ,const char*);
        char** getNames(Array);
        char* getFamily(OBJ);
        int isA(OBJ,const char*);
        void removeItem(OBJ);
        void setPos(OBJ,double ,double );
        void setPos(Array,Matrix);
        Matrix getPos(Array);
        double getY(OBJ);
        double getX(OBJ);
        void moveSelected(double ,double );
        void outputTable(Matrix m);
        void outputText(const char*);
        void errorReport(const char*);
        void clearText();
        void printFile(const char*);
        void createInputWindow(Matrix, const char*, const char*,const char*);
        void createInputWindow(Matrix, const char*, MatrixInputFunction);
        void addInputWindowOptions(const char*, int i, int j, char **);
        void openNewWindow(const char*);
        int isWindows();
        int isMac();
        int isLinux();
        char* appDir();

        char** getNumericalDataNames(OBJ);
        char** getStringDataNames(OBJ);

        double getNumericalData(OBJ,const char*, const char*, const char*);
        char* getStringData(OBJ,const char*, const char*, const char*);

        char** getNumericalDataRows(OBJ,const char*);
        char** getNumericalDataCols(OBJ,const char*);
        char** getStringDataRows(OBJ,const char*);
        char** getStringDataCols(OBJ,const char*);

        Matrix getNumericalDataMatrix(OBJ,const char*);
        void setNumericalDataMatrix(OBJ,const char*, Matrix);
        char** getStringDataRow(OBJ,const char*,const char*);
        char** getStringDataCol(OBJ,const char*,const char*);

        void setNumericalData(OBJ,const char*, const char*, const char*,double);
        void setStringData(OBJ,const char*, const char*, const char*,const char*);
        Array getChildren(OBJ);
        OBJ getParent(OBJ);
    };

}

#endif
