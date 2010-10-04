#!/bin/bash

#run in the bin folder
echo "MUST RUN IN THE BIN FOLDER. USE CURRENT PATH AS ARGUMENT"
CURPATH=$1
LIBFILES='*.dylib'
PLUGINFILES='plugins/*.dylib'
CPLUGINFILES='plugins/c/*.dylib'

#copy other supporting files

cp -R plugins Tinkercell.app/Contents/MacOS/
mkdir Tinkercell.app/Contents/MacOS/c
mkdir Tinkercell.app/Contents/MacOS/lib
cp ../../API/*.h Tinkercell.app/Contents/MacOS/c
cp ../../c/*.h Tinkercell.app/Contents/MacOS/c/
cp ../../Main/tinkercell.qss Tinkercell.app/Contents/MacOS/
cp -R ../../icons Tinkercell.app/Contents/MacOS/
cp plugins/c/.a Tinkercell.app/Contents/MacOS/lib/
cp lib* Tinkercell.app/Contents/MacOS/
cp -R ../../Graphics Tinkercell.app/Contents/MacOS/
mkdir Tinkercell.app/Contents/MacOS/NodesTree/
cp ../../NodesTree/*.xml Tinkercell.app/Contents/MacOS/NodesTree/
cp -R ../../NodesTree/Icons Tinkercell.app/Contents/MacOS/NodesTree/
cp -R python Tinkercell.app/Contents/MacOS/
cp ../../python/*.py Tinkercell.app/Contents/MacOS/python
cp ../../*.txt Tinkercell.app/Contents/MacOS/
cp -R octave Tinkercell.app/Contents/MacOS/
cp ../../octave/*.m Tinkercell.app/Contents/MacOS/octave/
cp -R ../../Modules/ Tinkercell.app/Contents/MacOS/
#QT frameworks install for TinkerCell.app

install_name_tool \
        -id @executable_path/../Frameworks/libQtCore.4.dylib \
        Tinkercell.app/Contents/Frameworks/libQtCore.4.dylib

install_name_tool \
        -id @executable_path/../Frameworks/libQtGui.4.dylib \
        Tinkercell.app/Contents/Frameworks/libQtGui.4.dylib

install_name_tool \
        -id @executable_path/../Frameworks/libQtXml.4.dylib \
        Tinkercell.app/Contents/Frameworks/libQtXml.4.dylib

install_name_tool \
        -id @executable_path/../Frameworks/libQtOpenGL.4.dylib \
        Tinkercell.app/Contents/Frameworks/libQtOpenGL.4.dylib

#QT frameworks install for NodeGraphics.app

install_name_tool \
        -id @executable_path/../Frameworks/libQtCore.4.dylib \
        NodeGraphics.app/Contents/Frameworks/libQtCore.4.dylib

install_name_tool \
        -id @executable_path/../Frameworks/libQtGui.4.dylib \
        NodeGraphics.app/Contents/Frameworks/libQtGui.4.dylib

install_name_tool \
        -id @executable_path/../Frameworks/libQtXml.4.dylib \
        NodeGraphics.app/Contents/Frameworks/libQtXml.4.dylib

install_name_tool \
        -id @executable_path/../Frameworks/libQtOpenGL.4.dylib \
        NodeGraphics.app/Contents/Frameworks/libQtOpenGL.4.dylib

#QtCore name change for other Qt frameworks in TinkerCell.app

install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtCore.4.dylib \
          Tinkercell.app/Contents/Frameworks/libQtGui.4.dylib

install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtCore.4.dylib \
          Tinkercell.app/Contents/Frameworks/libQtXml.4.dylib
          
install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtCore.4.dylib \
          Tinkercell.app/Contents/Frameworks/libQtOpenGL.4.dylib

install_name_tool \
          -change @QT_QTGUI_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtGui.4.dylib \
          Tinkercell.app/Contents/Frameworks/libQtOpenGL.4.dylib
          
#QtCore name change for other Qt frameworks in TinkerCell.app

install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtCore.4.dylib \
          NodeGraphics.app/Contents/Frameworks/libQtGui.4.dylib

install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtCore.4.dylib \
          NodeGraphics.app/Contents/Frameworks/libQtXml.4.dylib
          
install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtCore.4.dylib \
          NodeGraphics.app/Contents/Frameworks/libQtOpenGL.4.dylib

install_name_tool \
          -change @QT_QTGUI_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
          NodeGraphics.app/Contents/Frameworks/libQtOpenGL.4.dylib

#QT framework name change for TinkerCell.app

install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtCore.4.dylib \
          Tinkercell.app/Contents/MacOS/Tinkercell

install_name_tool \
          -change @QT_QTGUI_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtGui.4.dylib \
          Tinkercell.app/Contents/MacOS/Tinkercell

install_name_tool \
          -change @QT_QTXML_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtXml.4.dylib \
          Tinkercell.app/Contents/MacOS/Tinkercell

install_name_tool \
          -change @QT_QTOPENGL_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtOpenGL.4.dylib \
          Tinkercell.app/Contents/MacOS/Tinkercell

#QT framework name change for NodeGraphcs.app

install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtCore.4.dylib \
          NodeGraphics.app/Contents/MacOS/NodeGraphics

install_name_tool \
          -change @QT_QTGUI_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtGui.4.dylib \
          NodeGraphics.app/Contents/MacOS/NodeGraphics

install_name_tool \
          -change @QT_QTXML_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtXml.4.dylib \
          NodeGraphics.app/Contents/MacOS/NodeGraphics

install_name_tool \
          -change @QT_QTOPENGL_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtOpenGL.4.dylib \
          NodeGraphics.app/Contents/MacOS/NodeGraphics

#for all libraries used in Tinkercell.app and NodeGraphics.app

for f in $LIBFILES
do
  echo "Processing $f"
  
  cp $f Tinkercell.app/Contents/Frameworks/
  
  cp $f NodeGraphics.app/Contents/Frameworks/
  
  for f2 in $LIBFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/../Frameworks/$f2 \
          Tinkercell.app/Contents/Frameworks/$f
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/../Frameworks/$f2 \
          NodeGraphics.app/Contents/Frameworks/$f
  done
  
  install_name_tool \
        -id @executable_path/../Frameworks/$f \
        Tinkercell.app/Contents/Frameworks/$f
  
  install_name_tool \
        -change $CURPATH/$f \
         @executable_path/../Frameworks/$f \
         Tinkercell.app/Contents/MacOS/Tinkercell

  install_name_tool \
         -change $CURPATH/$f \
         @executable_path/../Frameworks/$f \
         NodeGraphics.app/Contents/MacOS/NodeGraphics

  install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtCore.4.dylib \
          Tinkercell.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTGUI_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtGui.4.dylib \
          Tinkercell.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTXML_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtXml.4.dylib \
          Tinkercell.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTOPENGL_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtOpenGL.4.dylib \
          Tinkercell.app/Contents/Frameworks/$f
          
  install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtCore.4.dylib \
          NodeGraphics.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTGUI_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtGui.4.dylib \
          NodeGraphics.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTXML_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtXml.4.dylib \
          NodeGraphics.app/Contents/Frameworks/$f

  install_name_tool \
          -change @QT_QTOPENGL_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtOpenGL.4.dylib \
          NodeGraphics.app/Contents/Frameworks/$f

done

#name change for all plugins that can depend on other plugins and libTinkerCellCore

for f1 in $PLUGINFILES
do
  echo "Processing $f1"
  install_name_tool \
          -id \
          @executable_path/$f1 \
          Tinkercell.app/Contents/MacOS/$f1

  install_name_tool \
          -change @QT_QTCORE_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtCore.4.dylib \
         Tinkercell.app/Contents/MacOS/$f1

  install_name_tool \
          -change @QT_QTGUI_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtGui.4.dylib \
          Tinkercell.app/Contents/MacOS/$f1

  install_name_tool \
          -change @QT_QTXML_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtXml.4.dylib \
          Tinkercell.app/Contents/MacOS/$f1

  install_name_tool \
          -change @QT_QTOPENGL_LIBRARY_RELEASE@ \
          @executable_path/../Frameworks/libQtOpenGL.4.dylib \
          Tinkercell.app/Contents/MacOS/$f1

  for f2 in $LIBFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/../Frameworks/$f2 \
          Tinkercell.app/Contents/MacOS/$f1
  done
  for f2 in $PLUGINFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/$f2 \
          Tinkercell.app/Contents/MacOS/$f1
  done
  for f2 in $CPLUGINFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/$f2 \
          Tinkercell.app/Contents/MacOS/$f1
  done
done

for f1 in $CPLUGINFILES
do
  echo "Processing $f1"
  install_name_tool \
          -id \
          @executable_path/$f1 \
          Tinkercell.app/Contents/MacOS/$f1  
  for f2 in $CPLUGINFILES
  do
    install_name_tool \
          -change $CURPATH/$f2 \
          @executable_path/$f2 \
          Tinkercell.app/Contents/MacOS/$f1
  done
done

mkdir TinkerCell
cp TinkerCell.app TinkerCell
cp NodeGraphics.app TinkerCell
ln -s /Applications TinkerCell

