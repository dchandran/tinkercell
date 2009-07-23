#include "OutputWindow.h"
#include "TinkerCellAboutBox.h"
#include <QPixmap>
#include <QBrush>
#include <QTextEdit>
#include <QDesktopServices>

namespace Tinkercell
{
	TinkercellAboutBox::TinkercellAboutBox() : Tool(tr("TinkerCell About Box"))
	{
		QHBoxLayout * layout1 = new QHBoxLayout;
		
		QString text("TinkerCell\nVersion: ");
		text += QCoreApplication::applicationVersion() + tr("\n\nAuthor: Deepak Chandran\n\nWebsite: www.tinkercell.com\n\n");
		
		QString appDir = QCoreApplication::applicationDirPath();
	
		QString filename("about.txt");
		QString name[] = {	MainWindow::userHome() + tr("/") + filename,
							filename,
							QDir::currentPath() + tr("/") + filename,
							appDir + tr("/") + filename };
		
		QFile file;
		bool opened = false;
		for (int i=0; i < 4; ++i)
		{
			file.setFileName(name[i]);
			if (file.open(QFile::ReadOnly | QFile::Text))
			{
				opened = true;
				break;
			}
		}
		if (!opened)
		{
			OutputWindow::error(filename + tr(" not found"));
		}
		else
		{
			QString allText(file.readAll());
			text += allText + tr("\n");
			file.close();
		}
		QTextEdit * edit = new QTextEdit(this);
		edit->setPlainText(text);
		edit->setReadOnly(true);
		
		QPalette palette;
		palette.setBrush(edit->backgroundRole(), QBrush(QPixmap(":/images/Tinkercell.png").scaled(256,256)));
		edit->setPalette(palette);
		layout1->addWidget(edit);
		
		
		dialog.setLayout(layout1);
		
	}
	
	bool TinkercellAboutBox::setMainWindow(MainWindow * main)
	{
		Tool::setMainWindow(main);
		if (mainWindow)
		{
			if (mainWindow->helpMenu)
			{
				mainWindow->helpMenu->addAction(QIcon(":/images/about.png"),tr("About TinkerCell"),this,SLOT(showAboutBox()));
				mainWindow->helpMenu->addAction(QIcon(":/images/question.png"),tr("User's Guide"),this,SLOT(openDocumentation()));
				mainWindow->helpMenu->addSeparator();
				mainWindow->helpMenu->addAction(QIcon(":/images/globe.png"),tr("TinkerCell Homepage"),this,SLOT(openHomePage()));
				mainWindow->helpMenu->addAction(QIcon(":/images/globe.png"),tr("TinkerCell Blog"),this,SLOT(openBlog()));
				mainWindow->helpMenu->addSeparator();
				mainWindow->helpMenu->addAction(tr("Write to us"),this,SLOT(emailAuthor()));
			}
			return true;
		}
		return false;
	}
	
	void TinkercellAboutBox::showAboutBox()
	{
		dialog.show();
	}
	
	void TinkercellAboutBox::openDocumentation()
	{
		
		QString appDir = QCoreApplication::applicationDirPath();
		
		QString doc = tr("file:") + appDir + tr("/Documentation/UserDocumentation.html");
		QDesktopServices::openUrl(QUrl(doc));
	}
	
	void TinkercellAboutBox::openBlog()
	{
		QDesktopServices::openUrl(QUrl("http://tinker-cell.blogspot.com/"));
	}
	
	void TinkercellAboutBox::openHomePage()
	{
		QDesktopServices::openUrl(QUrl("http://www.tinkercell.com/"));
	}
	
	void TinkercellAboutBox::emailAuthor()
	{
		QDesktopServices::openUrl(QUrl("mailto:admin@tinkercell.com?subject=TinkerCell Report"));
	}
}

extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main)
{
	if (!main) return;

	Tinkercell::TinkercellAboutBox * about = new Tinkercell::TinkercellAboutBox;
	main->addTool(about);

}

