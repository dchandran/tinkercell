/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 Tool for displaying the NodesTree and selecting the nodes on that tree.
 This tool also stores the tree of node families as a hashtable of <name,family> pairs.

****************************************************************************/

#ifndef TINKERCELL_PARTSTREEWIDGET_H
#define TINKERCELL_PARTSTREEWIDGET_H

#include <QWidget>
#include <QHash>
#include <QMap>
#include <QIcon>
#include <QPixmap>
#include <QToolButton>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QActionGroup>
#include "MainWindow.h"
#include "Tool.h"
#include "NetworkHandle.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ItemHandle.h"
#include "NodesTreeReader.h"
#include "TreeButton.h"
#include "CatalogWidget.h"

namespace Tinkercell
{

    class MainWindow;
    class NodeFamily;
    class FamilyTreeButton;

    /*!
      \brief This class loads the hierarchy of nodes along with their attributes, units, and other
      information from the NodesTree.xml file. A hash table with NodeFamilies is created, which can
      then be used by other tools to obtain the set of available node families and the set of
      attributes belonging to each family.
      \ingroup plugins
    */
    class TINKERCELLEXPORT NodesTree : public Tool
    {
        Q_OBJECT

    private:
        /*! \brief family name to NodeFamily hash*/
        QMap<QString,NodeFamily*> nodeFamilies;

        /*! \brief family name to tree items hash. These are the buttons located in the nodes tree window*/
        QHash<QString,QTreeWidgetItem*> treeItems;
		
		/*! \brief family name to button hash. These are the buttons located in the nodes tree window*/
        QHash<QString,QToolButton*> treeButtons;
        
        friend class NodesTreeReader;
        friend class CatalogWidget;

	public:
	
		/*! \brief folder with the theme to use*/
        static QString themeDirectory;
	
	    /*! \brief get a node family
        \param QString name of node family
        \return NodeFamily* can be 0 if none is found
         */
		NodeFamily * getFamily(const QString& name) const;
		
 	 	 /*! \brief insert a new node family
        \param NodeFamily * new family
        \param FamilyTreeButton* button
        \return bool returns false if the family already existed
         */
		bool insertFamily(NodeFamily *, FamilyTreeButton * );
		
		 /*! \brief get names of all families (sorted)
         \return QStringList family names*/
		QStringList getAllFamilyNames() const;

        /*! \brief default constructor
        \param Widget* parent widget
        \param QString filename from which to load heirarchy of nodes. default = NodesTree/NodesTree.xml
         */
        NodesTree(QWidget * parent = 0, const QString& filename = QString());
        
        /*! \brief load node families from XML file using NodesTreeReader
        \param QString filename
         */
        void readTreeFile(const QString& filename);

        /*! \brief load this tool into the main window
           \param MainWindow* the TinkerCell main window
        */
        bool setMainWindow(MainWindow *);

        /*! \brief get the name of a node family's icon from its name.
            Icon files are NodeItems/nodename.png, where nodename is the node family name.
           \param QString node family name
        */
        QString iconFile(QString name);

        /*! \brief get the name of a node family's node graphics file from its name.
            Graphics files are NodeItems/nodename.xml, where nodename is the node family name.
           \param QString node family name
        */
        QString nodeImageFile(QString name);

        /*! \brief get the tree widget located inside this Tool.
        */
        QTreeWidget & widget();

        /*! \brief destructor*/
        ~NodesTree();

        /*! \brief default window size*/
        QSize sizeHint() const;

    public slots:
        /*! \brief one of the buttons in the tree of nodes has been pressed
        \param NodeFamily* the family represented by the button*/
        void buttonPressed(NodeFamily * node);
        /*! \brief load a new hierarchy of nodes from a new xml file*/
        void changeTree();

    signals:
        /*! \brief one of the nodes in the tree has been selected*/
        void nodeSelected(NodeFamily* nodeFamily);
        /*! \brief key pressed inside this widget*/
        void keyPressed(int, Qt::KeyboardModifiers);
        /*! \brief exit the current operation*/
        void sendEscapeSignal(const QWidget*);

    protected:
        /*! \brief sends escape signal if ESC or SPACE is pressed*/
        //void keyPressEvent ( QKeyEvent * event );
        /*! \brief context menu is used to change tree file*/
        void contextMenuEvent(QContextMenuEvent * event);

    private slots:
        /*! \brief new node graphics xml file selected by user*/
        void nodeFileAccepted();
        /*! \brief user requested changing the nodes tree xml file*/
        void selectNewNodeFile();
		/*! \brief enter pressed or double clicked*/
		void itemActivated( QListWidgetItem * );
        /*! \brief one of the network is closing*/
        void networkClosing(NetworkHandle * , bool *);
        /*! \brief select theme for nodes and connection arrows*/
		void selectTheme(QAbstractButton * button);
    private:

        /*! \brief replaces the nodes graphics file with a new one*/
        QString replaceNodeFile();
        /*! \brief dialog for replacing a node family's graphics file*/
        void makeNodeSelectionDialog();

        /*! \brief tree widget that holds the buttons for each node family*/
        QTreeWidget treeWidget;
        /*! \brief the arrow button at the very top of the tree of nodes*/
        //QToolButton arrowButton;
        /*! \brief the tree item containing the arrow button at the very top of the tree of nodes*/
        //QTreeWidgetItem arrowItem;

        /*! \brief list of available node graphics files*/
        QListWidget * nodesListWidget;
        /*! \brief list of available node graphics files*/
        QStringList nodesFilesList;
        /*! \brief dialog for displaying the list of available node graphics files*/
        QDialog * nodeSelectionDialog;

        /*! \brief stores the previous graphics file name*/
        QString temporaryFileName;

        /*! \brief store all the node graphics file names*/
        QHash<QString,QString> nodeGraphicsFileNames;
        
        void setupThemesDialog();
        QDialog * selectThemesDialog;
        
        friend class FamilyTreeButton;
    };


}

#endif
