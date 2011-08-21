/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This class adds the "events" data to each item in Tinkercell.
 
****************************************************************************/

#ifndef TINKERCELL_VIEWALLTABLESWIDGET_H
#define TINKERCELL_VIEWALLTABLESWIDGET_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHash>
#include <QGroupBox>
#include <QUndoCommand>
#include <QGraphicsRectItem>
#include <QGraphicsWidget>
#include <QSlider>
#include <QToolButton>
#include <QButtonGroup>
#include <QListView>
#include <QTextCharFormat>

#include "CodeEditor.h"
#include "NodeGraphicsItem.h"
#include "DataTable.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "MainWindow.h"

#ifndef TINKERCELLEXPORT
#ifdef Q_WS_WIN
#   if defined(TC_EXPORTS) || defined(TinkerCell_EXPORTS)
#       define TINKERCELLEXPORT __declspec(dllexport)
#   else
#       define TINKERCELLEXPORT __declspec(dllimport)
#   endif
#else
#    define TINKERCELLEXPORT
#endif
#endif

namespace Tinkercell
{

	class ViewTablesTool : public Tool
	{
		Q_OBJECT

	public:
		ViewTablesTool();
		bool setMainWindow(MainWindow * main);
		QSize sizeHint() const;
	
	public slots:
		void select(int i=0);
		void deselect(int i=0);
		void itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& list, QPointF , Qt::KeyboardModifiers );
		void itemsInserted(NetworkHandle * , const QList<ItemHandle*>& handles);
		
	protected:

		CodeEditor * textEdit;
		QListWidget tables;
		void updateList();
		
	protected slots:
		void currentItemChanged(QListWidgetItem *, QListWidgetItem *);
	
	private:

		class GraphicsItem2 : public ToolGraphicsItem
		{
			public:
				GraphicsItem2(Tool * tool);
				void visible(bool);
		};

		bool openedByUser;
		NodeGraphicsItem item;
		ItemHandle* itemHandle;
		QTextCharFormat headerFormat, regularFormat;
		
	};


}

//extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif
