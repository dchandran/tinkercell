#include "MainWindow.h"
#include "NetworkHandle.h"
#include "GlobalSettings.h"
#include "Ontology.h"
#include "ConsoleWindow.h"
#include "NodeGraphicsItem.h"
#include "TextGraphicsItem.h"
#include "ItemHandle.h"
#include "MyFirstExtension.h"
using namespace Tinkercell;

MyFirstExtension::MyFirstExtension(): Tool("My Plugin 1", "Sample Plugins") //name, category
{
}

bool MyFirstExtension::setMainWindow(MainWindow * main)
{
	Tool::setMainWindow(main); //must call this to properly setup the extension

	//this extension performs some action when mouse button is released
	connect(main, SIGNAL(mouseReleased(GraphicsScene*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)),
				this, SLOT(mouseReleased(GraphicsScene*, QPointF, Qt::MouseButton, Qt::KeyboardModifiers)));

	//this extension performs some action when items on the screen are selected
	connect(main, SIGNAL(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)),
				this,SLOT(itemsSelected(GraphicsScene *, const QList<QGraphicsItem*>&, QPointF, Qt::KeyboardModifiers)));
}

void MyFirstExtension::mouseReleased(GraphicsScene * scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers)
{
	/*
		One network can be represented in multiple scenes (canvas) or
		multiple text editors, so a scene might be just a piece of the entire network
	*/
	NetworkHandle * network = scene->network;  //get the network

	NodeHandle * handle = new NodeHandle("x"); //create a new "Handle", or entity, called x
	scene->network->makeUnique(handle);  //assign unique name, if x is already taken

	NodeGraphicsItem * node;  //greate a graphical node, different from a Handle
	if (modifiers == Qt::ControlModifier) 
		node = new NodeGraphicsItem(":/images/fire.xml");  //load either the fire image
	else		
		node = new NodeGraphicsItem(":/images/defaultnode.xml"); //or the default image

	node->setHandle(handle);  //set the handle for the graphical item
	TextGraphicsItem * text = new TextGraphicsItem(handle);  //create a text box that also belong with the same handle

	node->setPos(point); //set the position of the node. point is where mouse was clicked

	QPointF bottom(0, node->boundingRect().height()/2);  //set the position of the text box
	text->setPos( point + bottom );  
	QFont font = text->font();  //increase the font of the text box
	font.setPointSize(22);
	text->setFont(font);
	
	QList<QGraphicsItem*> list;  //create a list of the node and the text box
	list << node << text;

	scene->insert("new node", list);   /*insert items into the scene. 
    DO NOT USE scene->addItem because that is part of the Qt 
    and would not do all the things that ->insert would do*/
}

void MyFirstExtension::itemsSelected(GraphicsScene * scene, const QList<QGraphicsItem*>& items, QPointF point, Qt::KeyboardModifiers modifiers)
{
	/*
		The following look ensures that text boxes also move
 		when nodes are selected, but nodes do not move when
		text boxes are selected

		A handle is a collection of graphical items that reprent
		the same entity. For example a node and the text box
		below the node belong with the same entity.
	*/
	for (int i=0; i < items.size(); ++i)
	{
		ItemHandle * h = getHandle(items[i]);
		if (h != 0 && NodeGraphicsItem::cast(items[i]))
			scene->moving() += h->graphicsItems;
	}
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
	mainWindow.addTool(new MyFirstExtension);
	
	//load Ontology -- OPTIONAL
	Ontology::readNodes("Nodes.nt");
	Ontology::readConnections("Connections.nt");
	
	//create an empty canvas -- RECOMMENDED
    GraphicsScene * scene = mainWindow.newScene();
    mainWindow.show();
    int output = app.exec();

    return output;
}


