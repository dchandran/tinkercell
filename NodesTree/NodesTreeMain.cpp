/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 Function that loads dll into main window

****************************************************************************/
#include <QDialog>
#include <QStringList>
#include <QPushButton>
#include <QInputDialog>
#include <QTabWidget>
#include <QMessageBox>
#include <QVBoxLayout>
#include "ItemFamily.h"
#include "NodesTreeMain.h"

namespace Tinkercell
{
	NodesTreeContainer::MODE NodesTreeContainer::layoutMode = NodesTreeContainer::TabView;

	NodesTreeContainer::NodesTreeContainer(NodesTree * nodesTree, ConnectionsTree * connectionsTree, QWidget * parent) :
		Tool(tr("Parts and Connections"),parent),
		nodesButtonGroup(this),
		connectionsButtonGroup(this),
		toolBox(0),
		tabWidget(0),
		nodesTree(nodesTree),
		connectionsTree(connectionsTree)
	{
		QSettings settings("TinkerCell", "TinkerCell");

		settings.beginGroup("NodesTreeContainer");
		NodesTreeContainer::layoutMode = (NodesTreeContainer::MODE)(settings.value(tr("Mode"),(int)layoutMode).toInt());
		settings.endGroup();

		arrowButton.setToolTip(QObject::tr("Cursor"));
        arrowButton.setPalette(QPalette(QColor(255,255,255)));
        arrowButton.setAutoFillBackground (true);
        arrowButton.setIcon(QIcon(QObject::tr(":/images/arrow.png")));
        arrowButton.setIconSize(QSize(20,20));
        //arrowButton.setPopupMode(QToolButton::MenuButtonPopup);
		initialValuesTable = 0;//new QTableWidget(this);
		initialValuesComboBox = 0;//new QComboBox(this);

		if (layoutMode == TabView)
			setUpTabView();
		else
			setUpTreeView();
	}

	void NodesTreeContainer::setupInitialSettingsWidget(MainWindow * main)
	{
		if (!main || !initialValuesTable || !initialValuesComboBox) return;

		QDialog * dialog = new QDialog(main);

		QVBoxLayout * layout = new QVBoxLayout;

		connect(initialValuesComboBox,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(initialValueComboBoxChanged(const QString&)));

		initialValuesTable->setColumnCount(1);
		initialValuesTable->setHorizontalHeaderLabels(QStringList() << tr("value"));

		layout->addWidget(initialValuesComboBox);
		layout->addWidget(initialValuesTable);

		QPushButton * okButton = new QPushButton;
		QPushButton * cancelButton = new QPushButton;
		okButton->setText(tr("Set"));
		cancelButton->setText(tr("Cancel"));

		QHBoxLayout * layout2 = new QHBoxLayout;
		layout2->addWidget(okButton);
		layout2->addWidget(cancelButton);

		connect(okButton,SIGNAL(pressed()),dialog,SLOT(accept()));
		connect(cancelButton,SIGNAL(pressed()),dialog,SLOT(reject()));

		layout->addLayout(layout2);
		dialog->setLayout(layout);
		dialog->hide();

		if (mainWindow->settingsMenu)
			mainWindow->settingsMenu->addAction(tr("Set initial values"), dialog, SLOT(exec()));

		connect(dialog,SIGNAL(accepted()),this,SLOT(initialValuesChanged()));
	}

	void NodesTreeContainer::setTreeMode(bool b)
	{
		if (b)
			NodesTreeContainer::layoutMode = NodesTreeContainer::TreeView;
		else
			NodesTreeContainer::layoutMode = NodesTreeContainer::TabView;
		QMessageBox::information(this,tr("Parts Layout"),tr("The change in display will take effect the next time TinkerCell starts"));
	}

	void NodesTreeContainer::escapeSignalSlot(const QWidget*)
	{
		for (int i=0; i < widgetsToUpdate.size(); ++i)
			if (widgetsToUpdate[i])
				widgetsToUpdate[i]->setCursor(Qt::ArrowCursor);
	}

	bool NodesTreeContainer::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow)
		{
			connect(&arrowButton,SIGNAL(pressed()),mainWindow,SLOT(sendEscapeSignal()));
			connect(this,SIGNAL(sendEscapeSignal(const QWidget*)),mainWindow,SIGNAL(escapeSignal(const QWidget*)));
			connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSignalSlot(const QWidget*)));
			setWindowTitle(tr("Parts and Connections"));
			setWindowIcon(QIcon(tr(":/images/appicon.png")));

			if (layoutMode == TreeView)
			{
				mainWindow->addToolWindow(this,MainWindow::DockWidget,Qt::LeftDockWidgetArea,Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
				QAction * setNumRows = mainWindow->settingsMenu->addAction(QIcon(tr(":/images/up.png")), tr("Number of recent items"));
				connect (setNumRows, SIGNAL(triggered()),this,SLOT(setNumberOfRecentItems()));
			}
			else
			{
				mainWindow->addToolWindow(this,MainWindow::DockWidget,Qt::TopDockWidgetArea,Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
			}

			if (mainWindow->settingsMenu)
			{
				mainWindow->settingsMenu->addSeparator();
				QAction * treeViewAction = mainWindow->settingsMenu->addAction(tr("Use Tree view of parts and connections"));
				treeViewAction->setCheckable(true);
				treeViewAction->setChecked(NodesTreeContainer::layoutMode == NodesTreeContainer::TreeView);

				connect(treeViewAction,SIGNAL(toggled(bool)),this,SLOT(setTreeMode(bool)));

				setupInitialSettingsWidget(mainWindow);
			}

			return true;
		}
		return false;
	}

	void NodesTreeContainer::nodeButtonPressed ( int i )
	{
		if (nodes.size() > i)
		{
			emit sendEscapeSignal(this);
			qDebug() << nodes[i]->name;
			emit nodeSelected(nodes[i]);
		}
	}
	void NodesTreeContainer::connectionButtonPressed ( int i )
	{
		if (connections.size() > i)
		{
			emit sendEscapeSignal(this);
			emit connectionSelected(connections[i]);
		}
	}

	void NodesTreeContainer::nodeSelectedSlot(NodeFamily* nodeFamily)
	{
		if (!nodeFamily || nodes.isEmpty()) return;

		int w = 20, h = 20;
		if (nodeFamily->pixmap.width() > nodeFamily->pixmap.height())
		{
			w = 20 * nodeFamily->pixmap.width()/nodeFamily->pixmap.height();
			if (w > 50) w = 50;
		}
		else
		{
			h = 20 * nodeFamily->pixmap.height()/nodeFamily->pixmap.width();
			if (h > 50) h = 50;
		}

		for (int i=0; i < widgetsToUpdate.size(); ++i)
			if (widgetsToUpdate[i])
				widgetsToUpdate[i]->setCursor(QCursor(nodeFamily->pixmap.scaled(w,h)));

		if (nodes.contains(nodeFamily)) return;

		QList<QAbstractButton*> buttons = nodesButtonGroup.buttons();
		if (nodes.size() != buttons.size()) return;

		for (int i=nodes.size()-1; i > 0; --i)
			if (buttons[i] && nodes[i] && nodes[i-1])
			{
				nodes[i] = nodes[i-1];
				buttons[i]->setIcon(QIcon(nodes[i]->pixmap));
				buttons[i]->setToolTip(nodes[i]->name);
				buttons[i]->setText(nodes[i]->name);
				if (nodes[i]->pixmap.width() > nodes[i]->pixmap.height())
				{
					int w = 20 * nodes[i]->pixmap.width()/nodes[i]->pixmap.height();
					if (w > 50) w = 50;
					buttons[i]->setIconSize(QSize(w,20));
				}
				else
				{
					int h = 20 * nodes[i]->pixmap.height()/nodes[i]->pixmap.width();
					if (h > 50) h = 50;
					buttons[i]->setIconSize(QSize(20, h));
				}
			}

		nodes[0] = nodeFamily;
		buttons[0]->setIcon(QIcon(nodes[0]->pixmap));
		buttons[0]->setToolTip(nodes[0]->name);
		buttons[0]->setText(nodes[0]->name);
		buttons[0]->setIconSize(QSize(w,h));
	}

    void NodesTreeContainer::connectionSelectedSlot(ConnectionFamily* family)
	{
		if (!family || connections.isEmpty()) return;

		int w = 20, h = 20;

		if (family->pixmap.width() > family->pixmap.height())
		{
			w = 20 * family->pixmap.width()/family->pixmap.height();
			if (w > 50) w = 50;
		}
		else
		{
			h = 20 * family->pixmap.height()/family->pixmap.width();
			if (h > 50) h = 50;
		}

		for (int i=0; i < widgetsToUpdate.size(); ++i)
			if (widgetsToUpdate[i])
				widgetsToUpdate[i]->setCursor(QCursor(family->pixmap.scaled(w,h)));

		if (connections.contains(family)) return;

		QList<QAbstractButton*> buttons = connectionsButtonGroup.buttons();
		if (connections.size() != buttons.size()) return;

		for (int i=connections.size()-1; i > 0; --i)
			if (buttons[i] && connections[i] && connections[i-1])
			{
				connections[i] = connections[i-1];
				buttons[i]->setIcon(QIcon(connections[i]->pixmap));
				buttons[i]->setToolTip(connections[i]->name);
				buttons[i]->setText(connections[i]->name);
			}

		connections[0] = family;
		buttons[0]->setIcon(QIcon(connections[0]->pixmap));
		buttons[0]->setToolTip(connections[0]->name);
		buttons[0]->setText(connections[0]->name);
		buttons[0]->setIconSize(QSize(w, h));
	}

	NodesTreeContainer::~NodesTreeContainer()
	{
		QCoreApplication::setOrganizationName("TinkerCell");
		QCoreApplication::setOrganizationDomain("www.tinkercell.com");
		QCoreApplication::setApplicationName("TinkerCell");

		QSettings settings("TinkerCell", "TinkerCell");

		settings.beginGroup("NodesTreeContainer");
		settings.setValue(tr("Mode"),(int)(NodesTreeContainer::layoutMode));

		if (layoutMode == TreeView)
		{
			settings.beginGroup("LastSelectedNodes");

			for (int i=0; i < nodes.size(); ++i)
			{
			   if (nodes[i])
				settings.setValue(QString::number(i),nodes[i]->name);
			}
			settings.endGroup();

			settings.beginGroup("LastSelectedConnections");

			for (int i=0; i < connections.size(); ++i)
			{
			   if (connections[i])
				settings.setValue(QString::number(i),connections[i]->name);
			}

			settings.endGroup();
		}
		else
		{
			settings.setValue(tr("numNodeTabs"),numNodeTabs);

			QStringList list1, list2;
			for (int i=0; i < tabGroups.size(); ++i)
			{
				list1 << tabGroups[i].first;
				list2 << tabGroups[i].second.join(tr(","));
			}

			settings.setValue(tr("familyTabNames"),list1);
			settings.setValue(tr("familyTabs"),list2);
		}

		settings.endGroup();
	}

	QSize NodesTreeContainer::sizeHint() const
	{
		if (layoutMode == TreeView)
			return QSize(140, 600);
		else
			return QSize(600,80);
	}

	void NodesTreeContainer::keyPressEvent ( QKeyEvent * event )
	{
		emit keyPressed(event->key(),event->modifiers());
          if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Space)
               emit sendEscapeSignal(this);
	}

	void NodesTreeContainer::contextMenuEvent(QContextMenuEvent *)
	{
		emit sendEscapeSignal(this);
	}

	void NodesTreeContainer::setNumberOfRecentItems()
	{
		if (layoutMode != TreeView) return;

		QCoreApplication::setOrganizationName("TinkerCell");
		QCoreApplication::setOrganizationDomain("www.tinkercell.com");
		QCoreApplication::setApplicationName("TinkerCell");

		QSettings settings("TinkerCell", "TinkerCell");

		settings.beginGroup("LastSelectedNodes");

		int n = settings.value(tr("numRows"),5).toInt();
		n = QInputDialog::getInt(this,tr("Recent items"), tr("Number of recent items (will take event when TinkerCell starts)"), 2*n, 2, 20, 2);

		n = n/2;

		settings.setValue("numRows",n);
		settings.endGroup();

		settings.beginGroup("LastSelectedConnections");
		settings.setValue("numRows",n);
		settings.endGroup();
	}

	void NodesTreeContainer::setUpTreeView()
	{
		toolBox = new QToolBox;
		QGridLayout * buttonsLayout = new QGridLayout;
		buttonsLayout->addWidget(&arrowButton,0,0,Qt::AlignCenter);

		QCoreApplication::setOrganizationName("TinkerCell");
		QCoreApplication::setOrganizationDomain("www.tinkercell.com");
		QCoreApplication::setApplicationName("TinkerCell");
		QSettings settings("TinkerCell", "TinkerCell");

		int n = 5;
		QStringList allFamilyNames;

		settings.beginGroup("NodesTreeContainer");

		if (nodesTree)
		{
			widgetsToUpdate << nodesTree;

			NodeFamily * family;

			settings.beginGroup("LastSelectedNodes");

			n = settings.value(tr("numRows"),5).toInt();

			connect(this,SIGNAL(nodeSelected(NodeFamily*)),nodesTree,SIGNAL(nodeSelected(NodeFamily*)));
			connect(nodesTree,SIGNAL(nodeSelected(NodeFamily*)),this,SLOT(nodeSelectedSlot(NodeFamily*)));
			connect(this,SIGNAL(keyPressed(int, Qt::KeyboardModifiers)),nodesTree,SIGNAL(keyPressed(int, Qt::KeyboardModifiers)));

			toolBox->addItem(nodesTree,tr("Parts"));

			QList<QString> keys = nodesTree->nodeFamilies.keys();

			for (int i=0; nodes.size() < n && i < keys.size(); ++i)
			{
				QString s = settings.value(QString::number(i),keys[i]).toString();

				if (nodesTree->nodeFamilies.contains(s) && (family = nodesTree->nodeFamilies[s]))
				{
					nodes << family;
					allFamilyNames << family->name;

					QToolButton * button = new QToolButton;
					button->setIcon(QIcon(family->pixmap));
					button->setText(family->name);

					if (family->pixmap.width() > family->pixmap.height())
					{
						int w = 20 * family->pixmap.width()/family->pixmap.height();
						if (w > 50) w = 50;
						button->setIconSize(QSize(w,20));
					}
					else
					{
						int h = 20 * family->pixmap.height()/family->pixmap.width();
						if (h > 50) h = 50;
						button->setIconSize(QSize(20, h));
					}

					button->setToolTip(family->name);
					button->setPalette(QPalette(QColor(255,255,255)));
					button->setAutoFillBackground (true);
					buttonsLayout->addWidget(button,i+1,0,Qt::AlignCenter);
					nodesButtonGroup.addButton(button,i);
				}
			}
			settings.endGroup();
		}

		if (connectionsTree)
		{
			widgetsToUpdate << connectionsTree;

			ConnectionFamily * family;

			settings.beginGroup("LastSelectedConnections");

			connect(this,SIGNAL(connectionSelected(ConnectionFamily*)),connectionsTree,SIGNAL(connectionSelected(ConnectionFamily*)));
			connect(connectionsTree,SIGNAL(connectionSelected(ConnectionFamily*)),this,SLOT(connectionSelectedSlot(ConnectionFamily*)));
			connect(this,SIGNAL(keyPressed(int, Qt::KeyboardModifiers)),connectionsTree,SIGNAL(keyPressed(int, Qt::KeyboardModifiers)));

			toolBox->addItem(connectionsTree,tr("Connections"));
			QList<QString> keys = connectionsTree->connectionFamilies.keys();

			for (int i=0; connections.size() < n && i < keys.size(); ++i)
			{
				QString s = settings.value(QString::number(i),keys[i]).toString();

				if (connectionsTree->connectionFamilies.contains(s) &&
					(family = connectionsTree->connectionFamilies[s]))
				{
					connections << family;
					allFamilyNames << family->name;

					QToolButton * button = new QToolButton;
					button->setIcon(QIcon(family->pixmap));
					button->setText(family->name);

					if (family->pixmap.width() > family->pixmap.height())
					{
						int w = 20 * family->pixmap.width()/family->pixmap.height();
						if (w > 50) w = 50;
						button->setIconSize(QSize(w,20));
					}
					else
					{
						int h = 20 * family->pixmap.height()/family->pixmap.width();
						if (h > 50) h = 50;
						button->setIconSize(QSize(20, h));
					}

					button->setToolTip(family->name);
					button->setPalette(QPalette(QColor(255,255,255)));
					button->setAutoFillBackground (true);
					buttonsLayout->addWidget(button,i+1,1,Qt::AlignCenter);
					connectionsButtonGroup.addButton(button,i);
				}
			}
			settings.endGroup();
		}
		settings.endGroup();

		if (initialValuesComboBox)
			initialValuesComboBox->addItems(allFamilyNames);

		QWidget * widget = new QWidget;
		widgetsToUpdate << widget;

		buttonsLayout->setContentsMargins(5,0,0,0);
		buttonsLayout->setSpacing(20);
		widget->setLayout(buttonsLayout);
		widget->setPalette(QPalette(QColor(255,255,255)));
		widget->setAutoFillBackground (true);

		QScrollArea * scrollArea = new QScrollArea;
		scrollArea->setWidget(widget);
		scrollArea->setPalette(QPalette(QColor(255,255,255)));
		scrollArea->setAutoFillBackground (true);
		widgetsToUpdate << scrollArea;

		toolBox->addItem(scrollArea,tr("Recent Items"));

		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(toolBox);
		layout->setContentsMargins(0,0,0,0);
		layout->setSpacing(0);

		toolBox->setCurrentIndex(2);

		connect(&nodesButtonGroup,SIGNAL(buttonPressed(int)),this,SLOT(nodeButtonPressed(int)));
		connect(&connectionsButtonGroup,SIGNAL(buttonPressed(int)),this,SLOT(connectionButtonPressed(int)));

		setLayout(layout);
	}

	void NodesTreeContainer::makeTabWidget()
	{
		QStringList tabGroups;

		for (int i=0; i < tabGroupButtons.size(); ++i)
			tabGroups << tabGroupButtons[i].first;

		QList<QWidget*> tabs;

		for (int i=0; i < tabGroups.size(); ++i)
		{
			QHBoxLayout * tempLayout = new QHBoxLayout;

			QList<QToolButton*> buttons = tabGroupButtons[i].second;
			for (int j=0; j < buttons.size(); ++j)
			{
				tempLayout->addWidget(buttons[j],0,Qt::AlignTop);
				buttons[j]->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
			}

			QWidget * widget = new QWidget;

            tempLayout->setContentsMargins(5,8,5,5);
			tempLayout->setSpacing(12);

			widget->setLayout(tempLayout);
			widget->setPalette(QPalette(QColor(255,255,255)));
			widget->setAutoFillBackground (true);

			QScrollArea * scrollArea = new QScrollArea;
			scrollArea->setWidget(widget);
			scrollArea->setPalette(QPalette(QColor(255,255,255)));
			scrollArea->setAutoFillBackground (true);

			widgetsToUpdate << scrollArea;
			tabs << scrollArea;
		}

		for (int i=0; i < tabWidget->count(); ++i)
		{
			widgetsToUpdate.removeAll(tabWidget->widget(i));
			delete tabWidget->widget(i);
		}
		tabWidget->clear();

		for (int i=0; i < tabGroups.size(); ++i)
		{
			tabWidget->addTab(tabs[i],tabGroups[i]);
		}
	}

	void NodesTreeContainer::addNewButton(const QList<QToolButton*>& buttons,const QString& group)
	{
		if (!tabWidget) return;
		int i = 0;
		for (i=0; i < tabGroupButtons.size(); ++i)
			if (group.toLower() == tabGroupButtons[i].first.toLower())
			{
				break;
			}
		if (i < tabGroupButtons.size())
			tabGroupButtons[i].second << buttons;
		else
			tabGroupButtons << QPair< QString,QList<QToolButton*> >(group,buttons);
		makeTabWidget();

		if (i < tabWidget->count()) tabWidget->setCurrentIndex(i);
	}

	void NodesTreeContainer::setUpTabView()
	{
		QList< QPair< QString, QStringList> > tabGroups;

		tabWidget = new QTabWidget;
		tabWidget->setWindowTitle(tr("Parts and Connections"));

		tabGroups	<< QPair<QString, QStringList>(
													tr("Species"),
													QStringList() << "Species" << "Empty")

					<< QPair<QString, QStringList>(
													tr("Parts"),
													QStringList() << "Part")

					<< QPair<QString, QStringList>(
													tr("Compartments"),
													QStringList() << "Compartment")

					<< QPair<QString, QStringList>(
													tr("Modules"),
													QStringList() << "Module")

					<< QPair<QString, QStringList>(
													tr("Reactions"),
													QStringList() << "Biochemical" << "Transcription")

					<< QPair<QString, QStringList>(
													tr("Regulations"),
													QStringList() << "Binding" << "Elongation" << "Modifiers");

		numNodeTabs = 4;

		QCoreApplication::setOrganizationName("TinkerCell");
		QCoreApplication::setOrganizationDomain("www.tinkercell.com");
		QCoreApplication::setApplicationName("TinkerCell");
		QSettings settings("TinkerCell", "TinkerCell");
		settings.beginGroup("NodesTreeContainer");
		numNodeTabs = settings.value(tr("numNodeTabs"),numNodeTabs).toInt();
		QStringList savedTabNames = settings.value(tr("familyTabNames"),QStringList()).toStringList();
		QStringList savedTabs = settings.value(tr("familyTabs"),QStringList()).toStringList();

		if (savedTabNames.size()  == savedTabs.size()  && savedTabs.size() > numNodeTabs)
		{
			tabGroups.clear();
			for (int i=0; i < savedTabs.size(); ++i)
			{
				QString s = savedTabs[i].trimmed();
				tabGroups << QPair<QString,QStringList>( savedTabNames[i], s.split(tr(",")) );
			}
		}
		settings.endGroup();

		QList<QToolButton*> usedButtons;

		QStringList allFamilyNames;

		QGridLayout * tempLayout = 0;

		for (int i=0; i < tabGroups.size(); ++i)
		{
			bool found = false;
			for (int j=0; j < tabGroupButtons.size(); ++j)
				if (tabGroupButtons[j].first == tabGroups[i].first)
				{
					found = true;
					break;
				}
			if (!found)
				tabGroupButtons << QPair< QString,QList<QToolButton*> >(tabGroups[i].first,QList<QToolButton*>());
		}

		if (nodesTree)
		{
			connect(nodesTree,SIGNAL(nodeSelected(NodeFamily*)),this,SLOT(nodeSelectedSlot(NodeFamily*)));
			connect(nodesTree,SIGNAL(addNewButtonSignal(const QList<QToolButton*>&,const QString&)),SLOT(addNewButton(const QList<QToolButton*>&,const QString&)));

			QList<NodeFamily*> allFamilies = nodesTree->nodeFamilies.values();
			QList<ItemFamily*> rootFamilies;
			QList<NodeFamily*> families;

			for (int i=0; i < allFamilies.size(); ++i)
				if (allFamilies[i] && !allFamilies[i]->parent())
				{
					rootFamilies << allFamilies[i];
				}

			for (int i=0; i < rootFamilies.size(); ++i)
			{
				QList<ItemFamily*> children = rootFamilies[i]->children();
				if (children.isEmpty())
				{
					if (!families.contains(NodeFamily::asNode(rootFamilies[i])))
						families << NodeFamily::asNode(rootFamilies[i]);
				}
				else
				{
					if (!families.contains(NodeFamily::asNode(rootFamilies[i])) && rootFamilies[i]->parent() && children.size() < 5)
						families << NodeFamily::asNode(rootFamilies[i]);
					rootFamilies << children;
				}
			}

			for (int i=0; i < families.size(); ++i)
			{
				if (!families[i] || !nodesTree->treeButtons.contains(families[i]->name)) continue;

				nodes << families[i];
				allFamilyNames << families[i]->name;

				for (int j=0; j < tabGroups.size(); ++j)
				{
					bool isA = false;

					if (j == 0 && families[i]->name.toLower() == tr("node"))
						isA = true;

					if (j == (tabGroups.size()-1))
						isA = true;

					QString tabName = tabGroups[j].first;
					if  (!isA)
					{
						for (int k=0; k < tabGroups[j].second.size(); ++k)
						{
							if (families[i]->isA(tabGroups[j].second[k]))
							{
								isA = true;
								break;
							}
						}
					}

					if (isA)
					{
						QList<QToolButton*> buttons = nodesTree->treeButtons.values(families[i]->name);
						if (buttons.size()>0 && buttons[0] && !usedButtons.contains(buttons[0]))
						{
							usedButtons << buttons[0];

							if (!tabName.isEmpty())
							{
								bool found = false;
								for (int j=0; j < tabGroupButtons.size(); ++j)
									if (tabGroupButtons[j].first == tabName)
									{
										found = true;
										tabGroupButtons[j].second << buttons[0];
									}

								if (!found)
								{
									QList<QToolButton*> tempList;
									tempList << buttons[0];
									tabGroupButtons << QPair< QString,QList<QToolButton*> >(tabName,tempList);
								}
							}
						}
					}
				}
			}
		}

		if (connectionsTree)
		{
			connect(connectionsTree,SIGNAL(connectionSelected(ConnectionFamily*)),this,SLOT(connectionSelectedSlot(ConnectionFamily*)));

			QList<ConnectionFamily*> allFamilies = connectionsTree->connectionFamilies.values();
			QList<ItemFamily*> rootFamilies;
			QList<ConnectionFamily*> families;

			for (int i=0; i < allFamilies.size(); ++i)
				if (allFamilies[i] && !allFamilies[i]->parent())
				{
					rootFamilies << allFamilies[i];
				}

			for (int i=0; i < rootFamilies.size(); ++i)
			{
				QList<ItemFamily*> children = rootFamilies[i]->children();
				if (children.isEmpty())
				{
					if (!families.contains(ConnectionFamily::asConnection(rootFamilies[i])))
						families << ConnectionFamily::asConnection(rootFamilies[i]);
				}
				else
				{
					if (!families.contains(ConnectionFamily::asConnection(rootFamilies[i])) && rootFamilies[i]->parent() && children.size() < 5)
						families << ConnectionFamily::asConnection(rootFamilies[i]);
					rootFamilies << children;
				}
			}

			for (int i=0; i < families.size(); ++i)
			{
				if (!families[i] || !connectionsTree->treeButtons.contains(families[i]->name)) continue;

				connections << families[i];
				allFamilyNames << families[i]->name;

				for (int j=0; j < tabGroups.size(); ++j)
				{
					bool isA = false;

					if (j == numNodeTabs && families[i]->name.toLower() == tr("connection"))
						isA = true;

					if (j == (tabGroups.size()-1))
						isA = true;

					QString tabName = tabGroups[j].first;
					if (!isA)
					{
						for (int k=0; k < tabGroups[j].second.size(); ++k)
						{
							if (families[i]->isA(tabGroups[j].second[k]))
							{
								isA = true;
								break;
							}
						}
					}

					if (isA)
					{
						QList<QToolButton*> buttons = connectionsTree->treeButtons.values(families[i]->name);
						if (buttons.size()>0 && buttons[0] && !usedButtons.contains(buttons[0]))
						{
							usedButtons << buttons[0];
							if (!tabName.isEmpty())
							{
								bool found = false;
								for (int j=0; j < tabGroupButtons.size(); ++j)
									if (tabGroupButtons[j].first == tabName)
									{
										found = true;
										tabGroupButtons[j].second << buttons[0];
									}

								if (!found)
								{
									QList<QToolButton*> tempList;
									tempList << buttons[0];
									tabGroupButtons << QPair< QString,QList<QToolButton*> >(tabName,tempList);
								}
							}
						}
					}
				}
			}
		}

		if (initialValuesComboBox)
			initialValuesComboBox->addItems(allFamilyNames);

		makeTabWidget();

		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(tabWidget);
		layout->setContentsMargins(0,0,0,0);
		layout->setSpacing(0);
		setLayout(layout);
	}

	void NodesTreeContainer::initialValueComboBoxChanged(const QString& s)
	{
		if (!initialValuesTable) return;

		ItemFamily * family = 0;
		for (int i=0; i < nodes.size(); ++i)
			if (nodes[i] && nodes[i]->name ==s)
			{
				family = nodes[i];
				break;
			}
		if (!family)
			for (int i=0; i < connections.size(); ++i)
				if (connections[i] && connections[i]->name ==s)
				{
					family = connections[i];
					break;
				}

		initialValuesTable->clearContents();
		if (!family) return;

		QStringList rowLabels = family->numericalAttributes.keys();
		rowLabels << family->textAttributes.keys();

		initialValuesTable->setVerticalHeaderLabels(rowLabels);

		QList<qreal> numbers = family->numericalAttributes.values();
		QStringList values;
		for (int i=0; i < numbers.size(); ++i)
			values << QString::number(numbers[i]);
		values << family->textAttributes.values();

		initialValuesTable->setRowCount(rowLabels.size());

		for (int i=0; i < values.size(); ++i)
			initialValuesTable->setItem(i,0,new QTableWidgetItem(values[i]));
	}

	void NodesTreeContainer::initialValuesChanged()
	{
		if (!initialValuesTable || !initialValuesComboBox) return;

		QString s = initialValuesComboBox->currentText();

		ItemFamily * family = 0;
		for (int i=0; i < nodes.size(); ++i)
			if (nodes[i] && nodes[i]->name == s)
			{
				family = nodes[i];
				break;
			}
		if (!family)
			for (int i=0; i < connections.size(); ++i)
				if (connections[i] && connections[i]->name ==s)
				{
					family = connections[i];
					break;
				}
		if (!family) return;

		int n1 = family->numericalAttributes.size();
		int n2 = family->textAttributes.size();

		if (initialValuesTable->rowCount() != (n1+n2)) return;

		double d;
		bool ok;
		QString s2;

		for (int i=0; i < (n1+n2); ++i)
			if (initialValuesTable->verticalHeaderItem(i) && initialValuesTable->item(i,0))
			{
				QString s = initialValuesTable->verticalHeaderItem(i)->text();
				s2 = initialValuesTable->item(i,0)->text();
				if (family->numericalAttributes.contains(s))
				{
					d = s2.toDouble(&ok);
					if (ok)
						family->numericalAttributes[s] = d;
				}
				else
				if (family->textAttributes.contains(s))
				{
					family->textAttributes[s] = s2;
				}
			}
	}
}


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

     Tinkercell::NodesTree * nodesTree = new Tinkercell::NodesTree;
     main->addTool(nodesTree);

     Tinkercell::ConnectionsTree * connectionsTree = new Tinkercell::ConnectionsTree;
	 main->addTool(connectionsTree);

	 Tinkercell::NodesTreeContainer * treeContainer = new Tinkercell::NodesTreeContainer(nodesTree,connectionsTree);
	 main->addTool(treeContainer);
}

