/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool contains a set of transformation function such as scale, rotate, etc.
A GraphicsTool is also defined that brings up the transformations window

****************************************************************************/

#ifndef TINKERCELL_GRAPHICSTRANSFORMTOOL_H
#define TINKERCELL_GRAPHICSTRANSFORMTOOL_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
#include <QPixmap>
#include <QString>
#include <QtDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAction>
#include <QFile>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QUndoCommand>
#include <QGraphicsRectItem>
#include <QGraphicsWidget>
#include <QGroupBox>
#include <QSlider>
#include <QToolButton>
#include <QPushButton>
#include <QButtonGroup>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "NodeGraphicsItem.h"
#include "NodesTree.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	class MY_EXPORT GraphicsTransformTool : public Tool
	{
		Q_OBJECT;

	public:
		GraphicsTransformTool();
		bool setMainWindow(MainWindow * main);
		public slots:
			void select(int);
			void deselect(int);

			void selectPenColor();
			void selectFillColor();
			void scale(int);
			void rotate(int);
			void scaleStart();
			void scaleEnd();
			void rotateStart();
			void rotateEnd();
			void reset();
			void onHide(bool);
			void flipVertically();
			void flipHorizontally();
			void updateTable();

			void itemsInserted(GraphicsScene* scene, const QList<QGraphicsItem *>& items, const QList<ItemHandle*>& handles);
			void itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
			void toolLoaded(Tool*);

			void xchanged(double dx);
			void ychanged(double dy);
			void wchanged(double dw);
			void hchanged(double dh);
			void linewchanged(double value);
			void alphaChanged(double value);
			void changePenType(int);

	protected:

		QList<QGraphicsItem*> targetItems;
		int scaleRef;
		int rotateRef;
		qreal totalScaled;
		qreal totalRotated;
		QSlider scaleSlider;
		QSlider rotateSlider;

		QDockWidget * dockWidget;
		bool moving;

		QTableWidget * tableWidget;
		QTableWidget * connectionsTableWidget;
		void setupTable();

		QGroupBox * sizeGroup;
		QGroupBox * connectionsGroup;
		QPushButton * replaceButton;

		QList<QWidget*> connectionTableWidgets;
		void connectionSelectionTool();

		NodeGraphicsItem eye;
		bool openedByUser;
	};


}

#endif
