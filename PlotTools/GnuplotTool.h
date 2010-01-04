/****************************************************************************

 Copyright (c) 2009 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 Tool that launches Gnuplot and generates the script for plotting.

****************************************************************************/

#include <QList>
#include "MainWindow.h"
#include "Tool.h"
#include "CodeEditor.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{
	class GnuplotTool_FToS : public QObject
	{
		Q_OBJECT

	signals:

		void runScriptFile(const QString&);
		void runScript(const QString&);


	public slots:

		void gnuplotFile(const QString&);
		void gnuplotScript(const QString&);
	};
	
	class MY_EXPORT GnuplotTool : public Tool
	{
	    Q_OBJECT

	public:

        /*! \brief launch gnuplot with the given script
        \param QString gnuplot script*/
        static void gnuplotScript(const QString& script=QString("plot sin(x);\n"));

        /*! \brief launch gnuplot with the given script filename
        \param QString gnuplot script file*/
        static void gnuplotFile(const QString&);

        /*! \brief launch gnuplot and plot the given matrix*/
        static void gnuplotMatrix(Matrix m, int x, const char* title, int all);

        /*! \brief launch gnuplot and plot the given matrix*/
        static void gnuplotDataTable(DataTable<qreal>& m, int x, const QString& title, int all);

        /*! \brief launch gnuplot and plot the given surface matrix*/
        static void gnuplotMatrix3D(Matrix m, const char * title);

        /*! \brief launch gnuplot and plot the given surface matrix*/
        static void gnuplotDataTable3D(DataTable<qreal>& m, const QString& title);

        /*! \brief launch gnuplot and plot histogram of each column in the given matrix*/
        static void gnuplotHistC(Matrix m, double bins, const char * title);

        /*! \brief launch gnuplot and plot histogram of each column in the given matrix*/
        static void gnuplotHist(DataTable<qreal>& m, double bins, const QString& title);

        /*! \brief launch gnuplot and plot each column with errors listed in the next 2 columns. So every 3rd column is the data.*/
        static void gnuplotErrorbarsC(Matrix m, int x, const char* title);

        /*! \brief launch gnuplot and plot each column with errors listed in the next 2 columns. So every 3rd column is the data.*/
        static void gnuplotErrorbars(DataTable<qreal>& m, int x, const QString& title);
		
		/*! \brief rows and columns for multiple  plots*/
		static void GnuplotTool::gnuplotMultiplot(int x, int y);

        /*! \brief get plotted data*/
        static DataTable<qreal>& getDataTable(int index);

        /*! \brief get plotted data*/
        static Matrix getDataMatrix(int index);

        /*! \brief default constructor*/
        GnuplotTool(QWidget * parent = 0);

        /*! \brief set main window*/
        bool setMainWindow(MainWindow * main);

    private slots:
	
		void runScriptFile(const QString&);
		
		void runScript(const QString&);

        /*! \brief run the current gnuplot script */
        void runScript();

        /*! \brief save plot */
        void savePlot();

        /*! \brief copy data */
        void copyData();

        /*! \brief copy data */
        void writeData();

        /*! \brief for C API */
        void setupFunctionPointers( QLibrary * );

        /*! \brief prevent other plot tool from loading */
        void toolAboutToBeLoaded( Tool * , bool * );

	private:
	
		static int multiplotRows;
		static int multiplotCols;
		QStringList previousCommands;
		
		static GnuplotTool_FToS fToS;
		
        /*! \brief all the data that have been plotted so far*/
        static QList< DataTable<qreal> > data;

        /*! \brief gnuplot script editor*/
        CodeEditor * editor;

	};
}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);
