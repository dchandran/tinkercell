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
	/*! \brief This class provides the C API for the StoichiometryTool class
	\ingroup capi
	*/
	class StoichiometryTool_FToS : public QObject
	{
		Q_OBJECT
	signals:
		void getStoichiometry(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&);
		void setStoichiometry(QSemaphore*,QList<ItemHandle*>&,const DataTable<qreal>&);
		void getRates(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
		void setRates(QSemaphore*,QList<ItemHandle*>&,const QStringList&);
	public slots:
		Matrix getStoichiometry(ArrayOfItems);
		void setStoichiometry(ArrayOfItems,Matrix );
		ArrayOfStrings getRates(ArrayOfItems);
		void setRates(ArrayOfItems,ArrayOfStrings );
	};

	/*! \brief This class adds the Rates string data and Stoichiometry numerical data 
				to each connection that is inserted and provides the table widgets
				for editting those tables. This class also contains a useful function that
				parses an equation and places any undefined variables in the item's Numerical Attributes
				table
	\ingroup plugins
	*/
	class MY_EXPORT StoichiometryTool : public Tool
	{
		Q_OBJECT

	public:

		/*! \brief handles that are currectly selected and being displayed by the table table widgets*/
		QList<ConnectionHandle*> connectionHandles;
		/*! \brief constructor: intializes the table widgets*/
		StoichiometryTool();
		/*! \brief sets main window and connects to main window's signals
			\param MainWindow*/
		bool setMainWindow(MainWindow * main);
		/*! \brief preferred size for this window*/
		QSize sizeHint() const;
		/*! \brief hide the stoichiometry matrix, i.e. show only the rates*/
		void hideMatrix();
		/*! \brief show stoichiometry matrix along with rates*/
		void showMatrix();

		/*! \brief get the stoichiometry matrix for all the given items, combined
		\param QList<ItemHandle*> all the items for which the stoichiometry matrix will be generated
		\param QString naming scheme to use instead of A.B, e.g A_B*/
		static DataTable<qreal> getStoichiometry(const QList<ItemHandle*>&,const QString& replaceDot = QString("_"), bool includeFixed=false);
		/*! \brief set the stoichiometry matrix for all the given items, combined
		\param NetworkWindow* current window
		\param QList<ItemHandle*> all the items for which the stoichiometry matrix will be set
		\param DataTable combined stoichiometry matrix for all the items selected
		\param QString naming scheme to use instead of A.B, e.g A_B*/
		static void setStoichiometry(NetworkWindow *,QList<ItemHandle*>&,const DataTable<qreal>&,const QString& replaceDot = QString("_"));
		/*! \brief get the rates array for all the given items, combined
		\param QList<ItemHandle*> all the items for which the rates will be generated
		\param QString naming scheme to use instead of A.B, e.g A_B*/
		static QStringList getRates(const QList<ItemHandle*>&,const QString& replaceDot = QString("_"));
		/*! \brief set the rates for all the given items, combined
		\param NetworkWindow* current window
		\param QList<ItemHandle*> all the items for which the rates will be set
		\param DataTable combined rates array for all the items selected
		\param QString naming scheme to use instead of A.B, e.g A_B*/
		static void setRates(NetworkWindow*,QList<ItemHandle*>&,const QStringList&,const QString& replaceDot = QString("_"));

	public slots:
		/*! \brief this widget has been selected. Argument does nothing */
		void select(int i=0);
		/*! \brief this widget has been deselected. Argument does nothing */
		void deselect(int i=0);
		/*! \brief inserts the Rates and Stoichiometry tables for any new connection handle */
		void itemsInserted(NetworkWindow * , const QList<ItemHandle*>& handles);
		/*! \brief updates the internal connectionHandles list, which is used to display the rates and stoichiometry*/
		void itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
		/*! \brief connects to ModelSummaryTool and ConnectionSelectionTool*/
		void toolLoaded(Tool*);
		/*! \brief updates the widget display in case rates have been changed*/
		void historyUpdate(int);
		/*! \brief sets the C pointers for getRates, setRates, getStoichiometry, setStoic...etc*/
		void setupFunctionPointers(QLibrary*);
		/*! \brief when scene is closing, close this window and clear connectionHandles*/
		void sceneClosing(NetworkWindow * , bool * close);
		/*! \brief used by ModelSummaryTool to show only the relevant parameters*/
		void aboutToDisplayModel(const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations);
		/*! \brief display rates in the ModelSummaryTool widget*/
		void displayModel(QTabWidget& widgets, const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations);

	signals:
		/*! \brief set the middle region of a connection for reversible reactions*/
		void setMiddleBox(int,const QString&);

	private slots:
		/*! \brief used for the C API*/
		void getStoichiometrySlot(QSemaphore*, DataTable<qreal>*, const QList<ItemHandle*>&);
		/*! \brief used for the C API*/
		void getRatesSlot(QSemaphore *, QStringList*, const QList<ItemHandle*>&);
		/*! \brief used for the C API*/
		void setStoichiometrySlot(QSemaphore*, QList<ItemHandle*>&, const DataTable<qreal>&);
		/*! \brief used for the C API*/
		void setRatesSlot(QSemaphore*, QList<ItemHandle*>&, const QStringList&);

	protected:
		/*! \brief insert Rates and Stoichiometry tables
		\param ConnectionHandle * target handle*/
		void insertDataMatrix(ConnectionHandle * handle);
		/*! \brief widgets for displaying rates*/
		QTableWidget ratesTable;
		/*! \brief widgets for displaying stoichiometry*/
		QTableWidget matrixTable;
		/*! \brief delegate for the tables*/
		SpinBoxDelegate delegate;
		/*! \brief update table widget based on currently selected connections*/
		void updateTable();

	protected slots:
		/*! \brief add new reaction*/
		void addRow();
		/*! \brief remove a reaction*/
		void removeRow();
		/*! \brief evaluate values for all visible rate equations*/
		void eval();
		/*! \brief add an intermediate species (column in transpose of stoichiometry matrix)*/
		void addCol();
		/*! \brief add a participating species (column in transpose of stoichiometry matrix)*/
		void removeCol();
		/*! \brief set rate of the connection at the given index*/
		void setRate(int, int);
		/*! \brief set stoichiometry for the connection/species at the given index*/
		void setStoichiometry(int, int);
		/*! \brief make a biochemical reaction reversible by adding another row in the stoichiometry table*/
		void addReverseReaction();
		/*! \brief make a dimer*/
		void addDimer();

	protected:
		/*! \brief not used any longer*/
		QList<GraphicsScene*> scenePtr;
		/*! \brief not used any longer*/
		QList<DataTable<qreal>*> numericalDataPtr;
		/*! \brief not used any longer*/
		QList<DataTable<QString>*> textDataPtr;
		/*! \brief Group boxes for displaying the rates and stoichiometry tables*/
		QGroupBox * ratesBox, * matrixBox;

		/*! \brief used for the C API*/
		static StoichiometryTool_FToS fToS;
		/*! \brief connect to the the C API static class*/
		void connectCFuntions();
		/*! \brief used for the C API*/
		static Matrix _getStoichiometry(ArrayOfItems);
		/*! \brief used for the C API*/
		static void _setStoichiometry(ArrayOfItems ,Matrix );
		/*! \brief used for the C API*/
		static ArrayOfStrings _getRates(ArrayOfItems );
		/*! \brief used for the C API*/
		static void _setRates(ArrayOfItems ,ArrayOfStrings );
		/*! \brief delete and + for adding/removing rows/columns*/
		virtual void keyPressEvent ( QKeyEvent * event );

	private:
		/*! \brief This function is useful for any tool that needs to parse an equation and automatically
		add any undefined variables in the Numerical Attributes table (where parameters are usually stored)
		\param NetworkWindow the target network window (for symbols table)
		\param ItemHandle* the item handle that the equation belongs with
		\param QString& the equation; this variable can get modified if it contains bad characters*/
		static bool parseRateString(NetworkWindow*, ItemHandle *, QString&);
		
		/*! \brief used to keep track of updated headers*/
		QStringList updatedRowNames, updatedColumnNames;

		/*! \brief used to keep track of when to close the window automatically*/
		bool openedByUser;
		/*! \brief the icon to display for this tool*/
		NodeGraphicsItem graphics1;
		/*! \brief the icon to display for this tool*/
		NodeGraphicsItem graphics2;
		/*! \brief the dock widget for this widget*/
		QDockWidget * dockWidget;
		/*! \brief make a reaction reversible*/
		QAction * autoReverse, * autoDimer;
		/*! \brief separator for the action that makes a reaction reversible*/
		QAction * separator;

	};


}

#endif
