/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 XML reader that populates the connections tree

****************************************************************************/

#include "ConnectionsTreeReader.h"
namespace Tinkercell
{

     /*! \brief Reads an tree from an XML file using the IO device provided
         * \param QIODevice to use
         * \return void*/
     void ConnectionsTreeReader::readXml(ConnectionsTree* tree, const QString& fileName)
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
     void ConnectionsTreeReader::readTree(ConnectionsTree* tree, QIODevice * device)
     {
          if (!device) return;

          setDevice(device);

          while (!atEnd() && !(isStartElement() && name() == QObject::tr("ConnectionsTree")))
          {
               readNext();
          }

          QPair<ConnectionFamily*,QTreeWidgetItem*> pair;
          ConnectionFamily * family = 0;
          QTreeWidgetItem * treeItem = 0;
          while (!atEnd())
          {
               pair = readConnection(tree);
               family = pair.first;
               treeItem = pair.second;
               if (family && treeItem)
               {
                    tree->widget().addTopLevelItem(treeItem);
                    treeItem->setExpanded(true);
               }
               readNext();
          }
     }

     QPair<ConnectionFamily*,QTreeWidgetItem*> ConnectionsTreeReader::readConnection(ConnectionsTree* tree, ConnectionFamily* parentFamily)
     {
          ConnectionFamily * family = 0;
          QTreeWidgetItem * treeItem = 0;
          if (tree && isStartElement() && name().toString().toLower() == QObject::tr("connection"))
          {
               family = new ConnectionFamily;
               if (parentFamily)
               {
                   family->color = parentFamily->color;
                   family->string = parentFamily->string;
               }

               treeItem = new QTreeWidgetItem;

               QXmlStreamAttributes vec = attributes();

               QString atrib,value;
               bool numerical = false;

               for (int i=0; i < vec.size(); ++i)  //for each attribute
               {
                    if (vec.at(i).name().toString().toLower() == QObject::tr("family"))  //get name of node
                    {
                         family->name = vec.at(i).value().toString();
                         treeItem->setText(0,family->name);
                         if (tree->connectionFamilies.contains(family->name))
                         {
                              family = tree->connectionFamilies[family->name];
                         }
                         else
                         {
                              tree->connectionFamilies[family->name] = family;
                         }
                         tree->treeItems.insertMulti(family->name,treeItem);
                    }
                    else
                    if (vec.at(i).name().toString().toLower() == QObject::tr("color"))  //get measuring unit for the node
                    {
                         QColor color(vec.at(i).value().toString());
                         if (color.isValid())
                         {
                              family->color = color;
                         }
                    }
                    else
                    if (vec.at(i).name().toString().toLower() == QObject::tr("syntax"))  //get measuring unit for the node
                    {
                         family->string = vec.at(i).value().toString();
                    }
                    else
                    if (vec.at(i).name().toString().toLower() == QObject::tr("units"))  //get measuring unit for the node
                    {
                         QStringList units = vec.at(i).value().toString().split(",");
                         if (units.size() == 2)
                              family->measurementUnit = Unit(units[0],units[1]);
                    }
                    else
                    if (vec.at(i).name().toString().toLower() == QObject::tr("participant_types"))  //family types for nodes in this connection
                    {
                         family->nodeFamilies = vec.at(i).value().toString().split(",");
                    }
                    else
                    if (vec.at(i).name().toString().toLower() == QObject::tr("participant_roles"))  //family types for nodes in this connection
                    {
                         family->nodeFunctions = vec.at(i).value().toString().split(",");
                    }
                    else
                    {
                         atrib = vec.at(i).name().toString();         //get other attributes
                         value = vec.at(i).value().toString();

                         qreal num = value.toDouble(&numerical);
                         if (numerical)
                              family->numericalAttributes[atrib] = num;
                         else
                              family->textAttributes[atrib] = value;

                    }
               }
               readNext();
               QString appDir = QCoreApplication::applicationDirPath();
               //set icon
               if (family->pixmap.load(appDir + QString("/") + ConnectionsTree::iconFile(family)))
                    family->pixmap.setMask(family->pixmap.createMaskFromColor(QColor(255,255,255)));
               else
                    if (parentFamily)		//if no icon file, same as parent's icon
                         family->pixmap = parentFamily->pixmap;

               if (parentFamily)
               {
                    QList<QString> nkeys = parentFamily->numericalAttributes.keys();
                    for (int i=0; i < nkeys.size(); ++i)
                         if (!family->numericalAttributes.contains(nkeys[i]))
                              family->numericalAttributes[ nkeys[i] ] = parentFamily->numericalAttributes[ nkeys[i] ];

                    QList<QString> skeys = parentFamily->textAttributes.keys();
                    for (int i=0; i < skeys.size(); ++i)
                         if (!family->textAttributes.contains(skeys[i]))
                              family->textAttributes[ skeys[i] ] = parentFamily->textAttributes[ skeys[i] ];

                    if (family->measurementUnit.name.isEmpty())
                         family->measurementUnit = parentFamily->measurementUnit;
                        
					if (family->nodeFamilies.isEmpty())
                    	family->nodeFamilies = parentFamily->nodeFamilies;
					
					if (family->nodeFamilies.isEmpty())
                    	family->nodeFamilies = parentFamily->nodeFamilies;

                    family->description = parentFamily->description;
               }
               
               if (family->nodeFamilies.isEmpty())
               	    family->nodeFamilies << "anything";

			   while (family->nodeFamilies.size() < family->nodeFunctions.size())
			   		family->nodeFamilies << family->nodeFamilies.last();

               //set arrow head

               QString nodeImageFile = appDir + QString("/") + ConnectionsTree::arrowImageFile(family->name);
               NodeGraphicsReader imageReader;
               ArrowHeadItem * nodeitem = new ArrowHeadItem;
               imageReader.readXml(nodeitem,nodeImageFile);
               nodeitem->normalize();
               family->graphicsItems += nodeitem;

               //if no arrow file, same as parent's arrow
               if (family != 0 && family->graphicsItems.size() > 0 &&
                   NodeGraphicsItem::topLevelNodeItem(family->graphicsItems.last()) != 0 &&
                   !NodeGraphicsItem::topLevelNodeItem(family->graphicsItems.last())->isValid() && parentFamily)
               {
                    family->graphicsItems.clear();
                    for (int i=0; i < parentFamily->graphicsItems.size(); ++i)
                         if (NodeGraphicsItem::topLevelNodeItem(parentFamily->graphicsItems[i]))
                              family->graphicsItems += (NodeGraphicsItem::topLevelNodeItem(parentFamily->graphicsItems[i]))->clone();
               }


               //read sub nodes and child nodes
               QString text;
               QPair<ConnectionFamily*,QTreeWidgetItem*> pair;

               while (!atEnd() && !(isEndElement() && name().toString().toLower() == QObject::tr("connection")))
               {
                    if (isStartElement())
                    {
                         if (name().toString().toLower() == QObject::tr("text"))
                         {
                              family->description = readElementText();
                         }
                         else
                              if (name().toString().toLower() == QObject::tr("connection"))
                              {
                              pair = readConnection(tree, family);
                              treeItem->addChild(pair.second);
                              pair.first->setParent(family);
                         }
                    }
                    readNext();
               }
               readNext();
          }

          return QPair<ConnectionFamily*,QTreeWidgetItem*>(family,treeItem);

     }

}
