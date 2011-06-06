/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

The BasicGraphicsToolbar is a tool that has various simple function such as
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
#include <QActionGroup>
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
	class AlphaControllingDialog;
	/*!
	\brief A tool that provides GUI features such as alignment, zoom, and coloring
	\ingroup plugins
	*/
	class TINKERCELLEXPORT BasicGraphicsToolbar : public Tool
	{
		Q_OBJECT

	public:
		BasicGraphicsToolbar();
		bool setMainWindow(MainWindow * main);

	public slots:

		void setBackgroundImage();
		void unsetBackgroundImage();
		void bringToFront();
		void sendToBack();
		void zoomIn();
		void find();
		void closeFind();
		void rename();
		void zoomOut();
		void fitAll();
		void changeBrush();
		void changePen();
		void selectBrushColor1();
		void selectBrushAlpha1();
		void selectBrushColor2();
		void selectBrushAlpha2();
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
		void setAlphaForSelected(int);
		void alphaUp();
		void alphaDown();
		void alphaDialogOpened();

		void mousePressed(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void mouseDragged(GraphicsScene * scene, QPointF from, QPointF to, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void mouseMoved(GraphicsScene * scene, QGraphicsItem* item, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers, QList<QGraphicsItem*>&);
		void mouseReleased(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void keyPressed(GraphicsScene * scene, QKeyEvent *);
		void escapeSlot ( const QWidget * );

	private slots:
		void alphaDialogClosing();
		void revertColors(const QList<QGraphicsItem*>&);

	protected:

		QList<QGraphicsItem*> itemsToAlign ( QList< QGraphicsItem*>&);

		void moveTextGraphicsItems(QList<QGraphicsItem*> &, QList<QPointF> &, int);
		void moveChildItems(QList<QGraphicsItem*> &, QList<QPointF> &);
		void init();

		QList<QGraphicsItem*> alphaChangedItems;
		QGradient::Type gradientType;
		QPointF gradientPos1, gradientPos2;
		QToolBar * findToolBar;
		QColor brushColor1, brushColor2, penColor;
		qreal penWidth;
		QAction * changeBrushColor1, * changeBrushColor2, *changePenWidth, *changeBrushAlpha1, *changeBrushAlpha2;
		QAction * findAction;
		QSpinBox * brushAlpha1, * brushAlpha2, * penAlpha;
		QLineEdit * findText;
		QLineEdit * replaceText;
		QMenu * gradientMenu;
		QSpinBox * brightnessSpinbox;
		QIcon linearGradientIcon, radialGradientIcon;
		enum Mode { none, gradient, brush, pen, zoom, unzoom, brightness };
		Mode mode;
		QGraphicsRectItem zoomRect;			
		QAction * alignButton;
		enum AlignMode { left, right, bottom, top, centervertical, centerhorizontal, evenspacedvertical, evenspacedhorizontal, compactvertical, compacthorizontal };
		AlignMode alignMode;
		QToolBar * toolBar;
		AlphaControllingDialog * alphaDialog;
	};

	class AlphaControllingDialog : public QDialog
	{
			Q_OBJECT
		public:
			AlphaControllingDialog(QWidget * parent) : QDialog(parent)
			{
			}
		
		signals:
			void closing();
		
		protected:
			void closeEvent(QCloseEvent * )
			{
				emit closing();
			}
	};
}

#endif
