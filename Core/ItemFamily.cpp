/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This is one of the main classes in Tinkercell
 This file defines the ItemFamily, NodeFamily, and ConnectionFamily classes.
 Each item in Tinkercell has an associated family. 
 
****************************************************************************/
#include <QtDebug>
#include "ItemFamily.h"

namespace Tinkercell
{
    int NodeFamily::Type = 1;
    int ConnectionFamily::Type = 2;

    NodeFamily * NodeFamily::asNode(ItemFamily* item)
    {
        if (item && item->type == NodeFamily::Type)
            return static_cast<NodeFamily*>(item);
        return 0;
    }

    ConnectionFamily * ConnectionFamily::asConnection(ItemFamily* item)
    {
        if (item && item->type == ConnectionFamily::Type)
            return static_cast<ConnectionFamily*>(item);
        return 0;
    }

    /*********************************
        ITEM FAMILY
    **********************************/

    ItemFamily::ItemFamily() : type(0) {}

    ItemFamily::~ItemFamily() {}

    bool ItemFamily::isA(const QString& ) const { return false; }

    bool ItemFamily::isA(const ItemFamily* family) const
    {
        if (!family) return false;
        return isA(family->name);
    }

    const ItemFamily * ItemFamily::root() const
    {
        const ItemFamily * root = this;
        while (root->parent())
            root = root->parent();
        return root;
    }

    bool ItemFamily::isRelatedTo(const ItemFamily * family) const
    {
        if (!family) return false;
        return isA(family->root()->name);
    }

    /**************************************
        NODE FAMILY
    **************************************/

    NodeFamily::NodeFamily()
    {
        type = NodeFamily::Type;
    }

    NodeFamily::~NodeFamily() {}

    ItemFamily* NodeFamily::parent() const
    {
        if (parentFamilies.isEmpty()) return 0;
        return parentFamilies.at(0);
    }

    /*! \brief indicates whether or not the given string is the name of this family or any of its parent families*/
    bool NodeFamily::isA(const QString& familyName) const
    {
        if (familyName.toLower() == QObject::tr("node") || familyName.toLower() == name.toLower()) return true;

        QList<NodeFamily*> families = parentFamilies;
        for (int i=0; i < families.size(); ++i)
        {
            //qDebug() << familyName << " is A? " << families[i]->name;
            if (families[i]->name.toLower() == familyName.toLower()) return true;
            families += families[i]->parentFamilies;
        }
        return false;
    }

    QList<ItemFamily*> NodeFamily::parents() const
    {
        QList<ItemFamily*> list;
        for (int i=0; i < parentFamilies.size(); ++i)
            list += parentFamilies.at(i);
        return list;
    }

    QList<ItemFamily*> NodeFamily::children() const
    {
        QList<ItemFamily*> list;
        for (int i=0; i < childFamilies.size(); ++i)
            list += childFamilies.at(i);
        return list;
    }

    void NodeFamily::setParent(NodeFamily* p)
    {
        if (!p) return;
        if (!parentFamilies.contains(p))
            parentFamilies.append(p);
        if (!p->childFamilies.contains(this))
            p->childFamilies.append(this);
    }

    /*********************************
        CONNECTION FAMILY
    **********************************/

    ConnectionFamily::ConnectionFamily()
    {
        type = ConnectionFamily::Type;
    }

    ConnectionFamily::~ConnectionFamily() {}

    /*! \brief indicates whether or not the given string is the name of this family or any of its parent families*/
    bool ConnectionFamily::isA(const QString& familyName) const
    {
        if (familyName.toLower() == QObject::tr("connection") || familyName.toLower() == name.toLower()) return true;

        QList<ConnectionFamily*> families = parentFamilies;
        for (int i=0; i < families.size(); ++i)
        {
            if (families[i]->name.toLower() == familyName.toLower()) return true;
            families += families[i]->parentFamilies;
        }
        return false;
    }

    ItemFamily* ConnectionFamily::parent() const
    {
        if (parentFamilies.isEmpty()) return 0;
        return parentFamilies.at(0);
    }

    QList<ItemFamily*> ConnectionFamily::parents() const
    {
        QList<ItemFamily*> list;
        for (int i=0; i < parentFamilies.size(); ++i)
            list += parentFamilies.at(i);
        return list;
    }

    QList<ItemFamily*> ConnectionFamily::children() const
    {
        QList<ItemFamily*> list;
        for (int i=0; i < childFamilies.size(); ++i)
            list += childFamilies.at(i);
        return list;
    }

    void ConnectionFamily::setParent(ConnectionFamily* p)
    {
        if (!p) return;
        if (!parentFamilies.contains(p))
            parentFamilies.append(p);
        if (!p->childFamilies.contains(this))
            p->childFamilies.append(this);
    }
}
