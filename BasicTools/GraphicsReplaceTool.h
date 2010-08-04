/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This is source file for the GraphicsReplaceTool plugin. 
This plugin adds the copy, cut, and paste features to Tinkercell.
It also places the buttons for these functions in the toolbar, in the edit menu, and
the context menu (mouse right-click).

****************************************************************************/


#ifndef TINKERCELL_GRAPHICSREPLACEWINDOWTOOL_H
#define TINKERCELL_GRAPHICSREPLACEWINDOWTOOL_H

#include <stdlib.h>
#include <QImage>
#include <QClipboard>

#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "NodeGraphicsWriter.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "GraphicsScene.h"

namespace Tinkercell
{

	class TINKERCELLEXPORT GraphicsReplaceTool : public Tool
	{
		Q_OBJECT

	public:
		GraphicsReplaceTool();

		bool setMainWindow(MainWindow * main);

	public slots:
		void substituteNodeGraphics();
		void substituteNodeGraphics(bool);

	private slots:
		void selectNewNodeFile();
		void replaceNode();
		void replaceNode(QListWidgetItem*);

	private:

		bool fullReactionSelected(const ConnectionGraphicsItem*);
		void makeNodeSelectionDialog();

		bool _transform;
		QList<QListWidget*> nodesListWidgets;
		QTabWidget * tabWidget;
		QStringList nodesFilesList;
		QDialog * nodeSelectionDialog;
	};

}
#endif
