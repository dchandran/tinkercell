/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT


****************************************************************************/
#include <QLabel>
#include <QTabWidget>
#include <QDesktopServices>
#include "LPSolveInput.h"
#include "ConnectionGraphicsItem.h"
#include "ConsoleWindow.h"
#include "StoichiometryTool.h"
#include "DynamicLibraryMenu.h"

namespace Tinkercell
{

	LPSolveInputWindow::LPSolveInputWindow() : AbstractInputWindow(tr("Flux Balance Analysis Input Window"),0)
	{
		dockWidget = 0;
		objectivesTable.setEditTriggers ( QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed );
		constraintsTable.setEditTriggers ( QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed );

		//objectives table
		QGroupBox * groupBox1 = new QGroupBox(tr(" Objective function "));
		QVBoxLayout * layout1 = new QVBoxLayout;

		objectivesTable.setColumnCount(2);
		objectivesTable.setVerticalHeader(0);
		objectivesTable.setHorizontalHeaderLabels ( QStringList() << "flux" << "weights" );
		objectivesTable.setItemDelegate(&delegate1);

		QRadioButton * max = new QRadioButton(tr("Maximize"),groupBox1);
		QRadioButton * min = new QRadioButton(tr("Minimize"),groupBox1);
		layout1->addWidget(new QLabel(tr("Select one or more reactions on the screen."),groupBox1),0);
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
		QPushButton * logButton = new QPushButton(this);
		logButton->setIcon(QIcon(tr(":/images/new.png")));
		logButton->setToolTip(tr("Open log file"));
		connect(logButton,SIGNAL(pressed()),this,SLOT(showLogFile()));

		hlayout->addWidget(runButton,0);
		hlayout->addWidget(logButton,0);
		hlayout->addStretch(6);
		layout3->addLayout(hlayout,0);

		setLayout(layout3);  //final layout

		//connect buttons to functions
		connect(runButton,SIGNAL(released()),this,SLOT(exec()));
		connect(addButton,SIGNAL(released()),this,SLOT(addRow()));
		connect(removeButton,SIGNAL(released()),this,SLOT(removeRow()));

		connect(&objectivesTable,SIGNAL(cellChanged(int,int)),this,SLOT(objectivesTableChanged(int,int)));
		connect(&constraintsTable,SIGNAL(cellChanged(int,int)),this,SLOT(constraintsTableChanged(int,int)));

		dataTable.value(0,0) = 1.0;
		dataTable.value(0,1) = 0.0;
		max->setChecked(true);
		min->setChecked(false);

		//connect(min,SIGNAL(toggled()),max,SLOT(toggle()));
		connect(max,SIGNAL(toggled(bool)),this,SLOT(checkboxSelected(bool)));
	}

	void LPSolveInputWindow::exec()
	{
		setInput(dataTable);
		AbstractInputWindow::exec();
	}

	void LPSolveInputWindow::showLogFile()
	{
		QString tempDir = MainWindow::tempDir();
		QDesktopServices::openUrl(QUrl(tempDir + tr("/lpsolve.out")));
	}

	bool LPSolveInputWindow::setMainWindow(MainWindow * main)
	{
		AbstractInputWindow::setMainWindow(main);
        if (mainWindow)
		{
			cthread = new CThread(main,tr("plugins/c/liblpsolve"),false);
			cthread->setMatrixFunction("run");

			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
					this,SLOT(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(itemsInserted(NetworkHandle *, const QList<ItemHandle*>& )),
					this,SLOT(itemsInserted(NetworkHandle *, const QList<ItemHandle*>& )));

			connect(mainWindow,SIGNAL(itemsRemoved(NetworkHandle *,const QList<ItemHandle*>& )),
					this,SLOT(itemsInserted(NetworkHandle *, const QList<ItemHandle*>& )));

			connect(mainWindow,SIGNAL(dataChanged(const QList<ItemHandle*>&)),this,SLOT(dataChanged(const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			toolLoaded(0);

			return true;
		}

		return false;
	}

	void LPSolveInputWindow::toolLoaded(Tool *)
    {
        static bool alreadyConnected = false;

		if (alreadyConnected || !mainWindow) return;

		Tool * tool = 0;

        if (tool = mainWindow->tool(tr("Dynamic Library Menu")))
        {
            DynamicLibraryMenu * libMenu = static_cast<DynamicLibraryMenu*>(tool);
            if (libMenu && this->dockWidget)
            {
                alreadyConnected = true;
				QString appDir = QCoreApplication::applicationDirPath();

				QIcon icon(appDir + tr("/icons/lpsolve.png"));

				QToolButton * button = libMenu->addFunction(tr("Analysis"), tr("Flux Balance Analysis"), icon);
				button->setToolTip(tr("uses LPsolve linear programming C library"));

				QAction * action = libMenu->addMenuItem(tr("Analysis"),tr("Flux Balance Analysis"), icon);
				action->setToolTip(tr("uses LPsolve linear programming C library"));

				connect(button,SIGNAL(pressed()),dockWidget,SLOT(show()));
				connect(action,SIGNAL(triggered()),dockWidget,SLOT(show()));
            }
        }
    }

	void LPSolveInputWindow::loadConstraints()
	{
         NetworkHandle * network = currentNetwork();

		if (network && network->globalHandle()->hasNumericalData(tr("Flux Constraints")))
		{
			NumericalDataTable dataTable2 = network->globalHandle()->numericalDataTable(tr("Flux Constraints"));
			constraintsTable.setRowCount(dataTable2.rows());

			for (int i=0; i < dataTable2.rows(); ++i)
			{
				for (int j=2; j < dataTable2.columns(); ++j)
				{
					if (dataTable2.value(i,j) > 0)
					{
						constraintsTable.setItem(i,0,new QTableWidgetItem(dataTable2.columnName(j)));
						constraintsTable.setItem(i,2,new QTableWidgetItem(QString::number(dataTable2.value(i,j))));
						if (dataTable2.value(i,j) > 0 && dataTable2.value(i,j) < 3)
							constraintsTable.setItem(i,1,new QTableWidgetItem(delegate2.options[1][(int)dataTable2.value(i,j)]));
						break;
					}
				}
			}
		}

		update(false);
	}

	void LPSolveInputWindow::update(bool mustBeVisible)
	{
         NetworkHandle * network = currentNetwork();

		if (!network || !network->currentScene() || (mustBeVisible && !isVisible())) return;
		
		GraphicsScene * scene = network->currentScene();

		QList<ItemHandle*> handles = network->handles();
		NumericalDataTable N = StoichiometryTool::getStoichiometry(handles,tr("."));
		dataTable.resize(N.rows()+1,N.columns()+2);
		dataTable.setColumnName(0, tr("inequality"));
		dataTable.setColumnName(1, tr("constraint"));
		dataTable.setRowName(0,tr("objective"));

		QVector<qreal> vec(N.columns());
		for (int j=0; j < vec.size(); ++j)
		{
			vec[j] = dataTable.value(0,j+2);
		}

		int k;
		for (int j=0; j < N.columns(); ++j)
		{
			dataTable.setColumnName(j+2, N.columnName(j));
			k = targetFluxes.indexOf(N.columnName(j));
			if (k > -1 && k < vec.size())
				dataTable.value(0,j+2) = vec[k];
		}

		for (int i=0; i < N.rows(); ++i)
		{
			dataTable.setRowName(i+1, N.rowName(i));
			for (int j=0; j < N.columns(); ++j)
			{
				dataTable.value(i+1,j+2) = N.value(i,j);
			}
			dataTable.value(i+1,0) = 0.0;
			dataTable.value(i+1,1) = 0.0;
		}
		this->N = N.rows() + 1;

		delegate1.options[0] = N.columnNames();
		delegate2.options[0] = N.columnNames();

		constraintsTableChanged(0,0);

		//update table
        QList<ItemHandle*> selected = getHandle(scene->selected());

		QStringList rownames = dataTable.columnNames(); //flux names
		targetFluxes.clear();
		objectivesTable.clearContents();

		ItemHandle * handle;
		for (int i=0; i < selected.size(); ++i)
                        if (ConnectionHandle::cast(selected[i]))
			{
                                handle = selected[i];
				if (handle && rownames.contains(handle->fullName())) //get flux that are selected
					targetFluxes << handle->fullName();
			}

		objectivesTable.setRowCount(targetFluxes.size());

		for (int i=2; i < dataTable.columns(); ++i)
			if (targetFluxes.contains(dataTable.columnName(i)))
			{
				if (dataTable.value(0,i) == 0.0)
					dataTable.value(0,i) = 1.0;
			}
			else
				dataTable.value(0,i) = 0.0;

		disconnect(&objectivesTable,SIGNAL(cellChanged(int,int)),this,SLOT(objectivesTableChanged(int,int)));
		for (int i=0; i < targetFluxes.size(); ++i)
		{
			objectivesTable.setItem(i,0,new QTableWidgetItem( targetFluxes[i] ));
			objectivesTable.setItem(i,1,new QTableWidgetItem( QString::number(dataTable.value(0,targetFluxes[i])) ));
		}
		connect(&objectivesTable,SIGNAL(cellChanged(int,int)),this,SLOT(objectivesTableChanged(int,int)));

		objectivesTable.resizeColumnsToContents();
		constraintsTable.resizeColumnsToContents();

	}

	void LPSolveInputWindow::itemsInserted(NetworkHandle * network, const QList<ItemHandle*>&)
	{
		if (!network || !isVisible()) return;
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

		for (int i=2; i < dataTable.columns(); ++i)
			if (targetFluxes.contains(dataTable.columnName(i)))
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
		NumericalDataTable dataTable2;
		dataTable.resize(N + constraintsTable.rowCount(),dataTable.columns());
		dataTable2.resize(constraintsTable.rowCount(),dataTable.columns());
		dataTable2.setColumnNames(dataTable.columnNames());
		QString s;
		bool ok;
		double d;
		int k;
		for (int i=0; i < constraintsTable.rowCount(); ++i)
			if (constraintsTable.item(i,0) && constraintsTable.item(i,1) && constraintsTable.item(i,2))
			{
				s = constraintsTable.item(i,0)->text();
				k = dataTable.columnNames().indexOf(s);
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

                NetworkWindow * scene = currentWindow();
		if (scene)
			scene->network->globalHandle()->numericalDataTable(tr("Flux Constraints")) = dataTable2;
	}

	void LPSolveInputWindow::setVisible(bool b)
	{
		if (b)
			loadConstraints();
		QWidget::setVisible(b);
	}

}

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;
	Tinkercell::LPSolveInputWindow * tool = new Tinkercell::LPSolveInputWindow;
    if (main->tool(tool->name))
	{
		main->tool(tool->name)->show();
		if (main->tool(tool->name)->parentWidget())
		{
			main->tool(tool->name)->parentWidget()->show();
		}
		delete tool;
	}
	else
	{
		main->addTool(tool);
	}
}
