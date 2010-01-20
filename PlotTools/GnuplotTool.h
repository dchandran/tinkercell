/****************************************************************************

 Copyright (c) 2009 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 Tool that launches Gnuplot and generates the script for plotting.

****************************************************************************/

#include <QList>
#include <QString>
#include <QStringList>
#include <QCheckBox>
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

		void gnuplotDataTable(QSemaphore*, DataTable<qreal>& m, int x, const QString& title, int all);
		void gnuplotDataTable3D(QSemaphore*,DataTable<qreal>& m, const QString& title);
		void gnuplotHist(QSemaphore*,DataTable<qreal>& m, double bins, const QString& title);
		void gnuplotErrorbars(QSemaphore*,DataTable<qreal>& m, int x, const QString& title);
		void gnuplotMultiplot(QSemaphore*,int x, int y);
		void getDataTable(QSemaphore*,DataTable<qreal>&, int index);

	public slots:

		void gnuplotMatrix(Matrix m, int x, const char* title, int all);
		void gnuplotMatrix3D(Matrix m, const char * title);
		void gnuplotHistC(Matrix m, double bins, const char * title);
		void gnuplotErrorbarsC(Matrix m, int x, const char* title);
		void gnuplotMultiplot(int x, int y);
		Matrix getDataMatrix(int index);

	};
	
	class MY_EXPORT GnuplotTool : public Tool
	{
	    Q_OBJECT

	private:

        /*! \brief launch gnuplot and plot the given matrix*/
        static void gnuplotMatrix(Matrix m, int x, const char* title, int all);

        /*! \brief launch gnuplot and plot the given surface matrix*/
        static void gnuplotMatrix3D(Matrix m, const char * title);

        /*! \brief launch gnuplot and plot histogram of each column in the given matrix*/
        static void gnuplotHistC(Matrix m, double bins, const char * title);

        /*! \brief launch gnuplot and plot each column with errors listed in the next 2 columns. So every 3rd column is the data.*/
        static void gnuplotErrorbarsC(Matrix m, int x, const char* title);
		
		/*! \brief rows and columns for multiple  plots*/
		static void gnuplotMultiplotC(int x, int y);

        /*! \brief get plotted data*/
        static Matrix getDataMatrix(int index);

	public:
        /*! \brief default constructor*/
        GnuplotTool(QWidget * parent = 0);

        /*! \brief set main window*/
        bool setMainWindow(MainWindow * main);

    public slots:
	
		void runScriptFile(const QString&);
		
		void runScript(const QString&);

	private slots:
	
		void gnuplotDataTable(QSemaphore*,DataTable<qreal>& m, int x, const QString& title, int all);
		
		void gnuplotDataTable3D(QSemaphore*,DataTable<qreal>& m, const QString& title);
		
		void gnuplotHist(QSemaphore*,DataTable<qreal>& m, double bins, const QString& title);
		
		void gnuplotErrorbars(QSemaphore*,DataTable<qreal>& m, int x, const QString& title);
		
		void gnuplotMultiplot(QSemaphore*,int x, int y);
		
		void getDataTable(QSemaphore*,DataTable<qreal>&, int index);

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
	
		int multiplotRows;
		int multiplotCols;
		QStringList previousCommands;
		QStringList labels;
		
		static GnuplotTool_FToS fToS;
		
        /*! \brief all the data that have been plotted so far*/
        QList< DataTable<qreal> > data;

        /*! \brief gnuplot script editor*/
        CodeEditor * editor;
		
		/*! \brief checkboxes*/
        QList<QCheckBox*> checkboxes;
		
	};
}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);
