/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool displays a dialog with the name and family information of selected items.
 An associated GraphicsTool is also defined.

****************************************************************************/

#ifndef TINKERCELL_NAMEFAMILYINFORMATIONTOOL_H
#define TINKERCELL_NAMEFAMILYINFORMATIONTOOL_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
#include <QDialog>

#include "Core/NodeGraphicsItem.h"
#include "Core/ItemHandle.h"
#include "Core/Tool.h"
#include "NodesTree/NodesTree.h"

namespace Tinkercell
{
    class NameFamilyDialog_FtoS : public QObject
    {
        Q_OBJECT;
    signals:
        void getAnnotation(QSemaphore* sem, QStringList* list, ItemHandle* item);
        void setAnnotation(QSemaphore* sem, ItemHandle* item, const QStringList& list);
    public slots:
        char** getAnnotation(OBJ);
        void setAnnotation(OBJ,char**);
    };

    class NameFamilyDialog : public Tool
    {
        Q_OBJECT;

    public:
        NameFamilyDialog();
        bool setMainWindow(MainWindow * main);
    signals:
        void itemsAboutToBeInserted(GraphicsScene * scene, QList<QGraphicsItem*>& items, QList<ItemHandle*>& handles);
        void itemsInserted(GraphicsScene * scene, const QList<QGraphicsItem*>& items, const QList<ItemHandle*>& handles);
        void familyChanged(const QList<ItemHandle*>& items);
        void nameChanged();

    public slots:
        void select(int);
        void deselect(int);
        void showDialog(const QList<ItemHandle*>&);
        void closeDialog();
        void itemsInsertedSlot(NetworkWindow * , const QList<ItemHandle*>& handles);
        void dialogFinished();
        void setupFunctionPointers( QLibrary * );

    protected:
        QDialog * dialog;
        QList<QLineEdit*> lineEdits;
        QList<ItemHandle*> selectedItems;
        void makeDialog(QWidget*);
        NodeGraphicsItem idcard;

    private slots:
        /*!
        * \brief get the annotation for an item. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param QStringList* output
        * \param ItemHandle* item
        * \return void
        */
        void getAnnotation(QSemaphore* sem, QStringList* list, ItemHandle* item);
        /*!
        * \brief set the annotation for an item. This function is designed to be used with the C API framework
        * \param QSemaphore* semaphore
        * \param ItemHandle* item
        * \param QStringList output
        * \return void
        */
        void setAnnotation(QSemaphore* sem, ItemHandle* item, const QStringList& list);
    private:
        void connectTCFunctions();
        /*! \brief node of the C API framework*/
        static char** _getAnnotation(OBJ);
        /*! \brief node of the C API framework*/
        static void _setAnnotation(OBJ,char**);
        /*! \brief node of the C API framework*/
        static NameFamilyDialog_FtoS fToS;
    };


}

#endif
