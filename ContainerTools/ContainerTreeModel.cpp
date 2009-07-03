/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 A tree model for storing the tree of handles

****************************************************************************/

#include "ContainerTreeModel.h"

namespace Tinkercell
{
	/*****************************************
	      CONTAINER TREE ITEM
	******************************************/
	
	
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
				if (itemHandle->type == ConnectionHandle::Type)
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

	bool ContainerTreeItem::isConnection() const
	{
		return (itemHandle && itemHandle->data && itemHandle->data->textData.contains(QString("Stoichiometry")));
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
				return QVariant(QString("Parameter"));

			return QVariant(QString("Value"));
		}
		if (column == 2)
		{
			if (itemHandle->family() && attributeName == itemHandle->family()->measurementUnit.first
				&& itemHandle->data->numericalData.contains(QString("Initial Value")))
				return QVariant(itemHandle->data->numericalData[QString("Initial Value")].value(0,0));
			else
			if (!attributeName.isEmpty() && itemHandle->data 
				&& itemHandle->data->numericalData.contains(QString("Numerical Attributes")) 
				&& itemHandle->data->numericalData[QString("Numerical Attributes")].rowNames().contains(attributeName))
				return QVariant(itemHandle->data->numericalData[QString("Numerical Attributes")].value(attributeName,0));
		}
		if (column == 1)
		{
			if (itemHandle->family() && attributeName == itemHandle->family()->measurementUnit.first && 
				!attributeName.isEmpty())
				return QVariant(itemHandle->family()->measurementUnit.first);
			else
			if (itemHandle->data && 
				itemHandle->data->numericalData.contains(QString("Numerical Attributes")) && 
				itemHandle->data->numericalData[QString("Numerical Attributes")].rowNames().contains(attributeName))
				return QVariant(attributeName);
		}

		if (column == 0)
			return QVariant(itemHandle->name);

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
		
		QStringList names;
		QHash<QString,ContainerTreeItem*> hash;
		
		for (int i=0; i < childItems.size(); ++i)
		{
			if (childItems[i] && childItems[i]->itemHandle)
			{
				names << childItems[i]->itemHandle->fullName();
				hash.insert( childItems[i]->itemHandle->fullName() , childItems[i] );
				childItems[i]->sortChildren();
			}
		}
		names.sort();
		childItems.clear();
		
		for (int i=0; i < names.size(); ++i)
		{
			if (hash.contains(names[i]))
			{
				ContainerTreeItem* item = hash.value(names[i]);
				if (item && !childItems.contains(item))
					childItems << item;
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
                        QList<ItemHandle*> items = win->allHandles();
			ItemHandle* handle = 0;

			QList<ItemHandle*> visited;
			ContainerTreeItem* treeItem = 0;

			for (int i=0; i < items.size(); ++i)
			{
				handle = items[i];
				if (handle && handle->family() && handle->parent == 0)
				{	
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
				if (queue[i]->itemHandle == handle) return queue[i];
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
			
                        QList<ItemHandle*> items = win->allHandles();
			ItemHandle* handle = 0;

			QList<ItemHandle*> visited;
			ContainerTreeItem* treeItem = 0;

			for (int i=0; i < items.size(); ++i)
			{
				handle = items[i];
				if (handle && handle->family() && handle->parent == 0)
				{	
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
				if (childHandle && childHandle->family() && childHandle->parent == handle)
				{
					ok = false;
					for (int j=0; j < childHandle->graphicsItems.size(); ++j)
						if (childHandle->graphicsItems[j] && childHandle->graphicsItems[j]->isVisible())
						{
							ok = true;
							break;
						}
					
					if (ok && (child = makeBranch(handle->children[i],item)))
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
                                window->rename(handle, value.toString());
		}
		else
		if (index.column() == 1)
		{
			item->text() = value.toString();
		}
		else
		{
			QString attributeName = item->text();
			if (attributeName.isEmpty()) return false;
			if (handle->data->numericalData.contains(QString("Numerical Attributes")) || handle->data->numericalData.contains(QString("Initial Value")))
			{
				//qDebug() << handle->family()->measurementUnit.first;
				if (attributeName == handle->family()->measurementUnit.first)
				{
					DataTable<qreal> * newTable = new DataTable<qreal>(
							handle->data->numericalData[QString("Initial Value")]);
					bool ok;
					double d = value.toDouble(&ok);
					if (ok && newTable->value(0,0) != d)
					{
						newTable->value(0,0) = d;
                                                window->changeData(handle,QString("Initial Value"),newTable);
					}
					delete newTable;
					return true;
				}
				else
				{
					DataTable<qreal> * newTable = new DataTable<qreal>(
							handle->data->numericalData[QString("Numerical Attributes")]);
					bool ok;
					double d = value.toDouble(&ok);
					if (ok && newTable->value(attributeName,0) != d)
					{
						newTable->value(attributeName,0) = d;
                                                window->changeData(handle,QString("Numerical Attributes"),newTable);
					}
					delete newTable;
					return true;
				}

				return false;
			}
			else
				return false;
		}
		return true;
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
