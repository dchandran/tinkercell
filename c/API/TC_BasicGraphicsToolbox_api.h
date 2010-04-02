#ifndef TINKERCELL_TC_BASICGRAPHICSTOOLBOX_API_H
#define TINKERCELL_TC_BASICGRAPHICSTOOLBOX_API_H

#include "../TCstructs.h"
/*! 
 \brief get the red color of the item
 \ingroup Appearance
*/
int tc_getColorR(Item item);
/*! 
 \brief get the green color of the item
 \ingroup Appearance
*/
int tc_getColorG(Item item);
/*! 
 \brief get the blue color of the item
 \ingroup Appearance
*/
int tc_getColorB(Item item);
/*! 
 \brief set the rgb color  of the item and indicate whether or not the color is permanenet
 \ingroup Appearance
*/
void tc_setColor(Item item,int R,int G,int B, int permanent);
/*! 
 \brief change the graphics file for drawing one of the nodes
 \ingroup Appearance
*/
void tc_changeNodeImage(Item item,String filename);
/*! 
 \brief change the graphics file for drawing the arrowheads for the given connection
 \ingroup Appearance
*/
void tc_changeArrowHead(Item connection,String filename);

/*! 
 \brief initializing function
 \ingroup init
*/
void tc_BasicGraphicsToolbox_api(
		int (*getColorR)(Item),
		int (*getColorG)(Item),
		int (*getColorB)(Item),
		void (*setColor)(Item,int,int,int, int),
		void (*changeNodeImage)(Item,String),
		void (*changeArrowHead)(Item,String)
);

#endif
