/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Function that loads dll into main window

****************************************************************************/
#include <QToolBox>
#include "MainWindow.h"
#include "TreeButton.h"
#include "NodesTree.h"
#include "ConnectionsTree.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif


namespace Tinkercell
{
    /*!
      \brief This class is provided for convenience and visual appeal. 
			It holds the recently selected nodes and connections and
			holds the nodes tree and connections tree as separate tool boxes.
      \ingroup plugins
    */
    class MY_EXPORT NodesTreeContainer : public Tool
    {
        Q_OBJECT

    public:
       
        /*! \brief default constructor
        \param Widget* parent widget
        \param QString filename from which to load heirarchy of nodes. default = NodesTree/NodesTree.xml
         */
        NodesTreeContainer(NodesTree * nodesTree=new NodesTree(), ConnectionsTree * connectionsTree=new ConnectionsTree(), QWidget * parent=0);

        /*! \brief destructor*/
        ~NodesTreeContainer();

        /*! \brief default window size*/
        QSize sizeHint() const;
	
	signals:
        /*! \brief key pressed inside this widget*/
        void keyPressed(int, Qt::KeyboardModifiers);
		/*! \brief one of the nodes in the tree has been selected*/
        void nodeSelected(NodeFamily* nodeFamily);
		 /*! \brief one of the items on the tree is selected (button pressed)*/
        void connectionSelected(ConnectionFamily* family);

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
		/*! \brief the list of nodes in the quick list*/
		QList<NodeFamily*> nodes;
		/*! \brief the list of connections in the quick list*/
		QList<ConnectionFamily*> connections;
    };


}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);
