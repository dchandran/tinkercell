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

	bool EquationParser::validate(NetworkHandle * win, ItemHandle * handle, QString& s, const QStringList& reservedWords)
	{
		if (!win || !handle) return false;

		mu::Parser parser;

		s.replace(QRegExp(QString("\\.(?!\\d)")),QString("__Q_X_Z_W__"));
		parser.SetExpr(s.toAscii().data());
		s.replace(QString("__Q_X_Z_W__"),QString("."));
		parser.SetVarFactory(AddVariable, 0);

		SymbolsTable * symbolsTable = &win->symbolsTable;
		QList<ItemHandle*> allHandles = symbolsTable->uniqueHandlesWithDot.values();

		for (int i=0; i < allHandles.size(); ++i)
		{
			if (allHandles[i] && allHandles[i]->hasTextData(QString("Functions")))
			{
				DataTable<QString>& sDat = allHandles[i]->textDataTable(QString("Functions"));
				for (int j=0; j < sDat.rows(); ++j)
					parser.DefineFun((allHandles[i]->fullName() + QString("__Q_X_Z_W__") + sDat.rowName(j)).toAscii().data(), &(CallFunction), true);
			}
		}

		QString str;

		try
		{
			parser.Eval();

			if (handle)
			{
				bool isItem = !handle->name.isEmpty() && handle->hasNumericalData(QString("Parameters"));
				// Get the map with the variables
				mu::varmap_type variables = parser.GetVar();

				// Get the number of variables
				mu::varmap_type::const_iterator item = variables.begin();

				// Query the variables
				for (; item!=variables.end(); ++item)
				{
					str = QString(item->first.data());
					str.replace(QRegExp(QString("[^A-Za-z0-9_]")),QString(""));
					str.replace(QString("__Q_X_Z_W__"),QString("."));
					QString str2 = str;

					if (handle && !reservedWords.contains(str) &&
						!symbolsTable->uniqueHandlesWithDot.contains(str) &&
						!symbolsTable->uniqueHandlesWithUnderscore.contains(str)) //maybe new symbol in the formula
					{
						if (symbolsTable->nonuniqueData.contains(str) && symbolsTable->nonuniqueData[str].first)
						{
							QList< QPair<ItemHandle*,QString> > localVariables = symbolsTable->nonuniqueData.values(str);
							bool handleHasVar = false;
							for (int i=0; i < localVariables.size(); ++i)
								if (localVariables[i].first == handle)
								{
									handleHasVar = true;
									break;
								}

							if (!handleHasVar)
								handle = symbolsTable->nonuniqueData[str].first;
							
							if (! str.contains(QRegExp(QString("^")+symbolsTable->nonuniqueData[str].first->fullName())) )
							{
								s.replace(QRegExp(QString("^")+str+QString("([^a-zA-Z0-9_])")),handle->fullName() + QString(".") + str + QString("\\1"));
								s.replace(QRegExp(QString("([^a-zA-Z0-9_\\.])")+str+QString("([^a-zA-Z0-9_])")), QString("\\1") + handle->fullName() + QString(".") + str + QString("\\2"));
								s.replace(QRegExp(QString("([^a-zA-Z0-9_\\.])")+str+QString("$")),QString("\\1") + handle->fullName() + QString(".")  + str);
							}
						}
						else
							if (symbolsTable->nonuniqueData.contains(str2) && symbolsTable->nonuniqueData[str2].first)
							{
								QList< QPair<ItemHandle*,QString> > localVariables = symbolsTable->nonuniqueData.values(str2);
								bool handleHasVar = false;
								for (int i=0; i < localVariables.size(); ++i)
									if (localVariables[i].first == handle)
									{
										handleHasVar = true;
										break;
									}

								if (!handleHasVar)
									handle = symbolsTable->nonuniqueData[str2].first;
								
								if (! str2.contains(QRegExp(QString("^")+symbolsTable->nonuniqueData[str2].first->fullName())) )
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
								if (symbolsTable->nonuniqueHandles.contains(str) && symbolsTable->nonuniqueHandles[str])
								{
									s.replace(QRegExp(QString("^")+str+QString("([^a-zA-Z0-9_])")),symbolsTable->nonuniqueHandles[str]->fullName() + QString("\\1"));
									s.replace(QRegExp(QString("([^a-zA-Z0-9_])")+str+QString("([^a-zA-Z0-9_])")), QString("\\1") + symbolsTable->nonuniqueHandles[str]->fullName() + QString("\\2"));
									s.replace(QRegExp(QString("([^a-zA-Z0-9_])")+str+QString("$")),QString("\\1") + symbolsTable->nonuniqueHandles[str]->fullName());
								}
								else
									if (symbolsTable->nonuniqueHandles.contains(str2) && symbolsTable->nonuniqueHandles[str2])
									{
										s.replace(QRegExp(QString("^")+str+QString("([^a-zA-Z0-9_])")),symbolsTable->nonuniqueHandles[str2]->fullName() + QString("\\1"));
										s.replace(QRegExp(QString("([^a-zA-Z0-9_])")+str+QString("([^a-zA-Z0-9_])")), QString("\\1") + symbolsTable->nonuniqueHandles[str2]->fullName() + QString("\\2"));
										s.replace(QRegExp(QString("([^a-zA-Z0-9_])")+str+QString("$")),QString("\\1") + symbolsTable->nonuniqueHandles[str2]->fullName());
									}
									else
									{
										ItemHandle * handle2 = handle;
										QString newp(str);
										int k = newp.indexOf(QString("."));
										bool found = false;
										
										while (k > -1 && !found)
										{
											newp = newp.left(k);
											
											if (symbolsTable->uniqueHandlesWithDot.contains(newp))
											{
												handle2 = symbolsTable->uniqueHandlesWithDot[newp];
												found = true;
											}
											else
											if (symbolsTable->uniqueHandlesWithUnderscore.contains(newp))
											{
												handle2 = symbolsTable->uniqueHandlesWithUnderscore[newp];
												found = true;
											}
											else
											if (symbolsTable->nonuniqueHandles.contains(newp))
											{
												handle2 = symbolsTable->nonuniqueHandles[newp];
												found = true;
											}
											
											k = newp.indexOf(QString("."));
										}
										
										if (found)
										{
											if (!handle2 || !handle2->hasNumericalData(QString("Parameters")))
												handle2 = handle;
											else
												str.remove(newp + QString("_"));
										}
										
										if (!isItem && (handle2 == handle))
										{
											if (win->console())
												win->console()->error(QString("unknown variable : " ) + QString(item->first.data()));
											return false;
										}

										DataTable<qreal> dat(handle2->numericalDataTable(QString("Parameters")));

										if (!str2.contains(QRegExp(QString("^") + handle2->fullName() + QString("\\."))) &&
											!str2.contains(QRegExp(QString("^") + handle2->fullName() + QString("_"))))
										{
											str = str2;
											str2 = handle2->fullName() + QString(".") + str2;
											s.replace(QRegExp(QString("^")+str+QString("([^a-zA-Z0-9_])")),str2 + QString("\\1"));
											s.replace(QRegExp(QString("([^a-zA-Z0-9_\\.])")+str+QString("([^a-zA-Z0-9_])")), QString("\\1") + str2 + QString("\\2"));
											s.replace(QRegExp(QString("([^a-zA-Z0-9_\\.])")+str+QString("$")),QString("\\1") + str2);
										}
										else
										{
											str.replace(QRegExp(QString("^") + handle2->fullName() + QString("\\.")),QString(""));
											str.replace(QRegExp(QString("^") + handle2->fullName() + QString("_")),QString(""));
										}
										
										dat.value(str,0) = 1.0;
										win->changeData(str2 + QString(" = 1"),handle2,QString("Parameters"),&dat);
										if (win->console())
                                            win->console()->message(str2 + QString(" = 1.0"));
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
	
	double EquationParser::eval(NetworkHandle * net, QString& s, bool * b)
	{
		QList<sd_pair> assignments;
		return eval(net,s,b,assignments,0);
	}

	double EquationParser::eval(NetworkHandle * net, QString& s, bool * b, QList<sd_pair> & assignments, mu::Parser * parserPtr)
	{
		if (b)
			(*b) = true;

		if (!net || s.isEmpty())
		{
			if (b)
				(*b) = false;
			return 0.0;
		}

		QString p,n,q;
		QRegExp regex1(QString("[\\n\\s]"));
		QRegExp regex2(QString("\\.(?!\\d)"));
		QRegExp regex3(QString("\\.([^\\.]+)"));

		SymbolsTable * symbolsTable = &net->symbolsTable;
		QList<ItemHandle*> allHandles = symbolsTable->uniqueHandlesWithDot.values();

		QStringList functionNames;
		QList<QStringList> argsList;

		for (int i=0; i < allHandles.size(); ++i)
		{
			if (allHandles[i] && allHandles[i]->hasTextData(QString("Functions")))
			{
				DataTable<QString>& sDat = allHandles[i]->textDataTable(QString("Functions"));
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
		
		try
		{
			parser.SetVarFactory(AddVariable, 0);
			parser.Eval();

			QStringList existingNames;
			for (int i=0; i < assignments.size(); ++i)
			{
				existingNames << assignments[i].first;
				existingNames[i].replace(regex1,QString(""));
				existingNames[i].replace(regex2,QString("_"));
				double * dp = &(assignments[i].second);
				parser.DefineVar(assignments[i].first.toAscii().data(), dp);
			}

			if (net)
			{
				mu::varmap_type variables = parser.GetVar();
				mu::varmap_type::const_iterator item = variables.begin();
									SymbolsTable & symbolsTable = net->symbolsTable;
				for (; item!=variables.end(); ++item)
				{
					n = QString(item->first.data());
					if (existingNames.contains(n)) continue;
					if ((symbolsTable.uniqueHandlesWithDot.contains(n) && (handle = symbolsTable.uniqueHandlesWithDot[n])) ||
						(symbolsTable.uniqueHandlesWithUnderscore.contains(n) && (handle = symbolsTable.uniqueHandlesWithUnderscore[n])))
					{
						if (handle->hasNumericalData(QString("Initial Value")))
						{
							assignments += sd_pair(n,handle->numericalDataTable(QString("Initial Value")).value(0,0));
							parser.DefineVar(item->first.data(), &(assignments.last().second));
						}
						continue;
					}

					if ((symbolsTable.uniqueDataWithDot.contains(n) && (handle = symbolsTable.uniqueDataWithDot[n].first)) ||
					    (symbolsTable.uniqueDataWithUnderscore.contains(n) && (handle = symbolsTable.uniqueDataWithUnderscore[n].first)))
					{
						if (symbolsTable.uniqueDataWithDot.contains(n))
							p = symbolsTable.uniqueDataWithDot[n].second;
						else
							p = symbolsTable.uniqueDataWithUnderscore[n].second;
						q = n;
						q.remove(handle->fullName("_") + QString("_"));
						if (handle && handle->hasNumericalData(p)
							&& handle->numericalDataTable(p).hasRow(q))
							{
								assignments += sd_pair(n,handle->numericalDataTable(p).value(q,0));
								parser.DefineVar(item->first.data(), &(assignments.last().second));
							}
						continue;
					}
					
					assignments += sd_pair(n,1.0);
				}
			}
			
			for (int i=0; i < assignments.size(); ++i)
			{
				assignments[i].first.replace(QObject::tr("_"),QObject::tr("."));
			}

			if (b)
				(*b) = true;
			
			if (parserPtr)
				(*parserPtr) = parser;

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
