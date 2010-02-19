/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool allows the loading and saving of models.

****************************************************************************/
#include "NodesTree.h"
#include "ConnectionsTree.h"
#include "LoadSaveTool.h"
#include "CThread.h"
#include "ConsoleWindow.h"
#include <QtDebug>
#include <QRegExp>
#include <QMessageBox>

namespace Tinkercell
{
	LoadSaveTool::LoadSaveTool() : Tool(tr("Save and Load"),tr("Basic GUI"))
	{
	}

	void LoadSaveTool::historyChanged(int)
	{
		GraphicsScene * scene = currentScene();
		if (scene)
			savedScenes[scene] = false;
	}

	void LoadSaveTool::windowClosing(NetworkWindow * win, bool * close)
	{
		if (mainWindow && win && close && (*close) && savedScenes.contains(win->scene) && !savedScenes.value(win->scene))
		{
			QString title = tr("Save");
			if (mainWindow->currentWindow())
				title = mainWindow->currentWindow()->windowTitle();
			QMessageBox::StandardButton button = QMessageBox::question(
				win,
				title,
				tr("Save before closing?"),
				QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel,
				QMessageBox::Ok);
			if (button == QMessageBox::Ok)
			{
				mainWindow->saveWindow();
			}
			else
				if (button == QMessageBox::Cancel)
					(*close) = false;
			
			if (*close)
				savedScenes.remove(win->scene);
		}
	}

	bool LoadSaveTool::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(saveModel(const QString&)),this,SLOT(saveModel(const QString&)));
			connect(mainWindow,SIGNAL(loadModel(const QString&)),this,SLOT(loadModel(const QString&)));
			connect(mainWindow,SIGNAL(windowClosing(NetworkWindow * , bool *)),this,SLOT(windowClosing(NetworkWindow * , bool *)));
			connect(mainWindow,SIGNAL(historyChanged( int )),this,SLOT(historyChanged( int )));

			connect(mainWindow,SIGNAL(prepareModelForSaving(NetworkWindow*,bool*)),
					this,SLOT(prepareModelForSaving(NetworkWindow*,bool*)));
			connect(this,SIGNAL(modelSaved(NetworkWindow*)),mainWindow,SIGNAL(modelSaved(NetworkWindow*)));
			connect(this,SIGNAL(modelLoaded(NetworkWindow*)),mainWindow,SIGNAL(modelLoaded(NetworkWindow*)));
			return true;
		}
		return false;
	}

	void LoadSaveTool::prepareModelForSaving(NetworkWindow*, bool* b)
	{
		if (currentScene() && b)
			(*b) = true;
	}

	void LoadSaveTool::writeNode(NodeGraphicsItem* node, QXmlStreamWriter& modelWriter)
	{
		if (!node) return;
		ItemHandle * handle = getHandle(node);
		if (!handle) return; //don't write arrow heads and decorators

		modelWriter.writeStartElement("NodeItem");
		modelWriter.writeAttribute("className",node->className);
		if (handle)
			modelWriter.writeAttribute("handle",handle->fullName(tr(".")));
		else
			modelWriter.writeAttribute("handle","");
		if (node->isVisible())
			modelWriter.writeAttribute("visible","yes");
		else
			modelWriter.writeAttribute("visible","no");
		modelWriter.writeAttribute("z",QString::number(node->zValue()));

		QTransform t1 = node->sceneTransform();
		QPointF pos = node->scenePos();

		modelWriter.writeStartElement("pos");
		modelWriter.writeAttribute("x",QString::number(pos.x()));
		modelWriter.writeAttribute("y",QString::number(pos.y()));
		modelWriter.writeEndElement();

		modelWriter.writeStartElement("transform");
		modelWriter.writeAttribute("m11",QString::number(t1.m11()));
		modelWriter.writeAttribute("m12",QString::number(t1.m12()));
		modelWriter.writeAttribute("m21",QString::number(t1.m21()));
		modelWriter.writeAttribute("m22",QString::number(t1.m22()));
		modelWriter.writeEndElement();

		NodeGraphicsWriter::writeNodeGraphics(node,&modelWriter);
		modelWriter.writeEndElement();
	}

	void LoadSaveTool::writeConnection(ConnectionGraphicsItem* connection, QXmlStreamWriter& modelWriter)
	{
		if (!connection) return;
		ItemHandle * handle = getHandle(connection);

		modelWriter.writeStartElement(tr("ConnectionItem"));
		modelWriter.writeAttribute(tr("className"),connection->className);

		if (handle)
			modelWriter.writeAttribute(tr("handle"),handle->fullName(tr(".")));
		else
			modelWriter.writeAttribute(tr("handle"),tr(""));
		if (connection->isVisible())
			modelWriter.writeAttribute("visible","yes");
		else
			modelWriter.writeAttribute("visible","no");
		modelWriter.writeAttribute("z",QString::number(connection->zValue()));

		ConnectionGraphicsWriter::writeConnectionGraphics(connection,&modelWriter);
		modelWriter.writeEndElement();
	}

	void LoadSaveTool::writeText(TextGraphicsItem* text, QXmlStreamWriter& modelWriter)
	{
		if (!text || !text->isVisible()) return;

		modelWriter.writeStartElement(tr("TextItem"));
		modelWriter.writeAttribute(tr("text"),text->toPlainText());
		ItemHandle * handle = getHandle(text);
		if (handle)
			modelWriter.writeAttribute(tr("handle"),handle->fullName());
		else
			modelWriter.writeAttribute(tr("handle"),tr(""));

		QPointF pos = text->scenePos();
		modelWriter.writeAttribute(tr("x"),QString::number(pos.x()));
		modelWriter.writeAttribute(tr("y"),QString::number(pos.y()));
		modelWriter.writeAttribute(tr("color"),text->defaultTextColor().name());

		QTransform transform = text->transform();
		modelWriter.writeAttribute(tr("m11"),QString::number(transform.m11()));
		modelWriter.writeAttribute(tr("m12"),QString::number(transform.m12()));
		modelWriter.writeAttribute(tr("m21"),QString::number(transform.m21()));
		modelWriter.writeAttribute(tr("m22"),QString::number(transform.m22()));

		modelWriter.writeAttribute(tr("z"),QString::number(text->zValue()));

		QFont font = text->font();
		modelWriter.writeAttribute(tr("font"),font.family());
		modelWriter.writeAttribute(tr("size"),QString::number(font.pointSize()));
		modelWriter.writeAttribute(tr("bold"),QString::number((int)font.bold()));
		modelWriter.writeAttribute(tr("italics"),QString::number((int)font.italic()));
		modelWriter.writeAttribute(tr("underline"),QString::number((int)font.underline()));

		modelWriter.writeEndElement();
	}

	void LoadSaveTool::saveItems(const QList<QGraphicsItem*>& allitems, const QString& filename)
	{
		if (allitems.isEmpty() || filename.isEmpty()) return;

		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;
		TextGraphicsItem * text = 0;

		QFile file (filename);

		if (!file.open(QFile::WriteOnly | QFile::Text))
		{
			mainWindow->statusBar()->showMessage(tr("file cannot be opened : ") + filename);
			if (console())
                console()->error(tr("file cannot be opened : ") + filename);
			return;
		}

		ModelWriter modelWriter;
		modelWriter.setDevice(&file);
		modelWriter.setAutoFormatting(true);

		modelWriter.writeStartDocument();
		modelWriter.writeDTD("<!DOCTYPE TinkerCell>");

		modelWriter.writeStartElement("Model");

		modelWriter.writeStartElement("Handles");

		QList<ItemHandle*> handles;
		ItemHandle * handle;
		for (int i=0; i < allitems.size(); ++i)
		{
			if ((handle = getHandle(allitems[i]))
				&& !handles.contains(handle))
			{
				handles << handle;
			}
		}

		modelWriter.writeModel(handles,&file);
		modelWriter.writeEndElement();

		QList<NodeGraphicsItem*> nodeItems;
		QList<TextGraphicsItem*> textItems;
		QList<ConnectionGraphicsItem*> connectionItems;

		for (int i=0; i < allitems.size(); ++i)
		{
			node = NodeGraphicsItem::topLevelNodeItem(allitems[i]);
			if (node && !nodeItems.contains(node))
			{
				nodeItems << node;
			}
			else
			{
				connection = ConnectionGraphicsItem::topLevelConnectionItem(allitems[i]);
				if (connection && !connectionItems.contains(connection))
				{
					connectionItems << connection;
				}
				else
				{
					text = TextGraphicsItem::cast(allitems[i]);
					if (text && !textItems.contains(text))
					{
						textItems << text;
					}
				}
			}
		}

		modelWriter.writeStartElement(tr("Nodes"));
		for (int i=0; i < nodeItems.size(); ++i)
		{
			node = nodeItems[i];
			writeNode(node,modelWriter);
		}
		modelWriter.writeEndElement();

		modelWriter.writeStartElement(tr("Connections"));
		QList<ConnectionGraphicsItem*> firstSetofConnections;
		for (int i=0; i < connectionItems.size(); ++i)
		{
			if (connectionItems[i] && connectionItems[i]->centerRegionItem &&
				connectionItems[i]->centerRegionItem->scene() &&
				!connectionItems[i]->centerRegionItem->connections().isEmpty())
			{
				firstSetofConnections += connectionItems[i];
				connectionItems.removeAt(i);
				--i;
			}
		}
		for (int i=0; i < firstSetofConnections.size(); ++i)
		{
			connection = firstSetofConnections[i];
			writeConnection(connection,modelWriter);
		}
		for (int i=0; i < connectionItems.size(); ++i)
		{
			connection = connectionItems[i];
			writeConnection(connection,modelWriter);
		}
		modelWriter.writeEndElement();

		modelWriter.writeStartElement(tr("Texts"));
		for (int i=0; i < textItems.size(); ++i)
		{
			text = textItems[i];
			writeText(text,modelWriter);
		}
		modelWriter.writeEndElement();

		modelWriter.writeEndElement();
		modelWriter.writeEndDocument();
	}

	void LoadSaveTool::saveModel(const QString& filename)
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		QList<QGraphicsItem*> allitems = scene->items();
		NodeGraphicsItem * node = 0;
		ConnectionGraphicsItem * connection = 0;
		TextGraphicsItem * text = 0;

		QFile file (filename);

		if (!file.open(QFile::WriteOnly | QFile::Text))
		{
			mainWindow->statusBar()->showMessage(tr("file cannot be opened : ") + filename);
			if (console())
                console()->error(tr("file cannot be opened : ") + filename);
			//qDebug() << "file cannot be opened : " << filename;
			return;
		}

		ModelWriter modelWriter;
		modelWriter.setDevice(&file);
		modelWriter.setAutoFormatting(true);

		modelWriter.writeStartDocument();
		modelWriter.writeDTD("<!DOCTYPE TinkerCell>");

		modelWriter.writeStartElement("Model");

		modelWriter.writeStartElement("Handles");
		modelWriter.writeModel(scene,&file);
		modelWriter.writeEndElement();

		QList<NodeGraphicsItem*> nodeItems;
		QList<TextGraphicsItem*> textItems;
		QList<ConnectionGraphicsItem*> connectionItems;

		for (int i=0; i < allitems.size(); ++i)
		{
			node = NodeGraphicsItem::topLevelNodeItem(allitems[i]);
			if (node && !nodeItems.contains(node))
			{
				nodeItems << node;
			}
			else
			{
				connection = ConnectionGraphicsItem::topLevelConnectionItem(allitems[i]);
				if (connection && !connectionItems.contains(connection))
				{
					connectionItems << connection;
				}
				else
				{
					text = TextGraphicsItem::cast(allitems[i]);
					if (text && !textItems.contains(text))
					{
						textItems << text;
					}
				}
			}
		}

		modelWriter.writeStartElement(tr("Nodes"));
		for (int i=0; i < nodeItems.size(); ++i)
		{
			node = nodeItems[i];
			writeNode(node,modelWriter);
		}
		modelWriter.writeEndElement();

		modelWriter.writeStartElement(tr("Connections"));
		QList<ConnectionGraphicsItem*> firstSetofConnections;
		for (int i=0; i < connectionItems.size(); ++i)
		{
			if (connectionItems[i] && connectionItems[i]->centerRegionItem &&
				connectionItems[i]->centerRegionItem->scene() &&
				!connectionItems[i]->centerRegionItem->connections().isEmpty())
			{
				firstSetofConnections += connectionItems[i];
				connectionItems.removeAt(i);
				--i;
			}
		}
		for (int i=0; i < firstSetofConnections.size(); ++i)
		{
			connection = firstSetofConnections[i];
			writeConnection(connection,modelWriter);
		}
		for (int i=0; i < connectionItems.size(); ++i)
		{
			connection = connectionItems[i];
			writeConnection(connection,modelWriter);
		}
		modelWriter.writeEndElement();

		modelWriter.writeStartElement(tr("Texts"));
		for (int i=0; i < textItems.size(); ++i)
		{
			text = textItems[i];
			writeText(text,modelWriter);
		}
		modelWriter.writeEndElement();

		modelWriter.writeEndElement();
		modelWriter.writeEndDocument();

		savedScenes[scene] = true;

		QRegExp regex(tr(".*\\/([^\\/]+)\\.\\S+$"));
		QString filename2 = filename;
		if (regex.indexIn(filename) >= 0)
			filename2 = regex.cap(1);

		if (mainWindow->currentWindow())
		{
			mainWindow->currentWindow()->setWindowTitle(filename2);
			mainWindow->currentWindow()->filename = filename;
			//qDebug() << filename;
		}

		emit modelSaved(scene->networkWindow);

		mainWindow->statusBar()->showMessage(tr("model successfully saved in : ") + filename);
		if (console())
            console()->message(tr("model successfully saved in : ") + filename);
	}

	void LoadSaveTool::loadModel(const QString& filename)
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !scene->allHandles().isEmpty())
		{
			mainWindow->newGraphicsWindow();
		}

		scene = currentScene();

		if (!scene) return;

		QList<QGraphicsItem*> items;
		loadItems(items,filename);

		if (items.size() > 0)
		{
		    scene->insert(tr("load"),items);

			ConnectionGraphicsItem * connection = 0;

			for (int i=0; i < items.size(); ++i)
			{
				if ((connection = ConnectionGraphicsItem::cast(items[i])))
				{
					connection->refresh();
					connection->setControlPointsVisible(false);
				}
			}

			scene->fitAll();

			if (scene->historyStack)
				scene->historyStack->clear();

			savedScenes[scene] = true;

			QRegExp regex(tr(".*\\/([^\\/]+)\\.\\S+$"));
			QString filename2 = filename;
			if (regex.indexIn(filename) >= 0)
				filename2 = regex.cap(1);

			if (mainWindow->currentWindow())
			{
				mainWindow->currentWindow()->setWindowTitle(filename2);
				mainWindow->currentWindow()->filename = filename;
			}

			emit modelLoaded(scene->networkWindow);
		}
	}

	void LoadSaveTool::loadItems(QList<QGraphicsItem*>& items, const QString& filename)
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		if (!mainWindow->tool(tr("Nodes Tree")) || !mainWindow->tool(tr("Connections Tree")))
		{
			//if (console())
                //console()->error(tr("No Nodes or Connections set available."));
			QMessageBox::information(this,tr("Error"),tr("No Nodes or Connections set available."));
			return;
		}

		NodesTree * nodesTree = static_cast<NodesTree*>(mainWindow->tool(tr("Nodes Tree")));
		ConnectionsTree * connectionsTree = static_cast<ConnectionsTree*>(mainWindow->tool(tr("Connections Tree")));

		QFile file1 (filename);

		if (!file1.open(QFile::ReadOnly | QFile::Text))
		{
			mainWindow->statusBar()->showMessage(tr("file cannot be opened : ") + filename);
			//if (console())
              //  console()->error(tr("file cannot be opened : ") + filename);

			QMessageBox::information(this,tr("Error"),tr("file cannot be opened : ") + filename);
			return;
		}

		//find starting point for the handles
		ModelReader modelReader;
		modelReader.setDevice(&file1);

		while (!modelReader.atEnd() && !(modelReader.isStartElement() && modelReader.name() == "Model"))
		{
			modelReader.readNext();
		}

		if (modelReader.atEnd()) //not a TinkerCell file
		{
			file1.close();
			return;
		}

		while (!modelReader.atEnd() && !(modelReader.isStartElement() && modelReader.name() == "Handles"))
		{
			modelReader.readNext();
		}

		if (modelReader.atEnd()) //not a TinkerCell file
		{
			file1.close();
			return;
		}

		modelReader.readNext();

		//read all the handles
		QList<QPair<QString,ItemHandle*> > handlesList = modelReader.readHandles(scene,&file1);

		QHash<QString,ItemHandle*> handlesHash;  //hash name->handle
		NodeHandle * nodeHandle;
		ConnectionHandle * connectionHandle;

		for (int i=0; i < handlesList.size(); ++i)
		{
			if (handlesList[i].second)
			{
				if (handlesList[i].second->type == NodeHandle::TYPE)
				{
					nodeHandle = static_cast<NodeHandle*>(handlesList[i].second);
					if (nodesTree->nodeFamilies.contains(handlesList[i].first))
						nodeHandle->setFamily( nodesTree->nodeFamilies[handlesList[i].first] );
				}
				else
					if (handlesList[i].second->type == ConnectionHandle::TYPE)
					{
						connectionHandle = static_cast<ConnectionHandle*>(handlesList[i].second);
						if (connectionsTree->connectionFamilies.contains(handlesList[i].first))
						{
							connectionHandle->setFamily( connectionsTree->connectionFamilies[handlesList[i].first] );
						}
					}
					if (handlesList[i].second->family())
						handlesHash[handlesList[i].second->fullName()] = handlesList[i].second;
			}
		}

		file1.close();

		//find starting point for nodes
		QFile file2 (filename);

		if (!file2.open(QFile::ReadOnly | QFile::Text))
		{
			mainWindow->statusBar()->showMessage(tr("file cannot be opened") + filename);
			if (console())
                console()->error(tr("file cannot be opened : ") + filename);

			//qDebug() << "file cannot be opened";
			return;
		}

		QList<NodeGraphicsItem*> nodes;
		QList<QTransform> transforms;
		QList<QPointF> points;
		QList<qreal> zValues;
		QList<bool> visibles;

		//find starting point for nodes
		NodeGraphicsReader nodeReader;
		nodeReader.setDevice(&file2);

		while (!nodeReader.atEnd() && !(nodeReader.isStartElement() && nodeReader.name() == "Nodes"))
		{
			nodeReader.readNext();
		}

		//read all the nodes
		while (!nodeReader.atEnd() && !(nodeReader.isEndElement() && nodeReader.name() == "Nodes"))
		{
			QString s;
			QTransform t;
			QPointF p;
			qreal z;
			bool v;
			NodeGraphicsItem * node = readNode(nodeReader,s,t,p,z,v);
			if (node)
			{
				if (!s.isEmpty() && handlesHash.contains(s))
					setHandle(node,handlesHash[s]);  //very important
				transforms << t;
				points << p;
				items << node;
				nodes << node;
				zValues << z;
				visibles << v;
			}
			nodeReader.readNext();
		}
		//read all connections
		while (!nodeReader.atEnd() && !(nodeReader.isStartElement() && nodeReader.name() == "Connections"))
		{
			nodeReader.readNext();
		}
		QList<ConnectionGraphicsItem*> connections;
		while (!nodeReader.atEnd() && !(nodeReader.isEndElement() && nodeReader.name() == "Connections"))
		{
			QString s;
			qreal z;
			bool v;
			ConnectionGraphicsItem * connection = readConnection(nodeReader,nodes,connections,s,z,v);
			if (connection)
			{
				if (!s.isEmpty() && handlesHash.contains(s))
					setHandle(connection,handlesHash[s]);  //very important
				transforms << QTransform();
				items << connection;
				connections << connection;
				points << QPointF(0,0);
				zValues << z;
				visibles << v;
				QList<ConnectionGraphicsItem::ControlPoint*> controlPoints = connection->controlPoints(true);
				for (int i=0; i < controlPoints.size(); ++i)
				{
					if (controlPoints[i])
					{
						transforms << QTransform();
						items << controlPoints[i];
						points << controlPoints[i]->pos();
						zValues << (z+0.1);
						visibles << false;
					}
				}
				QList<ArrowHeadItem*> arrowHeads;
				arrowHeads	<< connection->arrowHeads() 
							<< connection->modifierArrowHeads();

				if (connection->centerRegionItem)
						arrowHeads << connection->centerRegionItem;

				for (int i=0; i < arrowHeads.size(); ++i)
				{
					if (arrowHeads[i] && (arrowHeads.indexOf(arrowHeads[i]) == i))
					{
						console()->message(arrowHeads[i]->fileName);
						transforms << arrowHeads[i]->transform();
						points << arrowHeads[i]->pos();
						items << arrowHeads[i];
						zValues << (z+0.1);
						visibles << true;
					}
				}
			}
			nodeReader.readNext();
		}

		//read all texts
		while (!nodeReader.atEnd() && !(nodeReader.isStartElement() && nodeReader.name() == "Texts"))
		{
			nodeReader.readNext();
		}
		while (!nodeReader.atEnd() && !(nodeReader.isEndElement() && nodeReader.name() == "Texts"))
		{
			QString s;
			QTransform t;
			QPointF p;
			qreal z;
			TextGraphicsItem * text = readText(nodeReader,s,t,p,z);
			if (text)
			{
				if (!s.isEmpty() && handlesHash.contains(s))
					setHandle(text,handlesHash[s]);  //very important
				transforms << t;
				points << p;
				items << text;
				zValues << z;
				visibles << true;
			}
			nodeReader.readNext();
		}
		file2.close();

		if (items.size() > 0)
		{
			for (int i=0; i < items.size(); ++i)
			{
				items[i]->resetTransform();
				items[i]->setPos(QPointF(0,0));
				items[i]->setTransform(transforms[i]);
				items[i]->setPos(points[i]);
				items[i]->setZValue(zValues[i]);
				items[i]->setVisible(visibles[i]);
			}
		}

	}

	TextGraphicsItem * LoadSaveTool::readText(QXmlStreamReader & nodeReader,QString& handle, QTransform& transform,QPointF& pos, qreal& z)
	{
		if (nodeReader.isStartElement() && nodeReader.name() == "TextItem")
		{
			bool ok;
			TextGraphicsItem * text = new TextGraphicsItem;
			qreal m11=0,m21=0,m12=0,m22=0;
			QFont font;
			QXmlStreamAttributes attribs = nodeReader.attributes();
			for (int i=0; i < attribs.size(); ++i)
			{
				if (attribs[i].name().toString() == tr("text"))
					text->setPlainText(attribs[i].value().toString());
				else
					if (attribs[i].name().toString() == tr("handle"))
					{
						handle = attribs[i].value().toString();
					}
					else
						if (attribs[i].name().toString() == tr("x"))
							pos.rx() = attribs[i].value().toString().toDouble(&ok);
						else
							if (attribs[i].name().toString() == tr("y"))
								pos.ry() = attribs[i].value().toString().toDouble(&ok);
							else
								if (attribs[i].name().toString() == tr("color"))
									text->setDefaultTextColor(QColor(attribs[i].value().toString()));
								else
									if (attribs[i].name().toString() == tr("m11"))
										m11 = attribs[i].value().toString().toDouble(&ok);
									else
										if (attribs[i].name().toString() == tr("m12"))
											m12 = attribs[i].value().toString().toDouble(&ok);
										else
											if (attribs[i].name().toString() == tr("m21"))
												m21 = attribs[i].value().toString().toDouble(&ok);
											else
												if (attribs[i].name().toString() == tr("m22"))
													m22 = attribs[i].value().toString().toDouble(&ok);
												else
													if (attribs[i].name().toString() == tr("z"))
														z = attribs[i].value().toString().toDouble(&ok);
													else
														if (attribs[i].name().toString() == tr("font"))
															font.setFamily(attribs[i].value().toString());
														else
															if (attribs[i].name().toString() == tr("size"))
																font.setPointSize(attribs[i].value().toString().toInt(&ok));
															else
																if (attribs[i].name().toString() == tr("bold"))
																	font.setBold(attribs[i].value().toString().toInt(&ok)==1);
																else
																	if (attribs[i].name().toString() == tr("italics"))
																		font.setItalic(attribs[i].value().toString().toInt(&ok)==1);
																	else
																		if (attribs[i].name().toString() == tr("underline"))
																			font.setUnderline(attribs[i].value().toString().toInt(&ok)==1);
			}
			text->setFont(font);
			transform.setMatrix(m11,m12,0.0, m21, m22, 0.0, 0.0, 0.0, 1.0);

			return text;
		}
		return 0;
	}

	ConnectionGraphicsItem * LoadSaveTool::readConnection(NodeGraphicsReader & nodeReader,QList<NodeGraphicsItem*>& nodes, QList<ConnectionGraphicsItem*>& connections, QString& handle, qreal& z, bool& visible)
	{
		if (nodeReader.isStartElement() && nodeReader.name() == "ConnectionItem")
		{
			QFont font;
			QXmlStreamAttributes attribs = nodeReader.attributes();
			ConnectionGraphicsItem * connection = 0;
			QString type;
			for (int i=0; i < attribs.size(); ++i)
			{
				if (attribs[i].name().toString() == tr("className"))
				{
					type = attribs[i].value().toString();
				}
				else
					if (attribs[i].name().toString() == tr("handle"))
					{
						handle = attribs[i].value().toString();
					}
					else
						if (attribs[i].name().toString() == tr("z"))
						{
							bool ok;
							z = attribs[i].value().toString().toDouble(&ok);
							if (!ok) z = 1.0;
						}
						else
							if (attribs[i].name().toString() == tr("visible"))
							{
								visible = (attribs[i].value().toString().toLower() == QString("yes"));
							}
			}

			while (!nodeReader.atEnd() && !(nodeReader.isEndElement() && nodeReader.name() == QObject::tr("ConnectionItem")))
			{
				if (nodeReader.name() == "ConnectionGraphicsItem")
				{
					connection = ConnectionGraphicsReader::readConnectionGraphics(nodes,connections, &nodeReader);
				}
				nodeReader.readNext();
			}
			if (connection && !type.isEmpty())
				connection->className = type;
			return connection;
		}
		return 0;
	}

	NodeGraphicsItem * LoadSaveTool::readNode(NodeGraphicsReader & reader,QString& handle, QTransform& transform,QPointF& pos, qreal& z, bool& visible)
	{
		if (!(reader.isStartElement() && reader.name() == QObject::tr("NodeItem"))) return 0;

		QXmlStreamAttributes attribs = reader.attributes();
		QString type;
		for (int i=0; i < attribs.size(); ++i)
		{
			if (attribs[i].name().toString() == tr("className"))
			{
				type = attribs[i].value().toString();
			}
			else
				if (attribs.at(i).name().toString() == tr("handle"))
				{
					handle = attribs[i].value().toString();
				}
				else
					if (attribs[i].name().toString() == tr("z"))
					{
						bool ok;
						z = attribs[i].value().toString().toDouble(&ok);
						if (!ok) z = 1.0;
					}
					else
						if (attribs[i].name().toString() == tr("visible"))
						{
							visible = (attribs[i].value().toString().toLower() == QString("yes"));
						}
		}

		NodeGraphicsItem * node = 0;

		if (type == ArrowHeadItem::CLASSNAME)
			node = new ArrowHeadItem;
		else
			node = new NodeGraphicsItem;

		if (!type.isEmpty()) node->className = type;

		qreal n=0,m11=0,m12=0,m21=0,m22=0;

		while (!reader.atEnd() && !(reader.isEndElement() && reader.name() == QObject::tr("NodeItem")))
		{
			if (reader.isStartElement())
			{
				if (reader.name() == "PartGraphicsItem")
				{
					reader.readNodeGraphics(node,reader.device());
				}
				else
					if (reader.name() == "pos")
					{
						attribs = reader.attributes();
						bool ok;
						if (attribs.size() == 2)
						{
							n = attribs.at(0).value().toString().toDouble(&ok);
							if (ok)
								pos.rx() = n;

							n = attribs.at(1).value().toString().toDouble(&ok);
							if (ok)
								pos.ry() = n;
						}
					}
					else
						if (reader.name() == "transform")
						{
							QXmlStreamAttributes attribs = reader.attributes();
							bool ok;
							if (attribs.size() == 4)
							{
								n = attribs.at(0).value().toString().toDouble(&ok);
								if (ok) m11 = n;

								n = attribs.at(1).value().toString().toDouble(&ok);
								if (ok) m12 = n;

								n = attribs.at(2).value().toString().toDouble(&ok);
								if (ok) m21 = n;

								n = attribs.at(3).value().toString().toDouble(&ok);
								if (ok) m22 = n;
							}
						}
			}
			reader.readNext();
		}
		if (node)
		{
			transform.setMatrix(m11,m12,0.0, m21, m22, 0.0, 0.0, 0.0, 1.0);
			node->refresh();
			node->setPos(pos);
			node->setTransform(transform);
		}
		return node;
	}
}

