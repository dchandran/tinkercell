/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool allows insertion of nodes from the NodesTree

****************************************************************************/

#include <QtDebug>
#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NetworkWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "NodeInsertion.h"

namespace Tinkercell
{

	NodeInsertion::NodeInsertion(NodesTree * tree) : Tool(tr("Node Insertion")), selectedNodeFamily(0)
	{
		mainWindow = 0;
		nodesTree = tree;
		selectedNodeFamily = 0;
		connectTCFunctions();
	}

	bool NodeInsertion::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSignal(const QWidget*)));

			//connect(mainWindow,SIGNAL(sceneRightClick(GraphicsScene *, QGraphicsItem*, QPointF, Qt::KeyboardModifiers)),
			//	this, SLOT(sceneRightClick(GraphicsScene *, QGraphicsItem*, QPointF, Qt::KeyboardModifiers)));

			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			connectToNodesTree();

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			return nodesTree != 0;
		}
		return false;
	}

	void NodeInsertion::nodeSelected(NodeFamily * nodeFamily)
	{
		if ((selectedNodeFamily ||
			   (mainWindow && mainWindow->currentScene() && mainWindow->currentScene()->useDefaultBehavior))
			&& nodeFamily && nodesTree)
		{
			selectedNodeFamily = nodeFamily;

			while (nodeFamily != 0 && nodeFamily->pixmap.isNull())
				nodeFamily = static_cast<NodeFamily*>(nodeFamily->parent());

			if (nodeFamily != 0 && !nodeFamily->pixmap.isNull())
			{
				qreal asp = (double)nodeFamily->pixmap.height()/(double)nodeFamily->pixmap.width();
				
				QList<NetworkWindow*> allWindows = mainWindow->allWindows();
				for (int i=0; i < allWindows.size(); ++i)
					if (allWindows[i]->scene)
						allWindows[i]->setCursor(QCursor(nodeFamily->pixmap.scaled(30,(int)(30*asp))));
				
			}
			else
			{
				QList<NetworkWindow*> allWindows = mainWindow->allWindows();
				for (int i=0; i < allWindows.size(); ++i)
					if (allWindows[i]->scene)
						allWindows[i]->setCursor(Qt::ArrowCursor);
			}

			if (mainWindow->currentScene())
				mainWindow->currentScene()->useDefaultBehavior = false;
		}
	}

	void NodeInsertion::connectTCFunctions( )
	{
		connect(&fToS,SIGNAL(insertItem(QSemaphore*,ItemHandle**,QString,QString)),this,SLOT(insertItem(QSemaphore*,ItemHandle**,QString,QString)));
	}

	typedef void (*tc_NodeInsertion_api)(OBJ (*insertItem)(const char* , const char* ));

	void NodeInsertion::setupFunctionPointers( QLibrary * library )
	{
		tc_NodeInsertion_api f = (tc_NodeInsertion_api)library->resolve("tc_NodeInsertion_api");
		if (f)
		{
			f(
				&(_insertItem) );
		}
	}

	void NodeInsertion::insertItem(QSemaphore * sem, ItemHandle** item, QString name, QString family)
	{
		if (mainWindow && mainWindow->currentScene() && !name.isEmpty() && !family.isEmpty() && nodesTree
			&& nodesTree->nodeFamilies.contains(family))
		{
			NodeFamily * selectedFamily = nodesTree->nodeFamilies[family];
			GraphicsScene * scene = mainWindow->currentScene();
			if (item)
				(*item) = 0;
			if (selectedFamily && scene)
			{
				QPointF point = scene->lastPoint();

				//QList<ItemFamily*> subfamilies = selectedFamily->parents();
				QList<NodeFamily*> allFamilies;
				/*if (!subfamilies.isEmpty() && selectedFamily->graphicsItems.isEmpty())
				{
				for (int i=0; i < subfamilies.size(); ++i)
				if (subfamilies[i] && subfamilies[i]->isA("node"))
				allFamilies += static_cast<NodeFamily*>(subfamilies[i]);
				}
				if (allFamilies.isEmpty())*/
				allFamilies += selectedFamily;

				QString text;
				QList<QGraphicsItem*> list;
				qreal xpos = point.x();
				qreal height = 0.0;
				qreal width = 0.0;

				for (int j=0; j < allFamilies.size(); ++j)
				{
					NodeFamily * nodeFamily = allFamilies[j];

					for (int i=0; i < nodeFamily->graphicsItems.size(); ++i)
						if (NodeGraphicsItem::cast(nodeFamily->graphicsItems[i]))
							width += NodeGraphicsItem::cast(nodeFamily->graphicsItems[i])->defaultSize.width();
				}

				xpos -= width/2.0;
				bool alternate = false;

				for (int j=0; j < allFamilies.size(); ++j)
				{
					NodeFamily * nodeFamily = allFamilies[j];

					NodeHandle * handle = new NodeHandle(nodeFamily);

					if (allFamilies.size() == 1)
						handle->name = name;
					else
					{
						handle->name = nodeFamily->name.toLower();
						if (handle->name.length() > 4)
							handle->name.chop( handle->name.length() - 1 );
						handle->name = name + tr("_") + handle->name;
					}
					handle->name = findUniqueName(handle,scene->allHandles(),false);
					if (item)
						(*item) = handle;

					text += handle->name + tr(" ");

					for (int i=0; i < nodeFamily->graphicsItems.size(); ++i)
					{
						NodeGraphicsItem * image = (NodeGraphicsItem::topLevelNodeItem(nodeFamily->graphicsItems[i]));
						if (image)
						{
							image = image->clone();

							if (image->defaultSize.width() > 0 && image->defaultSize.height() > 0)
								image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),image->defaultSize.height()/image->sceneBoundingRect().height());

							qreal w = image->sceneBoundingRect().width();

							image->setPos(xpos + w/2.0, point.y());

							image->setBoundingBoxVisible(false);

							if (image->isValid())
							{
								xpos += w;
								setHandle(image,handle);
								list += image;
							}
							if (image->sceneBoundingRect().height() > height)
								height = image->sceneBoundingRect().height();
						}
					}

					if (nodeFamily->graphicsItems.size() > 0)
					{
						if (handle->family() && !handle->family()->isA("Empty"))
						{
							TextGraphicsItem * nameItem = new TextGraphicsItem(handle,0);
							QFont font = nameItem->font();
							font.setPointSize(22);
							nameItem->setFont(font);
							if (alternate)
								nameItem->setPos(xpos - nameItem->boundingRect().width(), point.y() - height/2.0 - 40.0);
							else
								nameItem->setPos(xpos - nameItem->boundingRect().width(), point.y() + height/2.0 + 5.0);
							list += nameItem;
							alternate = !alternate;
						}
					}
				}

				if (!list.isEmpty())
				{
					scene->insert(text + tr("inserted"),list);
				}
			}
		}
		if (sem)
			sem->release();
	}


	void NodeInsertion::toolLoaded(Tool*)
	{
		connectToNodesTree();
	}

	void NodeInsertion::sceneClicked(GraphicsScene *scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers)
	{
		if (mainWindow && scene && selectedNodeFamily)
		{
			if (button == Qt::LeftButton)
			{
				//QList<ItemFamily*> subfamilies = selectedNodeFamily->subFamilies();
				QList<NodeFamily*> allFamilies;
				/*if (!subfamilies.isEmpty() && selectedNodeFamily->graphicsItems.isEmpty())
				{
				for (int i=0; i < subfamilies.size(); ++i)
				if (subfamilies[i] && subfamilies[i]->isA("node"))
				allFamilies += static_cast<NodeFamily*>(subfamilies[i]);
				}
				if (allFamilies.isEmpty())*/
				allFamilies += selectedNodeFamily;

				QString text;
				QList<QGraphicsItem*> list;
				qreal xpos = point.x();
				qreal height = 0.0;
				qreal width = 0.0;

				for (int j=0; j < allFamilies.size(); ++j)
				{
					NodeFamily * nodeFamily = allFamilies[j];

					for (int i=0; i < nodeFamily->graphicsItems.size(); ++i)
						if (NodeGraphicsItem::cast(nodeFamily->graphicsItems[i]))
							width += NodeGraphicsItem::cast(nodeFamily->graphicsItems[i])->defaultSize.width();
				}

				xpos -= width/2.0;
				bool alternate = false;

				for (int j=0; j < allFamilies.size(); ++j)
				{
					NodeFamily * nodeFamily = allFamilies[j];

					NodeHandle * handle = new NodeHandle(nodeFamily);
					handle->name = nodeFamily->name.toLower();
					if (handle->name.length() > 4)
						handle->name.chop( handle->name.length() - 1 );
					handle->name = findUniqueName(handle,scene->allHandles());

					text += handle->name + tr(" ");

					for (int i=0; i < nodeFamily->graphicsItems.size(); ++i)
					{
						NodeGraphicsItem * image = (NodeGraphicsItem::topLevelNodeItem(nodeFamily->graphicsItems[i]));
						if (image)
						{
							image = image->clone();
							if (image->defaultSize.width() > 0 && image->defaultSize.height() > 0)
								image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),image->defaultSize.height()/image->sceneBoundingRect().height());

							qreal w = image->sceneBoundingRect().width();

							image->setPos(xpos + w/2.0, point.y());

							image->setBoundingBoxVisible(false);

							if (image->isValid())
							{
								xpos += w;
								setHandle(image,handle);
								list += image;
							}
							if (image->sceneBoundingRect().height() > height)
								height = image->sceneBoundingRect().height();
						}
					}

					if (nodeFamily->graphicsItems.size() > 0)
					{
						if (handle->family() && !handle->family()->isA("Empty"))
						{
							TextGraphicsItem * nameItem = new TextGraphicsItem(handle,0);
							QFont font = nameItem->font();
							font.setPointSize(22);
							nameItem->setFont(font);
							if (alternate)
								nameItem->setPos(xpos - nameItem->boundingRect().width(), point.y() - height/2.0 - 40.0);
							else
								nameItem->setPos(xpos - nameItem->boundingRect().width(), point.y() + height/2.0 + 5.0);
							list += nameItem;
							alternate = !alternate;
						}
					}
				}

				if (!list.isEmpty())
				{
					scene->insert(text + tr("inserted"),list);
				}
			}
			else
			{
				clear();
			}
		}
	}

	void NodeInsertion::connectToNodesTree()
	{
		if (nodesTree || !mainWindow) return;

		if (mainWindow->tool(tr("Nodes Tree")))
		{
			QWidget * treeWidget = mainWindow->tool(tr("Nodes Tree"));
			nodesTree = static_cast<NodesTree*>(treeWidget);
			if (nodesTree != 0)
			{
				connect(nodesTree,SIGNAL(nodeSelected(NodeFamily*)),this,SLOT(nodeSelected(NodeFamily*)));
				connect(mainWindow,SIGNAL(mousePressed(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),this,SLOT(sceneClicked(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));
			}
		}
	}

	void NodeInsertion::clear(bool setArrows)
	{
		selectedNodeFamily = 0;
		nodesTree->setCursor(Qt::ArrowCursor);
		if (setArrows)
		{
			QList<NetworkWindow*> allWindows = mainWindow->allWindows();
				for (int i=0; i < allWindows.size(); ++i)
					if (allWindows[i]->scene)
						allWindows[i]->setCursor(Qt::ArrowCursor);

			if (mainWindow->currentScene())
				mainWindow->currentScene()->useDefaultBehavior = true;
		}
	}

	void NodeInsertion::sceneRightClick(GraphicsScene *, QGraphicsItem*, QPointF, Qt::KeyboardModifiers)
	{
		if (selectedNodeFamily)
			clear();
	}

	void NodeInsertion::escapeSignal(const QWidget * )
	{
		if (selectedNodeFamily)
			clear();
	}

	/**************************************************/

	NodeInsertion_FToS NodeInsertion::fToS;

	OBJ NodeInsertion::_insertItem(const char* a, const char* b)
	{
		return fToS.insertItem(a,b);
	}

	OBJ NodeInsertion_FToS::insertItem(const char* a0, const char* a1)
	{
		QSemaphore * s = new QSemaphore(1);
		ItemHandle * item = 0;
		s->acquire();
		emit insertItem(s,&item,ConvertValue(a0),ConvertValue(a1));
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(item);
	}

}

