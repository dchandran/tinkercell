/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 Automatically manage gene regulatory network rates and parameters

****************************************************************************/

#include <QDebug>
#include "UndoCommands.h"
#include "ItemHandle.h"
#include "CThread.h"
#include "GraphicsScene.h"
#include "NetworkHandle.h"
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
		Tool(tr("Auto Gene Regulatory Tool"),tr("Modeling")),
		autoTFUp("Insert activator",this),
		autoTFDown("Insert repressor",this),
		autoDegradation("Insert degadation reaction",this),
		autoGeneProduct("Insert gene product",this),
		mRNAstep("Add mRNA stage",this),
		autoPhosphate("Insert phosphate",this)
	{
		separator = 0;
		autoAlignEnabled = true;
		setPalette(QPalette(QColor(255,255,255,255)));
		setAutoFillBackground(true);
		connect(&autoTFUp,SIGNAL(triggered()),this,SLOT(autoTFTriggeredUp()));
		connect(&autoTFDown,SIGNAL(triggered()),this,SLOT(autoTFTriggeredDown()));
		connect(&autoDegradation,SIGNAL(triggered()),this,SLOT(autoDegradationTriggered()));
		autoDegradation.setIcon(QIcon(":/images/empty.png"));
		connect(&autoGeneProduct,SIGNAL(triggered()),this,SLOT(autoGeneProductTriggered()));
		connect(&autoPhosphate,SIGNAL(triggered()),this,SLOT(autoPhosphateTriggered()));
		autoPhosphate.setIcon(QIcon(":/images/phosphate.png"));
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
			if (qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]) && handle && handle->isA("Molecule") && !handlesPhos.contains(handle))
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
					image = (NodeGraphicsItem::cast(nodeFamily->graphicsItems[j]));
					if (image)
					{
						image = image->clone();

						for (int k=0; k < image->boundaryControlPoints.size(); ++k)
							delete image->boundaryControlPoints[k];

						image->boundaryControlPoints.clear();

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
		
		QStringList usedNames;
		QList<QGraphicsItem*> list;

		QString appDir = QCoreApplication::applicationDirPath();

		QList<ItemHandle*> visited;

		QList<NodeGraphicsItem*> regulatorNodes;
		QList<ItemHandle*> regulatorHandles;
		QList< DataTable<QString>* > newTextData;

		for (int i=0; i < selected.size(); ++i)
		{
			handle = getHandle(selected[i]);
			if (qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]) && handle && handle->isA("Promoter") && !visited.contains(handle))
			{
				regulatorNodes += qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]);
				visited += handle;
				NodeHandle * node = new NodeHandle(nodeFamily);
				node->name = tr("tf");
				node->name = scene->network->makeUnique(node->name,usedNames);
				usedNames << node->name;

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

					item->curveSegments +=
						ConnectionGraphicsItem::CurveSegment(1,new ConnectionGraphicsItem::ControlPoint(item,image));

					item->curveSegments +=
						ConnectionGraphicsItem::CurveSegment(1,new ConnectionGraphicsItem::ControlPoint(item,selected[i]));

					ArrowHeadItem * arrow = 0;
					if (connectionFamily->graphicsItems.size() > 0)
					{
						NodeGraphicsItem * node = NodeGraphicsItem::cast(connectionFamily->graphicsItems[0]);
						if (node && node->className == ArrowHeadItem::CLASSNAME)
						{
							arrow = static_cast<ArrowHeadItem*>(node->clone());
							arrow->connectionItem = item;
							if (arrow->defaultSize.width() > 0 && arrow->defaultSize.height() > 0)
								arrow->scale(arrow->defaultSize.width()/arrow->sceneBoundingRect().width(),arrow->defaultSize.height()/arrow->sceneBoundingRect().height());
						}
					}

					item->curveSegments.last().arrowStart = arrow;
					list += arrow;

					QStringList words = regulationName.split(tr(" "));
					for (int k=0; k < words.size(); ++k)
						connection->name += words[k].toLower()[0];
					item->lineType = ConnectionGraphicsItem::line;
					connection->name = scene->network->makeUnique(connection->name,usedNames);
                    usedNames << node->name << connection->name;

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

		treeWidget = mainWindow->tool(tr("Connections Tree"));
		ConnectionsTree * connectionsTree = static_cast<ConnectionsTree*>(treeWidget);

		if (!nodesTree->nodeFamilies.contains("Protein") ||
			!connectionsTree->connectionFamilies.contains("Protein production")
			)
			return;

		NodeFamily * proteinFamily = nodesTree->nodeFamilies["Protein"];
		ConnectionFamily * productionFamily = connectionsTree->connectionFamilies["Protein production"];

		QList<QGraphicsItem*>& selected = scene->selected();
		ItemHandle * handle = 0;

		QStringList usedNames;
		QList<QGraphicsItem*> list;

		QString appDir = QCoreApplication::applicationDirPath();

		QList<ItemHandle*> visited;

		for (int i=0; i < selected.size(); ++i)
		{
			handle = getHandle(selected[i]);
			if (qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]) && handle && handle->isA("Coding") && !visited.contains(handle))
			{
				visited += handle;
				NodeHandle * proteinNode = new NodeHandle(proteinFamily);
				proteinNode->name = tr("P1");
				proteinNode->name = scene->network->makeUnique(proteinNode->name,usedNames);
				usedNames << proteinNode->name;

				qreal xpos = (selected[i]->sceneBoundingRect().right() + selected[i]->scenePos().x())/2.0,
					  ypos = selected[i]->scenePos().ry() - (selected[i]->sceneBoundingRect().height() * 2),
					  height = 0.0;

				for (int j=0; j < 100; ++j)
				{
					if (scene->items(QPointF(xpos,ypos)).isEmpty())
						break;

					ypos -= (selected[i]->sceneBoundingRect().height() * 1.0);
				}

				NodeGraphicsItem * proteinItem = 0;

				for (int j=0; j < proteinFamily->graphicsItems.size(); ++j)
				{
					proteinItem = (NodeGraphicsItem::topLevelNodeItem(proteinFamily->graphicsItems[j]));
					if (proteinItem)
					{
						proteinItem = proteinItem->clone();

						if (proteinItem->defaultSize.width() > 0 && proteinItem->defaultSize.height() > 0)
							proteinItem->scale(proteinItem->defaultSize.width()/proteinItem->sceneBoundingRect().width(),proteinItem->defaultSize.height()/proteinItem->sceneBoundingRect().height());

						qreal w = proteinItem->sceneBoundingRect().width();

						proteinItem->setPos(xpos, ypos - 4.0*proteinItem->sceneBoundingRect().height());

						proteinItem->setBoundingBoxVisible(false);

						if (proteinItem->isValid())
						{
							xpos += w;
							setHandle(proteinItem,proteinNode);
							list += proteinItem;
						}

						if (proteinItem->sceneBoundingRect().height() > height)
							height = proteinItem->sceneBoundingRect().height();
					}
				}

				if (proteinItem)
				{	
					TextGraphicsItem * nameItem;
					QFont font;

					ConnectionGraphicsItem * item = new ConnectionGraphicsItem;
					ConnectionHandle * connection = new ConnectionHandle(productionFamily,item);
					QPointF p = 0.5*(selected[i]->scenePos() + proteinItem->scenePos());
					
					ConnectionGraphicsItem::CurveSegment curveSegment;
					curveSegment += new ConnectionGraphicsItem::ControlPoint(item,selected[i]);
					curveSegment += new ConnectionGraphicsItem::ControlPoint(p,item);
					curveSegment += new ConnectionGraphicsItem::ControlPoint(p,item);
					curveSegment += new ConnectionGraphicsItem::ControlPoint(item,proteinItem);
					item->curveSegments += curveSegment;

					list += item;

					ArrowHeadItem * arrow = 0;
					QString nodeImageFile = appDir + tr("/ArrowItems/Production.xml");
					arrow = new ArrowHeadItem(nodeImageFile, item);
					item->curveSegments.last().arrowEnd = arrow;
					list += arrow;

					connection->name = tr("J1");
					item->lineType = ConnectionGraphicsItem::line;
					connection->name = scene->network->makeUnique(connection->name,usedNames);
                    usedNames << connection->name;

					nameItem = new TextGraphicsItem(connection,0);
					list += nameItem;
					nameItem->setPos( 0.5*(proteinItem->pos() + selected[i]->scenePos() ) );
					font = nameItem->font();
					font.setPointSize(22);
					nameItem->setFont(font);
					
					nodeImageFile = appDir + tr("/DecoratorItems/UpCircle.xml");
					ArrowHeadItem * middleItem = new ArrowHeadItem(nodeImageFile,item);
					item->centerRegionItem = middleItem;
					list += middleItem;
					
					nameItem = new TextGraphicsItem(proteinNode,0);
					font = nameItem->font();
					font.setPointSize(22);
					nameItem->setFont(font);
					nameItem->setPos(proteinItem->sceneBoundingRect().right() + 10.0, proteinItem->scenePos().y());
					list += nameItem;
				}
			}
		}

		if (!list.isEmpty())
		{
			scene->insert(tr("Gene product added"),list);
			scene->selected() += list;
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

		if (!connectionsTree->connectionFamilies.contains("Degradation")) return;

		NodeFamily * nodeFamily = nodesTree->nodeFamilies["Empty"];
		ConnectionFamily * connectionFamily = connectionsTree->connectionFamilies["Degradation"];

		QList<QGraphicsItem*>& selected = scene->selected();
		ItemHandle * handle = 0;

		QStringList usedNames;
		QList<QGraphicsItem*> list;

		QString appDir = QCoreApplication::applicationDirPath();

		QList<ItemHandle*> visited;

		for (int i=0; i < selected.size(); ++i)
		{
			handle = getHandle(selected[i]);
			if (qgraphicsitem_cast<NodeGraphicsItem*>(selected[i]) && handle && handle->isA("Molecule") && !visited.contains(handle))
			{
				visited += handle;
				NodeHandle * node = new NodeHandle(nodeFamily);
				node->name = tr("emp");
				node->name = scene->network->makeUnique(node->name,usedNames);
				usedNames << node->name;

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

					item->curveSegments +=
						ConnectionGraphicsItem::CurveSegment(1,new ConnectionGraphicsItem::ControlPoint(item,selected[i]));

					item->curveSegments +=
						ConnectionGraphicsItem::CurveSegment(1,new ConnectionGraphicsItem::ControlPoint(item,image));

					ArrowHeadItem * arrow = 0;
					if (!connectionFamily->graphicsItems.isEmpty())
					{
						NodeGraphicsItem * node = NodeGraphicsItem::cast(connectionFamily->graphicsItems.first());
						if (node && node->className == ArrowHeadItem::CLASSNAME)
						{
							arrow = new ArrowHeadItem(*static_cast<ArrowHeadItem*>(node));
							if (arrow->defaultSize.width() > 0 && arrow->defaultSize.height() > 0)
								arrow->scale(arrow->defaultSize.width()/arrow->sceneBoundingRect().width(),arrow->defaultSize.height()/arrow->sceneBoundingRect().height());
							arrow->connectionItem = item;
							list += arrow;
						}
					}
					
					item->curveSegments.last().arrowStart = arrow;

					connection->name = tr("deg");
					item->lineType = ConnectionGraphicsItem::line;
					connection->name = scene->network->makeUnique(connection->name,usedNames);
                    usedNames << node->name << connection->name;

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
			scene->insert(tr("Added degradation"),list);
		}
	}

	QUndoCommand * AutoGeneRegulatoryTool::autoAssignRates(QList<NodeHandle*>& parts)
	{
		QUndoCommand * command = 0;
		GraphicsScene * scene = currentScene();
		if (!scene || !mainWindow || parts.isEmpty()) return command;

		ItemHandle * rbs = 0, * promoter = 0;
		QString rate, s0;

		QList<ItemHandle*> targetHandles;
		QList<QString> hashStrings;
		QList<DataTable<QString>*> oldDataTables, newDataTables;
		
		QList<ConnectionHandle*> regulations, connections;
		QList<NodeHandle*> activators, repressors, nodes;

		for (int i=0; i < parts.size(); ++i)
			if (parts[i])
			{
				connections = parts[i]->connections();
				for (int j=0; j < connections.size(); ++j)
					if (connections[j] && connections[j]->isA("Regulation"))
					{
						bool b = connections[j]->isA("Repression");
						nodes = connections[j]->nodes();
						for (int k=0; k < nodes.size(); ++k)
							if (nodes[k] != parts[i])
							{
								regulations += connections[j];
								if (b)
									repressors += nodes[k];
								else
									activators += nodes[k];
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

				if (parts[i]->isA(tr("Coding")) && NodeHandle::cast(parts[i]))
				{
					QList<ConnectionHandle*> connections = NodeHandle::cast(parts[i])->connections();
					for (int j=0; j < connections.size(); ++j)
						if (connections[j] &&
							connections[j]->isA(tr("Production")) &&
							connections[j]->hasTextData(tr("Rate equations")))
					{
						DataTable<QString> * sDat = new DataTable<QString>(connections[j]->data->textData[tr("Rate equations")]);
						s0 = sDat->value(0,0);
						
						if (promoter)
						{
							s0.remove(tr(" "));
							bool ok1 = (s0 == (promoter->fullName() + tr(".strength*") + promoter->fullName()));
							bool ok2 = true;
							
							for (int k=0; k < activators.size(); ++k)
								if (!s0.contains(activators[k]->fullName()))
								{
									ok2 = false;
									break;
								}
							
							if (ok2)
								for (int k=0; k < repressors.size(); ++k)
									if (!s0.contains(repressors[k]->fullName()))
									{
										ok2 = false;
										break;
									}
							
							if (!ok1 && !ok2)
							{
								rate = tr("");
							
								if (promoter->hasNumericalData(tr("Parameters"))
									&& promoter->data->numericalData[tr("Parameters")].getRowNames().contains(tr("strength")))
									rate = promoter->fullName() + tr(".strength*") + rate;
								
								//rate += promoter->fullName();
								rate += hillEquation(regulations, activators, repressors);
								sDat->value(0,0) = rate;
								oldDataTables += &(connections[j]->data->textData[tr("Rate equations")]);
								newDataTables += sDat;
							}


							if (sDat->getRowNames().contains(tr("translation")))
							{
								QString s = sDat->value(tr("translation"),0);

								if (rbs && !s.contains(rbs->fullName()))
								{
									s = rbs->fullName() + tr(".strength * ") + sDat->value(tr("translation"),0);
									sDat->value(tr("translation"),0) = s;
									if (!newDataTables.contains(sDat))
									{
										oldDataTables += &(connections[j]->data->textData[tr("Rate equations")]);
										newDataTables += sDat;
									}
								}
							}
						

							QList<NodeHandle*> rna = connections[j]->nodesOut();
							for (int k=0; k < rna.size(); ++k)
								if (NodeHandle::cast(rna[k]) && rna[k]->isA(tr("RNA")))
								{
									QList<ConnectionHandle*> connections2 = NodeHandle::cast(rna[k])->connections();
									for (int l=0; l < connections2.size(); ++l)
										if (connections2[l] &&
											connections2[l]->isA(tr("Translation")) &&
											connections2[l]->hasTextData(tr("Rate equations")))
											if (rbs)
											{
													DataTable<QString> * sDat2 = new DataTable<QString>(connections2[l]->data->textData[tr("Rate equations")]);
													QString s = rbs->fullName() + tr(".strength * ") + rna[k]->fullName();

													if (!sDat2->value(0,0).contains(rbs->fullName()))
													{
														sDat2->value(0,0) = s;
														oldDataTables += &(connections[j]->data->textData[tr("Rate equations")]);
														newDataTables += sDat2;
													}
													else
														delete sDat2;
											}
								}
						}
					
					rbs = 0;
				}
				
				if ((parts[i]->isA(tr("Terminator")) || parts[i]->isA(tr("Vector")) ) && NodeHandle::cast(parts[i]))
				{
					promoter = 0;
					rbs = 0;
				}
			}
		}

		if (newDataTables.size() > 0)
		{
			command = new ChangeTextDataCommand(tr("gene regulation kinetics changed"),oldDataTables,newDataTables);

			for (int i=0; i < newDataTables.size(); ++i)
				if (newDataTables[i])
					delete newDataTables[i];
		}
		
		return command;
	}

	bool AutoGeneRegulatoryTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow != 0)
		{
			connect(this,SIGNAL(dataChanged(const QList<ItemHandle*>&)),
					mainWindow,SIGNAL(dataChanged(const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(itemsAboutToBeInserted(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)),
					this,SLOT(itemsAboutToBeInserted(GraphicsScene *, QList<QGraphicsItem*>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)));

			connect(mainWindow,SIGNAL(itemsAboutToBeRemoved(GraphicsScene*,QList<QGraphicsItem*>&,QList<ItemHandle*>&,QList<QUndoCommand*>&)),
                          this, SLOT(itemsRemoved(GraphicsScene*,QList<QGraphicsItem*>&, QList<ItemHandle*>&,QList<QUndoCommand*>&)));

			connect(mainWindow,SIGNAL(itemsAboutToBeMoved(GraphicsScene*, QList<QGraphicsItem*>&, QList<QPointF>&, QList<QUndoCommand*>&)),
						  this, SLOT(itemsMoved(GraphicsScene*, QList<QGraphicsItem*>&, QList<QPointF>&, QList<QUndoCommand*>&)));

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
				connect(collisionDetection,SIGNAL(nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>&)),
						this, SLOT( nodeCollided(const QList<QGraphicsItem*>& , NodeGraphicsItem * , const QList<QPointF>& )));
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

	void AutoGeneRegulatoryTool::nodeCollided(const QList<QGraphicsItem*>& items, NodeGraphicsItem * item, const QList<QPointF>& )
	{
		if (!autoAlignEnabled) return;

		GraphicsScene * scene = currentScene();
		if (!scene || !item || items.isEmpty()) return;

		ItemHandle * handle = item->handle();

		if (handle && handle->isA(tr("Vector")))
		{
			scene->network->push(adjustPlasmid(scene,item));
			return;
		}

		bool partCollided = false;

		for (int i=0; i < items.size(); ++i)
			if ((handle = getHandle(items[i])) && handle->isA(tr("Part")))
			{
				partCollided = true;
				break;
			}

		if (!partCollided) return;

		QList<QGraphicsItem*> select;

		QGraphicsItem * dnaItem = 0;

		if ((handle = getHandle(item)) && handle->isA("Part") && !handle->isA("Vector"))
		{
			dnaItem = item;
			select << dnaItem;
		}
		else
			for (int i=0; i < items.size(); ++i)
				if (qgraphicsitem_cast<NodeGraphicsItem*>(items[i]) && (handle = getHandle(items[i])) && handle->isA("Part") && !handle->isA("Vector"))
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
					if (!select.contains(items[i]) && qgraphicsitem_cast<NodeGraphicsItem*>(items[i]) && (handle = getHandle(items[i])) && handle->isA("Part") && !handle->isA("Vector"))
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
					if (!select.contains(items[i]) && qgraphicsitem_cast<NodeGraphicsItem*>(items[i]) && (handle = getHandle(items[i])) && handle->isA("Part") && !handle->isA("Vector"))
					{
						itemRight = items[i];
						select << itemRight;
						break;
					}
			}

			if (select.isEmpty()) return;

			scene->selected() = select;
			scene->select(0);

			autoAlignEnabled = false;
			emit alignCompactHorizontal();
			autoAlignEnabled = true;
		}
	}

	void AutoGeneRegulatoryTool::itemsAboutToBeInserted(GraphicsScene * scene, QList<QGraphicsItem*>& items, QList<ItemHandle*>& handle, QList<QUndoCommand*>& commands)
	{
		QGraphicsItem * item = 0;
		NodeGraphicsItem * node = 0;
		for (int i=0; i < items.size(); ++i)
		{
			if ((node = NodeGraphicsItem::cast(items[i]))
				&& node->boundaryControlPoints.size() > 0
				&& (node->name.toLower() == tr("phosphate")))
			{
				for (int k=0; k < node->boundaryControlPoints.size(); ++k)
					if (node->boundaryControlPoints[k])
					{
						if (node->boundaryControlPoints[k]->scene())
							node->boundaryControlPoints[k]->scene()->removeItem(node->boundaryControlPoints[k]);
						delete node->boundaryControlPoints[k];
					}

				node->boundaryControlPoints.clear();
			}

			if (!qgraphicsitem_cast<TextGraphicsItem*>(items[i]))
			{
				if (item)
					return;
				else
					item = items[i];
			}
		}
	}
	void AutoGeneRegulatoryTool::itemsMoved(GraphicsScene* scene, QList<QGraphicsItem*>& items, QList<QPointF>& distance, QList<QUndoCommand*>& commands)
	{
		if (!scene || !autoAlignEnabled) return;

		QList<NodeHandle*> parts2;

		QList<ItemHandle*> connections;
		NodeHandle * handle = 0;
		QString rate;
		
		for (int i=0; i < items.size(); ++i)
		{
			handle = NodeHandle::cast( getHandle(items[i]) );
			if (NodeGraphicsItem::cast(items[i]) && handle && (handle->isA(tr("Promoter")) || handle->isA(tr("RBS"))))
			{
				connections = scene->network->symbolsTable.handlesByFamily.values(tr("Transcription"));
				connections += scene->network->symbolsTable.handlesByFamily.values(tr("Translation"));
				connections += scene->network->symbolsTable.handlesByFamily.values(tr("Protein production"));
				for (int j=0; j < connections.size(); ++j)
				{
					if (connections[j] && connections[j]->hasTextData(tr("Rate equations")))
					{
						rate = connections[j]->textData(tr("Rate equations")); 
						if (rate.contains(handle->fullName()) && !rate.contains(tr(".") + handle->fullName()))
						{
							bool intersects = false, beforeIntersected = false;
							ConnectionGraphicsItem * connection = 0;
							QList<NodeGraphicsItem*> nodes0, nodes;
							QList<ConnectionGraphicsItem*> nodeConnections0, nodeConnections;
							for (int k=0; k < connections[j]->graphicsItems.size(); ++k)
							{
								if ((connection = ConnectionGraphicsItem::cast(connections[j]->graphicsItems[k]))
									&& !items.contains(connection))
									nodeConnections << connection;
							}

							for (int k=0; k < nodeConnections.size(); ++k)
							{
								nodes0 = nodeConnections[k]->nodesWithoutArrows();
								nodes << nodes0;
								for (int l=0; l < nodes0.size(); ++l)
								{
									nodeConnections0 = nodes0[l]->connectionsWithArrows();
									for (int m=0; m < nodeConnections0.size(); ++m)
										if (!nodeConnections.contains(nodeConnections0[m]))
											nodeConnections << nodeConnections0[m];
								}
							}

							QRectF rect1,rect2;
							QPointF pos = items[i]->scenePos();

							if (distance.size() > i)
								pos = pos - distance[i];
							else
								if (!distance.isEmpty())
									pos = pos - distance[0];

							for (int k=0; k < nodes.size(); ++k)
								if (nodes[k])
								{
									rect1 = items[i]->sceneBoundingRect();
									rect2 = nodes[k]->sceneBoundingRect();

									if (items.contains(nodes[k]) ||
										rect1.adjusted(-10.0,-10.0,10.0,10.0).intersects(rect2))
										intersects = true;

									rect1 = QRectF(pos.rx() - rect1.width()/2, pos.ry() - rect1.height()/2, pos.rx() + rect1.width()/2, pos.ry() + rect1.height()/2);
									if (rect1.intersects(rect2))
										beforeIntersected = true;

									if (intersects && beforeIntersected)
										break;
								}

							if (!intersects && beforeIntersected)
							{
								DataTable<QString> newRates(connections[j]->data->textData[tr("Rate equations")]);
								newRates.value(0,0) = tr("0.0");
								QString s = connections[j]->fullName() + tr(" rate = 0.0");
								
								commands << new ChangeTextDataCommand(s,&(connections[j]->textDataTable(tr("Rate equations"))),&newRates);
							}
						}
					}
				}
			}
		}

/*
		ItemHandle * h = 0;
		
		for (int i=0; i < items.size(); ++i)
		{
			h = getHandle(items[i]);

			if (h && h->isA(tr("Vector")))
			{
				for (int j=0; j < h->graphicsItems.size(); ++j)
					commands << adjustPlasmid(scene,NodeGraphicsItem::cast(h->graphicsItems[j]),false);
			}
			else
			if (h && (h = h->parentOfFamily(tr("Vector"))))
			{
				for (int j=0; j < h->graphicsItems.size(); ++j)
					commands << adjustPlasmid(scene,NodeGraphicsItem::cast(h->graphicsItems[j]),false);
			}
		}
*/
		for (int i=0; i < items.size(); ++i)
		{
			NodeGraphicsItem * startNode = NodeGraphicsItem::topLevelNodeItem(items[i]);

			if (!startNode) continue;

			handle = NodeHandle::cast(getHandle(startNode));
			if (parts2.contains(handle) || !handle) continue;

			QList<ItemHandle*> parts,upstream;

			findAllParts(scene,startNode,tr("Part"),parts,false,QStringList() << "Terminator" << "Vector",true);
			findAllParts(scene,startNode,tr("Part"),upstream,true,QStringList() << "Terminator" << "Vector",true);

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
				NodeHandle * node = NodeHandle::cast(parts[j]);
				parts3 += node;
				parts2 += node;
			}
			if (!parts3.isEmpty())
			{
				commands << autoAssignRates(parts3);
			}
			
		}
	}

    void AutoGeneRegulatoryTool::itemsRemoved(GraphicsScene * scene, QList<QGraphicsItem*>& , QList<ItemHandle*>& handles, QList<QUndoCommand*>& commands)
	{
		if (!scene) return;

		QList<ItemHandle*> regulatory;
		QStringList toolNames;
		QList< DataTable<QString>* > newTextData;

		QList<ItemHandle*> visited;
		ConnectionHandle * connection;

		for (int i=0; i < handles.size(); ++i)
		{
            if ((connection = ConnectionHandle::cast(handles[i]))
				&& !visited.contains(handles[i])
				&& handles[i]->isA("Gene Regulation"))
			{
				visited << handles[i];
				QList<ItemHandle*> parts, upstream;

				QList<NodeGraphicsItem*> nodes;
				QList<NodeHandle*> nodeHandles = connection->nodesOut();
				NodeGraphicsItem * startNode = 0;

				for (int j=0; j < nodeHandles.size(); ++j)
				{
					if (nodeHandles[j])
						for (int k=0; k < nodeHandles[j]->graphicsItems.size(); ++k)
						{
							startNode = NodeGraphicsItem::cast(nodeHandles[j]->graphicsItems[k]);
							if (startNode)
								break;
						}
					if (startNode)
						break;
				}

				ItemHandle * handle = getHandle(startNode);
				if (!startNode || !handle) continue;

				findAllParts(scene,startNode,tr("Part"),parts,false,QStringList() << "Terminator" << "Vector",true);
				findAllParts(scene,startNode,tr("Part"),upstream,true,QStringList() << "Terminator" << "Vector",true);

				if (!parts.contains(handle))
					parts.push_front(handle);

				while (!upstream.isEmpty())
				{
					parts.push_front(upstream.first());
					upstream.pop_front();
				}

				nodeHandles.clear();
				NodeHandle * nodeHandle = 0;

				for (int j=0; j < parts.size(); ++j)
					if ((nodeHandle = NodeHandle::cast(parts[j])) && !nodeHandles.contains(nodeHandle))
						nodeHandles << nodeHandle;

				if (!nodeHandles.isEmpty())
				{
					commands << autoAssignRates(nodeHandles);
				}
			}
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
		for (int i=0; i < items.size(); ++i)
		{
			handle = getHandle(items[i]);
			if (!containsSpecies && handle && handle->isA("Molecule"))
				containsSpecies = true;
			if (!containsProteins && handle && handle->isA("Protein"))
				containsProteins = true;
			if (!containsRegulator && handle && handle->isA("Promoter"))
				containsRegulator = true;
			if (!containsCoding && handle && handle->isA("Coding"))
				containsCoding = true;

			if (containsRegulator && containsSpecies && containsCoding) break;
		}

		if (containsSpecies || containsRegulator || containsCoding)
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

		}
		else
		{
			if (separator)
				mainWindow->contextItemsMenu.removeAction(separator);
			mainWindow->contextItemsMenu.removeAction(&autoDegradation);
			mainWindow->contextItemsMenu.removeAction(&autoTFUp);
			mainWindow->contextItemsMenu.removeAction(&autoTFDown);
			mainWindow->contextItemsMenu.removeAction(&autoGeneProduct);
		}
	}

	void AutoGeneRegulatoryTool::findAllParts(GraphicsScene* scene,NodeGraphicsItem * node, const QString& family,QList<ItemHandle*>& handles,bool upstream,const QStringList & until, bool stopIfElongation)
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
			if (handle && handle->isA(family) && handle->children.isEmpty() &&
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
					if (connections[j] && connections[j]->handle() && connections[j]->handle()->isA(tr("PoPS")))
					{
						QList<NodeGraphicsItem*> connectedNodes = connections[j]->nodesWithoutArrows();
						if (connectedNodes.size() > 0 && connectedNodes[0] && (h = connectedNodes[0]->handle())
							&& h->isA(family) && h->children.isEmpty() && !visited.contains(connectedNodes[0])
							&& (!stopIfElongation || !connections[j]->handle()->data
								|| !connections[j]->handle()->hasNumericalData(tr("Product stoichiometries"))
								|| connections[j]->handle()->data->numericalData[tr("Product stoichiometries")].rows() < 1)
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
					if (connections[j] && connections[j]->handle() && connections[j]->handle()->isA(tr("PoPS")))
					{
						QList<NodeGraphicsItem*> connectedNodes = connections[j]->nodesWithArrows();
						if (connectedNodes.size() > 0 && connectedNodes[0] && (h = connectedNodes[0]->handle())
							&& h->isA(family) && h->children.isEmpty() && !visited.contains(connectedNodes[0])
							&& (!stopIfElongation || !connections[j]->handle()->data
								|| !connections[j]->handle()->hasNumericalData(tr("Product stoichiometries"))
								|| connections[j]->handle()->data->numericalData[tr("Product stoichiometries")].rows() < 1)
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
						h = node->handle();

						if (!h || !h->isA(family) || !h->children.isEmpty()) continue;

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
							if (connections[j] && connections[j]->handle() && connections[j]->handle()->isA(tr("PoPS")))
							{
								QList<NodeGraphicsItem*> connectedNodes = connections[j]->nodesWithoutArrows();
								if (connectedNodes.size() > 0 && connectedNodes[0] && (h = connectedNodes[0]->handle())
									&& h->isA(family) && h->children.isEmpty() &&!visited.contains(connectedNodes[0]))
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
							if (connections[j] && connections[j]->handle() && connections[j]->handle()->isA(tr("PoPS")))
							{
								QList<NodeGraphicsItem*> connectedNodes = connections[j]->nodesWithArrows();
								if (connectedNodes.size() > 0 && connectedNodes[0] && (h = connectedNodes[0]->handle())
									&& h->isA(family) && h->children.isEmpty() && !visited.contains(connectedNodes[0]))
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
				if (!hit && (h = node->handle())) //look for other items in the handle
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
		if (!handle || !handle->isA(tr("Promoter"))) return QString();

		QList<ConnectionHandle*> connections = handle->connections();

		QStringList positives, allTFs;

		for (int i=0; i < connections.size(); ++i)
		if (connections[i])
			{
				if (!connections[i]->isA("Regulation") || connections[i] == except)
					continue;
                QString cname = connections[i]->fullName();
                bool isPositive = !(connections[i]->isA("Repression"));
                QList<NodeHandle*> nodesIn = connections[i]->nodesIn();
				for (int j=0; j < nodesIn.size(); ++j)
				{
					ItemHandle * nodeHandle = (nodesIn[j]);
					if (nodeHandle)
					{
						QString s0 = nodeHandle->fullName();
						QString s1 = tr("(1+((") + s0 + tr("/") + cname + tr(".Kd)^") + cname + tr(".h))");
						if (isPositive)
							positives << s1;

						allTFs << s1;
					}
				}
			}
		if (allTFs.isEmpty()) return tr("");

		QString rate;
		if (positives.isEmpty()) 
			rate = handle->fullName() + tr(".strength)/(") + allTFs.join("*") + tr(")");
		else
			rate = handle->fullName() + tr(".strength*(") + positives.join(" * ") + tr(" - 1)/(") + allTFs.join("*") + tr(")");
		return rate;
	}

	QString AutoGeneRegulatoryTool::hillEquation(QList<ConnectionHandle*>& connections, QList<NodeHandle*>& activators, QList<NodeHandle*> repressors)
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
				QString s1 = tr("(1 + ((") + s0 + tr("/") + cname + tr(".Kd)^") + cname + tr(".h))");
				positives << s1;
				allTFs << s1;
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
				QString s1 = tr("(1+ ((") + s0 + tr("/") + cname + tr(".Kd)^") + cname + tr(".h))");
				allTFs << s1;
			}
		}

		if (allTFs.isEmpty()) return tr("");

		QString rate;
		if (positives.isEmpty()) 
			rate =tr("1.0/(") + allTFs.join("*") + tr(")");
		else
			rate = tr("(") + positives.join(" * ") + tr(" - 1)/(") + allTFs.join("*") + tr(")");
		return rate;
	}

	QUndoCommand * AutoGeneRegulatoryTool::adjustPlasmid(GraphicsScene * scene, NodeGraphicsItem * vector, bool align)
	{
		QList<QUndoCommand*> commands;
		if (!vector || !scene) return new CompositeCommand(tr("plasmid adjusted"),commands);

		ItemHandle * vectorHandle = vector->handle();
		ItemHandle * handle = 0;

		QList<ItemHandle*> children, parents;
		QList<QGraphicsItem*> intersectingItems = scene->items(vector->sceneBoundingRect());

		for (int i=0; i < intersectingItems.size(); ++i)
		{
			handle = getHandle(intersectingItems[i]);
			if (handle && !handle->isChildOf(vectorHandle) &&
				handle->isA(tr("Part")) && !handle->isA(tr("Vector")) &&
				(!handle->parent || vectorHandle->isChildOf(handle->parent)))
			{
				children << handle;
				parents << vectorHandle;
			}
		}

		QList<QGraphicsItem*> list;
		QList<ItemHandle*> existingChildren = vectorHandle->children;
		QList<ItemHandle*> trueChildren = children + existingChildren;
		
		for (int i=0; i < existingChildren.size(); ++i)
			if (existingChildren[i])
			{
				list = existingChildren[i]->graphicsItems;
				for (int j=0; j < list.size(); ++j)
					if (NodeGraphicsItem::cast(list[j]) && !intersectingItems.contains(list[j]))
					{
						children << existingChildren[i];
						parents << 0;
						trueChildren.removeAll(existingChildren[i]);
						break;
					}
			}

		if (!children.isEmpty())
			commands << new SetParentHandleCommand(tr("parents set"), scene->network, children, parents);

		QList<QGraphicsItem*> nodesInPlasmid;

		for (int i=0; i < trueChildren.size(); ++i)
			if (trueChildren[i])
			{
				list = trueChildren[i]->graphicsItems;
				for (int j=0; j < list.size(); ++j)
					if (NodeGraphicsItem::cast(list[j]))
					{
						nodesInPlasmid << list[j];
					}
			}

		if (align && !nodesInPlasmid.isEmpty())
		{
			list = scene->selected();
			scene->selected() = nodesInPlasmid;
			
			autoAlignEnabled = false;
			emit alignCompactHorizontal();
			autoAlignEnabled = true;
			
			scene->selected() = list;

			QGraphicsItem * leftMost = nodesInPlasmid[0], * rightMost = nodesInPlasmid[0];
			for (int i=0; i < nodesInPlasmid.size(); ++i)
			{
				if (nodesInPlasmid[i] && leftMost && nodesInPlasmid[i]->scenePos().x() < leftMost->scenePos().x())
					leftMost = nodesInPlasmid[i];
				if (nodesInPlasmid[i] && rightMost && nodesInPlasmid[i]->scenePos().x() > rightMost->scenePos().x())
					rightMost = nodesInPlasmid[i];
			}

			if (vector->boundaryControlPoints.size() > 1 && vector->boundaryControlPoints[0] && vector->boundaryControlPoints[1])
			{
				QList<QGraphicsItem*> controls;
				controls << vector->boundaryControlPoints[0] << vector->boundaryControlPoints[1];
				QList<QPointF> dist;
				NodeGraphicsItem * leftMostNode = NodeGraphicsItem::cast(leftMost);
				QPointF p1,p2;
				QRectF rect = leftMost->sceneBoundingRect();
				p1.rx() = rect.left();
				p1.ry() = leftMost->scenePos().y();
				if (leftMostNode)
				{
					qreal leftx = 0.0;
					for (int i=0; i < leftMostNode->shapes.size(); ++i)
						if (leftMostNode->shapes[i] &&
							(leftx == 0.0 || leftMostNode->shapes[i]->boundingRect().left() <= leftx))
						{
							p1.ry() = leftMostNode->shapes[i]->sceneBoundingRect().y();
							leftx = leftMostNode->shapes[i]->boundingRect().left();
						}
				}
				p2 = rightMost->sceneBoundingRect().bottomRight();

				dist << (p1 - QPointF(100,0) - vector->boundaryControlPoints[0]->scenePos());
				
				QPointF p3(0,0);
				
				if (p2.y() > vector->boundaryControlPoints[1]->sceneBoundingRect().top())
		  			p3.ry() = p2.y() + 200.0 - vector->boundaryControlPoints[1]->scenePos().y();
				
				if (p2.x() > vector->boundaryControlPoints[1]->sceneBoundingRect().left())
		  			p3.rx() = p2.x() + 100.0 - vector->boundaryControlPoints[1]->scenePos().x();
				
				dist << p3;
				commands << new MoveCommand(scene,controls,dist);
			}
		}
		return new CompositeCommand(tr("plasmid adjusted"),commands);
	}

	/*****************************************
	            C API
	******************************************/

	typedef void (*tc_GRN_api) (tc_items (*f1)(long), tc_items (*f2)(long), tc_items (*f3)(long), void (*f4)(tc_items) );

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
				if (items[i] && NodeHandle::cast(items[i]))
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
			
			autoAlignEnabled = false;
			emit alignCompactHorizontal();
			autoAlignEnabled = true;
		}
		if (s)
			s->release();
	}

	void AutoGeneRegulatoryTool::_alignParts(tc_items A)
    {
        fToS.alignParts(A);
    }

    void AutoGeneRegulatoryTool_FtoS::alignParts(tc_items A)
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
			QList<ItemHandle*> allChildren = h->allChildren();
			NodeGraphicsItem * node = 0;
			NodeGraphicsItem * topmost = 0;
			for (int i=0; i < allChildren.size(); ++i)
				if (NodeHandle::cast(allChildren[i]) && allChildren[i]->isA(tr("Part")))
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
				findAllParts(scene,node,tr("Part"),upstream,true,QStringList());

				downstream.clear();
				if (node && node->handle())
				{
					downstream.push_front(node->handle());
					findAllParts(scene,node,tr("Part"),downstream,false,QStringList());
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

	tc_items AutoGeneRegulatoryTool::_partsIn(long o)
    {
        return fToS.partsIn(o);
    }

    tc_items AutoGeneRegulatoryTool_FtoS::partsIn(long o)
    {
        QSemaphore * s = new QSemaphore(1);
        QList<ItemHandle*>* p = new QList<ItemHandle*>;
        s->acquire();
        emit partsIn(s,ConvertValue(o),p);
        s->acquire();
        s->release();
        delete s;
        tc_items A = ConvertValue(*p);
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
				findAllParts(scene,node,tr("Part"),upstream,true,QStringList());

				downstream.clear();
				if (node && node->handle())
				{
					downstream.push_back(node->handle());
					//findAllParts(scene,node,tr("Part"),downstream,false,QStringList());
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

	tc_items AutoGeneRegulatoryTool::_partsUpstream(long o)
    {
        return fToS.partsUpstream(o);
    }

    tc_items AutoGeneRegulatoryTool_FtoS::partsUpstream(long o)
    {
        QSemaphore * s = new QSemaphore(1);
        QList<ItemHandle*>* p = new QList<ItemHandle*>;
        s->acquire();
        emit partsUpstream(s,ConvertValue(o),p);
        s->acquire();
        s->release();
        delete s;
        tc_items A = ConvertValue(*p);
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
				//findAllParts(scene,node,tr("Part"),upstream,true,QStringList());

				downstream.clear();
				if (node && node->handle())
				{
					//downstream.push_back(node->handle());
					findAllParts(scene,node,tr("Part"),downstream,false,QStringList());
				}

				(*parts) += downstream;
			}
		}
		if (s)
			s->release();
	}

	tc_items AutoGeneRegulatoryTool::_partsDownstream(long o)
    {
        return fToS.partsDownstream(o);
    }

    tc_items AutoGeneRegulatoryTool_FtoS::partsDownstream(long o)
    {
        QSemaphore * s = new QSemaphore(1);
        QList<ItemHandle*>* p = new QList<ItemHandle*>;
        s->acquire();
        emit partsDownstream(s,ConvertValue(o),p);
        s->acquire();
        s->release();
        delete s;
        tc_items A = ConvertValue(*p);
        delete p;
        return A;
    }

}

extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::AutoGeneRegulatoryTool * tool = new Tinkercell::AutoGeneRegulatoryTool;
	main->addTool(tool);
}

