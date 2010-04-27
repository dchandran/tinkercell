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
#include "MainWindow.h"
#include "NetworkWindow.h"
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

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	class MY_EXPORT LoadSaveTool : public Tool
	{
		Q_OBJECT

	public:
		LoadSaveTool();
		bool setMainWindow(MainWindow * main);

	signals:
		void modelSaved(NetworkWindow*);
		void modelLoaded(NetworkWindow*);

	public slots:
		void prepareModelForSaving(NetworkWindow*,bool*);
		void saveItems(GraphicsScene *, const QList<QGraphicsItem*>&, const QString& filename);
		void loadItems(QList<QGraphicsItem*>&, const QString& filename, QList<QGraphicsItem*>&);
		void saveModel(const QString& filename);
		void loadModel(const QString& filename);
		void historyChanged( int );
		void windowClosing(NetworkWindow * win, bool * close);
		void restore();

	protected:

		static NodeGraphicsItem* readNode(NodeGraphicsReader&,QString&,QTransform&,QPointF&,qreal&,bool&);
		static ConnectionGraphicsItem* readConnection(NodeGraphicsReader &,QList<NodeGraphicsItem*>&, QList<ConnectionGraphicsItem*>& , QString&,qreal&,bool&);
		static TextGraphicsItem * readText(QXmlStreamReader & ,QString&, QTransform&,QPointF&, qreal&, bool&);
		static void writeNode(GraphicsScene * scene, NodeGraphicsItem* node, QXmlStreamWriter& modelWriter);
		static void writeConnection(GraphicsScene * scene, ConnectionGraphicsItem* connection, QXmlStreamWriter& modelWriter);
		static void writeText(GraphicsScene * scene, TextGraphicsItem* text, QXmlStreamWriter& modelWriter);	

		QHash<GraphicsScene*,bool> savedScenes;
		int countHistory;
		
		QMessageBox * restoreFile;
	};

}

#endif
