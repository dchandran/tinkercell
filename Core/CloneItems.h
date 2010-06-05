/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines functions used to clone graphics items. 
These functions used by the copy/paste functions in GraphicsScene and may
also be used by other plugins

****************************************************************************/

#ifndef TINKERCELL_CLONEGRAPHICSITEMS_H
#define TINKERCELL_CLONEGRAPHICSITEMS_H

#include <stdlib.h>
#include <QtGui>
#include <QList>
#include <QGraphicsItem>

#ifdef Q_WS_WIN
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{
	class ItemHandle;
	class TextItem;
	
	/*! \brief gets the parent of this item that is a node, text, connection, or control point
	* \param QGraphicsItem * Qt graphics item
	\return QGraphicsItem * node, connection, text, or control point
	\ingroup core
	*/
	TINKERCELLEXPORT QGraphicsItem * getGraphicsItem( QGraphicsItem * item );
	/*! \brief Clone a graphics item. The item handle will NOT be duplicated
	* \param QGraphicsItem * a pointer to a QGraphicsItem
	\return QGraphicsItem * a QGraphicsItem that is clone of the argument
	\ingroup core
	*/
	TINKERCELLEXPORT QGraphicsItem * cloneGraphicsItem( QGraphicsItem * item );
	/*! \brief Clone a list of graphics items
	* \param QList<QGraphicsItem*> a list of pointers to a QGraphicsItems
	* \param QList<ItemHandle*> return value: returns all the new handles here
	* \param bool duplicate the handles as well (default = true). 
	\return QList<QGraphicsItem*> a new list of QGraphicsItems that are clones of the corresponding argument
	\ingroup core
	*/
	TINKERCELLEXPORT QList<QGraphicsItem*> cloneGraphicsItems( QList<QGraphicsItem*>& items, QList<ItemHandle*>& newHandles , bool deep = true);
	/*! \brief clone given handles
		\param QList<ItemHandle*> items to clone
		\return QList<ItemHandle*> clones
	*/
	TINKERCELLEXPORT QList<ItemHandle*> cloneHandles(const QList<ItemHandle*>&);
	
}

#endif
