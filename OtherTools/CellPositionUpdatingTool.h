/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This tool updates the x,y attribute for "Cell" items when they are moved

****************************************************************************/

#ifndef TINKERCELL_CELLPOSITIONAUTOUPDATINGTOOL_H
#define TINKERCELL_CELLPOSITIONAUTOUPDATINGTOOL_H

#include <QtGui>
#include <QtDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QTimeLine>
#include <QTextEdit>
#include <QTextCursor>

#include "ItemHandle.h"
#include "Tool.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	class MY_EXPORT CellPositionUpdateTool : public Tool
	{
		Q_OBJECT

	public:
		CellPositionUpdateTool();
		bool setMainWindow(MainWindow * main);

	public slots:

		void itemsMoved(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<QPointF>& distance, Qt::KeyboardModifiers modifiers);


	};


}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);


#endif
