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
#include "ItemHandle.h"
#include "ConsoleWindow.h"

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

	/*********************************
	ITEM FAMILY
	**********************************/
	
	QStringList ItemFamily::ALLNAMES;
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
	
	void ItemFamily::setName(const QString& s)
	{
		if (_name.toLower() == s.toLower() && 
			NAMETOID.contains(s.toLower()) && 
			NAMETOID.value(s.toLower()) == ID)
		{
			_name = s;
			return;
		}

		int n = 1;
		_name = s;
		while (NAMETOID.contains(_name.toLower()))
		{
			_name = s + QString::number(n); 
			++n;
		}
		ID = ALLNAMES.size();
		ALLNAMES += _name;
		NAMETOID.insert( _name.toLower() , ID );
	}
	
	bool ItemFamily::isA(int ID) const
	{
		return false;
	}

	bool ItemFamily::isA(const QString& name) const
	{
		QString s = _name.toLower();
		if (!NAMETOID.contains(s)) return false;
		return isA(NAMETOID.value(s));
	}

	bool ItemFamily::isA(const ItemFamily* family) const
	{
		if (!family) return false;
		return isA(family->ID);
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

	NodeFamily::NodeFamily(const QString& s): 
		ItemFamily(s)
	{
		type = NodeFamily::TYPE;
	}

	NodeFamily::~NodeFamily() {}

	ItemFamily* NodeFamily::parent() const
	{
		if (parentFamilies.isEmpty()) return 0;
		return parentFamilies.at(0);
	}

	/*! \brief indicates whether or not the given string is the name of this family or any of its parent families*/
	bool NodeFamily::isA(int id) const
	{
		if (ID == id) return true;

		QList<NodeFamily*> families = parentFamilies;
		for (int i=0; i < families.size(); ++i)
		{
			if (families[i]->ID == id) return true;
			families += families[i]->parentFamilies;
		}
		
		return false;
	}
	
	bool NodeFamily::isA(const QString& name) const
	{
		QString s = _name.toLower();
		if (!ItemFamily::NAMETOID.contains(s)) return false;
		return isA(ItemFamily::NAMETOID.value(s));
	}

	bool NodeFamily::isA(const ItemFamily* family) const
	{
		if (!family) return false;
		return isA(family->ID);
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

	int ConnectionFamily::TYPE = 2;
	QStringList ConnectionFamily::ALLROLENAMES;
	QHash<QString,int> ConnectionFamily::ROLEID;	

	ConnectionFamily * ConnectionFamily::cast(ItemFamily* item)
	{
		if (item && item->type == ConnectionFamily::TYPE)
			return static_cast<ConnectionFamily*>(item);
		return 0;
	}

	ConnectionFamily::ConnectionFamily(const QString& s): 
		ItemFamily(s)
	{
		type = ConnectionFamily::TYPE;
	}

	ConnectionFamily::~ConnectionFamily() {}

	/*! \brief indicates whether or not the given string is the name of this family or any of its parent families*/
	bool ConnectionFamily::isA(int id) const
	{
		if (ID == id) return true;

		QList<ConnectionFamily*> families = parentFamilies;
		for (int i=0; i < families.size(); ++i)
		{
			if (families[i]->ID == id) return true;
			families += families[i]->parentFamilies;
		}
		
		return false;
	}
	
	bool ConnectionFamily::isA(const QString& name) const
	{
		QString s = _name.toLower();
		if (!ItemFamily::NAMETOID.contains(s)) return false;
		return isA(ItemFamily::NAMETOID.value(s));
	}

	bool ConnectionFamily::isA(const ItemFamily* family) const
	{
		if (!family) return false;
		return isA(family->ID);
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
	
	bool ConnectionFamily::isValidSet(const QList<NodeHandle*>& nodes, bool full)
	{
		if (nodes.isEmpty()) return false;

		NodeHandle * h;
		
		if ((full && nodes.size() != nodeRoles.size()) ||
			(nodes.size() > nodeRoles.size()))
		{
			return false;
		}
		
		bool b;
		QVector<bool> allIncluded(nodeRoles.size());
		for (int i=0; i < nodeRoles.size(); ++i)
			allIncluded[i] = false;
		
		for (int i=0; i < nodes.size(); ++i)  //for each node in this connection
		{
			b = false;
			for (int j=0; j < nodeRoles.size(); ++j)   //check of the family allows it
			{
				if (!allIncluded[j] && nodes[i] && 
					nodes[i]->family() && 
					nodes[i]->family()->isA(nodeRoles[j].second))
				{
					allIncluded[j] = true;
					b = true;
					break;
				}
			}
			
			if (!b)
				return false;
		}
		
		if (full)
		{
			for (int i=0; i < allIncluded.size(); ++i)
				if (!allIncluded[i])
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
		QString f = family.toLower(), r = role.toLower();
		
		if (!ItemFamily::NAMETOID.contains(f)) return false;
		
		int nodeid = NAMETOID.value(f);
		int roleid = 0;
		
		if (ROLEID.contains(r) && ALLROLENAMES.size() > ROLEID.value(r))	
		{
			roleid = ROLEID.value(r);
		}
		else
		{
			roleid = ALLROLENAMES.size();
			ALLROLENAMES += r;
			ROLEID.insert( r, roleid );
		}

		nodeRoles += QPair<int,int>( roleid, nodeid );
		return true;
	}
	
	QString ConnectionFamily::participantFamily(const QString& role) const
	{
		QString r = role.toLower();
		if (!ROLEID.contains(r)) return QString();		
		int k1 = ROLEID.value(r);
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
			if (ALLROLENAMES.size() > nodeRoles[i].first)
				roles += ALLROLENAMES[ nodeRoles[i].first ];
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
}

