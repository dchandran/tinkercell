/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This class adds the "Functions" and "Assignments" data to each item in Tinkercell and
 provides the user interface and C functions for changing those values.
 
****************************************************************************/

#ifndef TINKERCELL_ASSIGNMENTFUNCTIONSTOOL_H
#define TINKERCELL_ASSIGNMENTFUNCTIONSTOOL_H

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
#include <QTableWidget>
#include <QButtonGroup>
#include <QListView>

#include "Core/PartGraphicsItem.h"
#include "Core/DataTable.h"
#include "Core/ItemHandle.h"
#include "Core/Tool.h"
#include "Core/MainWindow.h"

namespace Tinkercell
{

	class AssignmentFunctionsTool_FToS : public QObject
	{
		Q_OBJECT
		signals:
			void getForcingFunctionNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
			void getForcingFunctionAssignments(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
			void addForcingFunction(QSemaphore*,ItemHandle*,const QString&, const QString&);
		public slots:
			char** getForcingFunctionNames(Array);
			char** getForcingFunctionAssignments(Array);
			void addForcingFunction(OBJ,const char*, const char*);
	};

	class AssignmentFunctionsTool : public Tool
	{
		Q_OBJECT;
	
	signals:
		void plot(const DataTable<qreal>&,const QString& = QString(),int xaxis=0,int all = 0);

	public:
		QList<ItemHandle*> itemHandles;
		AssignmentFunctionsTool();
		bool setMainWindow(MainWindow * main);
		QSize sizeHint() const;
	
	public slots:
		void itemsInserted(NetworkWindow * , const QList<ItemHandle*>& handles);
		void itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& list, QPointF , Qt::KeyboardModifiers );
		void addFunction();
		void removeFunctions();
		void historyUpdate(int);
		void setupFunctionPointers( QLibrary * );
		void sceneClosing(GraphicsScene * scene, bool * close);
		
	private slots:
		void getForcingFunctionNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
		void getForcingFunctionAssignments(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
		void addForcingFunction(QSemaphore*,ItemHandle*,const QString&, const QString&);
		
	protected:

		//QListWidget functionsListWidget;
		QTableWidget tableWidget;
		void updateTable();
		void connectTCFunctions();
		void insertData(ItemHandle*);

		//QDialog * functionDialog;
		//QLineEdit * functionVar, * functionDef;
		//void setupDialogs();
		
		QList<GraphicsScene*> scenePtr;
		QList<DataTable<QString>*> textDataPtr;
		QString oldVar;
		QGroupBox * groupBox;
		
		static char** _getForcingFunctionNames(Array);
		static char** _getForcingFunctionAssignments(Array);
		static void _addForcingFunction(OBJ,const char*, const char*);
	protected slots:
		
		//void functionDialogFinished();
		//void editFunctionsList(QListWidgetItem*);
		//void visibilityChanged(bool);
		void aboutToDisplayModel(const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations);
		void displayModel(QTabWidget& widgets, const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations);
		void toolLoaded(Tool*);
		
		void assignmentTableChanged(int,int);
		void functionsTableChanged(int,int);
		
	//protected:		
		//void keyPressEvent(QKeyEvent*);
	
	private:
		//QStringList updatedFunctions, updatedFunctionNames;
	
		static AssignmentFunctionsTool_FToS fToS;
	
		class VisualTool : public GraphicalTool
		{
		public:
			VisualTool();
			void selected(const QList<QGraphicsItem*>& items);
			void deselected();
			AssignmentFunctionsTool * editTool;
			bool openedByUser;
		private:
			PartGraphicsItem item;
		};
		
		VisualTool visualTool;
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
