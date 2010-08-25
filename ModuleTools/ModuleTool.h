/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This tool handles module connections that merge items from two modules

****************************************************************************/

#ifndef TINKERCELL_BASICMODULETOOL_H
#define TINKERCELL_BASICMODULETOOL_H

#include <QtGui>
#include <QIcon>
#include <QPixmap>
#include <QString>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QAction>
#include <QFile>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QUndoCommand>
#include <QGraphicsRectItem>
#include <QGraphicsWidget>
#include <QSlider>
#include <QToolButton>
#include <QButtonGroup>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPair>
#include <QAbstractButton>

#include "GraphicsScene.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "NodeGraphicsWriter.h"
#include "NodesTree.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "ModuleConnectionGraphicsItem.h"

namespace Tinkercell
{
	class TINKERCELLEXPORT ModuleTool : public Tool
	{
		Q_OBJECT

	public:
		ModuleTool();
		bool setMainWindow(MainWindow * main);

	signals:

		void createTextWindow(TextEditor *, const QList<ItemHandle*>&);
		void loadItems(QList<QGraphicsItem*>&, const QString&);

	public slots:

		void select(int);
		void escapeSignal(const QWidget *);
		void itemsAboutToBeInserted (GraphicsScene* scene, QList<QGraphicsItem *>& items, QList<ItemHandle*>& handles, QList<QUndoCommand*>&);
		void itemsAboutToBeRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& item, QList<ItemHandle*>& handles, QList<QUndoCommand*>&);
		void toolLoaded (Tool * tool);
		void itemsInserted(NetworkHandle * network, const QList<ItemHandle*>& handles);
		void itemsMoved(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<QPointF>& distance);
		void mouseDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void keyPressed(GraphicsScene*,QKeyEvent *);
		void sceneClicked(GraphicsScene *scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
		void itemsDropped(GraphicsScene *, const QString&, const QPointF&);
		void mouseMoved(GraphicsScene* scene, QGraphicsItem*, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>& items);

	private slots:

		void moduleButtonPressed(const QString&);
		void modelButtonClicked (QAbstractButton *);

	private:

		enum Mode { none, inserting, linking, connecting };
		Mode mode;

		QGraphicsLineItem lineItem;
		
		QDockWidget * makeDockWidget(const QString&);

		QDialog * newModuleDialog;
		void showNewModuleDialog();

		void makeModuleConnection(NodeGraphicsItem*,NodeGraphicsItem*,GraphicsScene*);
		QUndoCommand * moduleConnectionsInserted(QList<QGraphicsItem*>& items);
		QUndoCommand * substituteStrings(const QList<ItemHandle*> & items);
		void removeSubnetworks(QList<QGraphicsItem*>& items, QList<ItemHandle*>& handles);
		
		static ItemHandle * findCorrespondingHandle(ItemHandle*,ConnectionHandle*);

		QList<NodeGraphicsItem*> selectedItems;
		QStringList substituteFrom, substituteWith;
		QAction * viewModule;
		NodeGraphicsItem image;

		static QList<QPointF> pathAroundRect(QRectF,QRectF,QPointF,QPointF);
	};


}

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);


#endif
