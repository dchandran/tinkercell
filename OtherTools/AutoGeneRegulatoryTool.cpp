/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 Automatically manage gene regulatory network rates and parameters

****************************************************************************/

#include "UndoCommands.h"
#include "ItemHandle.h"
#include "CThread.h"
#include "GraphicsScene.h"
#include "NetworkWindow.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "CollisionDetection.h"
#include "BasicGraphicsToolbox.h"
#include "ConnectionSelection.h"
#include "NodeInsertion.h"
#include "NodesTree.h"
#include "ConnectionsTree.h"
#include "AutoGeneRegulatoryTool.h"

namespace Tinkercell
{
	AutoGeneRegulatoryTool_FtoS AutoGeneRegulatoryTool::fToS;
	
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
		
		connect(&fToS,SIGNAL(partsIn(QSemaphore*, ItemHandle*, QList<ItemHandle*>*)),this,SLOT(partsIn(QSemaphore*, ItemHandle*, QList<ItemHandle*>*)));
		connect(&fToS,SIGNAL(partsUpstream(QSemaphore*, ItemHandle*, QList<ItemHandle*>*)),this,SLOT(partsUpstream(QSemaphore*, ItemHandle*, QList<ItemHandle*>*)));
		connect(&fToS,SIGNAL(partsDownstream(QSemaphore*, ItemHandle*, QList<ItemHandle*>*)),this,SLOT(partsDownstream(QSemaphore*, ItemHandle*, QList<ItemHandle*>*)));
		connect(&fToS,SIGNAL(alignParts(QSemaphore*,const QList<ItemHandle*>&)),this,SLOT(alignParts(QSemaphore*,const QList<ItemHandle*>&)));
	}

	void AutoGeneRegulatoryTool::autoPhosphateTriggered()
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !mainWindow) return;		
		if (!mainWindow->tool(tr("Nodes Tree")) || !mainWindow->tool(tr("Connections Tree"))) return;
		
		QWidget * treeWidget = mainWindow->tool(tr("Nodes Tree"));
		NodesTree * nodesTree = static_cast<NodesTree*>(treeWidget);
		
		if (!nodesTree->nodeFamilies.contains("Phosphate")) return;
		
		NodeFamily * nodeFamily = nodesTree->nodeFamilies["Phosphate"];
		
		QList<QGraphicsItem*>& selected = scene->selected();
		ItemHandle * handle = 0;
		
		QList<QGraphicsItem*> sceneItems = scene->items();
		QList<QGraphicsItem*> list;
		
		QString appDir = QCoreApplication::applicationDirPath();
		
		QList<ItemHandle*> handlesPhos;
		
		for (int i=0; i < selected.size(); ++i)
		{
			handle = getHandle(selected[i]);
			if (qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]) && handle && handle->isA("Species") && !handlesPhos.contains(handle))
			{
				handlesPhos += handle;
				ItemHandle * node = handle;
				
				QRectF boundingRect = selected[i]->sceneBoundingRect();
				for (int j=0; j < handle->graphicsItems.size(); ++j)
				{
					if (handle->graphicsItems[j]->sceneBoundingRect().intersects(boundingRect))
						boundingRect.unite(handle->graphicsItems[j]->sceneBoundingRect());
				}
				
				qreal xpos = (selected[i]->sceneBoundingRect().right()), 
					  ypos = (selected[i]->sceneBoundingRect().center().y()),
					  height = 0.0;
					  
				NodeGraphicsItem * image = 0;
				
				for (int j=0; j < nodeFamily->graphicsItems.size(); ++j)
				{
					image = (NodeGraphicsItem::topLevelNodeItem(nodeFamily->graphicsItems[j]));
					if (image)
					{
						image = image->clone();
					
						if (image->defaultSize.width() > 0 && image->defaultSize.height() > 0)
							image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),image->defaultSize.height()/image->sceneBoundingRect().height());
						
						qreal w = image->sceneBoundingRect().width();
						
						image->setPos(xpos, ypos);

						image->setBoundingBoxVisible(false);

						if (image->isValid())
						{
							xpos += w;
							setHandle(image,node);
							list += image;
						}
						if (image->sceneBoundingRect().height() > height)
							height = image->sceneBoundingRect().height();
					}
				}
			}
		}
		
		if (!list.isEmpty())
		{
			scene->insert(tr("phosphates added"),list);
		}
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
		if (!mainWindow->tool(tr("Nodes Tree")) || !mainWindow->tool(tr("Connections Tree"))) return;
		
		QWidget * treeWidget = mainWindow->tool(tr("Nodes Tree"));
		NodesTree * nodesTree = static_cast<NodesTree*>(treeWidget);
		
		if (!nodesTree->nodeFamilies.contains("Transcription Factor")) return;
		
                treeWidget = mainWindow->tool(tr("Connections Tree"));
		ConnectionsTree * connectionsTree = static_cast<ConnectionsTree*>(treeWidget);
		
		if (!connectionsTree->connectionFamilies.contains(regulationName)) return;
		
		NodeFamily * nodeFamily = nodesTree->nodeFamilies["Transcription Factor"];
		ConnectionFamily * connectionFamily = connectionsTree->connectionFamilies[regulationName];
		
		QList<QGraphicsItem*>& selected = scene->selected();
		ItemHandle * handle = 0;
		
                QList<ItemHandle*> sceneItems = scene->allHandles();
		QList<QGraphicsItem*> list;
		
		QString appDir = QCoreApplication::applicationDirPath();
  		
		QList<ItemHandle*> handlesDegraded;

		QList<NodeGraphicsItem*> regulatorNodes;
		QList<ItemHandle*> regulatorHandles;
		QList< DataTable<QString>* > newTextData;

		for (int i=0; i < selected.size(); ++i)
		{
			handle = getHandle(selected[i]);
			if (qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]) && handle && (handle->isA("Regulator") || handle->isA("Gene")) && !handlesDegraded.contains(handle))
			{
				regulatorNodes += qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]);
				handlesDegraded += handle;
				NodeHandle * node = new NodeHandle(nodeFamily);
				node->name = tr("TF");
				node->name = NodeInsertion::findUniqueName(node,sceneItems);
				
				qreal xpos = selected[i]->sceneBoundingRect().left() - 100.0, 
					  ypos = selected[i]->sceneBoundingRect().top() - 100.0, 
					  height = 0.0;
				
				for (int j=0; j < 100; ++j)
				{
					if (scene->items(QPointF(xpos,ypos)).isEmpty()) break;
					ypos -= (selected[i]->sceneBoundingRect().height() * 1.5);
				}
				NodeGraphicsItem * image = 0;
				
				for (int j=0; j < nodeFamily->graphicsItems.size(); ++j)
				{
					image = (NodeGraphicsItem::topLevelNodeItem(nodeFamily->graphicsItems[j]));
					if (image)
					{
						image = image->clone();
					
						if (image->defaultSize.width() > 0 && image->defaultSize.height() > 0)
							image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),image->defaultSize.height()/image->sceneBoundingRect().height());
						
						qreal w = image->sceneBoundingRect().width();
						
						image->setPos(xpos, ypos);
						
						image->setBoundingBoxVisible(false);

						if (image->isValid())
						{
							xpos += w;
							setHandle(image,node);
							list += image;
						}
						if (image->sceneBoundingRect().height() > height)
							height = image->sceneBoundingRect().height();
					}
				}
				if (image)
				{
					TextGraphicsItem * nameItem = new TextGraphicsItem(node,0);
					QFont font = nameItem->font();
					font.setPointSize(22);
					nameItem->setFont(font);
					nameItem->setPos(xpos - 2 * nameItem->boundingRect().width(), ypos + height/2.0);
					list += nameItem;
				
					ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
					ConnectionHandle * connection = new ConnectionHandle(connectionFamily,item);
					
					item->pathVectors += 
						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(item,image));
						
					item->pathVectors += 
						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(item,selected[i]));

					ArrowHeadItem * arrow = 0;
					QString nodeImageFile;
					if (regulationName == tr("Transcription Activation"))
						nodeImageFile = appDir + tr("/ArrowItems/Transcription_Activation.xml");
					else
						nodeImageFile = appDir + tr("/ArrowItems/Transcription_Repression.xml");
					NodeGraphicsReader imageReader;
					arrow = new ArrowHeadItem(item);
					imageReader.readXml(arrow,nodeImageFile);
					arrow->normalize();
					arrow->scale(0.1,0.1);
					item->pathVectors.last().arrowStart = arrow;
					list += arrow;
				

					connection->name = tr("J");
					item->lineType = ConnectionGraphicsItem::line;
					connection->name = NodeInsertion::findUniqueName(connection,sceneItems);
                                        sceneItems << node << connection;

					nameItem = new TextGraphicsItem(connection,0);
					list += nameItem;
					nameItem->setPos( 0.5*(image->pos() + selected[i]->scenePos() ) );
					font = nameItem->font();
					font.setPointSize(22);
					nameItem->setFont(font);
				
					list += item;

					if (handle->hasTextData("Assignments"))
					{
						regulatorHandles << handle;
					}
				}
			}
		}
		
		if (!list.isEmpty())
		{
			scene->insert(tr("TF's inserted"),list);
		}
		
	}
	
	void AutoGeneRegulatoryTool::autoGeneProductTriggered()
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !mainWindow) return;		
		if (!mainWindow->tool(tr("Nodes Tree")) || !mainWindow->tool(tr("Connections Tree"))) return;
		
		QWidget * treeWidget = mainWindow->tool(tr("Nodes Tree"));
		NodesTree * nodesTree = static_cast<NodesTree*>(treeWidget);
		
		if (!nodesTree->nodeFamilies.contains("Protein")) return;
		
		treeWidget = mainWindow->tool(tr("Connections Tree"));
		ConnectionsTree * connectionsTree = static_cast<ConnectionsTree*>(treeWidget);		
		
		if (!connectionsTree->connectionFamilies.contains("Transcription")) return;
		
		if (!nodesTree->nodeFamilies.contains("Empty")) return;	
		
		if (!connectionsTree->connectionFamilies.contains("Biochemical")) return;
		
		NodeFamily * nodeFamily2 = nodesTree->nodeFamilies["Empty"];
		ConnectionFamily * connectionFamily2 = connectionsTree->connectionFamilies["Biochemical"];
		
		NodeFamily * nodeFamily = nodesTree->nodeFamilies["Protein"];
		ConnectionFamily * connectionFamily = connectionsTree->connectionFamilies["Transcription"];
		
		QList<QGraphicsItem*>& selected = scene->selected();
		ItemHandle * handle = 0;
		
		QList<ItemHandle*> sceneItems = scene->allHandles();
		QList<QGraphicsItem*> list;
		
		QString appDir = QCoreApplication::applicationDirPath();
 		
		QList<ItemHandle*> handlesDegraded;
		
		for (int i=0; i < selected.size(); ++i)
		{
			handle = getHandle(selected[i]);
			if (qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]) && handle && (handle->isA("Gene") || handle->isA("Coding")) && !handlesDegraded.contains(handle))
			{
				handlesDegraded += handle;
				NodeHandle * node = new NodeHandle(nodeFamily);
				node->name = tr("P");
				node->name = NodeInsertion::findUniqueName(node,sceneItems);
				
				qreal xpos = selected[i]->scenePos().rx(), 
					  ypos = selected[i]->scenePos().ry() - (selected[i]->sceneBoundingRect().height() * 3), 
					  height = 0.0;
				
				for (int j=0; j < 100; ++j)
				{
					if (scene->items(QPointF(xpos,ypos)).isEmpty()) break;
					ypos -= (selected[i]->sceneBoundingRect().height() * 1.0);
				}
				NodeGraphicsItem * image = 0;
				
				for (int j=0; j < nodeFamily->graphicsItems.size(); ++j)
				{
					image = (NodeGraphicsItem::topLevelNodeItem(nodeFamily->graphicsItems[j]));
					if (image)
					{
						image = image->clone();
					
						if (image->defaultSize.width() > 0 && image->defaultSize.height() > 0)
							image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),image->defaultSize.height()/image->sceneBoundingRect().height());
						
						qreal w = image->sceneBoundingRect().width();
						ypos -= (image->sceneBoundingRect().height() * 1.0);
						
						image->setPos(xpos, ypos);
						
						image->setBoundingBoxVisible(false);

						if (image->isValid())
						{
							xpos += w;
							setHandle(image,node);
							list += image;
						}
						if (image->sceneBoundingRect().height() > height)
							height = image->sceneBoundingRect().height();
					}
				}
				if (image)
				{
					TextGraphicsItem * nameItem = new TextGraphicsItem(node,0);
					QFont font = nameItem->font();
					font.setPointSize(22);
					nameItem->setFont(font);
					nameItem->setPos(image->sceneBoundingRect().right(), ypos + height/2.0);
					list += nameItem;
				
					ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
					ConnectionHandle * connection = new ConnectionHandle(connectionFamily,item);
					
					item->pathVectors += 
						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(item,selected[i]));
					
					item->pathVectors += 
						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(item,image));

					ArrowHeadItem * arrow = 0;
					QString nodeImageFile = appDir + tr("/ArrowItems/Transcription.xml");
					NodeGraphicsReader imageReader;
					arrow = new ArrowHeadItem(item);
					imageReader.readXml(arrow,nodeImageFile);
					arrow->normalize();
					arrow->scale(0.1,0.1);
					item->pathVectors.last().arrowStart = arrow;
					list += arrow;							
				
					connection->name = tr("J");
					item->lineType = ConnectionGraphicsItem::line;
					connection->name = NodeInsertion::findUniqueName(connection,sceneItems);
                                        sceneItems << node << connection;

					nameItem = new TextGraphicsItem(connection,0);
					list += nameItem;
					nameItem->setPos(image->sceneBoundingRect().left() - nameItem->sceneBoundingRect().width(),image->sceneBoundingRect().bottom() + image->sceneBoundingRect().height());
					font = nameItem->font();
					font.setPointSize(22);
					nameItem->setFont(font);
				
					list += item;
				}
				else
				{
					continue;
				}
				
				node = new NodeHandle(nodeFamily2);
				node->name = tr("emp");
				node->name = NodeInsertion::findUniqueName(node,sceneItems);
				
				xpos = (image->sceneBoundingRect().right() + 100.0);
				ypos = (image->sceneBoundingRect().top() - 100.0);
				height = 0.0;
				
				NodeGraphicsItem * image2 = image;
				
				for (int j=0; j < nodeFamily2->graphicsItems.size(); ++j)
				{
					image = (NodeGraphicsItem::topLevelNodeItem(nodeFamily2->graphicsItems[j]));
					if (image)
					{
						image = image->clone();
					
						if (image->defaultSize.width() > 0 && image->defaultSize.height() > 0)
							image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),image->defaultSize.height()/image->sceneBoundingRect().height());
						
						qreal w = image->sceneBoundingRect().width();
						
						image->setPos(xpos, ypos);

						image->setBoundingBoxVisible(false);

						if (image->isValid())
						{
							xpos += w;
							setHandle(image,node);
							list += image;
						}
						if (image->sceneBoundingRect().height() > height)
							height = image->sceneBoundingRect().height();
					}
				}
				if (image)
				{
					TextGraphicsItem * nameItem;
					QFont font; 
					
					ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
					ConnectionHandle * connection = new ConnectionHandle(connectionFamily2,item);
					
					item->pathVectors += 
						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(item,image2));
					
					item->pathVectors += 
						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(item,image));

					ArrowHeadItem * arrow = 0;
					QString nodeImageFile = appDir + tr("/ArrowItems/Reaction.xml");
					NodeGraphicsReader imageReader;
					arrow = new ArrowHeadItem(item);
					imageReader.readXml(arrow,nodeImageFile);
					arrow->normalize();
					arrow->scale(0.1,0.1);
					item->pathVectors.last().arrowStart = arrow;
					list += arrow;							
				

					connection->name = tr("J");
					item->lineType = ConnectionGraphicsItem::line;
					connection->name = NodeInsertion::findUniqueName(connection,sceneItems);
                                        sceneItems << node << connection;

					nameItem = new TextGraphicsItem(connection,0);
					list += nameItem;
					nameItem->setPos( 0.5*(image->pos() + image2->scenePos() ) );
					font = nameItem->font();
					font.setPointSize(22);
					nameItem->setFont(font);
				
					list += item;
				}
			}
		}
		
		if (!list.isEmpty())
		{
			
			/*QUndoCommand * command = new CompositeCommand(tr("Transcription addded"), 
													  QList<QUndoCommand*>()
														<< new InsertGraphicsCommand(tr("Transcription added"),scene,list)
														<< insertmRNAstep(list));
													  
			if (scene->historyStack)
				scene->historyStack->push(command);
			else
			{
				command->redo();
				delete command;
			}
			
			ItemHandle * h = 0;
			QList<ItemHandle*> handles;
			for (int i=0; i < list.size(); ++i)
				if ( (h = getHandle(list[i])) && !handles.contains(h))
				{
					handles += h;
				}
			
			emit itemsInsertedSignal(scene, list , handles);
			emit dataChanged(handles);*/
			
			scene->insert(tr("Transcription added"),list);
			scene->selected() += list;
			insertmRNAstep();
			scene->select(0);
		}
	}
	
	void AutoGeneRegulatoryTool::autoDegradationTriggered()
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !mainWindow) return;		
		if (!mainWindow->tool(tr("Nodes Tree")) || !mainWindow->tool(tr("Connections Tree"))) return;
		
		QWidget * treeWidget = mainWindow->tool(tr("Nodes Tree"));
		NodesTree * nodesTree = static_cast<NodesTree*>(treeWidget);
		
		if (!nodesTree->nodeFamilies.contains("Empty")) return;
		
		treeWidget = mainWindow->tool(tr("Connections Tree"));
		ConnectionsTree * connectionsTree = static_cast<ConnectionsTree*>(treeWidget);		
		
		if (!connectionsTree->connectionFamilies.contains("Biochemical")) return;
		
		NodeFamily * nodeFamily = nodesTree->nodeFamilies["Empty"];
		ConnectionFamily * connectionFamily = connectionsTree->connectionFamilies["Biochemical"];
		
		QList<QGraphicsItem*>& selected = scene->selected();
		ItemHandle * handle = 0;
		
		QList<ItemHandle*> sceneItems = scene->allHandles();
		QList<QGraphicsItem*> list;
		
		QString appDir = QCoreApplication::applicationDirPath();
 		
		QList<ItemHandle*> handlesDegraded;
		
		for (int i=0; i < selected.size(); ++i)
		{
			handle = getHandle(selected[i]);
			if (qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]) && handle && handle->isA("Species") && !handlesDegraded.contains(handle))
			{
				handlesDegraded += handle;
				NodeHandle * node = new NodeHandle(nodeFamily);
				node->name = tr("emp");
				node->name = NodeInsertion::findUniqueName(node,sceneItems);
				
				qreal xpos = (selected[i]->sceneBoundingRect().right() + 100.0), 
					  ypos = (selected[i]->sceneBoundingRect().top() - 100.0),
					  height = 0.0;
					  
				NodeGraphicsItem * image = 0;
				
				for (int j=0; j < nodeFamily->graphicsItems.size(); ++j)
				{
					image = (NodeGraphicsItem::topLevelNodeItem(nodeFamily->graphicsItems[j]));
					if (image)
					{
						image = image->clone();
					
						if (image->defaultSize.width() > 0 && image->defaultSize.height() > 0)
							image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),image->defaultSize.height()/image->sceneBoundingRect().height());
						
						qreal w = image->sceneBoundingRect().width();
						
						image->setPos(xpos, ypos);

						image->setBoundingBoxVisible(false);

						if (image->isValid())
						{
							xpos += w;
							setHandle(image,node);
							list += image;
						}
						if (image->sceneBoundingRect().height() > height)
							height = image->sceneBoundingRect().height();
					}
				}
				if (image)
				{
					TextGraphicsItem * nameItem;
					QFont font; 
					
					ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
					ConnectionHandle * connection = new ConnectionHandle(connectionFamily,item);
					
					item->pathVectors += 
						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(item,selected[i]));
					
					item->pathVectors += 
						ConnectionGraphicsItem::PathVector(1,new ConnectionGraphicsItem::ControlPoint(item,image));

					ArrowHeadItem * arrow = 0;
					QString nodeImageFile = appDir + tr("/ArrowItems/Reaction.xml");
					NodeGraphicsReader imageReader;
					arrow = new ArrowHeadItem(item);
					imageReader.readXml(arrow,nodeImageFile);
					arrow->normalize();
					arrow->scale(0.1,0.1);
					item->pathVectors.last().arrowStart = arrow;
					list += arrow;							
				

					connection->name = tr("J");
					item->lineType = ConnectionGraphicsItem::line;
					connection->name = NodeInsertion::findUniqueName(connection,sceneItems);
                                        sceneItems << node << connection;

					nameItem = new TextGraphicsItem(connection,0);
					list += nameItem;
					nameItem->setPos( 0.5*(image->pos() + selected[i]->scenePos() ) );
					font = nameItem->font();
					font.setPointSize(22);
					nameItem->setFont(font);
				
					list += item;
				}
			}
		}
		
		if (!list.isEmpty())
		{
			scene->insert(tr("auto degradation"),list);
		}
	}
	
	void AutoGeneRegulatoryTool::autoAssignRates(QList<NodeHandle*>& parts)
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !mainWindow || parts.isEmpty()) return;
		
		QList<NodeHandle*> activators, repressors;
		QList<ConnectionHandle*> TFconnections;
		ItemHandle * rbs = 0, * promoter = 0, * regulator = 0;
		
		QList<ItemHandle*> targetHandles;
		QList<QString> hashStrings;
		QList<DataTable<QString>*> dataTables;
		
		for (int i=0; i < parts.size(); ++i)
			if (parts[i])
			{
				if (NodeHandle::asNode(parts[i]) && parts[i]->isA(tr("Regulator")))
				{
					if (regulator == 0) 
						regulator = parts[i];
					QList<ConnectionHandle*> connections = NodeHandle::asNode(parts[i])->connections();
					for (int j=0; j < connections.size(); ++j)
						if (connections[j] && connections[j]->isA(tr("Binding")))
						{
							if (connections[j]->isA(tr("Transcription Repression")))
							{
								QList<NodeHandle*> nodes = (connections[j])->nodesIn();
								for (int k=0; k < nodes.size(); ++k)
								{
									repressors += nodes[k];
									TFconnections += connections[j];
								}
							}
							else
							{
								QList<NodeHandle*> nodes = (connections[j])->nodesIn();
								for (int k=0; k < nodes.size(); ++k)
								{
									activators += nodes[k];
									TFconnections += connections[j];
								}
							}
						}
				}
				
				if (parts[i]->isA(tr("RBS")))
				{
					rbs = parts[i];
				}
				
				if (parts[i]->isA(tr("Promoter")))
				{
					if (promoter == 0)
						promoter = parts[i];
				}
				
				if (parts[i]->isA(tr("Coding")) && NodeHandle::asNode(parts[i]))
				{
					if (!promoter && regulator)
						promoter = regulator;
					
					if (promoter && promoter->hasTextData(tr("Assignments")) 
						&& promoter->hasNumericalData(tr("Numerical Attributes")) 
						&& promoter->data->numericalData[tr("Numerical Attributes")].getRowNames().contains(tr("strength")))
					{
						DataTable<QString> * sDat = new DataTable<QString>(promoter->data->textData[tr("Assignments")]);
						QString s0 = tr("");
						
						bool missing = activators.isEmpty() && repressors.isEmpty();
						if (sDat->getRowNames().contains(tr("rate")))
						{
							s0 = sDat->value(tr("rate"),0);
							for (int j=0; j < activators.size(); ++j)
								if (activators[j] && !s0.contains(activators[j]->fullName()))
								{
									missing = true;
									break;
								}
								
							for (int j=0; j < repressors.size(); ++j)
								if (repressors[j] && !s0.contains(repressors[j]->fullName()))
								{
									missing = true;
									break;
								}
						}
							
						if (!sDat->getRowNames().contains(tr("rate")) || missing)
						{
							QString s = hillEquation(TFconnections,activators,repressors);
							
							if (!s.isNull() && !s.isEmpty())
								s = promoter->fullName() + tr(".strength*") + s;
							else
								s = promoter->fullName() + tr(".strength");
							
							if (!sDat->getRowNames().contains(tr("rate")) ||
								 sDat->value(tr("rate"),0) != s)
								{
									sDat->value(tr("rate"),0) = s;
									targetHandles += promoter;
									hashStrings += tr("Assignments");
									dataTables += sDat;
								}
								else
								{
									delete sDat;
								}
						}
						else
						{
							delete sDat;
						}
					}
						
					QList<ConnectionHandle*> connections = NodeHandle::asNode(parts[i])->connections();
					for (int j=0; j < connections.size(); ++j)
						if (connections[j] && 
							connections[j]->isA(tr("Transcription")) &&
							connections[j]->hasTextData(tr("Rates")))
					{
						DataTable<QString> * sDat = new DataTable<QString>(connections[j]->data->textData[tr("Rates")]);
						
						if (promoter 
							&& promoter->hasNumericalData(tr("Numerical Attributes")) 
							&& promoter->data->numericalData[tr("Numerical Attributes")].getRowNames().contains(tr("strength")))
						{
							if (sDat->value(0,0) != promoter->fullName() + tr(".rate"))
							{
								sDat->value(0,0) = promoter->fullName() + tr(".rate");
								targetHandles += connections[j];
								hashStrings += tr("Rates");
								dataTables += sDat;
							}
						}
						else
							if (sDat->value(0,0) != tr("0.0"))
							{
								sDat->value(0,0) = tr("0.0");
								targetHandles += connections[j];
								hashStrings += tr("Rates");
								dataTables += sDat;
							}
							
						if (sDat->getRowNames().contains(tr("translation")))
						{
							QString s = sDat->value(tr("translation"),0);
							
							if (rbs && !sDat->value(tr("translation"),0).contains(rbs->fullName()))
								s = rbs->fullName() + tr(".strength * ") + sDat->value(tr("translation"),0);
							else
								s.replace(QRegExp(tr("\\S+\\.strength\\s*\\*\\s*")),tr(""));
								
							if (sDat->value(tr("translation"),0) != s)
							{
								sDat->value(tr("translation"),0) = s;
								if (!dataTables.contains(sDat))
								{
									targetHandles += connections[j];
									hashStrings += tr("Rates");
									dataTables += sDat;
								}
							}
						}
											
						QList<NodeHandle*> rna = connections[j]->nodesOut();
							for (int k=0; k < rna.size(); ++k)
								if (NodeHandle::asNode(rna[k]) && rna[k]->isA(tr("RNA")))
								{
									QList<ConnectionHandle*> connections2 = NodeHandle::asNode(rna[k])->connections();
									for (int l=0; l < connections2.size(); ++l)
										if (connections2[l] && 
											connections2[l]->nodesIn().contains(NodeHandle::asNode(rna[k])) &&
											connections2[l]->hasTextData(tr("Rates")))
											if (rbs)
											{
												bool hasProtein = false;
												QList<NodeHandle*> nodes2 = connections2[l]->nodesOut();
												for (int m=0; m < nodes2.size(); ++m)
													if (nodes2[m] && nodes2[m]->isA(tr("Protein")))
													{
														hasProtein = true;
														break;
													}
												if (hasProtein)
												{
													DataTable<QString> * sDat2 = new DataTable<QString>(connections2[l]->data->textData[tr("Rates")]);
													QString s = rbs->fullName() + tr(".strength * ") + rna[k]->fullName();
													
													if (sDat2->value(0,0) != s)
													{
														sDat2->value(0,0) = s;
														targetHandles += connections2[l];
														hashStrings += tr("Rates");
														dataTables += sDat2;
													}
													else
														delete sDat2;
												}
											}
											else
											{
												DataTable<QString> * sDat2 = new DataTable<QString>(connections2[l]->data->textData[tr("Rates")]);
												if (sDat2->value(0,0).contains(QRegExp(tr("\\S+\\.strength\\s*\\*\\s*"))))
												{
													sDat2->value(0,0).replace(QRegExp(tr("\\S+\\.strength\\s*\\*\\s*")),tr(""));
													targetHandles += connections2[l];
													hashStrings += tr("Rates");
													dataTables += sDat2;
												}
												else
													delete sDat2;
											}
								}
					}
					
					activators.clear();
					repressors.clear();
					TFconnections.clear();
					rbs = 0;
				}
				else
				if (i == (parts.size() - 1))
				{
					if (!promoter && regulator)
						promoter = regulator;
					if (promoter && promoter->hasTextData(tr("Assignments")) 
						&& promoter->hasNumericalData(tr("Numerical Attributes")) 
						&& promoter->data->numericalData[tr("Numerical Attributes")].getRowNames().contains(tr("strength")))
					{
						DataTable<QString> * sDat = new DataTable<QString>(promoter->data->textData[tr("Assignments")]);
						QString s0 = tr("");
						
						bool missing = false;
						if (sDat->getRowNames().contains(tr("rate")))
						{
							s0 = sDat->value(tr("rate"),0);
							for (int j=0; j < activators.size(); ++j)
								if (activators[j] && !s0.contains(activators[j]->fullName()))
								{
									missing = true;
									break;
								}
							for (int j=0; j < repressors.size(); ++j)
								if (repressors[j] && !s0.contains(repressors[j]->fullName()))
								{
									missing = true;
									break;
								}
						}
						
						if (!sDat->getRowNames().contains(tr("rate")) || missing)
						{
							QString s = hillEquation(TFconnections,activators,repressors);
							
							if (!s.isNull() && !s.isEmpty())
								s = promoter->fullName() + tr(".strength*") + s;
							else
								s = promoter->fullName() + tr(".strength");
							
							if (!sDat->getRowNames().contains(tr("rate")) ||
								 sDat->value(tr("rate"),0) != s)
								{
									sDat->value(tr("rate"),0) = s;
									targetHandles += promoter;
									hashStrings += tr("Assignments");
									dataTables += sDat;
								}
								else
								{
									delete sDat;
								}
						}
						else
						{
							delete sDat;
						}
						
						activators.clear();
						repressors.clear();
						TFconnections.clear();
						rbs = 0;
						promoter = 0;
						regulator = 0;
					}
				}
			}
			
			if (dataTables.size() > 0)
			{
				scene->changeData(tr("gene regulation kinetics changed"),targetHandles,hashStrings,dataTables);
			}
			
			for (int i=0; i < dataTables.size(); ++i)
				if (dataTables[i])
					delete dataTables[i];
	}
	
	QList<QUndoCommand*> AutoGeneRegulatoryTool::insertmRNAstep(const QList<QGraphicsItem*>& selected)
	{
		ItemHandle * handle = 0;
		
		//QList<ItemHandle*> sceneItems = scene->allHandles();
		QList<QGraphicsItem*> list;
		
		ConnectionGraphicsItem * connection;
		QList<ItemHandle*> handles;
		QList<QGraphicsItem*> genes;
		QList<DataTable<qreal>*> nDataNew, nDataOld;
		QList<DataTable<QString>*> sDataNew, sDataOld;
		
		QList<ConnectionGraphicsItem*> connectionItems;
		
		for (int i=0; i < selected.size(); ++i)
		{
			handle = getHandle(selected[i]);
			if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(selected[i])) && 
				!connectionItems.contains(connection) &&
				handle && 
				handle->isA(tr("Transcription")) && !handles.contains(handle) && handle->data &&	
				handle->hasNumericalData(tr("Stoichiometry")) && handle->hasTextData(tr("Rates")))
			{
				connectionItems << connection;
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
			/*else
			{
				selected.removeAt(i);
				--i;
			}*/
		}
		
		QList<QGraphicsItem*> penItems;
		QList<QPen> pens;
		
		for (int i=0; i < connectionItems.size(); ++i)
		{
			penItems << connectionItems[i];
			QPen pen = connectionItems[i]->defaultPen;
			pen.setStyle(Qt::DashLine);
			pens << pen;
		}
		
		QList<QUndoCommand*> commands;
		
		commands << new Change2DataCommand<qreal,QString>
					(tr("mRNA step added"),nDataOld,nDataNew,sDataOld,sDataNew)				 
				 << new ChangePenCommand(tr("change pen style"), penItems, pens);
		
		for (int i=0; i < nDataNew.size(); ++i)
			if (nDataNew[i])
				delete nDataNew[i];
		
		for (int i=0; i < sDataNew.size(); ++i)
			if (sDataNew[i])
				delete sDataNew[i];
		
		return commands;
		
		//scene->changeData(tr("mRNA step added"),handles,nDataOld,nDataNew,sDataOld,sDataNew);
		//scene->setPen(tr("dashed pen style"),penItems,pens);
		//itemsMoved(scene,genes,QList<QPointF>(),0);
	}
	
	void AutoGeneRegulatoryTool::insertmRNAstep()
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !mainWindow || !scene->networkWindow) return;
		
		QList<ItemHandle*> handles = scene->networkWindow->selectedHandles();
	
		QUndoCommand * command = new CompositeCommand(tr("insert mRNA step"), 
													  insertmRNAstep(scene->selected()));
													  
		if (scene->historyStack)
			scene->historyStack->push(command);
		else
		{
			command->redo();
			delete command;
		}
		
		emit dataChanged(handles);
	}
	
	bool AutoGeneRegulatoryTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow != 0)
		{	
			connect(this,SIGNAL(dataChanged(const QList<ItemHandle*>&)),
					mainWindow,SIGNAL(dataChanged(const QList<ItemHandle*>&)));
			connect(this,SIGNAL(itemsInsertedSignal(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
						mainWindow,SIGNAL(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));
			connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
						this,SLOT(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));
			connect(mainWindow,SIGNAL(itemsInserted(NetworkWindow*,const QList<ItemHandle*>&)),
						  this, SLOT(itemsInserted(NetworkWindow*,const QList<ItemHandle*>&)));
                        connect(mainWindow,SIGNAL(itemsAboutToBeRemoved(GraphicsScene*,QList<QGraphicsItem*>&,QList<ItemHandle*>&)),
                                                  this, SLOT(itemsRemoved(GraphicsScene*,QList<QGraphicsItem*>&, QList<ItemHandle*>&)));
			connect(mainWindow,SIGNAL(itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)),
						  this, SLOT(itemsMoved(GraphicsScene*, const QList<QGraphicsItem*>&, const QList<QPointF>&, Qt::KeyboardModifiers)));
			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
						this,SLOT(itemsSelected(GraphicsScene *,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
			connectPlugins();

			connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));
			
			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
		}
		return true;
	}
	
	void AutoGeneRegulatoryTool::toolLoaded(Tool*)
	{
		connectPlugins();
	}

	void AutoGeneRegulatoryTool::connectPlugins()
	{
		static bool alreadyConnected1 = false;
		static bool alreadyConnected2 = false;
		static bool alreadyConnected3 = false;
		
		if ((alreadyConnected1 && alreadyConnected2 && alreadyConnected3) || !mainWindow) return;
		
		if (!alreadyConnected1 && mainWindow->tool(tr("Collision Detection")))
		{
			QWidget * widget = mainWindow->tool(tr("Collision Detection"));
			CollisionDetection * collisionDetection = static_cast<CollisionDetection*>(widget);
			if (collisionDetection)
			{
				alreadyConnected1 = true;
				connect(collisionDetection,SIGNAL(nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& , Qt::KeyboardModifiers )),
						this, SLOT( nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& , Qt::KeyboardModifiers )));
			}
		}
		
		if (!alreadyConnected2 && mainWindow->tool(tr("Basic Graphics Toolbox")))
		{
			QWidget * widget = mainWindow->tool(tr("Basic Graphics Toolbox"));
			BasicGraphicsToolbox * basicToolBox = static_cast<BasicGraphicsToolbox*>(widget);
			if (basicToolBox)
			{
				alreadyConnected2 = true;
				connect(this,SIGNAL(alignCompactHorizontal()),basicToolBox, SLOT(alignCompactHorizontal()));
			}
		}
		
		
		if (!alreadyConnected3 && mainWindow->tool(tr("Connection Selection")))
		{
			QWidget * widget = mainWindow->tool(tr("Connection Selection"));
			ConnectionSelection * connectionSelection = static_cast<ConnectionSelection*>(widget);
			if (connectionSelection)
			{
				alreadyConnected3 = true;
				connect(this,SIGNAL(setMiddleBox(int,const QString&)),connectionSelection, SLOT(showMiddleBox(int,const QString&)));
			}
		}
	}
	
	
	void AutoGeneRegulatoryTool::nodeCollided(const QList<QGraphicsItem*>& items, NodeGraphicsItem * item, const QList<QPointF>&, Qt::KeyboardModifiers )
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !item || items.isEmpty()) return;
		
		bool partCollided = false;
		
		ItemHandle * handle = 0;
		
		for (int i=0; i < items.size(); ++i)
			if ((handle = getHandle(items[i])) && handle->isA(tr("Part")))
			{
				partCollided = true;
				break;
			}
		
		if (!partCollided) return;
		
		QList<QGraphicsItem*> select;
		
		QGraphicsItem * dnaItem = 0;
		
		if ((handle = getHandle(item)) && handle->isA("Part"))
		{
			dnaItem = item;
			select << dnaItem;
		}
		else
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
			
			if (select.isEmpty()) return;
			
			scene->selected() = select;
			scene->select(0);
			
			doAssignment = false;
			emit alignCompactHorizontal();
		}
	}
	
	void AutoGeneRegulatoryTool::itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>& )
	{	
		QGraphicsItem * item = 0;
		for (int i=0; i < items.size(); ++i)
		{
			if (!qgraphicsitem_cast<TextGraphicsItem*>(items[i]))
			{
				if (item)
					return;
				else
					item = items[i];
			}
		}
		
		if (item)
		{
			ItemHandle * handle = getHandle(item);
			if (handle && handle->isA(tr("coding")) && handle->isA(tr("promoter")))
			{
				scene->selected().clear();
				scene->selected() += item;
				autoGeneProductTriggered();
			}
		}
	}

	void AutoGeneRegulatoryTool::itemsInserted(NetworkWindow* scene, const QList<ItemHandle*>& handles0)
	{
		if (!scene) return;
		
		QList<ItemHandle*> parts, upstream;
		QList<NodeHandle*> parts2;
		
		QList<ItemHandle*> handles;
		
		for (int i=0; i < handles0.size(); ++i)
			if (handles0[i])
			{
				if (ConnectionHandle::asConnection(handles0[i]))
				{
					QList<NodeHandle*> nodes = (ConnectionHandle::asConnection(handles0[i]))->nodes();
					for (int j=0; j < nodes.size(); ++j)
						if (nodes[j]->isA(tr("Part")) && !handles.contains(nodes[j]) && !handles0.contains(nodes[j]))
						{
							handles += nodes[j];
						}
					
				}
			}
	
		for (int i=0; i < handles.size(); ++i)
		{
			if (!handles[i]) continue;
			
			NodeGraphicsItem * startNode = 0;
			
			QList<NodeHandle*> parts2;
			
			for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
			{
				startNode = NodeGraphicsItem::topLevelNodeItem(handles[i]->graphicsItems[j]);
				if (startNode)
					break;
			}

			ItemHandle * handle = getHandle(startNode);
			if (!startNode || !handle) continue;
	
			findAllPart(scene->scene,startNode,tr("Part"),parts,false,QStringList() << "Terminator",true);
			findAllPart(scene->scene,startNode,tr("Part"),upstream,true,QStringList() << "Terminator",true);
			
			if (!parts.contains(handle))
				parts.push_front(handle);
				
			while (!upstream.isEmpty())
			{
				parts.push_front(upstream.first());
				upstream.pop_front();
			}
			
			QList<NodeHandle*> parts3;
			
			for (int j=0; j < parts.size(); ++j)
			{
				NodeHandle * node = NodeHandle::asNode(parts[j]);
				if (node && !parts2.contains(node) && !handles0.contains(node))
				{
					parts2 += node;
					parts3 += node;
				}
			}
			
			if (!parts3.isEmpty())
			{
				autoAssignRates(parts3);
			}
		}
	}
	
	void AutoGeneRegulatoryTool::itemsMoved(GraphicsScene* scene, const QList<QGraphicsItem*>& items, const QList<QPointF>& distance, Qt::KeyboardModifiers)
	{
		if (!scene) return;
		
		QList<NodeHandle*> parts2;
		
		for (int i=0; i < items.size(); ++i)
		{
			
			NodeGraphicsItem * startNode = NodeGraphicsItem::topLevelNodeItem(items[i]);
			
			ItemHandle * handle = getHandle(startNode);
			if (!startNode || !handle) continue;
			
			QList<ItemHandle*> parts,upstream;			
			
			findAllPart(scene,startNode,tr("Part"),parts,false,QStringList() << "Terminator",true);
			findAllPart(scene,startNode,tr("Part"),upstream,true,QStringList() << "Terminator",true);
			
			if (!parts.contains(handle))
				parts.push_front(handle);
				
			while (!upstream.isEmpty())
			{
				parts.push_front(upstream.first());
				upstream.pop_front();
			}
			
			QList<NodeHandle*> parts3;
			
			QStringList lst;
			for (int j=0; j < parts.size(); ++j)
			{
				NodeHandle * node = NodeHandle::asNode(parts[j]);
				if (node && !parts2.contains(node))
				{
					parts3 += node;
					parts2 += node;
				}
			}
			if (!parts3.isEmpty())
			{
				autoAssignRates(parts3);
			}
		}
	}
	
    void AutoGeneRegulatoryTool::itemsRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& , QList<ItemHandle*>& handles)
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
                        ConsoleWindow::message(nodes[1]->fullName() + tr(" has rate: ") + s);
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
			scene->changeData(tr("some transcription rates changed"),regulatory,toolNames,newTextData);
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
			if (!containsRegulator && handle && (handle->isA("Gene") || handle->isA("Regulator")))
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
			
			if (containsSpecies)
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
		
		QList<NodeGraphicsItem*> visited;
		
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
		
		qreal ht = node->sceneBoundingRect().height();
		QList<QGraphicsItem*> items = scene->items(QRectF(p.x()-1.0,p.y()-10.0,2.0,ht));
		
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
							&& h->isA(family) && !visited.contains(connectedNodes[0]) 
							&& (!stopIfElongation || !connections[j]->itemHandle->data 
								|| !connections[j]->itemHandle->hasNumericalData(tr("Stoichiometry"))
								|| connections[j]->itemHandle->data->numericalData[tr("Stoichiometry")].rows() < 1)
							)
							{
								visited += connectedNodes[0];
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
							&& h->isA(family) && !visited.contains(connectedNodes[0]) 
							&& (!stopIfElongation || !connections[j]->itemHandle->data 
								|| !connections[j]->itemHandle->hasNumericalData(tr("Stoichiometry"))
								|| connections[j]->itemHandle->data->numericalData[tr("Stoichiometry")].rows() < 1)
							)
							{
								visited += connectedNodes[0];
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
		
		handles << handle;
		
		items.clear();
		
		if (handle->graphicsItems.isEmpty()) return;
		
		qreal y = -1.0;
		NodeGraphicsItem * topmost = 0;
	    for (int i=0; i < handle->graphicsItems.size(); ++i)
			if (NodeGraphicsItem::topLevelNodeItem(handle->graphicsItems[i]) 
				&& ((y < 0) || (y > handle->graphicsItems[i]->scenePos().y())))
			{
				topmost = NodeGraphicsItem::topLevelNodeItem(handle->graphicsItems[i]);
				y = topmost->scenePos().y();
			}
		
		if (!topmost) return;
		items << topmost;
		
		bool hit = false;
		
		for (int i=0; i < items.size(); ++i)
			if ((node = NodeGraphicsItem::topLevelNodeItem(items[i])))
			{
				if (upstream)
					p = QPointF(node->sceneBoundingRect().left() - 10.0,node->scenePos().ry());
				else
					p = QPointF(node->sceneBoundingRect().right() + 10.0,node->scenePos().ry());
				
				ht = node->sceneBoundingRect().height();
				QList<QGraphicsItem*> items2 = scene->items(QRectF(p.x()-1.0,p.y()-10.0,2.0,ht));
				
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
						if (!visited.contains(node))
						{
							visited << node;
							handles << h;
							items << node;
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
									&& h->isA(family) && !visited.contains(connectedNodes[0]))
									{
										visited << connectedNodes[0];
										handles << h;
										items << connectedNodes[0];
										hit = true;
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
									&& h->isA(family) && !visited.contains(connectedNodes[0]))
									{
										visited << connectedNodes[0];
										handles << h;
										items << connectedNodes[0];
										hit = true;
										break;
									}
							}
						}
					}
				}
				if (!hit && (h = node->itemHandle)) //look for other items in the handle
				{
					for (int j=0; j < h->graphicsItems.size(); ++j)
						if ((node = NodeGraphicsItem::topLevelNodeItem(h->graphicsItems[j]))
							&& !visited.contains(node))
						{
							items << node;
							visited << node;
							break;
						}
				}
			}
	}

	QString AutoGeneRegulatoryTool::hillEquation(NodeHandle * handle, ItemHandle * except)
	{
		if (!handle || !(handle->isA("Gene") || handle->isA(tr("Regulator")))) return QString();
		
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
	
	QString AutoGeneRegulatoryTool::hillEquation(QList<ConnectionHandle*> connections, QList<NodeHandle*>& activators, QList<NodeHandle*> repressors)
	{
		if (connections.size() != (activators.size() + repressors.size())) return QString();
		QStringList positives, allTFs;
		
		for (int j=0; j < activators.size(); ++j)
		{
			if (!connections[j]) continue;
			QString cname = connections[j]->fullName();
			
			ItemHandle * nodeHandle = (activators[j]);
			if (nodeHandle)
			{
				QString s0 = nodeHandle->fullName();
				QString s1 = tr("((") + s0 + tr("/") + cname + tr(".Kd)^") + cname + tr(".h)");
				positives << s1;
				allTFs << tr("(1+") + s1 + tr(")");
			}
		}
		
		for (int j=0; j < repressors.size(); ++j)
		{
			if (!connections[j+activators.size()]) continue;
			QString cname = connections[j+activators.size()]->fullName();
			ItemHandle * nodeHandle = (repressors[j]);
			if (nodeHandle)
			{
				QString s0 = nodeHandle->fullName();
				QString s1 = tr("((") + s0 + tr("/") + cname + tr(".Kd)^") + cname + tr(".h)");
				allTFs << tr("(1+") + s1 + tr(")");
			}
		}
	
		if (allTFs.isEmpty()) return tr("");
		
		if (positives.isEmpty()) positives << tr("1.0");
		QString rate = tr("(") + positives.join(" + ") + tr(")/(") + allTFs.join("*") + tr(")");
		return rate;
	}
	
	/*****************************************
	            C API
	******************************************/
	
	typedef void (*tc_GRN_api) (Array (*f1)(OBJ), Array (*f2)(OBJ), Array (*f3)(OBJ), void (*f4)(Array) );
	
	void AutoGeneRegulatoryTool::setupFunctionPointers( QLibrary * library )
	{
		tc_GRN_api f = (tc_GRN_api)library->resolve("tc_AutoGeneRegulatoryTool_api");
		if (f)
		{
			f(
				&(_partsIn),
				&(_partsUpstream),
				&(_partsDownstream),
				&(_alignParts)
			);
		}
	}
	
	void AutoGeneRegulatoryTool::alignParts(QSemaphore * s, const QList<ItemHandle*>& items)
	{
		GraphicsScene * scene = currentScene();
		if (scene)
		{
			scene->selected().clear();
			
			QList<QGraphicsItem*> selected;
			QList<QPointF> points;
			
			QPointF p;
			
			for (int i=0; i < items.size(); ++i)
				if (NodeHandle::asNode(items[i]))
				{
					for (int j=0; j < items[i]->graphicsItems.size(); ++j)
						if (qgraphicsitem_cast<NodeGraphicsItem*>(items[i]->graphicsItems[j]))
						{
							if (selected.isEmpty())
							{
								p = items[i]->graphicsItems[j]->scenePos();
								points << QPointF(0,0);
							}
							else
							{
								points << (p - items[i]->graphicsItems[j]->scenePos());
							}
							
							p.rx() += items[i]->graphicsItems[j]->sceneBoundingRect().width();
							
							selected << items[i]->graphicsItems[j];
						}
				}
			
			scene->move(selected,points);
			scene->select(selected);
			emit alignCompactHorizontal();
		}
		if (s) 
			s->release();
	}
	
	void AutoGeneRegulatoryTool::_alignParts(Array A)
    {
        fToS.alignParts(A);
    }

    void AutoGeneRegulatoryTool_FtoS::alignParts(Array A)
    {
        QList<ItemHandle*> * list = ConvertValue(A);
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit alignParts(s,*list);
		s->acquire();
		s->release();
		delete s;
		delete list;
    }
	
	void AutoGeneRegulatoryTool::partsIn(QSemaphore * s, ItemHandle* h,  QList<ItemHandle*>* parts)
	{
		GraphicsScene * scene = currentScene();
		if (parts && scene && h)
		{
			qreal y = -1.0;
			QList<ItemHandle*> allChildren = h->visibleChildren();
			NodeGraphicsItem * node = 0; 
			NodeGraphicsItem * topmost = 0;
			for (int i=0; i < allChildren.size(); ++i)
				if (NodeHandle::asNode(allChildren[i]) && allChildren[i]->isA(tr("Part")))
					for (int j=0; j < allChildren[i]->graphicsItems.size(); ++j)
						if ((node = NodeGraphicsItem::topLevelNodeItem(allChildren[i]->graphicsItems[j]))
							&& ((y < 0) || (node->scenePos().y() < y)))
						{
							topmost = node;
						}
			
			node = topmost;
			
			if (node)
			{
				QList<ItemHandle*> upstream, downstream;
				findAllPart(scene,node,tr("Part"),upstream,true,QStringList());
				
				downstream.clear();
				if (node && node->itemHandle)
				{
					downstream.push_front(node->itemHandle);
					findAllPart(scene,node,tr("Part"),downstream,false,QStringList());
				}

				while (!upstream.isEmpty())
				{
					downstream.push_front(upstream.first());
					upstream.pop_front();
				}
				
				(*parts) += downstream;
			}
		}
		
		if (s)
			s->release();
	}
	
	Array AutoGeneRegulatoryTool::_partsIn(OBJ o)
    {
        return fToS.partsIn(o);
    }

    Array AutoGeneRegulatoryTool_FtoS::partsIn(OBJ o)
    {
        QSemaphore * s = new QSemaphore(1);
        QList<ItemHandle*>* p = new QList<ItemHandle*>;
        s->acquire();
        emit partsIn(s,ConvertValue(o),p);
        s->acquire();
        s->release();
        delete s;
        Array A = ConvertValue(*p);
        delete p;
        return A;
    }
	
	void AutoGeneRegulatoryTool::partsUpstream(QSemaphore * s, ItemHandle* h, QList<ItemHandle*>* parts)
	{	
		GraphicsScene * scene = currentScene();
		if (parts && scene && h && h->graphicsItems.size() > 0)
		{
			qreal y = -1.0;
			NodeGraphicsItem * node = 0; 
			NodeGraphicsItem * topmost = 0;
			for (int i=0; i < h->graphicsItems.size(); ++i)
				if ((node = NodeGraphicsItem::topLevelNodeItem(h->graphicsItems[i]))
						&& ((y < 0) || (node->scenePos().y() < y)))
					{
						topmost = node;
					}
			
			node = topmost;
			
			if (node)
			{
				QList<ItemHandle*> upstream, downstream;
				findAllPart(scene,node,tr("Part"),upstream,true,QStringList());
				
				downstream.clear();
				if (node && node->itemHandle)
				{
					downstream.push_back(node->itemHandle);
					//findAllPart(scene,node,tr("Part"),downstream,false,QStringList());
				}

				while (!upstream.isEmpty())
				{
					downstream.push_front(upstream.first());
					upstream.pop_front();
				}
				
				(*parts) += downstream;
			}
		}	
		if (s)
			s->release();
	}
	
	Array AutoGeneRegulatoryTool::_partsUpstream(OBJ o)
    {
        return fToS.partsUpstream(o);
    }

    Array AutoGeneRegulatoryTool_FtoS::partsUpstream(OBJ o)
    {
        QSemaphore * s = new QSemaphore(1);
        QList<ItemHandle*>* p = new QList<ItemHandle*>;
        s->acquire();
        emit partsUpstream(s,ConvertValue(o),p);
        s->acquire();
        s->release();
        delete s;
        Array A = ConvertValue(*p);
        delete p;
        return A;
    }
	
	void AutoGeneRegulatoryTool::partsDownstream(QSemaphore * s, ItemHandle* h, QList<ItemHandle*>* parts)
	{
		GraphicsScene * scene = currentScene();
		if (parts && scene && h && h->graphicsItems.size() > 0)
		{
			qreal y = -1.0;
			NodeGraphicsItem * node = 0; 
			NodeGraphicsItem * topmost = 0;
			for (int i=0; i < h->graphicsItems.size(); ++i)
				if ((node = NodeGraphicsItem::topLevelNodeItem(h->graphicsItems[i]))
						&& ((y < 0) || (node->scenePos().y() < y)))
					{
						topmost = node;
					}
			
			node = topmost;
			
			if (node)
			{
				QList<ItemHandle*> downstream;
				//findAllPart(scene,node,tr("Part"),upstream,true,QStringList());
				
				downstream.clear();
				if (node && node->itemHandle)
				{
					//downstream.push_back(node->itemHandle);
					findAllPart(scene,node,tr("Part"),downstream,false,QStringList());
				}
				
				(*parts) += downstream;
			}
		}	
		if (s)
			s->release();
	}
	
	Array AutoGeneRegulatoryTool::_partsDownstream(OBJ o)
    {
        return fToS.partsDownstream(o);
    }

    Array AutoGeneRegulatoryTool_FtoS::partsDownstream(OBJ o)
    {
        QSemaphore * s = new QSemaphore(1);
        QList<ItemHandle*>* p = new QList<ItemHandle*>;
        s->acquire();
        emit partsDownstream(s,ConvertValue(o),p);
        s->acquire();
        s->release();
        delete s;
        Array A = ConvertValue(*p);
        delete p;
        return A;
    }

}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::AutoGeneRegulatoryTool * tool = new Tinkercell::AutoGeneRegulatoryTool;
	main->addTool(tool);
}
