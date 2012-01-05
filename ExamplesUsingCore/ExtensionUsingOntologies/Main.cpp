/****************************************************************************
This is an example program that uses Ontologies to 
change the nodes, arrows, and connections used in 
TinkerCell

This extension programs uses several extensions in the 
BasicTools folder
****************************************************************************/

#include "Ontology.h"
#include "GlobalSettings.h"
#include "PythonTool.h"
#include "OctaveTool.h"
#include "RubyTool.h"
#include "ContainerTool.h"
#include "ModuleTool.h"
#include "ParametersTool.h"
#include "CollisionDetection.h"
#include "ConnectionInsertion.h"
#include "ConnectionMaker.h"
#include "NodeInsertion.h"
#include "GraphicsReplaceTool.h"
#include "GraphicsTransformTool.h"
#include "GroupHandlerTool.h"
#include "NameFamilyDialog.h"
#include "ConnectionSelection.h"
#include "NodeSelection.h"
#include "GraphicsScene.h"
#include "ConsoleWindow.h"
#include <QColor>
#include <QBrush>

using namespace Tinkercell;

#if defined(Q_WS_WIN) && !defined(MINGW)
int WinMain(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	//these two lines are required by all QT apps
    QApplication::setColorSpec (QApplication::ManyColor);
    QApplication app(argc, argv);
    
    /*****************************************	
           Optional configuration
    (must be done before creating MainWindow)
    ******************************************/

	GlobalSettings::ENABLE_HISTORY_WINDOW = true;
	GlobalSettings::ENABLE_CONSOLE_WINDOW = true;
	GlobalSettings::ENABLE_GRAPHING_TOOLS = true;
	GlobalSettings::ENABLE_CODING_TOOLS = true;
	GlobalSettings::ENABLE_PYTHON = true;
	GlobalSettings::ENABLE_RUBY = true;
	GlobalSettings::ENABLE_OCTAVE = true;
	GlobalSettings::ENABLE_LOADSAVE_TOOL = true;
    
    GlobalSettings::PROJECTWEBSITE = "http://www.tinkercell.com";
    GlobalSettings::ORGANIZATIONNAME = "MyOrganization";
    GlobalSettings::PROJECTNAME = "MyProgram";
    //GlobalSettings::PLUGINS_SVN_URL = "https://tinkercellextra.svn.sourceforge.net/svnroot/tinkercellextra"; //for updating
   	GlobalSettings::SETTINGS_FILE_NAME = "settings.ini"; //for distributing TinkerCell in USB drive with the settings
	
    //ConsoleWindow::Prompt = ">";
	//ConsoleWindow::BackgroundColor = QColor("#555555");
	
	//QColor color("#00EE00";
	//color.setAlpha(50);
	//GraphicsScene::SelectionRectangleBrush = QBrush(color);

    QString appDir = QCoreApplication::applicationDirPath();	
    QFile styleFile(appDir + QString("/tinkercell.qss"));

    if (styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        app.setStyleSheet(styleFile.readAll());
        styleFile.close();
    }
    
    /*******  Main Window ***********/
    
	MainWindow mainWindow;
	mainWindow.hide();
  
  /*******  title , etc ***********/
    mainWindow.setWindowTitle("TinkerCell");
    mainWindow.statusBar()->showMessage("Welcome to TinkerCell");
    
	/**** setup the tabs ****/
	CatalogWidget::tabGroups	
					<< QPair<QString, QStringList>(
													"Reptiles",
													QStringList() << "bird" << "lizard" )
					<< QPair<QString, QStringList>(
													"Mammals",
													QStringList() << "ungulates" )
					<< QPair<QString, QStringList>(
													"Arthropods",
													QStringList() << "insect" )
                   << QPair<QString, QStringList>(
													"Birth",
													QStringList())
					<< QPair<QString, QStringList>(
													"Death",
													QStringList())
					<< QPair<QString, QStringList>(
													"Containers",
													QStringList() << "forest" );

	Ontology::GLOBAL_CHILDREN += "empty set";  //the empty set belongs to all classes
	NodesTree::themeDirectory = "theme1"; //this is the name of a folder inside the Graphics folder
 
	/****  load extensions  ******/
	QString nodesFile = appDir + QString("/nodes.nt");
	QString connectionsFile = appDir + QString("/connections.nt");

   	mainWindow.addTool(new CatalogWidget(nodesFile, connectionsFile));
	mainWindow.addTool(new CollisionDetection);
	mainWindow.addTool(new ConnectionInsertion);
	mainWindow.addTool(new NodeInsertion);
	mainWindow.addTool(new NodeSelection);
	mainWindow.addTool(new ConnectionSelection);
	mainWindow.addTool(new GraphicsReplaceTool);
	mainWindow.addTool(new GraphicsTransformTool);
	mainWindow.addTool(new GroupHandlerTool);
	mainWindow.addTool(new ConnectionMaker);
	mainWindow.addTool(new ContainerTool);	
	mainWindow.addTool(new ModuleTool);
	mainWindow.addTool(new ParametersTool("numeric"));

	/********* load python, octave, ruby plugins ***********/
	if (mainWindow.tool("Python Interpreter"))
	{
		PythonTool * tool = (PythonTool*)mainWindow.tool("Python Interpreter");
		tool->loadFilesInDir("python");
	}

	if (mainWindow.tool("Octave Interpreter"))
	{
		OctaveTool * tool = (OctaveTool*)mainWindow.tool("Octave Interpreter");
		tool->loadFilesInDir("octave");
	}

	if (mainWindow.tool("Ruby Interpreter"))
	{
		RubyTool * tool = (RubyTool*)mainWindow.tool("Ruby Interpreter");
		tool->loadFilesInDir("ruby");
	}

    /*******  Splash screen ***********/

    QPixmap pixmap(appDir + QString("/splash.png"));
	QSplashScreen splash(pixmap,Qt::SplashScreen | Qt::WindowStaysOnTopHint);
	QSize sz = mainWindow.size();
    QPoint pos = mainWindow.pos();
    splash.move(pos + QPoint(sz.width()-pixmap.width(), sz.height()-pixmap.height())/2 );
    splash.show();

    /*******  START TINKERCELL ***********/
    
    mainWindow.readSettings();
	GraphicsScene * scene = mainWindow.newScene();
	mainWindow.show();
    splash.finish(&mainWindow);

    /*******  process command line arguments, if any ***********/
	if (argc > 1)
		for (int i=1; i < argc; ++i)
		{
			mainWindow.open(QString(argv[i]));
		}

    /* main loop */
    int output = app.exec();	
	app.closeAllWindows();
    return output;
}

