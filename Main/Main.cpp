/****************************************************************************

 Copyright (C) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYWRITE.TXT

 This is the main application file for Tinkercell. It constructs a MainWindow
 and loads a list of default plugins.

****************************************************************************/

#include "DefaultPluginsMenu.h"
#include "GraphicsScene.h"
#include "ConsoleWindow.h"
#include <QColor>
#include <QBrush>

using namespace Tinkercell;

void LoadPluginsFromDir(const QString&,MainWindow *,QSplashScreen*);

int main(int argc, char *argv[])
{
    QApplication::setColorSpec (QApplication::ManyColor);
    QApplication app(argc, argv);

    PROJECTWEBSITE = QObject::tr("www.tinkercell.com");
    ORGANIZATIONNAME = QObject::tr("TinkerCell");
    PROJECTNAME = QObject::tr("TinkerCell");
    Tinkercell::MainWindow::defaultFileExtension = QObject::tr("tic");
    Tinkercell::ConsoleWindow::Prompt = QObject::tr(">>>");	
	Tinkercell::ConsoleWindow::BackgroundColor = QColor("#222222");
	
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

    QString splashFile(":/images/Tinkercell.png");


    MainWindow mainWindow;
    
    mainWindow.setWindowTitle(QObject::tr("Tinkercell: synthetic biology CAD"));
    mainWindow.statusBar()->showMessage(QObject::tr("Welcome to Tinkercell"));


	QPixmap pixmap(splashFile);

	QSplashScreen splash(pixmap,Qt::SplashScreen);//|Qt::WindowStaysOnTopHint);
	
    QSize sz = mainWindow.size();
    QPoint pos = mainWindow.pos();
    splash.move(pos + QPoint(sz.width()-pixmap.width(), sz.height()-pixmap.height())/2 );

    splash.setWindowOpacity(0.75);

    splash.show();

    DefaultPluginsMenu menu(&mainWindow);

    mainWindow.optionsMenu->addMenu(&menu);
	mainWindow.setDockOptions(QMainWindow::AnimatedDocks|QMainWindow::AllowNestedDocks);

    QString home = MainWindow::homeDir();

    LoadPluginsFromDir(appDir + QString("/plugins"),&mainWindow, &splash);
    LoadPluginsFromDir(home + QString("/plugins"),&mainWindow, &splash);

    LoadPluginsFromDir(appDir + QString("/plugins/c"),&mainWindow, &splash);
    LoadPluginsFromDir(home + QString("/plugins/c"),&mainWindow, &splash);

	mainWindow.newScene();
	
	mainWindow.readSettings();
	
    mainWindow.show();

    splash.finish(&mainWindow);
	
	if (argc > 1) mainWindow.open(QString(argv[1]));

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
