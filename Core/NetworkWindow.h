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

	private:
		/*! \brief close event sends signal to all tools asking for confirmation becore closing
		* \param QCloseEvent * event
		* \return void*/
		virtual void closeEvent(QCloseEvent *event);
		/*! \brief focus receved changes the main windows current network pointer
		* \param QFocusEvent*
		* \return void*/
		virtual void NetworkWindow::focusInEvent ( QFocusEvent * );
		/*! \brief resize event checks if the window has been minimized and calls popIn instead of minimizing
		* \param QResizeEvent*
		* \return void*/
		virtual void NetworkWindow::resizeEvent (QResizeEvent * event);
		/*! \brief calls main window's setAsCurrentWindow
		* \return void*/
		virtual void NetworkWindow::setAsCurrentWindow();
		/*! \brief calls main window's popOut
		* \return void*/
		virtual void NetworkWindow::popOut();
		/*! \brief calls main window's popIn
		* \return void*/
		virtual void NetworkWindow::popIn();
		/*! \brief calls popIn when minimized
		* \return void*/
		virtual void NetworkWindow::changeEvent ( QEvent * event );

		/*!\brief constructor with scene*/
		NetworkWindow(NetworkHandle * network, GraphicsScene * scene);

		/*!\brief constructor with text editor*/
		NetworkWindow(NetworkHandle * network, TextEditor * editor);

		/*! \brief set this window as the main window's current window*/		
		virtual void setAsCurrentWindow();
		
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

