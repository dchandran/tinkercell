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
#include <QSplashScreen>
#include <QDialog>

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
	/*!
	\brief This class provides the C API for the ConnectionInsertion class
	\ingroup capi
	*/
	class ModuleTool_FToS : public QObject
	{
		Q_OBJECT
	signals:
		void doSubstituteModel(QSemaphore*, ItemHandle*, const QString&);
	public slots:
		void substituteModel(long, const char*);
	};
	
	/*! This class looks at connections that are inserted, checks whether there is a model that
		can be used to fill in the internal steps of that connection. These models are obtained
		from either the TinkerCell home folder. If no models are found in TinkerCell home folder, then
		the application folder is searched for suitable models. The models are located in folders with
		name corresponding to the family names (case insensitive).
		
		When a model is loaded inside a connection, a new scene is created for that model. The model
		is merged with the connection using the "participants" table in that connection. All the tables
		are merged because MergeHandlesCommand is used for the merge. Each connection's middle decorator
		is replaced with the "expand.xml" node item.
		
		There are two important lists: numericalTablesToBeReplaced and textTablesToBeReplaced. 
		These two lists can be used to indicate which tables should not be merged during model replacement.
		
		listOfModels and substituteModel are two useful functions. listOfModels returns the model files
		associated with a particular family. substituteModel can be used to replace a model inside a
		connection.
	*/
	class TINKERCELLEXPORT ModuleTool : public Tool
	{
		Q_OBJECT

	public:
		ModuleTool();
		bool setMainWindow(MainWindow * main);
		
		static QStringList numericalTablesToBeReplaced;
		static QStringList textTablesToBeReplaced;
		
		static QStringList listOfModels(ItemFamily * family);
		void substituteModel(ItemHandle * , const QString& , NetworkWindow * window=0);

	signals:

		void getTextVersion(const QList<ItemHandle*>&, QString*);
		void saveModel(const QString&);

	public slots:

		void select(int);
		void exportModule();
		void setupFunctionPointers( QLibrary * );

	private slots:

		void itemsAboutToBeInserted (GraphicsScene* scene, QList<QGraphicsItem *>& items, QList<ItemHandle*>& handles, QList<QUndoCommand*>&);
		void itemsAboutToBeRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& item, QList<ItemHandle*>& handles, QList<QUndoCommand*>&);
		void toolLoaded (Tool * tool);
		void itemsInserted(NetworkHandle * network, const QList<ItemHandle*>& handles);
		void mouseDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void keyPressed(GraphicsScene*,QKeyEvent *);
		void mouseMoved(GraphicsScene* scene, QGraphicsItem*, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>& items);
		void itemsRenamed(NetworkHandle * window, const QList<ItemHandle*>& items, const QList<QString>& oldnames, const QList<QString>& newnames);

		void moduleButtonPressed(const QString&);
		void modelButtonClicked (QAbstractButton *);
		void showNewModuleDialog();
		void updateNumberForNewModule(int);
		
		void doSubstituteModel(QSemaphore*, ItemHandle*, const QString&);

	private:
	
		static ModuleTool_FToS * fToS;
		static tc_strings _listOfModels(long);
		static void _substituteModel(long, const char *);
		static QString emptyModel();

		QDialog * newModuleDialog;
		QDialog * exportModuleDialog;
		QComboBox * modulesComboBox;
		QLineEdit * moduleNameEdit;
		QLabel * moduleSavingStatus;
		QLineEdit * newModuleName;
		QTableWidget * newModuleTable;
		ConnectionsTree * connectionsTree;
		NodesTree * nodesTree;
		CatalogWidget * catalogWidget;
		QStringList substituteFrom, substituteWith;
		QAction * viewModule;
		NodeGraphicsItem image;
		QStringList aboutToBeRenamed;

		QDockWidget * makeDockWidget(const QStringList&);
		NetworkWindow * createNewWindow(ConnectionHandle * chandle, NetworkHandle * network);
		void makeNewModule();
		void initializeExportDialog();
		QHash< ItemHandle *, QPixmap > moduleSnapshots;
		QDialog * snapshotToolTip;
		QToolButton * snapshotIcon;
		
		static ItemHandle * findCorrespondingHandle(NodeHandle*,ConnectionHandle*);
	};


}

//extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);


#endif
