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
		void sceneChanged(GraphicsScene*);
		void modelSaved(NetworkWindow*);
		void modelLoaded(NetworkWindow*);

	public slots:
		void prepareModelForSaving(NetworkWindow*,bool*);
		void saveModel(const QString& filename);
		void loadModel(const QString& filename);
		void historyChanged( int );
		void windowClosing(NetworkWindow * win, bool * close);

	protected:

		static NodeGraphicsItem* readNode(NodeGraphicsReader&,QString&,QTransform&,QPointF&,qreal&,bool&);
		static ConnectionGraphicsItem* readConnection(NodeGraphicsReader &,QList<NodeGraphicsItem*>&, QList<ConnectionGraphicsItem*>& , QString&,qreal&,bool&);
		static TextGraphicsItem * readText(QXmlStreamReader & ,QString&, QTransform&,QPointF&, qreal&);
		static void writeNode(NodeGraphicsItem* node, QXmlStreamWriter& modelWriter);
		static void writeConnection(ConnectionGraphicsItem* connection, QXmlStreamWriter& modelWriter);
		static void writeText(TextGraphicsItem* text, QXmlStreamWriter& modelWriter);	

		QHash<GraphicsScene*,bool> savedScenes;
	};

}

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif
