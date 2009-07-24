/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool provides functions for the C API, allowing temporary labels to be placed on top
 of items.

****************************************************************************/

#ifndef TINKERCELL_CLABELSTOOL_H
#define TINKERCELL_CLABELSTOOL_H

#include <QtGui>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>

#include "GraphicsScene.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "MainWindow.h"
#include "NetworkWindow.h"
#include "ItemHandle.h"
#include "Tool.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	class CLabelsTool_FToS : public QObject
	{
		Q_OBJECT
		signals:
			void highlightItem(ItemHandle*,QColor);
			void displayText(ItemHandle*,const QString&);
			void setLabelColor(int r1, int g1, int b1, int r2, int g2, int b2);
		public slots:
			void highlightItem(OBJ,int,int,int);
			void displayText(OBJ,const char*);
			void displayNumber(OBJ,double);
			void setDisplayLabelColor(int r1, int g1, int b1, int r2, int g2, int b2);
	};

	class MY_EXPORT CLabelsTool : public Tool
	{
		Q_OBJECT;

	public:

		CLabelsTool();
		bool setMainWindow(MainWindow * main);
		
	public slots:
		void setupFunctionPointers( QLibrary * library );
		void clearLabels();
		void keyPressed(GraphicsScene * scene, QKeyEvent *);
		void sceneDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void itemsSelected(GraphicsScene *scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
		void escapeSignal(const QWidget*);
		void sceneClosing(NetworkWindow * , bool *);
		void displayText(ItemHandle*, const QString&);
		void highlightItem(ItemHandle*,QColor);
		void setDisplayLabelColor(int r1, int g1, int b1, int r2, int g2, int b2);
		
	protected:
		QList<QGraphicsSimpleTextItem*> textItems;
		QList<QGraphicsRectItem*> rectItems;
		QList<QGraphicsEllipseItem*> ellipseItems;
		QColor bgColor, textColor;
		
		static CLabelsTool_FToS fToS;
		static void _highlightItem(OBJ,int,int,int);
		static void _displayText(OBJ,const char*);
		static void _displayNumber(OBJ,double);
		static void _setDisplayLabelColor(int r1, int g1, int b1, int r2, int g2, int b2);
	};


}

#endif
