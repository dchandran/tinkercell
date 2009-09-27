/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This is the history stack class that is used to store the undo/redo commands.

****************************************************************************/

#include "HistoryWindow.h"

namespace Tinkercell
{

	void HistoryWindow::undo() { stack()->undo(); }
	void HistoryWindow::redo() { stack()->redo(); }
	void HistoryWindow::push(QUndoCommand * command) { stack()->push(command); }

}
