#include "MainWindow.h"
#include "NetworkHandle.h"
#include "GlobalSettings.h"
#include "Ontology.h"
#include "ConsoleWindow.h"
#include "NodeGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ItemHandle.h"
#include "SamplePlugin1.h"
using namespace Tinkercell;

SamplePlugin1::SamplePlugin1(): Tool("My Plugin 1", "Sample Plugins") //name, category
{
}

bool SamplePlugin1::setMainWindow(MainWindow * main)
{
	Tool::setMainWindow(main); //must call this

	connect(main, SIGNAL(mouseReleased(GraphicsScene*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
				this, SLOT(mouseReleased(GraphicsScene*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));

	connect(main, SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				this,SLOT(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
}

void SamplePlugin1::mouseReleased(GraphicsScene * scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
	NodeHandle * handle = new NodeHandle("x");
	scene->network->makeUnique(handle);  //assign unique name

	NodeGraphicsItem * node;
	if (modifiers == Qt::ControlModifier)
		node = new NodeGraphicsItem(":/images/fire.xml");
	else		
		node = new NodeGraphicsItem(":/images/defaultnode.xml");

	node->setHandle(handle);
	TextGraphicsItem * text = new TextGraphicsItem(handle);

	node->setPos(point);

	QPointF bottom(0, node->boundingRect().height()/2);
	text->setPos( point + bottom );
	QFont font = text->font();
	font.setPointSize(22);
	text->setFont(font);
	
	QList<QGraphicsItem*> list;
	list << node << text;

	scene->insert("new node", list);
}

void SamplePlugin1::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers)
{
	QList<ItemHandle*> handles = getHandle(items);

	for (int i=0; i < handles.size(); ++i)
		scene->moving() += handles[i]->graphicsItems;
}


/***********************
    The rest of the code
    is the same as simplemain.cpp
    except for the addTool line
************************/
#if defined(Q_WS_WIN) && !defined(MINGW)
int WinMain(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	//setup project name -- REQUIRED
    GlobalSettings::PROJECTWEBSITE = "www.tinkercell.com";
    GlobalSettings::ORGANIZATIONNAME = "My Wonderful Co.";
    GlobalSettings::PROJECTNAME = "My Wonderful App";
	
	//start Qt -- REQUIRED
	QApplication app(argc, argv);
	QString appDir = QCoreApplication::applicationDirPath();    
	
	//enable of disable features -- OPTIONAL (see default values in GlobalSettings.h)
	GlobalSettings::ENABLE_HISTORY_WINDOW = true;
	GlobalSettings::ENABLE_CONSOLE_WINDOW = true;
	GlobalSettings::ENABLE_GRAPHING_TOOLS = true;
	GlobalSettings::ENABLE_CODING_TOOLS = false;
	GlobalSettings::ENABLE_PYTHON = true;
	GlobalSettings::ENABLE_OCTAVE = false;
	GlobalSettings::ENABLE_LOADSAVE_TOOL = true;
	
	//create main window -- REQUIRED
	MainWindow mainWindow(true,false,false);  //@args: enable scene, text, allow pop-out windows
	mainWindow.readSettings();   //load settings such as window positions

	//set window title -- OPTIONAL
	mainWindow.setWindowTitle("My Wonderful App"); 
	
	//ADD PLUGINS
	mainWindow.addTool(new SamplePlugin1);
	
	//load Ontology -- OPTIONAL
	Ontology::readNodes("Nodes.nt");
	Ontology::readConnections("Connections.nt");
	
	//create an empty canvas -- RECOMMENDED
    GraphicsScene * scene = mainWindow.newScene();
    mainWindow.show();
    int output = app.exec();

    return output;
}


