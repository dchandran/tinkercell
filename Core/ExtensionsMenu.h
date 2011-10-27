#ifndef TINKERCELL_PLUGINS_LOADED_MENU_H
#define TINKERCELL_PLUGINS_LOADED_MENU_H

#include <QApplication>
#include <QCoreApplication>
#include <QLibrary>
#include <stdio.h>
#include <stdlib.h>
#include <QList>
#include <QString>
#include <QRegExp>
#include <QFile>
#include <QFileInfoList>
#include <QFileInfo>
#include <QAction>
#include <QString>
#include <QStringList>
#include <QSplashScreen>
#include <QMenu>
#include <QDir>
#include <QSettings>
#include <QtDebug>

#include "MainWindow.h"
#include "ItemHandle.h"
#include "DataTable.h"
#include "Tool.h"

namespace Tinkercell
{

class ExtensionsMenu : public QMenu
{
    Q_OBJECT

  public:
    ExtensionsMenu(MainWindow * main);

    virtual ~ExtensionsMenu();
    void saveSettings();

  public slots:
    void toolAboutToBeLoaded( Tool * tool, bool * shouldLoad );
	void toggleTool(bool);
	void toggleToolsLayout();
	void toggleHistoryLayout();
	void toggleConsoleLayout();

  private:
	QAction * toggleToolsLayoutAction;
	QAction * toggleHistoryLayoutAction;
	QAction * toggleConsoleLayoutAction;
    QStringList doNotLoadPluginNames;
    QList<QAction*> actions;
	QList<QMenu*> menus;
};

}

#endif // MAIN_H

