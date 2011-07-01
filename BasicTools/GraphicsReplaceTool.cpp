/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This is source file for the GraphicsReplaceTool plugin.
This plugin adds the copy, cut, and paste features to Tinkercell.
It also places the buttons for these functions in the toolbar, in the edit menu, and
the context menu (mouse right-click).

****************************************************************************/

#include <QClipboard>
#include "GlobalSettings.h"
#include "NetworkHandle.h"
#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "GraphicsReplaceTool.h"

namespace Tinkercell
{
	GraphicsReplaceTool::GraphicsReplaceTool() : Tool(tr("Graphics Replace Tool"),tr("Basic GUI"))
	{
	}

	bool GraphicsReplaceTool::setMainWindow(MainWindow *main)
	{
		Tool::setMainWindow(main);

		if (mainWindow && mainWindow->editMenu)
		{
			makeNodeSelectionDialog();

			QAction * replaceNode = new QAction(QIcon(":/images/parts.png"),tr("Replace graphics"),mainWindow->editMenu);
			replaceNode->setToolTip(tr("Replace node graphics"));
			connect(replaceNode,SIGNAL(triggered()),this,SLOT(substituteNodeGraphics()));
			//QToolBar * toolBar = mainWindow->toolBarEdits;
			//if (toolBar)
			//	toolBar->addAction(replaceNode);

			mainWindow->editMenu->addAction(replaceNode);
			mainWindow->contextItemsMenu.addAction(replaceNode);

			return true;
		}
		return false;
	}



	void GraphicsReplaceTool::makeNodeSelectionDialog()
	{
		QString appDir = QCoreApplication::applicationDirPath();
		QString homeDir = GlobalSettings::homeDir();

		tabWidget = new QTabWidget(mainWindow);

		QStringList headers;
		headers << "Nodes" << "Arrows" << "Decorators";

		for (int i=0; i < headers.size(); ++i)
		{
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
			
			QListWidget * nodesListWidget = new QListWidget(mainWindow);
			
			for (int j = 0; j < subdirs.size(); ++j) //for each theme file inside Graphics
			{
				QFileInfoList list;
				QDir dir(subdirs.at(j).absoluteFilePath() + tr("/") + headers[i]); //get Graphics/theme/header dir
				if (dir.exists())
				{
					dir.setFilter(QDir::Files);
					dir.setSorting(QDir::Name);
					list += dir.entryInfoList();
				}
				
				if (!list.isEmpty())
				{
					nodesListWidget->addItem(new QListWidgetItem(tr(""),nodesListWidget));
					nodesListWidget->addItem(new QListWidgetItem(tr("Theme: ") + subdirs.at(j).baseName(),nodesListWidget));
					nodesListWidget->addItem(new QListWidgetItem(tr(""),nodesListWidget));
				}

				for (int k = 0; k < list.size(); ++k)
				{
					QFileInfo fileInfo = list.at(k);				
					if (fileInfo.completeSuffix().toLower() == tr("png") &&
						QFile::exists(fileInfo.absolutePath() + tr("/") + fileInfo.baseName() + tr(".xml")))
					{
						QListWidgetItem * item = new QListWidgetItem(QIcon(fileInfo.absoluteFilePath()),
							fileInfo.baseName(),nodesListWidget);
		                item->setData(3,fileInfo.absolutePath() + tr("/") + fileInfo.baseName() + tr(".xml"));
						item->setSizeHint(QSize(20,20));
						nodesListWidget->addItem(item);
						nodesFilesList << item->data(3).toString();
					}
				}
			}

			connect(nodesListWidget,SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(replaceNode(QListWidgetItem*)));
			tabWidget->addTab(nodesListWidget,headers[i]);
			nodesListWidgets << nodesListWidget;
		}

		nodeSelectionDialog = new QDialog(mainWindow);
		nodeSelectionDialog->setSizeGripEnabled(true);
		QVBoxLayout * layout = new QVBoxLayout;

		layout->addWidget(tabWidget);

		QHBoxLayout * buttonsLayout = new QHBoxLayout;
		QPushButton * ok = new QPushButton(tr("Replace"));
		QPushButton * cancel = new QPushButton(tr("Cancel"));
		QPushButton * otherFile = new QPushButton(tr("File not listed..."));

		connect(ok,SIGNAL(released()),nodeSelectionDialog,SLOT(accept()));
		connect(cancel,SIGNAL(released()),nodeSelectionDialog,SLOT(reject()));
		connect(otherFile,SIGNAL(released()),this,SLOT(selectNewNodeFile()));

		buttonsLayout->addWidget(ok);
		buttonsLayout->addWidget(cancel);
		buttonsLayout->addWidget(otherFile);
		

		layout->addLayout(buttonsLayout);

		nodeSelectionDialog->setLayout(layout);
	}

	void GraphicsReplaceTool::selectNewNodeFile()
	{
		if (!tabWidget || tabWidget->currentIndex() < 0 || tabWidget->currentIndex() >= nodesListWidgets.size()) return;

		QListWidget * nodesListWidget = nodesListWidgets[tabWidget->currentIndex()];

		if (!nodesListWidget) return;

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
		else
		{
			if (console())
                console()->message(tr("this file is already listed"));
		}
	}
	
	void GraphicsReplaceTool::substituteNodeGraphics()
	{
		GraphicsScene * scene = currentScene();
		if (!scene || scene->selected().isEmpty()) return;

		nodeSelectionDialog->exec();
		if (nodeSelectionDialog->result() == QDialog::Accepted)
		{
			if (!tabWidget || tabWidget->currentIndex() < 0 || tabWidget->currentIndex() >= nodesListWidgets.size()) return;
			replaceNode(nodesListWidgets[tabWidget->currentIndex()]->currentItem());
		}
	}

	void GraphicsReplaceTool::replaceNode(QListWidgetItem * item)
	{
		if (!item || !mainWindow || !currentScene()) return;
		GraphicsScene * scene = currentScene();

		QString fileName = item->data(3).toString();
		if (fileName.isEmpty() || fileName.isNull()) return;

		QList<QGraphicsItem*> & list = scene->selected();

		QList<NodeGraphicsItem*> nodesList;
		QList<NodeGraphicsItem*> arrowHeadList;
		
		ConnectionGraphicsItem * connection = 0;
		NodeGraphicsItem * node = 0;
		
		for (int i=0; i < list.size(); ++i)
		{
			node = NodeGraphicsItem::cast(list[i]);
			if (node)
				nodesList += node;
			else
			{
				connection = ConnectionGraphicsItem::cast(list[i]);
				if (connection)
				{
					QList<ArrowHeadItem*> arrowHeads = connection->arrowHeads();
					for (int j=0; j < arrowHeads.size(); ++j)
						if (arrowHeads[j])
							arrowHeadList += arrowHeads[j];
				}
			}
		}

		if (nodesList.isEmpty()) 
			nodesList = arrowHeadList;
		
		if (nodesList.isEmpty()) return;
		/*
		for (int i=0; i < nodesList.size(); ++i)
			if (nodesList[i]->className == ArrowHeadItem::CLASSNAME)
			{
				_transform = false;
				break;
			}*/

		QList<QString> filenames;
		for (int i=0; i < nodesList.size(); ++i)
			filenames += fileName;

		ReplaceNodeGraphicsCommand * command = new ReplaceNodeGraphicsCommand(tr("Image replaced"),nodesList,filenames);
		if (scene->network)
			scene->network->push(command);
		else
			command->redo();

		return;

		for (int i=0; i < nodesList.size(); ++i)
		{
			QFile file (fileName);

			if (!file.open(QFile::ReadOnly | QFile::Text)) {
				QMessageBox::warning(mainWindow, tr("NodeGraphicsItem File"),
					tr("Cannot read file %1:\n%2.")
					.arg(fileName)
					.arg(file.errorString()));
				return;
			}
			QGraphicsItem * parent = nodesList[i]->parentItem();
			nodesList[i]->setParentItem(0);

			QPointF p = nodesList[i]->scenePos();
			QTransform t0 = nodesList[i]->sceneTransform();
			QTransform t1(t0.m11(),t0.m12(),0,t0.m21(),t0.m22(),0,0,0,1);

			nodesList[i]->setBoundingBoxVisible(false);
			nodesList[i]->clear();
			nodesList[i]->setPos(QPointF());
			nodesList[i]->resetTransform();

			NodeGraphicsReader reader;
			reader.readNodeGraphics(nodesList[i],&file);
			nodesList[i]->normalize();

			nodesList[i]->setPos(p);
			nodesList[i]->setTransform(t1);

			nodesList[i]->setParentItem(parent);
		}
	}

	bool GraphicsReplaceTool::fullReactionSelected(const ConnectionGraphicsItem*)
	{
		return true;
	}

}
