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
#include "PythonTool.h"
#include "OctaveTool.h"
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
  	    MainWindow mainWindow(true, false, false);
	    MainWindow::PROGRAM_MODE = QString("parts-only");
  #else
  #ifdef TINKERCELL_TEXT_ONLY
   	   MainWindow mainWindow(false, true, true);
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

    /*******  Dynamically load plugins from folders ***********/
    DefaultPluginsMenu menu(&mainWindow);
    mainWindow.settingsMenu->addMenu(&menu);
	mainWindow.setDockOptions(QMainWindow::AnimatedDocks|QMainWindow::AllowNestedDocks);

    QString home = MainWindow::homeDir();

    LoadPluginsFromDir(appDir + QString("/") + QString(TINKERCELL_CPP_PLUGINS_FOLDER),&mainWindow, &splash);
	LoadPluginsFromDir(home + QString("/") + QString(TINKERCELL_CPP_PLUGINS_FOLDER),&mainWindow, &splash);

	LoadPluginsFromDir(appDir + QString("/") + QString(TINKERCELL_C_PLUGINS_FOLDER),&mainWindow, &splash);
	LoadPluginsFromDir(home + QString("/") +  QString(TINKERCELL_CPP_PLUGINS_FOLDER),&mainWindow, &splash);
	

    /*******  START TINKERCELL ***********/

    mainWindow.readSettings();
  #ifdef TINKERCELL_TEXT_ONLY
       mainWindow.newTextEditor();
  #else
       mainWindow.newScene();
  #endif
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

