/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 

****************************************************************************/

#include "Core/NodeGraphicsItem.h"
#include "Core/NodeGraphicsReader.h"
#include "Core/ConnectionGraphicsItem.h"
//#include "NodesTree/NodesTree.h"
#include "NodesTree/TreeButton.h"


namespace Tinkercell
{

	FamilyTreeButton::FamilyTreeButton(NodeFamily* family , QWidget * parent) : QToolButton(parent), nodeFamily(family), connectionFamily(0)
	{	
		nodesTree = 0;
		if (!nodeFamily) return;
		
		QAction* infoAction = new QAction(QIcon(":/images/about.png"),tr("about ") + nodeFamily->name, this);
		QAction* graphicsAction = new QAction(QIcon(":/images/replace.png"),tr("change graphics"), this);
		connect(infoAction,SIGNAL(triggered()),this,SLOT(about()));
		connect(graphicsAction,SIGNAL(triggered()),this,SLOT(replaceAction()));
		menu.addAction(infoAction);
		menu.addAction(graphicsAction);
		
		if (nodeFamily->name.contains(tr("Node")) || nodeFamily->name.contains(tr("node")))
			setToolTip(QObject::tr("insert ") + (nodeFamily->name));
		else
			setToolTip(QObject::tr("insert ") + (nodeFamily->name) + tr(" node"));
		setPalette(QPalette(QColor(255,255,255)));
		setAutoFillBackground (true);
		setText(nodeFamily->name);
		setIcon(QIcon(nodeFamily->pixmap));
		setIconSize(QSize(40, 20));
	}
	
	FamilyTreeButton::FamilyTreeButton(ConnectionFamily* family, QWidget * parent) : QToolButton(parent), nodeFamily(0), connectionFamily(family)
	{
		if (!connectionFamily) return;
		
		QAction* infoAction = new QAction(QIcon(":/images/about.png"),tr("about ") + connectionFamily->name, this);
		connect(infoAction,SIGNAL(triggered()),this,SLOT(about()));
		menu.addAction(infoAction);
		
		if (connectionFamily->name.contains(tr("Connection")) || 
			connectionFamily->name.contains(tr("connection")) ||
			connectionFamily->name.contains(tr("Reaction")) || 
			connectionFamily->name.contains(tr("reaction")))
			setToolTip(QObject::tr("insert ") + (connectionFamily->name));
		else
			setToolTip(QObject::tr("insert ") + (connectionFamily->name) + tr(" reaction"));
			
		setPalette(QPalette(QColor(255,255,255)));
		setAutoFillBackground (true);	
		setText(connectionFamily->name);
		setIcon(QIcon(connectionFamily->pixmap));
		setIconSize(QSize(40, 20));
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
	
	void FamilyTreeButton::mousePressEvent(QMouseEvent * event)
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
			
			if (!family->measurementUnit.first.isEmpty() && !family->measurementUnit.second.isEmpty())
				text += tr("Unit of measurement: ") + family->measurementUnit.second + tr("(") + family->measurementUnit.first + tr(")\n\n");
			
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
			
			if (!family->measurementUnit.first.isEmpty() && !family->measurementUnit.second.isEmpty())
				text += tr("Unit of measurement: ") + family->measurementUnit.second + tr("(") + family->measurementUnit.first + tr(")\n\n");
			
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
				currentFile = (NodeGraphicsItem::topLevelNodeItem(node->graphicsItems[0]))->fileName;
			
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
			}
			else
			{
				delete nodeitem;
			}
		}
	}


}

