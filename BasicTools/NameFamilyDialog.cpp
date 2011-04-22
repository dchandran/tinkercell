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

		QString appDir = QCoreApplication::applicationDirPath();
		NodeGraphicsReader reader;
		reader.readXml(&idcard,tr(":/images/idcard.xml"));
		idcard.normalize();
		idcard.scale(40.0/idcard.sceneBoundingRect().width(),30.0/idcard.sceneBoundingRect().height());

		ToolGraphicsItem * toolGraphicsItem = new ToolGraphicsItem(this);
		addGraphicsItem(toolGraphicsItem);
		toolGraphicsItem->addToGroup(&idcard);
		toolGraphicsItem->setToolTip(tr("Name, family, and Annotation"));
	}
	void NameFamilyDialog::makeDialog(QWidget* parent)
	{
		dialog = new QDialog(parent);

		QPushButton * okButton = new QPushButton("OK");
		connect(okButton,SIGNAL(released()),dialog,SLOT(accept()));
		QPushButton * cancelButton = new QPushButton("Cancel");
		connect(cancelButton,SIGNAL(released()),dialog,SLOT(reject()));

		QVBoxLayout * layout = new QVBoxLayout;
		QHBoxLayout * layout2 = new QHBoxLayout;
		QHBoxLayout * layout3 = new QHBoxLayout;

		layout2->addWidget( textEdit = new CodeEditor );
		textEdit->setReadOnly(true);
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

//			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));

			connect(mainWindow,SIGNAL(itemsInserted(NetworkHandle*, const QList<ItemHandle*>&)),
				this, SLOT(itemsInsertedSlot(NetworkHandle*, const QList<ItemHandle*>&)));
			connect(this,SIGNAL(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)),
				mainWindow,SIGNAL(itemsInserted(GraphicsScene*,const QList<QGraphicsItem *>&, const QList<ItemHandle*>&)));
			connect(this,SIGNAL(itemsAboutToBeInserted(GraphicsScene*, QList<QGraphicsItem *>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)),
				mainWindow,SIGNAL(itemsAboutToBeInserted(GraphicsScene*,QList<QGraphicsItem *>&, QList<ItemHandle*>&, QList<QUndoCommand*>&)));

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
					data.setColumnName(0,tr("entry"));
					data.setRowNames( QStringList() << tr("author") << tr("date") << tr("description") << tr("uri") << tr("reference") );
					for (int j=0; j < 5; ++j)
						data.value(j,0) = tr("");

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
			if (annotation.columns() == 1)
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

}


