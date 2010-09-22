/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 XML reader that populates the connections tree

****************************************************************************/
#include <iostream>
#include "ConsoleWindow.h"
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

          while (!atEnd() && !(isStartElement() && name() == QObject::tr("ProcessGraph")))
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
          if (tree && isStartElement() && name().toString().toLower() == QObject::tr("process"))
          {
               family = new ConnectionFamily;

               treeItem = new QTreeWidgetItem;

               QXmlStreamAttributes vec = attributes();

               QString atrib,value;
               QStringList nodeRoles, nodeFamilies; 
               
               bool numerical = false;

               for (int i=0; i < vec.size(); ++i)  //for each attribute
               {
                    if (vec.at(i).name().toString().toLower() == QObject::tr("family"))  //get name of node
                    {
                         family->setName(vec.at(i).value().toString());
                         treeItem->setText(0,family->name());
                         if (tree->connectionFamilies.contains(family->name()))
                         {
                         	  ConnectionFamily * family2 = tree->connectionFamilies[family->name()];
	                       	  delete family;
                              family = family2;
                         }
                         else
                         {
                              tree->connectionFamilies[family->name()] = family;
                         }
                         
                         tree->treeItems.insertMulti(family->name(),treeItem);
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
                         nodeFamilies = vec.at(i).value().toString().split(",");
                    }
                    else
                    if (vec.at(i).name().toString().toLower() == QObject::tr("participant_roles"))  //family types for nodes in this connection
                    {
                    	 nodeRoles = vec.at(i).value().toString().split(",");
                    }
                    else
                    if (vec.at(i).name().toString().toLower() == QObject::tr("description"))
                    {
                    	family->description = vec.at(i).value().toString();
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
               
               if (nodeRoles.isEmpty() && parentFamily)
					nodeRoles = parentFamily->participantRoles();
               
               if (nodeFamilies.isEmpty() && parentFamily)
					nodeFamilies = parentFamily->participantTypes();
               
               if (!nodeRoles.isEmpty() && !nodeFamilies.isEmpty())
	               for (int i=0; i < nodeRoles.size(); ++i)
    	           {
    	           		if (nodeFamilies.size() > i)
    	           			family->addParticipant(nodeRoles[i],nodeFamilies[i]);
    	           		else
    	           			family->addParticipant(nodeRoles[i],nodeFamilies.last());
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

                    family->description = parentFamily->description;
               }

               //set arrow head
               ArrowHeadItem * nodeitem = 0;

               if (family->graphicsItems.isEmpty())
               {
               	   QString arrowImageFile = appDir + QString("/") + ConnectionsTree::arrowImageFile(family->name());
		           nodeitem = new ArrowHeadItem(arrowImageFile);

		           if (!nodeitem->isValid())
		               delete nodeitem;
		           else
			           family->graphicsItems += nodeitem;
		           //if no arrow file, same as parent's arrow
		           if (parentFamily && family->graphicsItems.isEmpty() && 
		           		!parentFamily->graphicsItems.isEmpty() &&
		                NodeGraphicsItem::cast(parentFamily->graphicsItems[0]))
		                family->graphicsItems += (NodeGraphicsItem::topLevelNodeItem(parentFamily->graphicsItems[0]))->clone();
			   }              
               if (family->graphicsItems.size() < 2)
               {
		           QString decoratorImageFile = appDir + QString("/") + ConnectionsTree::decoratorImageFile(family->name());
		           nodeitem = new ArrowHeadItem(decoratorImageFile);
		           
		           if (!nodeitem->isValid())
		               delete nodeitem;
		           else
			           family->graphicsItems += nodeitem;
			       
			       if (parentFamily && (family->graphicsItems.size() < 2) &&
			       		(parentFamily->graphicsItems.size() > 1) &&
		                NodeGraphicsItem::cast(parentFamily->graphicsItems.last()))
		                {
			                family->graphicsItems += (NodeGraphicsItem::topLevelNodeItem(parentFamily->graphicsItems.last()))->clone();
			            }
			   }

               //read sub nodes and child nodes
               QString text;
               QPair<ConnectionFamily*,QTreeWidgetItem*> pair;

               while (!atEnd() && !(isEndElement() && name().toString().toLower() == QObject::tr("process")))
               {
                    if (isStartElement())
                    {
                         if (name().toString().toLower() == QObject::tr("process"))
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
