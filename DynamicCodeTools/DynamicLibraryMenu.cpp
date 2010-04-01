/****************************************************************************
Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
see COPYRIGHT.TXT

Provides a toolbar with buttons that call C functions (run of separate threads)

****************************************************************************/
#include <QVBoxLayout>
#include <QDockWidget>
#include <QProcess>
#include <QLibrary>
#include <QRegExp>
#include <QDir>
#include <QFile>
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "DynamicLibraryMenu.h"
#include <QtDebug>

namespace Tinkercell
{

	/***************************
	RUN LIB FILES MENU
	***************************/
	DynamicLibraryMenu::DynamicLibraryMenu() : Tool(tr("Dynamic Library Menu"),tr("Coding")), actionGroup(this)
	{
		treeWidget.setHeaderHidden(true);
		treeWidget.setColumnCount(1);

		treeWidget.addTopLevelItem(new QTreeWidgetItem(QStringList() << tr("")));

		QFont font = treeWidget.font();
		font.setPointSizeF(font.pointSizeF()*1.2);
		treeWidget.setFont(font);

		connect(&actionGroup,SIGNAL(triggered ( QAction *  )),this,SLOT(actionTriggered ( QAction *  )));

		menuButton = 0;
		separator = 0;
		connectTCFunctions();

		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(&treeWidget);
		layout->setContentsMargins(0,0,0,0);
		layout->setSpacing(0);
		setLayout(layout);
	}

	DynamicLibraryMenu::~DynamicLibraryMenu()
	{
		/*for (int i=0; i < graphicalTools.size(); ++i)
		if (graphicalTools[i].second)
		{
		if (graphicalTools[i].second->scene())
		graphicalTools[i].second->scene()->removeItem(graphicalTools[i].second);
		delete (graphicalTools[i].second);
		}*/
		graphicalTools.clear();
	}

	QSize DynamicLibraryMenu::sizeHint() const
	{
		return QSize(200,100);
	}

	void DynamicLibraryMenu::actionTriggered ( QAction *  action )
	{
		if (menuButton)
		{
			disconnect(menuButton,SIGNAL(pressed()),functionsMenu.defaultAction(),SIGNAL(triggered()));
			functionsMenu.setDefaultAction(action);
			connect(menuButton,SIGNAL(pressed()),functionsMenu.defaultAction(),SIGNAL(triggered()));
		}
	}

	QToolButton * DynamicLibraryMenu::addFunction(const QString& category, const QString& functionName, const QIcon& icon)
	{
		QToolButton * toolButton = new QToolButton;
		toolButton->setFont(treeWidget.font());
		toolButton->setAutoRaise(true);
		toolButton->setText(functionName);
		toolButton->setIcon(icon);
		toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		toolButton->setDown(false);
		toolButton->setCheckable(false);

		QTreeWidgetItem * parentItem = 0;
		QList<QTreeWidgetItem*> parents = treeWidget.findItems(category,Qt::MatchFixedString);

		if (parents.size() < 1 || !parents[0])
		{
			parentItem = new QTreeWidgetItem(QStringList() << category);
			parentItem->setIcon(0,icon);
			treeWidget.addTopLevelItem(parentItem);
		}
		else
			parentItem = parents[0];

		QTreeWidgetItem * treeItem = new QTreeWidgetItem;
		parentItem->addChild(treeItem);
		treeWidget.setItemWidget(treeItem,0,toolButton);

		hashFunctionButtons.insert(functionName,toolButton);
		return toolButton;
	}

	QAction * DynamicLibraryMenu::addMenuItem(const QString& category, const QString& functionName, const QIcon& icon, bool defaultAction)
	{
		QAction * action = new QAction(icon,functionName,this);

		if (!menuButton) return action;

		QMenu * menu = 0;

		for (int i=0; i < functionsSubMenus.size(); ++i)
			if (functionsSubMenus[i] && functionsSubMenus[i]->title() == category)
			{
				menu = functionsSubMenus[i];
				break;
			}

			if (!menu)
			{
				menu = new QMenu(&functionsMenu);
				menu->setTitle(category);
				menu->setIcon(icon);
				functionsSubMenus.append(menu);
				functionsMenu.addMenu(menu);
			}

			menu->addAction(action);

			if (!functionsMenu.defaultAction() || defaultAction)
			{
				if (functionsMenu.defaultAction())
					disconnect(menuButton,SIGNAL(pressed()),functionsMenu.defaultAction(),SIGNAL(triggered()));

				if (menu->defaultAction())
					disconnect(menu,SIGNAL(pressed()),menu->defaultAction(),SIGNAL(triggered()));

				menu->setDefaultAction(action);
				functionsMenu.setDefaultAction(action);
				connect(menuButton,SIGNAL(pressed()),functionsMenu.defaultAction(),SIGNAL(triggered()));
			}

			actionGroup.addAction(action);
			return action;
	}

	DynamicLibraryMenu::GraphicalActionTool::GraphicalActionTool(const QString& family, const QString& name, const QPixmap& pixmap, Tool * tool)
		: GraphicsItem(tool), targetAction(QIcon(pixmap),name,0), targetFamily(family)
	{
		QGraphicsPixmapItem * pixmapItem = new QGraphicsPixmapItem(pixmap);
		pixmapItem->scale(30.0/pixmapItem->boundingRect().width(),30.0/pixmapItem->boundingRect().height());
		pixmapItem->setPos(-15.0,-15.0);
		QGraphicsRectItem * rectItem = new QGraphicsRectItem;
		rectItem->setRect(QRectF(-16.0,-16.0,32.0,32.0));
		rectItem->setPen(QPen(Qt::black,2));
		addToGroup(pixmapItem);
		addToGroup(rectItem);
		setToolTip(name);
	}

	void DynamicLibraryMenu::GraphicalActionTool::select()
	{
		targetAction.trigger();
	}

	QAction * DynamicLibraryMenu::addContextMenuItem(const QString& familyName,const QString& functionName, const QPixmap& icon, bool showTool)
	{
		GraphicalActionTool * gtool = new GraphicalActionTool(familyName, functionName,icon,this);
		graphicalTools += QPair<QString,GraphicalActionTool*>(familyName,gtool);
		showGraphicalTool += showTool;
		graphicsItems += gtool;
		return &(gtool->targetAction);
	}

	void DynamicLibraryMenu::select(int)
	{
	}

	void DynamicLibraryMenu::deselect(int)
	{
	}


	bool DynamicLibraryMenu::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			QToolBar * toolBar = mainWindow->toolBarForTools;
			menuButton = new QToolButton(toolBar);
			menuButton->setIcon(QIcon(":/images/play.png"));
			menuButton->setMenu(&functionsMenu);

			menuButton->setPopupMode(QToolButton::MenuButtonPopup);
			//connect(menuButton,SIGNAL(released()),functionsMenu.defaultAction(),SIGNAL(triggered()));

			toolBar->addWidget(menuButton);

			setWindowTitle(tr("Tools"));
			setWindowIcon(QIcon(tr(":/images/function.png")));
			mainWindow->addToolWindow(this, MainWindow::defaultToolWindowOption, Qt::BottomDockWidgetArea);

			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				this,SLOT(itemsSelected(GraphicsScene *,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(itemsInserted(NetworkWindow*, const QList<ItemHandle*>&)),
				this, SLOT(itemsInserted(NetworkWindow*, const QList<ItemHandle*>&)));


			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			return true;
		}
		return false;
	}

	void DynamicLibraryMenu::itemsInserted(NetworkWindow* , const QList<ItemHandle*>& handles)
	{
		for (int i=0; i < handles.size(); ++i)
		{
			if (handles[i] && !handles[i]->tools.contains(this))
			{
				handles[i]->tools += this;
			}
		}
	}

	void DynamicLibraryMenu::GraphicalActionTool::visible(bool b)
	{
		if (!tool) return;
		GraphicsScene * scene = tool->currentScene();
		if (!scene) return;

		ItemHandle * handle = 0;
		QList<QGraphicsItem*>& items = scene->selected();
		bool match = true, nonEmpty = false;

		for (int j=0; j < items.size(); ++j)
		{
			if (handle = getHandle(items[j]))
			{
				nonEmpty = true;
				if (!handle->isA(targetFamily))
				{
					match = false;
					break;
				}
			}
		}

		Tool::GraphicsItem::visible(b && match && nonEmpty);

	}

	void DynamicLibraryMenu::itemsSelected(GraphicsScene * scene,const QList<QGraphicsItem*>& items, QPointF, Qt::KeyboardModifiers)
	{
		if (!mainWindow || !scene) return;

		QList<QAction*> actionsToAdd, actionsToRemove;
		bool match, nonEmpty = false;
		ItemHandle * handle;

		for (int i=0; i < graphicalTools.size(); ++i)
			if (graphicalTools[i].second)
			{
				match = true;
				for (int j=0; j < items.size(); ++j)
				{
					if (handle = getHandle(items[j]))
					{
						nonEmpty = true;
						if (!handle->isA(graphicalTools[i].first))
						{
							match = false;
							break;
						}
					}
				}
				if (match && nonEmpty)
					actionsToAdd << &(graphicalTools[i].second->targetAction);
				else
					actionsToRemove << &(graphicalTools[i].second->targetAction);
			}

			if (actionsToAdd.size() > 0)
			{
				if (separator)
					mainWindow->contextItemsMenu.addAction(separator);
				else
					separator = mainWindow->contextItemsMenu.addSeparator();

				for (int i=0; i < actionsToAdd.size(); ++i)
					mainWindow->contextItemsMenu.addAction(actionsToAdd[i]);
			}
			else
			{
				if (separator)
					mainWindow->contextItemsMenu.removeAction(separator);
			}

			if (actionsToRemove.size() > 0)
			{
				for (int i=0; i < actionsToRemove.size(); ++i)
					mainWindow->contextItemsMenu.removeAction(actionsToRemove[i]);
			}
	}

	/**********************
	C API
	***********************/

	void DynamicLibraryMenu::connectTCFunctions()
	{
		connect(&fToS,SIGNAL(callFunction(QSemaphore*,const QString&)),this,SLOT(callFunction(QSemaphore*,const QString&)));
	}

	typedef void (*tc_DynamicLibraryMenu_api)(
		void (*callFuntion)(const char*)
		);

	void DynamicLibraryMenu::setupFunctionPointers( QLibrary * library)
	{
		tc_DynamicLibraryMenu_api f = (tc_DynamicLibraryMenu_api)library->resolve("tc_DynamicLibraryMenu_api");
		if (f)
		{
			//qDebug() << "tc_DynamicLibraryMenu_api resolved";
			f(
				&(_callFunction)
				);
		}
	}

	void DynamicLibraryMenu::callFunction(QSemaphore* s, const QString& functionName)
	{
		if (hashFunctionButtons.contains(functionName))
		{
			QToolButton * button = hashFunctionButtons[functionName];
			if (button)
				button->animateClick();
		}

		if (s) s->release();
	}

	/******************************************************/

	DynamicLibraryMenu_FToS DynamicLibraryMenu::fToS;

	void DynamicLibraryMenu_FToS::callFunction(const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit callFunction(s,ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
	}

	void DynamicLibraryMenu::_callFunction(const char * c)
	{
		return fToS.callFunction(c);
	}
}
