/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT


****************************************************************************/

#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ArrowSelection.h"
#include "GraphicsTransformTool.h"
#include "GraphicsReplaceTool.h"

namespace Tinkercell
{

    void ArrowSelectionTool::select(int)
    {
        replaceNodeSlot();
    }

    void ArrowSelectionTool::toolLoaded(Tool*)
    {
        static bool alreadyConnected = false;
        if (alreadyConnected || !mainWindow) return;

        if (!alreadyConnected && mainWindow->tool(tr("Graphics Replace Tool")))
        {
            alreadyConnected = true;
            GraphicsReplaceTool * copyPaste = static_cast<GraphicsReplaceTool*>(mainWindow->tool(tr("Graphics Replace Tool")));
            connect(this,SIGNAL(replaceNode()),copyPaste,SLOT(substituteNodeGraphics()));
        }
    }

    void ArrowSelectionTool::replaceNodeSlot()
    {
        emit replaceNode();
    }

    ArrowSelectionTool::ArrowSelectionTool() : Tool(tr("Arrow Selection Tool"),tr("Extra GUI")) , showArrowSelection("Change arrowhead",this), separator(0)
    {
		
        mainWindow = 0;
        transformTool = 0;

        QString appDir = QCoreApplication::applicationDirPath();
        NodeGraphicsReader reader;
        reader.readXml(&arrowPic,appDir + tr("/OtherItems/arrowSelection.xml"));
        arrowPic.normalize();
        arrowPic.scale(40.0/arrowPic.sceneBoundingRect().width(),40.0/arrowPic.sceneBoundingRect().height());
        arrowPic.setToolTip(tr("Select arrow head"));

        graphicsItems += new GraphicsItem(this);
        graphicsItems[0]->addToGroup(&arrowPic);
        graphicsItems[0]->setToolTip(tr("Select arrow head"));
    }

    bool ArrowSelectionTool::setMainWindow(MainWindow * main)
    {
        Tool::setMainWindow(main);
        if (mainWindow)
        {
            connect(&showArrowSelection,SIGNAL(triggered()),this,SLOT(replaceNodeSlot()));
            connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSignal(const QWidget*)));
            connect(mainWindow,SIGNAL(mousePressed(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),this,SLOT(sceneClicked(GraphicsScene *, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));
            //connect(mainWindow,SIGNAL(toolSelected(GraphicsScene*, GraphicalTool*, QPointF, Qt::KeyboardModifiers)),
            //	this,SLOT(toolSelected(GraphicsScene*, GraphicalTool*, QPointF, Qt::KeyboardModifiers)));
            connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),this,SLOT(itemsSelected(GraphicsScene *,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
            connect(mainWindow,SIGNAL(itemsRemoved(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)),this ,SLOT(itemsRemoved(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&)));
            connect(mainWindow,SIGNAL(windowClosing(NetworkWindow*,bool*)),this,SLOT(sceneClosed(NetworkWindow*,bool*)));

            if (mainWindow)
            {
                connect(mainWindow,SIGNAL(toolLoaded(Tool*)),this,SLOT(toolLoaded(Tool*)));
            }
            toolLoaded(0);

            return true;
        }
        return false;
    }

    void ArrowSelectionTool::turnOnGraphicalTools(const QList<QGraphicsItem*>& ,GraphicsScene * scene)
    {
        if (!scene || graphicsItems.isEmpty() || !graphicsItems[0]) return;

        GraphicsItem * graphicsItem = graphicsItems[0];

        QRectF rect;

        /*
                for (int i=0; i < items.size(); ++i)
                {
                        if (items[i])
                                rect = rect.unite(items[i]->sceneBoundingRect());
                }*/

        qreal scalex = 1, scaley = 1;

        QList<QGraphicsView*> views = scene->views();
        for (int j=0; j < views.size(); ++j)
        {
            if (views[j])
            {
                scalex = sqrt(views[j]->transform().m11());
                scaley = sqrt(views[j]->transform().m22());
                break;
            }
        }

        /*qreal maxx = rect.right() + 100.0/(scalex*scalex), miny = rect.top() - 20.0;
                if (maxx > scene->viewport().right() - 100.0) maxx = scene->viewport().left() + 100.0;
                */
        qreal maxx = scene->viewport().right() - 50.0/(scalex),
        miny = scene->viewport().top() + 50.0/(scaley);

        if (graphicsItem->scene() != scene)
        {
            if (graphicsItem->parentItem())
                graphicsItem->setParentItem(0);

            if (graphicsItem->scene() != 0)
                graphicsItem->scene()->removeItem(graphicsItem);

            scene->addItem(graphicsItem);
        }

        graphicsItem->setVisible(true);
        graphicsItem->setZValue(scene->ZValue()+0.1);
        graphicsItem->resetTransform();
        graphicsItem->scale(1.0/scalex,1.0/scaley);
        graphicsItem->setPos(QPointF(maxx,miny));

        miny += graphicsItem->sceneBoundingRect().height() * 1.5;

        if (!transformTool)
        {
            if (mainWindow && mainWindow->tool("Basic Transformations"))
            {
                transformTool = static_cast<GraphicsTransformTool*>(mainWindow->tool(tr("Basic Transformations")));
            }
        }

        if (transformTool && transformTool->graphicsItems.size() > 0 && transformTool->graphicsItems[0])
        {
            graphicsItem = transformTool->graphicsItems[0];
            if (graphicsItem->scene() != scene)
            {
                if (graphicsItem->parentItem())
                    graphicsItem->setParentItem(0);

                if (graphicsItem->scene() != 0)
                    graphicsItem->scene()->removeItem(graphicsItem);

                scene->addItem(graphicsItem);
            }

            if (!graphicsItem->isVisible())
            {
                graphicsItem->setVisible(true);
                graphicsItem->setZValue(scene->ZValue()+0.1);
                graphicsItem->resetTransform();
                graphicsItem->scale(1.0/scalex,1.0/scaley);
                graphicsItem->setPos(QPointF(maxx,miny));
            }
        }
    }

    void ArrowSelectionTool::sceneClicked(GraphicsScene *scene, QPointF , Qt::MouseButton , Qt::KeyboardModifiers )
    {
        if (mainWindow && scene && scene->useDefaultBehavior)
        {
            escapeSignal(0);
        }
    }

    void ArrowSelectionTool::sceneDoubleClicked (GraphicsScene * scene, QPointF , QGraphicsItem * item, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
    {
        if (!scene || !item || button != Qt::LeftButton || modifiers != 0) return;

        NodeGraphicsItem * node = NodeGraphicsItem::topLevelNodeItem(item);
        ArrowHeadItem * arrow = 0;

        if (node && node->className == ArrowHeadItem::CLASSNAME)
        {
            arrow = static_cast<ArrowHeadItem*>(node);
        }

        if (arrow && arrow->connectionItem)
        {
            scene->selected() = arrow->connectionItem->arrowHeadsAsGraphicsItems();
        }
        scene->select(0);
    }

    void ArrowSelectionTool::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF , Qt::KeyboardModifiers )
    {
        escapeSignal(0);

        if (mainWindow && scene && scene->useDefaultBehavior && !items.isEmpty())
        {
            NodeGraphicsItem * node = 0;
            ArrowHeadItem * arrow;

            for (int i=0; i < items.size(); ++i)
            {
                node = qgraphicsitem_cast<NodeGraphicsItem*>(items[i]);
                if (node && (node->className == ArrowHeadItem::CLASSNAME))
                {
                    arrow = (static_cast<ArrowHeadItem*>(node));
                    if (!arrow->connectionItem)
                        return;
                }
                else
                {
                    return;
                }
            }

            turnOnGraphicalTools(items,scene);

            if (separator)
                mainWindow->contextItemsMenu.addAction(separator);
            else
                separator = mainWindow->contextItemsMenu.addSeparator();
            mainWindow->contextItemsMenu.addAction(&showArrowSelection);

        }
        else
        {
            if (separator)
                mainWindow->contextItemsMenu.removeAction(separator);
            mainWindow->contextItemsMenu.removeAction(&showArrowSelection);
        }
    }

    void ArrowSelectionTool::sceneClosed(NetworkWindow * , bool * )
    {
        escapeSignal(0);
    }

    void ArrowSelectionTool::escapeSignal(const QWidget * )
    {
        if (graphicsItems.isEmpty() || !graphicsItems[0]) return;

        GraphicsItem * graphicsItem = graphicsItems[0];

        if (graphicsItem && graphicsItem->scene())
        {
            graphicsItem->scene()->removeItem(graphicsItem);
        }
        graphicsItem->setVisible(false);

        if (transformTool && transformTool->graphicsItems.size() > 0 && transformTool->graphicsItems[0]
            && currentScene() && currentScene()->selected().isEmpty())
        {
            graphicsItem = transformTool->graphicsItems[0];
            if (graphicsItem->scene())
            {
                graphicsItem->scene()->removeItem(graphicsItem);
            }
            graphicsItem->setVisible(false);
            graphicsItem->deselect();
        }
    }

    void ArrowSelectionTool::itemsRemoved(GraphicsScene * scene, const QList<QGraphicsItem*>& , const QList<ItemHandle*>& handles)
    {
        if (scene && !handles.isEmpty())
        {
            escapeSignal(0);
        }
    }

}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
    if (!main) return;

    Tinkercell::ArrowSelectionTool * arrowSelection = new Tinkercell::ArrowSelectionTool;
    main->addTool(arrowSelection);
}

