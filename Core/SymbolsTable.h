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
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{
	class Tool;
	class MainWindow;
	class NetworkHandle;
	class ItemHandle;
	class GraphicsScene;
	class TextEditor;

	/*! \brief 
	The symbols table is updated every time the scene or text editor changes. The symbols table contains the
	list of item names and ItemHandle pointers as well as names and pointers to each data entry in each item.
	\ingroup core
	*/
	class TINKERCELLEXPORT SymbolsTable
	{
	public:
		/*! \brief constructor
		\param NetworkWindow* network that this symbol table belongs in
		*/
		SymbolsTable(NetworkHandle *);
		/*! \brief update the symbols table*/
		virtual void update(int n=0);
		/*! \brief handle names and the corresponsing handles. This hash stores the unique full names, such a M.A and M_A
		*/
		QHash<QString,ItemHandle*> uniqueHandlesWithDot, uniqueHandlesWithUnderscore;
		/*! \brief handle names and the corresponsing handles. This hash stores the
		the non-unique names, such as A. Therefore the hash may contain multiple values for the same
		key (see QHash documentation)
		*/
		QHash<QString,ItemHandle*> nonuniqueHandles;
		/*! \brief row or column name and the corresponding handle and tool in which the row or column name belongs. Stores
		full names only. For example, if A.k0 is a data item, then this table will contain
		A.k0 and A_k0. All entries are unique.
		*/
		QHash<QString, QPair<ItemHandle*,QString> > uniqueDataWithDot, uniqueDataWithUnderscore;
		/*! \brief row or column name and the corresponding handle and tool in which the row or column name belongs. Stores
		just the row or column name. For example, if A.k0 is a data item, then this table will contain
		k0. The individual, non-unique, names such as k0 may have multiple hash values
		for the same hash key (see QHash documentation).
		*/
		QHash<QString, QPair<ItemHandle*,QString> > nonuniqueData;
		/*! \brief this hash contains all the list of items belonging in each family. The items are listed under their family only and
		not under their parent families. For example, you will not find an item of family "Elephant" under the "Mammals" key. You will
		have to specifically search under "Elephant" and use ItemFamily's isA method to find out that it is also a "Mammal"
		*/
		QHash<QString, ItemHandle* > handlesByFamily;

		/*! \brief checks whether the given item handle pointer is valid*/
		virtual bool isValidPointer(void*) const;
		
		/*! \brief get list of all items sorted according to family*/
		virtual QList<ItemHandle*> allHandlesSortedByFamily() const;
		
		/*! \brief get list of all items sorted according to their full name*/
		virtual QList<ItemHandle*> allHandlesSortedByName() const;
		
	protected:
		/*! \brief the network that this symbols table belongs with*/
		NetworkHandle * network;
		/*! \brief This is a special item handle that does not represent any item on the scene. It is used to store "global" data.*/
		ItemHandle globalHandle;
		/*! \brief update the symbols table*/
		virtual void update(const QList<ItemHandle*>&);

		/*! \brief addresses of all handles*/
		QHash<void*,QString> handlesAddress;
		
		friend class NetworkHandle;
	};
}

#endif
