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

class TINKERCELLEXPORT FamilyTreeButton : public QToolButton
{
	Q_OBJECT

signals:
	void nodeSelected(NodeFamily*);
	void connectionSelected(ConnectionFamily*);
	void pressed(const QString& name, const QPixmap& pixmap);
	
public:
	FamilyTreeButton(NodeFamily* family = 0, QWidget * parent = 0);
	FamilyTreeButton(ConnectionFamily* family, QWidget * parent = 0);
	FamilyTreeButton(const QString& custon, QWidget * parent = 0);
	ItemFamily * family() const;

protected slots:
	void about();
	void replaceAction();
	
protected:
	virtual void contextMenuEvent(QContextMenuEvent * event);
	//virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent * event);
	QMenu menu;
	
	QString name;
	NodeFamily * nodeFamily;
	ConnectionFamily* connectionFamily;
	QString newFileName;
	
	NodesTree * nodesTree;
	friend class NodesTree;
};


}

#endif
