/**************************************
This is a very simple plugin
It listens to main window's signals
and displays dialog boxes showing user
inputs
**************************************/


#ifndef TINKERCELL_SAMPLE_PLUGIN1_H
#define TINKERCELL_SAMPLE_PLUGIN1_H

#include "Tool.h"

namespace Tinkercell
{

	class SamplePlugin1 : public Tool
	{

		Q_OBJECT

	public:
		SamplePlugin1();
		bool setMainWindow(MainWindow * main);
	
	public slots:
		void mouseReleased(GraphicsScene * scene, QPointF point, Qt::MouseButton button, Qt::KeyboardModifiers modifiers);

	};

}

#endif

