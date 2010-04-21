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

			bool isElongation = (nodes.size() == 2) && nodes[0] && nodes[1] && nodes[0]->isA(QString("Part")) && nodes[1]->isA(QString("Part"));
			bool isRegulatory = (isElongation && nodes.size() > 0 && (nodes[0])->isA(QString("Regulator")));
			bool isTermination = (handle->family() && nodes.size() > 1 && (nodes[1]) && (nodes[1])->isA(QString("Terminator")));
			bool isSynthesis = (handle->isA(QString("Synthesis")));
			bool isTranscription = (handle->isA(QString("Transcription")));
			bool isGRN = (handle->isA(QString("Transcription Regulation")));
			bool isBinding = !isGRN && (handle->isA(QString("Binding")));

			DataTable<qreal> stoichiometryMatrix;
			DataTable<QString> rates;
			
			rates.value(0,0) = QString("0.0");
			if (handle->hasNumericalData(QString("Numerical Attributes")))
			{
				if (isBinding)
				{
					handle->data->numericalData[QString("Numerical Attributes")].value(QString("kf"),0) = 1.0;
					handle->data->numericalData[QString("Numerical Attributes")].value(QString("kb"),0) = 1.0;
					rates.value(0,0) = handle->fullName() + QString(".kf");
					rates.value(1,0) = handle->fullName() + QString(".kb*") + handle->fullName() + QString(".complex");
				}
				else
				{
					handle->data->numericalData[QString("Numerical Attributes")].value(QString("k0"),0) = 1.0;
					rates.value(0,0) = handle->fullName() + QString(".k0");
				}
			}

			nodes.clear();
			QList<NodeHandle*> connectedNodes = handle->nodes();
			QList<NodeHandle*> nodesIn = handle->nodesIn();
			QList<NodeHandle*> nodesOut = handle->nodesOut();
			NodeHandle * node;
			QStringList names;
			QList<qreal> stoichiometry;
			
			int k;


			for (int i=0; i < connectedNodes.size(); ++i)
			{
				node = connectedNodes[i];
				if (node && !(node->isA(QString("Empty"))) && node->isA(QString("Molecule")))
				{
					k = nodesIn.indexOf(node);
					if (k > -1)
					{
						nodesIn[k] = 0;
						
						if (!nodes.contains(node))
						{
							nodes += node;
							names += node->fullName();
							if (!isSynthesis && !isRegulatory && !isBinding && !isGRN)
								stoichiometry += -1.0;
							else
								stoichiometry += 0.0;
						}
						else
						{
							if (!isSynthesis && !isRegulatory && !isGRN)
								stoichiometry[ nodes.indexOf(node) ] += -1.0;
						}
						
						rates.value(0,0) += QString("*") + node->fullName();						
					}
					else
					{
						k = nodesOut.indexOf(node);
						
						if (k > -1)
						{
							nodesOut[k] = 0;
							if (!nodes.contains(node))
							{
								nodes += node;
								names += node->fullName();
								if (!isTermination && !isBinding && !isGRN)
									stoichiometry += 1.0;
								else
									stoichiometry += 0.0;
							}
							else
							{
								if (!isTermination && !isGRN)
									stoichiometry[ nodes.indexOf(node) ] += 1.0;
							}

							if (isBinding)
								rates.value(0,0) += QString("*") + node->fullName();
						}
					}
				}
			}

			if (isBinding)
			{
				stoichiometryMatrix.resize(2, 1+names.size());
				stoichiometryMatrix.colName(names.size()) = handle->fullName() + QString(".complex");
				rates.resize(2,1);
				stoichiometryMatrix.rowName(0) = QString("bind");
				rates.rowName(0) = QString("bind");
				stoichiometryMatrix.rowName(1) = QString("unbind");
				rates.rowName(1) = QString("unbind");
				for (int i=0; i < stoichiometryMatrix.cols()-1; ++i)
				{
					stoichiometryMatrix.value(0,i) = -1.0;
					stoichiometryMatrix.value(1,i) = 1.0;
				}
				stoichiometryMatrix.value(0,stoichiometryMatrix.cols()-1) = 1.0;
				stoichiometryMatrix.value(1,stoichiometryMatrix.cols()-1) = -1.0;
			}
			else
				if (!isGRN)
				{
					stoichiometryMatrix.resize(1, names.size());
					rates.resize(1,1);
					stoichiometryMatrix.rowName(0) = handle->fullName();
					rates.rowName(0) = handle->fullName();
					for (int i=0; i < stoichiometryMatrix.cols(); ++i)
						stoichiometryMatrix.value(0,i) = stoichiometry[i];
				}

			if (isGRN || isElongation)
			{
				stoichiometryMatrix.resize(0,0);
				rates.resize(0,0);
			}
			else
			{
				rates.colName(0) = QString("rates");
				for (int i=0; i < names.size(); ++i)
				{
					stoichiometryMatrix.colName(i) = names[i];
				}
			}

			stoichiometryMatrix.description() = QString("Stochiometry: transpose of the normal Stoichiometry matrix. The rows correspond to the reactions and columns to the molecular species. The number of rows in this table and the rates table will be the same.");
			rates.description() = QString("Rates: a set of rates, one for each reaction represented by this item. Row names correspond to reaction names. The number of rows in this table and the stoichiometry table will be the same.");

			handle->data->numericalData.insert(QString("Stoichiometry"),stoichiometryMatrix);
			handle->data->textData.insert(QString("Rates"),rates);

		}
	};
}

#endif
