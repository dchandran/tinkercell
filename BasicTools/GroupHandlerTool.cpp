/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool allows NodeGraphicsItems to be grouped together (i.e. merge handlers). 
A special QUndoCommand is provided for this functionality. Buttons are also placed
in the MainWindow toolbar.

****************************************************************************/

#include "NetworkWindow.h"
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

namespace Tinkercell
{
	GroupHandlerTool::GroupHandlerTool() : Tool(tr("Merge Handles"))
	{
		connectTCFunctions();
	}

	bool GroupHandlerTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			QToolBar * toolBar = new QToolBar(tr("Merging tool"),mainWindow);
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

			QAction * alias = new QAction(QIcon(tr(":/images/alias.png")),tr("Alias"),toolBar);
			connect(alias,SIGNAL(triggered()),this,SLOT(alias()));
			alias->setToolTip(tr("Alias selected items"));
			//toolBar->addAction(alias);

			if (mainWindow->toolBarEdits)
				mainWindow->toolBarEdits->addAction(alias);

			mainWindow->addToolBar(toolBar);

			mainWindow->contextItemsMenu.addAction(alias);
			mainWindow->contextItemsMenu.addAction(group);
			mainWindow->contextItemsMenu.addAction(ungroup);

			if (mainWindow->editMenu)
			{
				mainWindow->editMenu->addAction(group);
				mainWindow->editMenu->addAction(ungroup);
			}

			//connectCollisionDetector();

			connect(this,SIGNAL(handlesChanged(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
				mainWindow,SIGNAL(handlesChanged(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));

			//connect(mainWindow,SIGNAL(toolLoaded(Tool*),this,SLOT(toolLoaded(Tool*)));

			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			return true;
		}
		return false;
	}

	void GroupHandlerTool::connectCollisionDetector()
	{
		static bool alreadyConnected = false;
		if (alreadyConnected || !mainWindow) return;

		if (mainWindow->tool(tr("Collision Detection")))
		{
			QWidget * widget = mainWindow->tool(tr("Collision Detection"));
			CollisionDetection * collisionDetection = static_cast<CollisionDetection*>(widget);
			if (collisionDetection)
			{
				alreadyConnected = true;
				connect(collisionDetection,SIGNAL(nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , QPointF , Qt::KeyboardModifiers )),
					this, SLOT( nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , QPointF , Qt::KeyboardModifiers )));
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
			if (node && node->itemHandle && !node->connections().isEmpty())
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
					if (qgraphicsitem_cast<ConnectionGraphicsItem*>(handle->graphicsItems[j]))
					{
						notAllowed = true;
						break;
					}
			}

			if (notAllowed) continue;

			if (handle == 0) 
			{
				//only text items can be merged without a handle
				if (qgraphicsitem_cast<TextGraphicsItem*>(list[i])) 
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
			MergeHandlesCommand * mergeCommand = new MergeHandlesCommand(tr("items merged"),scene->networkWindow,handles);
			
			scene->deselect();

			/*QString newName = mergeCommand->newHandle->fullName();
			QList<QString> oldNames,newNames;

			QList<QGraphicsItem*> textToDelete;
			TextGraphicsItem * nameText = 0;

			for (int i=0; i < handles.size(); ++i)
			{
				if (handles[i])
					for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
						if ((nameText = qgraphicsitem_cast<TextGraphicsItem*>(handles[i]->graphicsItems[j]))
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
			commands += new RenameCommand(tr("name changed"),scene->networkWindow,handles,newNames);
			QUndoCommand * command = new CompositeCommand(tr("items merged"),commands);

			QList<ItemHandle*> oldHandles;
			for (int i=0; i < items.size(); ++i)
				oldHandles += getHandle(items[i]);*/

			if (scene->historyStack)
				scene->historyStack->push(mergeCommand);
			else
			{
				mergeCommand->redo();
				delete mergeCommand;
			}
			emit handlesChanged(scene,items,oldHandles);
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

						if (scene->historyStack)
							scene->historyStack->push(command);
						else
						{
							command->redo();
							delete command;
						}
						emit handlesChanged(scene,items,oldHandles);
					}
			}
			else
				if (list.size() > 1)
				{
					//QMessageBox::information(0,tr("Group items"),tr("Only items belonging to related families can be merged"),QMessageBox::Ok,QMessageBox::Ok);
					ConsoleWindow::error(tr("Only items belonging to related families can be merged"));
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
			if (qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]) ||
				qgraphicsitem_cast<ConnectionGraphicsItem*>(selected[i]) )
			{
				if ((handle = getHandle(selected[i])))
				{
					bool allUnique = true;
					for (int j=0; j < handle->graphicsItems.size(); ++j)
						if (qgraphicsitem_cast<NodeGraphicsItem*>(handle->graphicsItems[j]) ||
							qgraphicsitem_cast<ConnectionGraphicsItem*>(handle->graphicsItems[j]))
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
							if (qgraphicsitem_cast<TextGraphicsItem*>(handle->graphicsItems[j]))						
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

			QList<ItemHandle*> allitems = scene->allHandles();
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

			QUndoCommand * command2 = new RenameCommand(tr(""),allitems,handles,newNames);
			QUndoCommand * command = new CompositeCommand(tr("items separated"),(QList<QUndoCommand*>() << command1 << command2));

			QList<ItemHandle*> oldHandles;
			for (int i=0; i < list.size(); ++i)
				oldHandles += getHandle(list[i]);

			if (scene->historyStack)
				scene->historyStack->push(command);
			else
			{
				command->redo();
				delete command;
			}

			emit handlesChanged(scene,list,oldHandles);
		}

	}

	void GroupHandlerTool::connectTCFunctions()
	{
		connect(&fToS,SIGNAL(merge(QSemaphore*, QList<ItemHandle*>&)),
			this,SLOT(merge(QSemaphore*, QList<ItemHandle*>&)));

		connect(&fToS,SIGNAL(separate(QSemaphore*,ItemHandle*)),
			this,SLOT(separate(QSemaphore*,ItemHandle*)));
	}

	typedef void (*tc_GroupHandlerTool_api)(
		void (*merge)(Array),
		void (*separate)(OBJ)
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
				if (handles[i])
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
			if (handle)
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
			if ((qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]) && getHandle(selected[i])) 
				|| qgraphicsitem_cast<TextGraphicsItem*>(selected[i]))
				newItems += cloneGraphicsItem(selected[i]);

		scene->insert(tr("alias inserted"),newItems);
		scene->move(newItems,QPointF(100,100));		
	}

	void GroupHandlerTool::nodeCollided(const QList<QGraphicsItem*>& list, NodeGraphicsItem * item, QPointF , Qt::KeyboardModifiers )
	{
		if (mainWindow == 0 || mainWindow->currentScene() == 0 || item == 0 || list.isEmpty()) return;
		GraphicsScene * scene = mainWindow->currentScene();

		QList<QGraphicsItem*> items;
		QList<ItemHandle*> handles;

		ItemHandle * handle = getHandle(item);
		ItemFamily * family = 0;
		NodeGraphicsItem * node = item;
		bool hasConnections = false;

		if (node && node->itemHandle && !node->connections().isEmpty())
		{
			hasConnections = true;
		}
		else
			for (int i=0; i < list.size(); ++i)
			{
				node = NodeGraphicsItem::topLevelNodeItem(list[i]);
				if (node && node->itemHandle && !node->connections().isEmpty())
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
					if (qgraphicsitem_cast<NodeGraphicsItem*>(list.at(i)) && handle && handle->family() && !handles.contains(handle))
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
						if (!handle && qgraphicsitem_cast<TextGraphicsItem*>(list[i]))
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
					if (qgraphicsitem_cast<NodeGraphicsItem*>(items[i]))
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
						if (scene->historyStack)
							scene->historyStack->push(command);
						else
						{
							command->redo();
							delete command;
						}
					}
			}

			if (handles.size() > 1 && bestHandle)
			{
				MergeHandlesCommand * mergeCommand = new MergeHandlesCommand(tr("items merged"),scene->networkWindow,handles);
				
				scene->deselect();

				/*QString newName = mergeCommand->newHandle->fullName();
				QList<QString> oldNames,newNames;

				QList<QGraphicsItem*> textToDelete;
				TextGraphicsItem * nameText = 0;

				for (int i=0; i < handles.size(); ++i)
				{
					if (handles[i])
						for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
							if ((nameText = qgraphicsitem_cast<TextGraphicsItem*>(handles[i]->graphicsItems[j]))
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
				commands += new RenameCommand(tr("name changed"),scene->networkWindow,handles,newNames);
				QUndoCommand * command = new CompositeCommand(tr("items merged"),commands);
				*/
				QList<ItemHandle*> oldHandles;
				for (int i=0; i < items.size(); ++i)
					oldHandles += getHandle(items[i]);

				if (scene->historyStack)
					scene->historyStack->push(mergeCommand);
				else
				{
					mergeCommand->redo();
					delete mergeCommand;
				}
				emit handlesChanged(scene,items,oldHandles);
			}
			else
				if (handles.size() == 1 && handles[0] && !items.isEmpty())
				{
					QUndoCommand * command = new AssignHandleCommand(tr("items merged"),items,handles[0]);

					QList<ItemHandle*> oldHandles;
					for (int i=0; i < items.size(); ++i)
						oldHandles += getHandle(items[i]);

					if (scene->historyStack)
						scene->historyStack->push(command);
					else
					{
						command->redo();
						delete command;
					}

					emit handlesChanged(scene,items,oldHandles);
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

	GroupHandlerTool_FToS  GroupHandlerTool::fToS;

	void GroupHandlerTool::_merge( Array A)
	{
		return fToS.merge(A);
	}

	void GroupHandlerTool::_separate(OBJ o)
	{
		return fToS.separate(o);
	}

	void GroupHandlerTool_FToS::merge(Array a0)
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

	void GroupHandlerTool_FToS::separate(OBJ a0)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit separate(s,ConvertValue(a0));
		s->acquire();
		s->release();
		delete s;
	}


}
