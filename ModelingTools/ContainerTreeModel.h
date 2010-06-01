/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 A tree model for storing the tree of handles

****************************************************************************/

#ifndef TINKERCELL_CONTAINERTREEMODEL_H
#define TINKERCELL_CONTAINERTREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QVariant>

#include "NetworkHandle.h"
#include "ItemHandle.h"
#include "Tool.h"

namespace Tinkercell
{
	class ContainerTreeModel;
	
	class ContainerTreeItem
	{
		friend class ContainerTreeModel;

	public:

		ContainerTreeItem(ItemHandle* handle = 0, ContainerTreeItem *parent = 0);
		~ContainerTreeItem();

		void appendChild(ContainerTreeItem *child);

		ContainerTreeItem *child(int row);
		int childCount() const;
		int columnCount() const;
		QVariant data(int column) const;
		int row() const;
		ContainerTreeItem *parent();
		ItemHandle * handle();
		QString& text();
		void sortChildren();

	private:

		void populateAttributes();
		QList<ContainerTreeItem*> childItems;	
		ContainerTreeItem *parentItem;
		ItemHandle * itemHandle;
		QString attributeName;
	};

	class ContainerTreeModel : public QAbstractItemModel
	{
		Q_OBJECT

	public:
		
		ContainerTreeModel(NetworkHandle * net = 0, QObject *parent = 0);
        void reload(NetworkHandle *);
		~ContainerTreeModel();

		QVariant data(const QModelIndex &index, int role) const;
		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
		Qt::ItemFlags flags(const QModelIndex &index) const;
		QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;
		QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const;
		QModelIndex parent(const QModelIndex &index) const;
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount(const QModelIndex &parent = QModelIndex()) const;

		ContainerTreeItem * root();
		
		virtual void sort ( int column, Qt::SortOrder order = Qt::AscendingOrder );

	private:	
		ContainerTreeItem *rootItem;
		NetworkHandle * network;
		ContainerTreeItem* makeBranch(ItemHandle*,ContainerTreeItem*);
		ContainerTreeItem* findTreeItem(ItemHandle*);		
	
	public:
		static QStringList NUMERICAL_DATA;
		static QStringList TEXT_DATA;
	};
}


#endif
