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
#include "OutputWindow.h"


namespace Tinkercell
{
	class DefaultRateAndStoichiometry
	{
	public:
		static void setDefault(ConnectionHandle * handle)
		{
			if (!handle || !handle->data) return;

                        QList<NodeHandle*> nodes = handle->nodes();
			
			bool isElongation = (handle->isA(QString("Elongation")) && 
                                                                        nodes.size() > 0 && (nodes[0]));
                        bool isRegulatory = (isElongation && nodes.size() > 0 && (nodes[0])->isA(QString("Regulatory")));
			bool isTermination = (handle->family() && handle->family()->isA(QString("Elongation")) && 
                                                                        nodes.size() > 1 && (nodes[1]) && (nodes[1])->isA(QString("Terminator")));
			bool isTranscription = (handle->isA(QString("Transcription")));
			bool isGRN = (handle->isA(QString("Transcription Regulation")));
			bool isBinding = !isGRN && (handle->isA(QString("Binding")));

			DataTable<qreal> stoichiometryMatrix;
			DataTable<QString> rates;

                        if (handle->hasNumericalData(QString("Numerical Attributes")))
			{
				if (isBinding)
				{
					rates.value(0,0) = QString("1.0");
					rates.value(1,0) = handle->fullName() + QString(".Kd*") + handle->fullName() + QString(".complex");
				}
				else
				{
					rates.value(0,0) = handle->fullName() + QString(".k0");
					int sz = handle->data->numericalData[QString("Numerical Attributes")].rows();
					if (!handle->data->numericalData[QString("Numerical Attributes")].rowNames().contains(QString("k0")))
					{
						handle->data->numericalData[QString("Numerical Attributes")].value(sz,0) = 0.5;
						handle->data->numericalData[QString("Numerical Attributes")].rowName(sz) = QString("k0");
					}
				}
			}
			else
			{
				rates.value(0,0) = QString("0.5");
				if (isBinding)
					rates.value(1,0) = QString("0.5");
			}

                        nodes.clear();
			QList<NodeHandle*> connectedNodes = handle->nodes();
                        QList<NodeHandle*> nodesIn = handle->nodesIn();
                        NodeHandle * node;
                        QStringList names;
                        QList<qreal> stoichiometry;


			for (int i=0; i < connectedNodes.size(); ++i)
			{
				node = connectedNodes[i];
				if (node && !(node->isA(QString("Empty"))) && node->isA(QString("Species")))
				{
					if (nodesIn.contains(node))
					{
						if (!nodes.contains(node))
						{
							nodes += node;
							names += node->fullName();
							if (!isTranscription && !isRegulatory && !isBinding && !isGRN)
								stoichiometry += -1.0;
							else
								stoichiometry += 0.0;
						}
						else
						{
							if (!isTranscription && !isRegulatory && !isGRN)		
								stoichiometry[ nodes.indexOf(node) ] += -1.0;
						}
						
						rates.value(0,0) += QString("*") + node->fullName();
					}
					else			
					{
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
			
			if (isGRN)
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
			
			handle->data->numericalData.insert(QString("Stoichiometry"),stoichiometryMatrix);
			handle->data->textData.insert(QString("Rates"),rates);
			
			if (isBinding)
				OutputWindow::message( QString("Note: binding connections are composed of two reactions; use the stoichiometry tool to view this.") );
			else
			if (isGRN)
				OutputWindow::message( QString("Note: the default regulatory reactions do not have any associated kinetics") );
			else
			if (isElongation)
				OutputWindow::message( QString("Note: the default elongation reactions do not have any associated kinetics.") );
		}
	};
}

#endif
