/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 

****************************************************************************/

#ifndef TINKERCELL_LPSOLVEINPUT_H
#define TINKERCELL_LPSOLVEINPUT_H

#include "AbstractInputWindow.h"

namespace Tinkercell
{

class LPSolveInputWindow : public AbstractInputWindow
{
	Q_OBJECT
	
public:

	LPSolveInputWindow();
	
	bool setMainWindow(MainWindow * main);
	
	void loadConstraints();
	
public slots:
	
	void itemsInserted(NetworkHandle *, const QList<ItemHandle*>&);
	
	void dataChanged(const QList<ItemHandle*>&);
	
	void itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& , QPointF , Qt::KeyboardModifiers );
	
	void checkboxSelected(bool checked);
	
	//add constraint
	void addRow();
	
	void removeRow();
	
	void setVisible(bool);
	
	void exec();
	
private slots:

	void toolLoaded(Tool * tool);
	
	void objectivesTableChanged(int i,int j);
	
	void constraintsTableChanged(int i,int j);
	
	void showLogFile();
	
private:
	DataTable<qreal> dataTable;
	int N;
	QStringList targetFluxes;
	QTableWidget objectivesTable, constraintsTable;
	void update(bool mustBeVisible = true);
	
	/*! \brief delegate used to change table items */
	class ComboBoxDelegate : public QItemDelegate
	{
	public:
	
		QList<QStringList> options;
	
		ComboBoxDelegate(QObject *parent = 0) : QItemDelegate(parent)
		{
		}

		QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex & index) const
		{
			if (options.size() > index.column())
			{
				QComboBox *editor = new QComboBox(parent);
				editor->addItems(options[ index.column() ]);
				return editor;
			}
			else
			{
				QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
				editor->setMinimum(-1e300);
				editor->setMaximum(1E300);
				editor->setDecimals(10);
				return editor;
			}
		}

		void setEditorData(QWidget *editor, const QModelIndex &index) const
		{
			if (options.size() > index.column())
			{
				QString value = index.model()->data(index, Qt::EditRole).toString();
				QComboBox *combo = static_cast<QComboBox*>(editor);
				combo->setCurrentIndex(options[ index.column() ].indexOf(value));
			}
			else
			{
				double value = index.model()->data(index, Qt::EditRole).toDouble();
				QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
				spinBox->setValue(value);
			}
		}

		void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
		{
			if (options.size() > index.column())
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

		void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
		{
			editor->setGeometry(option.rect);
		}
	};
	
	ComboBoxDelegate delegate1,
					 delegate2;
};

}

//extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif

