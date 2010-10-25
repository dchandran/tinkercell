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
#include "NodeGraphicsItem.h"

#ifdef Q_WS_WIN
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{
	class ItemHandle;

	/*! \brief gets the point on the edge of the rect such that it is in the same
	line as the center of the rect and the point (arg)
	\param QRectF rectangle
	\param QPointF point outside rectangle
	\return QPointF the point on the edge of the rectangle
	\ingroup helper
	*/
	TINKERCELLEXPORT QPointF pointOnEdge(const QRectF&, const QPointF&, qreal dist = 0, bool straight = false);
	/*! \brief gets the point on the edge of the shape such that it is in the same
	line as the center of the shape's bounding rect and the point (arg)
	\param QPainterPath the shape
	\param QPointF point outside shape
	\return QPointF the point on the edge of the shape
	\ingroup helper
	*/
	TINKERCELLEXPORT QPointF pointOnEdge(const NodeGraphicsItem&, const QPointF&, qreal dist = 0, bool straight = false);

	class GraphicsScene;
	class ItemHandle;
	class ConnectionGraphicsItem;
	
	/*! \brief A node graphics item that is used to draw arrow heads on connection items.
	\ingroup core*/
	class TINKERCELLEXPORT ArrowHeadItem : public NodeGraphicsItem
	{
	public:
		/*! \brief The connection item that this arrow head belongs with*/
		ConnectionGraphicsItem * connectionItem;
		/*! \brief the direction (angle) that the arrow is pointing*/
		qreal angle;
		/*! \brief constructor -- initializes the angle and connection item*/
		ArrowHeadItem(ConnectionGraphicsItem * c = 0);
		/*! \brief construct from file */
		ArrowHeadItem(const QString&, ConnectionGraphicsItem * c = 0);
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
		/*! \brief cast a graphics item to a node graphics item using qgraphicsitem_cast
		\param QGraphicsItem* graphics item
		\return ArrowHeadItem* can be 0 if the cast is invalid
		*/
		static ArrowHeadItem* cast(QGraphicsItem *);
		/*! \brief for enabling dynamic_cast*/
		enum { Type = UserType + 6 };
		/*! \brief for enabling dynamic_cast*/
		virtual int type() const
		{
			// Enable the use of dynamic_cast with this item.
			return Type;
		}
		static const QString CLASSNAME;
	};

	/*! \brief A graphics nodes item that draws connection between two or more nodes and the arrow heads at the ends.
	\ingroup core*/
	class TINKERCELLEXPORT ConnectionGraphicsItem : public QGraphicsPathItem
	{
	public:
		/*! \brief cast a graphics item to a connection graphics item using qgraphicsitem_cast
		\param QGraphicsItem* graphics item
		\return ConnectionGraphicsItem* can be 0 if the cast is invalid
		*/
		static ConnectionGraphicsItem* cast(QGraphicsItem*);
		/*! \brief cast a list of graphics item to a list of connection graphics items using qgraphicsitem_cast
		\param QList<QGraphicsItem*> graphics items
		\return QList<ConnectionGraphicsItem*> can be empty if no cast is invalid
		*/
		static QList<ConnectionGraphicsItem*> cast(const QList<QGraphicsItem*>&);
		/*! \brief used for checking type before static casts */
		static const QString CLASSNAME;
		/*! \brief used to initialize the middle item for a connection */
		static QString DefaultMiddleItemFile;
		/*! \brief used to initialize the arrow heads for a connection */
		static QString DefaultArrowHeadFile;
		/*! \brief just a name used identifying the connection */
		QString name;
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
		/*! Constructor: does nothing */
		ConnectionGraphicsItem(QGraphicsItem * parent = 0 );
		/*! Constructor: constructs linear curve segments with arrow heads on the second set of nodes
		* \param QList<NodeGraphicsItem*> list of nodes to connect from (no arrow heads)
		* \param QList<NodeGraphicsItem*> list of nodes to connect to (have arrow heads)
		*/
		ConnectionGraphicsItem(const QList<NodeGraphicsItem*>&, const QList<NodeGraphicsItem*>&, QGraphicsItem * parent = 0);
		/*! Copy Constructor: copies handle but not control points */
		ConnectionGraphicsItem(const ConnectionGraphicsItem& copy);
		/*! operator =: remove everything from original connection and copy everything from the given connection */
		virtual ConnectionGraphicsItem& operator = (const ConnectionGraphicsItem& copy);
		/*! operator =: copy just the control point positions and pen */
		virtual ConnectionGraphicsItem& copyPoints (const ConnectionGraphicsItem& copy);
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
		/*! \brief for identifying which scene this item belongs in*/
		int sceneNumber;
		/*! \brief line or beizier */
		typedef enum {line, bezier} LineType;

		/*! \brief type of line for this reaction - line or beizier */
		LineType lineType;

		/*! \brief A control point with a pointer to a ConnectionGraphicsItem 
		\ingroup core*/
		class TINKERCELLEXPORT ControlPoint : public Tinkercell::ControlPoint
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
			virtual ControlPoint& operator = (const ControlPoint& copy);
			/*! \brief for enabling dynamic_cast*/
			enum { Type = UserType + 7 };
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
			/*! \brief same as connectionItem->handle() */
			virtual ItemHandle * handle() const;
			/*! \brief same as connectionItem->setHandle(...) */
			virtual void setHandle(ItemHandle *);
		};
		
		/*! \brief get the handle of this connection */
		virtual ItemHandle * handle() const;
		
		/*! \brief set the handle of this connection */
		virtual void setHandle(ItemHandle *);
		

		/*! \brief A set of control points and two arrow heads 
		\ingroup core*/
		class TINKERCELLEXPORT CurveSegment : public QVector<ConnectionGraphicsItem::ControlPoint*>
		{
		public:
			CurveSegment();
			CurveSegment(int);
			CurveSegment(int,ConnectionGraphicsItem::ControlPoint*);
			CurveSegment(const CurveSegment&);
			ArrowHeadItem *arrowStart, *arrowEnd;
		};

		/*! \brief vector of vector of control point*/
		QList<CurveSegment> curveSegments;	
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
		* \param bool tranform arrow heads
		* \return void*/
		virtual void refresh(bool arrows=true);
		/*! \brief set visibility of control points
		* \param visible = true, invisible = false
		* \return void*/
		virtual void setControlPointsVisible(bool visible = true);
		/*! \brief show control points. same as setControlPointsVisible(true)
		* \return void*/
		void showControlPoints();
		/*! \brief hide control points. same as setControlPointsVisible(false)
		* \return void*/
		void hideControlPoints();
		/*! \brief check is this connection represents a modifier, i.e. points to the centerRegion of another connection
		* \return boolean*/
		virtual bool isModifier() const;
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
		* \param index less than size of curveSegments
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
		* \param index less than size of curveSegments
		* \return node item or 0*/
		virtual ArrowHeadItem* arrowAt(int index) const;
		/*! \brief get the modifier arrow head at the particular index
		* \param index less than size of curveSegments
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
		/*! \brief Tinkercell object that this drawable belongs in */
		ItemHandle * itemHandle;
		/*! \brief path of the boundary region of the entire connection*/
		QGraphicsPathItem * boundaryPathItem;
		/*! \brief path of the selection region of the entire connection*/
		QPainterPath pathShape;
		/*! \brief the boundary rectangle for this path. It is recomputed during each refresh.*/
		QRectF pathBoundingRect;
		/*! \brief update the boundary path*/
		virtual void refreshBoundaryPath();
		/*! \brief adjust the end control points so that they point straight
		* \param bool adjust arrow transformations*/
		virtual void adjustEndPoints(bool arrows=true);
	};

}
#endif

