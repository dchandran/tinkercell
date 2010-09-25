/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This is the history stack class that is used to store the undo/redo commands.

****************************************************************************/

#include "HistoryWindow.h"

namespace Tinkercell
{

	void HistoryWindow::HistoryWindow(bool b) : QUndoView()
	{
		useRestorePoints = useRestorePoints;
		currentIndex = 0;
		restorePoints << 0;
	}

	void HistoryWindow::undo() 
	{ 
		QUndoStack * s = stack();
		if (s))
		{
			if (useRestorePoints && currentIndex > 0)
			{
				int d = restorePoints[currentIndex] - restorePoints[currentIndex-1];  //number of steps between restore points
				for (int i=0; i < d; ++i)
					s->undo();
				--currentIndex;
			}
			else
			{
				s->undo(); 
			}
		}
	}

	void HistoryWindow::redo() 
	{ 
		QUndoStack * s = stack();
		if (s))
		{
			if (useRestorePoints && currentIndex < (restorePoints.size()-1))
			{
				int d = restorePoints[currentIndex+1] - restorePoints[currentIndex];  //number of steps between restore points
				for (int i=0; i < d; ++i)
					s->redo();
				++currentIndex;
			}
			else
			{
				s->redo(); 
			}
		}
	}

	void HistoryWindow::push(QUndoCommand * command) 
	{ 
		stack()->push(command);
	}
	
	void setRestorePoint()
	{
		HistoryWindow::
	}

}
