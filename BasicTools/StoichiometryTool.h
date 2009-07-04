/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool places a stoichiometry table and a table of rates inside all connection handles.
 An associated GraphicsTool is also defined. This allow brings up a table for editting
 the stoichiometry and rates tables.

****************************************************************************/

#ifndef TINKERCELL_STOICHIOMETRYTOOL_H
#define TINKERCELL_STOICHIOMETRYTOOL_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QUndoCommand>
#include <QGraphicsRectItem>
#include <QGraphicsWidget>
#include <QSlider>
#include <QToolButton>
#include <QButtonGroup>
#include <QTableView>
#include <QRegExp>
#include <QTextEdit>

#include "NodeGraphicsItem.h"
#include "MainWindow.h"
#include "NetworkWindow.h"
#include "DataTable.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "SpinBoxDelegate.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	class StoichiometryTool_FToS : public QObject
	{
		Q_OBJECT
		signals:
			void getStoichiometry(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&);
			void setStoichiometry(QSemaphore*,QList<ItemHandle*>&,const DataTable<qreal>&);
			void getRates(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
			void setRates(QSemaphore*,QList<ItemHandle*>&,const QStringList&);
		public slots:
			Matrix getStoichiometry(Array);
			void setStoichiometry(Array,Matrix );
			char** getRates(Array);
			void setRates(Array,char** );
	};

	class MY_EXPORT StoichiometryTool : public Tool
	{
		Q_OBJECT;

	public:

		QList<ConnectionHandle*> connectionHandles;
		StoichiometryTool();
		bool setMainWindow(MainWindow * main);
		QSize sizeHint() const;
		void hideMatrix();
		void showMatrix();
		
		static bool parseRateString(GraphicsScene*, ItemHandle *, QString&);
		static DataTable<qreal> getStoichiometry(const QList<ItemHandle*>&,const QString& replaceDot = QString("_"));
		static void setStoichiometry(GraphicsScene*,QList<ItemHandle*>&,const DataTable<qreal>&,const QString& replaceDot = QString("_"));
		static QStringList getRates(const QList<ItemHandle*>&,const QString& replaceDot = QString("_"));
		static void setRates(GraphicsScene*,QList<ItemHandle*>&,const QStringList&,const QString& replaceDot = QString("_"));

	public slots:
                void select(int);
                void deselect(int);

		void itemsInserted(NetworkWindow * , const QList<ItemHandle*>& handles);
        void itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
		void toolLoaded(Tool*);
		void historyUpdate(int);
		void setupFunctionPointers(QLibrary*);
                void sceneClosing(NetworkWindow * scene, bool * close);
		
		void aboutToDisplayModel(const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations);
		void displayModel(QTabWidget& widgets, const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations);

	signals:
		void setMiddleBox(int,const QString&);
		
	private slots:
		void getStoichiometrySlot(QSemaphore*, DataTable<qreal>*, const QList<ItemHandle*>&);
		void getRatesSlot(QSemaphore *, QStringList*, const QList<ItemHandle*>&);
		void setStoichiometrySlot(QSemaphore*, QList<ItemHandle*>&, const DataTable<qreal>&);
		void setRatesSlot(QSemaphore*, QList<ItemHandle*>&, const QStringList&);
		
	protected:
		void insertDataMatrix(ConnectionHandle * handle);
		QTableWidget ratesTable, matrixTable;
		SpinBoxDelegate delegate;
		//QTextEdit * textView;
		//int updateText();
		void updateTable();
	
        protected slots:
		void addRow();
		void removeRow();
		void addCol();
		void removeCol();
		void setRate(int, int);
		void setStoichiometry(int, int);
		void addReverseReaction();

	protected:
		QList<GraphicsScene*> scenePtr;
		QList<DataTable<qreal>*> numericalDataPtr;
		QList<DataTable<QString>*> textDataPtr;
		QGroupBox * ratesBox, * matrixBox;
		
		static StoichiometryTool_FToS fToS;
		void connectCFuntions();
		static Matrix _getStoichiometry(Array);
		static void _setStoichiometry(Array ,Matrix );
		static char** _getRates(Array );
		static void _setRates(Array ,char** );
		virtual void keyPressEvent ( QKeyEvent * event );
		
	private:
                QStringList updatedRowNames, updatedColumnNames;
	
                bool openedByUser;
                NodeGraphicsItem graphics1;
                NodeGraphicsItem graphics2;
		QDockWidget * dockWidget;
		QAction autoReverse;
                QAction * separator;
		
	};


}

#endif
