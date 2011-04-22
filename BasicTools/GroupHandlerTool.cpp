/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool allows NodeGraphicsItems to be grouped together (i.e. merge handlers).
A special QUndoCommand is provided for this functionality. Buttons are also placed
in the MainWindow toolbar.

****************************************************************************/
#include <QMessageBox>
#include "NetworkHandle.h"
#include "GraphicsScene.h"
#include "CThread.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "GroupHandlerTool.h"
#include "CollisionDetection.h"
#include "ConsoleWindow.h"
#include "GraphicsReplaceTool.h"

namespace Tinkercell
{
	GroupHandlerTool::GroupHandlerTool() : Tool(tr("Merge Handles"),tr("Basic GUI"))
	{
		GroupHandlerTool::fToS = new GroupHandlerTool_FToS;
		GroupHandlerTool::fToS->setParent(this);
		connectTCFunctions();
	}

	bool GroupHandlerTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			/*QToolBar * toolBar = new QToolBar(tr("Merging tool"),mainWindow);
			
			QAction * group = new QAction(QIcon(tr(":/images/group.png")),tr("Merge"),toolBar);
			connect(group,SIGNAL(triggered()),this,SLOT(merge()));
			group->setToolTip(tr("Merge selected items"));
			group->setShortcut(tr("CTRL+M"));
			toolBar->addAction(group);

			QAction * ungroup = new QAction(QIcon(tr(":/images/ungroup.png")),tr("Separate"),toolBar);
			connect(ungroup,SIGNAL(triggered()),this,SLOT(separate()));
			ungroup->setToolTip(tr("Separate selected items"));
			ungroup->setShortcut(tr("CTRL+SHIFT+M"));
			toolBar->addAction(ungroup);
			*/

			QAction * alias = new QAction(QIcon(tr(":/images/alias.png")),tr("Alias"),this);
			connect(alias,SIGNAL(triggered()),this,SLOT(alias()));
			alias->setToolTip(tr("Create an alias of selected items"));
			//toolBar->addAction(alias);
			
			QAction * decorate = new QAction(QIcon(tr(":/images/star.png")),tr("Add decorator"),this);
			connect(decorate,SIGNAL(triggered()),this,SLOT(decorate()));
			alias->setToolTip(tr("Add a decoration item to selected object"));
			//toolBar->addAction(alias);

			/*if (mainWindow->toolBarEdits)
			{
				mainWindow->toolBarEdits->addAction(decorate);
			}*/

			mainWindow->contextItemsMenu.addAction(alias);
			mainWindow->contextItemsMenu.addAction(decorate);

			if (mainWindow->editMenu)
			{
				mainWindow->editMenu->addAction(decorate);
			}

			connect(this,SIGNAL(handlesChanged(NetworkHandle*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
				mainWindow,SIGNAL(handlesChanged(NetworkHandle*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
			
			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));
			
			toolLoaded(0);

			return true;
		}
		return false;
	}

	void GroupHandlerTool::connectCollisionDetector()
	{
		static bool alreadyConnected = false;
		if (alreadyConnected || !mainWindow) return;
		
		if (mainWindow->tool(tr("Graphics Replace Tool")))
		{
			QWidget * widget = mainWindow->tool(tr("Graphics Replace Tool"));
			GraphicsReplaceTool * replaceTool = static_cast<GraphicsReplaceTool*>(widget);
			if (replaceTool)
			{
				alreadyConnected = true;
				connect(this,SIGNAL(substituteNodeGraphics()), replaceTool, SLOT(substituteNodeGraphics()));
			}
		}
	}

	void GroupHandlerTool::toolLoaded(Tool*)
	{
		connectCollisionDetector();
	}

	void GroupHandlerTool::merge()
	{
		if (mainWindow == 0 || mainWindow->currentScene() == 0) return;
		GraphicsScene * scene = mainWindow->currentScene();

		mergeItems(scene,scene->selected());
	}

	ItemFamily* GroupHandlerTool::findBestFamilyOption(QList<ItemFamily*>& list)
	{
		for (int i=0; i < list.size(); ++i)
		{
			ItemFamily * best = list[i];
			for (int j=0; j < list.size(); ++j)
			{
				if (best && !best->isA(list[j]))
				{
					best = 0;
					break;
				}
			}
			if (best) return best;
		}
		return 0;
	}

	void GroupHandlerTool::mergeItems(GraphicsScene * scene, QList<QGraphicsItem*>& list)
	{
		if (!scene || list.isEmpty()) return;

		QList<QGraphicsItem*> items;
		QList<ItemHandle*> handles;
		ItemFamily * family = 0;
		NodeGraphicsItem* node;
		bool hasConnections = false;

		for (int i=0; i < list.size(); ++i)
		{
			node = NodeGraphicsItem::topLevelNodeItem(list[i]);
			if (node && node->handle() && !node->connections().isEmpty())
			{
				hasConnections = true;
				break;
			}
		}

		ItemHandle * bestHandle = 0;
		QGraphicsItem * bestItem = 0;

		QList<ItemFamily*> families;
		for (int i=0; i < list.size(); ++i)
		{
			ItemHandle * handle = getHandle(list[i]);
			if (handle && handle->family())
			{
				families += handle->family();
			}
		}

		family = findBestFamilyOption(families);

		for (int i=0; i < list.size(); ++i)
		{
			ItemHandle * handle = getHandle(list[i]);
			if (handle && ((!family && !hasConnections) || (family && handle->family() == family)))
			{
				if (!family) family = handle->family();
				bestHandle = handle;
				bestItem = list[i];
				break;
			}
		}
		if (!bestHandle) return;

		handles += bestHandle;
		items += bestItem;

		for (int i=0; i < list.size(); ++i)
		{
			ItemHandle * handle = getHandle(list[i]);

			//connections cannot be grouped
			bool notAllowed = false;

			if (handle)
			{
				for (int j=0; j < handle->graphicsItems.size(); ++j)
					if (ConnectionGraphicsItem::cast(handle->graphicsItems[j]))
					{
						notAllowed = true;
						break;
					}
			}

			if (notAllowed) continue;

			if (handle == 0)
			{
				//only text items can be merged without a handle
				if (TextGraphicsItem::cast(list[i]))
				{
					if (!items.contains(list[i]))
					{
						items += list[i];
						handles += handle;
					}
				}
			}
			else
			{
				if (handle && handle->family())
				{
					if (((family->isRelatedTo(handle->family())) && !hasConnections)
						|| (hasConnections && family->isA(handle->family())))
					{
						if (!items.contains(list[i]) && !handles.contains(handle))
						{
							items += list[i];
							handles += handle;
						}
					}
				}
			}
		}

		int numHandles = 0;
		QList<ItemHandle*> oldHandles;
		for (int i=0; i < items.size(); ++i)
		{
			oldHandles += getHandle(items[i]);
			if (oldHandles.last()) ++numHandles;
		}



		if (numHandles > 1)
		{
			MergeHandlesCommand * mergeCommand = new MergeHandlesCommand(tr("items merged"),scene->network,handles);

			scene->deselect();

			/*QString newName = mergeCommand->newHandle->fullName();
			QList<QString> oldNames,newNames;

			QList<QGraphicsItem*> textToDelete;
			TextGraphicsItem * nameText = 0;

			for (int i=0; i < handles.size(); ++i)
			{
				if (handles[i])
					for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
						if ((nameText = TextGraphicsItem::cast(handles[i]->graphicsItems[j]))
							&& nameText->toPlainText() == handles[i]->name)
							textToDelete += nameText;
			}

			if (textToDelete.size() > 1)
				textToDelete.pop_front();

			for (int i=0; i < handles.size(); ++i)
			{
				newNames += newName;
			}

			QList<QUndoCommand*> commands;
			commands += mergeCommand;
			if (textToDelete.size() > 0)
				commands += new RemoveGraphicsCommand(tr("remove text"),scene,textToDelete);
			commands += new RenameCommand(tr("name changed"),scene->network,handles,newNames);
			QUndoCommand * command = new CompositeCommand(tr("items merged"),commands);

			QList<ItemHandle*> oldHandles;
			for (int i=0; i < items.size(); ++i)
				oldHandles += getHandle(items[i]);*/

			if (scene->network)
				scene->network->push(mergeCommand);
			else
			{
				mergeCommand->redo();
				delete mergeCommand;
			}
			emit handlesChanged(scene->network,items,oldHandles);
			//scene->mergeHandles(handles);
		}
		else
		{
			if (handles.size() > 0 && handles[0] && !items.isEmpty())
			{
				bool allSame = true;
				for (int i=0; i < items.size(); ++i)
					if (getHandle(items[i]) != handles[0])
					{
						allSame = false;
						break;
					}
					if (!allSame)
					{
						QUndoCommand* command = new AssignHandleCommand(tr("items merged"),items,handles[0]);

						QList<ItemHandle*> oldHandles;
						for (int i=0; i < items.size(); ++i)
							oldHandles += getHandle(items[i]);

						if (scene->network)
							scene->network->push(command);
						else
						{
							command->redo();
							delete command;
						}
						emit handlesChanged(scene->network,items,oldHandles);
					}
			}
			else
				if (list.size() > 1)
				{
					if (console())
                        console()->error(tr("Only items belonging to related families can be merged"));
				}
		}
	}

	void GroupHandlerTool::separate()
	{
		if (mainWindow == 0 || mainWindow->currentScene() == 0) return;
		GraphicsScene * scene = mainWindow->currentScene();
		separateItems(scene,scene->selected());
	}

	void GroupHandlerTool::separateItems(GraphicsScene* scene, QList<QGraphicsItem*>& selected)
	{
		if (!scene || selected.isEmpty()) return;

		QList<QGraphicsItem*> list;
		QList<ItemHandle*> handles;

		ItemHandle * handle = 0;
		for (int i=0; i < selected.size(); ++i)
		{
			if (NodeGraphicsItem::cast(selected[i]) ||
				ConnectionGraphicsItem::cast(selected[i]) )
			{
				if ((handle = getHandle(selected[i])))
				{
					bool allUnique = true;
					for (int j=0; j < handle->graphicsItems.size(); ++j)
						if (NodeGraphicsItem::cast(handle->graphicsItems[j]) ||
							ConnectionGraphicsItem::cast(handle->graphicsItems[j]))
						{
							allUnique = false;
							break;
						}
						if (allUnique) continue; //no need to isolate this one

						list += selected[i];
						ItemHandle* handleClone = handle->clone();
						handles += handleClone;
						handleClone->graphicsItems.clear();

						//assign same handle to nearby text items
						for (int j=0; j < handle->graphicsItems.size(); ++j)
						{
							if (TextGraphicsItem::cast(handle->graphicsItems[j]))
							{
								QRectF rect = selected[i]->sceneBoundingRect();
								rect.adjust(-10,-10,20,20);
								if (rect.intersects(handle->graphicsItems[j]->sceneBoundingRect()))
								{
									list += handle->graphicsItems[j];
									handles += handleClone;
								}
							}
						}
				}
			}
		}

		if (!list.isEmpty() && !handles.isEmpty())
		{
			scene->clearSelection();
			QUndoCommand * command1 = new AssignHandleCommand(tr(""),list,handles);

			QList<ItemHandle*> allitems = scene->network->handles();
			QList<QString> newNames;

			QStringList allNames;
			ItemHandle * itemHandle;

			for (int i=0; i < allitems.size(); ++i)
			{
				if ((itemHandle = allitems[i]))
					allNames << itemHandle->name << itemHandle->fullName();
			}

			for (int i=0; i < handles.size() && i < list.size(); ++i)
			{
				newNames << findUniqueName(allNames);
				allNames << newNames.last();
			}

			QUndoCommand * command2 = new RenameCommand(tr(""),scene->network,allitems,handles,newNames);
			QUndoCommand * command = new CompositeCommand(tr("items separated"),(QList<QUndoCommand*>() << command1 << command2));

			QList<ItemHandle*> oldHandles;
			for (int i=0; i < list.size(); ++i)
				oldHandles += getHandle(list[i]);

			if (scene->network)
				scene->network->push(command);
			else
			{
				command->redo();
				delete command;
			}

			emit handlesChanged(scene->network,list,oldHandles);
		}

	}

	void GroupHandlerTool::connectTCFunctions()
	{
		connect(fToS,SIGNAL(merge(QSemaphore*, QList<ItemHandle*>&)),
			this,SLOT(merge(QSemaphore*, QList<ItemHandle*>&)));

		connect(fToS,SIGNAL(separate(QSemaphore*,ItemHandle*)),
			this,SLOT(separate(QSemaphore*,ItemHandle*)));
	}

	typedef void (*tc_GroupHandlerTool_api)(
		void (*merge)(tc_items),
		void (*separate)(long)
		);

	void GroupHandlerTool::setupFunctionPointers( QLibrary * library)
	{
		tc_GroupHandlerTool_api f = (tc_GroupHandlerTool_api)library->resolve("tc_GroupHandlerTool_api");
		if (f)
		{
			//qDebug() << "tc_GroupHandlerTool_api resolved";
			f(
				&(_merge),
				&(_separate)
				);
		}
	}

	void GroupHandlerTool::merge(QSemaphore* sem, QList<ItemHandle*>& handles)
	{
		GraphicsScene * scene = currentScene();
		if (scene)
		{
			QList<QGraphicsItem*> items;
			for (int i=0; i < handles.size(); ++i)
				if (mainWindow->isValidHandlePointer(handles[i]))
					items << handles[i]->graphicsItems;
			mergeItems(scene,items);
		}
		if (sem)
			sem->release();
	}

	void GroupHandlerTool::separate(QSemaphore* sem,ItemHandle* handle)
	{
		GraphicsScene * scene = currentScene();
		if (scene)
		{
			if (mainWindow->isValidHandlePointer(handle))
				separateItems(scene,handle->graphicsItems);
		}
		if (sem)
			sem->release();
	}


	void GroupHandlerTool::alias()
	{
		if (!mainWindow || !mainWindow->currentScene()) return;

		GraphicsScene * scene = mainWindow->currentScene();
		QList<QGraphicsItem*> newItems;

		QList<QGraphicsItem*> & selected = scene->selected();
		for (int i=0; i<selected.size(); ++i)
			if ((NodeGraphicsItem::cast(selected[i]) && getHandle(selected[i]))
				|| TextGraphicsItem::cast(selected[i]))
			{
				QGraphicsItem * item = cloneGraphicsItem(selected[i]);
				newItems += item;
				item->setPos( item->scenePos() + QPointF(100,100) );
			}

		scene->insert(tr("alias inserted"),newItems);
	}
	
	void GroupHandlerTool::decorate()
	{
		if (!mainWindow || !mainWindow->currentScene()) return;
		
		GraphicsScene * scene = mainWindow->currentScene();
		QList<QGraphicsItem*> & selected = scene->selected();
		
		if (selected.size() != 1)
		{
			QMessageBox::information(this,tr("Cannot add items"),tr("Please select exactly one item"));
			return;
		}
		
		NodeGraphicsItem * node = NodeGraphicsItem::cast(selected[0]);
		
		if (!node)
		{
			QMessageBox::information(this,tr("Cannot add items"),tr("Please select a node item (not connections or text)"));
			return;
		}
		
		QGraphicsItem * copy = cloneGraphicsItem(node);
		copy->setPos( node->scenePos() + QPointF(50,50) );
		scene->insert(tr("decorator inserted"),copy);
		scene->selected().clear();
		scene->selected() += copy;
		
		emit substituteNodeGraphics();
	}

	void GroupHandlerTool::nodeCollided(const QList<QGraphicsItem*>& list, NodeGraphicsItem * item, QPointF)
	{
		if (mainWindow == 0 || mainWindow->currentScene() == 0 || item == 0 || list.isEmpty()) return;
		GraphicsScene * scene = mainWindow->currentScene();

		QList<QGraphicsItem*> items;
		QList<ItemHandle*> handles;

		ItemHandle * handle = getHandle(item);
		ItemFamily * family = 0;
		NodeGraphicsItem * node = item;
		bool hasConnections = false;

		if (node && node->handle() && !node->connections().isEmpty())
		{
			hasConnections = true;
		}
		else
			for (int i=0; i < list.size(); ++i)
			{
				node = NodeGraphicsItem::topLevelNodeItem(list[i]);
				if (node && node->handle() && !node->connections().isEmpty())
				{
					hasConnections = true;
					break;
				}
			}

			if (handle && handle->family())
			{
				handles += handle;
				family = handle->family();
			}
			else
			{
				return;
			}

			ItemHandle * bestHandle = handle;
			family = handle->family();

			for (int i=0; i < list.size(); ++i)
			{
				if (list[i])
				{
					handle = getHandle(list.at(i));
					if (NodeGraphicsItem::cast(list.at(i)) && handle && handle->family() && !handles.contains(handle))
					{
						if (!( handle->family()->isA(tr("Compartment")) || handle->family()->isA(tr("Module")))
							&&
							(((family->isRelatedTo(handle->family())) && !hasConnections)
							|| (hasConnections && family->isA(handle->family()))))
						{
							handles += handle;
							if (!items.contains(list[i]))
								items += list[i];
						}
					}
					else
						if (!handle && TextGraphicsItem::cast(list[i]))
						{
							if (!items.contains(list[i]))
								items += list[i];
						}
				}
			}

			if (handle  && handle->family() && handle->family()->isA(tr("DNA")))
			{
				QList<QGraphicsItem*> moveItems;
				QList<QPointF> points;
				for (int i=0; i < items.size(); ++i)
					if (NodeGraphicsItem::cast(items[i]))
					{
						moveItems += items[i];
						if (item->scenePos().x() < items[i]->scenePos().x())
							points += QPointF(item->sceneBoundingRect().right() - items[i]->sceneBoundingRect().left(),
							item->sceneBoundingRect().top() - items[i]->sceneBoundingRect().top());
						else
							points += QPointF(item->sceneBoundingRect().left() - items[i]->sceneBoundingRect().right(),
							item->sceneBoundingRect().top() - items[i]->sceneBoundingRect().top());
					}
					if (!moveItems.isEmpty())
					{
						QUndoCommand * command = new MoveCommand(scene,moveItems,points);
						if (scene->network)
							scene->network->push(command);
						else
						{
							command->redo();
							delete command;
						}
					}
			}

			if (handles.size() > 1 && bestHandle)
			{
				MergeHandlesCommand * mergeCommand = new MergeHandlesCommand(tr("items merged"),scene->network,handles);

				scene->deselect();
				QList<ItemHandle*> oldHandles;
				for (int i=0; i < items.size(); ++i)
					oldHandles += getHandle(items[i]);

				if (scene->network)
					scene->network->push(mergeCommand);
				else
				{
					mergeCommand->redo();
					delete mergeCommand;
				}
				emit handlesChanged(scene->network,items,oldHandles);
			}
			else
				if (handles.size() == 1 && handles[0] && !items.isEmpty())
				{
					QUndoCommand * command = new AssignHandleCommand(tr("items merged"),items,handles[0]);

					QList<ItemHandle*> oldHandles;
					for (int i=0; i < items.size(); ++i)
						oldHandles += getHandle(items[i]);

					if (scene->network)
						scene->network->push(command);
					else
					{
						command->redo();
						delete command;
					}

					emit handlesChanged(scene->network,items,oldHandles);
				}
	}

	QString GroupHandlerTool::findUniqueName(const QStringList& names)
	{
		int	c = 0;
		QString name("A");
		while (names.contains(name))
		{
			if (c < 26)
				name = QString((char)((int)'A' + c));
			else
				name =  QString((char)((int)'A' + (c % 26))) + QString::number(c/25);
			++c;
		}

		return name;
	}

	/************************************************/

	GroupHandlerTool_FToS *  GroupHandlerTool::fToS;

	void GroupHandlerTool::_merge( tc_items A)
	{
		return fToS->merge(A);
	}

	void GroupHandlerTool::_separate(long o)
	{
		return fToS->separate(o);
	}

	void GroupHandlerTool_FToS::merge(tc_items a0)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		QList<ItemHandle*> * list = ConvertValue(a0);
		emit merge(s,*list);
		s->acquire();
		s->release();
		delete s;
		delete list;
	}

	void GroupHandlerTool_FToS::separate(long a0)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit separate(s,ConvertValue(a0));
		s->acquire();
		s->release();
		delete s;
	}


}
