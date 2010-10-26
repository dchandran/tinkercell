/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

Class for drawing text on a GraphicsScene. The text can be associated with
a handle

****************************************************************************/

#include <QPainter>
#include "MainWindow.h"
#include "TextGraphicsItem.h"

namespace Tinkercell
{

ItemHandle * TextGraphicsItem::handle() const
{
	return itemHandle;
}

void TextGraphicsItem::setHandle(ItemHandle * handle)
{
	if (handle != 0 && !handle->graphicsItems.contains(this))
	{
		handle->graphicsItems += this;
	}

	if (itemHandle)
	{
		if (itemHandle != handle)
		{
			itemHandle->graphicsItems.removeAll(this);
			itemHandle = handle;
		}
	}
	else
	{
		itemHandle = handle;
	}
}

/*! Constructor: sets text edit interaction */
TextGraphicsItem::TextGraphicsItem(const QString& text, QGraphicsItem* parent) :
QGraphicsTextItem(text,parent), relativePosition(QPair<QGraphicsItem*,QPointF>(0,QPointF()))
{
	setTextInteractionFlags(Qt::TextEditorInteraction);
	setFlag(QGraphicsItem::ItemIsMovable);
	//setFlag(QGraphicsItem::ItemIsSelectable);
	itemHandle = 0;
	boundingRectItem = new QGraphicsRectItem(this);
	boundingRectItem->setPen(QPen(QColor(100,100,100),2));
	boundingRectItem->setBrush(Qt::NoBrush);
	boundingRectItem->setVisible(false);
}
/*! Constructor: sets text edit interaction */
TextGraphicsItem::TextGraphicsItem(QGraphicsItem* parent) :
QGraphicsTextItem(parent), relativePosition(QPair<QGraphicsItem*,QPointF>(0,QPointF()))
{
	setTextInteractionFlags(Qt::TextEditorInteraction);
	setFlag(QGraphicsItem::ItemIsMovable);
	//setFlag(QGraphicsItem::ItemIsSelectable);
	itemHandle = 0;
	boundingRectItem = new QGraphicsRectItem(this);
	boundingRectItem->setPen(QPen(QColor(100,100,100),2));
	boundingRectItem->setBrush(Qt::NoBrush);
	boundingRectItem->setVisible(false);
}
/*! Constructor: sets text edit interaction and name of handle */
TextGraphicsItem::TextGraphicsItem(ItemHandle * handle, QGraphicsItem* parent) :
QGraphicsTextItem(parent), relativePosition(QPair<QGraphicsItem*,QPointF>(0,QPointF()))
{
	if (handle) setPlainText(handle->name);
	itemHandle = 0;
	setHandle(handle);
	setTextInteractionFlags(Qt::TextEditorInteraction);
	setFlag(QGraphicsItem::ItemIsMovable);
	//setFlag(QGraphicsItem::ItemIsSelectable);
	boundingRectItem = new QGraphicsRectItem(this);
	boundingRectItem->setPen(QPen(QColor(100,100,100),2));
	boundingRectItem->setBrush(Qt::NoBrush);
	boundingRectItem->setVisible(false);
}
/*! Copy Constructor */
TextGraphicsItem::TextGraphicsItem(const TextGraphicsItem& copy) : QGraphicsTextItem(), relativePosition(copy.relativePosition)
{
	setPos(copy.scenePos());
	setTransform(copy.transform());
	setDefaultTextColor(copy.defaultTextColor());
	setVisible(copy.isVisible());
	//setDocument(copy.document());
	setFont(copy.font());
	setHtml (copy.toHtml());
	setTextWidth(copy.textWidth());
	setTextCursor(copy.textCursor());
	setTextInteractionFlags ( copy.textInteractionFlags() );
	setFlag(QGraphicsItem::ItemIsMovable);
	//setFlag(QGraphicsItem::ItemIsSelectable);
	setPlainText(copy.toPlainText());
	itemHandle = copy.itemHandle;

	if (itemHandle)
		setHandle(itemHandle);

	boundingRectItem = new QGraphicsRectItem(this);
	boundingRectItem->setPen(QPen(QColor(100,100,100),2));
	boundingRectItem->setBrush(Qt::NoBrush);
	boundingRectItem->setVisible(false);
}
TextGraphicsItem* TextGraphicsItem::clone()
{
	return new TextGraphicsItem(*this);
}
TextGraphicsItem::~TextGraphicsItem()
{
    if (!itemHandle) return;
    setHandle(0);
}

void TextGraphicsItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	QGraphicsTextItem::paint(painter,option,widget);
}

void TextGraphicsItem::showBorder(bool showBorder)
{
	boundingRectItem->setRect(this->boundingRect());
	boundingRectItem->setVisible(showBorder);
	update();
}

QString TextGraphicsItem::text() const
{
	return toPlainText();
}

void TextGraphicsItem::setText(const QString& s)
{
	setPlainText(s);
}

TextGraphicsItem* TextGraphicsItem::cast(QGraphicsItem * q)
{
	//if (MainWindow::invalidPointers.contains( (void*)q )) return 0;
	return qgraphicsitem_cast<TextGraphicsItem*>(q);
}

}

