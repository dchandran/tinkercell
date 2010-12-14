/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool imports and exports SBML models

****************************************************************************/

#ifndef TINKERCELL_SBMLIMPORTEXPORTTOOL_H
#define TINKERCELL_SBMLIMPORTEXPORTTOOL_H

#include <QThread>
#include <QSemaphore>
#include <QStringList>
#include "NetworkWindow.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "CThread.h"
#include "common/sbmlfwd.h"

namespace Tinkercell
{
	class TINKERCELLEXPORT SBMLImportExport_FtoS : public QObject
	{
			Q_OBJECT

		signals:
			void exportSBML(QSemaphore*, const QString&);
			void importSBML(QSemaphore*, const QString&);
		public:
			void exportSBMLFile(const char *);
			void importSBMLString(const char*);
	};
	
	/*! \brief This class imports and exports SBML file format
	and can performs ODE and stochastic simulations. The ODE simulation
	uses SOSlib and the stochastic uses a custom implementation of
	Gillespie algorithm. The SBML document is updated whenever any data
	in the model is changed or when items are inserted or removed.
	/ingrou plugins
	*/
	class TINKERCELLEXPORT SBMLImportExport : public Tool
	{
		Q_OBJECT

	public:

		SBMLImportExport();
		~SBMLImportExport();
		bool setMainWindow(MainWindow * main);
		
		QList<ItemHandle*> importSBML(const QString&);
		SBMLDocument_t* exportSBML(QList<ItemHandle*>&);
		SBMLDocument_t* exportSBML(NetworkHandle * network = 0);
	
	signals:
		void getTextVersion(const QList<ItemHandle*>&, QString*);

	private slots:

		void loadNetwork(const QString& filename);
		void windowChanged(NetworkWindow*,NetworkWindow*);
		void historyChanged(int);
		void setupFunctionPointers( QLibrary * );
		void loadSBMLFile();
		void saveSBMLFile();
		void exportSBML(QSemaphore*, const QString&);
		void importSBML(QSemaphore*, const QString&);
	private:

		bool modelNeedsUpdate;
		void updateSBMLModel();

		SBMLDocument_t * sbmlDocument;
		
		static SBMLImportExport_FtoS fToS;
		static void exportSBMLFile(const char *);
		static void importSBMLString(const char*);
	};
}

//extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif

