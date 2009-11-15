#ifndef TINKERCELL_TC_CONNECTIONSELECTION_API_H
#define TINKERCELL_TC_CONNECTIONSELECTION_API_H

#include "../TCstructs.h"

double (*_tc_getControlPointX)(OBJ connection,OBJ part,int whichPoint) = 0;
/*! 
 \brief get x position of a control point
 \ingroup Control points
*/
double tc_getControlPointX(OBJ connection,OBJ part,int whichPoint)
{
	if (_tc_getControlPointX)
		return _tc_getControlPointX(connection,part,whichPoint);
	return 0.0;
}

double (*_tc_getControlPointY)(OBJ connection,OBJ part,int whichPoint) = 0;
/*! 
 \brief get y position of a control point
 \ingroup Control points
*/
double tc_getControlPointY(OBJ connection,OBJ part,int whichPoint)
{
	if (_tc_getControlPointY)
		return _tc_getControlPointY(connection,part,whichPoint);
	return 0.0;
}

void (*_tc_setControlPoint)(OBJ connection,OBJ part,int whichPoint, double x,double y) = 0;
/*! 
 \brief set x and y position of a control point
 \param OBJ the connection
 \param OBJ the node that is associated with the particular curve of interest
 \param int the index of the point on that curve of interest
 \param double x value
 \param double y value
 \ingroup Control points
*/
void tc_setControlPoint(OBJ connection,OBJ part,int whichPoint, double x,double y)
{
	if (_tc_setControlPoint)
		return _tc_setControlPoint(connection,part,whichPoint,x,y);
}

void (*_tc_setCenterPoint)(OBJ connection,double y,double x) = 0;
/*! 
 \brief set x and y position of the central control point
 \ingroup Control points
*/
void tc_setCenterPoint(OBJ connection,double y,double x)
{
	if (_tc_setCenterPoint)
		return _tc_setCenterPoint(connection, x, y);
}

double (*_tc_getCenterPointX)(OBJ connection) = 0;
/*! 
 \brief get x position of the central control point
 \ingroup Control points
*/
double tc_getCenterPointX(OBJ connection)
{
	if (_tc_getCenterPointX)
		return _tc_getCenterPointX(connection);
	return 0.0;
}

double (*_tc_getCenterPointY)(OBJ connection) = 0;
/*! 
 \brief get y position of the central control point
 \ingroup Control points
*/
double tc_getCenterPointY(OBJ connection)
{
	if (_tc_getCenterPointY)
		return _tc_getCenterPointY(connection);
	return 0.0;
}

void (*_tc_setStraight)(OBJ item,int straight) = 0;
/*! 
 \brief switch between beziers and lines for drawing the connector, where 1 = line, 0 = bezier
 \ingroup Control points
*/
void tc_setStraight(OBJ item,int straight)
{
	if (_tc_setStraight)
		return _tc_setStraight(item,straight);
}

void (*_tc_setAllStraight)(int straight) = 0;
/*! 
 \brief switch between beziers and lines for drawing the connector, where 1 = line, 0 = bezier
 \ingroup Control points
*/
void tc_setAllStraight(int straight)
{
	if (_tc_setAllStraight)
		return _tc_setAllStraight(straight);
}

void (*_tc_setLineWidth)(OBJ item,double width, int permanent) = 0;
/*! 
 \brief set the line width. Indicate whether the change should be temporary or permanent.
 \ingroup Control points
*/
void tc_setLineWidth(OBJ item,double width, int permanent)
{
	if (_tc_setLineWidth)
		return _tc_setLineWidth(item,width,permanent);
}

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
	_tc_getControlPointX = getControlPointX;
	_tc_getControlPointY = getControlPointY;
	_tc_setControlPoint = setControlPoint;
	_tc_setCenterPoint = setCenterPoint;
	_tc_getCenterPointX = getCenterPointX;
	_tc_getCenterPointY = getCenterPointY;
	_tc_setStraight = setStraight;
	_tc_setAllStraight = setAllStraight;
	_tc_setLineWidth = setLineWidth;
}

#endif
