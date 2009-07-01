/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT


****************************************************************************/

#include <QTabWidget>
#include "LPSolveInput.h"
#include "ConnectionGraphicsItem.h"
#include "OutputWindow.h"
#include "StoichiometryTool.h"

namespace Tinkercell
{

	LPSolveInputWindow::LPSolveInputWindow() : AbstractInputWindow(tr("Flux Balance Analysis Input Window"),tr("dlls/lpsolve"),tr("run"))
	{
		dockWidget = 0;
		objectivesTable.setEditTriggers ( QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed );
		constraintsTable.setEditTriggers ( QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed );

		//objectives table
		QGroupBox * groupBox1 = new QGroupBox(tr(" Objective Flux Ratios "));
		QVBoxLayout * layout1 = new QVBoxLayout;

		objectivesTable.setColumnCount(2);
		objectivesTable.setVerticalHeader(0);
		objectivesTable.setHorizontalHeaderLabels ( QStringList() << "flux" << "target ratio" );
		objectivesTable.setItemDelegate(&delegate1);

		QRadioButton * max = new QRadioButton(tr("Maximize"),groupBox1);
		QRadioButton * min = new QRadioButton(tr("Minimize"),groupBox1);
		layout1->addWidget(max,0);
		layout1->addWidget(min,0);

		layout1->addWidget(&objectivesTable,3);
		groupBox1->setLayout(layout1);

		//constraints table
		QGroupBox * groupBox2 = new QGroupBox(tr(" Additional Constraints "));
		QVBoxLayout * layout2 = new QVBoxLayout;

		constraintsTable.setVerticalHeader(0);
		constraintsTable.setColumnCount(3);
		constraintsTable.setHorizontalHeaderLabels ( QStringList() << "flux" << "operation" << "bound" );
		constraintsTable.setItemDelegate(&delegate2);
		constraintsTable.setShowGrid ( false );

		delegate1.options << (QStringList());
		delegate2.options << (QStringList()) << (QStringList() << "=" << "<=" << ">=");


		layout2->addWidget(&constraintsTable,6);
		QToolButton * addButton = new QToolButton(this);
		addButton->setIcon(QIcon(":/images/plus.png"));
		addButton->setToolTip(tr("add new constraint"));

		QToolButton * removeButton = new QToolButton(this);
		removeButton->setIcon(QIcon(":/images/minus.png"));
		removeButton->setToolTip(tr("remove new constraint"));

		QHBoxLayout * hlayout = new QHBoxLayout;  //layout for just the two buttons
		hlayout->addStretch(6);
		hlayout->addWidget(addButton,0);
		hlayout->addWidget(removeButton,0);
		layout2->addLayout(hlayout,0);

		groupBox2->setLayout(layout2);

		QVBoxLayout * layout3 = new QVBoxLayout;
		QTabWidget * tabWidget = new QTabWidget;

		tabWidget->addTab(groupBox1,tr("Objective"));
		tabWidget->addTab(groupBox2,tr("Constraints"));

		layout3->addWidget(tabWidget,4);
		hlayout = new QHBoxLayout;  //layout for just the two buttons
		QPushButton * runButton = new QPushButton(this);
		runButton->setIcon(QIcon(":/images/play.png"));
		runButton->setToolTip(tr("Optimize using linear programming"));
		hlayout->addWidget(runButton,0);
		hlayout->addStretch(6);
		layout3->addLayout(hlayout,0);

		setLayout(layout3);  //final layout

		//connect buttons to functions
		connect(runButton,SIGNAL(released()),this,SLOT(exec()));
		connect(addButton,SIGNAL(released()),this,SLOT(addRow()));
		connect(removeButton,SIGNAL(released()),this,SLOT(removeRow()));
		connect(max,SIGNAL(toggled(bool)),this,SLOT(checkboxSelected(bool)));
		connect(&objectivesTable,SIGNAL(cellChanged(int,int)),this,SLOT(objectivesTableChanged(int,int)));
		connect(&constraintsTable,SIGNAL(cellChanged(int,int)),this,SLOT(constraintsTableChanged(int,int)));

		dataTable.value(0,0) = 1.0;
		dataTable.value(0,1) = 0.0;
		max->setChecked(true);
		min->setChecked(false);
	}

	bool LPSolveInputWindow::setMainWindow(MainWindow * main)
	{
		AbstractInputWindow::setMainWindow(main);
		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
					this,SLOT(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>& )),
					this,SLOT(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>& )));

			connect(mainWindow,SIGNAL(itemsRemoved(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>& )),
					this,SLOT(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>& )));

			connect(mainWindow,SIGNAL(dataChanged(const QList<ItemHandle*>&)),this,SLOT(dataChanged(const QList<ItemHandle*>&)));

			return true;
		}

		return false;
	}

	void LPSolveInputWindow::loadConstraints()
	{
		GraphicsScene * scene = mainWindow->currentScene();

// 		if (scene && scene->symbolsTable.modelItem.data && scene->symbolsTable.modelItem.data->numericalData.contains(tr("Flux Constraints")))
// 		{
// 			DataTable<qreal> dataTable2 = scene->symbolsTable.modelItem.data->numericalData[tr("Flux Constraints")];
// 			constraintsTable.setRowCount(dataTable2.rows());
//
// 			for (int i=0; i < dataTable2.rows(); ++i)
// 			{
// 				for (int j=2; j < dataTable2.cols(); ++j)
// 				{
// 					if (dataTable2.value(i,j) > 0)
// 					{
// 						constraintsTable.setItem(i,0,new QTableWidgetItem(dataTable2.colName(j)));
// 						constraintsTable.setItem(i,2,new QTableWidgetItem(QString::number(dataTable2.value(i,j))));
// 						if (dataTable2.value(i,j) > 0 && dataTable2.value(i,j) < 3)
// 							constraintsTable.setItem(i,1,new QTableWidgetItem(delegate2.options[1][(int)dataTable2.value(i,j)]));
// 						break;
// 					}
// 				}
// 			}
// 		}

		update(false);
	}

	void LPSolveInputWindow::update(bool mustBeVisible)
	{
		GraphicsScene * scene = currentScene();

		if (!scene || (mustBeVisible && !isVisible())) return;

// 		DataTable<qreal> N = StoichiometryTool::getStoichiometry(scene->symbolsTable.handlesFullName.values(),tr("."));
// 		dataTable.resize(N.rows()+1,N.cols()+2);
// 		dataTable.colName(0) = tr("inequality");
// 		dataTable.colName(1) = tr("constraint");
// 		dataTable.rowName(0) = tr("objective");
//
// 		QVector<qreal> vec(N.cols());
// 		for (int j=0; j < vec.size(); ++j)
// 		{
// 			vec[j] = dataTable.value(0,j+2);
// 		}
//
// 		int k;
// 		for (int j=0; j < N.cols(); ++j)
// 		{
// 			dataTable.colName(j+2) = N.colName(j);
// 			k = targetFluxes.indexOf(N.colName(j));
// 			if (k > -1 && k < vec.size())
// 				dataTable.value(0,j+2) = vec[k];
// 		}
//
// 		for (int i=0; i < N.rows(); ++i)
// 		{
// 			dataTable.rowName(i+1) = N.rowName(i);
// 			for (int j=0; j < N.cols(); ++j)
// 			{
// 				dataTable.value(i+1,j+2) = N.value(i,j);
// 			}
// 			dataTable.value(i+1,0) = 0.0;
// 			dataTable.value(i+1,1) = 0.0;
// 		}
// 		this->N = N.rows() + 1;
//
// 		delegate1.options[0] = N.getColNames();
// 		delegate2.options[0] = N.getColNames();
//
// 		constraintsTableChanged(0,0);
//
// 		//update table
// 		QList<QGraphicsItem*>& selected = scene->selected();
//
// 		QStringList rownames = dataTable.getColNames(); //flux names
// 		targetFluxes.clear();
// 		objectivesTable.clearContents();
//
// 		ItemHandle * handle;
// 		for (int i=0; i < selected.size(); ++i)
// 			if (qgraphicsitem_cast<ConnectionGraphicsItem*>(selected[i]))
// 			{
// 				handle = getHandle(selected[i]);
// 				if (handle && rownames.contains(handle->fullName())) //get flux that are selected
// 					targetFluxes << handle->fullName();
// 			}
//
// 		objectivesTable.setRowCount(targetFluxes.size());
//
// 		for (int i=2; i < dataTable.cols(); ++i)
// 			if (targetFluxes.contains(dataTable.colName(i)))
// 			{
// 				if (dataTable.value(0,i) == 0.0)
// 					dataTable.value(0,i) = 1.0;
// 			}
// 			else
// 				dataTable.value(0,i) = 0.0;
//
// 		disconnect(&objectivesTable,SIGNAL(cellChanged(int,int)),this,SLOT(objectivesTableChanged(int,int)));
// 		for (int i=0; i < targetFluxes.size(); ++i)
// 		{
// 			objectivesTable.setItem(i,0,new QTableWidgetItem( targetFluxes[i] ));
// 			objectivesTable.setItem(i,1,new QTableWidgetItem( QString::number(dataTable.value(0,targetFluxes[i])) ));
// 		}
// 		connect(&objectivesTable,SIGNAL(cellChanged(int,int)),this,SLOT(objectivesTableChanged(int,int)));
//
// 		objectivesTable.resizeColumnsToContents();
// 		constraintsTable.resizeColumnsToContents();

	}

	void LPSolveInputWindow::itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)
	{
		if (!scene || !isVisible()) return;
		update();
	}

	void LPSolveInputWindow::dataChanged(const QList<ItemHandle*>&)
	{
		update();
	}

	void LPSolveInputWindow::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& , QPointF , Qt::KeyboardModifiers )
	{
		if (!scene || !isVisible()) return;
		update();
	}

	void LPSolveInputWindow::checkboxSelected(bool checked)
	{
		if (checked)
			dataTable.value(0,0) = 1.0;
		else
			dataTable.value(0,0) = 0.0;
	}

	//add constraint
	void LPSolveInputWindow::addRow()
	{
		constraintsTable.setRowCount(constraintsTable.rowCount() + 1);
		if (delegate2.options[0].size() > 0)
			constraintsTable.setItem(constraintsTable.rowCount()-1,0,new QTableWidgetItem(delegate2.options[0][0]));
		constraintsTable.setItem(constraintsTable.rowCount()-1,1,new QTableWidgetItem(">="));
		constraintsTable.setItem(constraintsTable.rowCount()-1,2,new QTableWidgetItem("0.0"));
	}

	void LPSolveInputWindow::removeRow()
	{
		if (constraintsTable.rowCount() > 0)
		{
			constraintsTable.setRowCount(constraintsTable.rowCount() - 1);
			constraintsTable.setItem(constraintsTable.rowCount()-1, 0, new QTableWidgetItem(""));
		}
	}

	void LPSolveInputWindow::objectivesTableChanged(int i,int j)
	{
		if (objectivesTable.columnCount() < 2) return;
		bool ok;

		for (int i=2; i < dataTable.cols(); ++i)
			if (targetFluxes.contains(dataTable.colName(i)))
			{
				if (dataTable.value(0,i) == 0.0)
					dataTable.value(0,i) = 1.0;
			}
			else
				dataTable.value(0,i) = 0.0;

		for (int i=0; i < objectivesTable.rowCount(); ++i)
			if (objectivesTable.item(i,0) && objectivesTable.item(i,1))
			{
				double d = objectivesTable.item(i,1)->text().toDouble(&ok);
				QString s = objectivesTable.item(i,0)->text();

				if (ok)
					dataTable.value(0,s) = d;
				else
					dataTable.value(0,s) = 0.0;
			}
	}

	void LPSolveInputWindow::constraintsTableChanged(int i,int j)
	{
		DataTable<qreal> dataTable2;
		dataTable.resize(N + constraintsTable.rowCount(),dataTable.cols());
		dataTable2.resize(constraintsTable.rowCount(),dataTable.cols());
		dataTable2.setColNames(dataTable.colNames());
		QString s;
		bool ok;
		double d;
		int k;
		for (int i=0; i < constraintsTable.rowCount(); ++i)
			if (constraintsTable.item(i,0) && constraintsTable.item(i,1) && constraintsTable.item(i,2))
			{
				s = constraintsTable.item(i,0)->text();
				k = dataTable.getColNames().indexOf(s);
				if (k > -1)
				{
					dataTable2.value(i,k) = dataTable.value(i+N,k) = 1.0;

					s = constraintsTable.item(i,1)->text();
					dataTable.value(i,0) = dataTable.value(i+N,0) = delegate2.options[1].indexOf(s);

					d = constraintsTable.item(i,2)->text().toDouble(&ok);
					if (!ok) d = 0.0;
					dataTable2.value(i,1) = dataTable.value(i+N,1) = d;
				}
			}

		GraphicsScene * scene = currentScene();
// 		if (scene)
// 			scene->symbolsTable.modelItem.data->numericalData[tr("Flux Constraints")] = dataTable2;
	}

	void LPSolveInputWindow::setVisible(bool b)
	{
		if (b)
			loadConstraints();
		QWidget::setVisible(b);
	}

}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;
	Tinkercell::LPSolveInputWindow * tool = new Tinkercell::LPSolveInputWindow;
// 	if (main->tools.contains(tool->name))
// 	{
// 		main->tools[tool->name]->show();
// 		if (main->tools[tool->name]->parentWidget())
// 		{
// 			main->tools[tool->name]->parentWidget()->show();
// 		}
// 		delete tool;
// 	}
// 	else
// 	{
// 		tool->setMainWindow(main);
// 		//tool->itemsSelected(main->currentScene(), QList<QGraphicsItem*>() , QPointF() , 0 );
// 	}
}
