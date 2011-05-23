#include "ItemFamily.h"
#include "Ontology.h"
#include "raptor.h"

namespace Tinkercell
{
		NodeFamily * Ontology::nodeFamily(const QString& s)
		{
			if (nodeFamilies.contains(s))
				return nodeFamilies.value(s);
			else
				return 0;
		}
		ConnectionFamily  * Ontology::connectionFamily(const QString& s)
		{
			if (connectionFamilies.contains(s))
				return connectionFamilies.value(s);
			else
				return 0;
		}
		bool Ontology::insertNodeFamily(const QString & s, NodeFamily * ptr)
		{
			if (!ptr || s.isEmpty() || nodeFamilies.contains(s)) return false;
			nodeFamilies.insert(s,ptr);
			return true;
		}
		bool Ontology::insertConnectionFamily(const QString & s, ConnectionFamily * ptr)
		{
			if (!ptr || s.isEmpty() || connectionFamilies.contains(s)) return false;
			connectionFamilies.insert(s,ptr);
			return true;
		}
		QList<NodeFamily*> Ontology::allNodeFamilies()
		{
			return nodeFamilies.values();
		}
		QList<ConnectionFamily*> Ontology::allConnectionFamilies()
		{
			return connectionFamilies.values();
		}
		QStringList Ontology::allNodeFamilyNames()
		{
			return QStringList(nodeFamilies.keys());
		}
		QStringList Ontology::allConnectionFamilyNames()
		{
			return QStringList(connectionFamilies.keys());
		}

		static void read_node_triple(void* user_data, raptor_statement* triple) 
		{
			QString s,p,o;
			NodeFamily *family1 = 0, *family2 = 0;

			if (triple->subject->type == RAPTOR_TERM_TYPE_URI)
				s = QObject::tr((char*)(raptor_uri_as_string(triple->subject->value.uri)));
			else
			if (triple->subject->type == RAPTOR_TERM_TYPE_LITERAL)
				s = QObject::tr((char*)(triple->subject->value.literal.string));

			if (triple->predicate->type == RAPTOR_TERM_TYPE_URI)
				p = QObject::tr((char*)(raptor_uri_as_string(triple->predicate->value.uri)));
			else
			if (triple->predicate->type == RAPTOR_TERM_TYPE_LITERAL)
				p = QObject::tr((char*)(triple->predicate->value.literal.string));

			if (triple->object->type == RAPTOR_TERM_TYPE_URI)
				o = QObject::tr((char*)(raptor_uri_as_string(triple->object->value.uri)));
			else
			if (triple->object->type == RAPTOR_TERM_TYPE_LITERAL)
				o = QObject::tr((char*)(triple->object->value.literal.string));

			if (!s.isEmpty()	 && !p.isEmpty() && !o.isEmpty())
			{
				//insert s as new family
				s = s.toLower();
				family1 = Ontology::nodeFamily(s);
				if (!family1)
				{
					family1 = new NodeFamily(s);
					Ontology::insertNodeFamily(s,family1);
				}
				if (p == QObject::tr("a"))  //if isa relationship
				{
					o = o.toLower(); 
					family2 = Ontology::nodeFamily(o);
					if (!family2)  //insert o as new family
					{
						family2 = new NodeFamily(o);
						Ontology::insertNodeFamily(o,family2);
					}
					family1->setParent(family2);
				}
				else
				if (p.toLower() == QObject::tr("synonyms"))
				{
					QStringList syn = o.split(",");
					for (int i=0; i < syn.size(); ++i)
					{
						QString s2 = syn[i].trimmed().toLower();
						if (!Ontology::nodeFamily(s2))
							Ontology::insertNodeFamily(s2, family1);
					}
				}
				else
				if (p.toLower() == QObject::tr("description"))
				{
					family1->description = o;
				}
				else
				if (p.toLower() == QObject::tr("units") || p.toLower() == QObject::tr("unit"))
				{
					QStringList lst = o.split(",");
					if (lst.size() > 1)
					{
						QString property = lst[0];
						QStringList unitnames = lst[1].split(" ");
						for (int i=0; i < unitnames.size(); ++i)
							family1->measurementUnitOptions += Unit(property, unitnames[i].trimmed());
					}
				}
				else
				if (p.toLower() == QObject::tr("conditions") || p.toLower() == QObject::tr("condition") ||
					 p.toLower() == QObject::tr("restrictions") || p.toLower() == QObject::tr("restriction"))
				{
					family1->restrictions += o.split(",");
				}
				else
				{
					bool ok;
					double d = o.toDouble(&ok);
					if (ok)
						family1->numericalAttributes[p.trimmed()] = d;
					else
						family1->textAttributes[p.trimmed()] = o.trimmed();
				}
			}
		}

		static void read_connection_triple(void* user_data, raptor_statement* triple) 
		{
			QString s,p,o;
			ConnectionFamily *family1 = 0, *family2 = 0;

			if (triple->subject->type == RAPTOR_TERM_TYPE_URI)
				s = QObject::tr((char*)raptor_uri_as_string(triple->subject->value.uri));
			else
			if (triple->subject->type == RAPTOR_TERM_TYPE_LITERAL)
				s = QObject::tr((char*)(triple->subject->value.literal.string));

			if (triple->predicate->type == RAPTOR_TERM_TYPE_URI)
				p = QObject::tr((char*)raptor_uri_as_string(triple->predicate->value.uri));
			else
			if (triple->predicate->type == RAPTOR_TERM_TYPE_LITERAL)
				p = QObject::tr((char*)(triple->predicate->value.literal.string));

			if (triple->object->type == RAPTOR_TERM_TYPE_URI)
				o = QObject::tr((char*)raptor_uri_as_string(triple->object->value.uri));
			else
			if (triple->object->type == RAPTOR_TERM_TYPE_LITERAL)
				o = QObject::tr((char*)(triple->object->value.literal.string));

			if (!s.isEmpty()	 && !p.isEmpty() && !o.isEmpty())
			{
				//insert s as new family
				s = s.toLower();
				family1 = Ontology::connectionFamily(s);
				if (!family1)
				{
					family1 = new ConnectionFamily(s);
					Ontology::insertConnectionFamily(s,family1);
				}
				if (p == QObject::tr("a"))  //if isa relationship
				{
					o = o.toLower(); 
					family2 = Ontology::connectionFamily(o);
					if (!family2)  //insert o as new family
					{
						family2 = new ConnectionFamily(o);
						Ontology::insertConnectionFamily(o,family2);
					}
					family1->setParent(family2);
				}
				else
				if (p.toLower() == QObject::tr("synonyms"))
				{
					QStringList syn = o.split(",");
					for (int i=0; i < syn.size(); ++i)
					{
						QString s2 = syn[i].trimmed().toLower();
						if (!Ontology::connectionFamily(s2))
							Ontology::insertConnectionFamily(s2, family1);
					}
				}
				else
				if (p.toLower() == QObject::tr("description"))
				{
					family1->description = o;
				}
				else
				if (p.toLower() == QObject::tr("units") || p.toLower() == QObject::tr("unit"))
				{
					QStringList lst = o.split(",");
					if (lst.size() > 1)
					{
						QString property = lst[0];
						QStringList unitnames = lst[1].split(" ");
						for (int i=0; i < unitnames.size(); ++i)
							family1->measurementUnitOptions += Unit(property, unitnames[i].trimmed());
					}
				}
				else
				if (p.toLower() == QObject::tr("participants") || p.toLower() == QObject::tr("participant"))
				{
					QStringList lst = o.split(",");
					if (lst.size() > 1)
					{
						QString type = lst[0].trimmed();
						QString role = lst[1].trimmed();
						family1->addParticipant(role, type);
					}
				}
				else
				if (p.toLower() == QObject::tr("conditions") || p.toLower() == QObject::tr("condition") ||
					 p.toLower() == QObject::tr("restrictions") || p.toLower() == QObject::tr("restriction"))
				{
					family1->restrictions += o.split(",");
				}
				else
				{
					bool ok;
					double d = o.toDouble(&ok);
					if (ok)
						family1->numericalAttributes[p.trimmed()] = d;
					else
						family1->textAttributes[p.trimmed()] = o.trimmed();
				}
			}
		}

		static void parse_rdf_file( void (*callback)(void*, raptor_statement*), const char * filename, const char * format)
		{
			raptor_world *world = NULL;
			raptor_parser* rdf_parser = NULL;
			unsigned char *uri_string;
			raptor_uri *uri, *base_uri;
			world = raptor_new_world();
			rdf_parser = raptor_new_parser(world, format);

			raptor_parser_set_statement_handler(rdf_parser, NULL, callback);

			uri_string = raptor_uri_filename_to_uri_string(filename);
			uri = raptor_new_uri(world, uri_string);
			base_uri = raptor_uri_copy(uri);
			raptor_parser_parse_file(rdf_parser, uri, base_uri);
			raptor_free_parser(rdf_parser);
			raptor_free_uri(base_uri);
			raptor_free_uri(uri);
			raptor_free_memory(uri_string);

			raptor_free_world(world);
		}

		void Ontology::readNodes(const QString& rdf, const QString& format)
		{
			parse_rdf_file(&read_node_triple, rdf.toAscii().data(), format.toAscii().data());
		}

		void Ontology::readConnections(const QString& rdf, const QString& format)
		{
			parse_rdf_file(&read_connection_triple, rdf.toAscii().data(), format.toAscii().data());
		}

		void Ontology::writeNodes(const QString& rdfFile, const QString& format)
		{
			raptor_world *world = NULL;
			raptor_serializer* rdf_serializer = NULL;
			unsigned char *uri_string;
			raptor_uri *base_uri;
			raptor_statement* triple;

			world = raptor_new_world();

			uri_string = raptor_uri_filename_to_uri_string(rdfFile.toAscii().data());
			base_uri = raptor_new_uri(world, uri_string);

			rdf_serializer = raptor_new_serializer(world, format.toAscii().data());
			raptor_serializer_start_to_file_handle(rdf_serializer, base_uri, stdout);

			/* Make a triple with URI subject, URI predicate, literal object */
			QList<NodeFamily*> nodes = nodeFamilies.values(), visited;
			for (int i=0; i < nodes.size(); ++i)
				if (nodes[i] && !visited.contains(nodes[i]))
				{
					visited << nodes[i];
					QList<NodeFamily*> parents = nodes[i]->parents();
					for (int j=0; j < parents.size(); ++j)
					{
						triple = raptor_new_statement(world);
						triple->subject = raptor_new_term_from_uri_string(world, (const unsigned char*)nodes[i]->name().toAscii().data());
						triple->predicate = raptor_new_term_from_uri_string(world, (const unsigned char*)"a");
						triple->object = raptor_new_term_from_literal(world,
						                                (const unsigned char*)parents[j]->name().toAscii().data(),
						                                NULL,
						                                (const unsigned char*)"en");

			/* Write the triple */
			raptor_serializer_serialize_statement(rdf_serializer, triple);

			/* Delete the triple */
			raptor_free_statement(triple);

			raptor_serializer_serialize_end(rdf_serializer);
			raptor_free_serializer(rdf_serializer);

			raptor_free_uri(base_uri);
			raptor_free_memory(uri_string);

			raptor_free_world(world);
			return 0;
		}

		void Ontology::writeConnections(const QString& rdfFile)
		{
		}

		QHash<QString, NodeFamily*>  Ontology::nodeFamilies;
		QHash<QString, ConnectionFamily*>  Ontology::connectionFamilies;
}

