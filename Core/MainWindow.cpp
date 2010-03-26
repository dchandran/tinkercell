/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This is source file for Tinkercell's main window
The MainWindow contains a set of GraphicScenes, which is the class
that performs all the drawing. Each GraphicsScene emits various signals. Those
signals are then emitted by the MainWindow; in this way, a plugin does not need
signals are then emitted by the MainWindow; in this way, a plugin does not need
to listen to each of the GraphicsScene signals but only the MainWindow's signals.

The MainWindow also has its own signals, such as a toolLoaded, modelSaved, etc.

The MainWindow keeps a list of all plugins, and it is also responsible for loading plugins.


****************************************************************************/
#include <QDebug>
#include <QLibrary>
#include <QSettings>
#include <QInputDialog>
#include <QActionGroup>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QtDebug>
#include <QSvgGenerator>
#include <QColorDialog>
#include <QImage>
#include "TextEditor.h"
#include "TextItem.h"
#include "NetworkWindow.h"
#include "GraphicsScene.h"
#include "GraphicsView.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "MainWindow.h"
#include "CThread.h"
#include "MultithreadedSliderWidget.h"
#include "ConsoleWindow.h"
#include "AbstractInputWindow.h"
#include "TextParser.h"

namespace Tinkercell
{
	typedef void (*TinkercellPluginEntryFunction)(MainWindow*);
	typedef void (*TinkercellCEntryFunction)();

	/********** GLOBAL VARIABLES **********/

	MainWindow::TOOL_WINDOW_OPTION MainWindow::defaultToolWindowOption = MainWindow::ToolBoxWidget;
	MainWindow::TOOL_WINDOW_OPTION MainWindow::defaultHistoryWindowOption = MainWindow::ToolBoxWidget;
	MainWindow::TOOL_WINDOW_OPTION MainWindow::defaultConsoleWindowOption = MainWindow::DockWidget;
	QString MainWindow::previousFileName;
	QString MainWindow::defaultFileExtension("tic");
	QString MainWindow::userHomePath;
	/*************************************/

	QString MainWindow::userHome()
	{
		if (!userHomePath.isEmpty() && QDir(userHomePath).exists())
			return userHomePath;

		QDir dir = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
		QString tcdir = PROJECTNAME;

		if (!dir.exists(tcdir))
			dir.mkdir(tcdir);

		dir.cd(tcdir);
		userHomePath = dir.absolutePath();

		return userHomePath;
	}

	QString MainWindow::userTemp()
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
		QString home = QFileDialog::getExistingDirectory(this,tr("Select new user home directory"),userHome());
		if (home.isEmpty() || home.isNull()) return;

		QCoreApplication::setOrganizationName(ORGANIZATIONNAME);
		QCoreApplication::setOrganizationDomain(PROJECTWEBSITE);
		QCoreApplication::setApplicationName(ORGANIZATIONNAME);
		QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);

		settings.beginGroup("MainWindow");
		settings.setValue("home", home);
		userHomePath = home;
		settings.endGroup();
	}

	typedef void (*progress_api_initialize)(void (*tc_showProgress)(const char *, int));

	void MainWindow::loadDynamicLibrary(const QString& dllFile)
	{
		QString home = userHome(),
			temp = userTemp(),
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
						emit setupFunctionPointersSlot(0,lib);
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
			statusBar()->showMessage(lib->fileName() + tr(" could not be opened"));
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

		readSettings();

		getStringDialog = 0;
        consoleWindow = 0;
		currentNetworkWindow = 0;
		toolBox = 0;
		setAutoFillBackground(true);
		setAcceptDrops(true);

		initializeMenus(enableScene,enableText);
		//setIconSize(QSize(25,25));

		tabWidget = new QTabWidget;
		tabWidget->setStyleSheet(tr("QTabBar::tab { min-width: 36ex; }"));
		connect(tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabIndexChanged(int)));

		QToolButton * upButton = new QToolButton;
		upButton->setIcon(QIcon(tr(":/images/rightarrow.png")));
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

		connect(this,SIGNAL(itemsInserted(TextEditor*,QList<TextItem*>,QList<ItemHandle*>)),
			this,SLOT(itemsInsertedSlot(TextEditor*,QList<TextItem*>,QList<ItemHandle*>)));

		connect(this,SIGNAL(itemsRemoved(TextEditor*,QList<TextItem*>,QList<ItemHandle*>)),
			this,SLOT(itemsRemovedSlot(TextEditor*,QList<TextItem*>,QList<ItemHandle*>)));

		if (showHistory)
		{
			historyWindow.setWindowTitle(tr("History"));
			historyWindow.setWindowIcon(QIcon(tr(":/images/undo.png")));
			addToolWindow(&historyWindow,MainWindow::defaultHistoryWindowOption,Qt::RightDockWidgetArea);
		}

		if (enableConsoleWindow)
		{
			consoleWindow = new ConsoleWindow(this);
			if (settingsMenu)
			{
				QMenu * consoleColorMenu = settingsMenu->addMenu(tr("Console window colors"));

				consoleColorMenu->addAction(tr("Background color"),this,SLOT(changeConsoleBgColor()));
				consoleColorMenu->addAction(tr("Text color"),this,SLOT(changeConsoleTextColor()));
				consoleColorMenu->addAction(tr("Output color"),this,SLOT(changeConsoleMsgColor()));
				consoleColorMenu->addAction(tr("Error message color"),this,SLOT(changeConsoleErrorMsgColor()));
			}
		}

		connectTCFunctions();

		parsersMenu = 0;
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
		settings.setValue("size", size());
		settings.setValue("pos", pos());
		settings.setValue("maximized",(isMaximized()));
		settings.setValue("previousFileName", previousFileName);
		settings.setValue("defaultToolWindowOption", (int)(defaultToolWindowOption));
		settings.setValue("defaultHistoryWindowOption", (int)(defaultHistoryWindowOption));
		settings.setValue("defaultConsoleWindowOption", (int)(defaultConsoleWindowOption));

		settings.endGroup();
	}

	void MainWindow::readSettings()
	{
		QCoreApplication::setOrganizationName(ORGANIZATIONNAME);
		QCoreApplication::setOrganizationDomain(PROJECTWEBSITE);
		QCoreApplication::setApplicationName(ORGANIZATIONNAME);

		QSettings settings(ORGANIZATIONNAME, ORGANIZATIONNAME);

		settings.beginGroup("MainWindow");

		resize(settings.value("size", QSize(1000, 800)).toSize());
		move(settings.value("pos", QPoint(100, 100)).toPoint());
		if (settings.value("maximized",false).toBool())
			showMaximized();
		previousFileName = settings.value("previousFileName", tr("")).toString();
		defaultToolWindowOption = (TOOL_WINDOW_OPTION)(settings.value("defaultToolWindowOption", (int)defaultToolWindowOption).toInt());
		defaultHistoryWindowOption = (TOOL_WINDOW_OPTION)(settings.value("defaultHistoryWindowOption", (int)defaultHistoryWindowOption).toInt());
		defaultConsoleWindowOption = (TOOL_WINDOW_OPTION)(settings.value("defaultConsoleWindowOption", (int)defaultConsoleWindowOption).toInt());

		settings.endGroup();
	}

	/*! \brief destructor*/
	MainWindow::~MainWindow()
	{
		GraphicsScene::clearStaticItems();
		saveSettings();

		QString tempDir = userTemp();
		QString cmd;

#ifdef Q_WS_WIN

		tempDir.replace(tr("/"),tr("\\"));
		cmd = tr("del \"") + tempDir + tr("\"\\*.* /q");

#else

		cmd = tr("rm ") + tempDir + tr("/*.*");

#endif
		int r = system(cmd.toAscii().data());
	}

	void MainWindow::tabIndexChanged(int i)
	{
		QWidget * w = tabWidget->currentWidget();
		if (w)
			setCurrentWindow(static_cast<NetworkWindow*>(w));
	}

	void MainWindow::setCurrentWindow(NetworkWindow * window)
	{
		currentNetworkWindow = 0;
		if (window && allNetworkWindows.contains(window))
		{
			if (tabWidget)
			{
				int i = tabWidget->indexOf(window);
				if (i > -1 && i < tabWidget->count() && i != tabWidget->currentIndex())
					tabWidget->setCurrentIndex(i);				
			}
			
			historyWindow.setStack(&(window->history));
			
			NetworkWindow * oldWindow = currentNetworkWindow;

			currentNetworkWindow = window;
			
			if (!window->hasFocus())
				window->setFocus();

			if (window != oldWindow)
			{
				emit escapeSignal(this);
				emit windowChanged(oldWindow,window);
			}
		}
	}

	GraphicsScene * MainWindow::newGraphicsWindow()
	{
		GraphicsScene * scene = new GraphicsScene;
		NetworkWindow * subWindow = new NetworkWindow(this, scene);

		if (!allNetworkWindows.contains(subWindow))
			allNetworkWindows << subWindow;

		connect (subWindow,SIGNAL(closing(NetworkWindow *, bool*)),this,SIGNAL(windowClosing(NetworkWindow *, bool*)));
		connect (subWindow,SIGNAL(closed(NetworkWindow *)),this,SIGNAL(windowClosed(NetworkWindow *)));

		popIn(subWindow);
		emit windowOpened(subWindow);

		return scene;
	}

	TextEditor * MainWindow::newTextWindow()
	{
		TextEditor * textedit = new TextEditor;
		NetworkWindow * subWindow = new NetworkWindow(this, textedit);

		if (!allNetworkWindows.contains(subWindow))
			allNetworkWindows << subWindow;
		connect (subWindow,SIGNAL(closing(NetworkWindow *, bool*)),this,SIGNAL(windowClosing(NetworkWindow *, bool*)));
		connect (subWindow,SIGNAL(closed(NetworkWindow *)),this,SIGNAL(windowClosed(NetworkWindow *)));

		popIn(subWindow);
		emit windowOpened(subWindow);

		return textedit;
	}

	void MainWindow::allowMultipleViewModes(bool b)
	{
		allowViewModeToChange = b;
	}

	void MainWindow::closeWindow()
	{
		if (currentWindow())
		{
			currentWindow()->close();
		}
	}

	void MainWindow::saveWindow()
	{
		NetworkWindow * win = currentWindow();

		if (!win) return;

		bool b = false;
		emit prepareModelForSaving(win,&b);

		if (!b) return;

		QString fileName = win->filename;

		if (fileName.isEmpty())
		{
			fileName =
				QFileDialog::getSaveFileName(this, tr("Save Current Model"),
				previousFileName,
				(PROJECTNAME + tr(" files (*.") + defaultFileExtension + tr(")")));
			if (fileName.isEmpty())
				return;
			else
				previousFileName = fileName;
		}
		QFile file (fileName);

		if (!file.open(QFile::WriteOnly | QFile::Text)) {
			QMessageBox::warning(this, (PROJECTNAME + tr(" files")),
				tr("Cannot write file %1:\n%2.")
				.arg(fileName)
				.arg(file.errorString()));
			return;
		}

		emit saveModel(fileName);
	}

	void MainWindow::saveWindowAs()
	{
		QString def = previousFileName;
		def.replace(QRegExp("\\..*$"),tr(".") + defaultFileExtension);

		QString fileName =
			QFileDialog::getSaveFileName(this, tr("Save Current Model"),
			def,
			(PROJECTNAME + tr(" files (*.") + defaultFileExtension + tr(")")));
		if (fileName.isEmpty())
			return;

		previousFileName = fileName;
		QFile file (fileName);

		if (!file.open(QFile::WriteOnly | QFile::Text)) {
			QMessageBox::warning(this, (PROJECTNAME + tr(" files")),
				tr("Cannot write file %1:\n%2.")
				.arg(fileName)
				.arg(file.errorString()));
			return;
		}

		emit saveModel(fileName);
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
		emit loadModel(fileName);
	}

	void MainWindow::open()
	{
		QString def = previousFileName;
		def.replace(QRegExp("\\..*$"),tr(".") + defaultFileExtension);

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

	Tool * MainWindow::tool(const QString& s) const
	{
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

		if (!toolsHash.contains(tool->name) && add)
		{
			toolsHash.insert(tool->name,tool);
			if (tool->mainWindow != this)
			{
				tool->setMainWindow( static_cast<MainWindow*>(this) );
			}
			emit toolLoaded(tool);
		}
		else
		if (!add)
		{
			if (toolsHash.contains(tool->name))
				toolsHash.remove(tool->name);
			if (!tool->parentWidget())
				delete tool;
		}
	}

	GraphicsScene* MainWindow::currentScene() const
	{
		NetworkWindow * net = currentWindow();
		if (net)
			return net->scene;
		return 0;
	}

	GraphicsView* MainWindow::currentView() const
	{
		NetworkWindow * net = currentWindow();
		if (net)
			return net->currentView();
		return 0;
	}

	TextEditor* MainWindow::currentTextEditor() const
	{
		NetworkWindow * net = currentWindow();
		if (net)
			return net->textEditor;
		return 0;
	}

	NetworkWindow* MainWindow::currentWindow() const
	{
	    return currentNetworkWindow;
	}

	NetworkWindow * MainWindow::currentNetwork() const
	{
	    return currentNetworkWindow;
	}

	SymbolsTable * MainWindow::currentSymbolsTable() const
	{
		if (currentNetworkWindow)
			return &(currentNetworkWindow->symbolsTable);
	    return 0;
	}

	QList<NetworkWindow*> MainWindow::allWindows() const
	{
		return allNetworkWindows;
	}

	void MainWindow::fitAll()
	{
		if (currentScene())
			currentScene()->fitAll();
	}

	void MainWindow::fitSelected()
	{
		if (currentScene())
			currentScene()->fitSelected();
	}

	void MainWindow::initializeMenus(bool enableScene, bool enableText)
	{
		fileMenu = menuBar()->addMenu(tr("&File"));
		toolBarBasic = new QToolBar(tr("Open/save/new toolbar"),this);
		toolBarEdits = new QToolBar(tr("Edit options"),this);
		toolBarForTools = new QToolBar(tr("Plug-ins"),this);

		if (enableScene)
		{
			QAction* newAction = fileMenu->addAction(QIcon(tr(":/images/newscene.png")),tr("&New Graphics Scene"));
			newAction->setShortcut(QKeySequence::New);
			connect (newAction, SIGNAL(triggered()),this,SLOT(newGraphicsWindow()));
			toolBarBasic->addAction(newAction);
		}

		if (enableText)
		{
			QAction* newAction2 = fileMenu->addAction(QIcon(tr(":/images/newtext.png")),tr("New Text &Editor"));
			newAction2->setShortcut(tr("CTRL+SHIFT+N"));
			connect (newAction2, SIGNAL(triggered()),this,SLOT(newTextWindow()));
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

		viewMenu = menuBar()->addMenu(tr("&View"));

		QAction* fitAll = viewMenu->addAction(QIcon(tr(":/images/fitAll.png")),tr("Fit &all"));
		fitAll->setShortcut(tr("F5"));
		connect(fitAll,SIGNAL(triggered()),this,SLOT(fitAll()));


		settingsMenu = menuBar()->addMenu(tr("&Settings"));
		QAction * changeUserHome = settingsMenu->addAction(QIcon(tr(":/images/appicon.png")), tr("&Set Home Directory"));
		connect (changeUserHome, SIGNAL(triggered()),this,SLOT(setUserHome()));

		QMenu * setGridModeMenu = settingsMenu->addMenu(tr("&Grid mode"));

		setGridModeMenu->addAction(tr("Grid ON"),this,SLOT(gridOn()));
		setGridModeMenu->addAction(tr("Grid OFF"),this,SLOT(gridOff()));
		setGridModeMenu->addAction(tr("Grid size"),this,SLOT(setGridSize()));

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

		QAction * createViewAction = new QAction(QIcon(":/images/changeView.png"),tr("Create &view"),this);
		viewMenu->addAction(createViewAction);
		createViewAction->setToolTip(tr("Create view of current network"));
		connect(createViewAction,SIGNAL(triggered()),this,SLOT(createView()));

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

		toolBarEdits->addAction(arrowAction);
		toolBarEdits->addAction(undoAction);
		toolBarEdits->addAction(redoAction);
		toolBarEdits->addAction(copyAction);
		toolBarEdits->addAction(cutAction);
		toolBarEdits->addAction(pasteAction);
		toolBarEdits->addAction(deleteAction);
		toolBarEdits->addAction(createViewAction);

		/*QSize iconSize(16,16);
		toolBarBasic->setIconSize(iconSize);
		toolBarEdits->setIconSize(iconSize);
		toolBarForTools->setIconSize(iconSize);*/

		addToolBar(Qt::TopToolBarArea, toolBarBasic);
		addToolBar(Qt::TopToolBarArea, toolBarEdits);
		addToolBar(Qt::TopToolBarArea, toolBarForTools);

		contextScreenMenu.addAction(fitAll);
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

		contextScreenMenu.addAction(fitAll);
		contextScreenMenu.addAction(closeAction);

		contextScreenMenu.addAction(undoAction);
		contextScreenMenu.addAction(redoAction);

		contextScreenMenu.addAction(createViewAction);
		contextEditorMenu.addAction(createViewAction);
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
		QList<NetworkWindow*> list = allNetworkWindows;
		currentNetworkWindow = 0;
		for (int i=0; i < list.size(); ++i)
			if (list[i] && allNetworkWindows.contains(list[i]))
			{
				b = true;
				emit windowClosing(list[i],&b);
				if (b)
				{
					emit windowClosed(list[i]);
					disconnect(list[i]);
					allNetworkWindows.removeAll(list[i]);
					list[i]->close();
				}
				else
				{
					currentNetworkWindow = list[i];
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

	void MainWindow::dragEnterEvent(QDragEnterEvent *event)
	{
		event->acceptProposedAction();
	}

	void MainWindow::dropEvent(QDropEvent * event)
	{
		QList<QUrl> urlList;
		QList<QFileInfo> files;
		QString fName;
		QFileInfo info;

		if (event->mimeData()->hasUrls())
		{
			urlList = event->mimeData()->urls(); // returns list of QUrls

			// if just text was dropped, urlList is empty (size == 0)
			if ( urlList.size() > 0) // if at least one QUrl is present in list
			{
				fName = urlList[0].toLocalFile(); // convert first QUrl to local path
				info.setFile( fName ); // information about file
				if ( info.isFile() )
					files += info;
			}
		}
		event->acceptProposedAction();
		dragAndDropFiles(files);
	}

	void MainWindow::dragAndDropFiles(const QList<QFileInfo>& files)
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
					emit loadModel(files[i].absoluteFilePath());
				}
			}
		}

		emit filesDropped(files);
	}

	void MainWindow::itemsRemovedSlot(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<ItemHandle*>& handles)
	{
		if (handles.size() > 0 && scene)
			emit itemsRemoved(scene->networkWindow, handles);
	}

	void MainWindow::itemsInsertedSlot(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<ItemHandle*>& handles)
	{
		if (handles.size() > 0 && scene)
			emit itemsInserted(scene->networkWindow, handles);
	}

	void MainWindow::itemsRemovedSlot(TextEditor * editor, const QList<TextItem*>& item, const QList<ItemHandle*>& handles)
	{
		if (handles.size() > 0 && editor)
			emit itemsRemoved(editor->networkWindow, handles);
	}

	void MainWindow::itemsInsertedSlot(TextEditor * editor, const QList<TextItem*>& item, const QList<ItemHandle*>& handles)
	{
		if (handles.size() > 0 && editor)
			emit itemsInserted(editor->networkWindow, handles);
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
		popOut(currentWindow());
	}

	void MainWindow::popOut(NetworkWindow * win)
	{
		if (allowViewModeToChange && win && tabWidget && tabWidget->count() > 1)
		{
			int i = tabWidget->indexOf(win);
			if (i > -1 && i < tabWidget->count())
			{
				tabWidget->removeTab(i);
				win->setParent(this);
				win->setWindowFlags(Qt::Window);
				if (!win->isVisible())
					win->show();
				setCurrentWindow(win);
			}
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

	GraphicsView * MainWindow::createView()
	{
		NetworkWindow * current = currentWindow();
		if (current)
			return current->createView(current->currentView());
		return 0;
	}

	void MainWindow::setCursor(QCursor cursor)
	{
		QMainWindow::setCursor(cursor);
		QList<NetworkWindow*> allWins = allWindows();
		QList<GraphicsView*> views;
		for (int i=0; i < allWins.size(); ++i)
		{
			allWins[i]->setCursor(cursor);
			views = allWins[i]->views();
			for (int j=0; j < views.size(); ++j)
                if (views[j])
                    views[j]->setCursor(cursor);
		}
	}

	/***********************************************
	            C Interface Functions
	***********************************************/

	void MainWindow::zoom(QSemaphore* sem, qreal factor)
	{
		if (currentScene())
		{
			currentScene()->scaleView(factor);
		}
		if (sem)
			sem->release();
	}

	void MainWindow::clearText(QSemaphore* sem)
	{
	    if (console())
            console()->clear();
		if (sem)
			sem->release();
	}

	void MainWindow::outputText(QSemaphore* sem,const QString& text)
	{
		if (console())
            console()->message(text);
		if (sem)
			sem->release();
	}

	void MainWindow::errorReport(QSemaphore* sem,const QString& text)
	{
		if (console())
            console()->error(text);
		if (sem)
			sem->release();
	}

	void MainWindow::printFile(QSemaphore* sem,const QString& filename)
	{
		QString appDir = QCoreApplication::applicationDirPath();

		QString name[] = {
			MainWindow::userTemp() + tr("/") + filename,
			MainWindow::userHome() + tr("/") + filename,
			filename,
			QDir::currentPath() + tr("/") + filename,
			appDir + tr("/") + filename };

		QFile file;
		bool opened = false;
		for (int i=0; i < 5; ++i)
		{
			file.setFileName(name[i]);
			if (file.open(QFile::ReadOnly | QFile::Text))
			{
				opened = true;
				break;
			}
		}
		if (!opened)
		{
			if (console())
                console()->error(tr("file not found"));
		}
		else
		{
			QString allText(file.readAll());
			if (console())
                console()->message(allText);
			file.close();
		}


		if (sem)
			sem->release();
	}

	void MainWindow::outputTable(QSemaphore* sem,const DataTable<qreal>& table)
	{
		if (console())
            console()->printTable(table);
		if (sem)
			sem->release();
	}
	
	void MainWindow::createInputWindow(QSemaphore* s,const DataTable<qreal>& data, const QString& dll,const QString& function,const QString& title)
	{
		SimpleInputWindow::CreateWindow(this,title,dll,function,data);
		if (s)
			s->release();
	}
	
	void MainWindow::createInputWindow(QSemaphore* s,const DataTable<qreal>& dat,const QString& title, MatrixInputFunction f)
	{
		SimpleInputWindow::CreateWindow(this,title,f,dat);
		if (s)
			s->release();
	}
	
	void MainWindow::createSliders(QSemaphore* s, CThread * cthread, const DataTable<qreal>& data, MatrixInputFunction f)
	{
		if (cthread)
		{
			cthread->setFunction(f);
			
			MultithreadedSliderWidget * widget = new MultithreadedSliderWidget(this, cthread, Qt::Horizontal);
			
			QStringList names(data.getRowNames());
			QList<double> min, max;
			for (int i=0; i < names.size(); ++i)
			{
				names[i].replace(tr("_"),tr("."));
				names[i].replace(tr(".."),tr("_"));
				min <<  data.at(i,0);
				max << data.at(i,1);
			}
			widget->setSliders(names, min, max);
			
			widget->show();
		}
		if (s)
			s->release();
	}
	
	void MainWindow::addInputWindowOptions(QSemaphore* s,const QString& name, int i, int j,const QStringList& options)
	{
		SimpleInputWindow::AddOptions(name,i,j,options);
		if (s)
			s->release();
	}
	void MainWindow::addInputWindowCheckbox(QSemaphore* s,const QString& name, int i, int j)
	{
		SimpleInputWindow::AddOptions(name,i,j);
		if (s)
			s->release();
	}
	void MainWindow::openNewWindow(QSemaphore* s,const QString& name)
	{
		newGraphicsWindow();
		if (currentWindow())
			currentWindow()->setWindowTitle(name);
		if (s)
			s->release();
	}
	void MainWindow::isWindows(QSemaphore* s, int * i)
	{
#ifdef Q_WS_WIN
		if (i) (*i) = 1;
#else
		if (i) (*i) = 0;
#endif
		if (s)
			s->release();
	}
	void MainWindow::isMac(QSemaphore* s, int * i)
	{
#ifdef Q_WS_MAC
		if (i) (*i) = 1;
#else
		if (i) (*i) = 0;
#endif
		if (s)
			s->release();
	}
	void MainWindow::isLinux(QSemaphore* s, int * i)
	{
#ifdef Q_WS_WIN
		if (i) (*i) = 0;
#else
#ifdef Q_WS_MAC
		if (i) (*i) = 0;
#else
		if (i) (*i) = 1;
#endif
#endif
		if (s)
			s->release();
	}

	void MainWindow::appDir(QSemaphore* s, QString * dir)
	{
		QString appDir = QCoreApplication::applicationDirPath();
		if (dir)
			(*dir) = appDir;
		if (s)
			s->release();
	}

	void MainWindow::allItems(QSemaphore* s,QList<ItemHandle*>* returnPtr)
	{
		NetworkWindow * win = currentWindow();
		if (!win || !returnPtr)
		{
			if (s) s->release();
			return;
		}

		if (returnPtr)
			(*returnPtr) = win->allHandles();

		if (s)
			s->release();
	}

	void MainWindow::itemsOfFamily(QSemaphore* s,QList<ItemHandle*>* returnPtr, const QString& family)
	{
		NetworkWindow * win = currentWindow();
		if (!win || !returnPtr)
		{
			if (s) s->release();
			return;
		}

		if (returnPtr)
		{
			QList<ItemHandle*> handles = win->allHandles();
			for (int i=0; i < handles.size(); ++i)
				if (handles[i] && handles[i]->isA(family))
					(*returnPtr) += handles[i];
		}

		if (s)
			s->release();
	}

	void MainWindow::itemsOfFamily(QSemaphore* s,QList<ItemHandle*>* returnPtr,const QList<ItemHandle*>& handles,const QString& family)
	{
		NetworkWindow * win = currentWindow();
		if (!win || !returnPtr)
		{
			if (s) s->release();
			return;
		}

		if (returnPtr)
		{
			for (int i=0; i < handles.size(); ++i)
				if (handles[i] && handles[i]->isA(family))
					(*returnPtr) += handles[i];
		}

		if (s)
			s->release();
	}

	void MainWindow::selectedItems(QSemaphore* s,QList<ItemHandle*>* returnPtr)
	{
		NetworkWindow * win = currentWindow();
		if (!win || !returnPtr)
		{
			if (s) s->release();
			return;
		}

		QList<ItemHandle*> list = win->selectedHandles();

		if (returnPtr)
			(*returnPtr) = list;

		if (s)
			s->release();
	}

	void MainWindow::itemNames(QSemaphore* s,QStringList* list,const QList<ItemHandle*>& items)
	{
		if (list)
		{
			(*list).clear();
			for (int i=0; i < items.size(); ++i)
			{
				if (items[i])
					(*list) << items[i]->fullName(tr("_"));
			}
		}
		if (s)
			s->release();
	}

	void MainWindow::itemName(QSemaphore* s,QString* name,ItemHandle* handle)
	{
		if (handle && name)
		{
			(*name) = handle->fullName(tr("_"));
		}
		if (s)
			s->release();
	}

	void MainWindow::setName(QSemaphore* s,ItemHandle* handle,const QString& name)
	{
		if (handle && !name.isNull() && !name.isEmpty() && currentWindow())
		{
			currentWindow()->rename(handle,name);
		}
		if (s)
			s->release();
	}

	void MainWindow::itemFamily(QSemaphore* s,QString* family,ItemHandle* handle)
	{
		if (handle && family && handle->family())
		{
			(*family) = handle->family()->name;
		}
		if (s)
			s->release();
	}

	void MainWindow::isA(QSemaphore* s,int* boolean,ItemHandle* handle,const QString& family)
	{
		if (handle && boolean)
		{
			(*boolean) = 0;

			if (handle->family())
			{
				(*boolean) = (int)(handle->family()->isA(family));
			}
		}
		if (s)
			s->release();
	}

	void MainWindow::findItem(QSemaphore* s,ItemHandle** returnPtr,const QString& name)
	{
		NetworkWindow * win = currentWindow();
		if (!win || !returnPtr)
		{
			if (returnPtr)
				(*returnPtr) = 0;
			if (s) s->release();
			return;
		}

		(*returnPtr) = 0;

		if (win->symbolsTable.handlesFullName.contains(name))
			(*returnPtr) = win->symbolsTable.handlesFullName[name];
		else
		{
			QString s = name;

			if (win->symbolsTable.handlesFullName.contains(s))
				(*returnPtr) = win->symbolsTable.handlesFullName[s];
			else
			{
				if (win->symbolsTable.handlesFirstName.contains(s))
					(*returnPtr) = win->symbolsTable.handlesFirstName[s];
				else
					if (win->symbolsTable.dataRowsAndCols.contains(s))
						(*returnPtr) = win->symbolsTable.dataRowsAndCols[s].first;
			}

			if ((*returnPtr) == 0)
			{
				int k = -1;
				while ( (k = s.lastIndexOf(tr("_"))) != -1 && ((*returnPtr) == 0))
				{
					s[k] = QChar('.');
					if (win->symbolsTable.handlesFullName.contains(s))
						(*returnPtr) = win->symbolsTable.handlesFullName[s];
					else
					{
						if (win->symbolsTable.handlesFirstName.contains(s))
							(*returnPtr) = win->symbolsTable.handlesFirstName[s];
						else
							if (win->symbolsTable.dataRowsAndCols.contains(s))
								(*returnPtr) = win->symbolsTable.dataRowsAndCols[s].first;
					}
				}
			}
		}

		if (s)
			s->release();
	}

	void MainWindow::findItems(QSemaphore* s,QList<ItemHandle*>* returnPtr,const QStringList& names)
	{
		NetworkWindow * win = currentWindow();
		if (!win || !returnPtr)
		{
			if (returnPtr)
				returnPtr->clear();
			if (s) s->release();
			return;
		}

		returnPtr->clear();
		QString name;
		ItemHandle * handle = 0;

		for (int i=0; i < names.size(); ++i)
		{
			name = names[i];
			handle = 0;
			int k;

			if (win->symbolsTable.handlesFullName.contains(name))
				handle = win->symbolsTable.handlesFullName[name];
			else
			{
				QString s = name;

				if (win->symbolsTable.handlesFullName.contains(s))
					handle = win->symbolsTable.handlesFullName[s];
				else
				{
					if (win->symbolsTable.handlesFirstName.contains(s))
						handle = win->symbolsTable.handlesFirstName[s];
					else
						if (win->symbolsTable.dataRowsAndCols.contains(s))
							handle= win->symbolsTable.dataRowsAndCols[s].first;
				}

				if (handle == 0)
				{
					while ( (k = s.lastIndexOf(tr("_"))) != -1 && (handle == 0))
					{
						s[k] = QChar('.');
						if (win->symbolsTable.handlesFullName.contains(s))
							handle = win->symbolsTable.handlesFullName[s];
						else
						{
							if (win->symbolsTable.handlesFirstName.contains(s))
								handle = win->symbolsTable.handlesFirstName[s];
							else
								if (win->symbolsTable.dataRowsAndCols.contains(s))
									handle = win->symbolsTable.dataRowsAndCols[s].first;
						}
					}
				}
			}

			if (handle)
				returnPtr->append(handle);
		}

		if (s)
			s->release();
	}

	/*! \brief select an item in the current scene.
	* \param graphics item pointer
	*/
	void MainWindow::select(QSemaphore* sem,ItemHandle* item)
	{
		GraphicsScene * scene = currentScene();
		if (item && scene)
			scene->select(item->graphicsItems);
		if (sem)
			sem->release();
	}

	/*! \brief deselect all items in the current scene.
	* \param graphics item pointer
	*/
	void MainWindow::deselect(QSemaphore* sem)
	{
		GraphicsScene * scene = currentScene();
		if (scene)
			scene->deselect();
		if (sem)
			sem->release();
	}


	/*! \brief delete an item in the current scene. This function is intended to be used
	in a C-style plugin or script.
	*/
	void MainWindow::removeItem(QSemaphore* sem, ItemHandle * item)
	{
		if (currentScene() && item)
			currentScene()->remove(tr("item removed"),item->graphicsItems);
		//else
		//if (currentTextEditor() && item)

		if (sem)
			sem->release();
	}

	void MainWindow::getX(QSemaphore * s, qreal * returnPtr, ItemHandle * item)
	{
		if (item == 0 || item->graphicsItems.isEmpty() || item->graphicsItems[0] == 0)
		{
			if (s) s->release();
			return;
		}
		if (returnPtr)
			(*returnPtr) = item->graphicsItems[0]->scenePos().x();
		if (s)
			s->release();
	}

	void MainWindow::getY(QSemaphore * s, qreal * returnPtr, ItemHandle * item)
	{
		if (item == 0 || item->graphicsItems.isEmpty() || item->graphicsItems[0] == 0)
		{
			if (s) s->release();
			return;
		}
		if (returnPtr)
			(*returnPtr) = item->graphicsItems[0]->scenePos().y();
		if (s)
			s->release();
	}

	void MainWindow::moveSelected(QSemaphore* sem,qreal dx, qreal dy)
	{
		if (currentScene())
		{
			currentScene()->move( currentScene()->moving() , QPointF(dx,dy) );
		}
		if (sem)
			sem->release();
	}

	void MainWindow::setPos(QSemaphore* sem, ItemHandle * item, qreal x, qreal y)
	{
		if (currentScene() && item && !item->graphicsItems.isEmpty() && item->graphicsItems[0])
		{
			currentScene()->move( item->graphicsItems , QPointF(x,y) - item->graphicsItems[0]->scenePos() );
		}
		if (sem)
			sem->release();
	}

	void MainWindow::setPos(QSemaphore* sem,const QList<ItemHandle*>& items, DataTable<qreal>& pos)
	{
		if (currentScene() && !items.isEmpty() && items.size() == pos.rows() && pos.cols() == 2)
		{
			QList<QGraphicsItem*> graphicsItems;
			QGraphicsItem* item;
			QList<QPointF> p;
			QPointF diff, target;
			ConnectionGraphicsItem * connection = 0;
			NodeGraphicsItem *  node = 0;
			int m;
			for (int i=0; i < items.size(); ++i)
			{
				for (int j=0; j < items[i]->graphicsItems.size(); ++j)
					if ((item = items[i]->graphicsItems[j]))
					{
						if ((connection = qgraphicsitem_cast<ConnectionGraphicsItem*>(item)))
						{
							ConnectionGraphicsItem::ControlPoint * cp;
							if ((cp = connection->centerPoint()))
							{
								graphicsItems << cp;
								diff = QPointF(pos.value(i,0),pos.value(i,1)) - cp->scenePos();
								p << diff;

								for (int k=0; k < connection->curveSegments.size(); ++k)
								{
									ConnectionGraphicsItem::ControlPoint * cp1 = connection->curveSegments[k].first();
									if (cp1 && cp && cp1 != cp)
									{
										QPointF p1 = cp1->scenePos(), p2 = QPointF(pos.value(i,0),pos.value(i,1));
										if ((node = qgraphicsitem_cast<NodeGraphicsItem*>(cp1->parentItem())) &&
											((m = items.indexOf(node->handle())) > -1))
										{
											p1 = QPointF(pos.value(m,0),pos.value(m,1)) - node->scenePos();
										}
										for (int l=1; l < connection->curveSegments[k].size(); ++l)
										{
											if (connection->curveSegments[k][l] && connection->curveSegments[k][l] != cp)
											{
												target =
													p1*((double)(connection->curveSegments[k].size() - l - 0.5))/((double)(connection->curveSegments[k].size()))
													+
													p2*((double)(l + 0.5))/((double)(connection->curveSegments[k].size()));
												diff = target - connection->curveSegments[k][l]->scenePos();
												if ((m = graphicsItems.indexOf(connection->curveSegments[k][l])) > -1)
												{
													p[m] = (p[m] + diff)/2.0;
												}
												else
												{
													p << diff;
													graphicsItems << connection->curveSegments[k][l];
												}
											}
										}
									}
								}
							}
						}
						else
						{
							graphicsItems << item;
							p << QPointF(pos.value(i,0),pos.value(i,1)) - items[i]->graphicsItems[j]->scenePos();
						}
					}
			}
			currentScene()->move( graphicsItems , p );
		}
		else
			if (console())
                console()->message(QString::number(items.size()) + tr(" ") + QString::number(pos.cols()));

		if (sem)
			sem->release();
	}

	void MainWindow::getPos(QSemaphore* sem,const QList<ItemHandle*>& items, DataTable<qreal>* pos)
	{
		if (currentScene() && !items.isEmpty() && pos)
		{
			QList<QGraphicsItem*> graphicsItems;
			QList<QPointF> p;
			for (int i=0; i < items.size(); ++i)
			{
				for (int j=0; j < items[i]->graphicsItems.size(); ++j)
				{
					if (qgraphicsitem_cast<NodeGraphicsItem*>(items[i]->graphicsItems[j]) || (j == (items[i]->graphicsItems.size() - 1)))
					{
						p << items[i]->graphicsItems[j]->scenePos();
						break;
					}
					else
						if (qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i]->graphicsItems[j]))
						{
							p << (qgraphicsitem_cast<ConnectionGraphicsItem*>(items[i]->graphicsItems[j]))->centerLocation();
							break;
						}
				}
			}

			(*pos).resize(p.size(),2);
			(*pos).colName(0) = tr("x");
			(*pos).colName(1) = tr("y");
			for (int i=0; i < p.size(); ++i)
			{
				(*pos).value(i,0) = p[i].rx();
				(*pos).value(i,1) = p[i].ry();
			}
		}
		if (sem)
			sem->release();
	}


	void MainWindow::getNumericalData(QSemaphore* sem,DataTable<qreal>* dat,ItemHandle* item,const QString& tool)
	{
		if (!item && currentWindow())
			item = &(currentWindow()->symbolsTable.modelItem);

		if (dat && item && item->data && item->data->numericalData.contains(tool))
		{
			(*dat) = item->data->numericalData[tool];
		}
		if (sem)
			sem->release();
	}

	void MainWindow::setNumericalData(QSemaphore* sem,ItemHandle* item,const QString& tool, const DataTable<qreal>& dat)
	{
		if (!item && currentWindow())
			item = &(currentWindow()->symbolsTable.modelItem);

		if (item && item->data)
		{
			if (!item->data->numericalData.contains(tool))
			{
				item->data->numericalData[tool] = DataTable<qreal>();
			}
			NetworkWindow * win = currentWindow();
			if (win)
			{
				win->changeData(tool + tr(" changed for ") + item->fullName(),item,tool,&dat);
			}
		}
		if (sem)
			sem->release();
	}

	void MainWindow::getTextData(QSemaphore* sem,DataTable<QString>* dat,ItemHandle* item,const QString& tool)
	{
		if (!item && currentWindow())
			item = &(currentWindow()->symbolsTable.modelItem);

		if (dat && item && item->data && item->data->textData.contains(tool))
		{
			(*dat) = item->data->textData[tool];
		}
		if (sem)
			sem->release();
	}

	void MainWindow::setTextData(QSemaphore* sem,ItemHandle* item,const QString& tool, const DataTable<QString>& dat)
	{
		if (!item && currentWindow())
			item = &(currentWindow()->symbolsTable.modelItem);

		if (item && item->data)
		{
			if (!item->data->textData.contains(tool))
			{
				item->data->textData[tool] = DataTable<QString>();
			}
			NetworkWindow * win = currentWindow();
			if (win)
			{
				win->changeData(tool + tr(" changed for ") + item->fullName(),item,tool,&dat);
			}
		}
		if (sem)
			sem->release();
	}
	
	void MainWindow::getChildren(QSemaphore* sem,QList<ItemHandle*>* ret,ItemHandle* item)
	{
		if (item && ret)
		{
			(*ret) << item->children;
		}
		if (sem)
			sem->release();
	}

	void MainWindow::getParent(QSemaphore* sem,ItemHandle** ret,ItemHandle* item)
	{
		if (item && ret)
		{
			(*ret) = item->parent;
		}
		if (sem)
			sem->release();
	}

	/*******************************************************************/
	/*******************FUNCTION TO SIGNAL******************************/
	/*******************************************************************/

	MainWindow_FtoS MainWindow::fToS;

	void MainWindow::_zoom(double x)
	{
		fToS.zoom(x);
	}

	void* MainWindow::_find(const char* c)
	{
		return fToS.find(c);
	}

	ArrayOfItems MainWindow::_findItems(ArrayOfStrings c)
	{
		return fToS.findItems(c);
	}

	void MainWindow::_select(void* o)
	{
		return fToS.select(o);
	}

	void MainWindow::_deselect()
	{
		return fToS.deselect();
	}

	ArrayOfItems MainWindow::_allItems()
	{
		return fToS.allItems();
	}

	ArrayOfItems MainWindow::_itemsOfFamily(const char* f)
	{
		return fToS.itemsOfFamily(f);
	}

	ArrayOfItems MainWindow::_itemsOfFamily2(const char* f, ArrayOfItems a)
	{
		return fToS.itemsOfFamily(f,a);
	}

	ArrayOfItems MainWindow::_selectedItems()
	{
		return fToS.selectedItems();
	}

	char* MainWindow::_getName(void* o)
	{
		return fToS.getName(o);
	}

	void MainWindow::_setName(void* o,const char* c)
	{
		return fToS.setName(o,c);
	}

	ArrayOfStrings MainWindow::_getNames(ArrayOfItems a)
	{
		return fToS.getNames(a);
	}

	char* MainWindow::_getFamily(void* o)
	{
		return fToS.getFamily(o);
	}

	int MainWindow::_isA(void* o,const char* c)
	{
		return fToS.isA(o,c);
	}

	void MainWindow::_removeItem(void* o)
	{
		return fToS.removeItem(o);
	}

	void MainWindow::_setPos(void* o,double x,double y)
	{
		return fToS.setPos(o,x,y);
	}

	void MainWindow::_setPos2(ArrayOfItems a,Matrix m)
	{
		return fToS.setPos(a,m);
	}

	Matrix MainWindow::_getPos(ArrayOfItems a)
	{
		return fToS.getPos(a);
	}

	double MainWindow::_getY(void* o)
	{
		return fToS.getY(o);
	}

	double MainWindow::_getX(void* o)
	{
		return fToS.getX(o);
	}

	void MainWindow::_moveSelected(double dx,double dy)
	{
		return fToS.moveSelected(dx,dy);
	}

	void MainWindow::_clearText()
	{
		return fToS.clearText();
	}

	void MainWindow::_outputTable(Matrix m)
	{
		return fToS.outputTable(m);
	}

	void MainWindow::_outputText(const char * c)
	{
		return fToS.outputText(c);
	}

	void MainWindow::_errorReport(const char * c)
	{
		return fToS.errorReport(c);
	}

	void MainWindow::_printFile(const char* c)
	{
		return fToS.printFile(c);
	}

	void  MainWindow::_createInputWindow1(Matrix m,const char* a,const char* b, const char* c)
	{
		return fToS.createInputWindow(m,a,b,c);
	}

	void  MainWindow::_createInputWindow2(Matrix m,const char* a, MatrixInputFunction f)
	{
		return fToS.createInputWindow(m,a,f);
	}
	
	void  MainWindow::_createSliders(void* c, Matrix m,MatrixInputFunction f)
	{
		return fToS.createSliders(c,m,f);
	}

	void  MainWindow::_addInputWindowOptions(const char* a,int i, int j, ArrayOfStrings c)
	{
		return fToS.addInputWindowOptions(a,i,j,c);
	}

	void  MainWindow::_addInputWindowCheckbox(const char* a,int i, int j)
	{
		return fToS.addInputWindowCheckbox(a,i,j);
	}

	void  MainWindow::_openNewWindow(const char* c)
	{
		return fToS.openNewWindow(c);
	}

	int  MainWindow::_isWindows()
	{
		return fToS.isWindows();
	}

	int  MainWindow::_isMac()
	{
		return fToS.isMac();
	}

	int  MainWindow::_isLinux()
	{
		return fToS.isLinux();
	}

	char*  MainWindow::_appDir()
	{
		return fToS.appDir();
	}

	Matrix MainWindow::_getNumericalData(void* o,const char* a)
	{
		return fToS.getNumericalData(o,a);
	}

	void MainWindow::_setNumericalData(void* o ,const char* a,Matrix m)
	{
		return fToS.setNumericalData(o,a,m);
	}
	
	TableOfStrings MainWindow::_getTextData(void* o,const char* a)
	{
		return fToS.getTextData(o,a);
	}

	void MainWindow::_setTextData(void* o ,const char* a,TableOfStrings m)
	{
		return fToS.setTextData(o,a,m);
	}

	ArrayOfItems MainWindow::_getChildren(void* o)
	{
		return fToS.getChildren(o);
	}

	void* MainWindow::_getParent(void* o)
	{
		return fToS.getParent(o);
	}

	void MainWindow_FtoS::zoom(double x)
	{
		emit zoom(0,x);
	}

	void* MainWindow_FtoS::find(const char* c)
	{
		QSemaphore * s = new QSemaphore(1);
		ItemHandle * p;
		s->acquire();
		emit find(s,&p,ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
		return p;
	}

	ArrayOfItems MainWindow_FtoS::findItems(ArrayOfStrings c)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* p = new QList<ItemHandle*>;
		s->acquire();
		emit findItems(s,p,ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
		ArrayOfItems A = ConvertValue(*p);
		delete p;
		return A;
	}

	void MainWindow_FtoS::select(void* o)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit select(s,ConvertValue(o));
		s->acquire();
		s->release();
		delete s;
	}

	void MainWindow_FtoS::deselect()
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit deselect(s);
		s->acquire();
		s->release();
		delete s;
	}

	ArrayOfItems MainWindow_FtoS::allItems()
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* p = new QList<ItemHandle*>;
		s->acquire();
		emit allItems(s,p);
		s->acquire();
		s->release();
		delete s;
		ArrayOfItems A = ConvertValue(*p);
		delete p;
		return A;
	}

	ArrayOfItems MainWindow_FtoS::itemsOfFamily(const char * f)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* p = new QList<ItemHandle*>;
		s->acquire();
		emit itemsOfFamily(s,p,ConvertValue(f));
		s->acquire();
		s->release();
		delete s;
		ArrayOfItems A = ConvertValue(*p);
		delete p;
		return A;
	}

	ArrayOfItems MainWindow_FtoS::itemsOfFamily(const char * f, ArrayOfItems a)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*> * list = ConvertValue(a);
		QList<ItemHandle*>* p = new QList<ItemHandle*>;
		s->acquire();
		emit itemsOfFamily(s,p,*list,ConvertValue(f));
		s->acquire();
		s->release();
		delete s;
		ArrayOfItems A = ConvertValue(*p);
		delete p;
		delete list;
		return A;
	}

	ArrayOfItems MainWindow_FtoS::selectedItems()
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* p = new QList<ItemHandle*>;
		s->acquire();
		emit selectedItems(s,p);
		s->acquire();
		s->release();
		delete s;
		ArrayOfItems A = ConvertValue(*p);
		delete p;
		return A;
	}

	char* MainWindow_FtoS::getName(void* o)
	{
		QSemaphore * s = new QSemaphore(1);
		QString p;
		s->acquire();
		emit getName(s,&p,ConvertValue(o));
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(p);
	}

	void MainWindow_FtoS::setName(void* o, const char* c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setName(s,ConvertValue(o),ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
	}

	ArrayOfStrings MainWindow_FtoS::getNames(ArrayOfItems a0)
	{
		QSemaphore * s = new QSemaphore(1);
		QStringList p;
		s->acquire();
		QList<ItemHandle*> * list = ConvertValue(a0);
		emit getNames(s,&p,*list);
		s->acquire();
		s->release();
		delete s;
		delete list;
		return ConvertValue(p);
	}

	char* MainWindow_FtoS::getFamily(void* a0)
	{
		QSemaphore * s = new QSemaphore(1);
		QString p;
		s->acquire();
		emit getFamily(s,&p,ConvertValue(a0));
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(p);
	}

	int MainWindow_FtoS::isA(void* a0, const char* name)
	{
		QSemaphore * s = new QSemaphore(1);
		int p = 0;
		s->acquire();
		emit isA(s,&p,ConvertValue(a0),ConvertValue(name));
		s->acquire();
		s->release();
		delete s;
		return p;
	}

	void MainWindow_FtoS::removeItem(void* a0)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit removeItem(s,ConvertValue(a0));
		s->acquire();
		s->release();
		delete s;
	}

	void MainWindow_FtoS::setPos(void* a0,double a1,double a2)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setPos(s,ConvertValue(a0),a1,a2);
		s->acquire();
		s->release();
		delete s;
	}

	void MainWindow_FtoS::setPos(ArrayOfItems a0,Matrix m)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		QList<ItemHandle*> * list = ConvertValue(a0);
		DataTable<qreal> * dat = ConvertValue(m);
		emit setPos(s,*list,*dat);
		s->acquire();
		s->release();
		delete dat;
		delete list;
		delete s;
	}

	Matrix MainWindow_FtoS::getPos(ArrayOfItems a0)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		QList<ItemHandle*> * list = ConvertValue(a0);
		DataTable<qreal> * p = new DataTable<qreal>;
		emit getPos(s,*list,p);
		s->acquire();
		s->release();
		delete list;
		delete s;
		Matrix m;
		if (p)
		{
			m = ConvertValue(*p);
			delete p;
			return m;
		}
		m.values = 0;
		m.rownames.length = m.colnames.length = 0;
		m.rownames.strings = m.colnames.strings = 0;
		return m;
	}


	double MainWindow_FtoS::getY(void* a0)
	{
		QSemaphore * s = new QSemaphore(1);
		qreal p;
		s->acquire();
		emit getY(s,&p,ConvertValue(a0));
		s->acquire();
		s->release();
		delete s;
		return (double)p;
	}

	double MainWindow_FtoS::getX(void* a0)
	{
		QSemaphore * s = new QSemaphore(1);
		qreal p;
		s->acquire();
		emit getX(s,&p,ConvertValue(a0));
		s->acquire();
		s->release();
		delete s;
		return (double)p;
	}

	void MainWindow_FtoS::moveSelected(double a0,double a1)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit moveSelected(s,a0,a1);
		s->acquire();
		s->release();
		delete s;
	}

	void MainWindow_FtoS::clearText()
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit clearText(s);
		s->acquire();
		s->release();
		delete s;
	}

	void MainWindow_FtoS::outputTable(Matrix m)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		DataTable<qreal> * dat = ConvertValue(m);
		emit outputTable(s,*dat);
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}

	void MainWindow_FtoS::outputText(const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit outputText(s,ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
	}

	void MainWindow_FtoS::errorReport(const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit errorReport(s,ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
	}

	void MainWindow_FtoS::printFile(const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit printFile(s,ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
	}

	void MainWindow_FtoS::createInputWindow(Matrix m, const char* cfile,const char* fname, const char* title)
	{
		DataTable<qreal>* dat = ConvertValue(m);
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit createInputWindow(s,*dat,ConvertValue(cfile),ConvertValue(fname),ConvertValue(title));
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}

	void MainWindow_FtoS::createInputWindow(Matrix m, const char* title, MatrixInputFunction f)
	{
		DataTable<qreal>* dat = ConvertValue(m);
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit createInputWindow(s,*dat,ConvertValue(title),f);
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}
	
	void MainWindow_FtoS::createSliders(void * c, Matrix m, MatrixInputFunction f)
	{
		CThread * cthread = static_cast<CThread*>(c);
		DataTable<qreal>* dat = ConvertValue(m);
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit createSliders(s,cthread,*dat,f);
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}

	void MainWindow_FtoS::addInputWindowOptions(const char * a, int i, int j, ArrayOfStrings list)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit addInputWindowOptions(s,ConvertValue(a),i,j,ConvertValue(list));
		s->acquire();
		s->release();
		delete s;
	}

	void MainWindow_FtoS::addInputWindowCheckbox(const char * a, int i, int j)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit addInputWindowCheckbox(s,ConvertValue(a),i,j);
		s->acquire();
		s->release();
		delete s;
	}

	void MainWindow_FtoS::openNewWindow(const char* c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit openNewWindow(s,ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
	}

	int MainWindow_FtoS::isWindows()
	{
		int i;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit isWindows(s,&i);
		s->acquire();
		s->release();
		delete s;
		return i;
	}

	int MainWindow_FtoS::isMac()
	{
		int i;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit isMac(s,&i);
		s->acquire();
		s->release();
		delete s;
		return i;
	}

	int MainWindow_FtoS::isLinux()
	{
		int i;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit isLinux(s,&i);
		s->acquire();
		s->release();
		delete s;
		return i;
	}

	char* MainWindow_FtoS::appDir()
	{
		QString dir;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit appDir(s,&dir);
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(dir);
	}

	Matrix MainWindow_FtoS::getNumericalData(void* o,const char* c)
	{
		QSemaphore * s = new QSemaphore(1);
		DataTable<qreal> * p = new DataTable<qreal>;
		s->acquire();
		emit getNumericalData(s,p,ConvertValue(o),ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
		Matrix m;
		if (p)
		{
			m = ConvertValue(*p);
			delete p;
			return m;
		}
		m.values = 0;
		m.rownames.length = m.colnames.length =0;
		m.rownames.strings = m.colnames.strings = 0;
		return m;
	}

	void MainWindow_FtoS::setNumericalData(void* o, const char * c, Matrix M)
	{
		DataTable<qreal>* dat = ConvertValue(M);
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setNumericalData(s,ConvertValue(o),ConvertValue(c),*dat);
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}
	
	TableOfStrings MainWindow_FtoS::getTextData(void* o,const char* c)
	{
		QSemaphore * s = new QSemaphore(1);
		DataTable<QString> * p = new DataTable<QString>;
		s->acquire();
		emit getTextData(s,p,ConvertValue(o),ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
		TableOfStrings m;
		if (p)
		{
			m = ConvertValue(*p);
			delete p;
			return m;
		}
		m.strings = 0;
		m.rownames.length = m.colnames.length =0;
		m.rownames.strings = m.colnames.strings = 0;
		return m;
	}

	void MainWindow_FtoS::setTextData(void* o, const char * c, TableOfStrings M)
	{
		DataTable<QString>* dat = ConvertValue(M);
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setTextData(s,ConvertValue(o),ConvertValue(c),*dat);
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}

	ArrayOfStrings MainWindow_FtoS::getNumericalDataNames(void* o)
	{
		QStringList p;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit getNumericalDataNames(s,&p,ConvertValue(o));
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(p);
	}

	ArrayOfStrings MainWindow_FtoS::getTextDataNames(void* o)
	{
		QStringList p;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit getTextDataNames(s,&p,ConvertValue(o));
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(p);
	}

	ArrayOfItems MainWindow_FtoS::getChildren(void* o)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* p = new QList<ItemHandle*>;
		s->acquire();
		emit getChildren(s,p,ConvertValue(o));
		s->acquire();
		s->release();
		delete s;
		ArrayOfItems A = ConvertValue(*p);
		delete p;
		return A;
	}

	void* MainWindow_FtoS::getParent(void* o)
	{
		ItemHandle * p = 0;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit getParent(s,&p,ConvertValue(o));
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(p);
	}

	void MainWindow::getNumber(QSemaphore* s,double* p,const QString& name)
    {
        if (p)
        {
            (*p) = QInputDialog::getDouble(this,tr("Get Number"),name);
        }
        if (s)
            s->release();
    }

    void MainWindow::getString(QSemaphore* s,QString* p,const QString& name)
    {
        if (p)
        {
            (*p) = QInputDialog::getText(this,tr("Get Text"),name);
            (*p).replace(tr("."),tr("_"));
        }
        if (s)
            s->release();
    }

    void MainWindow::getNumbers(QSemaphore* s,const QStringList& names,qreal* res)
    {
        QDialog * dialog = new QDialog(this);

        QGridLayout * layout = new QGridLayout;

        QList< QDoubleSpinBox* > spinBoxes;
        for (int i=0; i < names.size(); ++i)
        {
            QDoubleSpinBox * spinBox = new QDoubleSpinBox(dialog);
            spinBox->setRange(-1.0E300,1.0E300);

            spinBoxes += spinBox;
            layout->addWidget( new QLabel(names[i],dialog), i, 0 );
            layout->addWidget( spinBox, i, 1 );
        }

        QPushButton * ok = new QPushButton(tr("Done"), this);
        connect(ok,SIGNAL(released()),dialog,SLOT(accept()));

        layout->addWidget(ok, names.size(), 1 );
        dialog->setLayout(layout);
        dialog->exec();

        if (res)
        {
            for (int i=0; i < spinBoxes.size() && i < names.size(); ++i)
                if (spinBoxes[i])
                    res[i] = spinBoxes[i]->value();
        }

        if (s)
            s->release();
    }

    void MainWindow::getFilename(QSemaphore* s,QString* p)
    {
        if (p)
        {
            QString file = QFileDialog::getOpenFileName(this,tr("Select file"));
            if (!file.isNull() && !file.isEmpty())
                (*p) = file;
        }
        if (s)
            s->release();
    }

    void MainWindow::getStringListItemSelected(QListWidgetItem * item)
    {
        if (item)
            getStringListNumber = getStringList.currentRow();
        if (getStringDialog)
            getStringDialog->accept();
    }

    void MainWindow::getStringListRowChanged ( int  )
    {
        if (getStringList.currentItem())
            getStringListNumber = getStringListText.indexOf(getStringList.currentItem()->text());
    }

    void MainWindow::getStringListCanceled (  )
    {
        getStringListNumber = -1;
    }

    void MainWindow::getStringSearchTextEdited ( const QString & text )
    {
        getStringList.clear();

        QStringList list;

        if (text.isEmpty())
            list = getStringListText;
        else
            for (int i=0; i < getStringListText.size(); ++i)
                if (getStringListText[i].toLower().contains(text.toLower()))
                    list << getStringListText[i];

        getStringList.addItems(list);
        getStringList.setCurrentRow(0);
    }

    void MainWindow::getSelectedString(QSemaphore* s,int* p,const QString& name, const QStringList& list0,const QString& init, int option)
    {
        if (p)
        {
            getStringListText.clear();
            if (option == 0 && !getStringDialog)
            {
                getStringDialog = new QDialog(this);
                getStringDialog->setSizeGripEnabled (true);
                QVBoxLayout * layout = new QVBoxLayout;
                layout->addWidget(&getStringListLabel);
                layout->addWidget(&getStringList);
                QHBoxLayout * buttonsLayout = new QHBoxLayout;

                QLineEdit * search = new QLineEdit(tr("Search"));
                connect(search,SIGNAL(textEdited(const QString &)),this,SLOT(getStringSearchTextEdited(const QString &)));

                QPushButton * okButton = new QPushButton(tr("OK"));
                QPushButton * cancelButton = new QPushButton(tr("Cancel"));
                connect(okButton,SIGNAL(released()),getStringDialog,SLOT(accept()));
                connect(cancelButton,SIGNAL(released()),getStringDialog,SLOT(reject()));

                buttonsLayout->addWidget(okButton,1,Qt::AlignLeft);
                buttonsLayout->addWidget(cancelButton,1,Qt::AlignLeft);
                buttonsLayout->addStretch(2);
                buttonsLayout->addWidget(search,5,Qt::AlignRight);

                layout->addLayout(buttonsLayout);

                connect(&getStringList,SIGNAL(itemActivated(QListWidgetItem * item)),this,SLOT(getStringListItemSelected(QListWidgetItem * item)));
                connect(&getStringList,SIGNAL(currentRowChanged (int)),this,SLOT(getStringListRowChanged (int)));
                connect(getStringDialog,SIGNAL(rejected()),this,SLOT(getStringListCanceled()));

                getStringDialog->setLayout(layout);
            }

            QStringList list = list0;
            bool ok;
            QRegExp regex(QString("([A-Za-z0-9])_([A-Za-z0-9])"));

			int index = list.indexOf(init);
			if (index < 0) index = 0;

            for (int i=0; i < list.size(); ++i)
                list[i].replace(regex,tr("\\1.\\2"));

            if (option == 0 && !list0.isEmpty())
            {
                getStringListLabel.setText(name);
                getStringListText = list;
                getStringList.clear();
                getStringList.addItems(list);
                getStringList.setCurrentRow(index);
                getStringDialog->exec();
                (*p) = getStringListNumber;
            }
            else
            {
                QString s = QInputDialog::getItem(this,tr("Get Text"),name,list,index,false,&ok);
                if (ok)
                    (*p) = list.indexOf(s);
                else
                    (*p) = -1;
            }
        }
        if (s)
            s->release();
    }

	void MainWindow::askQuestion(QSemaphore* s, const QString& msg, int * x)
	{
		QMessageBox::StandardButton ans = QMessageBox::question(this,tr("Question"),msg,QMessageBox::Yes | QMessageBox::No);

		if (x)

			if (ans == QMessageBox::Yes)
				(*x) = 1;
			else
				(*x) = 0;

		if (s)
			s->release();
	}

	void MainWindow::messageDialog(QSemaphore* s, const QString& msg)
	{
		QMessageBox::information(this,tr("Message"),msg);

		if (s)
			s->release();
	}

    char* MainWindow::_getString(const char* title)
    {
        return fToS.getString(title);
    }

    char* MainWindow::_getFilename()
    {
        return fToS.getFilename();
    }

    int MainWindow::_getSelectedString(const char* title,ArrayOfStrings list,const char* c, int i)
    {
        return fToS.getSelectedString(title,list,c,i);
    }

    double MainWindow::_getNumber(const char* title)
    {
        return fToS.getNumber(title);
    }

    void MainWindow::_getNumbers(ArrayOfStrings names, double * res)
    {
        return fToS.getNumbers(names,res);
    }

	int MainWindow::_askQuestion(const char* msg)
    {
        return fToS.askQuestion(msg);
    }

	void MainWindow::_messageDialog(const char* msg)
    {
        return fToS.messageDialog(msg);
    }

    double MainWindow_FtoS::getNumber(const char* c)
    {
        //qDebug() << "get number dialog";
        QSemaphore * s = new QSemaphore(1);
        qreal p;
        s->acquire();
        emit getNumber(s,&p,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
        return (double)p;
    }

    void MainWindow_FtoS::getNumbers(ArrayOfStrings c, double * d)
    {
        //qDebug() << "get number dialog";
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit getNumbers(s,ConvertValue(c), d);
        s->acquire();
        s->release();
        delete s;
    }

    char* MainWindow_FtoS::getString(const char* c)
    {
        //qDebug() << "get string dialog";
        QSemaphore * s = new QSemaphore(1);
        QString p;
        s->acquire();
        emit getString(s,&p,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
        return ConvertValue(p);
    }

    char* MainWindow_FtoS::getFilename()
    {
        QSemaphore * s = new QSemaphore(1);
        QString p;
        s->acquire();
        emit getFilename(s,&p);
        s->acquire();
        s->release();
        delete s;
        return ConvertValue(p);
    }

	int MainWindow_FtoS::askQuestion(const char* c)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
		int x;
        emit askQuestion(s,ConvertValue(c), &x);
        s->acquire();
        s->release();
        delete s;
		return x;
    }

	void MainWindow_FtoS::messageDialog(const char* c)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit messageDialog(s,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
    }

    int MainWindow_FtoS::getSelectedString(const char* c, ArrayOfStrings list,const char* c1, int i)
    {
        //qDebug() << "get item dialog";
        QSemaphore * s = new QSemaphore(1);
        int p;
        s->acquire();
        emit getSelectedString(s,&p,ConvertValue(c),ConvertValue(list),ConvertValue(c1), i);
        s->acquire();
        s->release();
        delete s;
        return p;
    }

	/*******************************************/
	/*******************************************/
	/*******************************************/

	void MainWindow::connectTCFunctions()
	{
		connect(&fToS,SIGNAL(find(QSemaphore*,ItemHandle**,const QString&)),this,SLOT(findItem(QSemaphore*,ItemHandle**,const QString&)));
		connect(&fToS,SIGNAL(findItems(QSemaphore*,QList<ItemHandle*>*,const QStringList&)),
				this,SLOT(findItems(QSemaphore*,QList<ItemHandle*>*,const QStringList&)));

		connect(&fToS,SIGNAL(select(QSemaphore*,ItemHandle*)),this,SLOT(select(QSemaphore*,ItemHandle*)));
		connect(&fToS,SIGNAL(deselect(QSemaphore*)),this,SLOT(deselect(QSemaphore*)));
		connect(&fToS,SIGNAL(allItems(QSemaphore*,QList<ItemHandle*>*)),this,SLOT(allItems(QSemaphore*,QList<ItemHandle*>*)));
		connect(&fToS,SIGNAL(selectedItems(QSemaphore*,QList<ItemHandle*>*)),this,SLOT(selectedItems(QSemaphore*,QList<ItemHandle*>*)));
		connect(&fToS,SIGNAL(itemsOfFamily(QSemaphore*,QList<ItemHandle*>*,const QString&)),this,SLOT(itemsOfFamily(QSemaphore*,QList<ItemHandle*>*,const QString&)));
		connect(&fToS,SIGNAL(itemsOfFamily(QSemaphore*,QList<ItemHandle*>*,const QList<ItemHandle*>&, const QString&)),this,SLOT(itemsOfFamily(QSemaphore*,QList<ItemHandle*>*,const QList<ItemHandle*>&, const QString&)));
		connect(&fToS,SIGNAL(getX(QSemaphore*,qreal*,ItemHandle*)),this,SLOT(getX(QSemaphore*,qreal*,ItemHandle*)));
		connect(&fToS,SIGNAL(getY(QSemaphore*,qreal*,ItemHandle*)),this,SLOT(getY(QSemaphore*,qreal*,ItemHandle*)));

		connect(&fToS,SIGNAL(setPos(QSemaphore*,ItemHandle*,qreal,qreal)),this,SLOT(setPos(QSemaphore*,ItemHandle*,qreal,qreal)));
		connect(&fToS,SIGNAL(setPos(QSemaphore*,const QList<ItemHandle*>&,DataTable<qreal>&)),this,SLOT(setPos(QSemaphore*,const QList<ItemHandle*>&,DataTable<qreal>&)));
		connect(&fToS,SIGNAL(getPos(QSemaphore*,const QList<ItemHandle*>&,DataTable<qreal>*)),this,SLOT(getPos(QSemaphore*,const QList<ItemHandle*>&,DataTable<qreal>*)));

		connect(&fToS,SIGNAL(removeItem(QSemaphore*,ItemHandle*)),this,SLOT(removeItem(QSemaphore*,ItemHandle*)));
		connect(&fToS,SIGNAL(moveSelected(QSemaphore*,qreal,qreal)),this,SLOT(moveSelected(QSemaphore*,qreal,qreal)));
		connect(&fToS,SIGNAL(getName(QSemaphore*,QString*,ItemHandle*)),this,SLOT(itemName(QSemaphore*,QString*,ItemHandle*)));
		connect(&fToS,SIGNAL(setName(QSemaphore*,ItemHandle*,const QString&)),this,SLOT(setName(QSemaphore*,ItemHandle*,const QString&)));

		connect(&fToS,SIGNAL(getNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&)),this,SLOT(itemNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&)));
		connect(&fToS,SIGNAL(getFamily(QSemaphore*,QString*,ItemHandle*)),this,SLOT(itemFamily(QSemaphore*,QString*,ItemHandle*)));
		connect(&fToS,SIGNAL(isA(QSemaphore*,int*,ItemHandle*,const QString&)),this,SLOT(isA(QSemaphore*,int*,ItemHandle*,const QString&)));
		connect(&fToS,SIGNAL(clearText(QSemaphore*)),this,SLOT(clearText(QSemaphore*)));
		connect(&fToS,SIGNAL(outputText(QSemaphore*,const QString&)),this,SLOT(outputText(QSemaphore*,const QString&)));
		connect(&fToS,SIGNAL(errorReport(QSemaphore*,const QString&)),this,SLOT(errorReport(QSemaphore*,const QString&)));
		connect(&fToS,SIGNAL(printFile(QSemaphore*,const QString&)),this,SLOT(printFile(QSemaphore*,const QString&)));
		connect(&fToS,SIGNAL(outputTable(QSemaphore*,const DataTable<qreal>&)),this,SLOT(outputTable(QSemaphore*,const DataTable<qreal>&)));

		connect(&fToS,SIGNAL(createInputWindow(QSemaphore*,const DataTable<qreal>&,const QString&,const QString&,const QString&)),
			this,SLOT(createInputWindow(QSemaphore*,const DataTable<qreal>&,const QString&,const QString&,const QString&)));

		connect(&fToS,SIGNAL(createInputWindow(QSemaphore*,const DataTable<qreal>&,const QString&,MatrixInputFunction)),
			this,SLOT(createInputWindow(QSemaphore*,const DataTable<qreal>&,const QString&,MatrixInputFunction)));
		
		connect(&fToS,SIGNAL(createSliders(QSemaphore*,CThread*,const DataTable<qreal>&,MatrixInputFunction)),
			this,SLOT(createSliders(QSemaphore*,CThread*,const DataTable<qreal>&,MatrixInputFunction)));

		connect(&fToS,SIGNAL(addInputWindowOptions(QSemaphore*,const QString&, int, int, const QStringList&)),
			this,SLOT(addInputWindowOptions(QSemaphore*,const QString&, int, int, const QStringList&)));

		connect(&fToS,SIGNAL(addInputWindowCheckbox(QSemaphore*,const QString&, int, int)),
			this,SLOT(addInputWindowCheckbox(QSemaphore*,const QString&, int, int)));

		connect(&fToS,SIGNAL(openNewWindow(QSemaphore*,const QString&)),this,SLOT(openNewWindow(QSemaphore*,const QString&)));
		connect(&fToS,SIGNAL(isWindows(QSemaphore*,int*)),this,SLOT(isWindows(QSemaphore*,int*)));
		connect(&fToS,SIGNAL(isMac(QSemaphore*,int*)),this,SLOT(isMac(QSemaphore*,int*)));
		connect(&fToS,SIGNAL(isLinux(QSemaphore*,int*)),this,SLOT(isLinux(QSemaphore*,int*)));
		connect(&fToS,SIGNAL(appDir(QSemaphore*,QString*)),this,SLOT(appDir(QSemaphore*,QString*)));
		connect(&fToS,SIGNAL(getChildren(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)),this,SLOT(getChildren(QSemaphore*,QList<ItemHandle*>*,ItemHandle*)));
		connect(&fToS,SIGNAL(getParent(QSemaphore*,ItemHandle**,ItemHandle*)),this,SLOT(getParent(QSemaphore*,ItemHandle**,ItemHandle*)));

		connect(&fToS,SIGNAL(getNumericalData(QSemaphore*,DataTable<qreal>*,ItemHandle*,const QString&)),this,SLOT(getNumericalData(QSemaphore*,DataTable<qreal>*,ItemHandle*,const QString&)));
		connect(&fToS,SIGNAL(setNumericalData(QSemaphore*,ItemHandle*,const QString&,const DataTable<qreal>&)),this,SLOT(setNumericalData(QSemaphore*,ItemHandle*,const QString&,const DataTable<qreal>&)));
		connect(&fToS,SIGNAL(getTextData(QSemaphore*,DataTable<QString>*,ItemHandle*,const QString&)),this,SLOT(getTextData(QSemaphore*,DataTable<QString>*,ItemHandle*,const QString&)));
		connect(&fToS,SIGNAL(setTextData(QSemaphore*,ItemHandle*,const QString&,const DataTable<QString>&)),this,SLOT(setTextData(QSemaphore*,ItemHandle*,const QString&,const DataTable<QString>&)));

		connect(&fToS,SIGNAL(getTextDataNames(QSemaphore*,QStringList*,ItemHandle*)),this,SLOT(getTextDataNames(QSemaphore*,QStringList*,ItemHandle*)));
		connect(&fToS,SIGNAL(getNumericalDataNames(QSemaphore*,QStringList*,ItemHandle*)),this,SLOT(getNumericalDataNames(QSemaphore*,QStringList*,ItemHandle*)));

		connect(&fToS,SIGNAL(zoom(QSemaphore*,qreal)),this,SLOT(zoom(QSemaphore*,qreal)));

		connect(&fToS,SIGNAL(getString(QSemaphore*,QString*,const QString&)),this,SLOT(getString(QSemaphore*,QString*,const QString&)));
        connect(&fToS,SIGNAL(getSelectedString(QSemaphore*,int*,const QString&,const QStringList&,const QString&,int)),this,SLOT(getSelectedString(QSemaphore*,int*,const QString&,const QStringList&,const QString&,int)));
        connect(&fToS,SIGNAL(getNumber(QSemaphore*,qreal*,const QString&)),this,SLOT(getNumber(QSemaphore*,qreal*,const QString&)));
        connect(&fToS,SIGNAL(getNumbers(QSemaphore*,const QStringList&,qreal*)),this,SLOT(getNumbers(QSemaphore*,const QStringList&,qreal*)));
        connect(&fToS,SIGNAL(getFilename(QSemaphore*,QString*)),this,SLOT(getFilename(QSemaphore*,QString*)));

		connect(&fToS,SIGNAL(askQuestion(QSemaphore*,const QString&, int*)),this,SLOT(askQuestion(QSemaphore*,const QString&, int*)));
		connect(&fToS,SIGNAL(messageDialog(QSemaphore*,const QString&)),this,SLOT(messageDialog(QSemaphore*,const QString&)));
	}

	typedef void (*main_api_func)(
		ArrayOfItems (*tc_allItems0)(),
		ArrayOfItems (*tc_selectedItems0)(),
		ArrayOfItems (*tc_itemsOfFamily0)(const char*),
		ArrayOfItems (*tc_itemsOfFamily1)(const char*, ArrayOfItems),
		void * (*tc_find0)(const char*),
		ArrayOfItems (*tc_findItems0)(ArrayOfStrings),
		void (*tc_select0)(void *),
		void (*tc_deselect0)(),
		char* (*tc_getName0)(void *),
		void (*tc_setName0)(void * item,const char* name),
		ArrayOfStrings (*tc_getNames0)(ArrayOfItems),
		char* (*tc_getFamily0)(void *),
		int (*tc_isA0)(void *,const char*),

		void (*tc_clearText)(),
		void (*tc_outputText0)(const char*),
		void (*tc_errorReport0)(const char*),
		void (*tc_outputTable0)(Matrix),
		void (*tc_printFile0)(const char*),

		void (*tc_removeItem0)(void *),

		double (*tc_getY0)(void *),
		double (*tc_getX0)(void *),
		Matrix (*tc_getPos0)(ArrayOfItems),
		void (*tc_setPos0)(void *,double,double),
		void (*tc_setPos1)(ArrayOfItems,Matrix),
		void (*tc_moveSelected0)(double,double),

		int (*tc_isWindows0)(),
		int (*tc_isMac0)(),
		int (*tc_isLinux0)(),
		char* (*tc_appDir0)(),
		
		void (*tc_createInputWindow0)(Matrix,const char*,const char*, const char*),
        void (*tc_createInputWindow1)(Matrix, const char*, void (*f)(Matrix)),
		void (*createSliders)(void*, Matrix, void (*f)(Matrix)),
		
		void (*tc_addInputWindowOptions0)(const char*, int i, int j, ArrayOfStrings),
		void (*tc_addInputWindowCheckbox0)(const char*, int i, int j),
		void (*tc_openNewWindow0)(const char * title),
		
		ArrayOfItems (*tc_getChildren0)(void *),
		void * (*tc_getParent0)(void *),
		
		Matrix (*tc_getNumericalData0)(void *,const char*),
		void (*tc_setNumericalData0)(void *,const char*,Matrix),
		TableOfStrings (*tc_getTextData0)(void *,const char*),
		void (*tc_setTextData0)(void *,const char*, TableOfStrings),
				
		ArrayOfStrings (*tc_getNumericalDataNames0)(void *),
		ArrayOfStrings (*tc_getTextDataNames0)(void *),
		
		void (*tc_zoom0)(double factor),
		
		char* (*getString)(const char*),
		int (*getSelectedString)(const char*, ArrayOfStrings, const char*, int),
		double (*getNumber)(const char*),
		void (*getNumbers)( ArrayOfStrings, double * ),
		char* (*getFilename)(),
		
		int (*askQuestion)(const char*),
		void (*messageDialog)(const char*)

		);

	void MainWindow::setupFunctionPointersSlot(QSemaphore* s,QLibrary * library)
	{
		main_api_func f = (main_api_func)library->resolve("tc_Main_api_initialize");
		if (f)
		{
			f(
				&(_allItems),
				&(_selectedItems),
				&(_itemsOfFamily),
				&(_itemsOfFamily2),
				&(_find),
				&(_findItems),
				&(_select),
				&(_deselect),
				&(_getName),
				&(_setName),
				&(_getNames),
				&(_getFamily),
				&(_isA),
				&(_clearText),
				&(_outputText),
				&(_errorReport),
				&(_outputTable),
				&(_printFile),
				&(_removeItem),
				&(_getY),
				&(_getX),
				&(_getPos),
				&(_setPos),
				&(_setPos2),
				&(_moveSelected),
				&(_isWindows),
				&(_isMac),
				&(_isLinux),
				&(_appDir),
				&(_createInputWindow1),
				&(_createInputWindow2),
				&(_createSliders),
				&(_addInputWindowOptions),
				&(_addInputWindowCheckbox),
				&(_openNewWindow),
				&(_getChildren),
				&(_getParent),
				&(_getNumericalData),
				&(_setNumericalData),
				&(_getTextData),
				&(_setTextData),
				&(_getNumericalDataNames),
				&(_getTextDataNames),
				&(_zoom),
				&(_getString),
				&(_getSelectedString),
				&(_getNumber),
				&(_getNumbers),
				&(_getFilename),
				&(_askQuestion),
				&(_messageDialog)
				);
		}

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

		qRegisterMetaType< DataTable<qreal> >("DataTable<qreal>&");
		qRegisterMetaType< DataTable<QString> >("DataTable<QString>&");		

		qRegisterMetaType< DataTable<qreal>* >("DataTable<qreal>*");
		qRegisterMetaType< DataTable<QString>* >("DataTable<QString>*");

		qRegisterMetaType< ItemHandle* >("ItemHandle*");
		qRegisterMetaType< QList<ItemHandle*> >("QList<ItemHandle*>");
		qRegisterMetaType< QList<ItemHandle*> >("QList<ItemHandle*>&");

		qRegisterMetaType< Tool* >("Tool*");
		qRegisterMetaType< QList<Tool*> >("QList<Tool*>");
		qRegisterMetaType< QList<Tool*> >("QList<Tool*>&");

		qRegisterMetaType< QList<QStringList> >("QList<QStringList>");
		qRegisterMetaType< QList<QStringList> >("QList<QStringList>&");

		qRegisterMetaType< MatrixInputFunction >("MatrixInputFunction");

		qRegisterMetaType< Matrix >("Matrix");
		qRegisterMetaType< TableOfStrings >("TableOfStrings");
		qRegisterMetaType< ArrayOfStrings >("ArrayOfStrings");
		qRegisterMetaType< ArrayOfItems >("ArrayOfItems");
	}
}
