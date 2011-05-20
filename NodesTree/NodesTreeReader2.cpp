/****************************************************************************

 Copyright (C) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 XML reader that populates the nodes tree

****************************************************************************/
#include <QRegExp>
#include <QFile>
#include <QColor>
#include "MainWindow.h"
#include "NodesTreeReader.h"
#include "GlobalSettings.h"

namespace Tinkercell
{

     /*! \brief Reads an tree from an XML file using the IO device provided
      * \param QIODevice to use
      * \return void*/
     QStringList NodesTreeReader::readXml(NodesTree* tree, const QString& fileName)
     {
          QFile file (fileName);

          if (!file.open(QFile::ReadOnly | QFile::Text))
          {
               return QStringList();
          }

          return readTree(tree, &file);
     }

     /*! \brief Reads an tree from an XML file using the IO device provided
      * \param QIODevice to use
      * \return void*/
     QStringList NodesTreeReader::readTree(NodesTree* tree, QIODevice * device)
     {
     	  QStringList newFamilies;
          if (!device) return newFamilies;

          setDevice(device);

          while (!atEnd() && !(isStartElement() && name() == QObject::tr("NodesGraph")))
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
                          if (!newFamilies.contains(node->name()))
		                  	       newFamilies << node->name();
                          QList<ItemFamily*> children = node->allChildren();
		                  for (int i=0; i < children.size(); ++i)
        	                  if (!newFamilies.contains(children[i]->name()))
		                  	       newFamilies << children[i]->name();
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
                    hasGraphics = true;
               else
                    hasGraphics = false;

               hasNodeGraphics += hasGraphics;

               if (!hasGraphics)
               {
                    for (int j=0; j < node->graphicsItems.size(); ++j)
                         if (node->graphicsItems[j])
                              delete node->graphicsItems[j];
                    node->graphicsItems.clear();
               }
          }
          return newFamilies;
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

               treeItem = new QTreeWidgetItem;

               QXmlStreamAttributes vec = attributes();

               QString atrib,value;
               bool numerical = false;

               for (int i=0; i < vec.size(); ++i)  //for each attribute
               {
                    if (vec.at(i).name().toString().toLower() == QObject::tr("family"))  //get name of node
                    {
                         node->setName(vec.at(i).value().toString());
                         treeItem->setText(0,node->name());
                         if (tree->nodeFamilies.contains(node->name()))
                         {
	                          NodeFamily * node2 = tree->nodeFamilies[node->name()];
                              delete node;
                              node = node2;
                         }
                         else
                         {
                              tree->nodeFamilies[node->name()] = node;
                         }
                         tree->treeItems.insertMulti(node->name(),treeItem);
                    }
                    else
                    if (vec.at(i).name().toString().toLower() == QObject::tr("units"))  //get measuring unit for the node
                    {
                         QStringList units = vec.at(i).value().toString().split(",");
                         if (units.size() == 2)
                         {
                         	  QStringList terms = units[1].split(" ");
                         	  if (terms.size() > 0)
                         	  {
	                         	  for (int j=0; j < terms.size(); ++j)
    	                     	  		node->measurementUnitOptions += Unit(units[0],terms[j]);
    	                          node->measurementUnit = node->measurementUnitOptions[0];
    	                      }
						 }
                    }
                    else
                    if (vec.at(i).name().toString().toLower() == QObject::tr("description"))
                    {
                    	node->description = vec.at(i).value().toString();
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
               QString homeDir = GlobalSettings::homeDir();
               QString appDir = QCoreApplication::applicationDirPath();

               QString iconFile = tree->iconFile(node->name());
               
               if (!QFile::exists(iconFile) && QFile::exists(homeDir + QString("/") + iconFile))
			   		iconFile = homeDir + QString("/") + iconFile;
               else
			   if (!QFile::exists(iconFile) && QFile::exists(appDir + QString("/") + iconFile))
			   		iconFile = appDir + QString("/") + iconFile;

               QString nodeImageFile = tree->nodeImageFile(node->name()); 

           		if (!QFile::exists(nodeImageFile) && QFile::exists(homeDir + QString("/") + nodeImageFile))
					nodeImageFile = homeDir + QString("/") + nodeImageFile;
			    else
			    if (!QFile::exists(nodeImageFile) && QFile::exists(appDir + QString("/") + nodeImageFile))
					nodeImageFile = appDir + QString("/") + nodeImageFile;

                NodeGraphicsReader imageReader;
                NodeGraphicsItem * nodeitem = new NodeGraphicsItem;
                imageReader.readXml(nodeitem,nodeImageFile);
                if (nodeitem && nodeitem->isValid())
                {
                	if (node->graphicsItems.isEmpty())
                	{
	                     nodeitem->normalize();
    	                 node->graphicsItems += nodeitem;
    	            }
                }
                else
                {
                     if (nodeitem) delete nodeitem;
                     
                     if (parentNode)
					 {
					 	   for (int i=0; i < node->graphicsItems.size(); ++i)  //nodes from another parent
		                     	delete node->graphicsItems[i];
		                   node->graphicsItems.clear();
						   for (int i=0; i < parentNode->graphicsItems.size(); ++i)
						        if (NodeGraphicsItem::topLevelNodeItem(parentNode->graphicsItems[i]))
						             node->graphicsItems += (NodeGraphicsItem::topLevelNodeItem(parentNode->graphicsItems[i]))->clone();
					 }
                }
               
               //set icon
               if (node->pixmap.load(iconFile))
                    node->pixmap.setMask(node->pixmap.createMaskFromColor(QColor(255,255,255)));
               else
                    if (parentNode)
                         node->pixmap = parentNode->pixmap;

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

                    if (node->measurementUnitOptions.isEmpty())
                    {
                         node->measurementUnitOptions = parentNode->measurementUnitOptions;
                         node->measurementUnit = parentNode->measurementUnit;
					}
					if (node->description.isEmpty())
	                    node->description = parentNode->description;
               }
               //read sub nodes and child nodes

               QString text;
               QPair<NodeFamily*,QTreeWidgetItem*> childNode;
               while (!atEnd() && !(isEndElement() && name().toString().toLower() == QObject::tr("node")))
               {
                    if (isStartElement())
                    {
                         if (name().toString().toLower() == QObject::tr("node"))
                         {
                              childNode = readNode(tree, node);
                              treeItem->addChild(childNode.second);
                         }
                    }
                    readNext();
               }
               readNext();
          }

          return QPair<NodeFamily*,QTreeWidgetItem*>(node,treeItem);

     }

}
