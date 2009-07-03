/****************************************************************************

 Copyright (c) 2008 Deepak Chandran
 Contact: Deepak Chandran (dchandran1@gmail.com)
 See COPYRIGHT.TXT

 This tool allows insertion of parts from the NodesTree

****************************************************************************/

#ifndef TINKERCELL_TEXTEDITORLISTENERTOOL_H
#define TINKERCELL_TEXTEDITORLISTENERTOOL_H

#include <stdlib.h>
#include <QtGui>
#include <QIcon>
#include <QPixmap>
#include <QString>
#include <QtDebug>
#include <QAction>
#include <QFile>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QFrame>

#include "TextItem.h"
#include "TextEditor.h"
#include "ItemHandle.h"
#include "Tool.h"
#include "ConnectionsTree.h"
#include "NodesTree.h"

#ifdef Q_WS_WIN
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT
#endif

namespace Tinkercell
{

     /*! \brief This class is required for providing proper responses from the TextEditor.
       If this class is not loaded, another must be loaded in order for the TextEditor to be
       operable.
        \ingroup plugins
     */
     class MY_EXPORT TextEditorListener : public Tool
     {
          Q_OBJECT

     public:

          /* !\brief default constructor. */
          TextEditorListener() {}

          /* !\brief load this plug-in into the TinkerCell main window
        \param MainWindow* TinkerCell's main window*/
          //bool setMainWindow(MainWindow * main);

     public slots:
          /**/
          //void lineChanged(const QString&);
          /**/
          //void nodeModified(const QString&);

    private:
        /*! \brief the currently selected item from the parts tree*/
        //NodeFamily * selectedNodeFamily;
        /*! \brief the parts tree*/
        //NodesTree * partsTree;
        /*! \brief returns the mouse cursor to the normal arrow and exists from insertion mode*/
        //void clear(bool arrows=true);
        /*! \brief check to see whether the MainWindow has a NodesTree*/
        //void connectToNodesTree();

        /*! \brief connect the C API signals and slots*/
        //void connectTCFunctions();
        /*! \brief the C API function to signal converter*/
//         static NodeInsertion_FToS fToS;
        /*! \brief the C API function*/
        //static OBJ _insertItem(const char* , const char* );
    };

}


extern "C" MY_EXPORT void loadTCTool(Tinkercell::MainWindow * main);

#endif
