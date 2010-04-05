/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 

****************************************************************************/

#ifndef TINKERCELL_ARROWHEADSELECTIONTOOL_H
#define TINKERCELL_ARROWHEADSELECTIONTOOL_H

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
#include <QListWidget>

#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "NodeGraphicsWriter.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "GraphicsTransformTool.h"

namespace Tinkercell
{

class ArrowSelectionTool : public Tool
{
	Q_OBJECT
	
public:

	ArrowSelectionTool();
	
	bool setMainWindow(MainWindow * main);
	
public slots:
    void select(int);
	void itemsSelected(GraphicsScene *scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
	void itemsRemoved(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&);
	//void toolSelected(GraphicsScene*, GraphicalTool*, QPointF, Qt::KeyboardModifiers);
	void escapeSignal(const QWidget* );
	void sceneClicked(GraphicsScene *scene, QPointF point, Qt::MouseButton , Qt::KeyboardModifiers modifiers);
	void sceneClosed(NetworkWindow*,bool*);
	void sceneDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers);
	void toolLoaded(Tool*);
signals:
	void replaceNode(bool);
private slots:
	void replaceNodeSlot();
private:

        GraphicsTransformTool * transformTool;
	QAction showArrowSelection;
	QAction * separator;
	NodeGraphicsItem arrowPic;
        void turnOnGraphicalTools(const QList<QGraphicsItem*>& items,GraphicsScene * scene);
};

}

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif

