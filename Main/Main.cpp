/****************************************************************************

 Copyright (C) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYWRITE.TXT

 This is the main application file for Tinkercell. It constructs a MainWindow
 and loads a list of default plugins.

****************************************************************************/


#include "DefaultPluginsMenu.h"

using namespace Tinkercell;

void LoadPluginsFromDir(const QString&,MainWindow *,QSplashScreen*);

int main(int argc, char *argv[])
{
    PROJECTWEBSITE = QObject::tr("www.tinkercell.com");
    ORGANIZATIONNAME = QObject::tr("TinkerCell");
    PROJECTNAME = QObject::tr("Tinkercell");

    QApplication app(argc, argv);

    QString appDir = QCoreApplication::applicationDirPath();
    QFile styleFile(appDir + QString("/tinkercell.qss"));

    if (styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        app.setStyleSheet(styleFile.readAll());
        styleFile.close();
    }

    QString splashFile(":/images/Tinkercell.png");


    MainWindow mainWindow;
    
	mainWindow.setWindowTitle(QObject::tr("Tinkercell: design tool for biochemical networks"));
    mainWindow.statusBar()->showMessage(QObject::tr("Welcome to Tinkercell"));


    QSplashScreen splash(QPixmap(splashFile).scaled(250,250),Qt::SplashScreen);//|Qt::WindowStaysOnTopHint);

    QSize sz = mainWindow.size();
    QPoint pos = mainWindow.pos();
    splash.move(pos + QPoint(sz.width()-250 , sz.height()-250)/2 );

    splash.setWindowOpacity(0.75);

    splash.show();

    DefaultPluginsMenu menu(&mainWindow);

    mainWindow.settingsMenu->addMenu(&menu);

    QString home = MainWindow::userHome();

    LoadPluginsFromDir(appDir + QString("/Plugins"),&mainWindow, &splash);
    LoadPluginsFromDir(home + QString("/Plugins"),&mainWindow, &splash);

    LoadPluginsFromDir(appDir + QString("/Plugins/c"),&mainWindow, &splash);
    LoadPluginsFromDir(home + QString("/Plugins/c"),&mainWindow, &splash);

    //mainWindow.newTextWindow();
    mainWindow.newGraphicsWindow();
	mainWindow.newGraphicsWindow();

    mainWindow.show();

    splash.finish(&mainWindow);

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
