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
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QLineEdit>
#include <QDir>
#include <QToolBar>
#include <QMessageBox>
#include <QButtonGroup>
#include <QDockWidget>
#include <QScrollArea>

#include "GraphicsScene.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "NodeGraphicsWriter.h"
#include "NodesTree.h"
#include "ConnectionsTree.h"
#include "CatalogWidget.h"
#include "ItemHandle.h"
#include "Tool.h"

namespace Tinkercell
{
	class TINKERCELLEXPORT ModuleTool : public Tool
	{
		Q_OBJECT

	public:
		ModuleTool();
		bool setMainWindow(MainWindow * main);

	signals:

		void getTextVersion(const QList<ItemHandle*>&, QString*);
		void saveModel(const QString&);

	public slots:

		void select(int);
		void exportModule();

	private slots:		
		void escapeSignal(const QWidget *);
		void itemsAboutToBeInserted (GraphicsScene* scene, QList<QGraphicsItem *>& items, QList<ItemHandle*>& handles, QList<QUndoCommand*>&);
		void itemsAboutToBeRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& item, QList<ItemHandle*>& handles, QList<QUndoCommand*>&);
		void toolLoaded (Tool * tool);
		void itemsInserted(NetworkHandle * network, const QList<ItemHandle*>& handles);
		void mouseDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void keyPressed(GraphicsScene*,QKeyEvent *);
		void sceneClicked(GraphicsScene *scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);
		void itemsDropped(GraphicsScene *, const QString&, const QPointF&);
		void mouseMoved(GraphicsScene* scene, QGraphicsItem*, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>& items);
		void itemsRenamed(NetworkHandle * window, const QList<ItemHandle*>& items, const QList<QString>& oldnames, const QList<QString>& newnames);

		void moduleButtonPressed(const QString&);
		void modelButtonClicked (QAbstractButton *);
		void showNewModuleDialog();
		void updateNumberForNewModule(int);

	private:

		enum Mode { none, inserting, linking, connecting };
		Mode mode;

		QGraphicsLineItem lineItem;
		QDialog *	 newModuleDialog;
		QDialog * exportModuleDialog;
		QComboBox * modulesComboBox;
		QLineEdit * moduleNameEdit;
		QLabel * moduleSavingStatus;
		QLineEdit * newModuleName;
		QTableWidget * newModuleTable;
		ConnectionsTree * connectionsTree;
		NodesTree * nodesTree;
		CatalogWidget * catalogWidget;
		QList<NodeGraphicsItem*> selectedItems;
		QStringList substituteFrom, substituteWith;
		QAction * viewModule;
		NodeGraphicsItem image;
		QStringList aboutToBeRenamed;

		void makeModuleConnection(NodeGraphicsItem*,NodeGraphicsItem*,GraphicsScene*);
		QUndoCommand * moduleConnectionsInserted(QList<QGraphicsItem*>& items);
		QUndoCommand * substituteStrings(const QList<ItemHandle*> & items);
		void removeSubnetworks(QList<QGraphicsItem*>& items, QList<ItemHandle*>& handles);
		QDockWidget * makeDockWidget(const QStringList&);
		void createNewWindow(ConnectionHandle * chandle, NetworkHandle * network);
		void makeNewModule();
		void initializeExportDialog();
		QHash< ItemHandle *, QPixmap > moduleSnapshots;
		QDialog * snapshotToolTip;
		QToolButton * snapshotIcon;
		
		static ItemHandle * findCorrespondingHandle(NodeHandle*,ConnectionHandle*);
		static QList<QPointF> pathAroundRect(QRectF,QRectF,QPointF,QPointF);
	};


}

//extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);


#endif
