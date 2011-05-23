/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This class is used to load and store a set of NodeFamily instances and ConnectionFamily
instances. The families are loaded from RDF files.

****************************************************************************/

#ifndef TINKERCELL_FAMILYONTOLOGY_H
#define TINKERCELL_FAMILYONTOLOGY_H

#include <QList>
#include <QStringList>
#include <QHash>

#ifdef Q_WS_WIN
#define TINKERCELLEXPORT __declspec(dllexport)
#else
#define TINKERCELLEXPORT
#endif

namespace Tinkercell
{
	class ItemFamily;
	class NodeFamily;
	class ConnectionFamily;
	
	/*! \brief A set of node and connection families. All functions are static
	\ingroup core
	*/
	class TINKERCELLEXPORT Ontology
	{
	public:
		/*! \brief get a family pointer given its name (not case-sensitive)
			\param QString& name of family
			\return NodeFamily* can be 0 if name does not exist
		*/
		static NodeFamily * nodeFamily(const QString&);
		/*! \brief get a family pointer given its name (not case-sensitive)
			\param QString& name of family
			\return ConnectionFamily* can be 0 if name does not exist
		*/	
		static ConnectionFamily * connectionFamily(const QString&);
		/*! \brief add a new family to the ontology
			\param QString family name
			\param NodeFamily* new family
			\return bool did the insertion succeed
		*/	
		static bool insertNodeFamily(const QString &, NodeFamily * );
		/*! \brief add a new family to the ontology
			\param QString family name
			\param ConnectionFamily* new family
			\return bool did the insertion succeed
		*/	
		static bool insertConnectionFamily(const QString &, ConnectionFamily *);
		/*! \brief get list of all node families
			\param QList<NodeFamily*>
		*/	
		static QList<NodeFamily*> allNodeFamilies(); 
		/*! \brief get list of all connection families
			\param QList<ConnectionFamily*>
		*/	
		static QList<ConnectionFamily*> allConnectionFamilies();
		/*! \brief get list of all node family names
			\param QStringList
		*/	
		static QStringList allNodeFamilyNames(); 
		/*! \brief get list of all connection family names
			\param QStringList
		*/	
		static QStringList allConnectionFamilyNames();

		/*! \brief read RDF file and insert node families
			\param QString format, defaults to rdfxml
			\return QString file name
		*/	
		static void readNodes(const QString& rdfFile, const QString& format=QString("rdfxml"));
		/*! \brief read RDF file and insert connection families
			\param QString format, defaults to rdfxml
			\return QString file name
		*/	
		static void readConnections(const QString& rdfFile, const QString& format=QString("rdfxml"));

		/*! \brief write RDF file for node families
			\param QString format, defaults to rdfxml
			\return QString file name
		*/	
		static void writeNodes(const QString& rdfFile, const QString& format=QString("rdfxml-abbrev"));
		/*! \brief write RDF file for connection families
			\param QString format, defaults to rdfxml
			\return QString file name
		*/	
		static void writeConnections(const QString& rdfFile, const QString& format=QString("rdfxml-abbrev"));

	private:
		/*! \brief maps name to node families*/
		static QHash<QString, NodeFamily*> nodeFamilies;
		/*! \brief maps name to connection families*/
		static QHash<QString, ConnectionFamily*> connectionFamilies;
	};
}

#endif

