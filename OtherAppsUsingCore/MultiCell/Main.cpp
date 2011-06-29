#include <QApplication>
#include "MultiCell.h"
#include "GraphicsScene.h"
#include "CellTypeSelector.h"
#include "GlobalSettings.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	//enable features -- must do before creating MainWindow
	GlobalSettings::ENABLE_HISTORY_WINDOW = false;
	GlobalSettings::ENABLE_CONSOLE_WINDOW = true;
	GlobalSettings::ENABLE_GRAPHING_TOOLS = true;
	GlobalSettings::ENABLE_CODING_TOOLS = false;
	GlobalSettings::ENABLE_ALIGNMENT_TOOL = false;
	GlobalSettings::ENABLE_PYTHON = false;
	GlobalSettings::ENABLE_OCTAVE = true;
	GlobalSettings::ENABLE_LOADSAVE_TOOL = true;
	
    Tinkercell::GlobalSettings::PROJECTWEBSITE = QObject::tr("www.tinkercell.com");
    Tinkercell::GlobalSettings::ORGANIZATIONNAME = QObject::tr("MultiCell Simulator");
    Tinkercell::GlobalSettings::PROJECTNAME = QObject::tr("MultiCell Simulator");

	QString appDir = QCoreApplication::applicationDirPath();
	
	Tinkercell::GraphicsScene::USE_DEFAULT_BEHAVIOR = false;
	Tinkercell::GraphicsScene::GRID = 100; //grid mode	
	Tinkercell::GraphicsScene::SelectionRectangleBrush = QBrush(QColor(5,250,5,80));
    
    Tinkercell::MainWindow mainWindow(true,false);//,false,false);
	
	mainWindow.setWindowTitle(QString("MultiCell Simulator"));
    mainWindow.statusBar()->showMessage(QString("Welcome to MultiCell Simulator"));

	Tool * tool = new Multicell::MulticellInterface;
	mainWindow.addTool(tool);
	
	Tinkercell::GraphicsScene * scene = mainWindow.newScene();	
	scene->zoom(0.5);
	
    mainWindow.show();

    int output = app.exec();

    return output;
}

