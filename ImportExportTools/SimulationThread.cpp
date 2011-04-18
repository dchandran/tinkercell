#include "StoichiometryTool.h"
#include "BasicInformationTool.h"
#include "SimulationThread.h"

using namespace Tinkercell;

int SimulationThread::totalModelCount = 0;

SimulationThread::~SimulationThread()
{
	if (model.CopasiDataModelPtr)
		cRemoveModel(model);
	
	model.CopasiModelPtr = 0;
	model.CopasiDataModelPtr = 0;
	model.qHash = 0;
}

void SimulationThread::updateModelParameters(const NumericalDataTable & params)
{
	for (int i=0; i < params.rows(); ++i)
		cSetGlobalParameter(model, params.rowName(i).toUtf8().data(), params.at(i,0));
}

void SimulationThread::updateModel(QList<ItemHandle*> & handles)
{
	updateModel(handles, model, optimizationParameters);
}

void SimulationThread::updateModel(QList<ItemHandle*> & handles, copasi_model & model, NumericalDataTable & optimizationParameters)
{
	//make sure all children are included
	for (int i=0; i < handles.size(); ++i)
		if (handles[i])
			for (int j=0; j < handles[i]->children.size(); ++j)
				if (!handles.contains( handles[i]->children[j] ))
					handles += handles[i]->children[j];
		
	optimizationParameters.resize(0,0);
	if (model.CopasiDataModelPtr)
		cRemoveModel(model);
	
	model.CopasiModelPtr = 0;
	model.CopasiDataModelPtr = 0;
	model.qHash = 0;
	++totalModelCount;
	QString modelName = tr("tinkercell") + QString::number(totalModelCount);
	model = cCreateModel(modelName.toUtf8().data());

	NumericalDataTable params = BasicInformationTool::getUsedParameters(handles);
	for (int i=0; i < params.rows(); ++i)
		if (params(i,1) < params(i,2))
			optimizationParameters.value(params.rowName(i)) = params(i,0);
	
	NumericalDataTable stoic_matrix = StoichiometryTool::getStoichiometry(handles);
	QStringList rates = StoichiometryTool::getRates(handles);
	QStringList species, eventTriggers, eventActions, assignmentNames,
				assignmentDefs, fixedVars, functionNames, functionDefs, functionArgs;

	species = stoic_matrix.rowNames();
	QVector<double> initialValues(species.size(),0.0);
	QVector<QString> speciesCompartments(species.size(),tr("DefaultCompartment"));
	QVector<double> compartmentVolumes(species.size(),1.0);
	ItemHandle * parentHandle;

	QRegExp regex(tr("\\.(?!\\d)"));	
	int i,j;
	QString s1,s2,name;

	for (i=0; i < handles.size(); ++i)
	{
		if (handles[i])// && handles[i]->family())
		{
			if (handles[i]->children.isEmpty())
			{
				if (handles[i]->hasNumericalData(tr("Initial Value")))
				{
					int k = species.indexOf(handles[i]->fullName(tr("_")));
					if (k >= 0)
					{
						initialValues[k] = handles[i]->numericalData(tr("Initial Value"));
					}

					if (k < 0 || (	handles[i]->hasNumericalData(tr("Fixed")) && handles[i]->numericalData(tr("Fixed")) > 0 ))
					{
						if (	handles[i]->hasNumericalData(tr("Fixed")) && handles[i]->numericalData(tr("Fixed")) > 0 )
							fixedVars << handles[i]->fullName(tr("_"));
						if (!species.contains(handles[i]->fullName(tr("_"))))
						{
							k = species.size() - 1;
							species << handles[i]->fullName(tr("_"));
							initialValues << handles[i]->numericalData(tr("Initial Value"));
							compartmentVolumes << 1.0;
							speciesCompartments << tr("DefaultCompartment");
						}
					}
					
					if (parentHandle = handles[i]->parentOfFamily(tr("Compartment")))
					{
						speciesCompartments[k] = parentHandle->fullName(tr("_"));
						if (parentHandle->hasNumericalData(tr("Initial Value")))
							compartmentVolumes[k] = parentHandle->numericalData(tr("Initial Value"));
						else
							compartmentVolumes[k] = 1.0;
					}
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

						s1.replace(regex,tr("_"));
						s2.replace(regex,tr("_"));

						if (s1.isEmpty() || s2.isEmpty()) continue;

						eventTriggers << s1;
						eventActions << s2;
					}
			}
			if (handles[i]->hasTextData(tr("Functions")))
			{
				DataTable<QString>& dat = handles[i]->textDataTable(tr("Functions"));

				if (dat.columns() == 2)
				{
					for (j=0; j < dat.rows(); ++j)
					{
                        name = handles[i]->fullName(tr("_"));
						s1 = dat.value(j,1);
						s1.replace(regex,tr("_"));
						s2 = dat.value(j,0);

						functionNames << name + tr("_") + dat.rowName(j);
						
						if (s1.isEmpty() || s2.isEmpty()) continue;
						
						functionArgs << s2;
						functionDefs << s1;
					}
				}
			}

			if (handles[i]->hasTextData(tr("Assignments")))
			{
				DataTable<QString>& dat = handles[i]->textDataTable(tr("Assignments"));
				if (dat.columns() > 0 && dat.rows() > 0)
					for (j=0; j < dat.rows(); ++j)
					{
                        name = handles[i]->fullName(tr("_"));
                        
                        s1 =  dat.rowName(j);
						s2 =  dat.value(j,0);

                        s1.replace(regex,tr("_"));
						s2.replace(regex,tr("_"));

						if (s2.isEmpty()) continue;

						if (s1.isEmpty() || s1 == tr("self"))
						{
							assignmentNames << name;
							assignmentDefs << s2;
						}
						else								
						{
							assignmentNames << name + tr("_") + s1;
							assignmentDefs << s2;
						}
						
						if (!species.contains(assignmentNames.last()))
						{
							species << assignmentNames.last();
							initialValues << 0.0;
							if (parentHandle = handles[i]->parentOfFamily(tr("Compartment")))
							{
								speciesCompartments << parentHandle->fullName(tr("_"));
								if (parentHandle->hasNumericalData(tr("Initial Value")))
									compartmentVolumes << parentHandle->numericalData(tr("Initial Value"));
								else
									compartmentVolumes << 1.0;
							}
							else
							{
								speciesCompartments << tr("DefaultCompartment");
								compartmentVolumes << 1.0;
							}
						}
					}
			}
		}
	}

	//Make list of species types and units
	QVector<ItemHandle*> speciesHandles(species.size(),0);
	QList<ItemFamily*> families;
	
	for (int i=0,k=0; i < handles.size(); ++i)
	{
		k = species.indexOf(handles[i]->fullName(tr("_")));
		if (k >= 0 && handles[i]->family())
		{
			speciesHandles[k] = handles[i];
			if (!families.contains(handles[i]->family()))
				families.append(handles[i]->family());
		}
	}
	
	QFile fout("copasi.txt");
	fout.open(QFile::WriteOnly | QFile::Text );
	QString commands;
	QHash<QString, copasi_compartment> compartmentHash;

	//create list of species
	for (int i=0; i < species.size(); ++i)
	{
		copasi_compartment c;
		
		if (compartmentHash.contains(speciesCompartments[i]))
		{
			c = compartmentHash[ speciesCompartments[i] ];
		}
		else
		{
			c = cCreateCompartment(model, speciesCompartments[i].toUtf8().data(), compartmentVolumes[i]);
			compartmentHash[ speciesCompartments[i] ] = c;
			commands += speciesCompartments[i] + tr(" = cCreateCompartment(model,\"") + speciesCompartments[i] + tr("\",") + QString::number(compartmentVolumes[i]) + tr(");\n");
		}
		cCreateSpecies(c, species[i].toUtf8().data(), initialValues[i]);
		commands += tr("cCreateSpecies(") + speciesCompartments[i] + tr(",\"") + species[i] + tr("\",") + QString::number(initialValues[i]) + tr(");\n");
		if (fixedVars.contains(species[i]))
		{
			cSetBoundarySpecies(model, species[i].toUtf8().data(), 1);
			commands += tr("cSetBoundarySpecies(model, \"") + species[i] + tr("\",1);\n");
		}
	}
	
	//create list of parameters
	for (int i=0; i < params.rows(); ++i)
	{
		cSetGlobalParameter(model, params.rowName(i).toUtf8().data(), params.value(i,0));
		commands += tr("cSetGlobalParameter(model,\"") + params.rowName(i) + tr("\",") + QString::number(params.value(i,0)) + tr(");\n");
	}

	//list of assignments
	for (int i=0; i < assignmentNames.size(); ++i)
	{
		cSetAssignmentRule(model, assignmentNames[i].toUtf8().data(), assignmentDefs[i].toUtf8().data());
		commands += tr("cSetAssignmentRule(model, \"") + assignmentNames[i] + tr("\",\"") + assignmentDefs[i] + tr("\");\n");
	}

	//create list of reactions
	for (int i=0; i < stoic_matrix.columns(); ++i)
	{
		copasi_reaction reac = cCreateReaction(model, stoic_matrix.columnName(i).toUtf8().data());
		cSetReactionRate(reac, rates[i].toUtf8().data());
		
		commands += tr("r") + QString::number(i) + tr(" = cCreateReaction(model, \"") + stoic_matrix.columnName(i) + tr("\");\n");
		commands += tr("cSetReactionRate(") + tr("r") + QString::number(i) + tr(",\"") + rates[i] + tr("\");\n");

		for (int j=0; j < stoic_matrix.rows(); ++j)
			if (stoic_matrix.value(j,i) < 0)
			{
				cAddReactant(reac, stoic_matrix.rowName(j).toUtf8().data(), -stoic_matrix.value(j,i));
				commands += tr("cAddReactant(") + tr("r") + QString::number(i) + tr(",\"") + stoic_matrix.rowName(j) + tr("\",") + QString::number(-stoic_matrix.value(j,i)) + tr(");\n");
			}
			else
			if (stoic_matrix.value(j,i) > 0)
			{
				cAddProduct(reac, stoic_matrix.rowName(j).toUtf8().data(), stoic_matrix.value(j,i));
				commands += tr("cAddProduct(") + tr("r") + QString::number(i) + tr(",\"") + stoic_matrix.rowName(j) + tr("\",") + QString::number(stoic_matrix.value(j,i)) + tr(");\n");
			}
	}
	
	fout.write(commands.toUtf8());
	fout.close();
	
	//list of events
/*	for (int i=0; i < eventTriggers.size(); ++i)
	{
		QStringList actions = eventActions[i].split(";");
		for (int j=0; j < actions.size(); ++j)
		{
			QStringList words = actions[j].split("=");
			if (words.size() == 2)
			{
				cCreateEvent(model, (QString("event") + QString::number(i)).toUtf8().data(), eventTriggers[i].toUtf8().data(), words[0].trimmed().toUtf8().data(), words[1].trimmed().toUtf8().data());
				break;
			}
		}
	}*/
}

void SimulationThread::updateModel()
{
	QSemaphore * sem = new QSemaphore(1);
	bool changed = true;
	QList<ItemHandle*> handles;
	sem->acquire();
	emit getHandles( this, sem, &handles, &changed);
	sem->acquire();
	sem->release();

	if (changed)
	{
		argMatrix.resize(0,0);
		updateModel(handles,this->model, optimizationParameters);
	}
	
	delete sem;
}

SimulationThread::SimulationThread(MainWindow * parent) : CThread(parent)
{
	method = None;
	resultMatrix = tc_createMatrix(0,0);
	semaphore = 0;
	plot = false;
	model.CopasiModelPtr = 0;
	model.CopasiDataModelPtr = 0;
	model.qHash = 0;
	if (mainWindow)
	{
		QWidget * widget = mainWindow->tool("Default Plot Tool");
		if (widget)
		{
			PlotTool * plotTool = static_cast<PlotTool*>(widget);
			connect(this,SIGNAL(graph(const DataTable<qreal>&,const QString&,int, PlotTool::PlotType)),
							 plotTool, SLOT(plot(const DataTable<qreal>&,const QString&,int, PlotTool::PlotType)));
		}
	}
}

void SimulationThread::setMethod(AnalysisMethod mthd)
{
	method = mthd;
	scanItems.clear();
}

void SimulationThread::setSemaphore(QSemaphore * sem)
{
	semaphore = sem;
}

void SimulationThread::setStartTime(double d)
{
	startTime = d;
}

void SimulationThread::setEndTime(double d)
{
	endTime = d;
}

void SimulationThread::setNumPoints(int i)
{
	numPoints = i;
}

void SimulationThread::setParameterRange(const QString& param, double start, double end, int n)
{
	bool exists = false;
	for (int i=0; i < scanItems.size(); ++i)
		if (scanItems[i].name == param)
		{
			exists = true;
			break;
		}
	
	if (!exists)
	{
		ScanItem u = { param, start, end, n };	
		scanItems += u;
	}
}

tc_matrix SimulationThread::result()
{
	return resultMatrix;
}

void SimulationThread::run()
{
	if (!model.CopasiModelPtr)
	{
		if (semaphore)
			semaphore->release();
		return;
	}

	for (int i=0; i < argMatrix.rows(); ++i) //values from slider
	{
		cSetValue(model, argMatrix.rowName(i).toUtf8().data() ,  argMatrix(i,0) );
	}

	int x = 0;
	PlotTool::PlotType plotType;
	QString plotTitle;

	/*if (method == SteadyStateScan1D || method == SteadyStateScan2D)
	{
		QWidget * widget = CThread::dialog(this, QString("Steady state scan"));
		widget->show();
	}*/
	

	switch (method)
	{
		case None:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = tc_createMatrix(0,0);
			break;
		case ReducedStoichiometry:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cGetReducedStoichiometryMatrix(model);
			plot = false;
			break;
		case ElementaryFluxModes:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cGetElementaryFluxModes(model);
			plot = false;
			break;
		case KMatrix:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cGetKMatrix(model);
			plot = false;
			break;
		case LMatrix:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cGetLinkMatrix(model);
			plot = false;
			break;
		case K0Matrix:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cGetK0Matrix(model);
			plot = false;
			break;
		case L0Matrix:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cGetL0Matrix(model);
			plot = false;
			break;
		case GammaMatrix:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cGetGammaMatrix(model);
			plot = false;
			break;
		case DeterministicSimulation:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cSimulateDeterministic(model, startTime, endTime, numPoints);
			plotTitle = tr("Time-course");
			plotType = PlotTool::Plot2D;
			break;
		case StochasticSimulation:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cSimulateStochastic(model, startTime, endTime, numPoints);
			plotTitle = tr("Stochastic simulation");
			plotType = PlotTool::Plot2D;
			break;
		case HybridSimulation:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cSimulateHybrid(model, startTime, endTime, numPoints);
			plotTitle = tr("Hybrid simulation");
			plotType = PlotTool::Plot2D;
			break;
		case TauLeapSimulation:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cSimulateTauLeap(model, startTime, endTime, numPoints);
			plotTitle = tr("Stochastic simulation");
			plotType = PlotTool::Plot2D;
			break;
		case SteadyState:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cGetSteadyState(model);
			plotTitle = tr("Steady state");
			plotType = PlotTool::Text;
			x = -1;
			break;
		case SteadyStateScan1D:			
			if (scanItems.size() > 0)
			{
				int n = scanItems[0].numPoints;
				double start = scanItems[0].start, 
							end = scanItems[0].end;
				double step = (end - start)/n;
				double p = 0;
				QString param = scanItems[0].name;
				tc_matrix ss;
				int i,j;
				
				QString title("steady state scan");
				for (i=0; i < n; ++i)
				{
					showProgress( title, (int)(100 * i)/n  );
					p = start + (double)(i)*step;
					cSetValue(model, param.toUtf8().data(), p);
					ss = cGetSteadyState(model);					

					if (i == 0)
					{
						tc_deleteMatrix(resultMatrix);
						resultMatrix = tc_createMatrix(n, ss.rows+1);
						tc_setColumnName(resultMatrix, 0, param.toUtf8().data());
						for (j=0; j < ss.rows; ++j)
							tc_setColumnName(resultMatrix, j+1, tc_getRowName(ss, j));
					}
		
					tc_setMatrixValue(resultMatrix, i, 0, p);
					for (j=0; j < ss.rows; ++j)
						tc_setMatrixValue(resultMatrix, i, j+1, tc_getMatrixValue(ss, j, 0));
		
					tc_deleteMatrix(ss);
				}
				showProgress(title, 100);
				plotTitle = tr("Steady state scan");
				plotType = PlotTool::Plot2D;
			}
			break;
		case SteadyStateScan2D:
			if (scanItems.size() > 2)
			{
				int n1 = scanItems[0].numPoints,
					  n2 = scanItems[1].numPoints;
				double start1 = scanItems[0].start,
							start2 = scanItems[1].start, 
							end1 = scanItems[0].end,
							end2 = scanItems[1].end;
				double step1 = (end1 - start1)/n1,
								step2 = (end2 - start2)/n2;
				double p1 = 0, p2 = 0;
				QString param1 = scanItems[0].name,
								param2 = scanItems[1].name,
								param3 = scanItems[2].name;
				tc_matrix ss;
				int i,j,k,l=-1;
				
				QString title("steady state scan 2D");
				for (i=0; i < n1; ++i)
				{
					showProgress( title, (int)(100 * i)/n1  );
					p1 = start1 + (double)(i)*step1;
					cSetValue(model, param1.toUtf8().data(), p1);
					
					for (j=0; j < n2; ++j)
					{
						p2 = start2 + (double)(i)*step2;
						cSetValue(model, param2.toUtf8().data(), p2);
						
						ss = cGetSteadyState(model);

						if (l == -1)
						{
							tc_deleteMatrix(resultMatrix);
							resultMatrix = tc_createMatrix(n1*n2, 3);
							tc_setColumnName(resultMatrix, 0, param1.toUtf8().data());
							tc_setColumnName(resultMatrix, 1, param2.toUtf8().data());
							tc_setColumnName(resultMatrix, 2, param3.toUtf8().data());
							
							for (k=0; k < ss.rows; ++k)
								if (QString(tc_getRowName(ss, k)) == param3)
								{
									l = k;
									break;
								}
						}
		
						tc_setMatrixValue(resultMatrix, i*n2+j, 0, p1);
						tc_setMatrixValue(resultMatrix, i*n2+j, 1, p2);
						if (l >= 0)
							tc_setMatrixValue(resultMatrix, i*n2+j, 2, tc_getMatrixValue(ss,l,0));
						else
							tc_setMatrixValue(resultMatrix, i*n2+j, 2, 0.0);
		
						tc_deleteMatrix(ss);
					}
				}
				showProgress(title, 100);
			}
			plotTitle = tr("Steady state scan");
			plotType = PlotTool::SurfacePlot;
			break;
		case Jacobian:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cGetJacobian(model);
			plotTitle = tr("Jacobian");
			plotType = PlotTool::BarPlot;
			x = -1;
			break;
		case Eigenvalues:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cGetEigenvalues(model);
			plotTitle = tr("Eigenvalues");
			plotType = PlotTool::ScatterPlot;
			x = 0;
			break;
		case UnscaledElasticities:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cGetUnscaledElasticities(model);
			plotTitle = tr("Unscaled elasticities");
			plotType = PlotTool::BarPlot;
			x = -1;
			break;
		case UnscaledConcentrationCC:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cGetUnscaledConcentrationControlCoeffs(model);
			plotTitle = tr("Unscaled concentration control coefficients");
			plotType = PlotTool::BarPlot;
			x = -1;
			break;
		case UnscaledFluxCC:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cGetUnscaledFluxControlCoeffs(model);
			plotTitle = tr("Unscaled flux control coefficients");
			plotType = PlotTool::BarPlot;
			x = -1;
			break;
		case ScaledElasticities:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cGetScaledElasticities(model);
			plotTitle = tr("Scaled elasticities");
			plotType = PlotTool::BarPlot;
			x = -1;
			break;
		case ScaledConcentrationCC:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cGetScaledConcentrationConcentrationCoeffs(model);
			plotTitle = tr("Scaled concentration control coefficients");
			plotType = PlotTool::BarPlot;
			x = -1;
			break;
		case ScaledFluxCC:
			tc_deleteMatrix(resultMatrix);
			resultMatrix = cGetScaledFluxControlCoeffs(model);
			plotTitle = tr("Scaled flux control coefficients");
			plotType = PlotTool::BarPlot;
			x = -1;
			break;
		case GA:
			tc_deleteMatrix(resultMatrix);
			tc_matrix params = ConvertValue(optimizationParameters);
			resultMatrix = cOptimize(model, objective.toAscii().data(), params);
			plotTitle = tr("Optimized parameter distribution");
			plotType = PlotTool::ScatterPlot;
			tc_deleteMatrix(params);
			x = 0;
			break;
	}
	
	if (plot && !semaphore)
	{
		NumericalDataTable * dat = ConvertValue(resultMatrix);
		emit graph(*dat, plotTitle, x, plotType);
		if (method==Jacobian && mainWindow && mainWindow->console())
				mainWindow->console()->printTable(*dat);
		delete dat; 
	}
	
	if (semaphore)
		semaphore->release();
}

SimulationDialog::SimulationDialog(MainWindow * parent) : QDialog(parent)
{
	sliderWidget = 0;

	QVBoxLayout * layout1 = new QVBoxLayout;
	QGridLayout * simLayout = new QGridLayout;
	QGridLayout * box1Layout = new QGridLayout;
	QGridLayout * box2Layout = new QGridLayout;
	
	simStart = new QDoubleSpinBox;
	simStart->setValue(0.0);
	simStart->setRange(0.0,1.0E5);
	simStart->setDecimals(5);
	simStart->setSingleStep(0.1);

	simEnd = new QDoubleSpinBox;
	simEnd->setRange(0.0,1.0E5);
	simEnd->setDecimals(5);
	simEnd->setSingleStep(0.1);
	simEnd->setValue(100.0);

	param1Start = new QDoubleSpinBox;
	param1Start->setValue(0.0);
	param1Start->setRange(-1.0E5,1.0E5);
	param1Start->setDecimals(5);
	param1Start->setSingleStep(0.1);

	param2Start = new QDoubleSpinBox;
	param2Start->setValue(0.0);
	param2Start->setRange(-1.0E5,1.0E5);
	param2Start->setDecimals(5);
	param2Start->setSingleStep(0.1);

	param1End = new QDoubleSpinBox;
	param1End->setValue(10.0);
	param1End->setRange(-1.0E5,1.0E5);
	param1End->setDecimals(5);
	param1End->setSingleStep(0.1);

	param2End = new QDoubleSpinBox;
	param2End->setValue(10.0);
	param2End->setRange(-1.0E5,1.0E5);
	param2End->setDecimals(5);
	param2End->setSingleStep(0.1);

	numPoints1 = new QSpinBox;
	numPoints1->setRange(2,1000);
	numPoints1->setValue(100);

	numPoints2 = new QSpinBox;
	numPoints2->setRange(2,1000);
	numPoints2->setValue(10);

	numPoints3 = new QSpinBox;
	numPoints3->setRange(2,1000);
	numPoints3->setValue(10);
	
	menu1 = new QComboBox;
	menu2 = new QComboBox;
	menu3 = new QComboBox;

	simBox = new QGroupBox(tr(" simulate "));
	param1Box = new QGroupBox(tr(" parameter "));
	param2Box = new QGroupBox(tr(" parameter 2D "));
	
	simLayout->addWidget(new QLabel("start time"), 0, 0);
	simLayout->addWidget(new QLabel("end time"), 1, 0);
	simLayout->addWidget(new QLabel("num. points"), 2, 0);
	simLayout->addWidget(simStart, 0, 1);
	simLayout->addWidget(simEnd, 1, 1);
	simLayout->addWidget(numPoints1, 2, 1);

	box1Layout->addWidget(new QLabel("x-axis"), 0, 0);
	box1Layout->addWidget(new QLabel("start"), 1, 0);
	box1Layout->addWidget(new QLabel("end"), 2, 0);
	box1Layout->addWidget(new QLabel("num. points"), 3, 0);
	box1Layout->addWidget(menu1, 0, 1);
	box1Layout->addWidget(param1Start, 1, 1);
	box1Layout->addWidget(param1End, 2, 1);
	box1Layout->addWidget(numPoints2, 3, 1);

	box2Layout->addWidget(new QLabel("y-axis"), 0, 0);
	box2Layout->addWidget(new QLabel("start"), 1, 0);
	box2Layout->addWidget(new QLabel("end"), 2, 0);
	box2Layout->addWidget(new QLabel("num. points"), 3, 0);
	box2Layout->addWidget(new QLabel("z-axis"), 4, 0);
	box2Layout->addWidget(menu2, 0, 1);
	box2Layout->addWidget(param2Start, 1, 1);
	box2Layout->addWidget(param2End, 2, 1);
	box2Layout->addWidget(numPoints3, 3, 1);
	box2Layout->addWidget(menu3, 4, 1);

	simBox->setLayout(simLayout);
	param1Box->setLayout(box1Layout);
	param2Box->setLayout(box2Layout);
	
	layout1->addWidget(methodLabel = new QLabel(""));
	layout1->addWidget(simBox);
	layout1->addWidget(param1Box);
	layout1->addWidget(param2Box);
	
	QHBoxLayout * layout2 = new QHBoxLayout;
	QPushButton * playButton = new QPushButton;
	playButton->setIcon(QIcon(tr(":/images/play.png")));
	connect(playButton,SIGNAL(released()),this,SLOT(run()));
	
	layout2->addWidget(playButton, Qt::AlignRight);
	QVBoxLayout * layout3 = new QVBoxLayout;
	layout3->addLayout(layout1,1);
	layout3->addLayout(layout2,0);
	setLayout(layout3);
	
	connect(parent,SIGNAL(historyChanged(int)),this,SLOT(historyChanged(int)));
	
	thread = 0;

	simBox->show();
	param1Box->hide();
	param2Box->hide();
	hide();
}

void SimulationThread::setObjective(const QString& s)
{
	objective = s;
}

void SimulationDialog::setThread(SimulationThread * t)
{
	thread = t;
}

void SimulationDialog::setMethod(SimulationThread::AnalysisMethod method)
{
	if (thread)
	{
		thread->setMethod(method);
		thread->setSemaphore(0);
	}
	
	if (	method == SimulationThread::DeterministicSimulation ||
			method == SimulationThread::StochasticSimulation ||
			method == SimulationThread::HybridSimulation ||
			method == SimulationThread::TauLeapSimulation )
	{
		if (method == SimulationThread::DeterministicSimulation )
			methodLabel->setText(tr("Method: Deterministic"));
		else
		if (method == SimulationThread::StochasticSimulation)
			methodLabel->setText(tr("Method: Stochastic (exact)"));
		else
		if (method == SimulationThread::HybridSimulation)
			methodLabel->setText(tr("Method: Hybrid simulation"));
		else
		if (method == SimulationThread::TauLeapSimulation)
			methodLabel->setText(tr("Method: Stochastic (Tau-leap)"));
		
		this->show();
		simBox->show();
		param1Box->hide();
		param2Box->hide();
		this->raise();
	}
	else
	if ( method == SimulationThread::SteadyStateScan1D ||
			method == SimulationThread::SteadyStateScan2D )
	{
		if (method == SimulationThread::SteadyStateScan1D)
			methodLabel->setText(tr("Method: 1D Scan"));
		else
		if (method == SimulationThread::SteadyStateScan2D)
			methodLabel->setText(tr("Method: 2D Scan"));

		this->show();
		simBox->hide();
		param1Box->show();
		if (method == SimulationThread::SteadyStateScan2D)
			param2Box->show();
		else
			param2Box->hide();
		this->raise();
	}
	else
	{
		this->hide();
		enterEvent(0);
		run();
	}
}

void SimulationDialog::run()
{
	if (!thread || thread->isRunning()) return;

	if (sliderValues.rows() < 1)
	{
		if (thread->mainWindow)
			thread->mainWindow->console()->message(tr("No model"));
		return;
	}
	
	thread->startTime = simStart->value();
	thread->endTime = simEnd->value();
	thread->numPoints = numPoints1->value();
	thread->plot = true;
	thread->scanItems.clear();
	thread->setSemaphore(0);
	
	if (param1Box->isVisible())
	{
		thread->setParameterRange( 
										menu1->currentText(), 
										param1Start->value(),
										param1End->value(),
										numPoints2->value() );
	}

	if (param2Box->isVisible())
	{
		thread->setParameterRange(
										menu2->currentText(), 
										param2Start->value(),
										param2End->value(),
										numPoints3->value() );
		thread->setParameterRange(
										menu3->currentText(), 
										0,
										0,
										0 );
	}
	
	this->hide();
	thread->updateModel();
	
	QStringList rownames;
	QList<double> col1, col2;
	for (int i=0; i < sliderValues.rows(); ++i)
	{
		rownames += sliderValues.rowName(i);
		col1 += sliderValues(i) * 0.1;
		col2 += sliderValues(i) * 1.9;
	}
	
	QRect rect;
	
	if (sliderWidget)
	{
		rect = sliderWidget->geometry();
		sliderWidget->close();
		delete sliderWidget;
		sliderWidget = 0;
	}

	sliderWidget = new MultithreadedSliderWidget(thread->mainWindow,thread);
	if (!rect.isNull())
		sliderWidget->setGeometry(rect);
	sliderWidget->setAttribute(Qt::WA_DeleteOnClose,false);
	sliderWidget->setDefaultDataTable(tr("Initial value"));
	sliderWidget->setSliders(rownames,col1,col2);
	sliderWidget->show();
	//thread->start();
}

void SimulationDialog::historyChanged(int)
{
	if (sliderWidget && sliderWidget->isVisible())
	{
		updateParameterList();
		this->run();
	}
}

SimulationDialog::~SimulationDialog()
{
	if (sliderWidget)
	{
		sliderWidget->close();
		delete sliderWidget;
		sliderWidget = 0;
	}
}

void SimulationDialog::enterEvent ( QEvent * event )
{
	updateParameterList();
}

void SimulationDialog::updateParameterList()
{
	if (!thread || !thread->mainWindow)  return;
	NetworkHandle * network = thread->mainWindow->currentNetwork();

	if (!network) return;

	QList<ItemHandle*> handles = network->handles(true), handles2;

	sliderValues = BasicInformationTool::getUsedParameters(handles, tr("_"));
	
	for (int i=0; i < handles.size(); ++i)
		if (handles[i] && !handles[i]->name.isEmpty() && handles[i]->hasNumericalData(tr("Initial value")))
		{
			handles2 += handles[i];
		}
	
	int r = sliderValues.rows();
	sliderValues.resize(sliderValues.rows() + handles2.size(), 1);
	QStringList handleNames;

	for (int i=0; i < handles2.size(); ++i)
	{
		sliderValues.setRowName(i + r, handles2[i]->fullName(tr("_")));
		sliderValues(i+r,0) = handles2[i]->numericalData(tr("Initial value"));
		handleNames += handles2[i]->fullName(tr("_"));
	}
	
	if (param1Box->isVisible())
	{
		int i = menu1->currentIndex();
		if (i < 0) i = 0;
		menu1->clear();
		menu1->addItems(sliderValues.rowNames());
		menu1->setCurrentIndex(i);
	}
	
	if (param2Box->isVisible())
	{
		int i = menu2->currentIndex(),
			j = menu3->currentIndex();
		
		if (i < 0) i = 0;
		if (j < 0) j = 0;
		
		menu2->clear();
		menu3->clear();
		menu2->addItems(sliderValues.rowNames());
		menu3->addItems(handleNames);
		menu2->setCurrentIndex(i);
		menu3->setCurrentIndex(j);
	}
}

