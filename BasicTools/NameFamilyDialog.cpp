/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool displays a dialog with the name and family information of selected items.
An associated GraphicsTool is also defined.

****************************************************************************/

#include "MainWindow.h"
#include "NetworkHandle.h"
#include "GraphicsScene.h"
#include "UndoCommands.h"
#include "NodeGraphicsItem.h"
#include "ConnectionGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "CThread.h"
#include "CodeEditor.h"
#include "ConsoleWindow.h"
#include "NameFamilyDialog.h"
#include <QRegExp>

namespace Tinkercell
{
	NameFamilyDialog::NameFamilyDialog() : Tool(tr("Name and Family Dialog"),tr("Basic GUI"))
	{
		selectedItem = 0;
		connectTCFunctions();

		QString appDir = QCoreApplication::applicationDirPath();
		NodeGraphicsReader reader;
		reader.readXml(&idcard,appDir + tr("/DecoratorItems/idcard.xml"));
		idcard.normalize();
		idcard.scale(40.0/idcard.sceneBoundingRect().width(),30.0/idcard.sceneBoundingRect().height());

		ToolGraphicsItem * toolGraphicsItem = new ToolGraphicsItem(this);
		addGraphicsItem(toolGraphicsItem);
		toolGraphicsItem->addToGroup(&idcard);
		toolGraphicsItem->setToolTip(tr("Name, family, and Annotation"));

		/*QToolButton * toolButton = new QToolButton(this);
		toolButton->setIcon(QIcon(appDir + tr("/BasicTools/monitor.PNG")));
		toolButton->setToolTip(tr("Name, family, and Annotation"));
		this->buttons.addButton(toolButton);*/

	}
	void NameFamilyDialog::makeDialog(QWidget* parent)
	{
		dialog = new QDialog(parent);

		QPushButton * okButton = new QPushButton("OK");
		connect(okButton,SIGNAL(released()),dialog,SLOT(accept()));
		QPushButton * cancelButton = new QPushButton("Cancel");
		connect(cancelButton,SIGNAL(released()),dialog,SLOT(reject()));
		/*QLabel * label1 = new QLabel(tr("Name :"));
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

		layout->setColumnStretch(0,0);
		layout->setColumnStretch(1,1);*/

		QVBoxLayout * layout = new QVBoxLayout;
		QHBoxLayout * layout2 = new QHBoxLayout;
		QHBoxLayout * layout3 = new QHBoxLayout;

		layout2->addWidget( textEdit = new CodeEditor );
		layout3->addWidget(okButton);
		layout3->addWidget(cancelButton);

		layout->addLayout(layout2);
		layout->addLayout(layout3);
		layout->setStretchFactor(layout2,1);
		layout->setStretchFactor(layout3,0);

		QFont font = textEdit->font();
		font.setPointSize(12);
		textEdit->setFont(font);

		dialog->setWindowTitle(tr("Information Box"));
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

			connect(mainWindow,SIGNAL(itemsInserted(NetworkHandle*, const QList<ItemHandle*>&)),
				this, SLOT(itemsInsertedSlot(NetworkHandle*, const QList<ItemHandle*>&)));
			connect(this,SIGNAL(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)),
				mainWindow,SIGNAL(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)));
			connect(this,SIGNAL(itemsAboutToBeInserted(GraphicsScene*, QList<QGraphicsItem *>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)),
				mainWindow,SIGNAL(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)));

			//connect(this,SIGNAL(dataChanged(QList<ItemHandle*>&)),mainWindow,SIGNAL(dataChanged(QList<ItemHandle*>&)));
		}
		return (mainWindow != 0);
	}

	void NameFamilyDialog::itemsInsertedSlot(NetworkHandle * scene, const QList<ItemHandle*>& handles)
	{
		if (!scene || handles.isEmpty()) return;
		for (int i=0; i < handles.size(); ++i)
			if (handles[i] && handles[i]->family())
			{
				if (!handles[i]->hasTextData(tr("Annotation")))
				{
					DataTable<QString> data;
					data.resize(5,1);
					data.colName(0) = tr("entry");
					data.setRowNames( QStringList() << tr("author") << tr("date") << tr("description") << tr("uri") << tr("reference") );
					for (int j=0; j < 5; ++j)
						data.value(j,0) = data.rowName(j);

					data.description() = tr("Annotation: A set of fields and text values. The fields, such as author or date, are the row names. First column contains values.");

					handles[i]->textDataTable(tr("Annotation")) = data;
				}
				if (!handles[i]->tools.contains(this) )
					handles[i]->tools += this;
			}
	}

	void NameFamilyDialog::closeDialog()
	{
		if (dialog)
		{
			dialog->reject();
		}
	}

	void NameFamilyDialog::showDialog(ItemHandle* handle)
	{
		if (!handle || !textEdit) return;

		if (!dialog)
		{
			if (console())
                console()->error(tr("Tool not initialized."));
			return;
		}

		if (dialog->isVisible())
			dialog->reject();

		textEdit->clear();
		QTextCursor cursor = textEdit->textCursor();

		QTextCharFormat fieldFormat, textFormat;
		fieldFormat.setForeground(QColor("#003AA3"));
		textFormat.setFontWeight(QFont::Bold);
		textFormat.setForeground(QColor("#252F41"));

		cursor.setCharFormat(fieldFormat);
		cursor.insertText(tr("name : "));

		cursor.setCharFormat(textFormat);
		cursor.insertText(handle->name);
		cursor.insertText(tr("\n"));

		if (handle && handle->family())
		{
			cursor.setCharFormat(fieldFormat);
			cursor.insertText(tr("family : "));

			cursor.setCharFormat(textFormat);
			cursor.insertText(handle->family()->name());
			cursor.insertText(tr("\n"));
		}

		if (handle->hasTextData(tr("Annotation")))
		{
			DataTable<QString>& annotation = handle->textDataTable(tr("Annotation"));
			if (annotation.cols() == 1)
				for (int i=0; i < annotation.rows(); ++i)
				{
					cursor.setCharFormat(fieldFormat);
					cursor.insertText(annotation.rowName(i) + tr(" : "));

					cursor.setCharFormat(textFormat);
					cursor.insertText(annotation.value(i,0));
					cursor.insertText(tr("\n"));
				}
		}

		dialog->show();
	}

	void NameFamilyDialog::dialogFinished()
	{
		if (!textEdit || !selectedItem || !mainWindow || !currentNetwork()) return;

		NetworkHandle * net = currentNetwork();

		bool containsConnections = false;

		QStringList strlst = textEdit->toPlainText().split(tr("\n"));

		QRegExp regex(tr("^([^:]+):(.*)"));

		ItemHandle * handle = selectedItem;

		QString field, text;
		QString name, family;
		DataTable<QString> data;

		for (int i=0; i < strlst.size(); ++i)
		{
			if (regex.indexIn(strlst[i]) > -1)
			{
				field = regex.cap(1);
				text = regex.cap(2);

				if (field.trimmed().toLower() == tr("name")) name = text.trimmed();
				else
					if (field.trimmed().toLower() == tr("family")) family = text.trimmed();
					else
						if (!field.trimmed().isEmpty())
							data.value(field,0) = text.trimmed();
			}
		}

		if (name != handle->name)
			net->rename(handle,name);

		if (handle->hasTextData(tr("Annotation")))
		{
			bool changed = false;

			if (data != handle->textDataTable(tr("Annotation")))
				net->changeData(handle->fullName() + tr("'s annotation changed"), handle,tr("Annotation"),&data);
		}

		containsConnections = (ConnectionHandle::cast(selectedItem) != 0);

		if (!handle->family() || family.isEmpty() || handle->family()->name() == family) return;

		if (!mainWindow->tool("Nodes Tree"))
		{
			if (console())
                console()->error(tr("No nodes family tree available."));
			return;
		}

		NodesTree * nodesTree = static_cast<NodesTree*>(mainWindow->tool("Nodes Tree"));
		if (!(nodesTree && nodesTree->getFamily(family)))
		{
			if (console())
                console()->error(tr("Family name does not match any node family name."));
			return;
		}

		currentNetwork()->setHandleFamily(handle, nodesTree->getFamily(family));

		selectedItem = 0;

	}

	void NameFamilyDialog::select(int)
	{
		GraphicsScene * scene = currentScene();
		
		if (scene->selected().size() != 1)
		{
			if (console())
                console()->error(tr("please select one item"));
		}
		else
		{
			selectedItem = getHandle(scene->selected()[0]);
			if (!selectedItem)
			{
				if (console())
	                console()->error(tr("the selected item is not part of the network"));
	        }
	        else
				showDialog(selectedItem);
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
		if (mainWindow->isValidHandlePointer(item) && list)
		{
			(*list).clear();

			if (item->hasTextData(tr("Annotation")))
			{
				DataTable<QString> data = item->textDataTable(tr("Annotation"));

				//(*list) << data.getRowNames();

				for (int i=0; i < data.rows(); ++i)
					(*list) << data.rowName(i) << data.value(i,0);
			}
		}
		if (sem)
			sem->release();
	}

	void NameFamilyDialog::setAnnotation(QSemaphore* sem, ItemHandle* item, const QStringList& list)
	{
		if (mainWindow->isValidHandlePointer(item))
		{
			DataTable<QString> data;

			if (item->hasTextData(tr("Annotation")))
				data = item->textDataTable(tr("Annotation"));
			else
				item->textDataTable(tr("Annotation")) = data;

			for (int i=0; i < (list.size()-1); i+=2)
				data.value( list[i] ,0) = list[i+1];

			if (currentNetwork())
				currentNetwork()->changeData(item->fullName() + tr("'s annotation changed"),item,tr("Annotation"),&data);
		}
		if (sem)
			sem->release();
		if (sem)
			sem->release();
	}

	tc_strings NameFamilyDialog::_getAnnotation(long o)
	{
		return fToS.getAnnotation(o);
	}

	void NameFamilyDialog::_setAnnotation(long o, tc_strings a)
	{
		return fToS.setAnnotation(o,a);
	}

	tc_strings NameFamilyDialog_FtoS::getAnnotation(long o)
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

	void NameFamilyDialog_FtoS::setAnnotation(long o, tc_strings a)
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
		tc_strings (*tc_getAnnotation)(long),
		void (*tc_setAnnotation)(long,tc_strings)
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


