#ifndef TINKERCELL_TC_BASICGRAPHICSTOOLBOX_API_H
#define TINKERCELL_TC_BASICGRAPHICSTOOLBOX_API_H

#include "../TCstructs.h"

/*! 
 \brief get the red color of the item
 \ingroup Coloring
*/
int (*tc_getColorR)(OBJ item);
/*! 
 \brief get the green color of the item
 \ingroup Coloring
*/
int (*tc_getColorG)(OBJ item);
/*! 
 \brief get the blue color of the item
 \ingroup Coloring
*/
int (*tc_getColorB)(OBJ item);
/*! 
 \brief set the rgb color  of the item and indicate whether or not the color is permanenet
 \ingroup Coloring
*/
void (*tc_setColor)(OBJ item,int R,int G,int B, int permanent);
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
	tc_getColorR = getColorR0;
	tc_getColorG = getColorG0;
	tc_getColorB = getColorB0;
	tc_setColor = setColor0;
}

#endif
