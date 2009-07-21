/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 A special ConnectionGraphicsItem that draws a DNA-like connection

****************************************************************************/

#ifndef TINKERCELL_DNACONNECTIONIMAGE_H
#define TINKERCELL_DNACONNECTIONIMAGE_H

#include <math.h>
#include <QtDebug>
#include <QString>
#include <QColor>
#include <QPoint>
#include <QPainter>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QList>
#include "Core/ConnectionGraphicsItem.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

/*! \brief A simple figure made from one or more polygons
 * the class can be represented in an XML file */

namespace Tinkercell
{
 
class MY_EXPORT DnaGraphicsItem : public ConnectionGraphicsItem
{
public:
	/*! Constructor: does nothing */
    DnaGraphicsItem(QGraphicsItem * parent = 0 );
	/*! Copy Constructor */
    DnaGraphicsItem(const DnaGraphicsItem& copy);
	/*! Copy operator */
	DnaGraphicsItem& operator = (const DnaGraphicsItem& copy);
	/*! \brief make a copy of this item */
	virtual ConnectionGraphicsItem* clone();
	/*! \brief paint method. Call's parent's paint after setting antialiasing to true*/
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option=new QStyleOptionGraphicsItem() ,QWidget *widget=0);
	/*! \brief refresh the path if any controlpoints have moved
	* \param void
	* \return void*/
	virtual void refresh();
	/*! \brief pen used to draw lines representing nucleotides */
	//QPen nucleotidesPen;
	/*! \brief path used to draw lines representing nucleotides */
	QGraphicsPathItem * nucleotidesPath;
	/*! \brief the width of a single helix turn*/
	qreal width;
	/*! \brief the height of the helix*/
	qreal height;
	/*! \brief used for checking type before static casts */
	static QString class_name;
};

}
#endif
