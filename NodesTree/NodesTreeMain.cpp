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
		toolBox = new QToolBox;
		
		arrowButton.setToolTip(QObject::tr("cursor"));
        arrowButton.setPalette(QPalette(QColor(255,255,255)));
        arrowButton.setAutoFillBackground (true);
        arrowButton.setIcon(QIcon(QObject::tr(":/images/arrow.png")));
        arrowButton.setIconSize(QSize(30,30));
        //arrowButton.setPopupMode(QToolButton::MenuButtonPopup);

		QVBoxLayout * buttonsLayout = new QVBoxLayout;
		buttonsLayout->addWidget(&arrowButton);
		
		QCoreApplication::setOrganizationName("TinkerCell");
		QCoreApplication::setOrganizationDomain("www.tinkercell.com");
		QCoreApplication::setApplicationName("TinkerCell");
		QSettings settings("TinkerCell", "TinkerCell");
		
		if (nodesTree)
		{
			NodeFamily * family;
			
			settings.beginGroup("LastSelectedNodes");
			
			connect(this,SIGNAL(nodeSelected(NodeFamily*)),nodesTree,SIGNAL(nodeSelected(NodeFamily*)));
			connect(nodesTree,SIGNAL(nodeSelected(NodeFamily*)),this,SLOT(nodeSelectedSlot(NodeFamily*)));
			connect(this,SIGNAL(keyPressed(int, Qt::KeyboardModifiers)),nodesTree,SIGNAL(keyPressed(int, Qt::KeyboardModifiers)));
			
			toolBox->addItem(nodesTree,tr("Parts"));
			
			QList<QString> keys = nodesTree->nodeFamilies.keys();
			
			for (int i=0; nodes.size() < 5 && i < keys.size(); ++i)
			{
				QString s = settings.value(QString::number(i),keys[i]).toString();
				
				if (nodesTree->nodeFamilies.contains(s) && (family = nodesTree->nodeFamilies[s]))
				{
					nodes << family;
					QToolButton * button = new QToolButton;
					button->setIcon(QIcon(family->pixmap));
					button->setText(family->name);
					button->setIconSize(QSize(30,30));
					button->setToolTip(family->name);		
					button->setPalette(QPalette(QColor(255,255,255)));
					button->setAutoFillBackground (true);
					buttonsLayout->addWidget(button);
					nodesButtonGroup.addButton(button,i);
				}
			}
			settings.endGroup();
		}
		
		if (connectionsTree)
		{
			ConnectionFamily * family;
			
			settings.beginGroup("LastSelectedConnections");
			
			connect(this,SIGNAL(connectionSelected(ConnectionFamily*)),connectionsTree,SIGNAL(connectionSelected(ConnectionFamily*)));
			connect(connectionsTree,SIGNAL(connectionSelected(ConnectionFamily*)),this,SLOT(connectionSelectedSlot(ConnectionFamily*)));
			connect(this,SIGNAL(keyPressed(int, Qt::KeyboardModifiers)),connectionsTree,SIGNAL(keyPressed(int, Qt::KeyboardModifiers)));
			
			toolBox->addItem(connectionsTree,tr("Connections"));
			QList<QString> keys = connectionsTree->connectionFamilies.keys();
			
			for (int i=0; connections.size() < 5 && i < keys.size(); ++i)
			{
				QString s = settings.value(QString::number(i),keys[i]).toString();
				
				if (connectionsTree->connectionFamilies.contains(s) && 
					(family = connectionsTree->connectionFamilies[s]))
				{
					connections << family;
					QToolButton * button = new QToolButton;
					button->setIcon(QIcon(family->pixmap));
					button->setText(family->name);
					button->setIconSize(QSize(30,30));
					button->setToolTip(family->name);
					button->setPalette(QPalette(QColor(255,255,255)));
					button->setAutoFillBackground (true);
					buttonsLayout->addWidget(button);
					connectionsButtonGroup.addButton(button,i);
				}
			}
			settings.endGroup();
		}
		
		QWidget * widget = new QWidget;	
		buttonsLayout->setContentsMargins(50,0,0,0);
		buttonsLayout->setSpacing(20);	
		widget->setLayout(buttonsLayout);
		widget->setPalette(QPalette(QColor(255,255,255)));
		widget->setAutoFillBackground (true);
		
		QScrollArea * scrollArea = new QScrollArea;
		scrollArea->setWidget(widget);
		scrollArea->setPalette(QPalette(QColor(255,255,255)));
		scrollArea->setAutoFillBackground (true);
		
		toolBox->addItem(scrollArea,tr("Quick list"));
		
		QVBoxLayout * layout = new QVBoxLayout;
		layout->addWidget(toolBox);
		layout->setContentsMargins(0,0,0,0);
		layout->setSpacing(0);
		
		toolBox->setCurrentIndex(2);
		
		connect(&nodesButtonGroup,SIGNAL(buttonPressed(int)),this,SLOT(nodeButtonPressed(int)));
		connect(&connectionsButtonGroup,SIGNAL(buttonPressed(int)),this,SLOT(connectionButtonPressed(int)));
		
		setLayout(layout);
	}
	
	bool NodesTreeContainer::setMainWindow(MainWindow * main)
	{
		  Tool::setMainWindow(main);

		  if (mainWindow)
		  {
			   connect(&arrowButton,SIGNAL(pressed()),mainWindow,SLOT(sendEscapeSignal()));
			   connect(this,SIGNAL(sendEscapeSignal(const QWidget*)),mainWindow,SIGNAL(escapeSignal(const QWidget*)));
			   QDockWidget* dock = mainWindow->addDockingWindow(tr("Parts and Connections"),this,Qt::LeftDockWidgetArea,Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
			   dock->setWindowFlags(Qt::Widget);
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
		if (!nodeFamily || nodes.isEmpty()  || nodes.contains(nodeFamily)) return;
		
		QList<QAbstractButton*> buttons = nodesButtonGroup.buttons();
		if (nodes.size() != buttons.size()) return;
		
		for (int i=nodes.size()-1; i > 0; --i)
			if (buttons[i] && nodes[i] && nodes[i-1])
			{
				nodes[i] = nodes[i-1];
				buttons[i]->setIcon(QIcon(nodes[i]->pixmap));
				buttons[i]->setToolTip(nodes[i]->name);
				buttons[i]->setText(nodes[i]->name);					
			}
		
		nodes[0] = nodeFamily;
		buttons[0]->setIcon(QIcon(nodes[0]->pixmap));
		buttons[0]->setToolTip(nodes[0]->name);
		buttons[0]->setText(nodes[0]->name);				
	}
	
    void NodesTreeContainer::connectionSelectedSlot(ConnectionFamily* family)
	{
		if (!family || connections.isEmpty()  || connections.contains(family)) return;
		
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

