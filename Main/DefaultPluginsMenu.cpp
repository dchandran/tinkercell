#include "Main.h"

namespace Tinkercell
{

DefaultPluginsMenu::DefaultPluginsMenu(MainWindow * main)
        : QMenu(tr("Plug-ins"), main)
    {
      QCoreApplication::setOrganizationName(Tinkercell::ORGANIZATIONNAME);
      QCoreApplication::setOrganizationDomain(Tinkercell::PROJECTWEBSITE);
      QCoreApplication::setApplicationName(Tinkercell::ORGANIZATIONNAME);

      QSettings settings(Tinkercell::ORGANIZATIONNAME, Tinkercell::ORGANIZATIONNAME);

      settings.beginGroup("MainWindow");

      QStringList doNotLoad = settings.value("doNotLoadPlugins", QStringList()).toStringList();

      settings.endGroup();

      for (int i = 0; i < doNotLoad.size(); ++i)
        doNotLoadPluginNames += doNotLoad.at(i).toLower();

      connect(main,
              SIGNAL(toolAboutToBeLoaded( Tool * , bool * )),
              this,
              SLOT(toolAboutToBeLoaded( Tool * , bool * )));

    }

    DefaultPluginsMenu::~DefaultPluginsMenu()
    {
      saveSettings();
    }

    void DefaultPluginsMenu::saveSettings()
    {
      QCoreApplication::setOrganizationName(Tinkercell::ORGANIZATIONNAME);
      QCoreApplication::setOrganizationDomain(Tinkercell::PROJECTWEBSITE);
      QCoreApplication::setApplicationName(Tinkercell::ORGANIZATIONNAME);

      QSettings settings(Tinkercell::ORGANIZATIONNAME, Tinkercell::ORGANIZATIONNAME);

      QStringList doNotLoad;

      for (int i = 0; i < actions.size(); ++i)
        if (actions[i] && !actions[i]->isChecked())
          doNotLoad << actions[i]->text();

      settings.beginGroup("MainWindow");

      settings.setValue("doNotLoadPlugins", QVariant(doNotLoad));

      settings.endGroup();
    }

    void DefaultPluginsMenu::toolAboutToBeLoaded( Tool * tool, bool * shouldLoad )
    {
      if (!tool)
        return;

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

}

