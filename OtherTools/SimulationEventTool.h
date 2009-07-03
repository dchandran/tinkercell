/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This class adds the "events" data to each item in Tinkercell.

****************************************************************************/

#ifndef TINKERCELL_SIMULATIONEVENTSTOOL_H
#define TINKERCELL_SIMULATIONEVENTSTOOL_H

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

#include "NodeGraphicsItem.h"
#include "DataTable.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "MainWindow.h"

namespace Tinkercell
{

	class SimulationEventsTool_FToS : public QObject
	{
		Q_OBJECT
		signals:
			void getEventTriggers(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
			void getEventResponses(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
			void addEvent(QSemaphore*,ItemHandle*,const QString&, const QString&);
		public slots:
			char** getEventTriggers(Array);
			char** getEventResponses(Array);
			void addEvent(OBJ,const char*, const char*);
	};


	class SimulationEventsTool : public Tool
	{
		Q_OBJECT

	public:
		QList<ItemHandle*> itemHandles;
		SimulationEventsTool();
		bool setMainWindow(MainWindow * main);
		QSize sizeHint() const;

	public slots:
		//void mouseDoubleClicked(GraphicsScene*, QPointF, QGraphicsItem*, Qt::MouseButton, Qt::KeyboardModifiers);
		//void keyPressed(GraphicsScene*,QKeyEvent *);
		void itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>& handles);
		void itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& list, QPointF , Qt::KeyboardModifiers );
		void addEvent();
		void removeEvents();
		void historyUpdate(int);
		void setupFunctionPointers( QLibrary * );
		void sceneClosing(GraphicsScene * scene, bool * close);

	private slots:

		void getEventTriggers(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
		void getEventResponses(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
		void addEvent(QSemaphore*,ItemHandle*,const QString&, const QString&);

	protected:

		QListWidget eventsListWidget;
		void updateTable();
		void connectTCFunctions();
		void insertData(ItemHandle*);

		QDialog * eventDialog;
		QLineEdit * eventIf, * eventThen;
		void setupDialogs();

		QList<GraphicsScene*> scenePtr;
		QList<DataTable<QString>*> textDataPtr;
		QString oldEvent;
		QGroupBox * groupBox;

		static char** _getEventTriggers(Array);
		static char** _getEventResponses(Array);
		static void _addEvent(OBJ,const char*, const char*);

	protected slots:
		void eventDialogFinished();
		void editEventsList(QListWidgetItem*);
		void visibilityChanged(bool);

	protected:

		void keyPressEvent(QKeyEvent*);

	private:

		static SimulationEventsTool_FToS fToS;

// 		class VisualTool : public GraphicalTool
// 		{
// 		public:
// 			VisualTool();
// 			void selected(const QList<QGraphicsItem*>& items);
// 			void deselected();
// 			SimulationEventsTool * editTool;
// 			bool openedByUser;
// 		private:
// 			NodeGraphicsItem item;
// 		};

// 		VisualTool visualTool;
		QDockWidget * dockWidget;

		friend class VisualTool;

	};


}

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif
