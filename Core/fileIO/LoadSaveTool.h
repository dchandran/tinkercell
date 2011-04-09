/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool allows the loading and saving of models.

****************************************************************************/
#ifndef TINKERCELL_SAVEFILETOOL_H
#define TINKERCELL_SAVEFILETOOL_H

#include <QtGui>
#include <QIcon>
#include <QPixmap>
#include <QString>
#include <QtDebug>
#include <QAction>
#include <QFile>
#include <QHash>
#include <QDateTime>
#include <QUndoCommand>
#include <QMessageBox>
#include <QPushButton>
#include "MainWindow.h"
#include "NetworkHandle.h"
#include "GraphicsScene.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "Tool.h"
#include "ItemFamily.h"
#include "ModelWriter.h"
#include "ModelReader.h"
#include "NodeGraphicsWriter.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsWriter.h"
#include "ConnectionGraphicsReader.h"

namespace Tinkercell
{

	/*!\brief This class can save and load any model built using classes in the Core library.
		The loading process will assign 0 as the family for all the handles. If a non-zero family should
		be assigned, then it is required that the nodeFamilies and connectionFamilies hash tables
		should be populations with (name,family) pairs, storing the name and pointers for each family
		item. Auto-saves the current network every 10 changes
	*/
	class TINKERCELLEXPORT LoadSaveTool : public Tool
	{
		Q_OBJECT

	public:
		/*!\brief  if the program contains families, then this map should be set*/
		static QMap<QString,NodeFamily*> nodeFamilies;
		/*!\brief  if the program contains families, then this map should be set*/
		static QMap<QString,ConnectionFamily*> connectionFamilies;
		/*!\brief lookup family from its name*/
		static NodeFamily * getNodeFamily(const QString& name);
		/*!\brief lookup family from its name*/
		static ConnectionFamily * getConnectionFamily(const QString& name);
		
		/*!\brief default constructor*/
		LoadSaveTool();
		/*!\brief destructor*/
		~LoadSaveTool();
		/*!\brief connects to saveModel, loadModel, getItemsFromFile*/
		bool setMainWindow(MainWindow * main);

	signals:
		/*!\brief connects to MainWindow's networkSaved signal*/
		void networkSaved(NetworkHandle*);
		/*!\brief connects to MainWindow's networkLoaded signal*/
		void networkLoaded(NetworkHandle*);
		/*!\brief connects to MainWindow's itemsAbouToBeInsered signal*/
		void itemsAboutToBeInserted(GraphicsScene * scene, QList<QGraphicsItem*>& , QList<ItemHandle*>&, QList<QUndoCommand*>& );
		/*!\brief connects to MainWindow's itemsInsered signal*/
		void itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<ItemHandle*>& handles);
		/*!\brief connects to MainWindow's historyChanged signal*/
		void historyChanged( int i=0);

	public slots:
		/*!\brief not currently used*/
		void prepareNetworkForSaving(NetworkHandle*,bool*);
		/*!\brief save a network in a file*/
		void saveItems(NetworkHandle*, const QString& filename);
		/*!\brief load a list of graphics items from a file. Use getHandle to get the handles from the graphics items.*/
		void loadItems(QList<QGraphicsItem*>&, const QString& , ItemHandle * globalHandle = 0);
		/*!\brief connects to MainWindow's getItemsFromFile signal*/
		void getItemsFromFile(QList<ItemHandle*>&, QList<QGraphicsItem*>&, const QString&,ItemHandle* root=0);
		/*!\brief connects to MainWindow's saveNetwork signal*/
		void saveNetwork(const QString& filename);
		/*!\brief connects to MainWindow's loadNetwork signal*/
		void loadNetwork(const QString& filename);
		/*!\brief connects to MainWindow's historyChanged signal*/
		void historyChangedSlot( int );
		/*!\brief connects to MainWindow's networkClosing signal*/
		void networkClosing(NetworkHandle *, bool * close);
		/*!\brief used to restore a model when TinkerCell exits abnormally*/
		void restore(int);

	protected:

		/*!\brief read a single NodeGraphicsItem. Primarily uses NodeGraphicsReader, but adds extra information regarding the handles*/
		static NodeGraphicsItem* readNode(NodeGraphicsReader&,QString&,QTransform&,QPointF&,qreal&,int&);
		/*!\brief read a single ConnectionGraphicsItem. Primarily uses NodeGraphicsReader, but adds extra information regarding the handles*/
		static ConnectionGraphicsItem* readConnection(NodeGraphicsReader &,QList<NodeGraphicsItem*>&, QList<ConnectionGraphicsItem*>& , QString&,qreal&, int&);
		/*!\brief read a single TextGraphicsItem*/
		static TextGraphicsItem * readText(QXmlStreamReader & ,QString&, QTransform&,QPointF&, qreal&, int&);
		/*!\brief read a single NodeGraphicsItem. Primarily uses NodeGraphicsWriter, but adds extra information regarding the handles*/
		static void writeNode(NodeGraphicsItem* node, QXmlStreamWriter& modelWriter,int sceneNumber);
		/*!\brief read a single ConnectionGraphicsItem. Primarily uses NodeGraphicsWriter, but adds extra information regarding the handles*/
		static void writeConnection(ConnectionGraphicsItem* connection, QXmlStreamWriter& modelWriter,int sceneNumber);
		/*!\brief writes a single TextGraphicsItem*/
		static void writeText(TextGraphicsItem* text, QXmlStreamWriter& modelWriter,int sceneNumber);
		
		/*!\brief read a text table and assign the units for the Node and Connection families*/
		static void readUnitsFromTable(const TextDataTable & units);
		/*!\brief write all the units to a text table*/
		static void saveUnitsToTable(TextDataTable & units);

		/*!\brief hash table that is used to record which networks were saved after making any changes*/		
		QHash<NetworkHandle*,bool> savedNetworks;
		/*!\brief used to count 10 changed, which triggers auto-save*/
		int countHistory;
		/*!\brief dialog used to restore the last network when TinkerCell closes abnormally*/
		QMessageBox * restoreDialog;
		/*!\brief button in the dialog used to restore the last network when TinkerCell closes abnormally*/
		QPushButton * restoreButton;
		/*!\brief commands to be deleted at the end*/
		QList<QUndoCommand*> loadCommands;
		
		/*!\brief A simple struct used to store loaded models. 
		             This is used to speed up reloads by caching the models*/
		struct CachedModel
		{
			QDateTime time;
			ItemHandle * globalHandle;
			QList<QGraphicsItem*> items;
		};
		
		/*!\brief cache loaded files quick reload*/
		static QHash< QString, CachedModel* > cachedModels;
	};

}

#endif
