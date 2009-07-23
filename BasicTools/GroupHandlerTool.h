/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool allows NodeGraphicsItems to be grouped together (i.e. merge handlers). 
 A special QUndoCommand is provided for this functionality. Buttons are also placed
 in the MainWindow toolbar.

****************************************************************************/

#ifndef TINKERCELL_HANDLESTOOLBOX_H
#define TINKERCELL_HANDLESTOOLBOX_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
#include <QPixmap>
#include <QString>
#include <QtDebug>
#include <QUndoCommand>
#include <QToolButton>

#include "ItemHandle.h"
#include "Tool.h"
#include "MainWindow.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	class GroupHandlerTool_FToS : public QObject
	{
		Q_OBJECT
		signals:
			void merge(QSemaphore*, QList<ItemHandle*>&);
			void separate(QSemaphore*,ItemHandle*);
		public slots:
			void merge(Array);
			void separate(OBJ);
	};

	class MY_EXPORT GroupHandlerTool : public Tool
	{
		Q_OBJECT;
	
	signals:
		void handlesChanged(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>& old);
		
	public:
		GroupHandlerTool();
		bool setMainWindow(MainWindow * main);
		void mergeItems(GraphicsScene*,QList<QGraphicsItem*>&);
		void separateItems(GraphicsScene*,QList<QGraphicsItem*>&);
	public slots:
		void merge();
		void separate();
		void alias();
		void nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , QPointF , Qt::KeyboardModifiers );
		void toolLoaded(Tool*);
		void setupFunctionPointers( QLibrary * );
	private slots:
		void merge(QSemaphore*, QList<ItemHandle*>&);
		void separate(QSemaphore*,ItemHandle*);
	protected:
		QList<QGraphicsItem*> targetItems;
		void connectCollisionDetector();
		static QString findUniqueName(const QStringList&);
		void connectTCFunctions();
		static GroupHandlerTool_FToS fToS;
		static void _merge( Array );
		static void _separate(OBJ );
		static ItemFamily* findBestFamilyOption(QList<ItemFamily*>&);
	};

}

#endif
