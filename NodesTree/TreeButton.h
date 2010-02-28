/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 

****************************************************************************/

#ifndef TINKERCELL_TREEBUTTONWIDGET_H
#define TINKERCELL_TREEBUTTONWIDGET_H


#include <QWidget>
#include <QHash>
#include <QIcon>
#include <QPixmap>
#include <QToolButton>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QActionGroup>
#include "Tool.h"
#include "ItemFamily.h"
#include "ItemHandle.h"
#include "NodesTree.h"

namespace Tinkercell
{

class NodesTree;

class FamilyTreeButton : public QToolButton
{
	Q_OBJECT

signals:
	void nodeSelected(NodeFamily*);
	void connectionSelected(ConnectionFamily*);

public:
	FamilyTreeButton(NodeFamily* family = 0, QWidget * parent = 0);
	FamilyTreeButton(ConnectionFamily* family, QWidget * parent = 0);
	ItemFamily * family() const;

protected slots:
	void about();
	void replaceAction();
	
protected:
	virtual void contextMenuEvent(QContextMenuEvent * event);
	virtual void mousePressEvent(QMouseEvent * event);
	
	QMenu menu;
	NodeFamily * nodeFamily;
	ConnectionFamily* connectionFamily;
	QString newFileName;
	
	NodesTree * nodesTree;
	friend class NodesTree;
};


}

#endif
