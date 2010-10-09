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
#include "NetworkHandle.h"
#include "ConsoleWindow.h"
#include "PlotTool.h"
#include "Plot2DWidget.h"
#include "Plot3DWidget.h"
#include "qwt_scale_engine.h"

namespace Tinkercell
{

	/***********************************
		Plot Tool
	************************************/

	PlotTool::PlotTool() : Tool(tr("Default Plot Tool"),tr("Plot")), actionGroup(this)
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

		//setup toolbar

		addExportOption(QIcon(tr(":/images/save.png")),tr("Save graph"),tr("Save graph image"));
		//addExportOption(QIcon(tr(":/images/camera.png")),tr("Copy image"),tr("Copy image to clipboard"));
		addExportOption(QIcon(tr(":/images/new.png")),tr("Text"),tr("Show the data table"));
		addExportOption(QIcon(tr(":/images/latex.png")),tr("LaTeX"),tr("Export data to LaTeX"));
		addExportOption(QIcon(tr(":/images/copy.png")),tr("Clipboard"),tr("Copy data to clipboard"));

		//C interface
		connect(&fToS,SIGNAL(plotDataTable(QSemaphore*,DataTable<qreal>&, int, const QString& , int)),
				this, SLOT(plotData(QSemaphore*,DataTable<qreal>&, int, const QString& , int)));
		
		connect(&fToS,SIGNAL(plotDataTable3D(QSemaphore*,DataTable<qreal>&, const QString&)),
				this, SLOT(surface(QSemaphore*,DataTable<qreal>&, const QString&)));
		
		connect(&fToS,SIGNAL(plotHist(QSemaphore*,DataTable<qreal>&, double, const QString&)),
				this,SLOT(plotHist(QSemaphore*,DataTable<qreal>&, double, const QString&)));
		
		connect(&fToS,SIGNAL(plotErrorbars(QSemaphore*,DataTable<qreal>&, int, const QString&)),
				this, SLOT(plotErrorbars(QSemaphore*,DataTable<qreal>&, int, const QString&)));
				
		connect(&fToS,SIGNAL(plotScatter(QSemaphore*,DataTable<qreal>&, const QString&)),
				this, SLOT(plotScatter(QSemaphore*,DataTable<qreal>&, const QString&)));
		
		connect(&fToS,SIGNAL(plotMultiplot(QSemaphore*,int, int)), this, SLOT(plotMultiplot(QSemaphore*,int, int)));
		
		connect(&fToS,SIGNAL(getDataTable(QSemaphore*,DataTable<qreal>*, int)), this, SLOT(getData(QSemaphore*, DataTable<qreal>*,int)));
		
		connect(&fToS,SIGNAL(gnuplot(QSemaphore*,const QString&)), this, SLOT(gnuplot(QSemaphore*,const QString&)));
		
		connect(&fToS,SIGNAL(savePlotImage(QSemaphore*, const QString&)), this, SLOT(savePlotImage(QSemaphore*, const QString&)));
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

			//connect(mainWindow, SIGNAL(toolAboutToBeLoaded( Tool * , bool * )),
                //    this, SLOT(toolAboutToBeLoaded( Tool * , bool * )));

			setWindowTitle(name);
			setWindowIcon(QIcon(tr(":/images/graph.png")));
			dockWidget = mainWindow->addToolWindow(this,MainWindow::DockWidget,Qt::BottomDockWidgetArea, Qt::BottomDockWidgetArea);

			if (dockWidget)
			{
				QPoint p = mainWindow->rect().bottomRight() - QPoint(sizeHint().width(),sizeHint().height());
				dockWidget->hide();
				dockWidget->setFloating(true);
			}

			//QToolBar * toolBar = mainWindow->toolBarForTools;
			//QAction * action = new QAction(tr("Plot Window"),toolBar);
			//action->setIcon(QIcon(tr(":/images/graph.png")));

			if (dockWidget)
			{
				//connect(action,SIGNAL(triggered()),dockWidget,SLOT(show()));
			}
			else
			{
				if (mainWindow->viewMenu)
				{
					QAction * toggle = mainWindow->viewMenu->addAction(tr("Plot Window"));
					toggle->setCheckable(true);
					connect(toggle,SIGNAL(toggled(bool)),this,SLOT(setVisible(bool)));
				}
				//connect(action,SIGNAL(triggered()),this,SLOT(show()));
			}
			//toolBar->addAction(action);

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

		QList<QMdiSubWindow *> subWindowList = multiplePlotsArea->subWindowList();
		if (newPlot->type == Text || (keepOldPlots && keepOldPlots->isChecked()))
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

	void PlotTool::plot(const DataTable<qreal>& matrix,const QString& title,int x,int all,PlotTool::PlotType type)
	{
		if (!all)
			pruneDataTable(const_cast< DataTable<qreal>& >(matrix),x,mainWindow);
		
		if (dockWidget)
		{
			dockWidget->show();
			dockWidget->showNormal();
			dockWidget->raise();
		}
		else
		{
			show();
			showNormal();
			this->raise();
		}

		if ((holdCurrentPlot && holdCurrentPlot->isChecked()) ||
			!(keepOldPlots && keepOldPlots->isChecked()))
		{
			QList<QMdiSubWindow *>  list = multiplePlotsArea->subWindowList(QMdiArea::ActivationHistoryOrder);
			for (int i=0; i < list.size(); ++i)
			{
				PlotWidget * widget = static_cast<PlotWidget*>(list[i]->widget());
				if (widget && widget->type == type)
				{
					if (widget->canAppendData()  && holdCurrentPlot && holdCurrentPlot->isChecked())
						widget->appendData(matrix);
					else
						widget->updateData(matrix);
					if (mainWindow && mainWindow->statusBar())
						mainWindow->statusBar()->showMessage(tr("Finished plotting"));
					return;
				}
			}
		}
		Plot2DWidget * newPlot = new Plot2DWidget(this);
		newPlot->type = type;
		
		newPlot->plot(matrix,title,x);

		if (mainWindow && mainWindow->statusBar())
			mainWindow->statusBar()->showMessage(tr("Finished plotting"));

		addWidget(newPlot);
	}

	void PlotTool::surfacePlot(const DataTable<qreal>& matrix,const QString& title)
	{
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
		
		if ((holdCurrentPlot && holdCurrentPlot->isChecked()) ||
			!(keepOldPlots && keepOldPlots->isChecked()))
		{
			QList<QMdiSubWindow *>  list = multiplePlotsArea->subWindowList(QMdiArea::ActivationHistoryOrder);
			for (int i=0; i < list.size(); ++i)
			{
				PlotWidget * widget = static_cast<PlotWidget*>(list[i]->widget());
				if (widget && widget->type == SurfacePlot)
				{
					if (widget->canAppendData()  && holdCurrentPlot && holdCurrentPlot->isChecked())
						widget->appendData(matrix);
					else
						widget->updateData(matrix);
					if (mainWindow && mainWindow->statusBar())
							mainWindow->statusBar()->showMessage(tr("Finished plotting"));
					return;
				}
			}
		}

		Plot3DWidget * newPlot = new Plot3DWidget(this);
		newPlot->surface(matrix,title);

		if (mainWindow && mainWindow->statusBar())
			mainWindow->statusBar()->showMessage(tr("Finished 3D plot"));

		addWidget(newPlot);
	}

	void PlotTool::plotData(QSemaphore * s, DataTable<qreal>& matrix,int x,const QString& title,int all)
	{
		QRegExp regexp(tr("(?!\\d)_(?!\\d)"));
		for (int i=0; i < matrix.columns(); ++i)
		{
			QString s = matrix.columnName(i);
			s.replace(regexp,tr("."));
			matrix.setColumnName(i,s);
		}
		
		matrix.description() = title;
		plot(matrix,title,x,all,Plot2D);
		
		emit plotDataTable(matrix, x, title, all);
		
		if (multiplePlotsArea && numMultiplots > 0 && numMultiplots <= multiplePlotsArea->subWindowList().size())
		{
			numMultiplots = 0;
			hold(false);
		}

		if (s)
			s->release();
	}
	
	void PlotTool::plotScatter(QSemaphore * s, DataTable<qreal>& matrix,const QString& title)
	{
		QRegExp regexp(tr("(?!\\d)_(?!\\d)"));
		for (int i=0; i < matrix.columns(); ++i)
		{
			QString s = matrix.columnName(i);
			s.replace(regexp,tr("."));
			matrix.setColumnName(i,s);
		}

		matrix.description() = title;
		plot(matrix,title,0,1,ScatterPlot);
		
		emit plotScatterplot(matrix, title);
		
		if (multiplePlotsArea && numMultiplots > 0 && numMultiplots <= multiplePlotsArea->subWindowList().size())
		{
			numMultiplots = 0;
			hold(false);
		}
		
		if (s)
			s->release();
	}
	
	void PlotTool::plotHist(QSemaphore* s,DataTable<qreal>& data, double binsz, const QString& title)
	{
		QRegExp regexp(tr("(?!\\d)_(?!\\d)"));
		for (int i=0; i < data.columns(); ++i)
		{
			QString s = data.columnName(i);
			s.replace(regexp,tr("."));
			data.setColumnName(i,s);
		}

		data.description() = title;
		plot(data,title,0,1,HistogramPlot);
		
		emit plotHist(data , binsz, title);
		
		if (multiplePlotsArea && numMultiplots > 0 && numMultiplots <= multiplePlotsArea->subWindowList().size())
		{
			numMultiplots = 0;
			hold(false);
		}
		
		if (s)
			s->release();
	}
	
	void PlotTool::plotErrorbars(QSemaphore* s,DataTable<qreal>& data, int x, const QString& title)
	{
		QRegExp regexp(tr("(?!\\d)_(?!\\d)"));
		for (int i=0; i < data.columns(); ++i)
		{
			QString s = data.columnName(i);
			s.replace(regexp,tr("."));
			data.setColumnName(i,s);
		}
		
		data.description() = title;
		emit plotErrorbars(data , x, title);
		
		if (multiplePlotsArea && numMultiplots > 0 && numMultiplots <= multiplePlotsArea->subWindowList().size())
		{
			numMultiplots = 0;
			hold(false);
		}
		
		if (s)
			s->release();
	}
	
	void PlotTool::savePlotImage(QSemaphore * s, const QString& file)
	{
		QMdiSubWindow * subwindow = multiplePlotsArea->currentSubWindow();
		if (subwindow && subwindow->widget())
		{
			PlotWidget * plotWidget = static_cast<PlotWidget*>(subwindow->widget());
			plotWidget->exportData(tr("save graph"),file);
		}
		if (s)
			s->release();
	}
	
	void PlotTool::gnuplot(QSemaphore * s, const QString& script)
	{
		emit gnuplot(script);
		if (s)
			s->release();
	}
	
	void PlotTool::hold(bool b)
	{
		if (keepOldPlots)
			keepOldPlots->setChecked(b);
	}
	
	void PlotTool::overplot(bool b)
	{
		if (holdCurrentPlot)
			holdCurrentPlot->setChecked(b);
	}
	
	void PlotTool::plotMultiplot(QSemaphore* s, int x, int y)
	{
		numMultiplots = x*y;
		
		if (numMultiplots > 0)
		{
			QList<QMdiSubWindow *> subWindowList = multiplePlotsArea->subWindowList();
			for (int i=0; i < subWindowList.size(); ++i)
				if (subWindowList[i])
				{
					subWindowList[i]->close();
				}
			subWindowList.clear();
			hold(true);		
			emit plotMultiplot( x, y);
		}
		
		if (s)
			s->release();
	}

	void PlotTool::surface(QSemaphore * s, DataTable<qreal>& matrix,const QString& title)
	{
		QRegExp regexp(tr("(?!\\d)_(?!\\d)"));
		for (int i=0; i < matrix.columns(); ++i)
		{
			QString s = matrix.columnName(i);
			s.replace(regexp,tr("."));
			matrix.setColumnName(i,s);
		}

		matrix.description() = title;
		surfacePlot(matrix,title);
		
		emit plotDataTable3D(matrix, title);
		
		if (multiplePlotsArea && numMultiplots > 0 && numMultiplots <= multiplePlotsArea->subWindowList().size())
		{
			numMultiplots = 0;
			hold(false);
		}

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
		void (*plot)(tc_matrix,const char*) ,
		void (*surface)(tc_matrix,const char*) ,
		void (*hist)(tc_matrix,const char*) ,
		void (*errorbars)(tc_matrix,const char*) ,
		void (*scatterplot)(tc_matrix data,const char* title) ,
		void (*multiplot)(int,int),
		tc_matrix (*plotData)(int),
		void (*gnuplot)(const char*),
		void (*savePlotImage)(const char*)
	);

    void PlotTool::setupFunctionPointers( QLibrary * library )
    {
        tc_PlotTool_api f = (tc_PlotTool_api)library->resolve("tc_PlotTool_api");
		if (f)
		{
			f(
				&(plotMatrix),
				&(plotMatrix3D),
				&(plotHistC),
				&(plotErrorbarsC),
				&(plotScatterC),
				&(plotMultiplotC),
				&(getDataMatrix),
				&(_gnuplot),
				&(_savePlotImage)
			);
		}
    }

	void PlotTool::pruneDataTable(DataTable<qreal>& table, int& x, MainWindow * main)
	{
		if (!main) return;

		QList<NetworkHandle*> allNetworks = main->networks();
		QStringList names;
		QHash<QString,int> hash;
		for (int i=0; i < allNetworks.size(); ++i)
		{
			names = allNetworks[i]->symbolsTable.uniqueHandlesWithDot.keys();
			names << allNetworks[i]->symbolsTable.uniqueHandlesWithUnderscore.keys();
			for (int j=0; j < names.size(); ++j)
				hash[ names[j] ] = 1;
		}
		
		bool hasItems = false;

		for (int i=0; i < table.columns(); ++i)
			if (hash.contains(table.columnName(i)))
			{
				hasItems = true;
				break;
			}

		if (!hasItems) return;
		
		for (int i=0; i < table.columns(); ++i)
			if (!hash.contains(table.columnName(i)) && (x != i))
			{
				table.removeColumn(i);
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
			QString fileName;
			if (type.toLower() == tr("save graph"))
				fileName = QFileDialog::getSaveFileName(this, tr("Print to File"),
                                          MainWindow::previousFileName,
                                          tr("PNG Files (*.png)"));

			plotWidget->exportData(type,fileName);
		}
	}

	void PlotTool::subWindowActivated(QMdiSubWindow * subwindow)
	{
		if (subwindow && subwindow->widget() && window)
		{
			PlotWidget * plotWidget = static_cast<PlotWidget*>(subwindow->widget());
			if (otherToolBar != &plotWidget->toolBar)
			{
				if (otherToolBar)
					otherToolBar->hide();
			
				otherToolBar = &plotWidget->toolBar;
				
				if (otherToolBar)
				{
					window->addToolBar(Qt::RightToolBarArea,otherToolBar);
					otherToolBar->show();
				}
			}
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
			PlotWidget * widget = static_cast<PlotWidget*>(multiplePlotsArea->currentSubWindow()->widget());
			widget->mouseMoveEvent(event);
		}
	}
	
	void PlotTool_FtoS::plotMatrix(tc_matrix m, int x, const char* title, int all)
	{
		QSemaphore * s = new QSemaphore(1);
		DataTable<qreal> * dat = ConvertValue(m);
		s->acquire();
		emit plotDataTable(s, *dat,x,QString(title),all);
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}

	void PlotTool_FtoS::plotMatrix3D(tc_matrix m, const char * title)
	{
		QSemaphore * s = new QSemaphore(1);
		DataTable<qreal> * dat = ConvertValue(m);
		s->acquire();
		emit plotDataTable3D(s, *dat,QString(title));
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}

	void PlotTool_FtoS::plotHistC(tc_matrix m, double bins, const char * title)
	{
		QSemaphore * s = new QSemaphore(1);
		DataTable<qreal> * dat = ConvertValue(m);
		s->acquire();
		emit plotHist(s, *dat,bins,QString(title));
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}

	void PlotTool_FtoS::plotErrorbarsC(tc_matrix m, int x, const char* title)
	{
		QSemaphore * s = new QSemaphore(1);
		DataTable<qreal> * dat = ConvertValue(m);
		s->acquire();
		emit plotErrorbars(s,*dat,x,QString(title));
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}
	
	void PlotTool_FtoS::plotScatterC(tc_matrix m, const char* title)
	{
		QSemaphore * s = new QSemaphore(1);
		DataTable<qreal> * dat = ConvertValue(m);
		s->acquire();
		emit plotScatter(s,*dat,QString(title));
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}

	void PlotTool_FtoS::plotMultiplotC(int x, int y)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit plotMultiplot(s, x, y);
		s->acquire();
		s->release();
		delete s;
	}

	tc_matrix PlotTool_FtoS::getDataMatrix(int index)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		DataTable<qreal> dat;
		emit getDataTable(s, &dat, index);
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(dat);
	}
	
	void PlotTool_FtoS::gnuplot(const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit gnuplot(s,QString(c));
		s->acquire();
		s->release();
		delete s;
	}
	
	void PlotTool_FtoS::savePlotImage(const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit savePlotImage(s,QString(c));
		s->acquire();
		s->release();
		delete s;
	}

	
	void PlotTool::plotMatrix(tc_matrix m, const char* title)
	{
		int x = 0, all = 1;
		fToS.plotMatrix(m,x,title,all);
	}

	void PlotTool::plotMatrix3D(tc_matrix m, const char * title)
	{
		fToS.plotMatrix3D(m,title);
	}

	void PlotTool::plotHistC(tc_matrix m, const char * title)
	{
		int bins = 100;
		fToS.plotHistC(m,bins,title);
	}

	void PlotTool::plotErrorbarsC(tc_matrix m, const char* title)
	{
		int x = 0;
		fToS.plotErrorbarsC(m,x,title);
	}
	
	void PlotTool::plotScatterC(tc_matrix m, const char* title)
	{
		fToS.plotScatterC(m,title);
	}
	
	void PlotTool::plotMultiplotC(int x, int y)
	{
		fToS.plotMultiplotC(x,y);
	}

	tc_matrix PlotTool::getDataMatrix(int index)
	{
		return fToS.getDataMatrix(index);
	}
	
	void PlotTool::_gnuplot(const char * s)
	{
		return fToS.gnuplot(s);
	}
	
	void PlotTool::_savePlotImage(const char* s)
	{
		return fToS.savePlotImage(s);
	}
	
	PlotTool_FtoS PlotTool::fToS;
}

