#include <QtGui>
#include "SpinBoxDelegate.h"

namespace Tinkercell
{

	SpinBoxDelegate::SpinBoxDelegate(QObject *parent) : QItemDelegate(parent)
	{
	}

	QWidget *SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex & index ) const
	{
		QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
		if (suffix.size() > index.row())
			editor->setSuffix(suffix[index.row()]);
		editor->setMinimum(-1e300);
		editor->setMaximum(1E300);
		editor->setDecimals(10);
		return editor;
	}

	void SpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
	{
		double value = index.model()->data(index, Qt::EditRole).toDouble();
		QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
		spinBox->setValue(value);
	}

	void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
		QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
		spinBox->interpretText();
		double value = spinBox->value();
		model->setData(index, value, Qt::EditRole);
	}

	void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
	{
		editor->setGeometry(option.rect);
	}


	ComboBoxDelegate::ComboBoxDelegate(const QStringList& items, QObject *parent) : QItemDelegate(parent), entries(items), onlyThisColumn(-1)
	{
	}

	QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex & index) const
	{
		if (onlyThisColumn < 0 || index.column() == onlyThisColumn)
		{
			QComboBox *editor = new QComboBox(parent);
			editor->addItems(entries);
			return editor;
		}
		else
		{
			QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
			editor->setMinimum(-1e300);
			editor->setMaximum(1E300);
			editor->setDecimals(10);
			if (suffix.size() > index.row())
				editor->setSuffix(suffix[index.row()]);
			return editor;
		}
	}

	void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
	{
		if (onlyThisColumn < 0 || index.column() == onlyThisColumn)
		{
			QString value = index.model()->data(index, Qt::EditRole).toString();
			QComboBox *combo = static_cast<QComboBox*>(editor);
			combo->setCurrentIndex(entries.indexOf(value));
		}
		else
		{
			double value = index.model()->data(index, Qt::EditRole).toDouble();
			QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
			spinBox->setValue(value);
		}
	}

	void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
		if (onlyThisColumn < 0 || index.column() == onlyThisColumn)
		{
			QComboBox *combo = static_cast<QComboBox*>(editor);
			QString value = combo->currentText();
			model->setData(index, value, Qt::EditRole);
		}
		else
		{
			QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
			spinBox->interpretText();
			double value = spinBox->value();
			model->setData(index, value, Qt::EditRole);
		}
	}

	void ComboBoxDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
	{
		editor->setGeometry(option.rect);
	}

	/*********************
	THREE DELEGATES
	*********************/

	TextComboDoubleDelegate::TextComboDoubleDelegate(const QStringList& items, QObject *parent) : 
	QItemDelegate(parent), entries(items), textColumn(-1), comboBoxColumn(-1), doubleColumn(-1)
	{
	}

	QWidget *TextComboDoubleDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex & index) const
	{
		if (index.column() <= textColumn)
		{
			QLineEdit *editor = new QLineEdit(parent);
			return editor;
		}
		else
			if (index.column() == comboBoxColumn)
			{
				QComboBox *editor = new QComboBox(parent);
				editor->addItems(entries);
				return editor;
			}
			else
			{
				QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
				editor->setMinimum(-1e300);
				editor->setMaximum(1E300);
				editor->setDecimals(10);
				if (suffix.size() > index.row())
					editor->setSuffix(suffix[index.row()]);
				return editor;
			}
	}

	void TextComboDoubleDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
	{
		if (index.column() <= textColumn)
		{
			QString value = index.model()->data(index, Qt::EditRole).toString();

			if (index.column() == suffixColumn)
			{
				value.replace(suffix[index.row()],QString(""));
				value = value.trimmed();
			}

			QLineEdit * line = static_cast<QLineEdit*>(editor);
			line->setText(value);
		}
		else
			if (index.column() == comboBoxColumn)
			{
				QString value = index.model()->data(index, Qt::EditRole).toString();
				QComboBox *combo = static_cast<QComboBox*>(editor);
				combo->setCurrentIndex(entries.indexOf(value));
			}
			else
			{
				double value = index.model()->data(index, Qt::EditRole).toDouble();
				QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
				spinBox->setValue(value);
			}
	}

	void TextComboDoubleDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
		if (index.column() <= textColumn)
		{
			QLineEdit *line = static_cast<QLineEdit*>(editor);
			QString value = line->text().trimmed();
			if (index.column() == suffixColumn)
			{
				bool ok;
				value.toDouble(&ok);
				if (!ok) return;

				value += QString(" ") + suffix[index.row()];
			}
			model->setData(index, value, Qt::EditRole);
		}
		else
			if (index.column() == comboBoxColumn)
			{
				QComboBox *combo = static_cast<QComboBox*>(editor);
				QString value = combo->currentText();
				model->setData(index, value, Qt::EditRole);
			}
			else
			{
				QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
				spinBox->interpretText();
				double value = spinBox->value();
				model->setData(index, value, Qt::EditRole);
			}
	}

	void TextComboDoubleDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
	{
		editor->setGeometry(option.rect);
	}
}
