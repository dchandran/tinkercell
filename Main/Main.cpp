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
#include <QColor>
#include <QBrush>

using namespace Tinkercell;
#define LITE

void LoadPluginsFromDir(const QString&,MainWindow *,QSplashScreen*);

int main(int argc, char *argv[])
{
    QApplication::setColorSpec (QApplication::ManyColor);
    QApplication app(argc, argv);

    /***********************	
    	Optional configuration
    ***********************/
    
    Tinkercell::MainWindow::PROJECTWEBSITE = QObject::tr("www.tinkercell.com");
    Tinkercell::MainWindow::ORGANIZATIONNAME = QObject::tr("TinkerCell");
    Tinkercell::MainWindow::PROJECTNAME = QObject::tr("TinkerCell");
    Tinkercell::ConsoleWindow::Prompt = QObject::tr(">");	
	Tinkercell::ConsoleWindow::BackgroundColor = QColor("#555555");
	
	QColor color("#00EE00");
	color.setAlpha(50);
	Tinkercell::GraphicsScene::SelectionRectangleBrush = QBrush(color);

    QString appDir = QCoreApplication::applicationDirPath();	
    QFile styleFile(appDir + QString("/tinkercell.qss"));

    if (styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        app.setStyleSheet(styleFile.readAll());
        styleFile.close();
    }
    
    /*******  Main Window ***********/
    
    // "lite" mode
  #ifdef LITE
    	MainWindow mainWindow(true, false, false, true, true);
	    MainWindow::PROGRAM_MODE = QString("lite");
	    mainWindow.addTool(new BasicGraphicsToolbar());
	    mainWindow.addTool(new PlotTool());
	    mainWindow.addTool(new GnuplotTool());
  #else
    	MainWindow mainWindow;
    	mainWindow.addTool(new BasicGraphicsToolbar());
	    mainWindow.addTool(new PlotTool());
	#endif

    /*******  title , etc ***********/
    mainWindow.setWindowTitle(QObject::tr("Tinkercell: synthetic biology CAD"));
    mainWindow.statusBar()->showMessage(QObject::tr("Welcome to Tinkercell"));

    /*******  Splash screen ***********/

    QString splashFile(":/images/Tinkercell.png");
	QPixmap pixmap(splashFile);
	QSplashScreen splash(pixmap,Qt::SplashScreen);//|Qt::WindowStaysOnTopHint);
	
    QSize sz = mainWindow.size();
    QPoint pos = mainWindow.pos();
    splash.move(pos + QPoint(sz.width()-pixmap.width(), sz.height()-pixmap.height())/2 );

    splash.setWindowOpacity(0.75);

    splash.show();


    /*******  Load plugins from folders ***********/
    DefaultPluginsMenu menu(&mainWindow);

    mainWindow.optionsMenu->addMenu(&menu);
	mainWindow.setDockOptions(QMainWindow::AnimatedDocks|QMainWindow::AllowNestedDocks);

    QString home = MainWindow::homeDir();

    LoadPluginsFromDir(appDir + QString("/plugins"),&mainWindow, &splash);
#ifdef Q_WS_WIN
	LoadPluginsFromDir(home + QString("/plugins/windows"),&mainWindow, &splash);
#else
#ifdef Q_WS_MAC
	LoadPluginsFromDir(home + QString("/plugins/mac"),&mainWindow, &splash);
#else
	LoadPluginsFromDir(home + QString("/plugins/ubuntu"),&mainWindow, &splash);
#endif
#endif

    LoadPluginsFromDir(appDir + QString("/plugins/c"),&mainWindow, &splash);
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
	mainWindow.newScene();
    mainWindow.show();
    splash.finish(&mainWindow);

    /*******  command line arguments ***********/
	if (argc > 1) 
		for (int i=1; i < argc; ++i)
			mainWindow.open(QString(argv[i]));

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

