/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This tool allows insertion of parts from the NodesTree

****************************************************************************/

#include <QtDebug>
#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "TextEditorListener.h"

namespace Tinkercell
{

//      NodeInsertion::NodeInsertion(NodesTree * tree) : Tool(tr("Node Insertion")), selectedNodeFamily(0)
//      {
//           mainWindow = 0;
//           partsTree = tree;
//           selectedNodeFamily = 0;
//           connectTCFunctions();
//      }
//
//      bool NodeInsertion::setMainWindow(MainWindow * main)
//      {
//           Tool::setMainWindow(main);
//           if (mainWindow)
//           {
//                connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSignal(const QWidget*)));
//
//                //connect(mainWindow,SIGNAL(sceneRightClick(GraphicsScene *, QGraphicsItem*, QPointF, Qt::KeyboardModifiers)),
//                //	this, SLOT(sceneRightClick(GraphicsScene *, QGraphicsItem*, QPointF, Qt::KeyboardModifiers)));
//
//                connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
//
//                connectToNodesTree();
//
//                connect(mainWindow,SIGNAL(pluginLoaded(const QString&)),this,SLOT(pluginLoaded(const QString&)));
//
//                return partsTree != 0;
//           }
//           return false;
//      }
//
//      void NodeInsertion::partSelected(NodeFamily * partFamily)
//      {
//           //if (partFamily)
//           //qDebug() << "part selected " << partFamily->name;
//           if ((selectedNodeFamily != 0 ||
//                (mainWindow != 0 && mainWindow->currentScene() != 0 && mainWindow->currentScene()->actionsEnabled))
//                && partFamily && partsTree)
//                {
//                selectedNodeFamily = partFamily;
//
//                while (partFamily != 0 && partFamily->pixmap.isNull())
//                     partFamily = static_cast<NodeFamily*>(partFamily->parent());
//
//                if (partFamily != 0 && !partFamily->pixmap.isNull())
//                {
//                     qreal asp = (double)partFamily->pixmap.height()/(double)partFamily->pixmap.width();
//
//                     mainWindow->setCursor(QCursor(partFamily->pixmap.scaled(30,(int)(30*asp))));
//                     partsTree->setCursor(partFamily->pixmap.scaled(30,(int)(30*asp)));
//                }
//                else
//                {
//                     mainWindow->setCursor(Qt::ArrowCursor);
//                     partsTree->setCursor(Qt::ArrowCursor);
//                }
//
//                if (mainWindow->currentScene())
//                     mainWindow->currentScene()->actionsEnabled = false;
//           }
//      }
//
//      void NodeInsertion::connectTCFunctions( )
//      {
//           connect(&fToS,SIGNAL(insertItem(QSemaphore*,ItemHandle**,QString,QString)),this,SLOT(insertItem(QSemaphore*,ItemHandle**,QString,QString)));
//      }
//
//      typedef void (*tc_NodeInsertion_api)(OBJ (*insertItem)(const char* , const char* ));
//
//      void NodeInsertion::setupFunctionPointers( QLibrary * library )
//      {
//           tc_NodeInsertion_api f = (tc_NodeInsertion_api)library->resolve("tc_NodeInsertion_api");
//           if (f)
//           {
//                //qDebug() << "tc_NodeInsertion_api resolved";
//                f(
//                          &(_insertItem) );
//           }
//      }
//
//      void NodeInsertion::insertItem(QSemaphore * sem, ItemHandle** item, QString name, QString family)
//      {
//           if (mainWindow && mainWindow->currentScene() && !name.isEmpty() && !family.isEmpty() && partsTree
//               && partsTree->partFamilies.contains(family))
//           {
//                NodeFamily * selectedFamily = partsTree->partFamilies[family];
//                GraphicsScene * scene = mainWindow->currentScene();
//                if (item)
//                     (*item) = 0;
//                if (selectedFamily && scene)
//                {
//                     QPointF point = scene->lastPoint();
//
//                     QList<ItemFamily*> subfamilies = selectedFamily->subFamilies();
//                     QList<NodeFamily*> allFamilies;
//                     if (!subfamilies.isEmpty() && selectedFamily->graphicsItems.isEmpty())
//                     {
//                          for (int i=0; i < subfamilies.size(); ++i)
//                               if (subfamilies[i] && subfamilies[i]->isA("node"))
//                                    allFamilies += static_cast<NodeFamily*>(subfamilies[i]);
//                     }
//                     if (allFamilies.isEmpty())
//                          allFamilies += selectedFamily;
//
//                     QString text;
//                     QList<QGraphicsItem*> list;
//                     qreal xpos = point.x();
//                     qreal height = 0.0;
//                     qreal width = 0.0;
//
//                     for (int j=0; j < allFamilies.size(); ++j)
//                     {
//                          NodeFamily * partFamily = allFamilies[j];
//
//                          for (int i=0; i < partFamily->graphicsItems.size(); ++i)
//                               if (qgraphicsitem_cast<NodeGraphicsItem*>(partFamily->graphicsItems[i]))
//                                    width += qgraphicsitem_cast<NodeGraphicsItem*>(partFamily->graphicsItems[i])->defaultSize.width();
//                     }
//
//                     xpos -= width/2.0;
//                     bool alternate = false;
//
//                     for (int j=0; j < allFamilies.size(); ++j)
//                     {
//                          NodeFamily * partFamily = allFamilies[j];
//
//                          NodeHandle * handle = new NodeHandle(partFamily);
//
//                          if (allFamilies.size() == 1)
//                               handle->name = name;
//                          else
//                          {
//                               handle->name = partFamily->name.toLower();
//                               if (handle->name.length() > 4)
//                                    handle->name.chop( handle->name.length() - 1 );
//                               handle->name = name + tr("_") + handle->name;
//                          }
//                          handle->name = findUniqueName(handle,scene->allHandles(),false);
//                          if (item)
//                               (*item) = handle;
//
//                          text += handle->name + tr(" ");
//
//                          for (int i=0; i < partFamily->graphicsItems.size(); ++i)
//                          {
//                               NodeGraphicsItem * image = (NodeGraphicsItem::topLevelNodeItem(partFamily->graphicsItems[i]));
//                               if (image)
//                               {
//                                    image = image->clone();
//
//                                    if (image->defaultSize.width() > 0 && image->defaultSize.height() > 0)
//                                         image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),image->defaultSize.height()/image->sceneBoundingRect().height());
//
//                                    qreal w = image->sceneBoundingRect().width();
//
//                                    image->setPos(xpos + w/2.0, point.y());
//
//                                    image->setBoundingBoxVisible(false);
//
//                                    if (image->isValid())
//                                    {
//                                         xpos += w;
//                                         setHandle(image,handle);
//                                         list += image;
//                                    }
//                                    if (image->sceneBoundingRect().height() > height)
//                                         height = image->sceneBoundingRect().height();
//                               }
//                          }
//
//                          if (partFamily->graphicsItems.size() > 0)
//                          {
//                               if (handle->family() && !handle->family()->isA("Empty"))
//                               {
//                                    TextGraphicsItem * nameItem = new TextGraphicsItem(handle,0);
//                                    QFont font = nameItem->font();
//                                    font.setPointSize(22);
//                                    nameItem->setFont(font);
//                                    if (alternate)
//                                         nameItem->setPos(xpos - nameItem->boundingRect().width(), point.y() - height/2.0 - 40.0);
//                                    else
//                                         nameItem->setPos(xpos - nameItem->boundingRect().width(), point.y() + height/2.0 + 5.0);
//                                    list += nameItem;
//                                    alternate = !alternate;
//                               }
//                          }
//                     }
//
//                     if (!list.isEmpty())
//                     {
//                          scene->insert(text + tr("inserted"),list);
//                     }
//                }
//           }
//           if (sem)
//                sem->release();
//      }
//
//
//      void NodeInsertion::pluginLoaded(const QString&)
//      {
//           connectToNodesTree();
//      }
//
//      void NodeInsertion::sceneClicked(GraphicsScene *scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers)
//      {
//           if (mainWindow && scene && selectedNodeFamily)
//           {
//                if (button == Qt::LeftButton)
//                {
//                     QList<ItemFamily*> subfamilies = selectedNodeFamily->subFamilies();
//                     QList<NodeFamily*> allFamilies;
//                     if (!subfamilies.isEmpty() && selectedNodeFamily->graphicsItems.isEmpty())
//                     {
//                          for (int i=0; i < subfamilies.size(); ++i)
//                               if (subfamilies[i] && subfamilies[i]->isA("node"))
//                                    allFamilies += static_cast<NodeFamily*>(subfamilies[i]);
//                     }
//                     if (allFamilies.isEmpty())
//                          allFamilies += selectedNodeFamily;
//
//                     QString text;
//                     QList<QGraphicsItem*> list;
//                     qreal xpos = point.x();
//                     qreal height = 0.0;
//                     qreal width = 0.0;
//
//                     for (int j=0; j < allFamilies.size(); ++j)
//                     {
//                          NodeFamily * partFamily = allFamilies[j];
//
//                          for (int i=0; i < partFamily->graphicsItems.size(); ++i)
//                               if (qgraphicsitem_cast<NodeGraphicsItem*>(partFamily->graphicsItems[i]))
//                                    width += qgraphicsitem_cast<NodeGraphicsItem*>(partFamily->graphicsItems[i])->defaultSize.width();
//                     }
//
//                     xpos -= width/2.0;
//                     bool alternate = false;
//
//                     for (int j=0; j < allFamilies.size(); ++j)
//                     {
//                          NodeFamily * partFamily = allFamilies[j];
//
//                          NodeHandle * handle = new NodeHandle(partFamily);
//                          handle->name = partFamily->name.toLower();
//                          if (handle->name.length() > 4)
//                               handle->name.chop( handle->name.length() - 1 );
//                          handle->name = findUniqueName(handle,scene->allHandles());
//
//                          text += handle->name + tr(" ");
//
//                          for (int i=0; i < partFamily->graphicsItems.size(); ++i)
//                          {
//                               NodeGraphicsItem * image = (NodeGraphicsItem::topLevelNodeItem(partFamily->graphicsItems[i]));
//                               if (image)
//                               {
//                                    image = image->clone();
//                                    if (image->defaultSize.width() > 0 && image->defaultSize.height() > 0)
//                                         image->scale(image->defaultSize.width()/image->sceneBoundingRect().width(),image->defaultSize.height()/image->sceneBoundingRect().height());
//
//                                    qreal w = image->sceneBoundingRect().width();
//
//                                    image->setPos(xpos + w/2.0, point.y());
//
//                                    image->setBoundingBoxVisible(false);
//
//                                    if (image->isValid())
//                                    {
//                                         xpos += w;
//                                         setHandle(image,handle);
//                                         list += image;
//                                    }
//                                    if (image->sceneBoundingRect().height() > height)
//                                         height = image->sceneBoundingRect().height();
//                               }
//                          }
//
//                          if (partFamily->graphicsItems.size() > 0)
//                          {
//                               if (handle->family() && !handle->family()->isA("Empty"))
//                               {
//                                    TextGraphicsItem * nameItem = new TextGraphicsItem(handle,0);
//                                    QFont font = nameItem->font();
//                                    font.setPointSize(22);
//                                    nameItem->setFont(font);
//                                    if (alternate)
//                                         nameItem->setPos(xpos - nameItem->boundingRect().width(), point.y() - height/2.0 - 40.0);
//                                    else
//                                         nameItem->setPos(xpos - nameItem->boundingRect().width(), point.y() + height/2.0 + 5.0);
//                                    list += nameItem;
//                                    alternate = !alternate;
//                               }
//                          }
//                     }
//
//                     if (!list.isEmpty())
//                     {
//                          scene->insert(text + tr("inserted"),list);
//                     }
//                }
//                else
//                {
//                     clear();
//                }
//           }
//      }
//
//      void NodeInsertion::connectToNodesTree()
//      {
//           if (partsTree || !mainWindow) return;
//
//           if (mainWindow->tools.contains(tr("Nodes Tree")))
//           {
//                QWidget * treeWidget = mainWindow->tools.value(tr("Nodes Tree"));
//                partsTree = static_cast<NodesTree*>(treeWidget);
//                if (partsTree != 0)
//                {
//                     connect(partsTree,SIGNAL(partSelected(NodeFamily*)),this,SLOT(partSelected(NodeFamily*)));
//                     connect(mainWindow,SIGNAL(mousePressed(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),this,SLOT(sceneClicked(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));
//                }
//           }
//      }
//
//      void NodeInsertion::clear(bool setArrows)
//      {
//           selectedNodeFamily = 0;
//           partsTree->setCursor(Qt::ArrowCursor);
//           if (setArrows)
//           {
//                mainWindow->setCursor(Qt::ArrowCursor);
//                if (mainWindow->currentScene())
//                     mainWindow->currentScene()->actionsEnabled = true;
//           }
//      }
//
//      void NodeInsertion::sceneRightClick(GraphicsScene *, QGraphicsItem*, QPointF, Qt::KeyboardModifiers)
//      {
//           if (selectedNodeFamily)
//                clear();
//      }
//
//      void NodeInsertion::escapeSignal(const QWidget * )
//      {
//           if (selectedNodeFamily)
//                clear();
//      }
//
//      /**************************************************/
//
//      NodeInsertion_FToS NodeInsertion::fToS;
//
//      OBJ NodeInsertion::_insertItem(const char* a, const char* b)
//      {
//           return fToS.insertItem(a,b);
//      }
//
//      OBJ NodeInsertion_FToS::insertItem(const char* a0, const char* a1)
//      {
//           QSemaphore * s = new QSemaphore(1);
//           ItemHandle * item = 0;
//           s->acquire();
//           emit insertItem(s,&item,ConvertValue(a0),ConvertValue(a1));
//           s->acquire();
//           s->release();
//           delete s;
//           return ConvertValue(item);
//      }
//
//      QString NodeInsertion::findUniqueName(ItemHandle * handle1, const QList<ItemHandle*>& items, bool number)
//      {
//           if (!handle1) return QString("");
//           int	c = 1;
//           QString name;
//           if (number)
//                name = handle1->fullName() + QString::number(c);
//           else
//                name = handle1->fullName();
//           bool uniqueName = false;
//
//           ItemHandle* handle2 = 0;
//
//           while (!uniqueName)
//           {
//                uniqueName = true;
//
//                for (int i=0; i < items.size(); ++i)
//                {
//                     handle2 = (items[i]);
//                     if (handle2 && (handle1 != handle2) &&
//                         (handle2->fullName() == name || handle2->name == name))
//                     {
//                          uniqueName = false;
//                          break;
//                     }
//                }
//                if (!uniqueName)
//                {
//                     name = handle1->fullName() + QString::number(c);
//                     ++c;
//                }
//           }
//
//           return name;
//      }

}


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
        if (!main) return;

//         Tinkercell::NodeSelection * partSelection = new Tinkercell::NodeSelection;
//         if (main->tools.contains(partSelection->name))
//                 delete partSelection;
//         else
//                 partSelection->setMainWindow(main);

}

