/****************************************************************************

Copyright (c) 2008 Deepak Chandran
Contact: Deepak Chandran (dchandran1@gmail.com)
See COPYRIGHT.TXT

The tool class is the parent class for all plugins. 
A Tool is a Qt Widget with a name and pointer to the Tinkercell MainWindow. 


****************************************************************************/

#ifndef TINKERCELL_TOOLDEF_H
#define TINKERCELL_TOOLDEF_H

#include <QWidget>
#include <QList>
#include <QButtonGroup>
#include <QToolButton>
#include <QGraphicsItemGroup>

#include "MainWindow.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

	class MainWindow;
	class GraphicsScene;
	class NetworkWindow;
	class TextEditor;

	/*! \brief everything other than the main window is a tool
	\ingroup core
	*/
	class MY_EXPORT Tool : public QWidget
	{

		Q_OBJECT

	public:
		/*! \brief main window for this tool*/
		MainWindow * mainWindow;
		/*! \brief name of this tool*/
		QString name;
		/*! \brief constructor*/
		Tool();
		/*! \brief destructor. removes graphicsItem and toolButton is not 0*/
		~Tool();
		/*! \brief constructor
			\param QString name
			\param QWidget* parent*/
		Tool(const QString& Name, QWidget * parent = 0);
		/*! \brief set the main window for this tool*/
		virtual bool setMainWindow(MainWindow * main);
		/*! \brief the main window's current scene*/
		virtual GraphicsScene* currentScene() const;
		/*! \brief the main window's current text editor*/
		virtual TextEditor* currentTextEditor() const;
		/*! \brief the main window's current window*/
		virtual NetworkWindow* currentWindow() const;

		/*! \brief tools that are drawn on the scene instead of displayed as a window
		\ingroup core
		*/
		class MY_EXPORT GraphicsItem : public QGraphicsItemGroup
		{

		public:
			/*! \brief constructor must have an associated Tool*/
			GraphicsItem(Tool*);
			/*! \brief this item has been selected*/
			virtual void select();
			/*! \brief this item has been deselected*/
			virtual void deselect();
			/*! \brief main window for this tool*/
			Tool * tool;
			/*! \brief for enabling dynamic_cast*/
			enum { Type = UserType + 10 };
			/*! \brief for enabling dynamic_cast*/
			int type() const
			{
				// Enable the use of dynamic_cast with this item.
				return Type;
			}
			/*! \brief show or hide this graphical tool. The graphical tool may choose whether or not to be visible based on other factors.*/
			virtual void visible(bool);
			/*! \brief cast a graphics item to a Tool::GraphicsItem
			\return Tool::GraphicsItem* can be 0 if invalid cast*/
			static GraphicsItem* cast(QGraphicsItem*);
		};

		/*! \brief optional graphics item used to display this tool */
		QList<GraphicsItem*> graphicsItems;
		/*! \brief optional button used to display this tool */
		QButtonGroup buttons;

		public slots:
			/*! \brief what happens when this tool is selected */
			virtual void select(int);
			/*! \brief what happens when this tool is deselected */
			virtual void deselect(int);

		signals:
			/*! \brief this tool is selected */
			void selected();
			/*! \brief this tool is deselected */
			void deselected();
	};

}

#endif
