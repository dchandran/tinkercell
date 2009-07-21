/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 Whenever a new connection item is created, this class adds control points to the item
 so that it looks appealing
 
****************************************************************************/

#include "Core/GraphicsScene.h"
#include "Core/UndoCommands.h"
#include "Core/MainWindow.h"
#include "Core/NodeGraphicsItem.h"
#include "Core/ConnectionGraphicsItem.h"
#include "Core/TextGraphicsItem.h"
#include "BasicTools/ConnectionMaker.h"
#include "BasicTools/DnaGraphicsItem.h"


namespace Tinkercell
{

	ConnectionMaker::ConnectionMaker() : Tool(tr("Connection Maker"))
	{ 
		mainWindow = 0;
		collisionDetection = 0;
	}
		
	bool ConnectionMaker::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow != 0)
		{
			if (main->tool("Collision Detection"))
			{
				QWidget * collisionDetection2 = main->tool("Collision Detection");
				collisionDetection = static_cast<CollisionDetection*>(collisionDetection2);
				if (collisionDetection != 0)
				{
					//connect(collisionDetection,SIGNAL(nodeCollided(const QList<QGraphicsItem*>&, NodeGraphicsItem*, const QList<QPointF>&, Qt::KeyboardModifiers)),
				    //        			  this,SLOT(nodeCollided(const QList<QGraphicsItem*>&, NodeGraphicsItem*, const QList<QPointF>&, Qt::KeyboardModifiers)));

					//connect(collisionDetection,SIGNAL(connectionCollided(const QList<QGraphicsItem*>&, ConnectionGraphicsItem *, const QList<QPointF>&, Qt::KeyboardModifiers)),
				    //        			  this,SLOT(connectionCollided(const QList<QGraphicsItem*>&, ConnectionGraphicsItem *, const QList<QPointF>&, Qt::KeyboardModifiers)));
				}
			}
			connect(main,SIGNAL(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),
					this,SLOT(itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));
			
			return (collisionDetection != 0);
		}
		
		return false;
	}

	void ConnectionMaker::itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& inserts, const QList<ItemHandle*>& )
	{
		if (!scene) return;
		static bool callBySelf = false;
		
		QList<QGraphicsItem*> oldItems, newItems;
		QList<QGraphicsItem*> items;
		
		if (!callBySelf)
		{
			for (int i=0; i < inserts.size(); ++i)
			{
				ConnectionGraphicsItem * connection = ConnectionGraphicsItem::topLevelConnectionItem(inserts.at(i));
				if (connection)
				{
					items << connection;
					
					/*ItemHandle * handle = getHandle(connection);
					if (handle && handle->family())
					{
						if (handle->family()->isA("Elongation") || (connection->className == DnaGraphicsItem::class_name))
						{
							ConnectionGraphicsItem* newConnection = new DnaGraphicsItem;
							(*newConnection) = (*connection);
							setHandle(newConnection,handle);
							newItems += newConnection;
							oldItems += connection;
							items << newConnection;
						}
					}*/
				}
			}
		}
		
		callBySelf = false;
		
		for (int i=0; i < items.size(); ++i)
		{
			ConnectionGraphicsItem * connection = ConnectionGraphicsItem::topLevelConnectionItem(items.at(i));
			if (connection)
			{
				int inputs = 0;
				QList<NodeGraphicsItem*> nodes = connection->nodes();
				QList<ArrowHeadItem*> arrowHeadsStart, arrowHeadsEnd;
				
				for (int j=0; j < connection->pathVectors.size(); ++j)
				{
					arrowHeadsStart += connection->pathVectors[j].arrowStart;
					arrowHeadsEnd += connection->pathVectors[j].arrowEnd;
				}

				for (int j=0; j < arrowHeadsStart.size() && j < arrowHeadsEnd.size(); ++j)
					if (arrowHeadsStart[j] == 0 && arrowHeadsEnd[j] == 0)
						++inputs;
				
				if (!connection->isValid())
				{
					for (int j=0; j < connection->pathVectors.size(); ++j)
					{
						connection->pathVectors[j].arrowStart = 0;
						connection->pathVectors[j].arrowEnd = 0;
					}
					
					connection->clear();
					makeSegments(scene, connection, nodes, inputs);
					connection->refresh();
					
					if (connection->itemHandle && connection->centerPoint())  //set location of text
					{
						TextGraphicsItem * textItem = 0;
						QPointF centerPoint = connection->centerLocation();
						
						QPointF p[] = { 	(centerPoint + QPointF(-20.0,0.0) ),
											(centerPoint + QPointF(20.0,0.0) ),
											(centerPoint + QPointF(0.0,-20.0) ),
											(centerPoint + QPointF(0.0,20.0) ),
											(centerPoint + QPointF(-20.0,-20.0) ),
											(centerPoint + QPointF(20.0,-20.0) ),
											(centerPoint + QPointF(-20.0,20.0) ),
											(centerPoint + QPointF(20.0,20.0) ) };
						for (int j=0; j < connection->itemHandle->graphicsItems.size(); ++j)
						{
							if ((textItem = qgraphicsitem_cast<TextGraphicsItem*>(connection->itemHandle->graphicsItems[j])) 
								&& !textItem->relativePosition.first)
							{
								for (int k=0; k < 8; ++k)
								{
									connection->itemHandle->graphicsItems[j]->setPos( p[k] );
									if (scene->items( connection->itemHandle->graphicsItems[j]->sceneBoundingRect() ).size() < 2)
										break;
								}
								textItem->relativePosition = 
									QPair<QGraphicsItem*,QPointF>(connection,connection->itemHandle->graphicsItems[j]->scenePos() - centerPoint);
							}
						}
					}
					
					if (connection->pathVectors.size() == 1 && arrowHeadsStart.size() > 1)
					{
						connection->pathVectors[0].arrowEnd = arrowHeadsStart[1];
						if (connection->pathVectors[0].arrowEnd) connection->pathVectors[0].arrowEnd->setVisible(true);
					}
					else
					{					
						for (int j=0; j < connection->pathVectors.size() && j < arrowHeadsStart.size() && j < arrowHeadsEnd.size(); ++j)
						{
							connection->pathVectors[j].arrowStart = arrowHeadsStart[j];
							connection->pathVectors[j].arrowEnd = arrowHeadsEnd[j];
							if (connection->pathVectors[j].arrowStart) connection->pathVectors[j].arrowStart->setVisible(true);
							if (connection->pathVectors[j].arrowEnd) connection->pathVectors[j].arrowEnd->setVisible(true);
						}
					}
					connection->refresh();
					connection->setControlPointsVisible(false);	
				}
				
			}
		}
		if (scene)
		{
			ConnectionGraphicsItem * connection = 0;

			if (!oldItems.isEmpty())
				scene->remove(tr("remove regular connection"),oldItems);

			if (!newItems.isEmpty())
			{
				callBySelf = true;
				scene->insert(tr("add DNA connection"),newItems);
			}
				
			for (int i=0; i < newItems.size(); ++i)
				if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(newItems[i])))
					connection->refresh();
		}
	}

	void ConnectionMaker::nodeCollided(const QList<QGraphicsItem*>& itemsMoved, NodeGraphicsItem * itemCollided, const QList<QPointF>& , Qt::KeyboardModifiers )
	{
		if (itemsMoved.size() != 1 || itemsMoved[0] == 0 || itemCollided == 0) return;
		ConnectionGraphicsItem::ControlPoint * cp = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(itemsMoved[0]);
		if (cp == 0 || cp->connectionItem == 0) return;

		cp->setPos( pointOnEdge(itemCollided->sceneBoundingRect(), cp->pos()) );
	}

	void ConnectionMaker::connectionCollided(const QList<QGraphicsItem*>& itemsMoved, ConnectionGraphicsItem * itemCollided, const QList<QPointF>& , Qt::KeyboardModifiers )
	{
		if (itemsMoved.size() != 1 || itemsMoved[0] == 0 || itemCollided == 0) return;
		ConnectionGraphicsItem::ControlPoint * cp = qgraphicsitem_cast<ConnectionGraphicsItem::ControlPoint*>(itemsMoved[0]);
		if (cp == 0 || cp->connectionItem == 0 || cp->connectionItem == itemCollided) return;

		ConnectionGraphicsItem * connection = cp->connectionItem;

		for (int i=0; i < connection->pathVectors.size(); ++i)
		{
			itemCollided->pathVectors.append(connection->pathVectors[i]);
			for (int j=0; j < connection->pathVectors[i].size(); ++j)
				connection->pathVectors[i][j]->connectionItem = itemCollided;
		}

		GraphicsScene * scene = 0;
		if (mainWindow) scene = mainWindow->currentScene();

		AddControlPointCommand ctrlPtCmnd(tr("new control point"),scene,cp);
		ctrlPtCmnd.redo();

		connection->pathVectors.clear();
                RemoveGraphicsCommand removeCmd(tr("remove"),scene,connection);
		removeCmd.redo();

		//AddControlPointCommand command(tr(),itemCollided->scene(),cp);
		//command.redo();
	}

	void ConnectionMaker::setupMiddleSegment(ConnectionGraphicsItem * connection, const QList<NodeGraphicsItem*>& nodes, int inputs)
	{
		if (nodes.size() < 2 || inputs < 1 || inputs >= nodes.size()) return;

		QList<NodeGraphicsItem*> fullList,
								 inputList,
								 outputList;
	    
		for (int i = 0; i < nodes.size(); ++i)
		{
			if (i < inputs)
			{
				if (nodes.at(i) && !inputList.contains(nodes.at(i)))
				{
					inputList.append(nodes.at(i));
					fullList.append(nodes.at(i));
				}
			}
			else
			{
				if (nodes.at(i) && !outputList.contains(nodes.at(i)))
				{
					outputList.append(nodes.at(i));
					fullList.append(nodes.at(i));
				}
			}
		}
		
		QPointF midpt1(0, 0), midpt2(0, 0), midpt(0, 0);
		for (int i=0; i < fullList.size(); ++i) //getting the midpoint for inputs and outputs
			if (fullList[i] != 0)
			{
				midpt.rx() += fullList[i]->scenePos().x();
				midpt.ry() += fullList[i]->scenePos().y();
			}

		midpt.rx() /= fullList.size();
		midpt.ry() /= fullList.size();

		QVector<QPointF> closePointsIn(inputList.size()),
						 closePointsOut(outputList.size());
	    
		for (int i = 0; i < inputList.size(); ++i)
		{
			closePointsIn[i] = pointOnEdge(inputList[i]->sceneBoundingRect(), midpt, 20.0);             
		}
		for (int i = 0; i < outputList.size(); ++i)
		{
			closePointsOut[i] = pointOnEdge(outputList[i]->sceneBoundingRect(), midpt, 20.0);
		}

		//find the midpoints again
		midpt1 = QPointF(0, 0); midpt2 = QPointF(0, 0); midpt = QPointF(0, 0);
	    
		for (int i=0; i < inputList.size(); ++i) //getting the midpoint for inputs
		{
			midpt1.rx() += closePointsIn[i].x();
			midpt1.ry() += closePointsIn[i].y();
		}
		for (int i=0; i < outputList.size(); ++i) //getting the midpoint for outputs
		{
			midpt2.rx() += closePointsOut[i].x();
			midpt2.ry() += closePointsOut[i].y();
		}

		midpt1.rx() /= inputList.size();   //midpoint between inputs points
		midpt1.ry() /= inputList.size();
	    
		midpt2.rx() /= outputList.size();   //midpoint between output points
		midpt2.ry() /= outputList.size();

		midpt = (midpt1 + midpt2) * 0.5;

		connection->pathVectors.clear();

		connection->pathVectors.append(ConnectionGraphicsItem::PathVector());

		if (inputs == 1 && nodes.size() == 2)
		{
			if (inputList[0] == outputList[0]) //self loop
			{
				connection->pathVectors[0].append(new ConnectionGraphicsItem::ControlPoint(midpt1,connection));
				connection->pathVectors[0].append(new ConnectionGraphicsItem::ControlPoint((midpt * 0.75 + midpt1 * 0.25),connection));
				connection->pathVectors[0].append(new ConnectionGraphicsItem::ControlPoint((midpt * 0.75 + midpt2 * 0.25),connection));
				connection->pathVectors[0].append(new ConnectionGraphicsItem::ControlPoint(midpt2,connection));
			}
			else
			{
				connection->pathVectors[0].append(new ConnectionGraphicsItem::ControlPoint(midpt1,connection));
				connection->pathVectors[0].append(new ConnectionGraphicsItem::ControlPoint((midpt + midpt1) * 0.5,connection));
				connection->pathVectors[0].append(new ConnectionGraphicsItem::ControlPoint((midpt + midpt2) * 0.5,connection));
				connection->pathVectors[0].append(new ConnectionGraphicsItem::ControlPoint(midpt2,connection));
			}
			return;
		}
		else
		{
			connection->pathVectors[0].append(new ConnectionGraphicsItem::ControlPoint((midpt + midpt1) * 0.5,connection));
			connection->pathVectors[0].append(new ConnectionGraphicsItem::ControlPoint(midpt,connection));
			connection->pathVectors[0].append(new ConnectionGraphicsItem::ControlPoint(midpt,connection));
			connection->pathVectors[0].append(new ConnectionGraphicsItem::ControlPoint((midpt + midpt2) * 0.5,connection));    
		}

		bool loops = false;

		for (int i=0; i < inputList.size(); ++i)  //are there any loops?
		{
			if (outputList.contains(inputList[i]))
			{
				loops = true;
				break;
			}
		}

		if (loops)
		{
			float dx = connection->pathVectors[0][1]->x() - connection->pathVectors[0][2]->x(),
				  dy = connection->pathVectors[0][1]->y() - connection->pathVectors[0][2]->y();

			connection->pathVectors[0][0]->setPos(
				QPointF(connection->pathVectors[0][1]->x() + 0.25*dy,
						connection->pathVectors[0][1]->y() + 0.25*dx));
			connection->pathVectors[0][3]->setPos(
				QPointF(connection->pathVectors[0][1]->x() - 0.25*dy,
						connection->pathVectors[0][1]->y() - 0.25*dx));
		}
	}

	void ConnectionMaker::makeSegments(GraphicsScene* scene,ConnectionGraphicsItem * connection, const QList<NodeGraphicsItem*>& nodes, int inputs)
	{
		if (nodes.size() < 2 || inputs < 1 || inputs >= nodes.size()) return;

		int k = 0;

		setupMiddleSegment(connection, nodes, inputs);

		ConnectionGraphicsItem::PathVector middlePiece = connection->pathVectors[0];
		connection->pathVectors.clear();

		if (middlePiece.size() != 4) 
		{
			return;
		}

		QPointF center;

		if (nodes.size() == 2)  //just two points to connect
		{
			ConnectionGraphicsItem::PathVector vector;
			center = nodes.at(0)->scenePos();
			vector.append(new ConnectionGraphicsItem::ControlPoint(nodes.at(0)->mapFromScene(pointOnEdge(nodes.at(0)->sceneBoundingRect(),(center + middlePiece[0]->scenePos()) * 0.5)), connection, nodes.at(0) ));
			vector.append(middlePiece[1]);
			vector.append(middlePiece[2]);   
			center = nodes.at(1)->scenePos();
			vector.append(new ConnectionGraphicsItem::ControlPoint(nodes.at(1)->mapFromScene(pointOnEdge(nodes.at(1)->sceneBoundingRect(),(center + middlePiece[3]->scenePos()) * 0.5)),connection, nodes.at(1)) );
			connection->pathVectors.append(vector);
			
			delete middlePiece[0];
			delete middlePiece[3];
		}
		else  //multiple points to connect
		{
			k = 0;
			for (int i=0; i < nodes.size(); ++i)
			{
				center = nodes.at(i)->scenePos();
				if (k < inputs)
				{
					ConnectionGraphicsItem::PathVector vector;
					QPointF p = pointOnEdge(nodes.at(i)->sceneBoundingRect(),(center + middlePiece[0]->scenePos()) * 0.5);
					vector.append(new ConnectionGraphicsItem::ControlPoint(nodes.at(i)->mapFromScene(p), connection, nodes.at(i) ));
					vector.append(new ConnectionGraphicsItem::ControlPoint((p + middlePiece[0]->scenePos()) * 0.5,connection));
					vector.append(middlePiece[0]);
					vector.append(middlePiece[1]);

					connection->pathVectors.append(vector);

				}
				else
				{
					
					ConnectionGraphicsItem::PathVector vector;
					QPointF p = pointOnEdge(nodes.at(i)->sceneBoundingRect(),(center + middlePiece[3]->scenePos()) * 0.5);
					vector.append(new ConnectionGraphicsItem::ControlPoint(nodes.at(i)->mapFromScene(p),connection, nodes.at(i)) );
					vector.append(new ConnectionGraphicsItem::ControlPoint((p + middlePiece[3]->scenePos()) * 0.5,connection));
					vector.append(middlePiece[3]);
					vector.append(middlePiece[1]);

					connection->pathVectors.append(vector);
				}
				++k;
			}
			delete middlePiece[2];
		}

		FixMultipleConnections(connection,nodes,inputs);
		connection->lineType = ConnectionGraphicsItem::bezier;
		
		for (int i=0; i < nodes.size(); ++i) //line type = line if any other connection is a line
			if (nodes[i])
			{
				QList<ConnectionGraphicsItem*> otherConnections = nodes[i]->connections();
				for (int j=0; j < otherConnections.size(); ++j)
					if (otherConnections[j] && otherConnections[j]->lineType == ConnectionGraphicsItem::line)
					{
						connection->lineType = ConnectionGraphicsItem::line;
						break;
					}
				if (connection->lineType == ConnectionGraphicsItem::line)
					break;
			}
			
		if (connection->itemHandle && 
			(connection->itemHandle->isA(tr("Transcription Regulation")) || 
			connection->itemHandle->isA(tr("Elongation")) || 
			connection->itemHandle->isA(tr("Transcription"))) 
			&& nodes.size() > 1 && nodes[0] && nodes[1])
		{
			ConnectionGraphicsItem::ControlPoint * cp;
			if (connection->itemHandle->isA(tr("Elongation")))
			{
				cp = new ConnectionGraphicsItem::ControlPoint(QPointF((nodes[1]->scenePos().x() + nodes[0]->scenePos().x())/2.0,nodes[1]->scenePos().y()),connection);
				connection->defaultPen.setStyle(Qt::DashLine);
				connection->setPen(connection->defaultPen);
				AddControlPointCommand command(tr(""),scene,cp);
				command.redo();
			}
			else
			if (connection->itemHandle->isA(tr("Transcription Regulation")))
			{
				if (nodes[1]->scenePos().x() > nodes[0]->scenePos().x())
					cp = new ConnectionGraphicsItem::ControlPoint(QPointF(nodes[1]->sceneBoundingRect().left() + 20.0,nodes[0]->scenePos().y()),connection);
				else
					cp = new ConnectionGraphicsItem::ControlPoint(QPointF(nodes[1]->sceneBoundingRect().right() - 20.0,nodes[0]->scenePos().y()),connection);
			
				if (nodes[0]->sceneBoundingRect().contains(cp->pos()))
				{
					cp->setPos( cp->pos() + QPointF( 0.0, 1.5 * nodes[0]->sceneBoundingRect().height() ) );
				}
				AddControlPointCommand command(tr(""),scene,cp);
				command.redo();
			}
			connection->lineType = ConnectionGraphicsItem::line;
		}
	}

	void ConnectionMaker::FixMultipleConnections(ConnectionGraphicsItem * connection, const QList<NodeGraphicsItem*>& nodes, int inputs)
	{
		if (nodes.size() < 2 || inputs < 1 || inputs >= nodes.size()) return;
		QPointF center;
		float invslope, size;
	    
		for (int i = 1; i < connection->pathVectors.size(); ++i)
		{
			for (int j = 0; j < i; ++j)
			{
				ControlPoint * p1 = connection->pathVectors[i][ connection->pathVectors[i].size() - 2 ];
				ControlPoint * p2 = connection->pathVectors[j][ connection->pathVectors[j].size() - 2 ];

				if (p1 != 0 && p2 != 0)
				{
					if ( connection->nodeAt(i) != 0 && connection->nodeAt(i) == connection->nodeAt(j) )
					{
						center = connection->nodeAt(i)->sceneBoundingRect().center();						
						
						size = 80.0;

						invslope = (p1->scenePos().x() - center.x()) / (p1->scenePos().y() - center.y());
						if (invslope < 0) invslope = -invslope;

						if (size > size*invslope)
						{
							p1->setPos( (QPointF( p1->scenePos().x() - size, p1->scenePos().y() -  size*invslope)));
							p2->setPos( (QPointF( p2->scenePos().x() + size, p2->scenePos().y() +  size*invslope)));
						}
						else
						{
							p1->setPos( (QPointF( p1->pos().x() - size, p1->pos().y() -  size)));
							p2->setPos( (QPointF( p2->pos().x() + size, p2->pos().y() +  size)));
						}

					}
				}
			}
		}
	}

}


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::ConnectionMaker * connectionMaker = new Tinkercell::ConnectionMaker;
	main->addTool(connectionMaker);

}

