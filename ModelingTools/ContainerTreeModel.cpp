/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 A tree model for storing the tree of handles

****************************************************************************/

#include "EquationParser.h"
#include "ItemFamily.h"
#include "ContainerTreeModel.h"
#include "BasicInformationTool.h"

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
	}

	ContainerTreeItem::~ContainerTreeItem()
	{
		qDeleteAll(childItems);
	}

	void ContainerTreeItem::appendChild(ContainerTreeItem *item)
	{
		if (!childItems.contains(item))
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

		if (childItems.isEmpty() && !attributeName.isEmpty())
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
						return QVariant(itemHandle->textData(QString("Rate equations")));
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
				hash[family->name].insertMulti(handle->fullName(),childItems[i]);
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
			if (net != this->network)
			{
				delete rootItem;
				rootItem = 0;
				
				for (int i=0; i < markedForDeletion.size(); ++i)
					if (markedForDeletion[i])
						delete markedForDeletion[i];
		
				markedForDeletion.clear();
			}
			
			ContainerTreeItem * newRootItem = new ContainerTreeItem;
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
							newRootItem->appendChild(treeItem);
					}
				}
			
			if (rootItem)
				delete rootItem;
			rootItem = newRootItem;
			emit layoutChanged();
		}
	}
	
	ContainerTreeItem* ContainerTreeModel::makeBranch(ItemHandle* handle, ContainerTreeItem * parentItem, const QString & attribute)
	{
		if (!handle) return 0;
		
		ContainerTreeItem * item;
		
		if (attribute.isEmpty())
			item = findTreeItem(rootItem, handle,attribute);
		else
			item = findTreeItem(parentItem, handle,attribute);
		
		if (item)
		{
			if (item->parentItem && item->parentItem != parentItem)
			{
				item->parentItem->childItems.removeAll(item);
			}
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

		if (!item || item->childItems.size() != k)
		{
			if (item)
			{
				if (item->parentItem)
					item->parentItem = 0;
				item->childItems.clear();
				if (!markedForDeletion.contains(item))
					markedForDeletion << item;
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
						item->appendChild(child);
				}
			}
		}
		
		return item;
	}

	ContainerTreeModel::~ContainerTreeModel()
	{
		if (rootItem)
		{
			markedForDeletion.removeAll(rootItem);
			delete rootItem;
		}
		
		for (int i=0; i < markedForDeletion.size(); ++i)
			if (markedForDeletion[i])
				delete markedForDeletion[i];

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
				if (ConnectionHandle::cast(handle))
				{
					return QVariant(tr("Reaction rate equation"));
				}
				else
				{
					return QVariant(handle->family()->measurementUnit.property);
				}
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
				else
					if (handle->hasTextData(QString("Rate equations")))
					{
						QString s = value.toString();
						if (EquationParser::validate(network, handle, s, QStringList() << "time"))
						{
							 TextDataTable newTable(handle->data->textData[ QString("Rate equations") ]);
							 newTable.value(0,0) = s;
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
		if (rootItem)		
			rootItem->sortChildren();
	}

}
