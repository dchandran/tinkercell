#ifndef TINKERCELL_TC_BASICGRAPHICSTOOLBOX_API_H
#define TINKERCELL_TC_BASICGRAPHICSTOOLBOX_API_H

#include "../TCstructs.h"
/*! 
 \brief get the red color of the item
 \ingroup Appearance
*/
int tc_getColorR(void* item);
/*! 
 \brief get the green color of the item
 \ingroup Appearance
*/
int tc_getColorG(void* item);
/*! 
 \brief get the blue color of the item
 \ingroup Appearance
*/
int tc_getColorB(void* item);
/*! 
 \brief set the rgb color  of the item and indicate whether or not the color is permanenet
 \ingroup Appearance
*/
void tc_setColor(void* item,int R,int G,int B, int permanent);
/*! 
 \brief change the graphics file for drawing one of the nodes
 \ingroup Appearance
*/
void tc_changeNodeImage(void* item,const char* filename);
/*! 
 \brief change the graphics file for drawing the arrowheads for the given connection
 \ingroup Appearance
*/
void tc_changeArrowHead(void* connection,const char* filename);

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
);

#endif
