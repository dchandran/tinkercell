#include <QDebug>
#include <QVBoxLayout>
#include "GraphicsScene.h"
#include "TextEditor.h"
#include "NetworkHandle.h"
#include "MainWindow.h"
#include "GraphicsView.h"
#include "ConsoleWindow.h"
#include "NetworkWindow.h"

namespace Tinkercell
{
	void NetworkWindow::connectToMainWindow()
	{
		if (!network) return;
		MainWindow * main = network->mainWindow;
		
		if (!main) return;
		
		if (scene)
		{
			scene->contextItemsMenu = &(main->contextItemsMenu);
			
			scene->contextScreenMenu = &(main->contextScreenMenu);
		
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

			connect(scene,SIGNAL(itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>& )),
				main ,SIGNAL(itemsMoved(GraphicsScene*,const QList<QGraphicsItem*>&, const QList<QPointF>& )));

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

			connect(scene,SIGNAL(itemsAboutToBeInserted(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&,QList<QUndoCommand*>&)),
				main ,SIGNAL(itemsAboutToBeInserted(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&,QList<QUndoCommand*>&)));

			connect(scene,SIGNAL(itemsAboutToBeRemoved(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&,QList<QUndoCommand*>&)),
				main ,SIGNAL(itemsAboutToBeRemoved(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&,QList<QUndoCommand*>&)));
				
			connect(scene,SIGNAL(itemsAboutToBeMoved(GraphicsScene * , QList<QGraphicsItem*>& , QList<QPointF>& , QList<QUndoCommand*>&)),
				main ,SIGNAL(itemsAboutToBeMoved(GraphicsScene * , QList<QGraphicsItem*>& , QList<QPointF>& , QList<QUndoCommand*>&)));

			connect(scene,SIGNAL(itemsRemoved(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
				main ,SIGNAL(itemsRemoved(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));

			connect(scene,SIGNAL(colorChanged(GraphicsScene*,const QList<QGraphicsItem*>&)),
				main ,SIGNAL(colorChanged(GraphicsScene*,const QList<QGraphicsItem*>&)));

			connect(scene,SIGNAL(parentItemChanged(GraphicsScene*,const QList<QGraphicsItem*>&,const QList<QGraphicsItem*>&)),
				main ,SIGNAL(parentItemChanged(GraphicsScene*,const QList<QGraphicsItem*>&,const QList<QGraphicsItem*>&)));

			connect(network,SIGNAL(handlesChanged(NetworkHandle*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
				main ,SIGNAL(handlesChanged(NetworkHandle*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));

			connect(scene,SIGNAL(escapeSignal(const QWidget*)),
				main ,SIGNAL(escapeSignal(const QWidget*)));

			connect(scene,SIGNAL(filesDropped(const QList<QFileInfo>&)),
				main,SLOT(dragAndDropFiles(const QList<QFileInfo>&)));

			connect(scene,SIGNAL(copyItems(GraphicsScene*, QList<QGraphicsItem*>& , QList<ItemHandle*>&)),
				main, SIGNAL(copyItems(GraphicsScene*, QList<QGraphicsItem*>& , QList<ItemHandle*>&)));
		}
		
		if (editor)
		{
			editor->contextEditorMenu = &(main->contextEditorMenu);

			editor->contextSelectionMenu = &(main->contextSelectionMenu);
		
			connect(editor,SIGNAL(itemsInserted(NetworkHandle *, const QList<ItemHandle*>&)),
				main,SIGNAL(itemsInserted(NetworkHandle *, const QList<ItemHandle*>&)));

			connect(editor,SIGNAL(itemsRemoved(NetworkHandle * , const QList<ItemHandle*>& )),
				main,SIGNAL(itemsRemoved(NetworkHandle * , const QList<ItemHandle*>&)));

			connect(editor,SIGNAL(textChanged(TextEditor *, const QString&, const QString&, const QString&)),
				main,SIGNAL(textChanged(TextEditor *, const QString&, const QString&, const QString&)));

			connect(editor,SIGNAL(lineChanged(TextEditor *, int, const QString&)),
				main,SIGNAL(lineChanged(TextEditor *, int, const QString&)));
		}
	}
	
	NetworkWindow::NetworkWindow(NetworkHandle * network, GraphicsScene * scene)
		: QMainWindow(network->mainWindow), network(network), scene(scene), editor(0), handle(0)
	{
		if (!network) return;
		
		centralWidgetLayout = new QHBoxLayout;
		centralWidgetLayout->setContentsMargins(0,0,0,0);
		QWidget * centralWidget = new QWidget;
		centralWidget->setLayout(centralWidgetLayout);
		setCentralWidget(centralWidget);
		
		if (scene)
		{
			scene->networkWindow = this;
			scene->network = network;
			
			GraphicsView * view = new GraphicsView(this);		
			centralWidgetLayout->addWidget(view);
			setAttribute(Qt::WA_DeleteOnClose);
		}

		//if (!network->networkWindows.contains(this))
			//network->networkWindows += this;

		setWindowFlags(Qt::Window);
		show();

		MainWindow * main = network->mainWindow;

		if (main)
		{
			connect(this,SIGNAL(networkClosing(NetworkHandle *, bool*)),main,SIGNAL(networkClosing(NetworkHandle *, bool*)));
			connect(this,SIGNAL(networkClosed(NetworkHandle *)),main,SIGNAL(networkClosed(NetworkHandle *)));
			connectToMainWindow();
			setWindowTitle(tr("network ") + QString::number(1 + main->allNetworks.size()));
			main->setCurrentWindow(this);
		}
		
		setFocusPolicy(Qt::StrongFocus);
		setWindowIcon(QIcon(tr(":/images/newscene.png")));
	}
	
	NetworkWindow::NetworkWindow(NetworkHandle * network, TextEditor * editor)
		: QMainWindow(network->mainWindow), network(network), scene(0), editor(editor), handle(0)
	{
		if (!network) return;
		
		centralWidgetLayout = new QHBoxLayout;
		centralWidgetLayout->setContentsMargins(0,0,0,0);
		QWidget * centralWidget = new QWidget;
		centralWidget->setLayout(centralWidgetLayout);
		setCentralWidget(centralWidget);
		
		if (editor)
		{
			editor->networkWindow = this;
			editor->network = network;
			centralWidgetLayout->addWidget(editor);
			setAttribute(Qt::WA_DeleteOnClose);
		}

		MainWindow * main = network->mainWindow;
		
		//if (!network->networkWindows.contains(this))
		//	network->networkWindows += this;
		
		setWindowFlags(Qt::Window);
		show();
		
		if (main)
		{
			connect(this,SIGNAL(networkClosing(NetworkHandle *, bool*)),main,SIGNAL(networkClosing(NetworkHandle *, bool*)));
			connect(this,SIGNAL(networkClosed(NetworkHandle *)),main,SIGNAL(networkClosed(NetworkHandle *)));
			connectToMainWindow();
			setWindowTitle(tr("network ") + QString::number(1 + main->allNetworks.size()));
			main->setCurrentWindow(this);
		}
		
		setFocusPolicy(Qt::StrongFocus);
		setWindowIcon(QIcon(tr(":/images/newtext.png")));
	}

	void NetworkWindow::closeEvent(QCloseEvent * event)
	{		
		if (scene)
			scene->deselect();
		
		if (network && network->networkWindows.contains(this))
		{
			if (network->networkWindows.size() <= 1)
			{
				bool b = true;
		
				if (network)
					emit networkClosing(network,&b);
		
				if (b)
				{
					emit networkClosed(network);
					network->networkWindows.removeAll(this);

					if (network->mainWindow && network->mainWindow->currentNetworkWindow == this)
						network->mainWindow->currentNetworkWindow = 0;

					network->close();
					event->accept();
				}
			}
			else
			{
				network->networkWindows.removeAll(this);

				if (network->mainWindow && network->mainWindow->currentNetworkWindow == this)
					network->mainWindow->currentNetworkWindow = 0;

				event->accept();
			}
		}
		else
			event->accept();
	}
	
	void NetworkWindow::focusInEvent ( QFocusEvent * )
	{
		if (network && network->mainWindow && network->mainWindow->currentNetworkWindow != this)
			network->mainWindow->setCurrentWindow(this);
	}

	void NetworkWindow::resizeEvent (QResizeEvent * event)
	{
		if (network && network->mainWindow && windowState() == Qt::WindowMinimized)
		{
			setWindowState(Qt::WindowNoState);
			popIn();
		}
		else
			QWidget::resizeEvent(event);
	}

	void NetworkWindow::setAsCurrentWindow()
	{
		if (network && network->mainWindow && network->mainWindow->currentNetworkWindow != this)
			network->mainWindow->setCurrentWindow(this);
	}

	void NetworkWindow::popOut()
	{
		if (network && network->mainWindow)
			network->mainWindow->popOut(this);
	}

	void NetworkWindow::popIn()
	{
		if (network && network->mainWindow)
			network->mainWindow->popIn(this);
	}

	void NetworkWindow::changeEvent ( QEvent * event )
	{
		if (network && network->mainWindow && windowState() == Qt::WindowMinimized)
		{
			setWindowState(Qt::WindowNoState);
			popIn();
		}
		else
			QWidget::changeEvent(event);
	}
	
	GraphicsScene * NetworkWindow::newScene()
	{
		if (!network) return 0;
		QList<QGraphicsView*> views;
		
		if (scene)
		{
			QList<QGraphicsView*> views = scene->views();
			delete scene;
			for (int i=0; i < views.size(); ++i)
				if (views[i])
					delete views[i];
			scene = 0;
		}
		
		if (editor)
		{
			delete editor;
			editor = 0;
		}
		
		if (editor)
		{
			delete editor;
			editor = 0;
		}
		
		scene = new GraphicsScene(network);
		scene->networkWindow = this;
		scene->network = network;
		
		GraphicsView * view = new GraphicsView(this);
		
		if (centralWidgetLayout)
			delete centralWidgetLayout;
		centralWidgetLayout = new QHBoxLayout;
		centralWidgetLayout->setContentsMargins(0,0,0,0);
		centralWidgetLayout->addWidget(view);
	
		if (centralWidget())
			centralWidget()->setLayout(centralWidgetLayout);
		connectToMainWindow();
		
		return scene;
	}

	TextEditor * NetworkWindow::newTextEditor()
	{
		if (!network) return 0;
		
		if (scene)
		{
			QList<QGraphicsView*> views = scene->views();
			delete scene;
			for (int i=0; i < views.size(); ++i)
				if (views[i] && views[i]->parentWidget() == this)
					delete views[i];
			scene = 0;
		}
		
		if (editor)
		{
			delete editor;
			editor = 0;
		}
		
		editor = new TextEditor(network);
		editor->networkWindow = this;
		editor->network = network;
	
		if (centralWidgetLayout)
			delete centralWidgetLayout;
		centralWidgetLayout = new QHBoxLayout;
		centralWidgetLayout->setContentsMargins(0,0,0,0);
		centralWidgetLayout->addWidget(editor);
	
		if (centralWidget())
			centralWidget()->setLayout(centralWidgetLayout);		
		
		
		connectToMainWindow();
		
		return editor;
	}

	/*bool NetworkHandle::winEvent ( MSG * m, long * result )
	{
		if (mainWindow && m->message == WM_SIZE && m->wParam == SIZE_MINIMIZED)
		{
			popIn();
		}
		return QWidget::winEvent(m,result);
	}*/
	
}


