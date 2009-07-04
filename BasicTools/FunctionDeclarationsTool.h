/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This class adds the "attributes" data to each item in Tinkercell.
 Two types of attributes are added -- "Numerical Attributes" and "Text Attributes".
 Attributes are essentially a <name,value> pair that are used to characterize an item.
 
 The AssignmentFunctionsTool also comes with two GraphicalTools, one for text attributes and one
 for numerical attributes. The buttons are drawn as NodeGraphicsItems using the datasheet.xml and
 textsheet.xml files that define the NodeGraphicsItems.
 
****************************************************************************/

#ifndef TINKERCELL_ASSIGNMENTSANDFUNCTIONSTOOL_H
#define TINKERCELL_ASSIGNMENTSANDFUNCTIONSTOOL_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHash>
#include <QGroupBox>
#include <QUndoCommand>
#include <QGraphicsRectItem>
#include <QGraphicsWidget>
#include <QSlider>
#include <QToolButton>
#include <QButtonGroup>
#include <QTableView>

#include "NodeGraphicsItem.h"
#include "DataTable.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "NetworkWindow.h"
#include "MainWindow.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{
    /*!
     \brief This class provides the C API for the ConnectionInsertion class
     \ingroup capi
     */
    class AssignmentFunctionsTool_FToS : public QObject
    {
        Q_OBJECT
                signals:
        void getForcingFunctionNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
        void getForcingFunctionAssignments(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
        void addForcingFunction(QSemaphore*,ItemHandle*,const QString&, const QString&);
                public slots:
        char** getForcingFunctionNames(Array);
        char** getForcingFunctionAssignments(Array);
        void addForcingFunction(OBJ,const char*, const char*);
    };

    /*!
     \brief Inserts the "Functions" and "Assignments" data into all items and provides
     the widgets for viewing and editing each. The same table widget is used for both;
     the text is parsed to determine whether it is an assignment or function.
     \ingroup plugins
     */
    class MY_EXPORT AssignmentFunctionsTool : public Tool
    {
        Q_OBJECT

    public:
        QList<ItemHandle*> itemHandles;
        AssignmentFunctionsTool();
        bool setMainWindow(MainWindow * main);
        QSize sizeHint() const;

    signals:


    public slots:
        void select(int);
        void deselect(int);

        void setValue(int i, int j);
        void itemsInserted(NetworkWindow * scene, const QList<ItemHandle*>& handles);
        void itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers);
        void toolLoaded(Tool*);
        void addAttribute();
        void removeSelectedAttributes();
        void historyUpdate(int);
        void setupFunctionPointers( QLibrary * );

        void aboutToDisplayModel(const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations);
        void displayModel(QTabWidget& widgets, const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equations);

    private slots:
        void getForcingFunctionNames(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
        void getForcingFunctionAssignments(QSemaphore*,QStringList*,const QList<ItemHandle*>&);
        void addForcingFunction(QSemaphore*,ItemHandle*,const QString&, const QString&);

    protected:
        void insertDataMatrix(ItemHandle * handle);
        QTableWidget tableWidget;
        QList<ItemHandle*> tableItems;

        void updateTable();

        static AssignmentFunctionsTool_FToS fToS;
        void connectTCFunctions();

        virtual void keyPressEvent ( QKeyEvent * event );

    private:

        QStringList updatedFunctions, updatedFunctionNames;

        bool openedByUser;
        NodeGraphicsItem item;
        QDockWidget * dockWidget;

        static char** _getForcingFunctionNames(Array);
        static char** _getForcingFunctionAssignments(Array);
        static void _addForcingFunction(OBJ,const char*, const char*);
    };


}

#endif
