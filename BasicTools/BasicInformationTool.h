/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This class adds the "attributes" data to each item in Tinkercell.
Two types of attributes are added -- "Numerical Attributes" and "Text Attributes".
Attributes are essentially a <name,value> pair that are used to characterize an item.

The BasicInformationTool also comes with two GraphicalTools, one for text attributes and one
for numerical attributes. The buttons are drawn as NodeGraphicsItems using the datasheet.xml and
textsheet.xml files that define the NodeGraphicsItems.

****************************************************************************/

#ifndef TINKERCELL_BASICINFORMATIONTOOL_H
#define TINKERCELL_BASICINFORMATIONTOOL_H

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
#include <QTableView>

#include "NodeGraphicsItem.h"
#include "DataTable.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "MainWindow.h"
#include "GraphicsScene.h"
#include "NetworkWindow.h"
#include "SpinBoxDelegate.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	/*!
	\brief This class provides the C API for the ConnectionInsertion class
	\ingroup capi
	*/
	class BasicInformationTool_FToS : public QObject
	{
		Q_OBJECT
signals:
		void getInitialValues(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&);
		void getParameters(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&);
		void getFixedVars(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&);
		void getFixedAndParameters(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&);
		void getTextData(QSemaphore*,QString*,ItemHandle*,const QString&);
		void getNumericalData(QSemaphore*,qreal*,ItemHandle*,const QString&);
		void setTextData(QSemaphore*,ItemHandle*,const QString&,const QString&);
		void setNumericalData(QSemaphore*,ItemHandle*,const QString&,qreal);
		void getParametersNamed(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&,const QStringList&);
		void getParametersExcept(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&,const QStringList&);
		void getAllTextDataNamed(QSemaphore*,QStringList*,const QList<ItemHandle*>&,const QStringList&);
		public slots:
			Matrix getInitialValues(Array );
			Matrix getParameters(Array );
			Matrix getFixedVars(Array);
			Matrix getFixedAndParameters(Array);
			char* getTextData(OBJ ,const char* );
			double getNumericalData(OBJ ,const char* );
			Matrix getParametersNamed(Array, char**);
			Matrix getParametersExcept(Array, char**);
			char** getAllTextDataNamed(Array, char**);
			void setTextData(OBJ ,const char* ,const char* );
			void setNumericalData(OBJ ,const char* ,double );
	};

	/*!
	\brief Obtains the list of attributes from the nodes and connections trees and
	ensures than any inserted item will have the attributes listed for that familily.
	Also insert data for ``initial value" and ``fixed".
	\ingroup plugins
	*/
	class MY_EXPORT BasicInformationTool : public Tool
	{
		Q_OBJECT;

	public:
		QList<ItemHandle*> itemHandles;
		BasicInformationTool(const QString& typ = QString("both"));
		bool setMainWindow(MainWindow * main);
		QSize sizeHint() const;
		static DataTable<qreal> getParameters(const QList<QGraphicsItem*>& items, const QStringList& must = QStringList(), const QStringList& exclude = QStringList(), const QString& sep = QString("_"));
		static DataTable<qreal> getParameters(const QList<ItemHandle*>& handles, const QStringList& must = QStringList(),  const QStringList& exclude = QStringList(), const QString& sep = QString("_"));
		static DataTable<QString> getTextData(const QList<ItemHandle*>& handles, const QStringList& must = QStringList(),  const QStringList& exclude = QStringList(), const QString& sep = QString("_"));
		enum Type { numerical, text, both };
		Type type;

signals:


		public slots:
			void select(int);
			void deselect(int);

			void setValue(int i, int j);
			void itemsInserted(NetworkWindow* , const QList<ItemHandle*>& handles);
			void itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers);
			void pluginLoaded(Tool*);
			void addAttribute();
			void removeSelectedAttributes();
			void historyUpdate(int);
			void setupFunctionPointers( QLibrary * );
			void windowClosing(NetworkWindow * scene, bool * close);

			void aboutToDisplayModel(const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations);
			void displayModel(QTabWidget& widgets, const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations);

			private slots:
				void getInitialValues(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&);
				void getParameters(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&);
				void getFixedVars(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&);
				void getFixedAndParameters(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&);
				void getTextData(QSemaphore*,QString*,ItemHandle*,const QString&);
				void getNumericalData(QSemaphore*,qreal*,ItemHandle*,const QString&);
				void setTextData(QSemaphore*,ItemHandle*,const QString&,const QString&);
				void setNumericalData(QSemaphore*,ItemHandle*,const QString&,qreal);
				void getParametersNamed(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&,const QStringList&);
				void getParametersExcept(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&,const QStringList&);
				void getAllTextDataNamed(QSemaphore*,QStringList*,const QList<ItemHandle*>&,const QStringList&);

	protected:
		void insertDataMatrix(ItemHandle * handle);
		QTableWidget tableWidget;
		QList< QPair<ItemHandle*,int> > tableItems;
		QGroupBox * groupBox;

		void updateTable();
		static BasicInformationTool_FToS fToS;
		void connectTCFunctions();
		static Matrix _getInitialValues(Array );
		static Matrix _getParameters(Array );
		static Matrix _getFixedVars(Array);
		static Matrix _getFixedAndParameters(Array);
		static char* _getTextData(OBJ ,const char* );
		static double _getNumericalData(OBJ ,const char* );
		static Matrix _getParametersNamed(Array, char**);
		static Matrix _getParametersExcept(Array, char**);
		static char** _getAllTextDataNamed(Array, char**);
		static void _setTextData(OBJ ,const char* ,const char* );
		static void _setNumericalData(OBJ ,const char* ,double );

		virtual void keyPressEvent ( QKeyEvent * event );

	private:

		bool openedByUser;
		NodeGraphicsItem item;
		TextComboDoubleDelegate delegate;
		QDockWidget * dockWidget;
	};


}

#endif
