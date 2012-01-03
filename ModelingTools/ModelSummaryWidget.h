/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 A tool for displaying all the handles (as a tree) and their attributes. This tool
 also handles move events where an item is moved into a module or Compartment

****************************************************************************/

#ifndef TINKERCELL_PARAMETERWIDGETTOOL_H
#define TINKERCELL_PARAMETERWIDGETTOOL_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
#include <QPixmap>
#include <QPair>
#include <QList>
#include <QHash>
#include <QString>
#include <QtDebug>
#include <QGraphicsItem>
#include <QAction>
#include <QFile>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QStandardItem>
#include <QComboBox>
#include <QDoubleSpinBox>

#include "UndoCommands.h"
#include "GraphicsScene.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "ContainerTreeModel.h"

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
	class TINKERCELLEXPORT ModelSummaryWidget : public Tool
	{
		Q_OBJECT

	public:

		ModelSummaryWidget();
		~ModelSummaryWidget();
		bool setMainWindow(MainWindow * main);
		QSize sizeHint() const;

	public slots:
		void updateTree(int);
		void windowChanged(NetworkWindow * , NetworkWindow * );
		void windowClosed(NetworkHandle *);
		void indexSelected(const QModelIndex&);
	private:
		ContainerTreeDelegate * treeDelegate;
		ContainerTreeModel * treeModel;
		QTreeView * treeView;
		void adjustRates(GraphicsScene * scene, QList<ItemHandle*> childItems, QList<ItemHandle*> parents);
	};


}

#endif

