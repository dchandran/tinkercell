/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 A special ConnectionGraphicsItem for connecting modules

****************************************************************************/

#ifndef TINKERCELL_MODULECONNECTIONITEM_H
#define TINKERCELL_MODULECONNECTIONITEM_H

#include <QtDebug>
#include <QString>
#include <QColor>
#include <QDir>
#include <QGraphicsLineItem>
#include "ItemHandle.h"
#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "TextGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"

/*! \brief A 1-to-1 connection graphics item without a handle */

namespace Tinkercell
{
 
class ModuleLinkerItem : public NodeGraphicsItem
{
public:

	qreal setWidth;

	ModuleLinkerItem(NodeGraphicsItem * mod=0, QGraphicsItem * parent = 0, TextGraphicsItem * text = 0);
	
	virtual void setPosOnEdge();
	
	virtual NodeGraphicsItem * clone() const;
	
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option=new QStyleOptionGraphicsItem() ,QWidget *widget=0);
	
	NodeGraphicsItem * module;
	
	/*! \brief used for checking type before static casts */
	static QString class_name;
private:
	TextGraphicsItem  * textItem;
	QGraphicsLineItem * lineItem;
};

class ModuleConnectionGraphicsItem : public ConnectionGraphicsItem
{
public:
	/*! Constructor: sets the class name as ModuleConnectionGraphicsItem */
    ModuleConnectionGraphicsItem(QGraphicsItem * parent = 0);
	
	ModuleConnectionGraphicsItem(const ModuleConnectionGraphicsItem&);
	
	ConnectionGraphicsItem* clone() const;
	
	/*! \brief used for checking type before static casts */
	static QString class_name;
	
	QUndoCommand * command;
	
	~ModuleConnectionGraphicsItem();
	
	virtual void adjustEndPoints();
	
	static bool isModuleConnectionItem(ConnectionGraphicsItem*);
	
};

}
#endif
