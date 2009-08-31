/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This is the history stack class that is used to store the undo/redo commands.

****************************************************************************/

#include "HistoryStack.h"

namespace Tinkercell
{

	void HistoryStack::undo() { stack()->undo(); }
	void HistoryStack::redo() { stack()->redo(); }
	void HistoryStack::push(QUndoCommand * command) { stack()->push(command); }

}
