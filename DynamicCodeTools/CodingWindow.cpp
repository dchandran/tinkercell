/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 Provides a text window where C code can be written and run dynamically

****************************************************************************/
#include <QProcess>
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "CodingWindow.h"
#include "PythonTool.h"
#include "LoadCLibraries.h"
#include <QRegExp>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QProcess>
#include <QLibrary>
#include <QProgressBar>
#include <QDir>
#include <QtDebug>

namespace Tinkercell
{
	 /********************************
	      MAIN WINDOW
	 *********************************/

	CodingWindow::CodingWindow()
		 : Tool(tr("Coding Window"),tr("Coding"))
	{
		 QString appDir = QCoreApplication::applicationDirPath();
		 QString homeDir = MainWindow::userTemp();
		 fileName = homeDir + tr("/code.c");
		 
		 toolBar = 0;
		 window = new QMainWindow;

		 tabWidget =  new QTabWidget;
		 editorC = editorPy = editorR = 0;

		 setupEditor();
		 setupMenu();
		 setupDialog();
	}
	
	void CodingWindow::convertCodeToButton()
	{
		if (editorPy && tabWidget->currentIndex() == 1)
			convertCodeToButtonPy();
		else
		if (editorC && tabWidget->currentIndex() == 0)
			convertCodeToButtonC( editorC->toPlainText() );
	}

	void CodingWindow::convertCodeToButtonC( const QString & code, const QString & descr)
	{
		QString userHome = MainWindow::userTemp();
		QString filename = userHome + tr("/code.c");
		QFile qfile(filename);
		if (!qfile.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QMessageBox::about(this, tr("Error"),
					 tr("Cannot write file: ") + userHome + tr("/code.c"));
			return;
		}
		
		QString dllDescription = descr;
		if (dllDescription.isEmpty())
			dllDescription = QInputDialog::getText(this,tr("Program name"),tr("Name your program (2-4 words):"));
		QString dllName = dllDescription;
		dllName.replace(QRegExp("[^A-Za-z0-9]"),tr("_"));

		QTextStream out(&qfile);
		out << tr("void tc_main()\n{\n    tc_addFunction(&run, \"") 
			<< dllDescription 
			<< tr("\" , \"") << dllDescription 
			<< tr("\" ,") << tr("\"New\"")
			<< tr(",") << tr("\"Plugins/c/default.png\"")
			<< tr(",") << tr("\"\"") 
			<< tr(", 1, 0, 0);\n}\n\n")
			<< code;
		qfile.close();

		QString errors;
        QString output;
        QProcess proc;
        QString appDir = QCoreApplication::applicationDirPath();

		QDir userHomeDir(userHome);

		if (!userHomeDir.cd(tr("Plugins")))
		{
			userHomeDir.mkdir(tr("Plugins"));
			userHomeDir.cd(tr("Plugins"));
		}
		
		if (!userHomeDir.cd(tr("c")))
		{
			userHomeDir.mkdir(tr("c"));
			userHomeDir.cd(tr("c"));
		}

		proc.setWorkingDirectory(userHome);

#ifdef Q_WS_WIN

		dllName = tr("Plugins\\c\\") + dllName;
		appDir.replace(tr("/"),tr("\\"));
		userHome.replace(tr("/"),tr("\\"));
        proc.start(tr("\"") + appDir + tr("\"\\win32\\gcc -I\"") + appDir + ("\"\\win32\\include -I\"") + appDir + ("\"\\win32\\include\\sys -I\"") + appDir + ("\"/c -L\"") + appDir + ("\"/c -L\"") + appDir + ("\"\\win32\\lib -w --shared ") + filename + tr(" -lodesim -lssa -llapack -lblas -lf2c -o ") + dllName + tr(".dll "));
        proc.waitForFinished();
        errors += (proc.readAllStandardError());
        output += tr("\n\n") + (proc.readAllStandardOutput());
#else
#ifdef Q_WS_MAC

		dllName = tr("Plugins/c/") + dllName;
        proc.start(tr("gcc -bundle -w --shared -I") + appDir + tr("/c -L") + appDir + tr("/c -lodesim -lssa -llapack -lblas -lf2c -o ") + dllName + tr(".dylib ") + filename);
        proc.waitForFinished();
        if (!errors.isEmpty())	errors += tr("\n\n");
        errors += (proc.readAllStandardError());
        if (!output.isEmpty())	output += tr("\n\n");
        output += tr("\n\n") + (proc.readAllStandardOutput());
#else
		dllName = tr("Plugins/c/") + dllName;
        proc.start(tr("gcc -w --shared -fPIC -I") + appDir + tr("/c -L\"") + appDir + tr("/c -lodesim -lssa -llapack -lblas -lf2c -o ") + dllName + tr(".so ") + filename);
        proc.waitForFinished();
        if (!errors.isEmpty())	errors += tr("\n\n");
        errors += (proc.readAllStandardError());
        if (!output.isEmpty())	output += tr("\n\n");
        output += tr("\n\n") + (proc.readAllStandardOutput());
#endif
#endif

        if (console())
            if (!errors.isEmpty())
                console()->error(errors);
            else
                console()->message(output);

        if (errors.size() > 0)
        {
            return;
        }
		
		mainWindow->loadDynamicLibrary(dllName);
	}
		
	
	void CodingWindow::convertCodeToButtonPy()
	{
		if (!editorPy) return;
		
		QString userHome = MainWindow::userHome();
		QDir userHomeDir(userHome);

		if (!userHomeDir.cd(tr("Plugins")))
		{
			userHomeDir.mkdir(tr("Plugins"));
			userHomeDir.cd(tr("Plugins"));
		}
		
		if (!userHomeDir.cd(tr("py")))
		{
			userHomeDir.mkdir(tr("py"));
			userHomeDir.cd(tr("py"));
		}
		
		QString pyDescription = QInputDialog::getText(this,tr("Program name"),tr("Name your program (2-4 words):"));
		QString pyName = pyDescription;
		pyName.replace(QRegExp("[^A-Za-z0-9]"),tr("_"));
		
		QFile pyfile(userHome + tr("/Plugins/py/") + pyName + tr(".py"));
		if (!pyfile.open(QIODevice::WriteOnly | QIODevice::Text))
			return;

		QTextStream outpy(&pyfile);
		outpy << (editorPy->toPlainText());
		pyfile.close();

		convertCodeToButtonC(
			tr("#include \"TC_api.h\"\nvoid run()\n{\n    tc_runPythonFile(\"Plugins/py/") + pyName + tr(".py\");\n    return 0;\n}\n"),
			pyDescription);
	}

	bool CodingWindow::setMainWindow(MainWindow* main)
	{
		Tool::setMainWindow(main);

		QString appDir = QCoreApplication::applicationDirPath();

		QSplitter * splitter1 = new QSplitter(Qt::Horizontal);
        splitter1->addWidget(editorC);
        splitter1->addWidget(new TCFunctionsListView(mainWindow, appDir + tr("/c/API"), QString(), editorC));
        splitter1->setStretchFactor(0,2);

        QSplitter * splitter2 = new QSplitter(Qt::Horizontal);
        splitter2->addWidget(editorPy);

        splitter2->addWidget(new TCFunctionsListView(mainWindow, QString(), appDir + tr("/c/API/Python/TC_py.h"),editorPy));
        splitter2->setStretchFactor(0,2);

        tabWidget->addTab(splitter1,tr("C"));
        tabWidget->addTab(splitter2,tr("Python"));
        tabWidget->setCurrentIndex(1);

        window->setCentralWidget(tabWidget);
        window->setWindowTitle(name);

        QHBoxLayout * layout = new QHBoxLayout;
        layout->addWidget(window);
        layout->setContentsMargins(0,0,0,0);
        setLayout(layout);

		if (mainWindow)
		{
			QDockWidget * dock = 0;
			setWindowTitle(name);
			setWindowIcon(QIcon(tr(":/images/cmd.png")));
			dock = mainWindow->addToolWindow(this, MainWindow::DockWidget, Qt::BottomDockWidgetArea, Qt::BottomDockWidgetArea);
			dock->move(mainWindow->geometry().bottomRight() - QPoint(sizeHint().width()*2,sizeHint().height()));
			dock->hide();
			dock->setFloating(true);

			QToolBar * toolBar = mainWindow->toolBarForTools;
			QAction * action = new QAction(tr("Coding Window"),toolBar);
			action->setIcon(QIcon(tr(":/images/source.png")));

			if (dock)
				connect(action,SIGNAL(triggered()),dock,SLOT(show()));
			else
			{
				if (mainWindow->viewMenu)
				{
					QAction * toggle = mainWindow->viewMenu->addAction(tr("Coding Window"));
					toggle->setCheckable(true);
					connect(toggle,SIGNAL(toggled(bool)),this,SLOT(setVisible(bool)));
				}
				connect(action,SIGNAL(triggered()),this,SLOT(show()));
			}
			toolBar->addAction(action);

			if (mainWindow->tool(tr("Python Interpreter")))
			{
				QWidget * widget = mainWindow->tool(tr("Python Interpreter"));
				if (widget)
				{
					PythonTool * pyTool = static_cast<PythonTool*>(widget);
					connect(this,SIGNAL(runPy(const QString&)),pyTool,SLOT(runPythonCode(const QString&)));
					connect(this,SIGNAL(stopPy()),pyTool,SLOT(stopPython()));

					if (this->toolBar)
					{
						QProgressBar * progressBar = new QProgressBar(this->toolBar);
						progressBar->setRange(0,100);
						this->toolBar->addWidget(progressBar);
						connect(pyTool->pythonInterpreter,SIGNAL(progress(int)),progressBar,SLOT(setValue(int)));
					}
				}
			}

			if (mainWindow->tool(tr("Load C Libraries")))
			{
				QWidget * widget = mainWindow->tool(tr("Load C Libraries"));
				if (widget)
				{
					LoadCLibrariesTool * loadCTool = static_cast<LoadCLibrariesTool*>(widget);
					connect(this,SIGNAL(compileBuildLoadC(const QString&,const QString&,const QString&)),
							loadCTool,SLOT(compileBuildLoadC(const QString&,const QString&,const QString&)));
				}
			}

			if (mainWindow->helpMenu)
			{
				mainWindow->helpMenu->addAction(tr("PySCeS user manual"),this,SLOT(pyscesHelp()));
			}

			return true;
		}
		return false;
	}

	 void CodingWindow::pyscesHelp()
	 {
		QString appDir = QCoreApplication::applicationDirPath();
		//QProcess::execute(appDir + tr("/Documentation/pysces_userguide.pdf"));
		QDesktopServices::openUrl(QUrl(appDir + tr("/Documentation/pysces_userguide.pdf")));
	 }

	 void CodingWindow::about()
	 {
		 QMessageBox::about(this, tr("About C Script Editor"),
					 tr("This tool allows run-time execution of C and Python code."));
	 }

	 void CodingWindow::setupEditor()
	 {
		 QFont font;
		 font.setFamily("Courier");
		 font.setFixedPitch(true);
		 font.setPointSize(10);

		 editorC = new RuntimeCodeEditor;
		 editorC->setFont(font);

		 editorPy = new RuntimeCodeEditor;
		 editorPy->setFont(font);

		 highlighterC = new CandPythonSyntaxHighlighter(editorC->document());

		 highlighterPy = new CandPythonSyntaxHighlighter(editorPy->document());

         editorC->setPlainText(tr("#include \"TC_api.h\"\nvoid run()\n{\n\n\n\n   return; \n}\n"));
		 editorPy->setPlainText(tr("import pytc\n"));
	 }

	 void CodingWindow::setupMenu()
	 {
		 toolBar = new QToolBar(window);
		 QAction * action;

		 action = toolBar->addAction(QIcon(":/images/new.png"),tr("New"),this,SLOT(newDoc()));
		 action->setShortcut(QKeySequence(QKeySequence::New));
		 action->setToolTip(tr("New"));

		 action = toolBar->addAction(QIcon(":/images/open.png"),tr("Open"),this,SLOT(open()));
		 action->setShortcut(QKeySequence(QKeySequence::Open));
		 action->setToolTip(tr("Open"));

		 action = toolBar->addAction(QIcon(":/images/save.png"),tr("Save"),this,SLOT(save()));
		 action->setShortcut(QKeySequence(QKeySequence::Save));
		 action->setToolTip(tr("Save"));

		 action = toolBar->addAction(QIcon(":/images/undo.png"),tr("Undo"),this,SLOT(undo()));
		 action->setToolTip(tr("Undo"));

		 action = toolBar->addAction(QIcon(":/images/redo.png"),tr("Redo"),this,SLOT(redo()));
		 action->setToolTip(tr("Redo"));

		 action = toolBar->addAction(QIcon(":/images/cmd.png"),tr("Command"),&commandDialog,SLOT(exec()));
		 action->setToolTip(tr("Edit command"));

		 action = toolBar->addAction(QIcon(":/images/play.png"),tr("Run"),this,SLOT(run()));
		 action->setToolTip(tr("Run code"));

		 action = toolBar->addAction(QIcon(":/images/stop.png"),tr("Stop"),this,SIGNAL(stopPy()));
		 action->setToolTip(tr("Terminate (Python only)"));

		 action = toolBar->addAction(QIcon(":/images/default.png"),tr("Buttonize"),this,SLOT(convertCodeToButton()));
		 action->setToolTip(tr("Add code to the functions list"));

		 action = toolBar->addAction(QIcon(":/images/about.png"),tr("About"),this,SLOT(about()));
		 action->setToolTip(tr("About"));

		 //new QShortcut(tr("Ctrl+S"),this,SLOT(save()),SLOT(save()),Qt::WidgetShortcut);
		 //new QShortcut(tr("Ctrl+N"),this,SLOT(newDoc()),SLOT(newDoc()),Qt::WidgetShortcut);
		 //new QShortcut(tr("Ctrl+A"),this,SLOT(selectAll()),SLOT(selectAll()),Qt::WidgetShortcut);

		 window->addToolBar(toolBar);

		 //editorC->grabShortcut(tr("Ctrl+S"));
		 //editorC->grabShortcut(tr("Ctrl+N"));
		 //editorC->grabShortcut(tr("Ctrl+A"));
	 }

	void CodingWindow::newDoc()
	{
		if (tabWidget)
		{
			if (editorC && tabWidget->currentIndex() == 0)
			{
				editorC->clear();
				editorC->defaultSavedFilename.clear();
                                editorC->setPlainText(tr("#include \"TC_api.h\"\nvoid run()\n{\n\n\n\n   return 1; \n}\n"));
			}
			if (editorPy && tabWidget->currentIndex() == 1)
			{
				editorPy->clear();
				editorPy->defaultSavedFilename.clear();
				editorPy->setPlainText(tr("import pytc\n"));
			}
			if (editorR && tabWidget->currentIndex() == 2)
			{
				editorR->clear();
				editorR->defaultSavedFilename.clear();
			}
		}
	}

	void CodingWindow::selectAll()
	{
		if (tabWidget)
		{
			if (editorC && tabWidget->currentIndex() == 0)
				editorC->selectAll();
			if (editorPy && tabWidget->currentIndex() == 1)
				editorPy->selectAll();
			if (editorR && tabWidget->currentIndex() == 2)
				editorR->selectAll();
		}
	}

	void CodingWindow::open()
	{
		if (tabWidget)
		{
			if (editorC && tabWidget->currentIndex() == 0)
				editorC->open("C Files (*.c *.cpp *.h)");
			if (editorPy && tabWidget->currentIndex() == 1)
				editorPy->open("Python Files (*.py)");
			if (editorR && tabWidget->currentIndex() == 2)
				editorR->open("R Files (*.R)");
		}
	}

	void CodingWindow::save()
	{
		if (tabWidget)
		{
			if (editorC && tabWidget->currentIndex() == 0)
				editorC->save("C Files (*.c *.cpp *.h)");
			if (editorPy && tabWidget->currentIndex() == 1)
				editorPy->save("Python Files (*.py)");
			if (editorR && tabWidget->currentIndex() == 2)
				editorR->save("R Files (*.R)");
		}
	}

	void CodingWindow::undo()
	{
		if (tabWidget)
		{
			if (editorC && tabWidget->currentIndex() == 0)
				editorC->undo();
			if (editorPy && tabWidget->currentIndex() == 1)
				editorPy->undo();
			if (editorR && tabWidget->currentIndex() == 2)
				editorR->undo();
		}
	}

	void CodingWindow::redo()
	{
		if (tabWidget)
		{
			if (editorC && tabWidget->currentIndex() == 0)
				editorC->redo();
			if (editorPy && tabWidget->currentIndex() == 1)
				editorPy->redo();
			if (editorR && tabWidget->currentIndex() == 2)
				editorR->redo();
		}
	}

	 void CodingWindow::run()
	 {
		if (tabWidget)
		{
			if (editorC && tabWidget->currentIndex() == 0)
				runC(editorC->toPlainText());

			if (editorPy && tabWidget->currentIndex() == 1)
				emit runPy(editorPy->toPlainText());

			if (editorR && tabWidget->currentIndex() == 2)
				return;
		}
	 }

	 void CodingWindow::runC(const QString& code)
	 {
		if (mainWindow == 0) return;

 		QFile qfile(fileName);
		if (!qfile.open(QIODevice::WriteOnly | QIODevice::Text))
			 return;

		QTextStream out(&qfile);
        if (code.contains( QRegExp(tr("void\\s+run\\s*\\(\\s*\\)")) ))
		{
			out << code;
		}
		else
		{
            QMessageBox::information(mainWindow,tr("Error"),tr("no run() function in the code"));
			return;
		}

		qfile.close();

#ifdef Q_WS_WIN
		emit compileBuildLoadC(tr("code.c -lodesim -lssa"),tr("run"),tr("C code"));
#else
		emit compileBuildLoadC(tr("code.c -lodesim -lssa"),tr("run"),tr("C code"));
#endif
	 }

	 void CodingWindow::setupDialog()
	 {
		QGridLayout * layout = new QGridLayout;
		QPushButton * okButton = new QPushButton("Set");
		connect(okButton,SIGNAL(released()),&commandDialog,SLOT(accept()));
		QPushButton * cancelButton = new QPushButton("Cancel");
		connect(cancelButton,SIGNAL(released()),&commandDialog,SLOT(reject()));
		QLabel * label1 = new QLabel(tr("save code as:"));
		QLabel * label2 = new QLabel(tr("compile C using:"));
		QLabel * label3 = new QLabel(tr("compile Python using: "));

		fileNameEdit = new QLineEdit(fileName);
		//commandPyEdit = new QLineEdit(commandPy);
		//commandCEdit = new QLineEdit(commandC);

		layout->addWidget(label1,0,0,Qt::AlignLeft);
		layout->addWidget(label2,1,0,Qt::AlignLeft);
		layout->addWidget(label3,2,0,Qt::AlignLeft);

		layout->addWidget(fileNameEdit,0,1);//,Qt::AlignRight);
		//layout->addWidget(commandCEdit,1,1);//Qt::AlignRight);
		//layout->addWidget(commandPyEdit,2,1);//,Qt::AlignRight);

		layout->addWidget(okButton,3,0,Qt::AlignRight);
		layout->addWidget(cancelButton,3,1);//,Qt::AlignCenter);
		commandDialog.setWindowTitle(tr("Change Build Command"));
		layout->setColumnStretch(1,3);
		commandDialog.setLayout(layout);
		commandDialog.setSizeGripEnabled(true);
		connect(&commandDialog,SIGNAL(accepted()),this,SLOT(dialogFinished()));
	 }

	 void CodingWindow::dialogFinished()
	 {
		if (fileNameEdit == 0 /*|| commandPyEdit == 0 || commandCEdit == 0*/) return;
		fileName = fileNameEdit->text();
		//commandPy = commandPyEdit->text();
		//commandC = commandCEdit->text();
	 }

	 QSize CodingWindow::sizeHint() const
	 {
		 return QSize(500,300);
	 }


	 /********************************
	      TEXT EDITOR
	 *********************************/

	 void RuntimeCodeEditor::open(const QString& ext)
	 {
		 QString fileName;

		 if (fileName.isNull())
			 fileName = QFileDialog::getOpenFileName(this,
				 tr("Open File"), MainWindow::previousFileName, ext);

		 if (!fileName.isEmpty()) {
			 QFile file(fileName);
			 if (file.open(QFile::ReadOnly | QFile::Text))
				 setPlainText(file.readAll());
		 }
	 }

	 void RuntimeCodeEditor::save(const QString& ext)
	 {
		 QString fileName;

		 if (defaultSavedFilename.contains(ext))
			fileName = defaultSavedFilename[ext];

		 if (fileName.isNull() || fileName.isEmpty())
		 {
			 fileName = QFileDialog::getSaveFileName(this, tr("Save File"),MainWindow::previousFileName, ext);
		 }

		 if (!fileName.isNull() && !fileName.isEmpty())
		 {
			 MainWindow::previousFileName = fileName.remove(QRegExp(tr("\\.*")));
			 QFile file(fileName);
			 if (file.open(QFile::WriteOnly | QFile::Text))
			 {
				 file.write(toPlainText().toAscii());
				 defaultSavedFilename[ext] = fileName;
			 }
		 }
	 }

	 void RuntimeCodeEditor::saveAs(const QString& ext)
	 {
		 QString fileName;

		 if (fileName.isNull())
			 fileName = QFileDialog::getSaveFileName(this,
				 tr("Save File"), MainWindow::previousFileName, ext);

		 if (!fileName.isNull() && !fileName.isEmpty())
		 {
			 MainWindow::previousFileName = fileName.remove(QRegExp(tr("\\.*")));
			 QFile file(fileName);
			 if (file.open(QFile::WriteOnly | QFile::Text))
			 {
				 file.write(toPlainText().toAscii());
			 }
		 }
	 }
/*
	 void RuntimeCodeEditor::keyPressEvent ( QKeyEvent * keyEvent )
	 {
		 if (keyEvent == 0) return;
		 if (completer.widget() != this) completer.setWidget(this);
		 int key = keyEvent->key();
		 if (keyEvent->modifiers() == Qt::ControlModifier)
		 {
			 if (key == Qt::Key_Equal)
			 {
				 zoomIn();
			 }
			 else
			 if (key == Qt::Key_Minus)
			 {
				 zoomOut();
			 }
			 else
			 if (key == Qt::Key_A)
			 {
				 selectAll();
			 }
			 else
			 {
				 CodeEditor::keyPressEvent(keyEvent);
			 }
			 return;
		 }

		 if (completer.popup()->isVisible())
		 {
			 // The following keys are forwarded by the completer to the widget
			switch (keyEvent->key())
			{
				case Qt::Key_Enter:
				case Qt::Key_Return:
				case Qt::Key_Escape:
				case Qt::Key_Tab:
					keyEvent->ignore();
					 return;
				case Qt::Key_Backtab:
 					 CodeEditor::keyPressEvent(keyEvent);
					 return; // let the completer do default behavior
				default:
					break;
			}
		}

		 if (keyEvent->text().isEmpty())
		 {
			 CodeEditor::keyPressEvent(keyEvent);
			 return;
		 }
		QTextCursor tc = textCursor();
		tc.movePosition(QTextCursor::Left);
		tc.select(QTextCursor::WordUnderCursor);
		QString completionPrefix = tc.selectedText().trimmed();
		tc.movePosition(QTextCursor::Right);

		if (completionPrefix.length() < 1)
		{
			completer.popup()->hide();
			CodeEditor::keyPressEvent(keyEvent);
			return;
		}

		//auto complete
		if (completionPrefix != completer.completionPrefix())
		{
			completer.setCompletionPrefix(completionPrefix);
			completer.popup()->setCurrentIndex(completer.completionModel()->index(0, 0));
		}
		QRect cr = cursorRect();
		cr.setWidth(completer.popup()->sizeHintForColumn(0)
                    + completer.popup()->verticalScrollBar()->sizeHint().width());

		CodeEditor::keyPressEvent(keyEvent);

		completer.complete(cr); // show functions
	 }
*/
	 void RuntimeCodeEditor::insertCompletion(const QString& completion)
	 {
		 if (completer.widget() != this)
			 return;
		 QTextCursor tc = textCursor();
		 int extra = completion.length() - completer.completionPrefix().length();
		 tc.movePosition(QTextCursor::Left);
		 tc.movePosition(QTextCursor::EndOfWord);
		 tc.insertText(completion.right(extra));
		 setTextCursor(tc);
	 }

	 /********************************
	      FUNCTIONS LIST
	 *********************************/

	 TCFunctionsListView::TCFunctionsListView(MainWindow* mainWindow, const QString& cDir, const QString& pyFile, CodeEditor * textEdit)
	 {
		 console = mainWindow->console();
		 if (!cDir.isEmpty()) readCHeaders(cDir);
		 if (!pyFile.isEmpty()) readPythonHeader(mainWindow, pyFile);
		 sortItems(0,Qt::AscendingOrder);
		 setSelectionMode(QAbstractItemView::SingleSelection);
		 setEditTriggers(QAbstractItemView::NoEditTriggers);
		 setAlternatingRowColors ( true );
		 setHeaderHidden ( true );
		 if (textEdit)
			 connect(this,SIGNAL(insertText(const QString&)),textEdit,SLOT(insertPlainText(const QString&)));
	 }

	 void TCFunctionsListView::readPythonHeader(MainWindow * mainWindow, const QString& filename)
	 {
		QFile file(filename);
		if (!file.open(QFile::ReadOnly)) return;

		QStringList funcNames;

		QRegExp regexExample(tr("\"(.+)\",.+,.+,.*\".*(example.+)\""));

		QTreeWidgetItem * currentItem = new QTreeWidgetItem(QStringList() << tr("pytc"));
		addTopLevelItem(currentItem);
		QString name,ex;
		while (!file.atEnd())
		{
			QString line(file.readLine());
			regexExample.indexIn(line);
			if (regexExample.numCaptures() > 1 && !regexExample.capturedTexts().at(1).isEmpty())
			{
				name = regexExample.capturedTexts().at(1);
				funcNames << name;
				ex = tr("#") + regexExample.capturedTexts().at(2);
				ex.replace(tr("\\\""),tr("\""));
				QTreeWidgetItem * childItem = new QTreeWidgetItem(QStringList() << name << ex);
				currentItem->addChild(childItem);
			}
		}
		expandAll();
		file.close();

		funcNames.sort();
		if (mainWindow && mainWindow->console() && mainWindow->console()->editor())
		{
			QCompleter * completer = new QCompleter(this);
			completer->setModel(new QStringListModel(funcNames, completer));
			completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
			completer->setCaseSensitivity(Qt::CaseInsensitive);
			completer->setWrapAround(false);
			mainWindow->console()->editor()->setCompleter(completer);
		}
	 }

	 void TCFunctionsListView::readCHeaders(const QString& dirname)
	 {
		 QDir dir(dirname);
		 dir.setFilter(QDir::Files);
		 dir.setSorting(QDir::Name);

		 QFileInfoList list = dir.entryInfoList();
		 //QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

		 QHash<QString,QTreeWidgetItem*> tree;

		 for (int i = 0; i < list.size(); ++i)
		 {
			QFileInfo fileInfo = list.at(i);
			QFile file(fileInfo.absoluteFilePath());
			if (fileInfo.completeSuffix().toLower() != tr("h") || !file.open(QFile::ReadOnly)) continue;
			
			QRegExp regexComments(tr("brief\\s*([^\\n\\r]+)"));
			QRegExp regexGroup(tr("ingroup\\s*([^\\n\\r]+)"));
			QRegExp regexFunction(tr("\\s*(\\S+)\\s*(tc_[A-Za-z0-9]+)\\s*(\\([^\\)]*\\))"));
			QTreeWidgetItem * currentItem = 0;
			QString currentComment;
			
			QStringList visitedFunctions;

			 while (!file.atEnd())
			 {
				 QString line(file.readLine());
				 
				 regexGroup.indexIn(line);
				 regexComments.indexIn(line);
				 regexFunction.indexIn(line);
				 if (regexGroup.numCaptures() > 0 && regexGroup.capturedTexts().at(1).length() > 1)
				 {
					QString s = regexGroup.capturedTexts().at(1); //category
					if (s.contains(QRegExp("\\s*init\\s*")))
					{
						currentItem = 0;
						continue;
					}
					if (tree.contains(s))
					{
						currentItem = tree[s];
					}
					else
					{
						currentItem = new QTreeWidgetItem(QStringList() << s);
						tree[s] = currentItem;
						addTopLevelItem(currentItem);
					}
				 }

				 if (regexComments.numCaptures() > 0 && regexComments.capturedTexts().at(1).length() > 1)
				 {
					QString s = regexComments.capturedTexts().at(1); //comment
					currentComment = s;
				 }

				 if (currentItem && regexFunction.numCaptures() > 0 && regexFunction.capturedTexts().at(1).length() > 0)
				 {
					QString name, str;
					name = regexFunction.capturedTexts().at(2);
					if (visitedFunctions.contains(name)) continue;
					visitedFunctions << name;
					if (regexFunction.capturedTexts().at(1) == tr("void"))
						str = regexFunction.capturedTexts().at(2) + regexFunction.capturedTexts().at(3);
					else
						str = regexFunction.capturedTexts().at(1)
								+ tr(" var = ")
								+ regexFunction.capturedTexts().at(2)
								+ regexFunction.capturedTexts().at(3);

					QTreeWidgetItem * childItem = new QTreeWidgetItem(QStringList() << name << str);
					currentItem->addChild(childItem);
					childItem->setToolTip(1,currentComment);
					childItem->setWhatsThis(1,currentComment);
				 }
			 }
			file.close();
		 }

		 //QApplication::restoreOverrideCursor();
	 }

	 void TCFunctionsListView::mouseDoubleClickEvent ( QMouseEvent *  )
	 {
		 QTreeWidgetItem * item = this->currentItem();
		 if (item && indexOfTopLevelItem(item) == -1)
		 {
			emit insertText(item->text(1));
		 }
	 }

	 void TCFunctionsListView::keyPressEvent ( QKeyEvent * event )
	 {
		 if (event &&
			 (event->key() == Qt::Key_Enter ||
			 event->key() == Qt::Key_Return ||
			  event->key() == Qt::Key_Space ||
			  event->key() == Qt::Key_Tab))
		 {
			QTreeWidgetItem * item = this->currentItem();
			 if (item && indexOfTopLevelItem(item) == -1)
			 {
				emit insertText(item->text(1));
			 }
		 }
	 }

	 void CodingWindow::setVisible ( bool visible )
	 {
		activateWindow();
		if (isMinimized())
			showNormal();
		Tool::setVisible(visible);
	 }
}
