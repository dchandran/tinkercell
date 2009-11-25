/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This tool handles module connections that merge items from two modules

****************************************************************************/

#ifndef TINKERCELL_MODULE_ICON_TOOL_H
#define TINKERCELL_MODULE_ICON_TOOL_H

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
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QUndoCommand>
#include <QGraphicsRectItem>
#include <QGraphicsWidget>
#include <QSlider>
#include <QToolButton>
#include <QButtonGroup>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "NodeGraphicsWriter.h"
#include "NodesTree.h"
#include "ItemHandle.h"
#include "Tool.h"

namespace Tinkercell
{

	class ModuleIconTool : public Tool
	{
		Q_OBJECT

	public:
		ModuleIconTool();
		bool setMainWindow(MainWindow * main);

	signals:
		void addNewButton(const QList<QToolButton*>& ,const QString& );
		void loadItems(QList<QGraphicsItem*>&, const QString& );
		void saveItems(const QList<QGraphicsItem*>&, const QString& filename);

	public slots:

		void toolLoaded(Tool*);
        void mouseDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers modifiers);;
		void itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
		void mousePressed(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void escapeSignal(const QWidget * sender);

	protected:

		void loadModule(GraphicsScene *, QPointF);
		void readModuleFiles();
		void addNewButton(GraphicsScene * scene, ItemHandle * module, const QRectF&);

		enum Mode { none, inserting };
		Mode mode;

		ItemHandle * moduleHandle;
		NodeGraphicsItem * moduleItem;
		QList<QGraphicsItem*> insertList;

		QButtonGroup buttonGroup;
		QAction * addToTabMenu, *separator;
		QString filename;

	protected slots:
		void enterInsertMode(QAbstractButton* button);
		void addNewButton();
	};


}

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);


#endif
