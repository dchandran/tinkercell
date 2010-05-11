/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 Tool for displaying the NodesTree and selecting the nodes on that tree.
 This tool also stores the tree of node families as a hashtable of <name,family> pairs.

****************************************************************************/

#include "NodesTree.h"
#include <QtDebug>
#include <QRegExp>

namespace Tinkercell
{
     NodesTree::~NodesTree()
     {
          QList<NodeFamily*> list = nodeFamilies.values();

          for (int i=0; i < list.size(); ++i)
          {
               NodeFamily * node = list[i];
               if (node)
               {
                    delete node;
                    node = 0;
               }
          }
     }

     QSize NodesTree::sizeHint() const
     {
          return QSize(140, 200);
     }

     NodesTree::NodesTree(QWidget * parent, const QString& filename) :
               Tool(tr("Nodes Tree"),tr("Parts Catalog"),parent)
     {
          QVBoxLayout * layout = new QVBoxLayout;
          treeWidget.setHeaderHidden(true);

          treeWidget.header()->setResizeMode(QHeaderView::Stretch);

          layout->addWidget(&treeWidget);
          layout->setContentsMargins(0,0,0,0);
          layout->setSpacing(0);
          setLayout(layout);

          NodesTreeReader reader;
          QString appDir = QCoreApplication::applicationDirPath();

          QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);
          settings.beginGroup("NodesTree");
          
          QStringList nodeSettings = settings.value("nodeFiles",QStringList()).toStringList();
          QHash<QString,bool> expandedNodes;
          
		  for (int i=0; i < nodeSettings.size(); ++i)
          {
          	QStringList lst = nodeSettings[i].split(tr(","));
            if (lst.size() > 2)
            {
	        	nodeGraphicsFileNames[ lst[0] ] = lst[1];
	         	expandedNodes[ lst[0] ] = (lst[2] == tr("expanded"));
	        }
          }

          QString xmlFile ;
          if (xmlFile.isNull() || xmlFile.isEmpty())
               xmlFile = (appDir + tr("/NodesTree/NodesTree.xml"));

          reader.readXml(this,xmlFile);

          QList<QString> keys = nodeFamilies.keys();

          for (int i=0; i < keys.size(); ++i)
          {
               NodeFamily * node = nodeFamilies.value(keys[i]);
               if (!node)
                   continue;

               QList<QTreeWidgetItem*> treeItem = treeItems.values(keys[i]);
               if (node && !treeItem.isEmpty())
               {
               		bool expand = (expandedNodes.contains(keys[i]) && expandedNodes[ keys[i] ]);
              		for (int j=0; j < treeItem.size(); ++j)
   	                     treeItem[j]->setExpanded(expand);
                    for (int j=0; j < treeItem.size(); ++j)
                    {
                         FamilyTreeButton * button = new FamilyTreeButton(node,this);
                         button->nodesTree = this;

                         connect(button,SIGNAL(nodeSelected(NodeFamily*)),this,SLOT(buttonPressed(NodeFamily*)));
                         //button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
                         //button->setDefault(false);
                         //button->setFlat(true);

                         QHBoxLayout * layout = new QHBoxLayout;
                         layout->addWidget(button,0,Qt::AlignLeft);
                         layout->setContentsMargins(0,0,0,0);
                         QWidget * widget = new QWidget;
                         widget->setPalette(QPalette(QColor(255,255,255)));
                         widget->setAutoFillBackground (true);
                         widget->setLayout(layout);

                         treeWidget.setItemWidget(treeItem[j],0,widget);
						 treeButtons.insertMulti(keys[i],button);
                    }
               }
          }

          settings.endGroup();
          makeNodeSelectionDialog();
     }

     void NodesTree::buttonPressed(NodeFamily * node)
     {
          if (node)
          {
			   emit sendEscapeSignal(this);
               emit nodeSelected(node);
          }
     }

     bool NodesTree::setMainWindow(MainWindow * TinkercellWindow)
     {
          Tool::setMainWindow(TinkercellWindow);

          if (mainWindow)
          {
               connect(this,SIGNAL(sendEscapeSignal(const QWidget*)),mainWindow,SIGNAL(escapeSignal(const QWidget*)));
               
               connect(mainWindow,SIGNAL(windowClosing(NetworkHandle * , bool *)),this,SLOT(windowClosing(NetworkHandle * , bool *)));
               
			   return true;
          }
          return false;
     }

     /*void NodesTree::keyPressEvent ( QKeyEvent * event )
     {
          emit keyPressed(event->key(),event->modifiers());
          if (event->key() == Qt::Key_Escape || event->key() == Qt::Key_Space)
               emit sendEscapeSignal(this);
     }*/

     void NodesTree::changeTree()
     {
          QString fileName =
                    QFileDialog::getOpenFileName(this, tr("Nodes Tree File"),
                                                 tr(""),
                                                 tr("XML Files (*.xml)"));
          if (fileName.isEmpty())
               return;

          QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);

          settings.beginGroup("NodesTree");
          settings.setValue("file", fileName);
          settings.endGroup();

          QMessageBox::information (this, tr("Change tree"), tr("The new nodes tree will be active the next time you start TinkerCell"));
     }

     void NodesTree::contextMenuEvent(QContextMenuEvent * event)
     {
          emit sendEscapeSignal(this);
          static QMenu * menu = 0;
          if (!menu)
          {
               menu = new QMenu(this);
               menu->addAction(QIcon(":/images/image.png"),tr("Change nodes catalog"),this,SLOT(changeTree()));
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

	 void NodesTree::itemActivated( QListWidgetItem * )
	 {
		nodeFileAccepted();
	 }


     void NodesTree::makeNodeSelectionDialog()
     {
          QString appDir = QCoreApplication::applicationDirPath();

          QDir dir(appDir + tr("/NodeItems/"));
          dir.setFilter(QDir::Files);
          dir.setSorting(QDir::Name);

          QFileInfoList list = dir.entryInfoList();

          nodesListWidget = new QListWidget(this);
          nodesFilesList.clear();

          for (int i = 0; i < list.size(); ++i)
          {
               QFileInfo fileInfo = list.at(i);
               if (fileInfo.completeSuffix().toLower() == tr("png") &&
                   dir.exists(fileInfo.baseName() + tr(".xml")))
               {
                    QListWidgetItem * item = new QListWidgetItem(QIcon(fileInfo.absoluteFilePath()),
                                                                 fileInfo.baseName(),nodesListWidget);
                    item->setData(3,dir.absolutePath() + tr("/") + fileInfo.baseName() + tr(".xml"));
                    nodesListWidget->addItem(item);
                    nodesFilesList << item->data(3).toString();
               }
          }

          QDir userdir(MainWindow::userHome());
          if (userdir.exists(tr("/NodeItems/")))
          {
               userdir.cd(tr("/NodeItems/"));
               userdir.setFilter(QDir::Files);
               userdir.setSorting(QDir::Name);
               list = userdir.entryInfoList();

               for (int i = 0; i < list.size(); ++i)
               {
                    QFileInfo fileInfo = list.at(i);
                    if (fileInfo.completeSuffix().toLower() == tr("png") &&
                        userdir.exists(fileInfo.baseName() + tr(".xml")) &&
                        !nodesFilesList.contains(userdir.absolutePath() + tr("/") + fileInfo.baseName() + tr(".xml")))
                    {
                         QListWidgetItem * item = new QListWidgetItem(QIcon(fileInfo.absoluteFilePath()),
                                                                      fileInfo.baseName(),nodesListWidget);
                         item->setData(3,userdir.absolutePath() + tr("/") + fileInfo.baseName() + tr(".xml"));
                         nodesListWidget->addItem(item);
                         nodesFilesList << item->data(3).toString();
                    }
               }
          }

          nodeSelectionDialog = new QDialog(this);
          nodeSelectionDialog->setSizeGripEnabled(true);
          QVBoxLayout * layout = new QVBoxLayout;

          layout->addWidget(nodesListWidget);
		  connect(nodesListWidget,SIGNAL(itemActivated( QListWidgetItem * )),this,SLOT(itemActivated( QListWidgetItem * )));

          QHBoxLayout * buttonsLayout = new QHBoxLayout;
          QPushButton * ok = new QPushButton(tr("Replace"));
          QPushButton * cancel = new QPushButton(tr("Cancel"));
          QPushButton * otherFile = new QPushButton(tr("File not listed..."));

          connect(ok,SIGNAL(released()),nodeSelectionDialog,SLOT(accept()));
          connect(cancel,SIGNAL(released()),nodeSelectionDialog,SLOT(reject()));
          connect(otherFile,SIGNAL(released()),this,SLOT(selectNewNodeFile()));

          connect(nodeSelectionDialog,SIGNAL(accepted()),this,SLOT(nodeFileAccepted()));

		  buttonsLayout->addWidget(ok);
          buttonsLayout->addWidget(cancel);
		  buttonsLayout->addWidget(otherFile);
          

          layout->addLayout(buttonsLayout);

          nodeSelectionDialog->setLayout(layout);
     }

     void NodesTree::selectNewNodeFile()
     {
          QString fileName =
                    QFileDialog::getOpenFileName(mainWindow, tr("New Graphics File"),
                                                 QDir::currentPath(),
                                                 tr("XML Files (*.xml)"));
          if (fileName.isEmpty())
               return;

          QFileInfo fileInfo(fileName);

          if (fileInfo.completeSuffix().toLower() == tr("xml") &&
              !nodesFilesList.contains(fileInfo.dir().absolutePath() + tr("/") + fileInfo.baseName() + tr(".xml")))
          {
               QListWidgetItem * item = new QListWidgetItem(
                         QIcon(fileInfo.dir().absolutePath() + tr("/") + fileInfo.baseName() + tr(".png")),
                         fileInfo.baseName(),nodesListWidget);
               item->setData(3,fileInfo.dir().absolutePath() + tr("/") + fileInfo.baseName() + tr(".xml"));
               nodesListWidget->addItem(item);
               nodesFilesList << item->data(3).toString();
          }
     }

     QString NodesTree::replaceNodeFile()
     {
          temporaryFileName = QString();

          nodeSelectionDialog->exec();

          return temporaryFileName;
     }

     void NodesTree::nodeFileAccepted()
     {
          if (!nodesListWidget)
               temporaryFileName = QString();
          else
               if (!nodesListWidget->currentItem())
                    temporaryFileName = QString();
          else
               temporaryFileName = nodesListWidget->currentItem()->data(3).toString();
     }

     void NodesTree::windowClosing(NetworkHandle * , bool *)
     {
          //save state of the tree
          QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);

          settings.beginGroup("NodesTree");
          
          QList<QString> keys = nodeFamilies.keys();
          QStringList nodeFiles;

          for (int i=0; i < keys.size(); ++i)
          {
               NodeFamily * family = nodeFamilies[keys[i] ];
               QTreeWidgetItem* item = treeItems.value( keys[i] );
               QToolButton * button = treeButtons.value( keys[i] );
               FamilyTreeButton * treeButton = 0;
               if (button) treeButton = static_cast<FamilyTreeButton*>(button);	
               
               if (family && item && family->graphicsItems.size() > 0 && qgraphicsitem_cast<NodeGraphicsItem*>(family->graphicsItems[0]))
               {
                    QString newFile;
                    if (treeButton) newFile = treeButton->newFileName;
                    
                    if (newFile.isNull() || newFile.isEmpty())
                    	newFile = nodeImageFile(family->name);
                    
                    if (item->isExpanded())
                         nodeFiles << (family->name + tr(",") + newFile + tr(",expanded"));
                    else
                         nodeFiles << (family->name + tr(",") + newFile + tr(",collapsed"));
               }
          }
          settings.setValue("nodeFiles",nodeFiles);
          settings.endGroup();
     }

	 QString NodesTree::iconFile(QString name)
	 {
	 	QString file;
		if (nodeGraphicsFileNames.contains(name) && QFile::exists(nodeGraphicsFileNames[name]))
		{
			file = nodeGraphicsFileNames[name];
			file.replace(tr(".XML"),tr(".PNG"));
			file.replace(tr(".xml"),tr(".PNG"));
		}
		else
		{
			file = tr("NodeItems/");
			file += name;
			file.replace(tr(" "),tr(""));
			file += tr(".PNG");
		}
		return  file;
	 }

	 QString NodesTree::nodeImageFile(QString name)
	 {
		QString file;
		if (nodeGraphicsFileNames.contains(name) && QFile::exists(nodeGraphicsFileNames[name]))
		{
			file = nodeGraphicsFileNames[name];
		}
		else
		{
			file = tr("NodeItems/");
			file += name;
			file.replace(tr(" "),tr(""));
			file += tr(".xml");
		}
		return  file;
	 }

	QTreeWidget & NodesTree::widget()
	{
		return treeWidget;
	}
}


