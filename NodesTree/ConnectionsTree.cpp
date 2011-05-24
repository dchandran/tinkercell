/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 Tool for displaying the Connections Tree and selecting the nodes on that tree.
 This tool also stores the tree of connection families as a hashtable of <name,family> pairs.

****************************************************************************/
#include "ConsoleWindow.h"
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConnectionsTree.h"
#include "NodesTree.h"
#include "TreeButton.h"
#include "LoadSaveTool.h"
#include "GlobalSettings.h"
#include "Ontology.h"
#include <QDialog>

namespace Tinkercell
{
    ConnectionsTree::~ConnectionsTree()
    {
		networkClosing(0,0);
      /*QList<QToolButton*> buttons = treeButtons.values();
        for (int i=0; i < buttons.size(); ++i)
        	if (buttons[i] && !buttons[i]->parentWidget())
        		delete buttons[i];*/
    }

    QSize ConnectionsTree::sizeHint() const
    {
        return QSize(140, 200);
    }
    
    void ConnectionsTree::readTreeFile(const QString& filename)
    {
        QString appDir = QCoreApplication::applicationDirPath();
		QString homeDir = GlobalSettings::homeDir();
		QSettings settings(GlobalSettings::ORGANIZATIONNAME, GlobalSettings::ORGANIZATIONNAME);
        settings.beginGroup("ConnectionsTree");
        NodesTree::themeDirectory = settings.value("theme",tr("Bio1")).toString();

		QStringList keys;
		if (filename.isEmpty())
			keys = Ontology::readConnections(appDir + tr("/NodesTree/ConnectionsTree.nt"),"ntriples");
		else
			keys = Ontology::readConnections(filename,"ntriples");
		QList<ConnectionFamily*> families;
		QList<QTreeWidgetItem*> parentTreeItems; 
		
		for (int i=0; i < keys.size(); ++i)
		{
			ConnectionFamily * conn = Ontology::connectionFamily(keys[i]);
			if (conn && conn->parents().isEmpty())
			{
				families << conn;
				parentTreeItems << 0;
			}
		}
		
		for (int i=0; i < families.size(); ++i)
		{
			QTreeWidgetItem* treeItem = new QTreeWidgetItem;
			treeItem->setText(0,families[i]->name());
			treeItems.insertMulti(families[i]->name(), treeItem);
			if (parentTreeItems[i])
				parentTreeItems[i]->addChild(treeItem);
			else
				widget().addTopLevelItem(treeItem);
			QList<ConnectionFamily*> children = ConnectionFamily::cast(families[i]->children());
			for (int j=0; j < children.size(); ++j)
			{
				families += children[j];
				parentTreeItems += treeItem;
			}
		}

		for (int i=0; i < keys.size(); ++i)
		{
			ConnectionFamily * family = Ontology::connectionFamily(keys[i]);
			QList<QTreeWidgetItem*> treeItem = treeItems.values(keys[i]);
			if (family && !treeItem.isEmpty())
			{
				QString setting = settings.value(family->name(),QString()).toString();
				if (!setting.isEmpty())
					for (int j=0; j < treeItem.size(); ++j)
						treeItem[j]->setExpanded(setting == tr("expanded"));

				for (int j=0; j < treeItem.size(); ++j)
				{
					QToolButton * button = new FamilyTreeButton(family,this);
					connect(button,SIGNAL(connectionSelected(ConnectionFamily*)),this,SLOT(buttonPressed(ConnectionFamily*)));

					QHBoxLayout * layout = new QHBoxLayout;
					layout->addWidget(button,0,Qt::AlignLeft);
					layout->setContentsMargins(0,0,0,0);
					QWidget * widget = new QWidget;
					widget->setPalette(QPalette(QColor(255,255,255)));
					widget->setAutoFillBackground(true);
					widget->setLayout(layout);

					treeWidget.setItemWidget(treeItem[j],0,widget);
					treeButtons.insertMulti(keys[i],button);
				}
			}
		}
		
		for (int i=0; i < keys.size(); ++i)
		{
			ConnectionFamily * conn = Ontology::connectionFamily(keys[i]);
			if (conn)
				setConnectionGraphics(conn);
		}
	
		settings.endGroup();
    }

    ConnectionsTree::ConnectionsTree(QWidget * parent, const QString& filename) :
            Tool(tr("Connections Tree"),tr("Parts Catalog"),parent)
    {
        setWindowFlags(Qt::Tool);

        QVBoxLayout * layout = new QVBoxLayout;
        treeWidget.setHeaderHidden(true);

        treeWidget.header()->setResizeMode(QHeaderView::Stretch);
        //treeWidget->setHeaderLabels(labels);
        layout->addWidget(&treeWidget);
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
        setLayout(layout);
        readTreeFile(filename);
    }


    void ConnectionsTree::buttonPressed(ConnectionFamily * family)
    {
        if (family)
        {
			emit sendEscapeSignal(this);
            emit connectionSelected(family);
        }
    }

    bool ConnectionsTree::setMainWindow(MainWindow * main)
    {
        Tool::setMainWindow(main);

        if (mainWindow)
        {
			connect(this,SIGNAL(sendEscapeSignal(const QWidget*)),mainWindow,SIGNAL(escapeSignal(const QWidget*)));
			return true;
        }
        return false;
    }

    /*void ConnectionsTree::keyPressEvent ( QKeyEvent * event )
    {
        emit keyPressed(event->key(),event->modifiers());
        if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Space)
            emit sendEscapeSignal(this);

    }*/

    void ConnectionsTree::changeTree()
    {
        QString fileName =
                QFileDialog::getOpenFileName(this, tr("Nodes Tree File"),
                                             tr(""),
                                             tr("XML Files (*.xml)"));
        if (fileName.isEmpty())
            return;

        QSettings settings(GlobalSettings::ORGANIZATIONNAME, GlobalSettings::ORGANIZATIONNAME);

        settings.beginGroup("ConnectionsTree");
        settings.setValue("file", fileName);
        settings.endGroup();

        QMessageBox::information (this, tr("Change tree"), tr("The new connections tree will be active the next time you start TinkerCell"));
    }

    void ConnectionsTree::contextMenuEvent(QContextMenuEvent * event)
    {
        emit sendEscapeSignal(this);
        static QMenu * menu = 0;
        if (!menu)
        {
            menu = new QMenu(this);
            menu->addAction(QIcon(":/images/image.png"),tr("Change connections catalog"),this,SLOT(changeTree()));
        }

        QWidget * widget = this;
        QPoint pos = event->pos();
        while (widget)
        {
            pos += widget->pos();
            widget = widget->parentWidget();
        }
        if (event)
            menu->exec(pos);
    }

    void ConnectionsTree::networkClosing(NetworkHandle * , bool *)
    {
        //save state of the tree
        QSettings settings(GlobalSettings::ORGANIZATIONNAME, GlobalSettings::ORGANIZATIONNAME);

        settings.beginGroup("ConnectionsTree");
    	settings.setValue("theme",NodesTree::themeDirectory);

        QList<QString> keys = Ontology::allConnectionFamilyNames();

        for (int i=0; i < keys.size(); ++i)
        {
            ConnectionFamily * family = Ontology::connectionFamily(keys[i]);
            QTreeWidgetItem* item = treeItems.value( keys[i] );
            if (family && item)
            {
                if (item->isExpanded())
                    settings.setValue(family->name(), tr("expanded"));
                else
                    settings.setValue(family->name(), tr("collapsed"));
            }

        }
        settings.endGroup();
    }
	
	QString ConnectionsTree::iconFile(ConnectionFamily * family)
	{
		QString file = family->name().toLower();
		file.replace(tr(" "),tr(""));
		file += tr(".png");
		return  file;
	}
	
	QString ConnectionsTree::arrowImageFile(const QString& name)
	{
		QString file = tr("/Graphics/") + NodesTree::themeDirectory + tr("/Arrows/");
		file += name;
		file.replace(tr(" "),tr(""));
		file += tr(".xml");
		return  file;
	}
	
	QString ConnectionsTree::decoratorImageFile(const QString& name)
	{
		QString file = tr("/Graphics/") + NodesTree::themeDirectory + tr("/Decorators/");
		file += name;
		file.replace(tr(" "),tr(""));
		file += tr(".xml");
		return  file;
	}
	
	QTreeWidget & ConnectionsTree::widget()
	{ 
		return treeWidget; 
	}

	ConnectionFamily * ConnectionsTree::getFamily(const QString& name) const
	{
		return Ontology::connectionFamily(name);
	}
	
	bool ConnectionsTree::insertFamily(ConnectionFamily * family, FamilyTreeButton * button)
	{
		if (!family) return false;
		
		QString s = family->name().toLower();
		
		if (!Ontology::insertConnectionFamily(s,family))
			return false;

		if (button)
			treeButtons[s] = button;
		return true;
	}
	
	QStringList ConnectionsTree::getAllFamilyNames() const
	{
		QStringList names(Ontology::allConnectionFamilyNames());
		names.sort();
		return names;
	}
	
	void ConnectionsTree::setConnectionGraphics(ConnectionFamily * family)
	{
			QString homeDir = GlobalSettings::homeDir();
	        QString appDir = QCoreApplication::applicationDirPath();
			ConnectionFamily * parentFamily = ConnectionFamily::cast(family->parent());
           //set icon

			if (family->pixmap.load(homeDir + QString("/Graphics/") + NodesTree::themeDirectory + QString("/Decorators/") + ConnectionsTree::iconFile(family)))
					family->pixmap.setMask(family->pixmap.createMaskFromColor(QColor(255,255,255)));
			   else
			   if (family->pixmap.load(QString(":/images/") + ConnectionsTree::iconFile(family)))
					family->pixmap.setMask(family->pixmap.createMaskFromColor(QColor(255,255,255)));
			   else
			   if (family->pixmap.load(appDir + QString("/Graphics/") + NodesTree::themeDirectory + QString("/Decorators/") + ConnectionsTree::iconFile(family)))
					family->pixmap.setMask(family->pixmap.createMaskFromColor(QColor(255,255,255)));
			   else
					if (parentFamily)		//if no icon file, same as parent's icon
						 family->pixmap = parentFamily->pixmap;

           //set arrow head
           ArrowHeadItem * nodeitem = 0;

           if (family->graphicsItems.isEmpty())
           {
           	   QString arrowImageFile;
           	   nodeitem = 0;
           	   arrowImageFile = homeDir + QString("/") + ConnectionsTree::arrowImageFile(family->name());
           	   if (QFile::exists(arrowImageFile))
           	   {
           	   	   nodeitem = new ArrowHeadItem(arrowImageFile);
           	   }
           	   else
           	   {
             	   arrowImageFile = appDir + QString("/") + ConnectionsTree::arrowImageFile(family->name());
               	   if (QFile::exists(arrowImageFile))
			           nodeitem = new ArrowHeadItem(arrowImageFile);
			   }

	           if (!nodeitem || !nodeitem->isValid())
	           {
	               if (nodeitem) 
	                   delete nodeitem;
	           }
	           else
		           family->graphicsItems += nodeitem;
	           //if no arrow file, same as parent's arrow
	           if (parentFamily && family->graphicsItems.isEmpty() && 
	           		!parentFamily->graphicsItems.isEmpty() &&
	                NodeGraphicsItem::cast(parentFamily->graphicsItems[0]))
	                family->graphicsItems += (NodeGraphicsItem::topLevelNodeItem(parentFamily->graphicsItems[0]))->clone();
		   }

		   //decorator
           if (family->graphicsItems.size() < 2)
           {
	           QString decoratorImageFile;
	           decoratorImageFile = homeDir + QString("/") + ConnectionsTree::decoratorImageFile(family->name());
	           nodeitem = 0;

           	   if (QFile::exists(decoratorImageFile))
           	   {
           	   	   nodeitem = new ArrowHeadItem(decoratorImageFile);
           	   }
           	   else
           	   {
             	   decoratorImageFile = appDir + QString("/") + ConnectionsTree::decoratorImageFile(family->name());
               	   if (QFile::exists(decoratorImageFile))
			           nodeitem = new ArrowHeadItem(decoratorImageFile);
			   }
	           
	           if (!nodeitem || !nodeitem->isValid())
	           {
	           	   if (nodeitem)
		               delete nodeitem;
	           }
	           else
		           family->graphicsItems += nodeitem;
		       
		       if (parentFamily && (family->graphicsItems.size() < 2) &&
		       		(parentFamily->graphicsItems.size() > 1) &&
	                NodeGraphicsItem::cast(parentFamily->graphicsItems.last()))
	                {
		                family->graphicsItems += (NodeGraphicsItem::topLevelNodeItem(parentFamily->graphicsItems.last()))->clone();
		            }
		   }
		   
		   if (family && family->graphicsItems.isEmpty())
	       {
	        	QString file;
	        	file = homeDir + tr("/Graphics/") + NodesTree::themeDirectory + tr("/Arrows/default.xml");
	        	if (!QFile::exists(file))
	        		file = appDir + tr("/Graphics/") + NodesTree::themeDirectory + tr("/Arrows/default.xml");

	        	if (QFile::exists(file))
	       	    {
	       	   	    NodeGraphicsItem * node = new ArrowHeadItem(file);
	       	   	    if (node->isValid())
	       	   	        family->graphicsItems += node;
	       	    }
	       	    file = homeDir + tr("/Graphics/") + NodesTree::themeDirectory + tr("/Decorators/default.xml");
	        	if (!QFile::exists(file))
		        	file = appDir + tr("/Graphics/") + NodesTree::themeDirectory + tr("/Decorators/default.xml");
	        	
	        	if (QFile::exists(file))
	       	    {
	       	   	    NodeGraphicsItem * node = new ArrowHeadItem(file);
	       	   	    if (node->isValid())
	       	   	        family->graphicsItems += node;
	       	    }
	      }

		   QList<QToolButton*> buttons = treeButtons.values(family->name());

			if (!family->pixmap.isNull() && (family->pixmap.height() * family->pixmap.width()) > 0.0)
				for (int j=0; j < buttons.size(); ++j)
				{
					buttons[j]->setIcon(QIcon(family->pixmap));
					if (family->pixmap.width() > family->pixmap.height())
					{
						int w = 20 * family->pixmap.width()/family->pixmap.height();
						if (w > 50) w = 50;
						buttons[j]->setIconSize(QSize(w,20));
					}
					else
					{
						int h = 20 * family->pixmap.height()/family->pixmap.width();
						if (h > 50) h = 50;
						buttons[j]->setIconSize(QSize(20, h));
					}
				}
	}

	void ConnectionsTree::updateTheme()
	{
        QList<ConnectionFamily*> toplevel;
		ConnectionFamily * root;
		QList<ConnectionFamily*> allFamilies = Ontology::allConnectionFamilies();

		for (int i=0; i < allFamilies.size(); ++i)
		{
			root = ConnectionFamily::cast(allFamilies[i]->root());
			if (!toplevel.contains(root))
				toplevel += root;

			for (int j=0; j < allFamilies[i]->graphicsItems.size(); ++j)
				delete allFamilies[i]->graphicsItems[j];

			allFamilies[i]->graphicsItems.clear();
		}

		for (int i=0; i < toplevel.size(); ++i)
		{
				ConnectionFamily * family = toplevel[i];
				setConnectionGraphics(family);

			    QList<ItemFamily*> children = family->children();
				for (int j=0; j < children.size(); ++j)
					if (ConnectionFamily::cast(children[j]) && !toplevel.contains(ConnectionFamily::cast(children[j])))
						toplevel += ConnectionFamily::cast(children[j]);
		}
	}
}


