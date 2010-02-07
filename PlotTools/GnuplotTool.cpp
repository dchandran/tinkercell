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
	
	void GnuplotTool_FToS::gnuplotMatrix(Matrix m, int x, const char* title, int all)
	{
		QSemaphore * s = new QSemaphore(1);
		DataTable<qreal> * dat = ConvertValue(m);
		s->acquire();
		emit gnuplotDataTable(s, *dat,x,QString(title),all);
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}

	void GnuplotTool_FToS::gnuplotMatrix3D(Matrix m, const char * title)
	{
		QSemaphore * s = new QSemaphore(1);
		DataTable<qreal> * dat = ConvertValue(m);
		s->acquire();
		emit gnuplotDataTable3D(s, *dat,QString(title));
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}

	void GnuplotTool_FToS::gnuplotHistC(Matrix m, double bins, const char * title)
	{
		QSemaphore * s = new QSemaphore(1);
		DataTable<qreal> * dat = ConvertValue(m);
		s->acquire();
		emit gnuplotHist(s, *dat,bins,QString(title));
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}

	void GnuplotTool_FToS::gnuplotErrorbarsC(Matrix m, int x, const char* title)
	{
		QSemaphore * s = new QSemaphore(1);
		DataTable<qreal> * dat = ConvertValue(m);
		s->acquire();
		emit gnuplotErrorbars(s,*dat,x,QString(title));
		s->acquire();
		s->release();
		delete s;
		delete dat;
	}

	void GnuplotTool_FToS::gnuplotMultiplot(int x, int y)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		emit gnuplotMultiplot(s, x, y);
		s->acquire();
		s->release();
		delete s;
	}

	Matrix GnuplotTool_FToS::getDataMatrix(int index)
	{
		QSemaphore * s = new QSemaphore(1);
		s->acquire();
		DataTable<qreal> dat;
		emit getDataTable(s, dat, index);
		s->acquire();
		s->release();
		delete s;
		return ConvertValue(dat);
	}
	
	void GnuplotTool::gnuplotMatrix(Matrix m, int x, const char* title, int all)
	{
		fToS.gnuplotMatrix(m,x,title,all);
	}

	void GnuplotTool::gnuplotMatrix3D(Matrix m, const char * title)
	{
		fToS.gnuplotMatrix3D(m,title);
	}

	void GnuplotTool::gnuplotHistC(Matrix m, double bins, const char * title)
	{
		fToS.gnuplotHistC(m,bins,title);
	}

	void GnuplotTool::gnuplotErrorbarsC(Matrix m, int x, const char* title)
	{
		fToS.gnuplotErrorbarsC(m,x,title);
	}
	
	void GnuplotTool::gnuplotMultiplotC(int x, int y)
	{
		fToS.gnuplotMultiplot(x,y);
	}

	Matrix GnuplotTool::getDataMatrix(int index)
	{
		return fToS.getDataMatrix(index);
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
	
		QProcess process;

        if (process.state() == QProcess::Running)
            process.terminate();

        process.startDetached(cmd);
    }
	
    void GnuplotTool::runScript(const QString& script)
	{
		int numPlots = (multiplotRows * multiplotCols);

		previousCommands << script;
		
		if (previousCommands.size() < numPlots)
			return;
		
		QDir dir(MainWindow::userHome());
        if (!dir.cd(tr("gnuplot")))
        {
            dir.mkdir(tr("gnuplot"));
            dir.cd(tr("gnuplot"));
        }

        QString s, allscripts;

		if (previousCommands.size() <= 1)
		{
			allscripts = script;
		}
		else
		{
			allscripts = tr("set multiplot layout ");
			allscripts += QString::number(multiplotRows);
			allscripts += tr(",");
			allscripts += QString::number(multiplotCols);
			allscripts += tr("\n");
			allscripts += previousCommands.join(tr("\n"));
			allscripts += tr("\nunset multiplot\n");
		}

		if (editor)
            editor->setPlainText(allscripts);

		if (allscripts.contains(dir.absolutePath()))
			s = allscripts;
		else
			s = tr("cd '") + dir.absolutePath() + tr("'\n") + allscripts + tr("\n");

        QString filename(dir.absoluteFilePath(tr("script.txt")));
        QFile file(filename);
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(s.toAscii());
            file.close();
            runScriptFile(filename);
        }

		if (previousCommands.size() >= numPlots)
		{
			numPlots = multiplotRows = multiplotCols = 1;
			previousCommands.clear();
		}
    }
	
    void GnuplotTool::gnuplotDataTable(QSemaphore * sem, DataTable<qreal>& m, int x, const QString& title, int all)
    {
		m.description() = title;
        data << m;
		
		labels = m.getColNames();

        QDir dir(MainWindow::userHome());
        if (!dir.cd(tr("gnuplot")))
        {
            dir.mkdir(tr("gnuplot"));
            dir.cd(tr("gnuplot"));
        }

        QString file(dir.absoluteFilePath(tr("data") + QString::number(previousCommands.size()) + tr(".txt")));

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
					s += tr("set xlabel ");
					s += tr("\"");
					s += m.colName(x);
					s += tr("\"\n");
                    s += tr("set title '");
                    s += title;
                    s += tr("'\nplot 'data");
					s += QString::number(previousCommands.size());
					s += tr(".txt' using ");
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
        
		runScript(s);
		
		if (sem)
			sem->release();
	}

    
	void GnuplotTool::gnuplotDataTable3D(QSemaphore * sem, DataTable<qreal>& m, const QString& title)
	{
        if (m.cols() < 3) return;

		m.description() = title;
        data << m;
		
		labels = m.getColNames();

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

        QFile file(dir.absoluteFilePath(tr("data") + QString::number(previousCommands.size()) + tr(".txt")));
        if (file.open(QFile::WriteOnly))
        {
            file.write(s.toAscii());
            file.close();
        }

		s = tr("set title '");
		
		s += title;
		s += tr("\nset xlabel ");
		s += tr("'");
		s += m.colName(0);
		
		s += tr("'\nset ylabel ");
		s += tr("'");
		s += m.colName(1);
		
		s += tr("'\nset zlabel ");
		s += tr("'");
		s += m.colName(2);
		
		s += tr("'\nset pm3d; set nokey; set contour\nsplot 'data");
		s += QString::number(previousCommands.size());
		s += tr(".txt' with lines\n");

        runScript(s);
		
		if (sem)
			sem->release();
    }

    void GnuplotTool::gnuplotHist(QSemaphore * sem, DataTable<qreal>& m, double bins, const QString& title)
    {
		m.removeCol(tr("time"));
		m.removeCol(tr("Time"));
		
		m.description() = title;
        data << m;
		
		labels = m.getColNames();
		
        QDir dir(MainWindow::userHome());
        if (!dir.cd(tr("gnuplot")))
        {
            dir.mkdir(tr("gnuplot"));
            dir.cd(tr("gnuplot"));
        }

        QString file(dir.absoluteFilePath(tr("data") + QString::number(previousCommands.size()) + tr(".txt")));

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
				s += tr("bw = ");
				s += QString::number(bins);
				s += tr(";\nbin(x,width)=width*floor(x/width)\nset title '");
				s += title;
				s += tr("'\nplot 'data");
				s += QString::number(previousCommands.size());
				s += tr(".txt' using ");
			}
			else
				s += tr("'' u ");
			
			s += tr("(bin($");
			s += QString::number(i+1);
			s += tr(",bw)):(1.0) smooth freq with boxes title ");
			s += tr("'");
			s += m.colName(i);
			s += tr("'");
			if (i < cols-1)
				s += tr(", ");
		}
		
        runScript(s); 
		
		if (sem)
			sem->release();
    }

    void GnuplotTool::gnuplotErrorbars(QSemaphore * sem, DataTable<qreal>& m, int x, const QString& title)
    {
		m.description() = title;
        data << m;
		
		if (sem)
			sem->release();
    }

	void GnuplotTool::gnuplotMultiplot(QSemaphore * sem, int x, int y)
    {
		multiplotRows = x;
		multiplotCols = y;
		previousCommands.clear();
		
		if (sem)
			sem->release();
    }

    void GnuplotTool::getDataTable(QSemaphore * sem, DataTable<qreal> & dat, int index)
    {
        if (data.isEmpty())
            data << DataTable<qreal>();

        if (index < 0) index = 0;
        if (index >= data.size()) index = data.size() - 1;
        dat = data[index];
		
		if (sem)
			sem->release();
    }

    GnuplotTool::GnuplotTool(QWidget * parent) : Tool(tr("Gnuplot"),tr("Plot"),parent), editor(0)
    {
		multiplotRows = multiplotCols = 1;
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
        editor = new CodeEditor(this);
		layout->addWidget(editor);
		layout->setStretch(0,0);
		layout->setStretch(1,1);

        layout->setContentsMargins(0,0,0,0);
        setLayout(layout);
		
		connect(&fToS,SIGNAL(gnuplotDataTable(QSemaphore*,DataTable<qreal>&, int, const QString& , int)),
				this, SLOT(gnuplotDataTable(QSemaphore*,DataTable<qreal>&, int, const QString& , int)));
		
		connect(&fToS,SIGNAL(gnuplotDataTable3D(QSemaphore*,DataTable<qreal>&, const QString&)),
				this, SLOT(gnuplotDataTable3D(QSemaphore*,DataTable<qreal>&, const QString&)));
		
		connect(&fToS,SIGNAL(gnuplotHist(QSemaphore*,DataTable<qreal>&, double, const QString&)),
				this,SLOT(gnuplotHist(QSemaphore*,DataTable<qreal>&, double, const QString&)));
		
		connect(&fToS,SIGNAL(gnuplotErrorbars(QSemaphore*,DataTable<qreal>&, int, const QString&)),
				this, SLOT(gnuplotErrorbars(QSemaphore*,DataTable<qreal>&, int, const QString&)));
		
		connect(&fToS,SIGNAL(gnuplotMultiplot(QSemaphore*,int, int)), this, SLOT(gnuplotMultiplot(QSemaphore*,int, int)));
		
		connect(&fToS,SIGNAL(getDataTable(QSemaphore*,DataTable<qreal>&, int)), this, SLOT(getDataTable(QSemaphore*,DataTable<qreal>&, int)));
    }

    /*void GnuplotTool::toolAboutToBeLoaded( Tool * tool, bool * b)
    {
        if (tool && tool != this && tool->category.toLower() == tr("plot"))
            (*b) = false;
    }*/

    bool GnuplotTool::setMainWindow(MainWindow * main)
    {
        Tool::setMainWindow(main);

        if (mainWindow)
        {
            connect(mainWindow,SIGNAL(setupFunctionPointers( QLibrary * )),
                    this,SLOT(setupFunctionPointers( QLibrary * )));

            //connect(mainWindow, SIGNAL(toolAboutToBeLoaded( Tool * , bool * )),
                //    this, SLOT(toolAboutToBeLoaded( Tool * , bool * )));

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
		void (*hist)(Matrix,double,const char*) ,
		void (*errorbars)(Matrix,int,const char*) ,
		void (*multiplot)(int,int),
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
				&(gnuplotMultiplotC),
				&(getDataMatrix)
			);
		}
    }

    void GnuplotTool::runScript()
    {
        if (editor)
            runScript(editor->toPlainText());
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
			runScript(editor->toPlainText() + s);
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


