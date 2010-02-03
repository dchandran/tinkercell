/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

See header file

****************************************************************************/
#include "EquationParser.h"

namespace Tinkercell
{
	double EquationParser::d = 1.0;

	double* EquationParser::AddVariable(const char*, void*)
	{
		return &d;
	}

	double EquationParser::CallFunction(const double*, int)
	{
		return d;
	}

	void EquationParser::SubstituteFunctionCalls(const QStringList& functions, const QList<QStringList>& arglist, QString& s)
	{
		for (int j=0; j < functions.size() && j < arglist.size(); ++j)
		{
			QStringList args = arglist[j];

			for (int i=0; i < args.size(); ++i)
			{
				s.replace(QRegExp(QString("([^a-ZA-Z0-9])") + args[i] + QString("(?!\\d)")),QString("\\1") + functions[i]);
			}
		}
	}

	bool EquationParser::validate(NetworkWindow * win, ItemHandle * handle, QString& s, const QStringList& reservedWords)
	{
		if (!win || !handle) return false;

		mu::Parser parser;

		s.replace(QRegExp(QString("\\.(?!\\d)")),QString("_qqq_"));
		parser.SetExpr(s.toAscii().data());
		s.replace(QString("_qqq_"),QString("."));
		parser.SetVarFactory(AddVariable, 0);

		SymbolsTable * symbolsTable = &win->symbolsTable;
		QList<ItemHandle*> allHandles = symbolsTable->handlesFullName.values();

		for (int i=0; i < allHandles.size(); ++i)
		{
			if (allHandles[i] && allHandles[i]->hasTextData(QString("Functions")))
			{
				DataTable<QString>& sDat = allHandles[i]->data->textData[QString("Functions")];
				for (int j=0; j < sDat.rows(); ++j)
					parser.DefineFun((allHandles[i]->fullName() + QString("_qqq_") + sDat.rowName(j)).toAscii().data(), &(CallFunction), true);
			}
		}

		QString str;

		try
		{
			parser.Eval();

			if (handle && handle->data && handle->hasNumericalData(QString("Numerical Attributes")))
			{
				// Get the map with the variables
				mu::varmap_type variables = parser.GetVar();

				// Get the number of variables
				mu::varmap_type::const_iterator item = variables.begin();

				// Query the variables
				for (; item!=variables.end(); ++item)
				{
					str = QString(item->first.data());
					str.replace(QRegExp(QString("[^A-Za-z0-9_]")),QString(""));
					str.replace(QString("_qqq_"),QString("."));
					QString str2 = str;
					str2.replace(QString("_"),QString("."));
					if (handle && !reservedWords.contains(str) &&
						!symbolsTable->handlesFullName.contains(str)) //maybe new symbol in the formula
					{
						if (symbolsTable->dataRowsAndCols.contains(str) && symbolsTable->dataRowsAndCols[str].first)
						{
							QList< QPair<ItemHandle*,QString> > localVariables = symbolsTable->dataRowsAndCols.values(str);
							bool handleHasVar = false;
							for (int i=0; i < localVariables.size(); ++i)
								if (localVariables[i].first == handle)
								{
									handleHasVar = true;
									break;
								}

							if (!handleHasVar)
								handle = symbolsTable->dataRowsAndCols[str].first;
							
							if (! str.contains(QRegExp(QString("^")+symbolsTable->dataRowsAndCols[str].first->fullName())) )
							{
								s.replace(QRegExp(QString("^")+str+QString("([^a-zA-Z0-9_])")),handle->fullName() + QString(".") + str + QString("\\1"));
								s.replace(QRegExp(QString("([^a-zA-Z0-9_\\.])")+str+QString("([^a-zA-Z0-9_])")), QString("\\1") + handle->fullName() + QString(".") + str + QString("\\2"));
								s.replace(QRegExp(QString("([^a-zA-Z0-9_\\.])")+str+QString("$")),QString("\\1") + handle->fullName() + QString(".")  + str);
							}
						}
						else
							if (symbolsTable->dataRowsAndCols.contains(str2) && symbolsTable->dataRowsAndCols[str2].first)
							{
								QList< QPair<ItemHandle*,QString> > localVariables = symbolsTable->dataRowsAndCols.values(str2);
								bool handleHasVar = false;
								for (int i=0; i < localVariables.size(); ++i)
									if (localVariables[i].first == handle)
									{
										handleHasVar = true;
										break;
									}

								if (!handleHasVar)
									handle = symbolsTable->dataRowsAndCols[str2].first;
								
								if (! str2.contains(QRegExp(QString("^")+symbolsTable->dataRowsAndCols[str2].first->fullName())) )
								{
									s.replace(QRegExp(QString("^")+str+QString("([^a-zA-Z0-9_])")),handle->fullName() + QString(".") + str2 + QString("\\1"));
									s.replace(QRegExp(QString("([^a-zA-Z0-9_\\.])")+str+QString("([^a-zA-Z0-9_])")), QString("\\1") + handle->fullName() + QString(".") + str2 + QString("\\2"));
									s.replace(QRegExp(QString("([^a-zA-Z0-9_\\.])")+str+QString("$")),QString("\\1") + handle->fullName() + QString(".")  + str2);
								}
								else
								{
									s.replace(QRegExp(QString("^")+str+QString("([^a-zA-Z0-9_])")),str2 + QString("\\1"));
									s.replace(QRegExp(QString("([^a-zA-Z0-9_\\.])")+str+QString("([^a-zA-Z0-9_])")), QString("\\1") + str + QString("\\2"));
									s.replace(QRegExp(QString("([^a-zA-Z0-9_\\.])")+str+QString("$")),QString("\\1") + str);
								}
							}
							else
							{
								if (symbolsTable->handlesFirstName.contains(str) && symbolsTable->handlesFirstName[str])
								{
									s.replace(QRegExp(QString("^")+str+QString("([^a-zA-Z0-9_])")),symbolsTable->handlesFirstName[str]->fullName() + QString("\\1"));
									s.replace(QRegExp(QString("([^a-zA-Z0-9_])")+str+QString("([^a-zA-Z0-9_])")), QString("\\1") + symbolsTable->handlesFirstName[str]->fullName() + QString("\\2"));
									s.replace(QRegExp(QString("([^a-zA-Z0-9_])")+str+QString("$")),QString("\\1") + symbolsTable->handlesFirstName[str]->fullName());
								}
								else
									if (symbolsTable->handlesFirstName.contains(str2) && symbolsTable->handlesFirstName[str2])
									{
										s.replace(QRegExp(QString("^")+str+QString("([^a-zA-Z0-9_])")),symbolsTable->handlesFirstName[str2]->fullName() + QString("\\1"));
										s.replace(QRegExp(QString("([^a-zA-Z0-9_])")+str+QString("([^a-zA-Z0-9_])")), QString("\\1") + symbolsTable->handlesFirstName[str2]->fullName() + QString("\\2"));
										s.replace(QRegExp(QString("([^a-zA-Z0-9_])")+str+QString("$")),QString("\\1") + symbolsTable->handlesFirstName[str2]->fullName());
									}
									else
									{
										//qDebug() << str << "not in symbol table";
										DataTable<qreal> dat(handle->data->numericalData[QString("Numerical Attributes")]);

										if (!str.contains(QRegExp(QString("^") + handle->fullName() + QString("\\."))))
										{
											str2 = handle->fullName() + QString(".") + str;
											s.replace(QRegExp(QString("^")+str+QString("([^a-zA-Z0-9_])")),str2 + QString("\\1"));
											s.replace(QRegExp(QString("([^a-zA-Z0-9_\\.])")+str+QString("([^a-zA-Z0-9_])")), QString("\\1") + str2 + QString("\\2"));
											s.replace(QRegExp(QString("([^a-zA-Z0-9_\\.])")+str+QString("$")),QString("\\1") + str2);
										}
										else
										{
											str2 = str;
											str.replace(QRegExp(QString("^") + handle->fullName() + QString("\\.")),QString(""));
										}
										dat.value(str,0) = 1.0;
										win->changeData(handle->fullName() + QString(".") + str + QString(" = 1"),handle,QString("Numerical Attributes"),&dat);
										if (win->console())
                                            win->console()->message(QString("New parameter ") + str2 + QString(" = 1.0"));
									}
							}
					}
				}
			}
		}
		catch(mu::Parser::exception_type &e)
		{
            if (win->console())
                win->console()->error(QString(e.GetMsg().data()));
			return false;
		}
		return true;
	}

	double EquationParser::eval(NetworkWindow * net, QString& s, bool * b, const QList<sd_pair> & assignments)
	{
		if (!net || s.isEmpty())
		{
			if (b)
				(*b) = false;
			return 0.0;
		}

		QString p,n;
		QRegExp regex1(QString("[\\n\\s]"));
		QRegExp regex2(QString("\\.(?!\\d)"));
		QRegExp regex3(QString("\\.([^\\.]+)"));

		SymbolsTable * symbolsTable = &net->symbolsTable;
		QList<ItemHandle*> allHandles = symbolsTable->handlesFullName.values();

		QStringList functionNames;
		QList<QStringList> argsList;

		for (int i=0; i < allHandles.size(); ++i)
		{
			if (allHandles[i] && allHandles[i]->hasTextData(QString("Functions")))
			{
				DataTable<QString>& sDat = allHandles[i]->data->textData[QString("Functions")];
				for (int j=0; j < sDat.rows(); ++j)
				{
					functionNames << allHandles[i]->fullName() + QString("_") + sDat.rowName(j);
					argsList << sDat.at(j,0).split(QChar(','));
				}
			}
		}

		SubstituteFunctionCalls(functionNames,argsList,s);

		s.replace(regex1,QString(""));
		s.replace(regex2,QString("_"));

		mu::Parser parser;
		parser.SetExpr(s.toAscii().data());

		ItemHandle * handle;

		QList<double> doubles;

		try
		{
			parser.SetVarFactory(AddVariable, 0);
			parser.Eval();

			QStringList existingNames;
			for (int i=0; i < assignments.size(); ++i)
			{
				existingNames << assignments[i].first;
				doubles << assignments[i].second;
				existingNames[i].replace(regex1,QString(""));
				existingNames[i].replace(regex2,QString("_"));
				parser.DefineVar(existingNames[i].toAscii().data(), &(doubles[i]));
			}

			if (net)
			{
				mu::varmap_type variables = parser.GetVar();
				mu::varmap_type::const_iterator item = variables.begin();
									SymbolsTable & symbolsTable = net->symbolsTable;
				for (; item!=variables.end(); ++item)
				{
					n = QString(item->first.data());
					n.replace(QString("_"),QString("."));

					if (existingNames.contains(n)) continue;

					if (symbolsTable.handlesFullName.contains(n) && (handle = symbolsTable.handlesFullName[n]))
					{
						if (handle->data && handle->hasNumericalData(QString("Initial Value")))
						{
							if (handle->data->numericalData[QString("Initial Value")].value(0,0) == 0)
								parser.DefineVar(item->first.data(), &d);
							else
								parser.DefineVar(item->first.data(), &(handle->data->numericalData[QString("Initial Value")].value(0,0)));
						}
						continue;
					}

					if (symbolsTable.handlesFirstName.contains(n) && (handle = symbolsTable.handlesFirstName[n]))
					{
						if (handle->data && handle->hasNumericalData(QString("Initial Value")))
						{
							if (handle->data->numericalData[QString("Initial Value")].value(0,0) == 0)
								parser.DefineVar(item->first.data(), &d);
							else
								parser.DefineVar(item->first.data(), &(handle->data->numericalData[QString("Initial Value")].value(0,0)));
						}

						continue;
					}

					if (symbolsTable.dataRowsAndCols.contains(n) && (handle = symbolsTable.dataRowsAndCols[n].first))
					{
						p = symbolsTable.dataRowsAndCols[n].second;
						regex3.indexIn(n);

						if (regex3.numCaptures() > 0)
							n = regex3.cap(1);

						if (handle->data && handle->hasNumericalData(p)
							&& handle->data->numericalData[p].getRowNames().contains(n))
							{
								parser.DefineVar(item->first.data(), &(handle->data->numericalData[p].value(n,0)));
							}
					}
				}
			}

			if (b)
				(*b) = true;

			return parser.Eval();
		}
		catch(mu::Parser::exception_type &e)
		{
			if (b)
				(*b) = false;
			return 0;
		}

		if (b)
			(*b) = false;
		return 0;
	}

}
