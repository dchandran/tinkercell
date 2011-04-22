/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool displays a dialog with the name and family information of selected items.
An associated GraphicsTool is also defined.

****************************************************************************/

#ifndef TINKERCELL_NAMEFAMILYINFORMATIONTOOL_H
#define TINKERCELL_NAMEFAMILYINFORMATIONTOOL_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
#include <QTableWidget>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QDialog>

#include "NodeGraphicsItem.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "NodesTree.h"
#include "NetworkHandle.h"

namespace Tinkercell
{
	class TINKERCELLEXPORT NameFamilyDialog : public Tool
	{
		Q_OBJECT;

	public:
		NameFamilyDialog();
		bool setMainWindow(MainWindow * main);
	
	signals:
		void itemsAboutToBeInserted(GraphicsScene * scene, QList<QGraphicsItem*>& items, QList<ItemHandle*>& handles, QList<QUndoCommand*>&);
		void itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>& handles);
		void nameChanged();

	public slots:
		void select(int i=0);
		void deselect(int i=0);
		void showDialog(ItemHandle*);
		void closeDialog();
		void itemsInsertedSlot(NetworkHandle * , const QList<ItemHandle*>& handles);
		void dialogFinished();
	
	protected:
		QDialog * dialog;
		QPlainTextEdit * textEdit;
		ItemHandle* selectedItem;
		NodeGraphicsItem idcard;
		
		void makeDialog(QWidget*);
	};
}

#endif
