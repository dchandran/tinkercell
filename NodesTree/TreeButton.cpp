/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT


****************************************************************************/

#include <QMimeData>
#include <QDrag>
#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "NodeGraphicsItem.h"
#include "NodeGraphicsReader.h"
#include "ConnectionGraphicsItem.h"
#include "TreeButton.h"


namespace Tinkercell
{

	FamilyTreeButton::FamilyTreeButton(const QString& name, QWidget * parent) : 
		QToolButton(parent), nodeFamily(0), connectionFamily(0)
	{
		setPalette(QPalette(QColor(255,255,255)));
		setAutoFillBackground (true);
		setAcceptDrops(true);
		
		QString s = name;
		int sz = 16 - s.length();
		if (sz > 0)
		{
			s = s.leftJustified(sz/2 + s.length());
			s = s.rightJustified(16);
		}
		setText(s);
	}

	FamilyTreeButton::FamilyTreeButton(NodeFamily* family , QWidget * parent) : QToolButton(parent), nodeFamily(family), connectionFamily(0)
	{
		if (!nodeFamily) return;

		QAction* infoAction = new QAction(QIcon(":/images/about.png"),tr("about ") + nodeFamily->name, this);
		QAction* graphicsAction = new QAction(QIcon(":/images/replace.png"),tr("change graphics"), this);
		connect(infoAction,SIGNAL(triggered()),this,SLOT(about()));
		connect(graphicsAction,SIGNAL(triggered()),this,SLOT(replaceAction()));
		menu.addAction(infoAction);
		menu.addAction(graphicsAction);

		setToolTip(nodeFamily->name + tr(": ") + nodeFamily->description);
		setPalette(QPalette(QColor(255,255,255)));
		setAutoFillBackground (true);
		setAcceptDrops(true);
		
		QString s = nodeFamily->name;
		int sz = 16 - s.length();
		if (sz > 0)
		{
			s = s.leftJustified(sz/2 + s.length());
			s = s.rightJustified(16);
		}
		setText(s);
		
		if (nodeFamily->pixmap.isNull()) return;
		
		setIcon(QIcon(nodeFamily->pixmap));

		if (nodeFamily->pixmap.width() > nodeFamily->pixmap.height())
		{
			int w = 20 * nodeFamily->pixmap.width()/nodeFamily->pixmap.height();
			if (w > 50) w = 50;
			setIconSize(QSize(w,20));
		}
		else
		{
			int h = 20 * nodeFamily->pixmap.height()/nodeFamily->pixmap.width();
			if (h > 50) h = 50;
			setIconSize(QSize(20, h));
		}
		
		setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	}

	FamilyTreeButton::FamilyTreeButton(ConnectionFamily* family, QWidget * parent) : QToolButton(parent), nodeFamily(0), connectionFamily(family)
	{
		if (!connectionFamily) return;

		QAction* infoAction = new QAction(QIcon(":/images/about.png"),tr("about ") + connectionFamily->name, this);
		connect(infoAction,SIGNAL(triggered()),this,SLOT(about()));
		menu.addAction(infoAction);

		/*if (connectionFamily->name.contains(tr("Connection")) ||
			connectionFamily->name.contains(tr("connection")) ||
			connectionFamily->name.contains(tr("Reaction")) ||
			connectionFamily->name.contains(tr("reaction")))
			setToolTip(QObject::tr("insert ") + (connectionFamily->name));
		else
			setToolTip(QObject::tr("insert ") + (connectionFamily->name) + tr(" reaction"));*/
		setToolTip(connectionFamily->name + tr(": ") + connectionFamily->description);

		setPalette(QPalette(QColor(255,255,255)));
		setAutoFillBackground (true);
		setAcceptDrops(true);
		
		QString s = connectionFamily->name;
		int sz = 16 - s.length();
		if (sz > 0)
		{
			s = s.leftJustified(sz/2 + s.length());
			s = s.rightJustified(16);
		}
		setText(s);
		
		if (connectionFamily->pixmap.isNull()) return;
		
		setIcon(QIcon(connectionFamily->pixmap));

		if (connectionFamily->pixmap.width() > connectionFamily->pixmap.height())
		{
			int w = 20 * connectionFamily->pixmap.width()/connectionFamily->pixmap.height();
			if (w > 50) w = 50;
			setIconSize(QSize(w,20));
		}
		else
		{
			int h = 20 * connectionFamily->pixmap.height()/connectionFamily->pixmap.width();
			if (h > 50) h = 50;
			setIconSize(QSize(20, h));
		}
		
		setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	}
	void FamilyTreeButton::contextMenuEvent(QContextMenuEvent * event)
	{
		if (!event) return;
		QWidget * widget = this;
		QPoint pos = event->pos();
		while (widget)
		{
			pos += widget->pos();
			widget = widget->parentWidget();
		}
		menu.exec(pos);
	}
	
	void FamilyTreeButton::mousePressEvent(QMouseEvent *event)
	{
		if (event->button() == Qt::LeftButton) 
		{
			QDrag *drag = new QDrag(this);
			QMimeData *mimeData = new QMimeData;
			drag->setPixmap(icon().pixmap(QSize(30,30)));
			mimeData->setText(text());	
			drag->setMimeData(mimeData);
			drag->exec();
		}
	}

/*
	void FamilyTreeButton::mouseReleaseEvent(QMouseEvent * event)
	{
		if (event->button() == Qt::LeftButton)
		{
			if (nodeFamily)
				emit nodeSelected(nodeFamily);
			else
			if (connectionFamily)
				emit connectionSelected(connectionFamily);
		}
	}
*/

	void FamilyTreeButton::about()
	{
		static QDialog * messageBox = 0;
		static QTextEdit * textBox = 0;
		static QPushButton * okButton = 0;
		if (!messageBox || !textBox || !okButton)
		{
			okButton = new QPushButton("Close",this);
			textBox = new QTextEdit(this);
			textBox->setReadOnly(true);
			messageBox = new QDialog(parentWidget(),Qt::Tool);
			messageBox->setWindowTitle("Family information");
			QVBoxLayout * layout = new QVBoxLayout;
			layout->addWidget(textBox,0);
			layout->addWidget(okButton,1);
			connect(okButton,SIGNAL(pressed()),messageBox,SLOT(accept()));
			messageBox->setLayout(layout);
		}

		if (nodeFamily)
		{
			NodeFamily * family = nodeFamily;

			QString text = tr("Family: ") + family->name + tr("\nParent(s): ");
			if (family->parent())
			{
				QList<ItemFamily*> parents = family->parents();
				for (int j=0; j < parents.size(); ++j)
					if (parents[j])
						if ((j+1) < parents.size())
							text += parents[j]->name + tr(" , ");
						else
							text += parents[j]->name + tr("\n\n");
			}
			else
				text += tr("none\n\n");


			text += tr("Description: ") + family->description + tr("\n\n");

			if (!family->measurementUnit.name.isEmpty() && !family->measurementUnit.property.isEmpty())
				text += tr("Unit of measurement: ") + family->measurementUnit.name + tr("\n\n");

			text += tr("Attributes: ")
					+ ( QStringList() << family->numericalAttributes.keys() << family->textAttributes.keys() ).join(" , ")
					+ "\n";
			textBox->setText(text);
			messageBox->exec();
		}
		else
		if (connectionFamily)
		{
			ConnectionFamily * family = connectionFamily;
			QString text = tr("Family: ") + family->name + tr("\nParent(s): ");
			if (family->parent())
			{
				QList<ItemFamily*> parents = family->parents();
				for (int j=0; j < parents.size(); ++j)
					if (parents[j])
						if ((j+1) < parents.size())
							text += parents[j]->name + tr(" , ");
						else
							text += parents[j]->name + tr("\n\n");
			}
			else
				text += tr("none\n\n");

			text += tr("Description: ") + family->description + tr("\n\n");

			if (!family->measurementUnit.name.isEmpty() && !family->measurementUnit.property.isEmpty())
				text += tr("Unit of measurement: ") + family->measurementUnit.name + tr("\n\n");

			if (family->textAttributes.contains(tr("typein")) && family->textAttributes.contains(tr("typeout")) &&
				family->numericalAttributes.contains(tr("numin")) && family->numericalAttributes.contains(tr("numout")))
			{
				text += tr("Connects: ")
						+ QString::number(family->numericalAttributes[tr("numin")]) + tr(" ")
						+ family->textAttributes[tr("typein")] + tr(" to ")
						+ QString::number(family->numericalAttributes[tr("numout")]) + tr(" ")
						+ family->textAttributes[tr("typeout")] + tr(" \n\n");
			}

			text += tr("Attributes: ")
					+ ( QStringList() << family->numericalAttributes.keys() << family->textAttributes.keys() ).join(" , ")
					+ "\n";
			textBox->setText(text);
			messageBox->exec();
		}
	}

	void FamilyTreeButton::replaceAction()
	{
		if (nodeFamily)
		{
			NodeFamily * node = nodeFamily;

			QString currentFile = tr("");
			if (node->graphicsItems.size() > 0 &&
				NodeGraphicsItem::topLevelNodeItem(node->graphicsItems[0]))
				currentFile = newFileName;

			QString fileName;

               if (nodesTree)
                    fileName = nodesTree->replaceNodeFile();
               else
                    fileName = QFileDialog::getOpenFileName(this, tr("Select New Node Graphics File"),
																	currentFile,
																	tr("XML Files (*.xml)"));
			if (fileName.isEmpty() || fileName.isNull())
				return;

			QString pngFile = fileName;
			pngFile.replace(QRegExp(tr("xml$")),tr("PNG"));
			pngFile.replace(QRegExp(tr("XML$")),tr("PNG"));
			node->pixmap = QPixmap(pngFile);
			node->pixmap.setMask(node->pixmap.createMaskFromColor(QColor(255,255,255)));

			this->setIcon(QIcon(node->pixmap));

			NodeGraphicsReader imageReader;
			NodeGraphicsItem * nodeitem = new NodeGraphicsItem;
			imageReader.readXml(nodeitem,fileName);
			if (nodeitem->isValid())
			{
				for (int j=0; j < node->graphicsItems.size(); ++j)
						if (node->graphicsItems[j])
							delete node->graphicsItems[j];
				node->graphicsItems.clear();

				nodeitem->normalize();
				node->graphicsItems += nodeitem;
				newFileName = fileName;
			}
			else
			{
				delete nodeitem;
			}
		}
	}

	ItemFamily * FamilyTreeButton::family() const
	{
		if (nodeFamily)
			return nodeFamily;
		if (connectionFamily)
			return connectionFamily;
		return 0;
	}
	
	void FamilyTreeButton::dragEnterEvent(QDragEnterEvent *event)
	{
		event->accept();
	}
	
	void FamilyTreeButton::dropEvent(QDropEvent * event)
	{
		if (nodeFamily)
				emit nodeSelected(nodeFamily);
		else
		if (connectionFamily)
			emit connectionSelected(connectionFamily);	
		else
			emit pressed(text(),icon().pixmap(30));
	}

}

