/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 A tool for displaying all the handles (as a tree) and their attributes. This tool
 also handles move events where an item is moved into a module or Compartment

****************************************************************************/

#ifndef TINKERCELL_CONTAINERTOOL_H
#define TINKERCELL_CONTAINERTOOL_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
#include <QPixmap>
#include <QPair>
#include <QList>
#include <QHash>
#include <QString>
#include <QtDebug>
#include <QGraphicsItem>
#include <QAction>
#include <QFile>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QStandardItem>
#include <QComboBox>
#include <QDoubleSpinBox>

#include "UndoCommands.h"
#include "GraphicsScene.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "ContainerTreeModel.h"

namespace Tinkercell
{
	class TINKERCELLEXPORT CompartmentTool : public Tool
	{
		Q_OBJECT

	public:

		CompartmentTool();
		~CompartmentTool();
		bool setMainWindow(MainWindow * main);
		QSize sizeHint() const;

//signals:
  //      void parentHandleChanged(NetworkHandle * , const QList<ItemHandle*>&, const QList<ItemHandle*>&);

	public slots:
		void updateTree(int);
		void windowChanged(NetworkWindow * , NetworkWindow * );
		void windowClosed(NetworkHandle *);
		void itemsSelected(GraphicsScene * , const QList<QGraphicsItem*>& , QPointF point, Qt::KeyboardModifiers );
		void itemsInserted(GraphicsScene * , const QList<QGraphicsItem*>& , const QList<ItemHandle*>&);
		void itemsRemoved(GraphicsScene * , const QList<QGraphicsItem*>& , const QList<ItemHandle*>&);
		void itemsMoved(GraphicsScene * , const QList<QGraphicsItem*>& item, const QList<QPointF>& );
		void nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& );
		void toolLoaded(Tool*);
		void indexSelected(const QModelIndex&);
	private:
		ContainerTreeDelegate * treeDelegate;
		ContainerTreeModel * treeModel;
		QTreeView * treeView;
		void connectCollisionDetector();
		void sendToBack(QGraphicsItem*, GraphicsScene *);
		void adjustRates(GraphicsScene * scene, QList<ItemHandle*> childItems, QList<ItemHandle*> parents);
		static bool connectionInsideRect(ConnectionGraphicsItem* connection, const QRectF& rect,bool all=true);
		void moveChildItems(GraphicsScene * scene, const QList<QGraphicsItem*> & items0, const QList<QPointF> & dist);
	};


}

#endif

