/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 Automatically manage gene regulatory network rates and parameters

****************************************************************************/

#ifndef TINKERCELL_AUTOMATICGENEREGULATORYMODELTOOL_H
#define TINKERCELL_AUTOMATICGENEREGULATORYMODELTOOL_H

#include <QtGui>
#include <QtDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QTimeLine>
#include <QTextEdit>
#include <QTextCursor>

#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "NodeGraphicsWriter.h"
#include "NodesTree.h"
#include "ItemHandle.h"
#include "Tool.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	class MY_EXPORT AutoGeneRegulatoryTool : public Tool
	{
		Q_OBJECT

	public:
		AutoGeneRegulatoryTool();
		bool setMainWindow(MainWindow * main);
	public slots:

		void itemsInserted(GraphicsScene* scene, const QList<QGraphicsItem *>& items, const QList<ItemHandle*>& handles);
		void itemsRemoved(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&);
		void itemsMoved(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<QPointF>& distance, Qt::KeyboardModifiers modifiers);
		void nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& , Qt::KeyboardModifiers );
		void pluginLoaded(const QString&);
		void itemsSelected(GraphicsScene *,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers);
		void autoTFTriggered(const QString&);
		void autoDegradationTriggered();
		void autoPhosphateTriggered();
		void autoGeneProductTriggered();
		void autoTFTriggeredUp();
		void autoTFTriggeredDown();
		void insertmRNAstep();

	signals:
		void alignCompactHorizontal();
		void setMiddleBox(int,const QString&);

	private:
		QTimeLine glowTimer;
		void connectPlugins();
		QAction autoTFUp, autoTFDown;
		QAction autoDegradation;
		QAction autoGeneProduct;
		QAction mRNAstep;
		QAction autoPhosphate;
		QAction * separator;
		bool doAssignment;
	public:
		static void findAllPart(GraphicsScene*,NodeGraphicsItem*,const QString&,QList<ItemHandle*>&,bool,const QStringList&, bool stopIfElongation = false);
                static QString hillEquation(NodeHandle *,ItemHandle* exclude = 0);
	};


}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);


#endif
