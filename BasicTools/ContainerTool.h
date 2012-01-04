/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

This tool sets parent/child relationships for items belonging to the family "Container"

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

#ifndef TINKERCELLEXPORT
#ifdef Q_WS_WIN
#   if defined(TC_EXPORTS) || defined(TinkerCell_EXPORTS)
#       define TINKERCELLEXPORT __declspec(dllexport)
#   else
#       define TINKERCELLEXPORT __declspec(dllimport)
#   endif
#else
#    define TINKERCELLEXPORT
#endif
#endif

namespace Tinkercell
{
	class TINKERCELLEXPORT ContainerTool : public Tool
	{
		Q_OBJECT

	public:

		ContainerTool();
		bool setMainWindow(MainWindow * main);

	public slots:
		void itemsSelected(GraphicsScene * , const QList<QGraphicsItem*>& , QPointF point, Qt::KeyboardModifiers );
		void itemsInserted(GraphicsScene * , const QList<QGraphicsItem*>& , const QList<ItemHandle*>&);
		void itemsMoved(GraphicsScene * , const QList<QGraphicsItem*>& item, const QList<QPointF>& );
		void nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& );
		void toolLoaded(Tool*);
	private:
		void connectCollisionDetector();
		void sendToBack(QGraphicsItem*, GraphicsScene *);
		static bool connectionInsideRect(ConnectionGraphicsItem* connection, const QRectF& rect,bool all=true);
		void moveChildItems(GraphicsScene * scene, const QList<QGraphicsItem*> & items0, const QList<QPointF> & dist);
	};


}

#endif

