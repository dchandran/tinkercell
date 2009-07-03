/****************************************************************************
 **
 ** Copyright (c) 2008 Deepak Chandran
 ** Contact: Deepak Chandran (dchandran1@gmail.com)
 **
 ** See COPYWRITE.TXT
 **
 ****************************************************************************/

#ifndef PARTIMAGE_MAKER_MAINWINDOW_H
#define PARTIMAGE_MAKER_MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QToolBox>
#include <QtDebug>
#include <QtGui>
#include <QSlider>
#include <QAction>
#include <QFileDialog>
#include "NodeGraphicsScene.h"
#include "NodeGraphicsWriter.h"
#include "NodeGraphicsReader.h"

namespace NodeImageDesigner
{

typedef Tinkercell::NodeGraphicsReader NodeImageReader;
typedef Tinkercell::NodeGraphicsWriter NodeImageWriter;

/*! \brief The main window that provides the interface for constructing a new NodeGraphicsItem and saving it*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

public slots:
	void setArcStartAngle(int value);
	void setArcSpanAngle(int value);
	void setLineWidth(int value);
	
	void setAlpha1(int value);
	void setAlpha2(int value);
	void color1Changed();
	void color2Changed();
	
	void buttonPressed(int id);
	
	void fillSelected();
	void sizeChanged(double);

	void linearGradientSelected();

	void radialGradientSelected();
	
	void save();
	void load();
	void newFile();
	void openDocumentation();
signals:
	void saved();
	void exited();
	
protected:
	void keyPressEvent ( QKeyEvent * );
	void wheelEvent ( QWheelEvent * );

private:
    DrawScene drawScene;
	QGraphicsView graphicsView;
	QButtonGroup* buttonGroup;	
	
	QToolButton * shapeButtons[3];
	QPushButton * colorButtons[2];
	
	QWidget* makeToolBox();
	
	QIcon paintShape(int type, int linewdth);
	int alpha;
	
	void createMenus();
	
	QAction *fill, *linearGradient, *radialGradient;
	
	QString previousFileName;
	
	QDoubleSpinBox height,width;
};

}
#endif
