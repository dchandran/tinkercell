/****************************************************************************

 Copyright (C) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYWRITE.TXT

 This is the main application file for Tinkercell. It constructs a MainWindow
 and loads a list of default plugins.

****************************************************************************/

#include "GlobalSettings.h"
#include "PythonTool.h"
#include "OctaveTool.h"
#include "RubyTool.h"
#include "BasicInformationTool.h"
#include "StoichiometryTool.h"
#include "FunctionDeclarationsTool.h"
#include "ModelSummaryTool.h"
#include "ContainerTool.h"
#include "ModelFileGenerator.h"
#include "SimulationEventTool.h"
#include "ModuleTool.h"
#include "WetLabTool.h"
#include "AutoGeneRegulatoryTool.h"
#include "DNASequenceTool.h"
#include "ViewTablesTool.h"
#include "LPSolveInput.h"
#include "CellPositionUpdatingTool.h"
#include "OctaveExporter.h"
#include "EnglishExporter.h"
#include "AntimonyEditor.h"
#include "SBMLImportExport.h"
#include "CopasiExporter.h"
#include "TinkerCellAboutBox.h"
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

void LoadPluginsFromDir(const QString&,MainWindow *,QSplashScreen*);

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
    
    GlobalSettings::PROJECTWEBSITE = QObject::tr("http://www.tinkercell.com");
    GlobalSettings::ORGANIZATIONNAME = QObject::tr("TinkerCell");
    GlobalSettings::PROJECTNAME = QObject::tr("TinkerCell");
    GlobalSettings::PLUGINS_SVN_URL = QObject::tr("https://tinkercellextra.svn.sourceforge.net/svnroot/tinkercellextra"); //for updating
	
    ConsoleWindow::Prompt = QObject::tr(">");
	ConsoleWindow::BackgroundColor = QColor("#555555");
	
	QColor color("#00EE00");
	color.setAlpha(50);
	GraphicsScene::SelectionRectangleBrush = QBrush(color);

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
    mainWindow.setWindowTitle(QObject::tr("TinkerCell"));
    mainWindow.statusBar()->showMessage(QObject::tr("Welcome to TinkerCell"));
    
	/****  load plugins  ******/
   	mainWindow.addTool(new CatalogWidget);
	mainWindow.addTool(new CollisionDetection);
	mainWindow.addTool(new ConnectionInsertion);
	mainWindow.addTool(new NodeInsertion);
	mainWindow.addTool(new NodeSelection);
	mainWindow.addTool(new ConnectionSelection);
	mainWindow.addTool(new TinkercellAboutBox);
	mainWindow.addTool(new GraphicsReplaceTool);
	mainWindow.addTool(new GraphicsTransformTool);
	mainWindow.addTool(new GroupHandlerTool);
	mainWindow.addTool(new NameFamilyDialog);
	mainWindow.addTool(new ConnectionMaker);	
	mainWindow.addTool(new BasicInformationTool(QString("text")));
	mainWindow.addTool(new BasicInformationTool(QString("numerical")));
	mainWindow.addTool(new AssignmentFunctionsTool);
	mainWindow.addTool(new StoichiometryTool);
	mainWindow.addTool(new ModelSummaryTool);
	mainWindow.addTool(new ModelFileGenerator);
	mainWindow.addTool(new SimulationEventsTool);	
	mainWindow.addTool(new CompartmentTool);	
	mainWindow.addTool(new OctaveExporter);
	mainWindow.addTool(new SBMLImportExport);
	mainWindow.addTool(new CopasiExporter);
	mainWindow.addTool(new AntimonyEditor);
	mainWindow.addTool(new EnglishExporter);	
	mainWindow.addTool(new ModuleTool);
	mainWindow.addTool(new WetLabTool);
	mainWindow.addTool(new AutoGeneRegulatoryTool);
	mainWindow.addTool(new DNASequenceViewer);
	mainWindow.addTool(new ViewTablesTool);
	mainWindow.addTool(new LPSolveInputWindow);
	mainWindow.addTool(new CellPositionUpdateTool);

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

    QString splashFile(":/images/Tinkercell.png");
	QPixmap pixmap(splashFile);
	QSplashScreen splash(pixmap,Qt::SplashScreen);//|Qt::WindowStaysOnTopHint);
	
    QSize sz = mainWindow.size();
    QPoint pos = mainWindow.pos();
    splash.move(pos + QPoint(sz.width()-pixmap.width(), sz.height()-pixmap.height())/2 );

    splash.setWindowOpacity(0.75);

    splash.show();
    splash.showMessage(QString("loading ..."));

    /*******  Dynamically load plugins from folders ***********/    
	mainWindow.setDockOptions(QMainWindow::AnimatedDocks|QMainWindow::AllowNestedDocks);

    QString home = GlobalSettings::homeDir();

    LoadPluginsFromDir(appDir + QString("/") + QString(TINKERCELL_CPP_PLUGINS_FOLDER),&mainWindow, &splash);
	LoadPluginsFromDir(home + QString("/") + QString(TINKERCELL_CPP_PLUGINS_FOLDER),&mainWindow, &splash);

	LoadPluginsFromDir(appDir + QString("/") + QString(TINKERCELL_C_PLUGINS_FOLDER),&mainWindow, &splash);
	LoadPluginsFromDir(home + QString("/") +  QString(TINKERCELL_C_PLUGINS_FOLDER),&mainWindow, &splash);
	

    /*******  START TINKERCELL ***********/

    mainWindow.readSettings();
	GraphicsScene * scene = mainWindow.newScene();
	mainWindow.show();
    splash.finish(&mainWindow);

    /*******  process command line arguments, if any ***********/
	if (argc > 1)
		for (int i=1; i < argc; ++i)
		{
			if (mainWindow.console())
				mainWindow.console()->eval(QString(argv[i]));
			else
				mainWindow.open(QString(argv[i]));
		}

    /* main loop */
    int output = app.exec();	
	app.closeAllWindows();
    return output;
}

void LoadPluginsFromDir(const QString& dirname,MainWindow * main,QSplashScreen * splash)
{
    QDir dir(dirname);
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Time);
    QFileInfoList list = dir.entryInfoList();

    for (int i = (list.size()-1); i >= 0; --i)
    {
        QFileInfo fileInfo = list.at(i);
        QString filename = fileInfo.absoluteFilePath();
        if (!QLibrary::isLibrary(filename)) continue;

        if (splash)
            splash->showMessage(QString("loading ") + fileInfo.fileName() + QString("..."));
        main->loadDynamicLibrary(filename);
    }
}

