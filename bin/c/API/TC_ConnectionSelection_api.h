#ifndef TINKERCELL_TC_CONNECTIONSELECTION_API_H
#define TINKERCELL_TC_CONNECTIONSELECTION_API_H

#include "../TCstructs.h"

/*! 
 \brief get x position of a control point
 \ingroup Control points
*/
double (*tc_getControlPointX)(OBJ connection,OBJ part,int whichPoint) = 0;
/*! 
 \brief get y position of a control point
 \ingroup Control points
*/
double (*tc_getControlPointY)(OBJ connection,OBJ part,int whichPoint) = 0;
/*! 
 \brief set x and y position of a control point
 \ingroup Control points
*/
void (*tc_setControlPoint)(OBJ connection,OBJ part,int whichPoint, double x,double y) = 0;
/*! 
 \brief set x and y position of the central control point
 \ingroup Control points
*/
void (*tc_setCenterPoint)(OBJ connection,double y,double x) = 0;
/*! 
 \brief get x position of the central control point
 \ingroup Control points
*/
double (*tc_getCenterPointX)(OBJ connection) = 0;
/*! 
 \brief get y position of the central control point
 \ingroup Control points
*/
double (*tc_getCenterPointY)(OBJ connection) = 0;
/*! 
 \brief switch between beziers and lines for drawing the connector, where 1 = line, 0 = bezier
 \ingroup Control points
*/
void (*tc_setStraight)(OBJ item,int straight) = 0;
/*! 
 \brief switch between beziers and lines for drawing the connector, where 1 = line, 0 = bezier
 \ingroup Control points
*/
void (*tc_setAllStraight)(int straight) = 0;
/*! 
 \brief set the line width. Indicate whether the change should be temporary or permanent.
 \ingroup Control points
*/
void (*tc_setLineWidth)(OBJ item,double width, int permanent) = 0;
/*! 
 \brief initialize control point functions
 \ingroup init
*/
void tc_ConnectionSelection_api(
		double (*getControlPointX)(OBJ,OBJ,int),
		double (*getControlPointY)(OBJ,OBJ,int),
		void (*setControlPoint)(OBJ,OBJ,int,double,double),
		void (*setCenterPoint)(OBJ,double,double),
		double (*getCenterPointX)(OBJ),
		double (*getCenterPointY)(OBJ),
		void (*setStraight)(OBJ,int),
		void (*setAllStraight)(int),
		void (*setLineWidth)(OBJ,double,int)
	)
{
	tc_getControlPointX = getControlPointX;
	tc_getControlPointY = getControlPointY;
	tc_setControlPoint = setControlPoint;
	tc_setCenterPoint = setCenterPoint;
	tc_getCenterPointX = getCenterPointX;
	tc_getCenterPointY = getCenterPointY;
	tc_setStraight = setStraight;
	tc_setAllStraight = setAllStraight;
	tc_setLineWidth = setLineWidth;
}

#endif
