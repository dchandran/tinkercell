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
#include "CThread.h"
#include "MultithreadedSliderWidget.h"
#include "ConsoleWindow.h"
#include "AbstractInputWindow.h"
#include "TextParser.h"
#include "C_API_Slots.h"

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

		parsersMenu = 0;
		c_api_slots = new C_API_Slots(this);
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

	GraphicsScene * MainWindow::createScene()
	{
		GraphicsScene * scene = new GraphicsScene;
		NetworkHandle * network = new NetworkHandle(this, scene);

		if (!allNetworkWindows.contains(subWindow))
			allNetworkWindows << subWindow;

		popIn(subWindow);
		emit windowOpened(subWindow);

		return scene;
	}

	TextEditor * MainWindow::newTextExitor()
	{
		TextEditor * textedit = new TextEditor;
		NetworkWindow * subWindow = new NetworkWindow(this, textedit);

		if (!allNetworkWindows.contains(subWindow))
			allNetworkWindows << subWindow;

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
	
	QHash<void*,bool> MainWindow::invalidPointers;
}
