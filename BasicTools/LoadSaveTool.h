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
#include "ModelWriter.h"
#include "ModelReader.h"
#include "NodeGraphicsWriter.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsWriter.h"
#include "ConnectionGraphicsReader.h"

namespace Tinkercell
{

	class TINKERCELLEXPORT LoadSaveTool : public Tool
	{
		Q_OBJECT

	public:
		LoadSaveTool();
		bool setMainWindow(MainWindow * main);

	signals:
		void networkSaved(NetworkHandle*);
		void networkLoaded(NetworkHandle*);

	public slots:
		void prepareNetworkForSaving(NetworkHandle*,bool*);
		void saveItems(NetworkHandle*, const QString& filename);
		void loadItems(QList<QGraphicsItem*>&, const QString& );
		void getItemsFromFile(QList<ItemHandle*>&, const QString&,ItemHandle* root=0);
		void saveNetwork(const QString& filename);
		void loadNetwork(const QString& filename);
		void historyChanged( int );
		void networkClosing(NetworkHandle *, bool * close);
		void restore(int);

	protected:

		static NodeGraphicsItem* readNode(NodeGraphicsReader&,QString&,QTransform&,QPointF&,qreal&,int&);
		static ConnectionGraphicsItem* readConnection(NodeGraphicsReader &,QList<NodeGraphicsItem*>&, QList<ConnectionGraphicsItem*>& , QString&,qreal&, int&);
		static TextGraphicsItem * readText(QXmlStreamReader & ,QString&, QTransform&,QPointF&, qreal&, int&);
		static void writeNode(NodeGraphicsItem* node, QXmlStreamWriter& modelWriter,int sceneNumber);
		static void writeConnection(ConnectionGraphicsItem* connection, QXmlStreamWriter& modelWriter,int sceneNumber);
		static void writeText(TextGraphicsItem* text, QXmlStreamWriter& modelWriter,int sceneNumber);	

		QHash<NetworkHandle*,bool> savedNetworks;
		int countHistory;
		
		QMessageBox * restoreDialog;
		QPushButton * restoreButton;
	};

}

#endif