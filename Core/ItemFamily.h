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
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
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

	/*! \brief This class defines the family of a node or connection.
	The class contains the icon for the family, family name, and minimal data that
	defines the family.
	\ingroup core
	*/
	class MY_EXPORT ItemFamily
	{
	public:
		/*! \brief used for casting between different sub-classes*/
		int type;
		/*! \brief description of this family*/
		QString description;
		/*! \brief name of this family*/
		QString name;
		/*! \brief the measurement name and unit for items in this family. Example: <"concentration", "mM">*/
		QPair<QString, QString> measurementUnit;
		/*! \brief the list of numerical attributes that are common to all members of this family*/
		QHash<QString,qreal> numericalAttributes;
		/*! \brief the list of string attributes that are common to all members of this family*/
		QHash<QString,QString> textAttributes;
		/*! \brief the default set of graphics items used to represent items of this family*/
		QList<QGraphicsItem*> graphicsItems;
		/*! \brief color used to identify this family*/
		QColor color;
		/*! \brief the icon representing this family*/
		QPixmap pixmap;
		/*! \brief indicates whether or not the given string is the name of this family or any of its parent families*/
		virtual bool isA(const QString& ) const;
		/*! \brief indicates whether or not the given family is the name of this family or any of its parent families*/
		virtual bool isA(const ItemFamily*) const;
		/*! \brief get the top-most family*/
		virtual const ItemFamily * root() const;
		/*! \brief checks if the given family shares its root family with this family*/
		virtual bool isRelatedTo(const ItemFamily *) const;
		/*! \brief get the parent for this family. If there are more than one parents, returns the first*/
		virtual ItemFamily* parent() const { return 0; }
		/*! \brief get all the parents for this family.*/
		virtual QList<ItemFamily*> parents() const { return QList<ItemFamily*>(); }
		/*! \brief get all the families that inherit from this family*/
		virtual QList<ItemFamily*> children() const { return QList<ItemFamily*>(); }
		/*! \brief destructor.*/
		virtual ~ItemFamily();
		/*! \brief constructor.*/
		ItemFamily();
	};

	/*! \brief
	* This class defines the family of a node. Inherits from ItemFamily.
	* It contains a list of NodeGraphicsItems that is the default for this family of nodes
	* \ingroup core
	*/
	class MY_EXPORT NodeFamily: public ItemFamily
	{
	public:
		/*! \brief used for casting between different sub-classes*/
		static int TYPE;
		/*! \brief cast to connection family*/
		static NodeFamily * asNode(ItemFamily*);
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
		/*! \brief constructor.*/
		NodeFamily();
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
	class MY_EXPORT ConnectionFamily: public ItemFamily
	{
	public:
		/*! \brief used for casting between different sub-classes*/
		static int TYPE;
		/*! \brief cast to connection family*/
		static ConnectionFamily * asConnection(ItemFamily*);
		/*! \brief arrow used to represent this family in text-based networks*/
		QString string;
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
		ConnectionFamily();
	protected:
		/*! \brief all the parents*/
		QList<ConnectionFamily*> parentFamilies;
		/*! \brief all the families that are under this family*/
		QList<ConnectionFamily*> childFamilies;
	};

}

#endif
