/****************************************************************************
 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 Provides a toolbar with buttons that call C functions (run of separate threads)
 
****************************************************************************/
#include <QVBoxLayout>
#include <QDockWidget>
#include <QProcess>
#include <QLibrary>
#include <QRegExp>
#include <QDir>
#include <QFile>
#include "GraphicsScene.h"
#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ConsoleWindow.h"
#include "DynamicLibraryMenu.h"
#include <QtDebug>

namespace Tinkercell
{

    /***************************
            RUN LIB FILES MENU
         ***************************/
    DynamicLibraryMenu::DynamicLibraryMenu() : Tool(tr("Dynamic Library Menu")), getStringDialog(0), actionGroup(this)
    {
        treeWidget.setHeaderHidden(true);
        treeWidget.setColumnCount(1);

        treeWidget.addTopLevelItem(new QTreeWidgetItem(QStringList() << tr("")));

        QFont font = treeWidget.font();
        font.setPointSizeF(font.pointSizeF()*1.2);
        treeWidget.setFont(font);

        connect(&actionGroup,SIGNAL(triggered ( QAction *  )),this,SLOT(actionTriggered ( QAction *  )));

        menuButton = 0;
        separator = 0;
        connectTCFunctions();
		
		QVBoxLayout * layout = new QVBoxLayout;
        layout->addWidget(&treeWidget);
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
        setLayout(layout);
    }

    DynamicLibraryMenu::~DynamicLibraryMenu()
    {
        /*for (int i=0; i < graphicalTools.size(); ++i)
            if (graphicalTools[i].second)
            {
            if (graphicalTools[i].second->scene())
                graphicalTools[i].second->scene()->removeItem(graphicalTools[i].second);
            delete (graphicalTools[i].second);
        }*/
        graphicalTools.clear();
    }
	
	QSize DynamicLibraryMenu::sizeHint() const
	{
		return QSize(200,100);
	}

    void DynamicLibraryMenu::actionTriggered ( QAction *  action )
    {
        if (menuButton)
        {
            disconnect(menuButton,SIGNAL(pressed()),functionsMenu.defaultAction(),SIGNAL(triggered()));
            functionsMenu.setDefaultAction(action);
            connect(menuButton,SIGNAL(pressed()),functionsMenu.defaultAction(),SIGNAL(triggered()));
        }
    }

    QToolButton * DynamicLibraryMenu::addFunction(const QString& category, const QString& functionName, const QIcon& icon)
    {
        QToolButton * toolButton = new QToolButton;
        toolButton->setFont(treeWidget.font());
        toolButton->setAutoRaise(true);
        toolButton->setText(functionName);
        toolButton->setIcon(icon);
        toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        toolButton->setDown(false);
        toolButton->setCheckable(false);

        QTreeWidgetItem * parentItem = 0;
        QList<QTreeWidgetItem*> parents = treeWidget.findItems(category,Qt::MatchFixedString);

        if (parents.size() < 1 || !parents[0])
        {
            parentItem = new QTreeWidgetItem(QStringList() << category);
            parentItem->setIcon(0,icon);
            treeWidget.addTopLevelItem(parentItem);
        }
        else
            parentItem = parents[0];

        QTreeWidgetItem * treeItem = new QTreeWidgetItem;
        parentItem->addChild(treeItem);
        treeWidget.setItemWidget(treeItem,0,toolButton);

        hashFunctionButtons.insert(functionName,toolButton);
        return toolButton;
    }

    QAction * DynamicLibraryMenu::addMenuItem(const QString& functionName, const QIcon& icon, bool defaultAction)
    {
        QAction * action = new QAction(icon,functionName,this);

        if (!menuButton) return action;

        functionsMenu.addAction(action);

        if (!functionsMenu.defaultAction() || defaultAction)
        {
            if (functionsMenu.defaultAction())
				disconnect(menuButton,SIGNAL(pressed()),functionsMenu.defaultAction(),SIGNAL(triggered()));
            
			functionsMenu.setDefaultAction(action);
            connect(menuButton,SIGNAL(pressed()),functionsMenu.defaultAction(),SIGNAL(triggered()));
        }

        actionGroup.addAction(action);
        return action;
    }

    DynamicLibraryMenu::GraphicalActionTool::GraphicalActionTool(const QString& family, const QString& name, const QPixmap& pixmap, Tool * tool)
             : GraphicsItem(tool), targetAction(QIcon(pixmap),name,0), targetFamily(family) 
    {
        QGraphicsPixmapItem * pixmapItem = new QGraphicsPixmapItem(pixmap);
        pixmapItem->scale(30.0/pixmapItem->boundingRect().width(),30.0/pixmapItem->boundingRect().height());
        pixmapItem->setPos(-15.0,-15.0);
        QGraphicsRectItem * rectItem = new QGraphicsRectItem;
        rectItem->setRect(QRectF(-16.0,-16.0,32.0,32.0));
        rectItem->setPen(QPen(Qt::black,2));
        addToGroup(pixmapItem);
        addToGroup(rectItem);
        setToolTip(name);
    }

    void DynamicLibraryMenu::GraphicalActionTool::select()
    {
        targetAction.trigger();
    }

    QAction * DynamicLibraryMenu::addContextMenuItem(const QString& familyName,const QString& functionName, const QPixmap& icon, bool showTool)
    {
        GraphicalActionTool * gtool = new GraphicalActionTool(familyName, functionName,icon,this);
		graphicalTools += QPair<QString,GraphicalActionTool*>(familyName,gtool);
        showGraphicalTool += showTool;
        graphicsItems += gtool;
        return &(gtool->targetAction);
    }

    void DynamicLibraryMenu::select(int)
    {
    }

    void DynamicLibraryMenu::deselect(int)
    {
    }


    bool DynamicLibraryMenu::setMainWindow(MainWindow * main)
    {
        Tool::setMainWindow(main);
        if (mainWindow)
        {
            QToolBar * toolBar = mainWindow->toolBarForTools;
            menuButton = new QToolButton(toolBar);
            menuButton->setIcon(QIcon(":/images/play.png"));
            menuButton->setMenu(&functionsMenu);

            menuButton->setPopupMode(QToolButton::MenuButtonPopup);
            //connect(menuButton,SIGNAL(released()),functionsMenu.defaultAction(),SIGNAL(triggered()));

            toolBar->addWidget(menuButton);

			setWindowTitle(tr("Tools"));
			setWindowIcon(QIcon(tr(":/images/function.png")));
            mainWindow->addToolWindow(this, MainWindow::defaultToolWindowOption, Qt::BottomDockWidgetArea);

            connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
                    this,SLOT(itemsSelected(GraphicsScene *,const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));

            connect(mainWindow,SIGNAL(itemsInserted(NetworkWindow*, const QList<ItemHandle*>&)),
                    this, SLOT(itemsInserted(NetworkWindow*, const QList<ItemHandle*>&)));


            connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

            return true;
        }
        return false;
    }

    void DynamicLibraryMenu::itemsInserted(NetworkWindow* , const QList<ItemHandle*>& handles)
    {
        for (int i=0; i < handles.size(); ++i)
        {
            if (handles[i] && !handles[i]->tools.contains(this))
            {
                handles[i]->tools += this;
            }
        }
    }
	
	void DynamicLibraryMenu::GraphicalActionTool::visible(bool b)
	{
		if (!tool) return;
		GraphicsScene * scene = tool->currentScene();
		if (!scene) return;
		
		ItemHandle * handle = 0;
		QList<QGraphicsItem*>& items = scene->selected();
		bool match = true;
		
		for (int j=0; j < items.size(); ++j)
		{
			if (handle = getHandle(items[j]))
			{
				if (!handle->isA(targetFamily))
				{
					match = false;
					break;
				}
			}
		}
		
		Tool::GraphicsItem::visible(b && match);
		
	}

    void DynamicLibraryMenu::itemsSelected(GraphicsScene * scene,const QList<QGraphicsItem*>& items, QPointF, Qt::KeyboardModifiers)
    {
        if (!mainWindow || !scene) return;

        QList<QAction*> actionsToAdd, actionsToRemove;
        bool match;
        ItemHandle * handle;

        for (int i=0; i < graphicalTools.size(); ++i)
            if (graphicalTools[i].second)
            {
            match = true;
            for (int j=0; j < items.size(); ++j)
            {
                if (handle = getHandle(items[j]))
                {
                    if (!handle->isA(graphicalTools[i].first))
                    {
                        match = false;
                        break;
                    }
                }
            }
            if (match)
                actionsToAdd << &(graphicalTools[i].second->targetAction);
            else
                actionsToRemove << &(graphicalTools[i].second->targetAction);
        }

        if (actionsToAdd.size() > 0)
        {
            if (separator)
                mainWindow->contextItemsMenu.addAction(separator);
            else
                separator = mainWindow->contextItemsMenu.addSeparator();

            for (int i=0; i < actionsToAdd.size(); ++i)
                mainWindow->contextItemsMenu.addAction(actionsToAdd[i]);
        }
        else
        {
            if (separator)
                mainWindow->contextItemsMenu.removeAction(separator);
        }

        if (actionsToRemove.size() > 0)
        {
            for (int i=0; i < actionsToRemove.size(); ++i)
                mainWindow->contextItemsMenu.removeAction(actionsToRemove[i]);
        }
    }

    /**********************
            C API
        ***********************/

    void DynamicLibraryMenu::connectTCFunctions()
    {
        connect(&fToS,SIGNAL(getString(QSemaphore*,QString*,const QString&)),this,SLOT(getString(QSemaphore*,QString*,const QString&)));
        connect(&fToS,SIGNAL(getSelectedString(QSemaphore*,int*,const QString&,const QStringList&,const QString&,int)),this,SLOT(getSelectedString(QSemaphore*,int*,const QString&,const QStringList&,const QString&,int)));
        connect(&fToS,SIGNAL(getNumber(QSemaphore*,qreal*,const QString&)),this,SLOT(getNumber(QSemaphore*,qreal*,const QString&)));
        connect(&fToS,SIGNAL(getNumbers(QSemaphore*,const QStringList&,qreal*)),this,SLOT(getNumbers(QSemaphore*,const QStringList&,qreal*)));
        connect(&fToS,SIGNAL(getFilename(QSemaphore*,QString*)),this,SLOT(getFilename(QSemaphore*,QString*)));
        connect(&fToS,SIGNAL(callFunction(QSemaphore*,const QString&)),this,SLOT(callFunction(QSemaphore*,const QString&)));
    }

    typedef void (*tc_DynamicLibraryMenu_api)(
            char* (*getString)(const char*),
            int (*getSelectedString)(const char*, char**,const char*, int),
            double (*getNumber)(const char*),
            void (*getNumbers)(char**,double*),
            char* (*getFilename)(),
            void (*callFuntion)(const char*)
            );

    void DynamicLibraryMenu::setupFunctionPointers( QLibrary * library)
    {
        tc_DynamicLibraryMenu_api f = (tc_DynamicLibraryMenu_api)library->resolve("tc_DynamicLibraryMenu_api");
        if (f)
        {
            //qDebug() << "tc_DynamicLibraryMenu_api resolved";
            f(
                    &(_getString),
                    &(_getSelectedString),
                    &(_getNumber),
                    &(_getNumbers),
                    &(_getFilename),
                    &(_callFunction)
                    );
        }
    }

    void DynamicLibraryMenu::getNumber(QSemaphore* s,double* p,const QString& name)
    {
        if (p)
        {
            (*p) = QInputDialog::getDouble(mainWindow,tr("Get Number"),name);
        }
        if (s)
            s->release();
    }

    void DynamicLibraryMenu::getString(QSemaphore* s,QString* p,const QString& name)
    {
        if (p)
        {
            (*p) = QInputDialog::getText(mainWindow,tr("Get Text"),name);
            (*p).replace(tr("."),tr("_"));
        }
        if (s)
            s->release();
    }

    void DynamicLibraryMenu::getNumbers(QSemaphore* s,const QStringList& names,qreal* res)
    {
        QDialog * dialog = new QDialog(this);

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

        QPushButton * ok = new QPushButton(tr("Done"), this);
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

    void DynamicLibraryMenu::getFilename(QSemaphore* s,QString* p)
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

    void DynamicLibraryMenu::getStringListItemSelected(QListWidgetItem * item)
    {
        if (item)
            getStringListNumber = getStringList.currentRow();
        if (getStringDialog)
            getStringDialog->accept();
    }

    void DynamicLibraryMenu::getStringListRowChanged ( int  )
    {
        if (getStringList.currentItem())
            getStringListNumber = getStringListText.indexOf(getStringList.currentItem()->text());
    }

    void DynamicLibraryMenu::getStringListCanceled (  )
    {
        getStringListNumber = -1;
    }

    void DynamicLibraryMenu::getStringSearchTextEdited ( const QString & text )
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

    void DynamicLibraryMenu::getSelectedString(QSemaphore* s,int* p,const QString& name, const QStringList& list0,const QString& init, int option)
    {
        if (p && mainWindow)
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

    void DynamicLibraryMenu::callFunction(QSemaphore* s, const QString& functionName)
    {
        if (hashFunctionButtons.contains(functionName))
        {
            QToolButton * button = hashFunctionButtons[functionName];
            if (button)
                button->animateClick();
        }

        if (s) s->release();
    }

    /******************************************************/

    DynamicLibraryMenu_FToS DynamicLibraryMenu::fToS;

    char* DynamicLibraryMenu::_getString(const char* title)
    {
        return fToS.getString(title);
    }

    char* DynamicLibraryMenu::_getFilename()
    {
        return fToS.getFilename();
    }

    int DynamicLibraryMenu::_getSelectedString(const char* title,char ** list,const char* c, int i)
    {
        return fToS.getSelectedString(title,list,c,i);
    }

    double DynamicLibraryMenu::_getNumber(const char* title)
    {
        return fToS.getNumber(title);
    }

    void DynamicLibraryMenu::_getNumbers(char** names, double * res)
    {
        return fToS.getNumbers(names,res);
    }

    void DynamicLibraryMenu::_callFunction(const char * c)
    {
        return fToS.callFunction(c);
    }

    double DynamicLibraryMenu_FToS::getNumber(const char* c)
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

    void DynamicLibraryMenu_FToS::getNumbers(char** c, double * d)
    {
        //qDebug() << "get number dialog";
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit getNumbers(s,ConvertValue(c), d);
        s->acquire();
        s->release();
        delete s;
    }

    char* DynamicLibraryMenu_FToS::getString(const char* c)
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

    char* DynamicLibraryMenu_FToS::getFilename()
    {
        //qDebug() << "get string dialog";
        QSemaphore * s = new QSemaphore(1);
        QString p;
        s->acquire();
        emit getFilename(s,&p);
        s->acquire();
        s->release();
        delete s;
        return ConvertValue(p);
    }


    int DynamicLibraryMenu_FToS::getSelectedString(const char* c, char ** list,const char* c1, int i)
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

    void DynamicLibraryMenu_FToS::callFunction(const char * c)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit callFunction(s,ConvertValue(c));
        s->acquire();
        s->release();
        delete s;
    }
}
