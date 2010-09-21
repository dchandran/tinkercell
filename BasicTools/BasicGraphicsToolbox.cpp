/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

The BasicGraphicsToolbox is a tool that has various simple function such as
coloring, zooming, aligning, etc. A toolbar is placed on the main window that has
buttons for all these functions.

****************************************************************************/

#include "GraphicsScene.h"
#include "ConsoleWindow.h"
#include "TextEditor.h"
#include "NetworkHandle.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "BasicGraphicsToolbox.h"
#include "GraphicsTransformTool.h"

namespace Tinkercell
{

	BasicGraphicsToolbox::BasicGraphicsToolbox() : Tool(tr("Basic Graphics Toolbox"),tr("Basic GUI"))
	{
		mode = none;
	}
	
	void BasicGraphicsToolbox::init()
	{
		QToolBar * zoomToolBar = mainWindow->toolBarForTools;
		zoomToolBar->setObjectName(tr("Zoom tool"));
		
		zoomToolBar->addAction(QIcon(tr(":/images/zoomin.png")),tr("Zoom in"),this,SLOT(zoomIn()));
		zoomToolBar->addAction(QIcon(tr(":/images/zoomout.png")),tr("Zoom out"),this,SLOT(zoomOut()));
		
		QToolBar * colorToolBar = mainWindow->toolBarForTools;
		colorToolBar->setObjectName(name);

		QToolButton * changeBrush = new QToolButton(colorToolBar);
		changeBrush->setPopupMode(QToolButton::MenuButtonPopup);
		changeBrush->setIcon(QIcon(tr(":/images/bucket.png")));
		connect(changeBrush,SIGNAL(pressed()),this,SLOT(changeBrush()));

		gradientMenu = new QMenu(tr("Gradients"),changeBrush);
		QAction * noGradient = new QAction(tr("None"),gradientMenu);
		connect(noGradient,SIGNAL(triggered()),this,SLOT(noGradient()));
		QAction * linearGradient = new QAction(tr("Linear"),gradientMenu);
		connect(linearGradient,SIGNAL(triggered()),this,SLOT(linearGradient()));
		QAction * radialGradient = new QAction(tr("Radial"),gradientMenu);
		connect(radialGradient,SIGNAL(triggered()),this,SLOT(radialGradient()));
		
		linearGradientIcon = QIcon(tr(":/images/linearGradient.png"));
		radialGradientIcon = QIcon(tr(":/images/radialGradient.png"));
		
		noGradient->setCheckable(true);
		linearGradient->setCheckable(true);
		radialGradient->setCheckable(true);
		noGradient->setChecked(true);
		QActionGroup * gradientActions = new QActionGroup(this);
		gradientActions->addAction(noGradient);
		gradientActions->addAction(linearGradient);
		gradientActions->addAction(radialGradient);
		gradientActions->setExclusive(true);

		gradientMenu->addAction(noGradient);
		gradientMenu->addAction(linearGradient);
		gradientMenu->addAction(radialGradient);
		gradientType = QGradient::NoGradient;

		brushColor1 = QColor(255,255,255,255);
		brushColor2 = QColor(80,80,255,255);
		penColor = QColor(10,10,10,255);
		penWidth = 1.0;

		QMenu * changeBrushMenu = new QMenu(gradientMenu);

		changeBrushColor1 = new QAction(tr("Color 1"),changeBrushMenu);
		connect(changeBrushColor1,SIGNAL(triggered()),this,SLOT(selectBrushColor1()));

		changeBrushColor2 = new QAction(tr("Color 2"),changeBrushMenu);
		connect(changeBrushColor2,SIGNAL(triggered()),this,SLOT(selectBrushColor2()));

		changeBrushAlpha1 = new QAction(tr("Transparency 1"),changeBrushMenu);
		connect(changeBrushAlpha1,SIGNAL(triggered()),this,SLOT(selectBrushAlpha1()));

		changeBrushAlpha2 = new QAction(tr("Transparency 2"),changeBrushMenu);
		connect(changeBrushAlpha2,SIGNAL(triggered()),this,SLOT(selectBrushAlpha2()));

		QPixmap bcolor1(20,20);
		QPainter painter1(&bcolor1);
		painter1.setBrush(QBrush(brushColor1));
		painter1.setPen(Qt::NoPen);
		painter1.drawRect(0,0,20,20);
		changeBrushColor1->setIcon(QIcon(bcolor1));

		QPixmap bcolor2(20,20);
		QPainter painter2(&bcolor2);
		painter2.setBrush(QBrush(brushColor2));
		painter2.setPen(Qt::NoPen);
		painter2.drawRect(0,0,20,20);
		changeBrushColor2->setIcon(QIcon(bcolor2));

		QPixmap balpha1(20,20);
		QPainter painter3(&balpha1);
		painter3.setBrush(QBrush(QColor(0,0,0,brushColor1.alpha())));
		painter3.setPen(Qt::NoPen);
		painter3.drawRect(0,0,20,20);
		changeBrushAlpha1->setIcon(QIcon(balpha1));

		QPixmap balpha2(20,20);
		QPainter painter4(&balpha2);
		painter4.setBrush(QBrush(QColor(0,0,0,brushColor2.alpha())));
		painter4.setPen(Qt::NoPen);
		painter4.drawRect(0,0,20,20);
		changeBrushAlpha2->setIcon(QIcon(balpha2));

		changeBrushMenu->addAction(changeBrushColor1);
		changeBrushMenu->addAction(changeBrushColor2);
		changeBrushMenu->addSeparator();
		changeBrushMenu->addMenu(gradientMenu);
		changeBrushMenu->addSeparator();
		changeBrushMenu->addAction(changeBrushAlpha1);
		changeBrushMenu->addAction(changeBrushAlpha2);
		changeBrush->setMenu(changeBrushMenu);

		QToolButton * changePen = new QToolButton(colorToolBar);
		changePen->setPopupMode(QToolButton::MenuButtonPopup);
		changePen->setIcon(QIcon(tr(":/images/pencil.png")));
		connect(changePen,SIGNAL(pressed()),this,SLOT(changePen()));

		QMenu * changePenMenu = new QMenu(changePen);
		changePenColor = new QAction(tr("Color"),0);
		connect(changePenColor,SIGNAL(triggered()),this,SLOT(selectPenColor()));
		QPixmap pcolor(20,20);
		QPainter painter5(&pcolor);
		painter5.setBrush(QBrush(penColor));
		painter5.setPen(Qt::NoPen);
		painter5.drawRect(0,0,20,20);
		changePenColor->setIcon(QIcon(pcolor));
		changePenMenu->addAction(changePenColor);
		changePen->setMenu(changePenMenu);

		QToolButton * bringFront = new QToolButton(colorToolBar);
		bringFront->setIcon(QIcon(tr(":/images/bringFront.png")));
		connect(bringFront,SIGNAL(pressed()),this,SLOT(bringToFront()));

		QToolButton * sendBack = new QToolButton(colorToolBar);
		sendBack->setIcon(QIcon(tr(":/images/sendBack.png")));
		connect(sendBack,SIGNAL(pressed()),this,SLOT(sendToBack()));

		alignButton = new QToolButton(colorToolBar);
		alignButton->setPopupMode(QToolButton::MenuButtonPopup);

		QMenu * alignMenu = new QMenu(colorToolBar);

		alignMenu->addAction(QIcon(tr(":/images/alignright.png")),tr("Align right"),
			this,SLOT(alignRight()));

		alignMenu->addAction(QIcon(tr(":/images/alignleft.png")),tr("Align left"),
			this,SLOT(alignLeft()));

		alignMenu->addAction(QIcon(tr(":/images/aligntop.png")),tr("Align top"),
			this,SLOT(alignTop()));

		alignMenu->addAction(QIcon(tr(":/images/alignbottom.png")),tr("Align bottom"),
			this,SLOT(alignBottom()));

		alignMenu->addAction(QIcon(tr(":/images/aligncompactvertical.png")),tr("Align compact vertical"),
			this,SLOT(alignCompactVertical()));

		alignMenu->addAction(QIcon(tr(":/images/aligncompacthorizontal.png")),tr("Align compact horizontal"),
			this,SLOT(alignCompactHorizontal()));

		alignMenu->addAction(QIcon(tr(":/images/aligncentervertical.png")),tr("Align evenly vertical"),
			this,SLOT(alignEvenSpacedVertical()));

		alignMenu->addAction(QIcon(tr(":/images/aligncenterhorizontal.png")),tr("Align evenly horizontal"),
			this,SLOT(alignEvenSpacedHorizontal()));

		alignButton->setMenu(alignMenu);

		alignMode = this->compacthorizontal;
		alignButton->setIcon(QIcon(tr(":/images/aligncompacthorizontal.png")));
		connect(alignButton,SIGNAL(pressed()),this,SLOT(alignSelected()));

		changeBrush->setToolTip(tr("Change fill color"));
		colorToolBar->addWidget(changeBrush);
		changePen->setToolTip(tr("Change outline color"));
		colorToolBar->addWidget(changePen);
		bringFront->setToolTip(tr("Bring items forward"));
		colorToolBar->addWidget(bringFront);
		sendBack->setToolTip(tr("Send items back"));
		colorToolBar->addWidget(sendBack);
		alignButton->setToolTip(tr("Align items"));
		colorToolBar->addWidget(alignButton);

		changePenWidth = new QAction(tr("Width = ") + QString::number(penWidth),changePenMenu);
		connect(changePenWidth,SIGNAL(triggered()),this,SLOT(selectPenWidth()));
		changePenMenu->addAction(changePenWidth);

		findText = replaceText = 0;
		findToolBar = 0;
	}

	void BasicGraphicsToolbox::selectPenWidth()
	{
		bool ok;
		double d = QInputDialog::getDouble(mainWindow,tr("Pen Width"),tr("Set pen width"),penWidth,0.1,100.0,1,&ok);
		if (ok)
		{
			penWidth = d;
			changePenWidth->setText(tr("Width = ") + QString::number(penWidth));
		}
	}

	bool BasicGraphicsToolbox::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (main != 0)
		{
			init();

			findText = new QLineEdit;
			replaceText = new QLineEdit;
			findToolBar = new QToolBar(tr("Find tool"),main->centralWidget());
			findToolBar->setObjectName(tr("Find tool"));
			
			QAction * findAction = new QAction(QIcon(tr(":/images/find.png")),tr("Find text"),main->centralWidget());
			findAction->setShortcut(QKeySequence::Find);

			QAction * replaceAction = new QAction(QIcon(tr(":/images/replace.png")),tr("Replace text"),main->centralWidget());
			replaceAction->setShortcut(QKeySequence::Replace);

			findToolBar->addWidget(findText);
			//findToolBar->addAction(QIcon(tr(":/images/find.png")),tr("Find text"),this,SLOT(find()));
			findToolBar->addAction(tr("Find"),this,SLOT(find()));
			findToolBar->addSeparator();
			findToolBar->addWidget(replaceText);
			//findToolBar->addAction(QIcon(tr(":/images/replace.png")),tr("Replace text"),this,SLOT(rename()));
			findToolBar->addAction(tr("Replace"),this,SLOT(rename()));
			findToolBar->addSeparator();

			QAction * escape1 = new QAction(findText);
			escape1->setShortcut(QKeySequence(Qt::Key_Escape));
			connect(escape1,SIGNAL(triggered()),this,SLOT(closeFind()));

			QAction * escape2 = new QAction(replaceText);
			escape2->setShortcut(QKeySequence(Qt::Key_Escape));
			connect(escape2,SIGNAL(triggered()),this,SLOT(closeFind()));

			QAction * escape = findToolBar->addAction(QIcon(tr(":/images/x.png")),tr("close"),this,SLOT(closeFind()));
			escape->setShortcut(QKeySequence(Qt::Key_Escape));

			connect(findText,SIGNAL(returnPressed()),this,SLOT(find()));
			connect(replaceText,SIGNAL(returnPressed()),this,SLOT(rename()));

			connect(findAction,SIGNAL(triggered()),findToolBar,SLOT(show()));
			connect(replaceAction,SIGNAL(triggered()),findToolBar,SLOT(show()));

			connect(findAction,SIGNAL(triggered()),findText,SLOT(setFocus()));
			connect(replaceAction,SIGNAL(triggered()),findText,SLOT(setFocus()));

			mainWindow->addToolBar(Qt::BottomToolBarArea,findToolBar);
			findToolBar->setAllowedAreas(Qt::AllToolBarAreas);
			findToolBar->setFloatable(true);
			findToolBar->setVisible(false);

			if (mainWindow->editMenu)
			{
				mainWindow->editMenu->addAction(findAction);
				mainWindow->editMenu->addAction(replaceAction);
			}

			mainWindow->contextScreenMenu.addAction(
				QIcon(tr(":/images/image.png")),
				tr("Set background image"),
				this,
				SLOT(setBackgroundImage()));

			mainWindow->contextScreenMenu.addAction(
				tr("Remove background image"),
				this,
				SLOT(unsetBackgroundImage()));

			mainWindow->contextItemsMenu.addAction(
                QIcon(tr(":/images/bringFront.png")),
                tr("Bring items forward"),
                this,
                SLOT(bringToFront()));

            mainWindow->contextItemsMenu.addAction(
                QIcon(tr(":/images/sendBack.png")),
                tr("Send items back"),
                this,
                SLOT(sendToBack()));

			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSlot(const QWidget*)));

			connect(mainWindow,SIGNAL(mousePressed(GraphicsScene * , QPointF , Qt::MouseButton, Qt::KeyboardModifiers )),
				this,SLOT(mousePressed(GraphicsScene * , QPointF , Qt::MouseButton, Qt::KeyboardModifiers )));

			connect(mainWindow,SIGNAL(mouseReleased(GraphicsScene*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
				this,SLOT(mouseReleased(GraphicsScene*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(mouseDragged(GraphicsScene*,QPointF,QPointF,Qt::MouseButton,Qt::KeyboardModifiers)),
				this,SLOT(mouseDragged(GraphicsScene*,QPointF,QPointF,Qt::MouseButton,Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(mouseMoved(GraphicsScene*, QGraphicsItem*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)),
				this,SLOT(mouseMoved(GraphicsScene*, QGraphicsItem*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)));

			return true;
		}
		return false;
	}

	void BasicGraphicsToolbox::setBackgroundImage()
	{
		if (!currentScene()) return;

		QString imageFile = QFileDialog::getOpenFileName(this,tr("Select image file"),MainWindow::previousFileName);

		if (imageFile.isEmpty() || imageFile.isNull()) return;

		QPixmap image;
		if (image.load(imageFile))
			currentScene()->setBackground(image);
	}

	void BasicGraphicsToolbox::unsetBackgroundImage()
	{
		if (!currentScene()) return;
		currentScene()->setBackground(QPixmap());
	}

	void BasicGraphicsToolbox::closeFind()
	{
		if (findText) findText->clear();
		if (replaceText) replaceText->clear();
		find();
		if (findToolBar)
			findToolBar->hide();
	}

	void BasicGraphicsToolbox::find()
	{
		if (!mainWindow || !findText) return;

		if (currentScene())
			currentScene()->find(findText->text());
		else
			if (currentTextEditor())
				currentTextEditor()->find(findText->text());
	}

	void BasicGraphicsToolbox::rename()
	{
		if (!currentNetwork() || !findText || !replaceText || findText->text().isEmpty()) return;

		if (currentNetwork())
			currentNetwork()->rename(findText->text(),replaceText->text());
	}

	void BasicGraphicsToolbox::noGradient()
	{
		if (mode == gradient)
			mode = brush;
		gradientType = QGradient::NoGradient;
		gradientMenu->setIcon(QIcon());
	}

	void BasicGraphicsToolbox::linearGradient()
	{
		if (mode == brush)
			mode = gradient;
		gradientType = QGradient::LinearGradient;
		gradientMenu->setIcon(linearGradientIcon);
	}

	void BasicGraphicsToolbox::radialGradient()
	{
		if (mode == brush)
			mode = gradient;
		gradientType = QGradient::RadialGradient;
		gradientMenu->setIcon(radialGradientIcon);
	}

	void BasicGraphicsToolbox::bringToFront()
	{
		if (mainWindow != 0 && mainWindow->currentScene() != 0)
		{
			GraphicsScene * scene = mainWindow->currentScene();
			double z = 0;
			QList<QGraphicsItem*> selected = scene->selected();
			scene->deselect();

			QList<double> zvalues;
			QRectF rect;
			for (int i=0; i < selected.size(); ++i)
				if (selected[i] != 0)
					rect = rect.unite(selected[i]->sceneBoundingRect());
			QList<QGraphicsItem*> items = scene->items(rect);
			for (int i=0; i < items.size(); ++i)
				if (items[i] != 0 && (z == 0 || items[i]->zValue() > z))
					z = items[i]->topLevelItem()->zValue();

			z += 0.1;
			for (int i=0; i < selected.size(); ++i)
				zvalues += z;

			if (!selected.isEmpty())
				scene->setZValue(tr("bring forward"),selected,zvalues);
		}
	}

	void BasicGraphicsToolbox::sendToBack()
	{
		if (mainWindow != 0 && mainWindow->currentScene() != 0)
		{
			GraphicsScene * scene = mainWindow->currentScene();
			double z = 0;
			QList<QGraphicsItem*> selected = scene->selected();
			scene->deselect();

			QList<double> zvalues;
			QRectF rect;
			for (int i=0; i < selected.size(); ++i)
				if (selected[i] != 0)
					rect = rect.unite(selected[i]->sceneBoundingRect());
			QList<QGraphicsItem*> items = scene->items(rect);
			for (int i=0; i < items.size(); ++i)
				if (items[i] != 0 && (z == 0 || items[i]->zValue() < z))
					z = items[i]->topLevelItem()->zValue();

			z -= 0.1;
			for (int i=0; i < selected.size(); ++i)
				zvalues += z;

			if (!selected.isEmpty())
				scene->setZValue(tr("send back"),selected,zvalues);
		}
	}

	void BasicGraphicsToolbox::group()
	{
		if (mainWindow != 0 && mainWindow->currentScene() != 0)
		{
			GraphicsScene * scene = mainWindow->currentScene();
			QList<QGraphicsItem*> selected = scene->selected();

			if (selected.size() < 1) return;

			scene->deselect();

			QList<QGraphicsItem*> items;
			for (int i=0; i < selected.size(); ++i)
				if (selected[i] != 0)
				{
					ConnectionGraphicsItem * connection = ConnectionGraphicsItem::topLevelConnectionItem(selected[i]);
					if (connection != 0)
					{
						QList<ConnectionGraphicsItem::ControlPoint*> cplist = connection->controlPoints();
						for (int j=0; j < cplist.size(); ++j)
							items += cplist[j];
					}
					else
						items += selected[i]->topLevelItem();
				}
				QGraphicsItemGroup * grp = new QGraphicsItemGroup(0,scene);
				scene->setParentItem(tr("items grouped"),items,grp);
		}
	}

	void BasicGraphicsToolbox::ungroup()
	{
		if (mainWindow != 0 && mainWindow->currentScene() != 0)
		{
			GraphicsScene * scene = mainWindow->currentScene();
			QList<QGraphicsItem*> selected = scene->selected();

			if (selected.size() < 1) return;
			scene->deselect();

			QGraphicsItemGroup * groupItem = 0;
			for (int i=0; i < selected.size(); ++i)
				if (selected[i] != 0)
				{
					groupItem = dynamic_cast<QGraphicsItemGroup*>(selected[i]->topLevelItem());
					if (groupItem != 0 &&
						NodeGraphicsItem::cast(selected[i]->topLevelItem()) == 0)
					{
						QList<QGraphicsItem*> list = groupItem->childItems();
						scene->setParentItem(tr("ungroup"),list,0);
						scene->remove(tr("remove group"),groupItem);
						//scene->destroyItemGroup(groupItem);
						break;
					}
				}
		}
	}

	void BasicGraphicsToolbox::zoomIn()
	{
		if (currentScene())
		{
			currentScene()->useDefaultBehavior = false;
			mainWindow->sendEscapeSignal(this);

			mainWindow->setCursor(QCursor(QPixmap(tr(":/images/zoomin.png")).scaled(25,25)));

			mode = zoom;
			currentScene()->useDefaultBehavior = false;
		}
		else
			if (currentTextEditor())
			{
				currentTextEditor()->zoomIn();
			}
	}

	void BasicGraphicsToolbox::zoomOut()
	{
		if (currentScene())
		{
			currentScene()->useDefaultBehavior = false;
			mainWindow->sendEscapeSignal(this);
			mainWindow->setCursor(QCursor(QPixmap(tr(":/images/zoomout.png")).scaled(25,25)));
			mode = unzoom;
			currentScene()->useDefaultBehavior = false;
		}
		else
			if (currentTextEditor())
			{
				currentTextEditor()->zoomOut();
			}
	}

	void BasicGraphicsToolbox::changeBrush()
	{
		if (mainWindow != 0 && mainWindow->currentScene() != 0)
			//&& (mode != none || mainWindow->currentScene()->useDefaultBehavior))
		{
			mainWindow->sendEscapeSignal(this);
			mainWindow->currentScene()->useDefaultBehavior = false;
			mainWindow->setCursor(QCursor(QPixmap(tr(":/images/bucket.png")).scaled(25,25)));
			if (gradientType == QGradient::NoGradient)
				mode = this->brush;
			else
				mode = this->gradient;
		}
	}

	void BasicGraphicsToolbox::changePen()
	{
		if (mainWindow != 0 && mainWindow->currentScene() != 0)
			//&& (mode != none || mainWindow->currentScene()->useDefaultBehavior))
		{
			mainWindow->sendEscapeSignal(this);
			mainWindow->currentScene()->useDefaultBehavior = false;
			mainWindow->setCursor(QCursor(QPixmap(tr(":/images/pencil.png")).scaled(25,25)));
			mode = this->pen;
		}
	}

	void BasicGraphicsToolbox::selectBrushColor1()
	{
		if (!mainWindow || !mainWindow->currentScene()) return;
		bool b = mainWindow->currentScene()->useDefaultBehavior;
		QColor color = QColorDialog::getColor(brushColor1);
		mainWindow->currentScene()->useDefaultBehavior = b;
		if (color.isValid())
		{
			int a = brushColor1.alpha();
			brushColor1 = color;
			brushColor1.setAlpha(a);
			QPixmap bcolor1(20,20);
			QPainter painter(&bcolor1);
			painter.setBrush(QBrush(brushColor1));
			painter.setPen(Qt::NoPen);
			painter.drawRect(0,0,20,20);
			changeBrushColor1->setIcon(QIcon(bcolor1));
		}
	}

	void BasicGraphicsToolbox::selectBrushAlpha1()
	{
		if (!mainWindow || !mainWindow->currentScene()) return;
		bool b = mainWindow->currentScene()->useDefaultBehavior;
		bool ok;
		int d = QInputDialog::getInteger(mainWindow,tr("Transparency for Color 1"),tr("select transparency (0 = completely transparent)"),brushColor1.alpha(),0,255,1,&ok);
		mainWindow->currentScene()->useDefaultBehavior = b;
		if (ok)
		{
			brushColor1.setAlpha(d);
			QPixmap balpha1(20,20);
			QPainter painter(&balpha1);
			painter.setBrush(QBrush(QColor(255-brushColor1.alpha(),255-brushColor1.alpha(),255-brushColor1.alpha())));
			painter.setPen(Qt::NoPen);
			painter.drawRect(0,0,20,20);
			changeBrushAlpha1->setIcon(QIcon(balpha1));
		}
	}

	void BasicGraphicsToolbox::selectBrushColor2()
	{
		if (!mainWindow || !mainWindow->currentScene()) return;
		bool b = mainWindow->currentScene()->useDefaultBehavior;
		QColor color = QColorDialog::getColor(brushColor2);
		mainWindow->currentScene()->useDefaultBehavior = b;
		if (color.isValid())
		{
			int a = brushColor2.alpha();
			brushColor2 = color;
			brushColor2.setAlpha(a);
			QPixmap bcolor2(20,20);
			QPainter painter(&bcolor2);
			painter.setBrush(QBrush(brushColor2));
			painter.setPen(Qt::NoPen);
			painter.drawRect(0,0,20,20);
			changeBrushColor2->setIcon(QIcon(bcolor2));
		}
	}

	void BasicGraphicsToolbox::selectBrushAlpha2()
	{
		if (!mainWindow || !mainWindow->currentScene()) return;
		bool b = mainWindow->currentScene()->useDefaultBehavior;
		bool ok;
		int d = QInputDialog::getInteger(mainWindow,tr("Transparency for Color 2"),tr("select transparency (0 = completely transparent)"),brushColor2.alpha(),0,255,1,&ok);
		mainWindow->currentScene()->useDefaultBehavior = b;
		if (ok)
		{
			brushColor2.setAlpha(d);
			QPixmap balpha2(20,20);
			QPainter painter(&balpha2);
			painter.setBrush(QBrush(QColor(255-brushColor2.alpha(),255-brushColor2.alpha(),255-brushColor2.alpha())));
			painter.setPen(Qt::NoPen);
			painter.drawRect(0,0,20,20);
			changeBrushAlpha2->setIcon(QIcon(balpha2));
		}
	}

	void BasicGraphicsToolbox::selectPenColor()
	{
		if (!mainWindow || !mainWindow->currentScene()) return;
		bool b = mainWindow->currentScene()->useDefaultBehavior;
		QColor color = QColorDialog::getColor(penColor);
		mainWindow->currentScene()->useDefaultBehavior = b;
		if (color.isValid())
		{
			penColor = color;
			QPixmap pcolor(20,20);
			QPainter painter(&pcolor);
			painter.setBrush(QBrush(penColor));
			painter.setPen(Qt::NoPen);
			painter.drawRect(0,0,20,20);
			changePenColor->setIcon(QIcon(pcolor));
		}
	}

	void BasicGraphicsToolbox::mousePressed(GraphicsScene * scene, QPointF , Qt::MouseButton button, Qt::KeyboardModifiers )
	{
		if (scene && button == Qt::LeftButton && mode == zoom)
		{
			if (zoomRect.scene() != scene)
			{
				zoomRect.setZValue(0);
				scene->addItem(&zoomRect);
				//QPen pen(Qt::DotLine);
				//pen.setColor(QColor(50,50,50,255));
				//pen.setWidthF(1.0);
				zoomRect.setPen(Qt::NoPen);
				zoomRect.setBrush(QBrush(QColor(100,100,100,100)));
			}

			zoomRect.setVisible(true);
		}
	}

	void BasicGraphicsToolbox::mouseMoved(GraphicsScene * scene, QGraphicsItem* , QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers , QList<QGraphicsItem*>& )
	{
		if (scene && button == Qt::LeftButton && mode == zoom && zoomRect.isVisible())
		{
			QRectF rect(scene->lastPoint(), point );

			zoomRect.setRect(QRectF(rect.topLeft(),rect.bottomRight()));
		}
	}

	void BasicGraphicsToolbox::mouseDragged(GraphicsScene * scene, QPointF from, QPointF to, Qt::MouseButton button, Qt::KeyboardModifiers )
	{
		if (scene == 0) return;

		if (mode == zoom)
		{
			if (button == Qt::LeftButton)
			{
				QRectF rect(from,to);
				rect = QRectF(rect.topLeft(),rect.bottomRight());
				if (scene->items(rect).size() < 2)
				{
					scene->centerOn(to);
					scene->scaleView(1.5);
				}
				else
				{
				    scene->fitInView(rect);
				}
			}

			zoomRect.setVisible(false);
			zoomRect.setRect(QRectF(0,0,0,0));

			if (zoomRect.scene() == scene)
				scene->removeItem(&zoomRect);

			scene->useDefaultBehavior = true;
			mode = this->none;

			mainWindow->setCursor(Qt::ArrowCursor);
			return;
		}
		
		if (mode == pen && button == Qt::LeftButton)
		{
			to.rx() -= 1.0;
			to.ry() += 0.5;
			
			QGraphicsItem * item = scene->itemAt(to);
			
			if (item == 0)
			{
				scene->useDefaultBehavior = true;
				if (mode != this->none)
				{
					mode = this->none;
					mainWindow->setCursor(Qt::ArrowCursor);
					return;
				}
			}
			else
			{
				if (qgraphicsitem_cast<NodeGraphicsItem::Shape*>(item) || ControlPoint::cast(item))
				{
					scene->setPen(tr("pen changed"),item,QPen(penColor,penWidth));
					return;
				}
			}
		}

		if (mode == brush && button == Qt::LeftButton)
		{
			to.rx() += 1;
			to.ry() += 0.5;

			QGraphicsItem * item = scene->itemAt(to);
			if (item == 0)
			{
				scene->useDefaultBehavior = true;
				if (mode != this->none)
				{
					mode = this->none;
					mainWindow->setCursor(Qt::ArrowCursor);
					return;
				}
			}
			else
			{
				if (qgraphicsitem_cast<NodeGraphicsItem::Shape*>(item) || ControlPoint::cast(item))
				{
					scene->setBrush(tr("brush changed"),item,QBrush(brushColor1));
					return;
				}
			}
		}
		if (mode == gradient && button == Qt::LeftButton)
		{
			from.rx() += 1;
			to.rx() += 1;
			from.ry() += 0.5;
			to.ry() += 0.5;

			QGraphicsItem * item = scene->itemAt(from);
			if (item == 0)
			{
				scene->useDefaultBehavior = true;
				if (mode != this->none)
				{
					mode = this->none;
					mainWindow->setCursor(Qt::ArrowCursor);
					return;
				}
			}
			else
			{
				if (qgraphicsitem_cast<NodeGraphicsItem::Shape*>(item) || ControlPoint::cast(item))
				{
					QRectF rect;
					if (item->parentItem())
						rect = item->parentItem()->sceneBoundingRect();
					else
						rect = item->sceneBoundingRect();
					QPointF colorPt1 = (from - rect.topLeft()),
						colorPt2 = (to - rect.topLeft()),
						p1 = item->mapFromScene(from),
						p2 = item->mapFromScene(to);

					colorPt1.rx() = colorPt1.x() / rect.width() * 100.0 - 50.0;
					colorPt2.rx() = colorPt2.x() / rect.width() * 100.0 - 50.0;
					colorPt1.ry() = colorPt1.y() / rect.height() * 100.0 - 50.0;
					colorPt2.ry() = colorPt2.y() / rect.height() * 100.0 - 50.0;

					if (gradientType == QGradient::LinearGradient)
					{
						QLinearGradient gradient(p1,p2);
						gradient.setColorAt(0,brushColor1);
						gradient.setColorAt(1,brushColor2);
						scene->setBrush(tr("brush changed"),item,QBrush(gradient));
						NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(item);
						if (shape)
						{
							shape->gradientPoints.first = colorPt1;
							shape->gradientPoints.second = colorPt2;
						}
					}
					else
					{
						QRadialGradient gradient(p1,sqrt( (p2.y()-p1.y())*(p2.y()-p1.y()) +	(p2.x()-p1.x())*(p2.x()-p1.x())));
						gradient.setColorAt(0,brushColor1);
						gradient.setColorAt(1,brushColor2);
						scene->setBrush(tr("brush changed"),item,QBrush(gradient));
						NodeGraphicsItem::Shape * shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(item);
						if (shape)
						{
							shape->gradientPoints.first = colorPt1;
							shape->gradientPoints.second = colorPt2;
						}
					}
				}
			}
		}
	}

	void BasicGraphicsToolbox::mouseReleased(GraphicsScene * scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers )
	{
		if (scene != 0 && button == Qt::LeftButton)
		{
			if (mode == zoom)
			{
				zoomRect.setVisible(false);
				zoomRect.setRect(QRectF(0,0,0,0));

				if (zoomRect.scene() == scene)
					scene->removeItem(&zoomRect);

				scene->centerOn(point);
				scene->scaleView(1.5);
			}

			else
				if (mode == unzoom)
				{
					scene->centerOn(point);
					scene->scaleView(0.75);
				}
				else
					if (mode == this->brush || mode == this->pen || mode == this->gradient)
					{
						/*if (mode != pen)
						{
							point.rx() += 1;
							point.ry() += 0.5;
						}
						else
						{
							point.rx() -= 1;
							point.ry() += 0.5;
						}*/

						QGraphicsItem * item = getGraphicsItem(scene->itemAt(point));
						if (!item || (!ConnectionGraphicsItem::cast(item) && (item->sceneBoundingRect().width() > 500 || item->sceneBoundingRect().height() > 500)))
						{
							QList<QGraphicsItem*> ps = scene->items(QRectF(point.rx()-50.0,point.ry()-50.0,100.0,100.0));
							if (!ps.isEmpty())
							{
								int i=0;
								item = getGraphicsItem(ps[i]);
								while (!item)
								{
									item = getGraphicsItem(ps[i]);
									++i;
								}
							}
						}

						if (item == 0)
						{
							scene->useDefaultBehavior = true;
							mode = this->none;
							mainWindow->setCursor(Qt::ArrowCursor);
						}
						else
						{
							NodeGraphicsItem * node = NodeGraphicsItem::cast(item);
							if (node && !node->shapes.isEmpty())
							{
								NodeGraphicsItem::Shape * shape = node->shapes[0];
								for (int i=0; i < node->shapes.size(); ++i)
									if (node->shapes[i]->sceneBoundingRect().contains(point))
									{
										shape = node->shapes[i];
										break;
									}

								if (shape)
								{
									if (mode == brush)
										scene->setBrush(tr("brush changed"),shape,QBrush(brushColor1));
									else
										if (mode == gradient)
										{
											QPointF colorPt1 = shape->sceneBoundingRect().topLeft(),
												colorPt2 = shape->sceneBoundingRect().bottomRight();
											QLinearGradient gradient(colorPt1,colorPt2);
											gradient.setColorAt(0,brushColor1);
											gradient.setColorAt(1,brushColor2);
											scene->setBrush(tr("brush changed"),shape,QBrush(gradient));
											if (shape)
											{
												shape->gradientPoints.first = colorPt1;
												shape->gradientPoints.second = colorPt2;
											}
										}
										else
										{
											scene->setPen(tr("pen changed"),shape,QPen(penColor,penWidth));
										}
								}
							}
							else
							{
								ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(item);
								if (connection)
								{
									connection->setControlPointsVisible(true);
									QPen pen = connection->defaultPen;
									pen.setColor(penColor);
									pen.setWidthF(penWidth);
									connection->setPen(pen);
									//if (mode == this->brush || mode == this->gradient)
										//scene->setBrush(tr("brush changed"), connection, QBrush(brushColor1));
									//else
										scene->setPen(tr("pen changed"), connection, pen);
									connection->refresh();
									connection->setControlPointsVisible(false);
								}
								else
								{
									TextGraphicsItem * textItem = TextGraphicsItem::cast(item);
									if (textItem != 0)
									{
										if (mode == this->brush || mode == this->gradient)
											scene->setPen(tr("text color changed"), textItem, QPen(brushColor1));
										else
											scene->setPen(tr("text color changed"), textItem, QPen(penColor));
									}
								}
							}
						}
					}
		}
	}

	void BasicGraphicsToolbox::escapeSlot(const QWidget* )
	{
		if (mode != none)
		{
			mainWindow->setCursor(Qt::ArrowCursor);
			if (mainWindow && mainWindow->currentScene())
				mainWindow->currentScene()->useDefaultBehavior = true;
			mode = none;
		}
	}

	QList<QGraphicsItem*> BasicGraphicsToolbox::itemsToAlign(QList<QGraphicsItem*>& selected)
	{
		QList<QGraphicsItem*> nodeslist, textlist, cplist;

		ItemHandle * h1, * h2;
		QList<QGraphicsItem*> itemsToRemove;
		for (int i=0; i < selected.size(); ++i)
			for (int j=0; j < selected.size(); ++j)
			{
				h1 = getHandle(selected[i]);
				h2 = getHandle(selected[j]);
				if (h1 && h2 && h1->isChildOf(h2))
				{
					for (int k=0; k < h2->graphicsItems.size(); ++k)
						itemsToRemove += h2->graphicsItems[k];
				}
			}
			for (int i=0; i < itemsToRemove.size(); ++i)
				selected.removeAll(itemsToRemove[i]);

			bool containsNodes = false,
				containsText = false,
				containsControlPoints = false;

			for (int i=0; i < selected.size(); ++i)
			{
				if (NodeGraphicsItem::cast(selected[i]))
				{
					containsNodes = true;
					nodeslist += selected[i];
				}
				else
					if (TextGraphicsItem::cast(selected[i]))
					{
						containsText = true;
						textlist += selected[i];
					}
					else
						if (ControlPoint::cast(selected[i]))
						{
							containsControlPoints = true;
							cplist += selected[i];
						}
			}

			if (!nodeslist.isEmpty()) return nodeslist;
			if (!textlist.isEmpty()) return textlist;
			return cplist;
	}

	void BasicGraphicsToolbox::moveTextGraphicsItems(QList<QGraphicsItem*> & items,QList<QPointF> & points)
	{
		NodeGraphicsItem* node = 0;
		TextGraphicsItem* text = 0;
		QPointF textPos, p;
		QRectF rect;
		for (int i=0; i < items.size() && i < points.size(); ++i)
		{
			if ((node = NodeGraphicsItem::cast(items[i])) && node->handle())
			{
				rect = node->sceneBoundingRect();
				for (int j=0; j < node->handle()->graphicsItems.size(); ++j)
				{
					if ((text = TextGraphicsItem::cast(node->handle()->graphicsItems[j]))
						&& !items.contains(text))
					{
						items += text;
						p = points[i];

						textPos = text->sceneBoundingRect().center();

						if (textPos.rx() > rect.left() && textPos.rx() < rect.right())
							p += QPointF( rect.center().x() - textPos.rx() , 0.0 );
						else
							if (textPos.ry() > rect.top() && textPos.ry() < rect.bottom())
								p += QPointF( 0.0, rect.center().y() - textPos.ry() );

						points += p; //move text along with nodes
					}
				}
			}
		}
	}

	void BasicGraphicsToolbox::moveChildItems(QList<QGraphicsItem*> & items, QList<QPointF> & points)
	{
		ItemHandle * handle;
		QList<ItemHandle*> visited;
		NodeGraphicsItem * node;
		ConnectionGraphicsItem * connection;
		for (int i=0; i < items.size() && i < points.size(); ++i)
		{
			if ((handle = getHandle(items[i])) && !visited.contains(handle) && handle->children.size() > 0)
			{
				visited += handle;
				for (int j=0; j < handle->children.size(); ++j)
					if (handle->children[j])
					{
						for (int k=0; k < handle->children[j]->graphicsItems.size(); ++k)
						{
							node = NodeGraphicsItem::cast(handle->children[j]->graphicsItems[k]);
							if (node)
							{
								if (!items.contains(node))
								{
									items << node;
									points << points[i];
								}
							}
							else
							{
								connection = ConnectionGraphicsItem::cast(handle->children[j]->graphicsItems[k]);
								if (connection)
								{
									QList<ConnectionGraphicsItem::ControlPoint*> cplist = connection->controlPoints();
									for (int l=0; l < cplist.size(); ++l)
										if (cplist[l] && !items.contains(cplist[l]))
										{
											items << cplist[l];
											points << points[l];
										}
								}
							}
						}
					}
			}
		}
	}

	void BasicGraphicsToolbox::alignLeft()
	{
		if (!mainWindow || !mainWindow->currentScene()) return;
		GraphicsScene * scene = mainWindow->currentScene();

		QList<QGraphicsItem*> list = itemsToAlign(scene->selected());

		alignMode = left;
		if (alignButton)
			alignButton->setIcon(QIcon(tr(":/images/alignleft.png")));

		if (list.size() < 2) return;

		qreal minX = -1.0;
		for (int i=0; i < list.size(); ++i)
		{
			if (list[i] && (minX < 0 || list[i]->sceneBoundingRect().left() < minX))
				minX = list[i]->sceneBoundingRect().left();
		}

		if (minX >= 0)
		{
			QList<QPointF> newPositions;
			for (int i=0; i < list.size(); ++i)
			{
				if (list[i])
				{
					QPointF pos;
					pos.rx() = minX - list[i]->sceneBoundingRect().left();
					newPositions += pos;
				}
				else
					newPositions += QPointF();
			}
			//moveChildItems(list,newPositions);
			moveTextGraphicsItems(list,newPositions);
			scene->move(list,newPositions);
		}

	}
	void BasicGraphicsToolbox::alignRight()
	{
		if (!mainWindow || !mainWindow->currentScene()) return;
		GraphicsScene * scene = mainWindow->currentScene();

		QList<QGraphicsItem*> list = itemsToAlign(scene->selected());

		alignMode = right;
		if (alignButton)
			alignButton->setIcon(QIcon(tr(":/images/alignright.png")));

		if (list.size() < 2) return;

		qreal maxX = -1.0;
		for (int i=0; i < list.size(); ++i)
		{
			if (list[i] && (maxX < 0 || list[i]->sceneBoundingRect().right() > maxX))
				maxX = list[i]->sceneBoundingRect().right();
		}

		if (maxX >= 0)
		{
			QList<QPointF> newPositions;
			for (int i=0; i < list.size(); ++i)
			{
				if (list[i])
				{
					QPointF pos;
					pos.rx() = maxX - list[i]->sceneBoundingRect().right();
					newPositions += pos;
				}
				else
					newPositions += QPointF();
			}
			//moveChildItems(list,newPositions);
			moveTextGraphicsItems(list,newPositions);
			scene->move(list,newPositions);
		}
	}
	void BasicGraphicsToolbox::alignTop()
	{
		if (!mainWindow || !mainWindow->currentScene()) return;
		GraphicsScene * scene = mainWindow->currentScene();

		QList<QGraphicsItem*> list = itemsToAlign(scene->selected());

		alignMode = top;
		if (alignButton)
			alignButton->setIcon(QIcon(tr(":/images/aligntop.png")));

		if (list.size() < 2) return;

		qreal minY = -1.0;
		for (int i=0; i < list.size(); ++i)
		{
			if (list[i] && (minY < 0 || list[i]->sceneBoundingRect().top() < minY))
				minY = list[i]->sceneBoundingRect().top();
		}

		if (minY >= 0)
		{
			QList<QPointF> newPositions;
			for (int i=0; i < list.size(); ++i)
			{
				if (list[i])
				{
					QPointF pos;
					pos.ry() = minY - list[i]->sceneBoundingRect().top();
					newPositions += pos;
				}
				else
					newPositions += QPointF();
			}
			//moveChildItems(list,newPositions);
			moveTextGraphicsItems(list,newPositions);
			scene->move(list,newPositions);
		}
	}
	void BasicGraphicsToolbox::alignBottom()
	{
		if (!mainWindow || !mainWindow->currentScene()) return;
		GraphicsScene * scene = mainWindow->currentScene();

		QList<QGraphicsItem*> list = itemsToAlign(scene->selected());

		alignMode = bottom;
		if (alignButton)
			alignButton->setIcon(QIcon(tr(":/images/alignbottom.png")));

		if (list.size() < 2) return;

		qreal maxY = -1.0;
		for (int i=0; i < list.size(); ++i)
		{
			if (list[i] && (maxY < 0 || list[i]->sceneBoundingRect().bottom() > maxY))
				maxY = list[i]->sceneBoundingRect().bottom();
		}

		if (maxY >= 0)
		{
			QList<QPointF> newPositions;
			for (int i=0; i < list.size(); ++i)
			{
				if (list[i])
				{
					QPointF pos;
					pos.ry() = maxY - list[i]->sceneBoundingRect().bottom();
					newPositions += pos;
				}
				else
					newPositions += QPointF();
			}
			//moveChildItems(list,newPositions);
			moveTextGraphicsItems(list,newPositions);
			scene->move(list,newPositions);
		}
	}
	void BasicGraphicsToolbox::alignCompactVertical()
	{
		if (!mainWindow || !mainWindow->currentScene()) return;
		GraphicsScene * scene = mainWindow->currentScene();

		QList<QGraphicsItem*> list;
		QList<QGraphicsItem*> selected = itemsToAlign(scene->selected());

		if (selected.isEmpty()) return;

		qreal averageWidth = 0;

		for (int i=0; i < selected.size(); ++i)
		{
			if (selected[i])  //make a list where the selected items are ordered by y-pos
			{
				averageWidth += selected[i]->sceneBoundingRect().width();
				bool inserted = false;
				for (int j=0; j < list.size(); ++j)
				{
					if (list[j] && list[j]->scenePos().y() > selected[i]->scenePos().y())
					{
						list.insert(j,selected[i]);
						inserted = true;
						break;
					}
				}
				if (!inserted)
					list += selected[i];
			}
		}

		averageWidth /= selected.size();

		alignMode = compactvertical;
		if (alignButton)
			alignButton->setIcon(QIcon(tr(":/images/aligncompactvertical.png")));

		if (list.size() < 2 || !list[0]) return;

		qreal x = list[0]->sceneBoundingRect().left(),
			y = list[0]->sceneBoundingRect().bottom();

		QList<QPointF> newPositions;
		newPositions += QPointF(); //first item does not move

		for (int i=1; i < list.size(); ++i)
		{
			if (list[i])
			{
				QPointF pos;
				pos.setX(x - list[i]->sceneBoundingRect().left());
				pos.setY(y - list[i]->sceneBoundingRect().top());
				newPositions += pos;
				y += list[i]->sceneBoundingRect().height();
			}
			else
				newPositions += QPointF();
		}
		//moveChildItems(list,newPositions);
		moveTextGraphicsItems(list,newPositions);
		scene->move(list,newPositions);

		/*QList<QPointF> changeWidth;
		for (int i=0; i < selected.size(); ++i)
		changeWidth += QPointF(averageWidth/selected[i]->sceneBoundingRect().width(),1.0);

		QList<qreal> emptyList;

		scene->transform(tr("change size"),selected,changeWidth,emptyList,false,false);*/
	}
	void BasicGraphicsToolbox::alignCompactHorizontal()
	{
		if (!mainWindow || !mainWindow->currentScene()) return;
		GraphicsScene * scene = mainWindow->currentScene();

		QList<QGraphicsItem*> list;
		QList<QGraphicsItem*> selected = itemsToAlign(scene->selected());

		if (selected.isEmpty()) return;

		qreal averageHeight = 0;

		for (int i=0; i < selected.size(); ++i)
		{
			if (selected[i])  //make a list where the selected items are ordered by y-pos
			{
				averageHeight += selected[i]->sceneBoundingRect().height();
				bool inserted = false;
				for (int j=0; j < list.size(); ++j)
				{
					if (list[j] && list[j]->sceneBoundingRect().left() > selected[i]->sceneBoundingRect().left())
					{
						list.insert(j,selected[i]);
						inserted = true;
						break;
					}
				}
				if (!inserted)
					list += selected[i];
			}
		}

		averageHeight /= selected.size();

		alignMode = compacthorizontal;
		if (alignButton)
			alignButton->setIcon(QIcon(tr(":/images/aligncompacthorizontal.png")));

		if (list.size() < 2 || !list[0]) return;

		qreal x = list[0]->sceneBoundingRect().right(),
			y = list[0]->sceneBoundingRect().bottom();

		QList<QPointF> newPositions;
		newPositions += QPointF(); //first item does not move

		for (int i=1; i < list.size(); ++i)
		{
			if (list[i])
			{
				QPointF pos;
				pos.setY(y - list[i]->sceneBoundingRect().bottom());
				pos.setX(x - list[i]->sceneBoundingRect().left());
				newPositions += pos;
				x += list[i]->sceneBoundingRect().width();
			}
			else
				newPositions += QPointF();
		}
		//moveChildItems(list,newPositions);
		moveTextGraphicsItems(list,newPositions);
		scene->move(list,newPositions);
		/*
		QList<QPointF> changeHeight;
		for (int i=0; i < selected.size(); ++i)
		changeHeight += QPointF(1.0,averageHeight/selected[i]->sceneBoundingRect().height());

		QList<qreal> emptyList;

		scene->transform(tr("change size"),selected,changeHeight,emptyList,false,false);*/
	}
	void BasicGraphicsToolbox::alignEvenSpacedVertical()
	{
		if (!mainWindow || !mainWindow->currentScene()) return;
		GraphicsScene * scene = mainWindow->currentScene();

		QList<QGraphicsItem*> list;
		QList<QGraphicsItem*> selected = itemsToAlign(scene->selected());

		for (int i=0; i < selected.size(); ++i)
		{
			if (selected[i])  //make a list where the selected items are ordered by y-pos
			{
				bool inserted = false;
				for (int j=0; j < list.size(); ++j)
				{
					if (list[j] && list[j]->scenePos().y() > selected[i]->scenePos().y())
					{
						list.insert(j,selected[i]);
						inserted = true;
						break;
					}
				}
				if (!inserted)
					list += selected[i];
			}
		}

		alignMode = evenspacedvertical;
		if (alignButton)
			alignButton->setIcon(QIcon(tr(":/images/aligncentervertical.png")));

		if (list.size() < 2 || !list[0]) return;

		qreal x = 0, y = 0, minY = -1, maxY = -1, totalHeights = 0;

		for (int i=0; i < list.size(); ++i)
		{
			QRectF rect = list[i]->sceneBoundingRect();

			if (list[i])
				x += rect.center().x();

			if (minY < 0 || minY > rect.top())
				minY = rect.top();

			if (maxY < 0 || maxY < rect.bottom())
				maxY = rect.bottom();

			totalHeights += list[i]->sceneBoundingRect().height();
		}

		x /= list.size(); //center x
		qreal dy = (maxY - minY - totalHeights)/(list.size() - 1); //distance between each item

		QList<QPointF> newPositions;

		y = minY;

		for (int i=0; i < list.size(); ++i)
		{
			if (list[i])
			{
				QPointF pos;
				pos.setX(x - list[i]->scenePos().x());
				pos.setY(y - list[i]->sceneBoundingRect().top());
				newPositions += pos;
				y += list[i]->sceneBoundingRect().height() + dy;
			}
			else
				newPositions += QPointF();
		}
		//moveChildItems(list,newPositions);
		moveTextGraphicsItems(list,newPositions);
		scene->move(list,newPositions);
	}
	void BasicGraphicsToolbox::alignEvenSpacedHorizontal()
	{
		if (!mainWindow || !mainWindow->currentScene()) return;
		GraphicsScene * scene = mainWindow->currentScene();

		QList<QGraphicsItem*> list;
		QList<QGraphicsItem*> selected = itemsToAlign(scene->selected());

		for (int i=0; i < selected.size(); ++i)
		{
			if (selected[i])  //make a list where the selected items are ordered by y-pos
			{
				bool inserted = false;
				for (int j=0; j < list.size(); ++j)
				{
					if (list[j] && list[j]->scenePos().x() > selected[i]->scenePos().x())
					{
						list.insert(j,selected[i]);
						inserted = true;
						break;
					}
				}
				if (!inserted)
					list += selected[i];
			}
		}

		alignMode = evenspacedhorizontal;
		if (alignButton)
			alignButton->setIcon(QIcon(tr(":/images/aligncenterhorizontal.png")));

		if (list.size() < 2 || !list[0]) return;

		qreal x = 0, y = 0, minX = -1, maxX = -1, totalWidths = 0;

		for (int i=0; i < list.size(); ++i)
		{
			QRectF rect = list[i]->sceneBoundingRect();

			if (list[i])
				y += rect.center().y();

			if (minX < 0 || minX > rect.left())
				minX = rect.left();

			if (maxX < 0 || maxX < rect.right())
				maxX = rect.right();

			totalWidths += list[i]->sceneBoundingRect().width();
		}

		y /= list.size(); //center x
		qreal dx = (maxX - minX - totalWidths)/(list.size() - 1); //distance between each item

		QList<QPointF> newPositions;

		x = minX;

		for (int i=0; i < list.size(); ++i)
		{
			if (list[i])
			{
				QPointF pos;
				pos.setY(y - list[i]->scenePos().y());
				pos.setX(x - list[i]->sceneBoundingRect().left());
				newPositions += pos;
				x += list[i]->sceneBoundingRect().width() + dx;
			}
			else
				newPositions += QPointF();
		}
		//moveChildItems(list,newPositions);
		moveTextGraphicsItems(list,newPositions);
		scene->move(list,newPositions);
	}

	void BasicGraphicsToolbox::alignSelected()
	{
		switch (alignMode)
		{
		case left: return alignLeft();
		case right: return alignRight();
		case top: return alignTop();
		case bottom: return alignBottom();
		case compactvertical: return alignCompactVertical();
		case compacthorizontal: return alignCompactHorizontal();
		case evenspacedvertical: return alignEvenSpacedVertical();
		case evenspacedhorizontal: return alignEvenSpacedHorizontal();
		default: return;
		}
	}

}