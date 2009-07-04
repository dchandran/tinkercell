/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 A small class that generates the ode and rates file from the given items

****************************************************************************/

#ifndef TINKERCELL_MODELFILEGENERATOR_H
#define TINKERCELL_MODELFILEGENERATOR_H

#include <stdlib.h>
#include <QtGui>
#include <QString>
#include <QStringList>
#include <QLibrary>
#include "MainWindow.h"
#include "DataTable.h"
#include "ItemHandle.h"
#include "Tool.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{
	
	class ModelFileGenerator_FToS : public QObject
	{
		Q_OBJECT
		signals:
			void generateModelFile(QSemaphore*,int*, const QString&, const QList<ItemHandle*>&);
			void getParameters(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&);
			
		public slots:
			int generateModelFile(const char*, Array);
			Matrix getParameters(Array );
	};

	class MY_EXPORT ModelFileGenerator : public Tool
	{
		Q_OBJECT;

	public:
		
		static QString toString(double d);
		ModelFileGenerator();
		bool setMainWindow(MainWindow * main);
		
		static int generateModelFile(const QString& filename, const QList<ItemHandle*>&,const QString& replaceDot = QString("_"));
	public slots:
		void setupFunctionPointers(QLibrary*);
		
	private slots:
		void generateModelFile(QSemaphore*, int*, const QString&, const QList<ItemHandle*>&);
		void getParameters(QSemaphore*,DataTable<qreal>*,const QList<ItemHandle*>&);
		
	protected:
		static ModelFileGenerator_FToS fToS;
		void connectCFuntions();
		static int _generateModelFile(const char*,Array);
		static Matrix _getParameters(Array );
	};
	
	static void replaceHatWithPow(QString& string)
	{
		QRegExp hats[] = { 	QRegExp(QString("([A-Za-z0-9_\\.]+)\\^([A-Za-z0-9_\\.]+)")),
						QRegExp(QString("(\\([^\\(\\)]+\\))\\^([A-Za-z0-9_\\.]+)")),
						QRegExp(QString("([A-Za-z0-9_\\.]+)\\^(\\([^\\(\\)]+\\))")),
						QRegExp(QString("(\\([^\\(\\)]+\\))\\^(\\([^\\(\\)]+\\))")) 
						};
		bool hasHat = true;
		
		while (hasHat)
		{
			for (int l=0; l < 4; ++l)
			{	
				string.replace(hats[l],"pow(\\1,\\2)");
			}
			hasHat = false;
			for (int l=0; l < 4; ++l)
			{	
				if (hats[l].indexIn(string) > -1)
				{
					hasHat = true;
					break;
				}
			}
		}
	}

}

#endif
