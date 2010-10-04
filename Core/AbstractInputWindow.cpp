/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines an abstract class that is used to create interfaces for C functions.
LPSolveInputWindow is a good example.

****************************************************************************/
#include "AbstractInputWindow.h"
#include "CThread.h"

namespace Tinkercell
{

	AbstractInputWindow::AbstractInputWindow(const QString& name, CThread * thread)
		: Tool(name), cthread(thread), dockWidget(0)
	{
		if (cthread)
			connect(this,SIGNAL(updateThread()),cthread,SLOT(update()));
	}

	void AbstractInputWindow::setThread(CThread * thread)
	{
		if (cthread != thread)
		{
			cthread = thread;
			if (cthread)
				connect(this,SIGNAL(updateThread()),cthread,SLOT(update()));
			
			if (mainWindow && cthread)
				disconnect(mainWindow,SIGNAL(historyChanged(int)),cthread,SLOT(update()));
		}
	}

	CThread * AbstractInputWindow::thread() const
	{
		return cthread;
	}

	void AbstractInputWindow::loadAPI(Tool*)
	{
		if (mainWindow && cthread && cthread->library())
		{
			QSemaphore * s = new QSemaphore(1);
			s->acquire();
			mainWindow->setupNewThread(s,cthread->library());
			s->acquire();
			s->release();
		}
	}

	void AbstractInputWindow::setInput(const DataTable<qreal>& dat)
	{
		if (cthread)
			cthread->setArg(dat);
	}

	bool AbstractInputWindow::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			if (cthread)
				disconnect(mainWindow,SIGNAL(historyChanged(int)),cthread,SLOT(update()));
			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSignal(const QWidget*)));
			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(loadAPI(Tool*)));
			loadAPI(0);

			setWindowTitle(name);
			setWindowIcon(QIcon(tr(":/images/play.png")));
			dockWidget = mainWindow->addToolWindow(this,MainWindow::DockWidget,Qt::BottomDockWidgetArea,Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea,false);
			if (dockWidget)
			{
				//dockWidget->setAttribute(Qt::WA_ContentsPropagated);
				dockWidget->hide();
				dockWidget->setFloating(true);
				//dockWidget->setWindowOpacity(0.8);
			}
		}
		return true;
	}

	void AbstractInputWindow::escapeSignal(const QWidget*)
	{
		if (dockWidget)
			dockWidget->hide();
		else
			this->hide();
	}

	void AbstractInputWindow::exec()
	{
		if (cthread)
			cthread->start();
	}
	
	void AbstractInputWindow::enterEvent ( QEvent * )
	{
		emit updateThread();
	}

	/******************************
	SIMPLE INPUT WINDOW
	*******************************/

	QHash<QString,SimpleInputWindow*> SimpleInputWindow::inputWindows;

	SimpleInputWindow::SimpleInputWindow(MainWindow * main, const QString& title, const QString& lib, const QString& funcName, const DataTable<qreal>& data)
		: AbstractInputWindow(title)
	{
		CThread * thread = new CThread(main, lib);
		thread->setMatrixFunction(funcName.toAscii().data());
		this->dataTable = data;

		setThread(thread);

		QPushButton * runButton = new QPushButton(this);
		runButton->setIcon(QIcon(":/images/play.png"));
		connect(runButton,SIGNAL(released()),this,SLOT(exec()));

		QToolButton * addButton = new QToolButton(this);
		addButton->setIcon(QIcon(":/images/plus.png"));
		connect(addButton,SIGNAL(released()),this,SLOT(addRow()));

		QToolButton * removeButton = new QToolButton(this);
		removeButton->setIcon(QIcon(":/images/minus.png"));
		connect(removeButton,SIGNAL(released()),this,SLOT(removeRow()));

		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(&tableWidget,1);

		QHBoxLayout * hlayout = new QHBoxLayout;
		hlayout->addWidget(runButton,3);
		hlayout->addStretch(6);
		hlayout->addWidget(addButton,0);
		hlayout->addWidget(removeButton,0);
		layout->addLayout(hlayout,0);

		setLayout(layout);

		tableWidget.setAlternatingRowColors(true);
		tableWidget.setItemDelegate(&delegate);
		tableWidget.setEditTriggers ( QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed );

		setupDisplay(data);

		inputWindows[title.toLower()] = this;

		setMainWindow(main);
	}

	void SimpleInputWindow::exec()
	{
		if (cthread)
		{
			int rows = tableWidget.rowCount(), cols = tableWidget.columnCount();
			
			for (int i=0; i < rows; ++i)
				for (int j=0; j < cols; ++j)
					dataChanged(i,j);
			cthread->setArg(dataTable);
			cthread->start();
		}

	}

	SimpleInputWindow::SimpleInputWindow(MainWindow * main, const QString& title, void (*f)(tc_matrix), const DataTable<qreal>& data)
		: AbstractInputWindow(title)
	{
		CThread * thread = new CThread(main,0);
		thread->setFunction(f);
		this->dataTable = data;
		setThread(thread);

		QPushButton * runButton = new QPushButton(this);
		runButton->setIcon(QIcon(":/images/play.png"));
		connect(runButton,SIGNAL(released()),this,SLOT(exec()));

		QToolButton * addButton = new QToolButton(this);
		addButton->setIcon(QIcon(":/images/plus.png"));
		connect(addButton,SIGNAL(released()),this,SLOT(addRow()));

		QToolButton * removeButton = new QToolButton(this);
		removeButton->setIcon(QIcon(":/images/minus.png"));
		connect(removeButton,SIGNAL(released()),this,SLOT(removeRow()));

		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(&tableWidget,1);

		QHBoxLayout * hlayout = new QHBoxLayout;
		hlayout->addWidget(runButton,3);
		hlayout->addStretch(6);
		hlayout->addWidget(addButton,0);
		hlayout->addWidget(removeButton,0);
		layout->addLayout(hlayout,0);

		setLayout(layout);

		tableWidget.setAlternatingRowColors(true);
		tableWidget.setItemDelegate(&delegate);
		tableWidget.setEditTriggers ( QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed );

		setupDisplay(data);

		inputWindows[title.toLower()] = this;

		setMainWindow(main);
	}

	void SimpleInputWindow::AddOptions(const QString& title, int i, int j, const QStringList& options0)
	{
		if (inputWindows.contains(title.toLower()))
		{
			SimpleInputWindow * win = inputWindows[title.toLower()];
			if (!win) return;

			QStringList options = options0;
			
			for (int k=0; k < options.size(); ++k)
				options[k].replace(tr("_"),tr("."));

			win->delegate.options.value(i,j) = options;

			if (win->dataTable.value(i,j) >= options.size() || win->dataTable.value(i,j) < 0)
				win->dataTable.value(i,j) = -1;
			else
			if (win->tableWidget.item(i,j) && !options.contains(win->tableWidget.item(i,j)->text()))
				win->tableWidget.item(i,j)->setText(options[ (int)(win->dataTable.value(i,j)) ]);
		}
	}
	
	void SimpleInputWindow::AddOptions(const QString& title, int i, int j)
	{
		SimpleInputWindow::AddOptions(title,i,j,QStringList() << tr(""));
	}

	SimpleInputWindow::SimpleInputWindow() : AbstractInputWindow() { }

	SimpleInputWindow::SimpleInputWindow(const SimpleInputWindow&) : AbstractInputWindow() { }

	void SimpleInputWindow::CreateWindow(MainWindow * main, const QString& title, const QString& lib, const QString& funcName, const DataTable<qreal>& data)
	{
		if (!main || lib.isEmpty() || funcName.isEmpty()) return;
		SimpleInputWindow * inputWindow = 0;
		if (SimpleInputWindow::inputWindows.contains(title.toLower()))
		{
			inputWindow = SimpleInputWindow::inputWindows.value(title.toLower());
		}
		else
		{
			inputWindow = new SimpleInputWindow(main,title,lib,funcName,data);
		}
		if (inputWindow)
		{
			if (inputWindow->dockWidget)
				inputWindow->dockWidget->show();
			else
				inputWindow->show();
		}
	}

	void SimpleInputWindow::CreateWindow(MainWindow * main, const QString& title, void (*f)(tc_matrix), const DataTable<qreal>& data)
	{
		if (!main || title.isEmpty() || !f) return;
		SimpleInputWindow * inputWindow = 0;
		if (SimpleInputWindow::inputWindows.contains(title.toLower()))
		{
			inputWindow = SimpleInputWindow::inputWindows.value(title.toLower());
		}
		else
		{
			inputWindow = new SimpleInputWindow(main,title,f,data);
		}
		if (inputWindow)
		{
			if (inputWindow->dockWidget)
				inputWindow->dockWidget->show();
			else
				inputWindow->show();
		}
	}

	void SimpleInputWindow::addRow()
	{
		int r = tableWidget.rowCount();
		tableWidget.setRowCount(r+1);

		int n = r;
		QString name = tr("row") + QString::number(n);

		if (dataTable.rows() > 0)
		{
			name = dataTable.rowName( dataTable.rows()-1 );
			if (name.at( name.length()-1 ).isNumber())
			{
				name.chop(1);
				name = name + QString::number(n);
			}
			else
			{
				name = tr("row") + QString::number(n);
			}
		}
		QStringList names = dataTable.rowNames();

		while (names.contains(name))  //find a unique row name
		{
			++n;
			name.chop(1);
			name = name + QString::number(n);
		}

		tableWidget.setVerticalHeaderItem ( r, new QTableWidgetItem(name) );
		dataTable.insertRow(r,name);

		for (int i=0; i < tableWidget.columnCount(); ++i)
		{
			if (r > 0)
			{
				if (tableWidget.item(r-1,i))
					tableWidget.setItem(r,i,new QTableWidgetItem( tableWidget.item(r-1,i)->text() ));
				dataTable.value(r,i) = dataTable.value(r-1,i);
				delegate.options.value(r,i) = delegate.options.value(r-1,i);
			}
			else
			{
				tableWidget.setItem(r,i,new QTableWidgetItem("0.0"));
				dataTable.value(r,i) = 0.0;
				delegate.options.value(r,i) = QStringList();
			}
		}
	}

	void SimpleInputWindow::removeRow()
	{
		int r = tableWidget.rowCount()-1;
		tableWidget.removeRow(r);
		dataTable.removeRow(r);
	}

	void SimpleInputWindow::dataChanged(int i,int j)
	{
		QString s = tableWidget.item(i,j)->text();
		
		QStringList options = delegate.options.value(i,j);
		
		if (options.size() > 1 && options.contains(s))
		{
			dataTable.value(i,j) = delegate.options.value(i,j).indexOf(s);
			return;
		}
		
		if (options.size() == 1)
		{
			if (s.toLower() == QObject::tr("yes") || s.toLower() == QObject::tr("true"))
				dataTable.value(i,j) = 1.0;
			else
				dataTable.value(i,j) = 0.0;
			return;
		}

		bool ok;

		double d = s.toDouble(&ok);

		if (ok)
			dataTable.value(i,j) = d;
	}

	void SimpleInputWindow::setupDisplay(const DataTable<qreal>& table)
	{
		tableWidget.disconnect();

		dataTable = table;
		comboBoxes.clear();
		int r = dataTable.rows();
		int c = dataTable.columns();
		tableWidget.clear();
		tableWidget.setColumnCount(c);
		tableWidget.setRowCount(r);
		tableWidget.setVerticalHeaderLabels(dataTable.rowNames());
		tableWidget.setHorizontalHeaderLabels(dataTable.columnNames());

		for (int i=0; i < dataTable.rows(); ++i)
			for (int j=0; j < dataTable.columns(); ++j)
			{
				tableWidget.setItem(i,j,new QTableWidgetItem(QString::number(dataTable.at(i,j))));
				delegate.options.value(i,j) = QStringList();
			}

		connect(&tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(dataChanged(int,int)));
	}

	void SimpleInputWindow::comboBoxChanged(int)
	{
	}

	void SimpleInputWindow::enterEvent ( QEvent * event )
	{
		//if (dockWidget)
		//dockWidget->setWindowOpacity(1.0);
	}

	void SimpleInputWindow::leaveEvent ( QEvent * event )
	{
		//if (dockWidget)
		//dockWidget->setWindowOpacity(0.8);
	}

	/************************************
	ITEM DELEGATE FOR THE INPUT TABLE
	*************************************/

	SimpleInputWindow::ComboBoxDelegate::ComboBoxDelegate(QObject *parent) : QItemDelegate(parent)
	{
	}

	QWidget * SimpleInputWindow::ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex & index) const
	{
		QStringList strings = options.at(index.row(),index.column());
		if (strings.size() > 1)
		{
			QComboBox *editor = new QComboBox(parent);
			editor->addItems(strings);
			return editor;
		}
		else
		if (strings.size() == 1)
		{
			QCheckBox * editor = new QCheckBox(tr(""),parent);
			return editor;
		}
		else
		{
			/*QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
			editor->setMinimum(-1e300);
			editor->setMaximum(1E300);
			editor->setDecimals(10);*/
			QLineEdit *editor = new QLineEdit(parent);
			return editor;
		}
	}

	void SimpleInputWindow::ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
	{
		QStringList strings = options.at(index.row(),index.column());
		if (strings.size() > 1)
		{
			QString value = index.model()->data(index, Qt::EditRole).toString();
			QComboBox *combo = static_cast<QComboBox*>(editor);
			combo->setCurrentIndex(strings.indexOf(value));
		}
		else
		if (strings.size() == 1) //checkbox
		{
			QString value = index.model()->data(index, Qt::EditRole).toString();
			QCheckBox * check = static_cast<QCheckBox*>(editor);
			check->setChecked(value.toLower().contains(tr("yes")));
		}
		else
		{
			double value = index.model()->data(index, Qt::EditRole).toDouble();
			/*QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
			spinBox->setValue(value);*/
			QLineEdit * lineEdit = static_cast<QLineEdit*>(editor);
			lineEdit->setText(QString::number(value));
		}
	}

	void SimpleInputWindow::ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
		QStringList strings = options.at(index.row(),index.column());
		if (strings.size() > 1)
		{
			QComboBox *combo = static_cast<QComboBox*>(editor);
			QString value = combo->currentText();
			model->setData(index, value, Qt::EditRole);
		}
		else
		if (strings.size() == 1)
		{
			QCheckBox * check = static_cast<QCheckBox*>(editor);
			if (check->isChecked())
				model->setData(index, tr("True"), Qt::EditRole);
			else
				model->setData(index, tr("False"), Qt::EditRole);
		}
		else
		{
			/*QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
			spinBox->interpretText();
			double value = spinBox->value();*/
			QLineEdit * lineEdit = static_cast<QLineEdit*>(editor);

			bool ok;
			double value = lineEdit->text().toDouble(&ok);
			if (ok)
			{
				model->setData(index, value, Qt::EditRole);
			}
		}
	}

	void SimpleInputWindow::ComboBoxDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
	{
		editor->setGeometry(option.rect);
	}

};


