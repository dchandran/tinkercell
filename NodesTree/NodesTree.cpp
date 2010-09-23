/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 Tool for displaying the NodesTree and selecting the nodes on that tree.
 This tool also stores the tree of node families as a hashtable of <name,family> pairs.

****************************************************************************/

#include "NodesTree.h"
#include <QtDebug>
#include <QButtonGroup>
#include <QMessageBox>
#include <QRegExp>
#include <QScrollArea>

namespace Tinkercell
{
	 QString NodesTree::themeDirectory("Bio1");

     NodesTree::~NodesTree()
     {
		  networkClosing(0,0);

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
          
          QList<QToolButton*> buttons = treeButtons.values();
          
          for (int i=0; i < buttons.size(); ++i)
	          if (buttons[i] && !buttons[i]->parentWidget())
	          	delete buttons[i];
     }

     QSize NodesTree::sizeHint() const
     {
          return QSize(140, 200);
     }
     
	void  NodesTree::readTreeFile(const QString& filename)
	{
          NodesTreeReader reader;
          QString appDir = QCoreApplication::applicationDirPath();

          QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);
          settings.beginGroup("NodesTree");
          
	      themeDirectory = settings.value("theme",tr("Bio1")).toString();
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

          QString xmlFile = filename;
          if (xmlFile.isNull() || xmlFile.isEmpty())
               xmlFile = (appDir + tr("/NodesTree/NodesTree.xml"));

          QStringList keys = reader.readXml(this,xmlFile);

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
          makeNodeSelectionDialog();          
          setupThemesDialog();
          readTreeFile(filename);
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
               
		       if (mainWindow->optionsMenu)
			   {
					mainWindow->optionsMenu->addSeparator();
					QAction * treeViewAction = mainWindow->optionsMenu->addAction(tr("Select Theme"),selectThemesDialog,SLOT(exec()));
			   }

            return true;
          }
          return false;
     }
     
     void NodesTree::setupThemesDialog()
     {
		selectThemesDialog = new QDialog(this);
		
		QString appDir = QCoreApplication::applicationDirPath();
		QString homeDir = MainWindow::homeDir();

		QDir graphicsDir1(homeDir + tr("/Graphics"));
    	QDir graphicsDir2(appDir + tr("/Graphics"));
		graphicsDir1.setFilter(QDir::AllDirs);
		graphicsDir2.setFilter(QDir::AllDirs);
		graphicsDir1.setSorting(QDir::Name);
		graphicsDir2.setSorting(QDir::Name);
		QFileInfoList subdirs;
			
		if (graphicsDir1.exists())	
			subdirs += graphicsDir1.entryInfoList();
			
		if (graphicsDir2.exists())
			subdirs += graphicsDir2.entryInfoList();
		
		QDialog * dialog = selectThemesDialog;
		QHBoxLayout * themesLayout = new QHBoxLayout;
		QStringList visited;
		QString theme;
		QButtonGroup * buttonGroup = new QButtonGroup(this);
		for (int i=0; i < subdirs.size(); ++i)
		{
			theme = subdirs.at(i).baseName();
			if (!visited.contains(theme) && !theme.isEmpty())
			{
				visited << theme;
				QToolButton * button = new QToolButton;
				button->setIcon(QIcon(subdirs.at(i).absoluteFilePath() + tr("/screenshot.png")));
				button->setText(theme);
				button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
				button->setFixedWidth(120);
				button->setFixedHeight(100);
				themesLayout->addWidget(button);
				button->setCheckable(true);
				button->setChecked(false);
				buttonGroup->addButton(button);
				buttonGroup->setExclusive(true);				
			}
		}
		connect(buttonGroup,SIGNAL(buttonClicked(QAbstractButton*)),this,SLOT(selectTheme(QAbstractButton*)));
		QWidget * widget = new QWidget;
		widget->setLayout(themesLayout);
		QScrollArea * scrollArea = new QScrollArea;
		scrollArea->setWidget(widget);
		
		QHBoxLayout * buttonsLayout = new QHBoxLayout;
		QPushButton * cancelButton = new QPushButton;
		connect(cancelButton,SIGNAL(clicked()),dialog,SLOT(reject()));
		cancelButton->setText("&Close");		
		buttonsLayout->addStretch(2);
		buttonsLayout->addWidget(cancelButton);
		buttonsLayout->addStretch(2);
		QVBoxLayout * dialogLayout = new QVBoxLayout;
		dialogLayout->addWidget(scrollArea);
		dialogLayout->addLayout(buttonsLayout);
		dialog->setLayout(dialogLayout);
     }
     
      void NodesTree::selectTheme(QAbstractButton * button)
      { 
      	if (button)
      	{
	      	 themeDirectory = button->text();
	      	 QMessageBox::information(this,tr("Information"),tr("Theme ") + themeDirectory + tr(" will take effect when TinkerCell starts next time"));
	    }
      }

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
		QString homeDir = MainWindow::homeDir();
        QDir graphicsDir1(appDir + tr("/Graphics"));
		QDir graphicsDir2(homeDir + tr("/Graphics"));
		graphicsDir1.setFilter(QDir::AllDirs);
		graphicsDir2.setFilter(QDir::AllDirs);
		graphicsDir1.setSorting(QDir::Name);
		graphicsDir2.setSorting(QDir::Name);
		QFileInfoList subdirs;
		
		if (graphicsDir1.exists())	
			subdirs += graphicsDir1.entryInfoList();
		
		if (graphicsDir2.exists())
			subdirs += graphicsDir2.entryInfoList();
		
		QFileInfoList list;
		
		for (int j = 0; j < subdirs.size(); ++j) //for each theme file inside Graphics
		{
			QDir dir(subdirs.at(j).absoluteFilePath() + tr("/Nodes")); //get Grpahics/theme/Nodes dir
			if (dir.exists())
			{
				dir.setFilter(QDir::Files);
				dir.setSorting(QDir::Name);
				list += dir.entryInfoList();
			}
		}

		QListWidget * nodesListWidget = new QListWidget(mainWindow);

		for (int j = 0; j < list.size(); ++j)
		{
			QFileInfo fileInfo = list.at(j);				
			if (fileInfo.completeSuffix().toLower() == tr("png") &&
				QFile::exists(fileInfo.baseName() + tr(".xml")))
			{
				QListWidgetItem * item = new QListWidgetItem(QIcon(fileInfo.absoluteFilePath()),
					fileInfo.baseName(),nodesListWidget);
                item->setData(3,fileInfo.absolutePath() + tr("/") + fileInfo.baseName() + tr(".xml"));
				item->setSizeHint(QSize(20,20));
				nodesListWidget->addItem(item);
				nodesFilesList << item->data(3).toString();
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

     void NodesTree::networkClosing(NetworkHandle * , bool *)
     {
          //save state of the tree
          QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);

          settings.beginGroup("NodesTree");
     	  settings.setValue("theme",themeDirectory);
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
                    	newFile = nodeImageFile(family->name());
                    
                    if (item->isExpanded())
                         nodeFiles << (family->name() + tr(",") + newFile + tr(",expanded"));
                    else
                         nodeFiles << (family->name() + tr(",") + newFile + tr(",collapsed"));
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
			file.replace(tr(".XML"),tr(".png"));
			file.replace(tr(".xml"),tr(".png"));
		}
		else
		{
			file = tr("/Graphics/") + themeDirectory + tr("/Nodes/");
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
			file = tr("/Graphics/") + themeDirectory + tr("/Nodes/");
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
	
	NodeFamily * NodesTree::getFamily(const QString& name) const
	{
		if (nodeFamilies.contains(name))
			return nodeFamilies.value(name);
		
		QStringList words = name.split(" ");
		for (int i=0; i < words.size(); ++i)
		{
			words[i] = words[i].toLower();
			words[0] = words[0].toUpper();
		}

		QString s = words.join(" ");
		
		if (nodeFamilies.contains(s))
			return nodeFamilies.value(s);
		return 0;
	}
	
	bool NodesTree::insertFamily(NodeFamily * family, FamilyTreeButton * button)
	{
		if (name.isEmpty() || !family) return false;
		QStringList words = family->name().split(" ");
		for (int i=0; i < words.size(); ++i)
		{
			words[i] = words[i].toLower();
			words[0] = words[0].toUpper();
		}

		family->name() = words.join(" ");

		nodeFamilies [family->name()] = family;
		if (button)
			treeButtons [family->name()] = button;
		return true;
	}
	
	QStringList NodesTree::getAllFamilyNames() const
	{
		QStringList names(nodeFamilies.keys());
		names.sort();
		return names;
	}
}


