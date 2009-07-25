/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool performs several tasks:
1) highlight selected items
2) moves text items along with selected items
3) displays GraphicsTools for selected items
4) signals the GraphicsTools when they are selected

****************************************************************************/

#ifndef TINKERCELL_PARTSELECTIONTOOL_H
#define TINKERCELL_PARTSELECTIONTOOL_H

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
#include <QToolBar>
#include <QFrame>

#include "GraphicsScene.h"
#include "NetworkWindow.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "Tool.h"

namespace Tinkercell
{

	/*! \brief This class highlights items that are selected in a GraphicsScene and
	displays tools associated with those items. It also adds the child items
	and text belonging to selected items to the movingItems() list in
	GraphicsScene. Does not have any functionality related to TextEditor
	\ingroup plugins
	*/
	class NodeSelection : public Tool
	{
		Q_OBJECT

	public:

		QColor selectionColor1,
			selectionColor2,
			selectionColor3;

		NodeSelection();

		bool setMainWindow(MainWindow * main);

	public slots:

		void sceneClicked(GraphicsScene *scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
		void sceneDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void itemsSelected(GraphicsScene *scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
		void itemsRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& items, QList<ItemHandle*>& handles);
		void itemsMoved(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<QPointF>&, Qt::KeyboardModifiers modifiers);
		void escapeSignal(const QWidget*);
		void windowClosing(NetworkWindow * window, bool*);
		void historyChanged(int);

	private:
		void turnOnGraphicalTools(QList<QGraphicsItem*>&,QList<ItemHandle*>&,GraphicsScene*);
		void turnOffGraphicalTools();
		void selectNearByItems(GraphicsScene*, ItemHandle*, QGraphicsItem*,double);

		void setSelectColor();
		void revertColor();
		void deselect();
		void select();

		QList<Tool::GraphicsItem*> visibleTools;
	public:
		QList<QGraphicsItem*> allItems;
		QList<NodeGraphicsItem*> selectedNodes;
		QList<ConnectionGraphicsItem*> selectedConnections;
		QList<ControlPoint*> selectedControlPoints;
		QList<TextGraphicsItem*> selectedTexts;
		QList<NodeGraphicsItem*> selectedHandleNodes;
	};

}

#endif
