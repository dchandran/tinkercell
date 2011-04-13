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
#include "TC_structs.h"

namespace Tinkercell
{
	class ConsoleWindow;
	class NetworkHandle;
	class GraphicsScene;
	class MainWindow;
	class ItemHandle;
	class CThread;
	class NodeGraphicsItem;
	
	typedef void (*MatrixInputFunction)(tc_matrix);
	
	/*! \brief Function to Signal converter for MainWindow*/
	class TINKERCELLEXPORT Core_FtoS : public QObject
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
		void getUniqueName(QSemaphore*,QString*,ItemHandle* );
		void setName(QSemaphore*,ItemHandle*,const QString&);
		void getNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
		void getUniqueNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
		void isA(QSemaphore*,int*,ItemHandle*, const QString&);
		void outputText(QSemaphore*,const QString&);
		void errorReport(QSemaphore*,const QString&);
		void printFile(QSemaphore*,const QString&);
		void clearText(QSemaphore*);
		void outputTable(QSemaphore*,const DataTable<qreal>&);
		void createInputWindow(QSemaphore*,const DataTable<qreal>&, const QString&,const QString&);
		void createInputWindow(QSemaphore*,long, const DataTable<qreal>&, const QString &, MatrixInputFunction);
		void createSliders(QSemaphore*,CThread*, const DataTable<qreal>&, MatrixInputFunction);
		void addInputWindowOptions(QSemaphore*, const QString&, int i, int j, const QStringList&);
		void addInputWindowCheckbox(QSemaphore*, const QString&, int i, int j);
		void openNewWindow(QSemaphore*,const QString&);
		void isWindows(QSemaphore*,int*);
		void isMac(QSemaphore*,int*);
		void isLinux(QSemaphore*,int*);
		void appDir(QSemaphore*,QString*);
		void homeDir(QSemaphore*,QString*);
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
        void getSelectedString(QSemaphore*,int*,const QString&,const QStringList&,const QString&);
        void getNumber(QSemaphore*,qreal*,const QString&);
        void getNumbers(QSemaphore*,const QStringList&,qreal*);
		
		void askQuestion(QSemaphore*,const QString&,int*);
		void messageDialog(QSemaphore*,const QString&);
		void openFile(QSemaphore*,const QString&);
		void saveToFile(QSemaphore*,const QString&);
		
		void setSize(QSemaphore*, ItemHandle*,double,double,int);
		void getWidth(QSemaphore*, ItemHandle*, double*);
		void getHeight(QSemaphore*, ItemHandle*,double*);
		void setAngle(QSemaphore*, ItemHandle*,double,int);
		void getColor(QSemaphore*,QString*,ItemHandle*);
		void setColor(QSemaphore*,ItemHandle*,const QString&,int);
		
		void changeGraphics(QSemaphore*,ItemHandle*,const QString&);
		void changeArrowHead(QSemaphore*,ItemHandle*,const QString&);
		
		void screenshot(QSemaphore*, const QString&, int, int);
		void screenHeight(QSemaphore*, int*);
		void screenWidth(QSemaphore*, int*);
		void screenX(QSemaphore*, int*);
		void screenY(QSemaphore*, int*);
		
		void annotations(QSemaphore*, QString*);
		void insertAnnotation(QSemaphore*, const QString&, double, double);
		
		void setNumericalValues(QSemaphore*, const NumericalDataTable&);
		void setNumericalValue(QSemaphore*, const QString&, double);
		void setTextValues(QSemaphore*, const TextDataTable&);
		void setTextValue(QSemaphore*, const QString&, const QString&);
		
		void getNumericalValue(QSemaphore*, const QString&, double*);
		void getTextValue(QSemaphore*, const QString&, QString*);

	public:
		void zoom(double);
		tc_items allItems();
		tc_items itemsOfFamily(const char*);
		tc_items itemsOfFamily(const char*, tc_items);
		tc_items selectedItems();
		long find(const char*);
		tc_items findItems(tc_strings);
		void select(long);
		void deselect();
		const char* getName(long);
		const char* getUniqueName(long);
		void setName(long,const char*);
		tc_strings getNames(tc_items);
		tc_strings getUniqueNames(tc_items);
		const char* getFamily(long);
		int isA(long,const char*);
		void removeItem(long);
		void setPos(long,double ,double );

		void setPos(tc_items,tc_matrix);
		tc_matrix getPos(tc_items);

		double getY(long);
		double getX(long);
		void moveSelected(double ,double );
		void outputTable(tc_matrix m);
		void outputText(const char*);
		void errorReport(const char*);
		void clearText();
		void printFile(const char*);
		void createInputWindow(tc_matrix, const char*, const char*);
		void createInputWindow(long, tc_matrix, const char*, MatrixInputFunction);
		void createSliders(long, tc_matrix, MatrixInputFunction);
		void addInputWindowOptions(const char*, int i, int j, tc_strings);
		void addInputWindowCheckbox(const char*, int i, int j);
		void openNewWindow(const char*);
		int isWindows();
		int isMac();
		int isLinux();
		const char* appDir();
		const char* homeDir();

		tc_strings getNumericalDataNames(long);
		tc_strings getTextDataNames(long);

		tc_matrix getNumericalData(long,const char*);
		void setNumericalData(long,const char*, tc_matrix);
		
		tc_table getTextData(long,const char*);
		void setTextData(long,const char*,tc_table);

		tc_items getChildren(long);
		long getParent(long);
		
		const char* getString(const char*);
        const char* getFilename();
        int getSelectedString(const char*, tc_strings,const char*);
        double getNumber(const char*);
        void getNumbers(tc_strings, double*);
        
		int askQuestion(const char*);
		void messageDialog(const char*);
		void openFile(const char*);
		void saveToFile(const char*);
		
		void setSize(long,double,double,int);
		double getWidth(long);
		double getHeight(long);
		void setAngle(long,double,int);
		const char* getColor(long);
		void setColor(long,const char*,int);
		
		void changeGraphics(long,const char*);
		void changeArrowHead(long,const char*);
		
		void screenshot(const char*, int, int);
		int screenHeight();
		int screenWidth();
		int screenX();
		int screenY();
		
		const char * annotation();
		void insertAnnotation(const char *, double, double);
		
		void setNumericalValues(tc_matrix);
		void setNumericalValue(const char *, double);
		void setTextValues(tc_table);
		void setTextValue(const char *, const char *);
		
		double getNumericalValue(const char*);
		const char* getTextValue(const char*);
	};

	/*! \brief A set of slots that are called by C libraries
	\ingroup CAPI
	*/

	class C_API_Slots : public QObject
	{
		Q_OBJECT
	
	public:	
		C_API_Slots(MainWindow * );
	signals:
		void saveNetwork(const QString&);
	private:
	
		static Core_FtoS fToS;
	
		MainWindow * mainWindow;
		ConsoleWindow * console() const;
		NetworkHandle * currentNetwork() const;
		GraphicsScene * currentScene() const;
	
		static void _zoom(double);
		static tc_items _allItems();
		static tc_items _itemsOfFamily(const char*);
		static tc_items _itemsOfFamily2(const char*, tc_items);
		static tc_items _selectedItems();
		static long _find(const char*);
		static tc_items _findItems(tc_strings);
		static void _select(long);
		static void _deselect();
		static const char* _getName(long);
		static const char* _getUniqueName(long);
		static void _setName(long,const char*);
		static tc_strings _getNames(tc_items);
		static tc_strings _getUniqueNames(tc_items);
		static const char* _getFamily(long);
		static int _isA(long,const char*);
		static void _removeItem(long);
		static void _setPos(long,double ,double );

		static void _setPos2(tc_items,tc_matrix);
		static tc_matrix _getPos(tc_items);

		static double _getY(long);
		static double _getX(long);
		static void _moveSelected(double ,double );
		static void _outputTable(tc_matrix m);
		static void _outputText(const char*);
		static void _errorReport(const char*);
		static void _clearText();
		static void _printFile(const char*);

		static void _createInputWindow1(tc_matrix, const char*, const char*);
		static void _createInputWindow2(long, tc_matrix, const char*, MatrixInputFunction);
		static void _createSliders(long, tc_matrix, MatrixInputFunction);
		static void _addInputWindowOptions(const char*, int i, int j, tc_strings);
		static void _addInputWindowCheckbox(const char*, int i, int j);
		static void _openNewWindow(const char*);
		static int _isWindows();
		static int _isMac();
		static int _isLinux();
		static const char* _appDir();
		static const char* _homeDir();

		static tc_strings _getNumericalDataNames(long);
		static tc_strings _getTextDataNames(long);
		static tc_matrix _getNumericalData(long,const char*);
		static void _setNumericalData(long, const char *, tc_matrix);
		static tc_table _getTextData(long,const char*);
		static void _setTextData(long, const char *, tc_table);
		static tc_items _getChildren(long);

		static long _getParent(long);

		static const char* _getString(const char*);
        static int _getSelectedString(const char*, tc_strings,const char*);
        static double _getNumber(const char*);
        static void _getNumbers(tc_strings, double *);
        static const char* _getFilename();
		static void _setSize(long,double,double,int);
		static double _getWidth(long);
		static double _getHeight(long);
		static void _setAngle(long,double,int);
		static const char* _getColor(long);
		static void _setColor(long,const char *,int);
		static void _changeGraphics(long,const char*);
		static void _changeArrowHead(long,const char*);
		static int _askQuestion(const char*);
		static void _messageDialog(const char*);
		static void _openFile(const char *);
		static void _saveToFile(const char *);
		
		static void _screenshot(const char*, int, int);
		static int _screenHeight();
		static int _screenWidth();
		static int _screenX();
		static int _screenY();
		
		static const char * _annotations();
		static void _insertAnnotation(const char *, double, double);
		
		static void _setNumericalValues(tc_matrix);
		static void _setNumericalValue(const char *, double);
		static void _setTextValues(tc_table);
		static void _setTextValue(const char *, const char *);
		
		static double _getNumericalValue(const char*);
		static const char* _getTextValue(const char*);
		
		static void _openUrl(const char*);
		
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
		* \brief gets name of given item. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param return value
		* \param item pointer
		* \return void
		*/
		void uniqueName(QSemaphore*,QString*,ItemHandle*);
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
		* \brief gets names of given items. This function is designed to be used with the C API framework
		* \param QSemaphore * semaphore
		* \param return value
		* \param item pointers
		* \return void
		*/
		void uniqueNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
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
		void createInputWindow(QSemaphore*,const DataTable<qreal>&,const QString&,const QString&);
		/*!
		* \brief make a new input window. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \return void
		*/
		void createInputWindow(QSemaphore*,long, const DataTable<qreal>&,const QString&,MatrixInputFunction);
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
		* \brief returns the tinkercell home directory. This function is designed to be used with the C API framework
		* \param QSemaphore* semaphore
		* \param QString*  place to store the directory
		* \return void
		*/
		void homeDir(QSemaphore*,QString*);
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
        void getSelectedString(QSemaphore*, int*, const QString&, const QStringList&, const QString&);
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
		void openFile(QSemaphore*,const QString&);
		void saveToFile(QSemaphore*,const QString&);
        void setSize(QSemaphore*, ItemHandle*,double,double,int);
		void getWidth(QSemaphore*, ItemHandle*, double*);
		void getHeight(QSemaphore*, ItemHandle*,double*);
		void setAngle(QSemaphore*, ItemHandle*,double,int);
		void getColor(QSemaphore*,QString*,ItemHandle*);
		void setColor(QSemaphore*,ItemHandle*,const QString&,int);
		void changeGraphics(QSemaphore*,ItemHandle*,const QString&);
		void changeArrowHead(QSemaphore*,ItemHandle*,const QString&);
		void screenshot(QSemaphore*, const QString &, int, int);
		void screenHeight(QSemaphore*, int*);
		void screenWidth(QSemaphore*, int*);
		void screenX(QSemaphore*, int*);
		void screenY(QSemaphore*, int*);
		void annotations(QSemaphore*, QString*);
		void insertAnnotation(QSemaphore*, const QString&, double, double);
		void setNumericalValues(QSemaphore*, const NumericalDataTable&);
		void setNumericalValue(QSemaphore*, const QString&, double);
		void setTextValues(QSemaphore*, const TextDataTable&);
		void setTextValue(QSemaphore*, const QString&, const QString&);
		void getNumericalValue(QSemaphore*, const QString&, double*);
		void getTextValue(QSemaphore*, const QString&, QString*);
	private:
		QList<QGraphicsItem*> temporarilyColorChanged;
		QList< QPair<NodeGraphicsItem*,QPointF> > temporarilyChangedSize; 
		QList< QPair<NodeGraphicsItem*,double> > temporarilyChangedAngle;
	};
}
#endif

