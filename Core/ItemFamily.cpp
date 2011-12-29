/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This is one of the main classes in Tinkercell
This file defines the ItemFamily, NodeFamily, and ConnectionFamily classes.
Each item in Tinkercell has an associated family.

****************************************************************************/
#include <iostream>
#include <QtDebug>
#include "Ontology.h"
#include "ItemFamily.h"
#include "ItemHandle.h"

namespace Tinkercell
{
	/*********************************
  	Units
	**********************************/
	
	Unit::Unit(const QString& p, const QString& s): property(p), name(s)
	{
	}
	
	Unit::Unit(): property(QString()), name(QString())
	{
	}

	bool Unit::operator == (const Unit& a) const
	{
		return (a.property == property && a.name == name);
	}

	/*********************************
	ITEM FAMILY
	**********************************/
	
	QStringList ItemFamily::ALLNAMES;
	QList<const ItemFamily*> ItemFamily::ALLFAMILIES;
	QHash<QString,int> ItemFamily::NAMETOID;

	ItemFamily::ItemFamily(const QString& s): type(0)
	{
		setName(s);
	}

	ItemFamily::~ItemFamily()
	{
		for (int i=0; i < graphicsItems.size(); ++i)
			if (graphicsItems[i] && !graphicsItems[i]->scene())
				delete graphicsItems[i];
	}
	
	QString ItemFamily::name() const
	{
		return _name;
	}
	
	QList<ItemFamily*> ItemFamily::parents() const
	{
		QList<ItemFamily*> list;
		for (int i=0; i < parentFamilies.size(); ++i)
			list += parentFamilies.at(i);
		return list;
	}

	QList<ItemFamily*> ItemFamily::children() const
	{
		QList<ItemFamily*> list;
		for (int i=0; i < childFamilies.size(); ++i)
			list += childFamilies.at(i);
		return list;
	}

	int ItemFamily::depth() const
	{
		int i = 0;
		ItemFamily * p = parent();
		while (p)
		{
			p = p->parent();
			++i;
		}
		return i;
	}
	
	void ItemFamily::setName(const QString& s)
	{
		if (s.isNull() || s.isEmpty()) return;

		if (NAMETOID.contains(s.toLower()))
		{
			_name = s.toLower();
			ID = NAMETOID[ _name ];
			return;
		}

		_name = s.toLower();
		ID = ALLNAMES.size();
		ALLNAMES += _name;
		ALLFAMILIES += this;
		NAMETOID.insert( _name , ID );
	}

		/*! \brief indicates whether or not the given string is the name of this family or any of its parent families*/
	bool ItemFamily::isA(int id) const
	{
		if (ID == id) return true;

		if (ID >= 0 && ALLNAMES.size() > ID && Ontology::GLOBAL_CHILDREN.contains(ALLNAMES[ID])) return true;
		if (id >= 0 && ALLNAMES.size() > id && Ontology::GLOBAL_PARENTS.contains(ALLNAMES[id])) return true;

		QList<ItemFamily*> families = parentFamilies;
		for (int i=0; i < families.size(); ++i)
		{
			if (families[i]->ID == id) return true;
			families += families[i]->parentFamilies;
		}
		
		return false;
	}

	bool ItemFamily::isA(const QString& name) const
	{
		QString s1 = name.toLower();
		if (Ontology::GLOBAL_PARENTS.contains(s1)) return true;
 
		ConnectionFamily * f1 = Ontology::connectionFamily(s1);
		if (f1)
			s1 = f1->name(); //map possible synonyms to default names
		NodeFamily * f2 = Ontology::nodeFamily(s1);

		if (f2)
			s1 = f2->name(); //map possible synonyms to default names
	
		if (NAMETOID.contains(s1))
			return isA(NAMETOID.value(s1));
		
		if (s1.endsWith('s'))
			s1.chop(1);

		if (NAMETOID.contains(s1))
			return isA(NAMETOID.value(s1));
		
		return false;
	}

	bool ItemFamily::isA(const ItemFamily* family) const
	{
		if (!family) return false;
		if (Ontology::GLOBAL_PARENTS.contains(family->name())) return true;
		return isA(family->ID);
	}
	
	bool ItemFamily::isParentOf(const QString& name) const
	{
		if (!NAMETOID.contains(name)) return false;
		int id = NAMETOID.value(name);
		
		if (ALLFAMILIES.size() <= id) return false;
		
		const ItemFamily * family = ALLFAMILIES[id];
		
		if (!family) return false;
		return family->isA(ID);
	}

	bool ItemFamily::isParentOf(const ItemFamily* family) const
	{
		if (!family) return false;
		return family->isA(ID);
	}

	ItemFamily * ItemFamily::root() const
	{
		ItemFamily * root = const_cast<ItemFamily*>(this);
		while (root->parent())
			root = root->parent();
		return root;
	}

	bool ItemFamily::isRelatedTo(const ItemFamily * family) const
	{
		if (!family) return false;
		return isA(family->root()->ID);
	}
	
	QList<ItemFamily*> ItemFamily::allChildren() const
	{
		QList<ItemFamily*> list  = children(), list2;
		
		for (int i=0; i < list.size(); ++i)
			if (list[i])
			{
				list2 = list[i]->children();
				for (int j=0; j < list2.size(); ++j)
					if (list2[j] && !list.contains(list2[j]))
						list << list2[j];
			}
		
		return list;
	}

	ItemFamily* ItemFamily::parent() const
	{
		if (parentFamilies.isEmpty()) return 0;
		ItemFamily * p = 0;
		int maxd = 0, d = 0;
		for (int i=0; i < parentFamilies.size(); ++i)
			if (parentFamilies[i])
			{
				d = parentFamilies[i]->depth();
				if (p==0 || d >= maxd)
				{
					p = parentFamilies[i];
					maxd = d;
				}
			}
		return p;
	}

	void ItemFamily::setParent(ItemFamily* p)
	{
		if (!p || this == p || isA(p->ID) || p->isA(ID)) return;
		if (!parentFamilies.contains(p))
			parentFamilies.append(p);
		if (!p->childFamilies.contains(this))
			p->childFamilies.append(this);
	}


	/**************************************
				NODE FAMILY
	**************************************/
	
	int NodeFamily::TYPE = 1;
	
	NodeFamily * NodeFamily::cast(ItemFamily* item)
	{
		if (item && item->type == NodeFamily::TYPE)
			return static_cast<NodeFamily*>(item);
		return 0;
	}

	QList<NodeFamily*> NodeFamily::cast(const QList<ItemFamily*>& items)
	{
		QList<NodeFamily*> nodes;
		NodeFamily * item = 0;
		for (int i=0; i < items.size(); ++i)
			if ((item = cast(items[i])) && !nodes.contains(item))
				nodes += item;
		return nodes;
	}


	NodeFamily::NodeFamily(const QString& s): 
		ItemFamily(s)
	{
		type = NodeFamily::TYPE;
	}

	NodeFamily::~NodeFamily() {}

	/*********************************
	         CONNECTION FAMILY
	**********************************/

	int ConnectionFamily::TYPE = 2;

	ConnectionFamily * ConnectionFamily::cast(ItemFamily* item)
	{
		if (item && item->type == ConnectionFamily::TYPE)
			return static_cast<ConnectionFamily*>(item);
		return 0;
	}
	
	QList<ConnectionFamily*> ConnectionFamily::cast(const QList<ItemFamily*>& items)
	{
		QList<ConnectionFamily*> connections;
		ConnectionFamily * item = 0;
		for (int i=0; i < items.size(); ++i)
			if ((item = cast(items[i])) && !connections.contains(item))
				connections += item;
		return connections;
	}

	ConnectionFamily::ConnectionFamily(const QString& s): 
		ItemFamily(s)
	{
		type = ConnectionFamily::TYPE;
	}

	ConnectionFamily::~ConnectionFamily() {}

	bool ConnectionFamily::isValidSet(const QList<NodeHandle*>& nodes, bool full)
	{
		if (nodes.isEmpty())
			return !full;
		
		for (int i=0; i < restrictions.size(); ++i)
			if (!checkRestrictions(restrictions[i],nodes,full))
				return false;

		NodeHandle * h;

		if ((full && nodes.size() != nodeRoles.size()) ||
			(nodes.size() > nodeRoles.size()))
		{
			return false;
		}
		
		bool b;
		int boolean1 = 0, boolean2 = 1;
				
		for (int i=0; i < nodes.size(); ++i)  //for each node in this connection
		{
			b = false;
			boolean2 = boolean2 | (1 << i);
			for (int j=0; j < nodeRoles.size(); ++j)   //check of the family allows it
			{
				if (nodes[i] && 
					nodes[i]->family() && 
						(nodes[i]->family()->isA(nodeRoles[j].second) || 
							(nodeRoles[j].second < ALLFAMILIES.size() && ALLFAMILIES.at(nodeRoles[j].second)->isA(nodes[i]->family()))
						)
					)
				{
					boolean1 = (boolean1  | (1 << j));
					b = true;
				}
			}
			
			if (!b)
				return false;
		}
		
		if (full && (boolean1 != boolean2))
		{
			return false;
		}
		
		return true;
	}

	QList<ItemFamily*> ConnectionFamily::findValidChildFamilies(const QList<NodeHandle*>& nodes, bool full)
	{
		QList<ItemFamily*> validFamilies, childFamilies, list;

		//breadth first search starting from current family

		ConnectionFamily * connection;

		if (isValidSet(nodes,full))
			validFamilies << this;

		childFamilies << this;

		for (int i=0; i < childFamilies.size(); ++i)
			if (childFamilies[i])
			{
				list = childFamilies[i]->children();
				for (int j=0; j < list.size(); ++j)
				{
					connection = ConnectionFamily::cast(list[j]);
					if (connection && !childFamilies.contains(connection))
					{
						childFamilies << connection; 
						if (connection->isValidSet(nodes,full))
							validFamilies << connection;
					}
				}
			}

		return validFamilies;
	}

	bool ConnectionFamily::addParticipant(const QString& role, const QString& family)
	{
		QString f = family.toLower().trimmed(), r = role.toLower().trimmed();

		if (!ItemFamily::NAMETOID.contains(f) ||
			 !ItemFamily::NAMETOID.contains(r) ||  
			 !Ontology::nodeFamily(f) || 
			 !Ontology::participantRole(r)) return false;
		
		int nodeid = NAMETOID.value(f);
		int roleid = NAMETOID.value(r);

		if (!nodeRoles.contains(QPair<int,int>( roleid, nodeid ))) //don't allow duplicate roles
			nodeRoles += QPair<int,int>( roleid, nodeid );
		return true;
	}

	QString ConnectionFamily::participantFamily(const QString& role) const
	{
		QString r = role.toLower();
		if (!NAMETOID.contains(r)) return QString();
		int k1 = NAMETOID.value(r);
		int k2 = -1;
		
		for (int i=0; i < nodeRoles.size(); ++i)
			if (nodeRoles[i].first == k1)
			{
				k2 = nodeRoles[i].second;
				break;
			}
		
		if (k2 < 0)
			return QString();

		if (ItemFamily::ALLNAMES.size() > k2)
			return ItemFamily::ALLNAMES[k2];
		
		return QString();
	}

	QStringList ConnectionFamily::participantRoles() const
	{
		QStringList roles;
		for (int i=0; i < nodeRoles.size(); ++i)
			if (ALLNAMES.size() > nodeRoles[i].first)
				roles += ALLNAMES[ nodeRoles[i].first ];

		return roles;
	}

	QStringList ConnectionFamily::participantTypes() const
	{
		QStringList families;
		for (int i=0; i < nodeRoles.size(); ++i)
			if (ItemFamily::ALLNAMES.size() > nodeRoles[i].second)
				families += ItemFamily::ALLNAMES[ nodeRoles[i].second ];

		return families;
	}
	
	int ConnectionFamily::numberOfIdenticalNodesFamilies(ConnectionFamily * other) const
	{
		if (!other) return 0;
		
		int total = 0;
		for (int i=0; i < nodeRoles.size(); ++i)
			for (int j=0; j < other->nodeRoles.size(); ++j)
				if (nodeRoles[i].second == other->nodeRoles[j].second)
				{
					++total;
					break;
				}
	}
	
	QStringList ConnectionFamily::synonymsForRole(const QString& rolename)
	{
		QStringList rolelist;
	
		if (NAMETOID.contains(rolename.toLower()))
		{
			int roleid = NAMETOID[ rolename.toLower() ];
			if (ALLFAMILIES.size() > roleid && ALLFAMILIES[roleid])
			{
				QList<ItemFamily*> all = ALLFAMILIES[roleid]->allChildren();
				for (int i=0; i < all.size(); ++i)
					rolelist += all[i]->name();
			}
		}

		return rolelist;
	}
	
	bool ConnectionFamily::checkRestrictions(const QString & restriction, const QList<NodeHandle*>& nodes, bool checkFull)
	{
		if (checkFull && restriction.toLower() == QString("different compartments"))
		{
			if (nodes.size() < 2 || !nodes[0]) return false;

			ItemHandle * parent = nodes[0]->parent;
			for (int i=1; i < nodes.size(); ++i)
			{
				if (nodes[i] && nodes[i]->parent != parent)
					return true;
			}
			return false;
		}
		return true;
	}
}

