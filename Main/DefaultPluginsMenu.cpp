#include <QMessageBox>
#include "DefaultPluginsMenu.h"

namespace Tinkercell
{

	DefaultPluginsMenu::DefaultPluginsMenu(MainWindow * main)
        : QMenu(tr("Plug-ins"), main)
    {
      QCoreApplication::setOrganizationName(Tinkercell::MainWindow::ORGANIZATIONNAME);
      QCoreApplication::setOrganizationDomain(Tinkercell::MainWindow::PROJECTWEBSITE);
      QCoreApplication::setApplicationName(Tinkercell::MainWindow::ORGANIZATIONNAME);

      QSettings settings(Tinkercell::MainWindow::ORGANIZATIONNAME, Tinkercell::MainWindow::ORGANIZATIONNAME);

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
	  
	  if (main && main->optionsMenu)
	  {
			QMenu * menu = new QMenu(tr("Change layout"),main->optionsMenu);
			main->optionsMenu->addMenu(menu);
			
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
      QCoreApplication::setOrganizationName(Tinkercell::MainWindow::ORGANIZATIONNAME);
      QCoreApplication::setOrganizationDomain(Tinkercell::MainWindow::PROJECTWEBSITE);
      QCoreApplication::setApplicationName(Tinkercell::MainWindow::ORGANIZATIONNAME);

      QSettings settings(Tinkercell::MainWindow::ORGANIZATIONNAME, Tinkercell::MainWindow::ORGANIZATIONNAME);

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

	  QMenu * menu = 0;
	  QString s = tool->category;
	  if (s.isEmpty()) s = tr("misc.");
	  
	  for (int i=0; i < menus.size(); ++i)
		if (menus[i] && menus[i]->title() == s)
		{
			menu = menus[i];
			break;
		}
	  
	  if (!menu)
	  {
		menu = this->addMenu(s);
		menus << menu;
	  }
	  
      QAction * action = menu->addAction(name);
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
	  connect(action,SIGNAL(toggled(bool)),this,SLOT(toggleTool(bool)));
    }
	
	void DefaultPluginsMenu::toggleTool(bool)
	{
		QMessageBox::information(this->parentWidget(),tr("Plug-in menu"),tr("Changes in the plug-ins will take effect the next time TinkerCell starts"));
	}

}

