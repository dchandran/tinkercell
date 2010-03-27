#ifndef TINKERCELL_TC_CONNECTIONSELECTION_API_H
#define TINKERCELL_TC_CONNECTIONSELECTION_API_H

#include "../TCstructs.h"

/*! 
 \brief get x position of a control point
 \ingroup Control points
*/
double tc_getControlPointX(void* connection,void* part,int whichPoint);
/*! 
 \brief get y position of a control point
 \ingroup Control points
*/
double tc_getControlPointY(void* connection,void* part,int whichPoint);
/*! 
 \brief set x and y position of a control point
 \param void* the connection
 \param void* the node that is associated with the particular curve of interest
 \param int the index of the point on that curve of interest
 \param double x value
 \param double y value
 \ingroup Control points
*/
void tc_setControlPoint(void* connection,void* part,int whichPoint, double x,double y);
/*! 
 \brief set x and y position of the central control point
 \ingroup Control points
*/
void tc_setCenterPoint(void* connection,double y,double x);
/*! 
 \brief get x position of the central control point
 \ingroup Control points
*/
double tc_getCenterPointX(void* connection);
/*! 
 \brief get y position of the central control point
 \ingroup Control points
*/
double tc_getCenterPointY(void* connection);
/*! 
 \brief switch between beziers and lines for drawing the connector, where 1 = line, 0 = bezier
 \ingroup Control points
*/
void tc_setStraight(void* item,int straight);
/*! 
 \brief switch between beziers and lines for drawing the connector, where 1 = line, 0 = bezier
 \ingroup Control points
*/
void tc_setAllStraight(int straight);
/*! 
 \brief set the line width. Indicate whether the change should be temporary or permanent.
 \ingroup Control points
*/
void tc_setLineWidth(void* item,double width, int permanent);

/*! 
 \brief initialize control point functions
 \ingroup init
*/
void tc_ConnectionSelection_api(
		double (*getControlPointX)(void*,void*,int),
		double (*getControlPointY)(void*,void*,int),
		void (*setControlPoint)(void*,void*,int,double,double),
		void (*setCenterPoint)(void*,double,double),
		double (*getCenterPointX)(void*),
		double (*getCenterPointY)(void*),
		void (*setStraight)(void*,int),
		void (*setAllStraight)(int),
		void (*setLineWidth)(void*,double,int)
	);

#endif
