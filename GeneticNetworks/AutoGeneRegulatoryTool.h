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

namespace Tinkercell
{
	class TINKERCELLEXPORT AutoGeneRegulatoryTool_FtoS : public QObject
	{
		Q_OBJECT
		signals:
			void partsIn(QSemaphore*, ItemHandle*, QList<ItemHandle*>* parts);
			void partsUpstream(QSemaphore*, ItemHandle*, QList<ItemHandle*>* parts);
			void partsDownstream(QSemaphore*, ItemHandle*, QList<ItemHandle*>* parts);
			void alignParts(QSemaphore*,const QList<ItemHandle*>&);
			
		public slots:
			tc_items partsIn(long);
			tc_items partsUpstream(long);
			tc_items partsDownstream(long);
			void alignParts(tc_items);
	};


	class TINKERCELLEXPORT AutoGeneRegulatoryTool : public Tool
	{
		Q_OBJECT

	public:
		AutoGeneRegulatoryTool();
		bool setMainWindow(MainWindow * main);
	
	public slots:
		void itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& , const QList<ItemHandle*>& );
		void itemsInserted(NetworkHandle* , const QList<ItemHandle*>& handles);
		void itemsRemoved(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&,QList<QUndoCommand*>&);
		void itemsMoved(GraphicsScene * scene, QList<QGraphicsItem*>& item, QList<QPointF>& distance, QList<QUndoCommand*>& commands);
		void nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& );
		void toolLoaded(Tool*);
		void itemsSelected(GraphicsScene *,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers);
		void autoTFTriggered(const QString&);
		void autoDegradationTriggered();
		void autoPhosphateTriggered();
		void autoGeneProductTriggered();
		void autoTFTriggeredUp();
		void autoTFTriggeredDown();
		void autoAssignRates(QList<NodeHandle*>&);
		
	signals:
		void itemsInsertedSignal(GraphicsScene * scene, const QList<QGraphicsItem*>& , const QList<ItemHandle*>& );
		void alignCompactHorizontal();
		void setMiddleBox(int,const QString&);
		void dataChanged(const QList<ItemHandle*>& items);
	
	private slots:
	
		void setupFunctionPointers( QLibrary * );
		void partsIn(QSemaphore*, ItemHandle*, QList<ItemHandle*>* parts);
		void partsUpstream(QSemaphore*, ItemHandle*, QList<ItemHandle*>* parts);
		void partsDownstream(QSemaphore*, ItemHandle*, QList<ItemHandle*>* parts);
		void alignParts(QSemaphore*,const QList<ItemHandle*>&);
		void adjustPlasmid(GraphicsScene * , NodeGraphicsItem*, bool align=true);
	
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
		bool justAdjustedPlasmid;
		
		static tc_items _partsIn(long);
		static tc_items _partsUpstream(long);
		static tc_items _partsDownstream(long);
		static void _alignParts(tc_items);
		
		static AutoGeneRegulatoryTool_FtoS fToS;
		
	public:
		static void findAllParts(GraphicsScene*,NodeGraphicsItem*,const QString& family,QList<ItemHandle*>& ,bool upstream=true,const QStringList& stopIf=QStringList(), bool stopIfElongation = false);
		static QString hillEquation(NodeHandle *,ItemHandle* exclude = 0);
		static QString hillEquation(QList<ConnectionHandle*> connections, QList<NodeHandle*>& activators, QList<NodeHandle*> repressors);
	};


}

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);


#endif

