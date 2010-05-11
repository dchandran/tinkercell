/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

This file contains static functions for parsing a formula and placing undefined
variable names in the "Numerical Attributes" data table of appropriate item handles.
The class assumes existence of "Functions", "Assignments", and "Numerical Attributes"

****************************************************************************/
#ifndef TINKERCELL_BASICTOOLS_EQUATION_PARSER_H
#define TINKERCELL_BASICTOOLS_EQUATION_PARSER_H

#include <QString>
#include <QPair>
#include <QList>
#include <QStringList>
#include <QRegExp>
#include "NetworkHandle.h"
#include "ItemHandle.h"
#include "ConsoleWindow.h"
#include "muParserDef.h"
#include "muParser.h"
#include "muParserInt.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif


namespace Tinkercell
{
	/*! \brief this class uses Muparser to parse a math equatio and automatically insert undefined
		parameter names into the Numerical Attributes table. It uses the fact that the following tables
		exist: "Functions" and "Numerical Attributes", which is why this class is a bit more specific
		than the parseMath funciton provided in NetworkHandle class*/
	class MY_EXPORT EquationParser
	{
		typedef QPair<QString,qreal> sd_pair;
		
	private:
		
		static double d;
		static double* AddVariable(const char*, void*);
		static double CallFunction(const double*, int);
		static void SubstituteFunctionCalls(const QStringList& functions, const QList<QStringList>& arglist, QString& s);
	
	public:
		/*! \brief check whether the formula is valid and add the undefined strings to the Numerical Attributes
			table of the given handle
			\param NetworkHandle * the working window
			\param ItemHandle* the handle where new parameters will be added
			\param QString the equation, which may also get corrected if some names are incomplete
			\param QStringList list of variable names to ignore (optional)
		*/
		static bool validate(NetworkHandle * win, ItemHandle * handle, QString& s, const QStringList& reservedWords = QStringList());
		
		/*! \brief returns the value of the string. Note: the string should not contain any function calls, except for basic fun
			\param NetworkHandle * the working window
			\param QString the equation
			\param bool* used to indicate whether there was a parse error; false = parse error, true = no error
			\param QList< QPair<QString,double> > optional argument for assigning values to variables
		*/
		static double eval(NetworkHandle * net, QString& s, bool * b = 0, const QList<sd_pair> & assignments = QList<sd_pair>());
	};
}

#endif
