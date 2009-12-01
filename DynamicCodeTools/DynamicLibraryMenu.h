/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)

 see COPYRIGHT.TXT

 Provides a toolbar with buttons that call C functions (run of separate threads)

****************************************************************************/

#ifndef TINKERCELL_FUNCTIONSMENUTOOL_H
#define TINKERCELL_FUNCTIONSMENUTOOL_H

#include <QPair>
#include <QMainWindow>
#include <QHash>
#include <QToolButton>
#include <QDialog>
#include <QTreeWidget>
#include <QSemaphore>
#include <QListWidget>
#include <QListWidgetItem>
#include <QThread>
#include <QMenu>
#include <QLibrary>
#include <QLabel>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include "ItemHandle.h"
#include "Tool.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

    /*!
          \brief A "middle man" class that converts static C functions to signals. Part of the generic TinkerCell C interface protocol.
        */
    class MY_EXPORT DynamicLibraryMenu_FToS : public QObject
    {
        Q_OBJECT
     signals:
        /*! \brief Part of the TinkerCell C interface. */
        void getString(QSemaphore*,QString*,const QString&);
        /*! \brief Part of the TinkerCell C interface. */
        void getFilename(QSemaphore*,QString*);
        /*! \brief Part of the TinkerCell C interface. */
        void getSelectedString(QSemaphore*,int*,const QString&,const QStringList&,const QString&,int);
        /*! \brief Part of the TinkerCell C interface. */
        void getNumber(QSemaphore*,qreal*,const QString&);
        /*! \brief Part of the TinkerCell C interface. */
        void getNumbers(QSemaphore*,const QStringList&,qreal*);
        /*! \brief Part of the TinkerCell C interface. */
        void callFunction(QSemaphore*,const QString&);
     public slots:
        /*! \brief Part of the TinkerCell C interface. */
        char* getString(const char*);
        /*! \brief Part of the TinkerCell C interface. */
        char* getFilename();
        /*! \brief Part of the TinkerCell C interface. */
        int getSelectedString(const char*, char**,const char*,int);
        /*! \brief Part of the TinkerCell C interface. */
        double getNumber(const char*);
        /*! \brief Part of the TinkerCell C interface. */
        void getNumbers(char**, double*);
        /*! \brief Part of the TinkerCell C interface. */
        void callFunction(const char*);
    };

    /*!
          \brief Provides the widgets and functions for exposing generic functions to the user. This class is primarily meant for exposing
                third-part C functions (or Python, etc.). This class works in conjunction with other classes, such as the LoadCLibraries class.
                        This class provides methods for adding tool buttons to the functions tree and actions to the main toolbar. It also contains methods
                        for displaying graphical items or actions in the context menu. The supporting class needs to provide the functions that are triggered
                        as a response to these actions and tool buttons.
                \sa LoadCLibraries
        */
    class MY_EXPORT DynamicLibraryMenu : public Tool
    {
        Q_OBJECT

    public:
        /*! \brief default constructor
        */
        DynamicLibraryMenu();
        /*! \brief destructor. deletes all the graphical tools
        */
        virtual ~DynamicLibraryMenu();
        /*!
        * \brief sets the main window. Connects to itemsSelected
        * \param MainWindow main window
        */
        bool setMainWindow(MainWindow*);
        /*!
        * \brief add a new function to the tree of functions.
        * \param QString category that this function belongs in, e.g. "Simulate"
        * \param QString function title
        * \param QIcon optional icon
        * \return QToolButton* the button that was added
        */
        QToolButton * addFunction(const QString& category, const QString& functionName, const QIcon& icon = QIcon());
        /*!
        * \brief add a new action to the functions button in the main toolbar
        * \param QString function title
        * \param QIcon optional icon
        * \return QAction* the action that was added
        */
        QAction * addMenuItem(const QString& category, const QString& functionName, const QIcon& icon = QIcon(), bool deft = false);
        /*!
        * \brief add an action to the context menu (right mouse button) for items of the given family
        * \param QString family that this function targets
        * \param QString function title
        * \param QIcon optional icon
        * \return QAction* the action added to the context menu
        */
        QAction * addContextMenuItem(const QString& familyName,const QString& functionName, const QPixmap& icon = QPixmap(), bool tool = false);

		/*! \brief the preferred size for this window*/
		QSize sizeHint() const;

    protected slots:
        void select(int i=0);
        void deselect(int i=0);
        void itemsInserted(NetworkWindow* , const QList<ItemHandle*>& handles);
        void itemsSelected(GraphicsScene * scene,const QList<QGraphicsItem*>& items, QPointF, Qt::KeyboardModifiers);
        /*!
        * \brief action in the tool bar menu sets the default action
        * \param QAction* action
        */
        void actionTriggered ( QAction *  action );
        /*!
        * \brief setup the functions for the new C library. Part of the generic TinkerCell C interface protocol
        * \param QLibrary library that was loaded
        */
        void setupFunctionPointers( QLibrary * );
        /*!
        * \brief Get string from user. Part of the TinkerCell C interface.
        */
        void getString(QSemaphore*,QString*,const QString&);
        /*!
        * \brief Get string from user from a list. Part of the TinkerCell C interface.
        */
        void getSelectedString(QSemaphore*, int*, const QString&, const QStringList&, const QString&, int);
        /*!
        * \brief Get a number from user. Part of the TinkerCell C interface.
        */
        void getNumber(QSemaphore*,qreal*,const QString&);
        /*!
        * \brief Get more than one number from user. Part of the TinkerCell C interface.
        */
        void getNumbers(QSemaphore*,const QStringList&,qreal*);
        /*!
        * \brief Get file name from user. Part of the TinkerCell C interface.
        */
        void getFilename(QSemaphore*,QString*);
        /*!
        * \brief Call a function listed in the functions table. Part of the TinkerCell C interface.
        */
        void callFunction(QSemaphore*,const QString&);

    private:
        /*! \brief Part of the TinkerCell C interface. */
        static DynamicLibraryMenu_FToS fToS;
        /*! \brief Part of the TinkerCell C interface. */
        static char* _getString(const char*);
        /*! \brief Part of the TinkerCell C interface. */
        static int _getSelectedString(const char*, char**,const char*, int);
        /*! \brief Part of the TinkerCell C interface. */
        static double _getNumber(const char*);
        /*! \brief Part of the TinkerCell C interface. */
        static void _getNumbers(char**, double *);
        /*! \brief Part of the TinkerCell C interface. */
        static char* _getFilename();
        /*! \brief Part of the TinkerCell C interface. */
        static void _callFunction(const char*);
        /*! \brief Dialog for selecting strings. */
        QDialog * getStringDialog;
        /*! \brief widget for selecting strings. */
        QListWidget getStringList;
        /*! \brief number for selecting strings. */
        int getStringListNumber;
        /*! \brief list for selecting numbers. */
        QStringList getStringListText;
        /*! \brief label for selecting numbers. */
        QLabel getStringListLabel;

    private slots:
        /*! \brief Stores the index that the user selected from a list of strings
                        \sa getSelectedString
                */
        void getStringListItemSelected(QListWidgetItem *);
        /*! \brief Stores the index that the user selected from a list of strings
                        \sa getSelectedString
                */
        void getStringListRowChanged(int);
        /*! \brief Negates the index that the user selected from a list of strings
                        \sa getSelectedString
                */
        void getStringListCanceled();
        /*! \brief Searches the list of strings displayed to user
                        \sa getSelectedString
                */
        void getStringSearchTextEdited(const QString & text);

    protected:
        /*! \brief Connects the "middle man" class to the this class. Part of the genetic TinkerCell C interface protocol.*/
        void connectTCFunctions();
        /*! \brief The menu with the functions that is placed in the main window's toolbar*/
        QMenu functionsMenu;
        /*! \brief The menu with the functions that is placed in the main window's toolbar*/
        QList<QMenu*> functionsSubMenus;
        /*! \brief The tree widget with all the functions in categories*/
        QTreeWidget treeWidget;
        /*! \brief The menu button with the functions that is placed in the main window's toolbar*/
        QToolButton * menuButton;
        /*! \brief The action group stores all the actions in the functionsMenu in order to update the default action of the menuButton*/
        QActionGroup actionGroup;
        /*! \brief Hash table that stores the functions in the tree widget indexed by their name. Used for callFunction method*/
        QHash<QString,QToolButton*> hashFunctionButtons;

        /*! \brief A generic graphical tool class that triggers an action when selected.
                                This graphical tool is meant to serve as a user interface for C and other (Python, etc.) functions*/
        class GraphicalActionTool : public Tool::GraphicsItem
        {
        public:
            /*! \brief constructor
                \param QPixmap icon for the tool*/
            GraphicalActionTool(const QString& family, const QString& name, const QPixmap& pixmap, Tool*);
            /*! \brief triggered when user selects this graphical tool*/
            void select();
			/*! \brief show this graphics item if the selected items belong in the corresponding family*/
			void visible(bool);
            /*! \brief action triggered by this graphical tool*/
            QAction targetAction;
			/*! \brief the target family for this graphics item*/
			QString targetFamily;
        };
        /*! \brief list of all graphical tools and their target families (also used for context menu)*/
        QList< QPair<QString,GraphicalActionTool*> > graphicalTools;
        /*! \brief which graphical tools to show or not show*/
        QList< bool > showGraphicalTool;
        /*! \brief separator for the context menu (mouse right click)*/
        QAction * separator;
    };
}

#endif
