/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 The BasicGraphicsToolbox is a tool that has various simple function such as
 coloring, zooming, aligning, etc. A toolbar is placed on the main window that has
 buttons for all these functions.

****************************************************************************/

#ifndef TINKERCELL_BASICGRAPHICSTOOLBOX_H
#define TINKERCELL_BASICGRAPHICSTOOLBOX_H

#include <QtGui>
#include <QIcon>
#include <QWidget>
#include <QPointF>
#include <QList>
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
#include <QSlider>
#include <QToolButton>
#include <QButtonGroup>
#include <QTableView>

#include "MainWindow.h"
#include "ItemHandle.h"
#include "Tool.h"


namespace Tinkercell
{
	class BasicGraphicsToolbox_FToS : public QObject
	{
		Q_OBJECT
		signals:
			void getColorR(QSemaphore*,int*,ItemHandle*);
			void getColorG(QSemaphore*,int*,ItemHandle*);
			void getColorB(QSemaphore*,int*,ItemHandle*);
			void setColor(QSemaphore*,ItemHandle*,int,int,int,int);

		public slots:
			int getColorR(OBJ);
			int getColorG(OBJ);
			int getColorB(OBJ);
			void setColor(OBJ,int,int,int,int);
	};

	class BasicGraphicsToolbox : public Tool
	{
		Q_OBJECT

	public:
		BasicGraphicsToolbox();
		bool setMainWindow(MainWindow * main);

	private slots:
		void getColorRGB(ItemHandle*,int*,int);
		void getColorR(QSemaphore*,int*,ItemHandle*);
		void getColorG(QSemaphore*,int*,ItemHandle*);
		void getColorB(QSemaphore*,int*,ItemHandle*);
                void setColor(QSemaphore*,ItemHandle*,int,int,int,int);

	public slots:

		void setupFunctionPointers( QLibrary * );
		void bringToFront();
		void sendToBack();
		void zoomIn();
		void find();
                void closeFind();
                void rename();
		void zoomOut();
		void group();
		void ungroup();
		void changeBrush();
		void changePen();
		void selectBrushColor1();
		void selectBrushAlpha1();
		void selectBrushColor2();
		void selectBrushAlpha2();
		void selectPenColor();
		void selectPenWidth();
		void noGradient();
		void linearGradient();
		void radialGradient();
		void alignLeft();
		void alignRight();
		void alignTop();
		void alignBottom();
		void alignCompactVertical();
		void alignCompactHorizontal();
		void alignEvenSpacedVertical();
		void alignEvenSpacedHorizontal();
		void alignSelected();

		void mouseDragged(GraphicsScene * scene, QPointF from, QPointF to, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void mouseMoved(GraphicsScene * scene, QGraphicsItem* item, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers, QList<QGraphicsItem*>&);
		void mouseReleased(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void escapeSlot ( const QWidget * );

	protected:

		QList<QGraphicsItem*> itemsToAlign ( QList< QGraphicsItem*>&);

		void moveTextGraphicsItems(QList<QGraphicsItem*> &, QList<QPointF> &);
		void moveChildItems(QList<QGraphicsItem*> &, QList<QPointF> &);

		QList<QGraphicsItem*> targetItems;
		QGradient::Type gradientType;
		QPointF gradientPos1, gradientPos2;
                QToolBar* colorToolBar, * findToolBar;
		QColor brushColor1, brushColor2, penColor;
		qreal penWidth;
		QAction * changeBrushColor1, * changeBrushColor2, * changePenColor, *changePenWidth, *changeBrushAlpha1, *changeBrushAlpha2;
                QAction * findAction;
		QSpinBox * brushAlpha1, * brushAlpha2, * penAlpha;
		QLineEdit * findText;
                QLineEdit * replaceText;
		QMenu * gradientMenu;
		QIcon linearGradientIcon, radialGradientIcon;
		enum Mode { none, gradient, brush, pen, zoom, unzoom };
		Mode mode;
		QGraphicsRectItem zoomRect;
		QToolButton * alignButton;
		enum AlignMode { left, right, bottom, top, centervertical, centerhorizontal, evenspacedvertical, evenspacedhorizontal, compactvertical, compacthorizontal };
		AlignMode alignMode;

        private:
		static BasicGraphicsToolbox_FToS fToS;
		void connectTCFunctions();
		static int _getColorR(OBJ);
		static int _getColorG(OBJ);
		static int _getColorB(OBJ);
		static void _setColor(OBJ,int,int,int,int);
		QList<QGraphicsItem*> temporarilyChangedItems;
	};


}

#endif
