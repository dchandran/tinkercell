/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool allows insertion of nodes from the NodesTree

****************************************************************************/

#include <QtDebug>
#include "GraphicsScene.h"
#include "ConsoleWindow.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NetworkHandle.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "NodeInsertion.h"

namespace Tinkercell
{

	NodeInsertion::NodeInsertion(NodesTree * tree) : Tool(tr("Node Insertion"),tr("Basic GUI")), selectedNodeFamily(0)
	{
		mainWindow = 0;
		nodesTree = tree;
		selectedNodeFamily = 0;
	}

	bool NodeInsertion::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSignal(const QWidget*)));

			connect(mainWindow,SIGNAL(itemsDropped(GraphicsScene *, const QString&, QPointF)),
				this, SLOT(itemsDropped(GraphicsScene *, const QString&, QPointF)));

			connectToNodesTree();

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));

			return nodesTree != 0;
		}
		return false;
	}

	void NodeInsertion::nodeSelected(NodeFamily * nodeFamily)
	{
		if ((selectedNodeFamily ||
			   (mainWindow && currentScene() && currentScene()->useDefaultBehavior()))
			&& nodeFamily && nodesTree)
		{
			selectedNodeFamily = nodeFamily;

			while (nodeFamily != 0 && nodeFamily->pixmap.isNull())
				nodeFamily = static_cast<NodeFamily*>(nodeFamily->parent());

			if (mainWindow->currentScene())
				mainWindow->currentScene()->useDefaultBehavior(false);
		}
	}

	QList<QGraphicsItem*> NodeInsertion::createNewNode(GraphicsScene * scene, const QPointF& point, const QString& s, NodeFamily * family, const QStringList & usedNames)
	{
		QList<QGraphicsItem*> list;
		if (family && scene)
		{
			QString name = s;
			if (s.isNull() || s.isEmpty())
			{
				name = family->name().toLower() + tr("1");
				QStringList words = name.split(tr(" "));
				if (words.size() > 1)
				{
					name = words.first().left(1) + words.last().left(1);
					name += tr("1");
				}
				
				if (name.length() > 3)
					name = name.left( 3 ) + tr("1");
				
				if (!name[0].isLetter())
						name = tr("S") + name;
			}

			QList<NodeFamily*> allFamilies;		
			allFamilies += family;
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
				handle->name = RemoveDisallowedCharactersFromName(name);
				if (!handle->name[0].isLetter())
						handle->name = tr("S") + handle->name;
				handle->name = scene->network->makeUnique(handle->name,usedNames);
				
				NodeGraphicsItem * image = 0;

				for (int i=0; i < nodeFamily->graphicsItems.size(); ++i)
				{
					image = (NodeGraphicsItem::cast(nodeFamily->graphicsItems[i]));
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
						if (image)
							nameItem->relativePosition = QPair<QGraphicsItem*,QPointF>(image,QPointF(0,0));
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
		}
		return list;
	}

	void NodeInsertion::toolLoaded(Tool*)
	{
		connectToNodesTree();
	}
	
	void NodeInsertion::itemsDropped(GraphicsScene * scene, const QString& family, QPointF point)
	{
		if (mainWindow && scene->useDefaultBehavior() && !selectedNodeFamily && !family.isEmpty() && 
			nodesTree && nodesTree->getFamily(family))
		{
			selectedNodeFamily = nodesTree->getFamily(family);
			sceneClicked(scene,point,Qt::LeftButton,Qt::NoModifier);
			selectedNodeFamily = 0;
		}
	}

	void NodeInsertion::sceneClicked(GraphicsScene *scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers)
	{
		if (mainWindow && scene && selectedNodeFamily)
		{
			if (button == Qt::LeftButton)
			{
				QList<QGraphicsItem*> list = createNewNode(scene, point, tr(""), selectedNodeFamily);
				if (!list.isEmpty())
				{
					QString text;
					QList<ItemHandle*> handles = getHandle(list);
					for (int i=0; i < handles.size(); ++i)
						if (handles[i] && handles.indexOf(handles[i]) == i)
							text += handles[i]->name + " ";
					scene->insert(text + tr("inserted"),list);
					mainWindow->sendEscapeSignal(this);
				}
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
		if (setArrows)
		{
			if (mainWindow->currentScene())
				mainWindow->currentScene()->useDefaultBehavior(true);
		}
	}

	void NodeInsertion::sceneRightClick(GraphicsScene * scene, QGraphicsItem*, QPointF, Qt::KeyboardModifiers)
	{
		if (scene && scene->useDefaultBehavior()) return;
		mainWindow->sendEscapeSignal(this);
	}

	void NodeInsertion::escapeSignal(const QWidget * )
	{
		if (selectedNodeFamily)
			clear();
	}

}

