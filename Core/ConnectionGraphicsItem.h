/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This is the header file for the connection graphics item, which is a Qt graphics path item
 that draws a connection between two or more nodes.

 The connection graphics items defines a new control point item inside itself.

 This header file also contains the arrow head item class. The arrow head item inherits from
 node graphics item and is used to draw the arrow heads at the end of the connection items.


****************************************************************************/

#ifndef TINKERCELL_CONNECTIONGRAPHICSITEM_H
#define TINKERCELL_CONNECTIONGRAPHICSITEM_H

#include <math.h>
#include <QtDebug>
#include <QString>
#include <QColor>
#include <QPointF>
#include <QPainter>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QUndoCommand>
#include "NodeGraphicsItem.h"

namespace Tinkercell
{

/*! \brief gets the point on the edge of the rect such that it is in the same
line as the center of the rect and the point (arg)
 \param QRectF rectangle
 \param QPointF point outside rectangle
 \return QPointF the point on the edge of the rectangle
 \ingroup helper
*/
QPointF pointOnEdge(const QRectF&, const QPointF&, qreal dist = 0, bool straight = false);
/*! \brief gets the point on the edge of the shape such that it is in the same
line as the center of the shape's bounding rect and the point (arg)
 \param QPainterPath the shape
 \param QPointF point outside shape
 \return QPointF the point on the edge of the shape
 \ingroup helper
*/
QPointF pointOnEdge(const NodeGraphicsItem&, const QPointF&, qreal dist = 0, bool straight = false);

class GraphicsScene;
class ItemHandle;
class ConnectionGraphicsItem;

/*! \brief A node graphics item that is used to draw arrow heads on connection items.
	\ingroup core*/
class ArrowHeadItem : public NodeGraphicsItem
{
public:
	/*! \brief The connection item that this arrow head belongs with*/
	ConnectionGraphicsItem * connectionItem;
	/*! \brief the direction (angle) that the arrow is pointing*/
	qreal angle;
	/*! \brief constructor -- initializes the angle and connection item*/
	ArrowHeadItem(ConnectionGraphicsItem * c = 0);
	/*! \brief copy constructor */
	ArrowHeadItem(const ArrowHeadItem&);
	/*! \brief paint this arrow item. performs rotation using the angle member.
	\return void
	*/
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option=new QStyleOptionGraphicsItem() ,QWidget *widget=0);
	/*! \brief returns a duplicate of this arrow head
	\return duplicate arrow head item
	*/
	virtual NodeGraphicsItem* clone() const;
	/*! \brief used for checking type before static casts */
	static QString class_name;
};

/*! \brief A graphics nodes item that draws connection between two or more nodes and the arrow heads at the ends.
	\ingroup core*/
class ConnectionGraphicsItem : public QGraphicsPathItem
{
public:
	/*! \brief used for checking type before static casts */
	static QString class_name;
	/*! \brief used for checking type before static casts */
	QString className;
	/*! \brief permanent brush for this control point*/
	QBrush defaultBrush;
	/*! \brief permanent pen for this control point*/
	QPen defaultPen;
	/*! \brief gets the connection graphics item from its child item
	* \param QGraphicsItem* the target item
	* \param bool using true here will return the connection item for a control point, otherwise control points are ignored
	*/
	static ConnectionGraphicsItem * topLevelConnectionItem(QGraphicsItem* item,bool includeControlPoints = false);
	/*! \brief Tinkercell object that this drawable belongs in */
	ItemHandle * itemHandle;
	/*! Constructor: does nothing */
	ConnectionGraphicsItem(QGraphicsItem * parent = 0 );
	/*! Copy Constructor: copies handle but not control points */
	ConnectionGraphicsItem(const ConnectionGraphicsItem& copy);
	/*! operator =: remove everything from original connection and copy everything from the given connection */
	ConnectionGraphicsItem& operator = (const ConnectionGraphicsItem& copy);
	/*! operator =: copy just the control point positions and pen */
	ConnectionGraphicsItem& copyPoints (const ConnectionGraphicsItem& copy);
	/*! \brief make a copy of this connection item*/
	virtual ConnectionGraphicsItem* clone() const;
	/*! \brief returns the bounding rectangle for this reaction figure*/
	//virtual QRectF boundingRect() const;
	/*! \brief paint method. Call's parent's after drawing boundary true*/
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option=new QStyleOptionGraphicsItem() ,QWidget *widget=0);
	/*! \brief checks that this is a valid drawable*/
	virtual bool isValid();
	/*! \brief number of different type of shapes available*/
	static const int numLineTypes = 2;
	/*! \brief line or beizier */
	typedef enum {line, bezier} LineType;

	/*! \brief type of line for this reaction - line or beizier */
	LineType lineType;

	/*! \brief A control point with a pointer to a ConnectionGraphicsItem
		\ingroup core*/
	class ControlPoint : public Tinkercell::ControlPoint
	{
	public:
		/*! \brief idrawables that this control point belong in*/
		ConnectionGraphicsItem * connectionItem;
		/*! \brief Constructor: Setup colors and z value */
		ControlPoint(ConnectionGraphicsItem * reaction_ptr = 0, QGraphicsItem * parent = 0);
		/*! \brief Constructor: constructor with position */
		ControlPoint(const QPointF& pos, ConnectionGraphicsItem * reaction_ptr = 0, QGraphicsItem * parent = 0);
		/*! \brief Copy Constructor */
		ControlPoint(const ControlPoint& copy);
		/*! Copy operator */
		ControlPoint& operator = (const ControlPoint& copy);
		/*! \brief for enabling dynamic_cast*/
		enum { Type = UserType + 6 };
		/*! \brief for enabling dynamic_cast*/
		virtual int type() const
		{
			// Enable the use of dynamic_cast with this item.
			return Type;
		}
		/*! \breif destructor*/
		~ControlPoint();
		/*! \brief side effect when moved. always call this after moving*/
		//virtual void sideEffect();
		/*! \brief make a copy of this control point*/
		virtual Tinkercell::ControlPoint* clone() const;
	};

	/*! \brief A set of control points and two arrow heads
		\ingroup core*/
	class PathVector : public QVector<ConnectionGraphicsItem::ControlPoint*>
	{
	public:
		PathVector();
		PathVector(int);
		PathVector(int,ConnectionGraphicsItem::ControlPoint*);
		PathVector(const PathVector&);
		ArrowHeadItem *arrowStart, *arrowEnd;
	};

	/*! \brief vector of vector of control point*/
	QList<PathVector> pathVectors;
	/*! \brief distance from arrow head to the item that it is connected to*/
	qreal arrowHeadDistance;
	/*! \brief list of pointers to all the control points*/
	virtual QList<ControlPoint*> controlPoints(bool includeEnds = false) const;
	/*! \brief list of pointers to all the control points*/
	virtual QList<QGraphicsItem*> controlPointsAsGraphicsItems(bool includeEnds = false) const;
	/*! \brief gets a path that represents this reactionimage*/
	virtual QPainterPath shape() const;
	/*! \brief Clear all shapes and control points
	* \param void
	* \return void*/
	virtual void clear(bool all=false);
	/*! \brief refresh the path if any controlpoints have moved
	* \param void
	* \return void*/
	virtual void refresh();
	/*! \brief set visibility of control points
	* \param visible = true, invisible = false
	* \return void*/
	virtual void setControlPointsVisible(bool visible = true);
	/*! \brief get all nodes that are connected
	* \return node item list*/
	virtual QList<NodeGraphicsItem*> nodes() const;
	/*! \brief get all nodes that have an arrow pointing to them
	* \return node item list*/
	virtual QList<NodeGraphicsItem*> nodesWithArrows() const;
	/*! \brief get all nodes that do NOT have an arrow pointing to them
	* \return node item list*/
	virtual QList<NodeGraphicsItem*> nodesWithoutArrows() const;
	/*! \brief get all nodes that are not directle connected to the main connection, such as modifier nodes
	* \return node item list*/
	virtual QList<NodeGraphicsItem*> nodesDisconnected() const;
	/*! \brief get all nodes that are connected
	* \return graphics item list*/
	virtual QList<QGraphicsItem*> nodesAsGraphicsItems() const;
	/*! \brief get all the arrowHeads associated with the nodes. The order is the same order as nodes(), so values can be 0
	* \return node item list*/
	virtual QList<ArrowHeadItem*> arrowHeads() const;
	/*! \brief get all the arrowHeads associated with the nodes The order is the same order as nodes(), so values can be 0
	* \return arrow item list*/
	virtual QList<QGraphicsItem*> arrowHeadsAsGraphicsItems() const;
	/*! \brief get all the arrowHeads NOT associated with the nodes
	* \return graphics item list*/
	virtual QList<ArrowHeadItem*> modifierArrowHeads() const;
	/*! \brief get the node that connected to the particular path
	* \param index less than size of pathVectors
	* \return node item or 0*/
	virtual NodeGraphicsItem* nodeAt(int index) const;
	/*! \brief get the index of the node
	* \param node in this connection
	* \return index, -1 if node not found*/
	virtual int indexOf(QGraphicsItem * node) const;
	/*! \brief replace the node at the particular position with a new node
	* \param index where to insert the new node
	* \param new node
	* \return void*/
	virtual void replaceNodeAt(int,NodeGraphicsItem*);
	/*! \brief replace one node in the reaction with another
	* \param target node to replace
	* \param new node
	* \return void*/
	virtual void replaceNode(NodeGraphicsItem*,NodeGraphicsItem*);
	/*! \brief get the arrow head at the particular index
	* \param index less than size of pathVectors
	* \return node item or 0*/
	virtual ArrowHeadItem* arrowAt(int index) const;
	/*! \brief get the modifier arrow head at the particular index
	* \param index less than size of pathVectors
	* \return node item or 0*/
	virtual ArrowHeadItem* modifierArrowAt(int index) const;
	/*! Destructor: deletes all control points */
	virtual ~ConnectionGraphicsItem();
	/*! \brief get slope at the given point (or closest point)*/
	virtual qreal slopeAtPoint(const QPointF& point);
	/*! \brief indicates whether to show lines around the curves*/
	bool controlPointsVisible;
	/*! \brief the center point (if one exists)*/
	virtual ControlPoint * centerPoint() const;
	/*! \brief the center point (if one exists)*/
	virtual QPointF centerLocation() const;
	/*! \brief a rectangle that sits at the center of the connector*/
	QSizeF centerRegion;
	/*! \brief show or hide the rectangle that sits at the center of the connector*/
	bool centerRegionVisible;
	/*! \brief the image on the rectangle that sits at the center of the connector*/
	ArrowHeadItem * centerRegionItem;
	/*! \brief bounding rect*/
	virtual QRectF boundingRect() const;
	/*! \brief scene bounding rect*/
	virtual QRectF sceneBoundingRect() const;
	/*! \brief for enabling dynamic_cast*/
	enum { Type = UserType + 5 };
	/*! \brief for enabling dynamic_cast*/
	virtual int type() const
    {
		// Enable the use of dynamic_cast with this item.
        return Type;
    }
protected:
	/*! \brief path of the boundary region of the entire connection*/
	QGraphicsPathItem * boundaryPathItem;
	/*! \brief path of the selection region of the entire connection*/
	QPainterPath pathShape;
	/*! \brief the boundary rectangle for this path. It is recomputed during each refresh.*/
	QRectF pathBoundingRect;
	/*! \brief update the boundary path*/
	virtual void refreshBoundaryPath();
	/*! \brief adjust the end control points so that they point straight*/
	virtual void adjustEndPoints();
};

/*! \brief An command that adds a new control point to a connection item; it has undo and redo functionality
	\ingroup undo*/
class AddControlPointCommand : public QUndoCommand
{
public:
	/*! \brief constructor that makes the command. If added to history stack, also does redo
	* \param name
	* \param graphics scene
	* \param control point(s) that have been added
	* \return void*/
	AddControlPointCommand(const QString& name, QGraphicsScene * scene,
		            ConnectionGraphicsItem::ControlPoint * item );
	/*! \brief constructor that makes the command. If added to history stack, also does redo
	* \param name
	* \param graphics scene
	* \param control point(s) that have been added
	* \return void*/
	AddControlPointCommand(const QString& name, QGraphicsScene * scene,
		QList<ConnectionGraphicsItem::ControlPoint *> items);
	/*! \brief destructor. deletes all control points that do not belong a scene*/
	virtual ~AddControlPointCommand();
	/*! \brief Adds a new control point. Control points were set in the constructor
	* \param void
	* \return void*/
	void redo();
	/*! \brief Remove new control points. Control points were set in the constructor
	* \param void
	* \return void*/
	void undo();
	/*! \brief graphics scene to which control points were added*/
	QGraphicsScene * graphicsScene;
	/*! \brief control points that were added*/
	QList<ConnectionGraphicsItem::ControlPoint*> graphicsItems;
	/*! \brief the poisition(s) at which the control points were added*/
	QList<int> listK1, listK2;
};

/*! \brief A command that removed control points. Allows undo and redo
	\ingroup undo*/
class RemoveControlPointCommand : public QUndoCommand
{
public:
	/*! \brief constructor that makes the command. If added to history stack, also does redo
	* \param name
	* \param graphics scene
	* \param control point(s) that have been added
	* \return void*/
	RemoveControlPointCommand(const QString& name, QGraphicsScene * scene,
					ConnectionGraphicsItem::ControlPoint * item);
	/*! \brief constructor that makes the command. If added to history stack, also does redo
	* \param name
	* \param graphics scene
	* \param control point(s) that have been added
	* \return void*/
	RemoveControlPointCommand(const QString& name, QGraphicsScene * scene,
		             QList<ConnectionGraphicsItem::ControlPoint *> items);
	/*! \brief Remove new control points. Control points were set in the constructor
	* \param void
	* \return void*/
	void redo();
	/*! \brief Add new control points. Control points were set in the constructor
	* \param void
	* \return void*/
	void undo();
	/*! \brief control points that were added*/
	QList<ConnectionGraphicsItem::ControlPoint*> graphicsItems;
	/*! \brief graphics scene to which control points were added*/
	QGraphicsScene * graphicsScene;
	/*! \brief the poisition(s) at which the control points were added*/
	QList<int> listK1, listK2;
};

/*! \brief An command that adds a new control point to a connection item; it has undo and redo functionality
	\ingroup undo*/
class AddPathVectorCommand : public QUndoCommand
{
public:
	/*! \brief constructor that makes the command. If added to history stack, also does redo
	* \param name
	* \param graphics scene
	* \param control point(s) that have been added
	* \return void*/
	AddPathVectorCommand(const QString& name, QGraphicsScene * scene, ConnectionGraphicsItem* connection,
		            ConnectionGraphicsItem::PathVector& item );
	/*! \brief constructor that makes the command. If added to history stack, also does redo
	* \param name
	* \param graphics scene
	* \param control point(s) that have been added
	* \return void*/
	AddPathVectorCommand(const QString& name, QGraphicsScene * scene, ConnectionGraphicsItem* connection,
		QList<ConnectionGraphicsItem::PathVector> items);
	/*! \brief destructor. deletes all control points that do not belong a scene*/
	virtual ~AddPathVectorCommand();
	/*! \brief Adds a new control point. Control points were set in the constructor
	* \param void
	* \return void*/
	void redo();
	/*! \brief Remove new control points. Control points were set in the constructor
	* \param void
	* \return void*/
	void undo();
	/*! \brief graphics scene to which control points were added*/
	QGraphicsScene * graphicsScene;
	/*! \brief graphics item to which control points were added*/
	ConnectionGraphicsItem* connectionItem;
	/*! \brief vector of control points that were added*/
	QList<ConnectionGraphicsItem::PathVector> pathVectors;
	/*! \brief the poisition(s) at which the control point vectors were added*/
	QList<int> listK1;
};

/*! \brief A command that removed control points. Allows undo and redo
	\ingroup undo*/
class RemovePathVectorCommand : public QUndoCommand
{
public:
	/*! \brief constructor that makes the command. If added to history stack, also does redo
	* \param name
	* \param graphics scene
	* \param control point(s) that have been added
	* \return void*/
	RemovePathVectorCommand(const QString& name, QGraphicsScene * scene,
					ConnectionGraphicsItem::ControlPoint * item);
	/*! \brief constructor that makes the command. If added to history stack, also does redo
	* \param name
	* \param graphics scene
	* \param control point(s) that have been added
	* \return void*/
	RemovePathVectorCommand(const QString& name, QGraphicsScene * scene, ConnectionGraphicsItem* connection,
		             QList<ConnectionGraphicsItem::ControlPoint *> items);

	/*! \brief Remove new control points. Control points were set in the constructor
	* \param void
	* \return void*/
	void redo();
	/*! \brief Add new control points. Control points were set in the constructor
	* \param void
	* \return void*/
	void undo();
	/*! \brief vector of control points that were added*/
	QList<ConnectionGraphicsItem::PathVector> pathVectors;
	/*! \brief graphics scene from which control points were removed*/
	QGraphicsScene * graphicsScene;
	/*! \brief graphics item from which control points were removed*/
	ConnectionGraphicsItem* connectionItem;
	/*! \brief the poisition(s) at which the control point vectors were added*/
	QList<int> listK1;
};

}
#endif
