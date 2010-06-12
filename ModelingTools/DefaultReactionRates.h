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
			if (!handle || !handle->data) return;

			QList<NodeHandle*> nodes = handle->nodes();

			NumericalDataTable reactants, products;
			TextDataTable rates;
			
			rates.value(handle->fullName(),QObject::tr("rate")) = QString("0.0");
			if (handle->hasNumericalData(QString("Parameters")))
			{
				handle->data->numericalData[QString("Parameters")].value(QString("k0"),0) = 1.0;
				rates.value(handle->fullName(),QObject::tr("rate")) = handle->fullName() + QString(".k0");
			}

			nodes.clear();
			QList<NodeHandle*> connectedNodes = handle->nodes();
			QList<NodeHandle*> nodesIn = handle->nodesIn();
			QList<NodeHandle*> nodesOut = handle->nodesOut();
			NodeHandle * node;
			QStringList names;
			
			int k;


			for (int i=0; i < connectedNodes.size(); ++i)
			{
				node = connectedNodes[i];
				if (node && !(node->isA(QString("Empty"))))
				{
					k = nodesIn.indexOf(node);
					if (k > -1)
					{
						reactants.value(handle->fullName(), node->fullName()) += 1.0;
						rates.value(handle->fullName(),QObject::tr("rate")) += QString("*") + node->fullName();						
					}
					else
					{
						k = nodesOut.indexOf(node);
						if (k > -1)
						{
							products.value( handle->fullName(), node->fullName()) += 1.0;
						}
					}
				}
			}
	
			reactants.description() = QString("Number of each reactant participating in this reaction");
			products.description() = QString("Number of each product participating in this reaction");
			rates.description() = QString("Rates: a set of rates, one for each reaction represented by this item. Row names correspond to reaction names. The number of rows in this table and the stoichiometry table will be the same.");

			handle->data->numericalData.insert(QString("Reactant stoichiometries"),reactants);
			handle->data->numericalData.insert(QString("Product stoichiometries"),products);
			handle->data->textData.insert(QString("Rate equations"),rates);

		}
	};
}

#endif
