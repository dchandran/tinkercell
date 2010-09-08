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

namespace Tinkercell
{	
	class TINKERCELLEXPORT GnuplotTool : public Tool
	{
	    Q_OBJECT

	public:
        /*! \brief default constructor*/
        GnuplotTool(QWidget * parent = 0);

        /*! \brief set main window*/
        bool setMainWindow(MainWindow * main);

    public slots:
	
		void runScriptFile(const QString&);
		
		void runScript(const QString&);

	private slots:
	
		void gnuplotDataTable(DataTable<qreal>& m, int x, const QString& title, int all);
		
		void gnuplotDataTable3D(DataTable<qreal>& m, const QString& title);
		
		void gnuplotHist(DataTable<qreal>& m, double bins, const QString& title);
		
		void gnuplotErrorbars(DataTable<qreal>& m, int x, const QString& title);
		
		void gnuplotMultiplot(int x, int y);

        /*! \brief run the current gnuplot script */
        void runScript();

        /*! \brief save plot */
        void savePlot();

        /*! \brief for C API */
       // void setupFunctionPointers( QLibrary * );

        /*! \brief prevent other plot tool from loading */
       //void toolAboutToBeLoaded( Tool * , bool * );

	private:
	
		int multiplotRows;
		int multiplotCols;
		QStringList previousCommands;
		QStringList labels;
		
        /*! \brief gnuplot script editor*/
        CodeEditor * editor;
		
		/*! \brief checkboxes*/
        QList<QCheckBox*> checkboxes;
		
	};
}
