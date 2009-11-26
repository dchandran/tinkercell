/****************************************************************************

 Copyright (C) 2009 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYWRITE.TXT

 Provides a widget for selecting different types of cells
****************************************************************************/
#include "CellTypeSelector.h"
#include "ConsoleWindow.h"

using namespace Tinkercell;

namespace Multicell
{

	CellTypeSelector::CellTypeSelector(MainWindow * mainWindow, QWidget * parent) :
        QListWidget(parent), buttonGroup(this), main(mainWindow)
	{
		setSpacing(20);
		buttonGroup.setExclusive(true);
		connect(&buttonGroup,SIGNAL( buttonPressed ( int )),this,SLOT(buttonPressed(int)));

		QListWidgetItem * item = new QListWidgetItem;
		addItem (item);

		QToolButton * toolButton = new QToolButton(this);
		toolButton->setIcon(QIcon(":/images/arrow.png"));

		setItemWidget(item,toolButton);

		buttonGroup.addButton(toolButton,0);
	}

	void CellTypeSelector::buttonPressed ( int i )
	{
		if (i == 0)
			emit cellTypeSelected(0, QColor());
		else
		{
			--i;
			if (i < cellColors.size() && i < cellFamilies.size())
				emit cellTypeSelected(cellFamilies[i],cellColors[i]);
            if (main && main->console())
                main->console()->message(cellFamilies[i]->name + tr(" selected"));
		}
	}


	void CellTypeSelector::addCellType(NodeFamily* family,const QColor& color)
	{
		if (!family) return;
		cellColors << color;
		cellFamilies << family;

		QListWidgetItem * item = new QListWidgetItem(family->name);
		addItem ( item );

		QToolButton * toolButton = new QToolButton(this);
		QString style = tr("background-color: ") + color.name() + tr(";");
		toolButton->setStyleSheet(style);

		setItemWidget(item,toolButton);

		buttonGroup.addButton(toolButton,cellFamilies.size());

		if (family)
            if (main->console())
                main->console()->message(family->name + tr(" added"));
	}

	void CellTypeSelector::addCellType()
	{
	}

	void CellTypeSelector::removeCellType()
	{
	}

	CellTypeSelector::~CellTypeSelector()
	{
		for (int i=0; i < cellFamilies.size(); ++i)
			if (cellFamilies[i])
			{
				for (int j=(i+1); j < cellFamilies.size(); ++j) //avoid duplicate deletes
					if (cellFamilies[j] == cellFamilies[i])
						cellFamilies[j] = 0;

				delete cellFamilies[i];
				cellFamilies[i] = 0;
			}
	}
}

