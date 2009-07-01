/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This class adds the "attributes" data to each item in Tinkercell.
 Two types of attributes are added -- "Numerical Attributes" and "Text Attributes".
 Attributes are essentially a <name,value> pair that are used to characterize an item.

 The BasicInformationTool contains two tools, one for text attributes and one
 for numerical attributes. The buttons are drawn as NodeGraphicsItems using the datasheet.xml and
 textsheet.xml files that define the NodeGraphicsItems.

****************************************************************************/

#include <QSettings>
#include <QMessageBox>
#include "NetworkWindow.h"
#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "OutputWindow.h"
#include "MainWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ModelSummaryTool.h"
#include "BasicInformationTool.h"
#include "StoichiometryTool.h"

namespace Tinkercell
{

    void BasicInformationTool::select()
    {
        NetworkWindow * win = currentWindow();
        if (!win) return;

        itemHandles = win->selectedHandles();
        if (itemHandles.size() < 1) return;

        if (dockWidget && dockWidget->widget() != this)
            dockWidget->setWidget(this);

        openedByUser = true;
        updateTable();
        if (dockWidget != 0)
        {
            if (dockWidget->isVisible())
                openedByUser = false;
            else
                dockWidget->show();
        }
        else
        {
            if (isVisible())
                openedByUser = false;
            else
                show();
        }
        this->setFocus();
    }

    void BasicInformationTool::deselect()
    {
        if (openedByUser && (!dockWidget || dockWidget->isFloating()))
        {
            openedByUser = false;

            if (dockWidget != 0)
                dockWidget->hide();
            else
                hide();
        }
    }

    bool BasicInformationTool::setMainWindow(MainWindow * main)
    {
        Tool::setMainWindow(main);

        if (mainWindow)
        {
            connect(mainWindow,SIGNAL(windowClosing(NetworkWindow * , bool *)),this,SLOT(windowClosing(NetworkWindow * , bool *)));

            connect(mainWindow,SIGNAL(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)),
                    this, SLOT(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)));

            connect(mainWindow,SIGNAL(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
                    this,SLOT(itemsSelected(GraphicsScene*, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));

            connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

            connect(mainWindow,SIGNAL(historyChanged(int)),this,SLOT(historyUpdate(int)));

            connect(mainWindow,SIGNAL(pluginLoaded(const QString&)),this,SLOT(pluginLoaded(const QString&)));

            dockWidget = mainWindow->addDockingWindow(name,this,Qt::BottomDockWidgetArea,Qt::NoDockWidgetArea);

            if (dockWidget)
            {
                if (type == text)
                    dockWidget->move(mainWindow->geometry().bottomRight() - QPoint(sizeHint().width()*2,sizeHint().height()*3));
                else
                    dockWidget->move(mainWindow->geometry().bottomRight() - QPoint(sizeHint().width()*2,sizeHint().height()*2));

                dockWidget->setWindowFlags(Qt::Tool);
                dockWidget->setAttribute(Qt::WA_ContentsPropagated);
                dockWidget->setPalette(QPalette(QColor(255,255,255,255)));
                dockWidget->setAutoFillBackground(true);
                //dockWidget->setWindowOpacity(0.8);

                QCoreApplication::setOrganizationName("TinkerCell");
                QCoreApplication::setOrganizationDomain("www.tinkercell.com");
                QCoreApplication::setApplicationName("TinkerCell");

                QSettings settings("TinkerCell", "TinkerCell");

                if (type == both || type == numerical)
                {
                    settings.beginGroup("BasicInformationTool");
                    //dockWidget->resize(settings.value("size", sizeHint()).toSize());
                    //dockWidget->move(settings.value("pos", dockWidget->pos()).toPoint());
                }
                else
                {
                    settings.beginGroup("BasicInformationToolText");
                    //dockWidget->resize(settings.value("size", sizeHint()).toSize());
                    //dockWidget->move(settings.value("pos", dockWidget->pos()).toPoint());
                }
                dockWidget->setFloating(true);
                settings.endGroup();
                dockWidget->hide();
            }

            pluginLoaded(QString());
        }
        return (mainWindow != 0);
    }

    void BasicInformationTool::pluginLoaded(const QString&)
    {
        static bool connected = false;
        if (connected) return;

//         if (!connected && type != text && mainWindow && mainWindow->tools.contains(tr("Model Summary")))
//         {
//             QWidget * widget = mainWindow->tools[tr("Model Summary")];
//             ModelSummaryTool * modelSummary = static_cast<ModelSummaryTool*>(widget);
//
//             connect(modelSummary,SIGNAL(displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)),
//                     this,SLOT(displayModel(QTabWidget&, const QList<ItemHandle*>&, QHash<QString,qreal>&, QHash<QString,QString>&)));
//             connected = true;
//         }
    }

    void BasicInformationTool::windowClosing(NetworkWindow * , bool *)
    {
        QCoreApplication::setOrganizationName("TinkerCell");
        QCoreApplication::setOrganizationDomain("www.tinkercell.com");
        QCoreApplication::setApplicationName("TinkerCell");

        QSettings settings("TinkerCell", "TinkerCell");

        if (dockWidget)
        {
            if (type == text)
            {
                settings.beginGroup("BasicInformationToolText");
            }
            else
            {
                settings.beginGroup("BasicInformationTool");
            }
            //settings.setValue("floating", dockWidget && dockWidget->isFloating());
            settings.setValue("size", dockWidget->size());
            settings.setValue("pos", dockWidget->pos());
            settings.endGroup();
        }
    }

    void BasicInformationTool::aboutToDisplayModel(const QList<ItemHandle*>& , QHash<QString,qreal>& , QHash<QString,QString>& )
    {
    }

    void BasicInformationTool::displayModel(QTabWidget& widgets, const QList<ItemHandle*>& items, QHash<QString,qreal>& constants, QHash<QString,QString>& equationsList)
    {
        if (type == text) return;

        itemHandles.clear();
        tableItems.clear();
        tableWidget.clear();

        QString equations = QStringList(equationsList.values()).join(" ");

        for (int i=0; i < items.size(); ++i)
        {
            if (!itemHandles.contains(items[i]) && items[i])
                itemHandles += items[i];

            if (!equations.isEmpty())
                if (items[i] && items[i]->type == NodeHandle::Type)
                {
                QList<ConnectionHandle*> connections = static_cast<NodeHandle*>(items[i])->connections();
                for (int j=0; j < connections.size(); ++j)
                    if (connections[j] && !itemHandles.contains(connections[j]))
                        itemHandles += connections[j];
            }
            else
                if (items[i] && items[i]->type == ConnectionHandle::Type)
                {
                QList<NodeHandle*> nodes = static_cast<ConnectionHandle*>(items[i])->nodes();
                for (int j=0; j < nodes.size(); ++j)
                    if (nodes[j] && !itemHandles.contains(nodes[j]))
                        itemHandles += nodes[j];
            }
        }


        QStringList names, values;
        QStringList headers;

        DataTable<qreal> * nDataTable = 0;

        for (int i=0; i < itemHandles.size(); ++i) //build combined matrix for all selected reactions
        {
            bool remove = true;
            if (itemHandles[i] != 0 && itemHandles[i]->data != 0)
            {
                if (itemHandles[i]->hasNumericalData(this->name))
                {
                    nDataTable = &(itemHandles[i]->data->numericalData[this->name]);
                    for (int j=0; j < nDataTable->rows(); ++j)
                    {
                        QString str = itemHandles[i]->fullName() + tr(".") + nDataTable->rowName(j);
                        if ((equations.isEmpty() && !(itemHandles[i]->type == ConnectionHandle::Type &&
                                                      (nDataTable->rowName(j) == tr("numin") || nDataTable->rowName(j) == tr("numout"))))
                            || equations.contains(str) ||
                                    (itemHandles[i]->family() && !itemHandles[i]->family()->numericalAttributes.contains(nDataTable->rowName(j))))
                            {
                            tableItems << QPair<ItemHandle*,int>(itemHandles[i],j);
                            headers << itemHandles[i]->fullName();
                            names += nDataTable->rowName(j);
                            values += QString::number(nDataTable->value(j,0));
                            constants.insert(str,nDataTable->value(j,0));
                            remove = false;
                        }
                    }
                }
            }
            if (remove)
            {
                itemHandles.removeAt(i);
                --i;
            }
        }

        if (names.size() > 0)
        {
            if (dockWidget && dockWidget->isVisible())
                dockWidget->hide();

            widgets.addTab(this,tr("Parameters"));

            tableWidget.clear();
            tableWidget.setRowCount(names.size());
            tableWidget.setColumnCount(2);
            tableWidget.setHorizontalHeaderLabels(QStringList() << "variable" << "value");
            tableWidget.setVerticalHeaderLabels(headers);

            disconnect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));

            for (int i=0; i < tableWidget.rowCount(); ++i)
            {
                tableWidget.setItem(i,0,new QTableWidgetItem(names[i]));
                tableWidget.setItem(i,1,new QTableWidgetItem(values[i]));
            }

            connect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));
        }
        else
            if (dockWidget && dockWidget->widget() != this)
                dockWidget->setWidget(this);
    }

    void BasicInformationTool::historyUpdate(int i)
    {
        if (isVisible() || (parentWidget() && parentWidget() != mainWindow && parentWidget()->isVisible()))// && dockWidget && dockWidget->isVisible())
            updateTable();
        NetworkWindow * win = currentWindow();
        if (mainWindow && win && mainWindow->statusBar())
            mainWindow->statusBar()->showMessage(win->history.text(i-1));

    }

    void BasicInformationTool::itemsInserted(GraphicsScene* scene, const QList<QGraphicsItem *>& , const QList<ItemHandle*>& handles)
    {
        for (int i=0; i < handles.size(); ++i)
        {
            if (handles[i] && !handles[i]->tools.contains(this))
                handles[i]->tools += this;

            if (handles[i] && handles[i]->data)
            {
                insertDataMatrix(scene,handles[i]);
            }
        }
        if (scene && scene->symbolsTable)
            scene->symbolsTable->update();
    }

    void BasicInformationTool::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& list, QPointF , Qt::KeyboardModifiers )
    {
        if (scene && (parentWidget() && parentWidget() != mainWindow && parentWidget()->isVisible()))
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

    void BasicInformationTool::setValue(int row,int col)
    {
        static bool recursive = false;

        if (recursive)
        {
            recursive = false;
            return;
        }
        recursive = false;

        NetworkWindow * win = currentWindow();
        if (!win) return;

        if (col > 1 || row >= tableItems.size() || !tableWidget.item(row,col)) return;

        ItemHandle * handle = tableItems[row].first;
        int rowNumber = tableItems[row].second;

        if (!handle || !handle->data) return;

        if ((type == both || type == numerical) && handle->hasNumericalData(this->name))
        {
            DataTable<qreal> nDat(handle->data->numericalData[this->name]);

            if (col == 0)
            {
                if (rowNumber < nDat.rows())
                {
                    if (handle->family() && handle->family()->numericalAttributes.contains(nDat.rowName(rowNumber)))
                    {
                        recursive = true;
                        tableWidget.item(row,col)->setText(nDat.rowName(rowNumber));
                        OutputWindow::message(nDat.rowName(rowNumber) + tr(" cannot be removed because it is a family attribute"));
                    }
                    else
                    {
                        QString name = tableWidget.item(row,col)->text();
                        name.replace(QRegExp(tr("[^a-zA-Z_0-9]")),tr(""));
                        if (name.isEmpty())
                        {
                            nDat.removeRow(rowNumber);
                            win->changeData(handle,this->name,&nDat);
                        }
                        else
                        {
                            QString oldname = nDat.rowName(rowNumber);
                            nDat.rowName(rowNumber) = name;
                            QList<QUndoCommand*> commands;
                            commands << new ChangeDataCommand<qreal>(tr("change data"),&handle->data->numericalData[this->name],&nDat)
                                    << new RenameCommand(tr("rename"),win->allHandles(),handle->fullName() + tr(".") + oldname,handle->fullName() + tr(".") + name);
                            CompositeCommand * command = new CompositeCommand(
                                    tr("renamed ") + oldname + tr(" to ") + name,
                                    commands);

                            win->history.push(command);
                        }
                    }
                }
            }
            else
            {
                bool ok;
                double value = tableWidget.item(row,col)->text().toDouble(&ok);
                if (!ok)
                {
                    return;
                }
                nDat.value(rowNumber,0) = value;
                win->changeData(handle,this->name,&nDat);
            }
        }

        if ((type == both || type == text) && handle->hasTextData(this->name))
        {
            DataTable<QString> sDat(handle->data->textData[this->name]);

            if (col == 0)
            {
                if (rowNumber < sDat.rows())
                {
                    if (handle->family() && handle->family()->textAttributes.contains(sDat.rowName(rowNumber)))
                    {
                        recursive = true;
                        tableWidget.item(row,col)->setText(sDat.rowName(rowNumber));
                        OutputWindow::message(sDat.rowName(rowNumber) + tr(" cannot be removed because it is a family attribute"));
                    }
                    else
                    {
                        QString name = tableWidget.item(row,col)->text();
                        name.replace(QRegExp(tr("[^a-zA-Z_]")),tr(""));
                        if (name.isEmpty())
                        {
                            sDat.removeRow(rowNumber);
                            win->changeData(handle,this->name,&sDat);
                        }
                        else
                        {
                            QString oldname = sDat.rowName(rowNumber);
                            sDat.rowName(rowNumber) = name;
                            QList<QUndoCommand*> commands;
                            commands += new ChangeDataCommand<QString>(tr("change data"),&handle->data->textData[this->name],&sDat);
                            commands += new RenameCommand(tr("rename"),win->allHandles(),handle->fullName() + tr(".") + oldname,handle->fullName() + tr(".") + name);
                            CompositeCommand * command = new CompositeCommand(
                                    tr("renamed ") + oldname + tr(" to ") + name,
                                    commands);

                            win->history.push(command);
                        }
                    }
                }
            }
            else
            {
                QString value = tableWidget.item(row,col)->text();
                sDat.value(rowNumber,0) = value;
                win->changeData(handle,this->name,&sDat);
            }
        }
    }

    BasicInformationTool::BasicInformationTool(const QString& typ) : Tool(tr("Attributes")), delegate(QStringList())
    {
        QString appDir = QCoreApplication::applicationDirPath();
#ifdef Q_WS_MAC
        appDir += tr("/../../..");
#endif
        delegate.textColumn = 0;
        if (typ == tr("both"))
        {
            type = BasicInformationTool::both;
            groupBox = new QGroupBox(tr(" Attributes "),this);
        }
        else
            if (typ == tr("text"))
            {
            type = BasicInformationTool::text;
            name = tr("Text Attributes");
            groupBox = new QGroupBox(tr(" Strings "),this);
        }
        else
        {
            type = BasicInformationTool::numerical;
            name = tr("Numerical Attributes");
            groupBox = new QGroupBox(tr(" Constants "),this);
        }

        NodeGraphicsReader reader;

        graphicsItem = new GraphicsItem(this);
        if (type == BasicInformationTool::both)
        {
            reader.readXml(&item,appDir + tr("/BasicTools/textsheet.xml"));
            item.setToolTip(tr("Attributes"));
            graphicsItem->setToolTip(tr("Attributes"));
        }
        else
        if (type == BasicInformationTool::numerical)
        {
            reader.readXml(&item,appDir + tr("/BasicTools/datasheet.xml"));
            item.setToolTip(tr("Numerical attributes"));
            graphicsItem->setToolTip(tr("Numerical attributes"));
        }
        else
        {
            reader.readXml(&item,appDir + tr("/BasicTools/textsheet.xml"));
            item.setToolTip(tr("Text attributes"));
            graphicsItem->setToolTip(tr("Text attributes"));
        }
        item.normalize();
        item.scale(30.0/item.sceneBoundingRect().width(),40.0/item.sceneBoundingRect().height());
        graphicsItem->addToGroup(&item);

        openedByUser = false;

        tableWidget.setEditTriggers ( QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed );
        connect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));

        QFont font = this->font();
        font.setPointSize(12);
        tableWidget.setFont(font);

        QHBoxLayout * actionsLayout = new QHBoxLayout;

        QToolButton * addAttribAction = new QToolButton(this);
        addAttribAction->setIcon(QIcon(":/images/plus.png"));

        QToolButton * removeAttribAction = new QToolButton(this);
        removeAttribAction->setIcon(QIcon(":/images/minus.png"));

        QString message;
        if (type == BasicInformationTool::numerical)
        {
            addAttribAction->setToolTip(tr("Add constant"));
            removeAttribAction->setToolTip(tr("Remove constant"));
            //tableWidget.setItemDelegate(&delegate);
            //tableWidget.setFixedWidth(160);
            groupBox->setMinimumWidth(100);
            message = tr("This table shows the set of parameters, or numerical attributes, belonging with the selected objects. Parameters can be used inside rate equations or other equations. For example, if the name of the parameter is k0 and it belongs with an object name J0, then you can use J0.k0 inside questions in order to address this value. You may change the name or value of the parameter using this table. Changing the name will automatically update all the equations where it is used.");
        }
        else
            if (type == BasicInformationTool::text)
            {
            addAttribAction->setToolTip(tr("Add string"));
            removeAttribAction->setToolTip(tr("Remove string"));
            message = tr("This table shows the text attributes belonging with the selected objects. Text attributes are any properties of an object that is represented as a string. Examples include DNA sequence, database IDs, etc.");
        }
        else
        {
            addAttribAction->setToolTip(tr("Add attribute"));
            removeAttribAction->setToolTip(tr("Remove attribute"));
            message = tr("This table shows the set of parameters, or numerical attributes, belonging with the selected objects. Parameters can be used inside rate equations or other equations. For example, if the name of the parameter is k0 and it belongs with an object name J0, then you can use J0.k0 inside questions in order to address this value. You may change the name or value of the parameter using this table. Changing the name will automatically update all the equations where it is used.");
            message += tr("\nThis table also shows the text attributes belonging with the selected objects. Text attributes are any properties of an object that is represented as a string. Examples include DNA sequence, database IDs, etc.");
        }

        connect(addAttribAction,SIGNAL(pressed()),this,SLOT(addAttribute()));
        connect(removeAttribAction,SIGNAL(pressed()),this,SLOT(removeSelectedAttributes()));

        QToolButton * question = new QToolButton(this);
        question->setIcon(QIcon(":/images/question.png"));

        QMessageBox * messageBox = new QMessageBox(QMessageBox::Information,tr("About Attributes Table"),message,QMessageBox::StandardButtons(QMessageBox::Close), const_cast<QWidget*>((QWidget*)this), Qt::WindowFlags (Qt::Dialog));
        connect(question,SIGNAL(pressed()),messageBox,SLOT(exec()));

        actionsLayout->addWidget(addAttribAction);
        actionsLayout->addWidget(removeAttribAction);
        actionsLayout->addStretch(1);
        actionsLayout->addWidget(question);

        QVBoxLayout * boxLayout = new QVBoxLayout;
        boxLayout->addWidget(&tableWidget,1);

        boxLayout->addLayout(actionsLayout);
        groupBox->setLayout(boxLayout);

        dockWidget = 0;

        QVBoxLayout * layout = new QVBoxLayout;
        layout->addWidget(groupBox);
        setLayout(layout);

        connectTCFunctions();

    }

    QSize BasicInformationTool::sizeHint() const
    {
        return QSize(300, 200);
    }

    void BasicInformationTool::insertDataMatrix(GraphicsScene * scene, ItemHandle * handle)
    {
        if (handle == 0 || handle->family() == 0 || !handle->data) return;

        QStringList colNames;
        colNames << "value";

        ItemFamily* family = handle->family();

        if (!family->measurementUnit.first.isEmpty() && !handle->hasNumericalData(QString("Initial Value")))
        {
            DataTable<qreal> initialValues;
            initialValues.resize(1,1);

            initialValues.rowName(0) = family->measurementUnit.first;
            initialValues.colName(0) = family->measurementUnit.second;
            initialValues.value(0,0) = 1.0;

            handle->data->numericalData.insert(QString("Initial Value"),initialValues);
        }

        if (!handle->hasNumericalData(QString("Fixed")))
        {
            DataTable<qreal> fixed;
            fixed.resize(1,1);

            fixed.rowName(0) = QString("fix");
            fixed.colName(0) = QString("value");
            fixed.value(0,0) = 0.0;

            handle->data->numericalData.insert(QString("Fixed"),fixed);
        }

        if ((type == both || type == numerical) && !(handle->hasNumericalData(name)))
        {
            QList<QString> nKeys = family->numericalAttributes.keys();
            DataTable<qreal> numericalAttributes;
            numericalAttributes.resize(nKeys.size(),1);

            for (int i=0; i < numericalAttributes.rows() && i < nKeys.size(); ++i)
            {
                numericalAttributes.value(i,0) = family->numericalAttributes.value(nKeys[i]);
                numericalAttributes.rowName(i) = nKeys[i];
            }

            numericalAttributes.setColNames(colNames);
            handle->data->numericalData.insert(this->name,numericalAttributes);
        }

        if ((type == both || type == text) && !(handle->hasTextData(name)))
        {
            QList<QString> sKeys = family->textAttributes.keys();
            DataTable<QString> textAttributes;
            textAttributes.resize(sKeys.size(),1);

            for (int i=0; i < textAttributes.rows() && i < sKeys.size(); ++i)
            {
                textAttributes.value(i,0) = family->textAttributes.value(sKeys[i]);
                textAttributes.rowName(i) = sKeys[i];
            }

            textAttributes.setColNames(colNames);
            handle->data->textData.insert(this->name,textAttributes);
        }
    }

    void BasicInformationTool::updateTable()
    {
        tableItems.clear();
        tableWidget.clear();

        if (itemHandles.size() < 1)
        {
            tableWidget.clearContents();
            tableWidget.setRowCount(0);
            return;
        }

        QStringList names, values;
        QStringList headers;

        DataTable<qreal> * nDataTable = 0;
        DataTable<QString> * sDataTable = 0;

        for (int i=0; i < itemHandles.size(); ++i) //build combined matrix for all selected reactions
        {
            if (itemHandles[i] != 0 && itemHandles[i]->data != 0)
            {
                if ((type == both || type == numerical) && itemHandles[i]->hasNumericalData(this->name))
                {
                    nDataTable = &(itemHandles[i]->data->numericalData[this->name]);
                    for (int j=0; j < nDataTable->rows(); ++j)
                    {
                        if (!(itemHandles[i]->type == ConnectionHandle::Type &&
                              (nDataTable->rowName(j) == tr("numin") || nDataTable->rowName(j) == tr("numout"))))
                        {
                            tableItems << QPair<ItemHandle*,int>(itemHandles[i],j);
                            headers << itemHandles[i]->fullName() + tr(".");
                            names += nDataTable->rowName(j);
                            values += QString::number(nDataTable->value(j,0));
                        }
                    }
                }
                if ((type == both || type == text) && itemHandles[i]->hasTextData(this->name))
                {
                    sDataTable = &(itemHandles[i]->data->textData[this->name]);
                    for (int j=0; j < sDataTable->rows(); ++j)
                    {
                        if (!(itemHandles[i]->type == ConnectionHandle::Type &&
                              sDataTable->rowName(j) == tr("typein") || sDataTable->rowName(j) == tr("typeout")))
                        {
                            tableItems << QPair<ItemHandle*,int>(itemHandles[i],j);
                            headers << itemHandles[i]->fullName() + tr(".");
                            names += sDataTable->rowName(j);
                            values += (sDataTable->value(j,0));
                        }
                    }
                }
            }
        }

        tableWidget.setRowCount(names.size());
        tableWidget.setColumnCount(2);
        tableWidget.setHorizontalHeaderLabels(QStringList() << "variable" << "value");
        tableWidget.setVerticalHeaderLabels(headers);

        disconnect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));

        for (int i=0; i < tableWidget.rowCount(); ++i)
        {
            tableWidget.setItem(i,0,new QTableWidgetItem(names[i]));
            tableWidget.setItem(i,1,new QTableWidgetItem(values[i]));
        }

        connect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));
    }

    /*
        void BasicInformationTool::setupDialogs()
        {
                if (!commandDialog) return;
                QGridLayout * layout = new QGridLayout;
                QPushButton * okButton = new QPushButton("OK");
                connect(okButton,SIGNAL(released()),commandDialog,SLOT(accept()));
                QPushButton * cancelButton = new QPushButton("Cancel");
                connect(cancelButton,SIGNAL(released()),commandDialog,SLOT(reject()));
                QLabel * label1 = new QLabel(tr("Name (without prefix) :"));
                QLabel * label2 = new QLabel(tr("Value : "));

                dialogName = new QLineEdit(tr("k0"));
                dialogName->setFixedHeight(20);
                dialogValue = new QLineEdit(tr("3.14"));
                dialogValue->setFixedHeight(20);

                layout->addWidget(label1,0,0,Qt::AlignLeft);
                layout->addWidget(label2,1,0,Qt::AlignLeft);

                layout->addWidget(dialogName,0,1);
                layout->addWidget(dialogValue,1,1);

                layout->addWidget(okButton,2,0,Qt::AlignRight);
                layout->addWidget(cancelButton,2,1,Qt::AlignCenter);
                commandDialog->setWindowTitle(tr("New Attribute"));
                layout->setColumnStretch(1,3);
                commandDialog->setLayout(layout);

                connect(commandDialog,SIGNAL(accepted()),this,SLOT(dialogFinished()));
                connect(dialogValue,SIGNAL(returnPressed()),commandDialog,SIGNAL(accepted()));
        }
*/
    /*
        void BasicInformationTool::dialogFinished()
        {
                if (dialogName == 0 || dialogValue == 0 || itemHandles.isEmpty()) return;
                QString name = dialogName->text();
                QString value = dialogValue->text();

                if (name.isEmpty() || value.isEmpty()) return;

                ItemHandle * lastItem = itemHandles.last();
                if (lastItem == 0 || lastItem->data == 0) return;

                disconnect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));

                name.replace(QRegExp(tr(".*") + lastItem->name + tr(".")),"");
                name.replace(QRegExp(tr(".*") + lastItem->fullName() + tr(".")),"");

                name = RemoveDisallowedCharactersFromName(name);

                int n = tableWidget.rowCount();
                tableWidget.insertRow(n);
                tableWidget.setVerticalHeaderItem(n, new QTableWidgetItem(lastItem->fullName() + tr(".") + name));
                tableWidget.setItem(n,0,new QTableWidgetItem(value));

                //numerical or text data?
                bool ok = false;
                double num = value.toDouble(&ok);
                if (ok)
                {
                        if (lastItem->hasNumericalData(this->name))
                        {
                                DataTable<qreal> * newDataTable = new DataTable<qreal>(lastItem->data->numericalData[this->name]);
                                newDataTable->insertRow(newDataTable->rows(),name);
                                newDataTable->value(newDataTable->rows()-1,0) = num;
                                if (mainWindow != 0 && mainWindow->currentScene() != 0)
                                {
                                        mainWindow->currentScene()->changeData(lastItem,this->name,newDataTable);
                                }
                                delete newDataTable;
                        }
                }
                else
                {
                        if (lastItem->hasTextData(this->name))
                        {
                                DataTable<QString> * newDataTable = new DataTable<QString>(lastItem->data->textData[this->name]);
                                newDataTable->insertRow(newDataTable->rows(),name);
                                newDataTable->value(newDataTable->rows()-1,0) = value;

                                if (mainWindow != 0 && mainWindow->currentScene() != 0)
                                {
                                        mainWindow->currentScene()->changeData(lastItem,this->name,newDataTable);
                                }

                                delete newDataTable;
                        }
                }

                connect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));
        }
*/
    void BasicInformationTool::addAttribute()
    {
        NetworkWindow * win = currentWindow();
        if (!win) return;
        QString name;

        int i=0;
        ItemHandle * lastItem = 0;

        if (tableItems.size() < 1)
        {
            if (win->selectedHandles().isEmpty()) return;
            lastItem = win->selectedHandles()[0];
        }
        else
        {
            lastItem = tableItems[i].first;
            while (!lastItem && (i+1) < tableItems.size()) lastItem = tableItems[++i].first;
        }

        if (lastItem == 0 || lastItem->data == 0) return;

        disconnect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));

        int n = tableWidget.rowCount();
        tableWidget.insertRow(n);
        tableWidget.setVerticalHeaderItem(n, new QTableWidgetItem(lastItem->fullName()));
        if (type == both || type == numerical)
        {
            if (lastItem->hasNumericalData(this->name))
            {
                DataTable<qreal> nDat(lastItem->data->numericalData[this->name]);
                QStringList rownames(win->symbolsTable.dataRowsAndCols.keys());
                i = 0;
                name = tr("k0");
                while (rownames.contains(lastItem->fullName() + tr(".") + name))
                    name = tr("k") + QString::number(++i);
                tableWidget.setItem(n,1,new QTableWidgetItem(tr("1.0")));
                tableWidget.setItem(n,0,new QTableWidgetItem(name));
                tableItems << QPair<ItemHandle*,int>(lastItem,rownames.size());

                nDat.resize(nDat.rows()+1,nDat.cols());
                nDat.value(rownames.size(),0) = 1.0;
                nDat.rowName(rownames.size()) = name;

                win->changeData(lastItem,this->name,&nDat);
            }
        }

        if (type == both || type == text)
        {
            if (lastItem->hasTextData(this->name))
            {
                DataTable<QString> sDat(lastItem->data->textData[this->name]);
                QStringList rownames(win->symbolsTable.dataRowsAndCols.keys());
                i = 0;
                name = tr("s0");
                while (rownames.contains(lastItem->fullName() + tr(".") + name))
                    name = tr("s") + QString::number(++i);
                tableWidget.setItem(n,1,new QTableWidgetItem(tr("1.0")));
                tableWidget.setItem(n,0,new QTableWidgetItem(name));
                tableItems << QPair<ItemHandle*,int>(lastItem,rownames.size());

                sDat.resize(sDat.rows()+1,sDat.cols());
                sDat.value(rownames.size(),0) = tr("hello world");
                sDat.rowName(rownames.size()) = name;

                win->changeData(lastItem,this->name,&sDat);
            }
        }

        connect(&tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(setValue(int,int)));
    }

    void BasicInformationTool::removeSelectedAttributes()
    {
        QList<QTableWidgetItem*> selectedItems = tableWidget.selectedItems();
        NetworkWindow * win = currentWindow();
        if (!win) return;

        QList<ItemHandle*> handles1, handles2;
        QList<DataTable<qreal>*> nDats;
        QList<DataTable<QString>*> sDats;

        for (int i=0; i < selectedItems.size(); ++i)
        {
            int row = selectedItems[i]->row();

            if (row >= tableItems.size()) break;

            ItemHandle * handle = tableItems[row].first;
            int rowNumber = tableItems[row].second;

            if (!handle || !handle->data) continue;

            if ((type == both || type == numerical) && handle->hasNumericalData(this->name))
            {
                DataTable<qreal> * nDat = new DataTable<qreal>(handle->data->numericalData[this->name]);

                if (rowNumber < nDat->rows())
                {
                    if (handle->family() && handle->family()->numericalAttributes.contains(nDat->rowName(rowNumber)))
                    {
                        OutputWindow::message(nDat->rowName(rowNumber) + tr(" cannot be removed because it is a family attribute"));
                    }
                    else
                    {
                        nDat->removeRow(rowNumber);
                        nDats << nDat;
                        handles1 << handle;
                    }
                }
            }

            if ((type == both || type == text) && handle->hasTextData(this->name))
            {
                DataTable<QString> * sDat = new DataTable<QString>(handle->data->textData[this->name]);

                if (rowNumber < sDat->rows())
                {
                    if (handle->family() && handle->family()->textAttributes.contains(sDat->rowName(rowNumber)))
                    {
                        OutputWindow::message(sDat->rowName(rowNumber) + tr(" cannot be removed because it is a family attribute"));
                    }
                    else
                    {
                        sDat->removeRow(rowNumber);
                        sDats << sDat;
                        handles2 << handle;
                    }
                }
            }
        }

        if (nDats.size() > 0)
            win->changeData(handles1,this->name,nDats);

        if (sDats.size() > 0)
            win->changeData(handles2,this->name,sDats);

        for (int i=0; i < nDats.size(); ++i)
            delete nDats[i];

        for (int i=0; i < sDats.size(); ++i)
            delete sDats[i];
    }

    typedef void (*tc_BasicInformationTool_Text_api)(
            char* (*getTextData)(OBJ ,const char* ),
            char** (*getAllTextDataNamed)(Array, char**),
            void (*setTextData)(OBJ ,const char* ,const char* ));

    typedef void (*tc_BasicInformationTool_Numeric_api)(
            Matrix (*getInitialValues)(Array ),
            Matrix (*getParameters)(Array ),
            Matrix (*getFixedVars)(Array),
            Matrix (*getFixedAndParameters)(Array),
            double (*getNumericalData)(OBJ ,const char* ),
            Matrix (*getParametersNamed)(Array, char**),
            Matrix (*getParametersExcept)(Array, char**),
            void (*setNumericalData)(OBJ ,const char* ,double ));

    void BasicInformationTool::setupFunctionPointers( QLibrary * library )
    {
        if (type == both || type == numerical)
        {
            tc_BasicInformationTool_Numeric_api f = (tc_BasicInformationTool_Numeric_api)library->resolve("tc_BasicInformationTool_Numeric_api");
            if (f)
            {
                //qDebug() << "tc_BasicInformationTool_Numeric_api resolved";
                f(
                        &(_getInitialValues),
                        &(_getParameters),
                        &(_getFixedVars),
                        &(_getFixedAndParameters),
                        &(_getNumericalData),
                        &(_getParametersNamed),
                        &(_getParametersExcept),
                        &(_setNumericalData)
                        );
            }
        }
        if (type == both || type == text)
        {
            tc_BasicInformationTool_Text_api f = (tc_BasicInformationTool_Text_api)library->resolve("tc_BasicInformationTool_Text_api");
            if (f)
            {
                //qDebug() << "tc_BasicInformationTool_Text_api resolved";
                f(
                        &(_getTextData),
                        &(_getAllTextDataNamed),
                        &(_setTextData)
                        );
            }
        }
    }

    void BasicInformationTool::connectTCFunctions()
    {
        if (type == both || type == text)
        {
            connect(&fToS,SIGNAL(getTextData(QSemaphore*,QString*,ItemHandle*,const QString&)),this,SLOT(getTextData(QSemaphore*,QString*,ItemHandle*,const QString&)));
            connect(&fToS,SIGNAL(setTextData(QSemaphore*,ItemHandle*,const QString&,const QString&)),this,SLOT(setTextData(QSemaphore*,ItemHandle*,const QString&,const QString&)));
            connect(&fToS,SIGNAL(getAllTextDataNamed(QSemaphore*,QStringList*,const QList<ItemHandle*>&,const QStringList&)),this,SLOT(getAllTextDataNamed(QSemaphore*,QStringList*,const QList<ItemHandle*>&,const QStringList&)));
        }
        if (type == both || type == numerical)
        {
            connect(&fToS,SIGNAL(getInitialValues(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&)),this,SLOT(getInitialValues(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&)));
            connect(&fToS,SIGNAL(getParameters(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&)),this,SLOT(getParameters(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&)));
            connect(&fToS,SIGNAL(getFixedVars(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&)),this,SLOT(getFixedVars(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&)));
            connect(&fToS,SIGNAL(getFixedAndParameters(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&)),this,SLOT(getFixedAndParameters(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&)));
            connect(&fToS,SIGNAL(getNumericalData(QSemaphore*,qreal*,ItemHandle*,const QString&)),this,SLOT(getNumericalData(QSemaphore*,qreal*,ItemHandle*,const QString&)));
            connect(&fToS,SIGNAL(setNumericalData(QSemaphore*,ItemHandle*,const QString&,qreal)),this,SLOT(setNumericalData(QSemaphore*,ItemHandle*,const QString&,qreal)));
            connect(&fToS,SIGNAL(getParametersNamed(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&,const QStringList&)),this,SLOT(getParametersNamed(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&,const QStringList&)));
            connect(&fToS,SIGNAL(getParametersExcept(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&,const QStringList&)),this,SLOT(getParametersExcept(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&,const QStringList&)));
        }

    }

    DataTable<qreal> BasicInformationTool::getParameters(const QList<QGraphicsItem*>& items, const QStringList& mustHave, const QStringList& exclude, const QString& sep)
    {
        QList<ItemHandle*> handles;

        ItemHandle* handle = 0;

        for (int i=0; i < items.size(); ++i)
        {
            handle = getHandle(items.at(i));
            if (handle && !handles.contains(handle))
                handles += handle;
        }

        return getParameters(handles,mustHave,exclude,sep);
    }

    DataTable<qreal> BasicInformationTool::getParameters(const QList<ItemHandle*>& handles, const QStringList& mustHave, const QStringList& exclude, const QString& sep)
    {
        QStringList rownames;
        QList<qreal> values;

        QString name("Numerical Attributes");

        ItemHandle * handle = 0;
        DataTable<qreal> * dataTable = 0;

        for (int i=0; i < handles.size(); ++i)
        {
            handle = handles.at(i);
            if (handle && handle->data && handle->hasNumericalData(name))
            {
                dataTable = &(handle->data->numericalData[name]);
                if (dataTable && dataTable->cols() > 0)
                    for (int j=0; j < dataTable->rows(); ++j)
                    {
                    if ((mustHave.isEmpty() || mustHave.contains(dataTable->rowName(j).toLower()) || mustHave.contains(dataTable->rowName(j)))
                        && (exclude.isEmpty() || !(exclude.contains(dataTable->rowName(j).toLower()) || exclude.contains(dataTable->rowName(j))))
                        && !(handle->type == ConnectionHandle::Type && (dataTable->rowName(j) == QString("numin") || dataTable->rowName(j) == QString("numout")))
                        )
                        {
                        rownames += handle->fullName(sep) + sep + dataTable->rowName(j);
                        values += dataTable->at(j,0);
                    }
                }
            }
        }

        DataTable<qreal> combinedTable;
        combinedTable.resize(values.size(),1);
        for (int i=0; i < values.size() && i < rownames.size(); ++i)
        {
            combinedTable.rowName(i) = rownames[i];
            combinedTable.value(i,0) = values[i];
        }

        return combinedTable;
    }

    DataTable<QString> BasicInformationTool::getTextData(const QList<ItemHandle*>& handles, const QStringList& mustHave, const QStringList& exclude, const QString& sep)
    {
        QStringList rownames;
        QList<QString> values;

        QString name("Text Attributes");

        ItemHandle * handle = 0;
        DataTable<QString> * dataTable = 0;

        for (int i=0; i < handles.size(); ++i)
        {
            handle = handles.at(i);

            if (handle && handle->data && handle->hasTextData(name))
            {				dataTable = &(handle->data->textData[name]);
                if (dataTable && dataTable->cols() > 0)
                    for (int j=0; j < dataTable->rows(); ++j)
                    {
                    if ((mustHave.isEmpty() || mustHave.contains(dataTable->rowName(j).toLower()) || mustHave.contains(dataTable->rowName(j)))
                        && (exclude.isEmpty() || !(exclude.contains(dataTable->rowName(j).toLower()) || exclude.contains(dataTable->rowName(j))))
                        && !(handle->type == ConnectionHandle::Type && (dataTable->rowName(j) == QString("numin") || dataTable->rowName(j) == QString("numout")))
                        )
                        {
                        rownames += handle->fullName(sep) + sep + dataTable->rowName(j);
                        values += dataTable->at(j,0);
                    }
                }
            }
        }

        QRegExp regex(tr("\\.(?!\\d)"));
        for (int i=0; i < values.size(); ++i)
        {
            while (regex.indexIn(values[i]) != -1)
            {
                values[i].replace(regex,sep);
            }
        }

        DataTable<QString> combinedTable;
        combinedTable.resize(values.size(),1);
        for (int i=0; i < values.size() && i < rownames.size(); ++i)
        {
            combinedTable.rowName(i) = rownames[i];
            combinedTable.value(i,0) = values[i];
        }

        return combinedTable;
    }

    void BasicInformationTool::getInitialValues(QSemaphore* s,DataTable<qreal>* ptr,const QList<ItemHandle*>& handles)
    {
        if (ptr)
        {
            ItemHandle * handle = 0;
            DataTable<qreal> * dataTable = 0;

            QStringList names;
            QList<qreal> values;

            for (int i=0; i < handles.size(); ++i)
            {
                handle = handles.at(i);
                if (handle && handle->data && handle->hasNumericalData(tr("Initial Value")))
                {
                    if (handle->hasNumericalData(tr("Fixed")) && handle->data->numericalData[tr("Fixed")].value(0,0) > 0)
                        continue;

                    dataTable = &(handle->data->numericalData[tr("Initial Value")]);
                    if (dataTable && dataTable->cols() > 0 && dataTable->rows() > 0)
                    {
                        names << handle->fullName(tr("_"));
                        values << dataTable->value(0,0);
                    }
                }
            }

            int rows = (*ptr).rows();
            (*ptr).resize( rows + names.size(), (*ptr).cols() );

            for (int i=0; i < names.size(); ++i)
            {
                (*ptr).rowName(rows + i) = names[i];
                (*ptr).value(rows + i, 0) = values[i];
            }

        }
        if (s)
            s->release();
    }

    void BasicInformationTool::getFixedVars(QSemaphore* s,DataTable<qreal>* ptr,const QList<ItemHandle*>& handles)
    {
        if (ptr)
        {
            ItemHandle * handle = 0;
            DataTable<qreal> * dataTable = 0;

            QStringList names;
            QList<qreal> values;

            for (int i=0; i < handles.size(); ++i)
            {
                handle = handles.at(i);
                if (handle && handle->data && handle->hasNumericalData(tr("Fixed")))
                {
                    dataTable = &(handle->data->numericalData[tr("Fixed")]);
                    if (dataTable && dataTable->cols() > 0 && dataTable->rows() > 0 && (dataTable->value(0,0) > 0))
                    {
                        names << handle->fullName(tr("_"));
                        values << dataTable->value(0,0);
                    }
                }
            }

            int rows = (*ptr).rows();
            (*ptr).resize( rows + names.size(), (*ptr).cols() );

            for (int i=0; i < names.size(); ++i)
            {
                (*ptr).rowName(rows + i) = names[i];
                (*ptr).value(rows + i, 0) = values[i];
            }

        }
        if (s)
            s->release();
    }

    void BasicInformationTool::getFixedAndParameters(QSemaphore* s,DataTable<qreal>* ptr,const QList<ItemHandle*>& handles)
    {
        if (ptr)
        {
            //(*ptr) = getParameters(handles);

            int i,j;
            QString replaceDot("_");
            QStringList rates = StoichiometryTool::getRates(handles, replaceDot);
            DataTable<qreal> params = BasicInformationTool::getParameters(handles,QStringList(), QStringList(), replaceDot);
            params.insertCol(1,tr("used"));

            bool used = false;
            for (i=0; i < params.rows(); ++i)
            {
                used = false;
                for (j=0; j < rates.size(); ++j)
                {
                    if (rates[j].contains(params.rowName(i)))
                    {
                        used = true;
                        break;
                    }
                }
                if (used)
                    params.value(i,1) = 1.0;
                else
                    params.value(i,1) = 0.0;
            }

            QRegExp regex(tr("\\.(?!\\d)"));
            QString s1,s2;

            for (i=0; i < handles.size(); ++i)
            {
                if (!handles[i])
                    continue;

                if (handles[i]->hasTextData(tr("Events")))
                {
                    DataTable<QString>& dat = handles[i]->data->textData[tr("Events")];
                    if (dat.cols() == 1)
                        for (j=0; j < dat.rows(); ++j)
                        {
                        s1 =  dat.rowName(j);
                        s1.replace(regex,replaceDot);

                        s2 =  dat.value(j,0);

                        s2.replace(regex,replaceDot);

                        if (s1.isEmpty() || s2.isEmpty()) continue;

                        for (int k=0; k < params.rows(); ++k)
                            if (s2.contains(params.rowName(k)) || s1.contains(params.rowName(k)))
                                params.value(k,1) = 1.0;
                    }
                }

                if (handles[i]->hasTextData(tr("Assignments")))
                {
                    DataTable<QString>& dat = handles[i]->data->textData[tr("Assignments")];
                    if (dat.cols() == 1)
                        for (j=0; j < dat.rows(); ++j)
                        {
                        s1 =  dat.rowName(j);
                        s1.replace(regex,replaceDot);
                        s2 =  dat.value(j,0);
                        s2.replace(regex,replaceDot);

                        if (s1.isEmpty() || s2.isEmpty()) continue;

                        for (int k=0; k < params.rows(); ++k)
                            if (s2.contains(params.rowName(k)) || s1.contains(params.rowName(k)))
                                params.value(k,1) = 1.0;
                    }
                }

                if (handles[i]->hasTextData(tr("Functions")))
                {
                    DataTable<QString>& dat = handles[i]->data->textData[tr("Functions")];
                    if (dat.cols() == 1)
                        for (j=0; j < dat.rows(); ++j)
                        {
                        s1 =  dat.rowName(j);
                        s1.replace(regex,replaceDot);
                        s2 =  dat.value(j,0);
                        s2.replace(regex,replaceDot);

                        if (s1.isEmpty() || s2.isEmpty()) continue;

                        for (int k=0; k < params.rows(); ++k)
                            if (s2.contains(params.rowName(k)) || s1.contains(params.rowName(k)))
                                params.value(k,1) = 1.0;
                    }
                }
            }

            int count = 0;
            for (int i=0; i < params.rows(); ++i)
            {
                if (params.value(i,1) > 0.0) ++count;
            }

            DataTable<qreal> params2;
            params2.resize(count,1);
            params2.colName(0) = params.colName(0);

            for (int i=0, j=0; i < params.rows() && j < count; ++i)
            {
                if (params.value(i,1) > 0.0)
                {
                    params2.rowName(j) = params.rowName(i);
                    params2.value(j,0) = params.value(i,0);
                    ++j;
                }
            }

            ItemHandle * handle = 0;
            DataTable<qreal> * dataTable = 0;

            QStringList names;
            QList<qreal> values;

            for (int i=0; i < handles.size(); ++i)
            {
                handle = handles.at(i);
                if (handle && handle->data && handle->hasNumericalData(tr("Fixed")))
                {
                    dataTable = &(handle->data->numericalData[tr("Fixed")]);
                    if (dataTable && dataTable->cols() > 0 && dataTable->rows() > 0 && (dataTable->value(0,0) > 0))
                    {
                        names << handle->fullName(tr("_"));
                        values << dataTable->value(0,0);
                    }
                }
            }

            int rows = params2.rows();
            params2.resize( rows + names.size(), (*ptr).cols() );

            for (int i=0; i < names.size(); ++i)
            {
                params2.rowName(rows + i) = names[i];
                params2.value(rows + i, 0) = values[i];
            }

            (*ptr) = params2;
        }
        if (s)
            s->release();
    }

    void BasicInformationTool::getParameters(QSemaphore* s,DataTable<qreal>* ptr,const QList<ItemHandle*>& list)
    {
        if (ptr)
        {
            (*ptr) = getParameters(list);
        }
        if (s)
            s->release();
    }

    void BasicInformationTool::getParametersNamed(QSemaphore* s,DataTable<qreal>* ptr,const QList<ItemHandle*>& list,const QStringList& text)
    {
        if (ptr)
        {
            (*ptr) = getParameters(list,text);
        }
        if (s)
            s->release();
    }

    void BasicInformationTool::getParametersExcept(QSemaphore* s,DataTable<qreal>* ptr,const QList<ItemHandle*>& list,const QStringList& text)
    {
        if (ptr)
        {
            (*ptr) = getParameters(list,QStringList(),text);
        }
        if (s)
            s->release();
    }

    void BasicInformationTool::getAllTextDataNamed(QSemaphore* s,QStringList* ptr,const QList<ItemHandle*>& list,const QStringList& text)
    {
        if (ptr)
        {
            DataTable<QString> dat = getTextData(list,text);
            if (dat.cols() > 0)
            {
                for (int i=0; i < dat.rows(); ++i)
                    (*ptr) << dat.at(i,0);
            }
        }
        if (s)
            s->release();
    }

    void BasicInformationTool::getTextData(QSemaphore* s,QString* ptr,ItemHandle* handle,const QString& text)
    {
        if (ptr)
        {
            if (handle && handle->data && handle->hasTextData(this->name))
            {
                const QVector<QString>& rownames = handle->data->textData[name].rowNames();
                for (int i=0; i < rownames.size(); ++i)
                    if (rownames.at(i).toLower() == text.toLower())
                        (*ptr) = handle->data->textData[name].at(i,0);
                QRegExp regex(tr("\\.(?!\\d)"));
                (*ptr).replace(regex,tr("_"));
            }
            else
            {
                (*ptr) = tr("");
            }
        }
        if (s)
            s->release();
    }
    void BasicInformationTool::getNumericalData(QSemaphore* s,qreal* ptr,ItemHandle* handle,const QString& text)
    {
        if (ptr)
        {
            if (handle && handle->data && handle->hasNumericalData(name))
            {
                (*ptr) = 0.0;
                const QVector<QString>& rownames = handle->data->numericalData[name].rowNames();
                for (int i=0; i < rownames.size(); ++i)
                    if (rownames.at(i).toLower() == text.toLower())
                        (*ptr) = handle->data->numericalData[name].at(i,0);
            }
            else
            {
                (*ptr) = 0.0;
            }
        }
        if (s)
            s->release();
    }
    void BasicInformationTool::setTextData(QSemaphore* s,ItemHandle* handle,const QString& text,const QString& value)
    {
        if (handle)
        {
            if (handle->data && handle->hasTextData(name))
            {
                GraphicsScene * scene = mainWindow->currentScene();
                if (scene)
                {
                    DataTable<QString> * newData = new DataTable<QString>(handle->data->textData[name]);
                    bool contains = false;

                    const QVector<QString>& rownames = handle->data->textData[name].rowNames();
                    for (int i=0; i < rownames.size(); ++i)
                        if (rownames.at(i).toLower() == text.toLower())
                        {
                        newData->value(i,0) = value;
                        contains = true;
                    }
                    if (!contains)
                    {
                        newData->insertRow(rownames.size(),text);
                        newData->value(rownames.size(),0) = value;
                    }

                    scene->changeData(handle,name,newData);
                    delete newData;
                }
                else
                {
                    handle->data->textData[name].value(text,0) = value;
                }
            }
        }
        if (s)
            s->release();
    }

    void BasicInformationTool::setNumericalData(QSemaphore* s,ItemHandle* handle,const QString& text,qreal value)
    {
        if (handle)
        {
            if (handle->data && handle->hasNumericalData(name))
            {
                NetworkWindow * win = mainWindow->currentWindow();
                if (win)
                {
                    DataTable<qreal> * newData = new DataTable<qreal>(handle->data->numericalData[name]);

                    bool contains = false;

                    const QVector<QString>& rownames = handle->data->numericalData[name].rowNames();
                    for (int i=0; i < rownames.size(); ++i)
                        if (rownames.at(i).toLower() == text.toLower())
                        {
                        newData->value(i,0) = value;
                        contains = true;
                    }
                    if (!contains)
                    {
                        newData->insertRow(rownames.size(),text);
                        newData->value(rownames.size(),0) = value;
                    }

                    win->changeData(handle,this->name,newData);
                    delete newData;
                }
                else
                {
                    handle->data->numericalData[name].value(text,0) = value;
                }
            }
        }
        if (s)
            s->release();
    }

    /********************************************************/

    BasicInformationTool_FToS BasicInformationTool::fToS;

    Matrix BasicInformationTool::_getParameters(Array A)
    {
        return fToS.getParameters(A);
    }

    Matrix BasicInformationTool::_getInitialValues(Array A)
    {
        return fToS.getInitialValues(A);
    }

    Matrix BasicInformationTool::_getFixedVars(Array A)
    {
        return fToS.getFixedVars(A);
    }

    Matrix BasicInformationTool::_getFixedAndParameters(Array A)
    {
        return fToS.getFixedAndParameters(A);
    }

    char* BasicInformationTool::_getTextData(OBJ o,const char* c)
    {
        return fToS.getTextData(o,c);
    }

    double BasicInformationTool::_getNumericalData(OBJ o,const char* c)
    {
        return fToS.getNumericalData(o,c);
    }

    Matrix BasicInformationTool::_getParametersNamed(Array A, char** c)
    {
        return fToS.getParametersNamed(A,c);
    }

    Matrix BasicInformationTool::_getParametersExcept(Array A, char** c)
    {
        return fToS.getParametersExcept(A,c);
    }

    char** BasicInformationTool::_getAllTextDataNamed(Array A, char** c)
    {
        return fToS.getAllTextDataNamed(A,c);
    }

    void BasicInformationTool::_setTextData(OBJ o,const char* a,const char* b)
    {
        return fToS.setTextData(o,a,b);
    }

    void BasicInformationTool::_setNumericalData(OBJ o,const char* a,double b)
    {
        return fToS.setNumericalData(o,a,b);
    }

    Matrix BasicInformationTool_FToS::getParameters(Array a0)
    {
        QSemaphore * s = new QSemaphore(1);
        QList<ItemHandle*> * list = ConvertValue(a0);
        DataTable<qreal> * p = new DataTable<qreal>;
        s->acquire();
        emit getParameters(s,p,*list);
        s->acquire();
        s->release();
        delete s;
        delete list;
        if (p)
        {
            Matrix m = ConvertValue(*p);
            delete p;
            return m;
        }
        return emptyMatrix();
    }

    Matrix BasicInformationTool_FToS::getInitialValues(Array a0)
    {
        QSemaphore * s = new QSemaphore(1);
        QList<ItemHandle*> * list = ConvertValue(a0);
        DataTable<qreal> * p = new DataTable<qreal>;
        s->acquire();
        emit getInitialValues(s,p,*list);
        s->acquire();
        s->release();
        delete s;
        delete list;
        if (p)
        {
            Matrix m = ConvertValue(*p);
            delete p;
            return m;
        }
        return emptyMatrix();
    }

    Matrix BasicInformationTool_FToS::getFixedVars(Array a0)
    {
        QSemaphore * s = new QSemaphore(1);
        QList<ItemHandle*> * list = ConvertValue(a0);
        DataTable<qreal> * p = new DataTable<qreal>;
        s->acquire();
        emit getFixedVars(s,p,*list);
        s->acquire();
        s->release();
        delete s;
        delete list;
        if (p)
        {
            Matrix m = ConvertValue(*p);
            delete p;
            return m;
        }
        return emptyMatrix();
    }

    Matrix BasicInformationTool_FToS::getFixedAndParameters(Array a0)
    {
        QSemaphore * s = new QSemaphore(1);
        QList<ItemHandle*> * list = ConvertValue(a0);
        DataTable<qreal> * p = new DataTable<qreal>;
        s->acquire();
        emit getFixedAndParameters(s,p,*list);
        s->acquire();
        s->release();
        delete s;
        delete list;
        if (p)
        {
            Matrix m = ConvertValue(*p);
            delete p;
            return m;
        }
        return emptyMatrix();
    }

    Matrix BasicInformationTool_FToS::getParametersNamed(Array a0,char** name)
    {
        QList<ItemHandle*> * list = ConvertValue(a0);
        QSemaphore * s = new QSemaphore(1);
        DataTable<qreal> * p = new DataTable<qreal>;
        s->acquire();
        emit getParametersNamed(s,p,*list,ConvertValue(name));
        s->acquire();
        s->release();
        delete s;
        delete list;
        if (p)
        {
            Matrix m = ConvertValue(*p);
            delete p;
            return m;
        }
        return emptyMatrix();
    }

    Matrix BasicInformationTool_FToS::getParametersExcept(Array a0,char** name)
    {
        QList<ItemHandle*> * list = ConvertValue(a0);
        QSemaphore * s = new QSemaphore(1);
        DataTable<qreal> * p = new DataTable<qreal>;
        s->acquire();
        emit getParametersExcept(s,p,*list,ConvertValue(name));
        s->acquire();
        s->release();
        delete s;
        delete list;
        if (p)
        {
            Matrix m = ConvertValue(*p);
            delete p;
            return m;
        }
        return emptyMatrix();
    }

    char** BasicInformationTool_FToS::getAllTextDataNamed(Array a0,char** name)
    {
        QList<ItemHandle*> * list = ConvertValue(a0);
        QSemaphore * s = new QSemaphore(1);
        QStringList p;
        s->acquire();
        emit getAllTextDataNamed(s,&p,*list,ConvertValue(name));
        s->acquire();
        s->release();
        delete s;
        delete list;
        //qDebug() << "calling convert value";
        return ConvertValue(p);
    }

    char* BasicInformationTool_FToS::getTextData(OBJ a0,const char* a1)
    {
        QSemaphore * s = new QSemaphore(1);
        QString p;
        s->acquire();
        emit getTextData(s,&p,ConvertValue(a0),ConvertValue(a1));
        s->acquire();
        s->release();
        delete s;
        return (char*)ConvertValue(p);
    }

    double BasicInformationTool_FToS::getNumericalData(OBJ a0,const char* a1)
    {
        QSemaphore * s = new QSemaphore(1);
        qreal p;
        s->acquire();
        emit getNumericalData(s,&p,ConvertValue(a0),ConvertValue(a1));
        s->acquire();
        s->release();
        delete s;
        return (double)p;
    }

    void BasicInformationTool_FToS::setTextData(OBJ a0,const char* a1,const char* a2)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit setTextData(s,ConvertValue(a0),ConvertValue(a1),ConvertValue(a2));
        s->acquire();
        s->release();
        delete s;
    }

    void BasicInformationTool_FToS::setNumericalData(OBJ a0,const char* a1,double a2)
    {
        QSemaphore * s = new QSemaphore(1);
        s->acquire();
        emit setNumericalData(s,ConvertValue(a0),a1,a2);
        s->acquire();
        s->release();
        delete s;
    }

    void BasicInformationTool::keyPressEvent ( QKeyEvent * event )
    {
        if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
        {
            removeSelectedAttributes();
        }
    }
}
