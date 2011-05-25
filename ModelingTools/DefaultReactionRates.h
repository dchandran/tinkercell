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

			nodes.clear();
			QList<NodeHandle*> connectedNodes = handle->nodes();
			TextDataTable participants = handle->textDataTable(QObject::tr("Participants"));
			NodeHandle * node;
			QStringList names;
			
			int k;
			for (int i=0; i < connectedNodes.size(); ++i)
			{
				node = connectedNodes[i];
				if (node && !(node->isA(QString("Empty"))))
				{
					bool isReactant = false;
					bool isProduct = false;
					
					for (int j=0; j < participants.rows(); ++j)
						if (participants.value(j,0) == node->fullName())
						{
							participants.value(j,0) = QObject::tr("");
							
							isReactant = ( participants.rowName(j).toLower().contains(QObject::tr("reactant")) ||
													participants.rowName(j).toLower().contains(QObject::tr("substrate")));
						
							isProduct = ( participants.rowName(j).toLower().contains(QObject::tr("product")) );
							
							if (isReactant || isProduct)
								break;
						}
					
					if (isReactant)
					{
						reactants.value(QObject::tr("stoichiometry"), node->fullName()) += 1.0;
						products.value(QObject::tr("stoichiometry"), node->fullName()) += 0.0;
						rates.value(QObject::tr("rate"),QObject::tr("formula")) += QObject::tr("*") + node->fullName();						
					}
					if (isProduct)
					{
						products.value( QObject::tr("stoichiometry"), node->fullName()) += 1.0;
						reactants.value( QObject::tr("stoichiometry"), node->fullName()) += 0.0;
					}
				}
			}
			
			if (handle->isA("Multimerization"))
			{
				reactants.setRowName(0,QObject::tr("forward"));
				products.setRowName(0,QObject::tr("forward"));
				rates.setRowName(0,QObject::tr("forward"));

				reactants.setRowName(1,QObject::tr("reverse"));
				products.setRowName(1,QObject::tr("reverse"));
				rates.setRowName(1,QObject::tr("reverse"));
					
				reactants.setRowName(2,QObject::tr("diffuse"));
				products.setRowName(2,QObject::tr("diffuse"));
				rates.setRowName(2,QObject::tr("diffuse"));
				
				int reac = 0, prod = 1;
				if (products.value(0,0) > 0.0)
				{
					reac = 1;
					prod = 0;
				}

				double n = handle->numericalData(QObject::tr("Parameters"), QObject::tr("monomers"), QObject::tr("value"));
				reactants.value(0,reac) = n;
				products.value(0,prod) = 1.0;
					
				reactants.value(1,prod) = 1.0;
				products.value(1,reac) = n;					
				reactants.value(2,prod) = 1.0;
					
				rates.value(0,0) = handle->fullName() + QObject::tr(".kf * ") + reactants.columnName(reac) + QObject::tr("^") + QString::number(n);
				rates.value(1,0) = handle->fullName() + QObject::tr(".kb * ") + reactants.columnName(prod);
				rates.value(2,0) = handle->fullName() + QObject::tr(".deg * ") + reactants.columnName(prod);
				
				handle->numericalData(QObject::tr("Parameters"),QObject::tr("kf"),QObject::tr("value")) = 1.0;
				handle->numericalData(QObject::tr("Parameters"),QObject::tr("kf"),QObject::tr("min")) = 0.0;
				handle->numericalData(QObject::tr("Parameters"),QObject::tr("kf"),QObject::tr("max")) = 100.0;
				handle->numericalData(QObject::tr("Parameters"),QObject::tr("kb"),QObject::tr("value")) = 0.1;
				handle->numericalData(QObject::tr("Parameters"),QObject::tr("kb"),QObject::tr("min")) = 0.0;
				handle->numericalData(QObject::tr("Parameters"),QObject::tr("kb"),QObject::tr("max")) = 100.0;
				handle->numericalData(QObject::tr("Parameters"),QObject::tr("deg"),QObject::tr("value")) = 0.1;
				handle->numericalData(QObject::tr("Parameters"),QObject::tr("deg"),QObject::tr("min")) = 0.0;
				handle->numericalData(QObject::tr("Parameters"),QObject::tr("deg"),QObject::tr("max")) = 1.0;
			}
			
			reactants.description() = QString("Number of each reactant participating in this reaction");
			products.description() = QString("Number of each product participating in this reaction");
			rates.description() = QString("Rates: a set of rates, one for each reaction represented by this item. Row names correspond to reaction names. The number of rows in this table and the stoichiometry table will be the same.");

			if (rates.value(QObject::tr("rate"),QObject::tr("formula")).isEmpty())
				rates.value(QObject::tr("rate"),QObject::tr("formula")) = QObject::tr("0.0");
			else
			if (handle->hasNumericalData(QObject::tr("Parameters")))
			{
				handle->numericalDataTable(QObject::tr("Parameters")).value(QObject::tr("k0"),0) = 0.1;
				rates.value(QObject::tr("rate"),QObject::tr("formula")) = handle->fullName() + QObject::tr(".k0") + rates.value(QObject::tr("rate"),QObject::tr("formula"));
			}

			handle->numericalDataTable(QObject::tr("Reactant stoichiometries")) = reactants;
			handle->numericalDataTable(QObject::tr("Product stoichiometries")) = products;
			handle->textDataTable(QObject::tr("Rate equations")) = rates;
		}
	};
}

#endif
