/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This is source file for Tinkercell's main window
The MainWindow contains a set of GraphicScenes, which is the class
that performs all the drawing. Each GraphicsScene emits various signals. Those
signals are then emitted by the MainWindow; in this way, a plugin does not need
to listen to each of the GraphicsScene signals but only the MainWindow's signals.

The MainWindow also has its own signals, such as a toolLoaded, networkSaved, etc.

The MainWindow keeps a list of all plugins, and it is also responsible for loading plugins.

****************************************************************************/

#include <QLibrary>
#include <QSettings>
#include <QInputDialog>
#include <QActionGroup>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QSvgGenerator>
#include <QColorDialog>
#include <QImage>
#include "TextEditor.h"
#include "NetworkWindow.h"
#include "NetworkHandle.h"
#include "GraphicsScene.h"
#include "GraphicsView.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "CThread.h"
#include "MultithreadedSliderWidget.h"
#include "ConsoleWindow.h"
#include "AbstractInputWindow.h"
#include "TextParser.h"
#include "C_API_Slots.h"
#include "PlotTool.h"
#include "GnuplotTool.h"

namespace Tinkercell
{
	typedef void (*TinkercellPluginEntryFunction)(MainWindow*);
	typedef void (*TinkercellCEntryFunction)();

	/********** GLOBAL VARIABLES **********/

	MainWindow::TOOL_WINDOW_OPTION MainWindow::defaultToolWindowOption = MainWindow::ToolBoxWidget;
	MainWindow::TOOL_WINDOW_OPTION MainWindow::defaultHistoryWindowOption = MainWindow::ToolBoxWidget;
	MainWindow::TOOL_WINDOW_OPTION MainWindow::defaultConsoleWindowOption = MainWindow::DockWidget;
	QString MainWindow::previousFileName;
	QString MainWindow::homeDirPath;
	/*************************************/

	QString MainWindow::homeDir()
	{
		if (homeDirPath.contains(PROJECTNAME) && QDir(homeDirPath).exists())
		{
			return homeDirPath;
		}

		QDir dir = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
		QString tcdir = PROJECTNAME;

		if (!dir.exists(tcdir))
			dir.mkdir(tcdir);

		dir.cd(tcdir);
		homeDirPath = dir.absolutePath();
		return homeDirPath;
	}

	QString MainWindow::tempDir()
	{
		QString location = QDesktopServices::storageLocation(QDesktopServices::TempLocation);

		QString temp = location + tr("/TinkerCell");

		if (!temp.isEmpty() && QDir(temp).exists())
			return temp;

		QDir dir(location);

		if (!dir.exists(QString("TinkerCell")))
			dir.mkdir(QString("TinkerCell"));

		dir.cd(QString("TinkerCell"));
		temp = dir.absolutePath();
		return temp;
	}

	void MainWindow::setUserHome()
	{
		QString home = QFileDialog::getExistingDirectory(this,tr("Select new user home directory"),homeDir());
		if (home.isEmpty() || home.isNull()) return;

		//home must be inside user's home directory to avoid write permission problems
		QDir dir = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
		if (home.contains(dir.absolutePath()))  
			homeDirPath = home;
	}

	typedef void (*progress_api_initialize)(void (*tc_showProgress)(const char *, int));

	void MainWindow::loadDynamicLibrary(const QString& dllFile)
	{
		QString home = homeDir(),
			temp = tempDir(),
			current = QDir::currentPath(),
			appDir = QCoreApplication::applicationDirPath();

		QString name[] = {
			dllFile,
			home + tr("/") + dllFile,
			temp + tr("/") + dllFile,
			current + tr("/") + dllFile,
			appDir + tr("/") + dllFile,
		};

		QLibrary * lib = new QLibrary(this);

		bool loaded = false;
		for (int i=0; i < 5; ++i) //try different possibilities
		{
			lib->setFileName(name[i]);
			loaded = lib->load();
			if (loaded)
				break;
		}

		if (loaded)
		{
			dynamicallyLoadedLibraries.insert(lib->fileName(),lib);
			statusBar()->showMessage(lib->fileName() + tr(" loading ..."));
			TinkercellPluginEntryFunction f1 = (TinkercellPluginEntryFunction)lib->resolve(CPP_ENTRY_FUNCTION.toAscii().data());
			if (f1)
			{
				try
				{
					f1(this);
					dynamicallyLoadedLibraries.insert(lib->fileName(),lib);
					statusBar()->showMessage(lib->fileName() + tr(" successfully loaded"));
				}
				catch(...)
				{
					lib->unload();
					delete lib;
				}
			}
			else
			{
				TinkercellCEntryFunction f2 = (TinkercellCEntryFunction)lib->resolve(C_ENTRY_FUNCTION.toAscii().data());
				if (f2)
				{
					try
					{
						setupFunctionPointersSlot(0,lib);
						f2();
						dynamicallyLoadedLibraries.insert(lib->fileName(),lib);
						statusBar()->showMessage(lib->fileName() + tr(" successfully loaded"));
					}
					catch(...)
					{
						lib->unload();
						delete lib;
					}
				}
				else
				{
					lib->unload();
					delete lib;
				}
			}
		}
		else
		{
			if (consoleWindow)
				consoleWindow->message(dllFile + tr(" could not be opened"));
			statusBar()->showMessage(dllFile + tr(" could not be opened"));
			delete lib;
		}
	}

	void MainWindow::setupNewThread(QSemaphore* s,QLibrary * f)
	{
		if (!f)
		{
			if (s)
				s->release();
		}
		emit funtionPointersToMainThread(s,f );
	}

	MainWindow * MainWindow::globalInstance = 0;

	MainWindow * MainWindow::instance()
	{
		return globalInstance;
	}

	MainWindow::MainWindow(bool enableScene, bool enableText, bool enableConsoleWindow, bool showHistory, bool allowViews)
	{
		MainWindow::globalInstance = this;

		allowViewModeToChange = allowViews;

		setMouseTracking(true);
		RegisterDataTypes();
		previousFileName = QDir::currentPath();

		consoleWindow = 0;
		currentNetworkWindow = 0;
		toolBox = 0;
		setAutoFillBackground(true);

		initializeMenus(enableScene,enableText);
		//setIconSize(QSize(25,25));

		tabWidget = new QTabWidget;
		tabWidget->setStyleSheet(tr("QTabBar::tab { min-width: 36ex; }"));
		connect(tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabIndexChanged(int)));

		QToolButton * upButton = new QToolButton;
		upButton->setIcon(QIcon(tr(":/images/eject.png")));
		tabWidget->setCornerWidget(upButton);
		connect(upButton,SIGNAL(pressed()),this,SLOT(popOut()));
		upButton->setToolTip(tr("Pop-out"));

		setCentralWidget(tabWidget);

		setWindowTitle(tr("Tinkercell"));
		setStyleSheet("QMainWindow::separator { width: 0px; height: 0px; }");

		connect(this,SIGNAL(funtionPointersToMainThread(QSemaphore*,QLibrary*)),this,SLOT(setupFunctionPointersSlot(QSemaphore*,QLibrary*)));

		connect(this,SIGNAL(itemsInserted(GraphicsScene*,QList<QGraphicsItem*>,QList<ItemHandle*>)),
			this,SLOT(itemsInsertedSlot(GraphicsScene*,QList<QGraphicsItem*>,QList<ItemHandle*>)));

		connect(this,SIGNAL(itemsRemoved(GraphicsScene*,QList<QGraphicsItem*>,QList<ItemHandle*>)),
			this,SLOT(itemsRemovedSlot(GraphicsScene*,QList<QGraphicsItem*>,QList<ItemHandle*>)));

		if (showHistory)
		{
			historyWindow.setWindowTitle(tr("History"));
			historyWindow.setWindowIcon(QIcon(tr(":/images/scroll.png")));
			addToolWindow(&historyWindow,MainWindow::defaultHistoryWindowOption,Qt::RightDockWidgetArea);
		}

		if (enableConsoleWindow)
		{
			consoleWindow = new ConsoleWindow(this);
			if (optionsMenu)
			{
				QMenu * consoleColorMenu = optionsMenu->addMenu(tr("Console window colors"));

				consoleColorMenu->addAction(tr("Background color"),this,SLOT(changeConsoleBgColor()));
				consoleColorMenu->addAction(tr("Text color"),this,SLOT(changeConsoleTextColor()));
				consoleColorMenu->addAction(tr("Output color"),this,SLOT(changeConsoleMsgColor()));
				consoleColorMenu->addAction(tr("Error message color"),this,SLOT(changeConsoleErrorMsgColor()));
			}
		}

		parsersMenu = 0;
		c_api_slots = new C_API_Slots(this);
		
		readSettings();
	}

	ConsoleWindow * MainWindow::console() const
	{
	    return consoleWindow;
	}

	void MainWindow::saveSettings()
	{
		QCoreApplication::setOrganizationName(ORGANIZATIONNAME);
		QCoreApplication::setOrganizationDomain(PROJECTWEBSITE);
		QCoreApplication::setApplicationName(ORGANIZATIONNAME);

		QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);

		settings.beginGroup("MainWindow");
		
		settings.setValue("home", homeDirPath);
		settings.setValue("size", size());
		settings.setValue("pos", pos());
		settings.setValue("maximized",(isMaximized()));
		settings.setValue("previousFileName", previousFileName);
		settings.setValue("defaultToolWindowOption", (int)(defaultToolWindowOption));
		settings.setValue("defaultHistoryWindowOption", (int)(defaultHistoryWindowOption));
		settings.setValue("defaultConsoleWindowOption", (int)(defaultConsoleWindowOption));
		settings.setValue("windowState", saveState());

		settings.endGroup();
	}

	void MainWindow::readSettings()
	{
		QCoreApplication::setOrganizationName(ORGANIZATIONNAME);
		QCoreApplication::setOrganizationDomain(PROJECTWEBSITE);
		QCoreApplication::setApplicationName(ORGANIZATIONNAME);

		QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);

		settings.beginGroup("MainWindow");
		
		QDir dir = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
		QString home = settings.value("home", tr("")).toString();
		
		if (!home.isNull() && !home.isEmpty() && home.contains(dir.absolutePath()))  
			homeDirPath = home;

		resize(settings.value("size", QSize(1000, 800)).toSize());
		move(settings.value("pos", QPoint(100, 100)).toPoint());
		if (settings.value("maximized",false).toBool()) showMaximized();
		previousFileName = settings.value("previousFileName", tr("")).toString();
		defaultToolWindowOption = (TOOL_WINDOW_OPTION)(settings.value("defaultToolWindowOption", (int)defaultToolWindowOption).toInt());
		defaultHistoryWindowOption = (TOOL_WINDOW_OPTION)(settings.value("defaultHistoryWindowOption", (int)defaultHistoryWindowOption).toInt());
		defaultConsoleWindowOption = (TOOL_WINDOW_OPTION)(settings.value("defaultConsoleWindowOption", (int)defaultConsoleWindowOption).toInt());
		
		restoreState(settings.value("windowState").toByteArray());

		settings.endGroup();
	}

	/*! \brief destructor*/
	MainWindow::~MainWindow()
	{
		GraphicsScene::clearStaticItems();
		saveSettings();

		QString tempDir = MainWindow::tempDir();
		QString cmd;

#ifdef Q_WS_WIN

		tempDir.replace(tr("/"),tr("\\"));
		cmd = tr("del /s /f /q \"") + tempDir + tr("\"");
		
#else

		cmd = tr("rm -R ") + tempDir;

#endif
		int r = system(cmd.toAscii().data());
		
		if (c_api_slots)
			delete c_api_slots;
	}

	void MainWindow::tabIndexChanged(int i)
	{
		QWidget * w = tabWidget->currentWidget();
		if (w)
			setCurrentWindow(static_cast<NetworkWindow*>(w));
	}

	void MainWindow::setCurrentWindow(NetworkWindow * window)
	{
		if (window && window->network && allNetworks.contains(window->network))
		{
			if (tabWidget)
			{
				int i = tabWidget->indexOf(window);
				if (i > -1 && i < tabWidget->count() && i != tabWidget->currentIndex())
					tabWidget->setCurrentIndex(i);				
			}
			
			historyWindow.setStack(&(window->network->history));
			
			NetworkWindow * oldWindow = currentNetworkWindow;

			currentNetworkWindow = window;
	
			if (!window->isVisible())
				window->show();
			
			if (!window->hasFocus())
				window->setFocus();

			if (window != oldWindow)
			{
				//emit escapeSignal(this);
				if (window->scene)
					if (oldWindow && oldWindow->scene)
						window->scene->useDefaultBehavior = oldWindow->scene->useDefaultBehavior;
					else
						window->scene->useDefaultBehavior = GraphicsScene::USE_DEFAULT_BEHAVIOR;				
				
				emit windowChanged(oldWindow,window);
			}
		}
		else
			currentNetworkWindow = 0;
	}
	
	QList<NetworkHandle*> MainWindow::networks() const
	{
		return allNetworks;
	}

	GraphicsScene * MainWindow::newScene()
	{
		NetworkHandle * network = new NetworkHandle(this);
		GraphicsScene * scene = network->createScene();
		
		if (scene)
		{
			NetworkWindow * subWindow = scene->networkWindow;
			popIn(subWindow);
			emit networkOpened(subWindow->network);
		}
		
		return scene;
	}

	TextEditor * MainWindow::newTextEditor()
	{
		NetworkHandle * network = new NetworkHandle(this);
		TextEditor * editor = network->createTextEditor();
		
		if (editor)
		{
			NetworkWindow * subWindow = editor->networkWindow;
			popIn(subWindow);
			emit networkOpened(subWindow->network);
		}
		return editor;
	}

	void MainWindow::allowMultipleViewModes(bool b)
	{
		allowViewModeToChange = b;
	}

	void MainWindow::closeWindow()
	{
		if (currentNetworkWindow)
		{
			currentNetworkWindow->close();
		}
	}

	void MainWindow::saveWindow()
	{
		if (!currentNetworkWindow || !currentNetworkWindow->network) return;

		bool b = false;
		emit prepareNetworkForSaving(currentNetworkWindow->network,&b);

		if (!b) return;

		QString fileName = currentNetworkWindow->filename;

		if (fileName.isEmpty())
		{
			fileName =
				QFileDialog::getSaveFileName(this, tr("Save Current Network"),
				previousFileName,
				(PROJECTNAME + tr(" files (*.") + OPEN_FILE_EXTENSIONS.join(tr(" *.")) + tr(")")));
			if (fileName.isNull() || fileName.isEmpty())
				return;
			else
				currentNetworkWindow->filename = previousFileName = fileName;
		}
		QFile file (fileName);

		if (!file.open(QFile::WriteOnly | QFile::Text)) {
			QMessageBox::warning(this, (PROJECTNAME + tr(" files")),
				tr("Cannot write file %1:\n%2.")
				.arg(fileName)
				.arg(file.errorString()));
			return;
		}

		emit saveNetwork(fileName);
	}

	void MainWindow::saveWindowAs()
	{
		if (!currentNetworkWindow || !currentNetworkWindow->network) return;
		
		QString def = currentNetworkWindow->filename;
		def.replace(QRegExp("\\..*$"),tr(".") + SAVE_FILE_EXTENSIONS.join(tr(" *.")));

		QString fileName =
			QFileDialog::getSaveFileName(this, tr("Save Current Network"),
			def,
			(PROJECTNAME + tr(" files (*.") + SAVE_FILE_EXTENSIONS.join(tr(" *.")) + tr(")")));
		if (fileName.isEmpty())
			return;

		currentNetworkWindow->filename = previousFileName = fileName;
		QFile file (fileName);

		if (!file.open(QFile::WriteOnly | QFile::Text)) {
			QMessageBox::warning(this, (PROJECTNAME + tr(" files")),
				tr("Cannot write file %1:\n%2.")
				.arg(fileName)
				.arg(file.errorString()));
			return;
		}

		emit saveNetwork(fileName);
	}

	void MainWindow::open(const QString& fileName)
	{
		QFile file (fileName);

		if (!file.open(QFile::ReadOnly | QFile::Text)) {
			QMessageBox::warning(this, tr("Tinkercell File"),
				tr("Cannot read file %1:\n%2.")
				.arg(fileName)
				.arg(file.errorString()));
			return;
		}

		previousFileName = fileName;
		emit loadNetwork(fileName);
		
		if (currentNetworkWindow)
			currentNetworkWindow->filename = fileName;
	}

	void MainWindow::open()
	{
		QString def = previousFileName;
		def.replace(QRegExp("\\..*$"),tr(".") + OPEN_FILE_EXTENSIONS.join(tr(" *.")));

		QStringList fileNames =
			QFileDialog::getOpenFileNames(this, tr("Open File"),
			def);
		for (int i=0; i < fileNames.size(); ++i)
			if (!fileNames[i].isEmpty())
				open(fileNames[i]);
	}

	/*! \brief print the current scene*/
	void MainWindow::print()
	{
		QPrinter printer(QPrinter::HighResolution);
		//printer.setResolution(300);
		printer.setPageSize(QPrinter::B0);

		if (QPrintDialog(&printer,this).exec() == QDialog::Accepted)
		{
			if (currentScene())
				currentScene()->print(&printer);
			else
				if (currentTextEditor())
					currentTextEditor()->print(&printer);

		}
	}

	/*! \brief print the current scene*/
	void MainWindow::printToFile()
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		QString def = previousFileName;
		def.replace(QRegExp("\\..*$"),tr(""));

		QString fileName =
			QFileDialog::getSaveFileName(this, tr("Print to File"),
			def,
			//tr("PDF Files (*.pdf *.PDF)"));
			tr("PNG Files (*.png *.PNG)"));
		if (fileName.isEmpty())
			return;

		previousFileName = fileName;


		/*QPrinter printer(QPrinter::HighResolution);
		//printer.setResolution(300);
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOrientation(QPrinter::Landscape);
		//printer.setPageSize(QPrinter::A4);
		printer.setPageSize(QPrinter::B0);
		printer.setOutputFileName(fileName);
		*/
		/*
		QSvgGenerator printer;
		printer.setFileName(fileName);
		*/

		QRectF viewport = scene->viewport();
		int w = 2048;
		int h = (int)(viewport.height() * w/viewport.width());
		QImage printer(w,h,QImage::Format_ARGB32);
		scene->print(&printer);
		printer.save(fileName,"png");
	}

	QDockWidget * MainWindow::addToolWindow(QWidget * tool, TOOL_WINDOW_OPTION option, Qt::DockWidgetArea initArea, Qt::DockWidgetAreas allowedAreas, bool inMenu)
	{
		if (!tool || toolWindows.contains(tool)) return 0;

		toolWindows << tool;

		if (option == DockWidget)
		{
			QDockWidget *dock = new QDockWidget(tool->windowTitle(), this);
			dock->setObjectName(tool->windowTitle());
			dock->setWindowIcon(tool->windowIcon());
			dock->setAllowedAreas(allowedAreas);
			dock->setWidget(tool);
			addDockWidget(initArea,dock);
			if (inMenu)
				viewMenu->addAction(dock->toggleViewAction());

			return dock;
		}

		QDockWidget * dock = 0;

		if (!toolBox || option == NewToolBoxWidget)
		{
			dock = new QDockWidget(tr("Tools Window"), this);
			dock->setObjectName(tool->windowTitle());
			if (option == NewToolBoxWidget)
				dock->setWindowTitle(tool->windowTitle());
			toolBox = new QToolBox;
			toolBox->setMinimumWidth(300);
			dock->setWidget(toolBox);
			dock->setAllowedAreas(allowedAreas);
			addDockWidget(initArea,dock);
			if (inMenu)
				viewMenu->addAction(dock->toggleViewAction());
		}
		else
		{
			dock = static_cast<QDockWidget*>(toolBox->parentWidget()); //safe?
		}

		toolBox->addItem(tool,tool->windowIcon(),tool->windowTitle());
		toolBox->setCurrentWidget(tool);

		return dock;
	}

	Tool * MainWindow::tool(const QString& s0) const
	{
		QString s = s0.toLower();
		if (toolsHash.contains(s))
			return toolsHash.value(s);
		return 0;
	}

	QList<Tool*> MainWindow::tools() const
	{
		return toolsHash.values();
	}

	void MainWindow::addTool(Tool * tool)
	{
		if (!tool) return;

		bool add = true;
		emit toolAboutToBeLoaded(tool,&add);

		if (!toolsHash.contains(tool->name.toLower()) && add)
		{
			toolsHash.insert(tool->name.toLower(),tool);
			if (tool->mainWindow != this)
			{
				tool->setMainWindow( static_cast<MainWindow*>(this) );
			}
			emit toolLoaded(tool);
		}
		else
		if (!add)
		{
			if (toolsHash.contains(tool->name.toLower()))
				toolsHash.remove(tool->name.toLower());
			if (!tool->parentWidget())
				delete tool;
		}
	}
	
	NetworkWindow * MainWindow::currentWindow() const
	{
		return currentNetworkWindow;
	}

	GraphicsScene* MainWindow::currentScene() const
	{
		if (currentNetworkWindow)
			return currentNetworkWindow->scene;
		return 0;
	}

	TextEditor* MainWindow::currentTextEditor() const
	{
		if (currentNetworkWindow)
			return currentNetworkWindow->editor;
		return 0;
	}

	NetworkHandle * MainWindow::currentNetwork() const
	{
		if (currentNetworkWindow)
		    return currentNetworkWindow->network;
		return 0;
	}
	
	void MainWindow::initializeMenus(bool enableScene, bool enableText)
	{
		fileMenu = menuBar()->addMenu(tr("&File"));
		toolBarBasic = new QToolBar(tr("Open/save/new toolbar"),this);
		toolBarBasic->setObjectName(tr("Open/save/new toolbar"));
		
		toolBarEdits = new QToolBar(tr("Edit options"),this);
		toolBarEdits->setObjectName(tr("Edit options"));
		
		toolBarForTools = new QToolBar(tr("Plug-ins"),this);
		toolBarForTools->setObjectName(tr("Plug-ins"));

		if (enableScene)
		{
			QAction* newAction = fileMenu->addAction(QIcon(tr(":/images/new.png")),tr("&New canvas"));
			newAction->setShortcut(QKeySequence::New);
			connect (newAction, SIGNAL(triggered()),this,SLOT(newScene()));
			toolBarBasic->addAction(newAction);
		}

		if (enableText && !enableScene)
		{
			QAction* newAction2 = fileMenu->addAction(QIcon(tr(":/images/new.png")),tr("New text &editor"));
			newAction2->setShortcut(tr("CTRL+SHIFT+N"));
			connect (newAction2, SIGNAL(triggered()),this,SLOT(newTextEditor()));
			toolBarBasic->addAction(newAction2);
		}

		QAction* openAction = fileMenu->addAction(QIcon(tr(":/images/open.png")),tr("&Open"));
		openAction->setShortcut(QKeySequence::Open);
		connect (openAction, SIGNAL(triggered()),this,SLOT(open()));

		QAction* saveAction = fileMenu->addAction(QIcon(tr(":/images/save.png")),tr("&Save"));
		saveAction->setShortcut(QKeySequence::Save);
		connect (saveAction, SIGNAL(triggered()),this,SLOT(saveWindow()));

		QAction* saveAsAction = fileMenu->addAction(QIcon(tr(":/images/save.png")),tr("Save &As"));
		connect (saveAsAction, SIGNAL(triggered()),this,SLOT(saveWindowAs()));

		QAction* closeAction = fileMenu->addAction(QIcon(tr(":/images/close.png")), tr("&Close page"));
		closeAction->setShortcut(QKeySequence::Close);
		connect (closeAction, SIGNAL(triggered()),this,SLOT(closeWindow()));

		fileMenu->addSeparator();

		QAction * printAction = fileMenu->addAction(QIcon(tr(":/images/print.png")),tr("&Print"));
		printAction->setShortcut(QKeySequence::Print);
		connect(printAction,SIGNAL(triggered()),this,SLOT(print()));

		QAction * printToFileAction = fileMenu->addAction(QIcon(tr(":/images/camera.png")),tr("Screens&hot"));
		printToFileAction->setShortcut(tr("Ctrl+F5"));
		connect(printToFileAction,SIGNAL(triggered()),this,SLOT(printToFile()));

		fileMenu->addSeparator();

		QAction * exitAction = fileMenu->addAction(QIcon(tr(":/images/exit.png")), tr("E&xit"));
		exitAction->setShortcut(tr("Ctrl+Q"));
		connect(exitAction,SIGNAL(triggered()),this,SLOT(close()));

		editMenu = menuBar()->addMenu(tr("&Edit"));
		QAction * undoAction = editMenu->addAction(QIcon(tr(":/images/undo.png")),tr("&Undo"));
		undoAction->setShortcut(QKeySequence::Undo);
		connect(undoAction,SIGNAL(triggered()),this,SLOT(undo()));

		QAction * redoAction = editMenu->addAction(QIcon(tr(":/images/redo.png")),tr("&Redo"));
		redoAction->setShortcut(QKeySequence::Redo);
		connect(redoAction,SIGNAL(triggered()),this,SLOT(redo()));
		editMenu->addSeparator();

		viewMenu = menuBar()->addMenu(tr("&View"));

		optionsMenu = menuBar()->addMenu(tr("&Options"));
		QAction * changeUserHome = optionsMenu->addAction(QIcon(tr(":/images/appicon.png")), tr("&Set Home Directory"));
		connect (changeUserHome, SIGNAL(triggered()),this,SLOT(setUserHome()));

		QMenu * setGridModeMenu = optionsMenu->addMenu(tr("&Grid mode"));
		QActionGroup * actionGroup = new QActionGroup(this);
		
		QAction * gridOn = setGridModeMenu->addAction(tr("Grid ON"),this,SLOT(gridOn()));
		QAction * gridOff = setGridModeMenu->addAction(tr("Grid OFF"),this,SLOT(gridOff()));
		QAction * gridSz = setGridModeMenu->addAction(tr("Grid size"),this,SLOT(setGridSize()));
		
		gridOn->setCheckable(true);
		gridOff->setCheckable(true);
		gridSz->setCheckable(true);

		actionGroup->addAction(gridOn);
		actionGroup->addAction(gridOff);
		actionGroup->addAction(gridSz);
		actionGroup->setExclusive(true);

		helpMenu = menuBar()->addMenu(tr("&Help"));

		QAction * copyAction = new QAction(QIcon(":/images/copy.png"),tr("&Copy"),this);
		editMenu->addAction(copyAction);
		copyAction->setToolTip(tr("Copy selected items"));
		copyAction->setShortcut(QKeySequence::Copy);
		connect(copyAction,SIGNAL(triggered()),this,SLOT(copy()));

		QAction * cutAction = new QAction(QIcon(":/images/cut.png"),tr("Cu&t"),this);
		editMenu->addAction(cutAction);
		cutAction->setToolTip(tr("Cut selected items"));
		cutAction->setShortcut(QKeySequence::Cut);
		connect(cutAction,SIGNAL(triggered()),this,SLOT(cut()));

		QAction * pasteAction = new QAction(QIcon(":/images/paste.png"),tr("&Paste"),this);
		editMenu->addAction(pasteAction);
		pasteAction->setToolTip(tr("Paste copied items"));
		pasteAction->setShortcut(QKeySequence::Paste);
		connect(pasteAction,SIGNAL(triggered()),this,SLOT(paste()));

		QAction * deleteAction = new QAction(QIcon(":/images/delete.png"),tr("&Delete"),this);
		editMenu->addAction(deleteAction);
		deleteAction->setToolTip(tr("Delete selected items"));
		connect(deleteAction,SIGNAL(triggered()),this,SLOT(remove()));

		QAction * selectAllAction = new QAction(tr("Select all"),this);
		editMenu->addAction(selectAllAction);
		selectAllAction->setToolTip(tr("Select all items"));
		selectAllAction->setShortcut(QKeySequence::SelectAll);
		connect(selectAllAction,SIGNAL(triggered()),this,SLOT(selectAll()));

		QAction* arrowAction = editMenu->addAction(QIcon(tr(":/images/arrow.png")),tr("Cursor"));
		connect(arrowAction,SIGNAL(triggered()),this,SLOT(sendEscapeSignal()));

		toolBarBasic->addAction(openAction);
		toolBarBasic->addAction(closeAction);
		toolBarBasic->addAction(saveAction);
		toolBarBasic->addAction(printAction);
		toolBarBasic->addAction(printToFileAction);		

		toolBarEdits->addAction(arrowAction);
		toolBarEdits->addAction(undoAction);
		toolBarEdits->addAction(redoAction);
		toolBarEdits->addAction(copyAction);
		toolBarEdits->addAction(cutAction);
		toolBarEdits->addAction(pasteAction);
		toolBarEdits->addAction(deleteAction);
		
		/*QSize iconSize(16,16);
		toolBarBasic->setIconSize(iconSize);
		toolBarEdits->setIconSize(iconSize);
		toolBarForTools->setIconSize(iconSize);*/

		addToolBar(Qt::TopToolBarArea, toolBarBasic);
		addToolBar(Qt::TopToolBarArea, toolBarEdits);
		addToolBar(Qt::TopToolBarArea, toolBarForTools);

		contextScreenMenu.addAction(closeAction);
		contextScreenMenu.addAction(undoAction);
		contextScreenMenu.addAction(redoAction);

		contextEditorMenu.addAction(undoAction);
		contextEditorMenu.addAction(redoAction);
		contextEditorMenu.addAction(closeAction);

		contextItemsMenu.addAction(copyAction);
		contextItemsMenu.addAction(cutAction);
		contextScreenMenu.addAction(pasteAction);
		contextItemsMenu.addAction(deleteAction);

		contextSelectionMenu.addAction(copyAction);
		contextSelectionMenu.addAction(cutAction);
		contextEditorMenu.addAction(pasteAction);

		contextScreenMenu.addAction(closeAction);

		contextScreenMenu.addAction(undoAction);
		contextScreenMenu.addAction(redoAction);
	}

	void MainWindow::sendEscapeSignal(const QWidget * widget)
	{
		emit escapeSignal(widget);
	}

	QUndoStack * MainWindow::historyStack() const
	{
		if (historyWindow.stack())
			return historyWindow.stack();
		return 0;
	}

	QUndoView * MainWindow::historyWidget()
	{
		return &historyWindow;
	}

	void MainWindow::undo()
	{
		if (historyWindow.stack())
			historyWindow.stack()->undo();
	}

	void MainWindow::redo()
	{
		if (historyWindow.stack())
			historyWindow.stack()->redo();
	}
	void MainWindow::copy()
	{
		if (currentScene())
			currentScene()->copy();
		else
			if (currentTextEditor())
				currentTextEditor()->copy();
	}

	void MainWindow::cut()
	{
		if (currentScene())
			currentScene()->cut();
		else
			if (currentTextEditor())
				currentTextEditor()->cut();
	}

	void MainWindow::remove()
	{
		if (currentScene())
			currentScene()->removeSelected();
	}

	void MainWindow::selectAll()
	{
		if (currentScene())
			currentScene()->selectAll();
		else
			if (currentTextEditor())
				currentTextEditor()->selectAll();
	}

	void MainWindow::paste()
	{
		if (currentScene())
			currentScene()->paste();
		else
			if (currentTextEditor())
				currentTextEditor()->paste();
	}

	void MainWindow::closeEvent(QCloseEvent *event)
	{
		bool b = true;
		QList<NetworkHandle*> list = allNetworks;
		currentNetworkWindow = 0;
		for (int i=0; i < list.size(); ++i)
			if (list[i] && allNetworks.contains(list[i]))
			{
				b = true;
				emit networkClosing(list[i],&b);
				if (b)
				{
					emit networkClosed(list[i]);
					disconnect(list[i]);
					allNetworks.removeAll(list[i]);
					list[i]->close();
				}
				else
				if (list[i]->networkWindows.size() > 0 && list[i]->networkWindows[0]) 
				{
					currentNetworkWindow = list[i]->networkWindows[0];
					event->ignore();
					return;
				}
			}

		if (tabWidget)
			tabWidget->clear();

		QList<QString> keys = this->toolsHash.keys();
		QList<Tool*> toolsHash = this->toolsHash.values();

		for (int i=0; i < toolsHash.size(); ++i)
		{
			if (toolsHash[i])
				disconnect(toolsHash[i]);
		}

		for (int i=0; i < toolsHash.size(); ++i)
		{
			if (toolsHash[i])
			{
				if (toolsHash[i]->parentWidget() == 0)
				{
					for (int j=0; j < toolsHash.size(); ++j)
						if (i != j && toolsHash[j] == toolsHash[i])
							toolsHash[j] = 0;

					toolsHash[i]->close();
					delete toolsHash[i];
				}
			}
		}

		event->accept();
	}

	void MainWindow::loadFiles(const QList<QFileInfo>& files)
	{
		for (int i=0; i < files.size(); ++i)
		{
			if (files[i].isFile())
			{
				if (QLibrary::isLibrary(files[i].fileName()))
				{
					loadDynamicLibrary(files[i].absoluteFilePath());
				}
				else
				{
					previousFileName = files[i].absoluteFilePath();
					emit loadNetwork(files[i].absoluteFilePath());
				}
			}
		}

		emit filesLoaded(files);
	}

	void MainWindow::itemsRemovedSlot(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<ItemHandle*>& handles)
	{
		if (handles.size() > 0 && scene)
			emit itemsRemoved(scene->network, handles);
	}

	void MainWindow::itemsInsertedSlot(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<ItemHandle*>& handles)
	{
		if (handles.size() > 0 && scene)
			emit itemsInserted(scene->network, handles);
	}
	
	void MainWindow::addParser(TextParser * parser)
	{
		static QActionGroup * actionGroup = 0;

		if (!parser) return;

		if (!parsersMenu)
		{

			parsersMenu = new QMenu(tr("&Parsers"));
			menuBar()->insertMenu(helpMenu->menuAction(),parsersMenu);
		}

		if (!actionGroup)
		{
			actionGroup = new QActionGroup(this);
			actionGroup->setExclusive(true);
		}

		QAction * action = parsersMenu->addAction(QIcon(parser->icon),parser->name);
		connect(action,SIGNAL(triggered()),parser,SLOT(activate()));
		action->setCheckable(true);
		actionGroup->addAction(action);
		action->setChecked(true);

		TextParser::setParser(parser);
	}

	void MainWindow::gridOff()
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		scene->disableGrid();
	}

	void MainWindow::gridOn()
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		if (GraphicsScene::GRID == 0)
			GraphicsScene::GRID = 100;

		scene->enableGrid(GraphicsScene::GRID);
	}

	void MainWindow::setGridSize()
	{
		GraphicsScene * scene = currentScene();
		if (!scene) return;

		bool ok;
		int d = QInputDialog::getInteger (this,tr("Grid size"),tr("Set canvas grid size"),
											GraphicsScene::GRID,0,(int)(currentScene()->sceneRect().width()/10.0),1,&ok);
		if (ok)
		{
			GraphicsScene::GRID = d;
			scene->setGridSize(GraphicsScene::GRID);
		}
	}

	void MainWindow::changeConsoleBgColor()
	{
		if (consoleWindow && consoleWindow->editor())
		{
			QColor color = QColorDialog::getColor(ConsoleWindow::BackgroundColor,this);
			consoleWindow->editor()->setBackgroundColor(color);
		}
	}

	void MainWindow::changeConsoleTextColor()
	{
		if (consoleWindow && consoleWindow->editor())
		{
			QColor color = QColorDialog::getColor(ConsoleWindow::PlainTextColor,this);
			consoleWindow->editor()->setPlainTextColor(color);
		}
	}

	void MainWindow::changeConsoleMsgColor()
	{
		if (consoleWindow && consoleWindow->editor())
		{
			QColor color = QColorDialog::getColor(ConsoleWindow::OutputTextColor,this);
			consoleWindow->editor()->setOutputTextColor(color);
		}
	}

	void MainWindow::changeConsoleErrorMsgColor()
	{
		if (consoleWindow && consoleWindow->editor())
		{
			QColor color = QColorDialog::getColor(ConsoleWindow::ErrorTextColor,this);
			consoleWindow->editor()->setErrorTextColor(color);
		}
	}

	void MainWindow::popOut()
	{
		popOut(currentNetworkWindow);
	}

	void MainWindow::popOut(NetworkWindow * win)
	{
		if (win)
		{
			if (allowViewModeToChange && tabWidget)
			{
				int i = tabWidget->indexOf(win);
				if (i > -1 && i < tabWidget->count())
				{
					tabWidget->removeTab(i);
					win->setParent(this);
				}
				win->setWindowFlags(Qt::Window);
				setCurrentWindow(win);
			}
			win->raise();
		}
	}

	void MainWindow::popIn(NetworkWindow * win)
	{
		if (allowViewModeToChange && win && tabWidget)
		{
			int i = tabWidget->indexOf(win);
			if (i == -1)
			{
				//win->setParent(0);
				win->setWindowFlags(Qt::Widget);
				tabWidget->addTab(win,win->windowIcon(),win->windowTitle());
				setCurrentWindow(win);
			}
		}
	}

	void MainWindow::setCursor(QCursor cursor)
	{
		QMainWindow::setCursor(cursor);
		for (int i=0; i < allNetworks.size(); ++i)
			if (allNetworks[i])
			{
				QList<NetworkWindow*> windows = allNetworks[i]->networkWindows;
				for (int j=0; j < windows.size(); ++j)
				{
					windows[j]->setCursor(cursor);
					if (windows[j]->scene)
					{
						QList<QGraphicsView*> views = windows[j]->scene->views();
						for (int k=0; k < views.size(); ++k)
					        if (views[k])
					            views[k]->setCursor(cursor);
					}
				}
			}
	}

	void MainWindow::setupFunctionPointersSlot(QSemaphore* s,QLibrary * library)
	{
		emit setupFunctionPointers(library); //let all the other plug-ins setup their functions

		if (s)
			s->release();
	}

	void MainWindow::RegisterDataTypes()
	{
		//register new signal/slot data types
		qRegisterMetaType< QList<QGraphicsItem*> >("QList<QGraphicsItem*>");
		qRegisterMetaType< QStringList >("QStringList");

		qRegisterMetaType< QList<QGraphicsItem*> >("QList<QGraphicsItem*>&");
		qRegisterMetaType< QStringList >("QStringList&");

		qRegisterMetaType< QList<QGraphicsItem*>* >("QList<QGraphicsItem*>*");
		qRegisterMetaType< QStringList* >("QStringList*");

		qRegisterMetaType< DataTable<qreal> >("DataTable<qreal>");
		qRegisterMetaType< DataTable<QString> >("DataTable<QString>");
		
		qRegisterMetaType< NumericalDataTable >("NumericalDataTable");
		qRegisterMetaType< TextDataTable >("TextDataTable");
		
		qRegisterMetaType< NumericalDataTable >("NumericalDataTable&");
		qRegisterMetaType< TextDataTable >("TextDataTable&");

		qRegisterMetaType< DataTable<qreal> >("DataTable<qreal>&");
		qRegisterMetaType< DataTable<QString> >("DataTable<QString>&");		

		qRegisterMetaType< DataTable<qreal>* >("DataTable<qreal>*");
		qRegisterMetaType< DataTable<QString>* >("DataTable<QString>*");
		
		qRegisterMetaType< NumericalDataTable* >("NumericalDataTable*");
		qRegisterMetaType< TextDataTable* >("TextDataTable*");

		qRegisterMetaType< ItemHandle* >("ItemHandle*");
		qRegisterMetaType< QList<ItemHandle*> >("QList<ItemHandle*>");
		qRegisterMetaType< QList<ItemHandle*> >("QList<ItemHandle*>&");

		qRegisterMetaType< Tool* >("Tool*");
		qRegisterMetaType< QList<Tool*> >("QList<Tool*>");
		qRegisterMetaType< QList<Tool*> >("QList<Tool*>&");

		qRegisterMetaType< QList<QStringList> >("QList<QStringList>");
		qRegisterMetaType< QList<QStringList> >("QList<QStringList>&");

		qRegisterMetaType< MatrixInputFunction >("MatrixInputFunction");


		qRegisterMetaType< tc_matrix >("tc_matrix");
		qRegisterMetaType< tc_table >("tc_table");
		qRegisterMetaType< tc_strings >("tc_strings");
		qRegisterMetaType< tc_items >("tc_items");
	}
	
	QHash<void*,bool> MainWindow::invalidPointers;
	
	bool MainWindow::isValidHandlePointer(void * p)
	{
		if (!p || MainWindow::invalidPointers.contains(p)) return false;

		for (int i=0; i < allNetworks.size(); ++i)
			if (allNetworks[i] && allNetworks[i]->symbolsTable.isValidPointer(p))
				return true;

		return false;
	}

	void MainWindow::dragEnterEvent(QDragEnterEvent *event)
	{
		//event->accept();
	}

	void MainWindow::dropEvent(QDropEvent * event)
	{
		//event->accept();
	}
	
	QList<ItemHandle*> MainWindow::getItemsFromFile(const QString& filename, ItemHandle * root)
	{
		QList<ItemHandle*> items;
		emit getItemsFromFile(items,filename,root);
		return items;
	}
}
