#include <QCoreApplication>
#include <QInputDialog>
#include <QDesktopServices>
#include "C_API_Slots.h"
#include "ItemHandle.h"
#include "TextEditor.h"
#include "NetworkHandle.h"
#include "GraphicsScene.h"
#include "GraphicsView.h"
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
#include "UndoCommands.h"

namespace Tinkercell
{
	Core_FtoS C_API_Slots::fToS;
	
	C_API_Slots::C_API_Slots(MainWindow * main) : mainWindow(main), getStringDialog(0)
	{ 
		connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
		connect(mainWindow,SIGNAL(escapeSignal(const QWidget*)),this,SLOT(escapeSlot(const QWidget*)));
		connect(this,SIGNAL(saveNetwork(const QString&)), mainWindow, SIGNAL(saveNetwork(const QString&)));
		connectTCFunctions();
	}
	
	ConsoleWindow * C_API_Slots::console() const
	{
		if (mainWindow)
			return mainWindow->console();
		return 0;
	}
	
	NetworkHandle * C_API_Slots::currentNetwork() const
	{
		if (mainWindow)
			return mainWindow->currentNetwork();
		return 0;
	}
	
	GraphicsScene * C_API_Slots::currentScene() const
	{
		if (mainWindow)
			return mainWindow->currentScene();
		return 0;
	}

	typedef void (*main_api_func)(
		tc_items (*tc_allItems0)(),
		tc_items (*tc_selectedItems0)(),
		tc_items (*tc_itemsOfFamily0)(const char*),
		tc_items (*tc_itemsOfFamily1)(const char*, tc_items),
		long (*tc_find0)(const char*),
		tc_items (*tc_findItems0)(tc_strings),
		void (*tc_select0)(long),
		void (*tc_deselect0)(),
		const char* (*tc_getName0)(long),
		const char* (*tc_getUniqueName0)(long),
		void (*tc_setName0)(long item,const char* name),
		tc_strings (*tc_getNames0)(tc_items),
		tc_strings (*tc_getUniqueNames0)(tc_items),
		const char* (*tc_getFamily0)(long),
		int (*tc_isA0)(long,const char*),

		void (*tc_clearText)(),
		void (*tc_outputText0)(const char*),
		void (*tc_errorReport0)(const char*),
		void (*tc_outputTable0)(tc_matrix),
		void (*tc_printFile0)(const char*),

		void (*tc_removeItem0)(long),

		double (*tc_getY0)(long),
		double (*tc_getX0)(long),

		tc_matrix (*tc_getPos0)(tc_items),
		void (*tc_setPos0)(long,double,double),

		void (*tc_setPos1)(tc_items,tc_matrix),

		void (*tc_moveSelected0)(double,double),

		int (*tc_isWindows0)(),
		int (*tc_isMac0)(),
		int (*tc_isLinux0)(),
		const char* (*tc_appDir0)(),
		const char* (*tc_homeDir0)(),
		
		void (*tc_createInputWindow0)(tc_matrix,const char*,const char*),
        void (*tc_createInputWindow1)(long, tc_matrix, const char*, void (*f)(tc_matrix)),
		void (*createSliders)(long, tc_matrix, void (*f)(tc_matrix)),
		
		void (*tc_addInputWindowOptions0)(const char*, int i, int j, tc_strings),
		void (*tc_addInputWindowCheckbox0)(const char*, int i, int j),
		void (*tc_openNewWindow0)(const char * title),
		
		tc_items (*tc_getChildren0)(long),
		long (*tc_getParent0)(long),
	
		tc_matrix (*tc_getNumericalData0)(long,const char*),
		void (*tc_setNumericalData0)(long,const char*,tc_matrix),
		tc_table (*tc_getTextData0)(long,const char*),
		void (*tc_setTextData0)(long,const char*, tc_table),
				
		tc_strings (*tc_getNumericalDataNames0)(long),
		tc_strings (*tc_getTextDataNames0)(long),
		
		void (*tc_zoom0)(double factor),

		const char* (*getString)(const char*),
		int (*getSelectedString)(const char*, tc_strings, const char*),
		double (*getNumber)(const char*),
		void (*getNumbers)( tc_strings, double * ),
		const char* (*getFilename)(),
		
		int (*askQuestion)(const char*),
		void (*messageDialog)(const char*),
		void (*openFile)(const char*),
		void (*saveToFile)(const char*),
		
		void (*setSize)(long,double,double,int),
		double (*getWidth)(long),
		double (*getHeight)(long),
		void (*setAngle)(long,double,int),
		double (*getAngle)(long),
		const char* (*getColor)(long),
		void (*setColor)(long,const char*,int),
		
		void (*changeGraphics)(long,const char*),
		void (*changeArrowHead)(long,const char*),
		
		void (*screenshot)(const char*, int, int),
		int (*screenWidth)(),
		int (*screenHeight)(),
		int (*screenX)(),
		int (*screenY)()
	);
	
	void C_API_Slots::setupFunctionPointers(QLibrary * library)
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
				&(_getUniqueName),
				&(_setName),
				&(_getNames),
				&(_getUniqueNames),
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
				&(_homeDir),
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
				&(_messageDialog),
				&(_openFile),
				&(_saveToFile),
				&(_setSize),
				&(_getWidth),
				&(_getHeight),
				&(_setAngle),
				&(_getAngle),
				&(_getColor),
				&(_setColor),
				&(_changeGraphics),
				&(_changeArrowHead),
				&(_screenshot),
				&(_screenWidth),
				&(_screenHeight),
				&(_screenX),
				&(_screenY)
			);
		}
	}
	
	void C_API_Slots::connectTCFunctions()
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
		connect(&fToS,SIGNAL(getUniqueName(QSemaphore*,QString*,ItemHandle*)),this,SLOT(uniqueName(QSemaphore*,QString*,ItemHandle*)));
		connect(&fToS,SIGNAL(setName(QSemaphore*,ItemHandle*,const QString&)),this,SLOT(setName(QSemaphore*,ItemHandle*,const QString&)));

		connect(&fToS,SIGNAL(getNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&)),this,SLOT(itemNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&)));
		connect(&fToS,SIGNAL(getUniqueNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&)),this,SLOT(uniqueNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&)));
		connect(&fToS,SIGNAL(getFamily(QSemaphore*,QString*,ItemHandle*)),this,SLOT(itemFamily(QSemaphore*,QString*,ItemHandle*)));
		connect(&fToS,SIGNAL(isA(QSemaphore*,int*,ItemHandle*,const QString&)),this,SLOT(isA(QSemaphore*,int*,ItemHandle*,const QString&)));
		connect(&fToS,SIGNAL(clearText(QSemaphore*)),this,SLOT(clearText(QSemaphore*)));
		connect(&fToS,SIGNAL(outputText(QSemaphore*,const QString&)),this,SLOT(outputText(QSemaphore*,const QString&)));
		connect(&fToS,SIGNAL(errorReport(QSemaphore*,const QString&)),this,SLOT(errorReport(QSemaphore*,const QString&)));
		connect(&fToS,SIGNAL(printFile(QSemaphore*,const QString&)),this,SLOT(printFile(QSemaphore*,const QString&)));
		connect(&fToS,SIGNAL(outputTable(QSemaphore*,const DataTable<qreal>&)),this,SLOT(outputTable(QSemaphore*,const DataTable<qreal>&)));

		connect(&fToS,SIGNAL(createInputWindow(QSemaphore*,const DataTable<qreal>&,const QString&,const QString&)),
			this,SLOT(createInputWindow(QSemaphore*,const DataTable<qreal>&,const QString&,const QString&)));

		connect(&fToS,SIGNAL(createInputWindow(QSemaphore*,long, const DataTable<qreal>&,const QString&,MatrixInputFunction)),
			this,SLOT(createInputWindow(QSemaphore*,long, const DataTable<qreal>&,const QString&,MatrixInputFunction)));
		
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
		connect(&fToS,SIGNAL(homeDir(QSemaphore*,QString*)),this,SLOT(homeDir(QSemaphore*,QString*)));
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
        connect(&fToS,SIGNAL(getSelectedString(QSemaphore*,int*,const QString&,const QStringList&,const QString&)),this,SLOT(getSelectedString(QSemaphore*,int*,const QString&,const QStringList&,const QString&)));
        connect(&fToS,SIGNAL(getNumber(QSemaphore*,qreal*,const QString&)),this,SLOT(getNumber(QSemaphore*,qreal*,const QString&)));
        connect(&fToS,SIGNAL(getNumbers(QSemaphore*,const QStringList&,qreal*)),this,SLOT(getNumbers(QSemaphore*,const QStringList&,qreal*)));
        connect(&fToS,SIGNAL(getFilename(QSemaphore*,QString*)),this,SLOT(getFilename(QSemaphore*,QString*)));

		connect(&fToS,SIGNAL(askQuestion(QSemaphore*,const QString&, int*)),this,SLOT(askQuestion(QSemaphore*,const QString&, int*)));
		connect(&fToS,SIGNAL(messageDialog(QSemaphore*,const QString&)),this,SLOT(messageDialog(QSemaphore*,const QString&)));
		connect(&fToS,SIGNAL(openFile(QSemaphore*,const QString&)),this,SLOT(openFile(QSemaphore*,const QString&)));
		connect(&fToS,SIGNAL(saveToFile(QSemaphore*,const QString&)),this,SLOT(saveToFile(QSemaphore*,const QString&)));

		connect(&fToS,SIGNAL(setSize(QSemaphore*, ItemHandle*,double,double,int)),this,SLOT(setSize(QSemaphore*, ItemHandle*,double,double,int)));
		connect(&fToS,SIGNAL(getWidth(QSemaphore*, ItemHandle*, double*)),this,SLOT(getWidth(QSemaphore*, ItemHandle*, double*)));
		connect(&fToS,SIGNAL(getHeight(QSemaphore*, ItemHandle*,double*)),this,SLOT(getHeight(QSemaphore*, ItemHandle*,double*)));
		connect(&fToS,SIGNAL(setAngle(QSemaphore*, ItemHandle*,double,int)),this,SLOT(setAngle(QSemaphore*, ItemHandle*,double,int)));
		connect(&fToS,SIGNAL(getAngle(QSemaphore*, ItemHandle*, double*)),this,SLOT(getAngle(QSemaphore*, ItemHandle*, double*)));
		
		connect(&fToS,SIGNAL(getColor(QSemaphore*,QString*,ItemHandle*)),this,SLOT(getColor(QSemaphore*,QString*,ItemHandle*)));
		connect(&fToS,SIGNAL(setColor(QSemaphore*,ItemHandle*,const QString&,int)),this,SLOT(setColor(QSemaphore*,ItemHandle*,const QString&,int)));
		connect(&fToS,SIGNAL(changeGraphics(QSemaphore*,ItemHandle*,const QString&)),this,SLOT(changeGraphics(QSemaphore*,ItemHandle*,const QString&)));
		connect(&fToS,SIGNAL(changeArrowHead(QSemaphore*,ItemHandle*,const QString&)),this,SLOT(changeArrowHead(QSemaphore*,ItemHandle*,const QString&)));

		connect(&fToS,SIGNAL(screenshot(QSemaphore*, const QString&, int, int)),this,SLOT(screenshot(QSemaphore*, const QString&, int, int)));
		connect(&fToS,SIGNAL(screenHeight(QSemaphore*, int*)),this,SLOT(screenHeight(QSemaphore*, int*)));
		connect(&fToS,SIGNAL(screenWidth(QSemaphore*, int*)),this,SLOT(screenWidth(QSemaphore*, int*)));
		connect(&fToS,SIGNAL(screenX(QSemaphore*, int*)),this,SLOT(screenX(QSemaphore*, int*)));
		connect(&fToS,SIGNAL(screenY(QSemaphore*, int*)),this,SLOT(screenY(QSemaphore*, int*)));
	}
	
	void C_API_Slots::zoom(QSemaphore* sem, qreal factor)
	{
		if (currentScene())
		{
			currentScene()->zoom(factor);
		}
		if (sem)
			sem->release();
	}

	void C_API_Slots::clearText(QSemaphore* sem)
	{
	    if (console())
            console()->clear();
		if (sem)
			sem->release();
	}

	void C_API_Slots::outputText(QSemaphore* sem,const QString& text)
	{
		if (console())
            console()->message(text);
		if (sem)
			sem->release();
	}

	void C_API_Slots::errorReport(QSemaphore* sem,const QString& text)
	{
		if (console())
            console()->error(text);
		if (sem)
			sem->release();
	}

	void C_API_Slots::printFile(QSemaphore* sem,const QString& filename)
	{
		QString appDir = QCoreApplication::applicationDirPath();

		QString name[] = {
			MainWindow::tempDir() + tr("/") + filename,
			MainWindow::homeDir() + tr("/") + filename,
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

	void C_API_Slots::outputTable(QSemaphore* sem,const DataTable<qreal>& table)
	{
		if (console())
            console()->printTable(table);
		if (sem)
			sem->release();
	}
	
	void C_API_Slots::createInputWindow(QSemaphore* s,const DataTable<qreal>& data, const QString& title,const QString& function)
	{
		SimpleInputWindow::CreateWindow(mainWindow,title,function,data);
		if (s)
			s->release();
	}
	
	void C_API_Slots::createInputWindow(QSemaphore* s, long ptr, const DataTable<qreal>& dat,const QString& title, MatrixInputFunction f)
	{
		CThread * thread = static_cast<CThread*>( (void*)ptr );
		if (CThread::cthreads.contains(thread))
			SimpleInputWindow::CreateWindow(thread,title,f,dat);
		if (s)
			s->release();
	}
	
	void C_API_Slots::createSliders(QSemaphore* s, CThread * cthread, const DataTable<qreal>& data, MatrixInputFunction f)
	{
		if (cthread)
		{
			cthread->setFunction(f);
			
			MultithreadedSliderWidget * widget = new MultithreadedSliderWidget(mainWindow, cthread, Qt::Horizontal);
			
			QStringList names(data.rowNames());
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
	
	void C_API_Slots::addInputWindowOptions(QSemaphore* s,const QString& name, int i, int j,const QStringList& options)
	{
		SimpleInputWindow::AddOptions(name,i,j,options);
		if (s)
			s->release();
	}
	void C_API_Slots::addInputWindowCheckbox(QSemaphore* s,const QString& name, int i, int j)
	{
		QStringList list;
		list << "Yes" << "No";
		SimpleInputWindow::AddOptions(name,i,j,list);
		if (s)
			s->release();
	}
	void C_API_Slots::openNewWindow(QSemaphore* s,const QString& name)
	{
		if (mainWindow)
		{
			mainWindow->newScene();
			if (currentNetwork())
				currentNetwork()->setWindowTitle(name);
		}
		if (s)
			s->release();
	}
	void C_API_Slots::isWindows(QSemaphore* s, int * i)
	{
#ifdef Q_WS_WIN
		if (i) (*i) = 1;
#else
		if (i) (*i) = 0;
#endif
		if (s)
			s->release();
	}
	void C_API_Slots::isMac(QSemaphore* s, int * i)
	{
#ifdef Q_WS_MAC
		if (i) (*i) = 1;
#else
		if (i) (*i) = 0;
#endif
		if (s)
			s->release();
	}

	void C_API_Slots::isLinux(QSemaphore* s, int * i)
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

	void C_API_Slots::appDir(QSemaphore* s, QString * dir)
	{
		QString appDir = QCoreApplication::applicationDirPath();
		if (dir)
			(*dir) = appDir;
		if (s)
			s->release();
	}

	void C_API_Slots::homeDir(QSemaphore* s, QString * dir)
	{
		QString homeDir = MainWindow::homeDir();
		if (dir)
			(*dir) = homeDir;
		if (s)
			s->release();
	}


	void C_API_Slots::allItems(QSemaphore* s,QList<ItemHandle*>* returnPtr)
	{
		NetworkHandle * win = currentNetwork();
		if (!win || !returnPtr)
		{
			if (s) s->release();
			return;
		}

		if (returnPtr)
			(*returnPtr) = win->handles(true);

		if (s)
			s->release();
	}

	void C_API_Slots::itemsOfFamily(QSemaphore* s,QList<ItemHandle*>* returnPtr, const QString& family)
	{
		NetworkHandle * win = currentNetwork();
		if (!win || !returnPtr)
		{
			if (s) s->release();
			return;
		}

		if (returnPtr)
		{
			QList<ItemHandle*> handles = win->handles();
			for (int i=0; i < handles.size(); ++i)
				if (handles[i] && handles[i]->isA(family))
					(*returnPtr) += handles[i];
		}

		if (s)
			s->release();
	}

	void C_API_Slots::itemsOfFamily(QSemaphore* s,QList<ItemHandle*>* returnPtr,const QList<ItemHandle*>& handles,const QString& family)
	{
		NetworkHandle * win = currentNetwork();
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

	void C_API_Slots::selectedItems(QSemaphore* s,QList<ItemHandle*>* returnPtr)
	{
		GraphicsScene * scene = currentScene();
		if (!scene || !returnPtr)
		{
			if (s) s->release();
			return;
		}

		QList<QGraphicsItem*> & selected = scene->selected();

		if (returnPtr)
		{
			ItemHandle * h;
			for (int i=0; i < selected.size(); ++i)
				if ((h = getHandle(selected[i])) && !returnPtr->contains(h))
					(*returnPtr) << h;
		}

		if (s)
			s->release();
	}

	void C_API_Slots::screenshot(QSemaphore * s, const QString& fileName, int w, int h)
	{
		GraphicsScene * scene = currentScene();
		if (scene)
		{
			if (fileName.endsWith(tr("pdf"),Qt::CaseInsensitive))
			{
				QPrinter printer(QPrinter::HighResolution);
				printer.setOutputFormat(QPrinter::PdfFormat);
				printer.setOrientation(QPrinter::Landscape);
				printer.setPageSize(QPrinter::A4);
				printer.setOutputFileName(fileName);
				scene->print(&printer);
			}
			else
			if (fileName.endsWith(tr("jpg"),Qt::CaseInsensitive) || fileName.endsWith(tr("jpeg"),Qt::CaseInsensitive))
			{
				QPixmap printer(w,h);
				printer.fill();
				scene->print(&printer);
				printer.save(fileName,"jpg");
			}
			else
			{
				QPixmap printer(w,h);
				printer.fill();
				scene->print(&printer);
				printer.save(fileName,"png");
			}
		}
		if (s)
			s->release();
	}
	
	void C_API_Slots::screenWidth(QSemaphore * s, int * w)
	{
		GraphicsScene * scene = currentScene();
		if (scene && w)
		{
			QRectF viewport = scene->visibleRegion();
			(*w) = (int)viewport.width();
		}
		if (s)
			s->release();
	}
	
	void C_API_Slots::screenHeight(QSemaphore * s, int * h)
	{
		GraphicsScene * scene = currentScene();
		if (scene && h)
		{
			QRectF viewport = scene->visibleRegion();
			(*h) = (int)viewport.height();
		}
		if (s)
			s->release();
	}
	
	void C_API_Slots::screenX(QSemaphore * s, int * x)
	{
		GraphicsScene * scene = currentScene();
		if (scene && x)
		{
			QRectF viewport = scene->visibleRegion();
			(*x) = (int)viewport.x();
		}
		if (s)
			s->release();
	}
	
	void C_API_Slots::screenY(QSemaphore * s, int * y)
	{
		GraphicsScene * scene = currentScene();
		if (scene && y)
		{
			QRectF viewport = scene->visibleRegion();
			(*y) = (int)viewport.y();
		}
		if (s)
			s->release();
	}

	void C_API_Slots::itemNames(QSemaphore* s,QStringList* list,const QList<ItemHandle*>& items)
	{
		if (list)
		{
			(*list).clear();
			for (int i=0; i < items.size(); ++i)
			{
				if (items[i])
					(*list) << items[i]->name;
			}
		}
		if (s)
			s->release();
	}

	void C_API_Slots::uniqueNames(QSemaphore* s,QStringList* list,const QList<ItemHandle*>& items)
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

	void C_API_Slots::itemName(QSemaphore* s,QString* name,ItemHandle* handle)
	{
		if (handle && name)
		{
			(*name) = handle->name;
		}
		if (s)
			s->release();
	}

	void C_API_Slots::uniqueName(QSemaphore* s,QString* name,ItemHandle* handle)
	{
		if (handle && name)
		{
			(*name) = handle->fullName(tr("_"));
		}
		if (s)
			s->release();
	}

	void C_API_Slots::setName(QSemaphore* s,ItemHandle* handle,const QString& name)
	{
		if (handle && !name.isNull() && !name.isEmpty() && currentNetwork())
		{
			currentNetwork()->rename(handle,name);
		}
		if (s)
			s->release();
	}

	void C_API_Slots::itemFamily(QSemaphore* s,QString* family,ItemHandle* handle)
	{
		if (handle && family && handle->family())
		{
			(*family) = handle->family()->name();
		}
		if (s)
			s->release();
	}

	void C_API_Slots::isA(QSemaphore* s,int* boolean,ItemHandle* handle,const QString& family)
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

	void C_API_Slots::findItem(QSemaphore* s,ItemHandle** returnPtr,const QString& name)
	{
		NetworkHandle * win = currentNetwork();
		if (!win || !returnPtr)
		{
			if (returnPtr)
				(*returnPtr) = 0;
			if (s) s->release();
			return;
		}

		(*returnPtr) = 0;

		QList<ItemHandle*> items = win->findItem(name);
		
		if (items.isEmpty())
		{
			QList< QPair<ItemHandle*, QString> > data = win->findData(name);
			if (data.size() > 0 && data[0].first)
				(*returnPtr) = data[0].first;
		}
		else
			(*returnPtr) = items[0];
		
		if (s)
			s->release();
	}

	void C_API_Slots::findItems(QSemaphore* s,QList<ItemHandle*>* returnPtr,const QStringList& names)
	{
		NetworkHandle * win = currentNetwork();
		if (!win || !returnPtr)
		{
			if (returnPtr)
				returnPtr->clear();
			if (s) s->release();
			return;
		}

		QList<ItemHandle*> items = win->findItem(names);
		
		if (items.isEmpty())
		{
			QList< QPair<ItemHandle*, QString> > data = win->findData(names);
			for (int i=0; i < data.size(); ++i)
				if (data[i].first && !items.contains(data[i].first))
					items << data[i].first;
		}
		
		(*returnPtr) = items;

		if (s)
			s->release();
	}

	/*! \brief select an item in the current scene.
	* \param graphics item pointer
	*/
	void C_API_Slots::select(QSemaphore* sem,ItemHandle* item)
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
	void C_API_Slots::deselect(QSemaphore* sem)
	{
		mainWindow->sendEscapeSignal();
		if (sem)
			sem->release();
	}


	/*! \brief delete an item in the current scene. This function is intended to be used
	in a C-style plugin or script.
	*/
	void C_API_Slots::removeItem(QSemaphore* sem, ItemHandle * item)
	{
		if (currentScene() && item)
			currentScene()->remove(tr("item removed"),item->graphicsItems);
		//else
		//if (currentTextEditor() && item)

		if (sem)
			sem->release();
	}

	void C_API_Slots::getX(QSemaphore * s, qreal * returnPtr, ItemHandle * item)
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

	void C_API_Slots::getY(QSemaphore * s, qreal * returnPtr, ItemHandle * item)
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

	void C_API_Slots::moveSelected(QSemaphore* sem,qreal dx, qreal dy)
	{
		if (currentScene())
		{
			currentScene()->move( currentScene()->moving() , QPointF(dx,dy) );
		}
		if (sem)
			sem->release();
	}

	void C_API_Slots::setPos(QSemaphore* sem, ItemHandle * item, qreal x, qreal y)
	{
		if (currentScene() && item && !item->graphicsItems.isEmpty() && item->graphicsItems[0])
		{
			currentScene()->move( item->graphicsItems , QPointF(x,y) - item->graphicsItems[0]->scenePos() );
		}
		if (sem)
			sem->release();
	}

	void C_API_Slots::setPos(QSemaphore* sem,const QList<ItemHandle*>& items, DataTable<qreal>& pos)
	{
		if (currentScene() && !items.isEmpty() && items.size() == pos.rows() && pos.columns() == 2)
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
                console()->message(QString::number(items.size()) + tr(" ") + QString::number(pos.columns()));

		if (sem)
			sem->release();
	}

	void C_API_Slots::getPos(QSemaphore* sem,const QList<ItemHandle*>& items, DataTable<qreal>* pos)
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
			(*pos).columnName(0) = tr("x");
			(*pos).columnName(1) = tr("y");
			for (int i=0; i < p.size(); ++i)
			{
				(*pos).value(i,0) = p[i].rx();
				(*pos).value(i,1) = p[i].ry();
			}
		}
		if (sem)
			sem->release();
	}


	void C_API_Slots::getNumericalData(QSemaphore* sem,DataTable<qreal>* dat,ItemHandle* item,const QString& tool)
	{
		if (!item && currentNetwork())
			item = currentNetwork()->globalHandle();

		if (dat && item && item->hasNumericalData(tool))
		{
			(*dat) = item->numericalDataTable(tool);
		}
		if (sem)
			sem->release();
	}

	void C_API_Slots::setNumericalData(QSemaphore* sem,ItemHandle* item,const QString& tool, const DataTable<qreal>& dat)
	{
		if (!item && currentNetwork())
			item = currentNetwork()->globalHandle();

		if (item)
		{
			if (!item->hasNumericalData(tool))
			{
				item->numericalDataTable(tool) = DataTable<qreal>();
			}
			NetworkHandle * win = currentNetwork();
			if (win)
			{
				win->changeData(tool + tr(" changed for ") + item->fullName(),item,tool,&dat);
			}
		}
		if (sem)
			sem->release();
	}

	void C_API_Slots::getTextData(QSemaphore* sem,DataTable<QString>* dat,ItemHandle* item,const QString& tool)
	{
		if (!item && currentNetwork())
			item = currentNetwork()->globalHandle();

		if (dat && item && item->hasTextData(tool))
		{
			(*dat) = item->textDataTable(tool);
		}
		if (sem)
			sem->release();
	}

	void C_API_Slots::setTextData(QSemaphore* sem,ItemHandle* item,const QString& tool, const DataTable<QString>& dat)
	{
		if (!item && currentNetwork())
			item = currentNetwork()->globalHandle();

		if (item)
		{
			if (!item->hasTextData(tool))
			{
				item->textDataTable(tool) = TextDataTable();
			}
			NetworkHandle * win = currentNetwork();
			if (win)
			{
				win->changeData(tool + tr(" changed for ") + item->fullName(),item,tool,&dat);
			}
		}
		if (sem)
			sem->release();
	}
	
	void C_API_Slots::getNumericalDataNames(QSemaphore* sem,QStringList* list,ItemHandle* item)
	{
		if (!item && currentNetwork())
			item = currentNetwork()->globalHandle();
			
		if (item && list)
		{
			(*list) << item->numericalDataNames();
		}
		if (sem)
			sem->release();
	}

	void C_API_Slots::getTextDataNames(QSemaphore*sem,QStringList* list,ItemHandle* item)
	{
		if (!item && currentNetwork())
			item = currentNetwork()->globalHandle();
			
		if (item && list)
		{
			(*list) << item->textDataNames();
		}
		if (sem)
			sem->release();
	}

	void C_API_Slots::getChildren(QSemaphore* sem,QList<ItemHandle*>* ret,ItemHandle* item)
	{
		if (item && ret)
		{
			(*ret) << item->children;
		}
		if (sem)
			sem->release();
	}

	void C_API_Slots::getParent(QSemaphore* sem,ItemHandle** ret,ItemHandle* item)
	{
		if (item && ret)
		{
			(*ret) = item->parent;
		}
		if (sem)
			sem->release();
	}

	void C_API_Slots::_zoom(double x)
	{
		fToS.zoom(x);
	}

	long C_API_Slots::_find(const char* c)
	{
		return fToS.find(c);
	}

	tc_items C_API_Slots::_findItems(tc_strings c)
	{
		return fToS.findItems(c);
	}

	void C_API_Slots::_select(long o)
	{
		return fToS.select(o);
	}

	void C_API_Slots::_deselect()
	{
		return fToS.deselect();
	}

	tc_items C_API_Slots::_allItems()
	{
		return fToS.allItems();
	}

	tc_items C_API_Slots::_itemsOfFamily(const char* f)
	{
		return fToS.itemsOfFamily(f);
	}

	tc_items C_API_Slots::_itemsOfFamily2(const char* f, tc_items a)
	{
		return fToS.itemsOfFamily(f,a);
	}

	tc_items C_API_Slots::_selectedItems()
	{
		return fToS.selectedItems();
	}

	const char* C_API_Slots::_getName(long o)
	{
		return fToS.getName(o);
	}

	const char* C_API_Slots::_getUniqueName(long o)
	{
		return fToS.getUniqueName(o);
	}


	void C_API_Slots::_setName(long o,const char* c)
	{
		return fToS.setName(o,c);
	}

	tc_strings C_API_Slots::_getNames(tc_items a)
	{
		return fToS.getNames(a);
	}

	tc_strings C_API_Slots::_getUniqueNames(tc_items a)
	{
		return fToS.getUniqueNames(a);
	}

	const char* C_API_Slots::_getFamily(long o)
	{
		return fToS.getFamily(o);
	}

	int C_API_Slots::_isA(long o,const char* c)
	{
		return fToS.isA(o,c);
	}

	void C_API_Slots::_removeItem(long o)
	{
		return fToS.removeItem(o);
	}

	void C_API_Slots::_setPos(long o,double x,double y)
	{
		return fToS.setPos(o,x,y);
	}

	void C_API_Slots::_setPos2(tc_items a,tc_matrix m)
	{
		return fToS.setPos(a,m);
	}

	tc_matrix C_API_Slots::_getPos(tc_items a)
	{
		return fToS.getPos(a);
	}

	double C_API_Slots::_getY(long o)
	{
		return fToS.getY(o);
	}

	double C_API_Slots::_getX(long o)
	{
		return fToS.getX(o);
	}

	void C_API_Slots::_moveSelected(double dx,double dy)
	{
		return fToS.moveSelected(dx,dy);
	}

	void C_API_Slots::_clearText()
	{
		return fToS.clearText();
	}

	void C_API_Slots::_outputTable(tc_matrix m)
	{
		return fToS.outputTable(m);
	}

	void C_API_Slots::_outputText(const char * c)
	{
		return fToS.outputText(c);
	}

	void C_API_Slots::_errorReport(const char * c)
	{
		return fToS.errorReport(c);
	}

	void C_API_Slots::_printFile(const char* c)
	{
		return fToS.printFile(c);
	}

	void  C_API_Slots::_createInputWindow1(tc_matrix m,const char* a,const char* b)
	{
		return fToS.createInputWindow(m,a,b);
	}

	void  C_API_Slots::_createInputWindow2(long ptr, tc_matrix m,const char* a, MatrixInputFunction f)
	{
		return fToS.createInputWindow(ptr, m,a,f);
	}
	
	void  C_API_Slots::_createSliders(long c, tc_matrix m,MatrixInputFunction f)
	{
		return fToS.createSliders(c,m,f);
	}

	void  C_API_Slots::_addInputWindowOptions(const char* a,int i, int j, tc_strings c)
	{
		return fToS.addInputWindowOptions(a,i,j,c);
	}

	void  C_API_Slots::_addInputWindowCheckbox(const char* a,int i, int j)
	{
		return fToS.addInputWindowCheckbox(a,i,j);
	}

	void  C_API_Slots::_openNewWindow(const char* c)
	{
		return fToS.openNewWindow(c);
	}

	int  C_API_Slots::_isWindows()
	{
		return fToS.isWindows();
	}

	int  C_API_Slots::_isMac()
	{
		return fToS.isMac();
	}

	int  C_API_Slots::_isLinux()
	{
		return fToS.isLinux();
	}

	const char*  C_API_Slots::_appDir()
	{
		return fToS.appDir();
	}

	const char*  C_API_Slots::_homeDir()
	{
		return fToS.homeDir();
	}


	tc_matrix C_API_Slots::_getNumericalData(long o,const char* a)
	{
		return fToS.getNumericalData(o,a);
	}

	void C_API_Slots::_setNumericalData(long o ,const char* a,tc_matrix m)
	{
		return fToS.setNumericalData(o,a,m);
	}
	
	tc_table C_API_Slots::_getTextData(long o,const char* a)
	{
		return fToS.getTextData(o,a);
	}

	void C_API_Slots::_setTextData(long o ,const char* a,tc_table m)
	{
		return fToS.setTextData(o,a,m);
	}
	
	tc_strings C_API_Slots::_getTextDataNames(long o)
	{
		return fToS.getTextDataNames(o);
	}
	
	tc_strings C_API_Slots::_getNumericalDataNames(long o)
	{
		return fToS.getNumericalDataNames(o);
	}

	tc_items C_API_Slots::_getChildren(long o)
	{
		return fToS.getChildren(o);
	}

	long C_API_Slots::_getParent(long o)
	{
		return fToS.getParent(o);
	}

	void Core_FtoS::zoom(double x)
	{
		emit zoom(0,x);
	}

	long Core_FtoS::find(const char* c)
	{
		QSemaphore * s = new QSemaphore(1);
		ItemHandle * p;
		s->acquire();
		emit find(s,&p,ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(p);
	}

	tc_items Core_FtoS::findItems(tc_strings c)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* p = new QList<ItemHandle*>;
		s->acquire();
		emit findItems(s,p,ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
		tc_items A = ConvertValue(*p);
		delete p;
		return A;
	}

	void Core_FtoS::select(long o)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit select(s,ConvertValue(o));
		s->acquire();
		s->release();
		delete s;
	}

	void Core_FtoS::deselect()
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit deselect(s);
		s->acquire();
		s->release();
		delete s;
	}

	tc_items Core_FtoS::allItems()
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* p = new QList<ItemHandle*>;
		s->acquire();
		emit allItems(s,p);
		s->acquire();
		s->release();
		delete s;
		tc_items A = ConvertValue(*p);
		delete p;
		return A;
	}

	tc_items Core_FtoS::itemsOfFamily(const char * f)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* p = new QList<ItemHandle*>;
		s->acquire();
		emit itemsOfFamily(s,p,ConvertValue(f));
		s->acquire();
		s->release();
		delete s;
		tc_items A = ConvertValue(*p);
		delete p;
		return A;
	}

	tc_items Core_FtoS::itemsOfFamily(const char * f, tc_items a)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*> * list = ConvertValue(a);
		QList<ItemHandle*>* p = new QList<ItemHandle*>;
		s->acquire();
		emit itemsOfFamily(s,p,*list,ConvertValue(f));
		s->acquire();
		s->release();
		delete s;
		tc_items A = ConvertValue(*p);
		delete p;
		delete list;
		return A;
	}

	tc_items Core_FtoS::selectedItems()
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* p = new QList<ItemHandle*>;
		s->acquire();
		emit selectedItems(s,p);
		s->acquire();
		s->release();
		delete s;
		tc_items A = ConvertValue(*p);
		delete p;
		return A;
	}

	const char* Core_FtoS::getName(long o)
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
	
	const char* Core_FtoS::getUniqueName(long o)
	{
		QSemaphore * s = new QSemaphore(1);
		QString p;
		s->acquire();
		emit getUniqueName(s,&p,ConvertValue(o));
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(p);
	}


	void Core_FtoS::setName(long o, const char* c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setName(s,ConvertValue(o),ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
	}

	tc_strings Core_FtoS::getNames(tc_items a0)
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

	tc_strings Core_FtoS::getUniqueNames(tc_items a0)
	{
		QSemaphore * s = new QSemaphore(1);
		QStringList p;
		s->acquire();
		QList<ItemHandle*> * list = ConvertValue(a0);
		emit getUniqueNames(s,&p,*list);
		s->acquire();
		s->release();
		delete s;
		delete list;
		return ConvertValue(p);
	}

	const char* Core_FtoS::getFamily(long a0)
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

	int Core_FtoS::isA(long a0, const char* name)
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

	void Core_FtoS::removeItem(long a0)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit removeItem(s,ConvertValue(a0));
		s->acquire();
		s->release();
		delete s;
	}

	void Core_FtoS::setPos(long a0,double a1,double a2)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setPos(s,ConvertValue(a0),a1,a2);
		s->acquire();
		s->release();
		delete s;
	}

	void Core_FtoS::setPos(tc_items a0,tc_matrix m)
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

	tc_matrix Core_FtoS::getPos(tc_items a0)
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
		tc_matrix m;
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


	double Core_FtoS::getY(long a0)
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

	double Core_FtoS::getX(long a0)
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

	void Core_FtoS::moveSelected(double a0,double a1)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit moveSelected(s,a0,a1);
		s->acquire();
		s->release();
		delete s;
	}

	void Core_FtoS::clearText()
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit clearText(s);
		s->acquire();
		s->release();
		delete s;
	}

	void Core_FtoS::outputTable(tc_matrix m)
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

	void Core_FtoS::outputText(const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit outputText(s,ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
	}

	void Core_FtoS::errorReport(const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit errorReport(s,ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
	}

	void Core_FtoS::printFile(const char * c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit printFile(s,ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
	}

	void Core_FtoS::createInputWindow(tc_matrix m, const char* title,const char* fname)
	{
		DataTable<qreal>* dat = ConvertValue(m);
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit createInputWindow(s,*dat,ConvertValue(title),ConvertValue(fname));
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}

	void Core_FtoS::createInputWindow(long ptr, tc_matrix m, const char* title, MatrixInputFunction f)
	{
		DataTable<qreal>* dat = ConvertValue(m);
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit createInputWindow(s,ptr,*dat,ConvertValue(title),f);
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}
	
	void Core_FtoS::createSliders(long c, tc_matrix m, MatrixInputFunction f)
	{
		CThread * cthread = static_cast<CThread*>((void*)(c));
		DataTable<qreal>* dat = ConvertValue(m);
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit createSliders(s,cthread,*dat,f);
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}

	void Core_FtoS::addInputWindowOptions(const char * a, int i, int j, tc_strings list)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit addInputWindowOptions(s,ConvertValue(a),i,j,ConvertValue(list));
		s->acquire();
		s->release();
		delete s;
	}

	void Core_FtoS::addInputWindowCheckbox(const char * a, int i, int j)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit addInputWindowCheckbox(s,ConvertValue(a),i,j);
		s->acquire();
		s->release();
		delete s;
	}

	void Core_FtoS::openNewWindow(const char* c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit openNewWindow(s,ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
	}

	int Core_FtoS::isWindows()
	{
		int i=0;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit isWindows(s,&i);
		s->acquire();
		s->release();
		delete s;
		return i;
	}

	int Core_FtoS::isMac()
	{
		int i=0;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit isMac(s,&i);
		s->acquire();
		s->release();
		delete s;
		return i;
	}

	int Core_FtoS::isLinux()
	{
		int i=0;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit isLinux(s,&i);
		s->acquire();
		s->release();
		delete s;
		return i;
	}

	const char* Core_FtoS::appDir()
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

	const char* Core_FtoS::homeDir()
	{
		QString dir;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit homeDir(s,&dir);
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(dir);
	}

	tc_matrix Core_FtoS::getNumericalData(long o,const char* c)
	{
		QSemaphore * s = new QSemaphore(1);
		DataTable<qreal> * p = new DataTable<qreal>;
		s->acquire();
		emit getNumericalData(s,p,ConvertValue(o),ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
		tc_matrix m;
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

	void Core_FtoS::setNumericalData(long o, const char * c, tc_matrix M)
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
	
	tc_table Core_FtoS::getTextData(long o,const char* c)
	{
		QSemaphore * s = new QSemaphore(1);
		DataTable<QString> * p = new DataTable<QString>;
		s->acquire();
		emit getTextData(s,p,ConvertValue(o),ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
		tc_table m;
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
	
	void Core_FtoS::setTextData(long o, const char * c, tc_table M)
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

	tc_strings Core_FtoS::getNumericalDataNames(long o)
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

	tc_strings Core_FtoS::getTextDataNames(long o)
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

	tc_items Core_FtoS::getChildren(long o)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*>* p = new QList<ItemHandle*>;
		s->acquire();
		emit getChildren(s,p,ConvertValue(o));
		s->acquire();
		s->release();
		delete s;
		tc_items A = ConvertValue(*p);
		delete p;
		return A;
	}

	long Core_FtoS::getParent(long o)
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

	void C_API_Slots::getNumber(QSemaphore* s,double* p,const QString& name)
    {
        if (p)
        {
            (*p) = QInputDialog::getDouble(mainWindow,tr("Get Number"),name);
        }
        if (s)
            s->release();
    }

    void C_API_Slots::getString(QSemaphore* s,QString* p,const QString& name)
    {
        if (p)
        {
            (*p) = QInputDialog::getText(mainWindow,tr("Get Text"),name);
            (*p).replace(tr("."),tr("_"));
        }
        if (s)
            s->release();
    }

    void C_API_Slots::getNumbers(QSemaphore* s,const QStringList& names,qreal* res)
    {
        QDialog * dialog = new QDialog(mainWindow);

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

        QPushButton * ok = new QPushButton(tr("Done"), mainWindow);
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

    void C_API_Slots::getFilename(QSemaphore* s,QString* p)
    {
        if (p)
        {
            QString file = QFileDialog::getOpenFileName(mainWindow,tr("Select file"));
            if (!file.isNull() && !file.isEmpty())
                (*p) = file;
        }
        if (s)
            s->release();
    }

    void C_API_Slots::getStringListItemSelected(QListWidgetItem * item)
    {
        if (item)
            getStringListNumber = getStringList.currentRow();
        if (getStringDialog)
            getStringDialog->accept();
    }

    void C_API_Slots::getStringListRowChanged ( int  )
    {
        if (getStringList.currentItem())
            getStringListNumber = getStringListText.indexOf(getStringList.currentItem()->text());
    }

    void C_API_Slots::getStringListCanceled (  )
    {
        getStringListNumber = -1;
    }

    void C_API_Slots::getStringSearchTextEdited ( const QString & text )
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

    void C_API_Slots::getSelectedString(QSemaphore* s,int* p,const QString& name, const QStringList& list0,const QString& init)
    {
    	int option = 0;
    	
        if (p)
        {
            getStringListText.clear();
            if (option == 0 && !getStringDialog)
            {
                getStringDialog = new QDialog(mainWindow);
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
                QString s = QInputDialog::getItem(mainWindow,tr("Get Text"),name,list,index,false,&ok);
                if (ok)
                    (*p) = list.indexOf(s);
                else
                    (*p) = -1;
            }
        }
        if (s)
            s->release();
    }

	void C_API_Slots::askQuestion(QSemaphore* s, const QString& msg, int * x)
	{
		QMessageBox::StandardButton ans = QMessageBox::question(mainWindow,tr("Question"),msg,QMessageBox::Yes | QMessageBox::No);

		if (x)

			if (ans == QMessageBox::Yes)
				(*x) = 1;
			else
				(*x) = 0;

		if (s)
			s->release();
	}

	void C_API_Slots::messageDialog(QSemaphore* s, const QString& msg)
	{
		QMessageBox::information(mainWindow,tr("Message"),msg);

		if (s)
			s->release();
	}
	
	void C_API_Slots::openFile(QSemaphore* s,const QString& file)
	{
		mainWindow->open(file);
		if (s)
			s->release();
	}

	void C_API_Slots::saveToFile(QSemaphore* s,const QString& file)
	{
		emit saveNetwork(file);
		if (s)
			s->release();
	}

    const char* C_API_Slots::_getString(const char* title)
    {
        return fToS.getString(title);
    }

    const char* C_API_Slots::_getFilename()
    {
        return fToS.getFilename();
    }

    int C_API_Slots::_getSelectedString(const char* title,tc_strings list,const char* c)
    {
        return fToS.getSelectedString(title,list,c);
    }

    double C_API_Slots::_getNumber(const char* title)
    {
        return fToS.getNumber(title);
    }

    void C_API_Slots::_getNumbers(tc_strings names, double * res)
    {
        return fToS.getNumbers(names,res);
    }

	int C_API_Slots::_askQuestion(const char* msg)
    {
        return fToS.askQuestion(msg);
    }

	void C_API_Slots::_messageDialog(const char* msg)
    {
        return fToS.messageDialog(msg);
    }
    
    void C_API_Slots::_openFile(const char* file)
    {
    	return fToS.openFile(file);
    }
    
     void C_API_Slots::_saveToFile(const char* file)
    {
    	return fToS.saveToFile(file);
    }

    double Core_FtoS::getNumber(const char* c)
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

    void Core_FtoS::getNumbers(tc_strings c, double * d)
    {
        //qDebug() << "get number dialog";
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit getNumbers(s,ConvertValue(c), d);
        s->acquire();
        s->release();
        delete s;
    }

    const char* Core_FtoS::getString(const char* c)
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

    const char* Core_FtoS::getFilename()
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

	int Core_FtoS::askQuestion(const char* c)
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

	void Core_FtoS::messageDialog(const char* c)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit messageDialog(s,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
    }
    
	void Core_FtoS::openFile(const char* c)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit openFile(s,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
    }
    
	void Core_FtoS::saveToFile(const char* c)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit saveToFile(s,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
    }

    int Core_FtoS::getSelectedString(const char* c, tc_strings list,const char* c1)
    {
        //qDebug() << "get item dialog";
        QSemaphore * s = new QSemaphore(1);
        int p;
        s->acquire();
        emit getSelectedString(s,&p,ConvertValue(c),ConvertValue(list),ConvertValue(c1));
        s->acquire();
        s->release();
        delete s;
        return p;
    }
    
    void C_API_Slots::escapeSlot(const QWidget* )
	{
		if (temporarilyColorChanged.size() > 0)
		{
			NodeGraphicsItem::Shape * shape = 0;
			ConnectionGraphicsItem * connection = 0;
			for (int i=0; i < temporarilyColorChanged.size(); ++i)
				if (shape = qgraphicsitem_cast<NodeGraphicsItem::Shape*>(temporarilyColorChanged[i]))
				{
					shape->setPen(shape->defaultPen);
					shape->setBrush(shape->defaultBrush);
				}
				else
					if (connection = ConnectionGraphicsItem::cast(temporarilyColorChanged[i]))
					{
						connection->setPen(connection->defaultPen);
					}
			temporarilyColorChanged.clear();
		}
		
		NodeGraphicsItem * node;
		
		if (temporarilyChangedSize.size() > 0)
		{
			for (int i=0; i < temporarilyChangedSize.size(); ++i)
				if (node = temporarilyChangedSize[i].first)
				{
					QPointF p = node->scenePos();
					node->setBoundingRect( p - temporarilyChangedSize[i].second, p + temporarilyChangedSize[i].second );
				}
			temporarilyChangedSize.clear();
		}
		
		if (temporarilyChangedAngle.size() > 0)
		{
			for (int i=0; i < temporarilyChangedAngle.size(); ++i)
				if (node = temporarilyChangedAngle[i].first)
				{
					node->rotate( - temporarilyChangedAngle[i].second );
				}
			temporarilyChangedAngle.clear();
		}
	}
	
	void C_API_Slots::changeGraphics(QSemaphore* s,ItemHandle* h,const QString& file)
	{
		if (h)
		{
			QStringList filenames;
			QList<NodeGraphicsItem*> nodesList;
			NodeGraphicsItem * node;

			for (int i=0; i < h->graphicsItems.size(); ++i)
				if ((node = NodeGraphicsItem::cast(h->graphicsItems[i])))
				{
					nodesList << node;
					filenames << file;
				}

			if (nodesList.size() > 0)
			{
				ReplaceNodeGraphicsCommand * command = new ReplaceNodeGraphicsCommand(tr("image changed for ") + h->fullName(),nodesList,filenames);
				if (currentNetwork())
					currentNetwork()->history.push(command);
			}
		}

		if (s)
			s->release();
	}

	void C_API_Slots::changeArrowHead(QSemaphore* s,ItemHandle* h,const QString& file)
	{
		if (h)
		{
			QStringList filenames;
			QList<NodeGraphicsItem*> nodesList;
			ConnectionGraphicsItem * conn;

			for (int i=0; i < h->graphicsItems.size(); ++i)
				if ((conn = ConnectionGraphicsItem::cast(h->graphicsItems[i])))
				{
					QList<ArrowHeadItem*> arrows = conn->arrowHeads();

					for (int j=0; j < arrows.size(); ++j)
					{
						nodesList << arrows[j];
						filenames << file;
					}
				}

			if (nodesList.size() > 0)
			{
				ReplaceNodeGraphicsCommand * command = new ReplaceNodeGraphicsCommand(tr("arrowheads changed for ") + h->fullName(),nodesList,filenames,false);
				if (currentNetwork())
					currentNetwork()->history.push(command);
			}
		}

		if (s)
			s->release();
	}
	
	void C_API_Slots::_setSize(long o, double w, double h, int p)
	{
		fToS.setSize(o,w,h,p);
	}

	void Core_FtoS::setSize(long o,double w, double h,int p)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setSize(s,ConvertValue(o),w,h,p);
		s->acquire();
		s->release();
	}
	
	void C_API_Slots::_setAngle(long o, double t,int p)
	{
		fToS.setAngle(o,t,p);
	}

	void Core_FtoS::setAngle(long o,double t,int p)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setAngle(s,ConvertValue(o),t,p);
		s->acquire();
		s->release();
	}

	double C_API_Slots::_getWidth(long o)
	{
		return fToS.getWidth(o);
	}

	double Core_FtoS::getWidth(long o)
	{
		double d=1.0;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit getWidth(s,ConvertValue(o),&d);
		s->acquire();
		s->release();
		return d;
	}
	
	double C_API_Slots::_getHeight(long o)
	{
		return fToS.getHeight(o);
	}

	double Core_FtoS::getHeight(long o)
	{
		double d=1.0;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit getHeight(s,ConvertValue(o),&d);
		s->acquire();
		s->release();
		return d;
	}
	
	double C_API_Slots::_getAngle(long o)
	{
		return fToS.getAngle(o);
	}

	double Core_FtoS::getAngle(long o)
	{
		double d=0.0;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit getAngle(s,ConvertValue(o),&d);
		s->acquire();
		s->release();
		return d;
	}

	const char* C_API_Slots::_getColor(long o)
	{
		return fToS.getColor(o);
	}

	const char* Core_FtoS::getColor(long o)
	{
		QSemaphore * s = new QSemaphore(1);
		QString name("#000000");
		s->acquire();
		emit getColor(s,&name,ConvertValue(o));
		s->acquire();
		s->release();
		return ConvertValue(name);
	}

	void C_API_Slots::_setColor(long o,const char * c, int p)
	{
		return fToS.setColor(o,c,p);
	}

	void Core_FtoS::setColor(long o,const char * c, int p)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setColor(s,ConvertValue(o),ConvertValue(c),p);
		s->acquire();
		s->release();
		return;
	}

	void C_API_Slots::_changeGraphics(long o,const char* f)
	{
		fToS.changeGraphics(o,f);
	}

	void Core_FtoS::changeGraphics(long o,const char* f)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit changeGraphics(s,ConvertValue(o),ConvertValue(f));
		s->acquire();
		s->release();
		return;
	}

	void C_API_Slots::_changeArrowHead(long o,const char* f)
	{
		fToS.changeArrowHead(o,f);
	}

	void Core_FtoS::changeArrowHead(long o,const char* f)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit changeArrowHead(s,ConvertValue(o),ConvertValue(f));
		s->acquire();
		s->release();
		return;
	}
	
	void C_API_Slots::_screenshot(const char* s, int w, int h)
	{
		fToS.screenshot(s,w,h);
	}

	void Core_FtoS::screenshot(const char * file, int w, int h)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit screenshot(s,ConvertValue(file),w,h);
		s->acquire();
		s->release();
	}
	
	int C_API_Slots::_screenHeight()
	{
		return fToS.screenHeight();
	}

	int Core_FtoS::screenHeight()
	{
		int h = 0;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit screenHeight(s,&h);
		s->acquire();
		s->release();
		return h;
	}
	
	int C_API_Slots::_screenWidth()
	{
		return fToS.screenWidth();
	}

	int Core_FtoS::screenWidth()
	{
		int h = 0;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit screenWidth(s,&h);
		s->acquire();
		s->release();
		return h;
	}
	
	int C_API_Slots::_screenX()
	{
		return fToS.screenX();
	}

	int Core_FtoS::screenX()
	{
		int h = 0;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit screenX(s,&h);
		s->acquire();
		s->release();
		return h;
	}
	
	int C_API_Slots::_screenY()
	{
		return fToS.screenY();
	}

	int Core_FtoS::screenY()
	{
		int h = 0;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit screenY(s,&h);
		s->acquire();
		s->release();
		return h;
	}
	
	void C_API_Slots::getColor(QSemaphore* s,QString* name,ItemHandle* handle)
	{
		if (handle && name)
		{
			QGraphicsItem * item = 0;
			for (int i=0; i < handle->graphicsItems.size(); ++i)
			{
				item = handle->graphicsItems[i];
				if (item)
				{
					NodeGraphicsItem * node = NodeGraphicsItem::topLevelNodeItem(item);
					if (node)
					{
						if (node->shapes.size() > 0 && node->shapes[0])
						{
							(*name) = node->shapes[0]->defaultBrush.color().name();
							break;
						}
					}
					else
					{
						ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(item);
						if (connection)
						{
							(*name) = connection->defaultPen.color().name();
							break;
						}
						else
						{
							TextGraphicsItem * text = TextGraphicsItem::cast(item);
							if (text)
							{
								(*name) = text->defaultTextColor().name();
							}
							else
							{
								ControlPoint * cp = ControlPoint::cast(item);
								if (cp)
								{
									(*name) = cp->defaultBrush.color().name();
								}
							}
						}
					}
				}
			}
		}
		if (s)
			s->release();
	}

	void C_API_Slots::setColor(QSemaphore* s,ItemHandle* handle,const QString& name, int permanent)
	{
		GraphicsScene * scene = currentScene();
		QGraphicsItem* item;
		if (handle && scene)
			for (int i=0; i < handle->graphicsItems.size(); ++i)
			{
				item = handle->graphicsItems[i];
				if (item && scene)
				{
					QList<QGraphicsItem*> items;
					QList<QBrush> brushes;
					QList<QPen> pens;
					QColor color(name);
					NodeGraphicsItem * node = NodeGraphicsItem::topLevelNodeItem(item);
					if (node != 0)
					{
						for (int j=0; j < node->shapes.size(); ++j)
						{
							NodeGraphicsItem::Shape * aitem = node->shapes[j];

							if (aitem != 0)
							{
								QBrush brush = aitem->defaultBrush;
								if (brush.gradient() != 0)
								{
									QGradient gradient(*brush.gradient() );
									QGradientStops stops = gradient.stops();
									if (stops.size() > 0)
									{
										color.setAlpha(stops[ stops.size() - 1 ].second.alpha());
										stops[ stops.size() - 1 ].second = color;
										gradient.setStops(stops);
										QBrush newBrush(gradient);
										if (permanent)
										{
											brushes += newBrush;
											items += aitem;
										}
										else
										{
											aitem->setBrush(newBrush);
											if (!temporarilyColorChanged.contains(aitem));
												temporarilyColorChanged << aitem;
										}
									}
								}
								else
								{
									color.setAlpha(brush.color().alpha());
									QBrush newBrush(color);
									if (permanent)
									{
										if (brush.color().name().toLower() == tr("#ffffff") ||
											brush.color().name().toLower() == tr("#000000"))
											brushes += brush;
										else
											brushes += newBrush;
										items += aitem;
									}
									else
									{
										if (brush.color().name().toLower() == tr("#ffffff") ||
											brush.color().name().toLower() == tr("#000000"))
											aitem->setBrush(brush);
										else
											aitem->setBrush(newBrush);
										if (!temporarilyColorChanged.contains(aitem))
											temporarilyColorChanged << aitem;
									}
								}
								QPen newPen(aitem->defaultPen);
								newPen.setColor(QColor(name));
								if (permanent)
								{
									pens += newPen;
								}
								else
								{
									aitem->setPen(newPen);
									temporarilyColorChanged << aitem;
								}
							}
						}
					}
					else
					{
						ConnectionGraphicsItem * connection = ConnectionGraphicsItem::topLevelConnectionItem(item);
						if (connection != 0)
						{
							QPen newPen(color,connection->defaultPen.widthF());
							color.setAlpha(connection->defaultPen.color().alpha());
							if (permanent)
							{
								pens += newPen;
								brushes += QBrush( newPen.color() );
								items += connection;
							}
							else
							{
								connection->setPen(newPen);
								if (!temporarilyColorChanged.contains(connection))
									temporarilyColorChanged << connection;
							}
						}
					}
					if (permanent)
						scene->setBrushAndPen(tr("colors changed"),items,brushes,pens);
				}
			}
			if (s)
				s->release();
	}
	
	void C_API_Slots::setSize(QSemaphore* s, ItemHandle* item,double w,double h, int permanent)
	{
		if (item)
		{
			QSizeF sz(0,0);
			NodeGraphicsItem * node, * mainNode = 0;
			for (int i=0; i < item->graphicsItems.size(); ++i)
				if ((node = NodeGraphicsItem::cast(item->graphicsItems[i])) &&
					node->sceneBoundingRect().width() > sz.width() &&
					node->sceneBoundingRect().height() > sz.height())
				{
					mainNode = node;
				}
			if (mainNode)
			{
				if (permanent && currentScene())
				{
					QPointF p;
					p.rx() = w / mainNode->sceneBoundingRect().width();
					p.ry() = h / mainNode->sceneBoundingRect().height();
					currentScene()->transform(tr("resize ") + item->fullName(), mainNode, p);
				}
				else
				{
					QPointF dp1(w/2, h/2);
					QPointF p = mainNode->scenePos();
					QRectF rect = mainNode->sceneBoundingRect();
					QPointF dp0( rect.center() - rect.topLeft() );
					mainNode->setBoundingRect( p - dp1, p + dp1 );
					
					bool found = false;
					for (int i=0; i < temporarilyChangedSize.size(); ++i)
						if (mainNode == temporarilyChangedSize[i].first)
						{
							temporarilyChangedSize[i].second = dp0;
							found = true;
						}
					if (!found)
						temporarilyChangedSize << QPair<NodeGraphicsItem*,QPointF>(mainNode,dp0);
				}
			}
		}
		if (s)
			s->release();
	}
	
	void C_API_Slots::getWidth(QSemaphore* s, ItemHandle* item, double* x)
	{
		if (item && x)
		{
			QSizeF sz(0,0);
			NodeGraphicsItem * node, * mainNode = 0;
			for (int i=0; i < item->graphicsItems.size(); ++i)
				if ((node = NodeGraphicsItem::cast(item->graphicsItems[i])) &&
					node->sceneBoundingRect().width() > sz.width() &&
					node->sceneBoundingRect().height() > sz.height())
				{
					mainNode = node;
					sz = node->sceneBoundingRect().size();
				}
			if (mainNode)
			{
				(*x) = mainNode->sceneBoundingRect().width();
			}
		}
		if (s)
			s->release();
	}
	
	void C_API_Slots::getHeight(QSemaphore* s, ItemHandle* item,double* x)
	{
		if (item && x)
		{
			QSizeF sz(0,0);
			NodeGraphicsItem * node, * mainNode = 0;
			for (int i=0; i < item->graphicsItems.size(); ++i)
				if ((node = NodeGraphicsItem::cast(item->graphicsItems[i])) &&
					node->sceneBoundingRect().width() > sz.width() &&
					node->sceneBoundingRect().height() > sz.height())
				{
					mainNode = node;
					sz = node->sceneBoundingRect().size();
				}
			if (mainNode)
			{
				(*x) = mainNode->sceneBoundingRect().height();
			}
		}
		if (s)
			s->release();
	}
	
	void C_API_Slots::setAngle(QSemaphore* s, ItemHandle* item,double angle,int permanent)
	{
		if (item)
		{
			QSizeF sz(0,0);
			NodeGraphicsItem * node, * mainNode = 0;
			for (int i=0; i < item->graphicsItems.size(); ++i)
				if ((node = NodeGraphicsItem::cast(item->graphicsItems[i])) &&
					node->sceneBoundingRect().width() > sz.width() &&
					node->sceneBoundingRect().height() > sz.height())
				{
					mainNode = node;
				}
			if (mainNode)
			{
				if (permanent && currentScene())
				{
					currentScene()->transform(tr("rotate ") + item->fullName(), mainNode, QPointF(0,0), angle);
				}
				else
				{
					mainNode->rotate(angle);
					temporarilyChangedAngle << QPair<NodeGraphicsItem*,double>(mainNode,angle);
				}
			}
		}
		if (s)
			s->release();
	}
	
	void C_API_Slots::getAngle(QSemaphore* s, ItemHandle* item, double* angle)
	{
		if (item && angle)
		{
			(*angle) = 0.0;
		}
		if (s)
			s->release();
	}
}

