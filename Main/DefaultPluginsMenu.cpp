#include <QMessageBox>
#include "DefaultPluginsMenu.h"

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
			  
	  
	  toggleToolsLayoutAction = toggleHistoryLayoutAction = toggleConsoleLayoutAction = 0;
	  
	  if (main && main->settingsMenu)
	  {
			QMenu * menu = new QMenu(tr("Change layout"),main->settingsMenu);
			main->settingsMenu->addMenu(menu);
			
			if (MainWindow::defaultToolWindowOption == MainWindow::ToolBoxWidget)			
				toggleToolsLayoutAction = menu->addAction(tr("Use dock widgets for tools"));			
			else			
				toggleToolsLayoutAction = menu->addAction(tr("Use tool-box widgets for tools"));				
			
			if (MainWindow::defaultHistoryWindowOption == MainWindow::ToolBoxWidget)			
				toggleHistoryLayoutAction = menu->addAction(tr("Use dock widgets for history"));			
			else			
				toggleHistoryLayoutAction = menu->addAction(tr("Use tool-box widgets for history"));
				
			if (MainWindow::defaultConsoleWindowOption == MainWindow::ToolBoxWidget)			
				toggleConsoleLayoutAction = menu->addAction(tr("Use dock widgets for console"));			
			else			
				toggleConsoleLayoutAction = menu->addAction(tr("Use tool-box widgets for console"));

			connect(toggleToolsLayoutAction,SIGNAL(triggered()),this,SLOT(toggleToolsLayout()));
			connect(toggleHistoryLayoutAction,SIGNAL(triggered()),this,SLOT(toggleHistoryLayout()));
			connect(toggleConsoleLayoutAction,SIGNAL(triggered()),this,SLOT(toggleConsoleLayout()));
	  }
    }
	
	void DefaultPluginsMenu::toggleHistoryLayout()
	{
		if (MainWindow::defaultHistoryWindowOption == MainWindow::DockWidget)
		{
			toggleToolsLayoutAction->setText(tr("Use dock widgets for history"));
			MainWindow::defaultHistoryWindowOption =  MainWindow::ToolBoxWidget;
		}
		else
		{
			toggleToolsLayoutAction->setText(tr("Use tool-box widgets for history"));
			MainWindow::defaultHistoryWindowOption =  MainWindow::DockWidget;
		}
		QMessageBox::information(this,tr("History Window Layout"),tr("The change in layout will take effect the next time TinkerCell starts"));
	}
	
	void DefaultPluginsMenu::toggleConsoleLayout()
	{
		if (MainWindow::defaultConsoleWindowOption == MainWindow::DockWidget)
		{
			toggleConsoleLayoutAction->setText(tr("Use dock widgets for console"));
			MainWindow::defaultConsoleWindowOption =  MainWindow::ToolBoxWidget;
		}
		else
		{
			toggleConsoleLayoutAction->setText(tr("Use tool-box widgets for console"));
			MainWindow::defaultConsoleWindowOption =  MainWindow::DockWidget;
		}
		QMessageBox::information(this,tr("Console Window Layout"),tr("The change in layout will take effect the next time TinkerCell starts"));
	}
	
	void DefaultPluginsMenu::toggleToolsLayout()
	{
		if (MainWindow::defaultToolWindowOption == MainWindow::DockWidget)
		{
			toggleToolsLayoutAction->setText(tr("Use dock widgets for tools"));
			MainWindow::defaultToolWindowOption =  MainWindow::ToolBoxWidget;
		}
		else
		{
			toggleToolsLayoutAction->setText(tr("Use tool-box widgets for tools"));
			MainWindow::defaultToolWindowOption =  MainWindow::DockWidget;
		}
		QMessageBox::information(this,tr("Tool Window Layout"),tr("The change in layout will take effect the next time TinkerCell starts"));
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

