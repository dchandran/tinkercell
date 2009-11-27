/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.

****************************************************************************/

#include <math.h>
#include <QGroupBox>
#include <QLabel>
#include <QRegExp>
#include <QCheckBox>
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NetworkWindow.h"
#include "ConsoleWindow.h"
#include "EquationParser.h"
#include "PlotTool.h"
#include "Plot2DWidget.h"
#include "Plot3DWidget.h"
#include "qwt_scale_engine.h"
#include "muParser.h"
#include "muParserInt.h"


namespace Tinkercell
{

	/***********************************
		Plot Tool
	************************************/

	void PlotTool::toolAboutToBeLoaded( Tool * tool, bool * b)
    {
        if (tool && tool != this && tool->name.toLower().contains(tr("plot")))
            (*b) = false;
    }

	PlotTool::PlotTool() : Tool(tr("Plot Tool")), actionGroup(this)
	{
		otherToolBar = 0;
		dockWidget = 0;
		setPalette(QPalette(QColor(255,255,255,255)));
		setAutoFillBackground(true);

		connect(&actionGroup,SIGNAL(triggered(QAction*)),this,SLOT(actionTriggered(QAction*)));

		//setup main window and toolbar
		QVBoxLayout * layout = new QVBoxLayout;
		layout->setContentsMargins(0,0,0,0);

		multiplePlotsArea = new QMdiArea(this);
		multiplePlotsArea->setViewMode(QMdiArea::SubWindowView);
		connect(multiplePlotsArea,SIGNAL(subWindowActivated(QMdiSubWindow*)),this,SLOT(subWindowActivated(QMdiSubWindow*)));

		window = new QMainWindow;
		window->setCentralWidget(multiplePlotsArea);
		toolBar.setWindowTitle(tr("Plot toolbar"));

		exportMenu = new QMenu(tr("Export"),&toolBar);
		QToolButton * exportButton = new QToolButton(&toolBar);
		exportButton->setIcon(QIcon(":/images/export.png"));
		exportButton->setMenu(exportMenu);
		exportButton->setText(tr("E&xport current graph"));
		exportButton->setPopupMode(QToolButton::MenuButtonPopup);
		exportButton->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );

		toolBar.addWidget(exportButton);
		toolBar.addWidget(keepOldPlots = new QCheckBox(tr("K&eep Previous Graphs"),&toolBar));
		toolBar.addWidget(holdCurrentPlot = new QCheckBox(tr("A&ppend To Current Graph"),&toolBar));
		keepOldPlots->setChecked(false);
		holdCurrentPlot->setChecked(false);

		window->addToolBar(Qt::TopToolBarArea,&toolBar);
		layout->addWidget(window);
		setLayout(layout);

		/////////// make function plotting widget ////////////

		QHBoxLayout * layout1 = new QHBoxLayout;
		QVBoxLayout * layout2 = new QVBoxLayout;
		QHBoxLayout * layout3 = new QHBoxLayout;

		//function text editor
		QGroupBox * textEditGroup = new QGroupBox(tr(" Define functions here "));
		layout1->addWidget(&functionsTextEdit);
		textEditGroup->setLayout(layout1);

		//start, end, xaxis
		spinBox1.setPrefix(tr("start:   "));
		spinBox1.setRange(-1E30,1E30);
		spinBox1.setDecimals(3);

		spinBox2.setValue(10.0);
		spinBox2.setPrefix(tr("end:  "));
		spinBox2.setRange(-1E30,1E30);
		spinBox2.setDecimals(3);

		spinBox3.setValue(100);
		spinBox3.setRange(5,1000);
		spinBox3.setPrefix(tr("points:  "));

		QLabel * label = new QLabel(tr("x-axis: "));
		layout3->addWidget(label);
		layout3->addWidget(&xaxisLine);

		layout2->addLayout(layout3);
		layout2->addWidget(&spinBox1);
		layout2->addWidget(&spinBox2);
		layout2->addWidget(&spinBox3);

		//graph button

		QPushButton * button = new QPushButton(tr("Graph"));
		connect(button,SIGNAL(pressed()),this,SLOT(plotFormula()));
		layout2->addWidget(button);

		//put it all together

		QHBoxLayout * layout4 = new QHBoxLayout;
		layout4->addWidget(textEditGroup);
		layout4->addLayout(layout2);

		QWidget * widget = new QWidget;
		layout4->setContentsMargins(0,0,0,0);
		widget->setLayout(layout4);

		functionsWidgetDock = addDockWidget(tr("Plot functions"),widget);
		functionsWidgetDock->hide();

		///////////// done with function plotting widget ///////////////

		//setup toolbar

		addExportOption(QIcon(tr(":/images/save.png")),tr("Image"),tr("Save image"));
		addExportOption(QIcon(tr(":/images/camera.png")),tr("Snapshot"),tr("Copy image to clipboard"));
		addExportOption(QIcon(tr(":/images/new.png")),tr("Text"),tr("Show the data table"));
		addExportOption(QIcon(tr(":/images/latex.png")),tr("LaTeX"),tr("Export data to LaTeX"));
		addExportOption(QIcon(tr(":/images/copy.png")),tr("Clipboard"),tr("Copy data to clipboard"));
		QAction * action = functionsWidgetDock->toggleViewAction();
		action->setIcon(QIcon(tr(":/images/function.png")));
		action->setToolTip(tr("Plot one or more formulas"));
		toolBar.addAction(action);

		//C interface
		connectTCFunctions();
	}

	QSize PlotTool::sizeHint() const
	{
	    return QSize(600, 500);
	}

	bool PlotTool::setMainWindow(MainWindow * TinkercellWindow)
	{
		Tool::setMainWindow(TinkercellWindow);

		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			connect(mainWindow, SIGNAL(toolAboutToBeLoaded( Tool * , bool * )),
                    this, SLOT(toolAboutToBeLoaded( Tool * , bool * )));

			setWindowTitle(name);
			setWindowIcon(QIcon(tr(":/images/graph.png")));
			dockWidget = mainWindow->addToolWindow(this,MainWindow::DockWidget,Qt::BottomDockWidgetArea, Qt::BottomDockWidgetArea);

			if (dockWidget)
			{
				QPoint p = mainWindow->rect().bottomRight() - QPoint(sizeHint().width(),sizeHint().height());
				dockWidget->setFloating(true);
				dockWidget->hide();
			}

			QToolBar * toolBar = mainWindow->toolBarForTools;
			QAction * action = new QAction(tr("Plot Window"),toolBar);
			action->setIcon(QIcon(tr(":/images/graph.png")));

			if (dockWidget)
			{
				connect(action,SIGNAL(triggered()),dockWidget,SLOT(show()));
			}
			else
			{
				if (mainWindow->viewMenu)
				{
					QAction * toggle = mainWindow->viewMenu->addAction(tr("Plot Window"));
					toggle->setCheckable(true);
					connect(toggle,SIGNAL(toggled(bool)),this,SLOT(setVisible(bool)));
				}
				connect(action,SIGNAL(triggered()),this,SLOT(show()));
			}
			toolBar->addAction(action);

			return true;
		}
		return false;
	}

	void PlotTool::addWidget(PlotWidget* newPlot)
	{
		if (!newPlot) return;

		if (!multiplePlotsArea)
		{
			if (newPlot)
				delete newPlot;
			return;
		}

		if (!multiplePlotsArea->currentSubWindow())
		{
			otherToolBar = 0;
		}

		if (dockWidget)
		{
			dockWidget->show();
			dockWidget->raise();
		}
		else
		{
			show();
			this->raise();
		}

		QList<QMdiSubWindow *> subWindowList = multiplePlotsArea->subWindowList();
		if (keepOldPlots && keepOldPlots->isChecked())
		{
			for (int i=0; i < subWindowList.size(); ++i)
				if (subWindowList[i])
					subWindowList[i]->setWindowTitle( tr("plot ") + QString::number(i+1));
		}
		else
		{
			for (int i=0; i < subWindowList.size(); ++i)
				if (subWindowList[i])
				{
					subWindowList[i]->close();
				}
			subWindowList.clear();
		}

		QMdiSubWindow * window = multiplePlotsArea->addSubWindow(newPlot);
		window->setAttribute(Qt::WA_DeleteOnClose);
		window->setWindowIcon(QIcon(tr(":/images/graph.png")));
		window->setVisible(true);
		window->setWindowTitle( tr("plot ") + QString::number(1 + subWindowList.size()));

		if (keepOldPlots && keepOldPlots->isChecked())
			multiplePlotsArea->tileSubWindows();
		else
			window->showMaximized();

		multiplePlotsArea->setActiveSubWindow ( window );
	}

	void PlotTool::plot2D(const DataTable<qreal>& matrix,const QString& title,int x,int all)
	{
		if (mainWindow && mainWindow->statusBar())
			mainWindow->statusBar()->showMessage(tr("Plotting...."));

		if (!all)
			pruneDataTable(const_cast< DataTable<qreal>& >(matrix),x,mainWindow);

		if (holdCurrentPlot
			&& holdCurrentPlot->isChecked()
			&& multiplePlotsArea->currentSubWindow())
		{
			PlotWidget * widget = static_cast<PlotWidget*>(multiplePlotsArea->currentSubWindow()->widget());
			if (widget && widget->canAppendData())
			{
				DataTable<qreal> matrix2(matrix);
				matrix2.removeCol(x);
				widget->appendData(matrix2);
				if (mainWindow && mainWindow->statusBar())
					mainWindow->statusBar()->showMessage(tr("Finished plotting"));
				return;
			}
		}

		Plot2DWidget * newPlot = new Plot2DWidget(this);
		newPlot->plot(matrix,title,x);

		if (mainWindow && mainWindow->statusBar())
			mainWindow->statusBar()->showMessage(tr("Finished plotting"));

		addWidget(newPlot);
	}

	void PlotTool::plot3DSurface(const DataTable<qreal>& matrix,const QString& title)
	{
		if (mainWindow && mainWindow->statusBar())
			mainWindow->statusBar()->showMessage(tr("Plotting...."));

		Plot3DWidget * newPlot = new Plot3DWidget(this);
		newPlot->surface(matrix,title);

		if (mainWindow && mainWindow->statusBar())
			mainWindow->statusBar()->showMessage(tr("Finished 3D plot"));

		addWidget(newPlot);
	}

	void PlotTool::plotData(QSemaphore * s, DataTable<qreal>& matrix,int x,const QString& title,int all)
	{
		QRegExp regexp(tr("(?!\\d)_(?!\\d)"));
		for (int i=0; i < matrix.cols(); ++i)
		{
			matrix.colName(i).replace(regexp,tr("."));
		}

		plot2D(matrix,title,x,all);

		if (s)
			s->release();
	}

	void PlotTool::surface(QSemaphore * s, DataTable<qreal>& matrix,const QString& title)
	{
		QRegExp regexp(tr("(?!\\d)_(?!\\d)"));
		for (int i=0; i < matrix.cols(); ++i)
		{
			matrix.colName(i).replace(regexp,tr("."));
		}

		plot3DSurface(matrix,title);

		if (s)
			s->release();
	}

	void PlotTool::getData(QSemaphore* s, DataTable<qreal>* matrix,int index)
	{
		if (matrix && multiplePlotsArea)
		{
			QList<QMdiSubWindow*> list = multiplePlotsArea->subWindowList();
			if (index < 0 || index >= list.size())
				index = list.indexOf(multiplePlotsArea->currentSubWindow());
			if (index >= 0 && list.size() > index && list[index] && list[index]->widget())
			{
				PlotWidget * plotWidget = static_cast<PlotWidget*>(list[index]->widget());
				(*matrix) = *(plotWidget->data());
			}
		}
		if (s)
			s->release();
	}

	typedef void (*tc_PlotTool_api)(
		void (*plot)(Matrix,int,const char*,int) ,
		void (*surface)(Matrix,const char*) ,
		void (*hist)(Matrix,int,const char*) ,
		void (*errorbars)(Matrix,int,const char*) ,
		Matrix (*plotData)(int)
		);

	void PlotTool::setupFunctionPointers( QLibrary * library)
	{
		tc_PlotTool_api f = (tc_PlotTool_api)library->resolve("tc_PlotTool_api");
		if (f)
		{
			f(
				&(_plot),
				&(_surface),
				0,
				0,
				&(_plotData)
			);
		}
	}



	/*************************
		C Interface
	*************************/

	void PlotTool::connectTCFunctions()
	{
		connect(&fToS,SIGNAL(plot(QSemaphore *, DataTable<qreal>&,int,const QString&,int)),this,SLOT(plotData(QSemaphore *, DataTable<qreal>&,int,const QString&,int)));
		connect(&fToS,SIGNAL(surface(QSemaphore *, DataTable<qreal>&,const QString&)),this,SLOT(surface(QSemaphore *, DataTable<qreal>&,const QString&)));
		connect(&fToS,SIGNAL(plotData(QSemaphore *,DataTable<qreal>*,int)),this,SLOT(getData(QSemaphore *,DataTable<qreal>*,int)));
	}

	PlotTool_FToS PlotTool::fToS;

	void PlotTool::_plot(Matrix a, int b,const char* c,int all)
	{
		return fToS.plot(a,b,c,all);
	}

	void PlotTool::_surface(Matrix m,const char* s)
	{
		return fToS.surface(m,s);
	}

	Matrix PlotTool::_plotData(int i)
	{
		return fToS.plotData(i);
	}

	void PlotTool_FToS::plot(Matrix a0,int a1,const char* title,int all)
	{
		DataTable<qreal>* dat = ConvertValue(a0);
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit plot(s,*dat,a1,ConvertValue(title),all);
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}

	void PlotTool_FToS::surface(Matrix a0,const char* title)
	{
		DataTable<qreal>* dat = ConvertValue(a0);
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit surface(s,*dat,ConvertValue(title));
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}

	Matrix PlotTool_FToS::plotData(int i)
	{
		QSemaphore * s = new QSemaphore(1);
		DataTable<qreal> * p = new DataTable<qreal>;
		s->acquire();
		emit plotData(s,p,i);
		s->acquire();
		s->release();
		delete s;
		if (p)
		{
			Matrix m = ConvertValue(*p);
			delete p;
			return m;
		}
		return emptyMatrix();
	}

	void PlotTool::pruneDataTable(DataTable<qreal>& table, int& x, MainWindow * main)
	{
		if (!main) return;

		QList<NetworkWindow*> allWindows = main->allWindows();
		QList<ItemHandle*> allItems;
		for (int i=0; i < allWindows.size(); ++i)
			allItems << allWindows[i]->allHandles();
		QHash<QString,int> names;
		ItemHandle * handle = 0;

		for (int i=0; i < allItems.size(); ++i)
		{
			handle = allItems[i];
			if (handle)
				names[handle->fullName(".")] = names[handle->fullName("_")] = 1;
		}

		bool hasItems = false;

		for (int i=0; i < table.cols(); ++i)
			if (names.contains(table.colName(i)))
			{
				hasItems = true;
				break;
			}

		QList<int> removeCols;
		for (int i=0; i < table.cols(); ++i)
			if (!names.contains(table.colName(i)) && (x != i))
			{
				table.removeCol(i);
				--i;
			}
	}

	void PlotTool::setVisible ( bool visible )
	{
		activateWindow();
		if (isMinimized())
			showNormal();

		Tool::setVisible(visible);
	}

	static double d = 2.0;
	static double* AddVariable(const char* s, void*)
	{
		return &d;
	}

	void PlotTool::plotFormula()
	{
		if (!functionsWidgetDock) return;

		if (!functionsWidgetDock->isVisible())
		{
			functionsWidgetDock->show();
			return;
		}

		QStringList list = functionsTextEdit.toPlainText().split(QRegExp(tr("[\\n|\\r|;]+")),QString::SkipEmptyParts);
		if (list.isEmpty()) return;
		plotFormula(list,xaxisLine.text().replace(tr(" "),tr("")),spinBox1.value(),spinBox2.value(),spinBox3.value());
	}

	void PlotTool::plotFormula(const QStringList& functions,const QString& xaxis,qreal start, qreal end, int points,const QString& title)
	{
		if (!functionsWidgetDock) return;

		if (functions.isEmpty())
		{
			if (console())
				console()->message("Please enter at least one formula");
			return;
		}

		if (xaxis.isEmpty() || xaxis.isNull() )
		{
			if (!functionsWidgetDock->isVisible())
			{
				functionsTextEdit.setPlainText(functions.join(tr("\n")));
				functionsWidgetDock->show();
				return;
			}
			if (console())
				console()->message("Please specify the x-axis (one of the variables in the formula)");
			return;
		}

		if (points < 2 || end <= start)
		{
			if (!functionsWidgetDock->isVisible())
			{
				functionsTextEdit.setPlainText(functions.join(tr("\n")));
				functionsWidgetDock->show();
				return;
			}
			if (console())
				console()->message("Inputs are incorrect. Make sure the number of points > 1 and end > start.");
			return;
		}

        NetworkWindow * net = currentWindow();

		double x = start;
		double dx = (end - start)/(double)points;
		DataTable<qreal> data;
		data.resize(points,1+functions.size());

		QString s,p,n;

		data.colName(0) = xaxis;
		for (int i=1; i < data.cols(); ++i)
		{
			s = functions[i-1];
			if (s.length() > 10)
				s = s.left(10) + tr("...");
			data.colName(i) = s;
		}

		ItemHandle * handle;

		for (int i=0; i < functions.size(); ++i)
		{
			s = functions[i];

			if (!s.contains(xaxis))
			{
				if (console())
					console()->error(tr("equation ") + QString::number(i) + tr(" is not a function of ") + xaxis);
				continue;
			}

			bool b;

			QList< QPair<QString,qreal> > values;
			values << QPair<QString,qreal>(xaxis,x);

			for (int j=0; j < data.rows(); ++j)
			{
				data.value(j,0) = x;
				values[0].second = x;
				data.value(j,i+1) = EquationParser::eval(net,s,&b,values);
				x += dx;
			}
		}
		plot2D(data,title,0,1);
	}


	void PlotTool::addExportOption(const QIcon& icon,const QString& type, const QString & toolTip)
	{
		if (!exportMenu || exportOptions.contains(type)) return;

		QAction * action = exportMenu->addAction(icon,type);
		action->setText(type);
		if (toolTip.isEmpty())
			action->setToolTip(tr("Export current plot to ") + type);
		else
			action->setToolTip(toolTip);
		actionGroup.addAction(action);

		exportOptions << type;
	}

	void PlotTool::actionTriggered(QAction* action)
	{
		if (action && exportOptions.contains(action->text()))
		{
			exportData(action->text());
		}
	}

	void PlotTool::exportData(const QString& type)
	{
		QMdiSubWindow * subwindow = multiplePlotsArea->currentSubWindow();
		if (subwindow && subwindow->widget())
		{
			PlotWidget * plotWidget = static_cast<PlotWidget*>(subwindow->widget());
			plotWidget->exportData(type);
		}
	}

	void PlotTool::subWindowActivated(QMdiSubWindow * subwindow)
	{
		if (subwindow && subwindow->widget() && window)
		{
			PlotWidget * plotWidget = static_cast<PlotWidget*>(subwindow->widget());
			if (otherToolBar)
			{
				otherToolBar->hide();
			}
			otherToolBar = &plotWidget->toolBar;
			window->addToolBar(Qt::TopToolBarArea,otherToolBar);
			otherToolBar->show();
		}
	}

	QDockWidget * PlotTool::addDockWidget(const QString& title, QWidget * widget, Qt::DockWidgetArea area)
	{
		if (window && widget)
		{
			QDockWidget * dock = new QDockWidget;
			dock->setWidget(widget);
			dock->setWindowTitle(title);
			window->addDockWidget(area,dock);
			return dock;
		}
		return 0;
	}

	void PlotTool::setStatusBarMessage(const QString& s)
	{
		if (window && window->statusBar())
			window->statusBar()->showMessage(s);
	}

	void PlotTool::keyPressEvent ( QKeyEvent * event )
	{
		if (multiplePlotsArea
			&& multiplePlotsArea->currentSubWindow()
			&& multiplePlotsArea->currentSubWindow()->widget())
		{
			if (console())
				console()->message("key pressed");
			PlotWidget * widget = static_cast<PlotWidget*>(multiplePlotsArea->currentSubWindow()->widget());
			widget->keyPressEvent(event);
		}
	}

	void PlotTool::mouseMoveEvent ( QMouseEvent * event )
	{
		if (multiplePlotsArea
			&& multiplePlotsArea->currentSubWindow()
			&& multiplePlotsArea->currentSubWindow()->widget())
		{
			if (console())
				console()->message("key pressed");
			PlotWidget * widget = static_cast<PlotWidget*>(multiplePlotsArea->currentSubWindow()->widget());
			widget->mouseMoveEvent(event);
		}
	}

}
