/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Function that loads dll into main window

****************************************************************************/

#include "NodesTreeMain.h"

namespace Tinkercell
{
	NodesTreeContainer::NodesTreeContainer(NodesTree * nodesTree, ConnectionsTree * connectionsTree, QWidget * parent) : 
		Tool(tr("Parts and Connections"),parent),
		nodesButtonGroup(this),
		connectionsButtonGroup(this)
	{
		QWidget * widget = new QWidget;		
		QScrollArea * scrollArea = new QScrollArea;
		scrollArea->setWidget(widget);
		
		toolBox = new QToolBox;
		toolBox->addWidget(scrollArea);
		
		toolBox->addItem(scrollArea,tr("Quick list"));
		
		arrowButton.setToolTip(QObject::tr("cursor"));
        arrowButton.setPalette(QPalette(QColor(255,255,255)));
        arrowButton.setAutoFillBackground (true);
        arrowButton.setIcon(QIcon(QObject::tr(":/images/arrow.png")));
        arrowButton.setIconSize(QSize(25, 25));
        arrowButton.setPopupMode(QToolButton::MenuButtonPopup);

        buttonsLayout->addWidget(&arrowItem);
        
		QVBoxLayout * buttonsLayout = new QVBoxLayout;
		ItemFamily * family;
		
		if (nodesTree)
		{
			connect(this,SIGNAL(nodeSelected(NodeFamily*)),nodesTree,SIGNAL(nodeSelected(NodeFamily*)));
			toolBox->addItem(nodesTree,tr("Parts"));
			for (int i=0; nodes.size() < 3 && i < nodesTree->nodeFamilies.size(); ++i)
				if ((family = nodesTree->nodeFamilies[i]))
				{
					nodes << family;
					QToolButton * button = new QToolButton;
					button->setIcon(QIcon(family->pixmap));
					button->setText(family->name);
					button->setIconSize(QSize(40, 20));
					button->setToolTip(family->name);		
					button->setPalette(QPalette(QColor(255,255,255)));
					button->setAutoFillBackground (true);
					buttonsLayout->addWidget(button);
					nodesButtonGroup.addButton(button,i);
				}
		}
		
		if (connectionsTree)
		{			
			connect(this,SIGNAL(connectionSelected(ConnectionFamily*)),connectionsTree,SIGNAL(connectionSelected(ConnectionFamily*)));
			toolBox->addItem(connectionsTree,tr("Connections"));
			for (int i=0; connections.size() < 3 && i < connectionsTree->connectionFamilies.size(); ++i)
				if ((family = connectionsTree->connectionFamilies[i]))
				{
					connections << family;
					QToolButton * button = new QToolButton;
					button->setIcon(QIcon(family->pixmap));
					button->setText(family->name);
					button->setIconSize(QSize(40, 20));
					button->setToolTip(family->name);		
					button->setPalette(QPalette(QColor(255,255,255)));
					button->setAutoFillBackground (true);
					buttonsLayout->addWidget(button);
					connectionsButtonGroup.addButton(button,i);
				}
		}		
		
		widget->setLayout(buttonsLayout);		
		
		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(toolBox);
		layout->setContentsMargins(0,0,0,0);
		layout->setSpacing(0);
		setLayout(layout);
	}
	
	bool NodesTreeContainer::setMainWindow(MainWindow * TinkercellWindow)
	{
		  Tool::setMainWindow(TinkercellWindow);

		  if (mainWindow)
		  {
			   connect(this,SIGNAL(sendEscapeSignal(const QWidget*)),mainWindow,SIGNAL(escapeSignal(const QWidget*)));
			   QDockWidget* dock = mainWindow->addDockingWindow(tr("Parts"),this,Qt::LeftDockWidgetArea,Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
			   dock->setWindowFlags(Qt::Widget);
			   return true;
		  }
		  return false;
	}
	
	void NodesTreeContainer::nodeButtonPressed ( int i )
	{
		if (nodes.size() > i)
		{
			emit 
			emit nodesSelected(nodes[i]);
		}
	}
	
	void NodesTreeContainer::connectionButtonPressed ( int i )
	{
	}

	NodesTreeContainer::~NodesTreeContainer()
	{
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
		return QSize(140, 600);
	}

	void NodesTreeContainer::keyPressed(int, Qt::KeyboardModifiers)
	{
	}

	void NodesTreeContainer::nodeSelectedSlot(NodeFamily* nodeFamily)
	{
		
	}
	
	void NodesTreeContainer::connectionSelectedSlot(ConnectionFamily* family)
	{
	}

	void NodesTreeContainer::keyPressEvent ( QKeyEvent * event )
	{
	}
	
	void contextMenuEvent(QContextMenuEvent * event)
	{
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

