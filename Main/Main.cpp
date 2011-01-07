/****************************************************************************

 Copyright (C) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYWRITE.TXT

 This is the main application file for Tinkercell. It constructs a MainWindow
 and loads a list of default plugins.

****************************************************************************/
#include "PlotTool.h"
#include "GnuplotTool.h"
#include "BasicGraphicsToolbar.h"
#include "DefaultPluginsMenu.h"
#include "GraphicsScene.h"
#include "ConsoleWindow.h"
#include "LoadSaveTool.h"
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
#include "TinkerCellAboutBox.h"
#include "LoadCLibraries.h"
#include "PythonTool.h"
#include "OctaveTool.h"
#include "CodingWindow.h"
#include "DynamicLibraryMenu.h"
#include "BasicInformationTool.h"
#include "StoichiometryTool.h"
#include "FunctionDeclarationsTool.h"
#include "ModelSummaryTool.h"
#include "ContainerTool.h"
#include "ModelFileGenerator.h"
#include "SimulationEventTool.h"
#include "ModuleTool.h"
#include "AutoGeneRegulatoryTool.h"
#include "CLabelsTool.h"
#include "DNASequenceTool.h"
#include "ViewTablesTool.h"
#include "LPSolveInput.h"
#include "CellPositionUpdatingTool.h"
#include "OctaveExporter.h"
#include "AntimonyEditor.h"
#include "SBMLImportExport.h"
#include "CopasiExporter.h"
#include "ModuleCombinatorics.h"
#include <QColor>
#include <QBrush>

using namespace Tinkercell;

void LoadPluginsFromDir(const QString&,MainWindow *,QSplashScreen*);

int main(int argc, char *argv[])
{
    QApplication::setColorSpec (QApplication::ManyColor);
    QApplication app(argc, argv);

    /***********************	
    	Optional configuration
    ***********************/
    
    MainWindow::PROJECTWEBSITE = QObject::tr("www.tinkercell.com");
    MainWindow::ORGANIZATIONNAME = QObject::tr("TinkerCell");
    MainWindow::PROJECTNAME = QObject::tr("TinkerCell");
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
    
    // "lite" modes
  #ifdef TINKERCELL_PARTS_ONLY
  	    MainWindow mainWindow(true, false, false, true, true);
	    MainWindow::PROGRAM_MODE = QString("parts-only");
  #else
  #ifdef TINKERCELL_TEXT_ONLY
   	   MainWindow mainWindow(false, true, true, false, false);
 	   MainWindow::PROGRAM_MODE = QString("text-only");
  #else
    	MainWindow mainWindow;
  #endif
  #endif
  
   mainWindow.hide();
  
  /*******  title , etc ***********/
    mainWindow.setWindowTitle(QObject::tr("TinkerCell"));
    mainWindow.statusBar()->showMessage(QObject::tr("Welcome to TinkerCell"));

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

	/**********  statically loaded plugins  ****************/
	mainWindow.addTool(new BasicGraphicsToolbar);
	mainWindow.addTool(new PlotTool);
   	mainWindow.addTool(new GnuplotTool);
	mainWindow.addTool(new LoadSaveTool);
	
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
	mainWindow.addTool(new CompartmentTool);
	mainWindow.addTool(new SimulationEventsTool);
	
	mainWindow.addTool(new OctaveExporter);
	mainWindow.addTool(new SBMLImportExport);
	mainWindow.addTool(new CopasiExporter);
	mainWindow.addTool(new AntimonyEditor);
	
	mainWindow.addTool(new ModuleTool);
	mainWindow.addTool(new AutoGeneRegulatoryTool);
	mainWindow.addTool(new CLabelsTool);
	mainWindow.addTool(new DNASequenceViewer);
	mainWindow.addTool(new ViewTablesTool);
	mainWindow.addTool(new LPSolveInputWindow);
	mainWindow.addTool(new CellPositionUpdateTool);

	mainWindow.addTool(new DynamicLibraryMenu);
    mainWindow.addTool(new LoadCLibrariesTool);
	mainWindow.addTool(new PythonTool);
	mainWindow.addTool(new OctaveTool);
	mainWindow.addTool(new CodingWindow);
	mainWindow.addTool(new ModuleCombinatorics);

    /*******  Dynamically load plugins from folders ***********/
    DefaultPluginsMenu menu(&mainWindow);

    mainWindow.optionsMenu->addMenu(&menu);
	mainWindow.setDockOptions(QMainWindow::AnimatedDocks|QMainWindow::AllowNestedDocks);

    QString home = MainWindow::homeDir();

//    LoadPluginsFromDir(appDir + QString("/plugins"),&mainWindow, &splash);
#ifdef Q_WS_WIN
	LoadPluginsFromDir(home + QString("/plugins/windows"),&mainWindow, &splash);
#else
#ifdef Q_WS_MAC
	LoadPluginsFromDir(home + QString("/plugins/mac"),&mainWindow, &splash);
#else
	LoadPluginsFromDir(home + QString("/plugins/ubuntu"),&mainWindow, &splash);
#endif
#endif

//    LoadPluginsFromDir(appDir + QString("/plugins/c"),&mainWindow, &splash);
#ifdef Q_WS_WIN
	LoadPluginsFromDir(home + QString("/plugins/c/windows"),&mainWindow, &splash);
#else
#ifdef Q_WS_MAC
	LoadPluginsFromDir(home + QString("/plugins/c/mac"),&mainWindow, &splash);
#else
	LoadPluginsFromDir(home + QString("/plugins/c/ubuntu32"),&mainWindow, &splash);
	LoadPluginsFromDir(home + QString("/plugins/c/ubuntu64"),&mainWindow, &splash);
#endif
#endif

    /*******  START TINKERCELL ***********/

    mainWindow.readSettings();
  #ifdef TINKERCELL_LITE_TEXT
       mainWindow.newEditor();
  #else
       mainWindow.newScene();
  #endif
    mainWindow.show();
    splash.finish(&mainWindow);

    /*******  process command line arguments, if any ***********/
	if (argc > 1)
		for (int i=1; i < argc; ++i)
			mainWindow.open(QString(argv[i]));

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

