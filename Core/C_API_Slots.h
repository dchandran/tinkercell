/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

The two classes defined in this file provide the C API for the TinkerCellCore
library. Each plug-in can provide its own C API using this same procedure. Of course, 
the API provided by this file is probably much larger than individual plug-in APIs. 

****************************************************************************/

#ifndef TINKERCELL_CORECAPI_H
#define TINKERCELL_CORECAPI_H

#include <QObject>
#include <QWidget>
#include <QLibrary>
#include <QSemaphore>
#include <QList>
#include <QString>
#include <QStringList>
#include <QColor>
#include <QGraphicsItem>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include "DataTable.h"
#include "TCstructs.h"

namespace Tinkercell
{
	class ConsoleWindow;
	class NetworkWindow;
	class GraphicsScene;
	class MainWindow;
	class ItemHandle;
	class CThread;
	class NodeGraphicsItem;
	
	typedef void (*MatrixInputFunction)(Matrix);
	
	/*! \brief Function to Signal converter for MainWindow*/
	class MY_EXPORT Core_FtoS : public QObject
	{
		Q_OBJECT

	signals:
		void allItems(QSemaphore*,QList<ItemHandle*>*);
		void selectedItems(QSemaphore*,QList<ItemHandle*>*);
		void itemsOfFamily(QSemaphore*,QList<ItemHandle*>*,const QString&);
		void itemsOfFamily(QSemaphore*,QList<ItemHandle*>*,const QList<ItemHandle*>&,const QString&);
		void find(QSemaphore*,ItemHandle**,const QString&);
		void findItems(QSemaphore*,QList<ItemHandle*>*,const QStringList&);
		void select(QSemaphore*,ItemHandle*);
		void deselect(QSemaphore*);
		void removeItem(QSemaphore*,ItemHandle* );
		void setPos(QSemaphore*,ItemHandle* ,qreal ,qreal );
		void setPos(QSemaphore*,const QList<ItemHandle*>& , DataTable<qreal>&);
		void getPos(QSemaphore*,const QList<ItemHandle*>& , DataTable<qreal>*);
		void getY(QSemaphore*,qreal*,ItemHandle* );
		void getX(QSemaphore*,qreal*,ItemHandle* );
		void moveSelected(QSemaphore*,qreal ,qreal );
		void getFamily(QSemaphore*,QString*,ItemHandle* );
		void getName(QSemaphore*,QString*,ItemHandle* );
		void setName(QSemaphore*,ItemHandle*,const QString&);
		void getNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
		void isA(QSemaphore*,int*,ItemHandle*, const QString&);
		void outputText(QSemaphore*,const QString&);
		void errorReport(QSemaphore*,const QString&);
		void printFile(QSemaphore*,const QString&);
		void clearText(QSemaphore*);
		void outputTable(QSemaphore*,const DataTable<qreal>&);
		void createInputWindow(QSemaphore*,const DataTable<qreal>&, const QString&,const QString&,const QString&);
		void createInputWindow(QSemaphore*,const DataTable<qreal>&, const QString &, MatrixInputFunction);
		void createSliders(QSemaphore*,CThread*, const DataTable<qreal>&, MatrixInputFunction);
		void addInputWindowOptions(QSemaphore*, const QString&, int i, int j, const QStringList&);
		void addInputWindowCheckbox(QSemaphore*, const QString&, int i, int j);
		void openNewWindow(QSemaphore*,const QString&);
		void isWindows(QSemaphore*,int*);
		void isMac(QSemaphore*,int*);
		void isLinux(QSemaphore*,int*);
		void appDir(QSemaphore*,QString*);
		void zoom(QSemaphore*,qreal);

		void getNumericalDataNames(QSemaphore*,QStringList*,ItemHandle*);
		void getTextDataNames(QSemaphore*,QStringList*,ItemHandle*);

		void getNumericalData(QSemaphore*,DataTable<qreal>*,ItemHandle*,const QString&);
		void setNumericalData(QSemaphore*,ItemHandle*,const QString&,const DataTable<qreal>&);
		void getTextData(QSemaphore*,DataTable<QString>*,ItemHandle*,const QString&);
		void setTextData(QSemaphore*,ItemHandle*,const QString&,const DataTable<QString>&);

		void getChildren(QSemaphore*,QList<ItemHandle*>*,ItemHandle*);
		void getParent(QSemaphore*,ItemHandle**,ItemHandle*);
		
        void getString(QSemaphore*,QString*,const QString&);
        void getFilename(QSemaphore*,QString*);
        void getSelectedString(QSemaphore*,int*,const QString&,const QStringList&,const QString&,int);
        void getNumber(QSemaphore*,qreal*,const QString&);
        void getNumbers(QSemaphore*,const QStringList&,qreal*);
		
		void askQuestion(QSemaphore*,const QString&,int*);
		void messageDialog(QSemaphore*,const QString&);
		
		void setSize(QSemaphore*, ItemHandle*,double,double,int);
		void getWidth(QSemaphore*, ItemHandle*, double*);
		void getHeight(QSemaphore*, ItemHandle*,double*);
		void setAngle(QSemaphore*, ItemHandle*,double,int);
		void getAngle(QSemaphore*, ItemHandle*, double*);
		void getColorR(QSemaphore*,int*,ItemHandle*);
		void getColorG(QSemaphore*,int*,ItemHandle*);
		void getColorB(QSemaphore*,int*,ItemHandle*);
		void setColor(QSemaphore*,ItemHandle*,int,int,int,int);
		
		void changeGraphics(QSemaphore*,ItemHandle*,const QString&);
		void changeArrowHead(QSemaphore*,ItemHandle*,const QString&);

	public:
		void zoom(double);
		ArrayOfItems allItems();
		ArrayOfItems itemsOfFamily(const char*);
		ArrayOfItems itemsOfFamily(const char*, ArrayOfItems);
		ArrayOfItems selectedItems();
		void* find(const char*);
		ArrayOfItems findItems(ArrayOfStrings);
		void select(void*);
		void deselect();
		char* getName(void*);
		void setName(void*,const char*);
		ArrayOfStrings getNames(ArrayOfItems);
		char* getFamily(void*);
		int isA(void*,const char*);
		void removeItem(void*);
		void setPos(void*,double ,double );
		void setPos(ArrayOfItems,Matrix);
		Matrix getPos(ArrayOfItems);
		double getY(void*);
		double getX(void*);
		void moveSelected(double ,double );
		void outputTable(Matrix m);
		void outputText(const char*);
		void errorReport(const char*);
		void clearText();
		void printFile(const char*);
		void createInputWindow(Matrix, const char*, const char*,const char*);
		void createInputWindow(Matrix, const char*, MatrixInputFunction);
		void createSliders(void*, Matrix, MatrixInputFunction);
		void addInputWindowOptions(const char*, int i, int j, ArrayOfStrings);
		void addInputWindowCheckbox(const char*, int i, int j);
		void openNewWindow(const char*);
		int isWindows();
		int isMac();
		int isLinux();
		char* appDir();

		ArrayOfStrings getNumericalDataNames(void*);
		ArrayOfStrings getTextDataNames(void*);

		Matrix getNumericalData(void*,const char*);
		void setNumericalData(void*,const char*, Matrix);
		
		TableOfStrings getTextData(void*,const char*);
		void setTextData(void*,const char*,TableOfStrings);

		ArrayOfItems getChildren(void*);
		void* getParent(void*);
		
		char* getString(const char*);
        char* getFilename();
        int getSelectedString(const char*, ArrayOfStrings,const char*,int);
        double getNumber(const char*);
        void getNumbers(ArrayOfStrings, double*);
        
		int askQuestion(const char*);
		void messageDialog(const char*);
		
		void setSize(void*,double,double);
		double getWidth(void*);
		double getHeight(void*);
		void setAngle(void*,double);
		double getAngle(void*);
		int getColorR(void*);
		int getColorG(void*);
		int getColorB(void*);
		void setColor(void*,int,int,int,int);
		
		void changeGraphics(void*,const char*);
		void changeArrowHead(void*,const char*);
	};

	class C_API_Slots : public QObject
	{
		Q_OBJECT
		
		C_API_Slots(MainWindow * );
		
	private:
	
		static Core_FtoS fToS;
	
		MainWindow * mainWindow;
		ConsoleWindow * console() const;
		ConsoleWindow * currentWindow() const;
		ConsoleWindow * currentScene() const;
	
		static void _zoom(double);
		static ArrayOfItems _allItems();
		static ArrayOfItems _itemsOfFamily(const char*);
		static ArrayOfItems _itemsOfFamily2(const char*, ArrayOfItems);
		static ArrayOfItems _selectedItems();
		static void* _find(const char*);
		static ArrayOfItems _findItems(ArrayOfStrings);
		static void _select(void*);
		static void _deselect();
		static char* _getName(void*);
		static void _setName(void*,const char*);
		static ArrayOfStrings _getNames(ArrayOfItems);
		static char* _getFamily(void*);
		static int _isA(void*,const char*);
		static void _removeItem(void*);
		static void _setPos(void*,double ,double );
		static void _setPos2(ArrayOfItems,Matrix);
		static Matrix _getPos(ArrayOfItems);
		static double _getY(void*);
		static double _getX(void*);
		static void _moveSelected(double ,double );
		static void _outputTable(Matrix m);
		static void _outputText(const char*);
		static void _errorReport(const char*);
		static void _clearText();
		static void _printFile(const char*);
		static void _createInputWindow1(Matrix, const char*, const char*,const char*);
		static void _createInputWindow2(Matrix, const char*, MatrixInputFunction);
		static void _createSliders(void*, Matrix, MatrixInputFunction);
		static void _addInputWindowOptions(const char*, int i, int j, ArrayOfStrings);
		static void _addInputWindowCheckbox(const char*, int i, int j);
		static void _openNewWindow(const char*);
		static int _isWindows();
		static int _isMac();
		static int _isLinux();
		static char* _appDir();
		static ArrayOfStrings _getNumericalDataNames(void*);
		static ArrayOfStrings _getTextDataNames(void*);
		static Matrix _getNumericalData(void*,const char*);
		static void _setNumericalData(void*, const char *, Matrix);
		static TableOfStrings _getTextData(void*,const char*);
		static void _setTextData(void*, const char *, TableOfStrings);
		static ArrayOfItems _getChildren(void*);
		static void* _getParent(void*);
		static char* _getString(const char*);
        static int _getSelectedString(const char*, ArrayOfStrings,const char*, int);
        static double _getNumber(const char*);
        static void _getNumbers(ArrayOfStrings, double *);
        static char* _getFilename();
		static void _setSize(void*,double,double,int);
		static double _getWidth(void*);
		static double _getHeight(void*);
		static void _setAngle(void*,double,int);
		static double _getAngle(void*);
		static int _getColorR(void*);
		static int _getColorG(void*);
		static int _getColorB(void*);
		static void _setColor(void*,int,int,int,int);
		static void _changeGraphics(void*,const char*);
		static void _changeArrowHead(void*,const char*);
		static int _askQuestion(const char*);
		static void _messageDialog(const char*);
		/*! 
		* \brief Dialog for selecting strings. 
		*/
        QDialog * getStringDialog;
        /*! 
		* \brief widget for selecting strings. 
		*/
        QListWidget getStringList;
        /*! 
		* \brief number for selecting strings. 
		*/
        int getStringListNumber;
        /*! 
		* \brief list for selecting numbers. 
		*/
        QStringList getStringListText;
        /*! 
		* \brief label for selecting numbers. 
		*/
        QLabel getStringListLabel;
		/*!
		* \brief initializes all the functions in the fToS object
		*/
		void connectTCFunctions();
		
	private slots:
		/*!
		* \brief connect to all the C API functions in TinkerCellCore
		* \param QLibrary* target library
		* \return void
		*/		
		void setupFunctionPointers(QLibrary * library);
		/*!
		* \brief removes any temporary changes
		* \param QWidget * transmitting widget
		* \return void
		*/
		void escapeSlot ( const QWidget * );
		/*!
		* \brief zoom or unzoom. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param factor
		* \return void
		*/
		void zoom(QSemaphore*,qreal);
		/*!
		* \brief gets name of given item. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param return value
		* \param item pointer
		* \return void
		*/
		void itemName(QSemaphore*,QString*,ItemHandle*);
		/*!
		* \brief set name of given item. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param QString new name
		* \param item pointer
		* \return void
		*/
		void setName(QSemaphore*,ItemHandle*,const QString&);
		/*!
		* \brief gets names of given items. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param return value
		* \param item pointers
		* \return void
		*/
		void itemNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
		/*!
		* \brief gets family of given item. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param return value
		* \param item pointer
		* \return void
		*/
		void itemFamily(QSemaphore*,QString*,ItemHandle*);
		/*!
		* \brief Checks whether the given item belongs to the given family. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param return value
		* \param item pointer
		* \param name of family
		* \return void
		*/
		void isA(QSemaphore*,int*,ItemHandle*,const QString& );
		/*!
		* \brief Finds the first graphics item with the name. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param return value
		* \param QString name of item
		* \return void
		*/
		void findItem(QSemaphore*,ItemHandle**,const QString& name);
		/*!
		* \brief Finds all graphics items with the names. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param return value
		* \param QStringList names of items
		* \return void
		*/
		void findItems(QSemaphore*,QList<ItemHandle*>*,const QStringList& name);
		/*!
		* \brief selects the given item. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param item to select value
		* \return void
		*/
		void select(QSemaphore*,ItemHandle* item);
		/*!
		* \brief deselects all selected items. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \return void
		*/
		void deselect(QSemaphore*);
		/*!
		* \brief returns a list of currently selected items. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param return value
		* \return void
		*/
		void selectedItems(QSemaphore*,QList<ItemHandle*>*);
		/*!
		* \brief returns a list of all items. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param return value
		* \return void
		*/
		void allItems(QSemaphore*,QList<ItemHandle*>*);
		/*!
		* \brief returns a list of items of the specified family. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param QList return value
		* \param QString family to filter by
		* \return void
		*/
		void itemsOfFamily(QSemaphore*,QList<ItemHandle*>*,const QString&);
		/*!
		* \brief returns a list of items of the specified family. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param QListreturn value
		* \param QList list of items to search
		* \param QString family to filter by
		* \return void
		*/
		void itemsOfFamily(QSemaphore*,QList<ItemHandle*>*,const QList<ItemHandle*>&,const QString&);
		/*!
		* \brief sets the x,y position of the item. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \param ItemHandle* item pointer
		* \param double  new x position
		* \param double new y position
		* \return void
		*/
		void setPos(QSemaphore*,ItemHandle* item, qreal X, qreal Y);
		/*!
		* \brief sets the x,y position of several items. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \param QList<ItemHandle*> item pointers
		* \param DataTable<qreal> positions (n x 2 matrix)
		* \return void
		*/
		void setPos(QSemaphore*,const QList<ItemHandle*>& items, DataTable<qreal>& pos);
		/*!
		* \brief gets the x position of an item. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param return variable
		* \param item pointer
		* \return void
		*/
		void getX(QSemaphore*,qreal*,ItemHandle* item);
		/*!
		* \brief gets the y position of an item. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param return variable
		* \param item pointer
		* \return void
		*/
		void getY(QSemaphore*,qreal*,ItemHandle* item);
		/*!
		* \brief gets the x and y position of items. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param QList<ItemHandle*> item pointers
		* \param DataTable return variable (n x 2)
		* \return void
		*/
		void getPos(QSemaphore*,const QList<ItemHandle*>& item, DataTable<qreal>* pos);
		/*!
		* \brief removes the given item. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param item pointer
		* \return void
		*/
		void removeItem(QSemaphore*,ItemHandle* item);
		/*!
		* \brief moves all selected items. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param delta x
		* \param delta y
		* \return void
		*/
		void moveSelected(QSemaphore*,qreal x, qreal y);
		/*!
		* \brief show text in output window. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \return void
		*/
		void outputText(QSemaphore*,const QString&);
		/*!
		* \brief clear text in output window. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \return void
		*/
		void clearText(QSemaphore* sem);
		/*!
		* \brief show text in output window as error message. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \return void
		*/
		void errorReport(QSemaphore*,const QString&);
		/*!
		* \brief show text in file in output window. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \return void
		*/
		void printFile(QSemaphore*,const QString&);
		/*!
		* \brief show table in output window. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \return void
		*/
		void outputTable(QSemaphore*,const DataTable<qreal>&);
		/*!
		* \brief make a new input window. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \return void
		*/
		void createInputWindow(QSemaphore*,const DataTable<qreal>&,const QString&,const QString&,const QString&);
		/*!
		* \brief make a new input window. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \return void
		*/
		void createInputWindow(QSemaphore*,const DataTable<qreal>&,const QString&,MatrixInputFunction);
		/*!
		* \brief make a new dialog with sliders. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \return void
		*/
		void createSliders(QSemaphore*, CThread * , const DataTable<qreal>&, MatrixInputFunction);
		/*!
		* \brief change an input window. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \return void
		*/
		void addInputWindowOptions(QSemaphore*,const QString& name, int i, int j, const QStringList&);
		/*!
		* \brief change an input window. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \return void
		*/
		void addInputWindowCheckbox(QSemaphore*,const QString& name, int i, int j);
		/*!
		* \brief opens a new window. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param return value
		* \param item pointer
		* \return void
		*/
		void openNewWindow(QSemaphore*,const QString&);
		/*!
		* \brief returns 1 if current OS is Windows. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \param int* Boolean (1 or 0)
		* \return void
		*/
		void isWindows(QSemaphore*,int*);
		/*!
		* \brief returns 1 if current OS is Mac. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \param int*  Boolean (1 or 0)
		* \return void
		*/
		void isMac(QSemaphore*,int*);
		/*!
		* \brief returns 1 if current OS is Linux. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \param int*  Boolean (1 or 0)
		* \return void
		*/
		void isLinux(QSemaphore*,int*);
		/*!
		* \brief returns the application directory. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \param QString*  place to store the directory
		* \return void
		*/
		void appDir(QSemaphore*,QString*);
		/*!
		* \brief returns the name of all data entries for given item. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \param QStringList*  return value
		* \param ItemHandle* item to change
		* \return void
		*/
		void getNumericalDataNames(QSemaphore*,QStringList*,ItemHandle*);
		/*!
		* \brief returns the name of all data entries for given item. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \param QStringList*  return value
		* \param ItemHandle* item to change
		* \return void
		*/
		void getTextDataNames(QSemaphore*,QStringList*,ItemHandle*);
		/*!
		* \brief returns the data matrix. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \param DataTable* return value
		* \param ItemHandle* item
		* \param QString tool name
		* \return void
		*/
		void getNumericalData(QSemaphore*,DataTable<qreal>*,ItemHandle*,const QString&);
		/*!
		* \brief sets a data matrix for an item. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \param ItemHandle* item
		* \param QString tool name
		* \param DataTable new table to insert
		* \return void
		*/
		void setNumericalData(QSemaphore* sem,ItemHandle* item,const QString& tool, const DataTable<qreal>& dat);
		/*!
		* \brief returns the data matrix. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \param DataTable* return value
		* \param ItemHandle* item
		* \param QString tool name
		* \return void
		*/
		void getTextData(QSemaphore*,DataTable<QString>*,ItemHandle*,const QString&);
		/*!
		* \brief sets a data matrix for an item. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \param ItemHandle* item
		* \param QString tool name
		* \param DataTable new table to insert
		* \return void
		*/
		void setTextData(QSemaphore* sem,ItemHandle* item,const QString& tool, const DataTable<QString>& dat);

		/*!
		* \brief get children of an item. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \param QList<ItemHandle*>* return value
		* \param ItemHandle* item
		* \return void
		*/
		void getChildren(QSemaphore*,QList<ItemHandle*>*,ItemHandle*);
		/*!
		* \brief get parent of an item. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \param ItemHandle** return value
		* \param ItemHandle* item
		* \return void
		*/
		void getParent(QSemaphore*,ItemHandle**,ItemHandle*);
		/*!
		* \brief loads files (library files or model files)
		* \param QList<QFileInfo>& the name(s) of the file(s)
		* \return void
		*/
		void dragAndDropFiles(const QList<QFileInfo>& files);
		/*!
		* \brief change console background color
		* \return void
		*/
		void changeConsoleBgColor();
		/*!
		* \brief change console text color
		* \return void
		*/
		void changeConsoleTextColor();
		/*!
		* \brief change console message text color
		* \return void
		*/
		void changeConsoleMsgColor();
		/*!
		* \brief change console error text color
		* \return void
		*/
		void changeConsoleErrorMsgColor();
		/*! \brief Stores the index that the user selected from a list of strings
		*	\sa getSelectedString
		*/
        void getStringListItemSelected(QListWidgetItem *);
        /*! \brief Stores the index that the user selected from a list of strings
		*	\sa getSelectedString
		*/
        void getStringListRowChanged(int);
        /*! \brief Negates the index that the user selected from a list of strings
		*	\sa getSelectedString
		*/
		void getStringListCanceled();
        /*! \brief Searches the list of strings displayed to user
		*	\sa getSelectedString
		*/
        void getStringSearchTextEdited(const QString & text);
		/*!
        * \brief Get string from user. Part of the TinkerCell C interface.
        */
        void getString(QSemaphore*,QString*,const QString&);
        /*!
        * \brief Get string from user from a list. Part of the TinkerCell C interface.
        */
        void getSelectedString(QSemaphore*, int*, const QString&, const QStringList&, const QString&, int);
        /*!
        * \brief Get a number from user. Part of the TinkerCell C interface.
        */
        void getNumber(QSemaphore*,qreal*,const QString&);
        /*!
        * \brief Get more than one number from user. Part of the TinkerCell C interface.
        */
        void getNumbers(QSemaphore*,const QStringList&,qreal*);
        /*!
        * \brief Get file name from user. Part of the TinkerCell C interface.
        */
        void getFilename(QSemaphore*,QString*);
		void askQuestion(QSemaphore*, const QString&, int *);
		void messageDialog(QSemaphore*, const QString&);
        void setSize(QSemaphore*, ItemHandle*,double,double,int);
		void getWidth(QSemaphore*, ItemHandle*, double*);
		void getHeight(QSemaphore*, ItemHandle*,double*);
		void setAngle(QSemaphore*, ItemHandle*,double,int);
		void getAngle(QSemaphore*, ItemHandle*, double*);
		void getColorR(QSemaphore*,int*,ItemHandle*);
		void getColorG(QSemaphore*,int*,ItemHandle*);
		void getColorB(QSemaphore*,int*,ItemHandle*);
		void setColor(QSemaphore*,ItemHandle*,int,int,int,int);
		void changeGraphics(QSemaphore*,ItemHandle*,const QString&);
		void changeArrowHead(QSemaphore*,ItemHandle*,const QString&);
	
	private:
		QList<QGraphicsItem*> temporarilyColorChanged;
		QList< QPair<NodeGraphicsItem*,QPointF> > temporarilyChangedSize; 
		QList< QPair<NodeGraphicsItem*,double> > temporarilyChangedAngle;
	};
}
#endif

