/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

The BasicGraphicsToolbox is a tool that has various simple function such as
coloring, zooming, aligning, etc. A toolbar is placed on the main window that has
buttons for all these functions.

****************************************************************************/

#include "GraphicsScene.h"
#include "TextEditor.h"
#include "NetworkWindow.h"
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
		colorToolBar = new QToolBar(name,this);
		gradientType = QGradient::LinearGradient;
		QToolButton * changeBrush = new QToolButton(colorToolBar);
		changeBrush->setPopupMode(QToolButton::MenuButtonPopup);
		changeBrush->setIcon(QIcon(tr(":/images/bucket.png")));
		connect(changeBrush,SIGNAL(pressed()),this,SLOT(changeBrush()));

		gradientMenu = new QMenu(tr("Gradients"),changeBrush);
		QAction * noGradient = new QAction(tr("None"),gradientMenu);
		connect(noGradient,SIGNAL(triggered()),this,SLOT(noGradient()));
		QAction * linearGradient = new QAction(linearGradientIcon = QIcon(tr(":/images/linearGradient.png")),tr("Linear"),gradientMenu);
		connect(linearGradient,SIGNAL(triggered()),this,SLOT(linearGradient()));
		QAction * radialGradient = new QAction(radialGradientIcon = QIcon(tr(":/images/radialGradient.png")),tr("Radial"),gradientMenu);
		connect(radialGradient,SIGNAL(triggered()),this,SLOT(radialGradient()));

		gradientMenu->addAction(noGradient);
		gradientMenu->addAction(linearGradient);
		gradientMenu->addAction(radialGradient);

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

		//QToolButton * group = new QToolButton;
		//group->setIcon(QIcon(tr(":/images/group.png")));
		//connect(group,SIGNAL(pressed()),this,SLOT(group()));

		//QToolButton * ungroup = new QToolButton;
		//ungroup->setIcon(QIcon(tr(":/images/ungroup.png")));
		//connect(ungroup,SIGNAL(pressed()),this,SLOT(ungroup()));

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

		//group->setToolTip(tr("group selected items"));
		//colorToolBar->addWidget(group);
		//ungroup->setToolTip(tr("ungroup selected items"));
		//colorToolBar->addWidget(ungroup);

		changePenWidth = new QAction(tr("Width = ") + QString::number(penWidth),changePenMenu);
		connect(changePenWidth,SIGNAL(triggered()),this,SLOT(selectPenWidth()));
		changePenMenu->addAction(changePenWidth);

		connectTCFunctions();

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
			QToolBar * zoomToolBar = new QToolBar(tr("Zoom tool"),main->centralWidget());
			zoomToolBar->addAction(QIcon(tr(":/images/zoomin.png")),tr("Zoom in"),this,SLOT(zoomIn()));
			zoomToolBar->addAction(QIcon(tr(":/images/zoomout.png")),tr("Zoom out"),this,SLOT(zoomOut()));

			mainWindow->addToolBar(zoomToolBar);

			if (colorToolBar)
				mainWindow->addToolBar(colorToolBar);

			findText = new QLineEdit;
			replaceText = new QLineEdit;
			findToolBar = new QToolBar(tr("Find tool"),main->centralWidget());
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

			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSlot(const QWidget*)));

			connect(mainWindow,SIGNAL(mousePressed(GraphicsScene * , QPointF , Qt::MouseButton, Qt::KeyboardModifiers )),
				this,SLOT(mousePressed(GraphicsScene * , QPointF , Qt::MouseButton, Qt::KeyboardModifiers )));

			connect(mainWindow,SIGNAL(mouseReleased(GraphicsScene*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
				this,SLOT(mouseReleased(GraphicsScene*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(mouseDragged(GraphicsScene*,QPointF,QPointF,Qt::MouseButton,Qt::KeyboardModifiers)),
				this,SLOT(mouseDragged(GraphicsScene*,QPointF,QPointF,Qt::MouseButton,Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(mouseReleased(GraphicsScene*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
				this,SLOT(mouseReleased(GraphicsScene*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(mouseMoved(GraphicsScene*, QGraphicsItem*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)),
				this,SLOT(mouseMoved(GraphicsScene*, QGraphicsItem*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers, QList<QGraphicsItem*>&)));

			return true;
		}
		return false;
	}

	void BasicGraphicsToolbox::setBackgroundImage()
	{
		GraphicsView * currentView;
		if (!currentWindow() || !(currentView = currentWindow()->currentView())) return;

		QString imageFile = QFileDialog::getOpenFileName(this,tr("Select image file"),MainWindow::previousFileName);
		
		if (imageFile.isEmpty() || imageFile.isNull()) return;

		if (!currentView->background.load(imageFile))
			currentView->background = QPixmap();
	}

	void BasicGraphicsToolbox::unsetBackgroundImage()
	{
		GraphicsView * currentView;
		if (!currentWindow() || !(currentView = currentWindow()->currentView())) return;
		currentView->background = QPixmap();
	}

	void BasicGraphicsToolbox::connectTCFunctions()
	{
		connect(&fToS,SIGNAL(getColorR(QSemaphore*,int*,ItemHandle*)),this,SLOT(getColorR(QSemaphore*,int*,ItemHandle*)));
		connect(&fToS,SIGNAL(getColorG(QSemaphore*,int*,ItemHandle*)),this,SLOT(getColorG(QSemaphore*,int*,ItemHandle*)));
		connect(&fToS,SIGNAL(getColorB(QSemaphore*,int*,ItemHandle*)),this,SLOT(getColorB(QSemaphore*,int*,ItemHandle*)));
		connect(&fToS,SIGNAL(setColor(QSemaphore*,ItemHandle*,int,int,int,int)),this,SLOT(setColor(QSemaphore*,ItemHandle*,int,int,int,int)));
		connect(&fToS,SIGNAL(changeGraphics(QSemaphore*,ItemHandle*,const QString&)),
				this,SLOT(changeGraphics(QSemaphore*,ItemHandle*,const QString&)));
		connect(&fToS,SIGNAL(changeArrowHead(QSemaphore*,ItemHandle*,const QString&)),
				this,SLOT(changeArrowHead(QSemaphore*,ItemHandle*,const QString&)));
	}

	typedef void (*tc_BasicGraphicsToolbox_api)(
		int (*getColorR)(OBJ),
		int (*getColorG)(OBJ),
		int (*getColorB)(OBJ),
		void (*setColor)(OBJ,int,int,int, int),
		void (*changeGraphics)(OBJ,const char*),
		void (*changeArrowHead)(OBJ,const char*));


	void BasicGraphicsToolbox::setupFunctionPointers( QLibrary * library )
	{
		tc_BasicGraphicsToolbox_api f = (tc_BasicGraphicsToolbox_api)library->resolve("tc_BasicGraphicsToolbox_api");
		if (f)
		{
			//qDebug() << "tc_BasicGraphicsToolbox_api resolved";
			f(
				&(_getColorR),
				&(_getColorG),
				&(_getColorB),
				&(_setColor),
				&(_changeGraphics),
				&(_changeArrowHead)
				);
		}
	}

	void BasicGraphicsToolbox::getColorRGB(ItemHandle* handle,int* r,int rgb)
	{
		if (!handle || !r) return;
		QGraphicsItem * item;
		for (int i=0; i < handle->graphicsItems.size(); ++i)
		{
			item = handle->graphicsItems[i];
			if (item)
			{
				NodeGraphicsItem * node = NodeGraphicsItem::topLevelNodeItem(item);
				if (node)
				{
					if (node->shapes.size() > 0 && node->shapes[0])
					{
						if (rgb == 0)
							(*r) = node->shapes[0]->defaultBrush.color().red();
						else
							if (rgb == 1)
								(*r) = node->shapes[0]->defaultBrush.color().green();
							else
								(*r) = node->shapes[0]->defaultBrush.color().blue();
					}
				}
				else
				{
					ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(item);
					if (connection)
					{
						if (rgb == 0)
							(*r) = connection->defaultBrush.color().red();
						else
							if (rgb == 1)
								(*r) = connection->defaultBrush.color().green();
							else
								(*r) = connection->defaultBrush.color().blue();
					}
					else
					{
						TextGraphicsItem * text = TextGraphicsItem::cast(item);
						if (text)
						{
							if (rgb == 0)
								(*r) = text->defaultTextColor().red();
							else
								if (rgb == 1)
									(*r) = text->defaultTextColor().green();
								else
									(*r) = text->defaultTextColor().blue();
						}
						else
						{
							ControlPoint * cp = ControlPoint::cast(item);
							if (cp)
							{
								if (rgb == 0)
									(*r) = cp->defaultBrush.color().red();
								else
									if (rgb == 1)
										(*r) = cp->defaultBrush.color().green();
									else
										(*r) = cp->defaultBrush.color().blue();
							}
						}
					}
				}
			}
		}
	}

	void BasicGraphicsToolbox::getColorR(QSemaphore* s,int* r,ItemHandle* item)
	{
		getColorRGB(item,r,0);
		if (s)
			s->release();
	}

	void BasicGraphicsToolbox::getColorG(QSemaphore* s,int* g,ItemHandle* item)
	{
		getColorRGB(item,g,1);
		if (s)
			s->release();
	}

	void BasicGraphicsToolbox::getColorB(QSemaphore* s,int* b,ItemHandle* item)
	{
		getColorRGB(item,b,2);
		if (s)
			s->release();
	}

	void BasicGraphicsToolbox::setColor(QSemaphore* s,ItemHandle* handle,int r,int g,int b, int permanent)
	{
		GraphicsScene * scene = currentScene();
		QGraphicsItem* item;
		if (handle && scene)
			for (int i=0; i < handle->graphicsItems.size(); ++i)
			{
				item = handle->graphicsItems[i];
				if (item && scene)
				{
					QList<QGraphicsItem*> items;
					QList<QBrush> brushes;
					QList<QPen> pens;
					QColor color(r,g,b);
					NodeGraphicsItem * node = NodeGraphicsItem::topLevelNodeItem(item);
					if (node != 0)
					{
						for (int j=0; j < node->shapes.size(); ++j)
						{
							NodeGraphicsItem::Shape * aitem = node->shapes[j];

							if (aitem != 0)
							{
								QBrush brush = aitem->defaultBrush;
								if (brush.gradient() != 0)
								{
									QGradient gradient(*brush.gradient() );
									QGradientStops stops = gradient.stops();
									if (stops.size() > 0)
									{
										color.setAlpha(stops[ stops.size() - 1 ].second.alpha());
										stops[ stops.size() - 1 ].second = color;
										gradient.setStops(stops);
										QBrush newBrush(gradient);
										if (permanent)
										{
											brushes += newBrush;
											items += aitem;
										}
										else
										{
											aitem->setBrush(newBrush);
											temporarilyChangedItems << aitem;
										}
									}
								}
								else
								{
									color.setAlpha(brush.color().alpha());
									QBrush newBrush(color);
									if (permanent)
									{
										items += aitem;
										brushes += newBrush;
									}
									else
									{
										aitem->setBrush(newBrush);
										temporarilyChangedItems << aitem;
									}
								}
								QPen newPen(aitem->defaultPen);
								newPen.setColor(color);
								if (permanent)
								{
									pens += newPen;
								}
								else
								{
									aitem->setPen(newPen);
									temporarilyChangedItems << aitem;
								}
							}
						}
					}
					else
					{
						ConnectionGraphicsItem * connection = ConnectionGraphicsItem::topLevelConnectionItem(item);
						if (connection != 0)
						{
							QPen newPen(color,connection->defaultPen.widthF());
							color.setAlpha(connection->defaultBrush.color().alpha());
							//QBrush newBrush(color);
							if (permanent)
							{
								pens += newPen;
								brushes += connection->brush();
								items += connection;
							}
							else
							{
								connection->setPen(newPen);
								//connection->setBrush(newBrush);
								temporarilyChangedItems << connection;
							}
						}
					}
					if (permanent)
						scene->setBrushAndPen(tr("colors changed"),items,brushes,pens);
				}
			}
			if (s)
				s->release();
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
		if (!mainWindow || !findText || !replaceText || findText->text().isEmpty()) return;

		//if (currentWindow())
			//currentWindow()->rename(findText->text(),replaceText->text());
		if (currentScene())
			currentScene()->rename(findText->text(),replaceText->text());
		else
			if (currentTextEditor())
				currentTextEditor()->replace(findText->text(),replaceText->text());
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
			
			QList<NetworkWindow*> allWindows = mainWindow->allWindows();
			for (int i=0; i < allWindows.size(); ++i)
				allWindows[i]->setCursor(QCursor(QPixmap(tr(":/images/zoomin.png")).scaled(25,25)));

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
			QList<NetworkWindow*> allWindows = mainWindow->allWindows();
			for (int i=0; i < allWindows.size(); ++i)
				allWindows[i]->setCursor(QCursor(QPixmap(tr(":/images/zoomout.png")).scaled(25,25)));
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
			QList<NetworkWindow*> allWindows = mainWindow->allWindows();
			for (int i=0; i < allWindows.size(); ++i)
				allWindows[i]->setCursor(QCursor(QPixmap(tr(":/images/bucket.png")).scaled(25,25)));
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
			QList<NetworkWindow*> allWindows = mainWindow->allWindows();
			for (int i=0; i < allWindows.size(); ++i)
				allWindows[i]->setCursor(QCursor(QPixmap(tr(":/images/pencil.png")).scaled(25,25)));
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
			painter.setBrush(QBrush(QColor(0,0,0,brushColor1.alpha())));
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
			painter.setBrush(QBrush(QColor(0,0,0,brushColor2.alpha())));
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
			zoomRect.setRect( QRectF(scene->lastPoint(), point ));
		}
	}

	void BasicGraphicsToolbox::mouseDragged(GraphicsScene * scene, QPointF from, QPointF to, Qt::MouseButton button, Qt::KeyboardModifiers )
	{
		if (scene == 0) return;

		if (mode == zoom)
		{
			if (button == Qt::LeftButton)
			{
				QList<QGraphicsView*> views = scene->views();

				QRectF rect(from,to);
				if (scene->items(rect).size() < 2)
				{
					scene->centerOn(to);
					scene->scaleView(1.5);
				}
				else
				{
					for (int i=0; i < views.size(); ++i)
						if (views[i])
						{
							views[i]->fitInView(rect,Qt::KeepAspectRatio);
						}
				}
			}

			zoomRect.setVisible(false);
			zoomRect.setRect(QRectF(0,0,0,0));


			if (zoomRect.scene() == scene)
				scene->removeItem(&zoomRect);

			scene->useDefaultBehavior = true;
			mode = this->none;
			QList<NetworkWindow*> allWindows = mainWindow->allWindows();
			for (int i=0; i < allWindows.size(); ++i)
				allWindows[i]->setCursor(Qt::ArrowCursor);
			return;
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
					QList<NetworkWindow*> allWindows = mainWindow->allWindows();
					for (int i=0; i < allWindows.size(); ++i)
						allWindows[i]->setCursor(Qt::ArrowCursor);
					return;
				}
			}
			else
			{
				if (qgraphicsitem_cast<NodeGraphicsItem::Shape*>(item) || ControlPoint::cast(item))
				{
					scene->setBrush(tr("brush changed"),item,QBrush(brushColor1));
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
					QList<NetworkWindow*> allWindows = mainWindow->allWindows();
					for (int i=0; i < allWindows.size(); ++i)
						allWindows[i]->setCursor(Qt::ArrowCursor);
					return;
				}
			}
			else
			{
				if (qgraphicsitem_cast<NodeGraphicsItem::Shape*>(item) || ControlPoint::cast(item))
				{
					QPointF colorPt1 = item->mapFromScene(from),
						colorPt2 = item->mapFromScene(to);
					if (gradientType == QGradient::LinearGradient)
					{
						QLinearGradient gradient(colorPt1,colorPt2);
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
						QRadialGradient gradient(colorPt1,sqrt( (colorPt2.y()-colorPt1.y())*(colorPt2.y()-colorPt1.y()) +
							(colorPt2.x()-colorPt1.x())*(colorPt2.x()-colorPt1.x())));
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
					if (mode == brush || mode == pen || mode == gradient)
					{
						if (mode != pen)
						{
							point.rx() += 1;
							point.ry() += 0.5;
						}
						else
						{
							point.rx() -= 1.5;
							point.ry() += 1.5;
						}

						QGraphicsItem * item = scene->itemAt(point);
						if (!item || (!ConnectionGraphicsItem::cast(item) && (item->sceneBoundingRect().width() > 500 || item->sceneBoundingRect().height() > 500)))
						{
							QList<QGraphicsItem*> ps = scene->items(QRectF(point.rx()-50.0,point.ry()-50.0,100.0,100.0));
							if (!ps.isEmpty())
							{
								int i=0;
								item = ps[i];
								while (i < ps.size() && TextGraphicsItem::cast(item))
								{
									item = ps[i];
									++i;
								}
							}
						}

						if (item == 0)
						{
							scene->useDefaultBehavior = true;
							mode = this->none;
							QList<NetworkWindow*> allWindows = mainWindow->allWindows();
							for (int i=0; i < allWindows.size(); ++i)
								allWindows[i]->setCursor(Qt::ArrowCursor);
						}
						else
						{
							if (qgraphicsitem_cast<NodeGraphicsItem::Shape*>(item) || ControlPoint::cast(item))
							{
								if (mode == this->brush)
									scene->setBrush(tr("brush changed"),item,QBrush(brushColor1));
								else
									if (mode == this->gradient)
									{
										QPointF colorPt1 = item->sceneBoundingRect().topLeft(),
											colorPt2 = item->sceneBoundingRect().bottomRight();
										QLinearGradient gradient(colorPt1,colorPt2);
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
										scene->setPen(tr("pen changed"),item,QPen(penColor,penWidth));
							}
							else
							{
								ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(item);
								if (connection != 0)
								{
									connection->setControlPointsVisible(true);
									connection->setPen(QPen(QPen(penColor,penWidth,Qt::DashLine)));
									if (mode == this->brush || mode == this->gradient)
										scene->setBrush(tr("brush changed"), connection, QBrush(brushColor1));
									else
										scene->setPen(tr("pen changed"), connection, QPen(penColor,penWidth));
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
			QList<NetworkWindow*> allWindows = mainWindow->allWindows();
			for (int i=0; i < allWindows.size(); ++i)
				allWindows[i]->setCursor(Qt::ArrowCursor);
			if (mainWindow && mainWindow->currentScene())
				mainWindow->currentScene()->useDefaultBehavior = true;
			mode = none;
		}
		if (temporarilyChangedItems.size() > 0)
		{
			NodeGraphicsItem::Shape * shape = 0;
			ConnectionGraphicsItem * connection = 0;
			for (int i=0; i < temporarilyChangedItems.size(); ++i)
				if (shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(temporarilyChangedItems[i]))
				{
					shape->setPen(shape->defaultPen);
					shape->setBrush(shape->defaultBrush);
				}
				else
					if (connection = ConnectionGraphicsItem::cast(temporarilyChangedItems[i]))
					{
						connection->setPen(connection->defaultPen);
						connection->setBrush(connection->defaultBrush);
					}
					temporarilyChangedItems.clear();
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

	void BasicGraphicsToolbox::changeGraphics(QSemaphore* s,ItemHandle* h,const QString& file)
	{
		if (h)
		{
			QStringList filenames;
			QList<NodeGraphicsItem*> nodesList;
			NodeGraphicsItem * node;

			for (int i=0; i < h->graphicsItems.size(); ++i)
				if ((node = NodeGraphicsItem::cast(h->graphicsItems[i])))
				{
					nodesList << node;
					filenames << file;
				}

			if (nodesList.size() > 0)
			{
				ReplaceNodeGraphicsCommand * command = new ReplaceNodeGraphicsCommand(tr("image changed for ") + h->fullName(),nodesList,filenames);
				if (currentWindow())
					currentWindow()->history.push(command);
			}
		}

		if (s)
			s->release();
	}

	void BasicGraphicsToolbox::changeArrowHead(QSemaphore* s,ItemHandle* h,const QString& file)
	{
		if (h)
		{
			QStringList filenames;
			QList<NodeGraphicsItem*> nodesList;
			ConnectionGraphicsItem * conn;

			for (int i=0; i < h->graphicsItems.size(); ++i)		
				if ((conn = ConnectionGraphicsItem::cast(h->graphicsItems[i])))
				{
					QList<ArrowHeadItem*> arrows = conn->arrowHeads();

					for (int j=0; j < arrows.size(); ++j)
					{
						nodesList << arrows[j];
						filenames << file;
					}
				}

			if (nodesList.size() > 0)
			{
				ReplaceNodeGraphicsCommand * command = new ReplaceNodeGraphicsCommand(tr("arrowheads changed for ") + h->fullName(),nodesList,filenames);
				if (currentWindow())
					currentWindow()->history.push(command);
			}
		}

		if (s)
			s->release();
	}


	/****************************
	Function to Signal
	*****************************/
	BasicGraphicsToolbox_FToS BasicGraphicsToolbox::fToS;

	int BasicGraphicsToolbox::_getColorR(OBJ o)
	{
		return fToS.getColorR(o);
	}

	int BasicGraphicsToolbox_FToS::getColorR(OBJ o)
	{
		int i;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit getColorR(s,&i,ConvertValue(o));
		s->acquire();
		s->release();
		return i;
	}

	int BasicGraphicsToolbox::_getColorG(OBJ o)
	{
		return fToS.getColorG(o);
	}

	int BasicGraphicsToolbox_FToS::getColorG(OBJ o)
	{
		int i;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit getColorG(s,&i,ConvertValue(o));
		s->acquire();
		s->release();
		return i;
	}

	int BasicGraphicsToolbox::_getColorB(OBJ o)
	{
		return fToS.getColorB(o);
	}

	int BasicGraphicsToolbox_FToS::getColorB(OBJ o)
	{
		int i;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit getColorB(s,&i,ConvertValue(o));
		s->acquire();
		s->release();
		return i;
	}

	void BasicGraphicsToolbox::_setColor(OBJ o,int r, int g, int b, int p)
	{
		return fToS.setColor(o,r,g,b,p);
	}

	void BasicGraphicsToolbox_FToS::setColor(OBJ o,int r, int g, int b, int p)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setColor(s,ConvertValue(o),r,g,b,p);
		s->acquire();
		s->release();
		return;
	}

	void BasicGraphicsToolbox::_changeGraphics(OBJ o,const char* f)
	{
		fToS.changeGraphics(o,f);
	}
	
	void BasicGraphicsToolbox_FToS::changeGraphics(OBJ o,const char* f)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit changeGraphics(s,ConvertValue(o),ConvertValue(f));
		s->acquire();
		s->release();
		return;
	}

	void BasicGraphicsToolbox::_changeArrowHead(OBJ o,const char* f)
	{
		fToS.changeArrowHead(o,f);
	}

	void BasicGraphicsToolbox_FToS::changeArrowHead(OBJ o,const char* f)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit changeArrowHead(s,ConvertValue(o),ConvertValue(f));
		s->acquire();
		s->release();
		return;
	}

}
