#ifndef TINKERCELL_TC_BASICGRAPHICSTOOLBOX_API_H
#define TINKERCELL_TC_BASICGRAPHICSTOOLBOX_API_H

#include "TC_BasicGraphicsToolbox_api.h"

int (*_tc_getColorR)(void* item) = 0;
/*! 
 \brief get the red color of the item
 \ingroup Appearance
*/
int tc_getColorR(void* item)
{
	if (_tc_getColorR)
		return _tc_getColorR(item);
	return 0;
}

int (*_tc_getColorG)(void* item) = 0;
/*! 
 \brief get the green color of the item
 \ingroup Appearance
*/
int tc_getColorG(void* item)
{
	if (_tc_getColorG)
		return _tc_getColorG(item);
	return 0;
}

int (*_tc_getColorB)(void* item) = 0;
/*! 
 \brief get the blue color of the item
 \ingroup Appearance
*/
int tc_getColorB(void* item)
{
	if (_tc_getColorB)
		return _tc_getColorB(item);
	return 0;
}

void (*_tc_setColor)(void* item,int R,int G,int B, int permanent) = 0;
/*! 
 \brief set the rgb color  of the item and indicate whether or not the color is permanenet
 \ingroup Appearance
*/
void tc_setColor(void* item,int R,int G,int B, int permanent)
{
	if (_tc_setColor)
		_tc_setColor(item,R,G,B,permanent);
}

void (*_tc_changeNodeImage)(void*,const char*) = 0;
/*! 
 \brief change the graphics file for drawing one of the nodes
 \ingroup Appearance
*/
void tc_changeNodeImage(void* item,const char* filename)
{
	if (_tc_changeNodeImage)
		_tc_changeNodeImage(item,filename);
}

void (*_tc_changeArrowHead)(void*,const char*) = 0;
/*! 
 \brief change the graphics file for drawing the arrowheads for the given connection
 \ingroup Appearance
*/
void tc_changeArrowHead(void* connection,const char* filename)
{
	if (_tc_changeArrowHead)
		_tc_changeArrowHead(connection,filename);
}

/*! 
 \brief initializing function
 \ingroup init
*/
void tc_BasicGraphicsToolbox_api(
		int (*getColorR)(void*),
		int (*getColorG)(void*),
		int (*getColorB)(void*),
		void (*setColor)(void*,int,int,int, int),
		void (*changeNodeImage)(void*,const char*),
		void (*changeArrowHead)(void*,const char*)
)
{
	_tc_getColorR = getColorR;
	_tc_getColorG = getColorG;
	_tc_getColorB = getColorB;
	_tc_setColor = setColor;
	_tc_changeNodeImage = changeNodeImage;
	_tc_changeArrowHead = changeArrowHead;
}

#endif
