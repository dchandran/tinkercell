/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This class adds the "attributes" data to each item in Tinkercell.
Two types of attributes are added -- "Parameters" and "Text Attributes".
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
#include <QTableWidget>
#include <QList>
#include <QStringList>

#include "NodeGraphicsItem.h"
#include "DataTable.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "MainWindow.h"
#include "GraphicsScene.h"
#include "NetworkHandle.h"
#include "SpinBoxDelegate.h"

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
		void setInitialValues(QSemaphore*,const QList<ItemHandle*>&,const DataTable<qreal>&);
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
		tc_matrix getInitialValues(tc_items );
		void setInitialValues(tc_items,tc_matrix);
		tc_matrix getParameters(tc_items );
		tc_matrix getFixedVars(tc_items);
		tc_matrix getFixedAndParameters(tc_items);
		char* getTextData(long ,const char* );
		double getNumericalData(long ,const char* );
		tc_matrix getParametersNamed(tc_items, tc_strings);
		tc_matrix getParametersExcept(tc_items, tc_strings);
		tc_strings getAllTextDataNamed(tc_items, tc_strings);
		void setTextData(long ,const char* ,const char* );
		void setNumericalData(long ,const char* ,double );
	};

	/*!
	\brief Obtains the list of attributes from the nodes and connections trees and
	ensures than any inserted item will have the attributes listed for that familily.
	Also insert data for ``initial value" and ``fixed".
	\ingroup plugins
	*/
	class TINKERCELLEXPORT BasicInformationTool : public Tool
	{
		Q_OBJECT;

	public:
		QList<ItemHandle*> itemHandles;
		BasicInformationTool(const QString& typ = QString("both"));
		bool setMainWindow(MainWindow * main);
		QSize sizeHint() const;
		static DataTable<qreal> getParameters(const QList<QGraphicsItem*>& items, const QStringList& must = QStringList(), const QStringList& exclude = QStringList(), const QString& sep = QString("_"));
		static DataTable<qreal> getParameters(const QList<ItemHandle*>& handles, const QStringList& must = QStringList(),  const QStringList& exclude = QStringList(), const QString& sep = QString("_"));
		static DataTable<qreal> getUsedParameters(QList<ItemHandle*>& handles, const QString& replaceDot=QString("_"));
		static DataTable<QString> getTextData(const QList<ItemHandle*>& handles, const QStringList& must = QStringList(),  const QStringList& exclude = QStringList(), const QString& sep = QString("_"));
		enum Type { numerical, text, both };
		Type type;

	signals:
		/*! \brief indicate the some data have changed*/
		void dataChanged(const QList<ItemHandle*>&);

	public slots:
		void select(int i=0);
		void deselect(int i=0);

		void setValue(int i, int j);
		void itemsInserted(NetworkHandle * , const QList<ItemHandle*>& );
		void itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers);
		void pluginLoaded(Tool*);
		void addAttribute();
		void removeSelectedAttributes();
		void historyUpdate(int);
		void setupFunctionPointers( QLibrary * );
		void windowClosing(NetworkHandle * scene, bool * close);

		void aboutToDisplayModel(const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations);
		void displayModel(QTabWidget& widgets, const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations);

	private slots:
		void getInitialValues(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&);
		void setInitialValues(QSemaphore*,const QList<ItemHandle*>&,const DataTable<qreal>&);
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

	private:

		void insertDataMatrix(ItemHandle * handle);
		QStringList ignoredVarNames;
		QTableWidget tableWidget;
		QList< QPair<ItemHandle*,int> > tableItems;
		QGroupBox * groupBox;

		void updateTable();
		static BasicInformationTool_FToS fToS;
		void connectTCFunctions();
		static tc_matrix _getInitialValues(tc_items );
		static void _setInitialValues(tc_items,tc_matrix);
		static tc_matrix _getParameters(tc_items );
		static tc_matrix _getFixedVars(tc_items);
		static tc_matrix _getFixedAndParameters(tc_items);
		static char* _getTextData(long ,const char* );
		static double _getNumericalData(long ,const char* );
		static tc_matrix _getParametersNamed(tc_items, tc_strings);
		static tc_matrix _getParametersExcept(tc_items, tc_strings);
		static tc_strings _getAllTextDataNamed(tc_items, tc_strings);
		static void _setTextData(long ,const char* ,const char* );
		static void _setNumericalData(long ,const char* ,double );

		virtual void keyPressEvent ( QKeyEvent * event );

		bool openedByUser;
		NodeGraphicsItem item;
		TextComboDoubleDelegate delegate;
		QDockWidget * dockWidget;
	
	public:
		static QHash<QString,double> initialValues;
		void loadInitialValues();
	};


}

#endif
