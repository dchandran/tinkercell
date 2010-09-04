#ifndef TINKERCELL_TC_CONNECTIONSELECTION_API_H
#define TINKERCELL_TC_CONNECTIONSELECTION_API_H

#include "../TCstructs.h"
BEGIN_C_DECLS

/*! 
 \brief get x position of a control point
 \ingroup Connections
*/
TCAPIEXPORT double tc_getControlPointX(int connection,int part,int whichPoint);
/*! 
 \brief get y position of a control point
 \ingroup Connections
*/
TCAPIEXPORT double tc_getControlPointY(int connection,int part,int whichPoint);
/*! 
 \brief set x and y position of a control point
 \param int the connection
 \param int the node that is associated with the particular curve of interest
 \param int the index of the point on that curve of interest
 \param double x value
 \param double y value
 \ingroup Connections
*/
TCAPIEXPORT void tc_setControlPoint(int connection,int part,int whichPoint, double x,double y);
/*! 
 \brief set x and y position of the central control point
 \ingroup Connections
*/
TCAPIEXPORT void tc_setCenterPoint(int connection,double y,double x);
/*! 
 \brief get x position of the central control point
 \ingroup Connections
*/
TCAPIEXPORT double tc_getCenterPointX(int connection);
/*! 
 \brief get y position of the central control point
 \ingroup Connections
*/
TCAPIEXPORT double tc_getCenterPointY(int connection);
/*! 
 \brief switch between beziers and lines for drawing the connector, where 1 = line, 0 = bezier
 \ingroup Connections
*/
TCAPIEXPORT void tc_setStraight(int item,int straight);
/*! 
 \brief switch between beziers and lines for drawing the connector, where 1 = line, 0 = bezier
 \ingroup Connections
*/
TCAPIEXPORT void tc_setAllStraight(int straight);
/*! 
 \brief set the line width. Indicate whether the change should be temporary or permanent.
 \ingroup Connections
*/
TCAPIEXPORT void tc_setLineWidth(int item,double width, int permanent);

/*! 
 \brief initialize control point functions
 \ingroup init
*/
TCAPIEXPORT void tc_ConnectionSelection_api(
		double (*getControlPointX)(int,int,int),
		double (*getControlPointY)(int,int,int),
		void (*setControlPoint)(int,int,int,double,double),
		void (*setCenterPoint)(int,double,double),
		double (*getCenterPointX)(int),
		double (*getCenterPointY)(int),
		void (*setStraight)(int,int),
		void (*setAllStraight)(int),
		void (*setLineWidth)(int,double,int)
	);

END_C_DECLS
#endif

