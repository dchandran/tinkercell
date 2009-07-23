/****************************************************************************
 **
 ** Copyright (c) 2008 Deepak Chandran
 ** Contact: Deepak Chandran (dchandran1@gmail.com)
 **
 ** See COPYRIGHT.TXT
 **
 ****************************************************************************/

#include <QtDebug>
#include <QRegExp>
#include <QScrollArea>
#include <QSplitter>
#include <QDesktopServices>
#include "NodeGraphicsWindow.h"

	
	NodeImageDesigner::MainWindow::MainWindow()
	{
		createMenus();
		width.setRange(1,1000);
		height.setRange(1,1000);
		width.setValue(50);
		height.setValue(50);
		
		connect(&height,SIGNAL(valueChanged ( double ) ), this, SLOT(sizeChanged(double)));
		connect(&width,SIGNAL(valueChanged ( double ) ), this, SLOT(sizeChanged(double)));
		
		setPalette(QPalette(QColor(255, 255, 255)));
		setAutoFillBackground(true);
		
		QSplitter * splitter = new QSplitter;
		QWidget * toolBox = makeToolBox();
		QScrollArea * scrollArea = new QScrollArea;
		scrollArea->setWidget(toolBox);
		toolBox->setMaximumWidth(150);	
		splitter->addWidget(scrollArea);
		
		graphicsView.setScene(&drawScene);
		graphicsView.setRenderHint(QPainter::Antialiasing);
		splitter->addWidget(&graphicsView);
		
		splitter->setStretchFactor(0,1);
		splitter->setStretchFactor(1,1);
		
		setCentralWidget(splitter);
		setWindowTitle(tr("NodeGraphicsItem Builder"));
		
		statusBar()->showMessage("Welcome to NodeGraphicsItem builder...happy building!");
	}
	
	void NodeImageDesigner::MainWindow::sizeChanged(double)
	{
		drawScene.mode = -10;
		drawScene.node.resetTransform();
		drawScene.node.scale( width.value()/drawScene.node.sceneBoundingRect().width(), height.value()/drawScene.node.sceneBoundingRect().height());
	}

	void NodeImageDesigner::MainWindow::createMenus()
	{
		QMenu * fileMenu;
		QMenu * helpMenu;
		QAction	* newAction;
		QAction	* exitAction;
		QAction	* saveAction;
		QAction	* loadAction;
		QAction	* docAction;
	
		exitAction = new QAction(tr("E&xit"), this);
		exitAction->setShortcut(tr("Ctrl+X"));
		exitAction->setStatusTip(tr("Quit Scenediagram example"));
		connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
		connect(exitAction, SIGNAL(triggered()), this, SIGNAL(exited()));
		
		loadAction = new QAction(tr("&Open"), this);
		loadAction->setShortcut(tr("Ctrl+O"));
		loadAction->setStatusTip(tr("Load a new Diagram"));
		connect(loadAction, SIGNAL(triggered()), this, SLOT(load()));
		
		saveAction = new QAction(tr("&Save"), this);
		saveAction->setShortcut(tr("Ctrl+S"));
		saveAction->setStatusTip(tr("Save current Diagram"));
		connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
		connect(saveAction, SIGNAL(triggered()), this, SIGNAL(saved()));
		
		newAction = new QAction(tr("&New"), this);
		newAction->setShortcut(tr("Ctrl+N"));
		newAction->setStatusTip(tr("Start new Diagram"));
		connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));
		
		fileMenu = menuBar()->addMenu(tr("&File"));
		fileMenu->addAction(newAction);
		fileMenu->addAction(loadAction);
		fileMenu->addAction(saveAction);
		fileMenu->addAction(exitAction);
		
		docAction = new QAction(tr("&Tutorial"), this);
		docAction->setShortcut(tr("F1"));
		connect(docAction, SIGNAL(triggered()), this, SLOT(openDocumentation()));
		
		helpMenu = menuBar()->addMenu(tr("&Help"));
		helpMenu->addAction(docAction);
	}
	
	void NodeImageDesigner::MainWindow::openDocumentation()
	{
		
		QString appDir = QCoreApplication::applicationDirPath();
		QString doc = tr("file:") + appDir + tr("/Documentation/NodeGraphics_Documentat.html");
		QDesktopServices::openUrl(QUrl(doc));
	}

	void NodeImageDesigner::MainWindow::load()
	{
		if (previousFileName.isEmpty() || previousFileName.isEmpty())
			previousFileName = QDir::currentPath();
		QString fileName = 
				QFileDialog::getOpenFileName(this, tr("Save Drawable File"),
											  previousFileName,
											  tr("XML Files (*.xml)"));
		if (fileName.isEmpty())
			return;
		
		previousFileName = fileName;
			 
		QFile file (fileName);
		
		if (!file.open(QFile::ReadOnly | QFile::Text)) {
			QMessageBox::warning(this, tr("NodeGraphicsItem File"),
								  tr("Cannot read file %1:\n%2.")
								  .arg(fileName)
								  .arg(file.errorString()));
			return;
		}
		
		drawScene.removeItem(&drawScene.node);
		drawScene.clear();

		for (int i=0; i < drawScene.node.controlPoints.size(); ++i)
			drawScene.removeItem(drawScene.node.controlPoints[i]);
		
		NodeImageReader reader;	
		reader.readNodeGraphics(&drawScene.node,&file);
		drawScene.node.refresh();
		
		for (int i=0; i < drawScene.node.controlPoints.size(); ++i)
			drawScene.addItem(drawScene.node.controlPoints[i]);
		drawScene.addItem(&drawScene.node);
		drawScene.currentPoints.clear();
		drawScene.currentShape->controlPoints.clear();
		if (drawScene.node.defaultSize.width() > 0 && drawScene.node.defaultSize.height() > 0)
		{
			width.setValue(drawScene.node.defaultSize.width());
			height.setValue(drawScene.node.defaultSize.height());
			drawScene.node.resetTransform();
		}
	}

	void NodeImageDesigner::MainWindow::save()
	{
		if (previousFileName.isEmpty() || previousFileName.isEmpty())
			previousFileName = QDir::currentPath();
			
		QString fileName =
				 QFileDialog::getSaveFileName(this, tr("Save Drawable File"),
											  previousFileName,
											  tr("XML Files (*.xml)"));
		if (fileName.isEmpty())
			return;
		
		previousFileName = fileName;
		
		QFile file (fileName);
		
		if (!file.open(QFile::WriteOnly | QFile::Text)) {
			 QMessageBox::warning(this, tr("NodeGraphicsItem File"),
								  tr("Cannot write file %1:\n%2.")
								  .arg(fileName)
								  .arg(file.errorString()));
			 return;
		 }
		
		NodeImageWriter writer;
		
		for (int i=0; i < drawScene.node.shapes.size(); ++i)
		{
			if (drawScene.node.shapes[i])
			{
				drawScene.node.shapes[i]->defaultPen = drawScene.node.shapes[i]->pen();
				drawScene.node.shapes[i]->defaultBrush = drawScene.node.shapes[i]->brush();
			}
		}
		
		drawScene.node.fileName = fileName;
		writer.writeXml(&(drawScene.node),&file);
		
		QGraphicsView * view = 0;
		if (drawScene.views().size() > 0)
			view = drawScene.views().first();	
			
		for (int i=0; i < drawScene.node.controlPoints.size(); ++i)
			drawScene.node.controlPoints[i]->setVisible(false);

		QRectF sceneBoundingRect = drawScene.node.sceneBoundingRect();
		QPixmap pixmap(256, (int)(256.0 * sceneBoundingRect.height()/sceneBoundingRect.width()));
		pixmap.fill(QColor(255,255,255,255));
		
		QPainter painter(&pixmap);
		painter.setRenderHint(QPainter::Antialiasing);
		
		//if (view) 
			//view->render(&painter);
		drawScene.render(&painter,QRectF(),sceneBoundingRect.adjusted(-5,-5,5,5)); 
	
		fileName.replace(QRegExp("\\..+"),"");
		pixmap.save(fileName + tr(".PNG"),"PNG");

		for (int i=0; i < drawScene.node.controlPoints.size(); ++i)
			drawScene.node.controlPoints[i]->setVisible(true);
	}

	void NodeImageDesigner::MainWindow::newFile()
	{
		drawScene.removeItem(&drawScene.node);
		drawScene.clear();
		drawScene.node.clear();
		drawScene.currentShape->controlPoints.clear();
		drawScene.currentPoints.clear();
		drawScene.addItem(&drawScene.node);
	}

	void NodeImageDesigner::MainWindow::fillSelected()
	{
		drawScene.fillType = 3;	
	}

	void NodeImageDesigner::MainWindow::linearGradientSelected()
	{
		drawScene.fillType = 0;	
	}

	void NodeImageDesigner::MainWindow::radialGradientSelected()
	{
		drawScene.fillType = 1;	
	}

	void NodeImageDesigner::MainWindow::setArcStartAngle(int value)
	{
		drawScene.arcStart = value;	
			
		QPixmap pixmap(250, 250);
		pixmap.fill(Qt::transparent);
		
		QPainter painter(&pixmap);
		painter.setPen(QPen(Qt::black, drawScene.lineWidth));
		painter.translate(125, 125);
		
		painter.setRenderHint(QPainter::Antialiasing);
		
		painter.drawArc(QRect(-100,-100,200,200),16*drawScene.arcStart,16*drawScene.arcSpan);
			
		if (shapeButtons[0])
			shapeButtons[0]->setIcon(QIcon(pixmap));	
	}

	void NodeImageDesigner::MainWindow::setArcSpanAngle(int value)
	{
		drawScene.arcSpan = value;
			
		QPixmap pixmap(250, 250);
		pixmap.fill(Qt::transparent);
		
		QPainter painter(&pixmap);
		painter.setPen(QPen(Qt::black, drawScene.lineWidth));
		painter.translate(125, 125);
		
		painter.setRenderHint(QPainter::Antialiasing);
		
		painter.drawArc(QRect(-100,-100,200,200),16*drawScene.arcStart,16*drawScene.arcSpan);
			
		if (shapeButtons[0])
			shapeButtons[0]->setIcon(QIcon(pixmap));	
	}

	void NodeImageDesigner::MainWindow::setLineWidth(int value)
	{	
		for (int i=0; i < 3; ++i)
		{			
			shapeButtons[i]->setIcon(paintShape(i,value));
			drawScene.lineWidth = value;
		}
		
	}

	void NodeImageDesigner::MainWindow::setAlpha1(int value)
	{
		drawScene.color1.setAlpha(value);
		QPixmap pixmap(100,10);
		QPainter painter(&pixmap);
		painter.setBrush(QColor(255,255,255,255));
		painter.drawRoundRect(-100,-12,200,24);
		painter.setBrush(drawScene.color1);
		painter.drawRoundRect(-100,-12,200,24);
		colorButtons[0]->setIcon(QIcon(pixmap));
		colorButtons[0]->setIconSize(QSize(colorButtons[0]->width(), 10));
		buttonPressed(drawScene.mode);	
	}

	void NodeImageDesigner::MainWindow::setAlpha2(int value)
	{
		drawScene.color2.setAlpha(value);
		QPixmap pixmap(100,10);
		QPainter painter(&pixmap);
		painter.setBrush(QColor(255,255,255,255));
		painter.drawRoundRect(-100,-12,200,24);
		painter.setBrush(drawScene.color2);
		painter.drawRoundRect(-100,-12,200,24);
		colorButtons[1]->setIcon(QIcon(pixmap));
		colorButtons[1]->setIconSize(QSize(colorButtons[1]->width(), 10));
		buttonPressed(drawScene.mode);	
	}

	void NodeImageDesigner::MainWindow::color1Changed()
	{
		QColor color = QColorDialog::getColor(drawScene.color1);
		if (color.isValid())
		{
			color.setAlphaF(drawScene.color1.alphaF());
			drawScene.color1 = color;
			setAlpha1(drawScene.color1.alpha());
		}
	}

	void NodeImageDesigner::MainWindow::color2Changed()
	{
		QColor color = QColorDialog::getColor(drawScene.color2);
		if (color.isValid())
		{
			color.setAlphaF(drawScene.color2.alphaF());
			drawScene.color2 = color;
			setAlpha2(drawScene.color2.alpha());
		}
	}


	void NodeImageDesigner::MainWindow::buttonPressed(int id)
	{
		drawScene.node.resetTransform();
		drawScene.currentPoints.clear();	
		drawScene.mode = id;
		
		if (id == 4)
		{
			QPixmap bucket(QString(":/images/bucket.png"));	
			bucket = bucket.scaled(QSize(30,30));
			QTransform transform;
			transform.translate(-35,-35);
			bucket.setMask(bucket.createMaskFromColor(QColor(255,255,255)));
			setCursor(bucket.transformed(transform));
		}
		else
		if (id == 5)
		{
			QPixmap pencil(QString(":/images/pencil.png"));
			pencil.setMask(pencil.createMaskFromColor(QColor(255,255,255)));
			QTransform transform;
			pencil = pencil.transformed(transform.translate(-35,-35));
			pencil = pencil.scaled(QSize(30,30));
			setCursor(pencil);
		}
		else
			setCursor(Qt::ArrowCursor);			
	}

	QWidget* NodeImageDesigner::MainWindow::makeToolBox()
	{
		buttonGroup = new QButtonGroup;
		buttonGroup->setExclusive(true);
	    
		connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(buttonPressed(int)));
		
		QGroupBox * setSizeGroup = new QGroupBox("set width and height");
		QHBoxLayout * setSizeGroupLayout = new QHBoxLayout(setSizeGroup);
		setSizeGroup->setAttribute(Qt::WA_ContentsPropagated);
		setSizeGroupLayout->addWidget(&width,0,Qt::AlignLeft);
		setSizeGroupLayout->addWidget(&height,1,Qt::AlignLeft);
		
		QToolButton *button;
		
		QGroupBox * arrowPointGroup = new QGroupBox("select or insert points");	
		QVBoxLayout * arrowPointGroupLayout = new QVBoxLayout(arrowPointGroup);
		arrowPointGroup->setAttribute(Qt::WA_ContentsPropagated);
		button = new QToolButton(arrowPointGroup);
		button->setIcon(paintShape(-2,4));
		button->setIconSize(QSize(50, 50));
		button->setCheckable(true);
		buttonGroup->addButton(button, -2);
		arrowPointGroupLayout->addWidget(button,0,Qt::AlignCenter);
			
		button = new QToolButton(arrowPointGroup);
		button->setIcon(paintShape(-1,5));
		button->setIconSize(QSize(50, 50));
		button->setCheckable(true);
		buttonGroup->addButton(button, -1);
		arrowPointGroupLayout->addWidget(button,0,Qt::AlignCenter);

		QGroupBox * shapesGroup = new QGroupBox("choose shape type");
		QVBoxLayout * shapesGroupLayout = new QVBoxLayout(shapesGroup);
		//shapesGroup->setTitle("basic shapes");
		
		drawScene.lineWidth = 8;
		drawScene.arcStart = 0;
		drawScene.arcSpan = 90;
		alpha = 255;
		
		for (int i=0; i < 3; ++i)
		{
			button = new QToolButton(shapesGroup);
			button->setIcon(paintShape(i,drawScene.lineWidth));
			button->setIconSize(QSize(50, 50));
			button->setCheckable(true);
			buttonGroup->addButton(button, i);
			shapesGroupLayout->addWidget(button,0,Qt::AlignCenter);
			shapeButtons[i] = button;
		}
		
		QSlider *arcStart = new QSlider(Qt::Horizontal,shapesGroup);
		arcStart->setRange(0, 360);
		arcStart->setValue(drawScene.arcStart);
		connect(arcStart,SIGNAL(valueChanged(int)),this,SLOT(setArcStartAngle(int)));
		shapesGroupLayout->addWidget(arcStart);
		
		QSlider *arcSpan = new QSlider(Qt::Horizontal,shapesGroup);	
		arcSpan->setRange(0, 360);
		arcSpan->setValue(drawScene.arcSpan);
		connect(arcSpan,SIGNAL(valueChanged(int)),this,SLOT(setArcSpanAngle(int)));
		shapesGroupLayout->addWidget(arcSpan);
		
		QSlider *linewidth = new QSlider(Qt::Horizontal,shapesGroup);	
		linewidth->setRange(0, 20);
		linewidth->setValue(drawScene.lineWidth);
		connect(linewidth,SIGNAL(valueChanged(int)),this,SLOT(setLineWidth(int)));
		shapesGroupLayout->addWidget(linewidth);
		
		shapesGroup->setLayout(shapesGroupLayout);
		
		QGroupBox * attributesGroup = new QGroupBox("fill and outline color");
		QVBoxLayout * attributesGroupLayout = new QVBoxLayout(attributesGroup);
		//attributesGroup->setTitle("shape attributes");
		
		QHBoxLayout * bucketAndPencilLayout = new QHBoxLayout;
		
		button = new QToolButton(attributesGroup);
		button->setIcon(QIcon(QPixmap(QString(":/images/pencil.png"))));
		button->setIconSize(QSize(30, 30));
		button->setCheckable(true);
		button->setPopupMode(QToolButton::MenuButtonPopup);
		buttonGroup->addButton(button, 5);
		bucketAndPencilLayout->addWidget(button,0,Qt::AlignRight);
		
		button = new QToolButton(attributesGroup);
		button->setIcon(QIcon(QPixmap(QString(":/images/bucket.png"))));
		button->setIconSize(QSize(30, 30));
		button->setCheckable(true);
		
		QMenu * fillMenu = new QMenu(this);
		fill = new QAction(tr("no gradient"),this);
		linearGradient = new QAction(tr("linear gradient"),this);
		radialGradient = new QAction(tr("radial gradient"),this);
		fillMenu->addAction(fill);
		fillMenu->addAction(linearGradient);
		fillMenu->addAction(radialGradient);
		button->setMenu(fillMenu);
		button->setPopupMode(QToolButton::MenuButtonPopup);
		
		buttonGroup->addButton(button, 4);
		bucketAndPencilLayout->addWidget(button,0,Qt::AlignLeft);
		
		connect(fill,SIGNAL(triggered()),this,SLOT(fillSelected()));
		connect(linearGradient,SIGNAL(triggered()),this,SLOT(linearGradientSelected()));
		connect(radialGradient,SIGNAL(triggered()),this,SLOT(radialGradientSelected()));
		
		QWidget * bucketAndPencil = new QWidget;
		bucketAndPencil->setLayout(bucketAndPencilLayout);
		attributesGroupLayout->addWidget(bucketAndPencil,1);
		
		colorButtons[0] = new QPushButton(attributesGroup);
		QPixmap pixmap1(100, 10);
		QPainter painter1(&pixmap1);
		drawScene.color1 = QColor(100,100,255);
		painter1.setBrush(drawScene.color1);
		painter1.drawRoundRect(-attributesGroup->width(),-10,attributesGroup->width()*2,20);
		colorButtons[0]->setIcon(QIcon(pixmap1));
		colorButtons[0]->setIconSize(QSize(attributesGroup->width(), 10));
		connect(colorButtons[0], SIGNAL(pressed()), this, SLOT(color1Changed()));
		attributesGroupLayout->addWidget(colorButtons[0],1,Qt::AlignCenter);
		
		QSlider *transparency = new QSlider(Qt::Horizontal,attributesGroup);	
		transparency->setRange(0, 255);
		transparency->setValue(alpha);
		connect(transparency,SIGNAL(valueChanged(int)),this,SLOT(setAlpha1(int)));
		attributesGroupLayout->addWidget(transparency);
		
		colorButtons[1] = new QPushButton(attributesGroup);
		QPixmap pixmap2(100, 10);
		QPainter painter2(&pixmap2);
		drawScene.color2 = QColor(100,100,255);
		painter2.setBrush(drawScene.color2);
		painter2.drawRoundRect(-attributesGroup->width(),-10,attributesGroup->width()*2,20);
		colorButtons[1]->setIcon(QIcon(pixmap2));
		colorButtons[1]->setIconSize(QSize(attributesGroup->width(), 10));
		connect(colorButtons[1], SIGNAL(pressed()), this, SLOT(color2Changed()));
		attributesGroupLayout->addWidget(colorButtons[1],1,Qt::AlignCenter);	
		
		transparency = new QSlider(Qt::Horizontal,attributesGroup);	
		transparency->setRange(0, 255);
		transparency->setValue(alpha);
		connect(transparency,SIGNAL(valueChanged(int)),this,SLOT(setAlpha2(int)));
		attributesGroupLayout->addWidget(transparency);
		
		shapesGroup->setAttribute(Qt::WA_ContentsPropagated);
		attributesGroup->setAttribute(Qt::WA_ContentsPropagated);
		QVBoxLayout *layout = new QVBoxLayout;
		
		layout->addWidget(setSizeGroup);
		layout->addWidget(arrowPointGroup);
		layout->addWidget(shapesGroup);
		layout->addWidget(attributesGroup);
		
		QWidget *widget = new QWidget;
		widget->setLayout(layout);
		
		return widget;
	}


	QIcon NodeImageDesigner::MainWindow::paintShape(int type, int lineWidth)
	{
		QPixmap pixmap(250, 250);
		pixmap.fill(Qt::transparent);
		
		QPainter painter(&pixmap);
		painter.setPen(QPen(Qt::black, lineWidth));
		painter.translate(125, 125);
		
		painter.setRenderHint(QPainter::Antialiasing);
		
		switch(type)
		{
			case -2:
				{				
					QPainterPath path;
					path.moveTo(QPoint(-60,-60));
					path.lineTo(QPoint(-20,50));
					path.lineTo(QPoint(-20,0));
					path.lineTo(QPoint(60,80));
					
					path.lineTo(QPoint(80,60));				
					path.lineTo(QPoint(0,-20));
					path.lineTo(QPoint(50,-20));
					path.lineTo(QPoint(-60,-60));
					painter.fillPath(path,QBrush(QColor(100,100,255)));
					painter.setPen(QPen(QColor(0,0,0),5));
					
					//qDebug() << path.boundingRect().x();
					//painter.translate(-path.boundingRect().x(), -path.boundingRect().y());
					painter.drawPath(path);				
				}
				break;
			case -1:
				{				
					painter.setBrush(QBrush(QColor(0,0,255,100)));
					painter.drawEllipse(QRect(-50,-50,100,100));
				}
				break;
			case 0:
				{
					painter.drawArc(QRect(-100,-100,200,200),16*drawScene.arcStart,16*drawScene.arcSpan);
				}
				break;
			case 1:
				painter.drawLine(QLineF(QPoint(-100,-100),QPoint(100,100)));
				break;
			case 2:
				{
					QPainterPath path;
					path.moveTo(QPoint(-100,-100));
					path.cubicTo(QPoint(100,-90),QPoint(-100,90),QPoint(100,100));
					painter.drawPath(path);
				}
				break;
		}
		
		return QIcon(pixmap);
	}

	void NodeImageDesigner::MainWindow::keyPressEvent ( QKeyEvent * keyEvent )
	{
		if ((keyEvent->key() == Qt::Key_Plus) || (keyEvent->key() == Qt::Key_Equal) )
		{	
			drawScene.node.resetTransform();
			graphicsView.scale(1.1,1.1);
		}
		else
		if ((keyEvent->key() == Qt::Key_Underscore) || (keyEvent->key() == Qt::Key_Minus))
		{
			graphicsView.scale(0.9,0.9);
		}
	}

	void NodeImageDesigner::MainWindow::wheelEvent ( QWheelEvent * wheelEvent)
	{
		if (wheelEvent->modifiers() == Qt::ControlModifier)
		{
			int numDegrees = wheelEvent->delta() / 8;
			qreal numSteps = numDegrees / 15.0;
			if (numSteps > 0)
				graphicsView.scale(numSteps,numSteps);
			else
				graphicsView.scale(-1.0/numSteps,-1.0/numSteps);
			wheelEvent->accept();
		}
		else
		QMainWindow::wheelEvent(wheelEvent);
	}

