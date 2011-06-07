/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 Automatically manage gene regulatory network rates and parameters
****************************************************************************/
#include <QDebug>
#include <QMessageBox>
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
#include "BasicGraphicsToolbar.h"
#include "ConnectionSelection.h"
#include "StoichiometryTool.h"
#include "NodeInsertion.h"
#include "NodesTree.h"
#include "ConnectionsTree.h"
#include "AutoGeneRegulatoryTool.h"

namespace Tinkercell
{
	AutoGeneRegulatoryTool_FtoS * AutoGeneRegulatoryTool::fToS;

	AutoGeneRegulatoryTool::AutoGeneRegulatoryTool() :
		Tool(tr("Auto Gene Regulatory Tool"),tr("Modeling")),
		autoTFUp("Insert activator",this),
		autoTFDown("Insert repressor",this),
		autoDegradation("Insert degadation reaction",this),
		autoGeneProduct("Insert gene product",this),
		mRNAstep("Add mRNA stage",this),
		autoPhosphate("Insert phosphate",this)
	{
		AutoGeneRegulatoryTool::fToS = new AutoGeneRegulatoryTool_FtoS;
		AutoGeneRegulatoryTool::fToS->setParent(this);
		
		separator = 0;
		autoAlignEnabled = true;
		setPalette(QPalette(QColor(255,255,255,255)));
		setAutoFillBackground(true);
		connect(&autoTFUp,SIGNAL(triggered()),this,SLOT(autoTFTriggeredUp()));
		connect(&autoTFDown,SIGNAL(triggered()),this,SLOT(autoTFTriggeredDown()));
		connect(&autoDegradation,SIGNAL(triggered()),this,SLOT(autoDegradationTriggered()));
		autoDegradation.setIcon(QIcon(":/images/empty.png"));
		connect(&autoGeneProduct,SIGNAL(triggered()),this,SLOT(autoGeneProductTriggered()));
		//connect(&autoPhosphate,SIGNAL(triggered()),this,SLOT(autoPhosphateTriggered()));
		//autoPhosphate.setIcon(QIcon(":/images/phosphate.png"));
		mRNAstep.setIcon(QIcon(":/images/upArrow.png"));

		connect(fToS,SIGNAL(partsIn(QSemaphore*, ItemHandle*, QList<ItemHandle*>*)),this,SLOT(partsIn(QSemaphore*, ItemHandle*, QList<ItemHandle*>*)));
		connect(fToS,SIGNAL(partsUpstream(QSemaphore*, ItemHandle*, QList<ItemHandle*>*)),this,SLOT(partsUpstream(QSemaphore*, ItemHandle*, QList<ItemHandle*>*)));
		connect(fToS,SIGNAL(partsDownstream(QSemaphore*, ItemHandle*, QList<ItemHandle*>*)),this,SLOT(partsDownstream(QSemaphore*, ItemHandle*, QList<ItemHandle*>*)));
		connect(fToS,SIGNAL(alignParts(QSemaphore*,const QList<ItemHandle*>&)),this,SLOT(alignParts(QSemaphore*,const QList<ItemHandle*>&)));
		connect(fToS,SIGNAL(alignPartsOnPlasmid(QSemaphore*,ItemHandle*, const QList<ItemHandle*>&)), this, SLOT(alignPartsOnPlasmid(QSemaphore*,ItemHandle*, const QList<ItemHandle*>&)));
	}

	void AutoGeneRegulatoryTool::autoPhosphateTriggered()
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !mainWindow) return;
		if (!mainWindow->tool(tr("Nodes Tree")) || !mainWindow->tool(tr("Connections Tree"))) return;

		QWidget * treeWidget = mainWindow->tool(tr("Nodes Tree"));
		NodesTree * nodesTree = static_cast<NodesTree*>(treeWidget);

		if (!nodesTree->getFamily("Phosphate")) return;

		NodeFamily * nodeFamily = nodesTree->getFamily("Phosphate");

		QList<QGraphicsItem*>& selected = scene->selected();
		ItemHandle * handle = 0;

		QList<QGraphicsItem*> sceneItems = scene->items();
		QList<QGraphicsItem*> list;

		QString appDir = QCoreApplication::applicationDirPath();

		QList<ItemHandle*> handlesPhos;

		for (int i=0; i < selected.size(); ++i)
		{
			handle = getHandle(selected[i]);
			if (NodeGraphicsItem::cast(selected[i]) && handle && handle->isA("Molecule") && !handlesPhos.contains(handle))
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

		if (!nodesTree->getFamily("Transcription Factor")) return;

        treeWidget = mainWindow->tool(tr("Connections Tree"));
		ConnectionsTree * connectionsTree = static_cast<ConnectionsTree*>(treeWidget);

		if (!connectionsTree->getFamily(regulationName)) return;

		NodeFamily * nodeFamily = nodesTree->getFamily("Transcription Factor");
		ConnectionFamily * connectionFamily = connectionsTree->getFamily(regulationName);

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
			if (NodeGraphicsItem::cast(selected[i]) && handle && handle->isA("Operator") && !visited.contains(handle))
			{
				regulatorNodes += NodeGraphicsItem::cast(selected[i]);
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
						if (node && ArrowHeadItem::cast(node))
						{
							arrow = static_cast<ArrowHeadItem*>(node->clone());
							arrow->connectionItem = item;
							if (arrow->defaultSize.width() > 0 && arrow->defaultSize.height() > 0)
								arrow->scale(arrow->defaultSize.width()/arrow->sceneBoundingRect().width(),arrow->defaultSize.height()/arrow->sceneBoundingRect().height());
							item->curveSegments.last().arrowStart = arrow;
							list += arrow;
						}
					}

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

		if (!nodesTree->getFamily("Protein") ||
			!connectionsTree->getFamily("Protein Production"))
			return;

		NodeFamily * proteinFamily = nodesTree->getFamily("Protein");
		ConnectionFamily * productionFamily = connectionsTree->getFamily("Protein Production");

		QList<QGraphicsItem*>& selected = scene->selected();
		ItemHandle * handle = 0;

		QStringList usedNames;
		QList<QGraphicsItem*> list;

		QString appDir = QCoreApplication::applicationDirPath();

		QList<ItemHandle*> visited;

		for (int i=0; i < selected.size(); ++i)
		{
			handle = getHandle(selected[i]);
			if (NodeGraphicsItem::cast(selected[i]) && handle && handle->isA("Coding") && !visited.contains(handle))
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
					if (productionFamily->graphicsItems.size() > 0)
					{
						NodeGraphicsItem * node = NodeGraphicsItem::cast(productionFamily->graphicsItems[0]);
						if (node && ArrowHeadItem::cast(node))
						{
							arrow = new ArrowHeadItem(*static_cast<ArrowHeadItem*>(node));
							arrow->connectionItem = item;
							if (arrow->defaultSize.width() > 0 && arrow->defaultSize.height() > 0)
								arrow->scale(arrow->defaultSize.width()/arrow->sceneBoundingRect().width(),arrow->defaultSize.height()/arrow->sceneBoundingRect().height());
							item->curveSegments.last().arrowEnd = arrow;
							list += arrow;
						}
					}

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
					
					if (productionFamily->graphicsItems.size() > 1)
					{
						NodeGraphicsItem * node = NodeGraphicsItem::cast(productionFamily->graphicsItems.last());
						if (node && ArrowHeadItem::cast(node))
						{
							arrow = new ArrowHeadItem(*static_cast<ArrowHeadItem*>(node));
							arrow->connectionItem = item;
							if (arrow->defaultSize.width() > 0 && arrow->defaultSize.height() > 0)
								arrow->scale(arrow->defaultSize.width()/arrow->sceneBoundingRect().width(),arrow->defaultSize.height()/arrow->sceneBoundingRect().height());
							item->centerRegionItem = arrow;
							list += arrow;
						}
					}
					
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

		if (!nodesTree->getFamily("Empty")) return;

		treeWidget = mainWindow->tool(tr("Connections Tree"));
		ConnectionsTree * connectionsTree = static_cast<ConnectionsTree*>(treeWidget);

		if (!connectionsTree->getFamily("Degradation")) return;

		NodeFamily * nodeFamily = nodesTree->getFamily("Empty");
		ConnectionFamily * connectionFamily = connectionsTree->getFamily("Degradation");

		QList<QGraphicsItem*>& selected = scene->selected();
		ItemHandle * handle = 0;

		QStringList usedNames;
		QList<QGraphicsItem*> list;

		QString appDir = QCoreApplication::applicationDirPath();

		QList<ItemHandle*> visited;

		for (int i=0; i < selected.size(); ++i)
		{
			handle = getHandle(selected[i]);
			if (NodeGraphicsItem::cast(selected[i]) && handle && handle->isA("Molecule") && !visited.contains(handle))
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
						if (node && ArrowHeadItem::cast(node))
						{
							arrow = new ArrowHeadItem(*static_cast<ArrowHeadItem*>(node));
							if (arrow->defaultSize.width() > 0 && arrow->defaultSize.height() > 0)
								arrow->scale(arrow->defaultSize.width()/arrow->sceneBoundingRect().width(),arrow->defaultSize.height()/arrow->sceneBoundingRect().height());
							arrow->connectionItem = item;
							list += arrow;
						}
					}
					
					item->curveSegments.last().arrowStart = arrow;

					connection->name = tr("deg1");
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
		QList<QUndoCommand*> commands;
		GraphicsScene * scene = currentScene();
		if (!scene || !mainWindow || parts.isEmpty()) return command;

		ItemHandle * rbs = 0;
		QString rate, s0;

		QList<ItemHandle*> targetHandles, operators, repressibleOperators;
		NodeHandle * promoter = 0;
		QList<QString> hashStrings;
		QList<DataTable<QString>*> oldDataTables, newDataTables;

		for (int i=0; i < parts.size(); ++i)
			if (parts[i])
			{
				if (parts[i]->isA(tr("RBS")))
				{
					rbs = parts[i];
				}

				if (parts[i]->isA(tr("Promoter")))
					promoter = NodeHandle::cast(parts[i]);

				if (parts[i]->isA(tr("Operator")))
				{
					if (!operators.contains(parts[i]))
						operators += parts[i];

					if (!repressibleOperators.contains(parts[i]))
					{
						QList<ConnectionHandle*> connections = parts[i]->connections();
						if (parts[i]->isA(tr("repressor binding site")))
							repressibleOperators += parts[i];
						else
							for (int j=0; j < connections.size(); ++j)
								if (connections[j] && connections[j]->isA(tr("repression")))
								{
									repressibleOperators += parts[i];
									break;
								}
					}
				}

				if (parts[i]->isA(tr("Coding")) && NodeHandle::cast(parts[i]))
				{
					rate = tr("");
					if (promoter)
					{
						bool promoterOperator = !promoter->connections().isEmpty();
						for (int k=0; k < operators.size(); ++k)
							if (!repressibleOperators.contains(operators[k]) &&
								(!operators[k]->isA(tr("promoter")) || 
								operators[k] != promoter ||
								promoterOperator))
								if (rate.isEmpty())
									rate = operators[k]->fullName();
								else
									rate += tr(" + ") + operators[k]->fullName();

						for (int k=0; k < repressibleOperators.size(); ++k)
							if (!repressibleOperators[k]->isA(tr("promoter")) || 
								repressibleOperators[k] != promoter ||
								promoterOperator)
							{
								if (k == 0)
								{
									if (!rate.isEmpty())
										rate = tr("( ") + rate + tr(") *");
									rate += tr("(") + repressibleOperators[k]->fullName();
								}
								else
									rate += tr(" * ") + repressibleOperators[k]->fullName();
								if (k == (repressibleOperators.size()-1))
									rate += tr(")");
							}
					}

					if (!promoter)
					{
						rate = tr("0.0");
					}
					else
					{
						if (!promoter->hasNumericalData("Parameters") || !promoter->numericalDataTable("Parameters").hasRow("strength"))
						{
							NumericalDataTable & params = promoter->numericalDataTable(tr("Parameters"));
							NumericalDataTable params2(params);
							params2("strength",0) = 5.0;
							commands << new ChangeNumericalDataCommand(tr("New parameters"),&params,&params2);
						}
						
						if (rate.isEmpty())
							rate = promoter->fullName() + tr(".strength");
						else
							rate = promoter->fullName() + tr(".strength * (") + rate + tr(")");

						if (parts[i]->parent && parts[i]->parent->isA(tr("Vector")))
							rate = parts[i]->parent->fullName() + tr(" * ") + rate;
					}
					
					if (parts[i]->hasTextData("Assignments") &&
						(parts[i]->textDataTable("Assignments").hasRow("self") || rate != tr("0.0")))
					{
						QString oldrate = parts[i]->textData(tr("Assignments"),tr("self"),0);
					
						bool isCustomEqn = StoichiometryTool::userModifiedRates.contains(parts[i]);
					

						if (!parts[i]->textDataTable(tr("Assignments")).hasRow(tr("self")) ||
							(!isCustomEqn && oldrate != rate))
							 {
								TextDataTable * sDat = new TextDataTable(parts[i]->textDataTable(tr("Assignments")));
								sDat->value(tr("self"),0) = rate;
								oldDataTables += &(parts[i]->textDataTable(tr("Assignments")));
								newDataTables += sDat;
							}
					}
				
					QList<ConnectionHandle*> connections = NodeHandle::cast(parts[i])->connections();
					
					for (int j=0; j < connections.size(); ++j)
						if (connections[j] &&
							connections[j]->hasTextData(tr("Participants")) &&
							connections[j]->hasTextData(tr("Rate equations")))
						{
							TextDataTable & participants = connections[j]->textDataTable(tr("Participants"));

							if (participants.at(tr("template"),0) == parts[i]->fullName())
							{	
								if (rbs)
								{
									if (!rbs->hasNumericalData("Parameters") || !rbs->numericalDataTable("Parameters").hasRow("strength"))
									{
										NumericalDataTable & params = rbs->numericalDataTable(tr("Parameters"));
										NumericalDataTable params2(params);
										params2("strength",0) = 5.0;
										commands << new ChangeNumericalDataCommand(tr("New parameters"),&params,&params2);
									}

									QList<NodeHandle*> rna = connections[j]->nodes();
									rna << NodeHandle::cast(connections[j]->children);

									for (int k=0; k < rna.size(); ++k)
										if (NodeHandle::cast(rna[k]) && rna[k]->isA(tr("RNA")))
										{
											QList<ConnectionHandle*> connections2 = NodeHandle::cast(rna[k])->connections();
											for (int l=0; l < connections2.size(); ++l)
												if (connections2[l] &&
													connections2[l]->isA(tr("Translation")) &&
													connections2[l]->hasTextData(tr("Rate equations")))
												{
													DataTable<QString> * sDat2 = new DataTable<QString>(connections2[l]->textDataTable(tr("Rate equations")));
													QString s = rbs->fullName() + tr(".strength * ") + rna[k]->fullName();

													if (!sDat2->value(0,0).contains(rbs->fullName()))
													{
														sDat2->value(0,0) = s;
														oldDataTables += &(connections2[l]->textDataTable(tr("Rate equations")));
														newDataTables += sDat2;
													}
													else
														delete sDat2;
												}
										}
									}
								}
						}
						rbs = 0;
				
						if ((parts[i]->isA(tr("Terminator")) || parts[i]->isA(tr("Vector")) ) && NodeHandle::cast(parts[i]))
						{
							operators.clear();
							promoter = 0;
						}
					}
				}
		if (newDataTables.size() > 0)
		{
			command = new ChangeTextDataCommand(tr("Gene regulation kinetics changed"),oldDataTables,newDataTables);

			for (int i=0; i < newDataTables.size(); ++i)
				if (newDataTables[i])
					delete newDataTables[i];
		}
		
		if (commands.isEmpty())
			return command;
		
		commands << command;
		return new CompositeCommand(tr("Gene regulation kinetics changed"),commands);
	}

	bool AutoGeneRegulatoryTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow != 0)
		{
			connect(this,SIGNAL(dataChanged(const QList<ItemHandle*>&)),
					mainWindow,SIGNAL(dataChanged(const QList<ItemHandle*>&)));

			connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
					this,SLOT(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));

			//connect(mainWindow,SIGNAL(itemsAboutToBeRemoved(GraphicsScene*,QList<QGraphicsItem*>&,QList<ItemHandle*>&,QList<QUndoCommand*>&)),
            //              this, SLOT(itemsRemoved(GraphicsScene*,QList<QGraphicsItem*>&, QList<ItemHandle*>&,QList<QUndoCommand*>&)));

			connect(mainWindow,SIGNAL(itemsAboutToBeMoved(GraphicsScene*, QList<QGraphicsItem*>&, QList<QPointF>&, QList<QUndoCommand*>&)),
						  this, SLOT(itemsMoved(GraphicsScene*, QList<QGraphicsItem*>&, QList<QPointF>&, QList<QUndoCommand*>&)));

			connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
						this,SLOT(itemsSelected(GraphicsScene *,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
						
			
			//connect(mainWindow,SIGNAL(parentHandleChanged(NetworkHandle * , const QList<ItemHandle*>&, const QList<ItemHandle*>&)),
			//			this,SLOT(parentHandleChanged(NetworkHandle * , const QList<ItemHandle*>&, const QList<ItemHandle*>&)));

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
			BasicGraphicsToolbar * basicToolBox = static_cast<BasicGraphicsToolbar*>(widget);
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
			if ((handle = getHandle(items[i])) && handle->isA(tr("Part")) && !(handle->parent && handle->parent->isA(tr("Vector"))))
			{
				partCollided = true;
				break;
			}

		if (!partCollided)
		{
			handle = item->handle();
			if (handle && item->connections().isEmpty())
				for (int i=0; i < handle->graphicsItems.size(); ++i)
					if ((item = NodeGraphicsItem::cast(handle->graphicsItems[i])) &&
						(item->scene() == scene) &&
						!item->connections().isEmpty())
						break;
					else
						item = 0;
			
			NodeGraphicsItem * hitNode;
			for (int i=0; i < items.size(); ++i)
				if (hitNode = NodeGraphicsItem::cast(items[i]))
					break;

			ItemHandle * h;
			if (hitNode && item
				&& handle
				&& !hitNode->connections().isEmpty()
				&& !item->connections().isEmpty()
				&& (h = hitNode->handle()) 

				&& (h->isA(handle->family()) || handle->isA(h->family())) 
				&& h->isA("Node") && handle->isA("Node")
				)
			{
				if (handle->isA(h->family()))
				{
					QList<QUndoCommand*> commands;
					QList<ConnectionGraphicsItem*> connections = hitNode->connections();
					for (int i=0; i < connections.size(); ++i)
						commands << new ReplaceConnectedNodeCommand(tr(""), connections[i],hitNode,item);
					commands << new MergeHandlesCommand(tr(""), scene->network, QList<ItemHandle*>() << handle << h);
					QList<QGraphicsItem*> graphicsItems;
					for (int i=0; i < h->graphicsItems.size(); ++i)
						if (h->graphicsItems[i]->scene() == scene)
							graphicsItems << h->graphicsItems[i];
					commands << new RemoveGraphicsCommand(tr("removed ") + h->name, graphicsItems);
					scene->network->push(new CompositeCommand(h->name + tr(" merged into ") + handle->name, commands));
				}
				else
				{
					QList<QUndoCommand*> commands;
					QList<ConnectionGraphicsItem*> connections = item->connections();
					for (int i=0; i < connections.size(); ++i)
						commands << new ReplaceConnectedNodeCommand(tr(""), connections[i],item,hitNode);
					commands << new MergeHandlesCommand(tr(""), scene->network, QList<ItemHandle*>() << h << handle);
					QList<QGraphicsItem*> graphicsItems;
					for (int i=0; i < handle->graphicsItems.size(); ++i)
						if (handle->graphicsItems[i]->scene() == scene)
							graphicsItems << handle->graphicsItems[i];
					commands << new RemoveGraphicsCommand(tr("removed ") + handle->name, graphicsItems);
					scene->network->push(new CompositeCommand(handle->name + tr(" merged into ") + h->name, commands));
				}
			}
			return;
		}

		QList<QGraphicsItem*> select;

		QGraphicsItem * dnaItem = 0;

		if ((handle = getHandle(item)) && handle->isA("Part") && !handle->isA("Vector"))
		{
			dnaItem = item;
			select << dnaItem;
		}
		else
			for (int i=0; i < items.size(); ++i)
				if (NodeGraphicsItem::cast(items[i]) && (handle = getHandle(items[i])) && handle->isA("Part") && !handle->isA("Vector"))
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
					if (!select.contains(items[i]) && NodeGraphicsItem::cast(items[i]) && (handle = getHandle(items[i])) && handle->isA("Part") && !handle->isA("Vector"))
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
					if (!select.contains(items[i]) && NodeGraphicsItem::cast(items[i]) && (handle = getHandle(items[i])) && handle->isA("Part") && !handle->isA("Vector"))
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

	void AutoGeneRegulatoryTool::itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>& handles)
	{
		if (ConnectionGraphicsItem::cast(items).size() != 1 && !scene->localHandle()) return;

		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;
		QList<NodeGraphicsItem*> nodes;
		QList<ItemHandle*> visited;
		ItemHandle * handle;
		QList<QGraphicsItem*> itemsToRemove;
		
		for (int i=0; i < items.size(); ++i)
		{
			connection = ConnectionGraphicsItem::cast(items[i]);
			if (connection 
				&& (handle = connection->handle()) 
				&& (handle->isA(tr("Regulation"))))
				{
					QList<NodeGraphicsItem*> nodes = connection->nodesWithArrows();
					for (int j=0; j < nodes.size(); ++j)
					{
						if (nodes[j] && (handle = nodes[j]->handle()) && !handles.contains(handle) && handle->isA(tr("Operator")))
						{
							QList<ConnectionGraphicsItem*> connections = nodes[j]->connections();
							for (int k=0; k < connections.size(); ++k)
								if (connections[k] && connections[k] != connection && 
									connections[k]->handle() && connections[k]->handle()->isA("Regulation"))
									itemsToRemove << connections[k];
						}
					}
				}
		}
		
		if (!itemsToRemove.isEmpty())
		{
			scene->remove(tr("removed previous regulation"), itemsToRemove);
			QMessageBox::information(mainWindow, "One binding site", "Please insert additional operator sites -- only one transcription factor per operator site. Note: 'repressor binding site' and 'activator binding site' are sub-families of operators");
		}
		
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
			
			connection = ConnectionGraphicsItem::cast(items[i]);
			NodeGraphicsItem * startNode = 0;
			if (connection 
				&& (handle = connection->handle())
				&& (
						/*handle->isA(tr("Transcription Regulation")) ||*/
						handle->isA(tr("Production")) ||
						(handle->parent && handle->parent->isA(tr("Production")) && !handles.contains(handle->parent)) ||
						handle->isA(tr("Direct Gene Regulation")) ||
						(handle->parent && handle->parent->isA(tr("Direct Gene Regulation")) && !handles.contains(handle->parent))
					))
			{
				if (handle->parent && (handle->parent->isA(tr("Production")) || handle->parent->isA(tr("Direct Gene Regulation"))))
				{
					QList<ConnectionGraphicsItem*> connections = ConnectionGraphicsItem::cast(handle->parent->graphicsItems);
					if (!connections.isEmpty())
						nodes = connections[0]->nodes();
				}
				else
				{
					nodes = connection->nodes();
				}

				for (int j=0; j < nodes.size(); ++j)
					if (nodes[j] && !items.contains(nodes[j]) && nodes[j]->scene() && nodes[j]->handle() && nodes[j]->handle()->isA(tr("Coding")))
					{
						startNode = nodes[j];
						break;
					}
			}

			if (!startNode) continue;

			handle = NodeHandle::cast(getHandle(startNode));
			if (visited.contains(handle) || !handle) continue;

			QList<ItemHandle*> parts,upstream;

			if (!handle->parent || !handle->parent->isA(tr("Vector")))  //no "up" and "down" stream for circular plasmids
				findAllParts(startNode,tr("Part"),parts,false,QStringList() << "Terminator" << "Vector",true);

			findAllParts(startNode,tr("Part"),upstream,true,QStringList() << "Terminator" << "Vector",true);

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
				NodeHandle * h = NodeHandle::cast(parts[j]);
				parts3 += h;
				visited += h;
			}
			
			if (!parts3.isEmpty())
			{
				scene->network->push(autoAssignRates(parts3));
			}
		}
	}
	
	void AutoGeneRegulatoryTool::itemsMoved(GraphicsScene* scene, QList<QGraphicsItem*>& items, QList<QPointF>& distance, QList<QUndoCommand*>& commands)
	{
		if (!scene || !autoAlignEnabled) return;

		QList<NodeHandle*> parts2;

		QList<ItemHandle*> connections;
		NodeHandle * handle = 0;
		ItemHandle * h;
		QString rate;
		
		for (int i=0; i < items.size(); ++i)
		{
			handle = NodeHandle::cast( getHandle(items[i]) );
			if (NodeGraphicsItem::cast(items[i]) && handle && (handle->isA(tr("Operator")) || handle->isA(tr("RBS"))))
			{
				connections = scene->network->symbolsTable.handlesByFamily.values(tr("Transcription"));
				connections += scene->network->symbolsTable.handlesByFamily.values(tr("Translation"));
				connections += scene->network->symbolsTable.handlesByFamily.values(tr("Protein Production"));
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
									&& !items.contains(connection)
									&& connection->scene() == scene)
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
									
									if ((h = nodes[k]->handle()) && h->parent && h->parent->isA(tr("Part")))
									{
										rect2 = QRectF(0,0,0,0);
										for (int l=0; l < h->parent->graphicsItems.size(); ++l)
											if (h->parent->graphicsItems[l] && 
												h->parent->graphicsItems[l]->sceneBoundingRect().width() > rect2.width()) 
											rect2 = h->parent->graphicsItems[l]->sceneBoundingRect();
									}
									else
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
								DataTable<QString> newRates(connections[j]->textDataTable(tr("Rate equations")));
								newRates.value(0,0) = tr("0");
								QString s = connections[j]->fullName() + tr(" rate = 0.0");
								
								commands << new ChangeTextDataCommand(s,&(connections[j]->textDataTable(tr("Rate equations"))),&newRates);
							}
						}
					}
				}
			}
		}
		
		QList<QUndoCommand*> adjustPlasmidCommands;
		for (int i=0; i < items.size(); ++i)
			if (NodeGraphicsItem::cast(items[i]) && 
				NodeGraphicsItem::cast(items[i])->handle() &&
				NodeGraphicsItem::cast(items[i])->handle()->isA(tr("Vector")))
			{
				adjustPlasmidCommands += adjustPlasmid(scene, NodeGraphicsItem::cast(items[i]),false);
			}
		
		for (int i=0; i < items.size(); ++i)
			if (qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(items[i]) && 
				!items.contains(qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(items[i])->nodeItem) &&
				qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(items[i])->nodeItem &&
				qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(items[i])->nodeItem->handle() &&
				qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(items[i])->nodeItem->handle()->isA(tr("Vector")))
			{
				adjustPlasmidCommands += adjustPlasmid(scene,qgraphicsitem_cast<NodeGraphicsItem::ControlPoint*>(items[i])->nodeItem,true,true);
				items[i] = 0;
			}
		
		for (int i=0; i < items.size(); ++i)
			if (NodeGraphicsItem::cast(items[i]) && 
				NodeGraphicsItem::cast(items[i])->handle() &&
				NodeGraphicsItem::cast(items[i])->handle()->parent &&
				NodeGraphicsItem::cast(items[i])->handle()->parent->isA(tr("Vector")))
			{
				QList<QGraphicsItem*> & graphicsItems = NodeGraphicsItem::cast(items[i])->handle()->parent->graphicsItems;
				for (int j=0; j < graphicsItems.size(); ++j)			
					if (NodeGraphicsItem::cast(graphicsItems[j]) && !items.contains(graphicsItems[j]))
					{
						adjustPlasmidCommands += adjustPlasmid(scene, NodeGraphicsItem::cast(graphicsItems[j]),false);
					}
			}

		if (!adjustPlasmidCommands.isEmpty())
			for (int i=0; i < adjustPlasmidCommands.size(); ++i)
			{
				adjustPlasmidCommands[i]->redo();
			}
			
		for (int i=0; i < items.size(); ++i)
		{
			NodeGraphicsItem * startNode = NodeGraphicsItem::topLevelNodeItem(items[i]);

			if (!startNode) continue;

			handle = NodeHandle::cast(getHandle(startNode));
			if (parts2.contains(handle) || !handle) continue;

			if (distance.size() > i)
				startNode->setPos( startNode->scenePos() + distance[i] );

			QList<ItemHandle*> parts,upstream;

			if (!handle->parent || !handle->parent->isA(tr("Vector")))  //no "up" and "down" stream for circular plasmids
				findAllParts(startNode,tr("Part"),parts,false,QStringList() << "Terminator" << "Vector",true);

			findAllParts(startNode,tr("Part"),upstream,true,QStringList() << "Terminator" << "Vector",true);
			
			if (distance.size() > i)
				startNode->setPos( startNode->scenePos() - distance[i] );

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
				NodeHandle * node = NodeHandle::cast(parts[j]);
				parts3 += node;
				parts2 += node;
			}
			if (!parts3.isEmpty())
			{
				commands << autoAssignRates(parts3);
			}
		}
		
		if (!adjustPlasmidCommands.isEmpty())
		{
			for (int i=0; i < adjustPlasmidCommands.size(); ++i)
			{
				adjustPlasmidCommands[i]->undo();
				commands += adjustPlasmidCommands[i];
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

				if (!handle->parent || !handle->parent->isA(tr("Vector")))  //no "up" and "down" stream for circular plasmids
					findAllParts(startNode,tr("Part"),parts,false,QStringList() << "Terminator" << "Vector",true);

				findAllParts(startNode,tr("Part"),upstream,true,QStringList() << "Terminator" << "Vector",true);

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
		bool containsRegulatorUp = false;
		bool containsRegulatorDown = false;
		bool containsCoding = false;

		for (int i=0; i < items.size(); ++i)
		{
			handle = getHandle(items[i]);
			if (!containsSpecies && handle && handle->isA("Molecule"))
				containsSpecies = true;
			if (!containsProteins && handle && handle->isA("Protein"))
				containsProteins = true;
			if (!containsRegulatorDown && handle && handle->isA("Repressor Binding Site"))
				containsRegulatorDown = true;
			if (!containsRegulatorUp && handle && handle->isA("Activator Binding Site"))
				containsRegulatorUp = true;
			if (!containsCoding && handle && handle->isA("Coding"))
				containsCoding = true;

			if ((containsRegulatorDown || containsRegulatorUp) && containsSpecies && containsCoding) break;
		}

		if (containsSpecies || containsRegulatorDown || containsRegulatorUp || containsCoding)
		{
			if (separator)
				mainWindow->contextItemsMenu.addAction(separator);
			else
				separator = mainWindow->contextItemsMenu.addSeparator();

			if (containsSpecies)
				mainWindow->contextItemsMenu.addAction(&autoDegradation);
			else
				mainWindow->contextItemsMenu.removeAction(&autoDegradation);

			if (containsRegulatorUp)
				mainWindow->contextItemsMenu.addAction(&autoTFUp);
			else
				mainWindow->contextItemsMenu.removeAction(&autoTFUp);
			
			if (containsRegulatorDown)
				mainWindow->contextItemsMenu.addAction(&autoTFDown);
			else
				mainWindow->contextItemsMenu.removeAction(&autoTFDown);

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

	void AutoGeneRegulatoryTool::findAllParts(NodeGraphicsItem * node, const QString& family,QList<ItemHandle*>& handles,bool upstream,const QStringList & until, bool stopIfElongation)
	{
		if (!node) return;
		if (node->handle() && node->handle()->parent && node->handle()->parent->isA(tr("Vector")))
		{
			QList<QGraphicsItem*> & graphicsItems = node->handle()->graphicsItems;			
			QList<NodeGraphicsItem*> nodes;
			for (int i=0; i < graphicsItems.size(); ++i)
				if ((node = NodeGraphicsItem::cast(graphicsItems[i])) && !nodes.contains(node))
					nodes << node;
			
			for (int i=0; i < nodes.size(); ++i)
				if (nodes[i]->scene())
				{
					QList<ItemHandle*> list;
					GraphicsScene* scene = static_cast<GraphicsScene*>(nodes[i]->scene());
					findAllPartsCircular(scene,nodes[i],family,list,upstream,until,stopIfElongation);
					if (list.size() > handles.size())
						handles = list;
				}
		}
		else
		{
			QList<QGraphicsItem*> & graphicsItems = node->handle()->graphicsItems;			
			QList<NodeGraphicsItem*> nodes;
			for (int i=0; i < graphicsItems.size(); ++i)
				if ((node = NodeGraphicsItem::cast(graphicsItems[i])) && !nodes.contains(node))
					nodes << node;
			
			for (int i=0; i < nodes.size(); ++i)
				if (nodes[i]->scene())
				{
					QList<ItemHandle*> list;
					GraphicsScene* scene = static_cast<GraphicsScene*>(nodes[i]->scene());
					findAllPartsLinear(scene,nodes[i],family,list,upstream,until,stopIfElongation);
					if (list.size() > handles.size())
						handles = list;
				}
		}
	}
	
	void AutoGeneRegulatoryTool::findAllPartsCircular(GraphicsScene* scene,NodeGraphicsItem * node, const QString& family,QList<ItemHandle*>& handles,bool upstream,const QStringList & until, bool stopIfElongation)
	{
		if (!node || !scene || 
			!(node->handle() && node->handle()->parent && node->handle()->parent->isA(tr("Vector")))) return;
		
		QList<QGraphicsItem*> & parentItems = node->handle()->parent->graphicsItems;
		
		NodeGraphicsItem * vector = 0;
		for (int i=0; i < parentItems.size(); ++i)
		{
			if (NodeGraphicsItem::cast(parentItems[i]) && 
				parentItems[i]->sceneBoundingRect().width() > node->sceneBoundingRect().width())
			{
				vector = NodeGraphicsItem::cast(parentItems[i]);
			}
		}
		
		if (!vector) return;
		
		QPointF center = vector->sceneBoundingRect().center();
		QPointF p1, p2;
		
		qreal radius = vector->sceneBoundingRect().width()/2.0;
		
		p1 = node->sceneBoundingRect().center();
		qreal angle;
		if (p1.x() == center.x())
			if (p1.y() < center.y())
				angle = 3.14159/2.0;
			else
				angle = -3.14159/2.0;
		else
			angle = atan((p1.y()-center.y())/(p1.x()-center.x()));

		if (p1.x() < center.x())
		{
			angle -= 3.14159;
		}

		bool stop = false;
		int n = 100;
		qreal da = (2*3.14159)/(double)n;
		if (upstream)
			da = -da;
		
		for (int i=0; i < n; ++i)
		{
			p2.rx() = center.x() + cos(angle)*(radius);
			p2.ry() = center.y() + sin(angle)*(radius);
			
			QList<QGraphicsItem*> items = scene->items(QRectF(p2.rx() - 10.0, p2.ry() - 10.0, 20.0, 20.0));
			NodeGraphicsItem * topItem = 0;
			
			for (int j=0; j < items.size(); ++j)
				if ((topItem = NodeGraphicsItem::cast(items[j])) && 
					 (topItem->handle() && topItem->handle()->isA(family) && topItem->handle()->parent == vector->handle()))
					 break;
				else
					topItem = 0;
			
			if (topItem)
			{
				for (int j=0; j < until.size(); ++j)
					if (topItem->handle()->isA(until[j]))
					{
						stop = true;
						break;
					}
				if (!stop && !handles.contains(topItem->handle()))
					handles += topItem->handle();
			}
			
			angle += da;
			if (stop)
				break;
		}
	}
	
	void AutoGeneRegulatoryTool::findAllPartsLinear(GraphicsScene* scene,NodeGraphicsItem * node, const QString& family,QList<ItemHandle*>& handles,bool upstream,const QStringList & until, bool stopIfElongation)
	{
		if (!scene || !node) return;

		QList<NodeGraphicsItem*> visited;

		QTransform t = node->transform(); 
		bool flipped = (t.m11() < 0) || (t.m22() < 0) || (t.m12() != 0) || (t.m21() != 0);
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
			if (handle && handle->isA(family) && //handle->children.isEmpty() &&
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
					if (connections[j] && connections[j]->scene() == scene &&
							connections[j]->handle() && connections[j]->handle()->isA(tr("PoPS")))
					{
						QList<NodeGraphicsItem*> connectedNodes = connections[j]->nodesWithoutArrows();
						if (connectedNodes.size() > 0 && connectedNodes[0] && (h = connectedNodes[0]->handle())
							&& h->isA(family) && /*h->children.isEmpty() &&*/ !visited.contains(connectedNodes[0])
							&& (!stopIfElongation
								|| !connections[j]->handle()->hasNumericalData(tr("Product stoichiometries"))
								|| connections[j]->handle()->numericalDataTable(tr("Product stoichiometries")).rows() < 1)
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
					if (connections[j] && connections[j]->scene() == scene &&
							connections[j]->handle() && connections[j]->handle()->isA(tr("PoPS")))
					{
						QList<NodeGraphicsItem*> connectedNodes = connections[j]->nodesWithArrows();
						if (connectedNodes.size() > 0 && connectedNodes[0] && (h = connectedNodes[0]->handle())
							&& h->isA(family) && /*h->children.isEmpty() &&*/ !visited.contains(connectedNodes[0])
							&& (!stopIfElongation
								|| !connections[j]->handle()->hasNumericalData(tr("Product stoichiometries"))
								|| connections[j]->handle()->numericalDataTable(tr("Product stoichiometries")).rows() < 1)
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
			if (NodeGraphicsItem::cast(handle->graphicsItems[i]) && handle->graphicsItems[i]->scene() == scene
				&& ((y < 0) || (y > handle->graphicsItems[i]->scenePos().y())))
			{
				topmost = NodeGraphicsItem::cast(handle->graphicsItems[i]);
				y = topmost->scenePos().y();
			}

		if (!topmost) return;
		items << topmost;

		bool hit = false;

		for (int i=0; i < items.size(); ++i)
			if ((node = NodeGraphicsItem::cast(items[i])) && items[i]->scene() == scene)
			{
				if (upstream)
					p = QPointF(node->sceneBoundingRect().left() - 10.0,node->scenePos().ry());
				else
					p = QPointF(node->sceneBoundingRect().right() + 10.0,node->scenePos().ry());

				ht = node->sceneBoundingRect().height();
				QList<QGraphicsItem*> items2 = scene->items(QRectF(p.x()-1.0,p.y()-10.0,2.0,ht));

				hit = false;

				for (int j=0; j < items2.size(); ++j)
					if ((node = NodeGraphicsItem::cast(items2[j])) && items2[j]->scene() == scene)
					{
						h = node->handle();

						if (!h || !h->isA(family)/* || !h->children.isEmpty()*/) continue;

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
					node = NodeGraphicsItem::cast(items[i]);
					if (upstream && node && items[i]->scene() == scene)
					{
						QList<ConnectionGraphicsItem*> connections = node->connectionsWithArrows();
						for (int j=0; j < connections.size(); ++j)
						{
							if (connections[j] && connections[j]->scene() == scene &&
									connections[j]->handle() && connections[j]->handle()->isA(tr("PoPS")))
							{
								QList<NodeGraphicsItem*> connectedNodes = connections[j]->nodesWithoutArrows();
								if (connectedNodes.size() > 0 && connectedNodes[0] && (h = connectedNodes[0]->handle())
									&& h->isA(family) && /*h->children.isEmpty() &&*/ !visited.contains(connectedNodes[0]))
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
							if (connections[j] && connections[j]->scene() == scene &&
									connections[j]->handle() && connections[j]->handle()->isA(tr("PoPS")))
							{
								QList<NodeGraphicsItem*> connectedNodes = connections[j]->nodesWithArrows();
								if (connectedNodes.size() > 0 && connectedNodes[0] && (h = connectedNodes[0]->handle())
									&& h->isA(family) && /*h->children.isEmpty() &&*/ !visited.contains(connectedNodes[0]))
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
						if ((node = NodeGraphicsItem::cast(h->graphicsItems[j]))
							&& h->graphicsItems[j]->scene() == scene
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
		if (!handle || !handle->isA(tr("Operator"))) return QString();

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
			rate = handle->fullName() + tr(".strength/(") + allTFs.join("*") + tr(")");
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

	QUndoCommand * AutoGeneRegulatoryTool::adjustPlasmid(GraphicsScene * scene, NodeGraphicsItem * vector, bool align, bool keepChildren)
	{
		QList<QUndoCommand*> commands;
		if (!vector || !scene) return new CompositeCommand(tr("plasmid adjusted"),commands);

		ItemHandle * vectorHandle = vector->handle();
		ItemHandle * handle = 0;

		QRectF boundingRect = vector->sceneBoundingRect();
		
		if (fabs(boundingRect.width() - boundingRect.height()) > 2)
		{
			qreal w = boundingRect.width();
			if (boundingRect.height() > w)
				w = boundingRect.height();
			commands << new TransformCommand(
							tr("Circular arrangement"), 
							scene, 
							QList<QGraphicsItem*>() << vector, 
							QList<QPointF>() << QPointF(w/boundingRect.width(),w/boundingRect.height()), 
							QList<qreal>() , 
							QList<bool>(), QList<bool>());
		}

		QList<ItemHandle*> children, parents;
		QList<QGraphicsItem*> intersectingItems = scene->items(boundingRect);
		bool flipped;
		
		qreal radius = boundingRect.width()/2.0;
		QPointF center = boundingRect.center();
		
		qreal lowestZ = scene->ZValue();

		for (int i=0; i < intersectingItems.size(); ++i)
			if (intersectingItems[i] && intersectingItems[i] != vector)
			{
				if (intersectingItems[i]->zValue() < lowestZ)
					lowestZ = intersectingItems[i]->zValue();
				handle = getHandle(intersectingItems[i]);
				if (handle && 
					!handle->isChildOf(vectorHandle) &&
					handle->isA(tr("Part")) && 
					!handle->isA(tr("Vector")) &&
					(!handle->parent || vectorHandle->isChildOf(handle->parent)))
				{
					children << handle;
					parents << vectorHandle;
				}
			}

		QList<QGraphicsItem*> list;
		QList<ItemHandle*> existingChildren = vectorHandle->children;
		QList<ItemHandle*> trueChildren = children + existingChildren;
		
		if (keepChildren)
			trueChildren = existingChildren;
		
		for (int i=0; i < existingChildren.size(); ++i)
			if (existingChildren[i])
			{
				list = existingChildren[i]->graphicsItems;
				bool intersection = false;
				for (int j=0; j < list.size(); ++j)
					if (NodeGraphicsItem::cast(list[j]) && intersectingItems.contains(list[j]))
					{
						intersection = true;
						break;
					}
				if (!intersection)
				{
					children << existingChildren[i];
					parents << 0;
					trueChildren.removeAll(existingChildren[i]);
				}
			}
			
		if (keepChildren)
		{
			trueChildren = existingChildren;
			children.clear();
		}
		
		if (!children.isEmpty())
		{
			commands << new SetParentHandleCommand(tr("parents set"), scene->network, children, parents);
			commands << new ChangeZCommand(tr("adjust z"), scene, vector, lowestZ-1.0);
		}

		QList<NodeGraphicsItem*> nodesInPlasmid;
		QHash< QString, TextGraphicsItem* > textsInPlasmid;

		for (int i=0; i < trueChildren.size(); ++i)
			if (trueChildren[i])
			{
				list = trueChildren[i]->graphicsItems;
				for (int j=0; j < list.size(); ++j)
					if (list[j]->scene() == vector->scene())
						if (NodeGraphicsItem::cast(list[j]))
						{
							nodesInPlasmid << NodeGraphicsItem::cast(list[j]);
						}
						else
						if (TextGraphicsItem::cast(list[j]))
						{
							textsInPlasmid.insertMulti(trueChildren[i]->fullName(), TextGraphicsItem::cast(list[j]));
						}
			}
		
		//here comes the real part....
		
		QTransform t;

		QList<QGraphicsItem*> itemsToMove, textToMove, shapes;
		QList<QPointF> moveBy, textMoveBy;
		QList<qreal> rotateBy;
		QList<bool> flips;
		bool flip;
		QList<QBrush> noBrushes;
		QList<QPen> noPens;

		qreal angle;
		QPointF p1, p2, p3;

		if (align && !nodesInPlasmid.isEmpty())
		{
			for (int i=0; i < nodesInPlasmid.size(); ++i)
			{
				t = nodesInPlasmid[i]->sceneTransform();
				nodesInPlasmid[i]->resetToDefaults();
				
				boundingRect = nodesInPlasmid[i]->sceneBoundingRect();
				
				//if ((t.m11() < 0) && (t.m22() < 0))// || (t.m12() != 0) || (t.m21() != 0))
				//	flip = true;
				//else
				flip = false;

				p1 = boundingRect.center();
				qreal angle;
				if (p1.x() == center.x())
					if (p1.y() < center.y())
						angle = 3.14159/2.0;
					else
						angle = -3.14159/2.0;
				else
					angle = atan((p1.y()-center.y())/(p1.x()-center.x()));

				qreal w,w2;
				if (flip)
					w = p1.y() - nodesInPlasmid[i]->leftMostShape()->sceneBoundingRect().center().y();
				else
					w = nodesInPlasmid[i]->leftMostShape()->sceneBoundingRect().center().y() - p1.y();

				if (p1.x() > center.x())
				{
					p2.rx() = center.x() + cos(angle)*(radius + w);
					p2.ry() = center.y() + sin(angle)*(radius + w);
					w2 = nodesInPlasmid[i]->sceneBoundingRect().height() + 10.0;
					p3.rx() = center.x() + cos(angle)*(radius + w2);
					p3.ry() = center.y() + sin(angle)*(radius + w2);
					angle += 3.14159/2.0;
				}
				else
				{
					p2.rx() = center.x() - cos(angle)*(radius + w);
					p2.ry() = center.y() - sin(angle)*(radius + w);
					w2 = nodesInPlasmid[i]->sceneBoundingRect().height() + 10.0;
					p3.rx() = center.x() - cos(angle)*(radius + w2);
					p3.ry() = center.y() - sin(angle)*(radius + w2);
					angle -= 3.14159/2.0;
				}
				
				QPointF dx = p2 - p1;
				if (handle = getHandle(nodesInPlasmid[i]))
				{
					QList<TextGraphicsItem*> textItems = textsInPlasmid.values(handle->fullName());
					for (int j=0; j < textItems.size(); ++j)
						if (!textToMove.contains(textItems[j]))
						{
							textToMove += textItems[j];
							textMoveBy += (p3 - textItems[j]->sceneBoundingRect().center() - QPointF(0,j*5.0));
						}
				}
				
				if (!itemsToMove.contains(nodesInPlasmid[i]))
				{
					itemsToMove += nodesInPlasmid[i];
					moveBy += dx;
					rotateBy += (angle * 180/3.14159);
					
					NodeGraphicsItem::Shape * shape1 = nodesInPlasmid[i]->rightMostShape(),
																* shape2 = nodesInPlasmid[i]->leftMostShape();
					if (shape1 && shape2 && 
						shape1->defaultBrush.color() == QColor(0,0,0) &&
						 shape2->defaultBrush.color() == QColor(0,0,0))
					{
						shapes << shape1 << shape2;
                        QBrush nocolor(QColor(0,0,0,0));
						noBrushes << nocolor << nocolor;
						noPens << QPen(nocolor,0) << QPen(nocolor,0);
					}
					
					flips += flip;
	
					QList<QGraphicsItem*> & graphicsItems = nodesInPlasmid[i]->handle()->graphicsItems;
					for (int j=0; j < graphicsItems.size(); ++j)
						if ( nodesInPlasmid[i] != graphicsItems[j] && 
							graphicsItems[j]->sceneBoundingRect().intersects(boundingRect.adjusted(-10,-10,10,10)))
						{
							itemsToMove += graphicsItems[j];
							moveBy += (p2 - p1);
							rotateBy += 0.0;
							flips += false;
						}
				}
			}
			
			if (!itemsToMove.isEmpty())
			{
				commands << new TransformCommand(tr("rotate"), scene, itemsToMove, QList<QPointF>(), rotateBy, flips, flips);
				itemsToMove += textToMove;
				moveBy += textMoveBy;
				commands << new MoveCommand(scene, itemsToMove, moveBy)
								 << new ChangeBrushAndPenCommand(tr("invisible"),shapes, noBrushes, noPens);
			}
		}
		
		for (int i=0; i < children.size(); ++i)
		{
			NodeGraphicsItem * node;
			QList<QGraphicsItem*> & graphicsItems = children[i]->graphicsItems;
			for (int j=0; j < graphicsItems.size(); ++j)
				if ((node = NodeGraphicsItem::cast(graphicsItems[j])) && node->scene() == vector->scene())
				{
					node->resetToDefaults();
					NodeGraphicsItem::Shape * shape1 = node->rightMostShape(),
																* shape2 = node->leftMostShape();
					if (shape1 && shape2 &&
						shape1->defaultBrush.color() == QColor(0,0,0,0) &&
						 shape2->defaultBrush.color() == QColor(0,0,0,0))
					{
                        QBrush black(QColor(0,0,0));
						shape1->defaultBrush = black;
						shape2->defaultBrush = black;
                        shape1->defaultPen = QPen(black,1);
                        shape1->defaultPen = QPen(black,1);
					}
				}
		}
		
		return new CompositeCommand(tr("plasmid adjusted"),commands);
	}

	/*****************************************
	            C API
	******************************************/

	typedef void (*tc_GRN_api) (
		tc_items (*f1)(long), 
		tc_items (*f2)(long), 
		tc_items (*f3)(long), 
		void (*f4)(tc_items),
		void (*f5)(long, tc_items)
	);

	void AutoGeneRegulatoryTool::setupFunctionPointers( QLibrary * library )
	{
		tc_GRN_api f = (tc_GRN_api)library->resolve("tc_AutoGeneRegulatoryTool_api");
		if (f)
		{
			f(
				&(_partsIn),
				&(_partsUpstream),
				&(_partsDownstream),
				&(_alignParts),
				&(_alignPartsOnPlasmid)
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
						if (NodeGraphicsItem::cast(items[i]->graphicsItems[j]) && items[i]->graphicsItems[j]->scene() == scene)
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
	
	void AutoGeneRegulatoryTool::_alignPartsOnPlasmid(long o, tc_items A)
	{
		fToS->alignPartsOnPlasmid(o,A);
	}
	
	void AutoGeneRegulatoryTool_FtoS::alignPartsOnPlasmid(long o, tc_items A)
	{
        QList<ItemHandle*> * list = ConvertValue(A);
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit alignPartsOnPlasmid(s, ConvertValue(o), *list);
		s->acquire();
		s->release();
		delete s;
		delete list;
	}

	void AutoGeneRegulatoryTool::_alignParts(tc_items A)
    {
        fToS->alignParts(A);
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

			if (node && node->handle())
			{
				QList<ItemHandle*> upstream, downstream;
				findAllParts(node,tr("Part"),upstream,true,QStringList());

				downstream.clear();
				if (!node->handle()->parent || !node->handle()->parent->isA(tr("Vector")))
				{
					downstream.push_front(node->handle());
					findAllParts(node,tr("Part"),downstream,false,QStringList());
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
        return fToS->partsIn(o);
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

			if (node && node->handle())
			{
				QList<ItemHandle*> upstream, downstream;
				findAllParts(node,tr("Part"),upstream,true,QStringList());

				downstream.clear();
				downstream.push_back(node->handle());
				//findAllParts(node,tr("Part"),downstream,false,QStringList());

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
        return fToS->partsUpstream(o);
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

			if (node && node->handle())
			{
				QList<ItemHandle*> downstream;
				//findAllParts(node,tr("Part"),upstream,true,QStringList());

				downstream.clear();
				//downstream.push_back(node->handle());
				findAllParts(node,tr("Part"),downstream,false,QStringList());

				(*parts) += downstream;
			}
		}
		if (s)
			s->release();
	}

	tc_items AutoGeneRegulatoryTool::_partsDownstream(long o)
    {
        return fToS->partsDownstream(o);
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
    
    void AutoGeneRegulatoryTool::alignPartsOnPlasmid(QSemaphore * sem, ItemHandle * plasmid, const QList<ItemHandle*>& partHandles)
    {
    	if (!plasmid || partHandles.isEmpty()) return;

    	GraphicsScene * scene = currentScene();
    	if (!scene || !scene->network) return;
    	
    	NodeGraphicsItem * plasmidNode = 0;
    	QList<QGraphicsItem*> partNodes;
    	
    	for (int i=0; i < plasmid->graphicsItems.size(); ++i)
    		if (NodeGraphicsItem::cast(plasmid->graphicsItems[i]) &&
    			plasmid->graphicsItems[i]->scene() == scene)
    			{
    				if (!plasmidNode)
    					plasmidNode = NodeGraphicsItem::cast(plasmid->graphicsItems[i]);
    				else
    				{
	    				QRectF rect1 = plasmidNode->sceneBoundingRect(),
	    							 rect2 = plasmid->graphicsItems[i]->sceneBoundingRect();
    					if ((rect1.width() * rect1.height()) < (rect2.width() * rect2.height()))
    						plasmidNode = NodeGraphicsItem::cast(plasmid->graphicsItems[i]);
    				}
    			}

		if (!plasmidNode) return;

    	for (int i=0; i < partHandles.size(); ++i)
			if (partHandles[i])
			{
				ItemHandle * part = partHandles[i];
				NodeGraphicsItem * partNode = 0;
				for (int j=0; j < part->graphicsItems.size(); ++j)
					if (NodeGraphicsItem::cast(part->graphicsItems[j]) &&
						part->graphicsItems[j]->scene() == scene)
						{
							if (!partNode)
								partNode = NodeGraphicsItem::cast(part->graphicsItems[j]);
							else
							{
								QRectF rect1 = partNode->sceneBoundingRect(),
											 rect2 = part->graphicsItems[j]->sceneBoundingRect();
								if ((rect1.width() * rect1.height()) < (rect2.width() * rect2.height()))
									partNode = NodeGraphicsItem::cast(part->graphicsItems[j]);
							}
						}
				if (partNode)
					partNodes << partNode;
			}

		if (partNodes.isEmpty()) return;
		
		QRectF rect = plasmidNode->sceneBoundingRect();
		qreal dx = (rect.width())/(1+partNodes.size());
		QList<QPointF> distances;
		
		for (int i=0; i < partNodes.size(); ++i)
		{
			distances += QPointF(rect.left(),rect.center().y()) + QPointF((i+1)*dx, -30.0) - partNodes[i]->scenePos();
		}
		
		scene->move(partNodes, distances);
		
		QUndoCommand * command = adjustPlasmid(scene, plasmidNode);
		scene->network->push(command);
		
		if (sem)
			sem->release();
	}

}

/*
extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::AutoGeneRegulatoryTool * tool = new Tinkercell::AutoGeneRegulatoryTool;
	main->addTool(tool);
}
*/

