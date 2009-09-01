#include "MultiCell.h"

int main(int argc, char *argv[])
{
    PROJECTWEBSITE = QObject::tr("www.tinkercell.com");
    ORGANIZATIONNAME = QObject::tr("MultiCell Simulator");
    PROJECTNAME = QObject::tr("MultiCell Simulator");
	
    QApplication app(argc, argv);

	QString appDir = QCoreApplication::applicationDirPath();
    
    MainWindow mainWindow(true,false);//,false,false);
	
	mainWindow.setWindowTitle(QString("MultiCell Simulator"));
    mainWindow.statusBar()->showMessage(QString("Welcome to MultiCell Simulator"));

	Tool * tool = new MulticellInterface;
	mainWindow.addTool(tool);
	
	GraphicsScene::USE_DEFAULT_BEHAVIOR = false;
	GraphicsScene::GRID = 50; //grid mode	
	GraphicsScene::SelectionRectangleBrush = QBrush(QColor(5,30,5,40));

	mainWindow.newGraphicsWindow();
	
    mainWindow.show();

    int output = app.exec();

    return output;
}
