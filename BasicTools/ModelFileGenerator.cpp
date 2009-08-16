/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

A small class that generates the ode and rates file from the given items

****************************************************************************/

#include "ModelFileGenerator.h"
#include "StoichiometryTool.h"
#include "BasicInformationTool.h"

namespace Tinkercell
{
	QString ModelFileGenerator::toString(double d)
	{
		QString s = QString::number(d);
		if (!s.contains("."))
			s += QString(".0");
		return s;
	}

	ModelFileGenerator::ModelFileGenerator() : Tool(tr("Model File Generator"))
	{
		connectCFuntions();
	}

	bool ModelFileGenerator::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);

		if (mainWindow)
		{
			connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),this,SLOT(setupFunctionPointers( QLibrary * )));
		}
		return (mainWindow != 0);
	}

	typedef void (*tc_ModelFileGenerator_api)( 
		int (*writeModel)(const char*,Array),
		Matrix (*getParameters)(Array )
		);

	void ModelFileGenerator::setupFunctionPointers( QLibrary * library)
	{
		tc_ModelFileGenerator_api f = (tc_ModelFileGenerator_api)library->resolve("tc_ModelFileGenerator_api");
		if (f)
		{
			f(
				&(_generateModelFile),
				&(_getParameters)
				);
		}
	}

	void ModelFileGenerator::connectCFuntions()
	{
		connect(&fToS,SIGNAL(generateModelFile(QSemaphore*,int*, const QString&,const QList<ItemHandle*>&)),this,SLOT(generateModelFile(QSemaphore*,int*,const QString&,const QList<ItemHandle*>&)));
		connect(&fToS,SIGNAL(getParameters(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&)),this,SLOT(getParameters(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&)));
	}

	ModelFileGenerator_FToS ModelFileGenerator::fToS;

	int ModelFileGenerator::_generateModelFile(const char* filename, Array a0)
	{
		return fToS.generateModelFile(filename,a0);
	}

	int ModelFileGenerator_FToS::generateModelFile(const char* filename, Array a0)
	{
		QList<ItemHandle*> * list = ConvertValue(a0);
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		int i=0;
		emit generateModelFile(s,&i,ConvertValue(filename),*list);
		s->acquire();
		s->release();
		delete s;
		delete list;
		return i;	
	}

	Matrix ModelFileGenerator::_getParameters(Array A)
	{
		return fToS.getParameters(A);
	}

	Matrix ModelFileGenerator_FToS::getParameters(Array a0)
	{
		QSemaphore * s = new QSemaphore(1);
		QList<ItemHandle*> * list = ConvertValue(a0);
		DataTable<qreal> * p = new DataTable<qreal>;
		s->acquire();
		emit getParameters(s,p,*list);
		s->acquire();
		s->release();
		delete s;
		delete list;
		if (p)
		{
			Matrix m = ConvertValue(*p);
			delete p;
			return m;
		}
		return emptyMatrix();
	}

	void ModelFileGenerator::generateModelFile(QSemaphore * s, int* i, const QString& filename, const QList<ItemHandle*>& items)
	{
		if (i)
			(*i) = ModelFileGenerator::generateModelFile(filename, items);
		else
			ModelFileGenerator::generateModelFile(filename, items);

		if (s)
			s->release();
	}
	
	DataTable<qreal> ModelFileGenerator::getUsedParameters(const QList<ItemHandle*>& handles, const QString& replaceDot)
	{
		int i,j;
		QStringList rates = StoichiometryTool::getRates(handles, replaceDot);
		DataTable<qreal> params = BasicInformationTool::getParameters(handles,QStringList(), QStringList(), replaceDot);
		params.insertCol(1,tr("used"));

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

		for (i=0; i < handles.size(); ++i)
		{
			if (!handles[i])
				continue;

			if (handles[i]->hasTextData(tr("Events")))
			{
				DataTable<QString>& dat = handles[i]->data->textData[tr("Events")];
				if (dat.cols() == 1)
					for (j=0; j < dat.rows(); ++j)
					{
						s1 =  dat.rowName(j);

						s1.replace(regex,replaceDot);
						s2 =  dat.value(j,0);

						s2.replace(regex,replaceDot);

						if (s1.isEmpty() || s2.isEmpty()) continue;

						for (int k=0; k < params.rows(); ++k)
							if (s2.contains(params.rowName(k)) || s1.contains(params.rowName(k)))
								params.value(k,1) = 1.0;
					}
			}

			if (handles[i]->hasTextData(tr("Assignments")))
			{
				DataTable<QString>& dat = handles[i]->data->textData[tr("Assignments")];
				if (dat.cols() == 1)
					for (j=0; j < dat.rows(); ++j)
					{
						s1 =  dat.rowName(j);
						s1.replace(regex,replaceDot);
						s2 =  dat.value(j,0);
						s2.replace(regex,replaceDot);

						if (s1.isEmpty() || s2.isEmpty()) continue;

						for (int k=0; k < params.rows(); ++k)
							if (s2.contains(params.rowName(k)) || s1.contains(params.rowName(k)))
								params.value(k,1) = 1.0;
					}
			}
		}

		int count = 0;
		for (int i=0; i < params.rows(); ++i)
		{
			if (params.value(i,1) > 0.0) ++count;
		}

		DataTable<qreal> params2;
		params2.resize(count,1);
		params2.colName(0) = params.colName(0);

		for (int i=0, j=0; i < params.rows() && j < count; ++i)
		{
			if (params.value(i,1) > 0.0)
			{
				params2.rowName(j) = params.rowName(i);
				params2.value(j,0) = params.value(i,0);
				++j;
			}
		}
		return params2;
	}

	void ModelFileGenerator::getParameters(QSemaphore* s,DataTable<qreal>* ptr,const QList<ItemHandle*>& handles)
	{
		if (ptr)
		{
			QString replaceDot("_");
			(*ptr) = getUsedParameters(handles,replaceDot);
		}
		if (s)
			s->release();
	}

	int ModelFileGenerator::generateModelFile(const QString& filename, const QList<ItemHandle*>& handles,const QString& replaceDot)
	{
		//qDebug() << "model file in " << filename;
		QFile cfile (filename + QString(".c"));
		QFile pyfile (filename + QString(".py"));

		int i,j;

		if (!cfile.open(QFile::WriteOnly | QFile::Text) || !pyfile.open(QFile::WriteOnly | QFile::Text))
		{
			return 0;
		}
		QStringList cmodel, pymodel;

		//multiple cells

		QStringList assignedHandles;

		QList< QPair<ItemHandle*,double> > modifiedHandles;
		for (i=0; i < handles.size(); ++i)
		{
			if (handles[i] && handles[i]->data && handles[i]->hasNumericalData(QString("Numerical Attributes"))
				&& handles[i]->family() && handles[i]->family()->isA("Cell"))
			{
				QList<ItemHandle*> handlesInCell;
				double popSz = handles[i]->data->numericalData[QString("Numerical Attributes")].at("Count",0);
				if (popSz > 0)
				{
					handlesInCell << handles[i]->children;

					for (int j=0; j < handlesInCell.size(); ++j)
					{
						handlesInCell << handlesInCell[j]->children;
					}
					for (int j=0; j < handlesInCell.size(); ++j)
						modifiedHandles << QPair<ItemHandle*,double>(handlesInCell[j],popSz);

				}
			}
		}

		ItemHandle* handle;
		double popSz;
		for (i=0; i < modifiedHandles.size(); ++i)
		{
			handle = modifiedHandles[i].first;
			popSz =  modifiedHandles[i].second;
			if (handle && handle->family() && handle->family()->isA("Node") && handle->data && handle->hasNumericalData(QString("Initial Value")))
			{
				DataTable<qreal>& dat = handle->data->numericalData[QString("Initial Value")];
				dat.value(0,0) *= popSz;
			}
		}

		//

		QStringList rates = StoichiometryTool::getRates(handles, replaceDot);

		for (i=0; i < rates.size(); ++i)
		{
			if (rates[i].isEmpty())
				rates[i] = QString("0.0");
			replaceHatWithPow(rates[i]);
		}

		DataTable<qreal> N = StoichiometryTool::getStoichiometry(handles,replaceDot);
		if (N.rows() < 1 || N.cols() < 1 || rates.isEmpty()) return 0;

		DataTable<qreal> params = BasicInformationTool::getParameters(handles,QStringList(), QStringList(), replaceDot);
		params.insertCol(1,tr("used"));

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

		QStringList expressions, code, pycode;

		//restore modified cells
		for (i=0; i < modifiedHandles.size(); ++i)
		{
			handle = modifiedHandles[i].first;
			popSz =  modifiedHandles[i].second;
			if (handle && handle->family() && handle->family()->isA("Node") && handle->data && handle->hasNumericalData("Initial Value"))
			{
				DataTable<qreal>& dat = handle->data->numericalData["Initial Value"];
				dat.value(0,0) /= popSz;
			}
		}

		cmodel << "#include <math.h>\n\n";

		QRegExp regex(tr("\\.(?!\\d)"));
		QString s1,s2;
		QStringList vars;
		QStringList initValues;

		for (i=0; i < handles.size(); ++i)
		{
			if (handles[i] && handles[i]->family())
			{
				if (handles[i]->data)
				{
					if (handles[i]->hasNumericalData(tr("Initial Value")))
					{
						cmodel << tr("double ") + handles[i]->fullName(replaceDot) + tr(" = ") + 
							toString(handles[i]->data->numericalData[tr("Initial Value")].value(0,0)) + tr(";\n");
						pymodel << handles[i]->fullName(replaceDot) + tr(" = ") + 
							toString(handles[i]->data->numericalData[tr("Initial Value")].value(0,0)) + tr(";\n");
						vars << handles[i]->fullName(replaceDot);
						initValues << toString(handles[i]->data->numericalData[tr("Initial Value")].value(0,0));
					}
					else
					{
						cmodel << tr("double ") + handles[i]->fullName(replaceDot) + tr(" = 0.0;\n");
						pymodel << handles[i]->fullName(replaceDot) + tr(" = 0.0;\n");
						vars << handles[i]->fullName(replaceDot);
						initValues << "0.0";
					}
					if (handles[i]->hasNumericalData(tr("Fixed")) &&
						handles[i]->data->numericalData[tr("Fixed")].at(0,0) > 0)
					{
						int k = N.rowNames().indexOf(handles[i]->fullName(replaceDot));
						if (k >= 0)
							N.removeRow(k);
					}
				}
				if (handles[i]->hasTextData(tr("Events")))
				{
					DataTable<QString>& dat = handles[i]->data->textData[tr("Events")];
					if (dat.cols() == 1)
						for (j=0; j < dat.rows(); ++j)
						{
							s1 =  dat.rowName(j);
							replaceHatWithPow(s1);

							s1.replace(regex,replaceDot);
							s2 =  dat.value(j,0);

							replaceHatWithPow(s2);
							s2.replace(regex,replaceDot);

							if (s1.isEmpty() || s2.isEmpty()) continue;

							code << tr("   if (") + s1 + tr(") {") + s2 + tr(";}\n");
							pycode << tr("   if (") + s1 + tr("): ") + s2 + tr(";\n");

							for (int k=0; k < params.rows(); ++k)
								if (s2.contains(params.rowName(k)) || s1.contains(params.rowName(k)))
									params.value(k,1) = 1.0;
						}
				}
				if (handles[i]->hasTextData(tr("Functions")))
				{
					DataTable<QString>& dat = handles[i]->data->textData[tr("Functions")];

					if (dat.cols() == 2)
					{
						for (j=0; j < dat.rows(); ++j)
						{
							s1 = dat.value(j,1);
							replaceHatWithPow(s1);
							s1.replace(regex,replaceDot);

							s2 = dat.value(i,0);
							s2.replace(tr(","),tr(", double "));
							s2 = tr("(double ") + s2 + tr(")");

							cmodel << tr("   double ") + handles[i]->fullName() + tr("_") + dat.rowName(j)
								+ s2 + tr("{ return ") + s1 + tr(";}\n");

							pymodel << tr("   defn ") + handles[i]->fullName() + tr("_") + dat.rowName(j)
								+ s2 + tr(": return ") + s1 + tr(";\n");
						}
					}
				}

				if (handles[i]->hasTextData(tr("Assignments")))
				{
					DataTable<QString>& dat = handles[i]->data->textData[tr("Assignments")];
					if (dat.cols() == 1)
						for (j=0; j < dat.rows(); ++j)
						{
							s1 =  dat.rowName(j);
							s1.replace(regex,replaceDot);
							s2 =  dat.value(j,0);

							replaceHatWithPow(s2);
							s2.replace(regex,replaceDot);

							if (s1.isEmpty() || s2.isEmpty()) continue;

							if (s1.isEmpty() || s1 == handles[i]->fullName() || s1 == handles[i]->fullName(replaceDot))
							{
								expressions << tr("   ") + handles[i]->fullName(replaceDot) + tr(" = ") + s2 + tr(";\n");
								int k = N.rowNames().indexOf(handles[i]->fullName(replaceDot));
								if (k >= 0)
									N.removeRow(k);
							}
							else
								if (handles[i]->hasNumericalData(tr("Numerical Attributes")) &&
									handles[i]->data->numericalData["Numerical Attributes"].rowNames().contains(dat.rowName(j)))
								{
									expressions << tr("   ") + handles[i]->fullName(replaceDot) + replaceDot + s1 + tr(" = ") + s2 + tr(";\n");
								}
								else
								{
									expressions << tr("   ") + handles[i]->fullName(replaceDot) + replaceDot + s1 + tr(" = ") + s2 + tr(";\n");
									cmodel << tr("double ") + handles[i]->fullName(replaceDot) + replaceDot + s1 + tr(" = 0.0;\n");
									pymodel << handles[i]->fullName(replaceDot) + replaceDot + s1 + tr(" = 0.0;\n");
								}
								for (int k=0; k < params.rows(); ++k)
									if (s2.contains(params.rowName(k)) || s1.contains(params.rowName(k)))
										params.value(k,1) = 1.0;
						}
				}
			}
		}

		for (i=0; i < N.rows(); ++i)
			if (!vars.contains(N.rowName(i)))
			{
				cmodel << tr("double ") + N.rowName(i) + tr(" = 0.0;\n");
				pymodel << N.rowName(i) + tr(" = 0.0;\n");
			}

			for (i = 0; i < params.rows(); ++i)
				if (params.value(i,1) > 0)
				{
					cmodel << "double " << params.rowName(i) << " = " << toString(params.at(i,0)) << ";\n";
					pymodel << params.rowName(i) << " = " << toString(params.at(i,0)) << ";\n";
				}

				QStringList trueParams;
				//write assign parameters function (not needed for simulation, but could be useful for some purpose)
				cmodel << "\nvoid assignParameters( double * k )\n { \n";
				pymodel << "\ndefn assignParameters ( k ):\n";
				j = 0;
				for (i = 0; i < params.rows(); ++i)
					if (params.value(i,1) > 0)
					{
						trueParams << params.rowName(i);
						cmodel << "   " << params.rowName(i) << " = k[" << QString::number(j) << "];\n";
						pymodel << "   " << params.rowName(i) << " = k[" << QString::number(j) << "];\n";
						++j;
					}
					cmodel << "}\n\n";
					pymodel << "\n\n";

					//print header and beginning of ode function f
					cmodel << "\nvoid TCpropensity(double time, double * u, double * rates, void * data)\n{\n";
					pymodel << "\ndefn TCpropensity(u, time):\n";

					//declare variables
					for (i = 0; i < N.rows(); ++i)
					{
						cmodel << "   " << N.rowName(i) << " = u[" << QString::number(i) << "];\n";
						pymodel << "   " << N.rowName(i) << " = u[" << QString::number(i) << "];\n";
					}

					//write code
					if (!code.isEmpty() || !expressions.isEmpty())
					{

						cmodel << expressions;
						pymodel << expressions;

						cmodel << code;
						pymodel << pycode;

						//declare variables
						for (i = 0; i < N.rows(); ++i)
						{
							cmodel << "   " << "u[" << QString::number(i) << "] = " << N.rowName(i) << ";\n";
							pymodel << "   " << "u[" << QString::number(i) << "] = " << N.rowName(i) << ";\n";
						}
					}

					//print the rates
					pymodel << tr("   rates = range(0,") + QString::number(rates.size()) + tr(");\n");
					for (i = 0; i < rates.size(); ++i)
					{
						cmodel << "   rates[" << QString::number(i) << "] = " << rates[i] << ";\n";
						pymodel << "   rates[" << QString::number(i) << "] = " << rates[i] << ";\n";
					}
					cmodel << "};\n";
					pymodel << "   return rates;\n";

					cmodel << "double TCstoic[] = { ";
					pymodel << "TCstoic = ( ";

					for (i = 0; i < N.rows(); ++i)
						for (j=0; j < N.cols(); ++j)
						{
							cmodel << toString( N.at(i,j) ) << ",";
							pymodel << QString::number( N.at(i,j) ) << ",";
						}
						cmodel.pop_back(); //remove last comma
						pymodel.pop_back();

						cmodel << "};\n\n";
						pymodel << ");\n\n";

						/* ODE model
						cmodel << "\nvoid TCodeFunc( double time, double * u, double * du, void * udata )\n { \n";
						pymodel << "\ndefn TCodeFunc( u, time ):\n";

						//declare variables
						for (i = 0; i < N.rows(); ++i)
						{
						cmodel << "   " << N.rowName(i) << " = u[" << QString::number(i) << "];\n";
						pymodel << "   " << N.rowName(i) << " = u[" << QString::number(i) << "];\n";
						}

						//write code
						if (!code.isEmpty() || !expressions.isEmpty())
						{

						cmodel << expressions;
						pymodel << expressions;

						cmodel << code;
						pymodel << pycode;

						//declare variables
						for (i = 0; i < N.rows(); ++i)
						{
						cmodel << "   " << "u[" << QString::number(i) << "] = " << N.rowName(i) << ";\n";
						pymodel << "   " << "u[" << QString::number(i) << "] = " << N.rowName(i) << ";\n";
						}
						}

						cmodel << "   double rates[] = {";
						pymodel << "   rates = (";

						//print the rates
						for (i = 0; i < rates.size(); ++i)
						{
						if (i < rates.size() - 1)
						{
						cmodel << "( " << rates[i] << " ), ";
						pymodel << "( " << rates[i] << " ), ";
						}
						else
						{
						cmodel << "( " << rates[i] << " )};\n";
						pymodel << "( " << rates[i] << " ));\n";
						}
						}

						//print odes
						pymodel << tr("   du = range(0,") + QString::number(N.rows()) + tr(");\n");

						for (i = 0; i < N.rows(); ++i)
						{
						cmodel << "   du[" << QString::number(i) << "] = 0 ";
						pymodel << "   du[" << QString::number(i) << "] = 0 ";
						for (j = 0; j < N.cols(); ++j)
						{
						if (N.at(i,j) != 0)
						{
						cmodel << " + (" << toString(N.at(i,j)) << ")*rates[" << QString::number(j) << "]";
						pymodel << " + (" << QString::number(N.at(i,j)) << ")*rates[" << QString::number(j) << "]";
						}
						}
						cmodel << ";\n";
						pymodel << ";\n";
						}
						cmodel << "}\n\n";
						pymodel << "   return du;\n\n";
						*/
						//write simulation information
						cmodel << "int TCvars = " << QString::number(N.rows()) << ";\n";
						cmodel << "int TCreactions = " << QString::number(N.cols()) << ";\n";
						cmodel << "int TCparams = " << QString::number(trueParams.size()) << ";\n";

						pymodel << "TCvars = " << QString::number(N.rows()) << ";\n";
						pymodel << "TCreactions = " << QString::number(N.cols()) << ";\n";
						pymodel << "TCparams = " << QString::number(trueParams.size()) << ";\n";

						//variable names
						cmodel << "char * TCvarnames[] = {\"" << N.getRowNames().join("\",\"") << "\"};\n\n";
						pymodel << "TCvarnames = (\"" << N.getRowNames().join("\",\"") << "\");\n\n";

						//reaction names
						cmodel << "char * TCreactionnames[] = {\"" << N.getColNames().join("\",\"") << "\"};\n\n";
						pymodel << "TCreactionnames = (\"" << N.getColNames().join("\",\"") << "\");\n\n";

						//param names
						cmodel << "char * TCparamnames[] = {\"" << trueParams.join("\",\"") << "\"};\n\n";
						pymodel << "TCparamnames = (\"" << trueParams.join("\",\"") << "\");\n\n";

						cmodel << "double TCinit[" << QString::number(N.rows()) << "];\n";
						pymodel << "TCinit = range(0," << QString::number(N.rows()) << ");\n";

						cmodel << "\nvoid TCinitialize()\n{\n";
						pymodel << "\ndefn TCinitialize():\n";

						for (i = 0; i < N.rows(); ++i)
						{
							int k = vars.indexOf(N.rowName(i));
							if (k > -1)
							{
								cmodel << "   " <<  N.rowName(i) << " = TCinit["<< QString::number(i) << "] = " << initValues[k] << ";\n";
								pymodel << "   " <<  N.rowName(i) << " = TCinit[" << QString::number(i) << "] = " << initValues[k] << ";\n";
							}
							else
							{
								cmodel << "   TCinit[" << QString::number(i) << "] = 0.0;\n";
								pymodel << "   TCinit[" << QString::number(i) << "] = 0.0;\n";
							}
						}
						cmodel << "}\n\n";
						pymodel << "\n\n";
						
						cmodel << "\nvoid TCreinitialize()\n{\n";
						pymodel << "\ndefn TCreinitialize():\n";

						for (i = 0; i < N.rows(); ++i)
						{
							int k = vars.indexOf(N.rowName(i));
							if (k > -1)
							{
								cmodel << "   TCinit[" << QString::number(i) << "] = " << N.rowName(i) << ";\n";
								pymodel << "   TCinit[" << QString::number(i) << "] = " << N.rowName(i) << ";\n";
							}
						}
						cmodel << "}\n\n";
						pymodel << "\n\n";
						

						cfile.write(cmodel.join("").toAscii());
						cfile.close();

						pyfile.write(pymodel.join("").toAscii());
						pyfile.close();
						return 1;
	}
}
