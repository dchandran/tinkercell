/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT
 
 This tool exports COPASI models
****************************************************************************/

#ifndef TINKERCELL_COPASIEXPORTTOOL_H
#define TINKERCELL_COPASIEXPORTTOOL_H

#include <QThread>
#include <QSemaphore>
#include <QStringList>
#include "NetworkWindow.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "CThread.h"
#include "copasi_api.h"

namespace Tinkercell
{
	class TINKERCELLEXPORT CopasiExporter_FtoS : public QObject
	{
		Q_OBJECT

		signals:
			void getCopasiModel(QSemaphore*, copasi_model);

		public:
			void getCopasiModel(copasi_model);
	};
	
	/*! \brief This class exports COPASI models. The models are exported as the class
	structure so that simulating the model is simpler (as opposed to writing it to a file). See
	copasi_api.h for details.
	/ingrou plugins
	*/
	class TINKERCELLEXPORT CopasiExporter : public Tool
	{
		Q_OBJECT

	public:

		CopasiExporter();
		~CopasiExporter();
		bool setMainWindow(MainWindow * main);
	
	private slots:
		void getCopasiModel(QSemaphore*, copasi_model);
		void setupFunctionPointers( QLibrary * library);
		void historyChanged(int);
		void windowChanged(NetworkWindow*,NetworkWindow*);

	private:
		static void getCopasiModel(copasi_model);
		void updateModel(copasi_model);
		bool modelNeedsUpdate;

		static CopasiExporter_FtoS fToS;
	};
}

//extern "C" TINKERCELLEXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif

