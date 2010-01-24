#ifndef TINKERCELL_TC_BASICGRAPHICSTOOLBOX_API_H
#define TINKERCELL_TC_BASICGRAPHICSTOOLBOX_API_H

#include "../TCstructs.h"

int (*_tc_getColorR)(OBJ item) = 0;
/*! 
 \brief get the red color of the item
 \ingroup Coloring
*/
int tc_getColorR(OBJ item)
{
	if (_tc_getColorR)
		return _tc_getColorR(item);
	return 0;
}

int (*_tc_getColorG)(OBJ item) = 0;
/*! 
 \brief get the green color of the item
 \ingroup Coloring
*/
int tc_getColorG(OBJ item)
{
	if (_tc_getColorG)
		return _tc_getColorG(item);
	return 0;
}

int (*_tc_getColorB)(OBJ item) = 0;
/*! 
 \brief get the blue color of the item
 \ingroup Coloring
*/
int tc_getColorB(OBJ item)
{
	if (_tc_getColorB)
		return _tc_getColorB(item);
	return 0;
}

void (*_tc_setColor)(OBJ item,int R,int G,int B, int permanent) = 0;
/*! 
 \brief set the rgb color  of the item and indicate whether or not the color is permanenet
 \ingroup Coloring
*/
void tc_setColor(OBJ item,int R,int G,int B, int permanent)
{
	if (_tc_setColor)
		_tc_setColor(item,R,G,B,permanent);
}

/*! 
 \brief initializing function
 \ingroup init
*/
void tc_BasicGraphicsToolbox_api(
		int (*getColorR0)(OBJ),
		int (*getColorG0)(OBJ),
		int (*getColorB0)(OBJ),
		void (*setColor0)(OBJ,int,int,int, int)
)
{
	_tc_getColorR = getColorR0;
	_tc_getColorG = getColorG0;
	_tc_getColorB = getColorB0;
	_tc_setColor = setColor0;
}

#endif
