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
#include <QPair>

#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "NodeGraphicsWriter.h"
#include "NodesTree.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "ModuleConnectionGraphicsItem.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	class MY_EXPORT ModuleTool : public Tool
	{
		Q_OBJECT

	public:
		ModuleTool();
		bool setMainWindow(MainWindow * main);

        /*!
        \brief get all the substituted items, i.e. A has replaced with B
        \param QList<QGraphicsItem*> list of items to find substitutes in
        \param QList<ItemHandle*> the obsolete items, e.g. A
        \param QList<ItemHandle*> the new item, e.g. A is B (A is obsolete)
        */
		static void connectedItems(const QList<QGraphicsItem*>&, QList<ItemHandle*>&, QList<ItemHandle*>&);
		/*!
        \brief get all the substituted items, i.e. A has replaced with B
        \param QList<ItemHandle*> list of items to find substitutes in
        \param QList<ItemHandle*> the obsolete items, e.g. A
        \param QList<ItemHandle*> the new item, e.g. A is B (A is obsolete)
        */
		static void connectedItems(const QList<ItemHandle*>&, QList<ItemHandle*>&, QList<ItemHandle*>&);

	signals:
		void itemsInsertedSignal(GraphicsScene* scene, const QList<QGraphicsItem *>& items, const QList<ItemHandle*>& handles);

	public slots:

		void select(int);
		void escapeSignal(const QWidget*);
		void itemsAboutToBeInserted(GraphicsScene* scene, QList<QGraphicsItem *>& items, QList<ItemHandle*>& handles);

		void itemsInserted(GraphicsScene* scene, const QList<QGraphicsItem *>& items, const QList<ItemHandle*>& handles);
		void itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
		void itemsMoved(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<QPointF>& distance, Qt::KeyboardModifiers modifiers);
		//void itemsAboutToBeRemoved(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&);
		//void copyItems(GraphicsScene * scene, QList<QGraphicsItem*>& , QList<ItemHandle*>& );
		//void parentHandleChanged(NetworkWindow * window, const QList<ItemHandle*>&, const QList<ItemHandle*>&);

		void mouseDragged(GraphicsScene * scene, QPointF from, QPointF to, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void mouseMoved(GraphicsScene * scene, QGraphicsItem* item, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers, QList<QGraphicsItem*>&);
		void mouseReleased(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers);

	protected:

		void MakeModuleConnection(NodeGraphicsItem*,NodeGraphicsItem*,GraphicsScene*);

		enum Mode { none, connecting };
		Mode mode;
		QGraphicsLineItem lineItem;

		static QList<QPointF> pathAroundRect(QRectF,QRectF,QPointF,QPointF);

		class VisualTool : public Tool::GraphicsItem
		{
            public:
			ModuleTool * moduleTool;
			NodeGraphicsItem image;
			VisualTool(ModuleTool*);
			void visible(bool);
			static NodeGraphicsItem * parentModule(QGraphicsItem* item);
			static QPointF getPoint(QGraphicsItem* module, QPointF scenePos, QGraphicsItem * );
		};

		QAction * makeLink, * separator;
		friend class VisualTool;

	protected slots:
		void makeLinks();

	};


}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);


#endif
