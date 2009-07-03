/****************************************************************************
 **
 ** Copyright (c) 2008 Deepak Chandran
 ** Contact: Deepak Chandran (dchandran1@gmail.com)
 **
 ** See COPYWRITE.TXT
 **
 ****************************************************************************/

//#include <vld.h>
#include <QApplication>
#include <stdio.h>
#include <stdlib.h>

#include "NodeGraphicsWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    NodeImageDesigner::MainWindow mainWin;
	mainWin.setGeometry(100,100,700,400);
	
    mainWin.show();
    app.exec();

	return 0;
}
