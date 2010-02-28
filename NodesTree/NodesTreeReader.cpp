/****************************************************************************

 Copyright (C) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 XML reader that populates the nodes tree

****************************************************************************/
#include <QRegExp>
#include <QFile>
#include <QColor>
#include "NodesTreeReader.h"
namespace Tinkercell
{

     /*! \brief Reads an tree from an XML file using the IO device provided
      * \param QIODevice to use
      * \return void*/
     void NodesTreeReader::readXml(NodesTree* tree, const QString& fileName)
     {
          QFile file (fileName);

          if (!file.open(QFile::ReadOnly | QFile::Text))
          {
               return;
          }

          readTree(tree, &file);
     }

     /*! \brief Reads an tree from an XML file using the IO device provided
      * \param QIODevice to use
      * \return void*/
     void NodesTreeReader::readTree(NodesTree* tree, QIODevice * device)
     {
          if (!device) return;

          setDevice(device);

          while (!atEnd() && !(isStartElement() && name() == QObject::tr("NodesTree")))
          {
               readNext();
          }

          compositeNodes.clear();

          QPair<NodeFamily*,QTreeWidgetItem*> pair;
          NodeFamily * node = 0;
          QTreeWidgetItem * treeItem = 0;
          while (!atEnd())
          {
               pair = readNode(tree);
               node = pair.first;
               treeItem = pair.second;
               if (node)
               {
                    if (treeItem)
                    {
                         tree->widget().addTopLevelItem(treeItem);
                         treeItem->setExpanded(true);
                         for (int i=0; i < treeItem->childCount(); ++i)
                              if (treeItem->child(i))
                                   treeItem->child(i)->setExpanded(true);
                    }
               }
               readNext();
          }
    
          QList<bool> hasNodeGraphics;
          bool hasGraphics;
          for (int i=0; i < compositeNodes.size(); ++i)
          {
               node = compositeNodes[i].first;

               if (node->graphicsItems.size() > 0 &&
                   NodeGraphicsItem::topLevelNodeItem(node->graphicsItems[0]) &&
                   (NodeGraphicsItem::topLevelNodeItem(node->graphicsItems[0]))->isValid())
                    hasGraphics += true;
               else
                    hasGraphics += false;

               hasNodeGraphics += hasGraphics;

               if (!hasGraphics)
               {
                    for (int j=0; j < node->graphicsItems.size(); ++j)
                         if (node->graphicsItems[j])
                              delete node->graphicsItems[j];
                    node->graphicsItems.clear();
               }
          }
/*
          QString text;
          for (int i=0; i < compositeNodes.size(); ++i)
          {
               text = compositeNodes[i].second;
               node = compositeNodes[i].first;
               if (!node) continue;

               if (tree->nodeFamilies.contains(text))
               {
                    NodeFamily * subNode = tree->nodeFamilies[text];
                    if (subNode)
                    {
                         if (!node->includedFamilies.contains(subNode))
                              node->includedFamilies << subNode;
                    }
               }
          }*/
     }

     QPair<NodeFamily*,QTreeWidgetItem*> NodesTreeReader::readNode(NodesTree* tree, NodeFamily* parentNode)
     {
          NodeFamily * node = 0;
          QTreeWidgetItem * treeItem = 0;

          //bool isComposite = false;

          if (tree && isStartElement() && name().toString().toLower() == QObject::tr("node"))
          {
               node = new NodeFamily;
               if (parentNode)
                   node->color = parentNode->color;

               treeItem = new QTreeWidgetItem;

               QXmlStreamAttributes vec = attributes();

               QString atrib,value;
               bool numerical = false;

               for (int i=0; i < vec.size(); ++i)  //for each attribute
               {
                    if (vec.at(i).name().toString().toLower() == QObject::tr("family"))  //get name of node
                    {
                         node->name = vec.at(i).value().toString();
                         treeItem->setText(0,node->name);
                         if (tree->nodeFamilies.contains(node->name))
                         {
                              node = tree->nodeFamilies[node->name];
                         }
                         else
                         {
                              tree->nodeFamilies[node->name] = node;
                         }
                         tree->treeItems.insertMulti(node->name,treeItem);
                    }
                    else
                    if (vec.at(i).name().toString().toLower() == QObject::tr("color"))  //get measuring unit for the node
                    {
                         QString s = vec.at(i).value().toString();
                         QColor color(s);
                         if (color.isValid())
                         {
                              node->color = color;
                         }
                    }
                    else
                    if (vec.at(i).name().toString().toLower() == QObject::tr("units"))  //get measuring unit for the node
                    {
                         QStringList units = vec.at(i).value().toString().split(",");
                         if (units.size() == 2)
                              node->measurementUnit = QPair<QString,QString>(units[0],units[1]);
                    }
                    else
                    {
                         atrib = vec.at(i).name().toString();         //get other attributes
                         value = vec.at(i).value().toString();

                         qreal num = value.toDouble(&numerical);
                         if (numerical)
                         {
                              node->numericalAttributes[atrib] = num;
                         }
                         else
                         {
                              node->textAttributes[atrib] = value;
                         }
                    }
               }
               readNext();

               //node file
               QString appDir = QCoreApplication::applicationDirPath();

               QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);
               settings.beginGroup("NodesTree");
               QString s;

               QString iconFile = appDir + QString("/") + tree->iconFile(node->name);

               if (node->graphicsItems.isEmpty())
               {
                    QString nodeImageFile = appDir + QString("/") + tree->nodeImageFile(node->name);

                    s = settings.value(node->name, QString()).toString();
                    if (!s.isEmpty())
                    {
                         s = s.split(QString(","))[0];
                         if (QFile(s).exists())
                         {
                              nodeImageFile = s;
                              QString pngFile = nodeImageFile;
                              pngFile.replace(QRegExp(QString("xml$")),QString("PNG"));
                              pngFile.replace(QRegExp(QString("XML$")),QString("PNG"));
                              iconFile = pngFile;
                         }
                    }

                    NodeGraphicsReader imageReader;
                    NodeGraphicsItem * nodeitem = new NodeGraphicsItem;
                    imageReader.readXml(nodeitem,nodeImageFile);
                    if (nodeitem->isValid())
                    {
                         nodeitem->normalize();
                         node->graphicsItems += nodeitem;
                    }
                    else
                    {
                         delete nodeitem;
                    }
               }
               //set icon
               settings.endGroup();

               if (node->pixmap.load(iconFile))
                    node->pixmap.setMask(node->pixmap.createMaskFromColor(QColor(255,255,255)));
               else
                    if (parentNode)
                         node->pixmap = parentNode->pixmap;

               //get pixmap from node file -- doesn't work
               /*
               if (nodeitem && nodeitem->isValid())
               {
                    QPixmap pixmap(50, 50);
                    pixmap.fill(Qt::transparent);

                    QPainter painter(&pixmap);
                    painter.translate(25, 25);
                    nodeitem->scale( 25.0/nodeitem->boundingRect().width(), 25.0/nodeitem->boundingRect().height() );

                    painter.setRenderHint(QPainter::Antialiasing);
                    nodeitem->paint(&painter);
                    node->pixmap = pixmap;
               }
               */

               if (parentNode)
                    node->setParent(parentNode);

               if (parentNode)
               {
                    QList<QString> nkeys = parentNode->numericalAttributes.keys();
                    for (int i=0; i < nkeys.size(); ++i)
                         if (!node->numericalAttributes.contains(nkeys[i]))
                              node->numericalAttributes[ nkeys[i] ] = parentNode->numericalAttributes[ nkeys[i] ];

                    QList<QString> skeys = parentNode->textAttributes.keys();
                    for (int i=0; i < skeys.size(); ++i)
                         if (!node->textAttributes.contains(skeys[i]))
                              node->textAttributes[ skeys[i] ] = parentNode->textAttributes[ skeys[i] ];

                    if (node->measurementUnit.first.isEmpty())
                         node->measurementUnit = parentNode->measurementUnit;

                    node->description = parentNode->description;
               }
               //read sub nodes and child nodes

               QString text;
               QPair<NodeFamily*,QTreeWidgetItem*> childNode;
               while (!atEnd() && !(isEndElement() && name().toString().toLower() == QObject::tr("node")))
               {
                    if (isStartElement())
                    {
                         if (name().toString().toLower() == QObject::tr("text"))
                         {
                              node->description = readElementText();
                         }
                         else
                         if (name().toString().toLower() == QObject::tr("node"))
                         {
                              childNode = readNode(tree, node);
                              treeItem->addChild(childNode.second);
                         }
                         /*else
                             if (name().toString().toLower() == QObject::tr("subnode"))
                              {
                              text = readElementText();
                              compositeNodes += QPair<NodeFamily*,QString>(node,text);
                              isComposite = true;
                         }*/
                    }
                    readNext();
               }
               readNext();
          }

          /*if (isComposite)
          {
               if (node != 0 && node->graphicsItems.size() > 0)
               {
                    for (int i=0; i < node->graphicsItems.size(); ++i)
                         if (node->graphicsItems[i])
                              delete node->graphicsItems[i];
                    node->graphicsItems.clear();
               }
          }
          else //if no image file, same as parent's image*/
          if (node != 0 && node->graphicsItems.size() < 1 && parentNode)
          {
               for (int i=0; i < parentNode->graphicsItems.size(); ++i)
                    if (NodeGraphicsItem::topLevelNodeItem(parentNode->graphicsItems[i]))
                         node->graphicsItems += (NodeGraphicsItem::topLevelNodeItem(parentNode->graphicsItems[i]))->clone();
          }

          return QPair<NodeFamily*,QTreeWidgetItem*>(node,treeItem);

     }

}
