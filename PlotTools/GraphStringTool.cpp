/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This class adds the "attributes" data to each item in Tinkercell.
 Two types of attributes are added -- "Numerical Attributes" and "Text Attributes".
 Attributes are essentially a <name,value> pair that are used to characterize an item.
 
 The AssignmentFunctionsTool also comes with two GraphicalTools, one for text attributes and one
 for numerical attributes. The buttons are drawn as PartGraphicsItems using the datasheet.xml and
 textsheet.xml files that define the PartGraphicsItems.
 
****************************************************************************/


#include "GraphicsScene.h"
#include "MainWindow.h"
//#include "PartGraphicsItem.h"
#include "CThread.h"
#include "OutputWindow.h"
#include "GraphStringTool.h"
#include "ModelSummaryTool.h"
#include "DynamicLibraryMenu.h"
#include "muParser.h"
#include "muParserInt.h"
#include <QRegExp>
#include <QInputDialog>

namespace Tinkercell
{
	GraphStringTool::GraphStringTool() : Tool(tr("Graph String Tool"))
	{
		plotTool = 0;
		QFont font = this->font();
		font.setPointSize(12);
		functionsListWidget.setFont(font);
		
		QHBoxLayout * spinBoxLayout = new QHBoxLayout;
		spinBoxLayout->addWidget(&comboBox);
		spinBoxLayout->addWidget(&spinBox1);
		spinBox1.setRange(-1E300,1E300);
		spinBox1.setPrefix(tr("start at "));
		spinBox1.setValue(0.0);
		spinBoxLayout->addWidget(&spinBox2);
		spinBox2.setRange(-1E300,1E300);
		spinBox2.setValue(10.0);
		spinBox2.setPrefix(tr("end at "));
		
		QHBoxLayout * actionsLayout = new QHBoxLayout;
		
		QToolButton * addFuncAction = new QToolButton(this);
		addFuncAction->setIcon(QIcon(":/images/plus.png"));
		addFuncAction->setToolTip(tr("Add new function"));
		connect(addFuncAction,SIGNAL(pressed()),this,SLOT(addFunction()));
		
		QToolButton * removeFuncAction = new QToolButton(this);
		removeFuncAction->setIcon(QIcon(":/images/minus.png"));
		removeFuncAction->setToolTip(tr("Remove selected functions(s)"));
		connect(removeFuncAction,SIGNAL(pressed()),this,SLOT(removeFunctions()));
		
		QToolButton * plotAction = new QToolButton(this);
		plotAction->setIcon(QIcon(":/images/graph.png"));
		plotAction->setToolTip(tr("Plot functions(s)"));
		connect(plotAction,SIGNAL(pressed()),this,SLOT(plotFunctions()));
		
		actionsLayout->addWidget(addFuncAction);
		actionsLayout->addWidget(removeFuncAction);
		actionsLayout->addWidget(plotAction);
		actionsLayout->addStretch(1);
		
		QGroupBox * groupBox = new QGroupBox(tr("Functions to plot"),this);

		QVBoxLayout * eventBoxLayout = new QVBoxLayout;
		eventBoxLayout->addWidget(&functionsListWidget,1);
		eventBoxLayout->addLayout(spinBoxLayout);
		eventBoxLayout->addLayout(actionsLayout);
		groupBox->setLayout(eventBoxLayout);

		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(groupBox);
		setLayout(layout);
		
		connect(&functionsListWidget,SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(editFunctionsList(QListWidgetItem*)));
		
		dockWidget = 0;
	}
	
	void GraphStringTool::visibilityChanged(bool)
	{
		if (dockWidget && dockWidget->widget() != this)
			dockWidget->setWidget(this);
	}
	
	void GraphStringTool::editFunctionsList(QListWidgetItem* item)
	{
		if (!item) return;
		QString f = QInputDialog::getText(this,tr("Add function"),tr("Enter function to plot"),QLineEdit::Normal,item->text());
		if (f.isNull() || f.isEmpty()) return;
		
		item->setText(f);		
	}
	
	void GraphStringTool::addFunction()
	{
		QString f = QInputDialog::getText(this,tr("Add function"),tr("Enter function to plot"));
		if (f.isNull() || f.isEmpty()) return;
		
		functionsListWidget.addItem(f);		
	}
	
	void GraphStringTool::removeFunctions()
	{
		QStringList functions;
		for (int i=0; i < functionsListWidget.count(); ++i)
			if (functionsListWidget.item(i) && functionsListWidget.currentRow() != i)
				functions += functionsListWidget.item(i)->text();
		functionsListWidget.clear();
		functionsListWidget.addItems(functions);
	}
	
	void GraphStringTool::keyPressEvent(QKeyEvent* keyEvent)
	{
		if (keyEvent->key() == Qt::Key_Delete)
		{
			if (functionsListWidget.hasFocus())
				removeFunctions();
		}
	}
	
	bool GraphStringTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));
			//connect(mainWindow,SIGNAL(historyUpdate(int)),this,SLOT(historyUpdate(int)));

			dockWidget = mainWindow->addDockingWindow(name,this,Qt::BottomDockWidgetArea,Qt::NoDockWidgetArea);
			if (dockWidget)
			{
				connect(dockWidget,SIGNAL(visibilityChanged(bool)),this,SLOT(visibilityChanged(bool)));
				dockWidget->setWindowTitle(tr("Quick graphing tool"));
				dockWidget->setWindowFlags(Qt::Tool);
				dockWidget->setAttribute(Qt::WA_ContentsPropagated);
				dockWidget->setPalette(QPalette(QColor(255,255,255,255)));
				dockWidget->setAutoFillBackground(true);
				dockWidget->setWindowOpacity(0.9);
				
				dockWidget->move(mainWindow->geometry().center());

				dockWidget->setFloating(true);
				dockWidget->hide();
			}
		}
		return (mainWindow != 0);
	}
	
	void GraphStringTool::historyUpdate(int)
	{
		
	}
	
	void GraphStringTool::toolLoaded(Tool*)
	{
		static bool connected1 = false;
		static bool connected2 = false;
		if (connected1 && connected2) return;
		
		if (!connected1 && mainWindow && mainWindow->tool(tr("Dynamic Library Menu")) && mainWindow->tool(tr("Plot")))
		{
			QWidget * widget = mainWindow->tool(tr("Dynamic Library Menu"));
			//DynamicLibraryMenu * lib = static_cast<DynamicLibraryMenu*>(widget);
			//connect(this,SIGNAL(runPythonCode(const QString&)),lib,SLOT(runPythonCode(const QString&)));
			
			widget = mainWindow->tool(tr("Plot"));
			PlotTool * plot = static_cast<PlotTool*>(widget);
			connect(this,SIGNAL(plot(const DataTable<qreal>&,int,const QString&,int)),plot,SLOT(plot(const DataTable<qreal>&,int,const QString&,int)));
			connect(this,SIGNAL(changeData(const DataTable<qreal>&)),plot,SLOT(changeData(const DataTable<qreal>&)));
			
			plotTool = plot;
			connected1 = true;
		}
		
		if (!connected2 && mainWindow && mainWindow->tool(tr("Model Summary")))
		{
			QWidget * widget = mainWindow->tool(tr("Model Summary"));
			ModelSummaryTool * modelSummary = static_cast<ModelSummaryTool*>(widget);
			connect(modelSummary,SIGNAL(displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)),
					this,SLOT(displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)));
			connected2 = true;
		}
	}
	
	void GraphStringTool::displayModel(QTabWidget& widgets, const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations)
	{
		if (equations.size() > 0)
		{
			if (dockWidget && dockWidget->isVisible())
				dockWidget->hide();
				
			graph(equations.values(), constants.keys(),constants.values());
			
			bool alreadyExists = false;
			
			for (int i=0; i < widgets.count(); ++i)
				if (widgets.widget(i) == this)
				{
					alreadyExists = true;
					if (plotTool && plotTool->isVisible())
						plotFunctions();
					break;
				}
			
			if (!alreadyExists)
				widgets.addTab(this,tr("Quick graphs"));
		}
		else
			if (dockWidget && dockWidget->widget() != this)
				dockWidget->setWidget(this);
	}
	
	static double d = 2.0;
	static double* AddVariable(const char* s, void*)
	{
		return &d;
	}
	
	void GraphStringTool::plotFunctions()
	{
		QRegExp regex(tr("\\.(?!\\d)"));
		comboBoxSelection = comboBox.currentText();
		
		QStringList functions;
		QString s;
		
		int xaxis = names.indexOf(comboBoxSelection);
		if (xaxis < 0 || values.size() <= xaxis) return;
		
		qreal &x = values[xaxis];
		
		QStringList names = this->names;
		
		for (int i=0; i < names.size(); ++i)
		{
			names[i].replace(regex,tr("_"));
		}
		
		QList<mu::Parser> parsers;
		
		for (int i=0; i < functionsListWidget.count(); ++i)
			if (functionsListWidget.item(i))
			{
				s = functionsListWidget.item(i)->text();
				s.replace(regex,tr("_"));
				
				mu::Parser parser;
				parser.SetExpr(s.toAscii().data());
				for (int j=0; j < names.size() && j < values.size(); ++j)
					parser.DefineVar(names[j].toAscii().data(), &values[j]);
				parser.SetVarFactory(AddVariable, 0);
				try
				{
					parser.Eval();
					parsers << parser;
					functions += functionsListWidget.item(i)->text();
				}
				catch(mu::Parser::exception_type &e)
				{
				}
			}
		
		DataTable<qreal> data;
		data.resize(100,1+functions.size());
		data.setColNames(QStringList() << comboBoxSelection << functions);
		
		qreal start = spinBox1.value();
		qreal end = spinBox2.value();
		if (end <= start) end = start + 1.0;
		qreal dx = (end - start)/100.0;
		
		x = start;
		for (int i=0; i < data.rows(); ++i)
		{
			data.value(i,0) = x;
			for (int j=1; j < data.cols(); ++j)
			{
				data.value(i,j) = parsers[j-1].Eval();
			}
			x += dx;
		}
		
		emit plot(data,0,tr(""),1);
		
	}
	/*
	void GraphStringTool::plotFunctions()
	{
		comboBoxSelection = comboBox.currentText();
		
		QStringList functions;
		for (int i=0; i < functionsListWidget.count(); ++i)
			if (functionsListWidget.item(i))
				functions += functionsListWidget.item(i)->text();
		
		QString xaxis = comboBox.currentText();
		qreal start = spinBox1.value();
		qreal end = spinBox2.value();
		int points = 100;
		
		QString pyString = tr("import pytc\n");
		
		QString name;
		QRegExp regex(tr("\\.(?!\\d)"));
		for (int i=0; i < names.size() && i < values.size(); ++i)
		{
			name = names[i];
			name.replace(regex,tr("_"));
			pyString += name + tr("=") + QString::number(values[i]) + tr(";\n");	
		}
		
		QString xvar = xaxis;
		xvar.replace(regex,tr("_"));
		
		qreal dx = (end - start)/points;
		if (dx <= 0) return;
		int n = functions.size() + 1;
		
		pyString += tr("plot_array = range(0,") + QString::number(n*points) + tr(");\ndx = ") + 
					QString::number(dx) + tr(";\nfor i in range(0,") + QString::number(points) + tr("):\n");
		pyString += tr("     ") + xvar + tr(" = i * dx;\n");
		pyString += tr("     plot_array[i*") + QString::number(n) + tr("] = ") + xvar + tr(";\n");
		QStringList substrs;
		QString f;
		for (int i=0; i < (n-1); ++i)
		{
			f = functions[i];
			if (f.length() > 10)
				substrs += f.left(10) + tr("...");
			else
				substrs += f;
			
			f.replace(regex,tr("_"));
			f.replace(tr("^"),tr("**"));
			pyString += tr("     plot_array[i*") + QString::number(n) + tr("+") + QString::number(i+1) + tr("] = ") + f + tr(";\n");
		}
		
		substrs.push_front(xvar);
		
		pyString += tr("pytc.plot(('") + substrs.join("','") + tr("',),plot_array,0,'');\n");
		
		emit runPythonCode(pyString);
		//COutputWindow::Display(mainWindow,"",pyString,1);
	}*/
	
	void GraphStringTool::graph(const QStringList& functions, const QStringList& names, const QList<qreal>& values)
	{
		functionsListWidget.clear();
		functionsListWidget.addItems(functions);
		this->names = names;
		comboBox.clear();
		
		comboBox.addItems(names);
		if (names.contains(comboBoxSelection))
			comboBox.setCurrentIndex(names.indexOf(comboBoxSelection));
		
		this->values = values;
		
		//if (dockWidget && !dockWidget->isVisible())
			//dockWidget->setVisible(true);
	}
	
	QSize GraphStringTool::sizeHint() const
	{
		return QSize(100, 100);
	}

	
}


