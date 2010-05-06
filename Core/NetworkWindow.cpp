#include <QVBoxLayout>
#include "GraphicsScene.h"
#include "TextEditor.h"
#include "NetworkHandle.h"
#include "MainWindow.h"
#include "GraphicsView.h"
#include "NetworkWindow.h"

namespace Tinkercell
{
	NetworkWindow::NetworkWindow(NetworkHandle * network, GraphicsScene * scene)
		: QWidget(network), network(network), scene(scene), editor(0)
	{
		if (!network)
			return;
		
		if (scene)
		{
			GraphicsView * view = new GraphicsView(this);
			QVBoxLayout * layout = new QVBoxLayout;
			layout->setContentsMargins(0,0,0,0);
			layout->addWidget(view);
			setLayout(layout);
		}

		if (network->networkWindows.size() == 0 && network->mainWindow)
		{
			connect(this,SIGNAL(windowClosing(NetworkWindow *, bool*)),
					network->mainWindow,SIGNAL(windowClosing(NetworkWindow *, bool*)));
			connect(this,SIGNAL(windowClosed(NetworkWindow *)),
					network->mainWindow,SIGNAL(windowClosed(NetworkWindow *)));
		}
		
		if (!network->networkWindows.contains(this))
			network->networkWindows += this;

		MainWindow * main = network->mainWindow;

		if (main)
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

			connect(scene,SIGNAL(escapeSignal(const QWidget*)),
				main ,SIGNAL(escapeSignal(const QWidget*)));

			connect(scene,SIGNAL(filesDropped(const QList<QFileInfo>&)),
				main,SLOT(dragAndDropFiles(const QList<QFileInfo>&)));

			connect(scene,SIGNAL(copyItems(GraphicsScene*, QList<QGraphicsItem*>& , QList<ItemHandle*>&)),
				main, SIGNAL(copyItems(GraphicsScene*, QList<QGraphicsItem*>& , QList<ItemHandle*>&)));

			setWindowTitle(tr("network ") + QString::number(1 + main->allNetworks.size()));
			
			main->setCurrentWindow(this);
		}
		
		setFocusPolicy(Qt::StrongFocus);
		setWindowIcon(QIcon(tr(":/images/newscene.png")));
	}
	
	NetworkWindow::NetworkWindow(NetworkHandle * network, TextEditor * editor)
		: QWidget(network), network(network), scene(0), editor(editor)
	{
		if (!network) return;
		
		if (editor)
		{
			QVBoxLayout * layout = new QVBoxLayout;
			layout->setContentsMargins(0,0,0,0);
			layout->addWidget(editor);
			setLayout(layout);
		}
		
		MainWindow * main = network->mainWindow;

		if (network->networkWindows.size() == 0 && network->mainWindow)
		{
			connect(this,SIGNAL(windowClosing(NetworkWindow *, bool*)),
					network->mainWindow,SIGNAL(windowClosing(NetworkWindow *, bool*)));
			connect(this,SIGNAL(windowClosed(NetworkWindow *)),
					network->mainWindow,SIGNAL(windowClosed(NetworkWindow *)));
		}
		
		if (!network->networkWindows.contains(this))
			network->networkWindows += this;
		
		if (main)
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

			connect(this,SIGNAL(parentHandleChanged(NetworkHandle*, const QList<ItemHandle*>&, const QList<ItemHandle*>&)),
				main ,SIGNAL(parentHandleChanged(NetworkHandle*, const QList<ItemHandle*>&, const QList<ItemHandle*>&)));

			connect(this,SIGNAL(dataChanged(const QList<ItemHandle*>&)),
				main ,SIGNAL(dataChanged(const QList<ItemHandle*>&)));

			connect(this,SIGNAL(itemsRenamed(NetworkHandle*, const QList<ItemHandle*>&, const QList<QString>&, const QList<QString>&)),
				main ,SIGNAL(itemsRenamed(NetworkHandle*, const QList<ItemHandle*>&, const QList<QString>&, const QList<QString>&)));

			setWindowTitle(tr("network ") + QString::number(1 + main->allNetworks.size()));

			main->setCurrentWindow(this);
		}
		
		setFocusPolicy(Qt::StrongFocus);
		setWindowIcon(QIcon(tr(":/images/newtext.png")));
	}
	
	void NetworkWindow::closeEvent(QCloseEvent * event)
	{
		bool b = true;
		
		if (network)
			emit windowClosing(network,&b);
		
		if (b)
		{
			if (network)
			{
				emit windowClosed(network);
				network->networkWindows.removeAll(this);
				network->close();
			}

			event->accept();
		}
	}
	
}


