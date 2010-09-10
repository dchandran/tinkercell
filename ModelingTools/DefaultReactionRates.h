/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This tool places a stoichiometry table and a table of rates inside all connection handles.
An associated GraphicsTool is also defined. This allow brings up a table for editting
the stoichiometry and rates tables.

****************************************************************************/

#ifndef TINKERCELL_DEFAULTREACTIONRATESTOOL_H
#define TINKERCELL_DEFAULTREACTIONRATESTOOL_H

#include "MainWindow.h"
#include "DataTable.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "ConsoleWindow.h"


namespace Tinkercell
{
	class DefaultRateAndStoichiometry
	{
	public:
		static void setDefault(ConnectionHandle * handle)
		{
			if (!handle) return;

			QList<NodeHandle*> nodes = handle->nodes();

			NumericalDataTable reactants, products;
			TextDataTable rates;
			
			rates.value(QObject::tr("rate"),QObject::tr("formula")) = QObject::tr("0.0");
			if (handle->hasNumericalData(QObject::tr("Parameters")))
			{
				handle->numericalDataTable(QObject::tr("Parameters")).value(QObject::tr("k0"),0) = 1.0;
				rates.value(QObject::tr("rate"),QObject::tr("formula")) = handle->fullName() + QObject::tr(".k0");
			}

			nodes.clear();
			QList<NodeHandle*> connectedNodes = handle->nodes();
			TextDataTable & participants = handle->textDataTable(QObject::tr("Participants"));
			NodeHandle * node;
			QStringList names;
			
			int k;
			for (int i=0; i < connectedNodes.size(); ++i)
			{
				node = connectedNodes[i];
				if (node && !(node->isA(QString("Empty"))))
				{
					if (participants.value(node->fullName(),0).toLower().contains(QObject::tr("reactant")))
					{
						reactants.value(QObject::tr("stoichiometry"), node->fullName()) += 1.0;
						rates.value(QObject::tr("rate"),QObject::tr("formula")) += QObject::tr("*") + node->fullName();						
					}
					else
					if (participants.value(node->fullName(),0).toLower().contains(QObject::tr("product")))
					{
						products.value( QObject::tr("stoichiometry"), node->fullName()) += 1.0;
					}
				}
			}
	
			reactants.description() = QString("Number of each reactant participating in this reaction");
			products.description() = QString("Number of each product participating in this reaction");
			rates.description() = QString("Rates: a set of rates, one for each reaction represented by this item. Row names correspond to reaction names. The number of rows in this table and the stoichiometry table will be the same.");

			handle->numericalDataTable(QObject::tr("Reactant stoichiometries")) = reactants;
			handle->numericalDataTable(QObject::tr("Product stoichiometries")) = products;
			handle->textDataTable(QObject::tr("Rate equations")) = rates;

		}
	};
}

#endif
