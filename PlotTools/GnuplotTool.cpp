/****************************************************************************

 Copyright (c) 2009 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 see COPYRIGHT.TXT

 See GnuplotTool.h
****************************************************************************/

#include <QFileDialog>
#include <QIODevice>
#include <QFile>
#include <QHash>
#include <QTextStream>
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
	GnuplotTool_FToS GnuplotTool::fToS;
    QList< DataTable<qreal> > GnuplotTool::data;

    QProcess process;
	
	void GnuplotTool_FToS::gnuplotFile(const QString& s)
	{
		emit runScriptFile(s);
	}
	
	void GnuplotTool_FToS::gnuplotScript(const QString& s)
	{
		emit runScript(s);
	}
	
	void GnuplotTool::gnuplotFile(const QString& filename)
	{
		fToS.gnuplotFile(filename);
	}
	
	void GnuplotTool::runScriptFile(const QString& filename)
    {
    #ifdef Q_WS_WIN

        QString cmd = tr("\"") +
                        QCoreApplication::applicationDirPath().replace(tr("/"),tr("\\")) +
                        tr("\\win32\\gnuplot\\wgnuplot.exe\" \"") +
                        QString(filename).replace(tr("/"),tr("\\")) +
                        tr("\" - ");
    #else

        QString cmd = tr("gnuplot ") + filename + tr(" -");

    #endif

        if (process.state() == QProcess::Running)
            process.terminate();

        process.startDetached(cmd);
    }
	
	void GnuplotTool::gnuplotScript(const QString& filename)
	{
		fToS.gnuplotScript(filename);
	}

    void GnuplotTool::runScript(const QString& script)
    {
		QDir dir(MainWindow::userHome());
        if (!dir.cd(tr("gnuplot")))
        {
            dir.mkdir(tr("gnuplot"));
            dir.cd(tr("gnuplot"));
        }

        QString s;

        if (script.left(3) == tr("cd "))
            s = script;
        else
            s = (tr("cd '") + dir.absolutePath() + tr("'\n") + script + tr("\n"));

        QString filename(dir.absoluteFilePath(tr("script.txt")));
        QFile file(filename);
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(s.toAscii());
            file.close();
            gnuplotFile(filename);
        }

        if (editor)
            editor->setPlainText(s);
    }

    void GnuplotTool::gnuplotMatrix(Matrix m, int x, const char* title, int all)
    {
        DataTable<qreal> * data = ConvertValue(m);
        gnuplotDataTable(*data,x,tr(title),all);
        delete data;
    }

    void GnuplotTool::gnuplotDataTable(const DataTable<qreal>& m, int x, const QString& title, int all)
    {
		m.description() = title;
        GnuplotTool::data << m;

        QDir dir(MainWindow::userHome());
        if (!dir.cd(tr("gnuplot")))
        {
            dir.mkdir(tr("gnuplot"));
            dir.cd(tr("gnuplot"));
        }

        QString file(dir.absoluteFilePath(tr("data.txt")));

        QFile data(file);

        if (data.open(QFile::WriteOnly))
        {
            QTextStream out(&data);
            out << tr("#") << title << tr("\n");
            for (int i=0; i < m.cols(); ++i)
                if (i > 0)
                    out << tr("\t") << m.colName(i);
                else
                    out << m.colName(i);
            out << tr("\n");

            for (int i=0; i < m.rows(); ++i)
            {
                for (int j=0; j < m.cols(); ++j)
                    if (j > 0)
                        out << tr("\t") << QString::number(m.at(i,j));
                    else
                        out << QString::number(m.at(i,j));
                out << tr("\n");
            }

            data.close();
        }

        QString s;
        int cols = m.cols();

        for (int i=0; i < cols; ++i)
            if (i != x)
            {
                if (s.isEmpty())
                {
                    s += tr("set title '");
                    s += title;
                    s += tr("'\nplot 'data.txt' using ");
                }
                else
                    s += tr("'' u ");
                s += QString::number(x+1);
                s += tr(":");
                s += QString::number(i+1);
                s += tr(" ti col with lines lw 3");
                if (i < cols-2 || (i < (cols-1) && (cols-1)!=x))
                    s += tr(", ");
            }
        gnuplotScript(s);
    }

    void GnuplotTool::gnuplotMatrix3D(Matrix m, const char * title)
    {
        DataTable<qreal> * dat = ConvertValue(m);
        gnuplotDataTable3D(*dat,tr(title));
        delete dat;
    }

    void GnuplotTool::gnuplotDataTable3D(const DataTable<qreal>& m, const QString& title)
    {
        if (m.cols() < 3) return;

		m.description() = title;
        GnuplotTool::data << m;

        QString z;
        QString s = tr("#");
        s += m.getColNames().join(tr("\t")) += tr("\n");
        int rows = m.rows();

        int k = 0;

        for (int i=0; i < rows; ++i)
        {
            s += QString::number(m.at(i,0));
            s  += tr("\t");
            s  += QString::number(m.at(i,1));
            s  += tr("\t");
            s  += QString::number(m.at(i,2));
            s  += tr("\n");
            if (i==0)
                if (m.at(i,0) == m.at(i+1,0))
                    k = 0;
                else
                    k = 1;
            if (i < (rows-1) && m.at(i,k) != m.at(i+1,k))
                s += tr("\n"); //next block
        }

        QDir dir(MainWindow::userHome());
        if (!dir.cd(tr("gnuplot")))
        {
            dir.mkdir(tr("gnuplot"));
            dir.cd(tr("gnuplot"));
        }

        QFile file(dir.absoluteFilePath(tr("data.txt")));
        if (file.open(QFile::WriteOnly))
        {
            file.write(s.toAscii());
            file.close();
        }

		s = tr("set title '");
		s += title;
		s += tr("'\nset pm3d; set nokey; set contour\nsplot 'data.txt' with lines\n");

        gnuplotScript(s);

    }

    void GnuplotTool::gnuplotHistC(Matrix m, int bins, const char * title)
    {
		DataTable<qreal> * dat = ConvertValue(m);
        gnuplotHist(*dat,bins,tr(title));
        delete dat;
    }

    void GnuplotTool::gnuplotHist(const DataTable<qreal>& m, int bins, const QString& title)
    {
		m.description() = title;
        GnuplotTool::data << m;
		
		m.description() = title;
        GnuplotTool::data << m;

        QDir dir(MainWindow::userHome());
        if (!dir.cd(tr("gnuplot")))
        {
            dir.mkdir(tr("gnuplot"));
            dir.cd(tr("gnuplot"));
        }

        QString file(dir.absoluteFilePath(tr("data.txt")));

        QFile data(file);

        if (data.open(QFile::WriteOnly))
        {
            QTextStream out(&data);
            out << tr("#") << title << tr("\n");
            for (int i=0; i < m.cols(); ++i)
                if (i > 0)
                    out << tr("\t") << m.colName(i);
                else
                    out << m.colName(i);
            out << tr("\n");

            for (int i=0; i < m.rows(); ++i)
            {
                for (int j=0; j < m.cols(); ++j)
                    if (j > 0)
                        out << tr("\t") << QString::number(m.at(i,j));
                    else
                        out << QString::number(m.at(i,j));
                out << tr("\n");
            }

            data.close();
        }

        QString s;
        int cols = m.cols();

        for (int i=0; i < cols; ++i)
		{
			if (s.isEmpty())
			{
				s += tr("bw = 1\nbin(x,width)=width*floor(x/width)\nset title '");
				s += title;
				s += tr("'\nplot 'data.txt' using ");
			}
			else
				s += tr("'' u ");
			
			s += tr("(bin($");
			s += QString::number(i+1);
			s += tr(",bw)):(1.0) smooth freq with boxes");
			if (i < cols-1)
				s += tr(", ");
		}
        gnuplotScript(s); 
    }

    void GnuplotTool::gnuplotErrorbarsC(Matrix m, int x, const char* title)
    {
    }

    void GnuplotTool::gnuplotErrorbars(const DataTable<qreal>& m, int x, const QString& title)
    {
		m.description() = title;
        GnuplotTool::data << m;
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
        return ConvertValue(getDataTable(index));
    }

    GnuplotTool::GnuplotTool(QWidget * parent) : Tool(tr("gnuplot"),parent)
    {
        QVBoxLayout * layout = new QVBoxLayout;
        QHBoxLayout * buttonsLayout = new QHBoxLayout;

        QToolButton * button1 = new QToolButton;
        button1->setIcon(QIcon(tr(":/images/save.png")));
        connect(button1,SIGNAL(pressed()),this,SLOT(savePlot()));
        button1->setToolTip(tr("Save image"));
		button1->setIconSize(QSize(25,25));

        QToolButton * button2 = new QToolButton;
        button2->setIcon(QIcon(tr(":/images/export.png")));
        connect(button2,SIGNAL(pressed()),this,SLOT(writeData()));
        button2->setToolTip(tr("Save as tab-delimited text"));
		button2->setIconSize(QSize(25,25));

        QToolButton * button3 = new QToolButton;
        button3->setIcon(QIcon(tr(":/images/copy.png")));
        connect(button3,SIGNAL(pressed()),this,SLOT(copyData()));
        button3->setToolTip(tr("Copy data to clipboard"));
		button3->setIconSize(QSize(25,25));

        QToolButton * button4 = new QToolButton;
        button4->setIcon(QIcon(tr(":/images/play.png")));
        connect(button4,SIGNAL(pressed()),this,SLOT(runScript()));
        button4->setToolTip(tr("Run script"));
		button4->setIconSize(QSize(25,25));

        buttonsLayout->addWidget(button1);
        buttonsLayout->addWidget(button2);
        buttonsLayout->addWidget(button3);
        buttonsLayout->addWidget(button4);
		buttonsLayout->setSpacing(16);
		buttonsLayout->addStretch(1);

        layout->addLayout(buttonsLayout);
        if (!editor)
        {
            editor = new CodeEditor(this);
            layout->addWidget(editor);
            layout->setStretch(0,0);
            layout->setStretch(1,1);
        }

        layout->setContentsMargins(0,0,0,0);
        setLayout(layout);
		
		connect(&fToS,SIGNAL(runScriptFile(const QString&)),this,SLOT(runScriptFile(const QString&)));
		connect(&fToS,SIGNAL(runScript(const QString&)),this,SLOT(runScript(const QString&)));
    }	

    void GnuplotTool::toolAboutToBeLoaded( Tool * tool, bool * b)
    {
        if (tool && tool != this &&  tool->name.toLower().contains(tr("plot")))
            (*b) = false;
    }

    bool GnuplotTool::setMainWindow(MainWindow * main)
    {
        Tool::setMainWindow(main);

        if (mainWindow)
        {
            connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),
                    this,SLOT(setupFunctionPointers( QLibrary * )));

            connect(mainWindow, SIGNAL(toolAboutToBeLoaded( Tool * , bool * )),
                    this, SLOT(toolAboutToBeLoaded( Tool * , bool * )));

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
		void (*surface)(Matrix,const char*) ,
		void (*hist)(Matrix,int,const char*) ,
		void (*errorbars)(Matrix,int,const char*) ,
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
				&(gnuplotHistC),
				&(gnuplotErrorbarsC),
				&(getDataMatrix)
			);
		}
    }

    void GnuplotTool::runScript()
    {
        if (editor)
            gnuplotScript(editor->toPlainText());
    }

    void GnuplotTool::savePlot()
    {
		if (editor && !editor->toPlainText().isEmpty())
		{
			QString file =
				QFileDialog::getSaveFileName(this, tr("Save to File"),
                                          MainWindow::previousFileName,
                                          tr("GIF Files (*.gif)"));

			if (file.isNull() || file.isEmpty()) return;

			QString s("\nset terminal gif; set output \"");
			s += file;
			s += tr("\"\n replot\n");
			gnuplotScript(editor->toPlainText() + s);
		}
    }

    void GnuplotTool::copyData()
    {
		if (data.size() < 1) return;

		QClipboard * clipboard = QApplication::clipboard();

		if (!clipboard)
		{
			if (console())
				console()->error(tr("No clipboard available."));
			return;
		}

		DataTable<qreal>& m = data.last();

		QString s;

		for (int i=0; i < m.cols(); ++i)
			if (i > 0)
				s += tr("\t") + m.colName(i);
			else
				s += m.colName(i);
		s += tr("\n");

		for (int i=0; i < m.rows(); ++i)
		{
			for (int j=0; j < m.cols(); ++j)
				if (j > 0)
					s += tr("\t") += QString::number(m.at(i,j));
				else
					s += QString::number(m.at(i,j));
			s += tr("\n");
		}

		clipboard->setText(s);

		console()->message(tr("Data copied to clipboard."));
    }

    void GnuplotTool::writeData()
    {
		if (data.size() < 1) return;

		DataTable<qreal>& m = data.last();

		QString file =
				QFileDialog::getSaveFileName(this, tr("Save to File"),
                                          MainWindow::previousFileName,
                                          tr("TXT Files (*.txt *.tab)"));

		if (file.isEmpty() || file.isNull()) return;


		QFile data(file);

        if (data.open(QFile::WriteOnly))
        {
            QTextStream out(&data);
            out << tr("#") << m.description() << tr("\n");
            for (int i=0; i < m.cols(); ++i)
                if (i > 0)
                    out << tr("\t") << m.colName(i);
                else
                    out << m.colName(i);
            out << tr("\n");

            for (int i=0; i < m.rows(); ++i)
            {
                for (int j=0; j < m.cols(); ++j)
                    if (j > 0)
                        out << tr("\t") << QString::number(m.at(i,j));
                    else
                        out << QString::number(m.at(i,j));
                out << tr("\n");
            }

			data.close();

			if (console())
				console()->message(tr("Written to ") + file);
        }
		else
			if (console())
				console()->error(tr("Cannot write to file."));

    }
}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::GnuplotTool * gnuplotTool = new Tinkercell::GnuplotTool;
	main->addTool(gnuplotTool);
}

