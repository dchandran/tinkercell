/****************************************************************************

 Copyright (C) 2009 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYWRITE.TXT

 Provides a widget for selecting different types of cells
****************************************************************************/


#ifndef MULTICELL_INTERFACE_CELL_SELECTOR_TOOL
#define MULTICELL_INTERFACE_CELL_SELECTOR_TOOL

#include <QButtonGroup>
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QToolButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QListWidget>
#include <QGraphicsSimpleTextItem>
#include "ItemFamily.h"
#include "MainWindow.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

using namespace Tinkercell;

namespace Multicell
{

	class MY_EXPORT CellTypeSelector : public QListWidget
	{
		Q_OBJECT

	public:
		QList<QColor> cellColors;
		QList<NodeFamily*> cellFamilies;

		CellTypeSelector(MainWindow * , QWidget * parent = 0);
		virtual ~CellTypeSelector();

	signals:
		void cellTypeSelected(NodeFamily*,const QColor&);

	public slots:
		void addCellType(NodeFamily*,const QColor&);
		void addCellType();
		void removeCellType();

	private slots:
		void buttonPressed ( int );

	private:
		QButtonGroup buttonGroup;
        MainWindow * main;

	};

}

#endif
