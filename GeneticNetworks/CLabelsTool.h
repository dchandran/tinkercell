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
#include <QPair>
#include <QList>

#include "GraphicsScene.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "MainWindow.h"
#include "NetworkWindow.h"
#include "ItemHandle.h"
#include "Tool.h"

namespace Tinkercell
{

	class CLabelsTool_FToS : public QObject
	{
		Q_OBJECT
		signals:
			void highlightItem(ItemHandle*,QColor);
			void displayText(ItemHandle*,const QString&);
			void setLabelColor(QColor, QColor);
		public slots:
			void highlightItem(void*,const char *);
			void displayText(void*,const char*);
			void displayNumber(void*,double);
			void setDisplayLabelColor(const char *, const char *);
	};

	class TINKERCELLEXPORT CLabelsTool : public Tool
	{
		Q_OBJECT;

	public:

		CLabelsTool();
		bool setMainWindow(MainWindow * main);
		
	public slots:
		void historyChanged(int);
		void setupFunctionPointers( QLibrary * library );
		void clearLabels(ItemHandle * h=0);
		void keyPressed(GraphicsScene * scene, QKeyEvent *);
		void sceneDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void itemsSelected(GraphicsScene *scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
		void escapeSignal(const QWidget*);
		void sceneClosing(NetworkWindow * , bool *);
		void displayText(ItemHandle*, const QString&);
		void highlightItem(ItemHandle*,QColor);
		void setDisplayLabelColor(QColor, QColor);
		
	protected:
		QList< QPair<ItemHandle*,QGraphicsSimpleTextItem*> > textItems;
		QList< QPair<ItemHandle*,QGraphicsRectItem*> > rectItems;
		QList< QPair<ItemHandle*,QGraphicsEllipseItem*> > ellipseItems;
		QColor bgColor, textColor;
		
		static CLabelsTool_FToS fToS;
		static void _highlightItem(void*,const char *);
		static void _displayText(void*,const char*);
		static void _displayNumber(void*,double);
		static void _setDisplayLabelColor(const char *, const char *);
	};


}

#endif

