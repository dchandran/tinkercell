/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Displays the nodes and connection tree

****************************************************************************/
#ifndef TINKERCELL_CATALOGWIDGET_H
#define TINKERCELL_CATALOGWIDGET_H

#include <QAbstractButton>
#include <QButtonGroup>
#include <QToolBox>
#include <QComboBox>
#include <QCheckBox>
#include <QTableWidget>
#include <QHash>
#include "ItemFamily.h"
#include "MainWindow.h"
#include "TreeButton.h"

namespace Tinkercell
{
	class NodesTree;
	class ConnectionsTree;

    /*!
      \brief This class is provided for convenience and visual appeal. 
			It holds the recently selected nodes and connections and
			holds the nodes tree and connections tree as separate tool boxes.
      \ingroup plugins
    */
    class TINKERCELLEXPORT CatalogWidget : public Tool
    {
        Q_OBJECT

    public:
	
		/*! \brief provides two different ways of vieweing the parts/connections catalog. 
				TreeView uses a tree widget, which reflects the complete structure 
				of the family trees.
				TabView simplifies the view into tabs but sacrifices some information provided
				by the tree view.*/		
		enum MODE { TreeView, TabView };
	
		/*! \brief provides two different ways of vieweing the parts/connections catalog. 
				TreeView uses a tree widget, which reflects the complete structure 
				of the family trees.
				TabView simplifies the view into tabs but sacrifices some information provided
				by the tree view.*/
		static enum MODE layoutMode;
       
        /*! \brief default constructor
        \param Widget* parent widget
        \param QString filename from which to load heirarchy of nodes. default = NodesTree/NodesTree.xml
         */
        CatalogWidget(NodesTree * nodesTree=0, ConnectionsTree * connectionsTree=0, QWidget * parent=0);

        /*! \brief destructor*/
        ~CatalogWidget();

        /*! \brief default window size*/
        QSize sizeHint() const;
	
	public slots:
		/*! \brief connect to escape signal and add docking widget*/
		bool setMainWindow(MainWindow * );
		/*! \brief insert new button into the widget under the specific group
		* \param QStringList names of the buttons
		* \param QString category of the button
		* \param QList<QIcon> icons for the buttons
		* \param QStringList tool tip text for each button
		*/
        QList<QToolButton*> addNewButtons(const QString& group, const QStringList&, const QList<QIcon>& icons = QList<QIcon>(), const QStringList& tooltips = QStringList());
		/*! \brief show button for one or more families
		* \param QStringList names of the families
		*/
        void showButtons(const QStringList&);
		/*! \brief hide button for one or more families
		* \param QStringList names of the families
		*/
        void hideButtons(const QStringList&);
	signals:
        /*! \brief key pressed inside this widget*/
        void keyPressed(int, Qt::KeyboardModifiers);
		/*! \brief button pressed inside this widget*/
        void buttonPressed(const QString& name);
		/*! \brief one of the nodes in the tree has been selected*/
        void nodeSelected(NodeFamily* nodeFamily);
		 /*! \brief one of the items on the tree is selected (button pressed)*/
        void connectionSelected(ConnectionFamily* family);
		/*! \brief exit the current operation*/
        void sendEscapeSignal(const QWidget*);

    protected:
        /*! \brief sends escape signal if ESC or SPACE is pressed*/
        void keyPressEvent ( QKeyEvent * event );
        /*! \brief context menu is used to change tree file*/
        void contextMenuEvent(QContextMenuEvent * event);

    private slots:
        /*! \brief one of the node buttons selected*/
        void nodeButtonPressed ( int id );
		/*! \brief one of the connection buttons selected*/
		void connectionButtonPressed ( int id );
		/*! \brief one of the nodes in the tree has been selected*/
        void nodeSelectedSlot(NodeFamily* nodeFamily);
		 /*! \brief one of the items on the tree is selected (button pressed)*/
        void connectionSelectedSlot(ConnectionFamily* family);
		/*! \brief exit the current operation*/
        void escapeSignalSlot(const QWidget*);
		/*! \brief brings up a dialog asking for number of recent items*/
        void setNumberOfRecentItems();
		/*! \brief set tab mode (false = tree view)*/
		void setTreeMode(bool);
		/*! \brief make one of the groups visible*/
        void showGroup(const QString& group);
		/*! \brief set the cursor icon when button pressed*/
		void otherButtonPressed ( const QString&, const QPixmap & );
		/*! \brief select the families to show*/
		void selectFamiliesToShow();

    private:

        /*! \brief tree widget that holds the buttons for each node family*/
        QToolBox * toolBox;
        /*! \brief the arrow button at the very top of the tree of nodes*/
        QToolButton arrowButton;
        /*! \brief the tree item containing the arrow button at the very top of the tree of nodes*/
        QTreeWidgetItem arrowItem;
		/*! \brief stores the recently selected node button indices*/
		QButtonGroup nodesButtonGroup;
		/*! \brief stores the recently selected connection button indices*/
		QButtonGroup connectionsButtonGroup;
		/*! \brief the list of nodes in the recent list*/
		QList<NodeFamily*> nodes;
		/*! \brief the list of connections in the recent list*/
		QList<ConnectionFamily*> connections;
		/*! \brief the widgets for which the cursor needs to be updated*/
		QList<QWidget*> widgetsToUpdate;
		
		/*! \brief the nodes tree widgets*/
		NodesTree * nodesTree;
		/*! \brief the connections tree widgets*/
		ConnectionsTree * connectionsTree;
		
		/*! \brief the categories for the tabs*/
		QList< QPair< QString, QStringList> > tabGroups;
		
		int numNodeTabs;
		
		/*! \brief setup the widget using the TreeView layout*/
		void setUpTreeView();
		
		/*! \brief setup the buttons for widget using the TabView layout*/
		void setUpTabView();
		
		/*! \brief make the tab widget*/
		void makeTabWidget();
		
		/*! \brief checks whether this family is too generic to include in the catalog*/
		bool includeFamilyInCatalog(ItemFamily*);

		/*! \brief checks whether this is the first time loading*/
		bool isFirstTime;

		/*! \brief tab-view widget*/
		QTabWidget * tabWidget;
		
		/*! \brief buttons inside each tab of the tabview*/
		QList< QPair<QString,QList<QToolButton*> > > tabGroupButtons;
		
		/*! \brief buttons already visible*/
		QList<QToolButton*> usedButtons;
		
		/*! \brief families that should be displayed in the catalog*/
		QStringList familiesInCatalog;
		
		/*! \brief check boxes to select the families to show*/
		QList<QCheckBox*> selectFamilyCheckBoxes;		
		/*! \brief widget to select the families to show*/
		QDialog * selectFamilyWidget;
    };


}

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif

