/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 A tree model for storing the tree of handles

****************************************************************************/

#include "ItemFamily.h"
#include "ContainerTreeModel.h"

namespace Tinkercell
{
	/*****************************************
	      CONTAINER TREE ITEM
	******************************************/
	
	QStringList ContainerTreeModel::NUMERICAL_DATA(QStringList() << "Initial Value" << "Numerical Attributes");
	QStringList ContainerTreeModel::TEXT_DATA(QStringList() << "Rates" << "Assignments");
	
	ContainerTreeItem::ContainerTreeItem(ItemHandle * handle, ContainerTreeItem *parent)
	{
		parentItem = parent;
		itemHandle = handle;
		if (itemHandle && itemHandle->family())
		{
			if (!itemHandle->family()->measurementUnit.first.isEmpty())
				attributeName = itemHandle->family()->measurementUnit.first;
			else
			{
				QList<QString> keys = itemHandle->family()->numericalAttributes.keys();
				if (itemHandle->type == ConnectionHandle::TYPE)
				{
					keys.removeAll(QString("numin"));
					keys.removeAll(QString("numout"));
				}
				if (!keys.isEmpty())
					attributeName = keys.first();
			}
		}
	}

	ContainerTreeItem::~ContainerTreeItem()
	{
		qDeleteAll(childItems);
	}

	void ContainerTreeItem::appendChild(ContainerTreeItem *item)
	{
		childItems.append(item);
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
		return 3;
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

			if (column == 1)
				return QVariant(QString("Attribute"));

			return QVariant(QString("Value"));
		}
		
		if (column == 0)
			return QVariant(itemHandle->name);
	
		for (int i=0; i < ContainerTreeModel::NUMERICAL_DATA.size(); ++i)
			if (itemHandle->data->numericalData.contains(ContainerTreeModel::NUMERICAL_DATA[i])
				&& itemHandle->data->numericalData[ ContainerTreeModel::NUMERICAL_DATA[i] ].cols() == 1
				&& itemHandle->data->numericalData[ ContainerTreeModel::NUMERICAL_DATA[i] ].getRowNames().contains(attributeName))
			{
				if (column == 1)
					return QVariant(attributeName);
				if (column == 2)
					return QVariant(itemHandle->data->numericalData[ ContainerTreeModel::NUMERICAL_DATA[i] ].value(attributeName,0));
			}
			
		for (int i=0; i < ContainerTreeModel::TEXT_DATA.size(); ++i)
			if (itemHandle->data->textData.contains(ContainerTreeModel::TEXT_DATA[i])
				&& itemHandle->data->textData[ ContainerTreeModel::TEXT_DATA[i] ].cols() == 1
				&& itemHandle->data->textData[ ContainerTreeModel::TEXT_DATA[i] ].getRowNames().contains(attributeName))
			{
				if (column == 1)
					return QVariant(attributeName);
				if (column == 2)
					return QVariant(itemHandle->data->textData[ ContainerTreeModel::TEXT_DATA[i] ].value(attributeName,0));
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
				
				item_names[family->name] << handle->fullName();
				hash[family->name][handle->fullName()] = childItems[i];
				childItems[i]->sortChildren();
			}
		}

		for (int i=0; i < families.size(); ++i)
			if (families[i])
				family_names << families[i]->name;
		
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
						ContainerTreeItem* item = hash[ family_names[i] ][ names[j] ];
						if (item && !childItems.contains(item))
							childItems << item;
					}
			}
		}
	}

	/**************************************
		  CONTAINER TREE MODEL
	***************************************/

	ContainerTreeModel::ContainerTreeModel(NetworkWindow * win, QObject *parent)
	 : QAbstractItemModel(parent)
	{
		rootItem = new ContainerTreeItem;
                this->window = win;

        if (win)
		{
            QList<ItemHandle*> items = win->allHandlesSortedByFamily();
			ItemHandle* handle = 0;

			QList<ItemHandle*> visited;
			ContainerTreeItem* treeItem = 0;

			for (int i=0; i < items.size(); ++i)
				if (items[i])
				{
					handle = items[i]->root();
					
					if (handle && !visited.contains(handle) && handle->family() && handle->visible)
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
	
	ContainerTreeItem* ContainerTreeModel::findTreeItem(ItemHandle* handle)
	{
		if (handle == 0) return rootItem;
		if (!rootItem) return 0;
		
		QList<ContainerTreeItem*> queue = rootItem->childItems;
		
		for (int i=0; i < queue.size(); ++i)
		{
			if (queue[i])
			{
				if (queue[i]->handle() == handle) return queue[i];
				queue << queue[i]->childItems;
			}
		}		
		return 0;
	}
	
	void ContainerTreeModel::reload(NetworkWindow * win)
	{
		if (win)
		{
			if (win != this->window)
			{
				delete rootItem;
				rootItem = 0;
			}
			
			ContainerTreeItem * rootItemNew = new ContainerTreeItem;
            this->window = win;
			
            QList<ItemHandle*> items = win->allHandlesSortedByFamily();
			ItemHandle* handle = 0;

			QList<ItemHandle*> visited;
			ContainerTreeItem* treeItem = 0;

			for (int i=0; i < items.size(); ++i)
				if (items[i])
				{
					handle = items[i]->root();
					
					if (handle && !visited.contains(handle) && handle->family() && handle->visible)
					{	
						visited += handle;
						if ((treeItem = makeBranch(handle,rootItemNew)))
							rootItemNew->appendChild(treeItem);					
					}
				}
			
			rootItem = rootItemNew;
			emit layoutChanged();
		}
	}	
	
	ContainerTreeItem* ContainerTreeModel::makeBranch(ItemHandle* handle, ContainerTreeItem * parentItem)
	{
		if (!handle) return 0;
		
		ContainerTreeItem * item = findTreeItem(handle);
		
		if (item)
		{
			if (item->parentItem)
				item->parentItem->childItems.removeAll(item);
			item->parentItem = parentItem;
		}		
		
		if (!item || (item->childItems.size() != handle->children.size()))
		{
			if (item)
				delete item;
			item = new ContainerTreeItem(handle,parentItem);
		}
		
		//ContainerTreeItem * item = new ContainerTreeItem(handle,parentItem);
		bool ok = false;
		ItemHandle * childHandle = 0;
		ContainerTreeItem * child = 0;
		if (!handle->children.isEmpty())
		{
			for (int i=0; i < handle->children.size(); ++i)
			{
				childHandle = handle->children[i];
				if (childHandle && childHandle->visible && childHandle->family() && childHandle->parent == handle)
				{					
					if (child = makeBranch(handle->children[i],item))
						item->appendChild(child);
				}
			}
		}
		return item;
	}

	ContainerTreeModel::~ContainerTreeModel()
	{
		if (rootItem)
			delete rootItem;
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

		if (role == Qt::DecorationRole && index.column() == 0)
		{
			ItemHandle * handle = item->handle();
			if (handle && handle->family())
			{
				QPixmap pixmap = handle->family()->pixmap;
				if (pixmap.isNull()) return QVariant();
				int h = 15;
				int w = 15 * pixmap.width()/pixmap.height();
				if (w > 25) w = 25;
				return QVariant(pixmap.scaled(w, h));
			}
		}
		
		if (role != Qt::DisplayRole)
			return QVariant();

		return item->data(index.column());
	}

	bool ContainerTreeModel::setData(const QModelIndex & index, const QVariant & value, int)
	{
        if (!index.isValid() || !window)
			return false;

		ContainerTreeItem *item = static_cast<ContainerTreeItem*>(index.internalPointer());

		if (!item) return false;

		ItemHandle * handle = item->handle();
		
		if (!handle || !handle->data || !handle->family()) return false;

		if (index.column() == 0)
		{
            if (window && handle
				&& !value.toString().isEmpty()
				&& value.toString() != handle->name)
			{
				window->rename(handle, value.toString());
				return true;
			}
		}
		else
		if (index.column() == 1)
		{
			item->text() = value.toString();
			return true;
		}
		else
		if (index.column() == 2)
		{
			QString attributeName = item->text();
			if (attributeName.isEmpty()) return false;
			
			for (int i=0; i < ContainerTreeModel::NUMERICAL_DATA.size(); ++i)
			{
				if (handle->data->numericalData.contains(ContainerTreeModel::NUMERICAL_DATA[i]) 
					&& handle->data->numericalData[ ContainerTreeModel::NUMERICAL_DATA[i] ].getRowNames().contains(attributeName))
				{
					DataTable<qreal> newTable(handle->data->numericalData[ ContainerTreeModel::NUMERICAL_DATA[i] ]);
					bool ok;
					double d = value.toDouble(&ok);
					if (ok && newTable.value(attributeName,0) != d)
					{
						newTable.value(attributeName,0) = d;
                        window->changeData(handle->fullName() + tr(".") + attributeName + tr(" = ") + QString::number(d),
											handle,
											ContainerTreeModel::NUMERICAL_DATA[i],
											&newTable);
					}
					return true;
				}
			}
			
			for (int i=0; i < ContainerTreeModel::TEXT_DATA.size(); ++i)
			{
				if (handle->data->textData.contains(ContainerTreeModel::TEXT_DATA[i]) 
					&& handle->data->textData[ ContainerTreeModel::TEXT_DATA[i] ].getRowNames().contains(attributeName))
				{
					DataTable<QString> newTable(handle->data->textData[ ContainerTreeModel::TEXT_DATA[i] ]);
					QString s = value.toString();
					if (!s.isNull() && !s.isEmpty() && newTable.value(attributeName,0) != s)
					{
						newTable.value(attributeName,0) = s;
                        /*window->changeData(handle->fullName() + tr(".") + attributeName + tr(" = ") + s,
											handle,
											ContainerTreeModel::TEXT_DATA[i],
											&newTable);*/
					}
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
			
		ContainerTreeItem *parentItem = childItem->parent();
		if (!parentItem)
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
		if (rootItem)
			rootItem->sortChildren();
	}

}
