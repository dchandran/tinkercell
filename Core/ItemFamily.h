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
	};

	/*! \brief This class defines the family of a node or connection.
	The class contains the icon for the family, family name, and minimal data that
	defines the family.
	\ingroup core
	*/
	class TINKERCELLEXPORT ItemFamily
	{
	public:
		/*! \brief used for casting between different sub-classes*/
		int type;
		/*! \brief description of this family*/
		QString description;
		/*! \brief name of this family*/
		QString name;
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
		/*! \brief indicates whether or not the given string is the name of this family or any of its parent families*/
		virtual bool isA(const QString& ) const;
		/*! \brief indicates whether or not the given family is the name of this family or any of its parent families*/
		virtual bool isA(const ItemFamily*) const;
		/*! \brief get the top-most family*/
		virtual ItemFamily * root() const;
		/*! \brief checks if the given family shares its root family with this family*/
		virtual bool isRelatedTo(const ItemFamily *) const;
		/*! \brief get the parent for this family. If there are more than one parents, returns the first*/
		virtual ItemFamily* parent() const { return 0; }
		/*! \brief get all the parents for this family.*/
		virtual QList<ItemFamily*> parents() const { return QList<ItemFamily*>(); }
		/*! \brief get all the families that inherit directly from this family*/
		virtual QList<ItemFamily*> children() const { return QList<ItemFamily*>(); }
		/*! \brief get all the families that inherit from this family. the list will be ordered in a breadth-first ordering
		*	\return QList<ItemFamily*>
		*/
		virtual QList<ItemFamily*> allChildren() const;
		/*! \brief constructor.
			\param QString name*/
		ItemFamily(const QString& name = QString());
		/*! \brief destructor.*/
		virtual ~ItemFamily();
	};

	/*! \brief
	* This class defines the family of a node. Inherits from ItemFamily.
	* It contains a list of NodeGraphicsItems that is the default for this family of nodes
	* \ingroup core
	*/
	class TINKERCELLEXPORT NodeFamily: public ItemFamily
	{
	public:
		/*! \brief used for casting between different sub-classes*/
		static int TYPE;
		/*! \brief cast to connection family*/
		static NodeFamily * cast(ItemFamily*);
		/*! \brief indicates whether or not the given string is the name of this family or any of its parent families*/
		virtual bool isA(const QString& familyName) const;
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
	protected:
		/*! \brief all the parents*/
		QList<NodeFamily*> parentFamilies;
		/*! \brief all the families that are under this family*/
		QList<NodeFamily*> childFamilies;
	};

	/*! \brief
	* This class defines the family of a connection. Inherits from ItemFamily
	* It contains a list ofConnectioGraphicsItems that is the default for this family of connections
	* \ingroup core
	*/
	class TINKERCELLEXPORT ConnectionFamily: public ItemFamily
	{
	public:
		/*! \brief used for casting between different sub-classes*/
		static int TYPE;
		/*! \brief cast to connection family*/
		static ConnectionFamily * cast(ItemFamily*);
		
		/*! \brief the type of each node that is involved in this connection*/
		QStringList nodeFamilies;
		/*! \brief the role of each node in this connection*/
		QStringList nodeFunctions;
		
		/*! \brief indicates whether or not the given string is the name of this family or any of its parent families*/
		virtual bool isA(const QString& familyName) const;
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
	protected:
		/*! \brief all the parents*/
		QList<ConnectionFamily*> parentFamilies;
		/*! \brief all the families that are under this family*/
		QList<ConnectionFamily*> childFamilies;
	};

}

#endif
