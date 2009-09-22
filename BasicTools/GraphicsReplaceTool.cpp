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

#include "NetworkWindow.h"
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
	GraphicsReplaceTool::GraphicsReplaceTool() : Tool(tr("Graphics Replace Tool"))
	{
	}

	bool GraphicsReplaceTool::setMainWindow(MainWindow *main)
	{
		Tool::setMainWindow(main);

		if (mainWindow)
		{
			makeNodeSelectionDialog();

			QToolBar * toolBar = mainWindow->toolBarEdits;

			//connect(this,SIGNAL(itemsInserted(GraphicsScene*,const QList<QGraphicsItem*>&,const QList<ItemHandle*>&)),
			//		mainWindow,SIGNAL(itemsInserted(GraphicsScene*,const QList<QGraphicsItem*>&,const QList<ItemHandle*>&)));

			QAction * replaceNode = new QAction(QIcon(":/images/parts.png"),tr("Replace graphics"),mainWindow->centralWidget());
			replaceNode->setToolTip(tr("Replace node graphics"));
			connect(replaceNode,SIGNAL(triggered()),this,SLOT(substituteNodeGraphics()));

			if (toolBar)
				toolBar->addAction(replaceNode);

			mainWindow->editMenu->addAction(replaceNode);
			mainWindow->contextItemsMenu.addAction(replaceNode);

			return true;
		}
		return false;
	}



	void GraphicsReplaceTool::makeNodeSelectionDialog()
	{
		QString appDir = QCoreApplication::applicationDirPath();

		tabWidget = new QTabWidget(mainWindow);

		QStringList headers;
		QStringList paths; 
		headers << "Nodes, Cells, etc." << "Arrow heads" << "Decorations";
		paths << "/NodeItems/" << "/ArrowItems/" << "/OtherItems/";

		for (int i=0; i < headers.size() && paths.size(); ++i)
		{

			QDir dir(appDir + paths[i]);
			dir.setFilter(QDir::Files);
			dir.setSorting(QDir::Name);

			QFileInfoList list = dir.entryInfoList();

			QListWidget * nodesListWidget = new QListWidget(mainWindow);

			for (int j = 0; j < list.size(); ++j) 
			{
				QFileInfo fileInfo = list.at(j);
				if (fileInfo.completeSuffix().toLower() == tr("png") && 
					dir.exists(fileInfo.baseName() + tr(".xml")))
				{
					QListWidgetItem * item = new QListWidgetItem(QIcon(fileInfo.absoluteFilePath()),
						dir.absolutePath() + tr("/") + fileInfo.baseName() + tr(".xml"),nodesListWidget);
					item->setSizeHint(QSize(20,20));
					nodesListWidget->addItem(item);
					nodesFilesList << item->text();
				}
			}

			QDir userdir(MainWindow::userHome());
			if (userdir.exists(paths[i]))
			{
				userdir.cd(paths[i]);
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
							userdir.absolutePath() + tr("/") + fileInfo.baseName() + tr(".xml"),nodesListWidget);
						nodesListWidget->addItem(item);
						nodesFilesList << item->text();
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

		connect(nodeSelectionDialog,SIGNAL(accepted()),this,SLOT(replaceNode()));


		buttonsLayout->addWidget(otherFile);
		buttonsLayout->addWidget(cancel);
		buttonsLayout->addWidget(ok);

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
				fileInfo.dir().absolutePath() + tr("/") + fileInfo.baseName() + tr(".xml"),nodesListWidget);
			nodesListWidget->addItem(item);
			nodesFilesList << item->text();
		}
		else
		{
			ConsoleWindow::message(tr("this file is already listed"));
		}
	}


	void GraphicsReplaceTool::substituteNodeGraphics()
	{
		nodeSelectionDialog->exec();
	}

	void GraphicsReplaceTool::replaceNode()
	{
		if (!tabWidget || tabWidget->currentIndex() < 0 || tabWidget->currentIndex() >= nodesListWidgets.size()) return;

		replaceNode(nodesListWidgets[tabWidget->currentIndex()]->currentItem());
	}

	void GraphicsReplaceTool::replaceNode(QListWidgetItem * item)
	{
		if (!item || !mainWindow || !mainWindow->currentScene()) return;
		GraphicsScene * scene = mainWindow->currentScene();

		QString fileName = item->text();
		if (fileName.isEmpty() || fileName.isNull()) return;

		QList<QGraphicsItem*> & list = scene->selected();
		QList<NodeGraphicsItem*> nodesList;

		NodeGraphicsItem * node = 0;
		for (int i=0; i < list.size(); ++i)
		{
			node = NodeGraphicsItem::cast(list[i]);
			if (node)
				nodesList += node;
		}

		if (nodesList.isEmpty()) return;


		QList<QString> filenames;
		for (int i=0; i < nodesList.size(); ++i)
			filenames += fileName;

		//scene->clearSelection();

		ReplaceNodeGraphicsCommand * command = new ReplaceNodeGraphicsCommand(tr("nodes image replaced"),nodesList,filenames);
		if (scene->historyStack)
			scene->historyStack->push(command);
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
