#ifndef MAIN_H
#define MAIN_H

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
    class DefaultPluginsMenu : public QMenu
    {
        Q_OBJECT
    public:
        DefaultPluginsMenu(MainWindow * main)
            : QMenu(tr("Plug-ins"),main)
        {
            QCoreApplication::setOrganizationName(Tinkercell::ORGANIZATIONNAME);
            QCoreApplication::setOrganizationDomain(Tinkercell::PROJECTWEBSITE);
            QCoreApplication::setApplicationName(Tinkercell::ORGANIZATIONNAME);

            QSettings settings(Tinkercell::ORGANIZATIONNAME, Tinkercell::ORGANIZATIONNAME);

            settings.beginGroup("MainWindow");

            QStringList doNotLoad = settings.value("doNotLoadPlugins", QStringList()).toStringList();

            settings.endGroup();

            for (int i=0; i < doNotLoad.size(); ++i)
                doNotLoadPluginNames += doNotLoad.at(i).toLower();

            connect(main,
                        SIGNAL(toolAboutToBeLoaded( Tool * , bool * )),
                        this,
                        SLOT(toolAboutToBeLoaded( Tool * , bool * )));

        }

        virtual ~DefaultPluginsMenu()
        {
            saveSettings();
        }

        void saveSettings()
        {
            QCoreApplication::setOrganizationName(Tinkercell::ORGANIZATIONNAME);
            QCoreApplication::setOrganizationDomain(Tinkercell::PROJECTWEBSITE);
            QCoreApplication::setApplicationName(Tinkercell::ORGANIZATIONNAME);

            QSettings settings(Tinkercell::ORGANIZATIONNAME, Tinkercell::ORGANIZATIONNAME);

            QStringList doNotLoad;
            for (int i=0; i < actions.size(); ++i)
                if (actions[i] && !actions[i]->isChecked())
                    doNotLoad << actions[i]->text();

            settings.beginGroup("MainWindow");
            settings.setValue("doNotLoadPlugins",QVariant(doNotLoad));
            settings.endGroup();
        }
    public slots:
        void toolAboutToBeLoaded( Tool * tool, bool * shouldLoad )
        {
            if (!tool) return;
            QString name = tool->name;

            QAction * action = this->addAction(name);
            action->setCheckable(true);
            actions << action;
            if (doNotLoadPluginNames.contains(name.toLower()) && shouldLoad)
            {
                (*shouldLoad) = false;
                action->setChecked(false);
            }
            else
            {
                action->setChecked(true);
            }
        }
    private:
        QStringList doNotLoadPluginNames;
        QList<QAction*> actions;
    };

}

#endif // MAIN_H

