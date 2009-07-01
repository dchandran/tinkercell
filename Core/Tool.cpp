/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 The tool class is the parent class for all plugins.
 A Tool is a Qt Widget with a name and pointer to the Tinkercell MainWindow.


****************************************************************************/

#include "MainWindow.h"
#include "TextEditor.h"
#include "NetworkWindow.h"
#include "GraphicsScene.h"
#include "Tool.h"

namespace Tinkercell
{
     Tool::GraphicsItem::GraphicsItem(Tool * t) : tool(t)
     {
     }

     Tool::~Tool()
     {
          if (graphicsItem)
          {
               if (graphicsItem->scene())
                    graphicsItem->scene()->removeItem(graphicsItem);

               delete graphicsItem;
          }

          if (toolButton)
          {
               toolButton->disconnect();
               if (!toolButton->parentWidget())
                    delete toolButton;
          }
     }

     Tool::Tool()
     {
          name = "";
          mainWindow = 0;
          graphicsItem = 0;
          toolButton = 0;
     }

     Tool::Tool(const QString& Name, QWidget * parent): QWidget(parent)
     {
          name = Name;
          mainWindow = 0;
          graphicsItem = 0;
          toolButton = 0;
     }

     bool Tool::setMainWindow(MainWindow * main)
     {
          disconnect();
          mainWindow = main;
          if (toolButton)
               connect(toolButton,SIGNAL(clicked()),this,SLOT(select()));
          if (main)
          {
//                main->addTool(name,this);
               return true;
          }
          return false;
     }

     void Tool::select()
     {
          if (parentWidget() && !parentWidget()->isVisible())
               parentWidget()->show();
          show();
          emit selected();
     }

     void Tool::deselect()
     {
          hide();
          emit deselected();
     }

     void Tool::GraphicsItem::setVisible(bool b)
     {
          QGraphicsItemGroup::setVisible(b);
     }

     GraphicsScene* Tool::currentScene() const
     {
          if (mainWindow)
               return mainWindow->currentScene();
          return 0;
     }

     TextEditor* Tool::currentTextEditor() const
     {
          if (mainWindow)
               return mainWindow->currentTextEditor();
          return 0;
     }

     NetworkWindow * Tool::currentWindow() const
     {
          if (mainWindow)
               return mainWindow->currentWindow();
          return 0;
     }

}
