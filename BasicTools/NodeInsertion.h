/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool allows insertion of nodes from the NodesTree

****************************************************************************/

#ifndef TINKERCELL_PARTINSERTIONTOOL_H
#define TINKERCELL_PARTINSERTIONTOOL_H

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
#include <QGraphicsWidget>
#include <QFrame>

#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "NodeGraphicsWriter.h"
#include "NodesTree.h"
#include "ItemHandle.h"
#include "ConnectionsTree.h"
#include "Tool.h"

namespace Tinkercell
{
	/*! \brief This class provides the C API for the NodeInsertion class
	\ingroup capi
	*/
	class NodeInsertion_FToS : public QObject
	{
		Q_OBJECT
		signals:
			void insertItem(QSemaphore*, ItemHandle** item, QString name, QString family);
		public slots:
			void* insertItem(const char* , const char* );
	};

	/*! \brief This class listens to the NodesTree signals and allows users to insert items from
	the tree of nodes. It also ensures that the inserted item has a unique name and inserts a
	text item along with the node item. When user selects a node from the tree of nodes, this tool
	enters insertion mode, which disables the default GraphicsScene behaviors. When the insertion mode
	is exited, the default behaviors are returned, i.e. useDefaultBehavior is set to true.
	\ingroup plugins
	*/
	class TINKERCELLEXPORT NodeInsertion : public Tool
	{
		Q_OBJECT

	public:

		/* !\brief default constructor. It NodesTree is not given, this class will search for one in the MainWindow
		\param NodesTree* the tree from which the nodes will be inserted*/
		NodeInsertion(NodesTree * tree = 0);

		/* !\brief load this plug-in into the TinkerCell main window
		\param MainWindow* TinkerCell's main window*/
		bool setMainWindow(MainWindow * main);

	public slots:
		/*! \brief one of the nodes in the tree of nodes has been selected. Enters insertion mode.
		\param NodeFamily* the selected family*/
		void nodeSelected(NodeFamily * nodeFamily);

		/*! \brief User clicked on the screen. This would insert a new node if in insertion mode.*/
		void sceneClicked(GraphicsScene *scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);

		/*! \brief Right click on the screen exits from the insertion mode*/
		void sceneRightClick(GraphicsScene *, QGraphicsItem*, QPointF, Qt::KeyboardModifiers);

		/*! \brief exit from current operations such as insertion mode*/
		void escapeSignal(const QWidget * widget);

		/*! \brief exit from current operations such as insertion mode*/
		void toolLoaded(Tool*);

		/*! \brief this function sets up the C API function pointers whenever a C library is loaded*/
		void setupFunctionPointers( QLibrary * f );

		/*! \brief insert parts*/
		void itemsDropped(const QString&, const QPointF&);

	private slots:

		/*! \brief A C API function*/
		void insertItem(QSemaphore*, ItemHandle** item, QString name, QString family);

	private:
		/*! \brief the currently selected item from the nodes tree*/
		NodeFamily * selectedNodeFamily;
		/*! \brief the nodes tree*/
		NodesTree * nodesTree;
		/*! \brief returns the mouse cursor to the normal arrow and exists from insertion mode*/
		void clear(bool arrows=true);
		/*! \brief check to see whether the MainWindow has a NodesTree*/
		void connectToNodesTree();

		/*! \brief connect the C API signals and slots*/
		void connectTCFunctions();
		/*! \brief the C API function to signal converter*/
		static NodeInsertion_FToS fToS;
		/*! \brief the C API function*/
		static void* _insertItem(const char* , const char* );

	};

}

#endif
