/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This class adds the "attributes" data to each item in Tinkercell.
 Two types of attributes are added -- "Numerical Attributes" and "Text Attributes".
 Attributes are essentially a <name,value> pair that are used to characterize an item.
 
 The ViewTablesTool also comes with two GraphicalTools, one for text attributes and one
 for numerical attributes. The buttons are drawn as NodeGraphicsItems using the datasheet.xml and
 textsheet.xml files that define the NodeGraphicsItems.
 
****************************************************************************/


#include "GraphicsScene.h"
#include "NetworkWindow.h"
#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ViewTablesTool.h"
#include <QGroupBox>
#include <QSplitter>

namespace Tinkercell
{
	void ViewTablesTool::select(int)
	{
		NetworkWindow * win = currentWindow();
		if (!win) return;

		QList<ItemHandle*> list = win->selectedHandles();
		if (list.size() == 1 && list[0])
		{
			if (dockWidget && dockWidget->widget() != this)
				dockWidget->setWidget(this);

			itemHandle = list[0];

			openedByUser = true;

			updateList();
			if (parentWidget() != 0)
			{
				if (parentWidget()->isVisible())
					openedByUser = false;
				else
					parentWidget()->show();
			}
			else
			{
				if (isVisible())
					openedByUser = false;
				else
					show();
			}
		}
	}

	void ViewTablesTool::deselect(int)
	{
		if (openedByUser && (!dockWidget || dockWidget->isFloating()))
		{
			openedByUser = false;
			if (parentWidget() != 0)
				parentWidget()->hide();
			else
				hide();
		}
	}

	ViewTablesTool::GraphicsItem2::GraphicsItem2(Tool * tool) : Tool::GraphicsItem(tool)
	{
	}

	void ViewTablesTool::GraphicsItem2::visible(bool b)
	{
		if (!tool)
		{
			GraphicsItem::visible(false);
			return;
		}

		GraphicsScene * scene = tool->currentScene();
		if (!scene || scene->selected().size() != 1)
		{
			GraphicsItem::visible(false);
			return;
		}

		GraphicsItem::visible(b);
	}


	void ViewTablesTool::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& list, QPointF , Qt::KeyboardModifiers )
	{
		if (!scene) return;
		
		if (list.size() > 0 && !getHandle(list[0]) &&
			(isVisible() || (parentWidget() && parentWidget() != mainWindow && parentWidget()->isVisible())))
		{
			itemHandle = getHandle(list[0]);
			updateList();
		}
	}
	
	bool ViewTablesTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow)
		{
			
			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				         this,SLOT(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
			
			connect(mainWindow,SIGNAL(itemsInserted(NetworkWindow*, const QList<ItemHandle*>&)),
						  this, SLOT(itemsInserted(NetworkWindow*, const QList<ItemHandle*>&)));
			
			setWindowTitle(name);
			setWindowIcon(QIcon(tr(":/images/new.png")));
			dockWidget = mainWindow->addToolWindow(this,MainWindow::DockWidget,Qt::BottomDockWidgetArea,Qt::NoDockWidgetArea);
			
			if (dockWidget)
			{
				dockWidget->setFloating(true);
				dockWidget->move(mainWindow->geometry().bottomRight() - QPoint(sizeHint().width()*2,sizeHint().height()*2));
				dockWidget->hide();
			}
		}
		return (mainWindow != 0);
	}

	void ViewTablesTool::itemsInserted(NetworkWindow* , const QList<ItemHandle*>& handles)
	{
		for (int i=0; i < handles.size(); ++i)
		{
			if (handles[i] && handles[i]->family() && !handles[i]->tools.contains(this))
				handles[i]->tools += this;
		}
	}

	ViewTablesTool::ViewTablesTool() : Tool(tr("View Tables"))
	{
		QString appDir = QCoreApplication::applicationDirPath();
		openedByUser = false;
		NodeGraphicsReader reader;
		reader.readXml(&item,appDir + tr("/OtherItems/grid2.xml"));

		item.normalize();
		item.scale(35.0/item.sceneBoundingRect().width(),35.0/item.sceneBoundingRect().height());
		graphicsItems += new GraphicsItem2(this);
		graphicsItems[0]->setToolTip(tr("View all tables"));
		graphicsItems[0]->addToGroup(&item);

		itemHandle = 0;
		QFont font = this->font();
		font.setPointSize(12);
		numericalTables.setFont(font);
		textTables.setFont(font);
		
		textEdit = new QTextEdit(this);
		textEdit->setFont(font);
		
		QGroupBox * groupBox1 = new QGroupBox(tr(" Numerical tables "),this);
		QGroupBox * groupBox2 = new QGroupBox(tr(" Text tables "),this);
	
		QHBoxLayout * layout1 = new QHBoxLayout;
		layout1->addWidget(&numericalTables,1);
		groupBox1->setLayout(layout1);
		
		QHBoxLayout * layout2 = new QHBoxLayout;
		layout2->addWidget(&textTables,1);
		groupBox2->setLayout(layout2);
		
		QVBoxLayout * layout3 = new QVBoxLayout;
		layout3->addWidget(groupBox1);
		layout3->addWidget(groupBox2);
		
		QWidget * widget = new QWidget;
		widget->setLayout(layout3);

		dockWidget = 0;
		
		QSplitter * splitter = new QSplitter(Qt::Horizontal,this);
		
		splitter->addWidget( widget );
		splitter->addWidget( textEdit );
		splitter->setStretchFactor(0,1);
		splitter->setStretchFactor(1,2);
		textEdit->setReadOnly(true);
		
		QHBoxLayout * layout = new QHBoxLayout;
		layout->addWidget(splitter);
		setLayout(layout);
		
		headerFormat.setForeground(QColor("#003AA3"));
		
		regularFormat.setFontWeight(QFont::Bold);
		regularFormat.setForeground(QColor("#252F41"));
		
		connect(&numericalTables,SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
				this,SLOT(currentNumericalItemChanged(QListWidgetItem *, QListWidgetItem *)));
		
		connect(&textTables,SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
				this,SLOT(currentTextItemChanged(QListWidgetItem *, QListWidgetItem *)));
	}
	
	void ViewTablesTool::currentNumericalItemChanged(QListWidgetItem * item, QListWidgetItem *)
	{
		if (!item || !itemHandle || !itemHandle->hasNumericalData(item->text())) return;
		
		DataTable<qreal>& table = itemHandle->data->numericalData[item->text()];
		
		textEdit->clear();
		
		QTextCursor cursor = textEdit->textCursor();
		
		QString outputs;
		
		QStringList colnames = table.getColNames(), rownames = table.getRowNames();
		
		for (int i=0; i < colnames.size(); ++i)
		{
			outputs += tr("\t") + colnames.at(i);
		}
		
		cursor.setCharFormat(headerFormat);
		cursor.insertText(outputs + tr("\n"));
		
		for (int i=0; i < table.rows(); ++i)
		{
			cursor.setCharFormat(headerFormat);
			cursor.insertText(rownames.at(i));
			outputs = tr("");
			
			for (int j=0; j < table.cols(); ++j)
			{
				outputs += tr("\t") + QString::number(table.at(i,j));
			}
			
			cursor.setCharFormat(regularFormat);
			cursor.insertText(outputs + tr("\n"));
		}
	}
	
	void ViewTablesTool::currentTextItemChanged(QListWidgetItem * item, QListWidgetItem *)
	{
		if (!item || !itemHandle || !itemHandle->hasTextData(item->text())) return;
		
		DataTable<QString>& table = itemHandle->data->textData[item->text()];
		
		textEdit->clear();
		
		QTextCursor cursor = textEdit->textCursor();
		
		QString outputs;
		
		QStringList colnames = table.getColNames(), rownames = table.getRowNames();
		
		for (int i=0; i < colnames.size(); ++i)
		{
			outputs += tr("\t") + colnames.at(i);
		}
		
		cursor.setCharFormat(headerFormat);
		cursor.insertText(outputs + tr("\n"));
		
		for (int i=0; i < table.rows(); ++i)
		{
			cursor.setCharFormat(headerFormat);
			cursor.insertText(rownames.at(i));
			outputs = tr("");
			
			for (int j=0; j < table.cols(); ++j)
			{
				outputs += tr("\t") + (table.at(i,j));
			}
			
			cursor.setCharFormat(regularFormat);
			cursor.insertText(outputs + tr("\n"));
		}
		
	}
	
	QSize ViewTablesTool::sizeHint() const
	{
		return QSize(600, 300);
	}

	void ViewTablesTool::updateList()
	{
		numericalTables.clear();
		textTables.clear();
		
		if (!textEdit || !itemHandle || !itemHandle->data) return;
		
		textEdit->clear();
		
		QStringList list = itemHandle->data->numericalData.keys();
		
		QListWidgetItem * newItem;
		for (int i=0; i < list.size(); ++i)
		{
			newItem = new QListWidgetItem;
			newItem->setText(list[i]);
			newItem->setToolTip(itemHandle->data->numericalData[ list[i] ].description());
			numericalTables.addItem(newItem);
		}
		
		list = itemHandle->data->textData.keys();
		for (int i=0; i < list.size(); ++i)
		{
			newItem = new QListWidgetItem;
			newItem->setText(list[i]);
			newItem->setToolTip(itemHandle->data->textData[ list[i] ].description());
			textTables.addItem(newItem);
		}
	}
	
}


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::ViewTablesTool * ViewTablesTool = new Tinkercell::ViewTablesTool;
	main->addTool(ViewTablesTool);

}

