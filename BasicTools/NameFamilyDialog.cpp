/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool displays a dialog with the name and family information of selected items.
An associated GraphicsTool is also defined.

****************************************************************************/

#include "MainWindow.h"
#include "NetworkWindow.h"
#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "CThread.h"
#include "OutputWindow.h"
#include "NameFamilyDialog.h"

namespace Tinkercell
{
	NameFamilyDialog::NameFamilyDialog() : Tool(tr("Name and Family Dialog"))
	{
		selectedItem = 0;
		connectTCFunctions();

		QString appDir = QCoreApplication::applicationDirPath();
		NodeGraphicsReader reader;
		reader.readXml(&idcard,appDir + tr("/OtherItems/idcard.xml"));
		idcard.normalize();
		idcard.scale(40.0/idcard.sceneBoundingRect().width(),30.0/idcard.sceneBoundingRect().height());

		graphicsItems += new GraphicsItem(this);
		graphicsItems[0]->addToGroup(&idcard);
		graphicsItems[0]->setToolTip(tr("Name, family, and Annotation"));

		/*QToolButton * toolButton = new QToolButton(this);
		toolButton->setIcon(QIcon(appDir + tr("/BasicTools/monitor.PNG")));
		toolButton->setToolTip(tr("Name, family, and Annotation"));
		this->buttons.addButton(toolButton);*/

	}
	void NameFamilyDialog::makeDialog(QWidget* parent)
	{
		dialog = new QDialog(parent);

		QGridLayout * layout = new QGridLayout;
		QPushButton * okButton = new QPushButton("OK");
		connect(okButton,SIGNAL(released()),dialog,SLOT(accept()));
		QPushButton * cancelButton = new QPushButton("Cancel");
		connect(cancelButton,SIGNAL(released()),dialog,SLOT(reject()));
		QLabel * label1 = new QLabel(tr("Name :"));
		QLabel * label2 = new QLabel(tr("Family : "));
		QLabel * label3 = new QLabel(tr("Author(s) :"));
		QLabel * label4 = new QLabel(tr("Date : "));
		QLabel * label5 = new QLabel(tr("Description : "));

		layout->addWidget(label1,0,0);
		layout->addWidget(label2,1,0);
		layout->addWidget(label3,2,0);
		layout->addWidget(label4,3,0);
		layout->addWidget(label5,4,0);

		for (int i=0; i < 5; ++i)
		{
			QLineEdit * edit = new QLineEdit(tr(""));
			edit->setFixedHeight(20);
			connect(edit,SIGNAL(returnPressed()),dialog,SLOT(accept()));
			lineEdits << edit;
			layout->addWidget(edit,i,1);
		}

		QHBoxLayout * buttonsLayout = new QHBoxLayout;
		buttonsLayout->addWidget(okButton);
		buttonsLayout->addWidget(cancelButton);
		layout->addLayout(buttonsLayout,5,1,Qt::AlignRight);

		dialog->setWindowTitle(tr("Information Box"));
		layout->setColumnStretch(0,0);
		layout->setColumnStretch(1,1);
		dialog->setLayout(layout);
		dialog->setSizeGripEnabled(true);
		connect(dialog,SIGNAL(accepted()),this,SLOT(dialogFinished()));
	}

	bool NameFamilyDialog::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow)
		{
			makeDialog(mainWindow);

			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			connect(mainWindow,SIGNAL(itemsInserted(NetworkWindow*, const QList<ItemHandle*>&)),
				this, SLOT(itemsInsertedSlot(NetworkWindow*, const QList<ItemHandle*>&)));
			connect(this,SIGNAL(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)),
				mainWindow,SIGNAL(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)));
			connect(this,SIGNAL(itemsAboutToBeInserted(GraphicsScene*, QList<QGraphicsItem *>&, QList<ItemHandle*>&)),
				mainWindow,SIGNAL(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&)));

			//connect(this,SIGNAL(dataChanged(QList<ItemHandle*>&)),mainWindow,SIGNAL(dataChanged(QList<ItemHandle*>&)));
		}
		return (mainWindow != 0);
	}

	void NameFamilyDialog::itemsInsertedSlot(NetworkWindow * scene, const QList<ItemHandle*>& handles)
	{
		if (!scene || handles.isEmpty()) return;
		for (int i=0; i < handles.size(); ++i)
			if (handles[i])
			{
				if (handles[i]->data && !handles[i]->hasTextData(tr("Annotation")))
				{
					DataTable<QString> data;
					data.resize(5,1);
					data.colName(0) = tr("entry");
					data.setRowNames( QStringList() << tr("author") << tr("date") << tr("description") << tr("uri") << tr("reference") );
					for (int j=0; j < 5; ++j)
						data.value(j,0) = data.rowName(j);

					handles[i]->data->textData[tr("Annotation")] = data;
				}
				if (!handles[i]->tools.contains(this))
					handles[i]->tools += this;

				if (handles[i]->family())
				{
					for (int j=0; j < handles[i]->graphicsItems.size(); ++j)
						handles[i]->graphicsItems[j]->setToolTip(handles[i]->family()->name);
				}
			}
	}

	void NameFamilyDialog::closeDialog()
	{
		if (dialog)
		{
			dialog->accept();
		}
	}

	void NameFamilyDialog::showDialog(ItemHandle* handle)
	{
		if (!handle) return;

		if (!dialog)
		{
			OutputWindow::error(tr("Tool not initialized."));
			return;
		}

		if (dialog->isVisible())
			dialog->reject();


		if (handle && handle->family())
		{
			if (lineEdits.size() > 4 && 
				handle->data && handle->hasTextData(tr("Annotation")))
			{
				DataTable<QString> data = handle->data->textData[tr("Annotation")];
				if (lineEdits[2]) lineEdits[2]->setText(data.value(0,0));
				if (lineEdits[3]) lineEdits[3]->setText(data.value(1,0));
				if (lineEdits[4]) lineEdits[4]->setText(data.value(2,0));
			}

		}

		if (handle && handle->family() && lineEdits.size() > 1 && lineEdits[0] && lineEdits[1])
		{
			lineEdits[0]->setText(handle->name);
			lineEdits[1]->setText(handle->family()->name);
			dialog->show();
		}
	}

	void NameFamilyDialog::dialogFinished()
	{
		if (!selectedItem || lineEdits.size() < 5 || !mainWindow || !mainWindow->currentWindow()) return;

		for (int i=0; i < lineEdits.size(); ++i)
			if (!lineEdits[i])
				return;

		NetworkWindow * win = mainWindow->currentWindow();

		QString name = lineEdits[0]->text(),
			family = lineEdits[1]->text(),
			authors = lineEdits[2]->text(),
			date = lineEdits[3]->text(),
			desc = lineEdits[4]->text();

		bool containsConnections = false;

		ItemHandle * handle = selectedItem;
		if (handle)
		{
			if (!name.isEmpty() && handle->name != name)
				win->rename(handle,name);

			if (handle->data && handle->hasTextData(tr("Annotation")))
			{
				DataTable<QString> data = handle->data->textData[tr("Annotation")];
				if (data.value(0,0) != authors ||
					data.value(1,0) != date ||
					data.value(2,0) != desc)
				{
					data.value(0,0) = authors;
					data.value(1,0) = date;
					data.value(2,0) = desc;
					win->changeData(handle,tr("Annotation"),&data);
				}
			}
		}

		containsConnections = (ConnectionHandle::asConnection(selectedItem) != 0);

		if (family.isEmpty()) return;

		if (!mainWindow->tool("Nodes Tree"))
		{
			OutputWindow::error(tr("No nodes family tree available."));
			return;
		}

		NodesTree * nodesTree = static_cast<NodesTree*>(mainWindow->tool("Nodes Tree"));
		if (!(nodesTree && nodesTree->nodeFamilies.contains(family)))
		{
			OutputWindow::error(tr("Family name does not match any node family name."));
			return;
		}

		QList<QGraphicsItem*> newitems;
		QList<ItemHandle*> handles;

		QList<QUndoCommand*> commands;
		if (NodeHandle::asNode(selectedItem))
		{
			ItemHandle * handle = selectedItem;
			if (handle && handle->family() && handle->family()->name != family)
			{
				QString oldFamily = handle->family()->name;

				NodeHandle * node = NodeHandle::asNode(selectedItem);
				if (node && node == handle)
				{
					if (node->connections().isEmpty())
					{
						if (handle->type == NodeHandle::Type)
						{
							NodeHandle * handle2 = new NodeHandle(nodesTree->nodeFamilies[family]);
							handle2->name = handle->name;

							QGraphicsItem * q = 0;
							for (int j=0; j < handle->graphicsItems.size(); ++j)
							{
								if ((q = cloneGraphicsItem(handle->graphicsItems[j])))
								{
									newitems << q;
									setHandle(q,handle2);
								}
							}
							handles << handle2;
							ItemHandle * noHandle = 0;

							emit itemsAboutToBeInserted(win->scene,newitems,handles);

							commands << (new RemoveGraphicsCommand(tr(""),win->scene,handle->graphicsItems))
								<< (new InsertGraphicsCommand(tr(""),win->scene,newitems));
							if (handle->parent)
							{
								commands << (new SetParentHandleCommand(tr(""),win,handle,noHandle))
									<< (new SetParentHandleCommand(tr(""),win,handle2,handle->parent));
							}
						}
					}
					else
					{
						OutputWindow::error(tr("Cannot change family of connected items."));
						return;
					}
				}
				else
				{

				}
			}
		}
		if (!commands.isEmpty())
		{
			CompositeCommand * command = new CompositeCommand(tr("family changed"),commands);
			win->history.push(command);
			emit itemsInserted(win->scene,newitems,handles);
		}


		selectedItem = 0;

	}

	void NameFamilyDialog::select(int)
	{
		NetworkWindow * win = currentWindow();
		if (win->selectedHandles().size() != 1)
			OutputWindow::error(tr("please select one item"));
		else
		{
			showDialog(selectedItem = win->selectedHandles()[0]);
		}
	}

	void NameFamilyDialog::deselect(int)
	{
		if (parentWidget() != 0)
			parentWidget()->hide();
		else
		{
			closeDialog();
			hide();
		}
	}
	/*********************
	C API
	*********************/

	NameFamilyDialog_FtoS NameFamilyDialog::fToS;


	void NameFamilyDialog::getAnnotation(QSemaphore* sem, QStringList* list, ItemHandle* item)
	{
		if (item && list)
		{
			(*list).clear();

			if (item->data && item->hasTextData(tr("Annotation")))
			{
				DataTable<QString> data = item->data->textData[tr("Annotation")];

				//(*list) << data.getRowNames();

				for (int i=0; i < data.rows(); ++i)
					(*list) << data.value(i,0);
			}
		}
		if (sem)
			sem->release();
	}

	void NameFamilyDialog::setAnnotation(QSemaphore* sem, ItemHandle* item, const QStringList& list)
	{
		if (item && item->data)
		{
			DataTable<QString> data;

			if (item->hasTextData(tr("Annotation")))
				data = item->data->textData[tr("Annotation")];
			else
			{
				data.resize(5,1);
				data.setRowNames( QStringList() << tr("author") << tr("data") << tr("description") << tr("uri") << tr("reference") );
				item->data->textData[tr("Annotation")] = data;
			}

			for (int i=0; i < list.size() && i < data.rows(); ++i)
				data.value(i,0) = list[i];

			if (currentScene())
				currentScene()->changeData(item,tr("Annotation"),&data);
		}
		if (sem)
			sem->release();
		if (sem)
			sem->release();
	}

	char** NameFamilyDialog::_getAnnotation(OBJ o)
	{
		return fToS.getAnnotation(o);
	}

	void NameFamilyDialog::_setAnnotation(OBJ o, char ** a)
	{
		return fToS.setAnnotation(o,a);
	}

	char** NameFamilyDialog_FtoS::getAnnotation(OBJ o)
	{
		QSemaphore * s = new QSemaphore(1);
		QStringList p;
		s->acquire();
		emit getAnnotation(s,&p,ConvertValue(o));
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(p);
	}

	void NameFamilyDialog_FtoS::setAnnotation(OBJ o, char ** a)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit setAnnotation(s,ConvertValue(o),ConvertValue(a));
		s->acquire();
		s->release();
		delete s;
	}

	void NameFamilyDialog::connectTCFunctions()
	{

		connect(&fToS,SIGNAL(getAnnotation(QSemaphore*, QStringList*, ItemHandle*)),this,SLOT(getAnnotation(QSemaphore*, QStringList*, ItemHandle*)));
		connect(&fToS,SIGNAL(setAnnotation(QSemaphore*, ItemHandle*, const QStringList&)),this,SLOT(setAnnotation(QSemaphore*, ItemHandle*, const QStringList&)));

	}

	typedef void (*tc_NameFamily_api_func)(
		char** (*tc_getAnnotation)(OBJ),
		void (*tc_setAnnotation)(OBJ,char**)
		);

	void NameFamilyDialog::setupFunctionPointers(QLibrary * library)
	{
		tc_NameFamily_api_func f = (tc_NameFamily_api_func)library->resolve("tc_NameFamily_api_initialize");
		if (f)
		{
			f(
				&(_getAnnotation),
				&(_setAnnotation)
				);
		}
	}

}
