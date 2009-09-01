#include "MultiCell.h"
#include "CellTypeSelector.h"

int main(int argc, char *argv[])
{
    Tinkercell::PROJECTWEBSITE = QObject::tr("www.tinkercell.com");
    Tinkercell::ORGANIZATIONNAME = QObject::tr("MultiCell Simulator");
    Tinkercell::PROJECTNAME = QObject::tr("MultiCell Simulator");
	
    QApplication app(argc, argv);

	QString appDir = QCoreApplication::applicationDirPath();
    
    Tinkercell::MainWindow mainWindow(true,false);//,false,false);
	
	mainWindow.setWindowTitle(QString("MultiCell Simulator"));
    mainWindow.statusBar()->showMessage(QString("Welcome to MultiCell Simulator"));

	Tool * tool = new Multicell::MulticellInterface;
	mainWindow.addTool(tool);
	
	Tinkercell::GraphicsScene::USE_DEFAULT_BEHAVIOR = false;
	Tinkercell::GraphicsScene::GRID = 100; //grid mode	
	Tinkercell::GraphicsScene::SelectionRectangleBrush = QBrush(QColor(5,30,5,40));

	mainWindow.newGraphicsWindow();
	mainWindow.newGraphicsWindow();
	
    mainWindow.show();

    int output = app.exec();

    return output;
}
