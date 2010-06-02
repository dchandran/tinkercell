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
			ArrayOfItems partsIn(void*);
			ArrayOfItems partsUpstream(void*);
			ArrayOfItems partsDownstream(void*);
			void alignParts(ArrayOfItems);
	};


	class TINKERCELLEXPORT AutoGeneRegulatoryTool : public Tool
	{
		Q_OBJECT

	public:
		AutoGeneRegulatoryTool();
		bool setMainWindow(MainWindow * main);
	
	public slots:
		void itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& , const QList<ItemHandle*>& );
		void itemsInserted(NetworkWindow* , const QList<ItemHandle*>& handles);
		void itemsRemoved(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&);
		void itemsMoved(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<QPointF>& distance, Qt::KeyboardModifiers modifiers);
		void nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& , Qt::KeyboardModifiers );
		void toolLoaded(Tool*);
		void itemsSelected(GraphicsScene *,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers);
		void autoTFTriggered(const QString&);
		void autoDegradationTriggered();
		void autoPhosphateTriggered();
		void autoGeneProductTriggered();
		void autoTFTriggeredUp();
		void autoTFTriggeredDown();
		void insertmRNAstep();
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
		
		QList<QUndoCommand*> insertmRNAstep(const QList<QGraphicsItem*>& selected);
		
		static ArrayOfItems _partsIn(void*);
		static ArrayOfItems _partsUpstream(void*);
		static ArrayOfItems _partsDownstream(void*);
		static void _alignParts(ArrayOfItems);
		
		static AutoGeneRegulatoryTool_FtoS fToS;
		
	public:
		static void findAllParts(GraphicsScene*,NodeGraphicsItem*,const QString& family,QList<ItemHandle*>& ,bool upstream=true,const QStringList& stopIf=QStringList(), bool stopIfElongation = false);
		static QString hillEquation(NodeHandle *,ItemHandle* exclude = 0);
		static QString hillEquation(QList<ConnectionHandle*> connections, QList<NodeHandle*>& activators, QList<NodeHandle*> repressors);
	};


}

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);


#endif

