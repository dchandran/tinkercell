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
#include <QLabel>
#include <QToolButton>
#include <QButtonGroup>
#include <QTableView>
#include <QTabWidget>
#include <QRegExp>
#include <QTextEdit>
#include <QLineEdit>
#include <QDoubleSpinBox>

#include "NodeGraphicsItem.h"
#include "MainWindow.h"
#include "NetworkHandle.h"
#include "DataTable.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "Plot2DWidget.h"
#include "SpinBoxDelegate.h"

namespace Tinkercell
{
	/*! \brief This class provides the C API for the StoichiometryTool class
	\ingroup capi
	*/
	class StoichiometryTool_FToS : public QObject
	{
		Q_OBJECT
	signals:
		void getStoichiometry(QSemaphore*,NumericalDataTable*, QList<ItemHandle*>&);
		void setStoichiometry(QSemaphore*,QList<ItemHandle*>&,const NumericalDataTable&);
		void getRates(QSemaphore*,QStringList*, QList<ItemHandle*>&);
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
				parses an equation and places any undefined variables in the item's Parameters
				table
	\ingroup plugins
	*/
	class TINKERCELLEXPORT StoichiometryTool : public Tool
	{
		Q_OBJECT

	public:

		/*! \brief constructor: intializes the table widgets*/
		StoichiometryTool();
		/*! \brief sets main window and connects to main window's signals
			\param MainWindow*/
		bool setMainWindow(MainWindow * main);
		/*! \brief preferred size for this window*/
		QSize sizeHint() const;

		/*! \brief get the stoichiometry matrix for all the given items, combined
		\param QList<ItemHandle*> all the items for which the stoichiometry matrix will be generated
		\param QString naming scheme to use instead of A.B, e.g A_B*/
		static NumericalDataTable getStoichiometry(QList<ItemHandle*>&,const QString& replaceDot = QString("_"), bool includeFixed=false);
		/*! \brief set the stoichiometry matrix for all the given items, combined
		\param NetworkHandle* current window
		\param QList<ItemHandle*> all the items for which the stoichiometry matrix will be set
		\param DataTable combined stoichiometry matrix for all the items selected
		\param QString naming scheme to use instead of A.B, e.g A_B*/
		static void setStoichiometry(NetworkHandle *,QList<ItemHandle*>&,const NumericalDataTable&,const QString& replaceDot = QString("_"));
		/*! \brief get the rates array for all the given items, combined
		\param QList<ItemHandle*> all the items for which the rates will be generated
		\param QString naming scheme to use instead of A.B, e.g A_B*/
		static QStringList getRates(QList<ItemHandle*>&,const QString& replaceDot = QString("_"));
		/*! \brief set the rates for all the given items, combined
		\param NetworkHandle* current window
		\param QList<ItemHandle*> all the items for which the rates will be set
		\param DataTable combined rates array for all the items selected
		\param QString naming scheme to use instead of A.B, e.g A_B*/
		static void setRates(NetworkHandle*,QList<ItemHandle*>&,const QStringList&,const QString& replaceDot = QString("_"));

	public slots:
		/*! \brief this widget has been selected. Argument does nothing */
		void select(int i=0);
		/*! \brief this widget has been deselected. Argument does nothing */
		void deselect(int i=0);
		/*! \brief inserts the Rates and Stoichiometry tables for any new connection handle */
		void itemsInserted(NetworkHandle * , const QList<ItemHandle*>& );
		/*! \brief updates the internal connectionHandles list, which is used to display the rates and stoichiometry*/
		void itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
		/*! \brief connects to ModelSummaryTool and ConnectionSelectionTool*/
		void toolLoaded(Tool*);
		/*! \brief updates the widget display in case rates have been changed*/
		void historyUpdate(int);
		/*! \brief sets the C pointers for getRates, setRates, getStoichiometry, setStoic...etc*/
		void setupFunctionPointers(QLibrary*);
		/*!\brief remove items from stoichiometry table in each connection*/
		void itemsAboutToBeRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& item, QList<ItemHandle*>& handles, QList<QUndoCommand*>&);
		/*! \brief used to update rate equation for enzyme catalyzed reactions
		* \param NetworkHandle* network where the event took place
		* \param QList<ItemHandle*>& child items
		* \param QList<ItemFamily*>& old families
		* \return void*/
		void handleFamilyChanged(NetworkHandle * network, const QList<ItemHandle*>&, const QList<ItemFamily*>&);
	signals:
		/*! \brief set the middle region of a connection for reversible reactions*/
		void setMiddleBox(int,const QString&);
		/*! \brief indicate the some data have changed*/
		void dataChanged(const QList<ItemHandle*>&);

	private slots:
		/*! \brief used for the C API*/
		void getStoichiometrySlot(QSemaphore*, NumericalDataTable*, QList<ItemHandle*>&);
		/*! \brief used for the C API*/
		void getRatesSlot(QSemaphore *, QStringList*, QList<ItemHandle*>&);
		/*! \brief used for the C API*/
		void setStoichiometrySlot(QSemaphore*, QList<ItemHandle*>&, const NumericalDataTable&);
		/*! \brief used for the C API*/
		void setRatesSlot(QSemaphore*, QList<ItemHandle*>&, const QStringList&);

	private slots:
		void aboutToDisplayModel(const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations);
		void displayModel(QTabWidget& widgets, const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations);

		/*! \brief insert Rates and Stoichiometry tables
		\param ConnectionHandle * target handle*/
		void insertDataMatrix(ConnectionHandle * handle);	
		/*! \brief make a biochemical reaction reversible by adding another row in the stoichiometry table*/
		void addReverseReaction();
		/*! \brief make a dimer*/
		void addDimer();
		void xaxisChanged(const QString&);
		void startStopChanged(double);
		void rateEquationChanged();
		void stoichiometryChanged();

	protected:
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

	private:
		/*! \brief handles that are currectly selected and being displayed by the table table widgets*/
		ConnectionHandle * connectionHandle;
		/*! \brief This function is useful for any tool that needs to parse an equation and automatically
		add any undefined variables in the Parameters table (where parameters are usually stored)
		\param NetworkHandle the target network window (for symbols table)
		\param ItemHandle* the item handle that the equation belongs with
		\param QString& the equation; this variable can get modified if it contains bad characters*/
		static bool parseRateString(NetworkHandle*, ItemHandle *, QString&);
		
		/*! \brief make a reaction reversible*/
		QAction * autoReverse, * autoDimer;
		/*! \brief separator for the action that makes a reaction reversible*/
		QAction * separator;

		Plot2DWidget * plotWidget;		
		QLineEdit * plotLineEdit;
		QComboBox * plotVar;
		QDoubleSpinBox * startPlot, * endPlot;
		QWidget * ratePlotWidget, * stoichiometryWidget;
		QList<QLineEdit*> reactantCoeffs, productCoeffs;
		QList<QLabel*> reactantNames, productNames, plusSigns;
		QHBoxLayout * stoichiometryLayout;
		QString currentVar;
		QStringList varslist;
		
		bool replot(QString& rate, const QString& xaxis, qreal from, qreal to, QStringList& vars);
		bool updatePlotWidget();
		bool updateStoichiometryWidget();
		void updateWidgets();

	};


}

#endif

