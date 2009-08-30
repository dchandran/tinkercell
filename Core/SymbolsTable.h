/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file defines the class that stores all symbols, such as node and 
connection names and data columns and rows, for each scene 

****************************************************************************/


#ifndef TINKERCELL_SYMBOLSTABLE_H
#define TINKERCELL_SYMBOLSTABLE_H

#include <QtGui>
#include <QString>
#include <QHash>
#include <QPair>

#include "ItemHandle.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{
	class Tool;
	class MainWindow;
	class NetworkWindow;
	class ItemHandle;
	class GraphicsScene;
	class TextEditor;

	/*! \brief 
	The symbols table is updated every time the scene or text editor changes. The symbols table contains the
	list of item names and ItemHandle pointers as well as names and pointers to each data entry in each item. Only
	ItemHandles with visible=true are loaded into the SymbolsTable.
	\ingroup core
	*/
	MY_EXPORT class SymbolsTable
	{
	public:
		/*! \brief constructor
		\param NetworkWindow* network that this symbol table belongs in
		*/
		SymbolsTable(NetworkWindow *);
		/*! \brief update the symbols table*/
		virtual void update();
		/*! \brief handle names and the corresponsing handles. This hash stores the unique full names, such a M.A
		*/
		QHash<QString,ItemHandle*> handlesFullName;
		/*! \brief handle names and the corresponsing handles. This hash stores the
		the non-unique names, such as A. Therefore the hash may contain multiple values for the same
		key (see QHash documentation)
		*/
		QHash<QString,ItemHandle*> handlesFirstName;
		/*! \brief row or column name and the corresponding handle and tool in which the row or column name belongs. Stores
		full names as well as just the row or column name. For example, if A.k0 is a data item, then this table will contain
		k0 as well as A.k0 with the same contents. The individual, non-unique, names such as k0 may have multiple hash values
		for the same hash key (see QHash documentation).
		*/
		QHash<QString, QPair<ItemHandle*,QString> > dataRowsAndCols;
		/*! \brief this hash contains all the list of items belonging in each family. The items are listed under their family only and
		not under their parent families. For example, you will not find an item of family "Elephant" under the "Mammals" key. You will
		have to specifically search under "Elephant" and use the family inheritance structure to find out that it is also a "Mammal"
		*/
		QHash<QString, ItemHandle* > handlesFamily;
		/*! \brief This is a special item handle that does not represent any item on the scene. It is used to store "global" data.
		*/
		ItemHandle modelItem;
		/*! \brief destructor*/
		virtual ~SymbolsTable();
	protected:
		/*! \brief the graphics scene that this symbols table belongs with*/
		NetworkWindow * networkWindow;
		/*! \brief update the symbols table using a graphics scene*/
		virtual void update(GraphicsScene *);
		/*! \brief update the symbols table using a text editor*/
		virtual void update(TextEditor *);
		/*! \brief update the symbols table*/
		virtual void update(const QList<ItemHandle*>&);
	};
}

#endif
