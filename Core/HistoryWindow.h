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
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{

	/*! \brief This is a small class extending QUndoView that manages a stack of undo commands.
	The undo stack works the same way as QUndoView, but an additional feature of "restore points"
	is introduced. Multiple undo/redo events can occur, but they may all be related to a single user event. 
	These are marked as part of a single restore point. The undo/redo functions move between restore points, 
	but the undo view itself can be used to undo/redo events between restore points.
	\ingroup helper
	*/
	class TINKERCELLEXPORT HistoryWindow : public QUndoView
	{
		Q_OBJECT
	public:
		/*! \brief constructor
		\param bool use restore points. False here would mean that this is a normal history view*/
		HistoryWindow(bool useRestorePoints=true);

	public slots:
		void undo();
		void redo();
		void push(QUndoCommand * command);
		void setRestorePoint();
	
	private:
		bool useRestorePoints;
		int currentIndex;
		QList<int> restorePoints;
		
	};

}

#endif
