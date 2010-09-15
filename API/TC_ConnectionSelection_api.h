#ifndef TINKERCELL_TC_CONNECTIONSELECTION_API_H
#define TINKERCELL_TC_CONNECTIONSELECTION_API_H

#include "TC_structs.h"
BEGIN_C_DECLS

/*! 
 \brief get x position of a control point
 \param int address of a connection, e.g. obtained using tc_find 
 \param int address of a node, e.g. obtained using tc_find
 \param int index of the control point related to the given connection and the given node
 \return double x position
 \ingroup Connections
*/
TCAPIEXPORT double tc_getControlPointX(long connection,long part,int whichPoint);
/*! 
 \brief get y position of a control point
 \param int address of a connection, e.g. obtained using tc_find 
 \param int address of a node, e.g. obtained using tc_find
 \param int index of the control point related to the given connection and the given node
 \return double y position
 \ingroup Connections
*/
TCAPIEXPORT double tc_getControlPointY(long connection,long part,int whichPoint);
/*! 
 \brief set x and y position of a control point
 \param long the connection
 \param long the node that is associated with the particular curve of interest
 \param int the index of the point on that curve of interest
 \param double x value
 \param double y value
 \ingroup Connections
*/
TCAPIEXPORT void tc_setControlPoint(long connection,long part,int whichPoint, double x,double y);
/*! 
 \brief set x and y position of the central control point
 \param int address of a connection, e.g. obtained using tc_find 
 \param double x position
 \param double y position
 \ingroup Connections
*/
TCAPIEXPORT void tc_setCenterPoint(long connection,double y,double x);
/*! 
 \brief get x position of the central control point
 \param int address of a connection, e.g. obtained using tc_find 
 \return double x position
 \ingroup Connections
*/
TCAPIEXPORT double tc_getCenterPointX(long connection);
/*! 
 \brief get y position of the central control point
 \param int address of a connection, e.g. obtained using tc_find 
 \return double y position
 \ingroup Connections
*/
TCAPIEXPORT double tc_getCenterPointY(long connection);
/*! 
 \brief switch between beziers and lines for drawing the connector, where 1 = line, 0 = bezier
 \param int address of a connection, e.g. obtained using tc_find 
 \param int 0 (Bezier) or 1 (straight lines)
 \ingroup Connections
*/
TCAPIEXPORT void tc_setStraight(long item,int straight);
/*! 
 \brief switch between beziers and lines for drawing ALL connectors
 \param int 0 (Bezier) or 1 (straight lines)
 \ingroup Connections
*/
TCAPIEXPORT void tc_setAllStraight(int straight);
/*! 
 \brief set the line width. Indicate whether the change should be temporary or permanent.
 \param int address of a connection, e.g. obtained using tc_find 
 \param double line width
 \param int 0 (temporary change) or 1 (permanent change)
 \ingroup Connections
*/
TCAPIEXPORT void tc_setLineWidth(long item,double width, int permanent);

/*! 
 \brief initialize control points plug-in
 \ingroup init
*/
TCAPIEXPORT void tc_ConnectionSelection_api(
		double (*getControlPointX)(long,long,int),
		double (*getControlPointY)(long,long,int),
		void (*setControlPoint)(long,long,int,double,double),
		void (*setCenterPoint)(long,double,double),
		double (*getCenterPointX)(long),
		double (*getCenterPointY)(long),
		void (*setStraight)(long,int),
		void (*setAllStraight)(int),
		void (*setLineWidth)(long,double,int)
	);

END_C_DECLS
#endif

