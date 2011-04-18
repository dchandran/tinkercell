/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool provides functions for the C API, allowing temporary labels to be placed on top
 of items.

****************************************************************************/

#ifndef TINKERCELL_LABELINGTOOL_H
#define TINKERCELL_LABELINGTOOL_H

#include <QtGui>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QPair>
#include <QList>
#include <QTimeLine>

#include "GraphicsScene.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "MainWindow.h"
#include "NetworkHandle.h"
#include "ItemHandle.h"
#include "Tool.h"

namespace Tinkercell
{

	class LabelingTool_FToS : public QObject
	{
		Q_OBJECT
		signals:
			void highlightItem(ItemHandle*,QColor);
			void displayText(ItemHandle*,const QString&);
			void setLabelColor(QColor, QColor);
			void displayFire(ItemHandle*, double);
		public slots:
			void highlightItem(long,const char *);
			void displayText(long,const char*);
			void displayNumber(long,double);
			void setDisplayLabelColor(const char *, const char *);
			void displayFire(long, double);
	};

	/*!
	\brief A tool that provides features for highlighting or marking items on the scene
	\ingroup plugins
	*/
	class TINKERCELLEXPORT LabelingTool : public Tool
	{
		Q_OBJECT;

	public:

		LabelingTool();
		virtual ~LabelingTool();
		bool setMainWindow(MainWindow * main);
		
	public slots:
		void historyChanged(int);
		void setupFunctionPointers( QLibrary * library );
		void clearLabels(ItemHandle * h=0);
		void keyPressed(GraphicsScene * scene, QKeyEvent *);
		void sceneDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void itemsSelected(GraphicsScene *scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
		void escapeSignal(const QWidget*);
		void networkClosing(NetworkHandle * , bool *);
		void displayText(ItemHandle*, const QString&);
		void highlightItem(ItemHandle*,QColor);
		void displayFire(ItemHandle*,double);
		void setDisplayLabelColor(QColor, QColor);
		
	protected:
		QList< QPair<ItemHandle*,QGraphicsSimpleTextItem*> > textItems;
		QList< QPair<ItemHandle*,QGraphicsRectItem*> > rectItems;
		QList< QPair<ItemHandle*,QGraphicsEllipseItem*> > ellipseItems;
		QList< QPair<ItemHandle*, QPair<NodeGraphicsItem*,double> > > fireItems;
		NodeGraphicsItem * fireNode;
		QColor bgColor, textColor;
		
		static LabelingTool_FToS fToS;
		static void _highlightItem(long,const char *);
		static void _displayText(long,const char*);
		static void _displayNumber(long,double);
		static void _setDisplayLabelColor(const char *, const char *);
		static void _displayFire(long,double);
	private slots:
		/*!
		\brief used to make items "glow" using a QTimeLine
		\param int the time line status
		*/
		void makeNodeGlow(int alpha);
	private:
		/*!\brief timer used to make fire animation*/
		QTimeLine glowTimer;
	};


}

#endif

