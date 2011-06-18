/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This is one of the main classes in Tinkercell
This file defines the ItemFamily, NodeFamily, and ConnectionFamily classes.
Each item in Tinkercell has an associated family.


****************************************************************************/

#ifndef TINKERCELL_FAMILY_H
#define TINKERCELL_FAMILY_H

#include <QColor>
#include <QPair>
#include <QList>
#include <QStringList>
#include <QHash>
#include <QUndoCommand>
#include <QGraphicsItem>

#ifdef Q_WS_WIN
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{

	class Tool;
	class TextGraphicsItem;
	class ItemHandle;
	class NodeHandle;
	class ConnectionHandle;
	class NodeGraphicsItem;
	class ArrowHeadItem;
	class ConnectionGraphicsItem;
	class NodeFamily;
	class ConnectionFamily;
	
	/*! \brief A unit of measurement
	\ingroup core
	*/
	class TINKERCELLEXPORT Unit
	{
	public:
		QString property; //e.g. "distance"
		QString name; //e.g. m in "km"
		Unit();
		Unit(const QString& property, const QString& name);
		bool operator == (const Unit&) const;
	};

	/*! \brief This class defines the family of a node or connection.
	The class contains the icon for the family, family name, and minimal data that
	defines the family. Each family has a name, which is internally converted to an integer (ID)
	The ID is used to perform isA checks, thus avoiding repeated string matches
	\ingroup core
	*/
	class TINKERCELLEXPORT ItemFamily
	{
	public:
		/*! \brief description of this family*/
		QString description;
		/*! \brief other names for this family*/
		QStringList synonyms;
		/*! \brief restrictions that apply to this family*/
		QStringList restrictions;
		/*! \brief the possible options for measurement name and unit for items in this family*/
		QList<Unit> measurementUnitOptions;
		/*! \brief the measurement name and unit for items in this family*/
		Unit measurementUnit;
		/*! \brief the list of numerical attributes that are common to all members of this family*/
		QHash<QString,qreal> numericalAttributes;
		/*! \brief the list of string attributes that are common to all members of this family*/
		QHash<QString,QString> textAttributes;
		/*! \brief the default set of graphics items used to represent items of this family*/
		QList<QGraphicsItem*> graphicsItems;
		/*! \brief the icon representing this family*/
		QPixmap pixmap;
		/*! \brief name of this family*/
		virtual QString name() const;
		/*! \brief set name of this family*/
		virtual void setName(const QString&);
		/*! \brief indicates whether or not the given string is the name of this family or any of its parent families*/
		virtual bool isA(const QString& ) const;
		/*! \brief indicates whether or not the given family is the name of this family or any of its parent families*/
		virtual bool isA(const ItemFamily*) const;
		/*! \brief indicates whether or not the given string is the name of this family or any of its child families*/
		virtual bool isParentOf(const QString& ) const;
		/*! \brief indicates whether or not the given family is the name of this family or any of its child families*/
		virtual bool isParentOf(const ItemFamily*) const;
		/*! \brief get the top-most family*/
		virtual ItemFamily * root() const;
		/*! \brief checks if the given family shares its root family with this family*/
		virtual bool isRelatedTo(const ItemFamily *) const;
		/*! \brief get the parent for this family. If there are more than one parents, returns the one with the highest depth \sa depth*/
		virtual ItemFamily* parent() const;
		/*! \brief counts the number of parents that have to be traversed in order to reach the root handle. 
		If this handle has no parents, the values returned is 0. If its parent has no parent, then the value is 1, and so on.
		\return int*/
		virtual int depth() const;
		/*! \brief get all the parents for this family.*/
		virtual QList<ItemFamily*> parents() const;
		/*! \brief get all the families that inherit directly from this family*/
		virtual QList<ItemFamily*> children() const;
		/*! \brief get all the families that inherit from this family. the list will be ordered in a breadth-first ordering
		*	\return QList<ItemFamily*>
		*/
		virtual QList<ItemFamily*> allChildren() const;
		/*! \brief destructor.*/
		virtual ~ItemFamily();
		/*! \brief constructor
			\param QString name*/
		ItemFamily(const QString& name = QString());
	protected:
		/*! \brief used for casting between different sub-classes*/
		int type;
		/*! \brief indicates whether or not the given family ID is the name of this family or any of its parent families*/
		virtual bool isA(int ID) const;
		/*! \brief name of this family*/
		QString _name;
		/*! \brief the ID for this family. It is used for quick equality checks (instead of using strings)*/
		int ID;
		/*! \brief all family names. This list's lenth is used to assign the next ID*/
		static QStringList ALLNAMES;
		/*! \brief all families by index*/
		static QList<const ItemFamily*> ALLFAMILIES;
		/*! \brief the hash stores names for each ID*/
		static QHash<QString,int> NAMETOID;
		
		friend class NodeFamily;
		friend class ConnectionFamily;
	};

	/*! \brief
	* This class defines the family of a node. Inherits from ItemFamily.
	* It contains a list of NodeGraphicsItems that is the default for this family of nodes
	* \ingroup core
	*/
	class TINKERCELLEXPORT NodeFamily: public ItemFamily
	{
		/*! \brief used for casting between different sub-classes*/
		static int TYPE;
	public:
		/*! \brief cast to node family*/
		static NodeFamily * cast(ItemFamily*);
		/*! \brief cast to node family*/
		static QList<NodeFamily*> cast(const QList<ItemFamily*>&);
		/*! \brief get the parent for this family. If there are more than one parents, returns the first*/
		virtual ItemFamily* parent() const;
		/*! \brief get all the parents for this family.*/
		virtual QList<ItemFamily*> parents() const;
		/*! \brief get all the families that make up this family.*/
		virtual QList<ItemFamily*> children() const;
		/*! \brief set parent family*/
		virtual void setParent(NodeFamily*);
		/*! \brief destructor.*/
		virtual ~NodeFamily();
		/*! \brief constructor.
			\param QString name*/
		NodeFamily(const QString& name = QString());
		/*! \brief indicates whether or not the given string is the name of this family or any of its parent families*/
		virtual bool isA(const QString& ) const;
		/*! \brief indicates whether or not the given family is the name of this family or any of its parent families*/
		virtual bool isA(const ItemFamily*) const;
	protected:
		/*! \brief indicates whether or not the given ID is this family or any of its parent families*/
		virtual bool isA(int) const;
		/*! \brief all the parents*/
		QList<NodeFamily*> parentFamilies;
		/*! \brief all the families that are under this family*/
		QList<NodeFamily*> childFamilies;
		
		friend class ConnectionFamily;
	};

	/*! \brief
	* This class defines the family of a connection. Inherits from ItemFamily
	* It contains a list ofConnectioGraphicsItems that is the default for this family of connections
	* \ingroup core
	*/
	class TINKERCELLEXPORT ConnectionFamily: public ItemFamily
	{
		/*! \brief used for casting between different sub-classes*/
		static int TYPE;
	public:
		/*! \brief cast to connection family*/
		static ConnectionFamily * cast(ItemFamily*);
		/*! \brief cast to connection family*/
		static QList<ConnectionFamily*> cast(const QList<ItemFamily*>&);
		
		/*! \brief get the parent for this family. If there are more than one parents, returns the first*/
		virtual ItemFamily* parent() const;
		/*! \brief get all the parents for this family.*/
		virtual QList<ItemFamily*> parents() const;
		/*! \brief get all the families that make up this family.*/
		virtual QList<ItemFamily*> children() const;
		/*! \brief set parent family*/
		virtual void setParent(ConnectionFamily*);
		/*! \brief destructor.*/
		virtual ~ConnectionFamily();
		/*! \brief constructor.*/
		ConnectionFamily(const QString& name = QString());
		/*! \brief indicates whether or not the given string is the name of this family or any of its parent families*/
		virtual bool isA(const QString& ) const;
		/*! \brief indicates whether or not the given family is the name of this family or any of its parent families*/
		virtual bool isA(const ItemFamily*) const;
		
		/*!  @Participants in a connection and related functions \{*/
		
		/*! \brief add a participant
		* \param QString role of participant
		* \param QString type of participant, must be a family name of a node
		* \return bool false if the participant family does not exist (i.e role not added)*/
		virtual bool addParticipant(const QString& role, const QString& family);		
		/*! \brief get participant family
		* \param QString role of participant
		* \return QString family name (empty if none)*/
		virtual QString participantFamily(const QString& role) const;
		/*! \brief get all participant roles
		* \return QStringList role names (may not be unique)*/
		virtual QStringList participantRoles() const;
		/*! \brief get all participant family names
		* \return QStringList family names (may not be unique)*/
		virtual QStringList participantTypes() const;

		/*! \brief checks if this family is compatible with a connection composed of the given set of nodes
		\param bool QList<NodeHandle*> node handles
		\param bool use false here if the list of nodes is a partial list
		\return Boolean*/
		virtual bool isValidSet(const QList<NodeHandle*>& nodes, bool checkFull=true);
		/*! \brief find child-families of this family that the given set of nodes can potentially belong with
		\param bool QList<NodeHandle*> node handles
		\param bool use false here if the list of nodes is a partial list
		\return QList<ItemFamily*> valid connection families*/
		virtual QList<ItemFamily*> findValidChildFamilies(const QList<NodeHandle*>&, bool checkFull=true);		
		/*! \brief finds the number of node families that are common between the two connections  (the node families should be exactly the same, not isA)
		\param ConnectionFamily * 
		\return bool*/
		virtual int numberOfIdenticalNodesFamilies(ConnectionFamily *) const;
		/*! \brief finds possible role synonyms by looking at child families and finding roles with the same index
		\param QString role name
		\return QStringList synonyms for the input role*/
		virtual QStringList synonymsForRole(const QString& role) const;
		/*!  \} */
		
	protected:
		/*! \brief indicates whether or not the given ID is this family or any of its parent families*/
		virtual bool isA(int) const;
		/*! \brief check for restrictions. RESTRICTIONS ARE HARD CODED. SEE FUNCTION CODE*/
		static bool checkRestrictions(const QString & restriction, const QList<NodeHandle*>&, bool checkFull=true);
		/*! \brief all the parents*/
		QList<ConnectionFamily*> parentFamilies;
		/*! \brief all the families that are under this family*/
		QList<ConnectionFamily*> childFamilies;
		/*! \brief the role ID and type ID of each node that is involved in this connection*/
		QList< QPair<int,int> > nodeRoles;
		/*! \brief stored a list of all possible node roles as IDs*/
		static QHash<QString,int> ROLEID;
		/*! \brief all role names. used to assign role IDs*/
		static QStringList ALLROLENAMES;
	};

}

#endif

