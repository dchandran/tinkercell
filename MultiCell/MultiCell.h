/****************************************************************************

 Copyright (C) 2009 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYWRITE.TXT

Grid based multicell visual modeling interface
****************************************************************************/


#ifndef MULTICELL_INTERFACE_TOOL
#define MULTICELL_INTERFACE_TOOL

#include <QButtonGroup>
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QToolButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QListWidget>
#include <QGraphicsSimpleTextItem>
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "NetworkWindow.h"
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "Tool.h"
#include "NodeGraphicsReader.h"
#include "ConsoleWindow.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif


using namespace Tinkercell;

namespace Multicell
{


	class MY_EXPORT CellNode: public NodeGraphicsItem
	{
	public:
		
		/*! \brief constructed*/
		CellNode();
		
		/*! \brief copy constructed*/
		CellNode(const CellNode&);
		
		/*! \brief calls copy constructed*/
		NodeGraphicsItem* clone() const;
		
		void setCentralColor(const QColor&);

	};
	
	class CellTypeSelector;


	class MY_EXPORT MulticellInterface : public Tool
	{
		Q_OBJECT
		
	public:
		MulticellInterface();

		bool setMainWindow(MainWindow*);
		
		QColor currentColor;
		
		NodeFamily * currentFamily;
		
		CellTypeSelector * cellSelector;

	public slots:
	
		void cellTypeSelected(NodeFamily*,const QColor&);

		/*! \brief signals whenever an empty node of the screen is clicked
		* \param GraphicsScene* scene where the event took place
		* \param QPointF point where mouse is clicked
		* \param Qt::MouseButton which button was pressed
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		virtual void mousePressed(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		/*! \brief signals whenever an empty node of the screen is clicked
		* \param GraphicsScene* scene where the event took place
		* \param QPointF point where mouse is clicked
		* \param Qt::MouseButton which button was pressed
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		virtual void mouseReleased(GraphicsScene * scene, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		/*! \brief emits event when mouse is double clicked
		* \param GraphicsScene* scene where the event took place
		* \param point where mouse is clicked
		* \param modifier keys being used when mouse clicked
		* \return void*/
		virtual void mouseDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		/*! \brief signals whenever mouse is dragged from one point to another
		* \param GraphicsScene* scene where the event took place
		* \param QPointF point where mouse is clicked first
		* \param QPointF point where mouse is released
		* \param Qt::MouseButton button being pressed
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		virtual void mouseDragged(GraphicsScene * scene, QPointF from, QPointF to, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		/*! \brief signals whenever items are being moved (each item is the top-most item)
		* \param GraphicsScene* scene where the items were moved
		* \param QList<QGraphicsItem*>& list of pointes to all moving items
		* \param QPointF point where the item was
		* \param QPointF point where the item is moved to
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		virtual void itemsMoved(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<QPointF>& distance, Qt::KeyboardModifiers modifiers);
		/*! \brief signals whenever mouse moves, and indicates whether it is on top of an item
		* \param GraphicsScene* scene where the event took place
		* \param QGraphicsItem* pointer to item that mouse is on top of
		* \param QPointF point where mouse is clicked
		* \param Qt::MouseButton button being pressed
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \param QList<QGraphicsItem*>& list of items that are being moved with the mouse
		* \return void*/
		virtual void mouseMoved(GraphicsScene * scene, QGraphicsItem* item, QPointF point, Qt::MouseButton, Qt::KeyboardModifiers modifiers, QList<QGraphicsItem*>&);
		/*! \brief signals whenever items are selected (item can be sub-item, not top-level)
		* \param GraphicsScene* scene where items are selected
		* \param QList<QGraphicsItem*>& list of all selected item pointers
		* \param QPointF point where mouse is clicked
		* \param Qt::KeyboardModifiers modifier keys being used when mouse clicked
		* \return void*/
		virtual void itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers);
		void escapeSignal(const QWidget * sender);
	private:
			
		QString uniqueName();
				
		NodeGraphicsItem* itemOnTopOf;
		QList<NodeGraphicsItem*> selectedItems;
	};

}
#endif
