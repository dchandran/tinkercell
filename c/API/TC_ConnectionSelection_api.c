#include "TC_ConnectionSelection_api.h"

double (*_tc_getControlPointX)(Item connection,Item part,int whichPoint) = 0;
/*! 
 \brief get x position of a control point
 \ingroup Control points
*/
double tc_getControlPointX(Item connection,Item part,int whichPoint)
{
	if (_tc_getControlPointX)
		return _tc_getControlPointX(connection,part,whichPoint);
	return 0.0;
}

double (*_tc_getControlPointY)(Item connection,Item part,int whichPoint) = 0;
/*! 
 \brief get y position of a control point
 \ingroup Control points
*/
double tc_getControlPointY(Item connection,Item part,int whichPoint)
{
	if (_tc_getControlPointY)
		return _tc_getControlPointY(connection,part,whichPoint);
	return 0.0;
}

void (*_tc_setControlPoint)(Item connection,Item part,int whichPoint, double x,double y) = 0;
/*! 
 \brief set x and y position of a control point
 \param Item the connection
 \param Item the node that is associated with the particular curve of interest
 \param int the index of the point on that curve of interest
 \param double x value
 \param double y value
 \ingroup Control points
*/
void tc_setControlPoint(Item connection,Item part,int whichPoint, double x,double y)
{
	if (_tc_setControlPoint)
		_tc_setControlPoint(connection,part,whichPoint,x,y);
}

void (*_tc_setCenterPoint)(Item connection,double y,double x) = 0;
/*! 
 \brief set x and y position of the central control point
 \ingroup Control points
*/
void tc_setCenterPoint(Item connection,double y,double x)
{
	if (_tc_setCenterPoint)
		_tc_setCenterPoint(connection, x, y);
}

double (*_tc_getCenterPointX)(Item connection) = 0;
/*! 
 \brief get x position of the central control point
 \ingroup Control points
*/
double tc_getCenterPointX(Item connection)
{
	if (_tc_getCenterPointX)
		return _tc_getCenterPointX(connection);
	return 0.0;
}

double (*_tc_getCenterPointY)(Item connection) = 0;
/*! 
 \brief get y position of the central control point
 \ingroup Control points
*/
double tc_getCenterPointY(Item connection)
{
	if (_tc_getCenterPointY)
		return _tc_getCenterPointY(connection);
	return 0.0;
}

void (*_tc_setStraight)(Item item,int straight) = 0;
/*! 
 \brief switch between beziers and lines for drawing the connector, where 1 = line, 0 = bezier
 \ingroup Control points
*/
void tc_setStraight(Item item,int straight)
{
	if (_tc_setStraight)
		_tc_setStraight(item,straight);
}

void (*_tc_setAllStraight)(int straight) = 0;
/*! 
 \brief switch between beziers and lines for drawing the connector, where 1 = line, 0 = bezier
 \ingroup Control points
*/
void tc_setAllStraight(int straight)
{
	if (_tc_setAllStraight)
		_tc_setAllStraight(straight);
}

void (*_tc_setLineWidth)(Item item,double width, int permanent) = 0;
/*! 
 \brief set the line width. Indicate whether the change should be temporary or permanent.
 \ingroup Control points
*/
void tc_setLineWidth(Item item,double width, int permanent)
{
	if (_tc_setLineWidth)
		_tc_setLineWidth(item,width,permanent);
}

/*! 
 \brief initialize control point functions
 \ingroup init
*/
void tc_ConnectionSelection_api(
		double (*getControlPointX)(Item,Item,int),
		double (*getControlPointY)(Item,Item,int),
		void (*setControlPoint)(Item,Item,int,double,double),
		void (*setCenterPoint)(Item,double,double),
		double (*getCenterPointX)(Item),
		double (*getCenterPointY)(Item),
		void (*setStraight)(Item,int),
		void (*setAllStraight)(int),
		void (*setLineWidth)(Item,double,int)
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


