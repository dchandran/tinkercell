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
	*************************/

	DNASequenceViewerTextEdit::DNASequenceViewerTextEdit(QWidget * parent) : QTextEdit(parent)
	{
		QFont font = this->font();
		font.setPointSize(16);
		setFont(font);
		//this->setReadOnly(true);
	}

	void DNASequenceViewerTextEdit::updateText(const QList<ItemHandle*> & nodes)
	{
		setPlainText(tr(""));
		this->nodes = nodes;
		this->colors.clear();
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
		int i=0, j = 0;

		for (i=0; i < nodes.size(); ++i)
		{
			if (nodes[i] && nodes[i]->hasTextData(tr("Text Attributes"))
				&& nodes[i]->textDataTable(tr("Text Attributes")).hasRow(tr("sequence")))
				{
					j += nodes[i]->textDataTable(tr("Text Attributes")).value(tr("sequence"),0).length();
					if (j > k)
						return i;
				}
		}

		return (i-1);
	}

	void DNASequenceViewerTextEdit::updateNodes()
	{
	}
/*
	void DNASequenceViewerTextEdit::contextMenuEvent ( QContextMenuEvent * )
	{
		int i = currentNodeIndex();
		if (i > -1)
		{
			emit clearLabels();
			emit highlight(nodes[i],colors[i]);
		}
	}
*/
	void DNASequenceViewerTextEdit::mouseMoveEvent ( QMouseEvent * )
	{
		int i = currentNodeIndex();
		if (i > -1)
		{
			emit clearLabels();
			emit highlight(nodes[i],colors[i]);
			emit updateAnnotationsTable(nodes[i]);
		}
		else
			emit updateAnnotationsTable(0);
	}

	void DNASequenceViewerTextEdit::keyPressEvent ( QKeyEvent * event )
	{
		int key = event->key();
		if (key == Qt::Key_Enter || key == Qt::Key_Return || key == Qt::Key_Space || 
			 key == Qt::Key_Up || key == Qt::Key_Down || key == Qt::Key_Right || key == Qt::Key_Left ||
			 key == Qt::Key_PageUp || key == Qt::Key_PageDown)
		{
			int i = currentNodeIndex();
			QTextCursor cursor = textCursor();
			QTextCharFormat currentFormat = cursor.charFormat();
			
			int k = cursor.position();
			int start = k, len = 0;
			while (cursor.charFormat().background().color() == currentFormat.background().color())
			{
					--start;
					if (!cursor.movePosition(QTextCursor::Left)) break;
			}
			cursor.movePosition(QTextCursor::Right);

			while (cursor.charFormat().background().color() == currentFormat.background().color())
			{
					++len;
					if (!cursor.movePosition(QTextCursor::Right)) break;
			}
			
			QString text = toPlainText();
			text = text.left(start + len);
			text = text.right(len);
			
			if (i > -1)
			{
				emit clearLabels();
				emit highlight(nodes[i],colors[i]);
				emit sequenceChanged(nodes[i], text);
				emit updateAnnotationsTable(nodes[i]);
			}
			else
				emit updateAnnotationsTable(0);
		}
		else
		{
			QTextEdit::keyPressEvent(event);
		}
	}

	/************************
	DNA Sequence Tool
	************************/

	DNASequenceViewer::DNASequenceViewer() : Tool(tr("DNA Sequence Viewer"))
	{
		dockWidget = 0;
		QVBoxLayout * layout = new QVBoxLayout;
		layout->setContentsMargins(0,0,0,0);

		textEdit = new DNASequenceViewerTextEdit;
		annotationsTable = new QTableWidget;
		
		connect(annotationsTable, SIGNAL(cellChanged(int,int)), this, SLOT(tableValueChanged(int,int)));
		
		layout->addWidget(textEdit,2);

		QVBoxLayout * layout2 = new QVBoxLayout;
		layout2->addWidget(annotationsTable);		
		annotationsGroup = new QGroupBox(" Sequence annotation ");
		annotationsGroup->setLayout(layout2);

		layout->addWidget(annotationsGroup,1);
		annotationsTable->hide();
		setLayout(layout);
		connect(textEdit,SIGNAL(sequenceChanged(ItemHandle*, const QString&)),this,SLOT(sequenceChanged(ItemHandle*, const QString&)));
		connect(textEdit,SIGNAL(updateAnnotationsTable(ItemHandle*)),this,SLOT(updateAnnotationsTable(ItemHandle*)));

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
			
			if (mainWindow->tool(tr("Labeling Tool")))
			{
				LabelingTool * labelsTool = static_cast<LabelingTool*>(mainWindow->tool(tr("Labeling Tool")));
				connect(textEdit,SIGNAL(highlight(ItemHandle*,QColor)),labelsTool,SLOT(highlightItem(ItemHandle*,QColor)));
				connect(textEdit,SIGNAL(clearLabels()),labelsTool,SLOT(clearLabels()));
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

	void DNASequenceViewer::sequenceChanged(ItemHandle * handle, const QString& s)
	{
		NetworkHandle * net = currentNetwork();
		if (!net || !handle || !net->currentScene()) return;

		if (handle->isA(tr("Part")) && handle->hasTextData(tr("Text Attributes")))
		{
			DataTable<QString> data(handle->textDataTable(tr("Text Attributes")));
			if (data.value(tr("sequence"),0) != s)
			{
				data.value(tr("sequence"),0) = s;
				net->changeData(handle->fullName() + tr("'s sequence changed"),handle,tr("Text Attributes"),&data);
				updateText(net->currentScene(),net->currentScene()->selected());
			}
		}
	}

	void DNASequenceViewer::itemsSelected(GraphicsScene * scene,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)
	{
		if (scene && this->isVisible())
		{
			updateText(scene,scene->selected());
		}
	}

	bool DNASequenceViewer::updateText(GraphicsScene * scene, const QList<QGraphicsItem*>& selected)
	{
		if (!scene || !textEdit) return false;

		if (annotationsTable)
			annotationsTable->hide();

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
					NodeGraphicsItem * node2 = 0;
					node = 0;
					for (int j=0; j < h->children.size(); ++j)
						if (h->children[j]->isA(tr("Part")))
						{
							for (int k=0; k < h->children[j]->graphicsItems.size(); ++k)
								if (node2 = NodeGraphicsItem::cast(h->children[j]->graphicsItems[k]))
									break;
							if (node2 && (!node || (node->scenePos().x() > node2->scenePos().x())))
								node = node2;
						}
				}
				
				if (!node) continue;
				
				handlesUp.clear();
				AutoGeneRegulatoryTool::findAllParts(node,tr("Part"),handlesUp,true,QStringList());
				if (!handlesUp.isEmpty())
				{
					/*if (selected.size() > 1)
						if (console())
                            console()->message(tr("displayed sequence is for DNA strand containing ") + h->fullName());
					*/
					break;
				}
			}
		}

		handlesDown.clear();
		//get all upstream nodes
		if (node && (h = getHandle(node)) && h->isA(tr("Part")) && !(h->parent && h->parent->isA("Vector")))
		{
			AutoGeneRegulatoryTool::findAllParts(node,tr("Part"),handlesDown,false,QStringList());
		}
		
		if (h && h->isA("Part") && !handlesDown.contains(h))
			handlesDown.push_front(h);

		while (!handlesUp.isEmpty())
		{
			h = handlesUp.first();
			if (!handlesDown.contains(h))
				handlesDown.push_front(h);
			handlesUp.pop_front();
		}

		textEdit->updateText(handlesDown);
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
	
	void DNASequenceViewer::updateAnnotationsTable(ItemHandle * h)
	{
		if (!annotationsTable || !annotationsGroup) return;
		
		disconnect(annotationsTable, SIGNAL(cellChanged(int,int)), this, SLOT(tableValueChanged(int,int)));
		if (!h)
		{
			annotationsTable->setRowCount(0);
			annotationsTable->hide();
			annotationsGroup->setTitle("No part selected");
		}
		else
		{
			annotationsGroup->setTitle(tr("Sequence annotation for ") + h->fullName());
			annotationsTable->show();
			currentHandle = h;
			TextDataTable & annotationsData = h->textDataTable(tr("sequence annotation"));
			if (annotationsData.columns() == 0)
			{
				annotationsData.resize(0,2);
				annotationsData.setColumnName(0,"position");
				annotationsData.setColumnName(1,"description");
			}

			annotationsTable->setColumnCount(2);
			annotationsTable->clearContents();
			annotationsTable->setRowCount(annotationsData.rows() + 1);
			annotationsTable->setHorizontalHeaderLabels(QStringList() << "position" << "description");
			
			for (int i=0; i < annotationsData.rows(); ++i)
			{
				annotationsTable->setItem ( i, 0, new QTableWidgetItem( annotationsData(i,0) ) );
				annotationsTable->setItem ( i, 1, new QTableWidgetItem( annotationsData(i,1) ) );
			}
		}

		connect(annotationsTable, SIGNAL(cellChanged(int,int)), this, SLOT(tableValueChanged(int,int)));
	}
	
	void DNASequenceViewer::tableValueChanged(int r, int c)
	{
		if (!annotationsTable || !currentHandle || !currentNetwork()) return;
		
		if (c == 0)
		{
			QString s = annotationsTable->item(r,c)->text();
			bool ok;
			double d = s.toDouble(&ok);
			if (!ok) return;
		}

		QStringList col1, col2;
		for (int i=0; i < annotationsTable->rowCount(); ++i)
			if (annotationsTable->item(i,0) && annotationsTable->item(i,1))
			{
				QString s1 = annotationsTable->item(i,0)->text(),
							  s2 = annotationsTable->item(i,1)->text();
				if (!s1.isEmpty() && !s2.isEmpty())
				{
					col1 += s1;
					col2 += s2;
				}
			}
		
		ItemHandle * h = currentHandle;
		
		TextDataTable annotationsData;
		annotationsData.resize(col1.size(), annotationsTable->columnCount());
		annotationsData.setColumnName(0,"position");
		annotationsData.setColumnName(1,"description");

		for (int i=0; i < col1.size(); ++i)
		{
			annotationsData(i,0) = col1[i];
			annotationsData(i,1) = col2[i];
		}
	
		if (annotationsData.rows() > 0)
		{
			currentNetwork()->changeData(h->name + tr(" sequence annotated"), h, tr("sequence annotation"), &annotationsData);
			disconnect(annotationsTable, SIGNAL(cellChanged(int,int)), this, SLOT(tableValueChanged(int,int)));
			annotationsTable->setRowCount(annotationsData.rows() + 1);
			connect(annotationsTable, SIGNAL(cellChanged(int,int)), this, SLOT(tableValueChanged(int,int)));
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

