/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

****************************************************************************/

#ifndef TINKERCELL_TINKERCELLABOUTBOX_H
#define TINKERCELL_TINKERCELLABOUTBOX_H

#include <QtGui>
#include <QIcon>
#include <QPixmap>
#include <QString>
#include <QDialog>

#include "Tool.h"
#include "MainWindow.h"

#ifndef TINKERCELLEXPORT
#ifdef Q_WS_WIN
#   if defined(TC_EXPORTS) || defined(TinkerCell_EXPORTS)
#       define TINKERCELLEXPORT __declspec(dllexport)
#   else
#       define TINKERCELLEXPORT __declspec(dllimport)
#   endif
#else
#    define TINKERCELLEXPORT
#endif
#endif

namespace Tinkercell
{

	/*! \brief This class listens to selection events from the scene and responds by changing colors
	of the selected objects and displaying tools and widgets that point to that object.
	*/
	class TinkercellAboutBox : public Tool
	{
		Q_OBJECT

	public:
		TinkercellAboutBox();
		bool setMainWindow(MainWindow * main);	
	public slots:
		void showAboutBox();
		void openWhatIsTinkerCell();
		void openUserDocumentation();
		void openC_API();
		void openHomePage();
		void openTutorialsPage();
		void openFeedbackPage();
		void openBlog();
		void emailAuthor();
	private:
		QDialog dialog;
	};

}

#endif

