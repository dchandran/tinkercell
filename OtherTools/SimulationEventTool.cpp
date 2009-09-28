/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This class adds the "attributes" data to each item in Tinkercell.
 Two types of attributes are added -- "Numerical Attributes" and "Text Attributes".
 Attributes are essentially a <name,value> pair that are used to characterize an item.

 The SimulationEventsTool also comes with two GraphicalTools, one for text attributes and one
 for numerical attributes. The buttons are drawn as NodeGraphicsItems using the datasheet.xml and
 textsheet.xml files that define the NodeGraphicsItems.

****************************************************************************/

#include "NetworkWindow.h"
#include "SymbolsTable.h"
#include "GraphicsScene.h"
#include "EquationParser.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "SimulationEventTool.h"

#include "muParserDef.h"
#include "muParser.h"
#include "muParserInt.h"
#include <QGroupBox>

namespace Tinkercell
{

    void SimulationEventsTool::select(int)
    {
        NetworkWindow * net = currentWindow();
        if (!net) return;

        if (dockWidget && dockWidget->widget() != this)
            dockWidget->setWidget(this);

        itemHandles = net->selectedHandles();

        if (itemHandles.size() < 1) return;
        openedByUser = true;

        updateTable();
        if (parentWidget() != 0)
        {
            if (parentWidget()->isVisible())
                openedByUser = false;
            else
                parentWidget()->show();
        }
        else
        {
            if (isVisible())
                openedByUser = false;
            else
                show();
        }
    }

    void SimulationEventsTool::deselect(int)
    {
        if (openedByUser && (!dockWidget || dockWidget->isFloating()))
        {
            openedByUser = false;
            if (parentWidget() != 0)
                parentWidget()->hide();
            else
                hide();
        }
    }

    void SimulationEventsTool::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& list, QPointF , Qt::KeyboardModifiers )
    {
        if (scene && (isVisible() || (parentWidget() && parentWidget() != mainWindow && parentWidget()->isVisible())))
        {
            itemHandles.clear();
            ItemHandle * handle = 0;
            for (int i=0; i < list.size(); ++i)
            {
                if ((handle = getHandle(list[i])))
                    itemHandles += handle;
            }

            updateTable();
        }
    }

    bool SimulationEventsTool::setMainWindow(MainWindow * main)
    {
        Tool::setMainWindow(main);

        if (mainWindow)
        {
            eventDialog = new QDialog(mainWindow);
            setupDialogs();

            connect(mainWindow,SIGNAL(windowClosing(NetworkWindow * , bool *)),this,SLOT(sceneClosing(NetworkWindow * , bool *)));

            connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
                    this,SLOT(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));

            connect(mainWindow,SIGNAL(itemsInserted(NetworkWindow*, const QList<ItemHandle*>&)),
                    this, SLOT(itemsInserted(NetworkWindow*, const QList<ItemHandle*>&)));

            connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

            connect(mainWindow,SIGNAL(historyChanged(int)),this,SLOT(historyUpdate(int)));
			
			setWindowTitle(name);
            dockWidget = mainWindow->addToolWindow(this,MainWindow::DockWidget, Qt::BottomDockWidgetArea,Qt::NoDockWidgetArea);

            if (dockWidget)
            {
                dockWidget->move(mainWindow->geometry().bottomRight() - QPoint(sizeHint().width()*2,sizeHint().height()*2));

                connect(dockWidget,SIGNAL(visibilityChanged(bool)),this,SLOT(visibilityChanged(bool)));

                dockWidget->setWindowFlags(Qt::Tool);
                dockWidget->setAttribute(Qt::WA_ContentsPropagated);
                dockWidget->setPalette(QPalette(QColor(255,255,255,255)));
                dockWidget->setAutoFillBackground(true);
                //dockWidget->setWindowOpacity(0.9);

                QCoreApplication::setOrganizationName("TinkerCell");
                QCoreApplication::setOrganizationDomain("www.tinkercell.com");
                QCoreApplication::setApplicationName("TinkerCell");

                QSettings settings("TinkerCell", "TinkerCell");

                settings.beginGroup("SimulationEventsTool");
                //dockWidget->resize(settings.value("size", sizeHint()).toSize());
                //dockWidget->move(settings.value("pos", dockWidget->pos()).toPoint());

                if (settings.value("floating", true).toBool())
                    dockWidget->setFloating(true);

                settings.endGroup();
                dockWidget->hide();
            }
        }
        return (mainWindow != 0);
    }

    void SimulationEventsTool::sceneClosing(NetworkWindow * , bool *)
    {
        QCoreApplication::setOrganizationName("TinkerCell");
        QCoreApplication::setOrganizationDomain("www.tinkercell.com");
        QCoreApplication::setApplicationName("TinkerCell");

        QSettings settings("TinkerCell", "TinkerCell");

        if (dockWidget)
        {
            settings.beginGroup("SimulationEventsTool");
            //settings.setValue("floating", dockWidget && dockWidget->isFloating());
            settings.setValue("size", dockWidget->size());
            settings.setValue("pos", dockWidget->pos());
            settings.endGroup();
        }
    }

    void SimulationEventsTool::historyUpdate(int)
    {
        if (isVisible() || (parentWidget() && parentWidget() != mainWindow && parentWidget()->isVisible()))
            updateTable();
    }

    void SimulationEventsTool::itemsInserted(NetworkWindow* ,  const QList<ItemHandle*>& handles)
    {
        for (int i=0; i < handles.size(); ++i)
        {
            if (handles[i] && handles[i]->family() && !handles[i]->tools.contains(this))
                handles[i]->tools += this;

            if (handles[i] && handles[i]->family() && handles[i]->data &&
                !(handles[i]->hasTextData(tr("Events"))))
            {
                insertData(handles[i]);
            }
        }
    }

    SimulationEventsTool::SimulationEventsTool() : Tool(tr("Events"))
    {
        QString appDir = QCoreApplication::applicationDirPath();
        openedByUser = false;
        NodeGraphicsReader reader;
        reader.readXml(&item,appDir + tr("/OtherItems/clock.xml"));

        item.normalize();
        item.scale(35.0/item.sceneBoundingRect().width(),35.0/item.sceneBoundingRect().height());

        graphicsItems += new GraphicsItem(this);
        graphicsItems[0]->setToolTip(tr("Events"));
        graphicsItems[0]->addToGroup(&item);


        QFont font = this->font();
        font.setPointSize(12);
        eventsListWidget.setFont(font);

        QHBoxLayout * actionsLayout = new QHBoxLayout;

        QToolButton * addEventAction = new QToolButton(this);
        addEventAction->setIcon(QIcon(":/images/plus.png"));
        addEventAction->setToolTip(tr("Add new event"));
        connect(addEventAction,SIGNAL(pressed()),this,SLOT(addEvent()));

        QToolButton * removeEventAction = new QToolButton(this);
        removeEventAction->setIcon(QIcon(":/images/minus.png"));
        removeEventAction->setToolTip(tr("Remove selected event(s)"));
        connect(removeEventAction,SIGNAL(pressed()),this,SLOT(removeEvents()));

        QToolButton * question = new QToolButton(this);
        question->setIcon(QIcon(":/images/question.png"));

        QString message = tr("This table can be used to declare events. Events are a set of triggers and responses to those triggers. An example of an event is IF A < 0.1 THEN A = 10. In this event, whenever A goes below 0.1, the value of A is set to 10. Another example is IF time > 50 THEN A = 20; B = 10; where two values are set at time 50.");
        QMessageBox * messageBox = new QMessageBox(QMessageBox::Information,tr("About Forcing Functions"),message,QMessageBox::StandardButtons(QMessageBox::Close), const_cast<QWidget*>((QWidget*)this), Qt::WindowFlags (Qt::Dialog));
        connect(question,SIGNAL(pressed()),messageBox,SLOT(exec()));

        actionsLayout->addWidget(addEventAction);
        actionsLayout->addWidget(removeEventAction);
        actionsLayout->addStretch(1);
        actionsLayout->addWidget(question);

        groupBox = new QGroupBox(tr(" Events "),this);

        QVBoxLayout * eventBoxLayout = new QVBoxLayout;
        eventBoxLayout->addWidget(&eventsListWidget,1);

        //QWidget * widget = new QWidget(this);
        //widget->setLayout(actionsLayout);

        eventBoxLayout->addLayout(actionsLayout);
        groupBox->setLayout(eventBoxLayout);

        eventDialog = 0;
        dockWidget = 0;

        QVBoxLayout * layout = new QVBoxLayout;
        layout->addWidget(groupBox);
        setLayout(layout);

        connectTCFunctions();
    }

    void SimulationEventsTool::visibilityChanged(bool b)
    {
        if (b && dockWidget && groupBox)
        {
            dockWidget->setWidget(groupBox);
        }
    }

    QSize SimulationEventsTool::sizeHint() const
    {
        return QSize(400, 200);
    }

    void SimulationEventsTool::insertData(ItemHandle * handle)
    {
        if (handle == 0) return;

        DataTable<QString> events;
        events.resize(0,1);
        events.colName(0) = tr("event");
		
		events.description() = tr("Events: set of triggers and events. The row names are the triggers, and the first column contains a string describing one or more events, usually an assignment.");

        handle->data->textData.insert(tr("Events"),events);
    }

    void SimulationEventsTool::updateTable()
    {
        eventsListWidget.clear();

        QStringList ifthens;

        DataTable<QString> * sDataTable = 0;

        for (int i=0; i < itemHandles.size(); ++i) //build combined matrix for all selected reactions
        {
            if (itemHandles[i] != 0 && itemHandles[i]->data != 0)
            {
                if (itemHandles[i]->hasTextData(tr("Events")))
                {
                    sDataTable = &(itemHandles[i]->data->textData[tr("Events")]);
                    for (int j=0; j < sDataTable->rows(); ++j)
                    {
                        ifthens << (tr("IF   ") + sDataTable->rowName(j) + tr("   THEN   ") + sDataTable->value(j,0));
                    }
                }
            }
        }

        eventsListWidget.addItems(ifthens);

        eventsListWidget.setVisible((eventsListWidget.count() > 0));
    }

    void SimulationEventsTool::setupDialogs()
    {
        if (!eventDialog) return;

        //event dialog
        eventDialog->setSizeGripEnabled(true);

        QGridLayout * layout = new QGridLayout;
        QPushButton * okButton = new QPushButton("OK");
        connect(okButton,SIGNAL(released()),eventDialog,SLOT(accept()));
        QPushButton * cancelButton = new QPushButton("Cancel");
        connect(cancelButton,SIGNAL(released()),eventDialog,SLOT(reject()));
        QLabel * label1 = new QLabel(tr("If :"));
        QLabel * label2 = new QLabel(tr("Then : "));

        eventIf = new QLineEdit(tr(""));
        eventIf->setFixedHeight(20);
        eventThen = new QLineEdit(tr(""));
        eventThen->setFixedHeight(20);

        layout->addWidget(label1,0,0,Qt::AlignLeft);
        layout->addWidget(label2,1,0,Qt::AlignLeft);
        layout->addWidget(eventIf,0,1);
        layout->addWidget(eventThen,1,1);

        QHBoxLayout * okCancelLayout = new QHBoxLayout;
        okCancelLayout->addWidget(okButton);
        okCancelLayout->addWidget(cancelButton);
        layout->addLayout(okCancelLayout,2,1,Qt::AlignRight);

        eventDialog->setWindowTitle(tr("New Event"));
        layout->setColumnStretch(1,3);
        eventDialog->setLayout(layout);

        connect(eventDialog,SIGNAL(accepted()),this,SLOT(eventDialogFinished()));
        connect(eventThen,SIGNAL(returnPressed()),eventDialog,SIGNAL(accepted()));
        connect(&eventsListWidget,SIGNAL(itemActivated(QListWidgetItem*)),this,SLOT(editEventsList(QListWidgetItem*)));

        oldEvent = tr("");
    }

    void SimulationEventsTool::keyPressEvent(QKeyEvent* keyEvent)
    {
        if (keyEvent->key() == Qt::Key_Delete)
            removeEvents();

    }

    void SimulationEventsTool::editEventsList(QListWidgetItem* item)
    {
        if (!item) return;
        QString text = item->text();
        QRegExp regexp("IF   (.+)   THEN   (.+)");
        regexp.indexIn(text);
        if (eventIf && eventThen && eventDialog && regexp.numCaptures() > 0)
        {
            eventIf->setText(regexp.cap(1));
            eventThen->setText(regexp.cap(2));
            oldEvent = regexp.cap(1);
            eventDialog->resize(regexp.cap(2).length()*6,100);
            eventDialog->exec();
        }
    }

    void SimulationEventsTool::eventDialogFinished()
    {
        if (eventIf == 0 || eventThen == 0 || itemHandles.isEmpty()) return;

        QString ifs = eventIf->text();
        QString thens = eventThen->text();

        if (ifs.isEmpty() || thens.isEmpty()) return;

        ItemHandle * lastItem = itemHandles.last();
        if (lastItem == 0 || lastItem->data == 0) return;

        if (!lastItem->hasTextData(tr("Events")))
            insertData(lastItem);

        NetworkWindow * win = currentWindow();

        if (win)
        {
            if (!parseRateString(win, lastItem, ifs))
                return;

            if (!parseRateString(win, lastItem, thens))
                return;

            DataTable<QString> newData(lastItem->data->textData[tr("Events")]);

            if (!oldEvent.isEmpty())
            {
                int k = newData.rowNames().indexOf(oldEvent);
                if (k >= 0)
                    newData.rowName(k) = ifs;
            }

            if (!newData.rowNames().contains(ifs))
            {
                newData.insertRow(newData.rows(),ifs);
            }

            newData.value(ifs,0) = thens;

            win->changeData(tr("if ") + ifs + tr(" then ") + thens,lastItem,tr("Events"),&newData);
        }
        else
        {
            lastItem->data->textData[tr("Events")].value(ifs,0) = thens;
        }

        oldEvent = tr("");
    }

    void SimulationEventsTool::addEvent()
    {
        if (eventDialog)
            eventDialog->exec();
    }

    void SimulationEventsTool::removeEvents()
    {
        if (eventsListWidget.currentItem())
        {
            QRegExp regexp("IF   (.+)   THEN   (.+)");
            regexp.indexIn( eventsListWidget.currentItem()->text() );
            if (regexp.numCaptures() < 2) return;

            int n = eventsListWidget.currentRow();
            int j = 0;

            for (int i=0; i < itemHandles.size(); ++i)
            {
                if (itemHandles[i] && itemHandles[i]->data && itemHandles[i]->hasTextData(tr("Events")))
                {
                    DataTable<QString> dat(itemHandles[i]->data->textData[tr("Events")]);
                    if ( (j + dat.rows()) > n )
                    {
                        int k = n - j;
                        if (k > -1)
                        {
                            dat.removeRow(k);
                            NetworkWindow * win = currentWindow();

                            if (win)
                            {
                                win->changeData(itemHandles[i]->fullName() + tr("'s event changed"),itemHandles[i],tr("Events"),&dat);
                            }
                            else
                            {
                                itemHandles[i]->data->textData[tr("Events")].removeRow(k);
                            }
                            return;
                        }
                    }
                }
            }
        }
    }


    /******************
        C API
        ******************/
    SimulationEventsTool_FToS SimulationEventsTool::fToS;

    void SimulationEventsTool::connectTCFunctions()
    {
        connect(&fToS,SIGNAL(getEventTriggers(QSemaphore*,QStringList*,const QList<ItemHandle*>&)),this,SLOT(getEventTriggers(QSemaphore*,QStringList*,const QList<ItemHandle*>&)));
        connect(&fToS,SIGNAL(getEventResponses(QSemaphore*,QStringList*,const QList<ItemHandle*>&)),this,SLOT(getEventResponses(QSemaphore*,QStringList*,const QList<ItemHandle*>&)));
        connect(&fToS,SIGNAL(addEvent(QSemaphore*,ItemHandle*,const QString&, const QString&)),this,SLOT(addEvent(QSemaphore*,ItemHandle*,const QString&, const QString&)));
    }

    typedef void (*tc_SimulationEventsTool_api)(
            char** (*getEventTriggers)(Array),
            char** (*getEventResponses)(Array),
            void (*addEvent)(OBJ,const char*, const char*)
            );

    void SimulationEventsTool::setupFunctionPointers( QLibrary * library )
    {
        tc_SimulationEventsTool_api f = (tc_SimulationEventsTool_api)library->resolve("tc_SimulationEventsTool_api");
        if (f)
        {
            f(
                    &(_getEventTriggers),
                    &(_getEventResponses),
                    &(_addEvent)
                    );
        }
    }

    void SimulationEventsTool::getEventTriggers(QSemaphore* sem,QStringList* list,const QList<ItemHandle*>& items)
    {
        if (list && !items.isEmpty())
        {
            QList<ItemHandle*> visited;
            QRegExp regex(tr("\\.(?!\\d)"));
            for (int i=0; i < items.size(); ++i)
            {
                if (items[i] && !visited.contains(items[i]) && items[i]->data && items[i]->hasTextData(tr("Events")))
                {
                    QString s;
                    QStringList lst = items[i]->data->textData[tr("Events")].getRowNames();

                    for (int j=0; j < lst.size(); ++j)
                    {
                        s = lst[j];
                        s.replace(regex,tr("_"));
                        (*list) << items[i]->fullName(tr("_")) + tr("_") + s;
                    }

                }
            }
        }
        if (sem)
            sem->release();
    }

    void SimulationEventsTool::getEventResponses(QSemaphore* sem,QStringList* list,const QList<ItemHandle*>& items)
    {
        if (list && !items.isEmpty())
        {
            QList<ItemHandle*> visited;
            QRegExp regex(tr("\\.(?!\\d)"));
            for (int i=0; i < items.size(); ++i)
            {
                if (items[i] && !visited.contains(items[i]) && items[i]->data && items[i]->hasTextData(tr("Events"))
                    && items[i]->data->textData[tr("Events")].cols() > 0)
                    {
                    DataTable<QString>& dat = items[i]->data->textData[tr("Events")];
                    for (int j=0; j < dat.rows(); ++j)
                    {
                        QString s = dat.value(j,0);
                        s.replace(regex,tr("_"));
                        (*list) << s;
                    }
                }
            }
        }
        if (sem)
            sem->release();
    }

    void SimulationEventsTool::addEvent(QSemaphore* sem,ItemHandle* item,const QString& trigger, const QString& event)
    {
        if (item && item->data && !trigger.isEmpty() && !event.isEmpty())
        {
            if (!item->hasTextData(tr("Events")))
                item->data->textData[tr("Events")] = DataTable<QString>();

            DataTable<QString> dat = item->data->textData[tr("Events")];
			
			QRegExp regex(QString("([A-Za-z0-9])_([A-Za-z])"));
			
			QString s1 = trigger, s2 = event;
			s1.replace(regex,QString("\\1.\\2"));
			s2.replace(regex,QString("\\1.\\2"));
			
            dat.value(s1,0) = s2;
            if (currentScene())
                currentScene()->changeData(tr("if ") + s1 + tr(" then ") + s2,item,tr("Events"),&dat);
            else
                item->data->textData[tr("Events")] = dat;
        }
        if (sem)
            sem->release();
    }

    char** SimulationEventsTool::_getEventTriggers(Array a0)
    {
        return fToS.getEventTriggers(a0);
    }

    char** SimulationEventsTool_FToS::getEventTriggers(Array a0)
    {
        QSemaphore * s = new QSemaphore(1);
        QStringList p;
        s->acquire();
        QList<ItemHandle*> * list = ConvertValue(a0);
        emit getEventTriggers(s,&p,*list);
        s->acquire();
        s->release();
        delete s;
        delete list;
        return (char**)ConvertValue(p);
    }

    char** SimulationEventsTool::_getEventResponses(Array a0)
    {
        return fToS.getEventResponses(a0);
    }

    char** SimulationEventsTool_FToS::getEventResponses(Array a0)
    {
        QSemaphore * s = new QSemaphore(1);
        QStringList p;
        s->acquire();
        QList<ItemHandle*> * list = ConvertValue(a0);
        emit getEventResponses(s,&p,*list);
        s->acquire();
        s->release();
        delete s;
        delete list;
        return (char**)ConvertValue(p);
    }

    void SimulationEventsTool::_addEvent(OBJ o, const char* a, const char* b)
    {
        return fToS.addEvent(o,a,b);
    }

    void SimulationEventsTool_FToS::addEvent(OBJ o, const char* a, const char* b)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit addEvent(s,ConvertValue(o),ConvertValue(a),ConvertValue(b));
        s->acquire();
        s->release();
        delete s;
    }
	
	
	static double d = 1.0;
	static double* AddVariable(const char*, void*)
	{
		return &d;
	}

	static double CallFunction(const double*, int)
	{
		return d;
	}

	bool SimulationEventsTool::parseRateString(NetworkWindow * win, ItemHandle * handle, QString& s)
	{
		return EquationParser::validate(win, handle, s, QStringList() << "time");
	}


}


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
    if (!main) return;

    Tinkercell::SimulationEventsTool * simulationEventsTool = new Tinkercell::SimulationEventsTool;
    main->addTool(simulationEventsTool);

}

