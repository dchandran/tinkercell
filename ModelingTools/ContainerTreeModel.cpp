/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 A tree model for storing the tree of handles

****************************************************************************/
#include <iostream>
#include "EquationParser.h"
#include "ItemFamily.h"
#include "ContainerTreeModel.h"
#include "BasicInformationTool.h"

namespace Tinkercell
{
	/*****************************************
	      CONTAINER TREE ITEM
	******************************************/
	
	QHash<ContainerTreeItem*,bool> ContainerTreeModel::markedForDeletion;
	
	ContainerTreeItem::ContainerTreeItem(ItemHandle * handle, ContainerTreeItem *parent)
	{
		parentItem = parent;
		itemHandle = handle;
		attributeName = QString();
	}

	ContainerTreeItem::~ContainerTreeItem()
	{
		if (parentItem)
			parentItem->childItems.removeAll(this);
		qDeleteAll(childItems);
	}

	void ContainerTreeItem::appendChild(ContainerTreeItem *item)
	{
		if (!item)
			return;
		if (item->parentItem)
			item->parentItem->childItems.removeAll(item);
		if (!childItems.contains(item))
			childItems.append(item);
		item->parentItem = this;
	}

	ContainerTreeItem *ContainerTreeItem::child(int row)
	{
		return childItems.value(row);
	}

	int ContainerTreeItem::childCount() const
	{
		return childItems.count();
	}

	int ContainerTreeItem::columnCount() const
	{
		return 2;
	}

	ItemHandle * ContainerTreeItem::handle()
	{
		return itemHandle;
	}

	QString& ContainerTreeItem::text()
	{
		return attributeName;
	}

	QVariant ContainerTreeItem::data(int column) const
	{
		if (!itemHandle || parentItem == 0) //root
		{
			if (column == 0)
				return QVariant(QString("Name"));

			return QVariant(QString("Value"));
		}

		if (childItems.isEmpty() && !attributeName.isEmpty())
		{
			if (column == 0)
				return QVariant(attributeName);
			else
				if (itemHandle->numericalDataTable(QString("Parameters")).hasRow(attributeName))
					return QVariant(itemHandle->numericalData(QString("Parameters"),attributeName,0));
		}
		else
		{
			if (column == 0)
				return QVariant(itemHandle->name);
			else
			{
				if (itemHandle->hasNumericalData(QString("Initial Value")))
					if (itemHandle->hasTextData(QString("Assignments")) &&
						itemHandle->textDataTable(QString("Assignments")).hasRow(QString("self")) &&
						(itemHandle->textData(QString("Assignments"), QString("self"), 0).size() > 1) )
						return QVariant(
							itemHandle->textData(QString("Assignments"), QString("self"), 0));
					else	
						return QVariant(itemHandle->numericalData(QString("Initial Value")));
				else
					if (itemHandle->hasTextData(QString("Rate equations")))
						if (itemHandle->children.isEmpty())
							return QVariant(itemHandle->textData(QString("Rate equations")));
						else
							return QVariant("<grouped>");
			}
		}

		return QVariant();
	}

	ContainerTreeItem *ContainerTreeItem::parent()
	{
		return parentItem;
	}

	int ContainerTreeItem::row() const
	{
		if (parentItem)
			return parentItem->childItems.indexOf(const_cast<ContainerTreeItem*>(this));

		return 0;
	}
	
	void ContainerTreeItem::sortChildren()
	{
		if (childItems.isEmpty()) return;
		
		ItemHandle * handle;
		ItemFamily * family, * root;
		QHash<QString, QStringList> item_names; 
		QStringList family_names;
		QHash<QString,  QHash<QString,ContainerTreeItem*> > hash;
		QList<ItemFamily*> families;
		
		for (int i=0; i < childItems.size(); ++i)
		{
			if (childItems[i] && (handle = childItems[i]->handle()) && (family = handle->family()))
			{
				root = family->root();
				
				if (!families.contains(root))
					families << root->allChildren();
				
				item_names[family->name()] << handle->fullName();
				hash[family->name()].insertMulti(handle->fullName(),childItems[i]);
				childItems[i]->sortChildren();
			}
		}

		for (int i=0; i < families.size(); ++i)
			if (families[i])
				family_names << families[i]->name();
		
		for (int i=0; i < family_names.size(); ++i)
			item_names[ family_names[i] ].sort();
		childItems.clear();

		for (int i=0; i < family_names.size(); ++i)
		{
			if (hash.contains(family_names[i]) && item_names.contains(family_names[i]))
			{
				QStringList names = item_names[ family_names[i] ];
				for (int j=0; j < names.size(); ++j)
					if (hash[ family_names[i] ].contains(names[j]))
					{
						QList<ContainerTreeItem*> items = hash[ family_names[i] ].values( names[j] );
						for (int k=0; k < items.size(); ++k)
							if (items[k] && !childItems.contains(items[k]))
							{
								childItems << items[k];
							}
					}
			}
		}
	}

	/**************************************
		  CONTAINER TREE MODEL
	***************************************/

	ContainerTreeModel::ContainerTreeModel(NetworkHandle * net, QObject *parent)
	 : QAbstractItemModel(parent)
	{
		rootItem = new ContainerTreeItem;
		this->network = net;

        if (net)
		{
            QList<ItemHandle*> items = net->symbolsTable.allHandlesSortedByFamily();
			ItemHandle* handle = 0;

			QList<ItemHandle*> visited;
			ContainerTreeItem* treeItem = 0;

			for (int i=0; i < items.size(); ++i)
				if (items[i])
				{
					handle = items[i]->root();
					
					if (handle && !visited.contains(handle) && handle->family())
					{	
						visited += handle;
						if ((treeItem = makeBranch(handle,rootItem)))
						{
							rootItem->appendChild(treeItem);
						}
					}
				}
		}
	}
	
	ContainerTreeItem* ContainerTreeModel::findTreeItem(ContainerTreeItem* root, ItemHandle* handle, const QString& attribute)
	{
		if (handle == 0) return rootItem;
		if (!root) return 0;
		
		if (attribute.isEmpty() && treeItems.contains(handle)) return treeItems[handle];
		
		QList<ContainerTreeItem*> queue = root->childItems;
		
		for (int i=0; i < queue.size(); ++i)
		{
			if (queue[i])
			{
				if (queue[i]->handle() == handle && queue[i]->attributeName == attribute)
					return queue[i];

				queue << queue[i]->childItems;
			}
		}

		return 0;
	}
	
	void ContainerTreeModel::reload(NetworkHandle * net)
	{
		if (net)
		{
			ContainerTreeItem * newRootItem;
			
			if (net != this->network)
			{
				treeItems.clear();
				if (rootItem)
					markedForDeletion[rootItem] = true;
				newRootItem = rootItem = new ContainerTreeItem;
			}
			else
			{
				newRootItem = new ContainerTreeItem;
			}

            this->network = net;
			
            QList<ItemHandle*> items = net->symbolsTable.allHandlesSortedByFamily();
			ItemHandle* handle = 0;

			QList<ItemHandle*> visited;
			ContainerTreeItem* treeItem = 0;

			for (int i=0; i < items.size(); ++i)
				if (items[i])
				{
					handle = items[i]->root();
					
					if (handle && !visited.contains(handle) && handle->family())
					{	
						visited += handle;
						if ((treeItem = makeBranch(handle,newRootItem)))						
						{
							newRootItem->appendChild(treeItem);
						}
					}
				}
			
			if (rootItem != newRootItem)
			{
				if (rootItem)
				{
			        QList<ContainerTreeItem*> queue = rootItem->childItems;
		
				    for (int i=0; i < queue.size(); ++i)
					    if (queue[i])
					    {
						    treeItems.remove(queue[i]->handle());
						    queue << queue[i]->childItems;
					    }

					markedForDeletion[rootItem] = true;
				}

				rootItem = newRootItem;
			}
			emit layoutChanged();
		}
	}
	
	ContainerTreeItem* ContainerTreeModel::makeBranch(ItemHandle* handle, ContainerTreeItem * parentItem, const QString & attribute)
	{
		if (!handle) return 0;

		ContainerTreeItem * item;

		if (attribute.isEmpty())
			item = findTreeItem(rootItem, handle, attribute);
		else
			item = findTreeItem(parentItem, handle, attribute);

		treeItems.remove(handle);

		if (item)
		{
			if (item->parentItem)
				item->parentItem->childItems.removeAll(item);

			item->parentItem = parentItem;

			if (!attribute.isEmpty())	
				return item;
		}		
	
		int k = 0;
		NumericalDataTable * params = 0;
		
		if (attribute.isEmpty())
		{
			k = handle->children.size();
		
			if (handle->hasNumericalData(QString("Parameters")))
			{
				params = &(handle->numericalDataTable(QString("Parameters")));
				k += params->rows();
			}
		}

		if (!item || params)
		{
			if (item)
			{
				item->parentItem = 0;
				item->childItems.clear();
				for (int i=0; i < item->childItems.size(); ++i)
					item->childItems[i]->parentItem = 0;
				if (!markedForDeletion.contains(item))
					markedForDeletion[item] = true;
			}
			item = new ContainerTreeItem(handle,parentItem);
			item->attributeName = attribute;
			if (!attribute.isEmpty())
				return item;
		}
		
		ItemHandle * childHandle = 0;
		ContainerTreeItem * child = 0;
		
		if (params)
		{
			for (int i=0; i < params->rows(); ++i)
			{
				if (child = makeBranch(handle,item,params->rowName(i)))
				{
					item->appendChild(child);
				}
			}
		}
		
		if (!handle->children.isEmpty())
		{
			for (int i=0; i < handle->children.size(); ++i)
			{
				childHandle = handle->children[i];
				if (childHandle && childHandle->family() && childHandle->parent == handle)
				{
					if (child = makeBranch(handle->children[i],item))
					{
						item->appendChild(child);
					}
				}
			}
		}

		treeItems[handle] = item;

		return item;
	}

	ContainerTreeModel::~ContainerTreeModel()
	{
		if (rootItem)
		{
			treeItems.clear();
			markedForDeletion[rootItem] = true;
		}
		
		QList<ContainerTreeItem*> deleteItems =markedForDeletion.keys();
		for (int i=0; i < deleteItems.size(); ++i)
			if (deleteItems[i])
				delete deleteItems[i];

		markedForDeletion.clear();
	}

	int ContainerTreeModel::columnCount(const QModelIndex &parent) const
	{
		if (parent.isValid())
			return static_cast<ContainerTreeItem*>(parent.internalPointer())->columnCount();
		else
			return rootItem->columnCount();
	}

	QVariant ContainerTreeModel::data(const QModelIndex &index, int role) const
	{
		if (!index.isValid())
			return QVariant();

		ContainerTreeItem *item = static_cast<ContainerTreeItem*>(index.internalPointer());

		ItemHandle * handle = item->handle();

		
		if (handle && handle->family())
		{
			if (role == Qt::DecorationRole && index.column() == 0 && item->text().isEmpty())
			{	
				QPixmap pixmap = handle->family()->pixmap;
				if (pixmap.isNull()) return QVariant();
				int h = 15;
				int w = 15 * pixmap.width()/pixmap.height();
				if (w > 25) w = 25;
				return QVariant(pixmap.scaled(w, h));
			}
		
			if (role == Qt::ToolTipRole)
			{
				if (!item->attributeName.isEmpty())
					return QVariant(tr("Parameter"));
				
				if (ConnectionHandle::cast(handle))
					return QVariant(tr("Reaction rate equation"));
				
				return QVariant(handle->family()->measurementUnit.property);
			}
		}

		if (role != Qt::DisplayRole)
			return QVariant();

		return item->data(index.column());
	}

	bool ContainerTreeModel::setData(const QModelIndex & index, const QVariant & value, int)
	{
		if (!index.isValid() || !network)
			return false;

		ContainerTreeItem *item = static_cast<ContainerTreeItem*>(index.internalPointer());

		if (!item) return false;

		ItemHandle * handle = item->handle();

		QString attributeName = item->text();

		if (!handle || !handle->family()) return false;

		if (index.column() == 0)
		{
            if (network && handle)
			{
				if (attributeName.isEmpty())
				{
					if (value.toString() != handle->name)
					{
						if (value.toString().isEmpty())
						{
							network->remove(value.toString() + QString(" removed"), QList<ItemHandle*>() << handle);
						}
						else
						{
							network->rename(handle, value.toString());
						}
					}
				}
				else
				if (value.toString() != attributeName)
				{
					if (!value.toString().isEmpty())
						network->rename(handle->fullName() + tr(".") + attributeName, handle->fullName() + tr(".") + value.toString());
					else
					{
						QString err = BasicInformationTool::removeParameterFromModel(network, handle, attributeName);
						if (!err.isEmpty())
							QMessageBox::information(network->currentWindow(),tr("Cannot remove"), err);
					}
					return true;
				}
			}
		}
		else
		if (index.column() == 1)
		{
			if (attributeName.isEmpty())
			{
				if (handle->hasNumericalData(QString("Initial Value")))
				{
					bool ok;
					double d = value.toDouble(&ok);
					if (!ok && handle->hasTextData(QString("Assignments")) &&
						handle->textDataTable(QString("Assignments")).hasRow(QString("self")))
					{
						QString s = value.toString();
						if (EquationParser::validate(network, handle, s, QStringList() << "time" << "Time" << "TIME"))
						{
							 TextDataTable newTable(handle->textDataTable( QString("Assignments") ));
							 newTable.value("self",0) = s;
							 network->changeData(handle->fullName() + tr(" = ") + s,
											handle,
											QString("Assignments"),
											&newTable);
							BasicInformationTool::removeUnusedParametersInModel(network);
							return true;
						}
					}
					else
					{
						NumericalDataTable newTable(handle->numericalDataTable( QString("Initial Value") ));
						if (ok && newTable.value(0,0) != d)
						{
							newTable.value(0,0) = d;
						
							BasicInformationTool::initialValues[ handle->family()->measurementUnit.property ] = d;						
		                    network->changeData(handle->fullName() + tr(" = ") + QString::number(d),
												handle,
												QString("Initial Value"),
												&newTable);
							return true;
						}
					}
				}
				else
					if (handle->hasTextData(QString("Rate equations")) && handle->children.isEmpty())
					{
						QString s = value.toString();
						if (EquationParser::validate(network, handle, s, QStringList() << "time" << "Time" << "TIME"))
						{
							 TextDataTable newTable(handle->textDataTable( QString("Rate equations") ));
							 newTable.value(0,0) = s;
							 network->changeData(handle->fullName() + tr("'s rate = ") + s,
											handle,
											QString("Rate equations"),
											&newTable);
							BasicInformationTool::removeUnusedParametersInModel(network);
							return true;
						}
					}
			}
			else
				if (handle->hasNumericalData(QString("Parameters")))
				{
					DataTable<qreal> newTable(handle->numericalDataTable( QString("Parameters") ));
					bool ok;
					double d = value.toDouble(&ok);
					if (ok && newTable.value(attributeName,0) != d)
					{
						newTable.value(attributeName,0) = d;
						network->changeData(handle->fullName() + tr(".") + attributeName + tr(" = ") + QString::number(d),
											handle,
											QString("Parameters"),
											&newTable);
						return true;
					}
				}
		}
		return false;
	}

	Qt::ItemFlags ContainerTreeModel::flags(const QModelIndex &index) const
	{
		if (!index.isValid())
			return 0;

		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
	}

	QVariant ContainerTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
	{
		if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
			return rootItem->data(section);

		return QVariant();
	}

	ContainerTreeItem * ContainerTreeModel::root() { return rootItem; }

	QModelIndex ContainerTreeModel::index(int row, int column, const QModelIndex &parent) const
	{
		if (!hasIndex(row, column, parent))
			return QModelIndex();

		ContainerTreeItem *parentItem;
		if (!parent.isValid())
			parentItem = rootItem;
		else
			parentItem = static_cast<ContainerTreeItem*>(parent.internalPointer());

		ContainerTreeItem *childItem = parentItem->child(row);

		if (markedForDeletion.contains(childItem))
			return QModelIndex();
			
		if (childItem)
			return createIndex(row, column, childItem);
		else
			return QModelIndex();
	}

	QModelIndex ContainerTreeModel::parent(const QModelIndex &index) const
	{
		if (!index.isValid())
			return QModelIndex();

		ContainerTreeItem *childItem = static_cast<ContainerTreeItem*>(index.internalPointer());
		if (!childItem)
			return QModelIndex();
		
		if (markedForDeletion.contains(childItem))
			return QModelIndex();
		
		ContainerTreeItem *parentItem = childItem->parent();
		if (!parentItem)
			return QModelIndex();

		if (markedForDeletion.contains(parentItem))
			return QModelIndex();

		if (parentItem == rootItem)
			return QModelIndex();

		return createIndex(parentItem->row(), 0, parentItem);
	}

	int ContainerTreeModel::rowCount(const QModelIndex &parent) const
	{
		ContainerTreeItem *parentItem;
		if (parent.column() > 0)
			return 0;

		if (!parent.isValid())
			parentItem = rootItem;
		else
			parentItem = static_cast<ContainerTreeItem*>(parent.internalPointer());

		return parentItem->childCount();
	}
	
	void ContainerTreeModel::sort ( int , Qt::SortOrder )
	{
		//if (rootItem)
			//rootItem->sortChildren();
	}

	/********************************
            TREE DELEGATE
    *********************************/

    ContainerTreeDelegate::ContainerTreeDelegate(QTreeView *parent)
        : QItemDelegate(parent), treeView(parent)
    {
    }

    QWidget *ContainerTreeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */,	const QModelIndex & index) const
    {
    	if (index.isValid() && treeView)
			treeView->scrollTo(index.sibling(index.row(),0));
        return new QLineEdit(parent);
    }

    void ContainerTreeDelegate::setEditorData(QWidget *widget, const QModelIndex &index) const
    {
        if (index.isValid())
        {
            ContainerTreeItem * item = static_cast<ContainerTreeItem*>(index.internalPointer());
            if (ContainerTreeModel::markedForDeletion.contains(item)) return;

            ItemHandle * handle = item->handle();
            QString attributeName = item->text();
            QLineEdit * editor = static_cast<QLineEdit*>(widget);

            if (handle)
            {
				if (index.column() == 0)
				{
					if (attributeName.isEmpty())
						editor->setText( handle->name );
					else
						editor->setText( attributeName );
				}

				if (index.column() == 1)
				{
					if (attributeName.isEmpty())
					{
						if (handle->hasNumericalData(QString("Initial Value")))
							if (handle->hasTextData(QString("Assignments")) &&
								handle->textDataTable(QString("Assignments")).hasRow(QString("self")))
									editor->setText(handle->textData(QString("Assignments"), QString("self")));
								else
									editor->setText( QString::number(handle->numericalData(QString("Initial Value"))) );
						else
							if (handle->hasTextData(QString("Rate equations")))
								editor->setText(handle->textData(QString("Rate equations")));
					}
					else
						if (handle->hasNumericalData(QString("Parameters")))
							editor->setText( QString::number(handle->numericalData(QString("Parameters"),attributeName)) );
										
				}
			}
			
        }
    }

    void ContainerTreeDelegate::setModelData(QWidget *widget, QAbstractItemModel * model,
                                             const QModelIndex &index) const
    {
        if (index.isValid())
        {
            ContainerTreeItem * item = static_cast<ContainerTreeItem*>(index.internalPointer());
            if (ContainerTreeModel::markedForDeletion.contains(item)) return;

            ItemHandle * handle = item->handle();
            QString attributeName = item->text();
            QVariant value;

            if (handle)// && !attributeName.isEmpty())
            {
				QLineEdit * editor = static_cast<QLineEdit*>(widget);
				value = QVariant(editor->text());
                model->setData(index, value, Qt::EditRole);
            }
        }
    }

    void ContainerTreeDelegate::updateEditorGeometry(QWidget *editor,
                                                     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
    {
        editor->setGeometry(option.rect);
    }

}
