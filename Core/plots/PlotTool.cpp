/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.

****************************************************************************/

#include <math.h>
#include <QGroupBox>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QRegExp>
#include <QCheckBox>
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NetworkHandle.h"
#include "ConsoleWindow.h"
#include "PlotTool.h"
#include "PlotTextWidget.h"
#include "Plot2DWidget.h"
#include "Plot3DWidget.h"
#include "qwt_scale_engine.h"
#include "muParserDef.h"
#include "muParser.h"
#include "muParserInt.h"
#include "ClusterPlots.h"

namespace Tinkercell
{

	/***********************************
		Plot Tool
	************************************/
		
	QString PlotTool::ORGANIZER_DELIMITER = QString("::");

	PlotTool::PlotTool() : Tool(tr("Default Plot Tool"),tr("Plot")), actionGroup(this)
	{
		otherToolBar = 0;
		dockWidget = 0;
		organizerToolBar = 0;
		plotOrganizerEnabled = false;
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
		exportButton->setText(tr("E&xport"));
		exportButton->setPopupMode(QToolButton::MenuButtonPopup);
		exportButton->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
		
		QMenu * optionsMenu = new QMenu(tr("Options"),&toolBar);
		
		keepOldPlots = new QAction(tr("Keep Previous Graphs"),&toolBar);
		holdCurrentPlot = new QAction(tr("Append To Current Graph"),&toolBar);
		clusterPlots = new QAction(tr("Cluster Graphs"),&toolBar);
		keepOldPlots->setCheckable(true);
		holdCurrentPlot->setCheckable(true);
		//clusterPlots->setCheckable(true);
		keepOldPlots->setChecked(false);
		holdCurrentPlot->setChecked(false);
		//clusterPlots->setChecked(false);
		connect(clusterPlots,SIGNAL(triggered()),this,SLOT(clusteringToggled()));
		optionsMenu->addAction(keepOldPlots);
		optionsMenu->addAction(holdCurrentPlot);
		optionsMenu->addAction(clusterPlots);
		
		QToolButton * optionsButton = new QToolButton(&toolBar);
		optionsButton->setIcon(QIcon(":/images/tools.png"));
		optionsButton->setMenu(optionsMenu);
		optionsButton->setText(tr("&Options"));
		optionsButton->setPopupMode(QToolButton::MenuButtonPopup);
		optionsButton->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
		
		toolBar.addWidget(exportButton);
		toolBar.addWidget(optionsButton);
		
		QToolButton * customColumn = new QToolButton(&toolBar);
		customColumn->setIcon(QIcon(":/images/function.png"));
		customColumn->setText(tr("Custom Formula"));
		customColumn->setToolButtonStyle ( Qt::ToolButtonTextBesideIcon );
		toolBar.addWidget(customColumn);
		connect(customColumn,SIGNAL(pressed()),this,SLOT(plotCustomFormula()));

		window->addToolBar(Qt::TopToolBarArea,&toolBar);
		layout->addWidget(window);
		setLayout(layout);

		//setup toolbar

		addExportOption(QIcon(tr(":/images/save.png")),tr("Save graph"),tr("Save graph image"));
		//addExportOption(QIcon(tr(":/images/camera.png")),tr("Copy image"),tr("Copy image to clipboard"));
		addExportOption(QIcon(tr(":/images/new.png")),tr("Text"),tr("Show the data table"));
		addExportOption(QIcon(tr(":/images/latex.png")),tr("LaTeX"),tr("Export data to LaTeX"));
		addExportOption(QIcon(tr(":/images/copy.png")),tr("Clipboard"),tr("Copy data to clipboard"));
		
		enablePlotOrganizer(true);

		//C interface
		connect(&fToS,SIGNAL(plotDataTable(QSemaphore*,DataTable<qreal>&, int, const QString&)),
				this, SLOT(plotData(QSemaphore*,DataTable<qreal>&, int, const QString&)));
		
		connect(&fToS,SIGNAL(plotDataTable3D(QSemaphore*,DataTable<qreal>&, const QString&)),
				this, SLOT(surface(QSemaphore*,DataTable<qreal>&, const QString&)));
		
		connect(&fToS,SIGNAL(plotHist(QSemaphore*,DataTable<qreal>&, double, const QString&)),
				this,SLOT(plotHist(QSemaphore*,DataTable<qreal>&, double, const QString&)));
		
		connect(&fToS,SIGNAL(plotErrorbars(QSemaphore*,DataTable<qreal>&, int, const QString&)),
				this, SLOT(plotErrorbars(QSemaphore*,DataTable<qreal>&, int, const QString&)));
				
		connect(&fToS,SIGNAL(plotScatter(QSemaphore*,DataTable<qreal>&, const QString&)),
				this, SLOT(plotScatter(QSemaphore*,DataTable<qreal>&, const QString&)));
		
		connect(&fToS,SIGNAL(plotMultiplot(QSemaphore*,int, int)), this, SLOT(plotMultiplot(QSemaphore*,int, int)));

		connect(&fToS,SIGNAL(plotHold(QSemaphore*,int)), this, SLOT(plotHold(QSemaphore*,int)));
		
		connect(&fToS,SIGNAL(plotClustering(QSemaphore*,DataTable<qreal>&,int)), this, SLOT(plotClustering(QSemaphore*,DataTable<qreal>&,int)));
		
		connect(&fToS,SIGNAL(getDataTable(QSemaphore*,DataTable<qreal>*, int)), this, SLOT(getData(QSemaphore*, DataTable<qreal>*,int)));
		
		connect(&fToS,SIGNAL(gnuplot(QSemaphore*,const QString&)), this, SLOT(gnuplot(QSemaphore*,const QString&)));
		
		connect(&fToS,SIGNAL(savePlotImage(QSemaphore*, const QString&)), this, SLOT(savePlotImage(QSemaphore*, const QString&)));
		
		connect(&fToS,SIGNAL(setLog(QSemaphore*, int)), this, SLOT(setLogScale(QSemaphore*, int)));
		
		qRegisterMetaType< PlotTool::PlotType >("PlotTool::PlotType");
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

			setWindowTitle(name);
			setWindowIcon(QIcon(tr(":/images/graph2.png")));
			dockWidget = mainWindow->addToolWindow(this,MainWindow::DockWidget,Qt::BottomDockWidgetArea, Qt::BottomDockWidgetArea);

			if (dockWidget)
			{
				QPoint p = mainWindow->rect().bottomRight() - QPoint(sizeHint().width(),sizeHint().height());
				dockWidget->hide();
				dockWidget->setFloating(true);
				dockWidget->setWindowFlags(Qt::Window);
			}

			QToolBar * toolBar = mainWindow->toolBarForTools;
			QAction * action = new QAction(tr("Plot Window"),toolBar);
			action->setIcon(QIcon(tr(":/images/graph2.png")));

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

		QList<QMdiSubWindow *> subWindowList = multiplePlotsArea->subWindowList();
		QList<QMdiSubWindow *> subWindowList2;
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
					PlotWidget * plotWidget = static_cast<PlotWidget*>(subWindowList[i]->widget());
					if (plotWidget && !plotWidget->category.isNull() && !plotWidget->category.isEmpty())
					{
						if (plotWidget->category != newPlot->category)
						{
							subWindowList[i]->hide();
							subWindowList2 << subWindowList[i];
						}
						else
						{
							subWindowList[i]->show();
							subWindowList[i]->showNormal();
						}
					}
					else
						subWindowList[i]->close();
				}
		}

		QMdiSubWindow * window = multiplePlotsArea->addSubWindow(newPlot);
		if (newPlot->category.isNull() || newPlot->category.isEmpty())
			window->setAttribute(Qt::WA_DeleteOnClose);
		window->setWindowIcon(QIcon(tr(":/images/graph2.png")));
		window->setVisible(true);
		window->setWindowTitle( tr("plot ") + QString::number(1 + subWindowList.size()));

		multiplePlotsArea->tileSubWindows();
		for (int i=0; i < subWindowList2.size(); ++i)
			if (subWindowList2[i])
			{
				subWindowList2[i]->show();
				subWindowList2[i]->showMinimized();
			}

		multiplePlotsArea->setActiveSubWindow ( window );
	}
	
	DataTable<qreal> PlotTool::cluster(int numClusters)
	{
        DataTable<qreal> result;
		if (multiplePlotsArea && numClusters > 1 && ClusterPlot::tables.size() > 1)
		{
			QList<QMdiSubWindow *>  list = multiplePlotsArea->subWindowList(QMdiArea::ActivationHistoryOrder);
			PlotType type;
			for (int i=0; i < list.size(); ++i)
			{
				if (list[i]->widget())
				{
					PlotWidget * widget = static_cast<PlotWidget*>(list[i]->widget());
					if (widget)
					{
						//ClusterPlot::tables << (*widget->data());
						type = widget->type;
					}
				}
				list[i]->close();
			}
			
			int * clusters = ClusterPlot::getClusters(numClusters);
            
            QList<Plot2DWidget*> clusterWidgets;
			for (int i=0; i < numClusters; ++i)
			{
				Plot2DWidget * newPlot2D = new Plot2DWidget(this);
				newPlot2D->type = type;
				QMdiSubWindow * window = multiplePlotsArea->addSubWindow(newPlot2D);
				window->setAttribute(Qt::WA_DeleteOnClose);
				window->setWindowIcon(QIcon(tr(":/images/graph2.png")));
				window->setVisible(true);
				window->setWindowTitle( tr("plot ") + QString::number(i+1));
				newPlot2D->plot(DataTable<qreal>(),tr("Cluster ") + QString::number(i+1));
				clusterWidgets << newPlot2D;
			}
			
			result.resize(ClusterPlot::tables.size(),1);
			result.setColumnName(0,"clusterID");
			for (int i=0; i < ClusterPlot::tables.size(); ++i)
			{
				int j = clusters[i];
				result(i,0) = j;
				clusterWidgets[j]->appendData(ClusterPlot::tables[i],tr("Cluster ") + QString::number(j+1));
			}
			delete clusters;
			multiplePlotsArea->tileSubWindows();
		}
		return result;
	}

	void PlotTool::plot(const DataTable<qreal>& matrix,const QString& title0,int x,PlotTool::PlotType type)
	{
		//if (!all)
			//pruneDataTable(const_cast< DataTable<qreal>& >(matrix),x,mainWindow);
		
		QString title, category;
		
		//plot organizer uses
		if (title0.contains(ORGANIZER_DELIMITER))
		{
			QStringList parts = title0.split(ORGANIZER_DELIMITER);
			if (parts.size() > 1)
			{
				category = parts[0].trimmed();
				title = parts[1].trimmed();
			}
			else
			{
				title = title0;
			}
		}
		else
		{
			title = title0;
		}
		
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
			 (keepOldPlots && keepOldPlots->isChecked()))
			 {
				ClusterPlot::tables << matrix;
			}
			else
			{
				if (!ClusterPlot::tables.isEmpty())
					ClusterPlot::tables.clear();
			}
		
		if ((category.isNull() || category.isEmpty()) &&
			((holdCurrentPlot && holdCurrentPlot->isChecked()) ||
			 !(keepOldPlots && keepOldPlots->isChecked())))
		{
			QList<QMdiSubWindow *>  list = multiplePlotsArea->subWindowList(QMdiArea::ActivationHistoryOrder);
			for (int i=0; i < list.size(); ++i)
				if (list[i]->widget())
				{
					PlotWidget * widget = static_cast<PlotWidget*>(list[i]->widget());
					if (widget && widget->type == type)
					{
						if (widget->canAppendData() && holdCurrentPlot && holdCurrentPlot->isChecked())
							widget->appendData(matrix,title);
						else
							widget->updateData(matrix,title,x);
					
						for (int j=0; j < list.size(); ++j)
							if (i != j)
							{ 	
								widget = static_cast<PlotWidget*>(list[j]->widget());
								if (widget && widget->type == Text)
								{
									widget->updateData(matrix,title,x);
									break;
								}
							}
					
						return;
					}
				}
		}
		
		PlotWidget * newPlot = 0;
		
		if  (type == SurfacePlot)
		{
			Plot3DWidget * newPlot3D = new Plot3DWidget(this);
			newPlot3D->surface(matrix,title);
			newPlot = newPlot3D;
		}
		else
		if (type == Text)
		{
			PlotTextWidget * text = new PlotTextWidget(matrix, this);
			newPlot = text;
		}
		else
		{
			Plot2DWidget * newPlot2D = new Plot2DWidget(this);
			newPlot2D->type = type;
			newPlot2D->plot(matrix,title,x);
			newPlot = newPlot2D;
		}
		
		newPlot->category = category;
		addWidget(newPlot);
		
		if (!category.isNull() && !category.isEmpty())
			addWidgetToOrganizer(category, newPlot);
	}

	void PlotTool::surfacePlot(const DataTable<qreal>& matrix,const QString& title)
	{
		plot(matrix, title, 0, SurfacePlot);
	}

	void PlotTool::plotData(QSemaphore * s, DataTable<qreal>& matrix,int x,const QString& title)
	{
		QRegExp regexp(tr("(?!\\d)_(?!\\d)"));
		for (int i=0; i < matrix.columns(); ++i)
		{
			QString s = matrix.columnName(i);
			s.replace(regexp,tr("."));
			matrix.setColumnName(i,s);
		}
		
		matrix.description() = title;
		plot(matrix,title,x,Plot2D);
		
		emit plotDataTable(matrix, x, title);
		
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
		plot(matrix,title,0,ScatterPlot);
		
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
		plot(data,title,0,HistogramPlot);
		
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
	
	void PlotTool::setLogScale(QSemaphore * sem, int i)
	{
		if (multiplePlotsArea
			&& multiplePlotsArea->currentSubWindow()
			&& multiplePlotsArea->currentSubWindow()->widget())
		{
			PlotWidget * widget = static_cast<PlotWidget*>(multiplePlotsArea->currentSubWindow()->widget());
			widget->setLogScale(i);
		}
		
		if (sem)
			sem->release();
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
	
	void PlotTool::plotClustering(QSemaphore* s, DataTable<qreal>& res, int n)
	{
		res = cluster(n);
		if (s)
			s->release();
	}
	
	void PlotTool::plotHold(QSemaphore* s, int x)
	{
		overplot(x > 0);
		if (s)
			s->release();
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
					PlotWidget * plotWidget = static_cast<PlotWidget*>(subWindowList[i]->widget());
					if (plotWidget && !plotWidget->category.isNull() && !plotWidget->category.isEmpty())
						subWindowList[i]->showMinimized();
					else
						subWindowList[i]->close();
				}
			subWindowList.clear();
			hold(true);
			emit plotMultiplot(x, y);
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
	
	 QList<PlotWidget*> PlotTool::plotWidgets() const
	 {
	 	QList<PlotWidget*> widgets;
	 	if (multiplePlotsArea)
		{
			QList<QMdiSubWindow*> list = multiplePlotsArea->subWindowList();
			for (int i=0; i < list.size(); ++i)
				if (list[i] && list[i]->widget())
				{
					PlotWidget * plotWidget = static_cast<PlotWidget*>(list[i]->widget());
					widgets += plotWidget;
				}
		}
		return widgets;
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
		void (*hold)(int),
		tc_matrix (*enableClustering)(int),
		tc_matrix (*plotData)(int),
		void (*gnuplot)(const char*),
		void (*savePlotImage)(const char*),
		void (*setlog)(int)
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
				&(plotHoldC),
				&(plotClusteringC),
				&(getDataMatrix),
				&(_gnuplot),
				&(_savePlotImage),
				&(_setLogScale)
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
                                          tr("PDF Files (*.pdf)"));

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
					window->addToolBar(Qt::TopToolBarArea,otherToolBar);
					//window->addToolBar(Qt::RightToolBarArea,otherToolBar);
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
	
	void PlotTool_FtoS::plotMatrix(tc_matrix m, int x, const char* title)
	{
		QSemaphore * s = new QSemaphore(1);
		DataTable<qreal> * dat = ConvertValue(m);
		s->acquire();
		emit plotDataTable(s, *dat,x,QString(title));
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
	
	void PlotTool_FtoS::plotHoldC(int x)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit plotHold(s, x);
		s->acquire();
		s->release();
		delete s;
	}
	
	tc_matrix PlotTool_FtoS::plotClusteringC(int n)
	{
		QSemaphore * s = new QSemaphore(1);
		DataTable<qreal> dat;
		s->acquire();
		emit plotClustering(s, dat, n);
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(dat);
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

	void PlotTool_FtoS::setLogScale(int i)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setLog(s,i);
		s->acquire();
		s->release();
		delete s;
	}
	
	void PlotTool::plotMatrix(tc_matrix m, const char* title)
	{
		int x = 0, all = 1;
		fToS.plotMatrix(m,x,title);
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
	
	void PlotTool::plotHoldC(int x)
	{
		fToS.plotHoldC(x);
	}
	
	tc_matrix PlotTool::plotClusteringC(int n)
	{
		return fToS.plotClusteringC(n);
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
	
	void PlotTool::_setLogScale(int i)
	{
		return fToS.setLogScale(i);
	}
	
	void PlotTool::plotCustomFormula()
	{
		QString s = QInputDialog::getText(this, tr("Enter formula"), tr("Enter formula using existing column names"));
		if (!s.isNull() && !s.isEmpty())
		{
			QString error = computeNewColumn(s);
			if (!error.isEmpty())
				QMessageBox::information(this, tr("Error"), error);
		}
	}
	
	QString PlotTool::computeNewColumn(QString formula)
	{
		if (!multiplePlotsArea) return QString();
		
		QMdiSubWindow * subwindow = multiplePlotsArea->currentSubWindow();
		if (!subwindow || !subwindow->widget()) return QString();
		
		PlotWidget * plotWidget = static_cast<PlotWidget*>(subwindow->widget());
		
		QRegExp regex(tr("\\.(?!\\d)"));
		formula.replace(regex, tr("_"));
		
		DataTable<qreal> * pData = plotWidget->data();
		if (!pData)
			return QString("No data to compute function");
		
		DataTable<qreal> matrix(*pData);
		
		int k = 1;
		QString newcol("formula_1");
		while (matrix.hasColumn(newcol))
			newcol = tr("formula_") + QString::number(++k);
		
		QStringList colnames = matrix.columnNames();				
		double * params = new double[colnames.size()];
		mu::Parser parser;
		try
		{
			for (int i=0; i < colnames.size(); ++i)
			{
				double * dp = &(params[i]);
				colnames[i].replace(tr("."), tr("_"));
				parser.DefineVar(colnames[i].toAscii().data(), dp);
				if (!colnames[i].contains(colnames[i].toLower()))
					parser.DefineVar(colnames[i].toLower().toAscii().data(), dp);
			}
				
			parser.SetExpr(formula.toAscii().data());				

			for (int i=0; i < matrix.rows(); ++i)
			{
				for (int j=0; j < colnames.size(); ++j)
					params[j] = matrix(i,j);

				matrix(i,newcol) = parser.Eval();
			}
			
			plotWidget->updateData(matrix,plotWidget->windowTitle());
		}
		catch(mu::Parser::exception_type &e)
		{
		    return QString(e.GetMsg().data()) + tr("\n");
		}
		return QString();
	}
	
	void  PlotTool::organizerButtonClicked(QAbstractButton * button)
	{
		if (button)
		{
			QString category = button->text();			
			QList<QMdiSubWindow *> subWindowList = multiplePlotsArea->subWindowList();
			for (int i=0; i < subWindowList.size(); ++i)
				if (subWindowList[i] && subWindowList[i]->widget())
				{
					PlotWidget * plotWidget = static_cast<PlotWidget*>(subWindowList[i]->widget());
					if (plotWidget->category == category)
						subWindowList[i]->showNormal();
					else
						subWindowList[i]->showMinimized();
				}
		}
	}
	
	void PlotTool::addWidgetToOrganizer(const QString& category, PlotWidget * widget)
	{
		if (!widget || 
			category.isNull() || 
			category.isEmpty() || 
			!organizerButtonGroup || 
			!organizerToolBar) return;
		
		if (!organizerToolBar->isVisible())
			organizerToolBar->show();
			
		widget->category = category;
		
		QList<QAbstractButton *> buttons = organizerButtonGroup->buttons();

		bool exists = false;		
		for (int i=0; i < buttons.size(); ++i)
			if (buttons[i] && buttons[i]->text() == category)
			{
				exists = true;
				break;
			}

		if (!exists)
		{
			QToolButton * button = new QToolButton;
			button->setIcon(QIcon(":/images/graph2.png"));
			button->setText(category);
			button->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
			organizerButtonGroup->addButton(button);
			organizerToolBar->addWidget(button);
			organizerButtonClicked(button);
		}
	}
	
	void  PlotTool::enablePlotOrganizer(bool b)
	{
		plotOrganizerEnabled = b;

		if (plotOrganizerEnabled && !organizerToolBar && window)
		{
			organizerToolBar = new QToolBar(this);
			organizerToolBar->setObjectName(tr("plot organizer widget"));
			organizerButtonGroup = new QButtonGroup(this);
			connect(organizerButtonGroup,SIGNAL(buttonClicked ( QAbstractButton *  )),
						 this, SLOT(organizerButtonClicked ( QAbstractButton *  )));
			window->addToolBar(Qt::TopToolBarArea,organizerToolBar);
			organizerToolBar->hide();
		}
	}
	
	void PlotTool::clusteringToggled()
	{
		bool ok = false;
		int n = QInputDialog::getInt(this, tr("Clusters"), tr("Number of clusters: "), 1, 1, 10, 1, &ok);
		if (ok)
			cluster(n);
	}
	
	PlotTool_FtoS PlotTool::fToS;
}

