#ifndef TINKERCELL_SPINBOXDELEGATE_H
#define TINKERCELL_SPINBOXDELEGATE_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QDoubleSpinBox>

namespace Tinkercell
{
	/*! \brief this delegate is used by the ModelSummaryTool in order to create a table with spin box widgets*/
	class SpinBoxDelegate : public QItemDelegate
	{
		Q_OBJECT

	public:
		SpinBoxDelegate(QObject *parent = 0);
		QStringList suffix;
		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
		void setEditorData(QWidget *editor, const QModelIndex &index) const;
		void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
		void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const;
	};

	/*! \brief this delegate is used by the ModelSummaryTool in order to create a table with two types 
		of widgets -- spin box, and combo box*/
	class ComboBoxDelegate : public QItemDelegate
	{
		Q_OBJECT

	public:
		ComboBoxDelegate(const QStringList&, QObject *parent = 0);
		QStringList entries;
		int onlyThisColumn;
		QStringList suffix;
		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
		void setEditorData(QWidget *editor, const QModelIndex &index) const;
		void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
		void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const;
	};

	/*! \brief this delegate is used by the ModelSummaryTool in order to create a table with three types 
		of widgets -- text, spin box, and combo box*/
	class TextComboDoubleDelegate : public QItemDelegate
	{
		Q_OBJECT

	public:
		TextComboDoubleDelegate(const QStringList&, QObject *parent = 0);
		QStringList entries;
		int textColumn;
		int suffixColumn;
		int comboBoxColumn;
		int doubleColumn;
		QStringList suffix;
		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
		void setEditorData(QWidget *editor, const QModelIndex &index) const;
		void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
		void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const;
	};

}

#endif
