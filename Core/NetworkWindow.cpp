/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 An MDI sub window that can either be represented as text using TextEditor or visualized with graphical items in the 
 GraphicsScene. Each node and connection are contained in a handle, and each handle can either be represented as text or as graphics. 
 This class provides functions for editing handles, such as changing names, data, etc. 
 
****************************************************************************/


#include "MainWindow.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "GraphicsScene.h"
#include "TextEditor.h"
#include "UndoCommands.h"
#include "NetworkWindow.h"
#include "muParserDef.h"
#include "muParser.h"
#include "muParserInt.h"
#include <QRegExp>

namespace Tinkercell
{
	void NetworkWindow::GraphicsView::wheelEvent(QWheelEvent * wheelEvent)
	{
		if (wheelEvent->modifiers() & Qt::ControlModifier)
		{
			double factor = 1.0 + 0.2 * qAbs(wheelEvent->delta()/120.0);
			if (wheelEvent->delta() > 0)
				scale(factor,factor);
			else
				scale(1.0/factor,1.0/factor);
		}
		else
			QGraphicsView::wheelEvent(wheelEvent);
	}
	void NetworkWindow::GraphicsView::dragEnterEvent(QDragEnterEvent * /*event*/)
	{
		//event->acceptProposedAction();
	}

	void NetworkWindow::GraphicsView::dropEvent(QDropEvent * /*event*/)
	{
		/*if (parentWidget())
		{
			QList<QUrl> urlList;
			QList<QFileInfo> files;
			QString fName;
			QFileInfo info;
		 
			if (event->mimeData()->hasUrls())
			{
				urlList = event->mimeData()->urls(); // returns list of QUrls
			
				// if just text was dropped, urlList is empty (size == 0)
				if ( urlList.size() > 0) // if at least one QUrl is present in list
				{
					fName = urlList[0].toLocalFile(); // convert first QUrl to local path
					info.setFile( fName ); // information about file
					if ( info.isFile() ) 
						files += info;
				}
			}
			MainWindow * main = static_cast<MainWindow*>(parentWidget());
			main->dragAndDropFiles(files);
		}*/
	}
	/*! \brief Constructor: connects all the signals of the new window to that of the main window */
	NetworkWindow::GraphicsView::GraphicsView(GraphicsScene * scene, QWidget * parent) 
	: QGraphicsView (scene,parent)
	{
		setCacheMode(QGraphicsView::CacheBackground);
		//setViewportUpdateMode (QGraphicsView::SmartViewportUpdate);
		setViewportUpdateMode (QGraphicsView::BoundingRectViewportUpdate);
		//setDragMode(QGraphicsView::RubberBandDrag);
		//setDragMode(QGraphicsView::ScrollHandDrag);
		//setOptimizationFlags(QGraphicsView::DontClipPainter | QGraphicsView::DontSavePainterState);
		
		setPalette(QPalette(QColor(255,255,255)));
		setAutoFillBackground(true);
		setAcceptDrops(true);
		
		setRenderHint(QPainter::Antialiasing);
		setCacheMode(QGraphicsView::CacheBackground);
		setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
		fitInView(QRectF(0,0,1000,800),Qt::KeepAspectRatio);
	}
	
	NetworkWindow::~NetworkWindow()
	{
		disconnect(&history);
		history.clear();
		if (scene)
			delete scene;
	}
	
	void NetworkWindow::closeWindow ()
	{
		bool b = true;
		emit closing(&b);
		if (b)
		{
			disconnect();
			QMdiSubWindow::close();
		}
	}

	NetworkWindow::NetworkWindow(MainWindow * main, GraphicsScene * scene) : mainWindow(main), scene(0), textEditor(0), symbolsTable(this)
	{
		if (!scene) scene = new GraphicsScene;
		this->scene = scene;
                scene->networkWindow = this;
		
		GraphicsView * view = new GraphicsView(scene);
		setWidget(view);
		setAttribute(Qt::WA_DeleteOnClose);
		
		connect(&history, SIGNAL(indexChanged(int)), this, SLOT(updateSymbolsTable(int)));
		connect(&history, SIGNAL(indexChanged(int)), mainWindow, SIGNAL(historyChanged(int)));
		
		scene->symbolsTable = &symbolsTable;
		scene->historyStack = &history;	
		scene->contextItemsMenu = &(main->contextItemsMenu);
		scene->contextScreenMenu = &(main->contextScreenMenu);
		
		if (main)
		{
			connect(scene,SIGNAL(itemsSelected(GraphicsScene*,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				main, SIGNAL(itemsSelected(GraphicsScene*,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
			
			connect(scene,SIGNAL(mousePressed(GraphicsScene*,QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
					main ,SIGNAL(mousePressed(GraphicsScene*,QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));

			connect(scene,SIGNAL(mouseReleased(GraphicsScene*,QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
					main ,SIGNAL(mouseReleased(GraphicsScene*,QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));
			
			connect(scene,SIGNAL(mouseDoubleClicked (GraphicsScene*, QPointF, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers)),
					main ,SIGNAL(mouseDoubleClicked (GraphicsScene*, QPointF, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers)));
			
			connect(scene,SIGNAL(mouseDragged(GraphicsScene*,QPointF, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
					main ,SIGNAL(mouseDragged(GraphicsScene*,QPointF, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));
			
			connect(scene,SIGNAL(itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>& , Qt::KeyboardModifiers)),
					main ,SIGNAL(itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>& , Qt::KeyboardModifiers)));
			
			connect(scene,SIGNAL(mouseMoved(GraphicsScene*,QGraphicsItem*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers,QList<QGraphicsItem*>&)),
					main ,SIGNAL(mouseMoved(GraphicsScene*,QGraphicsItem*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers,QList<QGraphicsItem*>&)));
			
			connect(scene,SIGNAL(sceneRightClick(GraphicsScene*,QGraphicsItem*, QPointF, Qt::KeyboardModifiers)),
					main ,SIGNAL(sceneRightClick(GraphicsScene*,QGraphicsItem*, QPointF, Qt::KeyboardModifiers)));
			
			connect(scene,SIGNAL(keyPressed(GraphicsScene*,QKeyEvent *)),
					main ,SIGNAL(keyPressed(GraphicsScene*,QKeyEvent *)));
					
			connect(scene,SIGNAL(keyReleased(GraphicsScene*,QKeyEvent *)),
					main ,SIGNAL(keyReleased(GraphicsScene*,QKeyEvent *)));

			connect(scene,SIGNAL(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
					main ,SIGNAL(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));

			connect(scene,SIGNAL(itemsAboutToBeInserted(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&)),
					main ,SIGNAL(itemsAboutToBeInserted(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&)));

			connect(scene,SIGNAL(itemsAboutToBeRemoved(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&)),
					main ,SIGNAL(itemsAboutToBeRemoved(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&)));

			connect(scene,SIGNAL(itemsRemoved(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
					main ,SIGNAL(itemsRemoved(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));

			connect(scene,SIGNAL(colorChanged(GraphicsScene*,const QList<QGraphicsItem*>&)),
					main ,SIGNAL(colorChanged(GraphicsScene*,const QList<QGraphicsItem*>&)));

			connect(scene,SIGNAL(parentItemChanged(GraphicsScene*,const QList<QGraphicsItem*>&,const QList<QGraphicsItem*>&)),
					main ,SIGNAL(parentItemChanged(GraphicsScene*,const QList<QGraphicsItem*>&,const QList<QGraphicsItem*>&)));

			connect(scene,SIGNAL(handlesChanged(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
					main ,SIGNAL(handlesChanged(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));

			connect(this,SIGNAL(parentHandleChanged(NetworkWindow*, const QList<ItemHandle*>&, const QList<ItemHandle*>&)),
					main ,SIGNAL(parentHandleChanged(NetworkWindow*, const QList<ItemHandle*>&, const QList<ItemHandle*>&)));
					
			connect(this,SIGNAL(dataChanged(const QList<ItemHandle*>&)),
					main ,SIGNAL(dataChanged(const QList<ItemHandle*>&)));		

			connect(this,SIGNAL(itemsRenamed(NetworkWindow*, const QList<ItemHandle*>&, const QList<QString>&, const QList<QString>&)),
					main ,SIGNAL(itemsRenamed(NetworkWindow*, const QList<ItemHandle*>&, const QList<QString>&, const QList<QString>&)));

			connect(scene,SIGNAL(escapeSignal(const QWidget*)),
					main ,SIGNAL(escapeSignal(const QWidget*)));		
			
			connect(scene,SIGNAL(filesDropped(const QList<QFileInfo>&)),
					main,SLOT(dragAndDropFiles(const QList<QFileInfo>&)));
					
			setWindowTitle(tr("network ") + QString::number(1 + main->allWindows().size()));
		}
		
		view->centerOn(0,0)	;
	}
	
	NetworkWindow::NetworkWindow(MainWindow * main,TextEditor * editor) : mainWindow(main), scene(0), textEditor(0), symbolsTable(this)
	{
                if (!editor) editor = new TextEditor;
                this->textEditor = editor;
                editor->networkWindow = this;
		
		setWidget(editor);
		setAttribute(Qt::WA_DeleteOnClose);
		
		connect(&history, SIGNAL(indexChanged(int)), this, SLOT(updateSymbolsTable(int)));
		connect(&history, SIGNAL(indexChanged(int)), mainWindow, SIGNAL(historyChanged(int)));
		
		if (main)
		{
		   connect(editor,SIGNAL(itemsInserted(TextEditor *, const QList<TextItem*>& , const QList<ItemHandle*>&)),
						main,SIGNAL(itemsInserted(TextEditor *, const QList<TextItem*>& , const QList<ItemHandle*>&)));

		   connect(editor,SIGNAL(itemsRemoved(TextEditor * , const QList<TextItem*>& , const QList<ItemHandle*>& )),
						main,SIGNAL(itemsRemoved(TextEditor * , const QList<TextItem*>& , const QList<ItemHandle*>&)));

		   connect(editor,SIGNAL(findText(const QString&)),main,SIGNAL(findText(const QString&)));

		   connect(editor,SIGNAL(replaceText(const QRegExp&, const QString&)),
				   main,SIGNAL(replaceText(const QRegExp&, const QString&)));

		   connect(editor,SIGNAL(textInserted(const QString&)),
				   main,SIGNAL(textInserted(const QString&)));

		   connect(editor,SIGNAL(textRemoved(const QString&)),
				   main,SIGNAL(textRemoved(const QString&)));

		   connect(editor,SIGNAL(textChanged(TextEditor *, const QString&, const QString&, const QString&)),
				   main,SIGNAL(textChanged(TextEditor *, const QString&, const QString&, const QString&)));

		   connect(editor,SIGNAL(lineChanged(TextEditor *, int, const QString&)),
				   main,SIGNAL(lineChanged(TextEditor *, int, const QString&)));

			connect(this,SIGNAL(parentHandleChanged(NetworkWindow*, const QList<ItemHandle*>&, const QList<ItemHandle*>&)),
                                main ,SIGNAL(parentHandleChanged(NetworkWindow*, const QList<ItemHandle*>&, const QList<ItemHandle*>&)));
					
			connect(this,SIGNAL(dataChanged(const QList<ItemHandle*>&)),
                                main ,SIGNAL(dataChanged(const QList<ItemHandle*>&)));

			connect(this,SIGNAL(itemsRenamed(NetworkWindow*, const QList<ItemHandle*>&, const QList<QString>&, const QList<QString>&)),
                                main ,SIGNAL(itemsRenamed(NetworkWindow*, const QList<ItemHandle*>&, const QList<QString>&, const QList<QString>&)));

			setWindowTitle(tr("network ") + QString::number(1 + main->allWindows().size()));
		}
	}
	
	QList<ItemHandle*> NetworkWindow::allHandles() const
	{
		return symbolsTable.handlesFullName.values();
	}

        QList<ItemHandle*> NetworkWindow::selectedHandles() const
        {
            QList<ItemHandle*> handles;
            QHash<ItemHandle*,int> hash;
            ItemHandle* handle;
            if (scene)
            {
                QList<QGraphicsItem*>& selected = scene->selected();

                for (int i=0; i < selected.size(); ++i)
                {
                    handle = getHandle(selected[i]);
                    if (handle && !hash.contains(handle))
                    {
                        handles << handle;
                        hash.insert(handle,1);
                    }
                }
            }
            else
            if (textEditor)
            {
            }

            return handles;
        }
	
	void NetworkWindow::rename(const QString& oldname, const QString& newname)
	{
		QList<ItemHandle*> items;
		QList<QString> oldNames, newNames;
		oldNames += oldname;
		newNames += newname;
		
		newNames += Tinkercell::RemoveDisallowedCharactersFromName(newname);

		QUndoCommand * command = new RenameCommand(tr("name changed"),this->allHandles(),oldname,newname);
		
		history.push(command);		
		
		emit itemsRenamed(this, items, oldNames, newNames);
	}

	void NetworkWindow::rename(ItemHandle* handle, const QString& name)
	{
		if (!handle) return;
		
		QList<ItemHandle*> items;
		items += handle;
		QList<QString> oldNames, newNames;
		oldNames += handle->fullName();
		
		if (handle->parent && !name.contains(handle->parent->fullName()))
			newNames += handle->parent->fullName() + tr(".") + Tinkercell::RemoveDisallowedCharactersFromName(name);
		else
			newNames += Tinkercell::RemoveDisallowedCharactersFromName(name);
		

		QUndoCommand * command = new RenameCommand(tr("name changed"),this,items,newNames);
		
		history.push(command);
		
		emit itemsRenamed(this, items, oldNames, newNames);
	}
	
	void NetworkWindow::rename(const QList<ItemHandle*>& items, const QList<QString>& new_names)
	{
		if (items.isEmpty() || items.size() != new_names.size()) return;
		
		QList<QString> oldNames, newNames;
		ItemHandle * handle;
		QString name;
		
		for (int i=0; i < items.size(); ++i)
			if ((handle = items[i]))
			{
				name = new_names[i];
				oldNames += handle->fullName();
			
				if (handle->parent && !name.contains(handle->parent->fullName()))
					newNames += handle->parent->fullName() + tr(".") + Tinkercell::RemoveDisallowedCharactersFromName(name);
				else
					newNames += Tinkercell::RemoveDisallowedCharactersFromName(name);
			}
		
		QUndoCommand * command = new RenameCommand(tr("name changed"),this,items,newNames);
		
		history.push(command);
		
		emit itemsRenamed(this, items, oldNames, newNames);
	}

	void NetworkWindow::setParentHandle(const QList<ItemHandle*>& handles, const QList<ItemHandle*>& parentHandles)
	{
		if (handles.size() != parentHandles.size()) return;
		
		SetParentHandleCommand * command1 = new SetParentHandleCommand(tr("parent(s) changed"), this, handles, parentHandles);
		
		QList<QString> newNames;
		for (int i=0; i < handles.size(); ++i)
			if (handles[i] && parentHandles[i])
				newNames += parentHandles[i]->fullName() + tr(".") + handles[i]->name;
			else
				if (handles[i])
					newNames += handles[i]->fullName();

		QUndoCommand * command2 = new RenameCommand(tr("name changed"),this,handles,newNames);
		
		QList<QUndoCommand*> list;
		list << command2 << command1;
		
		CompositeCommand * command = new CompositeCommand(tr("parent(s) changed"),list);
		
		history.push(command);
		
		emit parentHandleChanged(this, command1->children, command1->oldParents);
	}

	void NetworkWindow::setParentHandle(ItemHandle * child, ItemHandle * parent)
	{
		QList<ItemHandle*> children, parents;
		children << child;
		parents << parent;
		setParentHandle(children,parents);
	}

	void NetworkWindow::setParentHandle(const QList<ItemHandle*> children, ItemHandle * parent)
	{
		QList<ItemHandle*> parents;
		for (int i=0; i < children.size(); ++i)
			parents << parent;
		setParentHandle(children,parents);
	}

	/*! \brief change numerical data table*/	
	void NetworkWindow::changeData(ItemHandle* handle, const QString& hashstring, const DataTable<qreal>* newdata)
	{
		if (handle && handle->data && handle->data->numericalData.contains(hashstring))
		{
			QUndoCommand * command = new ChangeDataCommand<qreal>(handle->name + tr(" table changed"),&(handle->data->numericalData[hashstring]),newdata);
			
			history.push(command);
				
			QList<ItemHandle*> handles;
			handles += handle;
			emit dataChanged(handles);
		}
	}
	/*! \brief change a list of numerical data tables*/
	void NetworkWindow::changeData(const QList<ItemHandle*>& handles, const QList<QString>& hashstrings, const QList<DataTable<qreal>*>& newdata)
	{
		QList<DataTable<qreal>*> oldTables, newTables;

		for (int i=0; i < handles.size() && i < hashstrings.size() && i < newdata.size(); ++i)
		{
			if (newdata[i] && handles[i] && handles[i]->data && handles[i]->data->numericalData.contains(hashstrings[i]))
			{
				oldTables += &(handles[i]->data->numericalData[ hashstrings[i] ]);
				newTables += newdata[i];
			}
		}

		if (oldTables.isEmpty() || newTables.isEmpty()) return;


		QUndoCommand * command = new ChangeDataCommand<qreal>(tr("numerical tables changed"),oldTables,newTables);
		
		history.push(command);
		
		emit dataChanged(handles);		
	}
	/*! \brief change a list of numerical data tables*/
	void NetworkWindow::changeData(const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<qreal>*>& newdata)
	{
		QList<DataTable<qreal>*> oldTables, newTables;

		for (int i=0; i < handles.size() && i < newdata.size(); ++i)
		{
			if (newdata[i] && handles[i] && handles[i]->data && handles[i]->data->numericalData.contains(hashstring))
			{
				oldTables += &(handles[i]->data->numericalData[ hashstring ]);
				newTables += newdata[i];
			}
		}

		if (oldTables.isEmpty() || newTables.isEmpty()) return;


		QUndoCommand * command = new ChangeDataCommand<qreal>(tr("numerical tables changed"),oldTables,newTables);
		
		history.push(command);
		
		emit dataChanged(handles);		
	}
	/*! \brief change text data table*/	
	void NetworkWindow::changeData(ItemHandle* handle, const QString& hashstring, const DataTable<QString>* newdata)
	{
		if (handle && handle->data && handle->data->textData.contains(hashstring))
		{
			QUndoCommand * command = new ChangeDataCommand<QString>(handle->name + tr(" table changed"),&(handle->data->textData[hashstring]),newdata);
			history.push(command);
		
			QList<ItemHandle*> handles;
			handles += handle;
			emit dataChanged(handles);
		}
	}
	/*! \brief change a list of text data tables*/
	void NetworkWindow::changeData(const QList<ItemHandle*>& handles, const QList<QString>& hashstrings, const QList<DataTable<QString>*>& newdata)
	{
		QList<DataTable<QString>*> oldTables, newTables;

		for (int i=0; i < handles.size() && i < hashstrings.size() && i < newdata.size(); ++i)
		{
			if (newdata[i] && handles[i] && handles[i]->data && handles[i]->data->textData.contains(hashstrings[i]))
			{
				oldTables += &(handles[i]->data->textData[ hashstrings[i] ]);
				newTables += newdata[i];
			}
		}

		if (oldTables.isEmpty() || newTables.isEmpty()) return;


		QUndoCommand * command = new ChangeDataCommand<QString>(tr("text tables changed"),oldTables,newTables);
		
		history.push(command);
		
		emit dataChanged(handles);
	}
	/*! \brief change a list of text data tables*/
	void NetworkWindow::changeData(const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<QString>*>& newdata)
	{
		QList<DataTable<QString>*> oldTables, newTables;

		for (int i=0; i < handles.size() && i < newdata.size(); ++i)
		{
			if (newdata[i] && handles[i] && handles[i]->data && handles[i]->data->textData.contains(hashstring))
			{
				oldTables += &(handles[i]->data->textData[ hashstring ]);
				newTables += newdata[i];
			}
		}

		if (oldTables.isEmpty() || newTables.isEmpty()) return;


		QUndoCommand * command = new ChangeDataCommand<QString>(tr("text tables changed"),oldTables,newTables);
		
		history.push(command);
		
		emit dataChanged(handles);
	}
	/*! \brief change two types of data tables*/
	void NetworkWindow::changeData(ItemHandle* handle, const QString& hashstring, const DataTable<qreal>* newdata1, const DataTable<QString>* newdata2)
	{
		if (handle && handle->data && handle->data->numericalData.contains(hashstring) && handle->data->textData.contains(hashstring))
		{
			QUndoCommand * command = new Change2DataCommand<qreal,QString>(handle->name + tr(" table changed"),&(handle->data->numericalData[hashstring]), newdata1, &(handle->data->textData[hashstring]),newdata2);
			
			history.push(command);
		
			QList<ItemHandle*> handles;
			handles += handle;
			emit dataChanged(handles);
		}
	}
	/*! \brief change a list of two types of data tables*/	
	void NetworkWindow::changeData(const QList<ItemHandle*>& handles, const QList<QString>& hashstrings, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& newdata2)
	{
		QList<DataTable<QString>*> oldTablesS, newTablesS;
		QList<DataTable<qreal>*> oldTablesN, newTablesN;
		
		int j = 0;
		for (int i=0; j < handles.size() && j < hashstrings.size() && i < newdata1.size(); ++i, ++j)
		{
			if (newdata1[i] && handles[j] && handles[j]->data && handles[j]->data->numericalData.contains(hashstrings[j]))
			{
				oldTablesN += &(handles[j]->data->numericalData[ hashstrings[j] ]);
				newTablesN += newdata1[i];
			}
		}

		for (int i=0; j < handles.size() && j < hashstrings.size() && i < newdata2.size(); ++i, ++j)
		{
			if (newdata2[i] && handles[j] && handles[j]->data && handles[j]->data->textData.contains(hashstrings[j]))
			{
				oldTablesS += &(handles[j]->data->textData[ hashstrings[j] ]);
				newTablesS += newdata2[i];
			}
		}

		if ((oldTablesS.isEmpty() || newTablesS.isEmpty()) &&
		    (oldTablesN.isEmpty() || newTablesN.isEmpty())) return;

		QUndoCommand * command = new Change2DataCommand<qreal,QString>(tr("data tables changed"),oldTablesN,newTablesN,oldTablesS,newTablesS);
		
		history.push(command);
		
		emit dataChanged(handles);
	}

	/*! \brief change a list of two types of data tables*/	
	void NetworkWindow::changeData(const QList<ItemHandle*>& handles, const QString& hashstring, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& newdata2)
	{
		QList<DataTable<QString>*> oldTablesS, newTablesS;

		for (int i=0; i < handles.size() && i < newdata2.size(); ++i)
		{
			if (newdata2[i] && handles[i] && handles[i]->data && handles[i]->data->textData.contains(hashstring))
			{
				oldTablesS += &(handles[i]->data->textData[ hashstring ]);
				newTablesS += newdata2[i];
			}
		}

		QList<DataTable<qreal>*> oldTablesN, newTablesN;

		for (int i=0; i < handles.size()  && i < newdata1.size(); ++i)
		{
			if (newdata1[i] && handles[i] && handles[i]->data && handles[i]->data->numericalData.contains(hashstring))
			{
				oldTablesN += &(handles[i]->data->numericalData[ hashstring ]);
				newTablesN += newdata1[i];
			}
		}

		if ((oldTablesS.isEmpty() || newTablesS.isEmpty()) &&
		    (oldTablesN.isEmpty() || newTablesN.isEmpty())) return;


		QUndoCommand * command = new Change2DataCommand<qreal,QString>(tr("data tables changed"),oldTablesN,newTablesN,oldTablesS,newTablesS);
		
		history.push(command);
		
		emit dataChanged(handles);
	}
	
	/*! \brief change a list of two types of data tables and also adds undo command to history window and emits associated signal(s)*/	
	void NetworkWindow::changeData(const QList<ItemHandle*>& handles, const QList<DataTable<qreal>*>& olddata1, const QList<DataTable<qreal>*>& newdata1, const QList<DataTable<QString>*>& olddata2, const QList<DataTable<QString>*>& newdata2)
	{
		if ((olddata1.isEmpty() || newdata1.isEmpty()) &&
		    (olddata2.isEmpty() || newdata2.isEmpty())) return;
			
		QUndoCommand * command = new Change2DataCommand<qreal,QString>(tr("data tables changed"),olddata1,newdata1,olddata2,newdata2);
		
		history.push(command);
		
		emit dataChanged(handles);
	}
	
	/*! \brief change a two types of data tables and also adds undo command to history window and emits associated signal(s)*/	
	void NetworkWindow::changeData(const QList<ItemHandle*>& handles, DataTable<qreal>* olddata1, const DataTable<qreal>* newdata1, DataTable<QString>* olddata2, const DataTable<QString>* newdata2)
	{
		if ((!olddata1 || !newdata1) &&
		    (!olddata2 || !newdata2)) return;
			
		QUndoCommand * command = new Change2DataCommand<qreal,QString>(tr("data tables changed"),olddata1,newdata1,olddata2,newdata2);
		
		history.push(command);
		
		emit dataChanged(handles);
	}
	
	/*! \brief change a data table and also adds undo command to history window and emits associated signal(s)*/	
	void NetworkWindow::changeData(const QList<ItemHandle*>& handles, DataTable<qreal>* olddata1, const DataTable<qreal>* newdata1)
	{
		if (!olddata1 || !newdata1) return;
			
		QUndoCommand * command = new ChangeDataCommand<qreal>(tr("data table changed"),olddata1,newdata1);
		
		history.push(command);
		
		emit dataChanged(handles);
	}
	
	/*! \brief change a data table and also adds undo command to history window and emits associated signal(s)*/	
	void NetworkWindow::changeData(const QList<ItemHandle*>& handles, DataTable<QString>* olddata1, const DataTable<QString>* newdata1)
	{
		if (!olddata1 || !newdata1) return;
			
		QUndoCommand * command = new ChangeDataCommand<QString>(tr("data table changed"),olddata1,newdata1);
		
		history.push(command);
		
		emit dataChanged(handles);
	}
	
	
	/*! \brief update symbols table*/
	void NetworkWindow::updateSymbolsTable()
	{
		symbolsTable.update();
	}
	
	/*! \brief update symbols table*/
	void NetworkWindow::updateSymbolsTable(int)
	{
		symbolsTable.update();
	}
	
     static double d = 1.0;
     static double* AddVariable(const char*, void*)
     {
          return &d;
     }

	 bool NetworkWindow::parseMath(QString& s, QStringList& newvars)
     {
          static QStringList reservedWords;
          if (reservedWords.isEmpty())
               reservedWords << "time";

          mu::Parser parser;

          s.replace(QRegExp(tr("\\.(?!\\d)")),tr("_qqq_"));
          parser.SetExpr(s.toAscii().data());
          s.replace(tr("_qqq_"),tr("."));
          parser.SetVarFactory(AddVariable, 0);
          QString str;

          try
          {
               parser.Eval();

               // Get the map with the variables
               mu::varmap_type variables = parser.GetVar();

               // Get the number of variables
               mu::varmap_type::const_iterator item = variables.begin();

               // Query the variables
               for (; item!=variables.end(); ++item)
               {
                    str = tr(item->first.data());
                    str.replace(QRegExp(tr("[^A-Za-z0-9_]")),tr(""));
                    str.replace(tr("_qqq_"),tr("."));
                    QString str2 = str;
                    str2.replace(tr("_"),tr("."));
                    if (!reservedWords.contains(str) &&
                        !symbolsTable.handlesFullName.contains(str)) //maybe new symbol in the formula
                    {
                         if (symbolsTable.dataRowsAndCols.contains(str) && symbolsTable.dataRowsAndCols[str].first)
                         {
                              if (! str.contains(QRegExp(tr("^")+symbolsTable.dataRowsAndCols[str].first->fullName())) )
                              {
                                   ItemHandle * handle = symbolsTable.dataRowsAndCols[str].first;
                                   s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),handle->fullName() + tr(".") + str + tr("\\1"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + handle->fullName() + tr(".") + str + tr("\\2"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("$")),tr("\\1") + handle->fullName() + tr(".")  + str);
                              }
                         }
                         else
                              if (symbolsTable.dataRowsAndCols.contains(str2) && symbolsTable.dataRowsAndCols[str2].first)
                              {
                              if (! str2.contains(QRegExp(tr("^")+symbolsTable.dataRowsAndCols[str2].first->fullName())) )
                              {
                                   ItemHandle * handle = symbolsTable.dataRowsAndCols[str2].first;
                                   s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),handle->fullName() + tr(".") + str2 + tr("\\1"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + handle->fullName() + tr(".") + str2 + tr("\\2"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("$")),tr("\\1") + handle->fullName() + tr(".")  + str2);
                              }
                              else
                              {
                                   s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),str2 + tr("\\1"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + str + tr("\\2"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_\\.])")+str+tr("$")),tr("\\1") + str);
                              }
                         }
                         else
                         {
                              if (symbolsTable.handlesFirstName.contains(str) && symbolsTable.handlesFirstName[str])
                              {
                                   s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),symbolsTable.handlesFirstName[str]->fullName() + tr("\\1"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + symbolsTable.handlesFirstName[str]->fullName() + tr("\\2"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("$")),tr("\\1") + symbolsTable.handlesFirstName[str]->fullName());
                              }
                              else
                              if (symbolsTable.handlesFirstName.contains(str2) && symbolsTable.handlesFirstName[str2])
                              {
                                   s.replace(QRegExp(tr("^")+str+tr("([^a-zA-Z0-9_])")),symbolsTable.handlesFirstName[str2]->fullName() + tr("\\1"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("([^a-zA-Z0-9_])")), tr("\\1") + symbolsTable.handlesFirstName[str2]->fullName() + tr("\\2"));
                                   s.replace(QRegExp(tr("([^a-zA-Z0-9_])")+str+tr("$")),tr("\\1") + symbolsTable.handlesFirstName[str2]->fullName());
                              }
                              else
                              {
                                   newvars << str;
                              }
                         }
                    }
               }
          }
          catch(mu::Parser::exception_type &e)
          {
               return false;
          }
          return true;
     }
}
