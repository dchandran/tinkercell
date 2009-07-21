/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT
 
 XML reader that populates the connections tree

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
#include "NodesTree/NodesTree.h"
#include "NodesTree/ConnectionsTree.h"

namespace Tinkercell
{

class ConnectionsTree;
class ConnectionFamily;

/*! \brief This class reads an XML file and loads the tree of connections from it.
   \ingroup plugins
 */
class ConnectionsTreeReader : public QXmlStreamReader
{	
public:
	void readXml(ConnectionsTree* tree, const QString & filename);	
	void readTree(ConnectionsTree* tree, QIODevice * device);
private:
	QPair<ConnectionFamily*,QTreeWidgetItem*> readConnection(ConnectionsTree* tree, ConnectionFamily* parentNode = 0);
};

}

#endif
