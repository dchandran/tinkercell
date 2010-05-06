#ifndef TINKERCELL_NETWORKWINDOW_H
#define TINKERCELL_NETWORKWINDOW_H

#include <QWidget>

namespace Tinkercell
{
	
	class MainWindow;
	class GraphicsScene;
	class TextEditor;
	class NetworkHandle;
		
	class NetworkWindow : public QWidget
	{
	signals:
		/*!
		* \brief signals when a window is going to close
		* \param NetworkWindow *  the window that is closing
		* \param Boolean setting to false will prevent this window from closing
		* \return void
		*/
		void windowClosing(NetworkWindow *, bool*);
		/*!
		* \brief signals after a window is closed
		* \param NetworkWindow *  the window that was closed
		* \return void
		*/
		void windowClosed(NetworkWindow *);

	protected:
		/*! \brief close window event
		* \param QCloseEvent * event
		* \return void*/
		virtual void closeEvent(QCloseEvent *event);

	private:

		/*!\brief constructor with scene*/
		NetworkWindow(NetworkHandle * network, GraphicsScene * scene);

		/*!\brief constructor with text editor*/
		NetworkWindow(NetworkHandle * network, TextEditor * editor);

		/*! \brief set this window as the main window's current window*/		
		void setAsCurrentWindow();
		
		/*!\brief the network displayed in this window*/
		NetworkHandle * network;

		/*!\brief the scene inside this window. Either the scene or the editor must be 0*/
		GraphicsScene * scene;

		/*!\brief the editor inside this window. Either the scene or the editor must be 0*/
		TextEditor * editor;
		
		friend MainWindow;
		friend GraphicsScene;
		friend TextEditor;
		friend NetworkHandle;
	};
}

#endif

