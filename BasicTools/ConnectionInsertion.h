/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This class received signals from the ConnectionsTree class and provides the interface
for connecting items using the connections in the ConnectionsTree

****************************************************************************/

#ifndef TINKERCELL_CONNECTIONINSERTIONTOOL_H
#define TINKERCELL_CONNECTIONINSERTIONTOOL_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
#include <QPixmap>
#include <QString>
#include <QtDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAction>
#include <QFile>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QGraphicsRectItem>
#include <QTimeLine>
#include <QHash>
#include <QList>

#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "ConnectionsTree.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "NodesTree.h"
#include "NodeInsertion.h"

namespace Tinkercell
{
	/*!
	\brief This class provides the C API for the ConnectionInsertion class
	\ingroup capi
	*/
	class ConnectionInsertion_FToS : public QObject
	{
		Q_OBJECT
	signals:
		void insertConnection(QSemaphore*,ItemHandle** item,const QList<ItemHandle*>&,const QString&, const QString&);
		void getConnectedNodes(QSemaphore*,QList<ItemHandle*>*,ItemHandle*);
		void getConnectedNodesIn(QSemaphore*,QList<ItemHandle*>*,ItemHandle*);
		void getConnectedNodesOut(QSemaphore*,QList<ItemHandle*>*,ItemHandle*);
		void getConnectedNodesOther(QSemaphore*,QList<ItemHandle*>*,ItemHandle*);
		void getConnections(QSemaphore*,QList<ItemHandle*>*,ItemHandle*);
		void getConnectionsIn(QSemaphore*,QList<ItemHandle*>*,ItemHandle*);
		void getConnectionsOut(QSemaphore*,QList<ItemHandle*>*,ItemHandle*);
		void getConnectionsOther(QSemaphore*,QList<ItemHandle*>*,ItemHandle*);
	public slots:
		void* insertConnection(ArrayOfItems, const char*, const char*);
		ArrayOfItems getConnectedNodes(void*);
		ArrayOfItems getConnectedNodesIn(void*);
		ArrayOfItems getConnectedNodesOut(void*);
		ArrayOfItems getConnectedNodesOther(void*);
		ArrayOfItems getConnections(void*);
		ArrayOfItems getConnectionsIn(void*);
		ArrayOfItems getConnectionsOut(void*);
		ArrayOfItems getConnectionsOther(void*);
	};

	/*!\brief This class allows users to select items from the connection tree and insert them onto the scene.
	When user selects an item from the connections tree,
	\ingroup plugins*/
	class TINKERCELLEXPORT ConnectionInsertion : public Tool
	{
		Q_OBJECT

	public:

		/*!\brief Constructor.
		\param the tree with connections (if none provided, will search main window for Connection Tree tool)
		*/
		ConnectionInsertion(ConnectionsTree * tree = 0);
		/*!\brief Sets the main window and finds the connection tree. Sets all the event connections.
		\param the main window. If null, this tool will not function.
		\return nothing
		*/
		virtual bool setMainWindow(MainWindow * main);

		public slots:
			/*!\brief Event that is fired when a tree item is selected
			\param the connection family that is selected
			\return nothing
			*/
			void connectionSelected(ConnectionFamily * family);
			/*!\brief Event that is fired when the scene is clicked.
			If a tree item was selected, a new connection will be inserted
			\param the scene that was clicked on
			\param the location where clicked
			\param the mouse button used to click (not used)
			\param keyboard modifiers used (not used)
			\return nothing
			*/
			void sceneClicked(GraphicsScene *scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);

			/*!\brief Exit from insertion mode*/
			void sceneRightClick(GraphicsScene *, QGraphicsItem*, QPointF, Qt::KeyboardModifiers);
			/*!\brief Exit from insertion mode*/
			void escapeSignal(const QWidget * widget);
			/*!\brief connects to ConnectionsTree if it is available in MainWindow's Tool hash*/
			void toolLoaded(Tool*);
			/*!\brief setup the C API pointers when a new C library is loaded*/
			void setupFunctionPointers( QLibrary * );
			/*!\brief add Participants table in each connection*/
			void itemsAboutToBeInserted (GraphicsScene* scene, QList<QGraphicsItem *>& items, QList<ItemHandle*>& handles, QList<QUndoCommand*>&);
			/*!\brief remove items from Participants table in each connection*/
			void itemsAboutToBeRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& item, QList<ItemHandle*>& handles, QList<QUndoCommand*>&);
			/*! \brief insert parts and connections*/
			void itemsDropped(GraphicsScene *, const QString&, const QPointF&);

		private slots:
			/*!\brief C API function*/
			void insertConnection(QSemaphore*,ItemHandle** ,const QList<ItemHandle*>&,const QString&, const QString&);
			/*!\brief C API function*/
			void getConnectedNodes(QSemaphore*,QList<ItemHandle*>*,ItemHandle*);
			/*!\brief C API function*/
			void getConnectedNodesIn(QSemaphore*,QList<ItemHandle*>*,ItemHandle*);
			/*!\brief C API function*/
			void getConnectedNodesOut(QSemaphore*,QList<ItemHandle*>*,ItemHandle*);
			/*!\brief C API function*/
			void getConnectedNodesOther(QSemaphore*,QList<ItemHandle*>*,ItemHandle*);
			/*!\brief C API function*/
			void getConnections(QSemaphore*,QList<ItemHandle*>* list,ItemHandle*);
			/*!\brief C API function*/
			void getConnectionsIn(QSemaphore*,QList<ItemHandle*>* list,ItemHandle*);
			/*!\brief C API function*/
			void getConnectionsOut(QSemaphore*,QList<ItemHandle*>* list,ItemHandle*);
			/*!\brief C API function*/
			void getConnectionsOther(QSemaphore*,QList<ItemHandle*>* list,ItemHandle*);

	protected:

		/*!\brief the family that was selected from the connections tree*/
		ConnectionFamily * selectedFamily;
		/*! \brief the connection tree with all the connection families*/
		ConnectionsTree * connectionsTree;
		/*! \brief the nodes tree*/
		NodesTree * nodesTree;
		/*! \brief the node insertion tool*/
		NodeInsertion * nodeInsertionTool;
		/*! \brief stores the default number of inputs and outputs*/
		QHash<QString,int> defaultInputs, defaultOutputs;
		/*! \brief set the default number of inputs and outputs for different reactions*/
		void initializeHashes();
		/*! \brief if user selects wrong node types, then change that selected connection type to match user's selected nodes*/
		bool changeSelectedFamilyToMatchSelection(bool all=false);
		/*!\brief the currently selected set of nodes under insertion mode that are to be connected*/
		QList<NodeGraphicsItem*> selectedNodes;
		/*!\brief the currently selected set of connections that are to be connected*/
		QList<ConnectionGraphicsItem*> selectedConnections;

		/*!\brief the number of nodes to be selected in order to make a connection.
		This number is determined by the ConnectionsTree*/
		int numRequiredIn, numRequiredOut;
		/*!\brief the type of nodes to be selected in order to make a connection.
		This number is determined by the ConnectionsTree*/
		QStringList typeIn, typeOut;
		/*!\brief set the number and types of nodes to be selected in order to make a connection.
		This number is determined using the ConnectionsTree*/
		void setRequirements();
		/*!\brief change the color of nodes selected to be connected*/
		void setSelectColor();
		/*!\brief clear currently selected items and exit insertion mode*/
		void clear(bool arrows=true);
		/*!\brief return the color of currently selected items*/
		void revertColors();
		/*!\brief connect to the connections tree*/
		void connectToConnectionsTree();
		/*!\brief insert nodes that have not been selected*/
		QList<QGraphicsItem*> autoInsertNodes(GraphicsScene *, const QPointF & point);

		/*!\brief connect the C API functions and signals*/
		void connectTCFunctions();
		/*!\brief the C API function to signal converter*/
		static ConnectionInsertion_FToS fToS;

		/*!\brief C API function*/
		static void* _insertConnection(ArrayOfItems, const char*, const char*);
		/*!\brief C API function*/
		static ArrayOfItems _getConnectedNodes(void*);
		/*!\brief C API function*/
		static ArrayOfItems _getConnectedNodesIn(void*);
		/*!\brief C API function*/
		static ArrayOfItems _getConnectedNodesOut(void*);
		/*!\brief C API function*/
		static ArrayOfItems _getConnectedNodesOther(void*);
		/*!\brief C API function*/
		static ArrayOfItems _getConnections(void*);
		/*!\brief C API function*/
		static ArrayOfItems _getConnectionsIn(void*);
		/*!\brief C API function*/
		static ArrayOfItems _getConnectionsOut(void*);
		/*!\brief C API function*/
		static ArrayOfItems _getConnectionsOther(void*);
		
		bool isReactant(NodeHandle*);
		bool isProduct(NodeHandle*);
		
		static bool isReactant(const QString&);
	};

}

#endif
