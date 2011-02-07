/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This tool handles module connections that merge items from two modules

****************************************************************************/

#include "ItemHandle.h"
#include "CThread.h"
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "DNASequenceTool.h"
#include "AutoGeneRegulatoryTool.h"
#include "CLabelsTool.h"
#include "ModelSummaryTool.h"

namespace Tinkercell
{

	void DNASequenceViewer::select(int)
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		if (dockWidget && dockWidget->widget() != this)
				dockWidget->setWidget(this);

		QList<QGraphicsItem*> & list = scene->selected();
		QList<ItemHandle*> itemHandles;
		ItemHandle * handle;

		for (int i=0; i < list.size(); ++i)
		{
			handle = getHandle(list[i]);
			if (handle && !itemHandles.contains(handle))
				itemHandles += handle;
		}
		if (itemHandles.size() < 1) return;
		openedByUser = true;

		updateText(currentScene(),list);

		if (dockWidget != 0)
		{
			if (dockWidget->isVisible())
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

	void DNASequenceViewer::deselect(int)
	{
		if (openedByUser && (!dockWidget || dockWidget->isFloating()))
		{
			openedByUser = false;
			if (dockWidget != 0)
			{
				//editTool->dockWidget->hide();
				//editTool->dockWidget->setWindowOpacity(0.5);
			}
			else
			{
				//editTool->hide();
				//editTool->setWindowOpacity(0.5);
			}
		}
	}

	/************************
	Sequence Text Edit Widget
	************************/

	DNASequenceViewerTextEdit::DNASequenceViewerTextEdit(QWidget * parent) : QTextEdit(parent)
	{
		QFont font = this->font();
		font.setPointSize(16);
		setFont(font);
		this->setReadOnly(true);
	}

	void DNASequenceViewerTextEdit::updateText(const QList<ItemHandle*> & nodes)
	{
		this->nodes = nodes;
		this->colors.clear();

		setPlainText(tr(""));
		QList<QColor> colors;
		colors << QColor("#CD0000") << QColor("#9803DD") << QColor("#1203DD")
			   << QColor("#03DD8D") << QColor("#B6DD03") << QColor("#DDAC03");
		QTextCursor cursor = this->textCursor();

		QTextCharFormat format;
		format.setFontWeight(QFont::Bold);
		format.setForeground(QColor(255,255,255));

		int k = 0;
		for (int i=0; i < nodes.size(); ++i)
		{
			if (nodes[i] && nodes[i]->hasTextData(tr("Text Attributes"))
				&& nodes[i]->textDataTable(tr("Text Attributes")).hasRow(tr("sequence")))
				{
					this->colors << colors[k];
					format.setBackground(colors[k]);
					cursor.setCharFormat(format);
					cursor.insertText(nodes[i]->textDataTable(tr("Text Attributes")).value(tr("sequence"),0));
					++k;

					if (k >= colors.size()) k = 0;
				}
		}
		while (this->colors.size() < nodes.size())
		{
			this->colors << QColor(0,0,0);
		}
	}

	int DNASequenceViewerTextEdit::currentNodeIndex()
	{
		QTextCursor cursor = this->textCursor();

		int k = cursor.position();
		int j = 0;

		for (int i=0; i < nodes.size(); ++i)
		{
			if (nodes[i] && nodes[i]->hasTextData(tr("Text Attributes"))
				&& nodes[i]->textDataTable(tr("Text Attributes")).hasRow(tr("sequence")))
				{
					j += nodes[i]->textDataTable(tr("Text Attributes")).value(tr("sequence"),0).length();
					if (j > k)
						return i;
				}
		}

		return -1;
	}

	void DNASequenceViewerTextEdit::updateNodes()
	{
	}

	void DNASequenceViewerTextEdit::contextMenuEvent ( QContextMenuEvent * )
	{
		int i = currentNodeIndex();
		if (i > -1)
			emit highlight(nodes[i],colors[i]);
	}

	void DNASequenceViewerTextEdit::mouseDoubleClickEvent ( QMouseEvent * )
	{
		int i = currentNodeIndex();
		if (i > -1)
			emit highlight(nodes[i],colors[i]);
	}

	void DNASequenceViewerTextEdit::keyPressEvent ( QKeyEvent * event )
	{
		if (event->key() == Qt::Key_Enter)
		{
			int i = currentNodeIndex();
			if (i > -1)
				emit highlight(nodes[i],colors[i]);
		}
	}

	/************************
	DNA Sequence Tool
	************************/

	DNASequenceViewer::DNASequenceViewer() : Tool(tr("DNA Sequence Viewer"))
	{
		dockWidget = 0;
		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(&textEdit);
		setLayout(layout);
		//connect(&textEdit,SIGNAL(textChanged()),this,SLOT(textChanged()));

		QString appDir = QCoreApplication::applicationDirPath();
		openedByUser = false;
		NodeGraphicsReader reader;
		reader.readXml(&item,tr(":/images/DNATool.xml"));
		item.setToolTip(tr("DNA sequence"));
		setToolTip(tr("DNA sequence"));

		item.normalize();
		item.scale(35.0/item.sceneBoundingRect().width(),35.0/item.sceneBoundingRect().height());

		ToolGraphicsItem * gitem = new ToolGraphicsItem(this);
		addGraphicsItem(gitem);
		gitem->addToGroup(&item);
		
		addAction(QIcon(), tr("DNA sequence"), tr("View the DNA sequence of selected items"));
	}
	bool DNASequenceViewer::setMainWindow(MainWindow* main)
	{
		Tool::setMainWindow(main);

		if (mainWindow != 0)
		{
			connect(mainWindow,SIGNAL(itemsInserted(NetworkHandle*,const QList<ItemHandle*>&)),
						  this, SLOT(itemsInserted(NetworkHandle*,const QList<ItemHandle*>&)));
			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
						this,SLOT(itemsSelected(GraphicsScene *,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			toolLoaded(0);
			setWindowTitle(name);
			dockWidget = mainWindow->addToolWindow(this,MainWindow::DockWidget, Qt::BottomDockWidgetArea,Qt::BottomDockWidgetArea);
			dockWidget->setAttribute(Qt::WA_ContentsPropagated);
			dockWidget->hide();
			dockWidget->setFloating(true);
			
			if (mainWindow->tool(tr("C Labels Tool")))
			{
				CLabelsTool * labelsTool = static_cast<CLabelsTool*>(mainWindow->tool(tr("C Labels Tool")));
				connect(&textEdit,SIGNAL(highlight(ItemHandle*,QColor)),labelsTool,SLOT(highlightItem(ItemHandle*,QColor)));
			}
		}
		return true;
	}

	void DNASequenceViewer::itemsInserted(NetworkHandle* , const QList<ItemHandle*>& handles)
	{
		for (int i=0; i < handles.size(); ++i)
		{
			if (handles[i] && handles[i]->isA(tr("Part")) && !handles[i]->tools.contains(this))
					handles[i]->tools += this;
		}
	}

	void DNASequenceViewer::itemsRemoved(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)
	{
	}

	void DNASequenceViewer::textChanged()
	{
		NetworkHandle * net = currentNetwork();
		if (!net || !net->currentScene()) return;

		ItemHandle * handle = 0;
		QList<QGraphicsItem*> & selected = net->currentScene()->selected();
		
		if (selected.size() == 1 &&
			(handle = getHandle(selected[0])) &&
			handle->isA(tr("Part")) && handle->hasTextData(tr("Text Attributes")))
		{
			DataTable<QString> data(handle->textDataTable(tr("Text Attributes")));
			data.value(tr("sequence"),0) = textEdit.toPlainText();
			net->changeData(handle->fullName() + tr("'s sequence changed"),handle,tr("Text Attributes"),&data);
		}
	}

	void DNASequenceViewer::itemsSelected(GraphicsScene * scene,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)
	{
		if (scene && this->isVisible())
		{
			updateText(scene,scene->selected());
			//textEdit.setReadOnly(scene->selected().size() > 1);
		}
	}

	bool DNASequenceViewer::updateText(GraphicsScene * scene, const QList<QGraphicsItem*>& selected)
	{
		if (!scene) return false;

		ItemHandle* h = 0;
		QList<ItemHandle*> handlesUp, handlesDown;
		NodeGraphicsItem * node = 0;

		//find the downstream-most
		for (int i=0; i < selected.size(); ++i)
		{
			if ((h = getHandle(selected[i])) && h->isA(tr("Part")) && (node = NodeGraphicsItem::cast(selected[i])))
			{
				if (!h->children.isEmpty())
				{
					node = 0;
					for (int j=0; j < h->children.size(); ++j)
						if (h->children[j]->isA(tr("Part")))
						{
							for (int k=0; k < h->children[j]->graphicsItems.size(); ++k)
								if (node = NodeGraphicsItem::cast(h->children[j]->graphicsItems[k]))
									break;
							if (node)
								break;
						}
				}
				
				if (!node) continue;
				
				handlesUp.clear();
				AutoGeneRegulatoryTool::findAllParts(node,tr("Part"),handlesUp,true,QStringList());
				if (!handlesUp.isEmpty())
				{
					if (selected.size() > 1)
						if (console())
                            console()->message(tr("displayed sequence is for DNA strand containing ") + h->fullName());

					break;
				}
			}
		}

		handlesDown.clear();
		//get all upstream nodes
		if (node && (h = getHandle(node)) && h->isA(tr("Part")) && !(h->parent && h->parent->isA("Vector")))
		{
			handlesDown.push_back(h);
			AutoGeneRegulatoryTool::findAllParts(node,tr("Part"),handlesDown,false,QStringList());
		}

		while (!handlesUp.isEmpty())
		{
			handlesDown.push_front(handlesUp.first());
			handlesUp.pop_front();
		}

		//disconnect(&textEdit,SIGNAL(textChanged()),this,SLOT(textChanged()));

		textEdit.updateText(handlesDown);

		//connect(&textEdit,SIGNAL(textChanged()),this,SLOT(textChanged()));

		return handlesDown.size() > 0;
	}

	void DNASequenceViewer::displayModel(QTabWidget& widgets, const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& )
	{

		if (currentScene() && updateText(currentScene(),currentScene()->selected()))
		{
			widgets.addTab(this,tr("DNA Sequence"));
		}
		else
			if (dockWidget && dockWidget->widget() != this)
				dockWidget->setWidget(this);
	}

	void DNASequenceViewer::toolLoaded(Tool*)
	{
		static bool connected = false;
		if (connected) return;

		if (mainWindow && mainWindow->tool(tr("Model Summary")))
		{
			QWidget * widget = mainWindow->tool(tr("Model Summary"));
			ModelSummaryTool * modelSummary = static_cast<ModelSummaryTool*>(widget);
			connect(modelSummary,SIGNAL(displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)),
					this,SLOT(displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)));
			connected = true;
		}
	}
}

/*
extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::CLabelsTool * labeltool = new Tinkercell::CLabelsTool;
	main->addTool(labeltool);

	Tinkercell::DNASequenceViewer * sequenceTool = new Tinkercell::DNASequenceViewer;
	main->addTool(sequenceTool);

}
*/

