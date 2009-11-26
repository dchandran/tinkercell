/****************************************************************************

 Copyright (c) 2009 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 See GnuplotTool.h
****************************************************************************/

#include <QIODevice>
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QToolButton>
#include <QProcess>
#include <QDir>
#include "ConsoleWindow.h"
#include "GnuplotTool.h"

namespace Tinkercell
{
    QList< DataTable<qreal> > GnuplotTool::data;

    void GnuplotTool::gnuplot(const QString& script)
    {
        QDir dir(MainWindow::userHome());
        if (!dir.cd(tr("gnuplot")))
        {
            dir.mkdir(tr("gnuplot"));
            dir.cd(tr("gnuplot"));
        }

        QString filename = dir.absoluteFilePath(tr("script.txt"));
        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        file.write(script.toAscii());
        file.close();
    #ifdef Q_WS_WIN

        filename = tr("\"") + filename.replace(tr("/"),tr("\\")) + tr("\\");
        QProcess::execute( tr("\"") +
                           QCoreApplication::applicationDirPath().replace(tr("/"),tr("\\")) +
                           tr("\"") +
                           tr("\\win32\\gnuplot\\pgnuplot.exe < ") + filename);
    #else

        QProcess::execute("gnuplot.exe < ") + filename);
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
        QVBoxLayout * layout = new QVBoxLayout;
        QHBoxLayout * buttonsLayout = new QHBoxLayout;

        QToolButton * button1 = new QToolButton;
        button1->setIcon(QIcon(tr(":/images/save.png")));
        connect(button1,SIGNAL(pressed()),this,SLOT(savePlot()));
        button1->setToolTip(tr("Save image"));

        QToolButton * button2 = new QToolButton;
        button2->setIcon(QIcon(tr(":/images/export.png")));
        connect(button2,SIGNAL(pressed()),this,SLOT(writeData()));
        button2->setToolTip(tr("Save as tab-delimited text"));

        QToolButton * button3 = new QToolButton;
        button3->setIcon(QIcon(tr(":/images/copy.png")));
        connect(button3,SIGNAL(pressed()),this,SLOT(copyData()));
        button3->setToolTip(tr("Copy data to clipboard"));

        QToolButton * button4 = new QToolButton;
        button4->setIcon(QIcon(tr(":/images/play.png")));
        connect(button4,SIGNAL(pressed()),this,SLOT(runScript()));
        button4->setToolTip(tr("Run script"));

        buttonsLayout->addWidget(button1);
        buttonsLayout->addWidget(button2);
        buttonsLayout->addWidget(button3);
        buttonsLayout->addWidget(button4);

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
                if (toolBar)
                    toolBar->addAction(QIcon(tr(":/images/graph.png")),tr("gnuplot"),dockWidget,SLOT(show()));
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

    void GnuplotTool::runScript()
    {
        if (editor)
            gnuplot(editor->toPlainText() + tr("\n"));
        editor->setPlainText("hello");
    }

    void GnuplotTool::savePlot()
    {
    }

    void GnuplotTool::copyData()
    {
    }
}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::GnuplotTool * gnuplotTool = new Tinkercell::GnuplotTool;
	main->addTool(gnuplotTool);
}

