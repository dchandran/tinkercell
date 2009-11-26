/****************************************************************************

 Copyright (c) 2009 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 See GnuplotTool.h
****************************************************************************/

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QToolButton>
#include <QProcess>
#include "ConsoleWindow.h"
#include "GnuplotTool.h"

namespace Tinkercell
{
    QList< DataTable<qreal> > GnuplotTool::data;

    void GnuplotTool::gnuplot(const QString& script)
    {
    #ifdef Q_WS_WIN
        QProcess::execute(QCoreApplication::applicationDirPath() +
                        tr("/win32/pgnuplot.exe"));
    #else

    #endif
    }

    void GnuplotTool::gnuplotMatrix(Matrix m, int x, const char* title, int all)
    {
    }

    void GnuplotTool::gnuplotDataTable(const DataTable<qreal>& m, int x, const QString& title, int all)
    {
    }

    void GnuplotTool::gnuplotMatrix3D(Matrix m, double xmin, double xmax, double ymin, double ymax, const char * title)
    {
    }

    void GnuplotTool::gnuplotDataTable3D(const DataTable<qreal>& m, double xmin, double xmax, double ymin, double ymax, const QString& title)
    {
    }

    DataTable<qreal>& GnuplotTool::getDataTable(int index)
    {
        if (data.isEmpty())
            data << DataTable<qreal>();

        if (index < 0) index = 0;
        if (index >= data.size()) index = data.size() - 1;
        return data[index];
    }

    Matrix GnuplotTool::getDataMatrix(int index)
    {
        ConvertValue(getDataTable(index));
    }

    GnuplotTool::GnuplotTool(QWidget * parent) : Tool(tr("gnuplot"),parent)
    {
        editor = new CodeEditor;
        QHBoxLayout * layout = new QHBoxLayout;
        QVBoxLayout * buttonsLayout = new QVBoxLayout;

        QToolButton * button1 = new QToolButton;
        button1->setIcon(QIcon(tr(":/images/save.png")));

        QToolButton * button2 = new QToolButton;
        button2->setIcon(QIcon(tr(":/images/copy.png")));

        QToolButton * button3 = new QToolButton;
        button3->setIcon(QIcon(tr(":/images/play.png")));

        buttonsLayout->addWidget(button1);
        buttonsLayout->addWidget(button2);
        buttonsLayout->addWidget(button3);

        layout->addWidget(editor);
        layout->addLayout(buttonsLayout);
        layout->setStretch(0,1);
        layout->setStretch(1,0);

        layout->setContentsMargins(0,0,0,0);
        setLayout(layout);
    }

    bool GnuplotTool::setMainWindow(MainWindow * main)
    {
        Tool::setMainWindow(main);

        if (mainWindow)
        {
            connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),
                    this,SLOT(setupFunctionPointers( QLibrary * )));

            setWindowTitle(name);
            QDockWidget * dockWidget = mainWindow->addToolWindow(this,MainWindow::DockWidget,Qt::BottomDockWidgetArea,Qt::BottomDockWidgetArea);
            if (dockWidget)
            {
                dockWidget->setFloating(true);
                dockWidget->hide();
                QToolBar * toolBar = mainWindow->toolBarForTools;
                QAction * action = new QAction(tr("gnuplot"),toolBar);
                action->setToolTip(tr("gnuplot"));
                action->setIcon(QIcon(tr(":/images/graph.png")));
				connect(action,SIGNAL(triggered()),dockWidget,SLOT(show()));
            }
        }

        return (mainWindow != 0);
    }

    typedef void (*tc_PlotTool_api)(
		void (*plot)(Matrix,int,const char*,int) ,
		void (*surface)(Matrix,double,double,double,double,const char*) ,
		Matrix (*plotData)(int)
		);

    void GnuplotTool::setupFunctionPointers( QLibrary * library )
    {
        tc_PlotTool_api f = (tc_PlotTool_api)library->resolve("tc_PlotTool_api");
		if (f)
		{
			f(
				&(gnuplotMatrix),
				&(gnuplotMatrix3D),
				&(getDataMatrix)
			);
		}
    }
}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::GnuplotTool * gnuplotTool = new Tinkercell::GnuplotTool;
	main->addTool(gnuplotTool);
}

