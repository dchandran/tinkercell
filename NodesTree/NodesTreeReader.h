/****************************************************************************

 Copyright (C) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 XML reader that populates the nodes tree

****************************************************************************/

#ifndef TINKERCELL_PARTSTREEREADER_H
#define TINKERCELL_PARTSTREEREADER_H

#include <QString>
#include <QHash>
#include <QIcon>
#include <QIODevice>
#include <QStatusBar>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QMessageBox>
#include <QTreeWidget>
#include <QPair>
#include "NodesTree.h"

namespace Tinkercell
{

class NodesTree;
class IDrawable;
class NodeFamily;

/*! \brief This class reads an XML file and loads the tree of nodes from it.
   \ingroup plugins
 */
class NodesTreeReader : public QXmlStreamReader
{	
public:
	QStringList readXml(NodesTree* tree, const QString & filename);	
	QStringList readTree(NodesTree* tree, QIODevice * device);
private:
	QPair<NodeFamily*,QTreeWidgetItem*> readNode(NodesTree* tree, NodeFamily* parentNode = 0);
	QList< QPair<NodeFamily*,QString> > compositeNodes;
};

}

#endif
