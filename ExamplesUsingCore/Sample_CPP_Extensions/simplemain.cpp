/****************************************************************************
This is a simple example of the main() application that uses the TinkerCell Core library

Assuming that the working directory is the trunk folder of the TinkerCell code
and that all the libraries are present in trunk/BUILD/bin

Use this command to build the executable:
g++ simplemain.cpp -ICore -LBUILD/bin -lTinkerCellCore -I. -I/usr/include/qt4/QtGui/ -I/usr/include/qt4/QtCore/ -I/usr/include/qt4/QtXml -I/usr/include/qt4/QtOpenGL/ -I/usr/include/qt4/ -o BUILD/bin/simplemain.out

Then to run:
cd BUILD/bin
export LD_LIBRARY_PATH=.
./simplemain

****************************************************************************/

#include "MainWindow.h"
#include "GlobalSettings.h"
#include "Ontology.h"

using namespace Tinkercell;

#if defined(Q_WS_WIN) && !defined(MINGW)
int WinMain(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	//setup project name -- REQUIRED
    GlobalSettings::PROJECTWEBSITE = "www.tinkercell.com";
    GlobalSettings::ORGANIZATIONNAME = "My Wonderful Co.";
    GlobalSettings::PROJECTNAME = "My Wonderful App";
	
	//start Qt -- REQUIRED
	QApplication app(argc, argv);
	QString appDir = QCoreApplication::applicationDirPath();    
	
	//enable of disable features -- OPTIONAL (see default values in GlobalSettings.h)
	GlobalSettings::ENABLE_HISTORY_WINDOW = false;
	GlobalSettings::ENABLE_CONSOLE_WINDOW = true;
	GlobalSettings::ENABLE_GRAPHING_TOOLS = true;
	GlobalSettings::ENABLE_CODING_TOOLS = false;
	GlobalSettings::ENABLE_PYTHON = true;
	GlobalSettings::ENABLE_OCTAVE = false;
	GlobalSettings::ENABLE_LOADSAVE_TOOL = true;
	
	//create main window -- REQUIRED
	MainWindow mainWindow(true,false,false);  //@args: enable scene, text, allow pop-out windows
	mainWindow.readSettings();   //load settings such as window positions

	//set window title -- OPTIONAL
	mainWindow.setWindowTitle("My Wonderful App"); 
	
	//ADD PLUGINS
	
	//load Ontology -- OPTIONAL
	Ontology::readNodes("Nodes.nt");
	Ontology::readConnections("Connections.nt");
	
	//create an empty canvas -- RECOMMENDED
    GraphicsScene * scene = mainWindow.newScene();
    mainWindow.show();
    int output = app.exec();

    return output;
}

