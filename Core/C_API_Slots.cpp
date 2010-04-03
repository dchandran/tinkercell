#include <QCoreApplication>
#include <QInputDialog>
#include <QDesktopServices>
#include "C_API_Slots.h"
#include "ItemHandle.h"
#include "TextEditor.h"
#include "TextItem.h"
#include "NetworkWindow.h"
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
		connectTCFunctions();
	}
	
	ConsoleWindow * C_API_Slots::console() const
	{
		if (mainWindow)
			return mainWindow->console();
		return 0;
	}
	
	NetworkWindow * C_API_Slots::currentWindow() const
	{
		if (mainWindow)
			return mainWindow->currentWindow();
		return 0;
	}
	
	GraphicsScene * C_API_Slots::currentScene() const
	{
		if (mainWindow)
			return mainWindow->currentScene();
		return 0;
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
		void (*messageDialog)(const char*),
		
		void (*setSize)(void*,double,double,int),
		double (*getWidth)(void*),
		double (*getHeight)(void*),
		void (*setAngle)(void*,double,int),
		double (*getAngle)(void*),
		int (*getColorR)(void*),
		int (*getColorG)(void*),
		int (*getColorB)(void*),
		void (*setColor)(void*,int,int,int,int),
		
		void (*changeGraphics)(void*,const char*),
		void (*changeArrowHead)(void*,const char*)

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
				&(_messageDialog),
				&(_setSize),
				&(_getWidth),
				&(_getHeight),
				&(_setAngle),
				&(_getAngle),
				&(_getColorR),
				&(_getColorG),
				&(_getColorB),
				&(_setColor),
				&(_changeGraphics),
				&(_changeArrowHead)
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

		connect(&fToS,SIGNAL(setSize(QSemaphore*, ItemHandle*,double,double,int)),this,SLOT(setSize(QSemaphore*, ItemHandle*,double,double,int)));
		connect(&fToS,SIGNAL(getWidth(QSemaphore*, ItemHandle*, double*)),this,SLOT(getWidth(QSemaphore*, ItemHandle*, double*)));
		connect(&fToS,SIGNAL(getHeight(QSemaphore*, ItemHandle*,double*)),this,SLOT(getHeight(QSemaphore*, ItemHandle*,double*)));
		connect(&fToS,SIGNAL(setAngle(QSemaphore*, ItemHandle*,double,int)),this,SLOT(setAngle(QSemaphore*, ItemHandle*,double,int)));
		connect(&fToS,SIGNAL(getAngle(QSemaphore*, ItemHandle*, double*)),this,SLOT(getAngle(QSemaphore*, ItemHandle*, double*)));
		
		connect(&fToS,SIGNAL(getColorR(QSemaphore*,int*,ItemHandle*)),this,SLOT(getColorR(QSemaphore*,int*,ItemHandle*)));
		connect(&fToS,SIGNAL(getColorG(QSemaphore*,int*,ItemHandle*)),this,SLOT(getColorG(QSemaphore*,int*,ItemHandle*)));
		connect(&fToS,SIGNAL(getColorB(QSemaphore*,int*,ItemHandle*)),this,SLOT(getColorB(QSemaphore*,int*,ItemHandle*)));
		connect(&fToS,SIGNAL(setColor(QSemaphore*,ItemHandle*,int,int,int,int)),this,SLOT(setColor(QSemaphore*,ItemHandle*,int,int,int,int)));
		connect(&fToS,SIGNAL(changeGraphics(QSemaphore*,ItemHandle*,const QString&)),this,SLOT(changeGraphics(QSemaphore*,ItemHandle*,const QString&)));
		connect(&fToS,SIGNAL(changeArrowHead(QSemaphore*,ItemHandle*,const QString&)),this,SLOT(changeArrowHead(QSemaphore*,ItemHandle*,const QString&)));
	}
	
	void C_API_Slots::zoom(QSemaphore* sem, qreal factor)
	{
		if (currentScene())
		{
			currentScene()->scaleView(factor);
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

	void C_API_Slots::outputTable(QSemaphore* sem,const DataTable<qreal>& table)
	{
		if (console())
            console()->printTable(table);
		if (sem)
			sem->release();
	}
	
	void C_API_Slots::createInputWindow(QSemaphore* s,const DataTable<qreal>& data, const QString& dll,const QString& function,const QString& title)
	{
		SimpleInputWindow::CreateWindow(mainWindow,title,dll,function,data);
		if (s)
			s->release();
	}
	
	void C_API_Slots::createInputWindow(QSemaphore* s,const DataTable<qreal>& dat,const QString& title, MatrixInputFunction f)
	{
		SimpleInputWindow::CreateWindow(mainWindow,title,f,dat);
		if (s)
			s->release();
	}
	
	void C_API_Slots::createSliders(QSemaphore* s, CThread * cthread, const DataTable<qreal>& data, MatrixInputFunction f)
	{
		if (cthread)
		{
			cthread->setFunction(f);
			
			MultithreadedSliderWidget * widget = new MultithreadedSliderWidget(mainWindow, cthread, Qt::Horizontal);
			
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
	
	void C_API_Slots::addInputWindowOptions(QSemaphore* s,const QString& name, int i, int j,const QStringList& options)
	{
		SimpleInputWindow::AddOptions(name,i,j,options);
		if (s)
			s->release();
	}
	void C_API_Slots::addInputWindowCheckbox(QSemaphore* s,const QString& name, int i, int j)
	{
		SimpleInputWindow::AddOptions(name,i,j);
		if (s)
			s->release();
	}
	void C_API_Slots::openNewWindow(QSemaphore* s,const QString& name)
	{
		if (mainWindow)
		{
			mainWindow->newGraphicsWindow();
			if (currentWindow())
				currentWindow()->setWindowTitle(name);
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

	void C_API_Slots::allItems(QSemaphore* s,QList<ItemHandle*>* returnPtr)
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

	void C_API_Slots::itemsOfFamily(QSemaphore* s,QList<ItemHandle*>* returnPtr, const QString& family)
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

	void C_API_Slots::itemsOfFamily(QSemaphore* s,QList<ItemHandle*>* returnPtr,const QList<ItemHandle*>& handles,const QString& family)
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

	void C_API_Slots::selectedItems(QSemaphore* s,QList<ItemHandle*>* returnPtr)
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

	void C_API_Slots::itemNames(QSemaphore* s,QStringList* list,const QList<ItemHandle*>& items)
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
			(*name) = handle->fullName(tr("_"));
		}
		if (s)
			s->release();
	}

	void C_API_Slots::setName(QSemaphore* s,ItemHandle* handle,const QString& name)
	{
		if (handle && !name.isNull() && !name.isEmpty() && currentWindow())
		{
			currentWindow()->rename(handle,name);
		}
		if (s)
			s->release();
	}

	void C_API_Slots::itemFamily(QSemaphore* s,QString* family,ItemHandle* handle)
	{
		if (handle && family && handle->family())
		{
			(*family) = handle->family()->name;
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

	void C_API_Slots::findItems(QSemaphore* s,QList<ItemHandle*>* returnPtr,const QStringList& names)
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
		GraphicsScene * scene = currentScene();
		if (scene)
			scene->deselect();
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


	void C_API_Slots::getNumericalData(QSemaphore* sem,DataTable<qreal>* dat,ItemHandle* item,const QString& tool)
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

	void C_API_Slots::setNumericalData(QSemaphore* sem,ItemHandle* item,const QString& tool, const DataTable<qreal>& dat)
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

	void C_API_Slots::getTextData(QSemaphore* sem,DataTable<QString>* dat,ItemHandle* item,const QString& tool)
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

	void C_API_Slots::setTextData(QSemaphore* sem,ItemHandle* item,const QString& tool, const DataTable<QString>& dat)
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
	
	void C_API_Slots::getNumericalDataNames(QSemaphore* sem,QStringList* list,ItemHandle* item)
	{
		if (item && list)
		{
			(*list) << item->data->numericalData.keys();
		}
		if (sem)
			sem->release();
	}

	void C_API_Slots::getTextDataNames(QSemaphore*sem,QStringList* list,ItemHandle* item)
	{
		if (item && list)
		{
			(*list) << item->data->textData.keys();
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

	void* C_API_Slots::_find(const char* c)
	{
		return fToS.find(c);
	}

	ArrayOfItems C_API_Slots::_findItems(ArrayOfStrings c)
	{
		return fToS.findItems(c);
	}

	void C_API_Slots::_select(void* o)
	{
		return fToS.select(o);
	}

	void C_API_Slots::_deselect()
	{
		return fToS.deselect();
	}

	ArrayOfItems C_API_Slots::_allItems()
	{
		return fToS.allItems();
	}

	ArrayOfItems C_API_Slots::_itemsOfFamily(const char* f)
	{
		return fToS.itemsOfFamily(f);
	}

	ArrayOfItems C_API_Slots::_itemsOfFamily2(const char* f, ArrayOfItems a)
	{
		return fToS.itemsOfFamily(f,a);
	}

	ArrayOfItems C_API_Slots::_selectedItems()
	{
		return fToS.selectedItems();
	}

	char* C_API_Slots::_getName(void* o)
	{
		return fToS.getName(o);
	}

	void C_API_Slots::_setName(void* o,const char* c)
	{
		return fToS.setName(o,c);
	}

	ArrayOfStrings C_API_Slots::_getNames(ArrayOfItems a)
	{
		return fToS.getNames(a);
	}

	char* C_API_Slots::_getFamily(void* o)
	{
		return fToS.getFamily(o);
	}

	int C_API_Slots::_isA(void* o,const char* c)
	{
		return fToS.isA(o,c);
	}

	void C_API_Slots::_removeItem(void* o)
	{
		return fToS.removeItem(o);
	}

	void C_API_Slots::_setPos(void* o,double x,double y)
	{
		return fToS.setPos(o,x,y);
	}

	void C_API_Slots::_setPos2(ArrayOfItems a,Matrix m)
	{
		return fToS.setPos(a,m);
	}

	Matrix C_API_Slots::_getPos(ArrayOfItems a)
	{
		return fToS.getPos(a);
	}

	double C_API_Slots::_getY(void* o)
	{
		return fToS.getY(o);
	}

	double C_API_Slots::_getX(void* o)
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

	void C_API_Slots::_outputTable(Matrix m)
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

	void  C_API_Slots::_createInputWindow1(Matrix m,const char* a,const char* b, const char* c)
	{
		return fToS.createInputWindow(m,a,b,c);
	}

	void  C_API_Slots::_createInputWindow2(Matrix m,const char* a, MatrixInputFunction f)
	{
		return fToS.createInputWindow(m,a,f);
	}
	
	void  C_API_Slots::_createSliders(void* c, Matrix m,MatrixInputFunction f)
	{
		return fToS.createSliders(c,m,f);
	}

	void  C_API_Slots::_addInputWindowOptions(const char* a,int i, int j, ArrayOfStrings c)
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

	char*  C_API_Slots::_appDir()
	{
		return fToS.appDir();
	}

	Matrix C_API_Slots::_getNumericalData(void* o,const char* a)
	{
		return fToS.getNumericalData(o,a);
	}

	void C_API_Slots::_setNumericalData(void* o ,const char* a,Matrix m)
	{
		return fToS.setNumericalData(o,a,m);
	}
	
	TableOfStrings C_API_Slots::_getTextData(void* o,const char* a)
	{
		return fToS.getTextData(o,a);
	}

	void C_API_Slots::_setTextData(void* o ,const char* a,TableOfStrings m)
	{
		return fToS.setTextData(o,a,m);
	}
	
	ArrayOfStrings C_API_Slots::_getTextDataNames(void * o)
	{
		return fToS.getTextDataNames(o);
	}
	
	ArrayOfStrings C_API_Slots::_getNumericalDataNames(void * o)
	{
		return fToS.getNumericalDataNames(o);
	}

	ArrayOfItems C_API_Slots::_getChildren(void* o)
	{
		return fToS.getChildren(o);
	}

	void* C_API_Slots::_getParent(void* o)
	{
		return fToS.getParent(o);
	}

	void Core_FtoS::zoom(double x)
	{
		emit zoom(0,x);
	}

	void* Core_FtoS::find(const char* c)
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

	ArrayOfItems Core_FtoS::findItems(ArrayOfStrings c)
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

	void Core_FtoS::select(void* o)
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

	ArrayOfItems Core_FtoS::allItems()
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

	ArrayOfItems Core_FtoS::itemsOfFamily(const char * f)
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

	ArrayOfItems Core_FtoS::itemsOfFamily(const char * f, ArrayOfItems a)
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

	ArrayOfItems Core_FtoS::selectedItems()
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

	char* Core_FtoS::getName(void* o)
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

	void Core_FtoS::setName(void* o, const char* c)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setName(s,ConvertValue(o),ConvertValue(c));
		s->acquire();
		s->release();
		delete s;
	}

	ArrayOfStrings Core_FtoS::getNames(ArrayOfItems a0)
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

	char* Core_FtoS::getFamily(void* a0)
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

	int Core_FtoS::isA(void* a0, const char* name)
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

	void Core_FtoS::removeItem(void* a0)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit removeItem(s,ConvertValue(a0));
		s->acquire();
		s->release();
		delete s;
	}

	void Core_FtoS::setPos(void* a0,double a1,double a2)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setPos(s,ConvertValue(a0),a1,a2);
		s->acquire();
		s->release();
		delete s;
	}

	void Core_FtoS::setPos(ArrayOfItems a0,Matrix m)
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

	Matrix Core_FtoS::getPos(ArrayOfItems a0)
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


	double Core_FtoS::getY(void* a0)
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

	double Core_FtoS::getX(void* a0)
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

	void Core_FtoS::outputTable(Matrix m)
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

	void Core_FtoS::createInputWindow(Matrix m, const char* cfile,const char* fname, const char* title)
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

	void Core_FtoS::createInputWindow(Matrix m, const char* title, MatrixInputFunction f)
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
	
	void Core_FtoS::createSliders(void * c, Matrix m, MatrixInputFunction f)
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

	void Core_FtoS::addInputWindowOptions(const char * a, int i, int j, ArrayOfStrings list)
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
		int i;
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
		int i;
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
		int i;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit isLinux(s,&i);
		s->acquire();
		s->release();
		delete s;
		return i;
	}

	char* Core_FtoS::appDir()
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

	Matrix Core_FtoS::getNumericalData(void* o,const char* c)
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

	void Core_FtoS::setNumericalData(void* o, const char * c, Matrix M)
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
	
	TableOfStrings Core_FtoS::getTextData(void* o,const char* c)
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
	
	void Core_FtoS::setTextData(void* o, const char * c, TableOfStrings M)
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

	ArrayOfStrings Core_FtoS::getNumericalDataNames(void* o)
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

	ArrayOfStrings Core_FtoS::getTextDataNames(void* o)
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

	ArrayOfItems Core_FtoS::getChildren(void* o)
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

	void* Core_FtoS::getParent(void* o)
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

    void C_API_Slots::getSelectedString(QSemaphore* s,int* p,const QString& name, const QStringList& list0,const QString& init, int option)
    {
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

    char* C_API_Slots::_getString(const char* title)
    {
        return fToS.getString(title);
    }

    char* C_API_Slots::_getFilename()
    {
        return fToS.getFilename();
    }

    int C_API_Slots::_getSelectedString(const char* title,ArrayOfStrings list,const char* c, int i)
    {
        return fToS.getSelectedString(title,list,c,i);
    }

    double C_API_Slots::_getNumber(const char* title)
    {
        return fToS.getNumber(title);
    }

    void C_API_Slots::_getNumbers(ArrayOfStrings names, double * res)
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

    void Core_FtoS::getNumbers(ArrayOfStrings c, double * d)
    {
        //qDebug() << "get number dialog";
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit getNumbers(s,ConvertValue(c), d);
        s->acquire();
        s->release();
        delete s;
    }

    char* Core_FtoS::getString(const char* c)
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

    char* Core_FtoS::getFilename()
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

    int Core_FtoS::getSelectedString(const char* c, ArrayOfStrings list,const char* c1, int i)
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
						connection->setBrush(connection->defaultBrush);
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
				if (currentWindow())
					currentWindow()->history.push(command);
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
				if (currentWindow())
					currentWindow()->history.push(command);
			}
		}

		if (s)
			s->release();
	}
	
	void C_API_Slots::_setSize(void* o, double w, double h, int p)
	{
		fToS.setSize(o,w,h,p);
	}

	void Core_FtoS::setSize(void* o,double w, double h,int p)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setSize(s,ConvertValue(o),w,h,p);
		s->acquire();
		s->release();
	}
	
	void C_API_Slots::_setAngle(void* o, double t,int p)
	{
		fToS.setAngle(o,t,p);
	}

	void Core_FtoS::setAngle(void* o,double t,int p)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setAngle(s,ConvertValue(o),t,p);
		s->acquire();
		s->release();
	}

	double C_API_Slots::_getWidth(void* o)
	{
		return fToS.getWidth(o);
	}

	double Core_FtoS::getWidth(void* o)
	{
		double d;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit getWidth(s,ConvertValue(o),&d);
		s->acquire();
		s->release();
		return d;
	}
	
	double C_API_Slots::_getHeight(void* o)
	{
		return fToS.getHeight(o);
	}

	double Core_FtoS::getHeight(void* o)
	{
		double d;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit getHeight(s,ConvertValue(o),&d);
		s->acquire();
		s->release();
		return d;
	}
	
	double C_API_Slots::_getAngle(void* o)
	{
		return fToS.getAngle(o);
	}

	double Core_FtoS::getAngle(void* o)
	{
		double d;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit getAngle(s,ConvertValue(o),&d);
		s->acquire();
		s->release();
		return d;
	}

	int C_API_Slots::_getColorR(void* o)
	{
		return fToS.getColorR(o);
	}

	int Core_FtoS::getColorR(void* o)
	{
		int i;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit getColorR(s,&i,ConvertValue(o));
		s->acquire();
		s->release();
		return i;
	}

	int C_API_Slots::_getColorG(void* o)
	{
		return fToS.getColorG(o);
	}

	int Core_FtoS::getColorG(void* o)
	{
		int i;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit getColorG(s,&i,ConvertValue(o));
		s->acquire();
		s->release();
		return i;
	}

	int C_API_Slots::_getColorB(void* o)
	{
		return fToS.getColorB(o);
	}

	int Core_FtoS::getColorB(void* o)
	{
		int i;
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit getColorB(s,&i,ConvertValue(o));
		s->acquire();
		s->release();
		return i;
	}

	void C_API_Slots::_setColor(void* o,int r, int g, int b, int p)
	{
		return fToS.setColor(o,r,g,b,p);
	}

	void Core_FtoS::setColor(void* o,int r, int g, int b, int p)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setColor(s,ConvertValue(o),r,g,b,p);
		s->acquire();
		s->release();
		return;
	}

	void C_API_Slots::_changeGraphics(void* o,const char* f)
	{
		fToS.changeGraphics(o,f);
	}

	void Core_FtoS::changeGraphics(void* o,const char* f)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit changeGraphics(s,ConvertValue(o),ConvertValue(f));
		s->acquire();
		s->release();
		return;
	}

	void C_API_Slots::_changeArrowHead(void* o,const char* f)
	{
		fToS.changeArrowHead(o,f);
	}

	void Core_FtoS::changeArrowHead(void* o,const char* f)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit changeArrowHead(s,ConvertValue(o),ConvertValue(f));
		s->acquire();
		s->release();
		return;
	}
	
	void C_API_Slots::getColorR(QSemaphore* s,int* r,ItemHandle* item)
	{
		getColorRGB(item,r,0);
		if (s)
			s->release();
	}

	void C_API_Slots::getColorG(QSemaphore* s,int* g,ItemHandle* item)
	{
		getColorRGB(item,g,1);
		if (s)
			s->release();
	}

	void C_API_Slots::getColorB(QSemaphore* s,int* b,ItemHandle* item)
	{
		getColorRGB(item,b,2);
		if (s)
			s->release();
	}

	void C_API_Slots::setColor(QSemaphore* s,ItemHandle* handle,int r,int g,int b, int permanent)
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
					QColor color(r,g,b);
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
										temporarilyColorChanged << aitem;
									}
								}
								QPen newPen(aitem->defaultPen);
								newPen.setColor(QColor(r,g,b));
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
							color.setAlpha(connection->defaultBrush.color().alpha());
							//QBrush newBrush(color);
							if (permanent)
							{
								pens += newPen;
								brushes += connection->brush();
								items += connection;
							}
							else
							{
								connection->setPen(newPen);
								//connection->setBrush(newBrush);
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
	
	void C_API_Slots::getColorRGB(ItemHandle* handle,int* r,int rgb)
	{
		if (!handle || !r) return;
		QGraphicsItem * item;
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
						if (rgb == 0)
							(*r) = node->shapes[0]->defaultBrush.color().red();
						else
							if (rgb == 1)
								(*r) = node->shapes[0]->defaultBrush.color().green();
							else
								(*r) = node->shapes[0]->defaultBrush.color().blue();
					}
				}
				else
				{
					ConnectionGraphicsItem * connection = ConnectionGraphicsItem::cast(item);
					if (connection)
					{
						if (rgb == 0)
							(*r) = connection->defaultBrush.color().red();
						else
							if (rgb == 1)
								(*r) = connection->defaultBrush.color().green();
							else
								(*r) = connection->defaultBrush.color().blue();
					}
					else
					{
						TextGraphicsItem * text = TextGraphicsItem::cast(item);
						if (text)
						{
							if (rgb == 0)
								(*r) = text->defaultTextColor().red();
							else
								if (rgb == 1)
									(*r) = text->defaultTextColor().green();
								else
									(*r) = text->defaultTextColor().blue();
						}
						else
						{
							ControlPoint * cp = ControlPoint::cast(item);
							if (cp)
							{
								if (rgb == 0)
									(*r) = cp->defaultBrush.color().red();
								else
									if (rgb == 1)
										(*r) = cp->defaultBrush.color().green();
									else
										(*r) = cp->defaultBrush.color().blue();
							}
						}
					}
				}
			}
		}
	}
	
}

