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
#include "MainWindow.h"
#include <QStringList>
#include "NetworkWindow.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "CThread.h"
#include "sbml/common/sbmlfwd.h"

#ifndef TINKERCELLEXPORT
#ifdef Q_WS_WIN
#   if defined(TC_EXPORTS) || defined(TinkerCell_EXPORTS)
#       define TINKERCELLEXPORT __declspec(dllexport)
#   else
#       define TINKERCELLEXPORT __declspec(dllimport)
#   endif
#else
#    define TINKERCELLEXPORT
#endif
#endif

namespace Tinkercell
{
	class TINKERCELLEXPORT SBMLImportExport_FtoS : public QObject
	{
			Q_OBJECT

		signals:
			void exportSBML(QSemaphore*, const QString&);
			void importSBML(QSemaphore*, const QString&);
			void exportAntimony(QSemaphore*, const QString&);
			void importAntimony(QSemaphore*, const QString&);
			void exportMath(QSemaphore*, const QString&);
			void exportSBMLString(QSemaphore*, QString*);
			void exportAntimonyString(QSemaphore*, QString*);
		public:
			void exportSBMLFile(const char *);
			void importSBMLString(const char*);
			void exportAntimonyFile(const char *);
			void importAntimonyString(const char*);
			void exportMathFile(const char *);
			const char * exportSBMLString();
			const char * exportAntimonyString();
	};
	
	/*! \brief This class imports and exports SBML file format.
	/ingrou plugins
	*/
	class TINKERCELLEXPORT SBMLImportExport : public Tool
	{
		Q_OBJECT

	public:

		SBMLImportExport();
		~SBMLImportExport();
		bool setMainWindow(MainWindow * main);
		
		void exportSBML(const QString&);
		static QList<ItemHandle*> importSBML(const QString&, ItemHandle * root=0);
		SBMLDocument_t* exportSBML(QList<ItemHandle*>&);
		SBMLDocument_t* exportSBML(NetworkHandle * network = 0);
	
	signals:
		void getTextVersion(const QList<ItemHandle*>&, QString*);

	private slots:

		void loadNetwork(const QString& filename, bool *);
		void windowChanged(NetworkWindow*,NetworkWindow*);
		void historyChanged(int);
		void setupFunctionPointers( QLibrary * );
		void loadSBMLFile();
		void saveSBMLFile();
		void exportSBML(QSemaphore*, const QString&);
		bool importSBML(QSemaphore*, const QString&);
		void exportAntimony(QSemaphore*, const QString&);
		void importAntimony(QSemaphore*, const QString&);
		void exportMath(QSemaphore*, const QString&);
		void exportSBMLString(QSemaphore*,QString*);
		void exportAntimonyString(QSemaphore*,QString*);

		void getItemsFromFile(QList<ItemHandle*>&, QList<QGraphicsItem*>&, const QString& filename,ItemHandle * root=0);

	private:

		bool modelNeedsUpdate;
		void updateSBMLModel();

		SBMLDocument_t * sbmlDocument;
		
		static SBMLImportExport_FtoS * fToS;
		static void exportSBMLFile(const char *);
		static void importSBMLString(const char*);
		static void exportAntimonyFile(const char *);
		static void importAntimonyString(const char*);
		static void exportMathFile(const char *);
		static const char * _exportSBMLString();
		static const char * _exportAntimonyString();
	};
}

//extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif

