/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 This tool displays a plot based on the DataTable contained.
 
****************************************************************************/

#include <math.h>
#include <QGroupBox>
#include <QRegExp>
#include <QCheckBox>
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NetworkWindow.h"
#include "OutputWindow.h"
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

	PlotTool::PlotTool() : Tool(tr("Graph Tool"))
	{
		multiplePlotsArea = 0;
		dockWidget = 0;
		setPalette(QPalette(QColor(255,255,255,255)));
		setAutoFillBackground(true);
		QVBoxLayout * layout = new QVBoxLayout;
		
		multiplePlotsArea = new QMdiArea(this);
		multiplePlotsArea->setViewMode(QMdiArea::SubWindowView);
		
		QSplitter * splitter = new QSplitter(Qt::Vertical,this);
		splitter->addWidget(multiplePlotsArea);
		
		QSplitter * functionsWidget = new QSplitter(this);
		QHBoxLayout * layout3 = new QHBoxLayout;

		QGroupBox * textEditGroup = new QGroupBox(tr(" Define functions here "));
		QHBoxLayout * layout6 = new QHBoxLayout;
		layout6->addWidget(&functionsTextEdit);
		textEditGroup->setLayout(layout6);
		functionsWidget->addWidget(textEditGroup);
		
		spinBox1.setPrefix(tr("start:   "));
		spinBox1.setRange(-1E30,1E30);
		spinBox1.setDecimals(3);
		spinBox2.setValue(10.0);
		spinBox2.setPrefix(tr("end:  "));
		spinBox2.setRange(-1E30,1E30);
		spinBox2.setDecimals(3);
		spinBox3.setValue(100);
		spinBox3.setRange(-1000,1000);
		spinBox3.setPrefix(tr("points:  "));
		
		QVBoxLayout * layout4 = new QVBoxLayout;
		
		QLabel * label = new QLabel(tr("xaxis:"));
		
		layout3->addWidget(label);
		layout3->addWidget(&xaxisLine);
		layout4->addLayout(layout3);
		layout4->addWidget(&spinBox1);
		layout4->addWidget(&spinBox2);
		layout4->addWidget(&spinBox3);
		
		QPushButton * button = new QPushButton(tr("Graph"));
		connect(button,SIGNAL(pressed()),this,SLOT(plotTexts()));
		layout4->addWidget(button);
		
		QWidget * widget = new QWidget;
		widget->setLayout(layout4);
		
		functionsWidget->addWidget(widget);
		
		QVBoxLayout * layout5 = new QVBoxLayout;
		QCheckBox * showHideButton = new QCheckBox(tr("Plot user defined functions"));
		showHideButton->setCheckable(true);
		functionsWidget->setVisible(false);
		connect(showHideButton,SIGNAL(toggled(bool)),functionsWidget,SLOT(setVisible(bool)));
		layout5->addWidget(showHideButton,1);
		layout5->addWidget(functionsWidget,20);
		
		QWidget * widget2 = new QWidget;
		widget2->setLayout(layout5);
		
		splitter->addWidget(widget2);
		splitter->setStretchFactor(0,10);
		splitter->setStretchFactor(1,1);
		splitter->setCollapsible(0,false);
		splitter->setCollapsible(1,false);
		
		layout->addWidget(splitter);
		layout->setContentsMargins(0,0,0,0);
		setLayout(layout);

		connectTCFunctions();
	}

	QSize PlotTool::sizeHint() const
	{
	    return QSize(600, 600);
	}

	bool PlotTool::setMainWindow(MainWindow * TinkercellWindow)
	{
		Tool::setMainWindow(TinkercellWindow);
		
		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
			
			//dockWidget = mainWindow->addDockingWindow(name,this,Qt::BottomDockWidgetArea, Qt::BottomDockWidgetArea);
		
			if (dockWidget)
			{
				QPoint p = mainWindow->rect().bottomRight() - QPoint(sizeHint().width(),sizeHint().height());
			
				/*dockWidget->setWindowFlags(Qt::Tool);				
				dockWidget->setAttribute(Qt::WA_ContentsPropagated);
				dockWidget->setWindowOpacity(0.8);*/
				
				QCoreApplication::setOrganizationName("TinkerCell");
				QCoreApplication::setOrganizationDomain("www.tinkercell.com");
				QCoreApplication::setApplicationName("TinkerCell");
				
				QSettings settings("TinkerCell", "TinkerCell");
				
				settings.beginGroup("SimulationEventsTool");
				//dockWidget->resize(settings.value("size", sizeHint()).toSize());
				//dockWidget->move(settings.value("pos", p).toPoint());
				
				//if (settings.value("floating", false).toBool())
				dockWidget->setFloating(true);
					
				settings.endGroup();
				dockWidget->hide();
			}
			
			//QToolBar * toolBar = new QToolBar;
                        QToolBar * toolBar = mainWindow->toolBarForTools;
			QAction * action = new QAction(tr("Plot Window"),toolBar);
			action->setIcon(QIcon(tr(":/images/graph.png")));
			
			if (dockWidget)
				connect(action,SIGNAL(triggered()),dockWidget,SLOT(show()));
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
			//mainWindow->addToolBar(toolBar);
			
			return true;
		}
		return false;	
	}
	
	void PlotTool::plot(const DataTable<qreal>& matrix,const QString& title,int x,int all)
	{	
		if (mainWindow && mainWindow->statusBar())
			mainWindow->statusBar()->showMessage(tr("Plotting...."));
		
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
		if (!all)
			pruneDataTable(const_cast< DataTable<qreal>& >(matrix),x,mainWindow);
		
		if (multiplePlotsArea)
		{
			PlotWidget * newPlot = new Plot2DWidget(this);
			newPlot->plot(matrix,title,x);
			QMdiSubWindow * window = multiplePlotsArea->addSubWindow(newPlot);
			window->setAttribute(Qt::WA_DeleteOnClose);
			window->setWindowIcon(QIcon(tr(":/images/graph.png")));
			//window->showMaximized();
			window->setWindowTitle( tr("plot ") + QString::number(multiplePlotsArea->subWindowList().size()));
			window->setVisible(true);
			
			QList<QMdiSubWindow *> subWindowList = multiplePlotsArea->subWindowList();
			for (int i=0; i < subWindowList.size(); ++i)
				if (subWindowList[i])
					subWindowList[i]->setWindowTitle( tr("plot ") + QString::number(i));
			
			multiplePlotsArea->tileSubWindows();
		}
		if (mainWindow && mainWindow->statusBar())
			mainWindow->statusBar()->showMessage(tr("Finished plotting"));
	}
	
	void PlotTool::plot3DSurface(const DataTable<qreal>& matrix,const QString& title,int meshX, int meshY)
	{	
		if (mainWindow && mainWindow->statusBar())
			mainWindow->statusBar()->showMessage(tr("Plotting...."));
		
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
		
		if (multiplePlotsArea)
		{
			Plot3DWidget * newPlot = new Plot3DWidget(this);
			newPlot->meshSizeX = meshX;
			newPlot->meshSizeY = meshX;
			newPlot->plot(matrix,title);
			QMdiSubWindow * window = multiplePlotsArea->addSubWindow(newPlot);
			window->setAttribute(Qt::WA_DeleteOnClose);
			window->setWindowIcon(QIcon(tr(":/images/graph.png")));
			//window->showMaximized();
			
			window->setWindowTitle( tr("plot ") + QString::number(multiplePlotsArea->subWindowList().size()));
			window->setVisible(true);
			
			QList<QMdiSubWindow *> subWindowList = multiplePlotsArea->subWindowList();
			for (int i=0; i < subWindowList.size(); ++i)
				if (subWindowList[i])
					subWindowList[i]->setWindowTitle( tr("plot ") + QString::number(i));
			
			multiplePlotsArea->tileSubWindows();
		}
		if (mainWindow && mainWindow->statusBar())
			mainWindow->statusBar()->showMessage(tr("Finished plotting"));
	}

	void PlotTool::plotData(QSemaphore * s, DataTable<qreal>& matrix,int x,const QString& title,int all)
	{
		QRegExp regexp(tr("(?!\\d)_(?!\\d)"));
		for (int i=0; i < matrix.cols(); ++i)
		{
			matrix.colName(i).replace(regexp,tr("."));
		}
		
		plot(matrix,title,x,all);

		if (s)
			s->release();
	}
	
	void PlotTool::surface(QSemaphore * s, DataTable<qreal>& matrix,const QString& title,int meshX, int meshY)
	{
		QRegExp regexp(tr("(?!\\d)_(?!\\d)"));
		for (int i=0; i < matrix.cols(); ++i)
		{
			matrix.colName(i).replace(regexp,tr("."));
		}
		
		plot3DSurface(matrix,title,meshX,meshY);

		if (s)
			s->release();
	}
	
	void PlotTool::getData(QSemaphore* s, DataTable<qreal>* matrix,int index)
	{
		if (matrix && multiplePlotsArea)
		{
			QList<QMdiSubWindow*> list = multiplePlotsArea->subWindowList();
			if (index < 0 || index >= list.size())
				index = list.size() - 1;
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
		void (*surface)(Matrix,const char*,int,int) , 
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
		connect(&fToS,SIGNAL(surface(QSemaphore *, DataTable<qreal>&,const QString&, int,int)),this,SLOT(surface(QSemaphore *, DataTable<qreal>&,const QString&,int,int)));
		connect(&fToS,SIGNAL(plotData(QSemaphore *,DataTable<qreal>*,int)),this,SLOT(getData(QSemaphore *,DataTable<qreal>*,int)));
	}
	
	PlotTool_FToS PlotTool::fToS;
	
	void PlotTool::_plot(Matrix a, int b,const char* c,int all)
	{
		return fToS.plot(a,b,c,all);
	}
	
	void PlotTool::_surface(Matrix m,const char* s,int x, int y) 
	{
		return fToS.surface(m,s,x,y);
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
	
	void PlotTool_FToS::surface(Matrix a0,const char* title,int x, int y)
	{
		DataTable<qreal>* dat = ConvertValue(a0);
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit surface(s,*dat,ConvertValue(title),x,y);
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
	
	void PlotTool::plotTexts()
	{
		QStringList list = functionsTextEdit.toPlainText().split(QRegExp(tr("[\\n|\\r|;]+")),QString::SkipEmptyParts);
		if (list.isEmpty()) return;
		plot(list,xaxisLine.text().replace(tr(" "),tr("")),spinBox1.value(),spinBox2.value(),spinBox3.value());
	}
	
	void PlotTool::plot(const QStringList& functions,const QString& xaxis,qreal start, qreal end, int points,const QString& title)
	{
		if (functions.isEmpty())
		{
			OutputWindow::message("Please enter one or more formulas");
			return;
		}
		if (xaxis.isEmpty() || xaxis.isNull() )
		{
			OutputWindow::message("Please specify the x-axis (one of the variables in the formula)");
			return;
		}
		if (points < 2 || end <= start)
		{
			OutputWindow::message("Inputs are incorrect. Make sure the number of points > 0 and end > start.");
			return;
		}
		
                NetworkWindow * net = currentWindow();
		
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
		QRegExp regex1(tr("[\\n\\s]"));
		QRegExp regex2(tr("\\.(?!\\d)"));
		QRegExp regex3(tr("\\.([^\\.]+)"));
		
		for (int i=0; i < functions.size(); ++i)
		{
			if (!functions[i].contains(xaxis))
			{
				OutputWindow::error(tr("equation ") + QString::number(i) + tr(" is not a function of ") + xaxis);
			}
			
			s = functions[i];
			
			s.replace(regex1,tr(""));
			s.replace(regex2,tr("_"));
			
			mu::Parser parser;
			parser.SetExpr(s.toAscii().data());
			
			try
			{
				parser.SetVarFactory(AddVariable, 0);
				parser.Eval();
                                if (net)
				{
					mu::varmap_type variables = parser.GetVar();
					mu::varmap_type::const_iterator item = variables.begin();
                                        SymbolsTable & symbolsTable = net->symbolsTable;
					for (; item!=variables.end(); ++item)
					{
						n = tr(item->first.data());
						n.replace(tr("_"),tr("."));
						if (symbolsTable.handlesFullName.contains(n) && (handle = symbolsTable.handlesFullName[n]))
						{
							if (handle->data && handle->hasNumericalData(tr("Initial Value")))
                                                                parser.DefineVar(item->first.data(), &(handle->data->numericalData[tr("Initial Value")].value(0,0)));
								
							continue;
						}
						
						if (symbolsTable.handlesFirstName.contains(n) && (handle = symbolsTable.handlesFirstName[n]))
						{
							if (handle->data && handle->hasNumericalData(tr("Initial Value")))
                                                                parser.DefineVar(item->first.data(), &(handle->data->numericalData[tr("Initial Value")].value(0,0)));
								
							continue;
						}
						
						if (symbolsTable.dataRowsAndCols.contains(n) && (handle = symbolsTable.dataRowsAndCols[n].first))
						{
							p = symbolsTable.dataRowsAndCols[n].second;
							regex3.indexIn(n);
							
							if (regex3.numCaptures() > 0)
								n = regex3.cap(1);
							
							if (handle->data && handle->hasNumericalData(p) 
								&& handle->data->numericalData[p].getRowNames().contains(n))
								{
									parser.DefineVar(item->first.data(), &(handle->data->numericalData[p].value(n,0)));
								}
						}
					}
				}
			}
			catch(mu::Parser::exception_type &e)
			{
				OutputWindow::error("cannot parse: " + s);
				continue;
			}
			
			s = xaxis;			
			s.replace(regex1,tr(""));
			s.replace(regex2,tr("_"));
			
			double x = start;
			try
			{
				parser.DefineVar(s.toAscii().data(), &x);
			}
			catch(mu::Parser::exception_type &e)
			{
				OutputWindow::error("cannot parse: " + s);
				continue;
			}
			
			for (int j=0; j < data.rows(); ++j)
			{
				data.value(j,0) = x;
				data.value(j,i+1) = parser.Eval();
				x += dx;
			}
		}
		this->plot(data,title,0,1);
	}
}
