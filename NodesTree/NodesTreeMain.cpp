/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Function that loads dll into main window

****************************************************************************/
#include <QInputDialog>
#include <QTabWidget>
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
		nodesTree(nodesTree),
		connectionsTree(connectionsTree)
	{
		arrowButton.setToolTip(QObject::tr("Cursor"));
        arrowButton.setPalette(QPalette(QColor(255,255,255)));
        arrowButton.setAutoFillBackground (true);
        arrowButton.setIcon(QIcon(QObject::tr(":/images/arrow.png")));
        arrowButton.setIconSize(QSize(20,20));
        //arrowButton.setPopupMode(QToolButton::MenuButtonPopup);
		
		if (layoutMode == TabView)
			setUpTabView();
		else
			setUpTreeView();
		
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
			return true;
		}
		return false;
	}
	
	void NodesTreeContainer::nodeButtonPressed ( int i )
	{
		if (nodes.size() > i)
		{
			emit sendEscapeSignal(this);
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
		if (layoutMode != TreeView) return;

		QCoreApplication::setOrganizationName("TinkerCell");
		QCoreApplication::setOrganizationDomain("www.tinkercell.com");
		QCoreApplication::setApplicationName("TinkerCell");

		QSettings settings("TinkerCell", "TinkerCell");

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

	QSize NodesTreeContainer::sizeHint() const
	{
		if (layoutMode == TreeView)
			return QSize(140, 600);
		else
			return QSize(600,120);
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
		
	void NodesTreeContainer::setUpTabView()
	{
		QList< QPair< QString, QStringList> > tabGroups;
		
		tabGroups	<< QPair<QString, QStringList>( 	
													tr("Species"),
													QStringList() << "Species")
		
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
		
		QTabWidget * tabWidget = new QTabWidget;
		tabWidget->setWindowTitle(tr("Parts and Connections"));
		
		QList<QGridLayout*> tabLayouts;
		QList<int> index;
		QList<QToolButton*> usedButtons;
		
		for (int i=0; i < tabGroups.size(); ++i)
		{
			tabLayouts << new QGridLayout;
			index << 0;
		}
		
		//tabLayouts[0]->addWidget(&arrowButton,0,0,Qt::AlignCenter);
		
		if (nodesTree)
		{
			QList<NodeFamily*> families = nodesTree->nodeFamilies.values();
			
			for (int i=0; i < families.size(); ++i)
			{
				if (!families[i] || !nodesTree->treeButtons.contains(families[i]->name)) continue;
				
				for (int j=0; j < tabGroups.size(); ++j)
				{
					bool isA = false;
					
					for (int k=0; k < tabGroups[j].second.size(); ++k)
					{
						if (families[i]->isA(tabGroups[j].second[k]))
						{
							isA = true;
							break;
						}
					}
					
					if (isA || j == (tabGroups.size()-1))
					{
						QGridLayout * buttonsLayout = tabLayouts[j];
						QList<QToolButton*> buttons = nodesTree->treeButtons.values(families[i]->name);
						if (buttons.size()>0 && !usedButtons.contains(buttons[0]))
						{
							usedButtons << buttons[0];
							buttonsLayout->addWidget(buttons[0],0, index[j]++ ,Qt::AlignCenter);
							buttons[0]->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
						}
					}
				}
			}
		}
		
		if (connectionsTree)
		{
			QList<ConnectionFamily*> families = connectionsTree->connectionFamilies.values();
			
			for (int i=0; i < families.size(); ++i)
			{
				if (!families[i] || !connectionsTree->treeButtons.contains(families[i]->name)) continue;
				
				for (int j=0; j < tabGroups.size(); ++j)
				{
					bool isA = false;
					
					for (int k=0; k < tabGroups[j].second.size(); ++k)
					{
						if (families[i]->isA(tabGroups[j].second[k]))
						{
							isA = true;
							break;
						}
					}
					
					if (isA || j == (tabGroups.size()-1))
					{
						QGridLayout * buttonsLayout = tabLayouts[j];
						QList<QToolButton*> buttons = connectionsTree->treeButtons.values(families[i]->name);		
						if (buttons.size()>0 && !usedButtons.contains(buttons[0]))
						{
							usedButtons << buttons[0];
							buttonsLayout->addWidget(buttons[0],0, index[j]++ ,Qt::AlignCenter);
							buttons[0]->setToolButtonStyle ( Qt::ToolButtonTextUnderIcon );
						}
					}
				}
			}
		}
		
		for (int i=0; i < tabGroups.size() && i < tabLayouts.size(); ++i)
		{
			QGridLayout * buttonsLayout = tabLayouts[i];
			
			if (!buttonsLayout) continue;
			
			QWidget * widget = new QWidget;			
			buttonsLayout->setContentsMargins(5,5,5,5);
			buttonsLayout->setSpacing(20);	
			widget->setLayout(buttonsLayout);
			widget->setPalette(QPalette(QColor(255,255,255)));
			widget->setAutoFillBackground (true);
			
			QScrollArea * scrollArea = new QScrollArea;
			scrollArea->setWidget(widget);
			scrollArea->setPalette(QPalette(QColor(255,255,255)));
			scrollArea->setAutoFillBackground (true);
			
			tabWidget->addTab(scrollArea,tabGroups[i].first);
		}
		
		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(tabWidget);
		layout->setContentsMargins(0,0,0,0);
		layout->setSpacing(0);
		setLayout(layout);
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

