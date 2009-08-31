/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This is the history stack class that is used to store the undo/redo commands.

****************************************************************************/

#ifndef TINKERCELL_HISTORYSTACK_H
#define TINKERCELL_HISTORYSTACK_H

#include <QtGui>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QString>
#include <QFileDialog>
#include <QtDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QAction>
#include <QMenu>
#include <QMdiArea>
#include <QThread>
#include <QFile>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QHash>
#include <QUndoCommand>
#include <QUndoStack>
#include <QUndoView>

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	/*! \brief This is a small class extending QUndoView that manages a stack of undo commands.
	\ingroup helper
	*/
	class MY_EXPORT HistoryStack : public QUndoView
	{
		Q_OBJECT
	public:

		void undo();
		void redo();
		
	public slots:
		void push(QUndoCommand * command);

	};

}

#endif
