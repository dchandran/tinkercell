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
#include <QScrollArea>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "NodesTree.h"
#include "ConnectionsTree.h"
#include "ConsoleWindow.h"
#include "ItemFamily.h"
#include "NetworkHandle.h"
#include "CatalogWidget.h"

namespace Tinkercell
{
	CatalogWidget::MODE CatalogWidget::layoutMode = CatalogWidget::TabView;

	CatalogWidget::CatalogWidget(NodesTree * nodesTree, ConnectionsTree * connectionsTree, QWidget * parent) :
		Tool(tr("Parts and Connections Catalog"),tr("Parts Catalog"),parent),
		nodesButtonGroup(this),
		connectionsButtonGroup(this),
		toolBox(0),
		tabWidget(0),
		nodesTree(nodesTree),
		connectionsTree(connectionsTree),
		selectFamilyWidget(0)
	{
		QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);

		settings.beginGroup("CatalogWidget");
		CatalogWidget::layoutMode = (CatalogWidget::MODE)(settings.value(tr("Mode"),(int)layoutMode).toInt());
		settings.endGroup();

		arrowButton.setToolTip(QObject::tr("Cursor"));
        arrowButton.setPalette(QPalette(QColor(255,255,255)));
        arrowButton.setAutoFillBackground (true);
        arrowButton.setIcon(QIcon(QObject::tr(":/images/arrow.png")));
        arrowButton.setIconSize(QSize(20,20));
	}

	void CatalogWidget::setTreeMode(bool b)
	{
		if (b)
			CatalogWidget::layoutMode = CatalogWidget::TreeView;
		else
			CatalogWidget::layoutMode = CatalogWidget::TabView;
		QMessageBox::information(this,tr("Parts Layout"),tr("The change in display will take effect the next time TinkerCell starts"));
	}

	void CatalogWidget::otherButtonPressed(const QString& text, const QPixmap& pixmap)
	{
		emit sendEscapeSignal(this);

		QCursor cursor(pixmap);
		mainWindow->setCursor(cursor);
		
		for (int i=0; i < widgetsToUpdate.size(); ++i)
			if (widgetsToUpdate[i])
				widgetsToUpdate[i]->setCursor(cursor);

		emit buttonPressed(text);
	}

	void CatalogWidget::escapeSignalSlot(const QWidget* widget)
	{
		mainWindow->setCursor(Qt::ArrowCursor);

		for (int i=0; i < widgetsToUpdate.size(); ++i)
			if (widgetsToUpdate[i])
				widgetsToUpdate[i]->setCursor(Qt::ArrowCursor);
	}
	
	bool CatalogWidget::setMainWindow(MainWindow * main)
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
				setUpTreeView();
				mainWindow->addToolWindow(this,MainWindow::DockWidget,Qt::LeftDockWidgetArea,Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
				QAction * setNumRows = mainWindow->optionsMenu->addAction(QIcon(tr(":/images/up.png")), tr("Number of recent items"));
				connect (setNumRows, SIGNAL(triggered()),this,SLOT(setNumberOfRecentItems()));
			}
			else
			{
				setUpTabView();
				mainWindow->addToolWindow(this,MainWindow::DockWidget,Qt::TopDockWidgetArea,Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
			}

			if (mainWindow->optionsMenu)
			{				
				QAction * treeViewAction = mainWindow->optionsMenu->addAction(tr("Tree-view catalog"));
				treeViewAction->setCheckable(true);
				treeViewAction->setChecked(CatalogWidget::layoutMode == CatalogWidget::TreeView);
				connect(treeViewAction,SIGNAL(toggled(bool)),this,SLOT(setTreeMode(bool)));
				if (layoutMode == TabView)
				{
					QAction * familyShowButton = mainWindow->optionsMenu->addAction(tr("Show/hide catalog items"));
					connect(familyShowButton,SIGNAL(triggered()),this,SLOT(selectFamiliesToShow()));
				}
			}

			return true;
		}
		return false;
	}

	void CatalogWidget::nodeButtonPressed ( int n )
	{
		if (nodes.size() > n && nodes[n])
		{
			emit nodeSelected(nodes[n]);
		}
	}

	void CatalogWidget::connectionButtonPressed ( int n )
	{
		if (connections.size() > n && connections[n])
		{
			emit connectionSelected(connections[n]);
		}
	}

	void CatalogWidget::nodeSelectedSlot(NodeFamily* nodeFamily)
	{
		if (!nodeFamily || nodes.isEmpty() || !currentScene()) return;

		emit buttonPressed(nodeFamily->name());

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

		QCursor cursor(nodeFamily->pixmap.scaled(w,h));

		mainWindow->setCursor(cursor);
		
		for (int i=0; i < widgetsToUpdate.size(); ++i)
			if (widgetsToUpdate[i])
				widgetsToUpdate[i]->setCursor(cursor);
		if (nodes.contains(nodeFamily)) return;

		QList<QAbstractButton*> buttons = nodesButtonGroup.buttons();
		if (nodes.size() != buttons.size()) return;

		for (int i=nodes.size()-1; i > 0; --i)
			if (buttons[i] && nodes[i] && nodes[i-1])
			{
				nodes[i] = nodes[i-1];
				buttons[i]->setIcon(QIcon(nodes[i]->pixmap));
				buttons[i]->setToolTip(nodes[i]->name());
				buttons[i]->setText(nodes[i]->name());
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
		buttons[0]->setToolTip(nodes[0]->name());
		buttons[0]->setText(nodes[0]->name());
		buttons[0]->setIconSize(QSize(w,h));
	}

    void CatalogWidget::connectionSelectedSlot(ConnectionFamily* family)
	{
		if (!family || connections.isEmpty() || !currentScene()) return;

		emit buttonPressed(family->name());
		
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

		QCursor cursor(family->pixmap.scaled(w,h));

		mainWindow->setCursor(cursor);
		
		for (int i=0; i < widgetsToUpdate.size(); ++i)
			if (widgetsToUpdate[i])
				widgetsToUpdate[i]->setCursor(cursor);
		if (connections.contains(family)) return;

		QList<QAbstractButton*> buttons = connectionsButtonGroup.buttons();
		if (connections.size() != buttons.size()) return;

		for (int i=connections.size()-1; i > 0; --i)
			if (buttons[i] && connections[i] && connections[i-1])
			{
				connections[i] = connections[i-1];
				buttons[i]->setIcon(QIcon(connections[i]->pixmap));
				buttons[i]->setToolTip(connections[i]->name());
				buttons[i]->setText(connections[i]->name());
			}

		connections[0] = family;
		buttons[0]->setIcon(QIcon(connections[0]->pixmap));
		buttons[0]->setToolTip(connections[0]->name());
		buttons[0]->setText(connections[0]->name());
		buttons[0]->setIconSize(QSize(w, h));
	}

	CatalogWidget::~CatalogWidget()
	{
		QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);
		settings.beginGroup("CatalogWidget");
		settings.setValue(tr("Mode"),(int)(CatalogWidget::layoutMode));

		if (layoutMode == TreeView)
		{
			settings.beginGroup("LastSelectedNodes");

			for (int i=0; i < nodes.size(); ++i)
			{
			   if (nodes[i])
				settings.setValue(QString::number(i),nodes[i]->name());
			}
			settings.endGroup();

			settings.beginGroup("LastSelectedConnections");

			for (int i=0; i < connections.size(); ++i)
			{
			   if (connections[i])
				settings.setValue(QString::number(i),connections[i]->name());
			}

			settings.endGroup();
		}
		else
		{
			if (tabWidget)
				settings.setValue(tr("currentIndex"),tabWidget->currentIndex());
			settings.setValue(tr("familiesInCatalog"),familiesInCatalog);
		}

		settings.endGroup();
	}

	QSize CatalogWidget::sizeHint() const
	{
		if (layoutMode == TreeView)
			return QSize(140, 600);
		else
			return QSize(600,80);
	}

	void CatalogWidget::keyPressEvent ( QKeyEvent * event )
	{
		emit keyPressed(event->key(),event->modifiers());
          if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Space)
               emit sendEscapeSignal(this);
	}

	void CatalogWidget::contextMenuEvent(QContextMenuEvent *)
	{		
		//emit sendEscapeSignal(this);
		selectFamiliesToShow();
	}
	
	void CatalogWidget::selectFamiliesToShow()
	{
		QStringList allNames;

		if (nodesTree)
			allNames << nodesTree->nodeFamilies.keys();
		if (connectionsTree)
			allNames << connectionsTree->connectionFamilies.keys();

		if (allNames.isEmpty()) return;

		allNames.sort();

		if (!selectFamilyWidget)
		{
			selectFamilyWidget = new QDialog(this);
			QTableWidget * table = new QTableWidget;
			table->setColumnCount(2);
			table->setColumnWidth(0,50);
			table->setHorizontalHeaderLabels( QStringList() << tr("show") << tr("family") );
			table->setRowCount(allNames.size());
			table->setWindowFlags(Qt::Window);
			table->setWindowTitle(tr("Show/hide catalog items"));

			for (int i=0; i < allNames.size(); ++i)
			{
				QCheckBox * checkbox = new QCheckBox;
				ItemFamily * family = 0;
				if (nodesTree && nodesTree->nodeFamilies.contains(allNames[i]))
					family = nodesTree->nodeFamilies[ allNames[i] ];
				else
					if (connectionsTree && connectionsTree->connectionFamilies.contains(allNames[i]))
					family = connectionsTree->connectionFamilies[ allNames[i] ];
				checkbox->setChecked(includeFamilyInCatalog(family));
				selectFamilyCheckBoxes << checkbox;
				table->setCellWidget(i,0,checkbox);
				QToolButton * tempButton = new QToolButton;
				if (family)
				{
					tempButton->setIcon(QIcon(family->pixmap));
					tempButton->setToolTip(family->description);
				}
				connect(tempButton,SIGNAL(pressed()),checkbox,SLOT(toggle()));
				tempButton->setText(allNames[i]);
				tempButton->setToolButtonStyle ( Qt::ToolButtonTextBesideIcon );
				table->setCellWidget(i,1,tempButton);
			}
			
			QHBoxLayout * hlayout = new QHBoxLayout;
			QPushButton * okButton = new QPushButton("&OK");
			connect(okButton,SIGNAL(pressed()),selectFamilyWidget,SLOT(accept()));
			QPushButton * cancelButton = new QPushButton("&Cancel");
			connect(cancelButton,SIGNAL(pressed()),selectFamilyWidget,SLOT(reject()));
			hlayout->addStretch(1);
			hlayout->addWidget(okButton);
			hlayout->addWidget(cancelButton);
			hlayout->addStretch(1);
			
			QVBoxLayout * vlayout = new QVBoxLayout;
			vlayout->addWidget(table,1);
			vlayout->addLayout(hlayout,0);
			selectFamilyWidget->setLayout(vlayout);
		}
		
		//run dialog
		selectFamilyWidget->exec();
		
		if (selectFamilyWidget->result() != QDialog::Accepted) 
			return;

		//if OK, then update catalog	
		QStringList showlist, hidelist;
		for (int i=0; i < allNames.size() && selectFamilyCheckBoxes.size(); ++i)
		{
			if (selectFamilyCheckBoxes[i]->isChecked())
				showlist << allNames[i];
			else
				hidelist << allNames[i];
		}
		showButtons(showlist);
		hideButtons(hidelist);

		QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);
		settings.beginGroup("CatalogWidget");
		settings.setValue(tr("familiesInCatalog"),familiesInCatalog);
		settings.endGroup();
	}

	void CatalogWidget::setNumberOfRecentItems()
	{
		if (layoutMode != TreeView) return;

		QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);

		settings.beginGroup("LastSelectedNodes");

		int n = settings.value(tr("numRows"),5).toInt();
#if QT_VERSION > 0x045000
		n = QInputDialog::getInt(this,tr("Recent items"), tr("Number of recent items (will take event when TinkerCell starts)"), 2*n, 2, 20, 2);
#elseif
	n = QInputDialog::getInteger(this,tr("Recent items"), tr("Number of recent items (will take event when TinkerCell starts)"), 2*n, 2, 20, 2);
#endif
		n = n/2;

		settings.setValue("numRows",n);
		settings.endGroup();

		settings.beginGroup("LastSelectedConnections");
		settings.setValue("numRows",n);
		settings.endGroup();
	}

	void CatalogWidget::setUpTreeView()
	{
		toolBox = new QToolBox;
		QGridLayout * buttonsLayout = new QGridLayout;
		buttonsLayout->addWidget(&arrowButton,0,0,Qt::AlignCenter);

		QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);

		int n = 5;
		QStringList allFamilyNames;

		settings.beginGroup("CatalogWidget");

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
					allFamilyNames << family->name();

					QToolButton * button = new QToolButton;
					button->setIcon(QIcon(family->pixmap));
					button->setText(family->name());

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

					button->setToolTip(family->name());
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
					allFamilyNames << family->name();

					QToolButton * button = new QToolButton;
					button->setIcon(QIcon(family->pixmap));
					button->setText(family->name());

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

					button->setToolTip(family->name());
					button->setPalette(QPalette(QColor(255,255,255)));
					button->setAutoFillBackground (true);
					buttonsLayout->addWidget(button,i+1,1,Qt::AlignCenter);
					connectionsButtonGroup.addButton(button,i);
				}
			}
			settings.endGroup();
		}
		settings.endGroup();

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

	void CatalogWidget::makeTabWidget()
	{
		if (!tabWidget || !connectionsTree || !nodesTree) return;
		
		int k = tabWidget->currentIndex();

		QStringList tabGroups;

		for (int i=0; i < tabGroupButtons.size(); ++i)
			tabGroups << tabGroupButtons[i].first;

		QList<QWidget*> tabs;
		
		QList<QToolButton*> buttons = nodesTree->treeButtons.values();
		buttons << connectionsTree->treeButtons.values();
		
		for (int i=0; i < buttons.size(); ++i)
			buttons[i]->setParent(0);

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

            tempLayout->setContentsMargins(5,5,5,5);
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
		
		if (k < tabWidget->count() && k >= 0) tabWidget->setCurrentIndex(k);
	}

	QList<QToolButton*> CatalogWidget::addNewButtons(const QString& group, const QStringList& names, const QList<QIcon>& icons, const QStringList& tooltips)
	{
		QList<QToolButton*> newButtons;
		if (!tabWidget) return newButtons;
		
		int i = 0;

		for (i=0; i < names.size(); ++i)
		{
			FamilyTreeButton * button = new FamilyTreeButton(names[i],this);
			if (icons.size() > i)
				button->setIcon(icons[i]);
			if (tooltips.size() > i)
				button->setToolTip(tooltips[i]);
			connect(button,SIGNAL(pressed(const QString&,const QPixmap&)),this,SLOT(otherButtonPressed(const QString&, const QPixmap&)));
			newButtons << button;
		}
		
		for (i=0; i < tabGroupButtons.size(); ++i)
			if (group.toLower() == tabGroupButtons[i].first.toLower())
			{
				break;
			}

		if (i < tabGroupButtons.size())
			tabGroupButtons[i].second << newButtons;
		else
		{
			tabGroupButtons << QPair< QString,QList<QToolButton*> >(group,newButtons);
			tabGroups << QPair<QString, QStringList>(group,QStringList());
		}

		makeTabWidget();
		
		return newButtons;
	}

	void CatalogWidget::showGroup(const QString& group)
	{
		if (!tabWidget) return;
		int i = 0;
		for (i=0; i < tabGroupButtons.size(); ++i)
			if (group.toLower() == tabGroupButtons[i].first.toLower())
			{
				break;
			}
			
		if (i < tabWidget->count()) tabWidget->setCurrentIndex(i);
	}
	
	bool CatalogWidget::includeFamilyInCatalog(ItemFamily * family)
	{
		if (!family) return false;
		
		bool b;

		if (isFirstTime)
		{
			if (NodeFamily::cast(family))
				b = family->children().isEmpty();
		
			if (ConnectionFamily::cast(family)) //ad-hoc
				b = family->parent() && family->parent()->name() == tr("Biochemical");
	
			if (b && !familiesInCatalog.contains(family->name()))
				familiesInCatalog << family->name();
		}
		else 
		{
			b = familiesInCatalog.contains(family->name());
		}

		return b;
	}
	
	void CatalogWidget::setUpTabView()
	{
		tabGroups.clear();

		tabWidget = new QTabWidget;
		tabWidget->setWindowTitle(tr("Parts and Connections"));

		tabGroups	<< QPair<QString, QStringList>(
													tr("Molecules"),
													QStringList() << "Molecule" << "Empty")

					<< QPair<QString, QStringList>(
													tr("Parts"),
													QStringList() << "Part")

					<< QPair<QString, QStringList>(
													tr("Compartments"),
													QStringList() << "Compartment")

					<< QPair<QString, QStringList>(
													tr("Regulation"),
													QStringList() << "Regulation")
					
					<< QPair<QString, QStringList>(
													tr("Reaction"),
													QStringList() << tr("Biochemical"));

		numNodeTabs = 4;

		QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);
		settings.beginGroup("CatalogWidget");
		familiesInCatalog = settings.value(tr("familiesInCatalog"),QStringList()).toStringList();
		int currentIndex = settings.value(tr("currentIndex"),0).toInt();
		settings.endGroup();
		
		isFirstTime = familiesInCatalog.size() < 10;

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
		QStringList families;
		if (nodesTree)
		{
			connect(nodesTree,SIGNAL(nodeSelected(NodeFamily*)),this,SLOT(nodeSelectedSlot(NodeFamily*)));
			QList<NodeFamily*> allFamilies = nodesTree->nodeFamilies.values();
			QList<ItemFamily*> rootFamilies;

			for (int i=0; i < allFamilies.size(); ++i)
				if (allFamilies[i] && !allFamilies[i]->parent())
				{
					rootFamilies << allFamilies[i];
				}

			for (int i=0; i < rootFamilies.size(); ++i)
			{
				QList<ItemFamily*> children = rootFamilies[i]->children();
				if (includeFamilyInCatalog(rootFamilies[i]))
					families << rootFamilies[i]->name();
				rootFamilies << children;
			}
		}
		if (connectionsTree)
		{
			connect(connectionsTree,SIGNAL(connectionSelected(ConnectionFamily*)),this,SLOT(connectionSelectedSlot(ConnectionFamily*)));

			QList<ConnectionFamily*> allFamilies = connectionsTree->connectionFamilies.values();
			QList<ItemFamily*> rootFamilies;

			for (int i=0; i < allFamilies.size(); ++i)
				if (allFamilies[i] && !allFamilies[i]->parent())					
					rootFamilies << allFamilies[i];

			for (int i=0; i < rootFamilies.size(); ++i)
			{
				QList<ItemFamily*> children = rootFamilies[i]->children();
				if (includeFamilyInCatalog(rootFamilies[i]))
					families << rootFamilies[i]->name();
				rootFamilies << children;
			}
		}

		showButtons(families);
		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(tabWidget);
		layout->setContentsMargins(0,0,0,0);
		layout->setSpacing(0);
		setLayout(layout);		
		isFirstTime = false;
		
		if (currentIndex > -1 && currentIndex < tabWidget->count())
			tabWidget->setCurrentIndex(currentIndex);
	}
		
	void CatalogWidget::showButtons(const QStringList& familyNames)
	{
		if (!tabWidget) return;
				
		bool widgetChanged = false;

		for (int i=0; i < familyNames.size(); ++i)
			if (!familiesInCatalog.contains(familyNames[i]))
				familiesInCatalog << familyNames[i];

		if (nodesTree)
		{
			QList<NodeFamily*> families;
			for (int i=0; i < familyNames.size(); ++i)
				if (nodesTree->nodeFamilies.contains(familyNames[i]))
					families << nodesTree->nodeFamilies.value(familyNames[i]);

			for (int i=0; i < families.size(); ++i)
			{
				if (!families[i] || nodes.contains(families[i])) continue;
				
				nodes << families[i];

				for (int j=0; j < tabGroups.size(); ++j)
				{
					bool isA = false;
					
					QString tabName = tabGroups[j].first;

					if (families[i]->isA(tabName))
						isA = true;
					
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
						QList<QToolButton*> buttons = nodesTree->treeButtons.values(families[i]->name());
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
										widgetChanged = true;
									}

								if (!found)
								{
									QList<QToolButton*> tempList;
									tempList << buttons[0];
									tabGroupButtons << QPair< QString,QList<QToolButton*> >(tabName,tempList);
									widgetChanged = true;
								}
							}
						}
					}
				}
			}
		}

		if (connectionsTree)
		{
			QList<ConnectionFamily*> families;
				for (int i=0; i < familyNames.size(); ++i)
					if (connectionsTree->connectionFamilies.contains(familyNames[i]))
						families << connectionsTree->connectionFamilies.value(familyNames[i]);

			for (int i=0; i < families.size(); ++i)
			{
				if (!families[i] || connections.contains(families[i])) continue;
				
				connections << families[i];

				for (int j=0; j < tabGroups.size(); ++j)
				{
					bool isA = false;

					QString tabName = tabGroups[j].first;
					
					if (families[i]->isA(tabName))
						isA = true;

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
						QList<QToolButton*> buttons = connectionsTree->treeButtons.values(families[i]->name());
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
										widgetChanged = true;
									}

								if (!found)
								{
									QList<QToolButton*> tempList;
									tempList << buttons[0];
									tabGroupButtons << QPair< QString,QList<QToolButton*> >(tabName,tempList);
									widgetChanged = true;
								}
							}
						}
					}
				}
			}
		}

		if (widgetChanged && tabWidget)
		{
			makeTabWidget();
		}
	}

	void CatalogWidget::hideButtons(const QStringList& familyNames)
	{
		if (!tabWidget) return;
				
		bool widgetChanged = false;
		
		for (int i=0; i < familyNames.size(); ++i)
			familiesInCatalog.removeAll(familyNames[i]);

		if (nodesTree)
		{
			QList<NodeFamily*> families;
			for (int i=0; i < familyNames.size(); ++i)
				if (nodesTree->nodeFamilies.contains(familyNames[i]))
					families << nodesTree->nodeFamilies.value(familyNames[i]);

			for (int i=0; i < families.size(); ++i)
			{
				if (!families[i] || !nodes.contains(families[i])) continue;
				
				nodes.removeAll(families[i]);

				for (int j=0; j < tabGroups.size(); ++j)
				{					
					bool isA = false;
					
					QString tabName = tabGroups[j].first;
					if (families[i]->isA(tabName))
						isA = true;
					
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
						QList<QToolButton*> buttons = nodesTree->treeButtons.values(families[i]->name());
						if (buttons[0] && usedButtons.contains(buttons[0]))
						{
							usedButtons.removeAll(buttons[0]);
							if (!tabName.isEmpty())
							{
								bool found = false;
								for (int j=0; j < tabGroupButtons.size(); ++j)
									if (tabGroupButtons[j].first == tabName)
									{
										found = true;
										tabGroupButtons[j].second.removeAll(buttons[0]);
										widgetChanged = true;
									}
							}
						}
					}
				}
			}
		}

		if (connectionsTree)
		{
			QList<ConnectionFamily*> families;
				for (int i=0; i < familyNames.size(); ++i)
					if (connectionsTree->connectionFamilies.contains(familyNames[i]))
						families << connectionsTree->connectionFamilies.value(familyNames[i]);

			for (int i=0; i < families.size(); ++i)
			{
				if (!families[i] || !connections.contains(families[i])) continue;

				connections.removeAll(families[i]);

				for (int j=0; j < tabGroups.size(); ++j)
				{
					bool isA = false;

					QString tabName = tabGroups[j].first;
					if (families[i]->isA(tabName))
						isA = true;

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
						QList<QToolButton*> buttons = connectionsTree->treeButtons.values(families[i]->name());
						if (buttons[0] && usedButtons.contains(buttons[0]))
						{
							usedButtons.removeAll(buttons[0]);
							if (!tabName.isEmpty())
							{
								bool found = false;
								for (int j=0; j < tabGroupButtons.size(); ++j)
									if (tabGroupButtons[j].first == tabName)
									{
										found = true;
										tabGroupButtons[j].second.removeAll(buttons[0]);
										widgetChanged = true;
									}
							}
						}
					}
				}
			}
		}

		if (widgetChanged && tabWidget)
		{
			makeTabWidget();
		}
	}

}


extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

     Tinkercell::NodesTree * nodesTree = new Tinkercell::NodesTree;
     main->addTool(nodesTree);

     Tinkercell::ConnectionsTree * connectionsTree = new Tinkercell::ConnectionsTree;
	 main->addTool(connectionsTree);

	 Tinkercell::CatalogWidget * widget = new Tinkercell::CatalogWidget(nodesTree,connectionsTree);
	 main->addTool(widget);
}

