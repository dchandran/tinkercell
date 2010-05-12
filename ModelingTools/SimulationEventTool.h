/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This class adds the "events" data to each item in Tinkercell.

****************************************************************************/

#ifndef TINKERCELL_SIMULATIONEVENTSTOOL_H
#define TINKERCELL_SIMULATIONEVENTSTOOL_H

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
#include <QListView>

#include "NetworkHandle.h"
#include "NodeGraphicsItem.h"
#include "DataTable.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "MainWindow.h"
#include "ConsoleWindow.h"

namespace Tinkercell
{

    class SimulationEventsTool_FToS : public QObject
    {
        Q_OBJECT
    signals:
        void getEventTriggers(QSemaphore*,QStringList*);
        void getEventResponses(QSemaphore*,QStringList*);
        void addEvent(QSemaphore*,const QString&, const QString&);
    public slots:
        ArrayOfStrings getEventTriggers();
        ArrayOfStrings getEventResponses();
        void addEvent(const char*, const char*);
    };


    class SimulationEventsTool : public Tool
    {
        Q_OBJECT

    public:
        SimulationEventsTool();
        bool setMainWindow(MainWindow * main);
        QSize sizeHint() const;

    public slots:
        void addEvent();
        void removeEvents();
        void historyUpdate(int);
        void setupFunctionPointers( QLibrary * );
        void sceneClosing(NetworkHandle * , bool * );

	signals:
		void addNewButtons(const QList<QToolButton*>&,const QString& group);
		void itemsInserted(GraphicsScene *, const QList<QGraphicsItem*>&, const QList<ItemHandle*>&);

    private slots:

		void keyPressed(GraphicsScene* scene,QKeyEvent * keyEvent);
		void escapeSignal(const QWidget*);
		void toolLoaded (Tool * tool);
		void mouseDoubleClicked (GraphicsScene * scene, QPointF point, QGraphicsItem *, Qt::MouseButton, Qt::KeyboardModifiers modifiers);
		void insertButtonPressed(const QString&);
        void getEventTriggers(QSemaphore*,QStringList*);
        void getEventResponses(QSemaphore*,QStringList*);
        void addEvent(QSemaphore*,const QString&, const QString&);
		void itemsRemoved(GraphicsScene * scene, const QList<QGraphicsItem*>& item, const QList<ItemHandle*>& handles);
		void sceneClicked(GraphicsScene *scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);

    protected:

        QListWidget eventsListWidget;
        void updateTable();
        void connectTCFunctions();

        QDialog * eventDialog;
        QLineEdit * eventIf, * eventThen;
        void setupDialogs();

        QList<GraphicsScene*> scenePtr;
        QList<DataTable<QString>*> textDataPtr;
        QString oldEvent;
        QGroupBox * groupBox;

        static ArrayOfStrings _getEventTriggers();
        static ArrayOfStrings _getEventResponses();
        static void _addEvent(const char*, const char*);

    protected slots:
        void eventDialogFinished();
        void editEventsList(QListWidgetItem*);
        void visibilityChanged(bool);

    protected:

        void keyPressEvent(QKeyEvent*);

    private:

		enum Mode { none, addingEvent, addingStep, addingPulse, addingWave };
		Mode mode;

        static SimulationEventsTool_FToS fToS;

        void select(int i=0);
        void deselect(int i=0);

        bool openedByUser;
        NodeGraphicsItem item;
        QDockWidget * dockWidget;

		static bool parseRateString(NetworkHandle * win, ItemHandle * handle, QString& s);

    };


}

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif
