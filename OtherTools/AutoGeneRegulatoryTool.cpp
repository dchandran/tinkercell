/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 Automatically manage gene regulatory network rates and parameters

****************************************************************************/

#include "ItemHandle.h"
#include "CThread.h"
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "OutputWindow.h"
#include "CollisionDetection.h"
#include "BasicGraphicsToolbox.h"
#include "ConnectionSelection.h"
#include "NodeInsertion.h"
#include "NodesTree.h"
#include "ConnectionsTree.h"
#include "AutoGeneRegulatoryTool.h"

namespace Tinkercell
{
	AutoGeneRegulatoryTool::AutoGeneRegulatoryTool() :
		Tool(tr("Auto Gene Regulatory Tool")),
		autoTFUp("Insert activator",this),
		autoTFDown("Insert repressor",this),
		autoDegradation("Insert degadation reaction",this),
		autoGeneProduct("Insert gene product",this),
		mRNAstep("Add mRNA stage",this),
		autoPhosphate("Insert phosphate",this)
	{
		separator = 0;
		doAssignment = true;
		setPalette(QPalette(QColor(255,255,255,255)));
		setAutoFillBackground(true);
		connect(&autoTFUp,SIGNAL(triggered()),this,SLOT(autoTFTriggeredUp()));
		connect(&autoTFDown,SIGNAL(triggered()),this,SLOT(autoTFTriggeredDown()));
		connect(&autoDegradation,SIGNAL(triggered()),this,SLOT(autoDegradationTriggered()));
		autoDegradation.setIcon(QIcon(":/images/empty.png"));
		connect(&autoGeneProduct,SIGNAL(triggered()),this,SLOT(autoGeneProductTriggered()));
		connect(&autoPhosphate,SIGNAL(triggered()),this,SLOT(autoPhosphateTriggered()));
		autoPhosphate.setIcon(QIcon(":/images/phosphate.png"));
		connect(&mRNAstep,SIGNAL(triggered()),this,SLOT(insertmRNAstep()));
		mRNAstep.setIcon(QIcon(":/images/upArrow.png"));
		//connect(&glowTimer,SIGNAL(frameChanged(int)),this,SLOT(makeNodeGlow(int)));
		//connect(&glowTimer,SIGNAL(stateChanged(QTimeLine::State)),this,SLOT(stopGlow(QTimeLine::State)));
	}

	void AutoGeneRegulatoryTool::autoPhosphateTriggered()
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !mainWindow) return;
// 		if (!mainWindow->tools.contains(tr("Nodes Tree")) || !mainWindow->tools.contains(tr("Connections Tree"))) return;

// 		QWidget * treeWidget = mainWindow->tools.value(tr("Nodes Tree"));
// 		NodesTree * nodesTree = static_cast<NodesTree*>(treeWidget);
//
// 		if (!nodesTree->nodeFamilies.contains("Phosphate")) return;
//
// 		NodeFamily * nodeFamily = nodesTree->nodeFamilies["Phosphate"];
//
// 		QList<QGraphicsItem*>& selected = scene->selected();
// 		ItemHandle * handle = 0;
//
// 		QList<QGraphicsItem*> sceneItems = scene->items();
// 		QList<QGraphicsItem*> list;
//
// 		QString appDir = QCoreApplication::applicationDirPath();
//   		#ifdef Q_WS_MAC
// 		appDir += tr("/../../..");
// 		#endif
//
// 		QList<ItemHandle*> handlesPhos;
//
// 		for (int i=0; i < selected.size(); ++i)
// 		{
// 			handle = getHandle(selected[i]);
// 			if (qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]) && handle && handle->isA("Protein") && !handlesPhos.contains(handle))
// 			{
// 				handlesPhos += handle;
// 				ItemHandle * node = handle;
//
// 				QRectF boundingRect = selected[i]->sceneBoundingRect();
// 				for (int j=0; j < handle->graphicsItems.size(); ++j)
// 				{
// 					if (handle->graphicsItems[j]->sceneBoundingRect().intersects(boundingRect))
// 						boundingRect.unite(handle->graphicsItems[j]->sceneBoundingRect());
// 				}
//
// 				qreal xpos = (selected[i]->sceneBoundingRect().right()),
// 					  ypos = (selected[i]->sceneBoundingRect().center().y()),
// 					  height = 0.0;
//
// 				NodeGraphicsItem * image = 0;
//
// 				for (int j=0; j < nodeFamily->graphicsItems.size(); ++j)
// 				{
// 					image = (NodeGraphicsItem::topLevelNodeItem(nodeFamily->graphicsItems[j]));
// 					if (image)
// 					{
// 						image = image->clone();
//
// 						if (image->defaultSize.width() > 0 && image->defaultSize.height() > 0)
// 							image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),image->defaultSize.height()/image->sceneBoundingRect().height());
//
// 						qreal w = image->sceneBoundingRect().width();
//
// 						image->setPos(xpos, ypos);
//
// 						image->setBoundingBoxVisible(false);
//
// 						if (image->isValid())
// 						{
// 							xpos += w;
// 							setHandle(image,node);
// 							list += image;
// 						}
// 						if (image->sceneBoundingRect().height() > height)
// 							height = image->sceneBoundingRect().height();
// 					}
// 				}
// 			}
// 		}
//
// 		if (!list.isEmpty())
// 		{
// 			scene->insert(tr("phosphates added"),list);
// 		}

	}

	void AutoGeneRegulatoryTool::autoTFTriggeredUp()
	{
		autoTFTriggered(tr("Transcription Activation"));
	}

	void AutoGeneRegulatoryTool::autoTFTriggeredDown()
	{
		autoTFTriggered(tr("Transcription Repression"));
	}

	void AutoGeneRegulatoryTool::autoTFTriggered(const QString& regulationName)
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !mainWindow) return;
// 		if (!mainWindow->tools.contains(tr("Nodes Tree")) || !mainWindow->tools.contains(tr("Connections Tree"))) return;
//
// 		QWidget * treeWidget = mainWindow->tools.value(tr("Nodes Tree"));
// 		NodesTree * nodesTree = static_cast<NodesTree*>(treeWidget);
//
// 		if (!nodesTree->nodeFamilies.contains("Transcription Factor")) return;
//
// 		treeWidget = mainWindow->tools.value(tr("Connections Tree"));
// 		ConnectionsTree * connectionsTree = static_cast<ConnectionsTree*>(treeWidget);
//
// 		if (!connectionsTree->connectionFamilies.contains(regulationName)) return;
//
// 		NodeFamily * nodeFamily = nodesTree->nodeFamilies["Transcription Factor"];
// 		ConnectionFamily * connectionFamily = connectionsTree->connectionFamilies[regulationName];
//
// 		QList<QGraphicsItem*>& selected = scene->selected();
// 		ItemHandle * handle = 0;
//
//                 QList<ItemHandle*> sceneItems = scene->allHandles();
// 		QList<QGraphicsItem*> list;
//
// 		QString appDir = QCoreApplication::applicationDirPath();
//   		#ifdef Q_WS_MAC
// 		appDir += tr("/../../..");
// 		#endif
//
// 		QList<ItemHandle*> handlesDegraded;
//
// 		QList<NodeGraphicsItem*> regulatorNodes;
// 		QList<ItemHandle*> regulatorHandles;
// 		QList< DataTable<QString>* > newTextData;
//
// 		for (int i=0; i < selected.size(); ++i)
// 		{
// 			handle = getHandle(selected[i]);
// 			if (qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]) && handle && (handle->isA("Regulatory") || handle->isA("Gene")) && !handlesDegraded.contains(handle))
// 			{
// 				regulatorNodes += qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]);
// 				handlesDegraded += handle;
// 				NodeHandle * node = new NodeHandle(nodeFamily);
// 				node->name = tr("TF");
// 				node->name = NodeInsertion::findUniqueName(node,sceneItems);
//
// 				qreal xpos = selected[i]->sceneBoundingRect().left() - 100.0,
// 					  ypos = selected[i]->sceneBoundingRect().top() - 100.0,
// 					  height = 0.0;
//
// 				for (int j=0; j < 100; ++j)
// 				{
// 					if (scene->items(QPointF(xpos,ypos)).isEmpty()) break;
// 					ypos -= (selected[i]->sceneBoundingRect().height() * 1.5);
// 				}
// 				NodeGraphicsItem * image = 0;
//
// 				for (int j=0; j < nodeFamily->graphicsItems.size(); ++j)
// 				{
// 					image = (NodeGraphicsItem::topLevelNodeItem(nodeFamily->graphicsItems[j]));
// 					if (image)
// 					{
// 						image = image->clone();
//
// 						if (image->defaultSize.width() > 0 && image->defaultSize.height() > 0)
// 							image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),image->defaultSize.height()/image->sceneBoundingRect().height());
//
// 						qreal w = image->sceneBoundingRect().width();
//
// 						image->setPos(xpos, ypos);
//
// 						image->setBoundingBoxVisible(false);
//
// 						if (image->isValid())
// 						{
// 							xpos += w;
// 							setHandle(image,node);
// 							list += image;
// 						}
// 						if (image->sceneBoundingRect().height() > height)
// 							height = image->sceneBoundingRect().height();
// 					}
// 				}
// 				if (image)
// 				{
// 					TextGraphicsItem * nameItem = new TextGraphicsItem(node,0);
// 					QFont font = nameItem->font();
// 					font.setPointSize(22);
// 					nameItem->setFont(font);
// 					nameItem->setPos(xpos - 2 * nameItem->boundingRect().width(), ypos + height/2.0);
// 					list += nameItem;
//
// 					ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
// 					ConnectionHandle * connection = new ConnectionHandle(connectionFamily,item);
//
// 					item->pathVectors +=
// 						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(item,image));
//
// 					item->pathVectors +=
// 						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(item,selected[i]));
//
// 					ArrowHeadItem * arrow = 0;
// 					QString nodeImageFile;
// 					if (regulationName == tr("Transcription Activation"))
// 						nodeImageFile = appDir + tr("/ArrowItems/Transcription_Activation.xml");
// 					else
// 						nodeImageFile = appDir + tr("/ArrowItems/Transcription_Repression.xml");
// 					NodeGraphicsReader imageReader;
// 					arrow = new ArrowHeadItem(item);
// 					imageReader.readXml(arrow,nodeImageFile);
// 					arrow->normalize();
// 					arrow->scale(0.1,0.1);
// 					item->pathVectors.last().arrowStart = arrow;
// 					list += arrow;
//
//
// 					connection->name = tr("J");
// 					item->lineType = ConnectionGraphicsItem::line;
// 					connection->name = NodeInsertion::findUniqueName(connection,sceneItems);
//                                         sceneItems << node << connection;
//
// 					nameItem = new TextGraphicsItem(connection,0);
// 					list += nameItem;
// 					nameItem->setPos( 0.5*(image->pos() + selected[i]->scenePos() ) );
// 					font = nameItem->font();
// 					font.setPointSize(22);
// 					nameItem->setFont(font);
//
// 					list += item;
//
// 					if (handle->hasTextData("Assignments"))
// 					{
// 						regulatorHandles << handle;
// 					}
// 				}
// 			}
// 		}
//
// 		if (!list.isEmpty())
// 		{
// 			scene->insert(tr("TF's inserted"),list);
// 		}

		/*
		for (int i=0; i < regulatorNodes.size(); ++i)
		{
			handle = regulatorNodes[i]->itemHandle;
			if (handle->hasTextData("Assignments"))
			{
				DataTable<QString>* newData = new DataTable<QString>(handle->data->textData[tr("Assignments")]);
				QString s = hillEquation(regulatorNodes[i]);
				if (!s.isEmpty())
				{
					newData->value(tr("rate"),0) = s;
					regulatorHandles << handle;
					newTextData << newData;
				}
			}
		}

		if (!regulatorHandles.isEmpty())
			scene->changeData(regulatorHandles,tr("Assignments"),newTextData);

		for (int i=0; i < newTextData.size(); ++i)
			delete newTextData[i];*/
	}

	void AutoGeneRegulatoryTool::autoGeneProductTriggered()
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !mainWindow) return;
// 		if (!mainWindow->tools.contains(tr("Nodes Tree")) || !mainWindow->tools.contains(tr("Connections Tree"))) return;
//
// 		QWidget * treeWidget = mainWindow->tools.value(tr("Nodes Tree"));
// 		NodesTree * nodesTree = static_cast<NodesTree*>(treeWidget);
//
// 		if (!nodesTree->nodeFamilies.contains("Protein")) return;
//
// 		treeWidget = mainWindow->tools.value(tr("Connections Tree"));
// 		ConnectionsTree * connectionsTree = static_cast<ConnectionsTree*>(treeWidget);
//
// 		if (!connectionsTree->connectionFamilies.contains("Transcription")) return;
//
// 		if (!nodesTree->nodeFamilies.contains("Empty")) return;
//
// 		if (!connectionsTree->connectionFamilies.contains("Biochemical")) return;
//
// 		NodeFamily * nodeFamily2 = nodesTree->nodeFamilies["Empty"];
// 		ConnectionFamily * connectionFamily2 = connectionsTree->connectionFamilies["Biochemical"];
//
// 		NodeFamily * nodeFamily = nodesTree->nodeFamilies["Protein"];
// 		ConnectionFamily * connectionFamily = connectionsTree->connectionFamilies["Transcription"];
//
// 		QList<QGraphicsItem*>& selected = scene->selected();
// 		ItemHandle * handle = 0;
//
//                 QList<ItemHandle*> sceneItems = scene->allHandles();
// 		QList<QGraphicsItem*> list;
//
// 		QString appDir = QCoreApplication::applicationDirPath();
//   		#ifdef Q_WS_MAC
// 		appDir += tr("/../../..");
// 		#endif
//
// 		QList<ItemHandle*> handlesDegraded;
//
// 		for (int i=0; i < selected.size(); ++i)
// 		{
// 			handle = getHandle(selected[i]);
// 			if (qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]) && handle && (handle->isA("Gene") || handle->isA("Coding")) && !handlesDegraded.contains(handle))
// 			{
// 				handlesDegraded += handle;
// 				NodeHandle * node = new NodeHandle(nodeFamily);
// 				node->name = tr("P");
// 				node->name = NodeInsertion::findUniqueName(node,sceneItems);
//
// 				qreal xpos = selected[i]->scenePos().rx(),
// 					  ypos = selected[i]->scenePos().ry() - (selected[i]->sceneBoundingRect().height() * 3),
// 					  height = 0.0;
//
// 				for (int j=0; j < 100; ++j)
// 				{
// 					if (scene->items(QPointF(xpos,ypos)).isEmpty()) break;
// 					ypos -= (selected[i]->sceneBoundingRect().height() * 1.0);
// 				}
// 				NodeGraphicsItem * image = 0;
//
// 				for (int j=0; j < nodeFamily->graphicsItems.size(); ++j)
// 				{
// 					image = (NodeGraphicsItem::topLevelNodeItem(nodeFamily->graphicsItems[j]));
// 					if (image)
// 					{
// 						image = image->clone();
//
// 						if (image->defaultSize.width() > 0 && image->defaultSize.height() > 0)
// 							image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),image->defaultSize.height()/image->sceneBoundingRect().height());
//
// 						qreal w = image->sceneBoundingRect().width();
// 						ypos -= (image->sceneBoundingRect().height() * 1.0);
//
// 						image->setPos(xpos, ypos);
//
// 						image->setBoundingBoxVisible(false);
//
// 						if (image->isValid())
// 						{
// 							xpos += w;
// 							setHandle(image,node);
// 							list += image;
// 						}
// 						if (image->sceneBoundingRect().height() > height)
// 							height = image->sceneBoundingRect().height();
// 					}
// 				}
// 				if (image)
// 				{
// 					TextGraphicsItem * nameItem = new TextGraphicsItem(node,0);
// 					QFont font = nameItem->font();
// 					font.setPointSize(22);
// 					nameItem->setFont(font);
// 					nameItem->setPos(image->sceneBoundingRect().right(), ypos + height/2.0);
// 					list += nameItem;
//
// 					ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
// 					ConnectionHandle * connection = new ConnectionHandle(connectionFamily,item);
//
// 					item->pathVectors +=
// 						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(item,selected[i]));
//
// 					item->pathVectors +=
// 						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(item,image));
//
// 					ArrowHeadItem * arrow = 0;
// 					QString nodeImageFile = appDir + tr("/ArrowItems/Transcription.xml");
// 					NodeGraphicsReader imageReader;
// 					arrow = new ArrowHeadItem(item);
// 					imageReader.readXml(arrow,nodeImageFile);
// 					arrow->normalize();
// 					arrow->scale(0.1,0.1);
// 					item->pathVectors.last().arrowStart = arrow;
// 					list += arrow;
//
// 					connection->name = tr("J");
// 					item->lineType = ConnectionGraphicsItem::line;
// 					connection->name = NodeInsertion::findUniqueName(connection,sceneItems);
//                                         sceneItems << node << connection;
//
// 					nameItem = new TextGraphicsItem(connection,0);
// 					list += nameItem;
// 					nameItem->setPos(image->sceneBoundingRect().left() - nameItem->sceneBoundingRect().width(),image->sceneBoundingRect().bottom() + image->sceneBoundingRect().height());
// 					font = nameItem->font();
// 					font.setPointSize(22);
// 					nameItem->setFont(font);
//
// 					list += item;
// 				}
// 				else
// 				{
// 					continue;
// 				}
//
// 				node = new NodeHandle(nodeFamily2);
// 				node->name = tr("emp");
// 				node->name = NodeInsertion::findUniqueName(node,sceneItems);
//
// 				xpos = (image->sceneBoundingRect().right() + 100.0);
// 				ypos = (image->sceneBoundingRect().top() - 100.0);
// 				height = 0.0;
//
// 				NodeGraphicsItem * image2 = image;
//
// 				for (int j=0; j < nodeFamily2->graphicsItems.size(); ++j)
// 				{
// 					image = (NodeGraphicsItem::topLevelNodeItem(nodeFamily2->graphicsItems[j]));
// 					if (image)
// 					{
// 						image = image->clone();
//
// 						if (image->defaultSize.width() > 0 && image->defaultSize.height() > 0)
// 							image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),image->defaultSize.height()/image->sceneBoundingRect().height());
//
// 						qreal w = image->sceneBoundingRect().width();
//
// 						image->setPos(xpos, ypos);
//
// 						image->setBoundingBoxVisible(false);
//
// 						if (image->isValid())
// 						{
// 							xpos += w;
// 							setHandle(image,node);
// 							list += image;
// 						}
// 						if (image->sceneBoundingRect().height() > height)
// 							height = image->sceneBoundingRect().height();
// 					}
// 				}
// 				if (image)
// 				{
// 					TextGraphicsItem * nameItem;
// 					QFont font;
//
// 					ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
// 					ConnectionHandle * connection = new ConnectionHandle(connectionFamily2,item);
//
// 					item->pathVectors +=
// 						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(item,image2));
//
// 					item->pathVectors +=
// 						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(item,image));
//
// 					ArrowHeadItem * arrow = 0;
// 					QString nodeImageFile = appDir + tr("/ArrowItems/Reaction.xml");
// 					NodeGraphicsReader imageReader;
// 					arrow = new ArrowHeadItem(item);
// 					imageReader.readXml(arrow,nodeImageFile);
// 					arrow->normalize();
// 					arrow->scale(0.1,0.1);
// 					item->pathVectors.last().arrowStart = arrow;
// 					list += arrow;
//
//
// 					connection->name = tr("J");
// 					item->lineType = ConnectionGraphicsItem::line;
// 					connection->name = NodeInsertion::findUniqueName(connection,sceneItems);
//                                         sceneItems << node << connection;
//
// 					nameItem = new TextGraphicsItem(connection,0);
// 					list += nameItem;
// 					nameItem->setPos( 0.5*(image->pos() + image2->scenePos() ) );
// 					font = nameItem->font();
// 					font.setPointSize(22);
// 					nameItem->setFont(font);
//
// 					list += item;
// 				}
// 			}
// 		}
//
//
//
// 		if (!list.isEmpty())
// 		{
// 			scene->insert(tr("Transcription added"),list);
// 		}
	}

	void AutoGeneRegulatoryTool::autoDegradationTriggered()
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !mainWindow) return;
// 		if (!mainWindow->tools.contains(tr("Nodes Tree")) || !mainWindow->tools.contains(tr("Connections Tree"))) return;
//
// 		QWidget * treeWidget = mainWindow->tools.value(tr("Nodes Tree"));
// 		NodesTree * nodesTree = static_cast<NodesTree*>(treeWidget);
//
// 		if (!nodesTree->nodeFamilies.contains("Empty")) return;
//
// 		treeWidget = mainWindow->tools.value(tr("Connections Tree"));
// 		ConnectionsTree * connectionsTree = static_cast<ConnectionsTree*>(treeWidget);
//
// 		if (!connectionsTree->connectionFamilies.contains("Biochemical")) return;
//
// 		NodeFamily * nodeFamily = nodesTree->nodeFamilies["Empty"];
// 		ConnectionFamily * connectionFamily = connectionsTree->connectionFamilies["Biochemical"];
//
// 		QList<QGraphicsItem*>& selected = scene->selected();
// 		ItemHandle * handle = 0;
//
//                 QList<ItemHandle*> sceneItems = scene->allHandles();
// 		QList<QGraphicsItem*> list;
//
// 		QString appDir = QCoreApplication::applicationDirPath();
//   		#ifdef Q_WS_MAC
// 		appDir += tr("/../../..");
// 		#endif
//
// 		QList<ItemHandle*> handlesDegraded;
//
// 		for (int i=0; i < selected.size(); ++i)
// 		{
// 			handle = getHandle(selected[i]);
// 			if (qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]) && handle && handle->isA("Species") && !handlesDegraded.contains(handle))
// 			{
// 				handlesDegraded += handle;
// 				NodeHandle * node = new NodeHandle(nodeFamily);
// 				node->name = tr("emp");
// 				node->name = NodeInsertion::findUniqueName(node,sceneItems);
//
// 				qreal xpos = (selected[i]->sceneBoundingRect().right() + 100.0),
// 					  ypos = (selected[i]->sceneBoundingRect().top() - 100.0),
// 					  height = 0.0;
//
// 				NodeGraphicsItem * image = 0;
//
// 				for (int j=0; j < nodeFamily->graphicsItems.size(); ++j)
// 				{
// 					image = (NodeGraphicsItem::topLevelNodeItem(nodeFamily->graphicsItems[j]));
// 					if (image)
// 					{
// 						image = image->clone();
//
// 						if (image->defaultSize.width() > 0 && image->defaultSize.height() > 0)
// 							image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),image->defaultSize.height()/image->sceneBoundingRect().height());
//
// 						qreal w = image->sceneBoundingRect().width();
//
// 						image->setPos(xpos, ypos);
//
// 						image->setBoundingBoxVisible(false);
//
// 						if (image->isValid())
// 						{
// 							xpos += w;
// 							setHandle(image,node);
// 							list += image;
// 						}
// 						if (image->sceneBoundingRect().height() > height)
// 							height = image->sceneBoundingRect().height();
// 					}
// 				}
// 				if (image)
// 				{
// 					TextGraphicsItem * nameItem;
// 					QFont font;
//
// 					ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
// 					ConnectionHandle * connection = new ConnectionHandle(connectionFamily,item);
//
// 					item->pathVectors +=
// 						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(item,selected[i]));
//
// 					item->pathVectors +=
// 						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(item,image));
//
// 					ArrowHeadItem * arrow = 0;
// 					QString nodeImageFile = appDir + tr("/ArrowItems/Reaction.xml");
// 					NodeGraphicsReader imageReader;
// 					arrow = new ArrowHeadItem(item);
// 					imageReader.readXml(arrow,nodeImageFile);
// 					arrow->normalize();
// 					arrow->scale(0.1,0.1);
// 					item->pathVectors.last().arrowStart = arrow;
// 					list += arrow;
//
//
// 					connection->name = tr("J");
// 					item->lineType = ConnectionGraphicsItem::line;
// 					connection->name = NodeInsertion::findUniqueName(connection,sceneItems);
//                                         sceneItems << node << connection;
//
// 					nameItem = new TextGraphicsItem(connection,0);
// 					list += nameItem;
// 					nameItem->setPos( 0.5*(image->pos() + selected[i]->scenePos() ) );
// 					font = nameItem->font();
// 					font.setPointSize(22);
// 					nameItem->setFont(font);
//
// 					list += item;
// 				}
// 			}
// 		}
//
// 		if (!list.isEmpty())
// 		{
// 			scene->insert(tr("auto degradation"),list);
// 		}

	}

	void AutoGeneRegulatoryTool::insertmRNAstep()
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !mainWindow) return;

		QList<QGraphicsItem*>& selected = scene->selected();
		ItemHandle * handle = 0;

                //QList<ItemHandle*> sceneItems = scene->allHandles();
		QList<QGraphicsItem*> list;

		ConnectionGraphicsItem * connection;
		QList<ItemHandle*> handles;
		QList<QGraphicsItem*> genes;
		QList<DataTable<qreal>*> nDataNew, nDataOld;
		QList<DataTable<QString>*> sDataNew, sDataOld;

		for (int i=0; i < selected.size(); ++i)
		{
			handle = getHandle(selected[i]);
			if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(selected[i])) &&
				handle &&
				handle->isA(tr("Transcription")) && !handles.contains(handle) && handle->data &&
				handle->hasNumericalData(tr("Stoichiometry")) && handle->hasTextData(tr("Rates")))
			{
				DataTable<qreal> * stoichiometryMatrix = new DataTable<qreal>(handle->data->numericalData[tr("Stoichiometry")]);
				DataTable<QString> * rates = new DataTable<QString>(handle->data->textData[tr("Rates")]);

				if (stoichiometryMatrix->rowNames().contains(QString("transcription")) &&
					rates->rowNames().contains(QString("transcription")) &&
					stoichiometryMatrix->rowNames().contains(QString("translation")) &&
					rates->rowNames().contains(QString("translation")) &&
					stoichiometryMatrix->rowNames().contains(QString("degradation")) &&
					rates->rowNames().contains(QString("degradation")))
				{
					delete stoichiometryMatrix;
					delete rates;
					continue;
				}

				int cols = stoichiometryMatrix->cols();
				int rows = stoichiometryMatrix->rows();

				stoichiometryMatrix->resize(rows + 2, cols + 1);
				stoichiometryMatrix->colName(cols) = handle->fullName() + QString(".mRNA");

				rates->resize(rows + 2,rates->cols());
				if (rows > 0)
				{
					stoichiometryMatrix->rowName(rows-1) = QString("transcription");
					rates->rowName(rows-1) = QString("transcription");
				}

				stoichiometryMatrix->rowName(rows) = QString("translation");
				rates->rowName(rows) = QString("translation");

				stoichiometryMatrix->rowName(rows+1) = QString("degradation");
				rates->rowName(rows+1) = QString("degradation");

				if (handle->hasNumericalData(tr("Numerical Attributes")))
				{
					DataTable<qreal> * params = new DataTable<qreal>(handle->data->numericalData[tr("Numerical Attributes")]);
					if (!params->rowNames().contains(QString("k1")))
					{
						int sz = params->rows();
						params->value(sz,0) = 1.0;
						params->rowName(sz) = QString("k1");
					}
					if (!params->rowNames().contains(QString("degr")))
					{
						int sz = params->rows();
						params->value(sz,0) = 1.0;
						params->rowName(sz) = QString("degr");
					}

					handles += handle;

					nDataOld += &(handle->data->numericalData[tr("Numerical Attributes")]);
					nDataNew += params;

					sDataOld += 0;
					sDataNew += 0;
				}

				for (int i=0; i < cols+1; ++i)
				{
					stoichiometryMatrix->value(tr("degradation"),i) = 0.0; //mrna degr
					stoichiometryMatrix->value(tr("transcription"),i) = 0.0; // transcription
					stoichiometryMatrix->value(tr("translation"),i) = 1.0;
				}

				stoichiometryMatrix->value(tr("degradation"),cols) = -1.0; //mrna degr
				stoichiometryMatrix->value(tr("transcription"),cols) = 1.0; //transcription
				stoichiometryMatrix->value(tr("translation"),cols) = 0.0; //transcription

				rates->value(tr("degradation"),0) = handle->fullName() + QString(".degr * ") + handle->fullName() + QString(".mRNA");
				rates->value(tr("translation"),0) = handle->fullName() + QString(".k1 * ") + handle->fullName() + QString(".mRNA");

				handles += handle;

				nDataOld += &(handle->data->numericalData[tr("Stoichiometry")]);
				sDataOld += &(handle->data->textData[tr("Rates")]);

				nDataNew += stoichiometryMatrix;
				sDataNew += rates;

				QList<NodeGraphicsItem*> nodes = connection->nodes();
				if (nodes.size() > 0 && nodes[0])
					genes += nodes[0];
			}
			else
			{
				selected.removeAt(i);
				--i;
			}
		}

		scene->changeData(handles,nDataOld,nDataNew,sDataOld,sDataNew);
		itemsMoved(scene,genes,QList<QPointF>(),0);

		QString appDir = QCoreApplication::applicationDirPath();
		#ifdef Q_WS_MAC
		appDir += tr("/../../..");
		#endif
		QString filename = appDir + tr("/OtherItems/UpCircle.xml");
		emit setMiddleBox(1,filename);
	}

	bool AutoGeneRegulatoryTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow != 0)
		{
			connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)),
						  this, SLOT(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)));
			connect(mainWindow,SIGNAL(itemsAboutToBeRemoved(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)),
						  this, SLOT(itemsRemoved(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)));
			connect(mainWindow,SIGNAL(itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)),
						  this, SLOT(itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)));
			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
						this,SLOT(itemsSelected(GraphicsScene *,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
			connectPlugins();

			connect(mainWindow,SIGNAL(pluginLoaded(const QString&)),this,SLOT(pluginLoaded(const QString&)));
		}
		return true;
	}

	void AutoGeneRegulatoryTool::pluginLoaded(const QString&)
	{
		connectPlugins();
	}

	void AutoGeneRegulatoryTool::connectPlugins()
	{
		static bool alreadyConnected1 = false;
		static bool alreadyConnected2 = false;
		static bool alreadyConnected3 = false;

		if ((alreadyConnected1 && alreadyConnected2 && alreadyConnected3) || !mainWindow) return;

// 		if (!alreadyConnected1 && mainWindow->tools.contains(tr("Collision Detection")))
// 		{
// 			QWidget * widget = mainWindow->tools[tr("Collision Detection")];
// 			CollisionDetection * collisionDetection = static_cast<CollisionDetection*>(widget);
// 			if (collisionDetection)
// 			{
// 				alreadyConnected1 = true;
// 				connect(collisionDetection,SIGNAL(nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& , Qt::KeyboardModifiers )),
// 						this, SLOT( nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& , Qt::KeyboardModifiers )));
// 			}
// 		}

// 		if (!alreadyConnected2 && mainWindow->tools.contains(tr("Basic Graphics Toolbox")))
// 		{
// 			QWidget * widget = mainWindow->tools[tr("Basic Graphics Toolbox")];
// 			BasicGraphicsToolbox * basicToolBox = static_cast<BasicGraphicsToolbox*>(widget);
// 			if (basicToolBox)
// 			{
// 				alreadyConnected2 = true;
// 				connect(this,SIGNAL(alignCompactHorizontal()),basicToolBox, SLOT(alignCompactHorizontal()));
// 			}
// 		}
//
//
// 		if (!alreadyConnected3 && mainWindow->tools.contains(tr("Connection Selection")))
// 		{
// 			QWidget * widget = mainWindow->tools[tr("Connection Selection")];
// 			ConnectionSelection * connectionSelection = static_cast<ConnectionSelection*>(widget);
// 			if (connectionSelection)
// 			{
// 				alreadyConnected3 = true;
// 				connect(this,SIGNAL(setMiddleBox(int,const QString&)),connectionSelection, SLOT(showMiddleBox(int,const QString&)));
// 			}
// 		}
	}

	void AutoGeneRegulatoryTool::nodeCollided(const QList<QGraphicsItem*>& items, NodeGraphicsItem * , const QList<QPointF>&, Qt::KeyboardModifiers )
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		ItemHandle * handle = 0;

		QList<QGraphicsItem*> select;

		QGraphicsItem * dnaItem = 0;
		for (int i=0; i < items.size(); ++i)
			if (qgraphicsitem_cast<NodeGraphicsItem*>(items[i]) && (handle = getHandle(items[i])) && handle->isA("Part"))
			{
				dnaItem = items[i];
				select << dnaItem;
				break;
			}

		if (dnaItem)
		{
			QGraphicsItem * itemLeft = dnaItem, * itemRight = dnaItem;
			while (itemLeft)
			{
				QRectF p1(itemLeft->sceneBoundingRect());
				p1.adjust(-10.0,0,-10.0,0.0);
				QList<QGraphicsItem*> items = scene->items(p1);
				itemLeft = 0;
				for (int i=0; i < items.size(); ++i)
					if (!select.contains(items[i]) && qgraphicsitem_cast<NodeGraphicsItem*>(items[i]) && (handle = getHandle(items[i])) && handle->isA("Part"))
					{
						itemLeft = items[i];
						select << itemLeft;
						break;
					}
			}
			while (itemRight)
			{
				QRectF p2(itemRight->sceneBoundingRect());
				p2.adjust(10.0,0,10.0,0.0);
				QList<QGraphicsItem*> items = scene->items(p2);
				itemRight = 0;
				for (int i=0; i < items.size(); ++i)
					if (!select.contains(items[i]) && qgraphicsitem_cast<NodeGraphicsItem*>(items[i]) && (handle = getHandle(items[i])) && handle->isA("Part"))
					{
						itemRight = items[i];
						select << itemRight;
						break;
					}
			}

			scene->selected() = select;
			scene->select(0);

			doAssignment = false;
			emit alignCompactHorizontal();
		}
	}

	void AutoGeneRegulatoryTool::itemsInserted(GraphicsScene* scene, const QList<QGraphicsItem *>&, const QList<ItemHandle*>& handles)
	{
		if (!scene) return;

		QList<QGraphicsItem*> items;

		QList<ItemHandle*> regulatory;
		QStringList toolNames;
		QList< DataTable<QString>* > newTextData;

		for (int i=0; i < handles.size(); ++i)
			if (handles[i] && handles[i]->type == ConnectionHandle::Type)
			{
                                QList<NodeGraphicsItem*> nodes;
                                    for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
                                    {
                                        if (qgraphicsitem_cast<ConnectionGraphicsItem*>(handles[i]->graphicsItems[j]))
                                            nodes += (qgraphicsitem_cast<ConnectionGraphicsItem*>(handles[i]->graphicsItems[j]))->nodes();
                                    }
				if (handles[i]->isA("Transcription"))
                                {
					for (int j=0; j < nodes.size(); ++j)
					{
						if (nodes[j] && nodes[j]->itemHandle && nodes[j]->itemHandle->isA("Coding") && !handles.contains(nodes[j]->itemHandle))
						{
							if (nodes[j]->itemHandle->isA("Regulatory") && !handles.contains(nodes[j]->itemHandle))
							{
								if (nodes[j]->itemHandle->data && nodes[j]->itemHandle->hasTextData("Assignments")
									&& !nodes[j]->itemHandle->data->textData[tr("Assignments")].rowNames().contains(tr("rate")))
								{
									DataTable<QString>* newData = new DataTable<QString>(nodes[j]->itemHandle->data->textData[tr("Assignments")]);
                                                                        QString s = hillEquation(NodeHandle::asNode(nodes[j]->itemHandle));
									if (!s.isEmpty())
									{
										newData->value(tr("rate"),0) = s;
										regulatory << nodes[j]->itemHandle;
										newTextData << newData;
										toolNames << tr("Assignments");
										OutputWindow::message(nodes[j]->itemHandle->fullName() + tr(" has rate: ") + s);
									}
									else
										delete newData;
								}

								if (handles[i]->data && handles[i]->hasTextData(tr("Rates")) &&
									nodes[j]->itemHandle->data && nodes[j]->itemHandle->hasTextData("Assignments") &&
									nodes[j]->itemHandle->data->textData[tr("Assignments")].rowNames().contains(tr("rate")))
								{
									DataTable<QString>* newData = new DataTable<QString>(handles[i]->data->textData[tr("Rates")]);
									newData->value(0,0) = nodes[j]->itemHandle->fullName() + tr(".rate");
									regulatory << handles[i];
									newTextData << newData;
									toolNames << tr("Rates");
								}
							}
							else
								items += nodes[j];
						}
					}
				}
				else
				if (handles[i]->isA("Transcription Regulation"))
				{
                                        if (nodes.size() > 1 && nodes[1] && nodes[1]->itemHandle && !handles.contains(nodes[1]->itemHandle) && nodes[1]->itemHandle->data)
					{
						items += nodes[1];
						if (nodes[1]->itemHandle->hasTextData("Assignments"))
						{
							DataTable<QString>* newData = new DataTable<QString>(nodes[1]->itemHandle->data->textData[tr("Assignments")]);
                                                        QString s0 = hillEquation(NodeHandle::asNode(nodes[1]->itemHandle),handles[i]);
							if (!newData->rowNames().contains(tr("rate")) || newData->value(tr("rate"),0) == s0)
							{
                                                                QString s = hillEquation(NodeHandle::asNode(nodes[1]->itemHandle));
								if (!s.isEmpty() && !(newData->rowNames().contains(nodes[1]->itemHandle->name)))
								{
									newData->value(tr("rate"),0) = s;
									regulatory << nodes[1]->itemHandle;
									newTextData << newData;
									toolNames << tr("Assignments");
									OutputWindow::message(nodes[1]->itemHandle->fullName() + tr(" has rate: ") + s);
								}
								else
									delete newData;
							}
							else
								delete newData;

							QList<ConnectionGraphicsItem*> connections = nodes[1]->connectionsWithoutArrows();
							for (int j=0; j < connections.size(); ++j)
							{
								if (connections[j] && connections[j]->itemHandle && connections[j]->itemHandle->isA("Transcription") &&
									connections[j]->itemHandle->data && connections[j]->itemHandle->hasTextData("Assignments")
									&&	connections[j]->itemHandle->hasTextData("Rates"))
								{
									DataTable<QString>* newData = new DataTable<QString>(connections[j]->itemHandle->data->textData[tr("Rates")]);
									newData->value(0,0) = nodes[1]->itemHandle->fullName() + tr(".rate");
									regulatory << connections[j]->itemHandle;
									newTextData << newData;
									toolNames << tr("Rates");
								}
							}
						}
					}
				}
			}

		if (!regulatory.isEmpty())
		{
			scene->changeData(regulatory,toolNames,newTextData);
			for (int i=0; i < newTextData.size(); ++i)
				delete newTextData[i];
		}

		if (!items.isEmpty())
			itemsMoved(scene,items,QList<QPointF>(),0);
	}

	void AutoGeneRegulatoryTool::itemsMoved(GraphicsScene* scene, const QList<QGraphicsItem*>& items, const QList<QPointF>& distance, Qt::KeyboardModifiers)
	{
		if (!scene) return;

		ItemHandle * handle, * handle2;

		doAssignment = true;

		QList<ItemHandle*> regulatorylist, rbslist, codinglist;

		for (int i=0; i < items.size(); ++i)
		{
			if (qgraphicsitem_cast<NodeGraphicsItem*>(items[i]) && (handle = getHandle(items[i])) && handle->isA("Part"))
			{
				if (handle->isA("Coding")  && !handle->isA("Regulatory"))
				{
					QList<ItemHandle*> upstream;

					findAllPart(scene,qgraphicsitem_cast<NodeGraphicsItem*>(items[i]),tr("Part"),upstream,true,QStringList() << "Terminator",true);

					ItemHandle * rbs = 0, * regulatory = 0, * coding = handle;
					for (int j=0; j < upstream.size() && !(rbs && regulatory); ++j)
					{
						if (!regulatory && upstream[j] && upstream[j]->isA("Regulatory"))
							regulatory = upstream[j];
						if (!rbs && upstream[j] && upstream[j]->isA("RBS"))
							rbs = upstream[j];
					}

					if (!codinglist.contains(coding))
					{
						codinglist << coding;
						regulatorylist << regulatory;
						rbslist << rbs;
					}
				}
			}
		}

		for (int i=0; i < items.size() && i < distance.size(); ++i)
		{
			if (qgraphicsitem_cast<NodeGraphicsItem*>(items[i]) && (handle = getHandle(items[i])) && handle->isA("Part"))
			{
				if (!handle->isA("Coding")  && handle->isA("Regulatory") && handle->data && handle->hasTextData(tr("Assignments")))
				{
					QList<ItemHandle*> downstream;

					items[i]->setPos( items[i]->scenePos() - distance[i] );
					findAllPart(scene,qgraphicsitem_cast<NodeGraphicsItem*>(items[i]),tr("Part"),downstream,false,QStringList() << "Regulatory" << "Terminator",true);
					items[i]->setPos( items[i]->scenePos() + distance[i] );

					ItemHandle * rbs = 0, * coding = 0, * regulatory = 0;
					for (int j=0; j < downstream.size() && !(rbs && regulatory); ++j)
					{
						if (!coding && downstream[j] && downstream[j]->isA("Coding"))
						{
							coding = downstream[j];
						}
						if (!rbs && downstream[j] && downstream[j]->isA("RBS"))
							rbs = downstream[j];
					}

					if (coding && !codinglist.contains(coding))
					{
						codinglist << coding;
						regulatorylist << regulatory;
						rbslist << rbs;
					}
				}
			}
		}

		for (int i=0; i < codinglist.size(); ++i)
		{
			if (codinglist[i])
			{
                                NodeHandle 	* regulatory = NodeHandle::asNode(regulatorylist[i]),
                                                * handle =  NodeHandle::asNode(codinglist[i]),
                                                * rbs =  NodeHandle::asNode(rbslist[i]);
                                if (regulatory && handle)
                                {

                                        QList<ConnectionHandle*> connections = handle->connections();

					for (int j=0; j < connections.size(); ++j)

                                                        if ((handle2 = (connections[j])) && handle2->isA("Transcription") &&
								handle2->data && handle2->hasTextData(tr("Rates"))
								&& handle2->hasNumericalData(tr("Stoichiometry")))
					{
						if (rbs)
						{
							//qDebug() << "here rbs";
							DataTable<QString> dat2( handle2->data->textData[tr("Rates")] );

							bool change = false;

							if (regulatory->data && regulatory->hasTextData(tr("Assignments")) &&
								regulatory->data->textData[tr("Assignments")].rowNames().contains(tr("rate")) &&
								dat2.value(0,0) != regulatory->fullName() + tr(".rate"))
							{
								change = true;
								dat2.value(0,0) = regulatory->fullName() + tr(".rate");
							}

							if (dat2.rowNames().contains(tr("translation")) &&
								handle2->data->numericalData[tr("Stoichiometry")].colNames().contains((handle2->fullName() + tr(".mRNA"))))
							{
								if (dat2.value(tr("translation"),0) != (rbs->fullName() + tr(".strength * ") + handle2->fullName() + tr(".mRNA")))
								{
									dat2.value(tr("translation"),0) = rbs->fullName() + tr(".strength * ") + handle2->fullName() + tr(".mRNA");
									change = true;
								}
							}
							else
							{
                                                                QList<NodeHandle*> nodesOut = connections[j]->nodesOut();
                                                                if (nodesOut.size() > 0 && nodesOut[0]
                                                                        && nodesOut[0]->type == NodeHandle::Type
                                                                        && nodesOut[0]->isA(tr("RNA")))
								{
                                                                        QList<ConnectionHandle*> connections2 = (static_cast<NodeHandle*>(nodesOut[0]))->connections();
                                                                        if (connections2.size() > 0 && connections2[0]
                                                                                && connections2[0]->type == ConnectionHandle::Type
                                                                                && connections2[0]->hasTextData(tr("Rates")))
									{
                                                                                ItemHandle * handle3 = connections2[0];
										DataTable<QString> dat3( handle3->data->textData[tr("Rates")] );
                                                                                dat3.value(0,0) = rbs->fullName() + tr(".strength * ") + nodesOut[0]->fullName();
										scene->changeData(handle3,tr("Rates"),&dat3);
                                                                                OutputWindow::message(QObject::tr("Rate changed for ") + handle3->fullName());
									}
								}
							}
							if (change)
							{
								scene->changeData(handle2,tr("Rates"),&dat2);
								OutputWindow::message(QObject::tr("Rate changed for ") + handle2->fullName());
							}
						}
						else
                                                {
                                                        QList<ConnectionHandle*> connections = (static_cast<NodeHandle*>(handle))->connections();
							for (int j=0; j < connections.size(); ++j)
                                                                if ((handle2 = (connections[j])) && handle2->isA("Transcription") &&
									handle2->data && handle2->hasTextData(tr("Rates"))
									&& handle2->hasNumericalData(tr("Stoichiometry")))
								{
									DataTable<QString> dat2( handle2->data->textData[tr("Rates")] );

									bool change = false;
									if (regulatory->data && regulatory->hasTextData(tr("Assignments")) &&
										regulatory->data->textData[tr("Assignments")].rowNames().contains(tr("rate")) &&
										dat2.value(0,0) != regulatory->fullName() + tr(".rate"))
									{
										change  = true;
										dat2.value(0,0) = regulatory->fullName() + tr(".rate");
									}

									if (dat2.rowNames().contains(tr("translation")) &&
										handle2->data->numericalData[tr("Stoichiometry")].colNames().contains((handle2->fullName() + tr(".mRNA"))))
									{
										if (dat2.value(tr("translation"),0) != (handle2->fullName() + tr(".k1 * ") + handle2->fullName() + tr(".mRNA")))
										{
											change = true;
											dat2.value(tr("translation"),0) = handle2->fullName() + tr(".k1 * ") + handle2->fullName() + tr(".mRNA");
										}
									}
									if (change)
									{
										scene->changeData(handle2,tr("Rates"),&dat2);
										OutputWindow::message(QObject::tr("Rate changed for ") + handle2->fullName());
									}
								}
						}
					}
				}
				else
				{
					//qDebug() << "here no regulatory";
					bool change = false;
					/*
					DataTable<QString> dat( handle->data->textData[tr("Assignments")] );

					if (dat.rowNames().contains(tr("rate")))
					{
						dat.removeRow( dat.rowNames().indexOf(tr("rate")) );
						scene->changeData(handle,tr("Assignments"),&dat);
					}*/

					if (handle->type == NodeHandle::Type)
					{
                                                QList<ConnectionHandle*> connections = (static_cast<NodeHandle*>(handle))->connections();
						for (int j=0; j < connections.size(); ++j)
                                                        if ((handle2 = (connections[j])) && handle2->isA("Transcription") &&
								handle2->data && handle2->hasTextData(tr("Rates"))
								&& handle2->hasNumericalData(tr("Stoichiometry")))
							{
								DataTable<QString> dat2( handle2->data->textData[tr("Rates")] );

								if (dat2.value(0,0).contains(tr(".rate")))
								{
									dat2.value(0,0) = "0.0";
									change = true;
								}

								if (dat2.rowNames().contains(tr("translation")) &&
									handle2->data->numericalData[tr("Stoichiometry")].colNames().contains((handle2->fullName() + tr(".mRNA"))))
								{
									if (dat2.value(tr("translation"),0) != (handle2->fullName() + tr(".k1 * ") + handle2->fullName() + tr(".mRNA")))
									{
										dat2.value(tr("translation"),0) = handle2->fullName() + tr(".k1 * ") + handle2->fullName() + tr(".mRNA");
										change = true;
									}
								}
								if (change)
								{
									scene->changeData(handle2,tr("Rates"),&dat2);
									OutputWindow::message(QObject::tr("Rate changed for ") + handle2->fullName());
								}
							}
					}
				}
			}
		}
	}

	void AutoGeneRegulatoryTool::itemsRemoved(GraphicsScene * scene, const QList<QGraphicsItem*>& , const QList<ItemHandle*>& handles)
	{
		if (!scene) return;

		QList<ItemHandle*> regulatory;
		QStringList toolNames;
		QList< DataTable<QString>* > newTextData;

		QList<ItemHandle*> visited;
                ConnectionHandle * connection;

		for (int i=0; i < handles.size(); ++i)
		{
                        if ((connection = ConnectionHandle::asConnection(handles[i]))
                            && !visited.contains(handles[i]) && handles[i]->isA("Transcription Regulation"))
			{
				visited << handles[i];
                                QList<NodeHandle*> nodes = connection->nodes();
                                if (nodes.size() > 1 && nodes[1] &&
                                        !handles.contains(nodes[1]) &&
                                        nodes[1]->data &&
                                        nodes[1]->hasTextData("Assignments") &&
                                        !visited.contains(nodes[1]))
				{
                                        DataTable<QString>* newData = new DataTable<QString>(nodes[1]->data->textData[tr("Assignments")]);

					QString s = hillEquation(nodes[1],handles[i]);
					if (newData->rowNames().contains(tr("rate")) &&
						newData->value(tr("rate"),0).contains(handles[i]->fullName()) &&
                                                !s.isEmpty() && !(newData->rowNames().contains(nodes[1]->name)))
					{
						newData->value(tr("rate"),0) = s;
                                                regulatory << nodes[1];
						newTextData << newData;
						toolNames << tr("Assignments");
                                                OutputWindow::message(nodes[1]->fullName() + tr(" has rate: ") + s);
					}
					else
					{
						delete newData;
					}

                                        visited << nodes[1];
				}
			}
		}
		if (!regulatory.isEmpty())
		{
			scene->changeData(regulatory,toolNames,newTextData);
			for (int i=0; i < newTextData.size(); ++i)
				delete newTextData[i];
		}
	}

	void AutoGeneRegulatoryTool::itemsSelected(GraphicsScene * scene,const QList<QGraphicsItem*>& items, QPointF, Qt::KeyboardModifiers)
	{
		if (!mainWindow || !scene) return;

		ItemHandle * handle = 0;
		bool containsSpecies = false;
		bool containsProteins = false;
		bool containsRegulator = false;
		bool containsCoding = false;
		bool containsTranscription = false;
		for (int i=0; i < items.size(); ++i)
		{
			handle = getHandle(items[i]);
			if (!containsSpecies && handle && handle->isA("Species"))
				containsSpecies = true;
			if (!containsProteins && handle && handle->isA("Protein"))
				containsProteins = true;
			if (!containsRegulator && handle && (handle->isA("Gene") || handle->isA("Regulatory")))
				containsRegulator = true;
			if (!containsCoding && handle && (handle->isA("Gene") || handle->isA("Coding")))
				containsCoding = true;
			if (!containsTranscription && handle && (handle->isA("Transcription")))
				containsTranscription = true;

			if (containsTranscription || (containsRegulator && containsSpecies && containsCoding)) break;
		}

		if (containsSpecies || containsRegulator || containsCoding || containsTranscription)
		{
			if (separator)
				mainWindow->contextItemsMenu.addAction(separator);
			else
				separator = mainWindow->contextItemsMenu.addSeparator();

			if (containsSpecies)
				mainWindow->contextItemsMenu.addAction(&autoDegradation);
			else
				mainWindow->contextItemsMenu.removeAction(&autoDegradation);

			if (containsProteins)
				mainWindow->contextItemsMenu.addAction(&autoPhosphate);
			else
				mainWindow->contextItemsMenu.removeAction(&autoPhosphate);

			if (containsRegulator)
			{
				mainWindow->contextItemsMenu.addAction(&autoTFUp);
				mainWindow->contextItemsMenu.addAction(&autoTFDown);
			}
			else
			{
				mainWindow->contextItemsMenu.removeAction(&autoTFUp);
				mainWindow->contextItemsMenu.removeAction(&autoTFDown);
			}

			if (containsCoding)
				mainWindow->contextItemsMenu.addAction(&autoGeneProduct);
			else
				mainWindow->contextItemsMenu.removeAction(&autoGeneProduct);

			if (containsTranscription)
				mainWindow->contextItemsMenu.addAction(&mRNAstep);
			else
				mainWindow->contextItemsMenu.removeAction(&mRNAstep);

		}
		else
		{
			if (separator)
				mainWindow->contextItemsMenu.removeAction(separator);
			mainWindow->contextItemsMenu.removeAction(&autoDegradation);
			mainWindow->contextItemsMenu.removeAction(&autoTFUp);
			mainWindow->contextItemsMenu.removeAction(&autoTFDown);
			mainWindow->contextItemsMenu.removeAction(&autoGeneProduct);
			mainWindow->contextItemsMenu.removeAction(&mRNAstep);
		}
	}

	void AutoGeneRegulatoryTool::findAllPart(GraphicsScene* scene,NodeGraphicsItem * node, const QString& family,QList<ItemHandle*>& handles,bool upstream,const QStringList & until, bool stopIfElongation)
	{
		if (!scene || !node) return;

		bool flipped = (node->transform().m11() < 0);
		if (flipped)
		{
			upstream = !upstream;
		}

		QPointF p;

		if (upstream)
			p = QPointF(node->sceneBoundingRect().left() - 10.0, node->scenePos().ry());
		else
			p = QPointF(node->sceneBoundingRect().right() + 10.0, node->scenePos().ry());

		QList<QGraphicsItem*> items = scene->items(QRectF(p.x()-1.0,p.y()-10.0,2.0,20.0));

		ItemHandle * handle = 0, * h = 0;

		for (int i=0; i < items.size(); ++i)
		{
			handle = getHandle(items[i]);
			if (handle && handle->isA(family) &&
				( (flipped && items[i]->transform().m11() < 0) || (!flipped && items[i]->transform().m11() > 0) ))
				break;
			else
				handle = 0;
		}

		if (!handle)
		{
			if (upstream)
			{
				QList<ConnectionGraphicsItem*> connections = node->connectionsWithArrows();
				for (int j=0; j < connections.size(); ++j)
				{
					if (connections[j] && connections[j]->itemHandle && connections[j]->itemHandle->isA(tr("Elongation")))
					{
						QList<NodeGraphicsItem*> connectedNodes = connections[j]->nodesWithoutArrows();
						if (connectedNodes.size() > 0 && connectedNodes[0] && (h = connectedNodes[0]->itemHandle)
							&& h->isA(family) && !handles.contains(h)
							&& (!stopIfElongation || !connections[j]->itemHandle->data
								|| !connections[j]->itemHandle->hasNumericalData(tr("Stoichiometry"))
								|| connections[j]->itemHandle->data->numericalData[tr("Stoichiometry")].rows() < 1)
							)
							{
								handle = h;
								break;
							}
					}
				}
			}
			else
			{
				QList<ConnectionGraphicsItem*> connections = node->connectionsWithoutArrows();
				for (int j=0; j < connections.size(); ++j)
				{
					if (connections[j] && connections[j]->itemHandle && connections[j]->itemHandle->isA(tr("Elongation")))
					{
						QList<NodeGraphicsItem*> connectedNodes = connections[j]->nodesWithArrows();
						if (connectedNodes.size() > 0 && connectedNodes[0] && (h = connectedNodes[0]->itemHandle)
							&& h->isA(family) && !handles.contains(h)
							&& (!stopIfElongation || !connections[j]->itemHandle->data
								|| !connections[j]->itemHandle->hasNumericalData(tr("Stoichiometry"))
								|| connections[j]->itemHandle->data->numericalData[tr("Stoichiometry")].rows() < 1)
							)
							{
								handle = h;
								break;
							}
					}
				}
			}
		}

		if (!handle) return;

		for (int k=0; k < until.size(); ++k)
			if (handle->isA(until[k]))
			{
				return;
			}

		if (!handles.contains(handle))
			handles << handle;

		items.clear();
		items = handle->graphicsItems;

		bool hit = false;

		for (int i=0; i < items.size(); ++i)
			if ((node = NodeGraphicsItem::topLevelNodeItem(items[i])))
			{
				if (upstream)
					p = QPointF(node->sceneBoundingRect().left() - 10.0,node->scenePos().ry());
				else
					p = QPointF(node->sceneBoundingRect().right() + 10.0,node->scenePos().ry());
				QList<QGraphicsItem*> items2 = scene->items(QRectF(p.x()-1.0,p.y()-10.0,2.0,20.0));

				hit = false;

				for (int j=0; j < items2.size(); ++j)
					if ((node = NodeGraphicsItem::topLevelNodeItem(items2[j])))
					{
						h = node->itemHandle;

						if (!h || !h->isA(family)) continue;

						if ((flipped && (node->transform().m11() > 0)) ||
							(!flipped && (node->transform().m11() < 0))) continue; //wrong orientation

						for (int k=0; k < until.size(); ++k)
							if (h->isA(until[k]))
							{
								return;
							}
						if (!handles.contains(h))
						{
							handles << h;
							items << h->graphicsItems;
						}
						hit = true;
						break;
					}
				if (!hit) //look for connected Elongation edges
				{
					node = NodeGraphicsItem::topLevelNodeItem(items[i]);
					if (upstream)
					{
						QList<ConnectionGraphicsItem*> connections = node->connectionsWithArrows();
						for (int j=0; j < connections.size(); ++j)
						{
							if (connections[j] && connections[j]->itemHandle && connections[j]->itemHandle->isA(tr("Elongation")))
							{
								QList<NodeGraphicsItem*> connectedNodes = connections[j]->nodesWithoutArrows();
								if (connectedNodes.size() > 0 && connectedNodes[0] && (h = connectedNodes[0]->itemHandle)
									&& h->isA(family) && !handles.contains(h))
									{
										handles << h;
										items << h->graphicsItems;
										break;
									}
							}
						}
					}
					else
					{
						QList<ConnectionGraphicsItem*> connections = node->connectionsWithoutArrows();
						for (int j=0; j < connections.size(); ++j)
						{
							if (connections[j] && connections[j]->itemHandle && connections[j]->itemHandle->isA(tr("Elongation")))
							{
								QList<NodeGraphicsItem*> connectedNodes = connections[j]->nodesWithArrows();
								if (connectedNodes.size() > 0 && connectedNodes[0] && (h = connectedNodes[0]->itemHandle)
									&& h->isA(family) && !handles.contains(h))
									{
										handles << h;
										items << h->graphicsItems;
										break;
									}
							}
						}
					}
				}
			}
	}

        QString AutoGeneRegulatoryTool::hillEquation(NodeHandle * handle, ItemHandle * except)
        {
		if (!handle || !(handle->isA("Gene") || handle->isA(tr("Regulatory")))) return QString();

                QList<ConnectionHandle*> connections = handle->connections();

		QStringList positives, allTFs;

		for (int i=0; i < connections.size(); ++i)
                        if (connections[i])
			{
                                if (!connections[i]->isA("Transcription Regulation") || connections[i] == except)
					continue;
                                QString cname = connections[i]->fullName();
                                bool isPositive = !(connections[i]->isA("Transcription Repression"));
                                QList<NodeHandle*> nodesIn = connections[i]->nodesIn();
				for (int j=0; j < nodesIn.size(); ++j)
				{
					ItemHandle * nodeHandle = (nodesIn[j]);
					if (nodeHandle)
					{
						QString s0 = nodeHandle->fullName();
						QString s1 = tr("((") + s0 + tr("/") + cname + tr(".Kd)^") + cname + tr(".h)");
						if (isPositive)
							positives << s1;

						allTFs << tr("(1+") + s1 + tr(")");
					}
				}
			}
		if (allTFs.isEmpty()) return tr("");

		if (positives.isEmpty()) positives << tr("1.0");
		QString rate = handle->fullName() + tr(".strength*(") + positives.join(" + ") + tr(")/(") + allTFs.join("*") + tr(")");
		return rate;
	}
}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::AutoGeneRegulatoryTool * tool = new Tinkercell::AutoGeneRegulatoryTool;
// 	if (main->tools.contains(tool->name))
// 		delete tool;
// 	else
// 		tool->setMainWindow(main);


}
