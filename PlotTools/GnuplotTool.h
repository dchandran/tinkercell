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
	class MY_EXPORT GnuplotTool : public Tool
	{
	    Q_OBJECT

	public:

        /*! \brief launch gnuplot with the given script
        \param QString gnuplot script*/
        static void gnuplot(MainWindow *, const QString& script=QString());

        /*! \brief launch gnuplot and plot the given matrix*/
        static void gnuplotMatrix(Matrix m, int x, const char* title, int all);

        /*! \brief launch gnuplot and plot the given matrix*/
        static void gnuplotDataTable(const DataTable<qreal>& m, int x, const QString& title, int all);

        /*! \brief launch gnuplot and plot the given surface matrix*/
        static void gnuplotMatrix3D(Matrix m, double xmin, double xmax, double ymin, double ymax, const char * title);

        /*! \brief launch gnuplot and plot the given surface matrix*/
        static void gnuplotDataTable3D(const DataTable<qreal>& m, double xmin, double xmax, double ymin, double ymax, const QString& title);

        /*! \brief get plotted data*/
        static DataTable<qreal>& getDataTable(int index);

        /*! \brief get plotted data*/
        static Matrix getDataMatrix(int index);

        /*! \brief default constructor*/
        GnuplotTool(QWidget * parent = 0);

        /*! \brief set main window*/
        bool setMainWindow(MainWindow * main);

    private slots:

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

	private:
        /*! \brief all the data that have been plotted so far*/
        static QList< DataTable<qreal> > data;

        /*! \brief gnuplot script editor*/
        CodeEditor * editor;

	};
}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);
