/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 A tree model for storing the tree of handles

****************************************************************************/

#include "EquationParser.h"
#include "ItemFamily.h"
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
		attributeName = QString();
		/*
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
		}*/
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
		
		
		if (childItems.isEmpty())
		{
			if (column == 0)
				return QVariant(attributeName);
			else
				if (itemHandle->data->numericalData[QString("Parameters")].getRowNames().contains(attributeName))
					return QVariant(itemHandle->numericalData(QString("Parameters"),attributeName,0));
		}
		else
		{
			if (column == 0)
				return QVariant(itemHandle->name);
			else
			{
				if (itemHandle->hasNumericalData(QString("Initial Value")))
					return QVariant(itemHandle->numericalData(QString("Initial Value")));
				else
					if (itemHandle->hasTextData(QString("Rate equations")))
						return QVariant(itemHandle->texData(QString("Rate equations")));
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

	ContainerTreeModel::ContainerTreeModel(NetworkHandle * net, QObject *parent)
	 : QAbstractItemModel(parent)
	{
		rootItem = new ContainerTreeItem;
		this->network = net;

        if (net)
		{
            QList<ItemHandle*> items = net->allHandlesSortedByFamily();
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
			if (win != this->network)
			{
				delete rootItem;
				rootItem = 0;
			}
			
			ContainerTreeItem * rootItemNew = new ContainerTreeItem;
            this->network = win;
			
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
	
	void ContainerTreeModel::populateAttributes()
	{
		if (!itemHandle || !itemHandle->hasNumericalData(QString("Parameters"))) return;
		
		NumericalDataTable & attributes = itemHandle->data->numericalData[QString("Parameters")];
		
		bool found;
		for (int i=0; i < attributes.rows(); ++i)
		{
			found = false;
			for (int i=0; j < childItems.size(); ++j)
				if (childItems[j] && childItems[j]->attributeName == attributes.rowName(i))
				{
					found = true;
					break;
				}
			if (!found)
			{
				ContainerTreeModel * item = new ContainerTreeItem(itemHandle,this);
				item->attributeName = attributes.rowName(i);
				item->appendChild(child);
			}
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
		
		item->populateAttributes();
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
        if (!index.isValid() || !network)
			return false;

		ContainerTreeItem *item = static_cast<ContainerTreeItem*>(index.internalPointer());

		if (!item) return false;

		ItemHandle * handle = item->handle();
		
		QString attributeName = item->text();
		
		if (!handle || !handle->data || !handle->family()) return false;

		if (index.column() == 0)
		{
            if (network 
            	&& handle
				&& !value.toString().isEmpty()
				&& value.toString() != handle->name)
			{
				if (attributeName.isEmpty())
					network->rename(handle, value.toString());
				else
					network->rename(handle->fullName() + tr(".") + attributeName, handle->fullName() + tr(".") + value.toString());
				return true;
			}
		}
		else
		if (index.column() == 1)
		{			
			if (attributeName.isEmpty())
			{
				if (handle->hasNumericalData(QString("Initial Value")))
				{
					NumericalDataTable newTable(handle->data->numericalData[ QString("Initial Value") ]);
					bool ok;
					double d = value.toDouble(&ok);
					if (ok && newTable.value(attributeName,0) != d)
					{
						newTable.value(attributeName,0) = d;
                        network->changeData(handle->fullName() + tr(" = ") + QString::number(d),
											handle,
											QString("Initial Value"),
											&newTable);
						return true;
					}
				}
				else
					if (handle->hasTextData(QString("Rate equations")))
					{
						QString s = value.toString();
						if (EquationParser::validate(network, handle, s, QStringList() << "time"))
						{
							 TextDataTable newTable(handle->data->textData[ QString("Rate equations") ]);
							 network->changeData(handle->fullName() + tr("'s rate = ") + s,
											handle,
											QString("Rate equations"),
											&newTable);
							return true;
						}
					}
			}
			else
				if (handle->hasNumericalData(QString("Parameters")))
				{
					DataTable<qreal> newTable(handle->data->numericalData[ QString("Parameters") ]);
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
