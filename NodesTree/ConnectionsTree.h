/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Tool for displaying the Connections Tree and selecting the nodes on that tree.
 This tool also stores the tree of connection families as a hashtable of <name,family> pairs.

****************************************************************************/

#ifndef TINKERCELL_CONNECTIONSTREEWIDGET_H
#define TINKERCELL_CONNECTIONSTREEWIDGET_H

#include <QWidget>
#include <QHash>
#include <QMap>
#include <QIcon>
#include <QPixmap>
#include <QToolButton>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QTreeWidget>
#include "MainWindow.h"
#include "Tool.h"
#include "ItemHandle.h"
#include "ConnectionGraphicsItem.h"
#include "ConnectionsTreeReader.h"
#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif
namespace Tinkercell
{

    class Widget;
    class MainWindow;
    class ConnectionFamily;

    /*!
      \brief This class loads the hierarchy of connections along with their attributes, units, and other
      information from the ConnectionsTree.xml file. A hash table with ConnectionFamilies is created,
      which can then be used by other tools to obtain the set of available connection families and the set of
      attributes belonging to each family.
      \ingroup plugins
    */
    class MY_EXPORT ConnectionsTree : public Tool
    {
        Q_OBJECT

    public:
        /*! \brief the hashtable of all connection families indexed by their name*/
        QMap<QString,ConnectionFamily*> connectionFamilies;
        /*! \brief the hashtable of all tree items indexed by their name*/
        QHash<QString,QTreeWidgetItem*> treeItems;
		/*! \brief the hashtable of all buttons on the tree widget indexed by their name*/
        QHash<QString,QToolButton*> treeButtons;
        /*! \brief Constructor - read the tree file and uses connection tree reader to fill in the tree
        \param the parent widget (usually 0)
        \param the XML file with the tree information
        */
        ConnectionsTree(QWidget * parent = 0, const QString& filename = QString());
        /*! \brief set the main window and dock this tool*/
        bool setMainWindow(MainWindow *);
        /*! \brief gets the icon file name for a family
        \param ConnectionFamily* family*/
        static QString iconFile(ConnectionFamily * family);
        /*! \brief gets the icon file name for a family
         \param QString family name*/
        static QString arrowImageFile(QString name);
        /*! \brief gets the tree widget that displays the connections*/
        QTreeWidget & widget();
        /*! \brief destructor -- removes all the families from memory*/
        ~ConnectionsTree();
        QSize sizeHint() const;
    public slots:
        /*! \brief this event is fired when one of the tree widget buttons is pressed. It sends a signal to the
        other tools with the selected family instead of the button*/
        void buttonPressed(ConnectionFamily*);
        /*! \brief request to change the connections tree hierarchy*/
        void changeTree();
    private slots:
        /*! \brief current window is closing*/
        void windowClosing(NetworkWindow * , bool *);
    signals:
        /*! \brief one of the items on the tree is selected (button pressed)*/
        void connectionSelected(ConnectionFamily* family);
        /*! \brief a key is pressed, if Esc, selection is cleared*/
        void keyPressed(int, Qt::KeyboardModifiers);
        /*! \brief selection is cleared*/
        void sendEscapeSignal(const QWidget*);
    protected:
        /*! \brief ESC or SPACE trigger escape signal*/
        //void keyPressEvent ( QKeyEvent * event );
        /*! \brief context menu displays the option to replace the tree file*/
        void contextMenuEvent(QContextMenuEvent * event);
        /*! \brief the tree widget that displays the family tree*/
        QTreeWidget treeWidget;
    };

}

#endif
