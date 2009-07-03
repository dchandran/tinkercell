/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This tool handles module connections that merge items from two modules

****************************************************************************/

#ifndef TINKERCELL_BASICMODULETOOL_H
#define TINKERCELL_BASICMODULETOOL_H

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
#include "ModuleConnectionGraphicsItem.h"

namespace Tinkercell
{

	class ModuleTool : public Tool
	{
		Q_OBJECT

	public:
		ModuleTool();
		bool setMainWindow(MainWindow * main);
	public slots:
		//void connectPoints();
		//void selectInterfaceNode();

		void modelSaved(GraphicsScene*);
		void prepareModelForSaving(GraphicsScene*);
		void escapeSignal(const QWidget*);
		void itemsInserted(GraphicsScene* scene, const QList<QGraphicsItem *>& items, const QList<ItemHandle*>& handles);
		void itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
		void itemsRemoved(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&);
		void itemsMoved(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<QPointF>& distance, Qt::KeyboardModifiers modifiers);

		void mouseDragged(GraphicsScene * scene, QPointF from, QPointF to, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void mouseMoved(GraphicsScene * scene, QGraphicsItem* item, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers, QList<QGraphicsItem*>&);
		void mouseReleased(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void mouseDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers modifiers);;

	protected:

		ModuleConnectionGraphicsItem * MakeModuleConnection(ModuleLinkerItem*,ModuleLinkerItem*,GraphicsScene*);

		enum Mode { none, connecting, inserting };
		Mode mode;
		QGraphicsLineItem lineItem;

		static QList<QPointF> pathAroundRect(QRectF,QRectF,QPointF,QPointF);

// 		class VisualTool : public GraphicalTool
// 		{
// 		public:
// 			ModuleTool * moduleTool;
// 			VisualTool();
// 			bool setMainWindow(MainWindow * main);
// 			void selected(const QList<QGraphicsItem*>&);
// 			void setVisible(bool);
// 			static NodeGraphicsItem * parentModule(QGraphicsItem* item);
// 			static QPointF getPoint(QGraphicsItem* module, QPointF scenePos, QGraphicsItem * );
// 		private:
// 			NodeGraphicsItem image;
// 			QAction * makeLinks;
// 			QAction * separator;
// 		};
// 		friend class VisualTool;
// 		VisualTool graphicalTool;
	protected slots:

		void makeLinks();
		void makeLinks(const QList<QGraphicsItem*>&);

	};


}

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);


#endif
