#include "PythonTool.h"
#include "SpecialCommandsConsole.h"

namespace Tinkercell
{
	SpecialCommandsConsole::SpecialCommandsConsole()
		: ConsoleWindow()
	{
		name = tr("Special commands console");
	}

	QString SpecialCommandsConsole::getCode(const QString& s)
	{
QString code = "s = '" + s + "'\n";
code += "\
s = s.replace(' ','')\n\
ar = s.split('--')\n\
if len(ar) < 2: ar = s.split('-')\n\
parts = []\n\
for x in ar:\n\
    p = re.match('(\\\\S+)\\((\\\\S+)\\)',x)\n\
    if p:\n\
        y = tc_insert(p.group(2), p.group(1))\n\
        parts.append(y)\n\
if len(parts) > 0:\n\
    tc_alignParts(toTC(parts))\n\
    tc_deselect()\n";
	return code;
	}

	bool SpecialCommandsConsole::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		connect(this, SIGNAL(commandExecuted(const QString&)), this, SLOT(commandExecutedSlot(const QString&)));
		setWindowTitle("Text input window");
		mainWindow->addToolWindow(this,MainWindow::defaultConsoleWindowOption,Qt::BottomDockWidgetArea);

		if (mainWindow && mainWindow->tool("Python interpreter"))
		{
			PythonTool * pytool = (PythonTool*)mainWindow->tool("Python interpreter");
			QString code = "import re\n";
			pytool->runPythonCode( code );
		}

		message("try the following command as an example: promoter(p1)--rbs(r1)--cds(g1)--terminator(t1)");
	}
		
	void SpecialCommandsConsole::commandExecutedSlot(const QString& s)
	{
		if (mainWindow && mainWindow->tool("Python interpreter"))
		{
			PythonTool * pytool = (PythonTool*)mainWindow->tool("Python interpreter");
			pytool->runPythonCode( getCode(s) );
		}
	}

}
