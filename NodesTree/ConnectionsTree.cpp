/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 Tool for displaying the Connections Tree and selecting the nodes on that tree.
 This tool also stores the tree of connection families as a hashtable of <name,family> pairs.

****************************************************************************/

#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConnectionsTree.h"
#include "TreeButton.h"
#include <QDialog>

namespace Tinkercell
{
    ConnectionsTree::~ConnectionsTree()
    {
		networkClosing(0,0);
        if (connectionFamilies.size() > 0)
        {
            QList<ConnectionFamily*> list = connectionFamilies.values();

            for (int i=0; i < list.size(); ++i)
            {
                ConnectionFamily * family = list[i];
                if (family)
                {
                    delete family;
                    family = 0;
                }
            }
        }
        QList<QToolButton*> buttons = treeButtons.values();
        for (int i=0; i < buttons.size(); ++i)
        	if (buttons[i] && !buttons[i]->parentWidget())
        		delete buttons[i];
    }

    QSize ConnectionsTree::sizeHint() const
    {
        return QSize(140, 200);
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

        QString appDir = QCoreApplication::applicationDirPath();
        ConnectionsTreeReader reader;

		QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);
        settings.beginGroup("ConnectionsTree");
       //QString xmlFile = settings.value("file", filename).toString();
        QString xmlFile ;
        if (xmlFile.isNull() || xmlFile.isEmpty())
            xmlFile = (appDir + tr("/NodesTree/ConnectionsTree.xml"));

        reader.readXml(this,xmlFile);

        QList<QString> keys = connectionFamilies.keys();

        for (int i=0; i < keys.size(); ++i)
        {
            ConnectionFamily * family = connectionFamilies.value(keys[i]);
            if (!family) continue;

            QList<QTreeWidgetItem*> treeItem = treeItems.values(keys[i]);
            if (family && !treeItem.isEmpty())
            {
                QString setting = settings.value(family->name,QString()).toString();
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
		
		settings.endGroup();
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

        QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);

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
        QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);

        settings.beginGroup("ConnectionsTree");

        QList<QString> keys = connectionFamilies.keys();

        for (int i=0; i < keys.size(); ++i)
        {
            ConnectionFamily * family = connectionFamilies[keys[i] ];
            QTreeWidgetItem* item = treeItems.value( keys[i] );
            if (family && item)
            {
                if (item->isExpanded())
                    settings.setValue(family->name, tr("expanded"));
                else
                    settings.setValue(family->name, tr("collapsed"));
            }

        }
        settings.endGroup();
    }
	
	QString ConnectionsTree::iconFile(ConnectionFamily * family)
	{
		QString file = tr("NodesTree/Icons/");
		file += family->name;
		file.replace(tr(" "),tr(""));
		file += tr(".PNG");
		return  file;
	}
	
	QString ConnectionsTree::arrowImageFile(QString name)
	{
		QString file = tr("ArrowItems/");
		file += name;
		file.replace(tr(" "),tr(""));
		file += tr(".xml");
		return  file;
	}
	
	QTreeWidget & ConnectionsTree::widget()
	{ 
		return treeWidget; 
	}
}


