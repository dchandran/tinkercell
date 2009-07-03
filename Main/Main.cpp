/****************************************************************************

 Copyright (C) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYWRITE.TXT
 
 This is the main application file for Tinkercell. It constructs a MainWindow
 and loads a list of default plugins. 

****************************************************************************/

#include <QApplication>
#include <QLibrary>
#include <stdio.h>
#include <stdlib.h>
#include <QList>
#include <QString>
#include <QRegExp>
#include <QFile>
#include <QAction>
#include <QStringList>
#include <QSplashScreen>
#include <QtDebug>

#include "MainWindow.h"
#include "ItemHandle.h"
#include "DataTable.h"

int main(int argc, char *argv[])
{

     QApplication app(argc, argv);

     QString appDir = QCoreApplication::applicationDirPath();
#ifdef Q_WS_MAC
     appDir += QObject::tr("/../../..");
#endif
     //QDir::setCurrent(appDir);

     QFile styleFile(appDir + QString("/tinkercell.qss"));
     if (styleFile.open(QFile::ReadOnly | QFile::Text))
     {
          app.setStyleSheet(styleFile.readAll());
          styleFile.close();
     }
     QString splashFile(":/images/Tinkercell.png");

     //QImage image(splashFile);
     //image.createMaskFromColor(QColor(255,255,255,255).rgb(),Qt::MaskInColor);

     QCoreApplication::setOrganizationName("TinkerCell");
     QCoreApplication::setOrganizationDomain("www.tinkercell.com");
     QCoreApplication::setApplicationName("TinkerCell");

     QSettings settings("TinkerCell", "TinkerCell");

     QSplashScreen splash(QPixmap(splashFile).scaled(250,250),Qt::SplashScreen|Qt::WindowStaysOnTopHint);

     settings.beginGroup("MainWindow");
     QSize sz = settings.value("size", QSize(1000, 500)).toSize();
     splash.move(settings.value("pos", QPoint(50, 100)).toPoint() + QPoint(sz.width() - 250,sz.height() - 250)/2);
     settings.endGroup();

     splash.setWindowOpacity(0.75);
     splash.show();

     Tinkercell::MainWindow mainWindow;

     //register new signal/slot data types

     qRegisterMetaType< QList<QGraphicsItem*> >("QList<QGraphicsItem*>");
     qRegisterMetaType< QStringList >("QStringList");

     qRegisterMetaType< QList<QGraphicsItem*> >("QList<QGraphicsItem*>&");
     qRegisterMetaType< QStringList >("QStringList&");

     qRegisterMetaType< QList<QGraphicsItem*>* >("QList<QGraphicsItem*>*");
     qRegisterMetaType< QStringList* >("QStringList*");

     qRegisterMetaType< Tinkercell::DataTable<qreal> >("DataTable<qreal>");
     qRegisterMetaType< Tinkercell::DataTable<QString> >("DataTable<qreal>");

     qRegisterMetaType< Tinkercell::DataTable<qreal> >("DataTable<qreal>&");
     qRegisterMetaType< Tinkercell::DataTable<QString> >("DataTable<qreal>&");

     qRegisterMetaType< Tinkercell::DataTable<qreal>* >("DataTable<qreal>*");
     qRegisterMetaType< Tinkercell::DataTable<QString>* >("DataTable<qreal>*");

     qRegisterMetaType< Tinkercell::ItemHandle* >("ItemHandle*");
     qRegisterMetaType< QList<Tinkercell::ItemHandle*> >("QList<ItemHandle*>");
     qRegisterMetaType< QList<Tinkercell::ItemHandle*> >("QList<ItemHandle*>&");

     qRegisterMetaType< QList<QStringList> >("QList<QStringList>");
     qRegisterMetaType< QList<QStringList> >("QList<QStringList>&");

     qRegisterMetaType< Matrix >("Matrix");

     //	Tinkercell::mainWindow = &mainWindow;
     //	app.setStyle(QObject::tr("cde"));


     //load default plugins from one of two files (user dir or app dir)

     //list of plugins not to load
     QList<QAction*> loadUnloadPluginActions;
     QStringList pluginFileNames;

     QMenu * defaultPluginsMenu = mainWindow.settingsMenu->addMenu(QString("Set default plugins"));

     QFile file1 (Tinkercell::MainWindow::userHome() + QString("/Tinkercell/DefaultPlugins.txt"));
     QFile file2 (appDir + QString("/DefaultPlugins.txt"));
     QFile file3 (Tinkercell::MainWindow::userHome() + QString("/Tinkercell/DoNotLoadThesePlugins.txt"));

     QStringList doNotLoad;
     if (file3.open(QFile::ReadOnly | QFile::Text))
     {
          while (!file3.atEnd())
          {
               QString plugin(file3.readLine());
               plugin = plugin.trimmed();
               if (!plugin.isEmpty())
               {
                    doNotLoad << plugin;
               }
          }
          file3.close();
     }

     QAction * action;
     if (file1.open(QFile::ReadOnly | QFile::Text))
     {
          while (!file1.atEnd())
          {
               QString line(file1.readLine());
               QStringList list(line.split(","));
               QString plugin, desc;

               if (list.size() > 1)
               {
                    plugin = list[0];
                    list.pop_front();
                    desc = list.join(",");
               }
               else
               {
                    plugin = line;
               }

               plugin = plugin.trimmed();
               desc = desc.trimmed();

               if (!plugin.isEmpty())
               {
                    action = defaultPluginsMenu->addAction(desc);
                    action->setCheckable(true);
                    loadUnloadPluginActions << action;
                    pluginFileNames << plugin;

                    if (doNotLoad.contains(plugin))
                    {
                         action->setChecked(false);
                    }
                    else
                    {
                         splash.showMessage(QString("loading ") + plugin);
                         mainWindow.loadDynamicLibrary(plugin);
                         action->setChecked(true);
                    }
               }
          }
          file1.close();
     }
     else
          if (file2.open(QFile::ReadOnly | QFile::Text))
          {
          while (!file2.atEnd())
          {
               QString line(file2.readLine());
               QStringList list(line.split(","));
               QString plugin, desc;

               if (list.size() > 1)
               {
                    plugin = list[0];
                    list.pop_front();
                    desc = list.join(",");
               }
               else
               {
                    plugin = line;
               }

               plugin = plugin.trimmed();
               desc = desc.trimmed();

               if (!plugin.isEmpty())
               {
                    action = defaultPluginsMenu->addAction(desc);
                    action->setCheckable(true);
                    loadUnloadPluginActions << action;
                    pluginFileNames << plugin;

                    if (doNotLoad.contains(plugin))
                    {
                         action->setChecked(false);
                    }
                    else
                    {
                         splash.showMessage(QString("loading ") + plugin);
                         mainWindow.loadDynamicLibrary(plugin);
                         action->setChecked(true);
                    }
               }
          }
          file2.close();
     }
     else
          splash.showMessage(QString("cannot open DefaultPlugins.txt") );

     mainWindow.newTextWindow();
     mainWindow.newGraphicsWindow();

     mainWindow.show();

     splash.finish(&mainWindow);

     int output = app.exec();

     QFile file4 (Tinkercell::MainWindow::userHome() + QString("/Tinkercell/DoNotLoadThesePlugins.txt"));

     if (file4.open(QFile::WriteOnly | QFile::Text))
     {
          QString s;
          for (int i=0; i < pluginFileNames.size() && i < loadUnloadPluginActions.size(); ++i)
               if (loadUnloadPluginActions[i] && !loadUnloadPluginActions[i]->isChecked())
               {
               s = pluginFileNames[i] + QString("\n");
               file4.write(s.toAscii().data());
          }
          file4.close();
     }

     return output;
}
