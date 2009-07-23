/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool contains a set of transformation function such as scale, rotate, etc.
 A GraphicsTool is also defined that brings up the transformations window

****************************************************************************/

#include <QComboBox>
#include <QDoubleSpinBox>
#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "GraphicsTransformTool.h"
#include "ConnectionSelection.h"
#include "GraphicsReplaceTool.h"


namespace Tinkercell
{

        void GraphicsTransformTool::select(int)
        {
            updateTable();
            openedByUser = true;

            if (parentWidget() != 0)
            {
                if (parentWidget()->isVisible())
                    openedByUser = false;
                else
                    parentWidget()->show();
            }
            else
            {
                if (isVisible())
                    openedByUser = false;
                else
                    show();
            }
        }

        void GraphicsTransformTool::deselect(int)
	{
                if (openedByUser)
		{
			openedByUser = false;
			
                        if (parentWidget() != 0)
                                parentWidget()->hide();
			else
                                hide();
		}
	}


	void GraphicsTransformTool::itemsInserted(GraphicsScene* , const QList<QGraphicsItem *>& , const QList<ItemHandle*>& handles)
	{
		for (int i=0; i < handles.size(); ++i)
		{
                        if (handles[i] && !handles[i]->tools.contains(this))
                                handles[i]->tools += this;
		}
	}
	
	void GraphicsTransformTool::toolLoaded(Tool*)
	{
		connectionSelectionTool();
	}
	
	void GraphicsTransformTool::connectionSelectionTool()
	{
		static bool alreadyConnected = false;
		static bool alreadyConnected2 = false; 
		if ((alreadyConnected && alreadyConnected2) || !mainWindow) return;

                if (!alreadyConnected && mainWindow->tool(tr("Connection Selection")))
		{
			QWidget * widget = mainWindow->tool(tr("Connection Selection"));
			ConnectionSelection * connectionSelection = static_cast<ConnectionSelection*>(widget);
			if (connectionSelection && connectionsTableWidget)
			{
				alreadyConnected = true;
				
				QComboBox * lineTypes = new QComboBox(this);
				lineTypes->addItems(QStringList() << tr("Bezier curves") << tr("straight lines"));
				connect(lineTypes,SIGNAL(currentIndexChanged(int)),connectionSelection,SLOT(setLineType(int)));
				
				connectionsTableWidget->setCellWidget(0,0,lineTypes);
				
				QComboBox * penTypes = new QComboBox(this);
				penTypes->addItems(QStringList() << tr("solid") << tr("dashed") << tr("dotted"));
				connect(penTypes,SIGNAL(currentIndexChanged(int)),this,SLOT(changePenType(int)));
				
				connectionsTableWidget->setCellWidget(1,0,penTypes);
				
				QComboBox * centerBox = new QComboBox(this);
				centerBox->addItems(QStringList() << tr("hide") << tr("show") );
				connect(centerBox,SIGNAL(currentIndexChanged(int)),connectionSelection,SLOT(showMiddleBox(int)));
				
				connectionsTableWidget->setCellWidget(3,0,centerBox);
				
				QDoubleSpinBox * arrowHeadDist = new QDoubleSpinBox(this);
				arrowHeadDist->setSingleStep(1.0);
				arrowHeadDist->setValue(20.0);
				arrowHeadDist->setRange(0.0,200.0);
				connect(arrowHeadDist,SIGNAL(valueChanged(double)),connectionSelection,SLOT(arrowHeadDistance(double)));
				
				connectionsTableWidget->setCellWidget(2,0,arrowHeadDist);
				
			}
		}
		
		if (!alreadyConnected2 && replaceButton && mainWindow->tool(tr("Graphics Replace Tool")))
		{
			alreadyConnected2 = true;
			GraphicsReplaceTool * copyPaste = static_cast<GraphicsReplaceTool*>(mainWindow->tool(tr("Graphics Replace Tool")));
			connect(replaceButton,SIGNAL(pressed()),copyPaste,SLOT(substituteNodeGraphics()));
		}
	}

	GraphicsTransformTool::GraphicsTransformTool() : Tool(tr("Basic Transformations"))
	{
                QString appDir = QCoreApplication::applicationDirPath();
                NodeGraphicsReader reader;
                reader.readXml(&eye,tr(":/BasicTools/eye.xml"));
                eye.normalize();
                eye.scale(40.0/eye.sceneBoundingRect().width(),30.0/eye.sceneBoundingRect().height());
                eye.setToolTip(tr("Appearance"));
                openedByUser = false;
                graphicsItems += new GraphicsItem(this);
                graphicsItems[0]->setToolTip(tr("Appearance"));
                graphicsItems[0]->addToGroup(&eye);

		setPalette(QPalette(QColor(255,255,255,255)));
		setAutoFillBackground(true);

		scaleSlider.setRange(1,100);
		rotateSlider.setRange(-180,180);
		scaleSlider.setValue(50);
		rotateSlider.setValue(0);
		scaleSlider.setOrientation(Qt::Horizontal);
		rotateSlider.setOrientation(Qt::Horizontal);

		QVBoxLayout * layout = new QVBoxLayout;

		QGroupBox * scaleGroup = new QGroupBox(tr(" Scale "));
		QGroupBox * rotateGroup = new QGroupBox(tr(" Rotate "));
		QGroupBox * flipGroup = new QGroupBox(tr(" Flip "));
		QGroupBox * buttonsGroup = new QGroupBox(tr(" Color and graphics "));

		QVBoxLayout * scaleLayout = new QVBoxLayout;
		scaleLayout->addWidget(&scaleSlider);
		scaleGroup->setLayout(scaleLayout);

		QVBoxLayout * rotateLayout = new QVBoxLayout;
		rotateLayout->addWidget(&rotateSlider);
		rotateGroup->setLayout(rotateLayout);


		QHBoxLayout * flipLayout = new QHBoxLayout;
		QPushButton * vFlipButton = new QPushButton(this);
		vFlipButton->setIcon(QIcon(tr(":/images/verticalFlip.png")));
		QPushButton * hFlipButton = new QPushButton(this);
		hFlipButton->setIcon(QIcon(tr(":/images/horizontalFlip.png")));
		flipLayout->addWidget(hFlipButton);
		flipLayout->addWidget(vFlipButton);
		connect(vFlipButton,SIGNAL(pressed()),this,SLOT(flipVertically()));
		connect(hFlipButton,SIGNAL(pressed()),this,SLOT(flipHorizontally()));
		flipGroup->setLayout(flipLayout);

		layout->addWidget(scaleGroup);
		layout->addWidget(rotateGroup);
		layout->addWidget(flipGroup);

		QHBoxLayout * buttonsLayout = new QHBoxLayout;
		QPushButton * fillButton = new QPushButton(this),
					* penButton = new QPushButton(this);
		replaceButton = new QPushButton(this);

		fillButton->setIcon(QIcon(QPixmap(QString(":/images/bucket.png"))));
                fillButton->setToolTip(tr("Fill color"));
		fillButton->setFixedSize(40,40);
		fillButton->setIconSize(QSize(30, 30));
		fillButton->setCheckable(false);
		//fillButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		//buttonGroup->addButton(fillButton, 0);
		
		penButton->setIcon(QIcon(QPixmap(QString(":/images/pencil.png"))));
		penButton->setToolTip(tr("Outline color"));
		//penButton->setText(tr("Outline color"));
		penButton->setFixedSize(40,40);
		penButton->setIconSize(QSize(30, 30));
		penButton->setCheckable(false);
		//penButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		//buttonGroup->addButton(penButton, 1);
		
		replaceButton->setIcon(QIcon(QPixmap(QString(":/images/replace.png"))));
		replaceButton->setToolTip(tr("Replace graphics"));
		//replaceButton->setText(tr("Replace graphics"));
		replaceButton->setFixedSize(40,40);
		replaceButton->setIconSize(QSize(30, 30));
		replaceButton->setCheckable(false);
		//replaceButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		//buttonGroup->addButton(replaceButton, 2);
		

		buttonsLayout->addWidget(fillButton);
		buttonsLayout->addWidget(penButton);
		buttonsLayout->addWidget(replaceButton);
		buttonsGroup->setLayout(buttonsLayout);

		layout->addWidget(buttonsGroup);

		connect(&scaleSlider,SIGNAL(sliderPressed()),this,SLOT(scaleStart()));
		connect(&scaleSlider,SIGNAL(sliderReleased()),this,SLOT(scaleEnd()));
		connect(&scaleSlider,SIGNAL(valueChanged(int)),this,SLOT(scale(int)));
		connect(&rotateSlider,SIGNAL(sliderPressed()),this,SLOT(rotateStart()));		
		connect(&rotateSlider,SIGNAL(sliderReleased()),this,SLOT(rotateEnd()));
		connect(&rotateSlider,SIGNAL(valueChanged(int)),this,SLOT(rotate(int)));
		connect(penButton,SIGNAL(pressed()),this,SLOT(selectPenColor()));
		connect(fillButton,SIGNAL(pressed()),this,SLOT(selectFillColor()));
		
		
		setupTable();
		
		sizeGroup = new QGroupBox("Position and Size");
		connectionsGroup = new QGroupBox("Connection Settings");
		
		QHBoxLayout * layout1 = new QHBoxLayout;
		if (tableWidget != 0) 
			layout1->addWidget(tableWidget);
		
		sizeGroup->setLayout(layout1);
		
		QHBoxLayout * layout2 = new QHBoxLayout;
		if (connectionsTableWidget != 0) 
			layout2->addWidget(connectionsTableWidget);
		
		connectionsGroup->setLayout(layout2);
		
		layout->addWidget(sizeGroup);
		layout->addWidget(connectionsGroup);

		setLayout(layout);
		moving = false;
		totalRotated = 0;
		totalScaled = 1;
	}

	void GraphicsTransformTool::onHide(bool visible)
	{
		if (!visible)
		{
			targetItems.clear();
			dockWidget->setVisible(false);
			moving = false;
			totalRotated = 0;
			totalScaled = 1;
		}
	}

	bool GraphicsTransformTool::setMainWindow(MainWindow * main)
	{
                Tool::setMainWindow(main);

		if (mainWindow != 0)
		{
			dockWidget = mainWindow->addDockingWindow(name,this,Qt::RightDockWidgetArea,Qt::NoDockWidgetArea);
			if (dockWidget != 0)
			{	
				/*dockWidget->setWindowFlags(Qt::Tool);
				dockWidget->setAttribute(Qt::WA_ContentsPropagated);
				dockWidget->setPalette(QPalette(QColor(255,255,255,255)));
				dockWidget->setAutoFillBackground(true);
				dockWidget->setWindowOpacity(0.75);*/
				
				dockWidget->hide();
				dockWidget->setFloating(true);
				
				QAction * menuAction = new QAction(QIcon(tr(":/images/eye.png")),tr("Scale, color, rotate, etc."),this);
				connect(menuAction,SIGNAL(triggered()),dockWidget,SLOT(show()));
				
				mainWindow->contextItemsMenu.addAction(menuAction);
			}

			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene * , const QList<QGraphicsItem*>& , QPointF , Qt::KeyboardModifiers )),
					this, SLOT(itemsSelected(GraphicsScene * , const QList<QGraphicsItem*>& , QPointF , Qt::KeyboardModifiers )));
			connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&,const QList<ItemHandle*>&)),
					this, SLOT(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)));
			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));
			
			connectionSelectionTool();
		}
		return true;
	}
	
	void GraphicsTransformTool::scale(int value)
	{
		if (scaleRef == 0)
			scaleRef = 1;
		qreal dx = (qreal)value/(qreal)scaleRef;
		//qreal dx = 1;
		//if (diff > 0) dx = diff * 1.1; else dx = -diff * 1.0/1.1;
		totalScaled *= dx;
		if (targetItems.size() > 0)
		{
			NodeGraphicsItem * node = 0;
			ConnectionGraphicsItem * connectionPtr = 0;
			for (int i=0; i < targetItems.size(); ++i)
			{
				if (targetItems[i] &&
					(qgraphicsitem_cast<NodeGraphicsItem*>(targetItems[i]) 
					|| qgraphicsitem_cast<TextGraphicsItem*>(targetItems[i])))
				{
					node = qgraphicsitem_cast<NodeGraphicsItem*>(targetItems[i]);
					if (node) node->setBoundingBoxVisible(false);
					
					targetItems[i]->scale(dx,dx);
				}
				else
				if ((connectionPtr = qgraphicsitem_cast<ConnectionGraphicsItem*>(targetItems[i]) ))
				{
					QPen p = connectionPtr->pen();
					p.setWidthF( p.widthF() * dx );
					connectionPtr->setPen(p);
					connectionPtr->refresh();
				}
			}
		}
		scaleRef = value;
		if (!moving) 
			scaleEnd();
	}

	void GraphicsTransformTool::rotate(int value)
	{
		qreal dx = value - rotateRef;
		totalRotated += dx;
		if (targetItems.size() > 0)
		{
			NodeGraphicsItem * node;
			for (int i=0; i < targetItems.size(); ++i)
			{
				if ( targetItems[i] && 
					(qgraphicsitem_cast<NodeGraphicsItem*>(targetItems[i]) 
					|| qgraphicsitem_cast<TextGraphicsItem*>(targetItems[i])))
				{
					node = qgraphicsitem_cast<NodeGraphicsItem*>(targetItems[i]);
					if (node) node->setBoundingBoxVisible(false);
					targetItems[i]->rotate(dx);
				}
			}
		}
		rotateRef = value;
		if (!moving) 
			rotateEnd();
	}

	void GraphicsTransformTool::rotateStart()
	{
		totalRotated = 0;
		moving = true;
	}

	void GraphicsTransformTool::rotateEnd()
	{
		QList<QGraphicsItem*> list;
		QList<QPointF> scaleList;
		QList<qreal> rotateList;
		if (targetItems.size() > 0 && totalRotated != 0)
		{
			for (int i=0; i < targetItems.size(); ++i)
			{
				if ( targetItems[i] && 
					(qgraphicsitem_cast<NodeGraphicsItem*>(targetItems[i]) 
					|| qgraphicsitem_cast<TextGraphicsItem*>(targetItems[i])) )
				{
					targetItems[i]->rotate(-totalRotated);
					list += targetItems[i];
					scaleList += QPointF(1,1);
					rotateList += totalRotated;
				}
			}
			if (mainWindow != 0 && mainWindow->currentScene() != 0)
			{
				mainWindow->currentScene()->transform(tr("items rotated by ") + QString::number(totalRotated),list,scaleList,rotateList,false,false);
			}
		}
		moving = false;
		totalRotated = 0;

		if (rotateSlider.value() >= 180 || rotateSlider.value() <= -180)
		{
			rotateRef = 0;
			rotateSlider.setValue(0);
		}
	}

	void GraphicsTransformTool::scaleStart()
	{
		totalScaled = 1;
		moving = true;
	}

	void GraphicsTransformTool::scaleEnd()
	{
		QList<QGraphicsItem*> list;
		QList<QPointF> scaleList;
		QList<qreal> rotateList;

		QList<QGraphicsItem*> penList;
		QList<QPen> pens;

		if (targetItems.size() > 0 && totalScaled != 1.0)
		{
			for (int i=0; i < targetItems.size(); ++i)
			{
				ConnectionGraphicsItem * connectionPtr = 0;

				if ( targetItems[i] && 
					(qgraphicsitem_cast<NodeGraphicsItem*>(targetItems[i]) 
					|| qgraphicsitem_cast<TextGraphicsItem*>(targetItems[i])))
				{
					targetItems[i]->scale(1.0/totalScaled,1.0/totalScaled);
					list += targetItems[i];
					scaleList += QPointF(totalScaled,totalScaled);
					rotateList += 0;
				}
				else
				if ((connectionPtr = qgraphicsitem_cast<ConnectionGraphicsItem*>(targetItems[i]) ))
				{
					QPen p = connectionPtr->defaultPen;
					p.setWidthF( p.widthF() * totalScaled );
					penList += targetItems[i];
					pens += p;
				}
			}
			if (mainWindow != 0 && mainWindow->currentScene() != 0)
			{
				mainWindow->currentScene()->transform(tr("items scaled by ") + QString::number(totalScaled),list,scaleList,rotateList,false,false);
				if (pens.size() > 0)
					mainWindow->currentScene()->setPen(tr("line width changed"),penList,pens);
			}
		}
		moving = false;
		totalScaled = 1;

		if (scaleSlider.value() >= 100 || scaleSlider.value() <= 1)
		{
			scaleRef = 50;
			scaleSlider.setValue(50);
		}
		
	}
	
	void GraphicsTransformTool::changePenType(int type)
	{
		type += 1;
		if (targetItems.size() > 0)
		{
			QList<QGraphicsItem*> penList;
			QList<QPen> pens;
			
			for (int i=0; i < targetItems.size(); ++i)
			{
				ConnectionGraphicsItem * connectionPtr = 0;

				if ((connectionPtr = qgraphicsitem_cast<ConnectionGraphicsItem*>(targetItems[i]) ))
				{
					QPen p = connectionPtr->defaultPen;
					p.setStyle((Qt::PenStyle)type);
					penList += targetItems[i];
					pens += p;
				}
			}
			if (mainWindow != 0 && mainWindow->currentScene() != 0)
			{
				if (pens.size() > 0)
					mainWindow->currentScene()->setPen(tr("pen style changed"),penList,pens);
			}
		}
	}

	void GraphicsTransformTool::reset()
	{
		scaleRef = 50;
		rotateRef = 0;
		scaleSlider.setValue(50);
		rotateSlider.setValue(0);
	}

	void GraphicsTransformTool::flipHorizontally()
	{
		if (targetItems.size() > 0 && mainWindow != 0 && mainWindow->currentScene() != 0)
		{
			QList<QPointF> plist;
			QList<qreal> dlist;
			QList<QGraphicsItem*> items;
			for (int i=0; i < targetItems.size(); ++i)
			{
				NodeGraphicsItem * item = NodeGraphicsItem::topLevelNodeItem(targetItems[i]);
				if (item != 0) items += item;
			}
			if (!items.isEmpty())
				mainWindow->currentScene()->transform(tr("hortizontal flip"),items,plist,dlist,false,true);
		}
	}

	void GraphicsTransformTool::flipVertically()
	{
		if (targetItems.size() > 0 && mainWindow != 0 && mainWindow->currentScene() != 0)
		{
			QList<QPointF> plist;
			QList<qreal> dlist;
			QList<QGraphicsItem*> items;
			for (int i=0; i < targetItems.size(); ++i)
			{
				NodeGraphicsItem * item = NodeGraphicsItem::topLevelNodeItem(targetItems[i]);
				if (item != 0) items += item;
			}
			if (!items.isEmpty())
				mainWindow->currentScene()->transform(tr("vertical flip"),items,plist,dlist,true,false);
		}
	}

	void GraphicsTransformTool::itemsSelected(GraphicsScene * , const QList<QGraphicsItem*>& items, QPointF , Qt::KeyboardModifiers )
	{
		reset();
		targetItems.clear();
		for (int i=0; i < items.size(); ++i)
		{
			NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(items[i]);
			if (node && !targetItems.contains(node))
			{
				targetItems += node;
			}
			else
			{
				ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i]);
				if (connection && !targetItems.contains(connection))
					targetItems += connection;
				else
				{
					TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(items[i]);
					if (textItem)
						targetItems += textItem;
				}
			}
		}
	}

	void GraphicsTransformTool::selectPenColor()
	{
		QColor color = QColorDialog::getColor();
		if (color.isValid() && targetItems.size() > 0)
		{
			QList<QGraphicsItem*> items;
			QList<QPen> pens;
			for (int i=0; i < targetItems.size(); ++i)
			{
				NodeGraphicsItem * node = qgraphicsitem_cast<NodeGraphicsItem*>(targetItems[i]);
				if (node != 0)
				{
					for (int j=0; j < node->shapes.size(); ++j)
					{
						NodeGraphicsItem::Shape * aitem = node->shapes[j];
				
						if (aitem != 0)
						{
							QPen pen = aitem->defaultPen;
							color.setAlpha(pen.color().alpha());
							pen.setColor(color);
							pens += pen;
							items += aitem;
						}
					}
				}
				else
				{
					ConnectionGraphicsItem * connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(targetItems[i]);
					if (connection != 0)
					{
						QPen pen = connection->defaultPen;
						color.setAlpha(pen.color().alpha());
						pen.setColor(color);
						pens += pen;
						items += connection;
					}
					else
					{
						TextGraphicsItem * textItem = qgraphicsitem_cast<TextGraphicsItem*>(targetItems[i]);
						if (textItem != 0)
						{
							QPen pen(textItem->defaultTextColor());
							color.setAlpha(pen.color().alpha());
							pen.setColor(color);
							pens += pen;
							items += textItem;
						}
					}
				}
			}
			if (mainWindow->currentScene() != 0)
				mainWindow->currentScene()->setPen(tr("pen changed"),items,pens);
		}
	}

	void GraphicsTransformTool::selectFillColor()
	{
		QColor color = QColorDialog::getColor();
		if (color.isValid() && targetItems.size() > 0)
		{
			QList<QGraphicsItem*> items;
			QList<QBrush> brushes;
			for (int i=0; i < targetItems.size(); ++i)
			{
				NodeGraphicsItem * node = NodeGraphicsItem::topLevelNodeItem(targetItems[i]);
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
									brushes += newBrush;
									items += aitem;
								}
							}
							else
							{
								color.setAlpha(brush.color().alpha());
								QBrush newBrush(color);
								items += aitem;
								brushes += newBrush;
							}
						}
					}
				}
				else
				{
					ConnectionGraphicsItem * connection = ConnectionGraphicsItem::topLevelConnectionItem(targetItems[i]);
					if (connection != 0)
					{
						color.setAlpha(connection->defaultBrush.color().alpha());
						QBrush newBrush(color);
						brushes += newBrush;
						items += connection;
					}
				}
			}
			if (mainWindow->currentScene() != 0)
				mainWindow->currentScene()->setBrush(tr("brush changed"),items,brushes);
		}
	}

	void GraphicsTransformTool::setupTable()
	{
		tableWidget = new QTableWidget(5,1);
		QStringList colNames, rowNames;
		colNames << tr("values");
		rowNames << tr("x") << tr("y") << tr("width") << tr("height") << tr("line width");// << tr("transparency");

		tableWidget->setHorizontalHeaderLabels(colNames);
		tableWidget->setVerticalHeaderLabels(rowNames);

		QDoubleSpinBox * spin;
		spin = new QDoubleSpinBox(this);
		spin->setRange(0.0,100000.0);
		tableWidget->setCellWidget(0,0,spin);
		connect(spin, SIGNAL(valueChanged(double)), this, SLOT(xchanged(double)));
		
		spin = new QDoubleSpinBox(this);
		spin->setRange(0.0,100000.0);
		tableWidget->setCellWidget(1,0,spin);
		connect(spin, SIGNAL(valueChanged(double)), this, SLOT(ychanged(double)));
		
		spin = new QDoubleSpinBox(this);
		spin->setRange(0.0,100000.0);
		tableWidget->setCellWidget(2,0,spin);
		connect(spin, SIGNAL(valueChanged(double)), this, SLOT(wchanged(double)));
		
		spin = new QDoubleSpinBox(this);
		spin->setRange(0.0,100000.0);
		tableWidget->setCellWidget(3,0,spin);
		connect(spin, SIGNAL(valueChanged(double)), this, SLOT(hchanged(double)));
		
		spin = new QDoubleSpinBox(this);
		spin->setRange(0.0,100000.0);
		tableWidget->setCellWidget(4,0,spin);
		connect(spin, SIGNAL(valueChanged(double)), this, SLOT(linewchanged(double)));
		
		connectionsTableWidget = new QTableWidget(4,1);
		rowNames.clear();
		rowNames << tr("line type") << tr("pen type") << tr("arrow head distance") << tr("show center box");
		
		connectionsTableWidget->setHorizontalHeaderLabels(colNames);
		connectionsTableWidget->setVerticalHeaderLabels(rowNames);
	}
	
	void GraphicsTransformTool::xchanged(double value)
	{
		if (value == 0 || targetItems.size() < 1 || mainWindow == 0 || mainWindow->currentScene() == 0) return;
		GraphicsScene * scene = mainWindow->currentScene();
		QList<QPointF> dx;
		QList<QGraphicsItem*> list;
		for (int k=0; k < targetItems.size(); ++k)
			if (qgraphicsitem_cast<NodeGraphicsItem*>(targetItems[k]))
			{
				dx += QPointF(value - targetItems[k]->scenePos().x(), 0);
				list += targetItems[k];
			}
		scene->move(list,dx);
	}
	
	void GraphicsTransformTool::ychanged(double value)
	{
		if (value == 0 || targetItems.size() < 1 || mainWindow == 0 || mainWindow->currentScene() == 0) return;
		GraphicsScene * scene = mainWindow->currentScene();
		QList<QPointF> dy;
		QList<QGraphicsItem*> list;
		for (int k=0; k < targetItems.size(); ++k)
			if (qgraphicsitem_cast<NodeGraphicsItem*>(targetItems[k]))
			{
				dy += QPointF(0, value - targetItems[k]->scenePos().y());
				list += targetItems[k];
			}
		scene->move(list,dy);
	}
	
	void GraphicsTransformTool::wchanged(double value)
	{
		if (value == 0 || targetItems.size() < 1 || mainWindow == 0 || mainWindow->currentScene() == 0) return;
		GraphicsScene * scene = mainWindow->currentScene();
		QList<QPointF> dw;
		QList<QGraphicsItem*> list;
		for (int k=0; k < targetItems.size(); ++k)
			if (qgraphicsitem_cast<NodeGraphicsItem*>(targetItems[k]))
			{
				dw += QPointF(value/targetItems[k]->sceneBoundingRect().width(),1);
				list += targetItems[k];
			}
		QList<qreal> emptyList;
		scene->transform(tr("changed width"),list,dw,emptyList,false,false);
	}
	
	void GraphicsTransformTool::hchanged(double value)
	{
		if (value == 0 || targetItems.size() < 1 || mainWindow == 0 || mainWindow->currentScene() == 0) return;
		GraphicsScene * scene = mainWindow->currentScene();
		QList<QPointF> dh;
		QList<QGraphicsItem*> list;
		for (int k=0; k < targetItems.size(); ++k)
			if (qgraphicsitem_cast<NodeGraphicsItem*>(targetItems[k]))
			{
				qreal s = value/targetItems[k]->sceneBoundingRect().height();
				dh += QPointF(1,s);
				list += targetItems[k];
			}
		QList<qreal> emptyList;
		scene->transform(tr("changed height"),list,dh,emptyList,false,false);
	}
	
	void GraphicsTransformTool::linewchanged(double value)
	{
		if (targetItems.size() < 1 || mainWindow == 0 || mainWindow->currentScene() == 0) return;
		GraphicsScene * scene = mainWindow->currentScene();
		QPen pen;
		QList<QPen> pens;
		QList<QGraphicsItem*> list;
		NodeGraphicsItem * node;
		ConnectionGraphicsItem * connection;
		for (int k=0; k < targetItems.size(); ++k)
		{
			node = NodeGraphicsItem::topLevelNodeItem(targetItems[k]);
			connection = ConnectionGraphicsItem::topLevelConnectionItem(targetItems[k]);
			if (node != 0)
			{
				for (int i=0; i < node->shapes.size(); ++i)
				{
					list += node->shapes[i];
					pen = node->shapes[i]->defaultPen;
					pen.setWidthF(value);
					pens += pen;
				}
			}
			else
			if (connection != 0)
			{
				list += connection;
				pen = connection->defaultPen;
				pen.setWidthF(value);
				pens += pen;
			}
		}
		scene->setPen(tr("change pen"),list,pens);
	}
	
	
	void GraphicsTransformTool::alphaChanged(double value)
	{
		if (targetItems.size() < 1 || mainWindow == 0 || mainWindow->currentScene() == 0) return;
		GraphicsScene * scene = mainWindow->currentScene();
		QColor color;
		QBrush brush;
		QList<QBrush> brushes;
		QList<QGraphicsItem*> list;
		NodeGraphicsItem * node;
		ConnectionGraphicsItem * connection;
		for (int k=0; k < targetItems.size(); ++k)
		{
			node = NodeGraphicsItem::topLevelNodeItem(targetItems[k]);
			connection = ConnectionGraphicsItem::topLevelConnectionItem(targetItems[k]);
			if (node != 0)
			{
				for (int i=0; i < node->shapes.size(); ++i)
				{
					list += node->shapes[i];
					brush = node->shapes[i]->defaultBrush;
					color = brush.color();
					color.setAlphaF(value);
					brush.setColor(color);
					brushes += brush;
				}
			}
			else
			if (connection != 0)
			{
				list += connection;
				brush = connection->defaultBrush;
				color = brush.color();
				color.setAlphaF(value);
				brush.setColor(color);
				brushes += brush;
			}
		}
		scene->setBrush(tr("change brush"),list,brushes);
	}

	void GraphicsTransformTool::updateTable()
	{
		if (tableWidget == 0) return;
		
		bool connections = false, nodes = false;
		for (int i=0; i < targetItems.size(); ++i)
		{
			if (qgraphicsitem_cast<ConnectionGraphicsItem*>(targetItems[i]))
			{
				connections = true;
			}
			else
			{
				nodes = true;
			}
			
			if (connections && nodes) break;
		}

		if (connections)
		{
			if (connectionsGroup) connectionsGroup->setVisible(true);
		}
		else
		{
			if (connectionsGroup) connectionsGroup->setVisible(false);
		}
		/*
		if (nodes)
		{
			if (sizeGroup) sizeGroup->setVisible(true);
		}
		else
		{
			if (sizeGroup) sizeGroup->setVisible(false);
		}*/
	}

}
