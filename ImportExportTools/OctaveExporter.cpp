/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool exports Octave models

****************************************************************************/
#include <QMessageBox>
#include <QDesktopServices>
#include "StoichiometryTool.h"
#include "BasicInformationTool.h"
#include "OctaveExporter.h"

namespace Tinkercell
{	
	OctaveExporter::OctaveExporter(): Tool(tr("Octave Export Tool"),tr("Export"))
	{
	}
	
	bool OctaveExporter::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);	
		if (!mainWindow) return false;

		if (mainWindow->fileMenu)
		{
			QList<QAction*> actions = mainWindow->fileMenu->actions();

			QAction * targetAction = 0;
			QMenu * exportmenu = 0;//, * importmenu = 0;
		
			for (int i=0; i < actions.size(); ++i)
				if (actions[i] && actions[i]->menu())
				{
					if (actions[i]->text() == tr("&Export"))
					{
						exportmenu = actions[i]->menu();
						targetAction = actions[i];
					}
					else
						if (actions[i]->text() == tr("&Import"))
						{
							targetAction = actions[i];
						}
				}
		
			if (!exportmenu)
			{
				for (int i=0; i < actions.size(); ++i)
					if (actions[i] && actions[i]->text() == tr("&Close page"))
					{
						exportmenu = new QMenu(tr("&Export"));
						mainWindow->fileMenu->insertMenu(actions[i],exportmenu);
					}
			}

			if (!exportmenu)
			{
				exportmenu = new QMenu(tr("&Export"));
				mainWindow->fileMenu->insertMenu(targetAction,exportmenu);
			}
		
			if (exportmenu)
			{
				exportmenu->addAction(tr("Octave/MATLAB"),this,SLOT(exportOctave()));
			}
		}
	
		return true;
	}


	void OctaveExporter::exportOctave()
	{
		QString filename(tempDir() + tr("/tinkercellmodel.m"));
		exportOctave(filename);
		QDesktopServices::openUrl(QUrl(filename));
	}

	void OctaveExporter::exportOctave(const QString& filename)
	{
		NetworkHandle * network = currentNetwork();
		if (!network)
		{
			QMessageBox::information(this,tr("No model"),tr("No model to export"));
			return;
		}

		QFile file(filename);
		
		if (!file.open(QFile::WriteOnly | QFile::Text))
		{
			QMessageBox::information(this,tr("Write error"),tr("Cannot write to ") + filename);
			return;
		}

		QList<ItemHandle*> handles = network->handles(true);
		
		QString name;
		QString replaceDot("_");

		int i,j;
		QStringList code;

		QStringList assignedHandles;
		QStringList rates = StoichiometryTool::getRates(handles, replaceDot);

		for (i=0; i < rates.size(); ++i)
		{
			if (rates[i].isEmpty())
				rates[i] = QString("0.0");
		}

		DataTable<qreal> N = StoichiometryTool::getStoichiometry(handles,replaceDot,true);

		int r = N.rows(), c = N.columns();

		if (r < 1 || c < 1 || rates.isEmpty())
		{
			QMessageBox::information(this,tr("No model"),tr("No model to export"));
			return;
		}

		DataTable<qreal> params = BasicInformationTool::getParameters(handles,QStringList(), QStringList(), replaceDot);
		
		params.insertColumn(1,tr("used"));

		bool used = false;
		for (i=0; i < params.rows(); ++i)
		{
			used = false;
			for (j=0; j < rates.size(); ++j)
			{
				if (rates[j].contains(params.rowName(i)))
				{
					used = true;
					break;
				}
			}
			if (used)
				params.value(i,1) = 1.0;
			else
				params.value(i,1) = 0.0;
		}

		QRegExp regex(tr("\\.(?!\\d)"));
		QString s1,s2;
		QStringList vars, fluxes, initValues, eventTriggers, eventActions, assignmentNames,
					assignmentDefs, fixedVars, functionNames, functionDefs, functionArgs;

		for (i=0; i < handles.size(); ++i)
		{
			if (network->symbolsTable.isValidPointer(handles[i]))
			{
				if (handles[i]->hasNumericalData(tr("Initial Value")))
				{
					vars << handles[i]->fullName(replaceDot);
					initValues << QString::number(handles[i]->numericalDataTable(tr("Initial Value")).value(0,0));
				}
				if (handles[i]->hasNumericalData(tr("Fixed")) &&
					handles[i]->numericalDataTable(tr("Fixed")).at(0,0) > 0)
				{
					int k = N.rowNames().indexOf(handles[i]->fullName(replaceDot));
					if (k >= 0)
					{
						fixedVars << handles[i]->fullName(replaceDot);
					}
				}
				
				if (handles[i]->hasTextData(tr("Events")))
				{
					DataTable<QString>& dat = handles[i]->textDataTable(tr("Events"));
					if (dat.columns() == 1)
						for (j=0; j < dat.rows(); ++j)
						{
							s1 =  dat.rowName(j);
							s2 =  dat.value(j,0);

							s1.replace(regex,replaceDot);
							s2.replace(regex,replaceDot);

							if (s1.isEmpty() || s2.isEmpty()) continue;

							eventTriggers << s1;
							eventActions << s2;

							for (int k=0; k < params.rows(); ++k)
								if (s2.contains(params.rowName(k)) || s1.contains(params.rowName(k)))
									params.value(k,1) = 1.0;
						}
				}
				if (handles[i]->hasTextData(tr("Functions")))
				{
					DataTable<QString>& dat = handles[i]->textDataTable(tr("Functions"));

					if (dat.columns() == 2)
					{
						for (j=0; j < dat.rows(); ++j)
						{
                            name = handles[i]->fullName(replaceDot);
							s1 = dat.value(j,1);
							s1.replace(regex,replaceDot);

							s2 = dat.value(j,0);

							functionNames << name + replaceDot + dat.rowName(j);
							functionArgs << s2;
							functionDefs << s1;
						}
					}
				}

				if (handles[i]->hasTextData(tr("Assignments")))
				{
					DataTable<QString>& dat = handles[i]->textDataTable(tr("Assignments"));
					if (dat.columns() == 1)
						for (j=0; j < dat.rows(); ++j)
						{
                            name = handles[i]->fullName(replaceDot);

							s1 =  dat.rowName(j);
							s2 =  dat.value(j,0);

                            s1.replace(regex,replaceDot);
							s2.replace(regex,replaceDot);

							if (s1.isEmpty() || s2.isEmpty()) continue;

							if (s1.isEmpty() || s1 == tr("self"))
							{
								assignmentNames << name;
								assignmentDefs << s2;

								if (!fixedVars.contains(name))
									fixedVars << name;
							}
							else								
							{
								assignmentNames << name + replaceDot + s1;
								assignmentDefs << s2;
							}
							for (int k=0; k < params.rows(); ++k)
								if (s2.contains(params.rowName(k)) || s1.contains(params.rowName(k)))
									params.value(k,1) = 1.0;
						}
				}
			}
		}

		for (i=0; i < functionNames.size(); ++i)
		{
			name = functionNames[i];
			s2 = functionArgs[i];
			s1 = functionDefs[i];

			s2 = tr("(") + s2 + tr(")");
			code += tr("    function retval = ") + name + s2 + tr("\n    retval = ") + s1 + tr("\nendfunction\n");
		}

		QStringList allSymbols = vars;
		
		for (i=0; i < N.rows(); ++i)
			if (!vars.contains(N.rowName(i)))
			{
				vars << N.rowName(i);
				initValues << tr("0.0");
			}
		
		for (int i=0; i < vars.size(); ++i)
		{
			code += vars[i];
			code += tr(" = ");
			code += initValues[i];
			code += tr(";\n");
		}
		
		if (!assignmentNames.isEmpty())
		{
			//code += tr("global");
			for (i=0; i < assignmentNames.size(); ++i)
				if (!allSymbols.contains(assignmentNames[i]))
				{
					code += tr(" ");
					code += assignmentNames[i];
					allSymbols += assignmentNames[i];
				}
			code += tr("\n");
			for (i=0; i < assignmentNames.size(); ++i)
				if (!allSymbols.contains(assignmentNames[i]))
				{
					code += assignmentNames[i];
					code += tr(" = 0.0;\n");
				}
		}

		QStringList trueParams;
		for (i = 0; i < params.rows(); ++i)
			if (!allSymbols.contains(params.rowName(i)) && params.at(i,1) > 0)
			{
				trueParams += params.rowName(i);
				allSymbols += params.rowName(i);

				code += params.rowName(i);
				code += tr(" = ");
				code += QString::number(params.at(i,0));
				code += tr(";\n");
			}

		for (i=0; i < r; ++i)
			if (fixedVars.contains(N.rowName(i)))
			{
				for (j=0; j < c; ++j)
				{
					N.value(i,j) = 0.0;
				}
			}

		QString globals = tr("global ") + allSymbols.join(tr(" ")) + tr("\n");		
		code.push_front(globals);
		
		globals = tr("    ") + globals;
		
		//get array of current param values
		code += tr("\nfunction retval = getParameters()\n");
		code += globals;
		code += tr("    retval = zeros(1,");
		code += QString::number(trueParams.size());
		code += tr(");\n");

		for (i = 0; i < trueParams.size(); ++i)
		{
			code += tr("    retval(");
			code += QString::number(i+1);
			code += tr(") = ");
			code += trueParams[i];
			code += tr(";\n");
		}
		code += tr("endfunction\n\n");

		//get array of current variable values
		code += tr("\nfunction retval = getVars()\n");
		code += globals;
		code += tr("    retval = zeros(");
		code += QString::number(r);
		code += tr(",1);\n");

		for (i = 0; i < r; ++i)
		{
			code += tr("    retval(");
			code += QString::number(i+1);
			code += tr(") = ");
			code += N.rowName(i);
			code += tr(";\n");
		}
		code += tr("endfunction\n");

		//write assign-parameters function (not needed for simulation, but could be useful for some purpose)
		code += tr("\nfunction [] = assignParameters(newparams)\n");
		code += globals;
		for (i = 0; i < trueParams.size(); ++i)
		{
			code += tr("   ");
			code += trueParams[i];
			code += tr(" = newparams(");
			code += QString::number(i+1);
			code += ");\n";
		}

		code += tr("endfunction\nglobal StoichiometryMatrix\n\n");
		code += tr("\n\nStoichiometryMatrix = [ ");

		for (i = 0; i < r; ++i)
		{
			code += tr("    ");
			for (j=0; j < c; ++j)
			{
				code += QString::number( N.at(i,j) );
				if (j < (c-1))
					code += tr(" , ");
				else
					if (i < (r-1))
						code += tr(" ;\n");
					else
						code += tr("];\n");
			}
		}

		//print header and beginning of propensity function
		code += tr("\nfunction DYDT = ODE(u, time)\n");
		code += globals;
		code += tr("    global StoichiometryMatrix\n");
		
		//declare variables
		for (i = 0; i < N.rows(); ++i)
		{
			code += tr("    ");
			code += N.rowName(i);
			code += tr(" = u(");
			code += QString::number(i+1);
			code += tr(");\n");
		}

		//assignments
		if (!assignmentNames.isEmpty())
		{
			for (i=0; i < assignmentNames.size(); ++i)
			{
				code += tr("    ");
				code += assignmentNames[i];
				code += tr(" = ");
				code += assignmentDefs[i];
				code += tr(";\n");
			}
		}
		
		//events		
		if (eventTriggers.size() > 0)
		{		
			code += tr("int TCtriggers(int i, double time, double * y, void * data)\n{\n    TCmodel * model = (TCmodel*)data;\n");
			for (i=0; i < eventTriggers.size(); ++i)
			{
				code += tr("    if (");
				code += eventTriggers[i];
				code += tr(")\n        ");
				code += eventActions[i];
				code += tr("\n    endif\n");
			}
		}
		
		code += tr("    rates = zeros(");
		code += QString::number(c);
		code += tr(",1);\n");
		//print the rates
		for (i = 0; i < rates.size() && i < c; ++i)
		{
			code += tr("    rates(");
			code += QString::number(i+1);
			code += tr(") = ");
			code += rates[i];
			code += tr(";\n");
		}
		
		//ODEs
		code += tr("    DYDT = StoichiometryMatrix * rates;\nendfunction\n\n");

		file.write(code.join("").toUtf8());
		file.close();
	}
}

/*
extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::OctaveExporter * octaveTool = new Tinkercell::OctaveExporter;
	main->addTool(octaveTool);
}*/

