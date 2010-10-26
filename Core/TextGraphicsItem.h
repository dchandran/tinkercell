/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

Class for drawing text on a GraphicsScene. The text can be associated with 
a handle

****************************************************************************/

#ifndef TINKERCELL_TEXTGRAPHICSITEM_H
#define TINKERCELL_TEXTGRAPHICSITEM_H

#include <QGraphicsTextItem>
#include <QTextCursor>
#include <QUndoCommand>
#include "ItemHandle.h"

#ifdef Q_WS_WIN
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{
	class ItemHandle;

	/*! \brief editable text item 
	\ingroup core
	*/
	class TINKERCELLEXPORT TextGraphicsItem : public QGraphicsTextItem
	{
		Q_OBJECT

	public:
		/*! \brief this text item's handle */
		virtual ItemHandle * handle() const;
		/*! \brief set this text item's handle */
		void setHandle(ItemHandle *);
		/*! \brief Constructor		
		* \param QString text
		* \param QGraphicsItem* parent
		*/
		TextGraphicsItem(const QString& text, QGraphicsItem* parent = 0);
		/*! \brief Constructor	
		* \param QGraphicsItem* parent
		*/
		TextGraphicsItem(QGraphicsItem* parent = 0);
		/*! \brief Copy Constructor		
		* \param TextGraphicsItem* copy
		*/
		TextGraphicsItem(const TextGraphicsItem& copy);
		/*! \brief Clone this item
		*/
		virtual TextGraphicsItem* clone();
		/*! \brief Copy Constructor		
		* \param ItemHandle* handle to which this item belongs
		* \param QGraphicsItem* parent
		*/
		TextGraphicsItem(ItemHandle * handle, QGraphicsItem* parent = 0);
		/*! \brief Destructor
		*/
		virtual ~TextGraphicsItem();
		/*! \brief Paint this text item with or without a border
		*/
		virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
		/*! \brief whether or not to paint this item with a border
		*/
		virtual void showBorder(bool show=true);
		/*! \brief the string painted by this text graphics item. same as toPlainText
		\return QString
		*/
		virtual QString text() const;
		/*! \brief set the string painted by this text graphics item. same as setPlainText
		\param QString
		*/
		virtual void setText(const QString&);
		/*! \brief cast a graphics item to a text item using qgraphicsitem_cast
		\param QGraphicsItem graphics item
		\return TextGraphicsItem this will be 0 if the cast is invalid
		*/
		static TextGraphicsItem* cast(QGraphicsItem *);
		/*! \brief relative position with a target item
		*/
		QPair<QGraphicsItem*,QPointF> relativePosition;

		/*! \brief for enabling dynamic_cast*/
		enum { Type = UserType + 8};
		/*! \brief for enabling dynamic_cast*/
		int type() const
		{
			// Enable the use of dynamic_cast with this item.
			return Type;
		}

		/*! \brief for identifying which group this item belongs in*/
		QString groupID;

signals:
		// void lostFocus(TextGraphicsItem *item);
		// void selectedChange(QGraphicsItem *item);

	protected:
		// QVariant itemChange(GraphicsItemChange change, const QVariant &value);
		// void focusOutEvent(QFocusEvent *event);
		//void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

		/*! \brief draws a border around the text item. hide or show using showBorder()*/
		QGraphicsRectItem * boundingRectItem;
		/*! \brief the handle in which this item belongs
		*/
		ItemHandle * itemHandle;

	};
}
#endif
